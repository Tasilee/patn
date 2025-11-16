// PickFuseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PickFuseDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickFuseTableDlg dialog


CPickFuseTableDlg::CPickFuseTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPickFuseTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPickFuseTableDlg)
	m_nDataType = ROWS;
	//}}AFX_DATA_INIT
}


void CPickFuseTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickFuseTableDlg)
	DDX_Radio(pDX, IDC_COLUMNS, m_nDataType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPickFuseTableDlg, CDialog)
	//{{AFX_MSG_MAP(CPickFuseTableDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPickFuseTableDlg message handlers
