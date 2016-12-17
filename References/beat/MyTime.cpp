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

#include "MyTime.h"
#include <mmsystem.h>
#define TIME_KILL_SYNCHRONOUS   0x0100  /* This flag prevents the event from occurring */
/* after the user calls timeKillEvent() to */
/* destroy it. */


TimeValue::TimeValue(void):
m_Microseconds(0)
{
}

TimeValue::~TimeValue(void)
{
}

TimeValue& TimeValue::Now(void)
{
#if defined (WIN32)
	m_Microseconds = timeGetTime();
	m_Microseconds *= 1000;
#else
	timeval val;
	int ret = gettimeofday(&val, 0);
	if(ret != 0)
	{
		throw Exception(_S("Error while getting Time"));
	}
	m_Microseconds = (unsigned long long)val.tv_sec * (unsigned long long)1000000;
	m_Microseconds += val.tv_usec;
#endif
	return *this;
}

unsigned long long TimeValue::Microseconds()
{
	return m_Microseconds;
}

TimeValue& TimeValue::Microseconds(unsigned long long microseconds)
{
	m_Microseconds = microseconds;
	return *this;
}

void Callback::Invoke(void* arg)
{
	if(m_Callback != 0)
	{
		m_Callback(m_Instance);
	}
	if(m_CallbackArg != 0)
	{
		m_CallbackArg(m_Instance, arg);
	}
}

Timer::Timer():
Thread()
,m_Initialized(false)
{
	m_Mutex.Init();
	m_Mutex.Aquire();
	m_Initialized = true;
	m_Mutex.Release();
}

Timer::~Timer()
{
	m_Mutex.Exit();
}

void Timer::DoSleep(TimeValue& interval)
{
	unsigned long long microsec = interval.Microseconds();
#if defined (WIN32)
	Sleep((long)(microsec/1000));
#else
	usleep(microsec);
#endif
}

#if defined (WIN32)

TimerImpl::TimerImpl():
Timer()
,m_FirstEvent(0)
,m_EventCount(1)
,m_EventArrayLength(100)
,m_EventArray(0)
{
	m_Mutex.Aquire();
	m_EventArray = new HANDLE[m_EventArrayLength];
	m_EventArray[0] = CreateEvent(0,0,false,0);
	m_EventCount = 1;
	Run();

	MMRESULT res;
	if(m_Initialized)
	{
		res = timeGetDevCaps(&m_TimeCaps, sizeof(TIMECAPS));
		if(res != TIMERR_NOERROR)
		{
			beatLog_Fatal(("Timer", "Timer", "Error while getting Time Caps"));
			m_Initialized = false;
		}
	}
	if(m_Initialized)
	{
		res = timeBeginPeriod(m_TimeCaps.wPeriodMin);
		if(res != TIMERR_NOERROR)
		{
			beatLog_Fatal(("Timer", "Timer", "Error while starting timer with Min Timer Resolution"));
		}
	}
	m_Mutex.Release();
}

TimerImpl::~TimerImpl()
{
	m_Mutex.Aquire();
	m_Stop = true;
	SetEvent(m_EventArray[0]);
	Stop();
	if(m_Initialized)
	{
		MMRESULT res = timeEndPeriod(m_TimeCaps.wPeriodMin);
		if(res != TIMERR_NOERROR)
		{
			beatLog_Error(("Timer", "~Timer", "Error while stoping timer with Min Timer Resolution"));
		}
	}

	TimerEvent* timerEvent = m_FirstEvent;
	while(timerEvent != 0)
	{
		MMRESULT res = 	timeKillEvent(timerEvent->TimerId);
		if(res != TIMERR_NOERROR)
		{
			beatLog_Error(("Timer", "~Timer", "Error while stopping all timers"));
		}
		TimerEvent* lastEvent = timerEvent;
		timerEvent = timerEvent->Next;
		free(lastEvent);
	}

	for(unsigned int i = 0; i < m_EventCount; i++)
	{
		BOOL res = CloseHandle(m_EventArray[i]);
		if(res == 0)
		{
			SString message;
			message.strcatf(_S("Error while closing the Handles: %s"), Global::GetErrorFromErrno(GetLastError()));
			beatLog_Fatal(("Timer", "Timer", message.c_str()));
		}
	}
	delete[] m_EventArray;

	m_Mutex.Release();
}

