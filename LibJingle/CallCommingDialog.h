#pragma once
#include "Resource.h"


// CCallCommingDialog 对话框

class CCallCommingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCallCommingDialog)

public:
	CCallCommingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCallCommingDialog();

// 对话框数据
	enum { IDD = IDD_CALLCOMMING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
