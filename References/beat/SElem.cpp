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
#include "SElem.h"

SElem SElem_gEmpty(_S(""), _S(""));

void SElem::Clear()
{
	m_link = 0;
	m_num = 0;
	m_ptr = 0;
	m_len = 0;
	m_size = 0;
	m_data = 0;
}

SElem::SElem()
{
	Clear();
}

SElem::SElem(SCHAR* name)
{
	Clear();
	m_name = name;
}

SElem::SElem(const SCHAR* name)
{
	Clear();
	m_name = name;
}

SElem::SElem(SCHAR* name, long value)
{
	Clear();
	m_name = name;
	m_num = value;
}

SElem::SElem(const SCHAR* name, long value)
{
	Clear();
	m_name = name;
	m_num = value;
}

SElem::SElem(SCHAR* name, SCHAR* str)
{
	Clear();
	m_name = name;
	m_str = str;
}

SElem::SElem(const SCHAR* name, const SCHAR* str)
{
	Clear();
	m_name = name;
	m_str = str;
}

SElem::SElem(SCHAR* name, long value, SCHAR* str)
{
	Clear();
	m_name = name;
	m_str = str;
	m_num = value;
}

SElem::SElem(const SCHAR* name, long value, const SCHAR* str)
{
	Clear();
	m_name = name;
	m_str = str;
	m_num = value;
}

SElem::SElem(long value)
{
	Clear();
	m_num = value;
}

SElem::~SElem()
{
  if (m_data != 0) { SSystem::free(m_data); }
	m_data= 0;
	m_size= 0;
	m_len= 0;
}

SCHAR* SElem::m_empty_string= _S("");

// ============================================================================
// data

SString& SElem::Name()
{
  return m_name;
}

bool SElem::Name(SCHAR* name)
{
	bool ok= true;
	m_name = name;
	return ok;
}

SString& SElem::String()
{
	return m_str;
}

void SElem::String(SCHAR* str)
{
  m_str = str;
}

void SElem::operator=(SElem &elem)
{
  m_name = elem.m_name;
  m_str = elem.m_str;
  m_num = elem.m_num;
  m_ptr = elem.m_ptr;

  long dataLen= 0;
  char* dataPtr= 0;
  if (elem.ReadData(&dataLen, &dataPtr)) {
    WriteData(dataLen, dataPtr);
  }
};

long SElem::Long()
{
	return m_num;
}

int SElem::Long(long value)
{
	return m_num = value;
}

void *SElem::Ptr()
{
	return m_ptr;
}

void SElem::Ptr(void *ptr)
{
	m_ptr = ptr;
}

bool SElem::ReadData(long *dataLen, char* *dataPtr)
{
	if (dataLen != 0) *dataLen = m_len;
	if (dataPtr != 0) *dataPtr = m_data;
	return true;
}

bool SElem::WriteData(long dataLen, char* dataPtr)
{
	bool ok= false;

  if (dataPtr != m_data) {
	  if (m_data != 0) {
		  SSystem::free(m_data);
		  m_len = 0;
		  m_size = 0;
		  m_data = 0;
	  }
	  if (dataPtr != 0) {
      m_data = (char*) SSystem::malloc(dataLen);
		  if (m_data != 0) {
			  m_size= dataLen;
			  memcpy(m_data, dataPtr, dataLen);
			  m_len = dataLen;
			  ok= true;
		  }
	  }
  }

	return ok;
}

bool SElem::operator==(const SCHAR* str)
{
  return m_str == str;
}

// ============================================================================
// filter

bool SElem_FindByObjectRefFilter(void *refCon, void *e)
{
	return(e == refCon);
}

bool SElem_FindByLongFilter(void *refCon, void *e)
{
  if ((long) refCon == ((SElem *)e)->Long()) {
	  return true;
  } else {
	  return false;
  }
}

bool SElem_FindByPtrFilter(void *refCon, void *e)
{
  if (refCon == ((SElem *)e)->Ptr()) {
	  return true;
  } else {
	  return false;
  }
}

bool SElem_FindByStringFilter(void *refCon, void *e)
{
	if ((refCon == 0) ||(((SElem *)e)->String() == 0) ) {
		return false;
	} else {
		if (SSystem::strcmp((SCHAR*) refCon,((SElem *)e)->String())) {
			return false;
		} else {
			return true;
		}
	}
}

