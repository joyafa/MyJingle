// ActionEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "SoundTestTool.h"
#include "ActionEdit.h"


// CActionEdit

IMPLEMENT_DYNAMIC(CActionEdit, CEdit)

CActionEdit::CActionEdit()
{

}

CActionEdit::~CActionEdit()
{
}


BEGIN_MESSAGE_MAP(CActionEdit, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CActionEdit 消息处理程序




void CActionEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	static TCHAR BASED_CODE szFilter[] = _T("PCM Files (*.pcm)|*.pcm|Voice Files (*.wav)|*.wav|All Files (*.*)|*.*||");

	CFileDialog fileDlg(TRUE, _T("*.pcm"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if (IDOK == fileDlg.DoModal())
	{
		SetWindowText(fileDlg.GetPathName());
	}

	CEdit::OnLButtonDblClk(nFlags, point);
}
