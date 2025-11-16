// PropertiesBWObjects.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesBWObjects.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesBWObjects property page

IMPLEMENT_DYNCREATE(CPropertiesBWObjects, CPropertyPage)

CPropertiesBWObjects::CPropertiesBWObjects() : CPropertyPage(CPropertiesBWObjects::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesBWObjects)
	m_Radio1 = -1;
	//}}AFX_DATA_INIT
}

CPropertiesBWObjects::~CPropertiesBWObjects()
{
}

void CPropertiesBWObjects::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesBWObjects)
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


BEGIN_MESSAGE_MAP(CPropertiesBWObjects, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesBWObjects)
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
// CPropertiesBWObjects message handlers

BOOL CPropertiesBWObjects::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	long i;

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
		if(((CPatnApp*)AfxGetApp())->m_fileerrormsgs) if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strError);
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

void CPropertiesBWObjects::OnRadio1() 
{
	//set m_Objects as objects
	long i;
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

void CPropertiesBWObjects::OnRadio2() 
{
	//set m_Objects as groups
	long i;
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

void CPropertiesBWObjects::OnSelchangeObjects() 
{
	if(m_ObjectsSelected){
		m_Patterns.SetTopIndex(m_pDoc->m_PatternObjects[m_Objects.GetCurSel()]);
		m_Patterns.SetCurSel(m_Patterns.GetTopIndex());
		m_ScrollPatterns.SetPos(m_pDoc->m_PatternObjects[m_Objects.GetCurSel()]);
		m_ScrollObjects.SetPos(m_Objects.GetCurSel());
	}else if(!m_apriori){
		m_Patterns.SetTopIndex(m_pDoc->m_PatternGroups[m_Objects.GetCurSel()]);
		m_Patterns.SetCurSel(m_Patterns.GetTopIndex());
		m_ScrollPatterns.SetPos(m_pDoc->m_PatternGroups[m_Objects.GetCurSel()]);
		m_ScrollObjects.SetPos(m_Objects.GetCurSel());
	}else{
		m_Patterns.SetTopIndex(m_pDoc->m_PatternApriori[m_Objects.GetCurSel()]);
		m_Patterns.SetCurSel(m_Patterns.GetTopIndex());
		m_ScrollPatterns.SetPos(m_pDoc->m_PatternApriori[m_Objects.GetCurSel()]);
		m_ScrollObjects.SetPos(m_Objects.GetCurSel());
	}
	
	RefreshPatterns();
}

void CPropertiesBWObjects::OnAssign() 
{
	CArray<int> items;
	long i = m_Objects.GetSelCount();

	if(i == 0) return;

	items.SetSize(i);
	m_Objects.GetSelItems(m_Objects.GetSelCount(),&items[0]);

	if(m_ObjectsSelected){
		for(i=0;i<m_Objects.GetSelCount();i++)
			m_pDoc->m_PatternObjects[items[i]] = 
				m_Patterns.GetCurSel();
	}else if(!m_apriori){
		for(i=0;i<m_Objects.GetSelCount();i++)
			m_pDoc->m_PatternGroups[items[i]] = 
				m_Patterns.GetCurSel();
	}else{
		for(i=0;i<m_Objects.GetSelCount() && m_pDoc->m_PatternApriori.GetSize();i++)
			m_pDoc->m_PatternApriori[items[i]] = 
				m_Patterns.GetCurSel();
	}
	RefreshPatterns();
}

void CPropertiesBWObjects::OnReset() 
{
	long i;
	//for current selection (object/group)
	if(m_ObjectsSelected){
		for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
			m_pDoc->m_PatternObjects[i] = i;
		}
	}else if(!m_apriori){
		for(i=0;i<m_pDoc->m_PatternGroups.GetSize();i++){
			m_pDoc->m_PatternGroups[i] = i;
		}
	}else{
		for(i=0;i<m_pDoc->m_PatternApriori.GetSize();i++){
			m_pDoc->m_PatternApriori[i] = i;
		}
	}
	RefreshPatterns();
}

void CPropertiesBWObjects::OnSelchangePatterns() 
{
	m_ScrollPatterns.SetPos(m_Patterns.GetCurSel());
	RefreshPatterns();
}

void CPropertiesBWObjects::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	long x,y,i,idx;

	//draw object patterns

	for(i=0;i<11 &&	i<m_Objects.GetCount()-m_Objects.GetTopIndex();i++){
		idx = i + m_Objects.GetTopIndex();
		x = 20; y = 40 + i*13;
		if(m_ObjectsSelected)
			m_tex.GDIPatterns(m_pDoc->m_PatternObjects[idx],&dc,x,y);
		else if(!m_apriori)
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[idx],&dc,x,y);
		else
			m_tex.GDIPatterns(m_pDoc->m_PatternApriori[idx],&dc,x,y);
	}	

	//draw all patterns
	for(i=0;i<17 &&	i<m_Patterns.GetCount()-m_Patterns.GetTopIndex();i++){
		idx = i + m_Patterns.GetTopIndex();
		x = 335; y = 40 + i*13;
		m_tex.GDIPatterns(idx,&dc,x,y);
	}	
}

void CPropertiesBWObjects::RefreshPatterns()
{
	CRect rect;

	rect.top = 35;
	rect.bottom = 35 + 18*13 + 10;
	rect.left = 335;
	rect.right = 335 + 10;
	RedrawWindow(rect);

	rect.top = 35;
	rect.bottom = 35 + 11*13 + 10;
	rect.left = 20;
	rect.right = 20 + 10;
	RedrawWindow(rect);
}
void CPropertiesBWObjects::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
    {
    // First determine if the user scrolled a scroll bar control
    // on the window or scrolled the window itself
        HWND hWndScroll;
        if ( pScrollBar == NULL )
            hWndScroll = m_hWnd;
        else
            hWndScroll = pScrollBar->m_hWnd;

        SCROLLINFO info;
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_TRACKPOS;
        ::GetScrollInfo(hWndScroll, SB_VERT, &info);

        nPos = info.nTrackPos;
    }
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	
	if(nSBCode == SB_ENDSCROLL)
		return;

	UpdateData();

	long newpos;
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

void CPropertiesBWObjects::OnRadio3() 
{
	//set m_Objects as groups
	long i;
	CString label;
	m_ObjectsSelected = false;
	m_apriori = true;
	m_Objects.ResetContent();
	for(i=1;i<m_pDoc->m_RowControlGroups.GetSize();i++){
		label.Format("a priori Group %d",i);
		m_Objects.AddString(label);
	}
	m_ObjectsSelection.SetCheck(0);
	m_Objects.SetCurSel(-1);
	m_ScrollObjects.SetRange(0,i-1);
	RefreshPatterns();	
}
