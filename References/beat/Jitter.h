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

#if !defined(Jitter_h_INCLUDED)
#define Jitter_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"
#include "Packet.h"
#include "Thread.h"
#include "MyTime.h"

class JitterLog
{
	// Formula for Excel: =WENN(C1="";"";C1/1000-$F$1)
public:
	JitterLog(void);
	~JitterLog(void);
	void AddPacket(const Packet* packet);
	float Margin();
	void Init();
	void Exit();
protected:
	unsigned int m_FirstTimeStamp;
	unsigned int m_FirstSequence;
	bool m_IsFirstSet;
	float m_PostiveDeviation;
	float m_NegativeDeviation;
	const static int s_TimeFactor = 5;
};


class JitterStrategy
{
public:
	JitterStrategy(void);
	~JitterStrategy(void);
	long FrequenzFactor(float averagePacketsInBuffer, long origFreq);
private:
	float m_TargetValue;
	//const static int s_FrequenzRegulateStepPercent = 1;
	float m_Threshold;
	int controlPacketCount;
	const static int s_RegulateStepPercent = 5;

};


#endif
