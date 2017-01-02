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


#include "beatLog_.h"

#if defined(WIN32)
  #include <Windows.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include <strsafe.h>
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
#endif

//////////////////////////////////////////////////////////////////////

class beatLog_TraceLevel
{
public:
	beatLog_TraceLevel(const char* _pszChannel, const char* _pszContext);
	virtual ~beatLog_TraceLevel(void);

  const char* Channel(void);
  const char* Context(void);

  beatLog_TraceLevel* Next(void);
  void Next(beatLog_TraceLevel* next);

protected:
  beatLog_TraceLevel* pNext_;

  char* pszChannel_;
  char* pszContext_;
};

class beatLog_LogLine
{
public:
	beatLog_LogLine(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);
	virtual ~beatLog_LogLine(void);

  int Mask(void);
  const char* Channel(void);
  const char* Context(void);
  const char* Message(void);

  beatLog_LogLine* Next(void);
  void Next(beatLog_LogLine* next);

protected:
  beatLog_LogLine* pNext_;

  int nMask_;
  char* pszChannel_;
  char* pszContext_;
  char* pszMessage_;
};

class beatLog_MutexObject
{
public:
	beatLog_MutexObject();
	virtual ~beatLog_MutexObject();
};

class beatLog_LogImpl
{
public:
	beatLog_LogImpl();
	virtual ~beatLog_LogImpl();

  beatLog_LogCallback SetLogCallback(beatLog_LogCallback _fpCallback);
  void SetMutexCallback(beatLog_MutexAcquireCallback _fpAcquireCallback, beatLog_MutexReleaseCallback _fpReleaseCallback);

  static void ConsoleOut(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);
  static void Format(char* _pszBuffer, int nBufferLen, int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);

  void Log(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);

  void StoreLine(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);
  int GetLineLength(int* _nChannel, int* _nContext, int* _nMessage);
  int GetLineData(int* _nMask, char* _pszChannel, char* _pszContext, char* _pszMessage);

  void TraceEnter(const char* _pszChannel, const char* _pszContext);
  void TraceLeave(void);
  const char* TraceChannel(void);
  const char* TraceContext(void);

  int SetMask(int _nMask);
  int IsMask(int _nMask);

  static int Mask2Level(int _nMask);

protected:
  void DeleteTraceHead();
  void DeleteLineHead();

protected:
  beatLog_TraceLevel* pTraceLevelHead_;
  int nTraceLevelDepth_;

  beatLog_LogCallback fpOutCallback_;
  beatLog_MutexAcquireCallback fpMutexAcquireCallback_;
  beatLog_MutexReleaseCallback fpMutexReleaseCallback_;
  beatLog_LogLine* pLogLineHead_;

  friend class beatLog_MutexObject;

  int nMask_;

  static char* pszLevelNames_[beatLog_Interface::NLogLevels];
};

//----------------------------------------------------------

class beatLog_OS
{
public:
  static int vsnprintf(char *str, size_t size, const char *format, va_list ap);
  static void consoleout(char *str);
  static int snprintf(char *str, size_t size, const char *format, ...);
};

int beatLog_OS::vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
  int result = 0;
#if defined(WIN32)
  HRESULT hr = ::StringCbVPrintfA(str, size, format, ap);
  if (FAILED(hr)) {
    result = -1;
  }
#else
  result = ::vsnprintf(str, size, format, ap);
#endif
  return result;
}

void beatLog_OS::consoleout(char *str)
{
#if defined(WIN32)
  fprintf(stderr, str);
  OutputDebugStringA((LPCSTR) str);
#else
  fprintf(stderr, str);
#endif
}

int beatLog_OS::snprintf(char *str, size_t size, const char *format, ...)
{
  int result = 0;
  va_list argp;
  va_start(argp, format);
  beatLog_OS::vsnprintf(str, size, format, argp);
  va_end(argp);
  return result;
}

//----------------------------------------------------------

beatLog_LogImpl beatLog_LogInst;
char* beatLog_LogImpl::pszLevelNames_[beatLog_Interface::NLogLevels];

//----------------------------------------------------------

beatLog_MutexObject::beatLog_MutexObject(void)
{
  if (beatLog_LogInst.fpMutexAcquireCallback_ != 0) {
    beatLog_LogInst.fpMutexAcquireCallback_();
  }
}

