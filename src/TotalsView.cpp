// TotalsView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "TotalsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTotalsView

IMPLEMENT_DYNCREATE(CTotalsView, CView)

CTotalsView::CTotalsView()
{
	m_brushWindow.CreateSolidBrush(RGB(255,255,128));
}

CTotalsView::~CTotalsView()
{
}


BEGIN_MESSAGE_MAP(CTotalsView, CView)
	//{{AFX_MSG_MAP(CTotalsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTotalsView drawing

void CTotalsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTotalsView diagnostics

#ifdef _DEBUG
void CTotalsView::AssertValid() const
{
	CView::AssertValid();
}

void CTotalsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTotalsView message handlers

BOOL CTotalsView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	WNDCLASS wc;
	if ( !GetClassInfo(AfxGetInstanceHandle(), "TotalsView", &wc) )  // if class not already reg'd
	{
		wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = AfxWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = AfxGetInstanceHandle();
		wc.hIcon = NULL;			
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)m_brushWindow;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "TotalsView";
		if ( !AfxRegisterClass(&wc) )
			return FALSE;
	}

	cs.lpszClass = "TotalsView";
	
	return CView::PreCreateWindow(cs);
}
