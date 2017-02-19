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

// LibJingleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibJingle.h"
#include "LibJingleDlg.h"
#include "FromJingleMessageBase.h"
#include "beat/beatLog_.h"
#include "afxcmn.h"
#include "CryptoService.h"
#include <afxpriv.h>
#include <mmsystem.h>
#include "Common.h"
#include "SQLConnector.h"
#include "Config.h"
#include <string>
#include <winbase.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_cLicence;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CString strFileName;
	AfxGetModuleFileName(theApp.m_hInstance, strFileName);
	int nPos = strFileName.ReverseFind('\\');
	strFileName.Delete(nPos, strFileName.GetLength() - nPos);

	BOOL bRet = ((strFileName + _T("\\alerting.mp3")).GetBuffer(), SND_ASYNC);
	if (bRet)
	{
		int nErr = GetLastError();
		nErr;
	}

	CDialog::OnInitDialog();

	CFile licence;
	if( 0 != licence.Open(_T(".\\Licence.rtf"), CFile::modeRead, 0))
	{
		#define CABOUT_BUFLEN 100000
		char buffer[CABOUT_BUFLEN];
		int charsRead = licence.Read(buffer, CABOUT_BUFLEN-1);
		if (charsRead > 0)
		{
			buffer[charsRead] = _T('\0');
			::SetWindowTextA(m_cLicence.m_hWnd, buffer);
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



// CLibJingleDlg dialog

CLibJingleDlg::CLibJingleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLibJingleDlg::IDD, pParent)
	, m_lDebug(_T(""))
	, m_pCallDialog(NULL)
    , m_pCallCommingDialog(NULL)
	, m_usbDevice(0x258A, 0x001B)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLibJingleDlg::~CLibJingleDlg()
{
	if(m_JingleMain.Running())
	{
		m_JingleMain.JingleAdapter().Message().Type(ToJingleMessage::EXIT);
		m_JingleMain.JingleAdapter().SetEvent();
		m_JingleMain.Stop();
	}
}

void CLibJingleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LROSTER, m_lRoster);
	DDX_Control(pDX, IDC_JID, m_sJid);
	DDX_Text(pDX, IDC_DEBUG, m_lDebug);
	DDX_Control(pDX, IDC_DEBUGSTATUS, m_bDebugStatus);
	DDX_Control(pDX, IDC_DEBUGBEAT, m_bDebugBeat);
	DDX_Control(pDX, IDC_DEBUGXML, m_bDebugXML);
	DDX_Control(pDX, IDC_JID2, m_cJid);
	DDX_Control(pDX, IDLOGIN, m_bLogin);
	DDX_Control(pDX, IDLOGOUT, m_bLogout);
	DDX_Control(pDX, IDCALL, m_bCall);
	DDX_Control(pDX, IDHANGUP, m_bHangup);
	DDX_Control(pDX, IDC_Passwd, m_cPasswd);
    DDX_Control(pDX, IDSENDMSG, m_bSendmsg);
    DDX_Control(pDX, IDC_MSG_INPUT, m_cChatMsg);
}

#define WM_SHOWTASK WM_USER  + 1 
#define WM_STOPMUSIC WM_USER + 2
#define WM_MAX_SHOW (WM_USER + 3)

NOTIFYICONDATA nid; 


BEGIN_MESSAGE_MAP(CLibJingleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND() 

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP, &CLibJingleDlg::OnWM_APP)
	
	ON_BN_CLICKED(IDCALL, &CLibJingleDlg::OnBnClickedCall)
	ON_BN_CLICKED(WM_MAX_SHOW, &CLibJingleDlg::OnBnClickedShowMax)
	ON_BN_CLICKED(IDHANGUP, &CLibJingleDlg::OnBnClickedHangup)
	ON_LBN_SELCHANGE(IDC_LROSTER, &CLibJingleDlg::OnLbnSelchangeLroster)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDLOGIN, &CLibJingleDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDLOGOUT, &CLibJingleDlg::OnBnClickedLogout)
	ON_WM_QUERYDRAGICON() 
	ON_MESSAGE(WM_SHOWTASK,OnShowTask) 
	//	ON_EN_CHANGE(IDC_JID2, &CLibJingleDlg::OnEnChangeJid2)
	ON_MESSAGE(MM_MCINOTIFY, &CLibJingleDlg::OnMCINotify)
	ON_MESSAGE(WM_STOPMUSIC, &CLibJingleDlg::OnStopMusic)
	ON_MESSAGE(WM_PHONE,     &CLibJingleDlg::OnHandlePhone)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDSENDMSG, &CLibJingleDlg::OnBnClickedSendmsg)
