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

#include "Win32CallClient.h"
#include <string>
#include <vector>

#include "talk/xmpp/constants.h"
#include "talk/base/thread.h"
#include "talk/base/network.h"
#include "talk/base/socketaddress.h"
#include "talk/p2p/base/sessionmanager.h"
#include "talk/p2p/base/helpers.h"
#include "talk/p2p/client/basicportallocator.h"
#include "talk/session/receiver.h"
#include "talk/session/sessionsendtask.h"
#include "talk/session/phone/phonesessionclient.h"
#include "presencepushtask.h"
#include "presenceouttask.h"
#include "Win32JingleAdapter.h"
#include "FromJingleMessage.h"

using namespace std;


Win32CallClient::Win32CallClient(Win32JingleAdapter& adapter, buzz::XmppClient* xmpp_client, HWND window):
m_Adapter(adapter)
,xmpp_client_(xmpp_client)
,m_Window(window)
,worker_thread_(0)
,call_(0)
,session_(0)
{
	xmpp_client_->SignalStateChange.connect(this, &Win32CallClient::OnStateChange);
	adapter.SignalMessage.connect(this, &Win32CallClient::OnMessage);
	xmpp_client_->SignalLogInput.connect(this, &Win32CallClient::OnInputLogDebug);
	xmpp_client_->SignalLogOutput.connect(this, &Win32CallClient::OnOutputLogDebug);
}

Win32CallClient::~Win32CallClient()
{
}

void Win32CallClient::OnStateChange(buzz::XmppEngine::State state)
{
  if(state == buzz::XmppEngine::STATE_CLOSED)
  {
	  CSessionStateChangeDisconnectMessage* cm = new CSessionStateChangeDisconnectMessage();
	  switch(xmpp_client_->GetError())
	  {
	  case buzz::XmppEngine::ERROR_NONE:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_NONE);
		  break;
	  case buzz::XmppEngine::ERROR_XML:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_XML);
		  break;
	  case buzz::XmppEngine::ERROR_STREAM:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_STREAM);
		  break;
	  case buzz::XmppEngine::ERROR_VERSION:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_VERSION);
		  break;
	  case buzz::XmppEngine::ERROR_UNAUTHORIZED:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_UNAUTHORIZED);
		  break;
	  case buzz::XmppEngine::ERROR_TLS:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_TLS);
		  break;
	  case buzz::XmppEngine::ERROR_AUTH:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_AUTH);
		  break;
	  case buzz::XmppEngine::ERROR_BIND:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_BIND);
		  break;
	  case buzz::XmppEngine::ERROR_CONNECTION_CLOSED:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_CONNECTION_CLOSED);
		  break;
	  case buzz::XmppEngine::ERROR_DOCUMENT_CLOSED:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_DOCUMENT_CLOSED);
		  break;
	  case buzz::XmppEngine::ERROR_SOCKET:
		  cm->Type(CSessionStateChangeDisconnectMessage::ERROR_SOCKET);
		  break;
	  }
	  cm->Post(m_Window);
	  if(worker_thread_ != 0)
	  {
	    worker_thread_->Stop();
	  }
	  cricket::Thread::Current()->Stop();
  }
  else
  {
	  CSessionStateChangeMessage* cm = new CSessionStateChangeMessage();
	  switch (state)
	  {
	  case buzz::XmppEngine::STATE_START:
		cm->Type(CSessionStateChangeMessage::CONNECTING);
		break;

	  case buzz::XmppEngine::STATE_OPENING:
		cm->Type(CSessionStateChangeMessage::LOGGINGIN);
		break;

	  case buzz::XmppEngine::STATE_OPEN:
		InitPhone();
		InitPresence();
        InitMessageTask();
		cm->Type(CSessionStateChangeMessage::LOGGEDIN);
		break;
	  }
      cm->Post(m_Window);
  }
}

