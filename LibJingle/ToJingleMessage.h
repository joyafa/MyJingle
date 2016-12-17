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
#include <string>
#include "beat/Thread.h"

class ToJingleMessage
{
public:
	ToJingleMessage(void);
	~ToJingleMessage(void);
	enum TYPE
 	{
		UNKNOWN,
		CALL,
		HANGUP,
		ACCEPT_CALL,
		REJECT_CALL,
		EXIT,
        STR_CHAT
	};
	void Type(TYPE type);
	TYPE Type();
	void operator=(const ToJingleMessage& msg);
	void LockMessage();
	void UnLockMessage();
	void Message(TCHAR* msg);
	std::string& Message();
    void ToJid(TCHAR* tojid);
    std::string& ToJid();
	void SetJidAndType(TCHAR* tojid, TYPE type);
    void SetChatMsgAndType(const std::string& tojid, const std::string& chatmsg, TYPE type);
private:
	TYPE m_Type;
	std::string m_Message;
    std::string m_ToJid;
	Mutex* m_Mutex;
};
