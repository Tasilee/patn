// TwayOptions.cpp : implementation file
//
//**PROTO1**
//See Tway.cpp for any user input necessary.
//Probably only presence/absence vs normal option.

#include "stdafx.h"
#include "patn.h"
#include "TwayOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTwayOptions dialog


CTwayOptions::CTwayOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CTwayOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTwayOptions)
	m_Scale = 5;
	//}}AFX_DATA_INIT
}


void CTwayOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTwayOptions)
	DDX_Control(pDX, IDC_GLOBSTD, m_GlobStd);
	DDX_Control(pDX, IDC_COLSTD, m_ColStd);
	DDX_Control(pDX, IDC_PRESABS, m_PresAbs);
	DDX_Control(pDX, IDC_RED, m_Red);
	DDX_Control(pDX, IDC_GREEN, m_Green);
	DDX_Control(pDX, IDC_BLUE, m_Blue);
	DDX_Control(pDX, IDC_BW, m_Bw);
	DDX_Text(pDX, IDC_SCALE, m_Scale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTwayOptions, CDialog)
	//{{AFX_MSG_MAP(CTwayOptions)
	ON_BN_CLICKED(IDC_GLOBSTD, OnGlobstd)
	ON_BN_CLICKED(IDC_COLSTD, OnColstd)
	ON_BN_CLICKED(IDC_GREEN, OnGreen)
	ON_BN_CLICKED(IDC_RED, OnRed)
	ON_BN_CLICKED(IDC_BLUE, OnBlue)
	ON_BN_CLICKED(IDC_BW, OnBw)
	ON_BN_CLICKED(IDC_PRESABS, OnPresabs)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTwayOptions message handlers

void CTwayOptions::OnGlobstd() 
{
	m_colstand = 0;
	m_ColStd.SetCheck(0);
	m_globstand = 1;
	m_GlobStd.SetCheck(1);	
	m_presabs = 0;
	m_PresAbs.SetCheck(0);	

//	m_ControlScale.EnableWindow(1);
}

void CTwayOptions::OnColstd() 
{
	m_colstand = 1;
	m_ColStd.SetCheck(1);
	m_globstand = 0;
	m_GlobStd.SetCheck(0);	
	m_presabs = 0;
	m_PresAbs.SetCheck(0);

//	m_ControlScale.EnableWindow(1);
}

BOOL CTwayOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nScale = 5;
	//SetDlgItemText(IDC_SCALE,"13");
	
	m_colstand = 1;
	m_ColStd.SetCheck(1);
	m_globstand = 0;
	m_GlobStd.SetCheck(0);
	m_presabs = 0;
	m_PresAbs.SetCheck(0);
	
	m_red = 0;
	m_Red.SetCheck(0);
	m_green = 0;
	m_Green.SetCheck(0);
	m_blue = 1;
	m_Blue.SetCheck(1);
	m_bw = 0;
	m_Bw.SetCheck(0);
	
	UpdateWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTwayOptions::OnPresabs() 
{
	m_colstand = 0;
	m_ColStd.SetCheck(0);
	m_globstand = 0;
	m_GlobStd.SetCheck(0);	
	m_presabs = 1;
	m_PresAbs.SetCheck(1);

//	m_ControlScale.EnableWindow(0);
}

void CTwayOptions::OnGreen() 
{
	m_red = 0;
	m_Red.SetCheck(0);
	m_green = 1;
	m_Green.SetCheck(1);
	m_blue = 0;
	m_Blue.SetCheck(0);
	m_bw = 0;
	m_Bw.SetCheck(0);
	
	
}

void CTwayOptions::OnRed() 
{
	m_red = 1;
	m_Red.SetCheck(1);
	m_green = 0;
	m_Green.SetCheck(0);
	m_blue = 0;
	m_Blue.SetCheck(0);
	m_bw = 0;
	m_Bw.SetCheck(0);
	
	
}

void CTwayOptions::OnBlue() 
{
	m_red = 0;
	m_Red.SetCheck(0);
	m_green = 0;
	m_Green.SetCheck(0);
	m_blue = 1;
	m_Blue.SetCheck(1);
	m_bw = 0;
	m_Bw.SetCheck(0);
	
	
}

void CTwayOptions::OnBw() 
{
	m_red = 0;
	m_Red.SetCheck(0);
	m_green = 0;
	m_Green.SetCheck(0);
	m_blue = 0;
	m_Blue.SetCheck(0);
	m_bw = 1;
	m_Bw.SetCheck(1);
}


void CTwayOptions::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}
