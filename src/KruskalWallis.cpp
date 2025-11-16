// KruskalWallis.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "KruskalWallis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKruskalWallis dialog


CKruskalWallis::CKruskalWallis(CPatnDoc * pDoc,CWnd* pParent /*=NULL*/)
	: CPropertyPage(CKruskalWallis::IDD)
{
	//{{AFX_DATA_INIT(CKruskalWallis)
	m_kw = FALSE;
	m_patn_generated_groups = -1;
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
}


void CKruskalWallis::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKruskalWallis)
	DDX_Control(pDX, IDC_ADD_ALL, m_AddAll);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_REMOVE_ALL, m_RemoveAll);
	DDX_Control(pDX, IDC_REMOVE, m_Remove);
	DDX_Control(pDX, IDC_SELECTED, m_Selected);
	DDX_Control(pDX, IDC_ALL, m_All);
	DDX_Check(pDX, IDC_KW, m_kw);
	DDX_Radio(pDX, IDC_PATNGENERATEDGROUPS, m_patn_generated_groups);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKruskalWallis, CPropertyPage)
	//{{AFX_MSG_MAP(CKruskalWallis)
	ON_LBN_DBLCLK(IDC_ALL, OnDblclkAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_LBN_DBLCLK(IDC_SELECTED, OnDblclkSelected)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_KW, OnKw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKruskalWallis message handlers

void CKruskalWallis::OnDblclkAll() 
{
	OnAdd();	
}

void CKruskalWallis::OnAdd() 
{
	long i,j,k,size;
	CString str;

	//update m_KWAttributes
	for(i=0;i<m_All.GetCount();i++){
		if(m_All.GetSel(i)){
			m_pDoc->GetColumnLabel(i,str);
			if((j = m_Selected.FindString(-1,str)) == LB_ERR){
				size = m_KWAttributes.GetSize();
				for(k=0;k<size;k++){
					if(m_KWAttributes[k] > i){
						m_KWAttributes.InsertAt(k,i);
						k = size;
					}
				}
				if(k != size+1)	m_KWAttributes.Add(i);
			}
		}
	}		
	//update m_Selected
	m_Selected.ResetContent();
	for(i=0;i<m_KWAttributes.GetSize();i++){
		m_pDoc->GetColumnLabel(m_KWAttributes[i],str);
		m_Selected.AddString(str);
	}	
}

void CKruskalWallis::OnAddAll() 
{

	long i;
	CString str;
	m_KWAttributes.RemoveAll();
	m_Selected.ResetContent();
	for(i=0;i<m_pDoc->GetNoOfColumns();i++){
		m_pDoc->GetColumnLabel(i,str);
		m_Selected.AddString(str);
		m_KWAttributes.Add(i);
	}	
}

void CKruskalWallis::OnDblclkSelected() 
{
	OnRemove();	
}

void CKruskalWallis::OnRemoveAll() 
{
	m_Selected.ResetContent();
	m_KWAttributes.RemoveAll();		
}

void CKruskalWallis::OnRemove() 
{
	long i;
	CString str;

	for(i=m_Selected.GetCount()-1;i>=0;i--){
		if(m_Selected.GetSel(i)){
			m_Selected.DeleteString(i);
			m_KWAttributes.RemoveAt(i);
		}
	}	
}

BOOL CKruskalWallis::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//setup lists
	CString str;
	long i;
	for(i=0;i<m_pDoc->GetNoOfColumns();i++){
		m_pDoc->GetColumnLabel(i,str);
		m_All.AddString(str);
	}
	for(i=0;i<m_KWAttributes.GetSize();i++){
		m_pDoc->GetColumnLabel(m_KWAttributes[i],str);
		m_Selected.AddString(str);
	}

	//Now make everything disabled...
	if(!m_kw){
		m_Add.EnableWindow(false);
		m_AddAll.EnableWindow(false);
		m_Remove.EnableWindow(false);
		m_RemoveAll.EnableWindow(false);
		m_Selected.EnableWindow(false);
		m_All.EnableWindow(false);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKruskalWallis::OnKw() 
{
	UpdateData(true);
	
	if(m_kw){
		m_Add.EnableWindow(true);
		m_AddAll.EnableWindow(true);
		m_Remove.EnableWindow(true);
		m_RemoveAll.EnableWindow(true);
		m_Selected.EnableWindow(true);
		m_All.EnableWindow(true);
	}else{
		m_Add.EnableWindow(false);
		m_AddAll.EnableWindow(false);
		m_Remove.EnableWindow(false);
		m_RemoveAll.EnableWindow(false);
		m_Selected.EnableWindow(false);
		m_All.EnableWindow(false);
	}
}
