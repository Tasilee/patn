// PageRowOptions.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "PageRowOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageRowOptions property page

IMPLEMENT_DYNCREATE(CPageRowOptions, CPropertyPage)

CPageRowOptions::CPageRowOptions() : CPropertyPage(CPageRowOptions::IDD)
{
	//{{AFX_DATA_INIT(CPageRowOptions)
	m_strLabel = _T("");
	m_nHide = FALSE;
	//}}AFX_DATA_INIT
	m_bMultiple = FALSE;
	m_bChangedLabel = FALSE;
	m_bChangedHidden = FALSE;
	m_bChangedGroup = FALSE;
}

CPageRowOptions::~CPageRowOptions()
{
}

void CPageRowOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageRowOptions)
	DDX_Control(pDX, IDC_EDIT_GROUP, m_editGroup);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_checkHide);
	DDX_Control(pDX, IDC_EDIT_LABEL, m_editLabel);
	DDX_Text(pDX, IDC_EDIT_LABEL, m_strLabel);
	DDX_Check(pDX, IDC_CHECK_HIDE, m_nHide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageRowOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CPageRowOptions)
	ON_BN_CLICKED(IDC_CHECK_HIDE, OnCheckHide)
	ON_EN_CHANGE(IDC_EDIT_LABEL, OnChangeEditLabel)
	ON_EN_CHANGE(IDC_EDIT_GROUP, OnChangeEditGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageRowOptions message handlers

void CPageRowOptions::OnCheckHide() 
{
	m_bChangedHidden = TRUE;
}

void CPageRowOptions::OnChangeEditLabel() 
{
	m_bChangedLabel = TRUE;
}

BOOL CPageRowOptions::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	//**PROTO1**

	long idx=0;
	long extra = 0; //to help determine if group is maximum

	CString groupNumber;
	groupNumber.Format("%d",m_strGroup);
	m_editGroup.SetWindowText(groupNumber);

	//Must set, if m_strGroup == -1, to empty.
    if(m_strGroup == -1) 
	{
		m_editGroup.SetWindowText("");
		m_strGroup = -1;
	}

	if ( m_bMultiple )
	{
		m_strLabel = _T("");
		m_editLabel.EnableWindow(FALSE);
		m_editGroup.EnableWindow(FALSE);
		m_nHide = 2; // indeterminate
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageRowOptions::OnChangeEditGroup() 
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
		m_editGroup.SetWindowText("");
		m_strGroup = -1;
	}
	running = 0;
	m_bChangedGroup = TRUE;	
}

void CPageRowOptions::OnOK() 
{
	CPropertyPage::OnOK();
}
