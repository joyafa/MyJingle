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

#if !defined(Codec_h_INCLUDED)
#define Codec_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "speex/speex.h"
#include "speex/speex_preprocess.h"
#include "Packet.h"
#include "g711common.h"

class Codec
{
public:
	Codec(int frameSize);
	virtual ~Codec(void);
	virtual void Init();
	virtual void Exit();
	virtual int GetDecompressFrameSize();
	virtual int DecompressPacket(Packet* packet, char* outFrame, int length);
	virtual int GetCompressFrameSize();
	virtual Packet* CompressFrame(char* inFrame, int length);
protected:
	int m_FrameSize;
};

class G711Codec: public Codec
{
public:
	G711Codec();
	virtual ~G711Codec(void);
	virtual int GetDecompressFrameSize();
	virtual int DecompressPacket(Packet* packet, char* outFrame, int length);
	virtual int GetCompressFrameSize();
	virtual Packet* CompressFrame(char* inFrame, int length);
private:
	static const int s_CompFrameSize;
	static const int s_DecompFrameSize;
};

class SpeexCodec: public Codec
{
public:
	SpeexCodec();
	~SpeexCodec(void);
	void Init();
	void Exit();
	int GetDecompressFrameSize();
	int DecompressPacket(Packet* packet, char* outFrame, int length);
	int GetCompressFrameSize();
	Packet* CompressFrame(char* inFrame, int length);
protected:
	SpeexBits m_EncodeSpeexBits;
	SpeexBits m_DecodeSpeexBits;
	SpeexPreprocessState* m_SpeexPreprocessState;
	void* m_DecoderState;
	void* m_EncoderState;
	int m_CompressFrameSize;
	int m_DecompressFrameSize;
	bool m_Silence;
};

#endif