bool SElem_FindByNameFilter(void *refCon, void *e)
{
	if ((refCon == 0) ||(((SElem *)e)->Name() == _S("")) ) {
		return false;
	} else {
		if (SSystem::strcmp((SCHAR*) refCon,((SElem *)e)->Name())) {
			return false;
		} else {
			return true;
		}
	}
}

bool SElem_FindByNameCaseFilter(void *refCon, void *e)
{
	if ((refCon == 0) ||(((SElem *)e)->Name() == _S("")) ) {
		return false;
	} else {
		if (SSystem::stricmp((SCHAR*) refCon,((SElem *)e)->Name())) {
			return false;
		} else {
			return true;
		}
	}
}

int SElem_CompareNameFilter(void *refCon, void *e)
{
	if ((refCon == 0) || (((SElem *)e)->Name() == _S("")) ) {
		return 0;
	} else {
		return SSystem::strcmp((SCHAR*) refCon,((SElem *)e)->Name());
	}
}

// ============================================================================
// Binary buffer wrapper for SElem

bool SBuffer::SetData(unsigned char* dataPtr, long dataLen)
{
  return WriteData(dataLen, (char*) dataPtr);
}

int SBuffer::GetString(SString& _data)
{
  int ok= 1;

  SFlexBuf buf(Length()+1);
  if (buf) {
    memcpy((SCHAR*) buf, Data(), Length());
    buf[Length()]= '\0';
    _data= (SCHAR*) buf;
  } else {
    ok= 0;
  }

  return ok;
}

unsigned char* SBuffer::Data(void)
{
  long dataLen= 0;
  char* dataPtr;
  ReadData(&dataLen, &dataPtr);
  return (unsigned char*) dataPtr;
}

long SBuffer::Length(void)
{
  long dataLen= 0;
  char* dataPtr;
  ReadData(&dataLen, &dataPtr);
  return dataLen;
}

bool SBuffer::Append(unsigned char* dataPtr, long dataLen)
{
  bool retval= false;
  long old_dataLen= 0, new_dataLen= 0;
  char* old_dataPtr= 0; unsigned char*new_dataPtr= 0;

  if (dataPtr != 0 && dataLen != 0) {
    if (ReadData(&old_dataLen, &old_dataPtr)) {
      if (old_dataLen == 0 || old_dataPtr == 0) {
        new_dataLen= dataLen;
        new_dataPtr= dataPtr;
      } else {
        new_dataLen= old_dataLen + dataLen;
        new_dataPtr= (unsigned char*) SSystem::malloc(new_dataLen);
        if (new_dataPtr != 0) {
          memcpy(new_dataPtr, old_dataPtr, old_dataLen);
          memcpy(&(new_dataPtr[old_dataLen]), dataPtr, dataLen);
        }
      }
      if (new_dataPtr != 0 && new_dataLen != 0) {
        retval= WriteData(new_dataLen, (char*) new_dataPtr);
      }
      if (new_dataPtr != 0 && new_dataPtr != dataPtr) {
        SSystem::free(new_dataPtr);
      }
    }
  }

  return retval;
}

bool SBuffer::Discard(long usedLen)
{
  bool retval= false;

  if (usedLen < Length()) {
    SBuffer tmp;
    tmp.SetData(Data() + usedLen, Length() - usedLen);
    retval= SetData(tmp.Data(), tmp.Length());
  } else if (usedLen == Length()) {
    Empty();
  }

  return retval;
}

int SBuffer::dump_encode(SString& _string_out)
{
  int ok= 0;
/*
  unsigned char b[]= {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
  };
  SetData(b, sizeof(b));
*/
  if (Length() > 0) {
    SFlexBufT<char,8192> encoded(Length()*3+1);
    if (encoded) {
      ok= 1;

      int escape_cnt= 0;
      char* p= (char*) Data();
      size_t len= Length();
      size_t j= 0;
      for (unsigned int i= 0; i<len; i++) {
        if (isprint(*p) && isascii(*p)) {
          ((char*) encoded)[j++]= *p;
        } else {
          escape_cnt++;
          char s[3];
          sprintf(s, "%02x", *((unsigned char*) p));
          ((char*) encoded)[j++]= '%';
          ((char*) encoded)[j++]= s[0];
          ((char*) encoded)[j++]= s[1];
        }
        p++;
      }

      ((char*) encoded)[j++]= '\0';
#if !defined(_UNICODE)
      _string_out= (char*) encoded;
#else
      SFlexBufT<wchar_t,8192> tmp(Length()*3+1);
      atow(encoded, tmp, encoded.length());
      _string_out= tmp;
#endif
    }
  } else {
    _string_out= _S("");
    ok= 1;
  }

  return ok;
}