beatLog_MutexObject::~beatLog_MutexObject(void)
{
  if (beatLog_LogInst.fpMutexReleaseCallback_ != 0) {
    beatLog_LogInst.fpMutexReleaseCallback_();
  }
}

//----------------------------------------------------------

beatLog_LogImpl::beatLog_LogImpl()
:pTraceLevelHead_(0)
,nTraceLevelDepth_(0)
,fpOutCallback_(&beatLog_LogImpl::ConsoleOut)
,fpMutexAcquireCallback_(0)
,fpMutexReleaseCallback_(0)
,pLogLineHead_(0)
,nMask_(beatLog_Interface::MaskDefault)
{
  pszLevelNames_[0] = "None.....";
  pszLevelNames_[1] = "Fatal....";
  pszLevelNames_[2] = "Error....";
  pszLevelNames_[3] = "Warning..";
  pszLevelNames_[4] = "Debug....";
  pszLevelNames_[5] = "Info.....";
  pszLevelNames_[6] = "Verbose..";
  pszLevelNames_[7] = "Trace....";
  pszLevelNames_[8] = "VVerbose.";
}

beatLog_LogImpl::~beatLog_LogImpl()
{
  while (pTraceLevelHead_ != 0) {
    DeleteTraceHead();
  }
  while (pLogLineHead_ != 0) {
    DeleteLineHead();
  }
}

void beatLog_LogImpl::DeleteTraceHead()
{
  beatLog_TraceLevel* pTraceLevel = pTraceLevelHead_;
  pTraceLevelHead_ = pTraceLevelHead_->Next();
  delete pTraceLevel;
}

void beatLog_LogImpl::DeleteLineHead()
{
  beatLog_LogLine* pLogLine = pLogLineHead_;
  pLogLineHead_ = pLogLineHead_->Next();
  delete pLogLine;
}

int beatLog_LogImpl::SetMask(int _nMask)
{
  int nMask = nMask_;
  nMask_ = _nMask;
  return nMask;
}

int beatLog_LogImpl::IsMask(int _nMask)
{
  return (_nMask & nMask_) != 0;
}

int beatLog_LogImpl::Mask2Level(int _nMask)
{
  switch (_nMask) {
    case beatLog_Interface::MaskFatal: return 1;
    case beatLog_Interface::MaskError: return 2;
    case beatLog_Interface::MaskWarning: return 3;
    case beatLog_Interface::MaskDebug: return 4;
    case beatLog_Interface::MaskInfo: return 5;
    case beatLog_Interface::MaskVerbose: return 6;
    case beatLog_Interface::MaskTrace: return 7;
    case beatLog_Interface::MaskVeryVerbose: return 8;
    default:
    return 0;
  }
}

beatLog_LogCallback beatLog_LogImpl::SetLogCallback(beatLog_LogCallback _fpCallback)
{
  beatLog_LogCallback fpOldCallback = fpOutCallback_;
  fpOutCallback_ = _fpCallback;
  return fpOldCallback;
}

void beatLog_LogImpl::SetMutexCallback(beatLog_MutexAcquireCallback _fpAcquireCallback, beatLog_MutexReleaseCallback _fpReleaseCallback)
{
  fpMutexAcquireCallback_ = _fpAcquireCallback;
  fpMutexReleaseCallback_ = _fpReleaseCallback;
}

#define beatLog_Const_MAXINDENT 512
#define beatLog_Const_MAXOUT 2048
#define beatLog_Const_MAXMESSAGE 2048
#define beatLog_Const_MAXCONTEXT 1024

static int beatLog_Const_MaxIndent = beatLog_Const_MAXINDENT;
static int beatLog_Const_MaxMessage = beatLog_Const_MAXMESSAGE;
static int beatLog_Const_MaxContext = beatLog_Const_MAXCONTEXT;


