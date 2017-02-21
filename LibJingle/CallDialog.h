#pragma once
#include "Resource.h"


// CCallDialog 对话框

class CCallDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCallDialog)

public:
	CCallDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCallDialog();

// 对话框数据
	enum { IDD = IDD_CALL_DIALOG };

	void SetJidName(const CString &strJid)
	{
		SetDlgItemText(IDC_NAME, strJid);
	}

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
