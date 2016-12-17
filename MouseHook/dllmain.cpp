// dllmain.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#include "msgh.h"

#pragma data_seg("ShareData")
HHOOK     g_Hook = NULL;
HWND     g_Hwnd = NULL;
DWORD    g_dwMsg = 0;
HINSTANCE g_hHINS = NULL;
#pragma data_seg()

static AFX_EXTENSION_MODULE MouseHookDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	g_hHINS = (HINSTANCE)hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("MouseHook.DLL ���ڳ�ʼ��!\n");
		
		// ��չ DLL һ���Գ�ʼ��
		if (!AfxInitExtensionModule(MouseHookDLL, hInstance))
			return 0;

		// ���� DLL ���뵽��Դ����
		// ע��: �������չ DLL ��
		//  MFC ���� DLL (�� ActiveX �ؼ�)��ʽ���ӵ���
		//  �������� MFC Ӧ�ó������ӵ�������Ҫ
		//  �����д� DllMain ���Ƴ������������һ��
		//  �Ӵ���չ DLL �����ĵ����ĺ����С�ʹ�ô���չ DLL ��
		//  ���� DLL Ȼ��Ӧ��ʽ
		//  ���øú����Գ�ʼ������չ DLL������
		//  CDynLinkLibrary ���󲻻ḽ�ӵ�
		//  ���� DLL ����Դ���������������ص�
		//  ���⡣

		new CDynLinkLibrary(MouseHookDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("MouseHook.DLL ������ֹ!\n");

		// �ڵ�����������֮ǰ��ֹ�ÿ�
		AfxTermExtensionModule(MouseHookDLL);
	}
	return 1;   // ȷ��
}


LRESULT CALLBACK MouseProc(
	int nCode,      // hook code
	WPARAM wParam,  // message identifier
	LPARAM lParam   // mouse coordinates
	)
{
    if (HC_ACTION == nCode)
	{
		if (WM_LBUTTONDOWN == wParam || 
			WM_RBUTTONDOWN == wParam)
		{
			SendMessage(g_Hwnd, g_dwMsg, wParam, lParam);
			
			//CallNextHookEx(g_Hook, nCode, wParam, lParam);
		}
	}

	return CallNextHookEx(g_Hook, nCode, wParam, lParam);

}

#define MSGH_API __declspec(dllexport)

extern "C" MSGH_API void installMouseHook(HWND hWnd, DWORD dwMsg)
{
	g_Hwnd  = hWnd;
	g_dwMsg = dwMsg;
	g_Hook  = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, g_hHINS, 0);
}
