// KWResults.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "KWResults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKWResults dialog


CKWResults::CKWResults(CPatnDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CKWResults::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKWResults)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
	m_values = true;
}


void CKWResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKWResults)
	DDX_Control(pDX, IDC_COLUMNS, m_Columns);
	DDX_Control(pDX, IDC_ORDERTABLE, m_ordertable);
	DDX_Control(pDX, IDC_ORDERVALUE, m_ordervalues);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKWResults, CDialog)
	//{{AFX_MSG_MAP(CKWResults)
	ON_BN_CLICKED(IDC_ORDERTABLE, OnOrdertable)
	ON_BN_CLICKED(IDC_ORDERVALUE, OnOrdervalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKWResults message handlers

BOOL CKWResults::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	long i,idx;
	CString string;
	CString tmp;

	//initialize column list
	for(i=0;i<m_pDoc->m_KW.GetSize();i++){
		idx = m_pDoc->m_KWAttributes[m_pDoc->m_KW[i]];
		m_pDoc->GetColumnLabel(idx,string);
		tmp.Format("%3.4f  %s",m_pDoc->m_KWValues[i],string);
		m_Columns.AddString(tmp);
	}

	m_ordertable.SetCheck(0);
	m_ordervalues.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKWResults::OnOrdertable() 
{
	CString string,tmp;
	long i,j;
	long idx;
	
	if(m_values){
		m_values = !m_values;
		m_ordertable.SetCheck(1);
		m_ordervalues.SetCheck(0);

		m_Columns.ResetContent();

		for(i=0;i<m_pDoc->m_KW.GetSize();i++){
			idx = m_pDoc->m_KWAttributes[i];
			m_pDoc->GetColumnLabel(idx,string);
			//find i in m_pDoc->m_KW(idx) & use idx
			for(j=0;j<m_pDoc->m_KW.GetSize();j++){
				if(i == m_pDoc->m_KW[j]) 
					break;
			}
			tmp.Format("%3.4f  %s",m_pDoc->m_KWValues[j],string);
			m_Columns.AddString(tmp);
		}
	}
}

void CKWResults::OnOrdervalue() 
{
	long i,idx;
	CString tmp,string;

	if(!m_values){
		m_values = !m_values;
		m_ordertable.SetCheck(0);
		m_ordervalues.SetCheck(1);

		m_Columns.ResetContent();
		for(i=0;i<m_pDoc->m_KW.GetSize() && m_pDoc->m_KWAttributes.GetSize()>m_pDoc->m_KW[i];i++){
			idx = m_pDoc->m_KWAttributes[m_pDoc->m_KW[i]];
			m_pDoc->GetColumnLabel(idx,string);
			tmp.Format("%3.4f  %s",m_pDoc->m_KWValues[i],string);
			m_Columns.AddString(tmp);
		}
	}
	
}
