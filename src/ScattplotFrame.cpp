// HistogramFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "scattplotFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScattplotFrame

IMPLEMENT_DYNCREATE(CScattplotFrame, CMDIChildWnd)

CScattplotFrame::CScattplotFrame()
{
}

CScattplotFrame::~CScattplotFrame()
{
}


BEGIN_MESSAGE_MAP(CScattplotFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CScattplotFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScattplotFrame message handlers

BOOL CScattplotFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;
	
cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_SCATTERPLOT_ICON));

	return CMDIChildWnd::PreCreateWindow(cs);
}


CDocument* CScattplotFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Scatterplot");
		SetWindowText(str);
	}	
	return CMDIChildWnd::GetActiveDocument();
}
