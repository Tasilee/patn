// AsimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "fusionthread.h"
#include "FusionDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFusionDlg dialog


CFusionDlg::CFusionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFusionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFusionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFusionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFusionDlg)
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFusionDlg, CDialog)
	//{{AFX_MSG_MAP(CFusionDlg)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFusionDlg message handlers

void CFusionDlg::Setup(CPatnDoc*pDoc,bool rows,
			CMatrix* pMatrix,
            CFuseTable* pFuseTable,
            long nFuseTechnique,
			double* pCutOff,  // TODO; temp code to facilitate no of groups line in dendrogram
            long nNoOfGroups,
            double nBeta,
			CArray<long>* pGroups)
{
	m_pDoc = pDoc;
	m_rows = rows;
	m_pMatrix = pMatrix;
	m_pFuseTable = pFuseTable;
	m_nFuseTechnique = nFuseTechnique;
	m_pCutOff = pCutOff;
	m_nNoOfGroups = nNoOfGroups;
	m_nBeta = nBeta;
	m_pGroups = pGroups;

}

BOOL CFusionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_hEventKilledFusionThread = CreateEvent(NULL, FALSE, FALSE, NULL);

	//write into the dlg title & 
	// setup progress range
	long max;
	long nCols,nRows;
	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(m_rows){
		max = nRows;
		max = (max*(max-1));
		switch(m_nFuseTechnique){
		case NearestNeighbour:
			m_Label.SetWindowText("Nearest Neighbour Fusion on Rows");			
			break;
		case FurthestNeighbour:
			m_Label.SetWindowText("Furthest Neighbour Fusion on Rows");			
			break;
		case FlexibleWPGMA:
			m_Label.SetWindowText("Flexible WPGMA Fusion on Rows");			
			break;
		case FlexibleUPGMA:
			m_Label.SetWindowText("Flexible UPGMA Fusion on Rows");			
			break;
		}
	}else{
		max = nCols;
		max = (max*(max-1));
		switch(m_nFuseTechnique){
		case NearestNeighbour:
			m_Label.SetWindowText("Nearest Neighbour Fusion on Columns");			
			break;
		case FurthestNeighbour:
			m_Label.SetWindowText("Furthest Neighbour Fusion on Columns");			
			break;
		case FlexibleWPGMA:
			m_Label.SetWindowText("Flexible WPGMA Fusion on Columns");			
			break;
		case FlexibleUPGMA:
			m_Label.SetWindowText("Flexible UPGMA Fusion on Columns");			
			break;
		}

	}
	m_Progress.SetRange32(0,max);
	m_Progress.SetStep(max/100);

	//all setup, safe to continue
	m_fusionthread = new CFusionThread;
	m_fusionthread->CreateThread(CREATE_SUSPENDED);

	m_fusionthread->m_pDoc = m_pDoc;
	m_fusionthread->m_dlg = this;
	m_fusionthread->m_type = m_type;
	m_fusionthread->m_value = &m_value;
	m_fusionthread->m_Step = max/100;
	m_value = !!m_fusionthread->Attach(m_pMatrix,m_pFuseTable,m_nFuseTechnique,
			m_pCutOff,m_nNoOfGroups,m_nBeta,m_pGroups);

	if(!m_value) {
		delete m_fusionthread;
		CDialog::OnCancel();
      //error handled by CFusionThread::Attach(...)
		return FALSE;
	}

	m_value = false;

	m_fusionthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	m_fusionthread->ResumeThread();

	if(((CPatnApp*)AfxGetApp())->silent_mode)
	{
		WaitForSingleObject(m_hEventKilledFusionThread,INFINITE);
		CDialog::OnOK();
		
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFusionDlg::OnCancel() 
{
	if(WaitForSingleObject(m_hEventKilledFusionThread,0) == WAIT_OBJECT_0){
		CDialog::OnOK();
	}else{
		SetEvent(m_fusionthread->m_hEventKillFusionThread);
	}
}

void CFusionDlg::StepIt()
{
	m_Progress.StepIt();
}

