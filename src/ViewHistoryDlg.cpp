// ViewHistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ViewHistoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewHistoryDlg dialog


CViewHistoryDlg::CViewHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewHistoryDlg::IDD, pParent)
{
	m_created = false;
	//{{AFX_DATA_INIT(CViewHistoryDlg)
	m_history = _T("");
	//}}AFX_DATA_INIT
	m_plog = NULL;
}


void CViewHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewHistoryDlg)
	DDX_Control(pDX, IDOK, m_Box3);
	DDX_Control(pDX, IDC_HISTORY, m_Box2);
	DDX_Control(pDX, IDC_BUTTON2, m_Box1);
	DDX_Text(pDX, IDC_HISTORY, m_history);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CLEAR_HISTORY, m_box4);
}


BEGIN_MESSAGE_MAP(CViewHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(CViewHistoryDlg)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CLEAR_HISTORY, OnClearHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewHistoryDlg message handlers

void CViewHistoryDlg::OnButton2() 
{
	CString strBuffer,strLabel;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save History:";

	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"txt","*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text File (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();

	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		delete pFileDlg;
		return;
	}

	f.WriteString(m_history);

	f.Close();
	delete pFileDlg;		
}

void CViewHistoryDlg::OnSize(UINT nType, int cx, int cy) 
{

	CDialog::OnSize(nType, cx, cy);
	
	if(!m_created)
		return;

	CRect rect,rect2,rect3,rect4,rect5;
	GetWindowRect(rect);
	
	m_Box1.GetWindowRect(rect2);
	m_Box2.GetWindowRect(rect3);
	m_Box3.GetWindowRect(rect4);
	m_box4.GetWindowRect(rect5);

	rect3.bottom = rect.bottom - 40;
	rect2.top = rect.bottom - 32;
	rect2.bottom = rect.bottom - 10;
	rect4.top = rect.bottom - 32;
	rect4.bottom = rect.bottom - 10;
	rect5.top = rect.bottom - 32;
	rect5.bottom = rect.bottom - 10;

	
	rect3.right = rect.right - 15;
	
	m_dist = rect.bottom;
	m_dist2 = rect.right;

	ScreenToClient(rect2);
	m_Box1.MoveWindow(rect2);
	ScreenToClient(rect3);
	m_Box2.MoveWindow(rect3);
	ScreenToClient(rect4);
	m_Box3.MoveWindow(rect4);	
	ScreenToClient(rect5);
	m_box4.MoveWindow(rect5);	
}

BOOL CViewHistoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_created = true;

	//get distance from bottom of box
	CRect rect;
	GetClientRect(rect);	
	m_dist = rect.bottom;
	m_dist2 = rect.right;

	long nlines = m_Box2.GetLineCount();

	m_Box2.LineScroll(nlines);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewHistoryDlg::OnClearHistory() 
{
	if(!((CPatnApp*)AfxGetApp())->silent_mode && AfxMessageBox("Do you want to clear the history?",MB_YESNO) == IDYES)
	{
		if(m_plog)
		{
			*m_plog = "";
			m_Box2.SetWindowText("");
		}
	}
}
