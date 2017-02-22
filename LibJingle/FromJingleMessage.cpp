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
#include "FromJingleMessage.h"
#include "LibJingle.h"
#include "LibJingleDlg.h"

// ==========================================================
// ================   CRosterChangeMessage    ===============
// ==========================================================
CRosterChangeMessage::CRosterChangeMessage(void)
:m_Type(CRosterChangeMessage::UNKNOWN)
{
}

CRosterChangeMessage::~CRosterChangeMessage(void)
{
}

void CRosterChangeMessage::Handle(void)
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		switch(m_Type)
		{
		case UNKNOWN:
			// dunno
			break;
		case ADDED:
			wnd->AddToRoster(m_sJID.c_str());
			break;
		case REMOVED:
			wnd->RemoveFromRoster(m_sJID.c_str());
			break;
		}
	}
	CMessageBase::Handle();
}

void CRosterChangeMessage::Jid(const char* jid)
{
	m_sJID = jid;
}

const char* CRosterChangeMessage::Jid()
{
	return m_sJID.c_str();
}

void CRosterChangeMessage::Type(CRosterChangeMessage::ChangeType type)
{
	m_Type = type;
}

CRosterChangeMessage::ChangeType CRosterChangeMessage::Type()
{
	return m_Type;
}

void CRosterChangeMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		switch(m_Type)
		{
		case UNKNOWN:
			wnd->AddToStatusDebug(_T("WARNING: unknown roster change\r\n"));
			break;
		case ADDED:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" added to roster\r\n"));
			break;
		case REMOVED:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" removed from roster\r\n"));
			break;
		}
	}
}

// ==========================================================
// =============  CSessionStateChangeMessage  ===============
// ==========================================================
CSessionStateChangeMessage::CSessionStateChangeMessage(void):
m_Type(CSessionStateChangeMessage::UNKNOWN)
{
}

CSessionStateChangeMessage::~CSessionStateChangeMessage(void)
{
}

void CSessionStateChangeMessage::Handle(void)
{
	CMessageBase::Handle();
}

void CSessionStateChangeMessage::Type(CSessionStateChangeMessage::ChangeType type)
{
	m_Type = type;
}

CSessionStateChangeMessage::ChangeType CSessionStateChangeMessage::Type()
{
	return m_Type;
}

void CSessionStateChangeMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		switch(m_Type)
		{
		case UNKNOWN:
			wnd->AddToStatusDebug(_T("WARNING: unknown Session State change\r\n"));
			break;
		case CONNECTING:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(_T(" connecting\r\n"));
			break;
		case LOGGINGIN:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(_T(" logging in\r\n"));
			wnd->m_bLogin.EnableWindow(false);
			wnd->m_bLogout.EnableWindow(false);
			wnd->m_bCall.EnableWindow(false);
			wnd->m_bHangup.EnableWindow(false);
            wnd->m_bSendmsg.EnableWindow(false);
			break;
		case LOGGEDIN:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(_T(" logged in\r\n"));
			wnd->m_bLogin.EnableWindow(false);
			wnd->m_bLogout.EnableWindow(true);
			wnd->m_bCall.EnableWindow(true);
			wnd->m_bHangup.EnableWindow(false);
            wnd->m_bSendmsg.EnableWindow(true);
			break;
		}
	}
}

// ==========================================================
// ==========  CSessionStateChangeDisconnectMessage =========
// ==========================================================
CSessionStateChangeDisconnectMessage::CSessionStateChangeDisconnectMessage(void):
m_Type(CSessionStateChangeDisconnectMessage::ERROR_NONE)
{
}

CSessionStateChangeDisconnectMessage::~CSessionStateChangeDisconnectMessage(void)
{
}

void CSessionStateChangeDisconnectMessage::Handle(void)
{
	CMessageBase::Handle();
}

void CSessionStateChangeDisconnectMessage::Type(CSessionStateChangeDisconnectMessage::ChangeType type)
{
	m_Type = type;
}

