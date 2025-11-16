// 3DFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "3DFrame.h"
#include ".\3dframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DFrame

IMPLEMENT_DYNCREATE(C3DFrame, CMDIChildWnd)

C3DFrame::C3DFrame()
{
}

C3DFrame::~C3DFrame()
{
}


BEGIN_MESSAGE_MAP(C3DFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(C3DFrame)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DFrame message handlers

BOOL C3DFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style |= WS_OVERLAPPED;

	cs.cx = 640;
	cs.cy = 480;

HICON icon = AfxGetApp()->LoadIcon(IDR_3DVIEW_ICON);

	// Styles required by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_PARENTDC | CS_SAVEBITS | CS_CLASSDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, icon);
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* C3DFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();

	CPatnDoc * pDoc = (CPatnDoc*) d;
	if(pDoc != NULL){
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Ordination View");
		SetWindowText(str);
	}
	
	return CMDIChildWnd::GetActiveDocument();
}


void C3DFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);
}

void C3DFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	/*int restrict_width = -1;

	if((pRect->bottom-pRect->top)/(double)(pRect->right-pRect->left) > 0.75)
	{
		restrict_width = 1;
	}
	else if((pRect->bottom-pRect->top)/(double)(pRect->right-pRect->left) < 0.75)
	{		
		restrict_width = 0;
	}
	if(restrict_width == 1 && (fwSide==1 || fwSide == 7 || fwSide == 4))
	{
		pRect->left = (int)((pRect->bottom-pRect->top)/0.75)+pRect->right;
	}
	else if(restrict_width == 1)
	{
		pRect->right = (int)((pRect->bottom-pRect->top)/0.75)+pRect->left;
	}
	else if(restrict_width == 0 && (fwSide!=8 && fwSide!=6 && fwSide != 7))
	{
		pRect->top = (int)(0.75*(pRect->right-pRect->left))+pRect->bottom;
	}
	else if(restrict_width == 0)
	{
		pRect->bottom = (int)(0.75*(pRect->right-pRect->left))+pRect->top;
	}

	

*/

	CMDIChildWnd::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
}
