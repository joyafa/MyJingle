#pragma once
#include "Resource.h"


// CCallDialog �Ի���

class CCallDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCallDialog)

public:
	CCallDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCallDialog();

// �Ի�������
	enum { IDD = IDD_CALL_DIALOG };

	void SetJidName(const CString &strJid)
	{
		SetDlgItemText(IDC_NAME, strJid);
	}

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