END_MESSAGE_MAP()


// CLibJingleDlg message handlers

BOOL CLibJingleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    CenterWindow(GetDesktopWindow());

	AfxInitRichEdit2();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_bDebugBeat.SetCheck(BST_CHECKED);
	m_bDebugStatus.SetCheck(BST_CHECKED);
	m_bDebugXML.SetCheck(BST_UNCHECKED);

	GetConfigInfo();
	//数据库中读取登录用户名和密码
	m_AccountInfo = GetUserPasswordFromServer();
	CString strAccountCode(m_AccountInfo.strAccount.c_str());
	m_cJid.SetWindowText(strAccountCode);
	CString strPassword(m_AccountInfo.strPassword.c_str());
	m_cPasswd.SetWindowText(strPassword);


	//运行模式，客户端模式界面最小化
	beatLog_Info(("CLibJingleDlg", __FUNCTION__, "RunningMode:%s", m_strRunningMode.c_str()));
	if ("ADMIN" != m_strRunningMode)
	{
		PostMessage(WM_CLOSE, 0, 0);
	}

	//主叫对话框,
	m_pCallDialog = new CCallDialog;
	m_pCallDialog->Create(IDD_CALL_DIALOG, this);
	m_pCallDialog->ShowWindow(SW_HIDE);

	//被叫对话框
	m_pCallCommingDialog = new CCallCommingDialog;
	m_pCallCommingDialog->Create(IDD_CALLCOMMING_DIALOG, this);
	m_pCallCommingDialog->ShowWindow(SW_HIDE);

	//被叫事件
	m_hAcceptCallEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//接听事件
	m_hAcceptCallEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//挂断事件

	//主叫事件
	m_hDialEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//主叫事件
	m_hDialEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//主动挂断事件
	m_hDialEvents[2] = CreateEventA(NULL, TRUE, FALSE, NULL);//被动挂断事件(对方挂断事件)

	//打开硬件设备
	m_usbDevice.SetOwner(m_hWnd);
	m_usbDevice.ConnectDevice();
	m_usbDevice.ReadData();
	return TRUE;  // return TRUE  unless you set the focus to a control
}


LRESULT CLibJingleDlg::OnMCINotify(WPARAM wParam, LPARAM lParam)  
{  
	if (wParam == MCI_NOTIFY_SUCCESSFUL )  
	{  
		//呼叫
		if(DIALING == m_callStatus)
		{
			PlaySound(m_strPathDialingBell);
		}
		//接听
		else if ( ACCEPTING == m_callStatus)
		{
			PlaySound(m_strPathIncommingBell);
		}
		//忙音
		else
		{
			PlaySound(m_strPathBusyBell);
		}
	}  
	return 0;  
}  

LRESULT CLibJingleDlg::OnStopMusic( WPARAM wParam, LPARAM lParam )
{
	m_mciMusic.stop();

	return 1;
}


LRESULT CLibJingleDlg::OnHandlePhone( WPARAM wParam, LPARAM lParam )
{
	HardwareEventType event = (HardwareEventType)wParam;

	switch (event)
	{
		//呼叫
	case LEFT_KEY:
		//根据当前状态,做相关处理
		break;
		//挂断
	case RIGHT_KEY:
		//主叫方通话中
		if (DIALSIE_ONLINE == m_callStatus )
		{
			//挂断
			//SetEvent();
		}
		else if (ACCEPTSIE_ONLINE == m_callStatus )
		{
			//挂断
			//SetEvent();
		}
		break;
	}

	return 1;
}

