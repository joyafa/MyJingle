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

#include "SSystem.h"
#include "SString.h"
#include <vector>

// ============================================================================

SString::SString(void)
{
  Init();
}

SString::SString(const SCHAR* begin)
{
  Init();
  InternalAppend(0, (SCHAR*) begin, SSystem::strlen(begin));
}

SString::SString(SString const &str)
{
  Init();
#if !defined(_UNICODE)
  InternalAppend(0, ((SString*) &str)->c_str(), ((SString*) &str)->length());
#else
  InternalAppend(0, ((SString*) &str)->w_str(), ((SString*) &str)->length());
#endif
}

SString::~SString(void)
{
  Clear();
}

void SString::Init(void)
{
  str_ = 0;
  allocated_ = 0;
  len_ = 0;
#if !defined(_UNICODE)
  w_str_ = 0;
#else
  c_str_ = 0;
#endif
}

void SString::Clear(void)
{
  if (str_ != 0) { SSystem::free(str_); }
  str_ = 0;
#if !defined(_UNICODE)
  if (w_str_ != 0) { SSystem::free(w_str_); }
  w_str_ = 0;
#else
  if (c_str_ != 0) { SSystem::free(c_str_); }
  c_str_ = 0;
#endif
  allocated_ = 0;
  len_ = 0;
}

// ============================================================================

void SString::InternalAppend(int clear, const SCHAR* begin, size_t len)
{
  if (clear) { Clear(); }

  size_t len1_in_chars = length();
  size_t len2_in_chars = len;
  size_t len1_in_bytes = len1_in_chars * sizeof(SCHAR);
  size_t len2_in_bytes = len2_in_chars * sizeof(SCHAR);

  if (len2_in_bytes > 0) {
    size_t len_in_bytes = len1_in_bytes + len2_in_bytes + sizeof(SCHAR);

    if (allocated_ > len_in_bytes) {
      memcpy(str_ + len1_in_chars, begin, len2_in_bytes);
      str_[len1_in_chars + len2_in_chars] = _S('\0');

    } else {

      size_t allocate = 64;
      if (allocate < len_in_bytes) {
        allocate = (len_in_bytes > allocated_) ? 2 * len_in_bytes : 2 * allocated_;
      }

      SCHAR* tmp = (SCHAR*) SSystem::malloc(allocate);
      if (tmp == 0) {
        return; // no change due to mem alloc problem
      }

      allocated_ = allocate;
      if (str_ != 0) {
        memcpy(tmp, str_, len1_in_bytes);
      }
      memcpy(tmp + len1_in_chars, begin, len2_in_bytes);
      tmp[len1_in_chars + len2_in_chars] = _S('\0');

      if (str_ != 0) { SSystem::free(str_); }
      str_ = tmp;
    }

    len_ = len1_in_chars + len2_in_chars;
  }
}

// ============================================================================

void SString::set(SCHAR* begin, size_t len_in_chars)
{
  if (begin != 0) {
    InternalAppend(1, begin, len_in_chars);
  }
}

void SString::set(SCHAR* begin, SCHAR* end)
{
  if (begin != 0 && end != 0) {
    InternalAppend(1, begin, (size_t) (end - begin));
  }
}

void SString::set(SCHAR* begin)
{
  if (begin != 0) {
    InternalAppend(1, begin, SSystem::strlen(begin));
  }
}

#if defined(_UNICODE)
void SString::operator=(const char* begin)
{
  int len = ::strlen(begin) + 1;
  SFlexBufT<wchar_t,8192> tmp(len);
  atow(begin, tmp, len);
  set((SCHAR*) tmp);
}
#endif

void SString::operator=(const SCHAR* begin)
{
  if (begin != 0) {
    InternalAppend(1, begin, SSystem::strlen(begin));
  }
}

void SString::operator=(SString &str)
{
#if !defined(_UNICODE)
  InternalAppend(1, str.c_str(), str.length());
#else
  InternalAppend(1, str.w_str(), str.length());
#endif
}

SString &SString::Append(SCHAR* begin, SCHAR* end)
{
  if (begin != 0 && end != 0) {
    InternalAppend(0, begin, (size_t) (end - begin));
  }
  return *this;
}

SString &SString::Append(SCHAR* begin)
{
  if (begin != 0) {
    InternalAppend(0, begin, SSystem::strlen(begin));
  }
  return *this;
}

void SString::operator+=(const SCHAR* begin)
{
  if (begin != 0) {
    InternalAppend(0, begin, SSystem::strlen(begin));
  }
}

void SString::operator+=(SString const &str)
{
#if !defined(_UNICODE)
  InternalAppend(0, ((SString*) &str)->c_str(), ((SString*) &str)->length());
#else
  InternalAppend(0, ((SString*) &str)->w_str(), ((SString*) &str)->length());
#endif
}

// ============================================================================

SString::operator SCHAR*(void) { return (SCHAR*) operator const SCHAR*(); }
SString::operator const SCHAR*(void) const
{
#if !defined(_UNICODE)
  return c_str();
#else
  return w_str();
#endif
}

char* SString::c_str(void) const
{
#if !defined(_UNICODE)
  if (str_ == 0) return "";
  else return str_;
#else
  if (str_ == 0) return "";
  else {
    ((class SString*) this)->PrepareCStr();
    if (c_str_ == 0) return "";
    else return c_str_;
  }
#endif
}

wchar_t* SString::w_str(void) const
{
#if !defined(_UNICODE)
  if (str_ == 0) return L"";
  else {
    ((class SString*) this)->PrepareWStr();
    if (w_str_ == 0) return L"";
    else return w_str_;
  }
#else
  if (str_ == 0) return L"";
  else return str_;
#endif
}

