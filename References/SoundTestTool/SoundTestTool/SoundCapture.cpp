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

#include "SoundCapture.h"

SoundCapture::SoundCapture(void):
m_NetAddress(0)
,m_nSeq(0)
,m_FrameSize(-1)
,m_Encoder(0)
,m_SoundSerializer(_S("TestRecord.pcm"))
{
}

SoundCapture::SoundCapture( SCHAR *pRecordFilePath ): 
    m_SoundSerializer(pRecordFilePath)
	, m_NetAddress(0)
	,m_nSeq(0)
	,m_FrameSize(-1)
	,m_Encoder(0){

}

SoundCapture::~SoundCapture(void)
{
}

Codec* SoundCapture::Encoder()
{
	return m_Encoder;
}

void SoundCapture::Encoder(Codec* encoder)
{
	m_Encoder = encoder;
	if(m_Encoder!=0)
	{
		m_FrameSize = m_Encoder->GetCompressFrameSize();
	}
}

NetAddress* SoundCapture::ToNetAddress()
{
	return m_NetAddress;
}

void SoundCapture::ToNetAddress(NetAddress* netaddress)
{
	m_NetAddress = netaddress;
}

Packet* SoundCapture::CompressFrame(char* frame, int length)
{
	Packet* packet = Packet::CreateNewDataPacket(length, 0);
	memcpy(packet->RwData(), frame, length);
	return packet;
}

//void SoundCapture::CompressFrame(char* frame, int length)
//{
//	if(m_Encoder!=0)
//	{
//		m_nSeq++;
//		Packet* pack = m_Encoder->CompressFrame(frame, length);
//		if(pack != 0)
//		{
//			TimeValue timeValue;
//			pack->SequenceNumber(m_nSeq);
//
//			unsigned long val = timeValue.Now().Microseconds();
//			val /= 100;
//
//			pack->Timestamp(val);
//			///speex处理后的数据写入到文件中
//			//m_SoundSerializer.AddPacket(pack);
//
//			SendPacketToReceivers(pack, m_NetAddress);
//		}
//	}
//}

// ========================================== SoundCaptureImpl Windows =======================================
#if defined (WIN32)

SoundCaptureImpl::SoundCaptureImpl(WAVEFORMATEX& format, HWND window, SCHAR *pFilePath):
m_Initialized(false)
,m_Audiodev(0)
,m_Events(0)
,m_Format(format)
,m_Window(window)
,m_PacketsInBuffer(-1)
,m_BytesInPacket(-1)
,m_BytesPerMsec(-1)
,m_Capturing(false)
,m_PrioritySet(false)
,m_Notifypos(0)
, SoundCapture(pFilePath)
{
}

SoundCaptureImpl::~SoundCaptureImpl()
{
	SoundCapture::~SoundCapture();
}

void SoundCaptureImpl::Init()
{
	if(!m_Initialized)
	{
		m_Initialized = true;
		HRESULT res;
		m_Audiodev = GetDirectXFullDuplexAudioDeviceInstance(m_Format, m_Window);
		m_Audiodev->Init();

		m_PacketsInBuffer = m_Audiodev->PacketsInBuffer();//40个包
		m_CaptureBuffer = m_Audiodev->CaptureBuffer();
		m_CaptureDevice = m_Audiodev->CaptureDevice();
		m_BytesInPacket = m_Audiodev->BytesInPacket();
		m_BytesPerMsec = m_Audiodev->BytesPerMsec();

		m_Notifypos = new DSBPOSITIONNOTIFY[m_PacketsInBuffer];
		m_Events = new HANDLE[m_PacketsInBuffer];
		for(int i = 0; i < m_PacketsInBuffer; i++)
		{
			//autoreset, false  自动的,初始化为无信号
			m_Events[i] = CreateEvent(0,0,false,0);
			m_Notifypos[i].dwOffset = i * m_BytesInPacket;
			m_Notifypos[i].hEventNotify = m_Events[i];
		}
		Run();

		LPDIRECTSOUNDNOTIFY8 INotify;

		res = m_CaptureBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&INotify);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While get notify, Capture Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}

		res = INotify->SetNotificationPositions(m_PacketsInBuffer, m_Notifypos);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While set notify, Capture Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}
	}
}

