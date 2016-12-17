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
#include "FromJingleMessageBase.h"
#include <string>

class CRosterChangeMessage :
	public CMessageBase
{
public:
	CRosterChangeMessage(void);
	~CRosterChangeMessage(void);

	enum ChangeType
	{
		UNKNOWN,
		ADDED,
		REMOVED
	};
	void Handle(void);
	const char* Jid();
	void Jid(const char* jid);
	void Type(ChangeType type);
	ChangeType Type();
protected:
	void Debug();
private:
	std::string m_sJID;
	ChangeType m_Type;
};

class CSessionStateChangeMessage :
	public CMessageBase
{
public:
	CSessionStateChangeMessage(void);
	~CSessionStateChangeMessage(void);
	enum ChangeType
	{
		UNKNOWN,
		CONNECTING,
		LOGGINGIN,
		LOGGEDIN,
	};
	void Handle(void);
	void Type(ChangeType type);
	ChangeType Type();
protected:
	void Debug();
private:
	ChangeType m_Type;
};

class CSessionStateChangeDisconnectMessage :
	public CSessionStateChangeMessage
{
public:
	CSessionStateChangeDisconnectMessage(void);
	~CSessionStateChangeDisconnectMessage(void);
	enum ChangeType
	{
		ERROR_NONE,
		ERROR_XML,
		ERROR_STREAM,
		ERROR_VERSION,
		ERROR_UNAUTHORIZED,
		ERROR_TLS,
		ERROR_AUTH,
		ERROR_BIND,
		ERROR_CONNECTION_CLOSED,
		ERROR_DOCUMENT_CLOSED,
		ERROR_SOCKET
	};
	void Handle(void);
	void Type(ChangeType type);
	ChangeType Type();
protected:
	void Debug();
private:
	ChangeType m_Type;
};

class CCallChangeMessage :
	public CMessageBase
{
public:
	CCallChangeMessage(void);
	~CCallChangeMessage(void);

	enum ChangeType
	{
		UNKNOWN,
		INCOMING,
		CALLING,
		ANSWERED,
		NOT_ANSWERED,
		IN_PROGRESS,
		TERMINATE,
		SENTTERMINATE
	};
	void Handle(void);
	const char* Jid();
	void Jid(const char* jid);
	void Type(ChangeType type);
	ChangeType Type();
protected:
	void Debug();
private:
	std::string m_sJID;
	ChangeType m_Type;
};

class CBeatLogMessage :
	public CMessageBase
{
public:
	CBeatLogMessage(void);
	~CBeatLogMessage(void);

	void Handle(void);
	const char* Message();
	void Message(const char* msg);
protected:
	void Debug();
private:
	std::string m_Msg;
};

class CXMLInLogMessage :
	public CMessageBase
{
public:
	CXMLInLogMessage(void);
	~CXMLInLogMessage(void);

	void Handle(void);
	const char* Message();
	void Message(const char* msg, int length);
protected:
	void Debug();
private:
	std::string m_Msg;
};

class CXMLOutLogMessage :
	public CMessageBase
{
public:
	CXMLOutLogMessage(void);
	~CXMLOutLogMessage(void);

	void Handle(void);
	const char* Message();
	void Message(const char* msg, int length);
protected:
	void Debug();
private:
	std::string m_Msg;
};

class CChatMessage :
    public CMessageBase
{
public:
    CChatMessage(void);
    ~CChatMessage(void);

    void Handle(void);
    const char* Message();
    const char* FromJid();
    void Message(const std::string& fromjid, const std::string& msg);
protected:
    void Debug();
private:
    std::string m_FromJid;
    std::string m_Msg;
};
