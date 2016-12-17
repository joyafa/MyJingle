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

#if !defined(beatLog_Component_h_INCLUDED)
#define beatLog_Component_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
// This is the interface to be used:

#define beatLog_Fatal(_args_) beatLog_Interface::Fatal _args_
#define beatLog_Error(_args_) beatLog_Interface::Error _args_
#define beatLog_Warning(_args_) beatLog_Interface::Warning _args_
#define beatLog_Debug(_args_) beatLog_Interface::Debug _args_
#define beatLog_Info(_args_) beatLog_Interface::Info _args_
//#define beatLog_Fatal(_args_)
//#define beatLog_Error(_args_)
//#define beatLog_Warning(_args_)
//#define beatLog_Debug(_args_)
//#define beatLog_Info(_args_)

#ifdef _DEBUG
  #define beatLog_Verbose(_args_) beatLog_Interface::Verbose _args_
  #define beatLog_Trace(_args_) beatLog_TraceObject beatLog_TraceInstance _args_
  #define beatLog_VeryVerbose(_args_) beatLog_Interface::VeryVerbose _args_
  #define beatLog_IsVerbose beatLog_Interface::IsMask(beatLog_Interface::MaskVerbose)
  #define beatLog_IsVeryVerbose beatLog_Interface::IsMask(beatLog_Interface::MaskVeryVerbose)
#else
  //#define beatLog_Verbose(_args_) beatLog_Interface::Verbose _args_
  //#define beatLog_Trace(_args_) beatLog_TraceObject beatLog_TraceInstance _args_
  //#define beatLog_VeryVerbose(_args_) beatLog_Interface::VeryVerbose _args_
  //#define beatLog_IsVerbose beatLog_Interface::IsMask(beatLog_Interface::MaskVerbose)
  //#define beatLog_IsVeryVerbose beatLog_Interface::IsMask(beatLog_Interface::MaskVeryVerbose)
// Compile out for release:
  #define beatLog_Verbose(_args_)
  #define beatLog_Trace(_args_)
  #define beatLog_VeryVerbose(_args_)
  #define beatLog_IsVerbose 0
  #define beatLog_IsVeryVerbose 0
#endif

#define beatLog_SetMask(_mask_) beatLog_Interface::SetMask(_mask_)

// Use these...
#define beatLog_ConfigRead(_path_, _default_) beatLog_Interface::Read(_path_, _default_)
#define beatLog_ConfigWrite(_path_, _value_) beatLog_Interface::Write(_path_, _value_)
// ...or these
//#define beatLog_ConfigRead(_level1_, _level2_, _item_, _default_) beatLog_Interface::Read(_level1_, _level2_, _item_, _default_)
//#define beatLog_ConfigWrite(_level1_, _level2_, _item_, _value_) beatLog_Interface::Write(_level1_, _level2_, _item_, _value_)

//////////////////////////////////////////////////////////////////////
// Usage:

//  beatLog_Trace(("CTestModule", "Method"));
//  beatLog_Info(("CTestModule", "Method", "A message with args: %s, %d", "String", 42));
//  if (beatLog_IsVeryVerbose) {}
//  const char* param = beatLog_ConfigRead("Path", "DefaultString");
//  int param = beatLog_ConfigRead(CONFIG_LEVEL1 "/" CONFIG_LEVEL2 "/" "Item", 42);
//  beatLog_ConfigWrite("Path", "Value");
//  beatLog_ConfigWrite(CONFIG_LEVEL1 "/" CONFIG_LEVEL2 "/" "Item", 42);

//  void logfunction(int level, const char* channel, const char* context, const char* message) {
//    printf("%d %s %s %s\n", level, channel, context, message);
//  }
//  beatLog_Interface::SetLogCallback((void*) logfunction);



//////////////////////////////////////////////////////////////////////
// This also needs to be here:

typedef void (*beatLog_LogCallback) (int _nLevel, const char* _pszChannel, const char* _pszContext, const char* _pszMessage);
typedef const char* (*beatLog_ReadCallback) (const char* _pszPath, const char* _pszDefault);
typedef void (*beatLog_WriteCallback) (const char* _pszPath, const char* _pszValue);
#define beatLog_PathSep "/"
#define beatLog_LogCallbackStore ((beatLog_LogCallback) 1)
#define beatLog_LogMaskDefault (-2)
typedef void (*beatLog_MutexAcquireCallback) (void);
typedef void (*beatLog_MutexReleaseCallback) (void);

class beatLog_Interface
{
public:
  static void Fatal(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Error(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Warning(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Debug(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Info(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Verbose(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Trace(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void VeryVerbose(const char* _pszChannel, const char* _pszContext, const char *fmt, ...);
  static void Log(int level, const char* _pszChannel, const char* _pszContext, const char *fmt, ...);

  static beatLog_LogCallback SetLogCallback(beatLog_LogCallback _fpCallback);
  static beatLog_ReadCallback SetReadCallback(beatLog_ReadCallback _fpCallback);
  static beatLog_WriteCallback SetWriteCallback(beatLog_WriteCallback _fpCallback);
  static void SetMutexCallback(beatLog_MutexAcquireCallback _fpAcquireCallback, beatLog_MutexReleaseCallback _fpReleaseCallback);

  static int GetLineLength(int* _nChannel, int* _nContext, int* _nMessage);
  static int GetLineData(int* _nMask, char* _pszChannel, char* _pszContext, char* _pszMessage);

  static int SetMask(int _nMask);
  static int IsMask(int _nMask);

  enum LogMask {
     MaskNone =        (1 << 0)

    ,MaskFatal       = (1 << 1) // 2
    ,MaskError       = (1 << 2) // 4
    ,MaskWarning     = (1 << 3) // 8
    ,MaskDebug       = (1 << 4) // 16
    ,MaskInfo        = (1 << 5) // 32
    ,MaskVerbose     = (1 << 6) // 64
    ,MaskTrace       = (1 << 7) // 128
    ,MaskVeryVerbose = (1 << 8) // 256

    ,NLogLevels      = 9

    ,MaskAll         = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo|MaskVerbose|MaskTrace|MaskVeryVerbose
    ,MaskMaxVerbose  = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo|MaskVerbose
    ,MaskMaxInfo     = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo
    ,MaskMaxError    = MaskFatal|MaskError|MaskWarning
    ,MaskSilent      = 0
    ,MaskNoTrace     = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo|MaskVerbose          |MaskVeryVerbose
    ,MaskDefault     = MaskFatal|MaskError|MaskWarning|MaskDebug|MaskInfo
  };

  static const char* Read(const char* _pszPath, const char* _pszDefault);
  static int Read(const char* _pszPath, int _nDefault);
  static void Write(const char* _pszPath, const char* _pszValue);
  static void Write(const char* _pszPath, int _nValue);

  static const char* Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszDefault);
  static int Read(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nDefault);
  static void Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, const char* _pszValue);
  static void Write(const char* _pszLevel1, const char* _pszLevel2, const char* _pszItem, int _nValue);
};

class beatLog_TraceObject
{
public:
	beatLog_TraceObject(const char* _pszChannel, const char* _pszContext);
	virtual ~beatLog_TraceObject();
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(beatLog_Component_h_INCLUDED)
