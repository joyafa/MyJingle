/**  
* @file MCIPlayMusic.h  
* @author ������ 
* @mail you_lan_hai@foxmail.com 
* @data 2011-11-30 
* ˵���� 
* ��װ���硶�β��Ŵ��� WAV �ļ�����  
* ԭ���ߣ�hermess 
* http://www.vckbase.com/document/viewdoc/?id=633 
*/  
#pragma once  

#include <MMSystem.h>  

/**����������*/  
class CMCIPlayMusic  
{  
public:  
	///���캯��  
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
