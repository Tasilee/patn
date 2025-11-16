// TableProperties.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "TableProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableProperties dialog


CTableProperties::CTableProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CTableProperties::IDD, pParent)
{
	CPatnApp *pApp;
	pApp = (CPatnApp*)AfxGetApp();
	//{{AFX_DATA_INIT(CTableProperties)
	m_TableName = _T("");
	m_MissingValue = 0.0;
	//}}AFX_DATA_INIT
}


void CTableProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableProperties)
	DDX_Text(pDX, IDC_EDIT1, m_TableName);
	DDX_Text(pDX, IDC_EDIT2, m_MissingValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableProperties, CDialog)
	//{{AFX_MSG_MAP(CTableProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableProperties message handlers

