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

#include "DirectXFullDuplexAudioDevice.h"
#include "objbase.h"
#include "SString.h"
#include "Exception.h"
#include <iostream>

DirectXFullDuplexAudioDevice* gDirectXFullDuplexAudioDevice = 0;
DirectXFullDuplexAudioDevice* GetDirectXFullDuplexAudioDeviceInstance(WAVEFORMATEX& format, HWND window)
{
	if (gDirectXFullDuplexAudioDevice == 0) {
		gDirectXFullDuplexAudioDevice = new DirectXFullDuplexAudioDevice(format, window);
	}
	return gDirectXFullDuplexAudioDevice;
}

DirectXFullDuplexAudioDevice::DirectXFullDuplexAudioDevice(WAVEFORMATEX& format, HWND window):
m_Format(format)
,m_Window(window)
,m_Usage(0)
,m_CaptureDevice(0)
,m_CaptureBuffer(0)
,m_PlayDevice(0)
,m_PlayBuffer(0)
,m_BytesInPacket(-1)
,m_BytesPerMsec(-1)
{
}


#define LS(r) {r, _S(#r)}

SCHAR *DirectXFullDuplexAudioDevice::GetErrName(int ErrID)
{
	static struct 
	{
		int nErrID;
		SCHAR *pErrName;
	}ErrName[] =
	{
		LS(DSERR_ALLOCATED         ),     
		LS(DSERR_CONTROLUNAVAIL    ), 
		LS(DSERR_INVALIDPARAM      ), 
		LS(DSERR_INVALIDCALL       ), 
		LS(DSERR_GENERIC           ), 
		LS(DSERR_PRIOLEVELNEEDED   ), 
		LS(DSERR_OUTOFMEMORY       ), 
		LS(DSERR_BADFORMAT         ), 
		LS(DSERR_UNSUPPORTED       ), 
		LS(DSERR_NODRIVER          ), 
		LS(DSERR_ALREADYINITIALIZED), 
		LS(DSERR_NOAGGREGATION     ), 
		LS(DSERR_BUFFERLOST        ), 
		LS(DSERR_OTHERAPPHASPRIO   ), 
		LS(DSERR_UNINITIALIZED     ), 
		LS(DSERR_NOINTERFACE       ), 
		LS(DSERR_ACCESSDENIED      ), 
		LS(DSERR_BUFFERTOOSMALL    ), 
		LS(DSERR_DS8_REQUIRED      ), 
		LS(DSERR_SENDLOOP          ), 
		LS(DSERR_BADSENDBUFFERGUID ), 
		LS(DSERR_OBJECTNOTFOUND    ), 
		LS(DSERR_FXUNAVAILABLE     ), 
		0, _S("")
	};
	using namespace std;
	for (int i=0;i<sizeof(ErrName) / sizeof(ErrName[0]);++i)
	{
		cout << ErrName[i].nErrID << endl;
		if (ErrID == ErrName[i].nErrID)
		{
			return ErrName[i].pErrName;
		}
	}

	return _S("");
}


