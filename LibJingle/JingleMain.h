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
#include "beat/Thread.h"
#include "talk/base/Physicalsocketserver.h"
#include "Win32JingleAdapter.h"

class JingleMain :
	protected ::Thread
{
public:
	JingleMain();
	~JingleMain(void);
	void Run(HWND window, SCHAR* username, SCHAR* passwd, SCHAR* domain);
	void Stop();
	Win32JingleAdapter& JingleAdapter(){ return m_JingleAdapter; }
	static HWND m_hWnd;
	bool Running(){ return m_Running; }
private:
	void SvcLoop();
	SString m_Username;
	SString m_Passwd;
	SString m_Domain;
	cricket::PhysicalSocketServer m_SS;
	Win32JingleAdapter m_JingleAdapter;
	bool m_Running;
};