// patn.h : main header file for the PATN application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


#include "randomnumbergeneration.h"

void OpenURL(CString url);

/////////////////////////////////////////////////////////////////////////////
// CPatnApp:
// See patn.cpp for the implementation of this class
//

#ifndef _CPATNAPP_
#define _CPATNAPP_

class CPatnApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CPatnApp();

	int m_mousewheelsensitivity;
	CString m_update_id_string;
	//for silent_mode
	bool silent_mode;
	CString sm_data;
	CString sm_recipe;

protected:
	// CPatnDoc::OnNewDocument checks to see whether this flag is set to determine
	// whether a new, empty document is to be created or whether an external data
	// source is to be opened to supply data for the new document
	BOOL m_bGetExternalData;
	BOOL m_bGetOdbc;
	
public:
	BOOL IsGetExternalData() { return m_bGetExternalData; }
	BOOL IsGetOdbc() { return m_bGetOdbc; }

	void OpenODBC(){
		m_bGetExternalData = TRUE;
		m_bGetOdbc = TRUE;

		CWinApp::OnFileNew();
	}

	CString m_UserName;
	CString m_OrganisationName;

	void RandomSeed(long seed);
	double Rand();

	void SetOrdType(int value);

	void WindowChange();

	CString m_ProgramPath;

	bool m_copylabels;
	double m_ordobjectsize;
	//int m_ordtype;
	bool m_fileerrormsgs;

	char m_globalFontName [32];

public:
	void OpenFuseView(COleDocument* pDoc);
	void OpenAssocView(COleDocument* pDoc);
	void OpenHistogram(COleDocument* pDoc);
	void OpenTway(COleDocument* pDoc);
	//void OpenGstaView(COleDocument* pDoc);
	void Open3DXView(COleDocument* pDoc);
	void Open3DX9View(COleDocument* pDoc);
	//void Open3DView(COleDocument* pDoc);
	//void OpenScat(COleDocument* pDoc);
	void OpenScatterplot(COleDocument* pDoc);
	void OpenAnosimView(COleDocument* pDoc);
	void OpenBoxWhiskerView(COleDocument* pDoc);
	void OpenConstFidView(COleDocument* pDoc);

	void SetLandscape(bool value);

	int GetRandomGenerator();
	void SetRandomGenerator(int type);

	void WritePatnProfile();

	int IsRegistryIdentityValid();
	int IsLocalIdentityValid();
	int IsUserIdenityValid();

	afx_msg void OnFileCheckforupdate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatnApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CPatnApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileImport();
	afx_msg void OnFileODBC();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileODBC(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileAppend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileCheckForUpdate(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnFileCheckforupdateCloseFiles();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void OnHelpPatnonline();
	afx_msg void OnHelpDiscussionforum();
	virtual BOOL OnDDECommand(LPTSTR lpszCommand);
};


/////////////////////////////////////////////////////////////////////////////

#endif