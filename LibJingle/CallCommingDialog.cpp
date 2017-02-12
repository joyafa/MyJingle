// CallCommingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CallCommingDialog.h"
#include "afxdialogex.h"


// CCallCommingDialog 对话框

IMPLEMENT_DYNAMIC(CCallCommingDialog, CDialogEx)

CCallCommingDialog::CCallCommingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCallCommingDialog::IDD, pParent)
{

}

CCallCommingDialog::~CCallCommingDialog()
{
}

void CCallCommingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCallCommingDialog, CDialogEx)
END_MESSAGE_MAP()


// CCallCommingDialog 消息处理程序
