// ProgressDlgThread.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ProgressDlgThread.h"
#include ".\progressdlgthread.h"


#include <sys/timeb.h>
#include <time.h>
// CProgressDlgThread

IMPLEMENT_DYNCREATE(CProgressDlgThread, CWinThread)

HWND CProgressDlgThread::mhnd = 0;
//FILE * fp = NULL;
//struct __timeb64 timebuffer;
//char *timeline;

CProgressDlgThread::CProgressDlgThread()
{
	dlg = NULL;
	m_hnd = mhnd;
	m_ready = false;
	//if(-8 == (int) mhnd)
//	{
//		fp = fopen("c:\\patn_debug_thread.csv","w");
//		if(fp != NULL)
//		_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Begin\n");
//	}	
}

CProgressDlgThread::~CProgressDlgThread()
{
	if(dlg != NULL)
		delete dlg;

//	if(fp != NULL)
//		_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"End\n");
//	if(fp != NULL)
//		fclose(fp);
//	fp = NULL;
}

BOOL CProgressDlgThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	
	//dlg->BringWindowToTop();
	//dlg->SetForegroundWindow();


//	if(fp != NULL)
//		_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"InitInstance - done\n");
	return TRUE;
}

int CProgressDlgThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	dlg->PostMessage(WM_QUIT,0,0);
	//if(fp != NULL)
//		_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"ExitInstance - done\n");
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProgressDlgThread, CWinThread)
END_MESSAGE_MAP()


// CProgressDlgThread message handlers

void CProgressDlgThread::End()
{
	//::AfxEndThread(0);
	if(dlg && ::IsWindow(dlg->m_hWnd))
		SendMessage(dlg->m_hWnd,IDOK,0,0);
	if(dlg != NULL)
		delete dlg;
	dlg = NULL;
	
	this->ExitInstance();
}
BOOL CProgressDlgThread::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	

	return CWinThread::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CProgressDlgThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == -23)
	{
		dlg->inc();

	}
	return CWinThread::PreTranslateMessage(pMsg);
}

BOOL CProgressDlgThread::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(lpMsg->message == -23)
	{
		dlg->inc();

	}

	return CWinThread::ProcessMessageFilter(code, lpMsg);
}

BOOL CProgressDlgThread::PumpMessage()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinThread::PumpMessage();
}

LRESULT CProgressDlgThread::ProcessWndProcException(CException* e, const MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == -23)
	{
		dlg->inc();

	}

	return CWinThread::ProcessWndProcException(e, pMsg);
}

int CProgressDlgThread::Run()
{	
	//if(fp != NULL)
	//	_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Run - start\n");
	//WAIT for 
	//if(this->m_hnd == NULL)
		dlg = new CnewDialog(CWnd::FromHandle(GetDesktopWindow()));
	//	if(fp != NULL)
	//	_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Run - new dlg\n");
	//else
	//{
	//	dlg = new CnewDialog(CWnd::FromHandle(m_hnd));		
	//}
	dlg->Create("Progress");	
	//if(fp != NULL)
	//	_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Run - dlg->Create\n");
	dlg->ShowWindow(SW_SHOW);
	//if(fp != NULL)
	//	_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Run - dlg->Show -\n");
//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("gogogo");
	m_ready = true;
	//if(fp != NULL)
	//	_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Run - done\n");
	return CWinThread::Run();
}
