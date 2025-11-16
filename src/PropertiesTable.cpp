// PropertiesTable.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTable dialog


CPropertiesTable::CPropertiesTable(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertiesTable)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropertiesTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesTable)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesTable, CDialog)
	//{{AFX_MSG_MAP(CPropertiesTable)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTable message handlers
