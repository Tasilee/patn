// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h" 
#include "patn.h" 
#include "resource.h" 

#include "Splash.h"  

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CRect g_rect;
CWnd* g_wnd = NULL;
CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();

	CDC * dc = g_wnd->GetDC();
	CString string;
	dc->SetBkColor(0x00C0C0C0);
	CRect rect;
	rect = g_rect;
	rect.bottom -= 0;rect.right -= 20;
	rect.left += 255;rect.top += 275;
	string = PATN_VERSION;
	dc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);

	rect.bottom += 35;rect.top += 35;
	string = PATN_DATESHORT;
	dc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);

	rect = g_rect;
	rect.top += 270;
	rect.left -= 230;
	//string = "Registered to: " + PATN_DISTRIBUTION_USER;
	//dc->DrawText(string,rect,DT_LEFT|DT_NOCLIP);
	//rect.top += 20;
	//string = "             " + PATN_DISTRIBUTION_ORG;
	//dc->DrawText(string,rect,DT_LEFT|DT_NOCLIP);

}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
	DeleteObject(&bm);

}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	m_bitmap.DeleteObject();
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	this->GetWindowRect(g_rect);
	g_wnd = (CWnd*) this;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(1, 3050, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	DeleteObject(&bm);

	CDC * pdc = g_wnd->GetDC();
	CString string;
	pdc->SetBkColor(0x00C0C0C0);
	CRect rect;
	rect = g_rect;
	rect.bottom -= 0;rect.right -= 20;
	rect.left += 255;rect.top += 275;
	string = PATN_VERSION;
	pdc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);

	rect.bottom += 35;rect.top += 35;
	string = PATN_DATESHORT;
	pdc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);

	rect = g_rect;
	rect.top += 270;
	rect.right -= 180;
	//string = "Registered to: " + PATN_DISTRIBUTION_USER;
	//pdc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);
	//rect.top += 20;	
	//string = "             " + PATN_DISTRIBUTION_ORG;
	//pdc->DrawText(string,rect,DT_CENTER|DT_NOCLIP);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// user hasn't taken any action (eg, clicking on a menu to begin),
	// so notify the application that nothing has happened yet ...
	GetParent()->PostMessage(NO_USER_ACTION_ON_STARTUP);

	// ... then destroy the splash screen window.
	HideSplashScreen();
}