void CLibJingleDlg::GetConfigInfo()
{
	CString strConfigPath = GetMoudleConfigPath() + GetMoudleConfigFileName();
	string strResPath = CString2String(GetMoudleResPath());
	if (LOADED_SUCCESS == CConfig::GetInstance().LoadConfigFile(CString2String(strConfigPath).c_str() ))
	{
		//运行模式
		m_strRunningMode   = CConfig::GetInstance().GetString("RunningMode");

		//来电铃声
		string strNameIncommingBell = CConfig::GetInstance().GetString("IncommingBell");
		m_strPathIncommingBell = strResPath + strNameIncommingBell;
		//忙音
		string strNameBusyBell      = CConfig::GetInstance().GetString("BusyBell");
		m_strPathBusyBell = strResPath + strNameBusyBell;
		//呼叫
		string	strNameDialingBell   = CConfig::GetInstance().GetString("DialingBell");
		m_strPathDialingBell = strResPath + strNameDialingBell;
	}
}


AccountInfo CLibJingleDlg::GetUserPasswordFromServer()
{
	AccountInfo info;
	CString strConfigPath = GetMoudleConfigPath() + GetMoudleConfigFileName();
	if (LOADED_SUCCESS == CConfig::GetInstance().LoadConfigFile(CString2String(strConfigPath).c_str() ))
	{
		do//数据库信息
		{
			string strDbName;
			string strHost;
			int    nPort = 3306;
			string strUsrName;
			string strPassword;

			try
			{
			strDbName   = CConfig::GetInstance().GetString("DBNAME");
			strHost     = CConfig::GetInstance().GetString("HOST");
			nPort       = CConfig::GetInstance().GetInt("PORT");
			strUsrName  = CConfig::GetInstance().GetString("USRNAME");
			strPassword = CConfig::GetInstance().GetString("PASSWORD");
			}
			catch(std::exception &e)
			{
				beatLog_Error(("CLibJingleDlg", __FUNCDNAME__, "An Exception has occured while getting user info from config file!"));
				break;
			}
			//连接数据库
			//TODO:为什么这里不需要端口,缺省只能是3306??
			CSQLConnector dbConnector;
			bool bRet = dbConnector.Connect(strHost, nPort, strDbName, strUsrName, strPassword);
			if (!bRet)
			{
				beatLog_Error(("CLibJingleDlg", __FUNCDNAME__, "An Exception has occured while connecting the mysql server!"));
				break;
			}
			//TODO:链接成功,取登录用户名和密码
			info = dbConnector.GetAccountInfo(GetIPAddress());
			info.strDomain = CConfig::GetInstance().GetString("DOMAIN");
		}while(false);
	}
	return info;
}


CString CLibJingleDlg::GetMoudlePath()
{
	CString strFileName;
	AfxGetModuleFileName(theApp.m_hInstance, strFileName);
	int nPos = strFileName.ReverseFind('\\');
	strFileName.Delete(nPos, strFileName.GetLength() - nPos);
	return strFileName;
}

LRESULT CLibJingleDlg::OnShowTask(WPARAM wParam,LPARAM lParam) 
{ 
	if (wParam != IDR_MAINFRAME) 
	{ 
		return 1; 
	} 
	switch(lParam) 
	{ 
	case WM_RBUTTONUP: 
		{ 
			LPPOINT lpoint = new tagPOINT; 
			::GetCursorPos(lpoint); 
			CMenu menu; 
			menu.CreatePopupMenu(); 
			menu.AppendMenu(MF_STRING,WM_MAX_SHOW, _T("最大化")); 
			menu.AppendMenu(MF_STRING,WM_DESTROY, _T("退出")); 
			menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this); 
			HMENU hmenu = menu.Detach(); 
			menu.DestroyMenu(); 
			delete lpoint; 
			this->DestroyWindow(); 
		} 
		break; 
	case WM_LBUTTONDBLCLK: 
		{ 
			this->ShowWindow(SW_SHOW); 
			Shell_NotifyIcon(NIM_DELETE,&nid); 
		} 
		break; 
	case WM_MAX_SHOW:
		this->ShowWindow(SW_SHOW); 
		Shell_NotifyIcon(NIM_DELETE,&nid); 
		break;

	} 
	return 0; 
} 


