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

#if !defined(AFX_CRYPTOSERVICE_H__620F569C_29F4_47C0_9DE1_7FD7223E6718__INCLUDED_)
#define AFX_CRYPTOSERVICE_H__620F569C_29F4_47C0_9DE1_7FD7223E6718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CryptoService  
{
public:
	CryptoService();
	virtual ~CryptoService();

	bool Encrypt (BYTE *in, DWORD inLen, TCHAR* szDataDescr);
	bool Decrypt (BYTE *in, DWORD inLen);

	BYTE *GetBuffer() { return m_Buffer; }
	DWORD GetLength() { return m_BufferLen; }

protected:
	void Free();

	BYTE *m_Buffer;
	DWORD m_BufferLen;
};

#endif // !defined(AFX_CRYPTOSERVICE_H__620F569C_29F4_47C0_9DE1_7FD7223E6718__INCLUDED_)
