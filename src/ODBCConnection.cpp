// ODBCConnection.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ODBCConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CODBCConnection dialog


CODBCConnection::CODBCConnection(CWnd* pParent /*=NULL*/)
	: CDialog(CODBCConnection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CODBCConnection)
	m_col_label_field = _T("");
	m_data_field = _T("");
	m_dsn = _T("");
	m_row_label_field = _T("");
	m_table_name = _T("");
	//}}AFX_DATA_INIT
}


void CODBCConnection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CODBCConnection)
	DDX_Control(pDX, IDC_DSN, m_disable);
	DDX_Text(pDX, IDC_COL_LABEL_FIELD, m_col_label_field);
	DDX_Text(pDX, IDC_DATA_FIELD, m_data_field);
	DDX_Text(pDX, IDC_DSN, m_dsn);
	DDX_Text(pDX, IDC_ROW_LABEL_FIELD, m_row_label_field);
	DDX_Text(pDX, IDC_TABLE_NAME, m_table_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CODBCConnection, CDialog)
	//{{AFX_MSG_MAP(CODBCConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODBCConnection message handlers

BOOL CODBCConnection::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_dsn == "search"){
		m_dsn = "";
		m_disable.EnableWindow(FALSE);
	}else{
		m_disable.EnableWindow(TRUE);
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
