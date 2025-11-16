// EditText2.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "EditText2.h"
#include ".\edittext2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditText2 dialog


CEditText2::CEditText2(CWnd* pParent /*=NULL*/)
	: CDialog(CEditText2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditText2)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void CEditText2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditText2)
	DDX_Text(pDX, IDC_EDIT1, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditText2, CDialog)
	//{{AFX_MSG_MAP(CEditText2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCLOSE, OnBnClickedClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditText2 message handlers

BOOL CEditText2::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_delete = false;
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditText2::OnButton1() 
{
	LOGFONT lf;
	memcpy(&lf,&(m_font),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,m_font.lfFaceName);

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = m_colour;

	if ( dlg.DoModal() == IDOK )
	{
		memcpy(&(m_font),&lf,sizeof(LOGFONT));
		strcpy(m_font.lfFaceName,lf.lfFaceName);
		m_colour = dlg.m_cf.rgbColors;

	}
}

void CEditText2::OnBnClickedClose()
{
	// TODO: Add your control notification handler code here
	m_delete = true;
	this->PostMessage(WM_CLOSE);
}
