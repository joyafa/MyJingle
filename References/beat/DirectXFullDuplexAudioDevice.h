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

#if !defined(DirectXFullDuplexAudioDevice_h_INCLUDED)
#define DirectXFullDuplexAudioDevice_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

#if defined (WIN32)
#include "Windows.h"
#include "Dsound.h"

class DirectXFullDuplexAudioDevice
{
public:
	DirectXFullDuplexAudioDevice(WAVEFORMATEX& format, HWND window);
	void Init();
	void Exit();
	WAVEFORMATEX& Format();
	HWND Window();
	LPDIRECTSOUNDCAPTURE8 CaptureDevice();
	LPDIRECTSOUNDCAPTUREBUFFER8 CaptureBuffer();
	LPDIRECTSOUND8 PlayDevice();
	LPDIRECTSOUNDBUFFER8 PlayBuffer();
	int PacketsInBuffer();
	int BytesInPacket();
	int BytesPerMsec();
private:
	WAVEFORMATEX& m_Format;
	HWND m_Window;
	int m_Usage;
	LPDIRECTSOUNDCAPTURE8 m_CaptureDevice;
	LPDIRECTSOUNDCAPTUREBUFFER8 m_CaptureBuffer;
	LPDIRECTSOUND8 m_PlayDevice;
	LPDIRECTSOUNDBUFFER8 m_PlayBuffer;
	//TODO:由原来的 10--->40 效果貌似没好, 这个值调大些,就比较连续了,不会说快了听不完整
	static const int s_PacketsInBuffer = 20;
	int m_BytesInPacket;
	int m_BytesPerMsec;
	DSCEFFECTDESC m_Effects[2];
	DSCBUFFERDESC m_BufferDesc;
	DSBUFFERDESC m_PlayBufferDesc;
};

DirectXFullDuplexAudioDevice* GetDirectXFullDuplexAudioDeviceInstance(WAVEFORMATEX& format, HWND window);

#endif

#endif
