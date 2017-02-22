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

// LibJingleDlg.h : header file
//

#pragma once

#include "JingleMain.h"
#include "afxwin.h"
#include "SQLConnector.h"
#include "CallDialog.h"
#include "CallCommingDialog.h"
#include "MCIPlayMusic.h"
#include "UsbDevice.h"


struct _tagJidFrom
{
	CString sJid;
	CWnd *pWnd;
};

enum CallStatus
{
	INITIAL    = 0, //初始状态     
	DIALING    = 1, //呼叫ing
	ACCEPTING     , //接听ing: 被叫方,来电铃声提醒,未接听,可以挂断(Reject),可以接听
	ONLINE        , //通话中,可以挂断(Hangup),或者等对方挂断(收到对方什么消息)
	BUSYING         //呼叫超时,忙音
};

// CLibJingleDlg dialog
class CLibJingleDlg : public CDialog
{
// Construction
public:
	CLibJingleDlg(CWnd* pParent = NULL);	// standard constructor
	~CLibJingleDlg();	// standard destructor

// Dialog Data
	enum { IDD = IDD_LIBJINGLE_DIALOG };

	void AddToStatusDebug(TCHAR* msg);
	void AddToXMLDebug(const char* msg);
	void AddToBeatDebug(const char* msg);
    void AddToChatDebug(const char* msg);

	virtual BOOL PreTranslateMessage( MSG* pMsg ) override;

	JingleMain &GetJingleMain()
	{
		return m_JingleMain;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	JingleMain m_JingleMain;
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	//从配置文件中读取相关配置信息
	void GetConfigInfo();

	//************************************
	// Method:    GetUserPasswordFromSql
	// FullName:  CLibJingleDlg::GetUserPasswordFromSql
	// Access:    protected 
	// Returns:   bool
	// Description: 启动时,通过数据库服务器,获取登录用户名和密码,通过IP地址方式获取,从用户资料表中获取,要求管理员需要按照ip地址配置好用户名和密码之类的信息
	//************************************
	AccountInfo GetUserPasswordFromServer();
	CString GetMoudlePath();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnWM_APP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMCINotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopMusic(WPARAM wParam, LPARAM lParam);
	//处理开关按键事件,由硬件读取数据线程发出来的消息
	afx_msg LRESULT OnHandlePhone(WPARAM wParam, LPARAM lParam);


	afx_msg void OnClose();
	LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
public:
	//当接听方接听后,触发事件有信号
	void SetAnswerEvent();
	//拒绝 / 中断 设置信号
	void SetRejectEvent();
	void AddToRoster(const char* jid);
	void RemoveFromRoster(const char* jid);
	bool AcceptCallFrom(const char* jid);
public:
	CListBox m_lRoster;
public:
	afx_msg void OnBnClickedCall();
	afx_msg void OnBnClickedShowMax();
	void PlaySound(const std::string &strSonndPath);
public:
	CEdit m_sJid;
public:
	afx_msg void OnBnClickedHangup();
public:
	afx_msg void OnLbnSelchangeLroster();
public:
	CString m_lDebug;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CButton m_bDebugStatus;
public:
	CButton m_bDebugBeat;
public:
	CButton m_bDebugXML;
public:
	afx_msg void OnBnClickedLogin();
public:
	afx_msg void OnBnClickedLogout();
public:
	CEdit m_cJid;
public:
    CEdit m_cChatMsg;
public:
	CButton m_bLogin;
public:
	CButton m_bLogout;
public:
	CButton m_bCall;
public:
	CButton m_bHangup;
public:
    CButton m_bSendmsg;

public:
//	afx_msg void OnEnChangeJid2();
public:
	afx_msg void OnDestroy();
public:
	CEdit m_cPasswd;
public:
    afx_msg void OnBnClickedSendmsg();
	//被叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hAcceptCallEvents[2];
	//主叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hDialEvents[2];


	//TODO:简单处理,1 客户端都是通过开关呼叫方,做呼叫相关的操作状态;
	//              2 前台服务端,被叫方,只能做接听电话操作,考虑前台呼叫客户端的情况--> 因为有界面,采取界面按钮方式,而非事件方式
	//记录当前状态: 主叫 还是 接听
	CallStatus m_callStatus;

	//主叫方 状态:  
	//拨打-->对方接听-->1对方挂断;2自己挂断(累了,就说到这里吧); 拨打-->超时挂断(对方不接听);拨打--->自己挂断(不想打了,打错了);
	enum DialSide
	{
		//DIALING,  //拨打状态
		//ONLINE, //通话中
	};


	//被叫方
	enum AcceptSide
	{

	};

private:
	AccountInfo m_AccountInfo;
	//呼叫对话框
	CCallDialog *m_pCallDialog;
	CCallCommingDialog *m_pCallCommingDialog;
	//运行模式 : "RunningMode属性，为ADMIN模式，需要弹出界面
	std::string m_strRunningMode;
	//来电铃声
	std::string m_strPathIncommingBell;
	std::string m_strPathBusyBell;
	std::string m_strPathDialingBell;

	


	//MCI声音播放设备
	CMCIPlayMusic m_mciMusic;
    //usb开关设备
	CUsbDevice m_usbDevice;
};
