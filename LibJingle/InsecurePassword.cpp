#include "InsecurePassword.h"

InsecurePassword::InsecurePassword(void)
{
}

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

InsecurePassword::~InsecurePassword(void)
{
}

std::string& InsecurePassword::Password()
{
	return m_Password; 
}

const std::string& InsecurePassword::Password() const 
{ 
	return m_Password; 
}

buzz::XmppPasswordImpl* InsecurePassword::Copy() const 
{
	InsecurePassword * copy = new InsecurePassword;
	copy->Password() = m_Password;
	return copy;
}

std::string InsecurePassword::UrlEncode() const 
{ 
	return m_Password; 
}

size_t InsecurePassword::GetLength() const
{ 
	return m_Password.size(); 
}

void InsecurePassword::CopyTo(char* dest, bool nullterminate) const 
{
	memcpy(dest, m_Password.data(), m_Password.size());
	if (nullterminate)
	{
		dest[m_Password.size()] = 0;
	}
}
