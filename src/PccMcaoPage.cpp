// PccMcaoPage.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PccMcaoPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPccMcaoPage dialog


CPccMcaoPage::CPccMcaoPage(CPatnDoc * pDoc,CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPccMcaoPage::IDD)
{
	//{{AFX_DATA_INIT(CPccMcaoPage)
	m_mcao = FALSE;
	m_pcc = FALSE;
	m_mcao_seed = 0;
	m_mcao_iterations = 0;
	//}}AFX_DATA_INIT

	m_pDoc = pDoc;
}


void CPccMcaoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPccMcaoPage)
	DDX_Control(pDX, IDC_MCAO_ITERATIONS, m_McaoIterations);
	DDX_Control(pDX, IDC_MCAO_SEED, m_McaoSeed);
	DDX_Control(pDX, IDC_REMOVE_ALL, m_RemoveAll);
	DDX_Control(pDX, IDC_REMOVE, m_Remove);
	DDX_Control(pDX, IDC_ADD_ALL, m_AddAll);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_ALLCOLS, m_All);
	DDX_Control(pDX, IDC_SELECTEDCOLS, m_Selected);
	DDX_Check(pDX, IDC_MCAO, m_mcao);
	DDX_Check(pDX, IDC_PCC, m_pcc);
	DDX_Text(pDX, IDC_MCAO_SEED, m_mcao_seed);
	DDX_Text(pDX, IDC_MCAO_ITERATIONS, m_mcao_iterations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPccMcaoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPccMcaoPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_LBN_DBLCLK(IDC_ALLCOLS, OnDblclkAllcols)
	ON_LBN_SELCHANGE(IDC_ALLCOLS, OnSelchangeAllcols)
	ON_BN_CLICKED(IDC_MCAO, OnMcao)
	ON_BN_CLICKED(IDC_PCC, OnPcc)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_LBN_SELCHANGE(IDC_SELECTEDCOLS, OnSelchangeSelectedcols)
	ON_LBN_DBLCLK(IDC_SELECTEDCOLS, OnDblclkSelectedcols)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPccMcaoPage message handlers

void CPccMcaoPage::OnAdd() 
{
	UpdateData();
	long nSelCount = m_All.GetSelCount();
	CArray<int> aItems;
	CString str;
	
	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_All.GetSelItems(nSelCount, &aItems[0]);
	}
	
	long i=0, j=0;
	while ( i < aItems.GetSize() && j < m_pcc_indexes.GetSize() )
	{
		if ( aItems[i] == m_pcc_indexes[j] )  // Already in selected list
		{
			i++;
		}
		else if ( aItems[i] < m_pcc_indexes[j] )
		{
			m_pcc_indexes.InsertAt(j,aItems[i]);
			m_All.GetText(aItems[i],str);
			m_Selected.InsertString(j,str);
			i++;
			j++;
		}
		else
			j++;
	}
	while ( i < aItems.GetSize() )
	{
		m_pcc_indexes.Add(aItems[i]);
		m_All.GetText(aItems[i],str);
		m_Selected.AddString(str);
		i++;
	}
	
	m_All.SetCurSel(-1);

	if ( m_Selected.GetCount() > 0 )
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
	}
		
	UpdateData(FALSE);	
}

void CPccMcaoPage::OnAddAll() 
{
	long i;
	CString str;
	
	m_Selected.ResetContent();
	m_pcc_indexes.RemoveAll();
	
	for ( i = 0; i < m_All.GetCount(); i++ )
	{
		m_All.GetText(i,str);
		m_Selected.AddString(str);
		m_pcc_indexes.Add(i);
	}
	
	m_All.SetCurSel(-1);

	if ( m_Selected.GetCount() > 0 )
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL && !pButton->IsWindowEnabled() )
			pButton->EnableWindow(TRUE);
	}
		
	UpdateData(FALSE);	
}

void CPccMcaoPage::OnDblclkAllcols() 
{
	OnAdd();	
}

void CPccMcaoPage::OnSelchangeAllcols() 
{
	// TODO: Add your control notification handler code here
	
}

