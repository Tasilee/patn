// PropertiesGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesGeneral dialog


CPropertiesGeneral::CPropertiesGeneral(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPropertiesGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesGeneral)
	m_tablename = _T("");
	m_missingvalue = 0.0;
	//}}AFX_DATA_INIT
}


void CPropertiesGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesGeneral)
	DDX_Text(pDX, IDC_TABLENAME, m_tablename);
	DDX_Text(pDX, IDC_MISSINGVALUE, m_missingvalue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesGeneral)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesGeneral message handlers
