// AppendOptiosn.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "AppendOptiosn.h"
#include ".\appendoptiosn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAppendOptiosn dialog


CAppendOptiosn::CAppendOptiosn(CWnd* pParent /*=NULL*/)
	: CDialog(CAppendOptiosn::IDD, pParent)
{
	m_selb = false;
	//{{AFX_DATA_INIT(CAppendOptiosn)
	m_columnlabels = TRUE;
	m_rowlabels = TRUE;
	m_match = FALSE;
	//}}AFX_DATA_INIT
}


void CAppendOptiosn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppendOptiosn)
	DDX_Control(pDX, IDC_ROWLABELS, m_r);
	DDX_Control(pDX, IDC_RL, m_rl);
	DDX_Control(pDX, IDC_MATCH, m_m);
	DDX_Control(pDX, IDC_INPUTFORMAT, m_f);
	DDX_Control(pDX, IDC_COLUMNLABELS, m_c);
	DDX_Control(pDX, IDC_CL, m_cl);
	DDX_Control(pDX, IDC_APPENDLHSPLUS, m_lp);
	DDX_Control(pDX, IDC_APPENDLHS, m_l);
	DDX_Control(pDX, IDC_APPENDBTMPLUS, m_bp);
	DDX_Control(pDX, IDC_APPENDBTM, m_b);
	DDX_Check(pDX, IDC_CL, m_columnlabels);
	DDX_Check(pDX, IDC_RL, m_rowlabels);
	DDX_Check(pDX, IDC_MATCH, m_match);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppendOptiosn, CDialog)
	//{{AFX_MSG_MAP(CAppendOptiosn)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MATCH, OnMatch)
	ON_BN_CLICKED(IDC_RL, OnRl)
	ON_BN_CLICKED(IDC_CL, OnCl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppendOptiosn message handlers

BOOL CAppendOptiosn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	b.LoadBitmap(IDB_APPEND03);
	m_b.SetBitmap(b);
	bp.LoadBitmap(IDB_APPEND06);
	m_bp.SetBitmap(bp);		

	l.LoadBitmap(IDB_APPEND04);
	m_l.SetBitmap(l);
	lp.LoadBitmap(IDB_APPEND05);
	m_lp.SetBitmap(lp);		
	
	OnMatch();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAppendOptiosn::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect1,rect2;
	CBitmap bmp;

	//determine if over bottom+plus
	m_b.GetWindowRect(rect1);
	m_bp.GetWindowRect(rect2);
	
	ScreenToClient(rect2);
	ScreenToClient(rect1);
	
	if(m_selb || (point.x > rect1.left && point.x < rect1.right &&
		point.y > rect2.top && point.y < rect1.bottom)){
		b.DeleteObject();
		b.LoadBitmap(IDB_APPEND04);
		m_b.SetBitmap(b);
		
		bp.DeleteObject();
		bp.LoadBitmap(IDB_APPEND05);
		m_bp.SetBitmap(bp);		
	}else if(!m_selb){
		b.DeleteObject();
		b.LoadBitmap(IDB_APPEND03);
		m_b.SetBitmap(b);
		
		bp.DeleteObject();
		bp.LoadBitmap(IDB_APPEND06);
		m_bp.SetBitmap(bp);		
	}


	//determine if over lefthandside+plus
	m_l.GetWindowRect(rect1);
	m_lp.GetWindowRect(rect2);
	ScreenToClient(rect2);
	ScreenToClient(rect1);
	if(!m_selb || (point.x > rect2.left && point.x < rect1.right &&
		point.y > rect1.top && point.y < rect1.bottom)){
		l.DeleteObject();
		l.LoadBitmap(IDB_APPEND04);
		m_l.SetBitmap(l);
		
		lp.DeleteObject();
		lp.LoadBitmap(IDB_APPEND05);
		m_lp.SetBitmap(lp);		
	}else if(m_selb){
		l.DeleteObject();
		l.LoadBitmap(IDB_APPEND03);
		m_l.SetBitmap(l);
		
		lp.DeleteObject();
		lp.LoadBitmap(IDB_APPEND06);
		m_lp.SetBitmap(lp);		
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CAppendOptiosn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect1,rect2;

	//determine if over lefthandside+plus
	m_l.GetWindowRect(rect1);
	m_lp.GetWindowRect(rect2);
	ScreenToClient(rect2);
	ScreenToClient(rect1);
	if(point.x > rect2.left && point.x < rect1.right &&
		point.y > rect1.top && point.y < rect1.bottom){
		m_selb = false;
	}

	//determine if over bottom+plus
	m_b.GetWindowRect(rect1);
	m_bp.GetWindowRect(rect2);
	
	ScreenToClient(rect2);
	ScreenToClient(rect1);
	
	if(point.x > rect1.left && point.x < rect1.right &&
		point.y > rect2.top && point.y < rect1.bottom){
		b.DeleteObject();
		b.LoadBitmap(IDB_APPEND04);
		m_b.SetBitmap(b);
		
		bp.DeleteObject();
		bp.LoadBitmap(IDB_APPEND05);
		m_bp.SetBitmap(bp);	
		m_selb = true;
	}else if(!m_selb){
		b.DeleteObject();
		b.LoadBitmap(IDB_APPEND03);
		m_b.SetBitmap(b);
		
		bp.DeleteObject();
		bp.LoadBitmap(IDB_APPEND06);
		m_bp.SetBitmap(bp);		
	}


	//determine if over lefthandside+plus
	m_l.GetWindowRect(rect1);
	m_lp.GetWindowRect(rect2);
	ScreenToClient(rect2);
	ScreenToClient(rect1);
	if(point.x > rect2.left && point.x < rect1.right &&
		point.y > rect1.top && point.y < rect1.bottom){
		l.DeleteObject();
		l.LoadBitmap(IDB_APPEND04);
		m_l.SetBitmap(l);
		
		lp.DeleteObject();
		lp.LoadBitmap(IDB_APPEND05);
		m_lp.SetBitmap(lp);		

	}else if(!m_selb){
		l.DeleteObject();
		l.LoadBitmap(IDB_APPEND03);
		m_l.SetBitmap(l);
		
		lp.DeleteObject();
		lp.LoadBitmap(IDB_APPEND06);
		m_lp.SetBitmap(lp);		
	}
	
	OnRl();
	OnCl();

	CDialog::OnLButtonDown(nFlags, point);
}

void CAppendOptiosn::OnDestroy() 
{
	CDialog::OnDestroy();
	
	b.DeleteObject();
	bp.DeleteObject();
	l.DeleteObject();
	lp.DeleteObject();

	r.DeleteObject();
	c.DeleteObject();
	f.DeleteObject();
	
}

void CAppendOptiosn::OnMatch() 
{
	UpdateData();

	//if(!m_m.IsWindowEnabled()){
		f.DeleteObject();
		f.LoadBitmap(IDB_APPEND10);
		m_f.SetBitmap(f);
		r.DeleteObject();
		r.LoadBitmap(IDB_APPEND12);
		m_r.SetBitmap(r);
		c.DeleteObject();
		c.LoadBitmap(IDB_APPEND14);
		m_c.SetBitmap(c);
	//}else if(!m_selb){
	if(!m_selb){
		if(m_m.GetCheck() && m_m.IsWindowEnabled()){
			f.DeleteObject();
			f.LoadBitmap(IDB_APPEND09);
			m_f.SetBitmap(f);
			r.DeleteObject();
			r.LoadBitmap(IDB_APPEND13);
			m_r.SetBitmap(r);
		}else{
			f.DeleteObject();
			f.LoadBitmap(IDB_APPEND10);
			m_f.SetBitmap(f);
			r.DeleteObject();
			r.LoadBitmap(IDB_APPEND12);
			m_r.SetBitmap(r);
		}
	}else {
		if(m_m.GetCheck() && m_m.IsWindowEnabled()){
			f.DeleteObject();
			f.LoadBitmap(IDB_APPEND08);
			m_f.SetBitmap(f);
			c.DeleteObject();
			c.LoadBitmap(IDB_APPEND11);
			m_c.SetBitmap(c);
		}else{
			f.DeleteObject();
			f.LoadBitmap(IDB_APPEND10);
			m_f.SetBitmap(f);
			c.DeleteObject();
			c.LoadBitmap(IDB_APPEND14);
			m_c.SetBitmap(c);
		}
	}	
}

void CAppendOptiosn::OnRl() 
{
	UpdateData();

//	if(!m_rl.IsWindowEnabled())
//		return;

	if(m_rl.GetCheck()){
		m_r.ShowWindow(SW_SHOW);
		if(!m_selb) m_m.EnableWindow(true);
		OnMatch();
	}else{
		m_r.ShowWindow(SW_HIDE);
		if(!m_selb) m_m.EnableWindow(false);
		OnMatch();
	}	
}

void CAppendOptiosn::OnCl() 
{
	UpdateData();

//	if(!m_cl.IsWindowEnabled())
//		return;

	if(m_cl.GetCheck()){
		m_c.ShowWindow(SW_SHOW);
		if(m_selb) m_m.EnableWindow(true);
		OnMatch();
	}else{
		m_c.ShowWindow(SW_HIDE);
		if(m_selb) m_m.EnableWindow(false);
		OnMatch();
	}	
}

void CAppendOptiosn::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
