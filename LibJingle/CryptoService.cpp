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

#include "stdafx.h"
#include "CryptoService.h"
#include <wincrypt.h>

CryptoService::CryptoService()
:m_Buffer(NULL)
,m_BufferLen(0)
{
}

CryptoService::~CryptoService()
{
	Free();
}

void CryptoService::Free()
{
	if (m_Buffer != NULL)
	{
		::LocalFree(m_Buffer);
		m_Buffer = NULL;
		m_BufferLen = 0;
	}
}


bool CryptoService::Encrypt (BYTE *in, DWORD inLen, TCHAR* szDataDescr)
{
	DATA_BLOB d_in, d_out;
	d_in.pbData = in;    
	d_in.cbData = inLen;

	bool ok = (CryptProtectData(	&d_in,
									szDataDescr,  // A description string. 
									NULL,         // Optional entropy not used.
									NULL,         // Reserved.
									NULL,         // PromptStruct.
									0,
									&d_out) > 0);
	if (ok) 
	{
		Free();
		m_Buffer = d_out.pbData;
		m_BufferLen = d_out.cbData;
	}
	return ok;
}

bool CryptoService::Decrypt (BYTE *in, DWORD inLen)
{
	DATA_BLOB d_in, d_out;
	d_in.pbData = in;    
	d_in.cbData = inLen;

	bool ok = (CryptUnprotectData(  &d_in,
									NULL,
									NULL,                 // Optional entropy
									NULL,                 // Reserved
									NULL,        // Optional PromptStruct
									0,
									&d_out) > 0);
	if (ok) 
	{
		Free();
		m_Buffer = d_out.pbData;
		m_BufferLen = d_out.cbData;
	}
	return ok;
}
