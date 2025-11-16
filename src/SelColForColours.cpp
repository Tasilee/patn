// SelColForColours.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "SelColForColours.h"
#include ".\selcolforcolours.h"


// CSelColForColours dialog

IMPLEMENT_DYNAMIC(CSelColForColours, CDialog)
CSelColForColours::CSelColForColours(CPatnDoc * doc, long sel, CWnd* pParent /*=NULL*/)
	: CDialog(CSelColForColours::IDD, pParent)
	, m_logScale(false)
{
	m_pDoc = doc;
	m_inputSel = sel;
}

CSelColForColours::~CSelColForColours()
{
}

BOOL CSelColForColours::OnInitDialog() {
	//set preset colour entries
	CDialog::OnInitDialog();

	m_colourPresets.AddString("blue - red");
	m_colourPresets.AddString("red - blue");
	m_colourPresets.AddString("green - red");
	m_colourPresets.AddString("red - green");
	m_colourPresets.AddString("white - black");
	m_colourPresets.AddString("black - white");

	long i;
	CString str;
	for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
		m_pDoc->GetColumnLabel(i, str);
		m_list.AddString(str);
	}
	if(m_inputSel < m_pDoc->GetNoOfColumns() && m_sel >= 0) {
		m_list.SetCurSel(m_inputSel);
	} else {
		m_list.SetCurSel(0);
	}
	m_colourPresets.SetCurSel(0);
	m_colour1 = RGB(0,0,255);
	m_colour2 = RGB(255,0,0);
	
	RedrawWindow();

	return TRUE;
}

void CSelColForColours::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_colourPresets);
	DDX_Control(pDX, IDC_ALL, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_c1);
	DDX_Control(pDX, IDC_EDIT5, m_c2);
}


BEGIN_MESSAGE_MAP(CSelColForColours, CDialog)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
END_MESSAGE_MAP()


// CSelColForColours message handlers

void CSelColForColours::OnBnClickedButton4()
{
	// end colour
}

void CSelColForColours::OnBnClickedButton1()
{
	// start colour
}

void CSelColForColours::OnLbnSelchangeList1()
{
	//pick colour presets
	if(m_colourPresets.GetCurSel() == 0) {
		m_colour1 = RGB(0,0,255);
		m_colour2 = RGB(255,0,0);
	} else if(m_colourPresets.GetCurSel() == 1) {
		m_colour2 = RGB(0,0,255);
		m_colour1 = RGB(255,0,0);
	} else if(m_colourPresets.GetCurSel() == 2) {
		m_colour1 = RGB(0,255,0);
		m_colour2 = RGB(255,0,0);
	}else if(m_colourPresets.GetCurSel() == 3) {
		m_colour2 = RGB(0,255,0);
		m_colour1 = RGB(255,0,0);
	}else if(m_colourPresets.GetCurSel() == 4) {
		m_colour1 = RGB(255,255,255);
		m_colour2 = RGB(0,0,0);
	}else if(m_colourPresets.GetCurSel() == 5) {
		m_colour2 = RGB(255,255,255);
		m_colour1 = RGB(0,0,0);
	}

	RedrawWindow();
	OnPaint();
}

void CSelColForColours::OnBnClickedButton2()
{
	// OK

	m_sel = m_list.GetCurSel();

	this->OnOK();
}

void CSelColForColours::OnBnClickedButton5()
{
	this->OnCancel();

}

void CSelColForColours::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;

	m_c1.GetWindowRect(&rect);
	
	CBrush brush;
	brush.CreateSolidBrush(m_colour1);
	dc.FillRect(rect,&brush);

	m_c2.GetWindowRect(&rect);
	CBrush brush2;
	brush2.CreateSolidBrush(m_colour2);
	dc.FillRect(rect,&brush2);
}