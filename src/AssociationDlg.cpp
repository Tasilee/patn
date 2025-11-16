// AssociationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "associationthread.h"
#include "AssociationDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssociationDlg dialog


CAssociationDlg::CAssociationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAssociationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAssociationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAssociationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAssociationDlg)
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAssociationDlg, CDialog)
	//{{AFX_MSG_MAP(CAssociationDlg)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAssociationDlg message handlers

void CAssociationDlg::Setup(CPatnDoc*pDoc,bool rows)
{
	m_pDoc = pDoc;
	m_rows = rows;
}

BOOL CAssociationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	this->ShowWindow(SW_HIDE);
	m_hEventKilledAssociationThread = CreateEvent(NULL, FALSE, FALSE, NULL);

	//write into the dlg title & 
	// setup progress range
	long max;
	long nRows,nCols;
	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(m_pDoc->IsExtrinsicColumns())
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();


	if(!m_rows){
		max = nCols*(nCols-1)/2;
		if(!((CPatnApp*)AfxGetApp())->silent_mode)
		{
			switch(m_type){
			case BrayCurtis:
				m_Label.SetWindowText("Bray & Curtis Association on Columns");
				break;
			case GowerMetric:	
				max += nRows;
				m_Label.SetWindowText("Gower Metric Association on Columns");			
				break;
			case MinkowskiSeries:
				max += nCols-1;
				m_Label.SetWindowText("Minkowski Series Association on Columns");			
				break;
			case TwoStep:
				max += nCols*nCols;
				m_Label.SetWindowText("Two Step Association on Columns");			
				break;
			case Kulzinski:
				m_Label.SetWindowText("Kulczynski Association on Columns");			
				break;
			}
		}
	}else{
		max = nRows*(nRows-1)/2;
		if(!((CPatnApp*)AfxGetApp())->silent_mode)
		{
			switch(m_type){
			case BrayCurtis:
				m_Label.SetWindowText("Bray & Curtis Association on Rows");			
				break;
			case GowerMetric:
				max += nCols;
				m_Label.SetWindowText("Gower Metric Association on Rows");			
				break;
			case MinkowskiSeries:
				max += nRows-1;
				m_Label.SetWindowText("Minkowski Series Association on Rows");			
				break;
			case TwoStep:	
				max += nRows*nRows;
				m_Label.SetWindowText("Two Step Association on Rows");			
				break;
			case Kulzinski:
				m_Label.SetWindowText("Kulczynski Association on Rows");
				break;
			}
		}
	}
	if(!((CPatnApp*)AfxGetApp())->silent_mode)m_Progress.SetRange32(0,max);
	if(!((CPatnApp*)AfxGetApp())->silent_mode)m_Progress.SetStep(max/100);

	//all setup, safe to continue
	m_associationthread = new CAssociationThread;
	m_associationthread->CreateThread(CREATE_SUSPENDED);

	m_associationthread->m_pDoc = m_pDoc;
	m_associationthread->m_dlg = this;
	
	m_associationthread->m_rows = m_rows;
	m_associationthread->m_type = m_type;
	m_associationthread->m_value = &m_value;
	m_associationthread->m_Step = max/100;
	m_value = false;

	m_associationthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	m_associationthread->ResumeThread();

	
	if(((CPatnApp*)AfxGetApp())->silent_mode)
	{
		WaitForSingleObject(m_hEventKilledAssociationThread,INFINITE);
		CDialog::OnOK();
		
		return FALSE;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAssociationDlg::StepIt()
{
	m_Progress.StepIt();
}


void CAssociationDlg::OnCancel() 
{
	if(WaitForSingleObject(m_hEventKilledAssociationThread,0) == WAIT_OBJECT_0){
		CDialog::OnOK();
	}else{
		SetEvent(m_associationthread->m_hEventKillAssociationThread);
	}
}



