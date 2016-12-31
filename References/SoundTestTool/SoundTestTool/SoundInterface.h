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

#if !defined(SoundInterface_h_INCLUDED)
#define SoundInterface_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"
#include "Packet.h"
//#include "NetAddress.h"
//#include "MyTime.h"
class NetAddress
{

};

class PacketReciever
{
public:
	PacketReciever();
	virtual ~PacketReciever();
	virtual void AddPacket(Packet* packet, NetAddress* address) = 0;
};

class PacketSender
{
public:
	PacketSender(void);
	virtual ~PacketSender(void);
	void AddPacketReceiver(PacketReciever* packetReceiver);
	void RemovePacketReceiver(PacketReciever* packetReceiver);
protected:
	void SendPacketToReceivers(Packet* packet, NetAddress* address);
private:
	const static int s_PacketRecieverArrayGrowthStep = 10;
	unsigned long m_PacketRecieverCount;
	unsigned long m_PacketRecieverArraySize;
	PacketReciever** m_PacketRecievers;
	void EnlargePacketRecieverArray();
};

#endif // SoundInterface_h_INCLUDED
