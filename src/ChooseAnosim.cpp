// ChooseAnosim.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ChooseAnosim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseAnosim dialog


CChooseAnosim::CChooseAnosim(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseAnosim::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseAnosim)
	m_Type = 0;
	//}}AFX_DATA_INIT
}


void CChooseAnosim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseAnosim)
	DDX_Radio(pDX, IDC_RADIO1, m_Type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseAnosim, CDialog)
	//{{AFX_MSG_MAP(CChooseAnosim)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseAnosim message handlers
