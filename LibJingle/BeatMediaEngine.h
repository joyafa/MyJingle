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
#include "talk/session/phone/mediaengine.h"
#include "BeatMediaChannel.h"
#include "beat/Codec.h"
#include "beat/SoundInterface.h"


namespace cricket {

class BeatMediaEngine :
	public MediaEngine
{
public:
	BeatMediaEngine();
	virtual ~BeatMediaEngine(void);
	bool Init();
	void Terminate(void) {};
	cricket::MediaChannel *cricket::MediaEngine::CreateChannel(void);
	int cricket::MediaEngine::SetAudioOptions(int) { return 0; };
	int cricket::MediaEngine::SetSoundDevices(int,int) { return 0; };
	int cricket::MediaEngine::GetInputLevel(void) { return 0; };
protected:
};
}