// HistogramFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "HistogramFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistogramFrame

IMPLEMENT_DYNCREATE(CHistogramFrame, CMDIChildWnd)

CHistogramFrame::CHistogramFrame()
{
}

CHistogramFrame::~CHistogramFrame()
{
}


BEGIN_MESSAGE_MAP(CHistogramFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CHistogramFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistogramFrame message handlers

BOOL CHistogramFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;

	HICON icon = AfxGetApp()->LoadIcon(IDR_HISTOGRAM_ICON);
	
	cs.lpszClass = AfxRegisterWndClass(0,::LoadCursor(NULL, IDC_ARROW), NULL, icon);
	
	return CMDIChildWnd::PreCreateWindow(cs);
}


CDocument* CHistogramFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Histogram");
		SetWindowText(str);
	}	
	return CMDIChildWnd::GetActiveDocument();
}
