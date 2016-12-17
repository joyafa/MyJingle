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

#if !defined(Global_h_INCLUDED)
#define Global_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(WIN32)
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0500
  #endif
  #define INITGUID
  #include <winsock2.h> //Bug ??? must be included before windows.h see: http://www.codeguru.com/forum/archive/index.php/t-286587.html
  #include "windows.h"
#else
  #include <stdio.h>
  #include <sys/types.h>
  #include <pthread.h>
#endif

#include "Exception.h"
#include "beatLog_.h"

#ifdef DIRECTSOUND_VERSION
#undef DIRECTSOUND_VERSION
#endif

#define DIRECTSOUND_VERSION 0x0800  /* Version 8.0 */
class Global
{
public:
	static SString GetErrorFromErrno(int errnum);
	static Global& GetInstance();
private:
	Global();
	static Global s_Instance;
};


#endif // !defined(Global_h_INCLUDED)
