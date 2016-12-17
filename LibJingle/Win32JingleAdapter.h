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

#pragma once
#include "talk/base/sigslot.h"
#include "talk/base/PhysicalSocketServer.h"
#include "ToJingleMessage.h"

namespace cricket
{
	class Dispatcher {
	public:
	  virtual uint32 GetRequestedEvents() = 0;
	  virtual void OnPreEvent(uint32 ff) = 0;
	  virtual void OnEvent(uint32 ff, int err) = 0;
	  virtual WSAEVENT GetWSAEvent() = 0;
	  virtual SOCKET GetSocket() = 0;
	  virtual bool CheckSignalClose() = 0;
	};
}

class Win32JingleAdapter: public cricket::Dispatcher, public sigslot::has_slots<>
{
public:
	Win32JingleAdapter(cricket::PhysicalSocketServer& ss);
	~Win32JingleAdapter(void);
	void OnPreEvent(uint32 ff);
	void OnEvent(uint32 ff, int err);
	uint32 GetRequestedEvents();
	int GetDescriptor();
	sigslot::signal1<ToJingleMessage&> SignalMessage;
	WSAEVENT GetWSAEvent();
	void SetEvent();
	SOCKET GetSocket();
	bool CheckSignalClose();
	void Exit();
	void Message(ToJingleMessage& msg);
	ToJingleMessage& Message();
protected:
	WSAEVENT m_Event;
	cricket::PhysicalSocketServer& m_SS;
	ToJingleMessage m_Message;
};
