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

#include "Thread.h"

void Process::SetPriority(Prio prio)
{
#if defined (WIN32)
	HANDLE p = GetCurrentProcess();
	BOOL res;
	switch (prio)
	{
		case LOWEST:
			res = SetPriorityClass(p, IDLE_PRIORITY_CLASS);
			break;
		case LOW:
			res = SetPriorityClass(p, BELOW_NORMAL_PRIORITY_CLASS);
			break;
		case NORMAL:
			res = SetPriorityClass(p, NORMAL_PRIORITY_CLASS);
			break;
		case HIGH:
			res = SetPriorityClass(p, ABOVE_NORMAL_PRIORITY_CLASS);
			break;
		case HIGHEST:
			res = SetPriorityClass(p, HIGH_PRIORITY_CLASS);
			break;
		case REALTIME:
			res = SetPriorityClass(p, REALTIME_PRIORITY_CLASS);
			break;
	}
	if(res == 0)
	{
		throw ErrnoException(_S("Error while setting process prio"), GetLastError());
	}

#endif

}

Thread::Thread(void)
:m_Stop(false)
,m_ThreadHandle(0)
{
}

Thread::~Thread(void)
{
}

void Thread::Run()
{
	if (m_ThreadHandle != 0)
	{
		throw Exception(_S("Thread already running can't create a second instance"));
	}
	m_Stop = false;
#if defined (WIN32)
	m_ThreadHandle = CreateThread(
		0,				//LPSECURITY_ATTRIBUTES lpThreadAttributes
		0,				//SIZE_T dwStackSize
		ThreadJumpIn,	//LPTHREAD_START_ROUTINE lpStartAddress
		this,			//LPVOID lpParameter
		0,				//DWORD dwCreationFlags
		0				//LPDWORD lpThreadId
		);
	if (m_ThreadHandle == 0)
	{
		throw ErrnoException(_S("Could not create Thread: "), GetLastError());
	}
#else
	int err = pthread_create(&m_ThreadHandle,  0, ThreadJumpIn, this);
	if (err!=0)
	{
		throw ErrnoException(_S("Could not create Thread: "), err);
	}
#endif
}

#if defined (WIN32)
DWORD WINAPI Thread::ThreadJumpIn(LPVOID threadParam)
#else
void * Thread::ThreadJumpIn(void * threadParam)
#endif
{
	try
	{
		((Thread *) threadParam)->SvcLoop();
	}
	catch (Exception& e)
	{
		beatLog_Error(("Thread", "ThreadJumpIn", "Unknown exception: %s", e.ToString().c_str()));
	}
	catch ( ... )
	{
		beatLog_Error(("Thread", "ThreadJumpIn", "Unknown exception"));
	}
	return 0;
}

void Thread::Stop()
{
	m_Stop = true;
#if defined (WIN32)
	WaitForSingleObject(m_ThreadHandle, INFINITE);
	CloseHandle(m_ThreadHandle);
#else
	pthread_join(m_ThreadHandle,0);
#endif
	m_ThreadHandle = 0;
}

void Thread::Svc()
{
#ifdef WIN32
	Sleep(1000);
#else
	sleep(1);
#endif
}

void Thread::SvcLoop()
{
	while (!m_Stop)
	{
		Svc();
	}
}

void Thread::SetPriority(Prio prio)
{
#if defined (WIN32)
	HANDLE t = GetCurrentThread();
	BOOL res;
	switch (prio)
	{
		case LOWEST:
			res = SetThreadPriority(t, THREAD_PRIORITY_IDLE);
			break;
		case LOW:
			res = SetThreadPriority(t, THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case NORMAL:
			res = SetThreadPriority(t, THREAD_PRIORITY_NORMAL);
			break;
		case HIGH:
			res = SetThreadPriority(t, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case HIGHEST:
			res = SetThreadPriority(t, THREAD_PRIORITY_HIGHEST);
			break;
		case REALTIME:
			res = SetThreadPriority(t, THREAD_PRIORITY_TIME_CRITICAL);
			break;
	}
	if(res == 0)
	{
		throw ErrnoException(_S("Error while setting thread prio"), GetLastError());
	}
#else
	//pthread_t t = pthread_self();
#endif

}


Mutex::Mutex():
m_Initialized(false)
{
}

Mutex::~Mutex()
{
}

void Mutex::Init()
{
#if defined (WIN32)
	InitializeCriticalSection(&m_Mutex);
#else
	pthread_mutex_init(&m_Mutex,0);
#endif
	m_Initialized = true;
}

void Mutex::Exit()
{
	Aquire();
	m_Initialized = false;
	Release();
#if defined (WIN32)
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
#endif
}

void Mutex::Aquire()
{
	if(!m_Initialized)
	{
		throw(Exception(_S("Mutex has been aquired before Initialized")));
	}
#if defined (WIN32)
	EnterCriticalSection(&m_Mutex);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
}

void Mutex::Release()
{
#if defined (WIN32)
	LeaveCriticalSection(&m_Mutex);
#else
	pthread_mutex_unlock(&m_Mutex);
#endif
}