void DirectXFullDuplexAudioDevice::Init()
{
	if (m_Usage == 0)
	{
		HRESULT res;

		res = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (!SUCCEEDED(res))
		{
			SString message;
			message.strcatf(_S("While init COM, Error: %d"), res);
			throw Exception(message);
		}

		ZeroMemory(m_Effects, sizeof(DSCEFFECTDESC)*2);

		m_Effects[0].dwSize = sizeof(DSCEFFECTDESC);
		m_Effects[0].guidDSCFXClass = GUID_DSCFX_CLASS_AEC;
		m_Effects[0].guidDSCFXInstance = GUID_DSCFX_MS_AEC;
		m_Effects[0].dwFlags = DSCFX_LOCSOFTWARE;

		m_Effects[1].dwSize = sizeof(DSCEFFECTDESC);
		m_Effects[1].guidDSCFXClass = GUID_DSCFX_CLASS_NS;
		m_Effects[1].guidDSCFXInstance = GUID_DSCFX_MS_NS;
		m_Effects[1].dwFlags = DSCFX_LOCSOFTWARE;

		ZeroMemory(&m_BufferDesc, sizeof(DSCBUFFERDESC));
		m_BufferDesc.dwSize = sizeof(DSCBUFFERDESC);
		m_BufferDesc.dwBufferBytes = s_PacketsInBuffer * BytesInPacket();//1个包是20ms的数据,  40*20=800ms
		m_BufferDesc.dwReserved = 0;
		m_BufferDesc.lpwfxFormat = &m_Format;
		m_BufferDesc.dwFlags = DSCBCAPS_CTRLFX;
		m_BufferDesc.lpDSCFXDesc = m_Effects;
		m_BufferDesc.dwFXCount = 2;

		ZeroMemory(&m_PlayBufferDesc, sizeof(DSBUFFERDESC));
		m_PlayBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		m_PlayBufferDesc.dwBufferBytes = s_PacketsInBuffer * BytesInPacket();
		//DSBCAPS_GLOBALFOCUS 标志位保证当程序窗口不在前面时仍能够播放，没有这个标志位，当其它程序或者对话框拥有焦点的时候缓冲区会静音
		m_PlayBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2;
		m_PlayBufferDesc.dwReserved = 0;
		m_PlayBufferDesc.lpwfxFormat = &m_Format;

		//m_Window不能为空,可能使用来做消息循环,发送消息需要有窗口句柄
		LPDIRECTSOUNDFULLDUPLEX fd;
		res = DirectSoundFullDuplexCreate8(
			&DSDEVID_DefaultCapture,
			&DSDEVID_DefaultPlayback,
			&m_BufferDesc,
			&m_PlayBufferDesc,
			m_Window,
			DSSCL_NORMAL,
			&fd,
			&m_CaptureBuffer,
			&m_PlayBuffer,
			0  // reserved
			);
		if (!SUCCEEDED(res))
		{
			m_BufferDesc.dwFXCount = 0;
			m_BufferDesc.lpDSCFXDesc = 0;

			res = DirectSoundFullDuplexCreate8(
			&DSDEVID_DefaultCapture,
			&DSDEVID_DefaultPlayback,
			&m_BufferDesc,
			&m_PlayBufferDesc,
			m_Window,
			DSSCL_NORMAL,
			&fd,
			&m_CaptureBuffer,
			&m_PlayBuffer,
			0  // reserved
			);
			if (!SUCCEEDED(res))
			{
				SString message;
				message.strcatf(_S("While init Capture Direct Sound, DirectX Error: %s"), GetErrName(res));
				throw Exception(message);
			}
		}
	}
	m_Usage++;
}

void DirectXFullDuplexAudioDevice::Exit()
{
	m_Usage--;
	if (m_Usage == 0)
	{
		//这里貌似有问题,退出时释放com控件
		CoUninitialize();
		/*HRESULT res;
		res = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (!SUCCEEDED(res))
		{
		SString message;
		message.strcatf(_S("While exit COM, Error: %d"), res);
		throw Exception(message);
		}*/
	}
}

WAVEFORMATEX& DirectXFullDuplexAudioDevice::Format()
{
	return m_Format;
}

HWND DirectXFullDuplexAudioDevice::Window()
{
	return m_Window;
}

LPDIRECTSOUNDCAPTURE8 DirectXFullDuplexAudioDevice::CaptureDevice()
{
	return m_CaptureDevice;
}

LPDIRECTSOUNDCAPTUREBUFFER8 DirectXFullDuplexAudioDevice::CaptureBuffer()
{
	return m_CaptureBuffer;
}

LPDIRECTSOUND8 DirectXFullDuplexAudioDevice::PlayDevice()
{
	return m_PlayDevice;
}

LPDIRECTSOUNDBUFFER8 DirectXFullDuplexAudioDevice::PlayBuffer()
{
	return m_PlayBuffer;
}

int DirectXFullDuplexAudioDevice::PacketsInBuffer()
{
	return s_PacketsInBuffer;
}

int DirectXFullDuplexAudioDevice::BytesInPacket()
{
	if (m_BytesInPacket == -1)
	{
		m_BytesInPacket = m_Format.nAvgBytesPerSec/50 * 10;
	}
	return m_BytesInPacket;
}

int DirectXFullDuplexAudioDevice::BytesPerMsec()
{
	if (m_BytesPerMsec== -1)
	{
		m_BytesPerMsec = m_Format.nAvgBytesPerSec/1000;
	}
	return m_BytesPerMsec;
}
