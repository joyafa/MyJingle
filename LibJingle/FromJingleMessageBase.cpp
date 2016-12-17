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
#include "FromJingleMessageBase.h"

CMessageBase::CMessageBase(void)
{
}

CMessageBase::~CMessageBase(void)
{
}

void CMessageBase::Post(HWND hWnd)
{
	::PostMessage(hWnd, WM_APP, (WPARAM) this, 0);
}

void CMessageBase::Handle(void)
{
	Debug();
	delete this;
}

//TCHAR* CMessageBase::GetString(const char* string, size_t length)
//{
//#ifdef _UNICODE
//	TCHAR* buffer = new TCHAR[length];
//	MultiByteToWideChar(CP_ACP, 0, string, -1, buffer, length);
//	return buffer;
//#else
//	return (TCHAR*)string;
//#endif
//}
