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

#include <string>
#include "talk/xmpp/xmpppassword.h"

class InsecurePassword : public buzz::XmppPasswordImpl 
{
public:
	InsecurePassword();
	virtual ~InsecurePassword();

	std::string& Password();
	const std::string& Password() const;

	virtual size_t GetLength() const;
	virtual void CopyTo(char * dest, bool nullterminate) const;
	virtual std::string UrlEncode() const;
	virtual buzz::XmppPasswordImpl* InsecurePassword::Copy() const;
private:
	std::string m_Password;
};
