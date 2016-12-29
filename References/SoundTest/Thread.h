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

#if !defined(Thread_h_INCLUDED)
#define Thread_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Global.h"

class Process
{
public:
	enum Prio{ LOWEST, LOW, NORMAL, HIGH, HIGHEST, REALTIME };
	static void SetPriority(Prio prio);
};

class  Thread
{
public:
	Thread(void);
	virtual ~Thread(void);
	void Run();
	void virtual Stop(void);
	enum Prio{ LOWEST, LOW, NORMAL, HIGH, HIGHEST, REALTIME };
	static void SetPriority(Prio prio);
protected:
	void virtual Svc();
	void virtual SvcLoop();
	bool m_Stop;
#if defined (WIN32)
	HANDLE m_ThreadHandle;
	static DWORD WINAPI ThreadJumpIn(LPVOID threadParam);
#else
	pthread_t m_ThreadHandle;
	static void * ThreadJumpIn(void *);
#endif
};

class Mutex
{
public:
	Mutex();
	~Mutex();
	void Init();
	void Exit();
	void Aquire();
	void Release();
protected:
#if defined (WIN32)
	CRITICAL_SECTION m_Mutex;
#else
	pthread_mutex_t m_Mutex;
#endif
private:
	bool m_Initialized;
};


#endif // !defined(Thread_h_INCLUDED)
