// LegendView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "LegendView.h"


// CLegendView

IMPLEMENT_DYNCREATE(CLegendView, CView)

CLegendView::CLegendView()
{
}

CLegendView::~CLegendView()
{
}

BEGIN_MESSAGE_MAP(CLegendView, CView)
END_MESSAGE_MAP()


// CLegendView drawing

void CLegendView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	pDC->TextOut(10,10,"Hello world!");
}


// CLegendView diagnostics

#ifdef _DEBUG
void CLegendView::AssertValid() const
{
	CView::AssertValid();
}

void CLegendView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CLegendView message handlers
