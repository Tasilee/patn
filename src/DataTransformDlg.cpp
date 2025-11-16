// DataTransformDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "TableView.h"
#include "DataTransformDlg.h"
#include "logconst.h"
#include "exponent.h"
#include "nonzeroc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataTransformDlg dialog


CDataTransformDlg::CDataTransformDlg(CPatnDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDataTransformDlg::IDD, pParent)
{
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTableView)) );
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPatnDoc)) );

	m_pView = (CTableView*)pParent;
	m_pDoc = pDoc;
	//{{AFX_DATA_INIT(CDataTransformDlg)
	m_nDataType = 0;
	//}}AFX_DATA_INIT
}

BOOL CDataTransformDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	long i;
	long nSelCount;
	CArray<int> aItems;
    CString strLabel;
    
	m_lbOptions.AddString("Value**Constant");
	m_lbOptions.AddString("Log base 10");	
	m_lbOptions.AddString("Log base e");
	m_lbOptions.AddString("Non zeros to a constant value");

	m_lbOptions.AddString("(Value - Minimum Value) / Range of Value");	
	m_lbOptions.AddString("Value / Sum of Values");
	m_lbOptions.AddString("Value / Square root of the sum of the squares of the Values");
	m_lbOptions.SetCurSel(-1);
	
	// if any columns/rows are already selected, then add them to the 'selected' list box
	if ( !m_pView->m_pSelectedColumns->IsEmpty() )
	{
		m_nDataType = columns;
		m_pView->m_pSelectedColumns->GetItems(&aItems,TRUE /*sort*/);
		nSelCount = (long)aItems.GetSize();
	    for ( i = 0; i < nSelCount; i++ )
	    {
	    	m_pDoc->GetColumnLabel(aItems[i], strLabel);
			m_lbSelected.AddString(strLabel);
			m_aSelectedItems.Add(aItems[i]);
	    }
	}
	else if ( !m_pView->m_pSelectedRows->IsEmpty() )
	{
		m_nDataType = rows;
		m_pView->m_pSelectedRows->GetItems(&aItems,TRUE /*sort*/);
		nSelCount = (long)aItems.GetSize();
	    for ( i = 0; i < nSelCount; i++ )
	    {
	    	m_pDoc->GetRowLabel(aItems[i], strLabel);
			m_lbSelected.AddString(strLabel);
			m_aSelectedItems.Add(aItems[i]);
	    }
	}
		
	FillList();
	m_lbAll.SetCurSel(-1);
	UpdateData(FALSE);
	
	if ( m_lbSelected.GetCount() == 0 )
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
		m_lbSelected.SetCurSel(0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDataTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataTransformDlg)
	DDX_Control(pDX, IDC_SELECTED, m_lbSelected);
	DDX_Control(pDX, IDC_ALL, m_lbAll);
	DDX_Control(pDX, IDC_OPTIONS, m_lbOptions);
	DDX_Radio(pDX, IDC_COLUMNS, m_nDataType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDataTransformDlg, CDialog)
	//{{AFX_MSG_MAP(CDataTransformDlg)
	ON_BN_CLICKED(IDC_COLUMNS, OnColumns)
	ON_BN_CLICKED(IDC_ROWS, OnRows)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_LBN_DBLCLK(IDC_ALL, OnDblclkAll)
	ON_LBN_DBLCLK(IDC_SELECTED, OnDblclkSelected)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDataTransformDlg message handlers

void CDataTransformDlg::OnColumns()
{
	ResetList();
}

void CDataTransformDlg::OnRows()
{
	ResetList();
}

void CDataTransformDlg::ResetList()
{
	long nOldDataType = m_nDataType;
	UpdateData(TRUE);
	if ( m_nDataType != nOldDataType )
	{
		m_lbAll.ResetContent();
		m_lbSelected.ResetContent();
		m_aSelectedItems.RemoveAll();
		m_lbAll.SetCurSel(-1);
		m_lbOptions.SetCurSel(-1);
		
		CButton* pButton = (CButton*)GetDlgItem(IDC_RUN);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(FALSE);
			
		FillList();
		UpdateData(FALSE);
	}
}

