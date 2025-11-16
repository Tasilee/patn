#pragma once



// CProgressDlgThread
#include "newdialog.h"

class CProgressDlgThread : public CWinThread
{
	DECLARE_DYNCREATE(CProgressDlgThread)

public:
	CProgressDlgThread();           // protected constructor used by dynamic creation
	virtual ~CProgressDlgThread();

	static HWND mhnd;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void End();

	bool m_ready;
	CnewDialog * dlg;
	HWND m_hnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	virtual BOOL PumpMessage();
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
	virtual int Run();
};