CSessionStateChangeDisconnectMessage::ChangeType CSessionStateChangeDisconnectMessage::Type()
{
	return m_Type;
}

void CSessionStateChangeDisconnectMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		wnd->m_bLogin.EnableWindow(true);
		wnd->m_bLogout.EnableWindow(false);
		wnd->m_bCall.EnableWindow(false);
		wnd->m_bHangup.EnableWindow(false);
        wnd->m_bSendmsg.EnableWindow(false);
		wnd->m_lRoster.ResetContent();
		switch(m_Type)
		{
		case ERROR_NONE:
			wnd->AddToStatusDebug(_T("INFO: Session Terminated without a error\r\n"));
			break;
		case ERROR_XML:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error in XML\r\n"));
			break;
		case ERROR_STREAM:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error in STREAM\r\n"));
			break;
		case ERROR_VERSION:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error with version\r\n"));
			break;
		case ERROR_UNAUTHORIZED:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because we are not authorized\r\n"));
			break;
		case ERROR_TLS:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error in TLS\r\n"));
			break;
		case ERROR_AUTH:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error in AUTH\r\n"));
			break;
		case ERROR_BIND:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a error while binding to socket\r\n"));
			break;
		case ERROR_CONNECTION_CLOSED:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a Connection clause error\r\n"));
			break;
		case ERROR_SOCKET:
			wnd->AddToStatusDebug(_T("ERROR: Session Terminated because of a Socket error\r\n"));
			break;
		}
	}
}

// ==========================================================
// ================   CCallChangeMessage      ===============
// ==========================================================
CCallChangeMessage::CCallChangeMessage(void)
{
}

CCallChangeMessage::~CCallChangeMessage(void)
{
}

void CCallChangeMessage::Handle(void)
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		switch(m_Type)
		{
		case CCallChangeMessage::ANSWERED:
			//对方接听了,设置有信号
			wnd->SetAnswerEvent();
		case CCallChangeMessage::IN_PROGRESS:
		case CCallChangeMessage::CALLING:
			wnd->m_bLogin.EnableWindow(false);
			wnd->m_bLogout.EnableWindow(true);
			wnd->m_bCall.EnableWindow(false);
			wnd->m_bHangup.EnableWindow(true);
            wnd->m_bSendmsg.EnableWindow(true);
			break;
		case CCallChangeMessage::NOT_ANSWERED:
		case CCallChangeMessage::TERMINATE:
		case CCallChangeMessage::SENTTERMINATE:
			wnd->m_bLogin.EnableWindow(false);
			wnd->m_bLogout.EnableWindow(true);
			wnd->m_bCall.EnableWindow(true);
			wnd->m_bHangup.EnableWindow(false);
            wnd->m_bSendmsg.EnableWindow(true);
			//对方主动挂断
			wnd->SetRejectEvent();
			break;
		case CCallChangeMessage::INCOMING:
			wnd->AcceptCallFrom(m_sJID.c_str());
			break;
		}
	}
	CMessageBase::Handle();
}

void CCallChangeMessage::Jid(const char* jid)
{
	m_sJID = jid;
}

const char* CCallChangeMessage::Jid()
{
	return m_sJID.c_str();
}

void CCallChangeMessage::Type(CCallChangeMessage::ChangeType type)
{
	m_Type = type;
}

CCallChangeMessage::ChangeType CCallChangeMessage::Type()
{
	return m_Type;
}

void CCallChangeMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		switch(m_Type)
		{
		case CCallChangeMessage::UNKNOWN:
			wnd->AddToStatusDebug(_T("Warning: Unkown Call Change Message\r\n"));
			break;
		case CCallChangeMessage::INCOMING:
			wnd->AddToStatusDebug(_T("INFO: Incoming Call from "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T("\r\n"));
			break;
		case CCallChangeMessage::CALLING:
			wnd->AddToStatusDebug(_T("INFO: Calling "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T("\r\n"));
			break;
		case CCallChangeMessage::ANSWERED:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" answered our call\r\n"));
			break;
		case CCallChangeMessage::NOT_ANSWERED:
			wnd->AddToStatusDebug(_T("INFO: "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" didn't answer\r\n"));
			break;
		case CCallChangeMessage::IN_PROGRESS:
			wnd->AddToStatusDebug(_T("INFO: Calling "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" in progress\r\n"));
			break;
		case CCallChangeMessage::TERMINATE:
			wnd->AddToStatusDebug(_T("INFO: Call to "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T(" terminated\r\n"));
			break;
		case CCallChangeMessage::SENTTERMINATE:
			wnd->AddToStatusDebug(_T("INFO: Sent terminate to "));
			wnd->AddToStatusDebug(CString(m_sJID.c_str()).GetBuffer());
			wnd->AddToStatusDebug(_T("\r\n"));
			break;
		}
	}
}

// ==========================================================
// ================   CBeatLogMessage      ===============
// ==========================================================
CBeatLogMessage::CBeatLogMessage(void)
{
}

CBeatLogMessage::~CBeatLogMessage(void)
{
}

void CBeatLogMessage::Handle(void)
{
	CMessageBase::Handle();
}

void CBeatLogMessage::Message(const char* msg)
{
	m_Msg = msg;
}

const char* CBeatLogMessage::Message()
{
	return m_Msg.c_str();
}

void CBeatLogMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		m_Msg.append("\r\n");
		wnd->AddToBeatDebug(m_Msg.c_str());
	}
}

// ==========================================================
// ================   CXMLInLogMessage      ===============
// ==========================================================
CXMLInLogMessage::CXMLInLogMessage(void)
{
}

CXMLInLogMessage::~CXMLInLogMessage(void)
{
}

void CXMLInLogMessage::Handle(void)
{
	CMessageBase::Handle();
}

void CXMLInLogMessage::Message(const char* msg, int length)
{
	m_Msg = "In XML: ";
	m_Msg.append(msg, length);
	m_Msg.append("\r\n");
}

const char* CXMLInLogMessage::Message()
{
	return m_Msg.c_str();
}

void CXMLInLogMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		wnd->AddToXMLDebug(m_Msg.c_str());
	}
}


// ==========================================================
// ================   CXMLOutLogMessage      ===============
// ==========================================================
CXMLOutLogMessage::CXMLOutLogMessage(void)
{
}

CXMLOutLogMessage::~CXMLOutLogMessage(void)
{
}

void CXMLOutLogMessage::Handle(void)
{
	CMessageBase::Handle();
}

void CXMLOutLogMessage::Message(const char* msg, int length)
{
	m_Msg = msg;
}

const char* CXMLOutLogMessage::Message()
{
	return m_Msg.c_str();
}

void CXMLOutLogMessage::Debug()
{
	CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
	if(wnd != 0)
	{
		std::string xml("Out XML: ");
		xml.append(m_Msg);
		xml.append("\r\n");
		wnd->AddToXMLDebug(xml.c_str());
	}
}

// ==========================================================
// ================   CChatMessage      ===============
// ==========================================================
CChatMessage::CChatMessage(void)
{
}

CChatMessage::~CChatMessage(void)
{
}

void CChatMessage::Handle(void)
{
    CMessageBase::Handle();
}

void CChatMessage::Message(const std::string& fromjid, const std::string& msg)
{
    m_FromJid = fromjid;
    m_Msg = msg;
}

const char* CChatMessage::Message()
{
    return m_Msg.c_str();
}

const char* CChatMessage::FromJid()
{
    return m_FromJid.c_str();
}

void CChatMessage::Debug()
{
    CLibJingleDlg* wnd = (CLibJingleDlg*)theApp.m_pMainWnd;
    if(wnd != 0)
    {
        std::string xml;
        xml.append(m_FromJid);
        xml.append(": ");
        xml.append(m_Msg);
        xml.append("\r\n");
        wnd->AddToChatDebug(xml.c_str());
    }
}