unsigned int TimerImpl::Add(Callback& callback, Timer::Mode mode, unsigned int delay)
{
	m_Mutex.Aquire();

	SetEvent(m_EventArray[0]);

	unsigned int timerID;
	TimerEvent* newEvent = (TimerEvent*)malloc(sizeof(TimerEvent));
	ZeroMemory(newEvent, sizeof(TimerEvent));

	newEvent->Event = CreateEvent(0,0,false,0);
	newEvent->Callback = callback;
	newEvent->Mode = mode;
	newEvent->Next = m_FirstEvent;
	m_FirstEvent->Next = newEvent;

	unsigned int eventType = TIME_CALLBACK_EVENT_SET | TIME_KILL_SYNCHRONOUS;

	if(mode == Timer::Once)
	{
		eventType = eventType | TIME_ONESHOT;
	}
	else
	{
		eventType = eventType | TIME_PERIODIC;
	}

	newEvent->TimerId = 	timeSetEvent(
									delay,
									0,
									(LPTIMECALLBACK)newEvent->Event,
									0,
									eventType
									);
	if(newEvent->TimerId == 0)
	{
		throw Exception(_S("Failure while Setting Timer"));
	}

	timerID = newEvent->TimerId;

	m_Mutex.Release();
	return timerID;
}

void TimerImpl::Svc()
{
	m_Mutex.Aquire();

	m_EventCount = 1;
	TimerEvent* timerEvent = m_FirstEvent;
	while(timerEvent != 0)
	{
		if(++m_EventCount == m_EventArrayLength)
		{
			HANDLE* newArray = new HANDLE[m_EventArrayLength*2];
			memcpy(newArray, m_EventArray, m_EventArrayLength);
			delete[] m_EventArray;
			m_EventArrayLength = m_EventArrayLength*2;
		}
		m_EventArray[m_EventCount] = timerEvent->Event;

		timerEvent = timerEvent->Next;
	}

	m_Mutex.Release();
	DWORD res = WaitForMultipleObjects(m_EventCount, m_EventArray, false,INFINITE);
	if(res == WAIT_FAILED)
	{
		beatLog_Error(("TimerImpl", "Svc", "Wait for multiple Objects failed: %s", Global::GetErrorFromErrno(GetLastError())));
	}
	int objIndex = res - WAIT_OBJECT_0;
	HANDLE event = m_EventArray[objIndex];
	m_Mutex.Aquire();

	TimerEvent* lastEvent = 0;
	timerEvent = m_FirstEvent;
	while(timerEvent != 0)
	{
		if(timerEvent->Event == event)
		{
			timerEvent->Callback.Invoke(0);
		}
		if(timerEvent->Mode == Timer::Once)
		{
			MMRESULT res = 	timeKillEvent(timerEvent->TimerId);
			if(res != TIMERR_NOERROR)
			{
				beatLog_Error(("Timer", "~Timer", "Error while stopping timer"));
			}
			if(lastEvent != 0)
			{
				lastEvent->Next = timerEvent->Next;
			}
			else
			{
				m_FirstEvent = 0;
			}
			free(timerEvent);
		}
		lastEvent = timerEvent;
	}

	m_Mutex.Release();
}
#else

TimerImpl::TimerImpl():
Timer()
{
}

TimerImpl::~TimerImpl()
{
}

unsigned int TimerImpl::Add(Callback& callback, Timer::Mode mode, TimeValue delay)
{
	m_Mutex.Aquire();
	//Timer::Add(callback, mode, delay);
	m_Mutex.Release();
	return 0;
}

#endif

#ifdef BEAT_USE_TIMER
Timer& Timer::s_Instance = TimerImpl();
#endif
