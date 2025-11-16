// nonzeroc.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "nonzeroc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNonZeroConstantDlg dialog


CNonZeroConstantDlg::CNonZeroConstantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNonZeroConstantDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNonZeroConstantDlg)
	m_nNonZeroConstant = 1;
	//}}AFX_DATA_INIT
}

void CNonZeroConstantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNonZeroConstantDlg)
	DDX_Text(pDX, IDC_CONSTANT, m_nNonZeroConstant);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNonZeroConstantDlg, CDialog)
	//{{AFX_MSG_MAP(CNonZeroConstantDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNonZeroConstantDlg message handlers
