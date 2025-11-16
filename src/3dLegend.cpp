// 3dLegend.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "3dLegend.h"


// C3dLegend

IMPLEMENT_DYNCREATE(C3dLegend, CListView)

C3dLegend::C3dLegend()
{
}

C3dLegend::~C3dLegend()
{
}

BEGIN_MESSAGE_MAP(C3dLegend, CListView)
END_MESSAGE_MAP()


// C3dLegend diagnostics

#ifdef _DEBUG
void C3dLegend::AssertValid() const
{
	CListView::AssertValid();
}

void C3dLegend::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// C3dLegend message handlers
