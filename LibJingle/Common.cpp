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


CString GetMoudleConfigIniPath()
{
	CString strFileName = _T("");
	AfxGetModuleFileName(theApp.m_hInstance, strFileName);
	int nPos = strFileName.Find(_T(".exe"));
	if (nPos != -1)
	{
		strFileName.Delete(nPos, 4);
		strFileName.Append(_T(".xml"));
	}

	return strFileName;
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
