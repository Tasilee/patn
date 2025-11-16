// SelectWorksheet.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "SelectWorksheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheet dialog


CSelectWorksheet::CSelectWorksheet(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectWorksheet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectWorksheet)
	//}}AFX_DATA_INIT
}


void CSelectWorksheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectWorksheet)
	DDX_Control(pDX, IDC_TABLES, m_tables);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectWorksheet, CDialog)
	//{{AFX_MSG_MAP(CSelectWorksheet)
	ON_LBN_DBLCLK(IDC_TABLES, OnDblclkTables)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheet message handlers

void CSelectWorksheet::OnDblclkTables() 
{
	OnOK();
	
}

void CSelectWorksheet::OnOK() 
{
	m_selectedidx = m_tables.GetCurSel();
	
	CDialog::OnOK();
}

BOOL CSelectWorksheet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(long i=0;i<tables.GetSize();i++){
		m_tables.AddString(tables.GetAt(i));
	}

	m_selectedidx = 0;
	m_tables.SetCurSel(m_selectedidx);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