void CDataTransformDlg::FillList()
{
	switch (m_nDataType)
	{
		case 0:
			FillWithColumnLabels();
			break;
			
		case 1:
			FillWithRowLabels();
			break;
			
		default:
			break;
	}
}

void CDataTransformDlg::FillWithColumnLabels()
{   
	long		i;
    CString	strLabel;
	long		nCount = m_pDoc->GetNoOfColumns();
    
    for ( i = 0; i < nCount; i++ )
    {
		m_pDoc->GetColumnLabel(i, strLabel);
		m_lbAll.AddString(strLabel);
    }
}

void CDataTransformDlg::FillWithRowLabels()
{   
	long		i;
    CString	strLabel;
	long		nCount = m_pDoc->GetNoOfRows();
    
    for ( i = 0; i < nCount; i++ )
    {
		m_pDoc->GetRowLabel(i, strLabel);
		m_lbAll.AddString(strLabel);
    }
}

void CDataTransformDlg::OnAdd()
{
	long nSelCount = m_lbAll.GetSelCount();
	CArray<int> aItems;
	CString str;
	
	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_lbAll.GetSelItems(nSelCount, &aItems[0]);
	}
	
	long i=0, j=0;
	while ( i < aItems.GetSize() && j < m_aSelectedItems.GetSize() )
	{
		if ( aItems[i] == m_aSelectedItems[j] )  // Already in selected list
		{
			i++;
		}
		else if ( aItems[i] < m_aSelectedItems[j] )
		{
			m_aSelectedItems.InsertAt(j,aItems[i]);
			m_lbAll.GetText(aItems[i],str);
			m_lbSelected.InsertString(j,str);
			i++;
			j++;
		}
		else
			j++;
	}
	while ( i < aItems.GetSize() )
	{
		m_aSelectedItems.Add(aItems[i]);
		m_lbAll.GetText(aItems[i],str);
		m_lbSelected.AddString(str);
		i++;
	}
	
	m_lbAll.SetCurSel(-1);

	if ( m_lbSelected.GetCount() > 0 )
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

void CDataTransformDlg::OnAddAll()
{
	long i;
	CString str;
	
	m_lbSelected.ResetContent();
	m_aSelectedItems.RemoveAll();
	
	for ( i = 0; i < m_lbAll.GetCount(); i++ )
	{
		m_lbAll.GetText(i,str);
		m_lbSelected.AddString(str);
		m_aSelectedItems.Add(i);
	}
	
	m_lbAll.SetCurSel(-1);

	if ( m_lbSelected.GetCount() > 0 )
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

void CDataTransformDlg::OnRemove()
{
	long nSelCount = m_lbSelected.GetSelCount();
	CArray<int> aItems;
	
	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_lbSelected.GetSelItems(nSelCount, &aItems[0]);
		     
		long i;
		for ( i = (long)aItems.GetSize()-1; i >= 0 ; i-- )
		{
			m_aSelectedItems.RemoveAt(aItems[i]);
			m_lbSelected.DeleteString(aItems[i]);
		}
	}
	
	m_lbSelected.SetCurSel(-1);

	if ( m_lbSelected.GetCount() == 0 )
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
	
	UpdateData(FALSE);
}

