#pragma once


// CActionEdit

class CActionEdit : public CEdit
{
	DECLARE_DYNAMIC(CActionEdit)

public:
	CActionEdit();
	virtual ~CActionEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


