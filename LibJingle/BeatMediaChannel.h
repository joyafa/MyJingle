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
#include "beat/SoundCapture.h"
#include "beat/SoundPlay.h"
#include "beat/SoundInterface.h"
#include "beat/JitterBuffer.h"



namespace cricket{

class BeatMediaChannel :
	public MediaChannel, public ::PacketReciever
{
public:
	BeatMediaChannel(void);
	virtual ~BeatMediaChannel(void);
	void cricket::BeatMediaChannel::SetCodec(const char *);
	void cricket::BeatMediaChannel::OnPacketReceived(const void *,int);
	void cricket::BeatMediaChannel::SetPlayout(bool);
	void cricket::BeatMediaChannel::SetSend(bool);
	float cricket::BeatMediaChannel::GetCurrentQuality(void) { return 0; };
	int cricket::BeatMediaChannel::GetOutputLevel(void){ return 0; };
	void AddPacket(Packet *,NetAddress *);
private:
	::SoundCaptureImpl* m_SoundCapture;
	::SoundPlayImpl* m_SoundPlay;
	::Codec m_Codec_PCM;
	::G711Codec m_Codec_G711;
	::SpeexCodec m_Codec_Speex;
	::JitterBuffer m_JitterBuffer;
};

}