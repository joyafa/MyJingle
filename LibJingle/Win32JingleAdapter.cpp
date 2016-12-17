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

#include "Win32JingleAdapter.h"

Win32JingleAdapter::Win32JingleAdapter(cricket::PhysicalSocketServer& ss):
m_SS(ss)
{
	m_Event = WSACreateEvent();
    m_SS.Add(this);
}

Win32JingleAdapter::~Win32JingleAdapter(void)
{
	m_SS.Remove(this);
}

uint32 Win32JingleAdapter::GetRequestedEvents()
{
    return 0;
}

void Win32JingleAdapter::OnPreEvent(uint32 ff)
{
	WSAResetEvent(m_Event);
}

void Win32JingleAdapter::OnEvent(uint32 ff, int err)
{
	SignalMessage.emit(m_Message);
}

int Win32JingleAdapter::GetDescriptor()
{
	return 0;
}

WSAEVENT Win32JingleAdapter::GetWSAEvent()
{
  return m_Event;
}

void Win32JingleAdapter::SetEvent()
{
   WSASetEvent(m_Event);
}

SOCKET Win32JingleAdapter::GetSocket()
{
return INVALID_SOCKET;
}

bool Win32JingleAdapter::CheckSignalClose()
{
	return false;
}

void Win32JingleAdapter::Exit()
{
}

void Win32JingleAdapter::Message(ToJingleMessage& msg)
{
	m_Message = msg;
}

ToJingleMessage& Win32JingleAdapter::Message()
{
	return m_Message;
}
