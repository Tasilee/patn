// PropertiesVisibleStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesVisibleStatistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesVisibleStatistics dialog


CPropertiesVisibleStatistics::CPropertiesVisibleStatistics(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPropertiesVisibleStatistics::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesVisibleStatistics)
	m_colmax = FALSE;
	m_colmean = FALSE;
	m_colmin = FALSE;
	m_colmissing = FALSE;
	m_colno = FALSE;
	m_colsum = FALSE;
	m_rowmax = FALSE;
	m_rowmean = FALSE;
	m_rowmin = FALSE;
	m_rowmissing = FALSE;
	m_rowno = FALSE;
	m_rowsum = FALSE;
	m_visible_decimals = 0;
	m_dec = 0;
	m_check_bool = FALSE;
	//}}AFX_DATA_INIT
}


void CPropertiesVisibleStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesVisibleStatistics)
	DDX_Control(pDX, IDC_VISDEC_SPIN, m_vis_spin);
	DDX_Control(pDX, IDC_COLSPIN, m_col_spin);
	DDX_Control(pDX, IDC_COLDECCHECK, m_check);
	DDX_Control(pDX, IDC_COLDEC, m_col_edit_ctrl);
	DDX_Check(pDX, IDC_COLMAX, m_colmax);
	DDX_Check(pDX, IDC_COLMEAN, m_colmean);
	DDX_Check(pDX, IDC_COLMIN, m_colmin);
	DDX_Check(pDX, IDC_COLMISSING, m_colmissing);
	DDX_Check(pDX, IDC_COLNO, m_colno);
	DDX_Check(pDX, IDC_COLSUM, m_colsum);
	DDX_Check(pDX, IDC_ROWMAX, m_rowmax);
	DDX_Check(pDX, IDC_ROWMEAN, m_rowmean);
	DDX_Check(pDX, IDC_ROWMIN, m_rowmin);
	DDX_Check(pDX, IDC_ROWMISSING, m_rowmissing);
	DDX_Check(pDX, IDC_ROWNO, m_rowno);
	DDX_Check(pDX, IDC_ROWSUM, m_rowsum);
	DDX_Text(pDX, IDC_VISIBLEDEC_EDIT, m_visible_decimals);
	DDV_MinMaxInt(pDX, m_visible_decimals, 0, 7);
	DDX_Text(pDX, IDC_COLDEC, m_dec);
	DDV_MinMaxInt(pDX, m_dec, 0, 7);
	DDX_Check(pDX, IDC_COLDECCHECK, m_check_bool);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesVisibleStatistics, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesVisibleStatistics)
	ON_BN_CLICKED(IDC_COLDECCHECK, OnColdeccheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesVisibleStatistics message handlers

void CPropertiesVisibleStatistics::OnColdeccheck() 
{
	UpdateData();
	if(!m_check_bool){
		m_col_edit_ctrl.EnableWindow(false);
		m_col_spin.EnableWindow(false);
	}else{
		m_col_edit_ctrl.EnableWindow(true);
		m_col_spin.EnableWindow(true);
	}	
}

BOOL CPropertiesVisibleStatistics::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_vis_spin.SetRange(0,7);
	m_col_spin.SetRange(0,7);


	
	OnColdeccheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
