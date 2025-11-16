// PageNewTableOptions.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "PageNewTableOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageNewTableOptions property page

IMPLEMENT_DYNCREATE(CPageNewTableOptions, CPropertyPage)

CPageNewTableOptions::CPageNewTableOptions() : CPropertyPage(CPageNewTableOptions::IDD)
{
	//{{AFX_DATA_INIT(CPageNewTableOptions)
	m_nNoOfColumns = 0;
	m_nNoOfRows = 0;
	m_nWhichInitialValues = -1;
	m_nWhichColumnWidth = -1;
	m_nDecimalPrecision = 0;
	m_nColumnWidth = 0.0;
	m_nMissingValue = 0.0;
	m_random_seed = 12345;
	//}}AFX_DATA_INIT
}

CPageNewTableOptions::~CPageNewTableOptions()
{
	
}

void CPageNewTableOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageNewTableOptions)
	DDX_Control(pDX, IDC_EDIT_COLUMN_WIDTH, m_editColumnWidth);
	DDX_Control(pDX, IDC_SPIN_DEC_PLACES, m_spinDecimalPrecision);
	DDX_Control(pDX, IDC_EDIT_MISSING_VALUE, m_MissingValue);
	DDX_Text(pDX, IDC_EDIT_NUM_COLUMNS, m_nNoOfColumns);
	DDX_Text(pDX, IDC_EDIT_NUM_ROWS, m_nNoOfRows);
	DDX_Radio(pDX, IDC_RADIO_INIT_CELLS, m_nWhichInitialValues);
	DDX_Radio(pDX, IDC_RADIO_STANDARD, m_nWhichColumnWidth);
	DDX_Text(pDX, IDC_EDIT_DECIMAL_PRECISION, m_nDecimalPrecision);
	DDV_MinMaxInt(pDX, m_nDecimalPrecision, 0, 6);
	DDX_Text(pDX, IDC_EDIT_COLUMN_WIDTH, m_nColumnWidth);
	DDV_MinMaxDouble(pDX, m_nColumnWidth, 0., 30.);
	DDX_Text(pDX, IDC_EDIT_MISSING_VALUE, m_nMissingValue);
	DDX_Text(pDX, IDC_RANDOM_SEED, m_random_seed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageNewTableOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CPageNewTableOptions)
	ON_BN_CLICKED(IDC_RADIO_STANDARD, OnRadioStandard)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, OnRadioCustom)
	ON_EN_CHANGE(IDC_EDIT_MISSING_VALUE, OnChangeEditMissingValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageNewTableOptions message handlers

void CPageNewTableOptions::OnRadioStandard() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_nColumnWidth = g_nStandardColumnWidth;
	m_editColumnWidth.EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CPageNewTableOptions::OnRadioCustom() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_editColumnWidth.EnableWindow(TRUE);
	GotoDlgCtrl(GetDlgItem(IDC_EDIT_COLUMN_WIDTH));
}

BOOL CPageNewTableOptions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_spinDecimalPrecision.SetRange(0,6);
	m_editColumnWidth.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageNewTableOptions::OnChangeEditMissingValue() 
{
	CString temp;
	UpdateData();
	m_MissingValue.GetWindowText(temp);
	if((m_nMissingValue < 0 && temp.GetLength() > 12) ||
		(m_nMissingValue > 0 && temp.GetLength() > 11)){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Missing value must be between -999999.999999 and 999999.999999",MB_OK | MB_ICONSTOP);
		temp.Format("%.4f",ANALYSIS_VALUE);
		m_MissingValue.SetWindowText(temp);
		
	}
}