int SBuffer::base64_encode(SString& _string_out)
{
  int ok= 0;

  if (Length() > 0) {
    SFlexBufT<char,8192> encoded(Length()<<1);
    if (encoded) {
      int outlen = HTUU_encode(Data(), Length(), (char*) encoded);
      if (outlen > 0) {
#if !defined(_UNICODE)
        _string_out = (char*) encoded;
#else
        SFlexBufT<wchar_t,8192> tmp(Length()*3+1);
        atow(encoded, tmp, encoded.length());
        _string_out = tmp;
#endif
        ok= 1;
      }
    }
  } else {
    _string_out= _S("");
    ok= 1;
  }

  return ok;
}

int SBuffer::base64_decode(SString& _string_in)
{
  int ok= 0;
  SFlexBufT<char,8192> decoded(_string_in.length()+1);

  if (decoded) {
    if (_string_in.length() > 0) {
      int outlen = HTUU_decode(_string_in.c_str(), (unsigned char*) (char*) decoded, decoded.length());
      if (outlen > 0) {
        ok= SetData((char*) decoded, outlen);
      }
    } else {
      Empty();
    }
  }


  return ok;
}

static char six2pr[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/'
};

static unsigned char pr2six[256];


/*--- function HTUU_encode -----------------------------------------------
 *
 *   Encode a single line of binary data to a standard format that
 *   uses only printing ASCII characters (but takes up 33% more bytes).
 *
 *    Entry    bufin    points to a buffer of bytes.  If nbytes is not
 *                      a multiple of three, then the byte just beyond
 *                      the last byte in the buffer must be 0.
 *             nbytes   is the number of bytes in that buffer.
 *                      This cannot be more than 48.
 *             bufcoded points to an output buffer.  Be sure that this
 *                      can hold at least 1 + (4*nbytes)/3 characters.
 *
 *    Exit     bufcoded contains the coded line.  The first 4*nbytes/3 bytes
 *                      contain printing ASCII characters representing
 *                      those binary bytes. This may include one or
 *                      two '=' characters used as padding at the end.
 *                      The last byte is a zero byte.
 *             Returns the number of ASCII characters in "bufcoded".
 */
int SBuffer::HTUU_encode (unsigned char* bufin, unsigned int nbytes, char* bufcoded)
{
/* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) six2pr[c]

   register char* outptr = bufcoded;
   unsigned int i;

   for (i=0; i<nbytes; i += 3) {
      *(outptr++) = ENC(*bufin >> 2);            /* c1 */
      *(outptr++) = ENC(((*bufin << 4) & 060) | ((bufin[1] >> 4) & 017)); /*c2*/
      *(outptr++) = ENC(((bufin[1] << 2) & 074) | ((bufin[2] >> 6) & 03));/*c3*/
      *(outptr++) = ENC(bufin[2] & 077);         /* c4 */

      bufin += 3;
   }

   /* If nbytes was not a multiple of 3, then we have encoded too
    * many characters.  Adjust appropriately.
    */
   if(i == nbytes+1) {
      /* There were only 2 bytes in that last group */
      outptr[-1] = '=';
   } else if(i == nbytes+2) {
      /* There was only 1 byte in that last group */
      outptr[-1] = '=';
      outptr[-2] = '=';
   }
   *outptr = '\0';
   return(outptr - bufcoded);
}


/*--- function HTUU_decode ------------------------------------------------
 *
 *  Decode an ASCII-encoded buffer back to its original binary form.
 *
 *    Entry    bufcoded    points to a uuencoded string.  It is
 *                         terminated by any character not in
 *                         the printable character table six2pr, but
 *                         leading whitespace is stripped.
 *             bufplain    points to the output buffer; must be big
 *                         enough to hold the decoded string (generally
 *                         shorter than the encoded string) plus
 *                         as many as two extra bytes used during
 *                         the decoding process.
 *             outbufsize  is the maximum number of bytes that
 *                         can fit in bufplain.
 *
 *    Exit     Returns the number of binary bytes decoded.
 *             bufplain    contains these bytes.
 */
