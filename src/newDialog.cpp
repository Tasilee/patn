// newDialog.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "newDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CnewDialog dialog


CnewDialog::CnewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CnewDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CnewDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//id = CnewDialog::IDD;
	id = IDD_NEWDIALOG;
	parent = pParent;
	m_time = false;
}


void CnewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CnewDialog)
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CnewDialog, CDialog)
	//{{AFX_MSG_MAP(CnewDialog)
	ON_WM_CANCELMODE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CnewDialog message handlers

bool CnewDialog::Create(CString str)
{
	m_str = str;
	return (CDialog::Create(id,parent))?true:false;
}


void CnewDialog::inc()
{
	m_Progress.StepIt();
	RedrawWindow();
}


BOOL CnewDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(1);	

	if(m_str.GetLength())
		m_Label.SetWindowText(m_str);

	m_Cancel = false;

	//if(m_time == true){
		//this->SetTimer(1,200,NULL);
	//}
	
	//GetLocalTime(&m_systime);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CnewDialog::OnCancel() 
{
	m_Cancel = true;	
}

BOOL CnewDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}

void CnewDialog::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CnewDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CnewDialog::OnDestroy() 
{
	//KillTimer(1);
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here	
}

//to use the timer: set m_timer = true; m_end; m_start;

void CnewDialog::OnTimer(UINT nIDEvent) 
{
/*	KillTimer(1);

	SYSTEMTIME tmp;
	GetLocalTime(&tmp);
	
	CString text;

	long mpass,spass,mend,send;

	long milliseconds;

	spass = 60*60*(tmp.wHour - m_systime.wHour) + 60*(tmp.wMinute - m_systime.wMinute) + (tmp.wSecond - m_systime.wSecond);

	milliseconds = spass*60 + (tmp.wMilliseconds - m_systime.wMilliseconds);

	send = (long)((*m_end / *m_start*milliseconds)/60);		
	
	mpass = spass/60;
	spass = spass%60;
	mend = send/60;
	send = send%60;
	text.Format("%d:%d / %d:%d",mpass,spass,mend,send);
	m_Label.SetWindowText(m_str + text);

	SetTimer(1,200,NULL);
*/	
	CDialog::OnTimer(nIDEvent);
}
