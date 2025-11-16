// LegendCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "LegendCtrl.h"
#include ".\legendctrl.h"


// CLegendCtrl

IMPLEMENT_DYNAMIC(CLegendCtrl, CListCtrl)
CLegendCtrl::CLegendCtrl()
{
}

CLegendCtrl::~CLegendCtrl()
{
}


BEGIN_MESSAGE_MAP(CLegendCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_DRAWITEM()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()



// CLegendCtrl message handlers


void CLegendCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CListCtrl::DrawItem(lpDrawItemStruct);

	// TODO:  Add your code to draw the specified item
}

void CLegendCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	DWORD d = this->GetStyle();
	if(d & LVS_OWNERDRAWFIXED)
	{
		CRect r;	
	}
	
	*pResult = 0;
}

void CLegendCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CLegendCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style |= LVS_OWNERDRAWFIXED;

	return CListCtrl::PreCreateWindow(cs);
}

void CLegendCtrl::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnNcPaint() for painting messages
	CRect r;
}
