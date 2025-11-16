// InstallHelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "InstallHelpDlg.h"
#include ".\installhelpdlg.h"


// CInstallHelpDlg dialog

IMPLEMENT_DYNAMIC(CInstallHelpDlg, CDialog)
CInstallHelpDlg::CInstallHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallHelpDlg::IDD, pParent)
	, m_text(_T(""))
{
}

CInstallHelpDlg::~CInstallHelpDlg()
{
}

void CInstallHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Control(pDX, IDC_PICTURE, m_pic);
}


BEGIN_MESSAGE_MAP(CInstallHelpDlg, CDialog)
END_MESSAGE_MAP()


// CInstallHelpDlg message handlers

BOOL CInstallHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
