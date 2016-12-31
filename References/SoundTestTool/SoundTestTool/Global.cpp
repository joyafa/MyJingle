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

#include "Global.h"

SString Global::GetErrorFromErrno(int errnum)
{
	SString message;

#if defined(WIN32)
	SCHAR *lpMsgBuf = 0;
	int result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		errnum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(SCHAR *) &lpMsgBuf,
		0,
		0);
	if (result) {
		message = lpMsgBuf;
	} else {
		message = ::strerror(errnum);
	}
	::LocalFree (lpMsgBuf);
#else
	message = (const char*) ::strerror(errnum);
#endif

	return message;
}

Global::Global()
{
}

Global& Global::GetInstance()
{
    return s_Instance;
}

Global Global::s_Instance;
