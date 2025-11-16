// ConstFid.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ConstFid.h"

#include "patndoc.h"

// CConstFid

IMPLEMENT_DYNCREATE(CConstFid, CMDIChildWnd)

CConstFid::CConstFid()
{
}

CConstFid::~CConstFid()
{
}

BOOL CConstFid::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;

		cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_BW_ICON));
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CConstFid::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Constancy and Fidelity");
		SetWindowText(str);
	}

	return CMDIChildWnd::GetActiveDocument();
}


BEGIN_MESSAGE_MAP(CConstFid, CMDIChildWnd)
END_MESSAGE_MAP()


// CConstFid message handlers
