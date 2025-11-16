// ViewExtension.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ViewExtension.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewExtension

IMPLEMENT_DYNCREATE(CViewExtension, CView)

CViewExtension::CViewExtension()
{
}

CViewExtension::~CViewExtension()
{
}


BEGIN_MESSAGE_MAP(CViewExtension, CView)
	//{{AFX_MSG_MAP(CViewExtension)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewExtension drawing

void CViewExtension::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CViewExtension diagnostics

#ifdef _DEBUG
void CViewExtension::AssertValid() const
{
	CView::AssertValid();
}

void CViewExtension::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewExtension message handlers

CMenu * CViewExtension::GetMenu(){ 
	return &m_menu;
};

void CViewExtension::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	m_activated = (bActivate)?true:false;
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

bool CViewExtension::IsActivated()
{
	return m_activated;
}
