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
	INITIAL    = 0, //��ʼ״̬     
	DIALING    = 1, //����ing
	ACCEPTING     , //����ing: ���з�,������������,δ����,���ԹҶ�(Reject),���Խ���
	ONLINE        , //ͨ����,���ԹҶ�(Hangup),���ߵȶԷ��Ҷ�(�յ��Է�ʲô��Ϣ)
	BUSYING         //���г�ʱ,æ��
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

	//�������ļ��ж�ȡ���������Ϣ
	void GetConfigInfo();

	//************************************
	// Method:    GetUserPasswordFromSql
	// FullName:  CLibJingleDlg::GetUserPasswordFromSql
	// Access:    protected 
	// Returns:   bool
	// Description: ����ʱ,ͨ�����ݿ������,��ȡ��¼�û���������,ͨ��IP��ַ��ʽ��ȡ,���û����ϱ��л�ȡ,Ҫ�����Ա��Ҫ����ip��ַ���ú��û���������֮�����Ϣ
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
	//�����ذ����¼�,��Ӳ����ȡ�����̷߳���������Ϣ
	afx_msg LRESULT OnHandlePhone(WPARAM wParam, LPARAM lParam);


	afx_msg void OnClose();
	LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
public:
	//��������������,�����¼����ź�
	void SetAnswerEvent();
	//�ܾ� / �ж� �����ź�
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
	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hAcceptCallEvents[2];
	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hDialEvents[2];


	//TODO:�򵥴���,1 �ͻ��˶���ͨ�����غ��з�,��������صĲ���״̬;
	//              2 ǰ̨�����,���з�,ֻ���������绰����,����ǰ̨���пͻ��˵����--> ��Ϊ�н���,��ȡ���水ť��ʽ,�����¼���ʽ
	//��¼��ǰ״̬: ���� ���� ����
	CallStatus m_callStatus;

	//���з� ״̬:  
	//����-->�Է�����-->1�Է��Ҷ�;2�Լ��Ҷ�(����,��˵�������); ����-->��ʱ�Ҷ�(�Է�������);����--->�Լ��Ҷ�(�������,�����);
	enum DialSide
	{
		//DIALING,  //����״̬
		//ONLINE, //ͨ����
	};


	//���з�
	enum AcceptSide
	{

	};

private:
	AccountInfo m_AccountInfo;
	//���жԻ���
	CCallDialog *m_pCallDialog;
	CCallCommingDialog *m_pCallCommingDialog;
	//����ģʽ : "RunningMode���ԣ�ΪADMINģʽ����Ҫ��������
	std::string m_strRunningMode;
	//��������
	std::string m_strPathIncommingBell;
	std::string m_strPathBusyBell;
	std::string m_strPathDialingBell;

	


	//MCI���������豸
	CMCIPlayMusic m_mciMusic;
    //usb�����豸
	CUsbDevice m_usbDevice;
};
