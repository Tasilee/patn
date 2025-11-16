// KWThreashold.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "KWThreashold.h"


// CKWThreashold dialog

IMPLEMENT_DYNAMIC(CKWThreashold, CDialog)
CKWThreashold::CKWThreashold(CPatnDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CKWThreashold::IDD, pParent)
	, m_KWThreashold(0)
{
	m_pDoc = pDoc;
}

CKWThreashold::~CKWThreashold()
{
}

void CKWThreashold::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_KWThreashold);
	DDX_Control(pDX, IDC_COLUMNS, m_list);
}


BEGIN_MESSAGE_MAP(CKWThreashold, CDialog)
END_MESSAGE_MAP()


// CKWThreashold message handlers
BOOL CKWThreashold::OnInitDialog() {
	
	CDialog::OnInitDialog();

	long i, idx;
	CString tmp,string;

	m_list.ResetContent();
	for(i=0;i<m_pDoc->m_KW.GetSize() && m_pDoc->m_KWAttributes.GetSize()>m_pDoc->m_KW[i];i++){
		idx = m_pDoc->m_KWAttributes[m_pDoc->m_KW[i]];
		m_pDoc->GetColumnLabel(idx,string);
		tmp.Format("%3.4f  %s",m_pDoc->m_KWValues[i],string);
		m_list.AddString(tmp);
	}

	return TRUE;
}
