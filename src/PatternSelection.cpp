// PatternSelection.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "PatternSelection.h"
#include "texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatternSelection dialog

CPatternSelection::CPatternSelection(CWnd* pParent /*=NULL*/)
	: CDialog(CPatternSelection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatternSelection)
	m_Radio1 = -1;
	//}}AFX_DATA_INIT
}


void CPatternSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternSelection)
	DDX_Control(pDX, IDC_SCROLL_PATTERNS, m_ScrollPatterns);
	DDX_Control(pDX, IDC_SCROLL_OBJECTS, m_ScrollObjects);
	DDX_Control(pDX, IDC_ASSIGN, m_Assign);
	DDX_Control(pDX, IDC_RESET, m_Reset);
	DDX_Control(pDX, IDC_RADIO1, m_ObjectsSelection);
	DDX_Control(pDX, IDC_PATTERNS, m_Patterns);
	DDX_Control(pDX, IDC_OBJECTS, m_Objects);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatternSelection, CDialog)
	//{{AFX_MSG_MAP(CPatternSelection)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_LBN_SELCHANGE(IDC_OBJECTS, OnSelchangeObjects)
	ON_BN_CLICKED(IDC_ASSIGN, OnAssign)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_LBN_SELCHANGE(IDC_PATTERNS, OnSelchangePatterns)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternSelection message handlers

BOOL CPatternSelection::OnInitDialog() 
{
	int i;

	CDialog::OnInitDialog();
	
	//set m_Objects as objects
	CString label;
	m_ObjectsSelected = true;
	m_apriori = false;
	m_Objects.ResetContent();
	for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
		m_pDoc->GetRowLabel(i,label);
		m_Objects.AddString(label);
	}

	//set m_Patterns
/*	CString patterns = ((CPatnApp*)AfxGetApp())->m_ProgramPath;
	patterns += "patterns\\patterns.dat";
	CString strError ;
	if(!m_tex.LoadPatterns(patterns)){
		strError = "Failed to find file ";
		strError += patterns;
		strError += "\nNo symbols will be displayed.";
		if(((CPatnApp*)AfxGetApp())->m_fileerrormsgs) AfxMessageBox(strError);
	}*/
	
	CString patterns;
	HRSRC rsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_TEXT1),"TEXT");
	HGLOBAL h = LoadResource(NULL,rsrc);
	char * c = (char*)h;
	i=0;//loop until "eee" occurs - last entry
	while(!(c[i] == 'e' && c[i+1] == 'e' && c[i+2] == 'e')){
		patterns.Insert(i,c[i]);
		i++;
	}
	patterns.Insert(i,'e');

	if(!m_tex.LoadPatterns(0,patterns)){
	
	}

	for(i=0;i<m_tex.GetNoOfPatterns();i++){
		label.Format("%d",i);
		m_Patterns.AddString(label);
	}

	m_Patterns.SetCurSel(0);
	m_Objects.SetCurSel(0);

	m_ScrollObjects.SetRange(0,m_Objects.GetCount()-1);
	m_ScrollPatterns.SetRange(0,m_Patterns.GetCount()-1);

	m_ObjectsSelection.SetCheck(1);

	for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
		if(m_pDoc->m_PatternObjects[i] == -1)
			m_pDoc->m_PatternObjects[i] = i;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatternSelection::OnRadio1() 
{
	//set m_Objects as objects
	int i;
	CString label;
	m_ObjectsSelected = true;
	m_Objects.ResetContent();
	for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
		m_pDoc->GetRowLabel(i,label);
		m_Objects.AddString(label);
	}
	m_Objects.SetCurSel(0);
	m_ScrollObjects.SetRange(0,i-1);
	RefreshPatterns();
}

void CPatternSelection::OnRadio2() 
{
	//set m_Objects as groups
	int i;
	CString label;
	m_ObjectsSelected = false;
	m_apriori = false;
	m_Objects.ResetContent();
	for(i=0;i<m_pDoc->m_PatternGroups.GetSize();i++){
		label.Format("Group %d",m_pDoc->m_Groups[i]);
		m_Objects.AddString(label);
	}
	m_Objects.SetCurSel(0);
	m_ScrollObjects.SetRange(0,i-1);
	RefreshPatterns();
}

