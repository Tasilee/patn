// ScatFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "ScatFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScatFrame

IMPLEMENT_DYNCREATE(CScatFrame, CMDIChildWnd)

CScatFrame::CScatFrame()
{
}

CScatFrame::~CScatFrame()
{
}


BEGIN_MESSAGE_MAP(CScatFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CScatFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScatFrame message handlers

BOOL CScatFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CScatFrame::GetActiveDocument() 
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
