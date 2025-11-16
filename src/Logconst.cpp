// logconst.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "logconst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogConstDlg dialog

CLogConstDlg::CLogConstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogConstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogConstDlg)
	m_nLogConstant = 1.0;
	//}}AFX_DATA_INIT
}

void CLogConstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogConstDlg)
	DDX_Text(pDX, IDC_CONSTANT, m_nLogConstant);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogConstDlg, CDialog)
	//{{AFX_MSG_MAP(CLogConstDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogConstDlg message handlers