void Win32CallClient::InitPhone()
{
  std::string client_unique = xmpp_client_->jid().Str();
  cricket::InitRandom(client_unique.c_str(), client_unique.size());

  worker_thread_ = new cricket::Thread();

  network_manager_ = new cricket::NetworkManager();

  //cricket::SocketAddress *stun_addr = new cricket::SocketAddress("64.233.167.126", 19302);
  cricket::SocketAddress *stun_addr = new cricket::SocketAddress("stun.fwdnet.net", 3478);
  port_allocator_ = new cricket::BasicPortAllocator(network_manager_, stun_addr, NULL);

  session_manager_ = new cricket::SessionManager(
      port_allocator_, worker_thread_);
  session_manager_->SignalRequestSignaling.connect(
      this, &Win32CallClient::OnRequestSignaling);
  session_manager_->OnSignalingReady();

  phone_client_ = new cricket::PhoneSessionClient(
      xmpp_client_->jid(),session_manager_);
  phone_client_->SignalCallCreate.connect(this, &Win32CallClient::OnCallCreate);
  phone_client_->SignalSendStanza.connect(this, &Win32CallClient::OnSendStanza);

  receiver_ = new cricket::Receiver(xmpp_client_, phone_client_);
  receiver_->Start();

  worker_thread_->Start();
}

void Win32CallClient::InitPresence()
{
  presence_push_ = new buzz::PresencePushTask(xmpp_client_);
  presence_push_->SignalStatusUpdate.connect(
    this, &Win32CallClient::OnStatusUpdate);
  presence_push_->Start();

  buzz::Status my_status;
  my_status.set_jid(xmpp_client_->jid());
  my_status.set_available(true);
  my_status.set_invisible(false);
  my_status.set_show(buzz::Status::SHOW_ONLINE);
  my_status.set_priority(0);
  my_status.set_know_capabilities(true);
  my_status.set_phone_capability(true);
  my_status.set_is_google_client(true);
  my_status.set_version("1.0.0.66");

  buzz::PresenceOutTask* presence_out_ =
      new buzz::PresenceOutTask(xmpp_client_);
  presence_out_->Send(my_status);
  presence_out_->Start();
}

void Win32CallClient::InitMessageTask()
{
    chat_receiver_ = new buzz::MessageTask(xmpp_client_);
    chat_receiver_->SignalChatMessage.connect(
        this, &Win32CallClient::OnChatMessage);
    chat_receiver_->Start();
}

void Win32CallClient::OnInputLogDebug(const char* bytes, int bytes_read)
{
	CXMLInLogMessage* cm = new CXMLInLogMessage();
	cm->Message(bytes, bytes_read);
	cm->Post(m_Window);
}

void Win32CallClient::OnOutputLogDebug(const char* bytes, int bytes_read)
{
	CXMLOutLogMessage* cm = new CXMLOutLogMessage();
	cm->Message(bytes, bytes_read);
	cm->Post(m_Window);
}

void Win32CallClient::OnStatusUpdate(const buzz::Status& status)
{
  RosterItem item;
  item.jid = status.jid();
  item.show = status.show();
  item.status = status.status();

  std::string key = item.jid.Str();

  CRosterChangeMessage* cm = new CRosterChangeMessage();
  cm->Jid(key.c_str());

  // if (status.available() && status.phone_capability()) 
  if (status.available()) //changed by wanglg 
  {
	  cm->Type(CRosterChangeMessage::ADDED);
  }
  else
  {
	  cm->Type(CRosterChangeMessage::REMOVED);
  }
  cm->Post(m_Window);
}

void Win32CallClient::OnSendStanza(cricket::SessionClient *client, const buzz::XmlElement* stanza, bool isSend)
{
  cricket::SessionSendTask* sender =
      new cricket::SessionSendTask(xmpp_client_, phone_client_);
  sender->Send(stanza);
  sender->Start();
}

void Win32CallClient::OnCallCreate(cricket::Call* call)
{
  call->SignalSessionState.connect(this, &Win32CallClient::OnSessionState);
}