void beatLog_LogImpl::Log(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
{
  beatLog_MutexObject mutex;

  if (_nMask > beatLog_Interface::MaskNone && _nMask <= beatLog_Interface::MaskVeryVerbose) {

    const char* pszChannel = _pszChannel; if (pszChannel == 0) pszChannel = "-";
    const char* pszMessage = _pszMessage; if (pszMessage == 0) pszMessage = "-";

    char pszContext[beatLog_Const_MAXCONTEXT];
    char* p = pszContext;
    for (int i = 1; i < nTraceLevelDepth_ && (p-pszContext) + 1 < beatLog_Const_MaxIndent; i++) {
      *p++ = ' ';
      *p++ = ' ';
    }
    *p = '\0';

    if (_pszContext != 0) {
      int nContextLen = ::strlen(_pszContext);
      int nMemCpyLen = nContextLen + (p-pszContext) + 3 < beatLog_Const_MaxContext ? nContextLen : beatLog_Const_MaxContext - (p-pszContext) - 3;
      ::memcpy(p, _pszContext, nMemCpyLen);
      p += nMemCpyLen;
    }
    *p = '\0';

    if (fpOutCallback_ == beatLog_LogCallbackStore) {
      StoreLine(_nMask, pszChannel, pszContext, pszMessage);
    } else if (fpOutCallback_ != 0) {
      fpOutCallback_(_nMask, pszChannel, pszContext, pszMessage);
    }

  }
}

void beatLog_LogImpl::Format(char* _pszBuffer, int nBufferLen, int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
{
  const char* pszChannel = _pszChannel; if (pszChannel == 0) pszChannel = "-";
  const char* pszContext = _pszContext; if (pszContext == 0) pszContext = "-";
  const char* pszMessage = _pszMessage; if (pszMessage == 0) pszMessage = "-";

  int nLen = 0;
  const char* pszData = 0;
  char* p = _pszBuffer;

  pszData = pszLevelNames_[beatLog_LogImpl::Mask2Level(_nMask)];
  nLen = ::strlen(pszData); if (nLen > 0) { ::memcpy(p, pszData, nLen + (p-_pszBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_pszBuffer) - 3 ); p += nLen; } *p++ = ' ';

  pszData = pszChannel;
  nLen = ::strlen(pszData); if (nLen > 0) { ::memcpy(p, pszData, nLen + (p-_pszBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_pszBuffer) - 3 ); p += nLen; } *p++ = ' ';

  pszData = pszContext;
  nLen = ::strlen(pszData); if (nLen > 0) { ::memcpy(p, pszData, nLen + (p-_pszBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_pszBuffer) - 3 ); p += nLen; } *p++ = ' ';

  pszData = pszMessage;
  nLen = ::strlen(pszData); if (nLen > 0) { ::memcpy(p, pszData, nLen + (p-_pszBuffer) + 3 < nBufferLen ? nLen : nBufferLen - (p-_pszBuffer) - 3 ); p += nLen; } *p++ = ' ';

  *p++ = '\n';
  *p = '\0';
}

void beatLog_LogImpl::StoreLine(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
{
  beatLog_LogLine* pLogLine = new beatLog_LogLine(_nMask, _pszChannel, _pszContext, _pszMessage);
  if (pLogLine != 0) {
    if (pLogLineHead_ == 0) {
      pLogLineHead_ = pLogLine;
    } else {
      beatLog_LogLine* pLastLogLine = pLogLineHead_;
      while (pLastLogLine != 0) {
        beatLog_LogLine* pNextLogLine = pLastLogLine->Next();
        if (pNextLogLine == 0) {
          pLastLogLine->Next(pLogLine);
        }
        pLastLogLine = pNextLogLine;
      }
    }
  }
}

int beatLog_LogImpl::GetLineLength(int* _nChannel, int* _nContext, int* _nMessage)
{
  beatLog_MutexObject mutex;

  if (pLogLineHead_ == 0) {
    return 0;
  } else {
    *_nChannel = ::strlen(pLogLineHead_->Channel()) + 1;
    *_nContext = ::strlen(pLogLineHead_->Context()) + 1;
    *_nMessage = ::strlen(pLogLineHead_->Message()) + 1;
    return 1;
  }
}

int beatLog_LogImpl::GetLineData(int* _nMask, char* _pszChannel, char* _pszContext, char* _pszMessage)
{
  beatLog_MutexObject mutex;

  *_pszChannel = '\0';
  *_pszContext = '\0';
  *_pszMessage = '\0';

  if (pLogLineHead_ == 0) {
    return 0;
  } else {

    int nChannelLen = ::strlen(pLogLineHead_->Channel());
    ::memcpy(_pszChannel, pLogLineHead_->Channel(), nChannelLen);
    _pszChannel[nChannelLen] = '\0';

    int nContextLen = ::strlen(pLogLineHead_->Context());
    ::memcpy(_pszContext, pLogLineHead_->Context(), nContextLen);
    _pszContext[nContextLen] = '\0';

    int nMessageLen = ::strlen(pLogLineHead_->Message());
    ::memcpy(_pszMessage, pLogLineHead_->Message(), nMessageLen);
    _pszMessage[nMessageLen] = '\0';

    *_nMask = pLogLineHead_->Mask();

    DeleteLineHead();

    return 1;
  }
}

void beatLog_LogImpl::TraceEnter(const char* _pszChannel, const char* _pszContext)
{
  beatLog_MutexObject mutex;

  beatLog_TraceLevel* pTraceLevel = new beatLog_TraceLevel(_pszChannel, _pszContext);
  if (pTraceLevel != 0) {
    if (pTraceLevelHead_ != 0) {
      pTraceLevel->Next(pTraceLevelHead_);
    }
    pTraceLevelHead_ = pTraceLevel;
  }
  nTraceLevelDepth_++;
}

void beatLog_LogImpl::TraceLeave(void)
{
  beatLog_MutexObject mutex;

  nTraceLevelDepth_--;
  if (pTraceLevelHead_ != 0) {
    DeleteTraceHead();
  }
}

const char* beatLog_LogImpl::TraceChannel(void)
{
  const char* result = "";
  if (pTraceLevelHead_ != 0) {
    result = pTraceLevelHead_->Channel();
  }
  return result;
}

const char* beatLog_LogImpl::TraceContext(void)
{
  const char* result = "";
  if (pTraceLevelHead_ != 0) {
    result = pTraceLevelHead_->Context();
  }
  return result;
}

//----------------------------------------------------------

beatLog_LogLine::beatLog_LogLine(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
:pNext_(0)
,nMask_(0)
,pszChannel_(0)
,pszContext_(0)
,pszMessage_(0)
{
  nMask_ = _nMask;

  if (_pszChannel != 0) {
    int nChannelLen = ::strlen(_pszChannel);
    pszChannel_ = new char[nChannelLen+1];
    if (pszChannel_ != 0) {
      ::memcpy(pszChannel_, _pszChannel, nChannelLen+1);
    }
  }
  if (_pszContext != 0) {
    int nContextLen = ::strlen(_pszContext);
    pszContext_ = new char[nContextLen+1];
    if (pszContext_ != 0) {
      ::memcpy(pszContext_, _pszContext, nContextLen+1);
    }
  }
  if (_pszMessage != 0) {
    int nMessageLen = ::strlen(_pszMessage);
    pszMessage_ = new char[nMessageLen+1];
    if (pszMessage_ != 0) {
      ::memcpy(pszMessage_, _pszMessage, nMessageLen+1);
    }
  }
}

beatLog_LogLine::~beatLog_LogLine(void)
{
  if (pszChannel_ != 0) {
    delete [] pszChannel_;
  }
  if (pszContext_ != 0) {
    delete [] pszContext_;
  }
  if (pszMessage_ != 0) {
    delete [] pszMessage_;
  }
}

int beatLog_LogLine::Mask(void)
{
  return nMask_;
}

const char* beatLog_LogLine::Channel()
{
  return pszChannel_ ? pszChannel_ : "";
}

const char* beatLog_LogLine::Context()
{
  return pszContext_ ? pszContext_ : "";
}

const char* beatLog_LogLine::Message()
{
  return pszMessage_ ? pszMessage_ : "";
}

beatLog_LogLine* beatLog_LogLine::Next()
{
  return pNext_;
}

void beatLog_LogLine::Next(beatLog_LogLine* next)
{
  pNext_ = next;
}

//----------------------------------------------------------

beatLog_TraceLevel::beatLog_TraceLevel(const char* _pszChannel, const char* _pszContext)
:pNext_(0)
,pszChannel_(0)
,pszContext_(0)
{
  if (_pszChannel != 0) {
    unsigned long nLen = ::strlen(_pszChannel);
    pszChannel_ = new char[nLen+1];
    if (pszChannel_ != 0) {
      ::memcpy(pszChannel_, _pszChannel, nLen+1);
    }
  }
  if (_pszContext != 0) {
    unsigned long nLen = ::strlen(_pszContext);
    pszContext_ = new char[nLen+1];
    if (pszContext_ != 0) {
      ::memcpy(pszContext_, _pszContext, nLen+1);
    }
  }
}

beatLog_TraceLevel::~beatLog_TraceLevel(void)
{
  if (pszChannel_ != 0) {
    delete [] pszChannel_;
  }
  if (pszContext_ != 0) {
    delete [] pszContext_;
  }
}

const char* beatLog_TraceLevel::Channel()
{
  const char* result = "";
  if (pszChannel_ != 0) {
    result = pszChannel_;
  }
  return result;
}

const char* beatLog_TraceLevel::Context()
{
  const char* result = "";
  if (pszContext_ != 0) {
    result = pszContext_;
  }
  return result;
}

beatLog_TraceLevel* beatLog_TraceLevel::Next()
{
  return pNext_;
}

void beatLog_TraceLevel::Next(beatLog_TraceLevel* next)
{
  pNext_ = next;
}

beatLog_TraceObject::beatLog_TraceObject(const char* _pszChannel, const char* _pszContext)
{
  if (beatLog_LogInst.IsMask(beatLog_Interface::MaskTrace)) {
    beatLog_LogInst.TraceEnter(_pszChannel, _pszContext);
    beatLog_LogInst.Log(beatLog_Interface::MaskTrace, beatLog_LogInst.TraceChannel(), beatLog_LogInst.TraceContext(), "Entering");
  }
}

beatLog_TraceObject::~beatLog_TraceObject()
{
  if (beatLog_LogInst.IsMask(beatLog_Interface::MaskTrace)) {
    beatLog_LogInst.Log(beatLog_Interface::MaskTrace, beatLog_LogInst.TraceChannel(), beatLog_LogInst.TraceContext(), "Leaving");
    beatLog_LogInst.TraceLeave();
  }
}

//////////////////////////////////////////////////////////////////////

class beatLog_ConfigImpl
{
public:
	beatLog_ConfigImpl();
	virtual ~beatLog_ConfigImpl();

  static const char* ReadConfig(const char* _pszPath, const char* _pszDefault);
  static void WriteConfig(const char* _pszPath, const char* _pszValue);

  beatLog_ReadCallback SetReadCallback(beatLog_ReadCallback _fpCallback);
  beatLog_WriteCallback SetWriteCallback(beatLog_WriteCallback _fpCallback);

  const char* Read(const char* _pszPath, const char* _pszDefault);
  int Read(const char* _pszPath, int _nDefault);
  void Write(const char* _pszPath, const char* _pszValue);
  void Write(const char* _pszPath, int _nValue);

  const char* Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszDefault);
  int Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nDefault);
  void Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszValue);
  void Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nValue);

  static int Level2Path(char* _pszPath, int _nPathLen, const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem);

protected:
  beatLog_ReadCallback fpReadCallback_;
  beatLog_WriteCallback fpWriteCallback_;

  static const char* pszSep_;
};

