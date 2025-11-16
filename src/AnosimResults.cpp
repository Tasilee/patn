// AnosimResults.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "AnosimResults.h"
#include ".\anosimresults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnosimResults dialog


CAnosimResults::CAnosimResults(CPatnDoc * pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CAnosimResults::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnosimResults)
	m_Best = 0;
	m_Bw = 0;
	m_Iterations = 0;
	m_Stat = 0;
	m_2Best = 0;
	m_2Bw = 0;
	m_2Iterations = 0;
	m_2Stat = 0;
	m_2Seed = 0;
	m_Seed = 0;
	//}}AFX_DATA_INIT
	if(!pDoc->m_ColumnAsim_pairs && pDoc->m_ANOSIMColsExists){
		m_2Best = pDoc->m_ColumnAsim_rbest;
		m_2Bw = pDoc->m_ColumnAsim_save;
		m_2Stat = pDoc->m_ColumnAsim_stat;		
	}
	if(pDoc->m_ANOSIMColsExists){
		m_2Iterations = pDoc->m_ColumnAsim_iterations;		
		m_2Seed = pDoc->m_ColumnAsim_seed;
	}
	
	if(!pDoc->m_RowAsim_pairs && pDoc->m_ANOSIMRowsExists){
		m_Best = pDoc->m_RowAsim_rbest;
		m_Bw = pDoc->m_RowAsim_save;		
		m_Stat = pDoc->m_RowAsim_stat;
		
	}
	if(pDoc->m_ANOSIMRowsExists){
		m_Iterations = pDoc->m_RowAsim_iterations;		
		m_Seed = pDoc->m_RowAsim_seed;
	}
	
	m_pDoc = pDoc;
}


void CAnosimResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnosimResults)
	DDX_Control(pDX, IDC_ROWPAIRS, m_RowPairs);
	DDX_Control(pDX, IDC_COLPAIRS, m_ColPairs);
	DDX_Text(pDX, IDC_BEST, m_Best);
	DDX_Text(pDX, IDC_BW, m_Bw);
	DDX_Text(pDX, IDC_ITERATIONS, m_Iterations);
	DDX_Text(pDX, IDC_STAT, m_Stat);
	DDX_Text(pDX, IDC_BEST2, m_2Best);
	DDX_Text(pDX, IDC_BW2, m_2Bw);
	DDX_Text(pDX, IDC_ITERATIONS2, m_2Iterations);
	DDX_Text(pDX, IDC_STAT2, m_2Stat);
	DDX_Text(pDX, IDC_SEED2, m_2Seed);
	DDX_Text(pDX, IDC_SEED, m_Seed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnosimResults, CDialog)
	//{{AFX_MSG_MAP(CAnosimResults)
	ON_BN_CLICKED(IDC_ROWPAIRS, OnRowpairs)
	ON_BN_CLICKED(IDC_COLPAIRS, OnColpairs)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_STAT, OnEnChangeStat)
	ON_STN_CLICKED(IDC_1, OnStnClicked1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnosimResults message handlers

BOOL CAnosimResults::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	if(!m_pDoc->m_RowAsim_pairs || m_pDoc->m_AnosimRowView){
		m_RowPairs.EnableWindow(false);	
	}else{
		m_RowPairs.EnableWindow(true);
	}
	if(!m_pDoc->m_ColumnAsim_pairs || m_pDoc->m_AnosimColView)
		m_ColPairs.EnableWindow(false);		
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnosimResults::OnRowpairs() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	if(!m_pDoc->m_AnosimRowView){
		m_pDoc->m_AnosimPairView = false;
		m_pDoc->m_AnosimRowView = true;
		pApp->OpenAnosimView(m_pDoc);
		CDialog::OnCancel();
	}
}

void CAnosimResults::OnColpairs() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	if(!m_pDoc->m_AnosimColView){
		m_pDoc->m_AnosimPairView = true;
		m_pDoc->m_AnosimColView = true;
		pApp->OpenAnosimView(m_pDoc);
		CDialog::OnCancel();
	}
}

void CAnosimResults::OnEnChangeStat()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CAnosimResults::OnStnClicked1()
{
	// TODO: Add your control notification handler code here
}
