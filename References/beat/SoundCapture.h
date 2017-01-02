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

#if !defined(SoundCapture_h_INCLUDED)
#define SoundCapture_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

#include "Codec.h"
#include "JitterBuffer.h"
#include "Packet.h"
#include "NetAddress.h"

#if defined (WIN32)
#include "DirectXFullDuplexAudioDevice.h"
#include "Windows.h"
#include "Dsound.h"
#include "Thread.h"
#include "SoundInterface.h"
#include "SoundRecorder.h"
#endif


class SoundCapture: public PacketSender
{
public:
	SoundCapture(void);
	virtual ~SoundCapture(void);
	Codec* Encoder();
	void Encoder(Codec* encoder);
	NetAddress* ToNetAddress();
	void ToNetAddress(NetAddress* adress);
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Init() = 0;
	virtual void Exit() = 0;
protected:
	NetAddress* m_NetAddress;
	unsigned short m_nSeq;
	int m_FrameSize;
	Codec* m_Encoder;
	void CompressFrame(char* frame, int length);
};


#if defined (WIN32)
class SoundCaptureImpl: public SoundCapture, private Thread
{
public:
	SoundCaptureImpl(WAVEFORMATEX& format, HWND window);
	~SoundCaptureImpl(void);
	void Start();
	void Stop();
	void Init();
	void Exit();
protected:
	bool m_Initialized;
	void Svc();
	DirectXFullDuplexAudioDevice* m_Audiodev;
	LPDIRECTSOUNDCAPTURE8 m_CaptureDevice;
	LPDIRECTSOUNDCAPTUREBUFFER8 m_CaptureBuffer;
	HWND m_Window;
	HANDLE* m_Events;
	WAVEFORMATEX& m_Format;
	int m_PacketsInBuffer;
	int m_BytesInPacket;
	int m_BytesPerMsec;
	bool m_Capturing;
	bool m_PrioritySet;
	DSBPOSITIONNOTIFY* m_Notifypos;
};
#else

class SoundCaptureImpl
{
};

#endif


#endif
