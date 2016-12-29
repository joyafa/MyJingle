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

#include "Jitter.h"

JitterLog::JitterLog(void):
m_FirstTimeStamp(0)
,m_FirstSequence(0)
,m_IsFirstSet(false)
,m_PostiveDeviation(0)
,m_NegativeDeviation(0)
{
}

JitterLog::~JitterLog(void)
{
}

void JitterLog::Init()
{
}

void JitterLog::Exit()
{
}

void JitterLog::AddPacket(const Packet* packet)
{
	if(!m_IsFirstSet)
	{
		m_IsFirstSet = true;
		m_FirstSequence = packet->SequenceNumber();
		m_FirstTimeStamp = packet->RecieveTime();
	}
	int deviation = packet->RecieveTime() - m_FirstTimeStamp - ( 20000 * ( packet->SequenceNumber() - m_FirstSequence));
	if(deviation > 0)
	{
		m_PostiveDeviation = (m_PostiveDeviation * (s_TimeFactor -1) + deviation)/s_TimeFactor;
		m_NegativeDeviation = ( m_NegativeDeviation * (s_TimeFactor -1))/s_TimeFactor;
	}
	else
	{
		m_NegativeDeviation = (m_NegativeDeviation * (s_TimeFactor -1) - deviation)/s_TimeFactor;
		m_PostiveDeviation = ( m_PostiveDeviation * (s_TimeFactor -1))/s_TimeFactor;
	}
}

float JitterLog::Margin()
{
	return (m_PostiveDeviation + m_NegativeDeviation)/2;
}

JitterStrategy::JitterStrategy():
m_TargetValue(2.5f)
,m_Threshold(0.2f)
{
}

JitterStrategy::~JitterStrategy()
{
}

long JitterStrategy::FrequenzFactor(float averagePacketsInBuffer, long origFreq)
{
	long hertz = origFreq;

	float diff = averagePacketsInBuffer - m_TargetValue;
	float absDiff = ( diff < 0 ? -diff : diff );

	if(absDiff > m_Threshold)
	{
		float deviationPerc = absDiff / m_TargetValue;
		if(deviationPerc > 1) deviationPerc = 1;
		if(diff > 0)
		{
			// too much packets in buffer
			hertz = (long)(( 100.0 + (s_RegulateStepPercent*deviationPerc) ) / 100 * origFreq);
		}
		else
		{
			//not enough packets in buffer
			hertz = (long)(( 100.0 - (s_RegulateStepPercent*deviationPerc)) / 100 * origFreq);
		}
	}
	//beatLog_Debug(("JitterStrategy", "FrequenzFactor", "The Factor is: %d" , hertz));

	return hertz;
}

