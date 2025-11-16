// PropertiesOrdinationView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesOrdinationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesOrdinationView dialog


CPropertiesOrdinationView::CPropertiesOrdinationView(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPropertiesOrdinationView::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesOrdinationView)
	m_ordtype = -1;
	//}}AFX_DATA_INIT
}


void CPropertiesOrdinationView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesOrdinationView)
	DDX_Radio(pDX, IDC_ORDTYPE, m_ordtype);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesOrdinationView, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesOrdinationView)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesOrdinationView message handlers
