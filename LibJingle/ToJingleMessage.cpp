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

#include "StdAfx.h"
#include "ToJingleMessage.h"
#include "beat/SString.h"

ToJingleMessage::ToJingleMessage(void):
m_Type(ToJingleMessage::UNKNOWN)
,m_Mutex(0)
{
}

ToJingleMessage::~ToJingleMessage(void)
{
	if(m_Mutex != 0)
	{
		m_Mutex->Exit();
		delete m_Mutex;
	}
}

void ToJingleMessage::Type(TYPE type)
{
	LockMessage();
	m_Type = type;
	UnLockMessage();
}

ToJingleMessage::TYPE ToJingleMessage::Type()
{
	return m_Type;
}

void ToJingleMessage::operator=(const ToJingleMessage& msg)
{
	LockMessage();
	m_Type = msg.m_Type;
    m_ToJid.assign(msg.m_ToJid);
	m_Message.assign(msg.m_Message);
	UnLockMessage();
}

void ToJingleMessage::LockMessage()
{
	if( m_Mutex == 0)
	{
		m_Mutex = new Mutex();
		m_Mutex->Init();
	}
	m_Mutex->Aquire();
}

void ToJingleMessage::UnLockMessage()
{
	m_Mutex->Release();
}

void ToJingleMessage::Message(TCHAR* msg)
{
	LockMessage();
	SString sMsg(msg);
	m_Message.assign(sMsg.c_str());
	UnLockMessage();
}

std::string& ToJingleMessage::Message()
{
	return m_Message;
}

void ToJingleMessage::ToJid(TCHAR* tojid)
{
    LockMessage();
    SString sMsg(tojid);
    m_ToJid.assign(sMsg.c_str());
    UnLockMessage();
}

std::string& ToJingleMessage::ToJid()
{
    return m_ToJid;
}

void ToJingleMessage::SetJidAndType(TCHAR* tojid, TYPE type)
{
	LockMessage();
	SString sJid(tojid);
	m_ToJid.assign(sJid.c_str());
	m_Type = type;
	UnLockMessage();
}

void ToJingleMessage::SetChatMsgAndType(const std::string& tojid, const std::string& chatmsg, TYPE type)
{
    LockMessage();
    m_ToJid = tojid;
    m_Message = chatmsg;
    m_Type = type;
    UnLockMessage();
}
