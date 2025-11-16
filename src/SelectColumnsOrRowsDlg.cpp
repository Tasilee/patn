// SelectColumnsOrRowsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "TableView.h"
#include "SelectColumnsOrRowsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectColumnsOrRowsDlg dialog


CSelectColumnsOrRowsDlg::CSelectColumnsOrRowsDlg(CWnd* pParent, CPatnDoc* pDoc)
	: CDialog(CSelectColumnsOrRowsDlg::IDD, pParent)
{
	ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CTableView)));
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	m_pView = (CTableView*)pParent;
	m_pDoc = pDoc;
	//{{AFX_DATA_INIT(CSelectColumnsOrRowsDlg)
	m_nDataType = -1;
	//}}AFX_DATA_INIT
}


void CSelectColumnsOrRowsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectColumnsOrRowsDlg)
	DDX_Control(pDX, IDC_LB_SELECTED, m_lbSelected);
	DDX_Control(pDX, IDC_LB_ALL, m_lbAll);
	DDX_Control(pDX, IDC_BTN_REMOVE_ALL, m_btnRemoveAll);
	DDX_Control(pDX, IDC_BTN_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BTN_ADD_ALL, m_btnAddAll);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Radio(pDX, IDC_RADIO_COLUMNS, m_nDataType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectColumnsOrRowsDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectColumnsOrRowsDlg)
	ON_BN_CLICKED(IDC_BTN_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BTN_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ALL, OnRemoveAll)
	ON_LBN_DBLCLK(IDC_LB_ALL, OnDblclkAll)
	ON_LBN_DBLCLK(IDC_LB_SELECTED, OnDblclkSelected)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_RADIO_ROWS, OnRadioRows)
	ON_BN_CLICKED(IDC_RADIO_COLUMNS, OnRadioColumns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSelectColumnsOrRowsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	FillList();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// CSelectColumnsOrRowsDlg message handlers

void CSelectColumnsOrRowsDlg::OnRadioColumns()
{
	ResetList();
}

void CSelectColumnsOrRowsDlg::OnRadioRows()
{
	ResetList();
}

void CSelectColumnsOrRowsDlg::ResetList()
{
	BOOL bOldColumnsOrRows = m_nDataType;
	UpdateData(TRUE);
	if ( m_nDataType != bOldColumnsOrRows )
	{
		m_lbAll.ResetContent();
		m_lbSelected.ResetContent();
		m_aSelectedItems.RemoveAll();
		FillList();
	}
}

void CSelectColumnsOrRowsDlg::FillList()
{
	BeginWaitCursor();

	switch (m_nDataType)
	{
		case columns:
			FillWithColumnLabels();
			break;
			
		case rows:
			FillWithRowLabels();
			break;
			
		default:
			break;
	}

	m_lbAll.SetSel(-1,FALSE);
	m_lbSelected.SetSel(-1,FALSE);

	EndWaitCursor();
}

void CSelectColumnsOrRowsDlg::FillWithColumnLabels()
{   
	long		i;
    CString	strLabel;
	long		nSelCount;
	CArray<int> aItems;
    
	// fill 'all' list box with labels of all columns
    for ( i = 0; i < m_pDoc->GetNoOfColumns(); i++ )
    {
		m_pDoc->GetColumnLabel(i, strLabel);
		if ( strLabel.IsEmpty() )
			strLabel = "(no label)";
		m_lbAll.AddString(strLabel);
    }
    
	// fill 'selected' list box with labels of any already selected columns
	if ( !m_pView->m_pSelectedColumns->IsEmpty() )
	{
		m_pView->m_pSelectedColumns->GetItems(&aItems); // returns items in sorted array
		nSelCount = aItems.GetSize();
	    for ( i = 0; i < nSelCount; i++ )
	    {
			m_pDoc->GetColumnLabel(aItems[i], strLabel);
			if ( strLabel.IsEmpty() )
				strLabel = "(no label)";
			m_lbSelected.AddString(strLabel);
			m_aSelectedItems.Add(aItems[i]);
	    }
	}
}

void CSelectColumnsOrRowsDlg::FillWithRowLabels()
{   
	long		i;
    CString	strLabel;
	long		nSelCount;
	CArray<int> aItems;
    
	// fill 'all' list box with labels of all rows
    for ( i = 0; i < m_pDoc->GetNoOfRows(); i++ )
    {   
		m_pDoc->GetRowLabel(i, strLabel);
		if ( strLabel.IsEmpty() )
			strLabel = "(no label)";
		m_lbAll.AddString(strLabel);
    }

	// fill 'selected' list box with labels of any already selected rows
	if ( !m_pView->m_pSelectedRows->IsEmpty() )
	{
		m_pView->m_pSelectedRows->GetItems(&aItems); // returns items in sorted array
		nSelCount = aItems.GetSize();
	    for ( i = 0; i < nSelCount; i++ )
	    {
			m_pDoc->GetRowLabel(aItems[i], strLabel);
			if ( strLabel.IsEmpty() )
				strLabel = "(no label)";
			m_lbSelected.AddString(strLabel);
			m_aSelectedItems.Add(aItems[i]);
	    }
	}
}

void CSelectColumnsOrRowsDlg::OnAdd()
{
	long nSelCount = m_lbAll.GetSelCount(); // get number of selected items
	CArray<int> aItems;
	CString str;
	
	if ( nSelCount <= 0 )
		return;

	BeginWaitCursor();

	aItems.SetSize(nSelCount);
	m_lbAll.GetSelItems(nSelCount,&aItems[0]); // fill with item no's
	
	// insert highlighted items from 'all' list box into 'selected' list box
	int i=0, j=0;
	while ( i < nSelCount && j < m_aSelectedItems.GetSize() )
	{
		if ( aItems[i] < m_aSelectedItems[j] )
		{
			m_aSelectedItems.InsertAt(j,aItems[i]);
			m_lbAll.GetText(aItems[i],str);
			m_lbSelected.InsertString(j,str);
			i++;
			j++;
		}
		else if ( aItems[i] == m_aSelectedItems[j] )  // Already in selected list
		{
			i++;
		}
		else
			j++;
	}

	// add any remaining highlighted items to end of list in 'selected' list box
	while ( i < nSelCount )
	{
		m_aSelectedItems.Add(aItems[i]);
		m_lbAll.GetText(aItems[i],str);
		m_lbSelected.AddString(str);
		i++;
	}

	// if selecting columns, then clear selected rows and vice versa
	switch ( m_nDataType )
	{
		case columns:
			m_pView->m_pSelectedRows->RemoveAll();
			break;

		case rows:
			m_pView->m_pSelectedColumns->RemoveAll();
			break;
	}

	// update view & selections
	for ( i = 0; i < nSelCount; i++ )
	{
		switch ( m_nDataType )
		{
			case columns:
				m_pView->m_pSelectedColumns->SetAt(aItems[i]);
				break;

			case rows:
				m_pView->m_pSelectedRows->SetAt(aItems[i]);
				break;
		}
	}

	EndWaitCursor();
}

void CSelectColumnsOrRowsDlg::OnAddAll()
{
	long nCount = m_lbAll.GetCount();
	long i;
	CString str;

	if ( nCount <= 0 )
		return;
	
	BeginWaitCursor();

	switch ( m_nDataType )
	{
		case columns:
			m_pView->m_pSelectedRows->RemoveAll();
			break;

		case rows:
			m_pView->m_pSelectedColumns->RemoveAll();
			break;
	}

	m_lbSelected.ResetContent();
	m_aSelectedItems.RemoveAll();
	
	for ( i = 0; i < nCount; i++ )
	{
		m_lbAll.GetText(i,str);
		m_lbSelected.AddString(str);
		m_aSelectedItems.Add(i);

		// update view & selections
		switch ( m_nDataType )
		{
			case columns:
				m_pView->m_pSelectedColumns->SetAt(i);
				break;

			case rows:
				m_pView->m_pSelectedRows->SetAt(i);
				break;
		}
	}

	EndWaitCursor();
}

void CSelectColumnsOrRowsDlg::OnRemove()
{
	long nSelCount = m_lbSelected.GetSelCount();
	CArray<int> aItems;
	long i;
	
	if ( nSelCount <= 0 )
		return;

	BeginWaitCursor();

	// get the items in the 'selected' list box
	aItems.SetSize(nSelCount);
	m_lbSelected.GetSelItems(nSelCount,&aItems[0]);

	for ( i = aItems.GetUpperBound(); i >= 0 ; i-- )
	{
		// update view & selections
		switch ( m_nDataType )
		{
			case columns:
				m_pView->m_pSelectedColumns->RemoveAt(m_aSelectedItems[aItems[i]]);
				break;

			case rows:
				m_pView->m_pSelectedRows->RemoveAt(m_aSelectedItems[aItems[i]]);
				break;
		}

		m_aSelectedItems.RemoveAt(aItems[i]);
		m_lbSelected.DeleteString(aItems[i]);
	}

	EndWaitCursor();
}

void CSelectColumnsOrRowsDlg::OnRemoveAll()
{
	BeginWaitCursor();
	
	m_lbSelected.ResetContent();
	m_aSelectedItems.RemoveAll();

	// update view & selections
	switch ( m_nDataType )
	{
		case columns:
			m_pView->m_pSelectedColumns->RemoveAll();
			break;

		case rows:
			m_pView->m_pSelectedRows->RemoveAll();
			break;
	}

	EndWaitCursor();
}

void CSelectColumnsOrRowsDlg::OnDblclkAll()
{
	OnAdd();
}

void CSelectColumnsOrRowsDlg::OnDblclkSelected()
{
	OnRemove();	
}

void CSelectColumnsOrRowsDlg::OnBtnClose() 
{
	EndDialog(IDC_BTN_CLOSE);
}
