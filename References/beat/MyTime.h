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

#if !defined(MyTime_h_INCLUDED)
#define MyTime_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"
#include "Thread.h"

#if defined (WIN32)
#include "Time.h"
#else
#include "sys/time.h"
#endif

typedef void (*CallbackFunc)(void* instance);
typedef void (*CallbackFuncArg)(void* instance, void* arg);

class TimeValue
{
public:
	TimeValue(void);
	~TimeValue(void);

	unsigned long long Microseconds();
	TimeValue& Microseconds(unsigned long long microseconds);
	TimeValue& Now();
protected:
	unsigned long long m_Microseconds;
	const static unsigned long long s_MaxMicroseconds;
};

class Callback
{
public:
	Callback(CallbackFunc callback){Callback::Callback(callback,0);}
	Callback(CallbackFuncArg callback){Callback::Callback(callback,0);}
	Callback(CallbackFunc callback, void* instance) : m_Callback(callback), m_CallbackArg(0), m_Instance(instance) {}
	Callback(CallbackFuncArg callback, void* instance) : m_Callback(0), m_CallbackArg(callback), m_Instance(instance) {}
	~Callback(){}
	void Instance(void* instance) { m_Instance = instance; }
	void* Instance() { return m_Instance; }
	void Invoke(void* Arg);
private:
	CallbackFunc m_Callback;
	CallbackFuncArg m_CallbackArg;
	void* m_Instance;
};


class Timer : protected Thread
{
public:
	static Timer& GetInstance(){ return s_Instance; }
	virtual ~Timer();
	enum Mode{Once = 0, Interval = 0};
	static void DoSleep(TimeValue& interval);
	virtual unsigned int Add(Callback& callback, Timer::Mode mode, unsigned int delay) = 0;
protected:
	Timer();
	Mutex m_Mutex;
	bool m_Initialized;
private:
	static Timer& s_Instance;
};

#if defined (WIN32)

typedef struct _TimerEvent
{
	struct _TimerEvent* Next;
	int TimerId;
	HANDLE Event;
	Callback& Callback;
	Timer::Mode Mode;
} TimerEvent;

class TimerImpl : public Timer
{
public:
	~TimerImpl();
	unsigned int Add(Callback& callback, Timer::Mode mode, unsigned int delay);
	TimerImpl();
private:
	void Svc();
	TimerEvent* m_FirstEvent;
	unsigned int m_EventCount;
	unsigned int m_EventArrayLength;
	HANDLE* m_EventArray;
	TIMECAPS m_TimeCaps;
};

#else //defined (WIN32)

class TimerImpl : public Timer
{
public:
	~TimerImpl();
	unsigned int Add(Callback& callback, Timer::Mode mode, unsigned int delay);
	TimerImpl();
private:
};
//Stichwort timer_create

#endif

#endif //defined (WIN32)
