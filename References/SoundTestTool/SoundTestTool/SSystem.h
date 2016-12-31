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

#if !defined(__SSystem__)
#define __SSystem__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if defined(WIN32)
  #include <direct.h>
  #include <crtdbg.h>
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <errno.h>
#endif

#if !defined(_UNICODE)
  typedef char SCHAR;
  #define _S(x) x
#else
  typedef wchar_t SCHAR;
  #define _S(x) L ## x
#endif

class SSystem
{
public:
  static void* malloc(size_t len)
  {
    return ::malloc(len);
  }
  static void free(void* ptr)
  {
    ::free(ptr);
  }

  static int atoi(const SCHAR* arg)
  {
#if !defined(_UNICODE)
    return ::atoi(arg);
#else
    return ::_wtoi(arg);
#endif
  };

  static size_t strlen(const SCHAR* string)
  {
#if !defined(_UNICODE)
    return ::strlen(string);
#else
    return ::wcslen(string);
#endif
  };

  static int strcmp(const SCHAR* string1, const SCHAR* string2)
  {
#if !defined(_UNICODE)
    return ::strcmp(string1, string2);
#else
    return ::wcscmp(string1, string2);
#endif
  };

  static int stricmp(const SCHAR* string1, const SCHAR* string2)
  {
#if !defined(_UNICODE)
  #if defined(WIN32)
    return ::_stricmp(string1, string2);
  #else
    return ::strcasecmp(string1, string2);
  #endif
#else
  #if defined(WIN32)
    return ::_wcsicmp(string1, string2);
  #else
    // ?
  #endif
#endif
  };

  static int strncmp(const SCHAR* string1, const SCHAR* string2, size_t count)
  {
#if !defined(_UNICODE)
    return ::strncmp(string1, string2, count);
#else
    return ::wcsncmp(string1, string2, count);
#endif
  };

#if !defined(_UNICODE)
#else
  #define printf wprintf
#endif

#if !defined(_UNICODE)
  #if defined(WIN32)
    static int mkdir(const SCHAR* dirname) { return ::_mkdir(dirname); };
  #else
    static int mkdir(const SCHAR* dirname) { return ::mkdir(dirname, 0770); };// 0770 not 770 due to octal
  #endif
#else
  #if defined(WIN32)
    static int mkdir(const SCHAR* dirname) { return ::_wmkdir(dirname); };
  #else
    // ?
  #endif
#endif


};

#if defined (WIN32)
  #include "Windows.h"
  #define atow(strA, strW, lenW) MultiByteToWideChar(CP_ACP, 0, strA, -1, strW, lenW)
  #define wtoa(strW, strA, lenA) WideCharToMultiByte(CP_ACP, 0, strW, -1, strA, lenA, NULL, NULL)
#else
int atow(char* strA, unsigned short* strW, int lenW)
{
  int len;
  char* pA;
  unsigned short* pW;

  // Start with len=1, not len=0, as string length returned
  // must include null terminator, as in MultiByteToWideChar()
  for (pA=strA, pW=strW, len=1; lenW; pA++, pW++, lenW--, len++) {
    *pW = (lenW == 1) ? 0 : (unsigned short)(*pA);
    if (!(*pW)) {
      break;
    }
  }
  return len;
}

int wtoa(unsigned short* strW, char* strA, int lenA)
{
  int len;
  char* pA;
  unsigned short* pW;

  // Start with len=1, not len=0, as string length returned
  // must include null terminator, as in WideCharToMultiByte()
  for (pA=strA, pW=strW, len=1; lenA; pA++, pW++, lenA--, len++) {
    *pA = (lenA == 1) ? 0 : (char)(*pW);
    if (!(*pA)) {
      break;
    }
  }
  return len;
}
#endif

// ============================================================================
// Delete Sentinel

// A watchdog tells the referer, when the referee is deleted so that the referer
// can be reset to 0. The user of the pointer can then check the pointer against 0.

// Instead of:
//   FB_DisplayGroup* display_;
// note:
//   SReferer<FB_DisplayGroup> display_;
//
// At the target class add a sentinel as instance variable:
//   SRefererSentinel<FB_DisplayGroup> delete_watchdog_;
// This sentinel will demand initialization with a reference of the pointer,
// which is to be notified upon deletion of the sentinel. So Initialize the
// sentinel like:
//   FB_DisplayGroup::FB_DisplayGroup(SReferer<FB_DisplayGroup>& _referer, ....
//   :delete_watchdog_(_referer)

template <class T> class SRefererSentinel;

template <class T> class SReferer
{
public:
  // it wants to be initialized
  // supply 0 if you don't know better
  SReferer<T>(T* _p)
  {
    forward_ptr_= _p;
    sentinel_= 0;
  }

  ~SReferer<T>()
  {
    if (sentinel_ != 0) {
      sentinel_->dtor();
    }
  }

  operator T* (void)
  {
    return forward_ptr_;
  }

  T* operator -> (void)
  {
    return forward_ptr_;
  }

  SReferer<T>* operator = (T* _old)
  {
    forward_ptr_= _old;
    return this;
  }

  SReferer<T>& operator = (SReferer<T>& _old)
  {
    forward_ptr_= _old.forward_ptr_;
    sentinel_= _old.sentinel_;
    return *this;
  }

  void dtor(void)
  {
    forward_ptr_= 0;
    sentinel_= 0;
  }

  void ctor(SRefererSentinel<T>* _sentinel)
  {
    sentinel_= _sentinel;
  }

private:
  T* forward_ptr_;
  SRefererSentinel<T>* sentinel_;
};

template <class T> class SRefererSentinel
{
public:
  SRefererSentinel<T>(SReferer<T>& _referer)
    :referer_(&_referer)
  {
    if (referer_ != 0) {
      referer_->ctor(this);
    }
  }

  ~SRefererSentinel<T>(void)
  {
    if (referer_ != 0) {
      referer_->dtor();
    }
  }

  void dtor(void)
  {
    referer_= 0;
  }

private:
  SReferer<T>* referer_;
};


#endif // !defined(__SSystem__)