void CLibJingleDlg::OnBnClickedShowMax()
{
	this->ShowWindow(SW_SHOW); 
	Shell_NotifyIcon(NIM_DELETE,&nid); 
}


UINT __stdcall AcceptCallFunc(void *pvoid)
{
	_tagJidFrom *pFrom = (_tagJidFrom *)pvoid;
	CLibJingleDlg *pDlag = (CLibJingleDlg *)pFrom->pWnd;
	if (NULL== pDlag) return 0;

	//电话60s不接听，认为超时，关闭铃声
	//TODO：是否需要通过配置文件设置
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hAcceptCallEvents, FALSE, 60 * 1000);
	if (dwRet ==  WAIT_OBJECT_0)//接听事件
	{
		//直接停止不行,需要发送消息
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//pDlag->StopPlay();
		//重置为无信号状态
		//调用 SetEvent设置有信号之后,需要调用reset设置为无信号
		ResetEvent(pDlag->m_hAcceptCallEvents[0]);

		//TODO： 
		pDlag->m_callStatus = ACCEPTSIE_ONLINE;
		//接听
		pDlag->GetJingleMain().JingleAdapter().Message().SetJidAndType(pFrom->sJid.GetBuffer(), ToJingleMessage::ACCEPT_CALL);
		pDlag->GetJingleMain().JingleAdapter().SetEvent();
	}
	else  //挂断事件,超时停止 或 异常 都停止
	{
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//1. 超时或者是拒绝进接听
		pDlag->GetJingleMain().JingleAdapter().Message().SetJidAndType(pFrom->sJid.GetBuffer(), ToJingleMessage::REJECT_CALL);
		pDlag->GetJingleMain().JingleAdapter().SetEvent();
		//2. 接听过程中,需要挂断
		if (dwRet ==  WAIT_OBJECT_0 + 1)
		{
			//设置为无信号
			ResetEvent(pDlag->m_hAcceptCallEvents[1]);
		}
	}

	return 1;
}

void CLibJingleDlg::PlaySound(const std::string &strSonndPath)
{
	//先停掉原有的声音
	m_mciMusic.stop();

	DWORD dwResult = m_mciMusic.play(this, CString(strSonndPath.c_str()) );  
	if (dwResult != 0)  
	{  
		beatLog_Error(("CLibJingleDlg", __FUNCDNAME__, "Play sound failed: %s", m_mciMusic.getErrorMsg(dwResult)));
	}  
}



bool CLibJingleDlg::AcceptCallFrom(const char* jid)
{
	//接听状态
	m_callStatus = ACCEPTING;

	bool acceptCall(false);
	CString cJid(jid);
	CString message(_T("您有新的来电: "));
	message.Append(cJid);
	message.Append(_T("\r\n 是否接听?"));

	//显示来电对话框
	m_pCallCommingDialog->ShowWindow(SW_NORMAL);
	//循环播放来电铃声
	//创建播放声音线程

	_tagJidFrom from;
	from.sJid = cJid;
	from.pWnd = this;
	//状态1: 新来电，可以 接听 or 挂断
	//状态2: 通话中,可以挂断;
	HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, AcceptCallFunc, &from, 0, 0);
	CloseHandle(hPlayMusic);
	PlaySound(m_strPathIncommingBell);

	return acceptCall;
}


void CLibJingleDlg::AddToStatusDebug(TCHAR* msg)
{
	if(m_bDebugStatus.GetCheck()==BST_CHECKED)
	{
		m_lDebug.Append(CString(msg));
		UpdateData(false);
	}
}


void CLibJingleDlg::AddToChatDebug(const char* msg)
{
    if(m_bDebugStatus.GetCheck()==BST_CHECKED)
    {
        CString chatmsg = UTF82Unicode(msg);
        m_lDebug.Append(chatmsg);
        UpdateData(false);
        
        CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_DEBUG);
        if (pEdit != NULL) 
        { 
            pEdit->LineScroll(pEdit->GetLineCount());
        }
    }
}

