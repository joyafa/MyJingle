// FriendList.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FriendList.h"
#include "resource.h"

// CFriendList
IMPLEMENT_DYNAMIC(CFriendList, CListCtrl)

CFriendList::CFriendList()
{
	m_nLastSel = -2;
}

CFriendList::~CFriendList()
{
	m_bmpSel1.DeleteObject();
	m_bmpSel2.DeleteObject();
	m_bmpUnSel.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFriendList, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CFriendList::PreSubclassWindow()
{
	CRect rectBig(6,10,36,22),rectSmall(7,7,15,15),
		rectBig2(6,0,36,12),rectAll(0,0,186,21);
	CDC dcMem;
	CClientDC dc(this);

	CBrush hBrushBack,hBrushGray;
	CPen hPen,hPenWrite;
	hBrushBack.CreateSolidBrush(RGB(255,255,255));
	hBrushGray.CreateSolidBrush(GRAY_BRUSH);
	hPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	hPenWrite.CreatePen(PS_SOLID,1,RGB(255,255,255));
	CFont font;
	font.CreatePointFont(90,TEXT("����"),NULL);
	
#ifdef IDB_FNDLST_SEL
	m_bmpSel1.LoadBitmap(IDB_FNDLST_SEL);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpSel1.CreateCompatibleBitmap(&dcMem,186,22);

	dcMem.SelectObject(&m_bmpSel1);

	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPenWrite);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hPen);
	dcMem.SelectObject(hBrushGray);

	dcMem.MoveTo(0,22);
	dcMem.LineTo(0,0);
	dcMem.LineTo(185,0);
	dcMem.LineTo(185,22);
	
	dcMem.Rectangle(rectBig);

	dcMem.SelectObject(&font);
	dcMem.SetBkMode(TRANSPARENT);
	CRect rectText1(9,15,40,22);
	dcMem.SetTextColor(RGB(255,255,255));
	dcMem.DrawText(TEXT("ͷ��"),&rectText1,DT_LEFT);

	dcMem.DeleteDC();	
#endif
	
#ifdef IDB_FNDLST_SEL2
	m_bmpSel2.LoadBitmap(IDB_FNDLST_SEL2);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpSel2.CreateCompatibleBitmap(&dcMem,186,22);

	dcMem.SelectObject(&m_bmpSel2);
	
	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPenWrite);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hBrushGray);
	dcMem.SelectObject(hPen);

	dcMem.MoveTo(0,0);
	dcMem.LineTo(0,20);
	dcMem.LineTo(185,20);
	dcMem.LineTo(185,-1);
	
	dcMem.Rectangle(rectBig2);

	dcMem.SelectObject(&font);
	dcMem.SetBkMode(TRANSPARENT);
	CRect rectText2(9,-6,40,22);
	dcMem.SetTextColor(RGB(255,255,255));
	dcMem.DrawText(TEXT("ͷ��"),&rectText2,DT_LEFT);

	dcMem.DeleteDC();
#endif
	
#ifdef IDB_FNDLST_UNSEL
	m_bmpUnSel.LoadBitmap(IDB_FNDLST_UNSEL);
#else
	dcMem.CreateCompatibleDC(&dc);
	m_bmpUnSel.CreateCompatibleBitmap(&dcMem,186,22);

	dcMem.SelectObject(&m_bmpUnSel);
	
	dcMem.SelectObject(&hBrushBack);
	dcMem.SelectObject(&hPen);
	dcMem.SetBkColor(RGB(255,255,255));
	dcMem.Rectangle(rectAll);

	dcMem.SelectObject(hBrushGray);
	dcMem.SelectObject(hPen);
	
	dcMem.Rectangle(rectSmall);

	dcMem.DeleteDC();
#endif

	// �����б���߶�
	m_bmpPortrait.CreateBitmap(1,22,0,0,NULL);
	m_imgPortrait.Create(1,21,ILC_COLOR24,1,1);
	m_imgPortrait.Add(&m_bmpPortrait,RGB(11,11,11));

	SetImageList(&m_imgPortrait,2);

	CRect rect;
	GetWindowRect(&rect);
	InsertColumn(0,TEXT("PID"),LVCFMT_LEFT);
	SetColumnWidth(0,rect.right - rect.left-4);

	SetExtendedStyle( LVS_EX_FULLROWSELECT
		|LVS_OWNERDRAWFIXED 
		|LVS_EX_SUBITEMIMAGES
		|LBS_OWNERDRAWVARIABLE);

	CListCtrl::PreSubclassWindow();

	hBrushBack.DeleteObject();
	hBrushGray.DeleteObject();
	hPen.DeleteObject();
	hPenWrite.DeleteObject();
}



void CFriendList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rectBottom,rectList;

	GetItemRect(GetItemCount()-1,&rectBottom,LVIR_BOUNDS);
	GetWindowRect(&rectList);

	LVHITTESTINFO   LVHTestInfo;    
	LVHTestInfo.pt = point;    
	HitTest(&LVHTestInfo);    
	int nRow=LVHTestInfo.iItem;   //���� 
	SubItemHitTest(&LVHTestInfo);    
	int nCol=LVHTestInfo.iSubItem;  //���� 

	if (nRow == -1)
	{
		CListCtrl::OnLButtonDblClk(nFlags, point);	
		return;
	}	
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CFriendList::AddNewUser( CString str )
{
	//int nCount = GetItemCount();
	InsertItem(0,str,0);
	m_nLastSel = -2;
}

