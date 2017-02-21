// CallDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CallDialog.h"
#include "afxdialogex.h"


// CCallDialog 对话框

IMPLEMENT_DYNAMIC(CCallDialog, CDialogEx)

CCallDialog::CCallDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCallDialog::IDD, pParent)
{

}

CCallDialog::~CCallDialog()
{
}

void CCallDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCallDialog, CDialogEx)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CCallDialog 消息处理程序


BOOL CCallDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	return CDialogEx::OnEraseBkgnd(pDC);
}