BOOL CLibJingleDlg::PreTranslateMessage( MSG* pMsg )
{
    if (WM_KEYDOWN == pMsg->message)
	{
		//拦截esc消息
		if (VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CLibJingleDlg::AddToXMLDebug(const char* msg)
{
	if(m_bDebugXML.GetCheck()==BST_CHECKED)
	{
		m_lDebug.Append(CString(msg));
		UpdateData(false);
	}
}

void CLibJingleDlg::AddToBeatDebug(const char* msg)
{
	if(m_bDebugBeat.GetCheck()==BST_CHECKED)
	{
		m_lDebug.Append(CString(msg));
		UpdateData(false);
	}
}

void CLibJingleDlg::AddToRoster(const char* jid)
{
	CString cJid(jid);
	if(m_lRoster.FindStringExact(-1, cJid) == LB_ERR)
	{
		m_lRoster.AddString(cJid);
	}
}

void CLibJingleDlg::RemoveFromRoster(const char* jid)
{
	CString cJid(jid);
	int i = m_lRoster.FindStringExact(-1, cJid);
	if(i != LB_ERR)
	{
		m_lRoster.DeleteString(i);
	}
}

void CLibJingleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLibJingleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLibJingleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CLibJingleDlg::OnWM_APP(WPARAM wParam, LPARAM lParam)
{
	LRESULT res = 0;

	if (wParam != 0)
	{
		CMessageBase* oMsg = (CMessageBase*) wParam;
		oMsg->Handle();
	}

	return res;
}

void CLibJingleDlg::OnClose()
{
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA); 
	nid.hWnd = this->m_hWnd; 
	nid.uID = IDR_MAINFRAME; 
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
	nid.uCallbackMessage = WM_SHOWTASK; 
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)); 
	wcscpy(nid.szTip, _T("计划任务提醒"));// 
	Shell_NotifyIcon(NIM_ADD,&nid); 
	this->ShowWindow(SW_HIDE); 
}


UINT __stdcall DialFunc(void *pvoid)
{
	_tagJidFrom *pFrom = (_tagJidFrom *)pvoid;
	CLibJingleDlg *pDlag = (CLibJingleDlg *)pFrom->pWnd;
	if (NULL== pDlag) return 0;

	//呼叫60s对方不接听，认为超时，关闭铃声
	//TODO:是否要忙音
	//TODO：是否需要通过配置文件设置超时时间
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hDialEvents, FALSE, 60 * 1000);
	if (dwRet ==  WAIT_OBJECT_0)//接听事件
	{
		//直接停止不行,需要发送消息
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//pDlag->StopPlay();
		//重置为无信号状态
		//调用 SetEvent设置有信号之后,需要调用reset设置为无信号
		ResetEvent(pDlag->m_hDialEvents[0]);

		//TODO： 
		//接听
		pDlag->GetJingleMain().JingleAdapter().Message().SetJidAndType(pFrom->sJid.GetBuffer(), ToJingleMessage::ACCEPT_CALL);
		pDlag->GetJingleMain().JingleAdapter().SetEvent();
	}
	else  //挂断事件,超时停止 或 异常 都停止
	{
		PostMessage(pDlag->m_hWnd, WM_STOPMUSIC, NULL, NULL);
		//超时或者是拒绝进接听
		pDlag->GetJingleMain().JingleAdapter().Message().SetJidAndType(pFrom->sJid.GetBuffer(), ToJingleMessage::REJECT_CALL);
		pDlag->GetJingleMain().JingleAdapter().SetEvent();

		if (dwRet ==  WAIT_OBJECT_0 + 1)
		{
			//设置为无信号
			ResetEvent(pDlag->m_hDialEvents[1]);
		}
	}

	return 1;
}