//----------------------------------------------------------

beatLog_ConfigImpl beatLog_ConfigInst;
const char* beatLog_ConfigImpl::pszSep_ = beatLog_PathSep;

beatLog_ConfigImpl::beatLog_ConfigImpl()
:fpReadCallback_(&beatLog_ConfigImpl::ReadConfig)
,fpWriteCallback_(&beatLog_ConfigImpl::WriteConfig)
{
}

beatLog_ConfigImpl::~beatLog_ConfigImpl()
{
}

beatLog_ReadCallback beatLog_ConfigImpl::SetReadCallback(beatLog_ReadCallback _fpCallback)
{
  beatLog_ReadCallback fpOldCallback = fpReadCallback_;
  fpReadCallback_ = _fpCallback;
  return fpOldCallback;
}

beatLog_WriteCallback beatLog_ConfigImpl::SetWriteCallback(beatLog_WriteCallback _fpCallback)
{
  beatLog_WriteCallback fpOldCallback = fpWriteCallback_;
  fpWriteCallback_ = _fpCallback;
  return fpOldCallback;
}

#define beatLog_Const_MAXPATH 2048
static int beatLog_Const_MaxPath = beatLog_Const_MAXPATH;

int beatLog_ConfigImpl::Level2Path(char* _pszPath, int _nPathLen, const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem)
{
  int ok = 1;

  char* p = _pszPath;

  if (_pszLevel1 != 0) {
    const char* pszData = _pszLevel1;
    int nLen = ::strlen(pszData);
    if (nLen > 0) {
      ::memcpy(p, pszData, nLen + (p-_pszPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_pszPath) - 3 ); p += nLen;
      *p++ = beatLog_ConfigImpl::pszSep_[0];
    }
  }

  if (_pszLevel2 != 0) {
    const char* pszData = _pszLevel2;
    int nLen = ::strlen(pszData);
    if (nLen > 0) {
      ::memcpy(p, pszData, nLen + (p-_pszPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_pszPath) - 3 ); p += nLen;
      *p++ = beatLog_ConfigImpl::pszSep_[0];
    }
  }

  if (_pszItem != 0) {
    const char* pszData = _pszItem;
    int nLen = ::strlen(pszData);
    if (nLen > 0) {
      ::memcpy(p, pszData, nLen + (p-_pszPath) + 3 < _nPathLen ? nLen : _nPathLen - (p-_pszPath) - 3 ); p += nLen;
    }
  }

  *p = '\0';

  return ok;
}

