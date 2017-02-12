#include "StdAfx.h"
#include "MCIPlayMusic.h"
#pragma comment(lib,"WINMM.LIB")
#pragma comment(lib,"msimg32.lib")

CMCIPlayMusic::CMCIPlayMusic()  
{  
	m_nDeviceID=0;  
	m_nElementID=0;  
}  

CMCIPlayMusic::~CMCIPlayMusic()  
{  
	if(m_nElementID != 0)  
	{  
		stop();  
	}  
	if(m_nDeviceID != 0)  
	{  
		closeDevice();  
	}  
}  

DWORD CMCIPlayMusic::openDevice()  
{  
	DWORD dwResult=0;  

	if (m_nDeviceID == 0)  
	{  
		MCI_OPEN_PARMS mciOpenParms;  
		ZeroMemory(&mciOpenParms, sizeof(mciOpenParms));  

		//mciOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;  
		mciOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_SEQUENCER;  

		//open the wave device  
		dwResult = mciSendCommand(NULL,  
			MCI_OPEN,  
			MCI_OPEN_TYPE|MCI_OPEN_TYPE_ID|MCI_WAIT,  
			(DWORD)(LPVOID)&mciOpenParms);  

		//save device identifier,will use eith other MCI commands  
		m_nDeviceID = mciOpenParms.wDeviceID;  

	}  
	//return result of MCI operation  
	return dwResult;  
}  

DWORD CMCIPlayMusic::closeDevice()  
{  
	DWORD dwResult=0;  

	//close if currently open  
	if(m_nDeviceID)  
	{  
		//close the MCI device  
		dwResult=mciSendCommand(m_nDeviceID,MCI_CLOSE,NULL,NULL);  

		//display error message if failed  
		if(!dwResult)  
		{  
			m_nDeviceID=0;  
		}  
	}  

	//return result of MCI operation  
	return dwResult;  
}  

DWORD CMCIPlayMusic::play(CWnd* pWnd,LPCTSTR pFileName)  
{  
	if (m_nElementID == 0)  
	{  
		MCI_OPEN_PARMS mciOpenParms;  
		//initialize structure  
		memset(&mciOpenParms,0,sizeof(MCI_OPEN_PARMS));  

		//set the WAV file name to be played  
		mciOpenParms.lpstrElementName = pFileName;  

		//first open the device  
		DWORD dwResult = mciSendCommand(m_nDeviceID,MCI_OPEN,  
			MCI_OPEN_ELEMENT,(DWORD)(LPVOID)&mciOpenParms);  

		if (dwResult != 0)  
		{  
			return dwResult;  
		}  
		else  
		{  
			//save element indentifier  
			m_nElementID = mciOpenParms.wDeviceID;  

		}  
	}  

	MCI_PLAY_PARMS mciPlayParms;  

	//set the window that will receive notification message  
	mciPlayParms.dwCallback = (DWORD)pWnd->m_hWnd;  

	//instruct device to play file  
	DWORD dwResult=mciSendCommand(m_nElementID,MCI_PLAY,  
		MCI_NOTIFY,(DWORD)(LPVOID)&mciPlayParms);  

	//display error and close element if failed  
	if(dwResult != 0)//Ê§°Ü  
	{  
		stop();  
	}  

	//return result of MCI operation  
	return dwResult;  
}  


DWORD CMCIPlayMusic::stop()  
{  
	DWORD dwResult=0;  

	//close if element is currently open  
	if(m_nElementID != 0)  
	{  
		dwResult=mciSendCommand(m_nElementID,MCI_CLOSE,NULL,NULL);  

		//display error message if failed  
		if(dwResult == 0)  
		{  
			m_nElementID=0;  
		}  
	}  
	return dwResult;  
}  

DWORD CMCIPlayMusic::pause()
{
	DWORD dwResult=0;  

	//close if element is currently open  
	if(m_nElementID != 0)  
	{  
		dwResult = mciSendCommand(m_nElementID, MCI_PAUSE, NULL, NULL);  
	}  

	return dwResult;  
}


DWORD CMCIPlayMusic::seekToStart()  
{  
	DWORD dwResult=0;  

	//close if element is currently open  
	if(m_nElementID != 0)  
	{  
		MCI_SEEK_PARMS seekParam;  
		ZeroMemory(&seekParam, sizeof(seekParam));  
		dwResult=mciSendCommand(m_nElementID,  
			MCI_SEEK,  
			MCI_SEEK_TO_START,  
			(DWORD)&seekParam);  
	}  
	return dwResult;  
}  

LPCTSTR CMCIPlayMusic::getErrorMsg(DWORD dwError)  
{  
	//character string that contains error message  
	static TCHAR szErrorMsg[MAXERRORLENGTH];  

	//check if there was an error  
	if(dwError)  
	{  
		//retrieve string associated error message  
		if(!mciGetErrorString(dwError,szErrorMsg,sizeof(szErrorMsg)))  
		{  
			lstrcpy(szErrorMsg,_T("Î´Öª´íÎó¡£"));  
		}  
	}  
	else  
	{  
		lstrcpy(szErrorMsg, _T("Ã»ÓÐ´íÎó¡£"));  
	}  
	return szErrorMsg;  
}     
