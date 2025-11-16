// KWOptions.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "KWOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKWOptions dialog


CKWOptions::CKWOptions(CPatnDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CKWOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKWOptions)
	m_Cutoff = 0.0;
	m_NumVisible = 0;
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
}


void CKWOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKWOptions)
	DDX_Control(pDX, IDC_NUMVISIBLE, m_Visible);
	DDX_Control(pDX, IDC_CUTOFF, m_Cut);
	DDX_Control(pDX, IDC_NUMVISIBLESPIN, m_Spin);
	DDX_Control(pDX, IDC_COLUMNS, m_ColumnList);
	DDX_Text(pDX, IDC_CUTOFF, m_Cutoff);
	DDX_Text(pDX, IDC_NUMVISIBLE, m_NumVisible);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKWOptions, CDialog)
	//{{AFX_MSG_MAP(CKWOptions)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKWOptions message handlers



void CKWOptions::OnOK() 
{
	//write back option
	UpdateData();

	m_pDoc->m_KWCutoff = m_Cutoff;
	
	m_pDoc->m_KWNumVisible = m_NumVisible;
	
	m_pDoc->m_KWVisible.RemoveAll();
	m_pDoc->m_KWVisible.SetSize(m_ColumnList.GetCount());
	long i;
	int *items;
	items = new int[m_ColumnList.GetCount()];
	for(i=0;i<m_ColumnList.GetCount();i++)
		items[i] = -1;
	m_ColumnList.GetSelItems(m_ColumnList.GetCount(),items);
	for(i=0;i<m_ColumnList.GetSelCount();i++)
		m_pDoc->m_KWVisible.SetAt(items[i],1);
	
	if(m_ColumnList.GetCount() > 0) delete items;
	
	CDialog::OnOK();
}

BOOL CKWOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	long i,idx;
	CString string;
	CString tmp;

	//if no columns selected, assume that they all should be selected.
	if(m_pDoc->m_KWAttributes.GetSize() == 0){
		for(i=0;i<m_pDoc->GetNoOfColumns()&& (m_pDoc->GetIdxLeftExtrinsicColumn() < 0 ||
			i<m_pDoc->GetIdxLeftExtrinsicColumn());i++){
			m_pDoc->m_KWAttributes.Add(i);
		}
	}

	//initialize column list
	for(i=0;i<m_pDoc->m_KW.GetSize();i++){
		idx = m_pDoc->m_KWAttributes[m_pDoc->m_KW[i]];
		m_pDoc->GetColumnLabel(idx,string);
		tmp.Format("%3.4f  %s",m_pDoc->m_KWValues[i],string);
		m_ColumnList.AddString(tmp);
	}
	
	for(i=0;i<m_pDoc->m_KW.GetSize();i++){
		if(m_pDoc->m_KWVisible[i])
			m_ColumnList.SetSel(i);		
	}

	//initialize cutoff box
	m_Cutoff = m_pDoc->m_KWCutoff;
	string.Format("%f",m_pDoc->m_KWCutoff);
	m_Cut.SetWindowText(string);
	
	//initialize # visible box
	m_NumVisible = m_pDoc->m_KWNumVisible;
	m_Spin.SetPos(m_pDoc->m_KWNumVisible);

	UpdateData();

	CButton * button;

	//select previous option (1,2,3)
	switch(m_pDoc->m_KWOption){
	case 1:
		button = (CButton*) GetDlgItem(IDC_RADIO1);
		button->SetCheck(1);
		m_ColumnList.EnableWindow(true);
		m_Spin.EnableWindow(false);
		m_Visible.EnableWindow(false);
		m_Cut.EnableWindow(false);
		break;
	case 2:
		button = (CButton*) GetDlgItem(IDC_RADIO2);
		button->SetCheck(2);
		m_ColumnList.EnableWindow(false);
		m_Spin.EnableWindow(false);
		m_Visible.EnableWindow(false);
		m_Cut.EnableWindow(true);
		break;
	default:
		button = (CButton*) GetDlgItem(IDC_RADIO3);
		button->SetCheck(3);
		m_ColumnList.EnableWindow(false);
		m_Spin.EnableWindow(true);
		m_Visible.EnableWindow(true);
		m_Cut.EnableWindow(false);	
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKWOptions::OnRadio1() 
{
	m_pDoc->m_KWOption = 1;	
	m_ColumnList.EnableWindow(true);
	m_Spin.EnableWindow(false);
	m_Visible.EnableWindow(false);
	m_Cut.EnableWindow(false);
}

void CKWOptions::OnRadio2() 
{
	m_pDoc->m_KWOption = 2;	
	m_ColumnList.EnableWindow(false);
	m_Spin.EnableWindow(false);
	m_Visible.EnableWindow(false);
	m_Cut.EnableWindow(true);	
}

void CKWOptions::OnRadio3() 
{
	m_pDoc->m_KWOption = 3;	
	m_ColumnList.EnableWindow(false);
	m_Spin.EnableWindow(true);
	m_Visible.EnableWindow(true);
	m_Cut.EnableWindow(false);
}
