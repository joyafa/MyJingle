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

#include "BeatMediaChannel.h"
#include "beat/Packet.h"
#include "JingleMain.h"
#include "beat/Exception.h"
#include "beat/beatLog_.h"


namespace cricket{
BeatMediaChannel::BeatMediaChannel(void):
m_Codec_PCM(160)
,m_Codec_G711()
,m_Codec_Speex()
{
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;//立体声
	format.nSamplesPerSec = 16000;//实际测试16K定位音质效果很好
	format.nAvgBytesPerSec = format.nSamplesPerSec * 2;
	format.nBlockAlign = 2;
	format.wBitsPerSample = 16;
	format.cbSize = 0;
	//format.nSamplesPerSec = 22.05  * 1000;//8000; kHz
	//format.nAvgBytesPerSec = 16000;//16000;
	//format.wBitsPerSample = 16;
	//format.nBlockAlign = (format.nChannels*format.wBitsPerSample)/8;//2;
	//format.cbSize = 0;
	try
	{
		m_Codec_PCM.Init();
		m_Codec_G711.Init();
		m_Codec_Speex.Init();

		HWND window = JingleMain::m_hWnd;
		m_SoundCapture = new ::SoundCaptureImpl(format, window);
		m_SoundPlay = new ::SoundPlayImpl(format, window);
		m_SoundCapture->Encoder(&m_Codec_G711);
		m_SoundPlay->Decoder(&m_Codec_G711);
		m_SoundPlay->JitterBuf(&m_JitterBuffer);
		m_SoundCapture->AddPacketReceiver(this);

		m_JitterBuffer.Init();
		m_SoundCapture->Init();
		m_SoundPlay->Init();
	}
	catch(Exception& ex)
	{
		beatLog_Fatal(("BeatMediaChannel", "BeatMediaChannel", "An Exception has occured while initialising BeatMediaChannel %s", ex.ToString()));
	}

}

BeatMediaChannel::~BeatMediaChannel(void)
{
	try
	{
		m_SoundPlay->Stop();
		m_SoundPlay->Exit();
		m_SoundCapture->Stop();
		m_SoundCapture->Exit();
		m_Codec_PCM.Exit();
		m_Codec_G711.Exit();
		m_Codec_Speex.Exit();
		m_JitterBuffer.Exit();
	}
	catch(Exception& ex)
	{
		beatLog_Error(("BeatMediaChannel", "~BeatMediaChannel", "An Exception has occured while uninitialising BeatMediaChannel \n%s", ex.ToString()));
	}
}

void cricket::BeatMediaChannel::OnPacketReceived(const void * buffer,int length)
{
	void* packBuff = malloc(length);
	memcpy(packBuff, buffer, length);

	try
	{
		Packet* packet = Packet::TmpGetPacketFromBuffer((char *)packBuff, length);
		m_JitterBuffer.AddPacket(packet, 0);
	}
	catch(Exception& ex)
	{
		beatLog_Error(("BeatMediaChannel", "OnPacketReceived", "An Exception has occured while adding packet to buffer \n%s", ex.ToString()));
	}
}

void cricket::BeatMediaChannel::SetCodec(const char* codec)
{
	try
	{
		if (!strcmp(codec, "speex"))
		{
		  m_SoundCapture->Encoder((Codec*)&m_Codec_Speex);//m_Codec_Speex
		  m_SoundPlay->Decoder((Codec*)&m_Codec_Speex);
		}
		else
		{
		  m_SoundCapture->Encoder(&m_Codec_G711);
		  m_SoundPlay->Decoder(&m_Codec_G711);
		}

	}
	catch(Exception& ex)
	{
		beatLog_Error(("BeatMediaChannel", "SetCodec", "An Exception has occured while setting the codec \n%s", ex.ToString()));
	}
}
void cricket::BeatMediaChannel::SetPlayout(bool playout)
{
	try
	{
		if(playout)
		{
			m_SoundPlay->Start();
		}
		else
		{
			m_SoundPlay->Stop();
		}
	}
	catch(Exception& ex)
	{
		beatLog_Error(("BeatMediaChannel", "SetPlayout", "An Exception has occured while setting playout \n%s", ex.ToString()));
	}
}

void cricket::BeatMediaChannel::SetSend(bool send)
{
	try
	{
		if(send)
		{
			m_SoundCapture->Start();
		}
		else
		{
			m_SoundCapture->Stop();
		}
	}
	catch(Exception& ex)
	{
		beatLog_Error(("BeatMediaChannel", "SetSend", "An Exception has occured while setting send \n%s", ex.ToString()));
	}
}

void cricket::BeatMediaChannel::AddPacket(Packet* packet, NetAddress *)
{
	if (packet != 0)
	{
		MediaChannel::NetworkInterface* iface = network_interface();
		if (iface!= 0)
		{
			iface->SendPacket(packet->GetPacket(), packet->PacketLength());
		}
	}
}

}
