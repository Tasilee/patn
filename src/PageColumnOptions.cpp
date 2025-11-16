// PageColumnOptions.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
//troy's include for search function
#include "PatnDoc.h"
#include "PageColumnOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPageColumnOptions property page

IMPLEMENT_DYNCREATE(CPageColumnOptions, CPropertyPage)

CPageColumnOptions::CPageColumnOptions() : CPropertyPage(CPageColumnOptions::IDD)
{
	//{{AFX_DATA_INIT(CPageColumnOptions)
	m_nWhichColumnWidth = -1;
	m_nColumnWidth = 0.0;
	m_nHide = FALSE;
	m_strLabel = _T("");
	m_nDecimalPrecision = 0;
	//}}AFX_DATA_INIT
	m_bMultiple = FALSE;
	m_bChangedLabel = FALSE;
	m_bChangedHidden = FALSE;
	m_bChangedColumnWidth = TRUE;
	m_bChangedDecimalPrecision = FALSE;
	m_bChangedDecimalPrecision = FALSE;
	m_bChangedGroup = FALSE;
}


CPageColumnOptions::~CPageColumnOptions()
{
}

void CPageColumnOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageColumnOptions)
	DDX_Control(pDX, IDC_EDIT_GROUP, m_editGroup);
	DDX_Control(pDX, IDC_SPIN_DEC_PLACES, m_spinDecimalPrecision);
	DDX_Control(pDX, IDC_EDIT_LABEL, m_editLabel);
	DDX_Control(pDX, IDC_EDIT_DECIMAL_PRECISION, m_editDecimalPrecision);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_checkHide);
	DDX_Control(pDX, IDC_EDIT_COLUMN_WIDTH, m_editColumnWidth);
	DDX_Radio(pDX, IDC_RADIO_STANDARD, m_nWhichColumnWidth);
	DDX_Text(pDX, IDC_EDIT_COLUMN_WIDTH, m_nColumnWidth);
	DDV_MinMaxDouble(pDX, m_nColumnWidth, 0., 30.);
	DDX_Check(pDX, IDC_CHECK_HIDE, m_nHide);
	DDX_Text(pDX, IDC_EDIT_LABEL, m_strLabel);
	DDX_Text(pDX, IDC_EDIT_DECIMAL_PRECISION, m_nDecimalPrecision);
	DDV_MinMaxInt(pDX, m_nDecimalPrecision, 0, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageColumnOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CPageColumnOptions)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, OnRadioCustom)
	ON_BN_CLICKED(IDC_RADIO_STANDARD, OnRadioStandard)
	ON_BN_CLICKED(IDC_RADIO_SIZETOFIT, OnRadioSizetofit)
	ON_BN_CLICKED(IDC_CHECK_HIDE, OnCheckHide)
	ON_EN_CHANGE(IDC_EDIT_DECIMAL_PRECISION, OnChangeEditDecimalPrecision)
	ON_EN_CHANGE(IDC_EDIT_LABEL, OnChangeEditLabel)
	ON_EN_CHANGE(IDC_EDIT_GROUP, OnChangeEditGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPageColumnOptions message handlers

BOOL CPageColumnOptions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	long extra = 0; //to help determine if group is maximum
	CString groupNumber;
	groupNumber.Format("%d",m_strGroup);
	m_editGroup.SetWindowText(groupNumber);

    if(m_strGroup == -1) m_editGroup.SetWindowText("");

	m_spinDecimalPrecision.SetRange(0,10);

	if ( m_bMultiple )
	{
		m_strLabel = _T("");
		m_editLabel.EnableWindow(FALSE);
		m_editGroup.EnableWindow(FALSE);
		m_nWhichColumnWidth = -1;
		m_nHide = 2; // indeterminate
	}

	if ( m_nWhichColumnWidth != custom )
		m_editColumnWidth.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageColumnOptions::OnRadioStandard() 
{
	UpdateData(TRUE);
	m_nColumnWidth = g_nStandardColumnWidth;
	m_editColumnWidth.EnableWindow(FALSE);
	UpdateData(FALSE);
	m_bChangedColumnWidth = TRUE;
}

void CPageColumnOptions::OnRadioCustom() 
{
	UpdateData(TRUE);
	m_editColumnWidth.EnableWindow(TRUE);
	GotoDlgCtrl(GetDlgItem(IDC_EDIT_COLUMN_WIDTH));
	m_bChangedColumnWidth = TRUE;
}

void CPageColumnOptions::OnRadioSizetofit() 
{
	UpdateData(TRUE);
	m_editColumnWidth.EnableWindow(FALSE);
	m_bChangedColumnWidth = TRUE;
}

void CPageColumnOptions::OnCheckHide() 
{
	m_bChangedHidden = TRUE;
}

void CPageColumnOptions::OnChangeEditDecimalPrecision() 
{
	m_bChangedDecimalPrecision = TRUE;
}

void CPageColumnOptions::OnChangeEditLabel() 
{
	m_bChangedLabel = TRUE;
}


void CPageColumnOptions::OnChangeEditGroup() 
{	
	static long running = 0;
	CString strGroupNumber;
	CString strMessage;
	
	if(running) return;
	running = 1;
	m_editGroup.GetWindowText(strGroupNumber);
	m_strGroup = atoi(strGroupNumber);
	if(strcmp(strGroupNumber,"") == 0 || m_strGroup < 0)
	{
		//don't report that group deleted
		m_strGroup = -1;
		m_editGroup.SetWindowText("");
	}

	running = 0;
	m_bChangedGroup = TRUE;
}
