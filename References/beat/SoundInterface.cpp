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

#include "SoundInterface.h"

PacketReciever::PacketReciever()
{
}

PacketReciever::~PacketReciever()
{
}

PacketSender::PacketSender():
m_PacketRecieverCount(0)
,m_PacketRecieverArraySize(0)
,m_PacketRecievers(0)
{
}

PacketSender::~PacketSender()
{
	if( m_PacketRecievers != 0)
	{
		delete [] m_PacketRecievers;
	}
}

void PacketSender::AddPacketReceiver(PacketReciever* packetReciever)
{
	if(packetReciever != 0)
	{
		if(m_PacketRecievers == 0 || m_PacketRecieverCount == m_PacketRecieverArraySize)
		{
			EnlargePacketRecieverArray();
		}
		m_PacketRecievers[m_PacketRecieverCount++] = packetReciever;
	}
}

void PacketSender::RemovePacketReceiver(PacketReciever* packetReciever)
{
	if(packetReciever != 0)
	{
		for(unsigned long i = 0; i < m_PacketRecieverCount; i++)
		{
			if(m_PacketRecievers[i] == packetReciever)
			{
				m_PacketRecievers[i] = 0;
				break;
			}
		}
	}
}

void PacketSender::SendPacketToReceivers(Packet* packet, NetAddress* address)
{
	for( unsigned long i = 0; i < m_PacketRecieverCount; i++)
	{
		if(m_PacketRecievers[i] != 0)
		{
			m_PacketRecievers[i]->AddPacket(packet, address);
		}
	}
}

void PacketSender::EnlargePacketRecieverArray()
{
	PacketReciever** tmpRecievers = new PacketReciever* [m_PacketRecieverArraySize+s_PacketRecieverArrayGrowthStep];
	if(m_PacketRecievers != 0)
	{
		memcpy(tmpRecievers, m_PacketRecievers, m_PacketRecieverCount*sizeof(PacketReciever*));
		delete[] m_PacketRecievers;
	}
	m_PacketRecievers = tmpRecievers;
}
