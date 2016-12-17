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

#if !defined(NetAddress_h_INCLUDED)
#define NetAddress_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

#if defined (WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "SString.h"

class NetAddress
{
public:
	NetAddress(void);
	~NetAddress(void);

	struct sockaddr_in * Address(void);
	void Address(struct sockaddr_in *);

	void Port(unsigned short port);
	unsigned short Port(void);

	void IP(const SCHAR* ip);
	void IP(unsigned long ip);
	unsigned long IP(void) const;

protected:
	struct sockaddr_in m_NetAddress;
	unsigned long m_IP;
	unsigned short m_Port;
	bool m_Dirty;
};

#endif // !defined(NetAddress_h_INCLUDED)