const char* beatLog_ConfigImpl::Read(const char* _pszPath, const char* _pszDefault)
{
  const char* pszResult = _pszDefault;
  if (fpWriteCallback_ != 0) {
   const char* pszValue = fpReadCallback_(_pszPath, _pszDefault);
   if (pszValue != 0) {
     pszResult = pszValue;
   }
  }
  return pszResult;
}

int beatLog_ConfigImpl::Read(const char* _pszPath, int _nDefault)
{
  int nValue = _nDefault;
  const char* pszValue = Read(_pszPath, (const char*) 0);
  if (pszValue != 0) {
    nValue = ::atoi(pszValue);
  }
  return nValue;
}

void beatLog_ConfigImpl::Write(const char* _pszPath, const char* _pszValue)
{
  if (fpWriteCallback_ != 0) {
    fpWriteCallback_(_pszPath, _pszValue);
  }
}

void beatLog_ConfigImpl::Write(const char* _pszPath, int _nValue)
{
  char pszValue[100];
  beatLog_OS::snprintf(pszValue, 100, "%d", _nValue);
  Write(_pszPath, pszValue);
}

//////////////////////////////////////////////////////////////////////

beatLog_LogCallback beatLog_Interface::SetLogCallback(beatLog_LogCallback _fpCallback)
{
  return beatLog_LogInst.SetLogCallback(_fpCallback);
}

