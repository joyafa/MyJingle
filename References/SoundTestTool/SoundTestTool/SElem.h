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

#if !defined(AFX_SELEM_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
#define AFX_SELEM_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SString.h"

class SElem
{
public:
	SElem();
	SElem(SCHAR* name);
	SElem(const SCHAR* name);
	SElem(SCHAR* name, long value);
	SElem(const SCHAR* name, long value);
	SElem(SCHAR* name, SCHAR* str);
	SElem(const SCHAR* name, const SCHAR* str);
	SElem(SCHAR* name, long value, SCHAR* str);
	SElem(const SCHAR* name, long value, const SCHAR* str);
	SElem(long value);
  void operator=(SElem &elem);
	virtual ~SElem();

  // write/read the data part as plain data
	bool WriteData(long dataLen, char* dataPtr);
	bool ReadData(long *dataLen, char* *dataPtr);

  // write/read the long value
	long Long(); // read long
  int Long(long value); // write long

  void String(SCHAR* str);
	void String(const SCHAR* str) { String((SCHAR*) str); }
	void operator=(const SCHAR* str) { String(str); }
	SString& String();

  bool Name(SCHAR* name);
	bool Name(const SCHAR* name) { return Name((SCHAR*) name); }
  SString& Name();

  // write/read the pointer
	void *Ptr(); // read pointer
  void Ptr(void *ptr); // write pointer

  // for associative array
  bool operator==(const SCHAR* str);
  bool operator!=(const SCHAR* str) { return !operator==(str); }

  // clear fields, not free
  void Clear();

	static SCHAR* m_empty_string;

	SElem* m_link;
protected:
	SString m_name;
	SString m_str;
	long m_num; // stores a long value
	void *m_ptr; // stores a void pointer
	char* m_data; // stores data
	long m_len; // length of data bytes (<=size)
	long m_size; // size of allocated data area
};

extern SElem SElem_gEmpty;

#if defined(__cplusplus)
extern "C" {
#endif

bool SElem_FindByObjectRefFilter(void *refCon, void *e);
bool SElem_FindByStringFilter(void *refCon, void *e);
bool SElem_FindByNameFilter(void *refCon, void *e);
bool SElem_FindByNameCaseFilter(void *refCon, void *e);
bool SElem_FindByLongFilter(void *refCon, void *e);
bool SElem_FindByPtrFilter(void *refCon, void *e);

int SElem_CompareNameFilter(void *refCon, void *e);

#if defined(__cplusplus)
}
#endif

// ============================================================================
// Binary buffer wrapper for SElem

class SBuffer: public SElem
{
public:
  bool SetData(unsigned char* dataPtr, long dataLen);
  bool SetData(const char* dataPtr, long dataLen) { return SetData((unsigned char*) dataPtr, dataLen); }
  bool SetData(SString& data) { return SetData((unsigned char*) data.c_str(), data.length() + 1); }
  unsigned char* Data(void);
  int GetString(SString& _data);
  long Length(void);
  bool Append(unsigned char* dataPtr, long dataLen);
  bool Discard(long usedLen);
  void Empty(void) { SetData((unsigned char*) 0, 0); }

public:
  int base64_encode(SString& _string_out);
  int base64_decode(SString& _string_in);
  int dump_encode(SString& _string_out);
private:
  int HTUU_encode (unsigned char* bufin, unsigned int nbytes, char* bufcoded);
  int HTUU_decode(char* bufcoded, unsigned char* bufplain, int outbufsize);

private:
  // disallow to avoid confusion with Data()
  void *Ptr() { return (void*) 0; }
  void Ptr(void*) {};
};

// ============================================================================

class SFile: protected SBuffer
{
  typedef SBuffer super;
public:
  SFile(const SCHAR* _pathname) { pathname_ = _pathname; }
  SString& Path(void) { return pathname_; }
  void Path(SString& _pathname) { pathname_ = _pathname; }
  int Exists(void);
  int Delete(void);
  int Age(void);
  int Load(void);
  int Save(int _flags= 0);
  int CreatePath(void);
  int GetData(SBuffer& _data);
  int GetData(SString& _data);
  int SetData(SBuffer& _data);
  int SetData(SString& _data);
  int SetData(unsigned char* dataPtr, size_t dataLen)  { return SBuffer::SetData(dataPtr, (long) dataLen); }
  enum {
    SaveFlag_CreatePath= 1
  };
private:
  SString pathname_;
  unsigned char* Data(void) { return SBuffer::Data(); }
  long Length(void) { return SBuffer::Length(); }
};

#endif // !defined(AFX_SELEM_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