void Win32CallClient::OnSessionState(cricket::Call* call,
                                cricket::Session* session,
                                cricket::Session::State state)
{
  CCallChangeMessage* cm = new CCallChangeMessage();
  cm->Jid(session->remote_address().c_str());
  switch(state)
  {
  case cricket::Session::STATE_RECEIVEDINITIATE:
	  if(call_ == 0 && session_ == 0)
	  {
		  call_ = call;
		  session_ = session;
		  cm->Type(CCallChangeMessage::INCOMING);
	  }
	  else
	  {
		  // To prevent the problems with deregistering multiple calls we just accept on call at a time
		  // see original callclient
		  call->RejectSession(session);
	  }
	break;
  case cricket::Session::STATE_SENTINITIATE:
	  cm->Type(CCallChangeMessage::CALLING);
	break;
  case cricket::Session::STATE_RECEIVEDACCEPT:
	  cm->Type(CCallChangeMessage::ANSWERED);
	break;
  case cricket::Session::STATE_RECEIVEDREJECT:
	  if(call == call_ && session == session_)
	  {
		  call->Terminate();
		  call_ = 0;
		  session_ = 0;
	  }
	  cm->Type(CCallChangeMessage::NOT_ANSWERED);
	break;
  case cricket::Session::STATE_INPROGRESS:
	  phone_client()->SetFocus(call_);
	  cm->Type(CCallChangeMessage::IN_PROGRESS);
	break;
  case cricket::Session::STATE_RECEIVEDTERMINATE:
	  cm->Type(CCallChangeMessage::TERMINATE);
	  if(call == call_ && session == session_)
	  {
		  call->Terminate();
		  call_ = 0;
		  session_ = 0;
	  }
	break;
  case cricket::Session::STATE_SENTTERMINATE:
	  if(call == call_ && session == session_)
	  {
		  call->Terminate();
		  call_ = 0;
		  session_ = 0;
	  }
	  cm->Type(CCallChangeMessage::SENTTERMINATE);
	break;
  }
  cm->Post(m_Window);
}

void Win32CallClient::OnRequestSignaling()
{
  session_manager_->OnSignalingReady();
}

void Win32CallClient::OnMessage(ToJingleMessage& msg)
{
	buzz::Jid jid(msg.ToJid());
	msg.LockMessage();
	std::string msgString(msg.Message());
	ToJingleMessage::TYPE msgType = msg.Type();
	msg.UnLockMessage();
	switch(msgType)
	{
	case ToJingleMessage::UNKNOWN:
		break;
	case ToJingleMessage::CALL:
	  if(call_ == 0 && session_ == 0)
	  {
	    call_ = phone_client()->CreateCall();
        session_ = call_->InitiateSession(jid);
	  }
	  break;
	case ToJingleMessage::HANGUP:
	  if(call_ != 0 && session_ != 0)
	  {
	    call_->Terminate();
	    call_ = 0;
        session_ = 0;
	  }
	  break;
	case ToJingleMessage::EXIT:
		if(call_ != 0 && session_ != 0)
		{
			call_->Terminate();
			call_ = 0;
			session_ = 0;
		}
		xmpp_client_->Disconnect();
		break;
	case ToJingleMessage::ACCEPT_CALL:
		if(call_ != 0 && session_ != 0)
		{
			call_->AcceptSession(session_);
		}
		else
		{
			call_ = 0;
			session_ = 0;
		}
		break;
	case ToJingleMessage::REJECT_CALL:
		if(call_ != 0 && session_ != 0)
		{
			call_->RejectSession(session_);
		}
		call_ = 0;
		session_ = 0;
		break;
    case ToJingleMessage::STR_CHAT:
        buzz::XmlElement *message = new buzz::XmlElement(buzz::QN_MESSAGE);
        message->AddAttr(buzz::QN_FROM,xmpp_client_->jid().Str());
        message->AddAttr(buzz::QN_TO,jid.Str());
        message->AddAttr(buzz::QN_TYPE,"chat");
        buzz::XmlElement* bodymsg = new buzz::XmlElement(buzz::QN_BODY);

        //bodymsg->AddText("王林光"); //直接中文乱码
        bodymsg->AddText(msgString);
        message->AddElement(bodymsg);
        xmpp_client_->SendStanza(message); 
        break;
	}
}

void Win32CallClient::OnChatMessage(const buzz::TextMessage& msg)
{
    CChatMessage* cm = new CChatMessage();
    cm->Message(msg.from_jid, msg.body);
    cm->Post(m_Window);
}
