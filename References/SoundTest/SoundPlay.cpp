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

#include "SoundPlay.h"
#include "JitterBuffer.h"

SoundPlay::SoundPlay():
m_JitterBuffer(0)
//,m_Decoder(0)
,m_FrameSize(0)
{
}

SoundPlay::~SoundPlay(void)
{
}

//Codec* SoundPlay::Decoder()
//{
//	return m_Decoder;
//}
//
//void SoundPlay::Decoder(Codec* decoder)
//{
//	m_Decoder = decoder;
//	if(m_Decoder!=0)
//	{
//		m_FrameSize = m_Decoder->GetDecompressFrameSize();
//	}
//}

JitterBuffer* SoundPlay::JitterBuf()
{
	return m_JitterBuffer;
}

void SoundPlay::JitterBuf(JitterBuffer* jitterBuf)
{
	m_JitterBuffer = jitterBuf;
}

int SoundPlay::DecompressPacket(Packet* packet, char* outFrame, int length)
{
	int decompLength = 0;
	/*if(m_Decoder!=0)
	{
		decompLength = m_Decoder->DecompressPacket(packet, outFrame, length);
	}
*/
	return decompLength;
}

// ========================================== SoundPlayImpl Windows =======================================
#if defined (WIN32)

SoundPlayImpl::SoundPlayImpl(WAVEFORMATEX& format, HWND window):
m_Playing(false)
,m_Initialized(false)
,m_Audiodev(0)
,m_Events(0)
,m_Window(window)
,m_PrioritySet(false)
,m_PacketsInBuffer(-1)
,m_BytesInPacket(-1)
,m_BytesPerMsec(-1)
,m_InitialBufferRun(true)
,m_Notifypos(0)
{
	memcpy(&m_Format, &format, sizeof(WAVEFORMATEX));
}

void SoundPlayImpl::Init()
{
	if(!m_Initialized)
	{
		m_Mutex.Init();
		m_Mutex.Aquire();
		m_Initialized = true;
		HRESULT res;
		m_Audiodev = GetDirectXFullDuplexAudioDeviceInstance(m_Format, m_Window);
		m_Audiodev->Init();

		m_PacketsInBuffer = m_Audiodev->PacketsInBuffer();
		m_Buffer = m_Audiodev->PlayBuffer();
		m_PlayDevice = m_Audiodev->PlayDevice();
		m_BytesInPacket = m_Audiodev->BytesInPacket();
		m_BytesPerMsec = m_Audiodev->BytesPerMsec();

		void* start = 0;
		DWORD length = 0;

		res = m_Buffer->Lock(0,0,&start, &length, 0, 0,DSBLOCK_ENTIREBUFFER);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While locking buffer Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}

		ZeroMemory(start, length);

		res = m_Buffer->Unlock(start, length, 0, 0);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While unlocking buffer Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}

		m_Notifypos = new DSBPOSITIONNOTIFY[m_PacketsInBuffer];
		m_Events = new HANDLE[m_PacketsInBuffer];
		for(int i = 0; i < m_PacketsInBuffer; i++)
		{
			m_Events[i] = CreateEvent(0,0,false,0);
			m_Notifypos[i].dwOffset = ((m_PacketsInBuffer*m_BytesInPacket) + i * m_BytesInPacket - (m_BytesPerMsec * s_mSecDecodeLatency))%(m_PacketsInBuffer*m_BytesInPacket);
			m_Notifypos[i].hEventNotify = m_Events[i];
		}

		LPDIRECTSOUNDNOTIFY8 INotify;

		res = m_Buffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&INotify);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While get notify Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}

		res = INotify->SetNotificationPositions(m_PacketsInBuffer, m_Notifypos);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While set notification Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}
		m_Mutex.Release();
		Run();
	}
}

void SoundPlayImpl::Exit()
{
	if(m_Initialized)
	{
		m_Mutex.Aquire();
		m_PrioritySet = false;
		m_Stop = true;
		m_Playing = false;
		m_InitialBufferRun = false;
		BOOL res = SetEvent(m_Events[0]);
		if(res == 0)
		{
			beatLog_Error(("SoundPlayImpl", "Stop", "Can not signal event. Pherhaps we can't join thread %s", Global::GetErrorFromErrno(WSAGetLastError()) ));
		}
		Thread::Stop();
		m_Buffer->Stop();
		m_Audiodev->Exit();
		delete[] m_Events;
		delete[] m_Notifypos;
		m_Mutex.Release();
		m_Mutex.Exit();
	}
}