void CFriendList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 22;
}

void CFriendList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDc->SetBkMode(TRANSPARENT);

	CRect rect = lpDrawItemStruct->rcItem;

	if (GetItemState(lpDrawItemStruct->itemID,LVIS_SELECTED))
	{
		// ���ѡ�е����°벿��, ��ѡ�����°벿
		if (lpDrawItemStruct->itemID == m_nLastSel+1)
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel2);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,186,21,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID - 1,0);
			tmp = tmp + TEXT("�ĸ���ǩ����");
			if (tmp.GetLength()>20)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+=47;
			rect.top+=4;
			pDc->DrawText(tmp,rect,DT_LEFT);
			
			dcMem.SelectObject(pOldBitmap);

			return;
		}
		else
		{
			pDc->SetTextColor(RGB(77,77,77));

			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel1);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top+1,186,21,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>16)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+=47;
			rect.top+=6;
			pDc->DrawText(tmp,rect,DT_LEFT);

			dcMem.SelectObject(pOldBitmap);
		}		
	} 
	else	// ���������û��ѡ�еĲ���
	{

		// ��������ǵ�ǰѡ����Ŀ�ϰ벿
		if (lpDrawItemStruct->itemID == m_nLastSel)
		{
			pDc->SetTextColor(RGB(77,77,77));
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel1);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top+1,186,21,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>16)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+=47;
			rect.top+=6;
			pDc->DrawText(tmp,rect,DT_LEFT);

			return;
		}
		else if (lpDrawItemStruct->itemID == m_nLastSel + 1)	
		{	// ���������ѡ����Ŀ���°벿
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpSel2);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,186,21,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID - 1,0);
			tmp = tmp + TEXT("�ĸ���ǩ����");
			if (tmp.GetLength()>20)
			{
				tmp = GetStringLeft(tmp,8) + TEXT("...");
			}
			rect.left+=47;
			rect.top+=4;
			pDc->DrawText(tmp,rect,DT_LEFT);

			dcMem.SelectObject(pOldBitmap);
			return;
		}
		else	// ���������û��ѡ�еĲ���
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDc);
			pDc->SetTextColor(RGB(77,77,77));

			CBitmap *pOldBitmap = dcMem.SelectObject(&m_bmpUnSel);

			pDc->BitBlt(rect.left + ITEMOFFSET,rect.top,186,21,&dcMem,0,0,SRCCOPY);

			CString tmp;
			tmp = GetItemText(lpDrawItemStruct->itemID,0);
			if (tmp.GetLength()>=24)
			{
				tmp = tmp.Left(22) + TEXT("...");
			}
			rect.left+=22;
			rect.top+=5;
			pDc->DrawText(tmp,rect,DT_LEFT);
		}	
	}
	// TODO:  ������Ĵ����Ի���ָ����
}

void CFriendList::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO   LVHTestInfo;    
	LVHTestInfo.pt = point;    
	HitTest(&LVHTestInfo);    
	unsigned int nRow=LVHTestInfo.iItem;   //���� 
	SubItemHitTest(&LVHTestInfo);    
	unsigned int nCol=LVHTestInfo.iSubItem;  //���� 

	if (nRow == -1 && m_nLastSel != -1)
	{
		DeleteItem(m_nLastSel+1);
		m_nLastSel = -2;
		SetItemState(-1, LVIS_SELECTED,LVIS_SELECTED);
		CListCtrl::OnLButtonDown(nFlags, point);
		Invalidate();
		return;
	}
	

	if (m_nLastSel == -2)
	{
		InsertItem(nRow + 1, GetItemText(nRow,0));
	}
	else if (m_nLastSel != nRow 
		&& m_nLastSel + 1 != nRow
		&& m_nLastSel != -1)
	{
		DeleteItem(m_nLastSel+1);
		InsertItem(nRow + 1, GetItemText(nRow,0));		
		Invalidate();
	}
	

	if (m_nLastSel == -2)
	{
		m_nLastSel = nRow;
		SetItemState(nRow , LVIS_SELECTED,LVIS_SELECTED);
	}
	else if(nRow > m_nLastSel)
	{
		m_nLastSel = nRow - 1;
		SetItemState(nRow , LVIS_SELECTED,LVIS_SELECTED);
	}
	else
	{
		m_nLastSel = nRow;
		SetItemState(nRow, LVIS_SELECTED,LVIS_SELECTED);
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CFriendList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFriendList::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;

	CListCtrl::OnRButtonDown(nFlags, point);
}

CString CFriendList::GetStringLeft( CString &str, int nLeft )
{
	CString   sRet=_T("");   
	int   nPos=0;   
	while(nPos<str.GetLength()   &&   nLeft)   
	{   
		if(str.GetAt(nPos)<0){   
			sRet=sRet+str.GetAt(nPos)+str.GetAt(nPos+1);   
			nPos++;   
		}else   
			sRet+=str.GetAt(nPos);   
        nPos++;   
        nLeft--;   
	}   
	return   sRet;   
}