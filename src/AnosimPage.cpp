// AnosimPage.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "AnosimPage.h"
#include "IndividualGroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnosimPage dialog


CAnosimPage::CAnosimPage(CPatnDoc * pDoc,CWnd* pParent /*=NULL*/)
	: CPropertyPage(CAnosimPage::IDD)
{
	m_pDoc = pDoc;
	//{{AFX_DATA_INIT(CAnosimPage)
	m_cols = FALSE;
	m_cols_iterations = 0;
	m_cols_seed = 0;
	m_rows_iterations = 0;
	m_rows_seed = 0;
	m_rows = FALSE;
	m_cols_pairs = -1;
	m_rows_pairs = -1;
	m_patn_generated_groups = -1;
	//}}AFX_DATA_INIT
}


void CAnosimPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnosimPage)
	DDX_Control(pDX, IDC_SETROWGROUPS, m_SetRowGroups);
	DDX_Control(pDX, IDC_SETCOLGROUPS, m_SetColGroups);
	DDX_Control(pDX, IDC_ROW_SEED, m_RowSeed);
	DDX_Control(pDX, IDC_ROW_ITERATIONS, m_RowIterations);
	DDX_Control(pDX, IDC_COLUMN_ITERATIONS, m_ColumnIterations);
	DDX_Control(pDX, IDC_COLUMN_SEED, m_ColumnSeed);
	DDX_Check(pDX, IDC_COLUMNS, m_cols);
	DDX_Text(pDX, IDC_COLUMN_ITERATIONS, m_cols_iterations);
	DDX_Text(pDX, IDC_COLUMN_SEED, m_cols_seed);
	DDX_Text(pDX, IDC_ROW_ITERATIONS, m_rows_iterations);
	DDX_Text(pDX, IDC_ROW_SEED, m_rows_seed);
	DDX_Check(pDX, IDC_ROWS, m_rows);
	DDX_Radio(pDX, IDC_COLALLGROUPS, m_cols_pairs);
	DDX_Radio(pDX, IDC_ROWALLGROUPS, m_rows_pairs);
	DDX_Radio(pDX, IDC_PATNGENERATEDGROUPS, m_patn_generated_groups);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnosimPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAnosimPage)
	ON_BN_CLICKED(IDC_ROWS, OnRows)
	ON_BN_CLICKED(IDC_COLUMNS, OnColumns)
	ON_BN_CLICKED(IDC_ROWPAIRGROUPS, OnRowpairgroups)
	ON_BN_CLICKED(IDC_ROWALLGROUPS, OnRowallgroups)
	ON_BN_CLICKED(IDC_SETCOLGROUPS, OnSetcolgroups)
	ON_BN_CLICKED(IDC_SETROWGROUPS, OnSetrowgroups)
	ON_BN_CLICKED(IDC_COLPAIRGROUPS, OnColpairgroups)
	ON_BN_CLICKED(IDC_COLALLGROUPS, OnColallgroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnosimPage message handlers

BOOL CAnosimPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CButton * button;

   m_SetColGroups.ShowWindow(false);
   m_SetRowGroups.ShowWindow(false);
   
	if(m_cols){
		m_ColumnIterations.EnableWindow(true);
		m_ColumnSeed.EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_COLPAIRGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_COLALLGROUPS);
		button->EnableWindow(true);
		if(m_cols_pairs)
			m_SetColGroups.EnableWindow(true);
	}else{
		m_ColumnIterations.EnableWindow(false);
		m_ColumnSeed.EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_COLPAIRGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_COLALLGROUPS);
		button->EnableWindow(false);
		m_SetColGroups.EnableWindow(false);
	}

	if(m_rows){
		m_RowIterations.EnableWindow(true);
		m_RowSeed.EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_ROWPAIRGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_ROWALLGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_APRIORIGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_PATNGENERATEDGROUPS);
		button->EnableWindow(true);
		if(m_rows_pairs)
			m_SetRowGroups.EnableWindow(true);
	}else{
		m_RowIterations.EnableWindow(false);
		m_RowSeed.EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_ROWPAIRGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_ROWALLGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_APRIORIGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_PATNGENERATEDGROUPS);
		button->EnableWindow(false);
		m_SetRowGroups.EnableWindow(false);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnosimPage::OnRows() 
{
	UpdateData(true);

	CButton * button;

	if(m_rows){
		m_RowIterations.EnableWindow(true);
		m_RowSeed.EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_ROWPAIRGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_ROWALLGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_APRIORIGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_PATNGENERATEDGROUPS);
		button->EnableWindow(true);
		if(m_rows_pairs)
			m_SetRowGroups.EnableWindow(true);
	}else{
		m_RowIterations.EnableWindow(false);
		m_RowSeed.EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_ROWPAIRGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_ROWALLGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_APRIORIGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_PATNGENERATEDGROUPS);
		button->EnableWindow(false);
		m_SetRowGroups.EnableWindow(false);
	}
	
}

void CAnosimPage::OnColumns() 
{
	UpdateData(true);

	CButton * button;

	if(m_cols){
		m_ColumnIterations.EnableWindow(true);
		m_ColumnSeed.EnableWindow(true); 
		button = (CButton*)GetDlgItem(IDC_COLPAIRGROUPS);
		button->EnableWindow(true);
		button = (CButton*)GetDlgItem(IDC_COLALLGROUPS);
		button->EnableWindow(true);
		if(m_cols_pairs)
			m_SetColGroups.EnableWindow(true);
	}else{
		m_ColumnIterations.EnableWindow(false);
		m_ColumnSeed.EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_COLPAIRGROUPS);
		button->EnableWindow(false);
		button = (CButton*)GetDlgItem(IDC_COLALLGROUPS);
		button->EnableWindow(false);
		m_SetColGroups.EnableWindow(false);
	}
}

void CAnosimPage::OnRowpairgroups() 
{
	m_SetRowGroups.EnableWindow(true);
	
}

void CAnosimPage::OnRowallgroups() 
{
	m_SetRowGroups.EnableWindow(false);
}

void CAnosimPage::OnSetcolgroups()
{
	CIndividualGroups obj(m_pDoc,true,&m_cols_groups);

	obj.DoModal();
}

void CAnosimPage::OnSetrowgroups()
{
 	//CIndividualGroups obj(m_pDoc,false,&m_rows_groups);

	//obj.DoModal();
}

void CAnosimPage::OnColpairgroups() 
{
	m_SetColGroups.EnableWindow(true);
	
}

void CAnosimPage::OnColallgroups() 
{
	m_SetColGroups.EnableWindow(false);
}
