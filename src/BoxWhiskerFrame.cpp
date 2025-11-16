// BoxWhiskerFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "BoxWhiskerFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerFrame

IMPLEMENT_DYNCREATE(CBoxWhiskerFrame, CMDIChildWnd)

CBoxWhiskerFrame::CBoxWhiskerFrame()
{
}

CBoxWhiskerFrame::~CBoxWhiskerFrame()
{
}


BEGIN_MESSAGE_MAP(CBoxWhiskerFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CBoxWhiskerFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerFrame message handlers

BOOL CBoxWhiskerFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;

		cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_BW_ICON));
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CBoxWhiskerFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Box and Whisker Statistics - " + CString((pDoc->m_kw_patn_generated_groups)?"A-Priori Groups":"PATN Groups"));
		SetWindowText(str);
	}

	return CMDIChildWnd::GetActiveDocument();
}
