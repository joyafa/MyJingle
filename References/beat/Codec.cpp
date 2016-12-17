//----------------------------------------------------------------------//
//                                                                      //
// MyJingle is a GoogleTalk compatible VoIP Client based on Jingle      //
//                                                                      //
// Copyright (C) 2006  bluehands GmbH & Co.mmunication KG               //
//                                                                      //
// This program is free software; you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation; either version 2 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful, but  //
// WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     //
// General Public License for more details.                             //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA //
//                                                                      //
//----------------------------------------------------------------------//

#include "Codec.h"

Codec::Codec(int frameSize):
m_FrameSize(frameSize)
{
}

Codec::~Codec()
{
}

void Codec::Init()
{
}

void Codec::Exit()
{
}

int Codec::GetCompressFrameSize()
{
	return m_FrameSize;
}

int Codec::GetDecompressFrameSize()
{
	return m_FrameSize;
}

Packet* Codec::CompressFrame(char* inFrame, int length)
{
	Packet* packet = Packet::CreateNewDataPacket(length, 0);
	memcpy(packet->RwData(), inFrame, length);
	return packet;
}

int Codec::DecompressPacket(Packet* packet, char* outFrame, int length)
{
	int outlength = 0;
	if(packet != 0)
	{
		outlength = packet->DataLength();
		if( outlength != length) // probably silence
		{
			beatLog_Debug(("test", "test",  "Sensed Silence"));
			outlength = length;
#if defined (WIN32)
		ZeroMemory(outFrame, length);
#else
		bzero(outFrame, length);
#endif
		}
		else
		{
		//beatLog_Debug(("test", "test",  "Not Sensed Silence"));
			memcpy(outFrame, packet->RwData(), length);
		}
	}
	else
	{
		outlength = GetDecompressFrameSize();
#if defined (WIN32)
		ZeroMemory(outFrame, outlength);
#else
		bzero(outFrame, outlength);
#endif
	}
	return outlength;
}

const int G711Codec::s_CompFrameSize = 160;
const int G711Codec::s_DecompFrameSize = 320;

G711Codec::G711Codec():
Codec(s_DecompFrameSize)
{
}

G711Codec::~G711Codec(void)
{
}

int G711Codec::GetCompressFrameSize()
{
	return s_CompFrameSize;
}

int G711Codec::GetDecompressFrameSize()
{
	return s_DecompFrameSize;
}


Packet* G711Codec::CompressFrame(char* inFrame, int length)
{
	Packet* packet = 0;
	if(length == GetDecompressFrameSize())
	{
		short* uncBuff = (short*)inFrame;
		int cSize = GetCompressFrameSize();
		packet = Packet::CreateNewDataPacket(cSize, 0);
		unsigned char* compBuff = (unsigned char*)packet->RwData();
		for(int i = 0; i < cSize; i++)
		{
			compBuff[i] = s16_to_ulaw(uncBuff[i]);
		}
	}
	else
	{
		//Silence Detected
	}
	return packet;
}

int G711Codec::DecompressPacket(Packet* packet, char* outFrame, int length)
{
	short* outBuf = (short*)outFrame;
	if(packet != 0)
	{
		int packetLength = packet->DataLength();
		if((packetLength * GetDecompressFrameSize()) !=
			(length * GetCompressFrameSize())) //silence detected
		{
			ZeroMemory(outFrame, length);
		}
		else
		{
			unsigned char* data = (unsigned char*)packet->RwData();
			for(int i = 0; i < packetLength; i++)
			{
				outBuf[i] = ulaw_to_s16(data[i]);
			}
		}
	}
	else
	{
#if defined (WIN32)
		ZeroMemory(outFrame, length);
#else
		bzero(outFrame, outlength);
#endif
	}
	return length;
}

SpeexCodec::SpeexCodec():
Codec(0)
,m_CompressFrameSize(-1)
,m_DecompressFrameSize(-1)
,m_Silence(false)
{
}

SpeexCodec::~SpeexCodec(void)
{
}

void SpeexCodec::Init(void)
{
	speex_bits_init(&m_EncodeSpeexBits);
	m_EncoderState = speex_encoder_init(&speex_nb_mode);

	speex_bits_init(&m_DecodeSpeexBits);
	m_DecoderState = speex_decoder_init(&speex_nb_mode);

	m_SpeexPreprocessState = speex_preprocess_state_init(GetCompressFrameSize()/2, 8000); //Size is in dwords not bytes


	int msg = 4;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_COMPLEXITY, &msg);
	msg = 6;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_QUALITY, &msg);
	msg = 0;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_VBR, &msg);
	msg = 6;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_VBR_QUALITY, &msg);
	msg = 0;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_VAD, &msg);
	msg = 0;
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_DTX, &msg);

	msg = 1;
	speex_decoder_ctl(m_DecoderState, SPEEX_SET_ENH, &msg);

	msg = 1;
	speex_preprocess_ctl(m_SpeexPreprocessState, SPEEX_PREPROCESS_SET_DENOISE, &msg);
	msg = 1;
	speex_preprocess_ctl(m_SpeexPreprocessState, SPEEX_PREPROCESS_SET_AGC, &msg);
	//msg = 4300;
	//speex_preprocess_ctl(m_SpeexPreprocessState, SPEEX_PREPROCESS_GET_AGC_LEVEL, &msg);
	//msg = 1150000000;
	//speex_preprocess_ctl(m_SpeexPreprocessState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &msg);
	msg = 1;
	speex_preprocess_ctl(m_SpeexPreprocessState, SPEEX_PREPROCESS_SET_VAD, &msg);
}