int SBuffer::HTUU_decode(char* bufcoded, unsigned char* bufplain, int outbufsize)
{
/* single character decode */
#define DEC(c) pr2six[(int)c]
#define MAXVAL 63

   static int first = 1;

   int nbytesdecoded, j;
   register char* bufin = bufcoded;
   register unsigned char* bufout = bufplain;
   register int nprbytes;

   /* If this is the first call, initialize the mapping table.
    * This code should work even on non-ASCII machines.
    */
   if(first) {
      first = 0;
      for(j=0; j<256; j++) pr2six[j] = MAXVAL+1;

      for(j=0; j<64; j++) pr2six[(int)six2pr[j]] = (unsigned char) j;
#if 0
      pr2six['A']= 0; pr2six['B']= 1; pr2six['C']= 2; pr2six['D']= 3;
      pr2six['E']= 4; pr2six['F']= 5; pr2six['G']= 6; pr2six['H']= 7;
      pr2six['I']= 8; pr2six['J']= 9; pr2six['K']=10; pr2six['L']=11;
      pr2six['M']=12; pr2six['N']=13; pr2six['O']=14; pr2six['P']=15;
      pr2six['Q']=16; pr2six['R']=17; pr2six['S']=18; pr2six['T']=19;
      pr2six['U']=20; pr2six['V']=21; pr2six['W']=22; pr2six['X']=23;
      pr2six['Y']=24; pr2six['Z']=25; pr2six['a']=26; pr2six['b']=27;
      pr2six['c']=28; pr2six['d']=29; pr2six['e']=30; pr2six['f']=31;
      pr2six['g']=32; pr2six['h']=33; pr2six['i']=34; pr2six['j']=35;
      pr2six['k']=36; pr2six['l']=37; pr2six['m']=38; pr2six['n']=39;
      pr2six['o']=40; pr2six['p']=41; pr2six['q']=42; pr2six['r']=43;
      pr2six['s']=44; pr2six['t']=45; pr2six['u']=46; pr2six['v']=47;
      pr2six['w']=48; pr2six['x']=49; pr2six['y']=50; pr2six['z']=51;
      pr2six['0']=52; pr2six['1']=53; pr2six['2']=54; pr2six['3']=55;
      pr2six['4']=56; pr2six['5']=57; pr2six['6']=58; pr2six['7']=59;
      pr2six['8']=60; pr2six['9']=61; pr2six['+']=62; pr2six['/']=63;
#endif
   }

   /* Strip leading whitespace. */

   while(*bufcoded==' ' || *bufcoded == '\t') bufcoded++;

   /* Figure out how many characters are in the input buffer.
    * If this would decode into more bytes than would fit into
    * the output buffer, adjust the number of input bytes downwards.
    */
   bufin = bufcoded;
   while(pr2six[(int)*(bufin++)] <= MAXVAL);
   nprbytes = bufin - bufcoded - 1;
   nbytesdecoded = ((nprbytes+3)/4) * 3;
   if(nbytesdecoded > outbufsize) {
      nprbytes = (outbufsize*4)/3;
   }

   bufin = bufcoded;

   while (nprbytes > 0) {
      *(bufout++) = (unsigned char) (DEC(*bufin) << 2 | DEC(bufin[1]) >> 4);
      *(bufout++) = (unsigned char) (DEC(bufin[1]) << 4 | DEC(bufin[2]) >> 2);
      *(bufout++) = (unsigned char) (DEC(bufin[2]) << 6 | DEC(bufin[3]));
      bufin += 4;
      nprbytes -= 4;
   }

   if(nprbytes & 03) {
      if(pr2six[(int)bufin[-2]] > MAXVAL) {
         nbytesdecoded -= 2;
      } else {
         nbytesdecoded -= 1;
      }
   }

   return(nbytesdecoded);
}

// ============================================================================

#include <sys/stat.h>
#if !defined (EEXIST)
#  define EEXIST 17
#endif

#if defined(_UNICODE)
  #include <io.h>
#endif

int SFile::Exists(void)
{
  int ok= 1;

#if !defined(_UNICODE)
  FILE* f= ::fopen(Path(), _S("rb"));
#else
  FILE* f= ::_wfopen(Path(), _S("rb"));
#endif
  if (f == 0) {
    ok= 0;
  } else {
    ::fclose(f);
  }

  return ok;
}

