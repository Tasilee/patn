// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "patn.h"

#include "MainFrame.h"
#include "Splash.h"
#include "StartupDlg.h"
#include ".\mainframe.h"
#include "howtocheckforupdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	//}}AFX_MSG_MAP
	ON_MESSAGE(NO_USER_ACTION_ON_STARTUP, OnNoUserActionOnStartup)
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_FILE_CHECKFORUPDATE, OnFileCheckforupdate2)
	ON_COMMAND(ID_VIEW_PATNTOOLBAR, OnViewPatntoolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PATNTOOLBAR, OnUpdateViewPatntoolbar)
	ON_WM_PAINT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_viewtool = true;
	m_viewtoolpatn = true;
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}
						

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;



	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	ShowControlBar(&m_wndToolBar,true,0);
	
	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	
	//OK, now build the View Toolbar
	if (!m_wndViewToolBar.Create(this) ||
		!m_wndViewToolBar.LoadToolBar(IDR_VIEWTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndViewToolBar.SetBarStyle(m_wndViewToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndViewToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	ShowControlBar(&m_wndViewToolBar,true,0);
	
	//move to left of 1st toolbar
	CRect rect;
	m_wndToolBar.GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	DockControlBar(&m_wndViewToolBar,m_wndViewToolBar.GetStyle(),rect);

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);	

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LRESULT CMainFrame::OnNoUserActionOnStartup(WPARAM wParam, LPARAM)
{

	CStartupDlg dlg(this);
	long nAction = dlg.DoModal();
	switch (nAction)
	{
		case IDC_BTN_OPEN: // "Open" button clicked
			PostMessage(WM_COMMAND,ID_FILE_OPEN,0);
			break;

		case IDC_BTN_NEW: // "New" button clicked
			PostMessage(WM_COMMAND,ID_FILE_NEW,0);
			break;

		case IDC_BTN_IMPORT: // "Import" button clicked
			PostMessage(WM_COMMAND,ID_FILE_IMPORT,0);
			break;

		case IDC_BTN_WIZARD: // "Wizard" button clicked
			PostMessage(WM_COMMAND,ID_TOOLS_WIZARD,0);
			break;

 		case IDCANCEL:
		default:
			break;
	}

	return 0;
}


void CMainFrame::OnViewToolbar() 
{
	if(m_viewtool){
		ShowControlBar(&m_wndToolBar,false,0);
	}else{
		ShowControlBar(&m_wndToolBar,true,0);
	}

	m_viewtool = !m_viewtool;
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);

	if(m_viewtool)
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}



BOOL CMainFrame::DestroyWindow() 
{
	((CPatnApp*)AfxGetApp())->WindowChange();

	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnFileCheckforupdate2()
{

	// removed

}

//void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	CMDIFrameWnd::WinHelp(dwData, nCmd);
//}

void CMainFrame::OnViewPatntoolbar()
{
	// TODO: Add your command handler code here
	if(m_viewtoolpatn){
		ShowControlBar(&m_wndViewToolBar,false,0);
	}else{
		ShowControlBar(&m_wndViewToolBar,true,0);
	}

	m_viewtoolpatn = !m_viewtoolpatn;
}

void CMainFrame::OnUpdateViewPatntoolbar(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);

	if(m_viewtoolpatn)
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CMDIFrameWnd::OnPaint() for painting messages
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message != WM_ERASEBKGND) {
		return CMDIFrameWnd::OnWndMsg(message, wParam, lParam, pResult);
	} else {
		return true;
	}
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIFrameWnd::OnCommand(wParam, lParam);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
    cs.style |= WS_CLIPCHILDREN;

	return CMDIFrameWnd::PreCreateWindow(cs);
}
