#pragma once
#include "Resource.h"


// CCallCommingDialog �Ի���

class CCallCommingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCallCommingDialog)

public:
	CCallCommingDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCallCommingDialog();

// �Ի�������
	enum { IDD = IDD_CALLCOMMING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
