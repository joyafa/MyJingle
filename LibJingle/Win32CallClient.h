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

#include "Win32JingleAdapter.h"
#include <map>
#include <string>
#include "talk/p2p/base/session.h"
#include "talk/p2p/client/sessionclient.h"
#include "talk/xmpp/xmppclient.h"
#include "talk/examples/call/status.h"
#include "MessageTask.h"
namespace buzz {
class PresencePushTask;
class Status;
class MessageTask;
}

namespace cricket {
class Thread;
class NetworkManager;
class PortAllocator;
class PhoneSessionClient;
class Receiver;
class Call;
}

struct RosterItem {
  buzz::Jid jid;
  buzz::Status::Show show;
  std::string status;
};

class Win32CallClient: public sigslot::has_slots<>
{
public:
  Win32CallClient(Win32JingleAdapter& adapter, buzz::XmppClient* xmpp_client, HWND window);
  ~Win32CallClient();

  cricket::PhoneSessionClient* phone_client() const { return phone_client_; }

  void MakeCallTo(const std::string& jid);

private:
  Win32JingleAdapter& m_Adapter;
  HWND m_Window;

  buzz::XmppClient* xmpp_client_;
  cricket::Thread* worker_thread_;
  cricket::NetworkManager* network_manager_;
  cricket::PortAllocator* port_allocator_;
  cricket::SessionManager* session_manager_;
  cricket::PhoneSessionClient* phone_client_;
  cricket::Receiver* receiver_;
  buzz::PresencePushTask* presence_push_;
  buzz::MessageTask* chat_receiver_;
  cricket::Call* call_;
  cricket::Session* session_;
  void OnStateChange(buzz::XmppEngine::State state);

  void InitPhone();
  void OnRequestSignaling();
  void OnCallCreate(cricket::Call* call);
  void OnSessionState(cricket::Call* call,
                      cricket::Session* session,
                      cricket::Session::State state);
  void OnSendStanza(cricket::SessionClient *client, const buzz::XmlElement* stanza, bool isSend);

  void InitPresence();
  void OnInputLogDebug(const char * bytes, int bytes_read);
  void OnOutputLogDebug(const char * bytes, int bytes_read);
  void OnStatusUpdate(const buzz::Status& status);
  void OnMessage(ToJingleMessage& msg);
  void InitMessageTask();
  void OnChatMessage(const buzz::TextMessage& msg);
};