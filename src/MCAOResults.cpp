// MCAOResults.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "MCAOResults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCAOResults dialog


CMCAOResults::CMCAOResults(CPatnDoc * pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CMCAOResults::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMCAOResults)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
}


void CMCAOResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCAOResults)
	DDX_Control(pDX, IDC_COLUMNS, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCAOResults, CDialog)
	//{{AFX_MSG_MAP(CMCAOResults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCAOResults message handlers

BOOL CMCAOResults::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//calculate values from m_pDoc->m_MCAOTable...

	long i,j;
	double total,value;
	CString strBuffer,strLabel;

	if(m_pDoc->m_MCAOTable.GetSize() > m_pDoc->m_Vectors.GetSize()) 
		return false;

	for(i=0;i<m_pDoc->m_MCAOTable.GetSize();i++){
		if(m_pDoc->m_Vectors[i]->GetAt(0) == ANALYSIS_VALUE) 
			continue;
		value = m_pDoc->m_Vectors[i]->GetAt(m_pDoc->m_Vectors[i]->GetSize()-1);
		for(j=0,total=0;j<m_pDoc->m_MCAOTable[i]->GetSize();j++){
			if(m_pDoc->m_MCAOTable[i]->GetAt(j) > value){
				total++;
			}	
		}
		total /= (double)(m_pDoc->m_MCAOTable[i]->GetSize());

		m_pDoc->GetColumnLabel(i,strLabel);
		strBuffer.Format("%3.2f%%   %s",total*100.0,strLabel);

		m_List.AddString(strBuffer);
	}



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