void CDataTransformDlg::OnRemoveAll()
{
	m_lbSelected.ResetContent();
	m_aSelectedItems.RemoveAll();
	
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

void CDataTransformDlg::OnRun()
{
    CLogConstDlg dlgLogConstant;
    double nLogConstant;
    CExponentDlg dlgExponent;
    double nExponent;
    CNonZeroConstantDlg dlgNonZeroConstant;
    double nNonZeroConstant;
	CDataTransform transform(m_pDoc);

	m_pDoc->m_loading = true;
	
	long nOption = m_lbOptions.GetCurSel();
	if ( nOption == LB_ERR )
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("You must select a data transformation option");
		return;
	}
	
	switch(nOption)
	{
		case 4:	// (Value - Min)/Range
			if ( m_nDataType == columns)
				transform.RangeColumns(&m_aSelectedItems);
			else if ( m_nDataType == rows)
				transform.RangeRows(&m_aSelectedItems);
			break;
		
		case 2:	// Log base e
			if ( dlgLogConstant.DoModal() == IDOK )
			{
				nLogConstant = dlgLogConstant.m_nLogConstant;
				if ( m_nDataType == columns)
					transform.LogeColumns(&m_aSelectedItems,nLogConstant);
				else if ( m_nDataType == rows)
					transform.LogeRows(&m_aSelectedItems,nLogConstant);
			}
			break;
			
		case 1:	// Log base 10
			if ( dlgLogConstant.DoModal() == IDOK )
			{
				nLogConstant = dlgLogConstant.m_nLogConstant;
				if ( m_nDataType == columns)
					transform.Log10Columns(&m_aSelectedItems,nLogConstant);
				else if ( m_nDataType == rows)
					transform.Log10Rows(&m_aSelectedItems,nLogConstant);
			}
			break;
			
		case 0:	// Value**Constant
			if ( dlgExponent.DoModal() == IDOK )
			{
				nExponent = dlgExponent.m_nExponent;
				if ( m_nDataType == columns)
					transform.PowerColumns(&m_aSelectedItems,nExponent);
				else if ( m_nDataType == rows)
					transform.PowerRows(&m_aSelectedItems,nExponent);
			}
			break;
		case 3:	
			if ( dlgNonZeroConstant.DoModal() == IDOK )
			{
				nNonZeroConstant = dlgNonZeroConstant.m_nNonZeroConstant;
				if ( m_nDataType == columns)
					transform.NonZeroConstantColumns(&m_aSelectedItems,nNonZeroConstant);
				else if ( m_nDataType == rows)
					transform.NonZeroConstantRows(&m_aSelectedItems,nNonZeroConstant);
			}
			break;
		case 5: // Dij / Sum i,j
			if ( m_nDataType == columns)
				transform.SumCols(&m_aSelectedItems);
			else if ( m_nDataType == rows)
				transform.SumRows(&m_aSelectedItems);
			break;

		case 6: // Dij / SQRT (SOS i,j)
			if ( m_nDataType == columns)
				transform.SqrtSosCols(&m_aSelectedItems);
			else if ( m_nDataType == rows)
				transform.SqrtSosRows(&m_aSelectedItems);
			break;
	}
	
	m_pDoc->m_loading = false;	
	m_pDoc->UpdateAllTotals();
	m_pDoc->UpdateIntrinsicTotals();

	if(!m_pDoc->m_TwayChange ||
		!m_pDoc->m_AssocRowChange ||
		!m_pDoc->m_AssocColChange ||
		!m_pDoc->m_FuseRowChange ||
		!m_pDoc->m_FuseColChange ||
		!m_pDoc->m_OrdChange)
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("This has invalidated anaysis data",MB_OK | MB_ICONSTOP );
		{
			//Graphs invalid
			m_pDoc->m_TwayChange = 1;
			m_pDoc->m_AssocRowChange = 1;
			m_pDoc->m_AssocColChange = 1;
			m_pDoc->m_FuseRowChange = 1;
			m_pDoc->m_FuseColChange = 1;
			m_pDoc->m_OrdChange = 1;
		}
	}

	m_lbOptions.SetCurSel(-1);
	//SetDefID(IDC_CLOSE);
	EndDialog(IDCANCEL);	
}

void CDataTransformDlg::OnDblclkAll()
{
	OnAdd();
}

void CDataTransformDlg::OnDblclkSelected()
{
	OnRemove();	
}

void CDataTransformDlg::OnClose() 
{
	EndDialog(IDCANCEL);
}