beatLog_ReadCallback beatLog_Interface::SetReadCallback(beatLog_ReadCallback _fpCallback)
{
  return beatLog_ConfigInst.SetReadCallback(_fpCallback);
}

beatLog_WriteCallback beatLog_Interface::SetWriteCallback(beatLog_WriteCallback _fpCallback)
{
  return beatLog_ConfigInst.SetWriteCallback(_fpCallback);
}

void beatLog_Interface::SetMutexCallback(beatLog_MutexAcquireCallback _fpAcquireCallback, beatLog_MutexReleaseCallback _fpReleaseCallback)
{
  beatLog_LogInst.SetMutexCallback(_fpAcquireCallback, _fpReleaseCallback);
}

int beatLog_Interface::SetMask(int _nMask)
{
  return beatLog_LogInst.SetMask(_nMask);
}

int beatLog_Interface::IsMask(int _nMask)
{
  return beatLog_LogInst.IsMask(_nMask);
}

int beatLog_Interface::GetLineLength(int* _nChannel, int* _nContext, int* _nMessage)
{
  return beatLog_LogInst.GetLineLength(_nChannel, _nContext, _nMessage);
}

int beatLog_Interface::GetLineData(int* _nMask, char* _pszChannel, char* _pszContext, char* _pszMessage)
{
  return beatLog_LogInst.GetLineData(_nMask, _pszChannel, _pszContext, _pszMessage);
}

