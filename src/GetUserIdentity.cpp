// GetUserIdentity.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "GetUserIdentity.h"
#include ".\getuseridentity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetUserIdentity dialog


CGetUserIdentity::CGetUserIdentity(CWnd* pParent /*=NULL*/)
	: CDialog(CGetUserIdentity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetUserIdentity)
	m_user_name = _T("");
	m_user_key = _T("");
	m_user_org = _T("");
	//}}AFX_DATA_INIT
}


void CGetUserIdentity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetUserIdentity)
	DDX_Text(pDX, IDC_EDIT1, m_user_name);
	DDX_Text(pDX, IDC_EDIT2, m_user_key);
	DDX_Text(pDX, IDC_EDIT5, m_user_org);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetUserIdentity, CDialog)
	//{{AFX_MSG_MAP(CGetUserIdentity)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetUserIdentity message handlers

void CGetUserIdentity::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CGetUserIdentity::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialog::OnHelpInfo(pHelpInfo);
	return false;
}
