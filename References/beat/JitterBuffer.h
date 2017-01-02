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

#if !defined(PlayBuffer_h_INCLUDED)
#define PlayBuffer_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"
#include "Thread.h"
#include "Packet.h"
#include "Jitter.h"
#include "SoundInterface.h"

class BufferElem
{
public:
	BufferElem(void);
	~BufferElem(void);
	char* Packet;
	int Length;
	int nSeq;
private:
};

class JitterBuffer: public PacketReciever, public PacketSender
{
public:
	JitterBuffer(void);
	~JitterBuffer(void);
	void Init();
	void Exit();
	void AddPacket(Packet* packet, NetAddress* address);
	Packet* GetPacket(long* frequency, long origFrequency);
	float AveragePacketsInBuffer();
	unsigned int PacketsInBuffer();
protected:
	//TODO:改成40 也没啥效果 20--->40,网上有人时间过20个最好
	const static unsigned int s_MaxElems = 20;
	Packet* m_Packets[s_MaxElems];
	int m_Anfang;
	int m_Ende;
	void PlayStart();
	void PlayStop();
	Mutex m_Mutex;
	//累计连续空包次数
	unsigned int m_DontHaveAPacketSince;
	float m_AveragePacketsInBuffer;
	//衰减次数
	const static int s_Attenuation = 3;
	JitterStrategy m_JitterStrategy;
	const static unsigned int s_DetectSilencePacketCount = 10;
	bool m_Silence;
};

#endif
