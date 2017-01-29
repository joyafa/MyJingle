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

#include "JingleMain.h"
#include "talk/xmpp/xmppclientsettings.h"
#include "talk/examples/login/xmppthread.h"
#include "talk/examples/login/xmppauth.h"
#include "Win32CallClient.h"
#include "FromJingleMessage.h"
#include "InsecurePassword.h"
#include <string>

void BeatLogCallback(int _nLevel, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
{
	std::string msg(_pszChannel);
	msg.append("::");
	msg.append(_pszContext);
	msg.append(" ");
	msg.append(_pszMessage);
	CBeatLogMessage* cm = new CBeatLogMessage();
	cm->Message(msg.c_str());
	cm->Post(JingleMain::m_hWnd);
}


JingleMain::JingleMain()
:m_Running(false)
,m_JingleAdapter(m_SS)
{
//	beatLog_Interface::SetLogCallback(::BeatLogCallback);
}

JingleMain::~JingleMain(void)
{
}

void JingleMain::SvcLoop()
{
  cricket::Thread main_thread(&m_SS);
  cricket::ThreadManager::SetCurrent(&main_thread);

  XmppPump pump;
  Win32CallClient client(m_JingleAdapter, pump.client(), m_hWnd);
  InsecurePassword pass;
  std::string& sPass = pass.Password();
  sPass = m_Passwd.c_str();

  bool tls = true;

  buzz::XmppClientSettings xcs;
  xcs.set_user(m_Username.c_str());
  xcs.set_host(m_Domain.c_str());
  xcs.set_use_tls(tls);
  xcs.set_pass(buzz::XmppPassword(pass));
  SString sConnectHost = m_Domain;
  if (sConnectHost == _S("gmail.com") || sConnectHost == _S("googlemail.com"))
  {
	  sConnectHost = _S("talk.google.com");
  }
  xcs.set_server(cricket::SocketAddress(sConnectHost.c_str(), 5222));
  pump.DoLogin(xcs, new XmppSocket(tls), 0);

  main_thread.Loop();
}

void JingleMain::Run(HWND window, SCHAR* username, SCHAR* passwd, SCHAR* domain)
{
	m_Running = true;
	m_hWnd = window;
	m_Username = username;
	m_Passwd = passwd;
	m_Domain = domain;
	Thread::Run();
}

void JingleMain::Stop()
{
	Thread::Stop();
	m_Running = false;
}

HWND JingleMain::m_hWnd(0);