void beatLog_Interface::Fatal(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskFatal)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskFatal, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Error(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskError)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskError, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Warning(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskWarning)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskWarning, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Debug(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskDebug)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskDebug, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Info(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskInfo)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskInfo, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Verbose(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskVerbose)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskVerbose, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Trace(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskTrace)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskTrace, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

void beatLog_Interface::Log(int level, const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(level)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(level, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Log", "format == 0"));
    }
  }
}

void beatLog_Interface::VeryVerbose(const char* _pszChannel, const char* _pszContext, const char *format, ...)
{
  if (beatLog_LogInst.IsMask(MaskVeryVerbose)) {
    if (format != 0) {
      char pszMessage[beatLog_Const_MAXMESSAGE]; pszMessage[0] = '\0';
      va_list argp;
      va_start(argp, format);
      beatLog_OS::vsnprintf(pszMessage, beatLog_Const_MaxMessage, format, argp);
      va_end(argp);
      beatLog_LogInst.Log(beatLog_Interface::MaskVeryVerbose, _pszChannel, _pszContext, pszMessage);
    } else {
      beatLog_Warning(("beatLog_Interface", "Info", "format == 0"));
    }
  }
}

//----------------------------------------------------------

const char* beatLog_Interface::Read(const char* _pszPath, const char* _pszDefault)
{
  return beatLog_ConfigInst.Read(_pszPath, _pszDefault);
}

int beatLog_Interface::Read(const char* _pszPath, int _nDefault)
{
  return beatLog_ConfigInst.Read(_pszPath, _nDefault);
}

void beatLog_Interface::Write(const char* _pszPath, const char* _pszValue)
{
  beatLog_ConfigInst.Write(_pszPath, _pszValue);
}

void beatLog_Interface::Write(const char* _pszPath, int _nValue)
{
  beatLog_ConfigInst.Write(_pszPath, _nValue);
}


const char* beatLog_Interface::Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszDefault)
{
  char pszPath[beatLog_Const_MAXPATH];
  if (beatLog_ConfigImpl::Level2Path(pszPath, beatLog_Const_MaxPath, _pszLevel1, _pszLevel2, _pszItem)) {
    return beatLog_ConfigInst.Read(pszPath, _pszDefault);
  } else {
    return _pszDefault;
  }
}

int beatLog_Interface::Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nDefault)
{
  char pszPath[beatLog_Const_MAXPATH];
  if (beatLog_ConfigImpl::Level2Path(pszPath, beatLog_Const_MaxPath, _pszLevel1, _pszLevel2, _pszItem)) {
    return beatLog_ConfigInst.Read(pszPath, _nDefault);
  } else {
    return _nDefault;
  }
}

void beatLog_Interface::Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszValue)
{
  char pszPath[beatLog_Const_MAXPATH];
  if (beatLog_ConfigImpl::Level2Path(pszPath, beatLog_Const_MaxPath, _pszLevel1, _pszLevel2, _pszItem)) {
    beatLog_ConfigInst.Write(pszPath, _pszValue);
  }
}

void beatLog_Interface::Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nValue)
{
  char pszPath[beatLog_Const_MAXPATH];
  if (beatLog_ConfigImpl::Level2Path(pszPath, beatLog_Const_MaxPath, _pszLevel1, _pszLevel2, _pszItem)) {
    beatLog_ConfigInst.Write(pszPath, _nValue);
  }
}

//////////////////////////////////////////////////////////////////////

void beatLog_LogImpl::ConsoleOut(int _nMask, const char* _pszChannel, const char* _pszContext, const char* _pszMessage)
{
  char pszOut[beatLog_Const_MAXOUT];
  beatLog_LogImpl::Format(pszOut, beatLog_Const_MAXOUT, _nMask, _pszChannel, _pszContext, _pszMessage);

  beatLog_OS::consoleout(pszOut);
}

const char* beatLog_ConfigImpl::ReadConfig(const char* _pszPath, const char* _pszDefault)
{
  const char* pszValue = _pszDefault;

  if (0) {
  } else if (strcmp(_pszPath, "") == 0) {
  }

  return pszValue;
}

void beatLog_ConfigImpl::WriteConfig(const char* _pszPath, const char* _pszValue)
{
  // do nothing
  // could store the pairs in a list here
}
