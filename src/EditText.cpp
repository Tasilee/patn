// EditText.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "EditText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditText dialog


CEditText::CEditText(CWnd* pParent /*=NULL*/)
	: CDialog(CEditText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditText)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void CEditText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditText)
	DDX_Control(pDX, IDC_SPIN_DEC_PLACES, m_Spin);
	DDX_Text(pDX, IDC_EDIT1, m_text);
	DDX_Text(pDX, IDC_EDIT_DECIMAL_PRECISION, m_Fontsize);
	DDV_MinMaxInt(pDX, m_Fontsize, 0, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditText, CDialog)
	//{{AFX_MSG_MAP(CEditText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditText message handlers

BOOL CEditText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Spin.SetRange(0,30);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