void SoundCaptureImpl::Exit()
{
	if(m_Initialized)
	{
		m_Initialized = false;
		m_Stop = true;
		m_Capturing = false;
		m_PrioritySet = false;
		BOOL res = SetEvent(m_Events[0]);
		if(res == 0)
		{
			beatLog_Error(("SoundCaptureImpl", "Stop", "Can not signal event. Pherhaps we can't join thread %s", Global::GetErrorFromErrno(WSAGetLastError()) ));
		}
		Thread::Stop();
		m_Audiodev->Exit();
		delete[] m_Events;
		delete[] m_Notifypos;
	}
}

void SoundCaptureImpl::Start()
{
	if (!m_Capturing)
	{
		if(!m_Initialized)
		{
			throw Exception(_S("SoundCaptureImpl::Start() Can not start when not initialized"));
		}
		HRESULT res;

		res = m_CaptureBuffer->Start(DSCBSTART_LOOPING);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While capture Direct Sound, DirectX Error: %d"), res);
			throw Exception(message);
		}
	}
	m_Capturing = true;
}

void SoundCaptureImpl::Stop()
{
	m_Capturing = false;
}

void SoundCaptureImpl::Svc()
{
	if(!m_PrioritySet)
	{
		m_PrioritySet = true;
		try
		{
			Thread::SetPriority(Thread::REALTIME);
		}
		catch(Exception& e)
		{
			beatLog_Error(("SoundPlayImpl", "Svc", "Can not set Thread prio: %s", e.ToString().c_str()));
		}
	}
	DWORD res = WaitForMultipleObjects(m_PacketsInBuffer, m_Events, false, INFINITE);
	if(res == WAIT_FAILED)
	{
		beatLog_Error(("SoundPlayImpl", "Svc", "Wait for multiple Objects failed: %s", Global::GetErrorFromErrno(GetLastError())));
	}

	int objIndex = res - WAIT_OBJECT_0;
	beatLog_Info(("SoundPlayImpl", "Svc", "m_Events[%d] have single", objIndex));

	if(!m_Stop && m_Capturing)
	{
		int offset = m_PacketsInBuffer * m_BytesInPacket - m_BytesInPacket + objIndex * m_BytesInPacket;
		offset = offset % (m_BytesInPacket * m_PacketsInBuffer);
		beatLog_Info(("SoundPlayImpl1", "Svc", "offset = %d ", offset));

		int length = m_BytesInPacket;
		void* start1 = 0;
		void* start2 = 0;
		DWORD length1 = 0;
		DWORD length2 = 0;

		res = m_CaptureBuffer->Lock(offset,length,&start1, &length1, &start2, &length2,0);
		if (!SUCCEEDED(res))
		{
			DWORD read, write;
			m_CaptureBuffer->GetCurrentPosition(&read, &write);

			beatLog_Error(("SoundCaptureImpl", "Svc", "Reading at position %d write: %d", read, write));
			beatLog_Error(("SoundCaptureImpl", "Svc", "Trying to read at offset %d from buffer", offset, objIndex));
			beatLog_Error(("SoundCaptureImpl", "Svc", "Called for obect index:", objIndex));
			beatLog_Error(("SoundCaptureImpl", "Svc", "Can not Lock buffer DirectX error: %d", res));
		}
		if(length2 != 0)
		{
			throw Exception(_S("SoundCaptureImpl::Svc() Buffer does wrap! TSNH!!!!!"));
		}

		//TODO:记录到文件中
		//Packet *packet = CompressFrame((char*)start1, length1);
		//将声卡取到的原始数据写入到文件中,不包含头信息
		m_SoundSerializer.AddRwdata((char*)start1, length1);

		m_CaptureBuffer->Unlock(start1,length1,start2,length2);
		if (!SUCCEEDED(res))
		{
			beatLog_Error(("SoundCaptureImpl", "Svc", "Can not unlock buffer DirectX error: %d", res));
		}
		//beatLog_Debug(("SoundCaptureImpl", "Svc", "Captured a Frame"));
	}
}



// ========================================== SoundCaptureImpl Linux =======================================
#else


#endif

