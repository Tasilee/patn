// PccVectorSelection.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PccVectorSelection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPccVectorSelection dialog


CPccVectorSelection::CPccVectorSelection(CWnd* pParent /*=NULL*/)
	: CDialog(CPccVectorSelection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPccVectorSelection)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPccVectorSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPccVectorSelection)
	DDX_Control(pDX, IDC_LABELS, m_Labels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPccVectorSelection, CDialog)
	//{{AFX_MSG_MAP(CPccVectorSelection)
	ON_BN_CLICKED(IDC_SELECTNONE, OnSelectnone)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPccVectorSelection message handlers

BOOL CPccVectorSelection::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString text;
	
	long nCols = m_pDoc->GetNoOfColumns();
	long i,j;
	for(i=0;i<nCols;i++){
		if(m_pDoc->m_Vectors.GetAt(i)->GetAt(0) != ANALYSIS_VALUE){
			m_pDoc->GetColumnLabel(i,text);
			text.Format("%2.3f   %s",m_pDoc->m_Vectors.GetAt(i)->GetAt(
				m_pDoc->m_Vectors.GetAt(i)->GetSize()-1),text);
			
			m_Labels.AddString(text);
			for(j=0;j<m_PccVectors->GetSize();j++){
				if(m_PccVectors->GetAt(j) == i)
					m_Labels.SetSel(m_Labels.GetCount()-1);
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPccVectorSelection::OnOK() 
{
	long count = 0;
	long nCols = m_pDoc->GetNoOfColumns();

	m_PccVectors->RemoveAll();
	for(long i=0;i<nCols;i++){
		if(m_pDoc->m_Vectors.GetAt(i)->GetAt(0) != ANALYSIS_VALUE){
			if(m_Labels.GetSel(count)) m_PccVectors->Add(i);
			count++;
		}
	}
	
	CDialog::OnOK();
}

void CPccVectorSelection::OnSelectnone() 
{
	for(long i=0;i<m_pDoc->GetNoOfColumns();i++){
		m_Labels.SetSel(i,false);
	}
}

void CPccVectorSelection::OnSelectall() 
{
	for(long i=0;i<m_pDoc->GetNoOfColumns();i++){
		m_Labels.SetSel(i,true);
	}
}