void CPccMcaoPage::OnMcao() 
{
	UpdateData(true);
	
	if(m_mcao){
		m_McaoIterations.EnableWindow(true);
		m_McaoSeed.EnableWindow(true);
	}else{
		m_McaoIterations.EnableWindow(false);
		m_McaoSeed.EnableWindow(false);

	}
}

void CPccMcaoPage::OnPcc() 
{
	UpdateData(true);

	if(m_pcc){
		m_Selected.EnableWindow(true);
		m_All.EnableWindow(true);
		m_Add.EnableWindow(true);
		m_AddAll.EnableWindow(true);
		m_Remove.EnableWindow(true);
		m_RemoveAll.EnableWindow(true);
	}else{
		m_Selected.EnableWindow(false);
		m_All.EnableWindow(false);
		m_Add.EnableWindow(false);
		m_AddAll.EnableWindow(false);
		m_Remove.EnableWindow(false);
		m_RemoveAll.EnableWindow(false);
	}	
}

void CPccMcaoPage::OnRemove() 
{
	long nSelCount = m_Selected.GetSelCount();
	CArray<int> aItems;
	
	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_Selected.GetSelItems(nSelCount, &aItems[0]);
		     
		long i;
		for ( i = (long)aItems.GetSize()-1; i >= 0 ; i-- )
		{
			m_pcc_indexes.RemoveAt(aItems[i]);
			m_Selected.DeleteString(aItems[i]);
		}
	}
	
	m_Selected.SetCurSel(-1);

	if ( m_Selected.GetCount() == 0 )
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
	}
	
	UpdateData(FALSE);}

void CPccMcaoPage::OnRemoveAll() 
{
	m_Selected.ResetContent();
	m_pcc_indexes.RemoveAll();
	
	CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
	if ( pButton != NULL )
		pButton->EnableWindow(FALSE);
	pButton = (CButton*)GetDlgItem(IDC_REMOVE);
	if ( pButton != NULL )
		pButton->EnableWindow(FALSE);
	pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
	if ( pButton != NULL )
		pButton->EnableWindow(FALSE);
		
	UpdateData(FALSE);
}

void CPccMcaoPage::OnSelchangeSelectedcols() 
{
	// TODO: Add your control notification handler code here
	
}

void CPccMcaoPage::OnDblclkSelectedcols() 
{
	OnRemove();		
}

void CPccMcaoPage::FillList()
{
	long		i;
    CString	strLabel;
	long		nCount = m_pDoc->GetNoOfColumns();

	for ( i = 0; i < nCount; i++ )
    {
		m_pDoc->GetColumnLabel(i, strLabel);
		m_All.AddString(strLabel);
    }	
}

BOOL CPccMcaoPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	long i;
//	long nSelCount;
	CArray<int> aItems;
    CString strLabel;
    
	//setup current selection as per m_pcc_indexes	
	for(i=0;i<m_pcc_indexes.GetSize();i++){
		m_pDoc->GetColumnLabel(m_pcc_indexes[i], strLabel);
		m_Selected.AddString(strLabel);
	}
		
	FillList();
	m_All.SetCurSel(-1);
	UpdateData(FALSE);
	
	if ( m_Selected.GetCount() == 0 )
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
	}
	else
		m_Selected.SetCurSel(0);

	if(m_mcao){
		m_McaoIterations.EnableWindow(true);
		m_McaoSeed.EnableWindow(true);
	}else{
		m_McaoIterations.EnableWindow(false);
		m_McaoSeed.EnableWindow(false);

	}

	if(m_pcc){
		m_Selected.EnableWindow(true);
		m_All.EnableWindow(true);
		m_Add.EnableWindow(true);
		m_AddAll.EnableWindow(true);
		m_Remove.EnableWindow(true);
		m_RemoveAll.EnableWindow(true);
	}else{
		m_Selected.EnableWindow(false);
		m_All.EnableWindow(false);
		m_Add.EnableWindow(false);
		m_AddAll.EnableWindow(false);
		m_Remove.EnableWindow(false);
		m_RemoveAll.EnableWindow(false);
	}
	
	return TRUE;  // return TRUE  unless you set the foc
}
