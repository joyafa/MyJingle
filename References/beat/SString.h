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

#if !defined(AFX_SSTRING_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
#define AFX_SSTRING_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SSystem.h"
#include <vector>

class SString
{
public:
// Core interface
  SString(void);
  SString(const SCHAR* begin);
  SString(SString const &str);
  virtual ~SString(void);
  void Init(void);
  void Clear(void);

  void set(SCHAR* begin, size_t len_in_chars);
  void set(SCHAR* begin, SCHAR* end);
  void set(SCHAR* begin);
  void operator=(const SCHAR* begin);
#if defined(_UNICODE)
  void operator=(const char* begin);
#endif
  void operator=(SString &str);
  SString &Append(SCHAR* begin, SCHAR* end);
  SString &Append(SCHAR* begin);
  void operator+=(const SCHAR* begin);
  void operator+=(SString const &str);

  char* c_str(void) const;
  wchar_t* w_str(void) const;
  operator SCHAR*(void);
  operator const SCHAR*(void) const;

  int strcatf(const SCHAR* fmt, ...);

  unsigned int length(void);

  bool operator==(const SCHAR* str);
  bool operator!=(const SCHAR* str) { return !operator==(str); }

  SCHAR operator[](int pos) { return str_[pos]; }

// More
  void operator=(int value);
  void operator=(unsigned int value);
  int operator<(const SString &s) const;
  int operator>(const SString &s) const;

// Tools
  //SString FilenameExtension();
  //SString FilenameBasePath();

  SString substr(int start, int length);
  int startswith(const SCHAR* start);
  static int npos;
  int find(SCHAR* sep);
  int tok(SCHAR* sep, SString &_part);
  //int replace(const char *_from, const char *_to);

  //SString& trim(char sep);
  //SString& head(size_t _maxlen, char* _postfix= 0);
  //SString& tail(size_t _maxlen, char* _prefix= 0);
  //SString& tolower();

protected:
  void PrepareCStr(void);
  void PrepareWStr(void);
  void InternalAppend(int clear, const SCHAR* begin, size_t len);

protected:
  SCHAR* str_;
  size_t allocated_;
  size_t len_;

#if !defined(_UNICODE)
  wchar_t *w_str_;
#else
  char *c_str_;
#endif
};

SString operator+(SString& a, SString& b);
SString operator+(SString& a, SCHAR* b);

bool UnicodeToMultiBytes(std::vector<char>& pmb, const wchar_t * pun, int uLen);
// ============================================================================

template <class T, int N> class SFlexBufT
{
  // allocates a static buffer of STATIC_LENGTH size
  // if this is not sufficient for size then allocate a dynamic one
public:
  SFlexBufT<T,N>(size_t size)
  :dynamic_buf_(0)
  ,length_(N)
  {
    if (size > N) {
      size_t alloc_size= size;
      dynamic_buf_= new T [alloc_size];
      length_= alloc_size;
    }
  }

  ~SFlexBufT<T,N>()
  {
    if (dynamic_buf_ != 0) {
      delete [] dynamic_buf_;
    }
  }

  operator T* (void)
  {
    if (dynamic_buf_ == 0) {
      return static_buf_;
    } else {
      return dynamic_buf_;
    }
  }

  operator bool (void) { return (operator T* () != 0); }
  size_t length(void)
  {
    return length_;
  }

private:
  T* dynamic_buf_;
  size_t length_;
  T static_buf_[N];
};

typedef SFlexBufT<SCHAR,8192> SFlexBuf;

#endif // !defined(AFX_SSTRING_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
