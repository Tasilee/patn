// AttributeSelection.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "AttributeSelection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeSelection dialog


CAttributeSelection::CAttributeSelection(CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeSelection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttributeSelection)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAttributeSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributeSelection)
	DDX_Control(pDX, IDC_LABELS, m_Labels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttributeSelection, CDialog)
	//{{AFX_MSG_MAP(CAttributeSelection)
	ON_LBN_SELCHANGE(IDC_LABELS, OnSelchangeLabels)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeSelection message handlers

BOOL CAttributeSelection::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString text;
	
	long nCols;
	//if((nCols=m_pDoc->GetIdxLeftExtrinsicColumn()) == -1)
		nCols = m_pDoc->GetNoOfColumns();

	for(long i=0;i<nCols;i++){
		m_pDoc->GetColumnLabel(i,text);
		m_Labels.AddString(text);
	}
	m_Labels.SetCurSel(m_attrib);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttributeSelection::OnSelchangeLabels() 
{
	
}

void CAttributeSelection::OnOK() 
{
	m_attrib = m_Labels.GetCurSel();
	CDialog::OnOK();
	RedrawWindow();
}