int SFile::Delete(void)
{
  int ok= 1;

#if !defined(_UNICODE)
  int res= unlink(Path());
#else
  int res= _wunlink(Path());
#endif
  if (res != 0) {
    ok= 0;
  }

  return ok;
}

int SFile::Age(void)
{
  int age= -1;

#if !defined(_UNICODE)
  struct stat statbuf;
  if (stat(Path(), &statbuf) == 0) {
#else
  struct _stat statbuf;
  if (_wstat(Path(), &statbuf) == 0) {
#endif
    age= time(0) - statbuf.st_ctime;
  }

  return age;
}

int SFile::Load()
{
  int ok= 1;

#if !defined(_UNICODE)
  FILE* f= ::fopen(Path(), _S("rb"));
#else
  FILE* f= ::_wfopen(Path(), _S("rb"));
#endif
  if (f == 0) {
    ok= 0;
  } else {
    ok= 1; // revert ok logic
    unsigned char buf[16384];

    int done= 0;
    while (!done && ok) {
      int n= ::fread(buf, sizeof( char ), sizeof(buf), f );
      if (n > 0) {
        ok= this->Append(buf, n);
      } else {
        done= 1;
        if (! feof(f) || ferror(f)) ok= 0;
      }
    }
    ::fclose(f);
  }

  return ok;
}

int SFile::CreatePath()
{
  int ok = 1;

  int first = 1;
  int done = 0;
  SString path = Path();
  SString dirpath;
  SString part;
  while (!done && path.tok(_S("\\/"), part)) {
    if (first && part == _S("")) {
      done = 1;
    } else {
      first = 0;
      if (part.operator!=(_S(""))) {
        if (path.operator==(_S(""))) {
          done = 1;
        } else {
          dirpath += part;

          int res = SSystem::mkdir(dirpath);
          if (res == -1) {
            if (errno != EEXIST) {
              ok = 0;
              done = 1;
            }
          }
          dirpath += _S("/");

        }
      }
    }
  }

  return ok;
}

int SFile::Save(int _flags)
{
  int ok= 1;

  if (_flags && SaveFlag_CreatePath) {
    ok= CreatePath();
  }

  if (ok) {
#if !defined(_UNICODE)
    FILE* f= ::fopen(Path(), _S("wb"));
#else
    FILE* f= ::_wfopen(Path(), _S("wb"));
#endif
    if (f == 0) {
      ok= 0;
    } else {
      ok= 1; // revert ok logic

      int n= ::fwrite( this->Data(), 1, this->Length(), f );
      if (n < this->Length()) {
        ok = 0;
      }

      ::fclose(f);
    }
  }

  return ok;
}

int SFile::GetData(SBuffer& _data)
{
  int ok= 1;

  if (this->Data() == 0) {
    ok= this->Load();
  }
  if (ok) {
    _data.SetData(this->Data(), this->Length());
  }

  return ok;
}

int SFile::GetData(SString& _data)
{
  // String from file data: append trailing 0
  SFlexBufT<char,8192> buf(Length() + 1);
  if (buf) {
    memcpy((char*) buf, Data(), Length());
    ((char*) buf)[Length()] = 0;
#if !defined(_UNICODE)
    _data = (char*) buf;
#else
    SFlexBufT<wchar_t,8192> tmp(Length() + 1);
    atow(buf, tmp, Length() + 1);
    _data = tmp;
#endif
    return 1;
  }
  return 0;
}

int SFile::SetData(SBuffer& _data)
{
  return super::SetData(_data.Data(), _data.Length());
}

int SFile::SetData(SString& _data)
{
  // File data from ascii string without trailing 0
  return super::SetData(_data.c_str(), _data.length());
}

// ============================================================================

#include "SListT.h"

void KeyValueBlob2SList(const SCHAR* _text, SList &_list, const SCHAR* _linesep, const SCHAR* _fieldsep)
{
  if (_text != 0) {
    int fieldsep_len = SSystem::strlen(_fieldsep);
    int len_in_bytes = (SSystem::strlen(_text) + 1) * sizeof(SCHAR);
    SCHAR* p = (SCHAR*) SSystem::malloc(len_in_bytes);
    if (p != 0) {
      memcpy(p, _text, len_in_bytes);

#if !defined(_UNICODE)
      SCHAR* q= strtok(p, _linesep);
#else
      SCHAR* q= wcstok(p, _linesep);
#endif
      while (q != 0) {
        SCHAR* r= 0;

        for (int i = 0; i < fieldsep_len; ++i) {
#if !defined(_UNICODE)
          SCHAR* rx= strchr(q, _fieldsep[i]);
#else
          SCHAR* rx= wcschr(q, _fieldsep[i]);
#endif
          if (r == 0 && rx != 0) r = rx;
          if (rx != 0 && rx < r) r = rx;
        }

        if (r != 0) {
          *r++= 0;
#if !defined(_UNICODE)
          while (strchr(_fieldsep, *r) && *r != _S('\0')) r++;
#else
          while (wcschr(_fieldsep, *r) && *r != _S('\0')) r++;
#endif
        } else {
          r = _S("");
        }

        _list.AddLast(q, r);
#if !defined(_UNICODE)
        q= strtok(0, _linesep);
#else
        q= wcstok(0, _linesep);
#endif
      }
    }
    SSystem::free(p);
  }
}
void KeyValueLfBlob2SList(const SCHAR* _text, SList &_list) { KeyValueBlob2SList((const SCHAR* ) _text, _list); }
void KeyValueLfBlob2SList(SCHAR* _text, SList &_list) { KeyValueBlob2SList((const SCHAR* ) _text, _list); }

// ============================================================================
// we need this for the gcc
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
  template class SListT<SElem, SElem>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
  #pragma instantiate SListT<SElem, SElem>
#endif // #if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)


// ============================================================================
// test

/*
#include <iostream.h>

int testmain(int argc, char* argv[])
{
  cout << "sizeof(SElem)= " << sizeof(SElem) << '\n';
  cout << "sizeof(SList)= " << sizeof(SList) << '\n';
  cout << '\n';

  cout << "Name" << ' ' << "Long-value" << ' ' << "Data-as-string" << '\n';
  cout << '\n';

  // just the List with SElems
  // created with new that it can be added to a cascaded list
  SList *l1= new SList(_S("l1"));
  SElem *s1= new SElem(_S("s1"));
  SElem *s2= new SElem(_S("s2"));
  SElem *s3= new SElem(_S("s3"));
  SElem *s4= new SElem(_S("s4"));

  l1->Add(s1);
  l1->AddFirst(s2);
  l1->Add(s3);
  l1->AddLast(s4);
  l1->Rmv(s2);
  s1->String(_S("SElem1-of-List1"));
  s3->Long(33);

  SElem* t1= l1->FindByName(_S("s1"));
	SElem &t2=(*l1)[_S("s1")]; // search a known elem
	SElem &t3=(*l1)[_S("u1")]; // search unknown -> add new
	(*l1)[_S("s4")] = _S("SElem2-of-List1-written-by-operator="); // write to existing
	(*l1)[_S("u2")] = _S("new-Elem-of-List1-written-by-operator="); // write to new

  {
    SElem *ei= 0;
    while ((ei= l1->Next(ei)) != 0) {
      cout << (SCHAR*) ei->Name() << ' ' << ei->Long() << ' ' << (SCHAR*) ei->String() << '\n';
    }
  }

  if (l1->FindByName(_S("t3"))) {}
  if (l1->FindByNameI(_S("T3"))) {}
  //	if (l1->FindByPtr(t1)) {}

  cout << '\n';

  //  a cascaded list of lists of SElem
  SList *l2= new SList(_S("l2"));
  SListList *l3= new SListList(_S("l3"));
  l3->AddLast(l1);
  l3->AddLast(l2);
  l1->String(_S("List1-is-Element-of-List3"));
  l2->String(_S("List2-is-also-an-Element-of-List3"));

  {
    SList *ei= 0;
    while ((ei= l3->Next(ei)) != 0) {
      cout << (SCHAR*) ei->Name() << _S(' ') << ei->Long() << _S(' ') << (SCHAR*) ei->String() << _S('\n');
    }
  }

  delete s2;
  delete l3;

  return 0;
}
*/

/*
int testmain(int argc, char* argv[])
{
  cout << "sizeof(SElem)= " << sizeof(SElem) << '\n';
  cout << "sizeof(SList)= " << sizeof(SList) << '\n';
  cout << '\n';

  cout << "Name" << ' ' << "Long-value" << ' ' << "Data-as-string" << '\n';
  cout << '\n';

  // just the List with SElems
  // created with new that it can be added to a cascaded list
  SList * l1= new SList("l1");
  SElem *s1= new SElem("s1");
  SElem *s2= new SElem("s2");
  SElem *s3= new SElem("s3");
  SElem *s4= new SElem("s4");

  l1->AddSorted(s1);
  l1->AddSorted(s4);
  l1->AddSorted(s3);
  l1->AddSorted(s2);

  {
    SElem *ei= 0;
    while ((ei= l1->Next(ei)) != 0) {
      cout << ei->Name() << ' ' << ei->Long() << ' ' << ei->String() << '\n';
    }
  }

	char c;
  cin >> c;
  return 0;
}
*/

/*
int testmain(int argc, char* argv[])
{
  SArray a;

  // create SElem("1")
  a["1"]= 1;
  // find SElem("1", 1)
  a[1]= 1;
  // dont find -> create SElem(2) and assign 2 (which is redundant, because int index and int value use the same field)
  a[2]= 2;

  long b;
  // find SElem("1") return 1
  b= a["1"];
  // dont find -> create SElem("2"), return 0
  b= a["2"];
  // find SElem("1", 1), return 1
  b= a[1];
  // find SElem(2), return 2
  b= a[2];
  // dont find -> create SElem(3), return 3
  b= a[3];

  // beware: a[(int) i] allocates a i element and returns i, if not found

  return 0;
}
*/

/* test operator= for SElem and SList
int testmain(int argc, char* argv[])
{
  SElem x("x", 1, "xx");
  SElem *y= new SElem("y", 2, "yy");
  *y= x;

  SList b;
  {
    SList a;

    a.Write("a", 1);
    a.Write("b", 2);
    a.Write("c", 3);

    b= a;
  }

  return 0;
}
*/

/* test operator= for SListList
int testmain(int argc, char* argv[])
{
  SListList b;
  {
    SListList a;
    SList *a1= new SList();
    SList *a2= new SList();

    a1->Write("a", 1);
    a1->Write("b", 2);
    a1->Write("c", 3);

    a2->Write("d", 4);
    a2->Write("e", 5);
    a2->Write("f", 6);

    a.Add(a1);
    a.Add(a2);

    b= a;
  }

  return 0;
}
*/


/* test operator= for SList as instance variable of copied object
class Contains_a_Slist
{
public:
  Contains_a_Slist() {}
  virtual ~Contains_a_Slist() {};
  SList a;
};

int testmain(int argc, char* argv[])
{
  Contains_a_Slist d;
  {
    Contains_a_Slist c;

    c.a.Write("a", 1);
    c.a.Write("b", 2);
    c.a.Write("c", 3);

    d= c;
  }

  return 0;
}
*/

/* tests the bad heap block problem
// seems to be an interworking of ACE/AFX and debug heap
int testmain(int argc, char* argv[])
{
  SElem x("x", 1, "xx");
  SElem *y= new SElem("y", 2, "yy");

  *y= x;

  delete y;

  return 0;
}
*/

/* test obj ref iterator
template <class E> class SIterT
{
public:
  SIterT() : eptr(0) {};
  operator bool() { return eptr != 0; };
  operator E*() { return eptr; };
  operator =(E* e) { eptr= e; };
  E* operator ->(void) { return eptr; };
private:
  E* eptr;
};

typedef SIterT<SElem> SIter;


  SIterT<E> Next(SIterT<E> e)
  {
    SIterT<E> retval;

    if (e== 0) {
      retval= m_sublink;
    } else {
      retval= (E *) (e->m_link);
    }
    return retval;
  }

int testmain(int argc, char* argv[])
{
  SList l;
  SElem *e1= new SElem("a", 1);;
  SElem *e2= new SElem("b", "c");
  SElem *e3= new SElem();
  l.AddLast(e1);
  l.AddLast(e2);
  l.AddLast(e3);

  SIter i;
  while (i= l.Next(i)) {
    char* s= i->Name();
    s=s;
  }

  return 0;
}
*/

/*
class SXX: public SElem
{
};

int testmain(int argc, char* argv[])
{
  SListT<SXX,SElem> l;
  l.AddLast("e1");

  return 0;
}
*/
