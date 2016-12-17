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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef void (*installHook)(HWND, DWORD);
#define WM_MM (WM_USER + 2)


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
	DDX_Control(pDX, IDC_RICHEDIT21, m_cLicence);
}

BOOL CAboutDlg::OnInitDialog()
{
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

BEGIN_MESSAGE_MAP(CLibJingleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP, &CLibJingleDlg::OnWM_APP)
	ON_MESSAGE(WM_MM, &CLibJingleDlg::OnHardwareMessage)
	
	ON_BN_CLICKED(IDCALL, &CLibJingleDlg::OnBnClickedCall)
	ON_BN_CLICKED(IDHANGUP, &CLibJingleDlg::OnBnClickedHangup)
	ON_LBN_SELCHANGE(IDC_LROSTER, &CLibJingleDlg::OnLbnSelchangeLroster)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDLOGIN, &CLibJingleDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDLOGOUT, &CLibJingleDlg::OnBnClickedLogout)
//	ON_EN_CHANGE(IDC_JID2, &CLibJingleDlg::OnEnChangeJid2)
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

	m_cJid.SetWindowText((theApp.GetProfileString(_T("Account"), _T("JID"), 0)).GetBuffer());
	
	BYTE* encryptedPasswd = 0;
	UINT encryptedPasswdLength = 0;
	if (theApp.GetProfileBinary(_T("Account"), _T("PASSWD"), &encryptedPasswd, &encryptedPasswdLength))
	{
		CryptoService cs;
		if(cs.Decrypt(encryptedPasswd, encryptedPasswdLength))
		{
			m_cPasswd.SetWindowText((TCHAR*) cs.GetBuffer());
		}
	}

#ifdef __RELEASE_VERSION__
	//初始化鼠标钩子
	InitMouseHook();
	OnBnClickedLogin();
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//初始化硬件，硬件是一个鼠标，只是修改过鼠标键值
bool CLibJingleDlg::InitMouseHook()
{
	HINSTANCE hInstance = LoadLibrary(_T("MouseHook.dll"));
	if (NULL == hInstance) return false;
	installHook mouseActivtion = (installHook)GetProcAddress(hInstance, "installMouseHook");
	mouseActivtion(m_hWnd, WM_MM);

	return true;
}

LRESULT CLibJingleDlg::OnHardwareMessage(WPARAM wParam, LPARAM lParam)
{
	CString objTip;
	MSLLHOOKSTRUCT* pp = (MSLLHOOKSTRUCT*)lParam;
	//objTip.Format(_T("%d, %d"), pp->pt.x, pp->pt.y);
	//SetWindowText(objTip);

	switch(wParam)
	{
		//左键打电话
	case WM_LBUTTONDOWN:
		//TODO：当打了之后，重复按要屏蔽
		OnBnClickedCall();
		break;
	   //右键挂断电话
	case WM_RBUTTONDOWN:
		//TODO:要检查，正在通话中才能执行挂断，否则不处理
		OnBnClickedHangup();
		break;
	default:
		break;
	}

	return 1;
}


bool CLibJingleDlg::AcceptCallFrom(const char* jid)
{
	bool acceptCall(false);
	CString cJid(jid);
	CString message(_T("您有新的来电: "));
	message.Append(cJid);
	message.Append(_T("\r\n 是否接听?"));
	if (IDYES == AfxMessageBox(message, MB_YESNO, 0))
	{
		acceptCall = true;
		m_JingleMain.JingleAdapter().Message().SetJidAndType(cJid.GetBuffer(), ToJingleMessage::ACCEPT_CALL);
		m_JingleMain.JingleAdapter().SetEvent();
	}
	else
	{
		m_JingleMain.JingleAdapter().Message().SetJidAndType(cJid.GetBuffer(), ToJingleMessage::REJECT_CALL);
		m_JingleMain.JingleAdapter().SetEvent();
	}
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

void CLibJingleDlg::OnBnClickedCall()
{
	CString sJID;
	m_sJid.GetWindowText(sJID);
	m_JingleMain.JingleAdapter().Message().SetJidAndType(sJID.GetBuffer(), ToJingleMessage::CALL);
	m_JingleMain.JingleAdapter().SetEvent();
}

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
		sDomain = _T("gmail.com");
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
	CString sJID;
	m_cJid.GetWindowText(sJID);
	theApp.WriteProfileString(_T("Account"), _T("JID"), sJID);

	CString sPasswd;
	m_cPasswd.GetWindowText(sPasswd);

	CryptoService cs;
	if(cs.Encrypt((BYTE*)sPasswd.GetBuffer(), (sPasswd.GetLength() + 1) * sizeof(TCHAR), _T("XMPP Password")))
	{
		theApp.WriteProfileBinary(_T("Account"), _T("PASSWD"), cs.GetBuffer(), cs.GetLength());
	}

	CDialog::OnDestroy();
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
