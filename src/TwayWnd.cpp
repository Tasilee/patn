// TwayWnd.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "TwayWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTwayWnd

IMPLEMENT_DYNCREATE(CTwayWnd, CMDIChildWnd)

CTwayWnd::CTwayWnd()
{
}

CTwayWnd::~CTwayWnd()
{
}


BEGIN_MESSAGE_MAP(CTwayWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CTwayWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTwayWnd message handlers


BOOL CTwayWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;

		cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_TWAY_ICON));

	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CTwayWnd::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();
	// TODO: Add your specialized code here and/or call the base class

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Two-Way Table");
		SetWindowText(str);
	}
	
	return CMDIChildWnd::GetActiveDocument();
}
