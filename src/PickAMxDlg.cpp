// pickam.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PickAMxDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickAssocMatrixDlg dialog


CPickAssocMatrixDlg::CPickAssocMatrixDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPickAssocMatrixDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPickAssocMatrixDlg)
	m_nDataType = ROWS;
	//}}AFX_DATA_INIT
}


void CPickAssocMatrixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickAssocMatrixDlg)
	DDX_Radio(pDX, IDC_COLUMNS, m_nDataType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPickAssocMatrixDlg, CDialog)
	//{{AFX_MSG_MAP(CPickAssocMatrixDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPickAssocMatrixDlg message handlers
