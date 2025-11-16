// exponent.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "exponent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExponentDlg dialog


CExponentDlg::CExponentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExponentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExponentDlg)
	m_nExponent = 0.5;
	//}}AFX_DATA_INIT
}

void CExponentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExponentDlg)
	DDX_Text(pDX, IDC_EXPONENT, m_nExponent);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExponentDlg, CDialog)
	//{{AFX_MSG_MAP(CExponentDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExponentDlg message handlers