void CPatternSelection::OnSelchangeObjects() 
{
	m_Patterns.SetTopIndex(m_pDoc->m_PatternObjects[m_Objects.GetCurSel()]);
	m_Patterns.SetCurSel(m_Patterns.GetTopIndex());
	m_ScrollPatterns.SetPos(m_pDoc->m_PatternObjects[m_Objects.GetCurSel()]);
	m_ScrollObjects.SetPos(m_Objects.GetCurSel());
	RefreshPatterns();
}

void CPatternSelection::OnAssign() 
{
	CArray<int,int> items;
	int i = m_Objects.GetSelCount();

	if(i == 0) return;

	items.SetSize(i);
	m_Objects.GetSelItems(m_Objects.GetSelCount(),&items[0]);

	if(m_ObjectsSelected){
		for(i=0;i<m_Objects.GetSelCount();i++)
			m_pDoc->m_PatternObjects[items[i]] = 
				m_Patterns.GetCurSel();
	}else{
		for(i=0;i<m_Objects.GetSelCount();i++)
			m_pDoc->m_PatternGroups[items[i]] = 
				m_Patterns.GetCurSel();
	}	
	RefreshPatterns();
}

void CPatternSelection::OnReset() 
{
	int i;
	//for current selection (object/group)
	if(m_ObjectsSelected){
		for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
			m_pDoc->m_PatternObjects[i] = i;
		}
	}else{
		for(i=0;i<m_pDoc->m_PatternGroups.GetSize();i++){
			m_pDoc->m_PatternGroups[i] = i;
		}
	}
	RefreshPatterns();
}

void CPatternSelection::OnSelchangePatterns() 
{
	m_ScrollPatterns.SetPos(m_Patterns.GetCurSel());
	RefreshPatterns();
}

void CPatternSelection::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	int x,y,i,idx;

	//draw object patterns
	for(i=0;i<11 &&	i<m_Objects.GetCount()-m_Objects.GetTopIndex();i++){
		idx = i + m_Objects.GetTopIndex();
		x = 20; y = 35 + i*13;
		if(m_ObjectsSelected)
			m_tex.GDIPatterns(m_pDoc->m_PatternObjects[idx],&dc,x,y);
		else
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[idx],&dc,x,y);
	}	

	//draw all patterns
	for(i=0;i<18 &&	i<m_Patterns.GetCount()-m_Patterns.GetTopIndex();i++){
		idx = i + m_Patterns.GetTopIndex();
		x = 220; y = 35 + i*13;
		m_tex.GDIPatterns(idx,&dc,x,y);
	}	
}

void CPatternSelection::RefreshPatterns()
{
	CRect rect;

	rect.top = 35;
	rect.bottom = 35 + 18*13 + 10;
	rect.left = 220;
	rect.right = 220 + 10;
	RedrawWindow(rect);

	rect.top = 35;
	rect.bottom = 35 + 11*13 + 10;
	rect.left = 20;
	rect.right = 20 + 10;
	RedrawWindow(rect);
}
void CPatternSelection::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	
	if(nSBCode == SB_ENDSCROLL)
		return;

	UpdateData();

	int newpos;
	CString buffer;

	// objects slider
	if((CSliderCtrl *)pScrollBar == &m_ScrollObjects){
		newpos = m_ScrollObjects.GetPos();
		m_Objects.SetCurSel(newpos);
		m_Objects.SetTopIndex(newpos);
		m_Patterns.SetTopIndex(m_pDoc->m_PatternObjects[newpos]);
		m_Patterns.SetCurSel(m_Patterns.GetTopIndex());
		m_ScrollPatterns.SetPos(m_pDoc->m_PatternObjects[newpos]);
	}
	// patterns slider
	if((CSliderCtrl *)pScrollBar == &m_ScrollPatterns){
		newpos = m_ScrollPatterns.GetPos();
		m_Patterns.SetCurSel(newpos);
		m_Patterns.SetTopIndex(newpos);
	}


	RefreshPatterns();
	
}

void CPatternSelection::OnRadio3() 
{
	//set m_Objects as a priori groups
	int i;
	CString label;
	m_ObjectsSelected = false;
	m_apriori = true;
	m_Objects.ResetContent();
	for(i=0;i<m_pDoc->m_PatternGroups.GetSize();i++){
		label.Format("Group %d",m_pDoc->m_RowControlGroups[i]);
		m_Objects.AddString(label);
	}
	m_Objects.SetCurSel(0);
	m_ScrollObjects.SetRange(0,i-1);
	RefreshPatterns();	
}
