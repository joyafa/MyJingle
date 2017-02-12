#include "stdafx.h"
#include "LibJingle.h"
#include <windows.h>
#include <afxpriv.h>
#include <atlconv.h>

using namespace std;

extern CLibJingleApp theApp;

CString GetMoudlePath()
{
	CString strFileName;
	AfxGetModuleFileName(theApp.m_hInstance, strFileName);
	int nPos = strFileName.ReverseFind('\\');
	strFileName.Delete(nPos, strFileName.GetLength() - nPos);

	return strFileName;
}


CString GetMoudleConfigPath()
{
	CString strConfigPath = _T("");
	AfxGetModuleFileName(theApp.m_hInstance, strConfigPath);
	int nPos = strConfigPath.ReverseFind(_T('\\'));
	if (nPos != -1)
	{
		strConfigPath.Delete(nPos + 1, strConfigPath.GetLength() - nPos - 1);
	}
	strConfigPath += _T("etc\\");

	return strConfigPath;
}


CString GetMoudleResPath()
{
	CString strConfigPath = _T("");
	AfxGetModuleFileName(theApp.m_hInstance, strConfigPath);
	int nPos = strConfigPath.ReverseFind(_T('\\'));
	if (nPos != -1)
	{
		strConfigPath.Delete(nPos + 1, strConfigPath.GetLength() - nPos - 1);
	}
	strConfigPath += _T("res\\");

	return strConfigPath;
}

CString GetMoudleConfigFileName()
{
	CString strConfigFileName = _T("");
	AfxGetModuleFileName(theApp.m_hInstance, strConfigFileName);
	int nPos = strConfigFileName.Find(_T(".exe"));
	if (nPos != -1)
	{
		strConfigFileName.Delete(nPos, 4);
		strConfigFileName.Append(_T(".xml"));

		int nPos = strConfigFileName.ReverseFind(_T('\\'));
		if (nPos != -1)
		{
			strConfigFileName.Delete(0, nPos + 1);
		}
	}

	return strConfigFileName;
}


//*******************************************************************
// Method:      GetIPAddress
// Returns:     std::string
// Description: 获取IP地址
// ModifyTime:  2015-11-12 16:29:09
// Author:      
//*******************************************************************
std::string GetIPAddress()
{
	char szhostname[128] = {0};
	char szIPAddress[64] = {0};
	//获取名字
    if (gethostname(szhostname, 128) == 0)
    {
        // get host adresses
        CString addr = _T("");
		//通过名字获取地址
        struct hostent *phost = gethostbyname(szhostname);

        for (int i = 0; (phost && phost->h_addr_list[i]); i++)
        {
            for (int j = 0; j < phost->h_length; j++)
            {
                szIPAddress[j] = phost->h_addr_list[i][j];
            }
            break;
        }
    }

	char buf[64];
	sprintf(buf, "%d.%d.%d.%d", (BYTE)szIPAddress[0], (BYTE)szIPAddress[1], (BYTE)szIPAddress[2], (BYTE)szIPAddress[3]);
	string strIpAddress(buf);

	return strIpAddress;
}


int UTF82Unicode(LPCSTR pInput, int nInputLen, WCHAR **ppOutPut, int* nOutputLen)
{
	if (NULL == (*ppOutPut))
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pInput, nInputLen, NULL, 0);
		*ppOutPut = new WCHAR[nLen + 1];
		ASSERT(NULL != *ppOutPut);
		(*ppOutPut)[nLen] = '\0';
		*nOutputLen = nLen + 1;
	}else
	{
		memset((*ppOutPut), 0, *nOutputLen);
	}

	return MultiByteToWideChar(CP_UTF8, 0, pInput, nInputLen, *ppOutPut, *nOutputLen);
}

CString UTF82Unicode(LPCSTR pUtf8)
{
	if (NULL == pUtf8)
	{
		return CString(L"");
	}

	WCHAR* pOut = NULL;
	int    nOut = 0;

	UTF82Unicode(pUtf8, strlen(pUtf8), &pOut, &nOut);

	CString strTemp = pOut;
	delete[] pOut;

	return strTemp;
}


string CString2String(const CString &str)
{
	USES_CONVERSION;
	return (string)W2CA((LPCWSTR)str);
}


int Unicode2UTF8(LPCWSTR pInput, int nInputLen, char **ppOutPut, int* nOutputLen)
{
	if (NULL == (*ppOutPut))
	{
		int nLen = WideCharToMultiByte(CP_UTF8, 0, pInput, nInputLen, NULL, 0, NULL, 0);
		*ppOutPut = new char[nLen + 1];
		ASSERT(NULL != *ppOutPut);
		(*ppOutPut)[nLen] = '\0';
		*nOutputLen = nLen + 1;
	}

	return WideCharToMultiByte(CP_UTF8, 0, pInput, nInputLen, *ppOutPut, *nOutputLen, NULL, 0);

}

CStringA Unicode2UTF8(LPCWSTR pInput)
{
	char * tmp=NULL;
	int i=0;
	Unicode2UTF8(pInput,_tcslen(pInput),&tmp,&i);
	CStringA tmpstr=tmp;
	delete tmp;
	return tmpstr;
}