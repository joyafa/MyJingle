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

#include "SoundRecorder.h"

SoundSerializer::SoundSerializer(const SCHAR* filename):
m_Serializer(filename, FileWriter::Mode::APPEND)
,PacketReciever()
,m_PacketsInFile(0)
{
}

SoundSerializer::~SoundSerializer(void)
{
	m_Serializer.Serialize(&m_PacketsInFile, sizeof(int));
}

void SoundSerializer::AddPacket(Packet* packet, NetAddress* address)
{
	if(packet != 0)
	{
		m_Serializer.Serialize(packet->GetPacket(), packet->PacketLength());
		//m_Serializer.Serialize(address, sizeof(NetAddress));
		m_PacketsInFile++;
	}
}

SoundDeserializer::SoundDeserializer(const SCHAR* filename):
m_Serializer(filename, FileWriter::Mode::APPEND)
,PacketSender()
,m_PacketsCount(0)
,m_PosInPacketsArray(0)
,m_Packets(0)
,m_NetAddress(0)
{
}

SoundDeserializer::~SoundDeserializer(void)
{
}

void SoundDeserializer::Init()
{
	m_PacketsCount = m_Serializer.ObjectsInFile();
	// throws Exception but can't be handled here
	m_Packets = new Packet*[m_PacketsCount];
	int length;
	for(int i = 0; i < m_PacketsCount; m_PacketsCount++)
	{
		m_Serializer.Deserialize((void**)&m_Packets[i], &length, i);
	}
}

void SoundDeserializer::Exit()
{
	for(int i = 0; i < m_PacketsCount; i++)
	{
		delete m_Packets[i];
	}
	delete[] m_Packets;
}

void SoundDeserializer::Address(NetAddress* address)
{
	m_NetAddress = address;
}

NetAddress* SoundDeserializer::Address()
{
	return m_NetAddress;
}



void SoundDeserializer::EmitPacket()
{
	SendPacketToReceivers(m_Packets[m_PosInPacketsArray], m_NetAddress);
	m_PosInPacketsArray = ++m_PosInPacketsArray % m_PacketsCount;
}