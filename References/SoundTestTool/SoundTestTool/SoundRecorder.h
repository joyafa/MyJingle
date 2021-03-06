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

#if !defined(SoundRecorder_h_INCLUDED)
#define SoundRecorder_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WM_MSG_SEND_RECORDSTATUS WM_USER + 1

#include "Global.h"
#include "SoundInterface.h"
#include "Serializer.h"
#include "JitterBuffer.h"
class SoundSerializer : public PacketReciever
{
public:
	SoundSerializer(const SCHAR* filename);
	~SoundSerializer(void);
	void AddRwdata(char *pData, int nLength);
	void AddPacket(Packet* packet, NetAddress* address = NULL);
private:
	int m_PacketsInFile;
	Serializer m_Serializer;
};

class SoundDeserializer : public PacketSender
{
public:
	SoundDeserializer(const SCHAR* filename);
	~SoundDeserializer(void);
	void Init();
	void Exit();
	void EmitPacket();
	void Address(NetAddress* netAddress);
	NetAddress* Address(void);

	void SetJitterBuffer(JitterBuffer *pJitterBuffer)
	{
		m_pJitterBuffer = pJitterBuffer;
	}

	int SetJitterBufferData()
	{
		if (NULL == m_pJitterBuffer) return 0;
		for (int i=0;i<m_PacketsCount;++i)
		{
			m_pJitterBuffer->AddPacket(m_Packets[i]);
		}

		return m_PacketsCount;
	}
private:
	int m_PacketsCount;
	int m_PosInPacketsArray;
	Packet** m_Packets;
	Serializer m_Serializer;
	NetAddress* m_NetAddress;
	::JitterBuffer *m_pJitterBuffer;
};

#endif
