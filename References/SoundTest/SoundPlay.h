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

#if !defined(SoundPlay_h_INCLUDED)
#define SoundPlay_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

#if defined (WIN32)
#include "DirectXFullDuplexAudioDevice.h"
#include "Windows.h"
#include "Dsound.h"
#include "Thread.h"
#include "Jitter.h"
#endif

//#include "Codec.h"

class JitterBuffer;

class SoundPlay
{
	friend class PlayBuffer;
public:
	virtual ~SoundPlay();
	/*void Decoder(Codec* decoder);
	Codec* Decoder();*/
	JitterBuffer* JitterBuf();
	void JitterBuf(JitterBuffer* playBuf);
	virtual void Init() = 0;
	virtual void Exit() = 0;
	virtual void Stop() = 0;
	virtual void Start() = 0;
protected:
	SoundPlay();
	JitterBuffer* m_JitterBuffer;
	//Codec* m_Decoder;
	int m_FrameSize;
	static const int s_mSecDecodeLatency = 18;  // sensible value 0 - 20
	static const int s_mSecInitialBuffer = 50;  // sensible value may be up to packets in buffer
	int DecompressPacket(Packet* packet, char* outFrame, int length);
};

#if defined (WIN32)

class SoundPlayImpl: public SoundPlay, private Thread
{
public:
	SoundPlayImpl(WAVEFORMATEX& format, HWND window);
	//~SoundPlayImpl(void);
	void Init();
	void Exit();
	void Stop();
	void Start();
protected:
	bool m_Playing;
	bool m_Initialized;
	bool m_PrioritySet;
	void Svc();
	DirectXFullDuplexAudioDevice* m_Audiodev;
	LPDIRECTSOUND8 m_PlayDevice;
	LPDIRECTSOUNDBUFFER m_Buffer;
	HWND m_Window;
	HANDLE* m_Events;
	WAVEFORMATEX m_Format;
	int m_PacketsInBuffer;
	int m_BytesInPacket;
	int m_BytesPerMsec;
	bool m_InitialBufferRun;
	DSBPOSITIONNOTIFY* m_Notifypos;
	Mutex m_Mutex;
};
#else
class SoundPlayImpl: public SoundPlay
{

};
#endif

#endif
