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

#if !defined(Exception_h_INCLUDED)
#define Exception_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SString.h"

class Exception
{
public:
	Exception() {}
	Exception(const Exception& ex) { m_Message = SString(ex.m_Message); }
	Exception(const SCHAR* s);
	virtual ~Exception(void);

	virtual SString ToString() const { return (SString&) m_Message; }
protected:
	SString m_Message;
};

class ErrnoException : public Exception
{
public:
	typedef Exception super;
	ErrnoException(const SCHAR* s, int errnum):super(s), m_Errno(errnum) {}

	SString ToString() const;

protected:
	int m_Errno;
};

class InstanciateException : public Exception
{
	typedef Exception super;

public:
	InstanciateException();
	InstanciateException(const Exception& ex):super(ex){}
	InstanciateException(const SCHAR* s):super(s){}
};

#endif // !defined(Exception_h_INCLUDED)
