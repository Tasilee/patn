// PickAnosim.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PickAnosim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickAnosim dialog


CPickAnosim::CPickAnosim(CWnd* pParent /*=NULL*/)
	: CDialog(CPickAnosim::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPickAnosim)
	m_columns = 0;
	//}}AFX_DATA_INIT
}


void CPickAnosim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickAnosim)
	DDX_Radio(pDX, IDC_ROW, m_columns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPickAnosim, CDialog)
	//{{AFX_MSG_MAP(CPickAnosim)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickAnosim message handlers
