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

#include "JitterBuffer.h"
#include "SoundPlay.h"


BufferElem::BufferElem(void):
Packet(0)
,Length(0)
,nSeq(0)
{
}

BufferElem::~BufferElem()
{
}

JitterBuffer::JitterBuffer(void):
m_Anfang(0)
,m_Ende(0)
,m_DontHaveAPacketSince(0)
,m_AveragePacketsInBuffer(1)
,m_Silence(true)
,m_nIndex(0)
{
	m_Mutex.Init();
	for(unsigned int i = 0; i < s_MaxElems; i++)
	{
		m_Packets[i] = 0;
	}
}

JitterBuffer::~JitterBuffer(void)
{
}

void JitterBuffer::Init()
{
	m_Mutex.Init();
}

void JitterBuffer::Exit()
{
	m_Mutex.Exit();
}

void JitterBuffer::AddPacket(Packet* packet, NetAddress* address)
{
	// TODO: case nSeq wraps
	static int index =0;
	if (packet != 0)
	{
		m_Mutex.Aquire();
		m_Packets[index++] = packet;
		m_Mutex.Release();
		return; 
		unsigned int offset = 0;
		if(m_Packets[m_Anfang] != 0)
		{
			offset = (int)packet->SequenceNumber() - (int)m_Packets[m_Anfang]->SequenceNumber();
		}
		if(offset>=0)
		{
			int pos;
			if(offset<s_MaxElems)
			{
				pos = (m_Anfang + offset)%s_MaxElems;
			}
			else
			{
				//TODO: Reset the Buffer and inform the user
				pos = m_Anfang;
				m_Anfang = (m_Anfang+1)%s_MaxElems ;
			}

			if(m_Packets[pos]!=0)
			{
				//should never happen, but who knows
				beatLog_Debug(("PlayBuffer", "AddPacket", "packet %d is too late and will be disposed", packet->SequenceNumber()));
				delete m_Packets[pos];
			}
			m_Packets[pos] = packet;
		}
		m_Mutex.Release();
	}
	PlayStart();
}

Packet* JitterBuffer::GetPacket()
{
	SString ss;
	ss.strcatf(_S("GetPacket: 取 第 %d 帧声音数据\n"), m_nIndex);
	OutputDebugString(ss);
	if (m_nIndex < s_MaxElems)
	{
		return m_Packets[m_nIndex++];
	}
	return NULL;
}

Packet* JitterBuffer::GetPacket(long* frequency, long origFrequency)
{
	m_Mutex.Aquire();

	unsigned int packetsInBuffer = PacketsInBuffer();

	Packet* packet = m_Packets[m_Anfang];
	m_Packets[m_Anfang] = 0;

	m_Anfang = (m_Anfang+1)%s_MaxElems;

	m_Mutex.Release();

	if(packet == 0)
	{
		m_DontHaveAPacketSince++;
	}
	else
	{
		m_DontHaveAPacketSince = 0;
	}

	if((packet != 0 && packet->Silence()) || m_Silence)
	{
		m_AveragePacketsInBuffer = (float)packetsInBuffer;
	}
	else
	{
		m_AveragePacketsInBuffer = (m_AveragePacketsInBuffer * (s_Attenuation - 1) + packetsInBuffer)/s_Attenuation;
	}

	if(packet != 0) m_Silence = false;

	if(m_DontHaveAPacketSince >= s_DetectSilencePacketCount) // no more packets means Silence
	{
		m_Silence = true;
	}

	if(m_Silence)
	{
		PlayStop();
		packet = 0;
	}

	*frequency = m_JitterStrategy.FrequenzFactor(m_AveragePacketsInBuffer, origFrequency);
	//*frequency = origFrequency;

	return packet;
}

void JitterBuffer::PlayStart()
{
	//PlayStop();
	//try
	//{
	//	if(m_SoundPlayer!=0)
	//	{
	//		m_SoundPlayer->Start();
	//	}
	//}
	//catch(Exception& e)
	//{
	//	beatLog_Error(("PlayBuffer", "Start", "m_SoundPlayer.Start() failed: %s", e.ToString().c_str()));
	//}
}

void JitterBuffer::PlayStop()
{
	//try
	//{
	//	if(m_SoundPlayer)
	//	{
	//		m_SoundPlayer->Stop();
	//	}
	//}
	//catch(Exception& e)
	//{
	//	beatLog_Error(("PlayBuffer", "SoundPlayer", "m_SoundPlayer.Stop() failed: %s", e.ToString().c_str()));
	//}
}

float JitterBuffer::AveragePacketsInBuffer()
{
	return m_AveragePacketsInBuffer;
}

unsigned int JitterBuffer::PacketsInBuffer()
{
	unsigned int packets = 0;
	if(m_Packets[m_Anfang] != 0)
	{
		int firstSeq = m_Packets[m_Anfang]->SequenceNumber();
		for(; packets < s_MaxElems; packets++)
		{
			if (m_Packets[(m_Anfang + packets) % s_MaxElems] == 0
				|| m_Packets[(m_Anfang + packets) % s_MaxElems]->SequenceNumber() < firstSeq)
			{
				break;
			}
		}
	}
	return packets;
}
