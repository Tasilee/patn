// AnosimFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "AnosimFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnosimFrame

IMPLEMENT_DYNCREATE(CAnosimFrame, CMDIChildWnd)

CAnosimFrame::CAnosimFrame()
{
}

CAnosimFrame::~CAnosimFrame()
{
}


BEGIN_MESSAGE_MAP(CAnosimFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CAnosimFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnosimFrame message handlers


BOOL CAnosimFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;


	cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_ANOSIM_ICON));
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CAnosimFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Table");
		SetWindowText(str);

	}
	
	return CMDIChildWnd::GetActiveDocument();
}