//呼叫
void CLibJingleDlg::OnBnClickedCall()
{
	CString sJID;
	m_sJid.GetWindowText(sJID);

	//主叫状态
    m_callStatus = DIALING;

	_tagJidFrom from;
	from.sJid = sJID;
	from.pWnd = this;

	//状态1: 呼叫,可以自行挂断;
	//状态2: 呼叫超时不接听,本地听到忙音
	HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, DialFunc, &from, 0, 0);
	CloseHandle(hPlayMusic);
	PlaySound(m_strPathDialingBell);//打电话

	//发起呼叫动作
	m_JingleMain.JingleAdapter().Message().SetJidAndType(sJID.GetBuffer(), ToJingleMessage::CALL);
	m_JingleMain.JingleAdapter().SetEvent();

	//显示呼叫界面
	m_pCallDialog->ShowWindow(SW_NORMAL);
	::SetDlgItemText(m_pCallDialog->m_hWnd, IDC_NAME, sJID);
}

//挂断对方电话
void CLibJingleDlg::OnBnClickedHangup()
{
	m_JingleMain.JingleAdapter().Message().Type(ToJingleMessage::HANGUP);
	m_JingleMain.JingleAdapter().SetEvent();
}

void CLibJingleDlg::OnLbnSelchangeLroster()
{
	int nCurSel = m_lRoster.GetCurSel();
	if(nCurSel != LB_ERR)
	{
		CString sText;
		m_lRoster.GetText(nCurSel, sText);
		m_sJid.SetWindowText(sText);
	}
}

void CLibJingleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add some code for resizing here
}

void CLibJingleDlg::OnBnClickedLogin()
{
	if(m_JingleMain.Running())
	{
		m_JingleMain.JingleAdapter().Message().Type(ToJingleMessage::EXIT);
		m_JingleMain.JingleAdapter().SetEvent();
		m_JingleMain.Stop();
	}
	CString sJID;
	CString sDomain;
	CString sUser;
	m_cJid.GetWindowText(sJID);

	int i = sJID.Find(_T("@"));
	if(i == -1)
	{
		sUser = sJID;
		sDomain = m_AccountInfo.strDomain.c_str();
	}
	else
	{
		sUser = sJID.Left(i);
		sDomain = sJID.Right(sJID.GetLength()-i-1);
	}

	CString sPass;
	m_cPasswd.GetWindowText(sPass);

	m_JingleMain.Run(m_hWnd,
	sUser.GetBuffer(),
	sPass.GetBuffer(),
	sDomain.GetBuffer()
	);
}

void CLibJingleDlg::OnBnClickedLogout()
{
	if(m_JingleMain.Running())
	{
		m_JingleMain.JingleAdapter().Message().Type(ToJingleMessage::EXIT);
		m_JingleMain.JingleAdapter().SetEvent();
		m_JingleMain.Stop();
	}
}

void CLibJingleDlg::OnDestroy()
{
	CDialog::OnDestroy();
}


//added by wanglg
void CLibJingleDlg::OnBnClickedSendmsg()
{
    // TODO: ?ú′?ìí?ó???tí¨?a′|àí3ìDò′ú??
    CString csStr;
    m_cChatMsg.GetWindowText(csStr);
    CStringA caStr = Unicode2UTF8(csStr);
    std::string cnmsg = caStr;

    if (cnmsg.empty())
    {
        csStr = _T("??o?￡?XXX￡?");
        caStr = Unicode2UTF8(csStr);
        cnmsg = caStr;
    }

    CString sJID;
    m_sJid.GetWindowText(sJID);
    CStringA jidStr = Unicode2UTF8(sJID);
    std::string jidmsg = jidStr;

    if (jidmsg.empty())
    {
        AfxMessageBox(_T("??????ó??§￡?"));
        return;
    }

    m_JingleMain.JingleAdapter().Message().SetChatMsgAndType(jidmsg, cnmsg, ToJingleMessage::STR_CHAT);
    m_JingleMain.JingleAdapter().SetEvent();

    m_cChatMsg.SetWindowText(L"");
    m_cChatMsg.SetFocus();
    m_cChatMsg.UpdateData(FALSE);
}



