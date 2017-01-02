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

#include "NetAddress.h"

NetAddress::NetAddress(void):
m_IP(0)
,m_Port(0)
,m_Dirty(true)
{
}

NetAddress::~NetAddress(void)
{
}

struct sockaddr_in * NetAddress::Address()
{
	if ( m_Dirty )
	{
		m_NetAddress.sin_family = AF_INET;
		if (m_IP == 0)
		{
			m_NetAddress.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			m_NetAddress.sin_addr.s_addr = m_IP;
		}
		m_NetAddress.sin_port = htons(m_Port);
		m_Dirty = false;
	}

	return &m_NetAddress;
}

void NetAddress::Address(struct sockaddr_in * address)
{
	if (address == 0)
	{
		m_IP = 0;
		m_Port = 0;
		m_Dirty = true;
	}
	else
	{
		m_NetAddress = *address;
		m_Port = ntohs(m_NetAddress.sin_port);
		m_IP = m_NetAddress.sin_addr.s_addr;
		m_Dirty = false;
	}
}


void NetAddress::Port(unsigned short port)
{
	m_Dirty = true;
	m_Port = port;
}

unsigned short NetAddress::Port(void)
{
	return m_Port;
}

unsigned long NetAddress::IP() const
{
	return m_IP;
}

void NetAddress::IP(const SCHAR* ip)
{
	SString sIp(ip);
	IP(inet_addr(sIp.c_str()));
}

void NetAddress::IP(unsigned long ip)
{
	m_IP = ip;
	m_Dirty = true;
}
