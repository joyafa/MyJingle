/**  
* @file MCIPlayMusic.h  
* @author 游蓝海 
* @mail you_lan_hai@foxmail.com 
* @data 2011-11-30 
* 说明： 
* 改装自如《何播放大型 WAV 文件？》  
* 原作者：hermess 
* http://www.vckbase.com/document/viewdoc/?id=633 
*/  
#pragma once  

#include <MMSystem.h>  

/**播放音乐类*/  
class CMCIPlayMusic  
{  
public:  
	///构造函数  
	CMCIPlayMusic();  
	virtual ~CMCIPlayMusic();  

public:  
	DWORD openDevice();  
	DWORD closeDevice();  
	DWORD play(CWnd *pParentWnd,LPCTSTR pFileName);  
	DWORD stop();  
	DWORD pause();  
	DWORD seekToStart();  

	LPCTSTR getErrorMsg(DWORD dwError);  

	MCIDEVICEID getDeviceID(void){ return m_nDeviceID; }  
	MCIDEVICEID getElementID(void){ return m_nElementID; }  

private:  
	MCIDEVICEID m_nDeviceID;  
	MCIDEVICEID m_nElementID;  
};  
