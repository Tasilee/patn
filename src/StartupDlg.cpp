// StartupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "StartupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg dialog


CStartupDlg::CStartupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStartupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStartupDlg)
	//}}AFX_DATA_INIT
}


void CStartupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStartupDlg)
	DDX_Control(pDX, IDC_GROUP, m_grp);
	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BTN_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BTN_IMPORT, m_btnImport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStartupDlg, CDialog)
	//{{AFX_MSG_MAP(CStartupDlg)
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpen)
	ON_BN_CLICKED(IDC_BTN_NEW, OnBtnNew)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg message handlers

BOOL CStartupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CFont font;
	font.CreateFont(-12,0,0,0,FW_BOLD,TRUE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH | FF_SWISS, ((CPatnApp*)AfxGetApp())->m_globalFontName);
	m_btnOpen.SetIcon(AfxGetApp()->LoadIcon(IDI_BTN_OPEN));
	m_btnNew.SetIcon(AfxGetApp()->LoadIcon(IDI_BTN_NEW));
	m_btnImport.SetIcon(AfxGetApp()->LoadIcon(IDI_BTN_IMPORT));
	m_grp.SetFont(&font,TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStartupDlg::OnBtnOpen() 
{
	EndDialog(IDC_BTN_OPEN);
}

void CStartupDlg::OnBtnNew() 
{
	EndDialog(IDC_BTN_NEW);
}

void CStartupDlg::OnBtnImport() 
{
	EndDialog(IDC_BTN_IMPORT);
}