void SpeexCodec::Exit(void)
{
	speex_bits_destroy(&m_EncodeSpeexBits);
	speex_encoder_destroy(m_EncoderState);

	speex_bits_destroy(&m_DecodeSpeexBits);
	speex_decoder_destroy(m_DecoderState);

	speex_preprocess_state_destroy(m_SpeexPreprocessState);
}

int SpeexCodec::GetDecompressFrameSize()
{
	if(m_DecompressFrameSize==-1)
	{
		speex_decoder_ctl(m_DecoderState, SPEEX_GET_FRAME_SIZE, &m_DecompressFrameSize);
		m_DecompressFrameSize *= 2; //since we are comuting with byte rather than dwords
	}
	return m_DecompressFrameSize;
}


int SpeexCodec::DecompressPacket(Packet* packet, char* outFrame, int length)
{
	if (packet != 0)
	{
		speex_bits_read_from(&m_DecodeSpeexBits, packet->RwData(), packet->DataLength());
		speex_decode_int(m_DecoderState, &m_DecodeSpeexBits, (short *)outFrame);
	}
	else
	{
#if defined (WIN32)
		ZeroMemory(outFrame, length);
#else
		bzero(outFrame, length);
#endif
	}
	return length;
}

int SpeexCodec::GetCompressFrameSize()
{
	if(m_CompressFrameSize==-1)
	{
		speex_encoder_ctl(m_EncoderState,SPEEX_GET_FRAME_SIZE,&m_CompressFrameSize);
		m_CompressFrameSize *= 2;	//since we are comuting with byte rather than dwords
	}
	return m_CompressFrameSize;
}

Packet* SpeexCodec::CompressFrame(char* inFrame, int length)
{
	Packet* packet = 0;
	int voiceActive = 0;
	if(length==GetCompressFrameSize())
	{
		voiceActive = speex_preprocess(m_SpeexPreprocessState, (short*)inFrame, 0);
	}
	else
	{
		speex_preprocess_estimate_update(m_SpeexPreprocessState, (short*)inFrame, 0);
	}
	speex_bits_reset(&m_EncodeSpeexBits);
	speex_encode_int(m_EncoderState, (short*)inFrame, &m_EncodeSpeexBits);
	int outSize = speex_bits_nbytes(&m_EncodeSpeexBits);
	//voiceActive = 1;

	//// ===== silence Bit set before Silence
	//if(voiceActive != 0 || !m_Silence)
	//{
	//	packet = Packet::CreateNewDataPacket(outSize, 0);
	//	speex_bits_write(&m_EncodeSpeexBits, packet->RwData(), outSize);
	//	if (voiceActive == 0)
	//	{
	//		beatLog_Debug((_S("test"), _S("test"),  _S("Sensed Silence ")));
	//		m_Silence = true;
	//		packet->Silence(true);
	//	}
	//	else
	//	{
	//		beatLog_Debug((_S("test"), _S("test"),  _S("Not Sensed Silence ")));
	//		m_Silence = false;
	//	}
	//}


	// ==== silence Bit set after Silence
	if (voiceActive == 0)
	{
		m_Silence = true;
		//beatLog_Debug((_S("test"), _S("test"),  _S("Sensed Silence %d"), outSize));
		//sensed silence
	}
	else
	{
		packet = Packet::CreateNewDataPacket(outSize, 0);
		speex_bits_write(&m_EncodeSpeexBits, packet->RwData(), outSize);
		if (m_Silence)
		{
			packet->Silence(true);
			beatLog_Debug(("test", "test",  "Sensed Silence"));
		}
		else
		{
			beatLog_Debug(("test", "test",  "not Sensed Silence"));
		}
		m_Silence = false;
	}

	// ==== silence Bit set before silence and after
	//if (voiceActive == 0 && m_Silence)
	//{
	//	beatLog_Debug((_S("test"), _S("test"),  _S("Sensed Silence %d"), outSize));
	//	//sensed silence
	//}
	//else
	//{
	//	packet = Packet::CreateNewDataPacket(outSize, 0);
	//	speex_bits_write(&m_EncodeSpeexBits, packet->RwData(), outSize);
	//	if (voiceActive == 0 && !m_Silence) //last Packet before Silence
	//	{
	//		m_Silence = true;
	//		packet->Silence(true);
	//	}
	//	else
	//	{
	//		if(voiceActive != 0 && m_Silence) // first Packet after Silence
	//		{
	//			m_Silence = false;
	//			packet->Silence(true);
	//		}
	//	}

	//	beatLog_Debug((_S("test"), _S("test"),  _S("not Sensed Silence")));
	//}

	return packet;
}