#if !defined(_UNICODE)
void SString::PrepareWStr(void)
{
  if (w_str_ != 0) {
    SSystem::free(w_str_);
  }
  int allocate = (len_ + 1) * sizeof(unsigned short);
  w_str_ = (wchar_t* ) SSystem::malloc(allocate);
  if (w_str_ != 0) {
    atow(str_, w_str_, allocate);
  }
}
#else
void SString::PrepareCStr(void)
{
  if (c_str_ != 0) {
    SSystem::free(c_str_);
  }
  int allocate = (len_ + 1) * sizeof(char);
  c_str_ = (char* ) SSystem::malloc(allocate);
  if (c_str_ != 0) {
    wtoa(str_, c_str_, allocate);
  }
}
#endif

unsigned int SString::length(void)
{
  if (str_ == 0) return 0;
  else return len_;
}

bool SString::operator==(const SCHAR* str)
{
  if (str_ == 0 ) {
    if (str == 0 || *str == _S('\0')) return 1;
    else return 0;
  } else if (str == 0) {
    if (str_ == 0 || *str_ == _S('\0')) return 1;
    else return 0;
  } else {
    return (! SSystem::strcmp(str_, str) );
  }
}

#if defined (WIN32)
  #if defined(vsnprintf)
    #undef vsnprintf
  #endif
  #if !defined(_UNICODE)
    #define vsnprintf _vsnprintf
  #else
    #define vsnprintf _vsnwprintf
  #endif
#endif

#include <stdarg.h>

#define SString_strcatf_STATIC_STRING_LENGTH 8192

int SString::strcatf(const SCHAR* fmt, ...)
{
  va_list argp;
  // default buffer large enough for most tasks
  SCHAR buf[(SString_strcatf_STATIC_STRING_LENGTH+1) * sizeof(SCHAR)], *bufp = buf, *varbuf = 0;
  int len_in_chars = 0;
  *buf= _S('\0');

  va_start (argp, fmt);
  len_in_chars = vsnprintf(bufp, SString_strcatf_STATIC_STRING_LENGTH, fmt, argp);
  va_end(argp);
  // make a heap buffer, if default buffer too small
  // retry vsnprintf and increase the heap buffer until the thing fits
  if (len_in_chars < 0) {
    int done= 0;
    size_t alloc_size_in_chars = this->length() > (int) SString_strcatf_STATIC_STRING_LENGTH ? this->length() : (int) SString_strcatf_STATIC_STRING_LENGTH;
    while (!done) {
      if (varbuf != 0) {
        SSystem::free(varbuf);
      }
      alloc_size_in_chars = alloc_size_in_chars << 1;
      varbuf = (SCHAR*) SSystem::malloc((alloc_size_in_chars + 1) * sizeof(SCHAR));
      if (varbuf != 0) {
        bufp = varbuf;
        va_start(argp, fmt);
        len_in_chars = vsnprintf(bufp, alloc_size_in_chars, fmt, argp);
        va_end(argp);
        done = (len_in_chars != -1);
      }
      if (done) {
        bufp[alloc_size_in_chars] = _S('\0');
      }
    }
  }

  InternalAppend(0, bufp, len_in_chars);

  if (varbuf != 0) {
    SSystem::free(varbuf);
  }

  return len_in_chars;
}

// ============================================================================

SString SString::substr(int start, int length)
{
  SString sub;

  sub.set(&(str_[start]), length);

  return sub;
}

int SString::startswith(const SCHAR* start)
{
  return (SSystem::strncmp(str_, start, SSystem::strlen(start)) == 0);
}

int SString::npos= -1;
int SString::find(SCHAR* sep)
{
  int index = npos;

  int i = 0;
  const SCHAR* p = str_;
  if (p != 0) {
    while (
#if !defined(_UNICODE)
      strchr(sep, p[i])
#else
      wcschr(sep, p[i])
#endif
      == 0 && p[i] != _S('\0')) {
      i++;
    }
    if (p[i] != _S('\0')) { index = i; }
  }

  return index;
}

int SString::tok(SCHAR* sep, SString &part)
{
  int ok = 0;

  int index = this->find(sep);
  if (index != npos) {
    part = this->substr(0, index);
    this->set(this->substr(index + 1, this->length() - index - 1));
    ok = 1;
  } else if (this->length() > 0) {
    part = str_;
    Clear();
    ok = 1;
  }

  return ok;
}

// ============================================================================

int SString::operator<(const SString &s) const
{
  return (this->str_ && s.str_) ? SSystem::strcmp(this->str_, s.str_) < 0 : ((s.str_) ? 1 : 0 );
}

int SString::operator>(const SString &s) const
{
  return ! this->operator<(s);
}

// ============================================================================

SString operator+(SString& a, SString& b)
{
  SString ab = a;
  ab += b;
  return ab;
}

SString operator+(SString& a, SCHAR* b)
{
  SString sb(b);
  return a + sb;
}



//Unicode 编码转为多字节编码   
bool  UnicodeToMultiBytes(std::vector<char>& pmb,  const wchar_t * pun, int uLen)  
{  
	// convert an widechar string to Multibyte    
	int MBLen = WideCharToMultiByte(CP_ACP, 0, pun, uLen, NULL, 0, NULL, NULL);  
	if (MBLen <=0)  
	{  
		return false ;  
	}  
	pmb.resize(MBLen);  
	int nRtn = WideCharToMultiByte(CP_ACP, 0, pun, uLen, &*pmb.begin(), MBLen, NULL, NULL);  

	if(nRtn != MBLen)  
	{  
		pmb.clear();  
		return false ;  
	}  

	return true ;  
}  