void SoundPlayImpl::Start()
{
	m_Mutex.Aquire();
	if (!m_Playing && m_JitterBuffer!=0)
	{
		beatLog_Debug(("SoundPlayImpl", "Start", "Start"));
		if(!m_Initialized)
		{
			throw Exception(_S("SoundPlayImpl::Start() Can not start when not initialized"));
		}
		HRESULT res;

		res = m_Buffer->SetCurrentPosition((m_BytesInPacket * m_PacketsInBuffer) - (m_BytesPerMsec * s_mSecInitialBuffer) - (m_BytesPerMsec * s_mSecDecodeLatency));
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While set position Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}
		//speex_echo_playback(echo_state, echo_frame);

		res = m_Buffer->Play(0, 0, DSBPLAY_LOOPING);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While play Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}
	}
	m_Playing = true;
	m_Mutex.Release();
}

void SoundPlayImpl::Stop()
{
	m_Mutex.Aquire();
	if(m_Playing)
	{
		beatLog_Debug(("SoundPlayImpl", "Stop", "Stop"));
		m_Playing = false;
		m_InitialBufferRun = true;
		HRESULT res = m_Buffer->Stop();
		if (!SUCCEEDED(res))
		{
			beatLog_Error(("SoundPlayImpl", "Stop", "Can not stop playing DirectX error: %d", res));
		}
	}
	m_Mutex.Release();
}

void SoundPlayImpl::Svc()
{
	if(!m_PrioritySet)
	{
		m_PrioritySet = true;
		try
		{
			Thread::SetPriority(Thread::HIGHEST);
		}
		catch(Exception& e)
		{
			beatLog_Error(("SoundPlayImpl", "Svc", "Can not set Thres prio: %s", e.ToString().c_str()));
		}
	}
	DWORD res = WaitForMultipleObjects(m_PacketsInBuffer, m_Events, false, INFINITE);
	if(res == WAIT_FAILED)
	{
		beatLog_Error(("SoundPlayImpl", "Svc", "Wait for multiple Objects failed: %s", Global::GetErrorFromErrno(GetLastError())));
	}

	//m_Mutex.Aquire();

	int objIndex = res - WAIT_OBJECT_0;
	if(m_InitialBufferRun)
	{
		//beatLog_Error(("SoundPlayImpl", "Svc", "Wait for multiple Objects failed: %d", objIndex));
		if(objIndex == 0) m_InitialBufferRun = false;
	}

	if(!m_InitialBufferRun && m_JitterBuffer && !m_Stop && m_Playing)
	{
		long freq;
		Packet* packet = m_JitterBuffer->GetPacket(&freq, m_Format.nSamplesPerSec);
		int offset = m_BytesInPacket * objIndex;
		int length = m_BytesInPacket;
		void* start1 = 0;
		void* start2 = 0;
		DWORD length1 = 0;
		DWORD length2 = 0;

		res = m_Buffer->Lock(offset,length,&start1, &length1, &start2, &length2,0);
		if (!SUCCEEDED(res))
		{
			beatLog_Error(("SoundPlayImpl", "Svc", "Can not Lock buffer DirectX error: %d", res));
		}
		if(length2 != 0)
		{
			throw Exception(_S("SoundPlayImpl::Svc() Buffer does wrap! TSNH!!!!!"));
		}

		length = DecompressPacket(packet, (char*)start1, length1);
		if(length != length1)
		{
			beatLog_Error(("SoundPlayImpl", "Svc", "More bytes written than locked !!!!"));
		}

		m_Buffer->Unlock(start1,length1,start2,length2);
		if (!SUCCEEDED(res))
		{
			beatLog_Error(("SoundPlayImpl", "Svc", "Can not unlock buffer DirectX error: %d", res));
		}

		if(m_JitterBuffer != 0)
		{
			m_Buffer->SetFrequency(freq);
			if (!SUCCEEDED(res))
			{
				beatLog_Error(("SoundPlayImpl", "Svc", "Can not set Frequency to buffer DirectX error: %d", res));
			}
		}

		if(packet)
		{
			delete packet;
		}
	}
	//m_Mutex.Release();
}

#else

// ========================================== SoundPlayImpl Linux =======================================


#endif
