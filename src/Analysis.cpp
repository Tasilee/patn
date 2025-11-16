// analysis.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "TableView.h"
#include "Analysis.h"
#include "Fuse.h"
#include "Association.h"
#include "AssociationDlg.h"                      
#include "Classification.h"
#include "Ordination.h"
#include "autoevaluation.h"
#include "colr.h"
#include "mst.h"
#include "gsta.h"
#include "evaluation.h"
#include "evaluationrecipe.h"

#include <math.h>
#include <stdlib.h>
#include ".\analysis.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

long evaluations;
/////////////////////////////////////////////////////////////////////////////
// CAssociationPage property page

IMPLEMENT_DYNCREATE(CAssociationPage, CPropertyPage)

CAssociationPage::CAssociationPage() : CPropertyPage(CAssociationPage::IDD)
, m_noverride(false)
{
	//{{AFX_DATA_INIT(CAssociationPage)
	m_nAssocMeasure = CAssociation::none;
	//}}AFX_DATA_INIT
}

CAssociationPage::~CAssociationPage()
{
}

void CAssociationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAssociationPage)
	DDX_Control(pDX, IDC_CONSTANT, m_Constant);
	DDX_Control(pDX, IDC_ASSOC_MEASURES, m_listAssocMeasures);
	DDX_LBIndex(pDX, IDC_ASSOC_MEASURES, m_nAssocMeasure);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ALLOWASSOC, m_override_defaults);
	DDX_Control(pDX, IDC_INFO, m_info);
	DDX_Control(pDX, IDC_INFO3, m_info3);
	DDX_Control(pDX, IDC_INFO2, m_info2);
}


BEGIN_MESSAGE_MAP(CAssociationPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAssociationPage)
	ON_LBN_SELCHANGE(IDC_ASSOC_MEASURES, OnSelchangeAssocMeasures)
	ON_EN_CHANGE(IDC_CONSTANT, OnChangeConstant)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ALLOWASSOC, OnBnClickedAllowassoc)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAssociationPage message handlers

BOOL CAssociationPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString text;
	text.Format("%.4f",m_nConstant);
	
	// TODO: Add extra initialization here
	m_listAssocMeasures.AddString("None");
	m_listAssocMeasures.AddString("Bray and Curtis");
	m_listAssocMeasures.AddString("Gower Metric");
	m_listAssocMeasures.AddString("Minkowski Series");
	m_listAssocMeasures.AddString("Two-step");
	m_listAssocMeasures.AddString("Kulczynski");
	m_listAssocMeasures.SetCurSel(m_nAssocMeasure);

	if(m_noverride)
	{
		//enable list
		this->m_listAssocMeasures.EnableWindow(true);
		m_override_defaults.SetCheck(1);
	}
	else
	{
		this->m_listAssocMeasures.EnableWindow(false);
	}

	//show tag information
	if(m_LargeNoOfObjs)
	{
		this->m_override_defaults.ShowWindow(SW_SHOW);
		this->m_info.ShowWindow(SW_SHOW);
		this->m_info2.ShowWindow(SW_SHOW);
		this->m_info3.ShowWindow(SW_SHOW);
	}
	else
	{
		this->m_override_defaults.ShowWindow(SW_HIDE);
		this->m_info.ShowWindow(SW_HIDE);
		this->m_info2.ShowWindow(SW_HIDE);
		this->m_info3.ShowWindow(SW_HIDE);
	}




	m_Constant.SetWindowText(text);
	if(m_nAssocMeasure != CAssociation::MinkowskiSeries)
		m_Constant.EnableWindow(FALSE);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAssociationPage::OnSelchangeAssocMeasures() 
{
	// TODO: Add your control notification handler code here
	
	m_nAssocMeasure = m_listAssocMeasures.GetCurSel();
	if ( m_nAssocMeasure == LB_ERR )
		m_nAssocMeasure = 0;
	if ( m_nAssocMeasure == CAssociation::none )
	{
		UpdateData(FALSE);
	}
	if(m_nAssocMeasure == CAssociation::MinkowskiSeries)
		m_Constant.EnableWindow(TRUE);
	else
		m_Constant.EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CClassRowsPage property page

IMPLEMENT_DYNCREATE(CClassRowsPage, CPropertyPage)

CClassRowsPage::CClassRowsPage() : CPropertyPage(CClassRowsPage::IDD)
, m_constant(0)
{
	//{{AFX_DATA_INIT(CClassRowsPage)
	m_nBeta = 0.0;
	m_nClassStrategy = CClassification::none;
	//}}AFX_DATA_INIT
	m_nFuseTechnique = -1;
	m_nAllocAssocMeasure = -1;
}

CClassRowsPage::~CClassRowsPage()
{
}

void CClassRowsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassRowsPage)
	DDX_Control(pDX, IDC_NUM_GROUPS, m_editNoOfGroups);
	DDX_Control(pDX, IDC_BETA, m_editBeta);
	DDX_Control(pDX, IDC_FUSE_TECHNIQUE, m_comboFuseTechnique);
	DDX_Control(pDX, IDC_ALOC_ASSOC_MEASURE, m_listAlocAssocMeasure);
	DDX_Text(pDX, IDC_NUM_GROUPS, m_nNoOfGroups);
	DDX_Radio(pDX, IDC_NONE, m_nClassStrategy);
	DDX_Text(pDX, IDC_BETA, m_nBeta);
	DDV_MinMaxDouble(pDX, m_nBeta, -1., 1.);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_CONSTANT, m_constant);
	DDX_Control(pDX, IDC_CONSTANT, m_constant_box);
}


BEGIN_MESSAGE_MAP(CClassRowsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CClassRowsPage)
	ON_CBN_SELCHANGE(IDC_FUSE_TECHNIQUE, OnSelchangeFuseTechnique)
	ON_LBN_SELCHANGE(IDC_ALOC_ASSOC_MEASURE, OnSelchangeAlocAssocMeasure)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	ON_BN_CLICKED(IDC_ALOC, OnAloc)
	ON_BN_CLICKED(IDC_FUSE, OnFuse)
	ON_EN_UPDATE(IDC_NUM_GROUPS, OnUpdateNumGroups)
	ON_EN_KILLFOCUS(IDC_BETA, OnKillfocusBeta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CClassRowsPage message handlers

BOOL CClassRowsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_listAlocAssocMeasure.AddString("Bray and Curtis");
	m_listAlocAssocMeasure.AddString("Gower Metric");
	m_listAlocAssocMeasure.AddString("Minkowski");
	m_comboFuseTechnique.SetCurSel(m_nFuseTechnique);
	m_listAlocAssocMeasure.SetCurSel(m_nAllocAssocMeasure-1);

	if(m_nClassStrategy != CClassification::fuse)
		OnAloc();
	else
		OnFuse();
	
 	UpdateData(FALSE);
	//if ( m_nClassStrategy != CClassification::alloc )
	//	m_listAlocAssocMeasure.EnableWindow(FALSE);
	//if ( m_nClassStrategy != CClassification::fuse )
	//{
	//	m_comboFuseTechnique.EnableWindow(FALSE);
	//	m_editBeta.EnableWindow(FALSE);
	//}
	//if ( m_nClassStrategy == CClassification::none )
	//	m_editNoOfGroups.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClassRowsPage::OnSelchangeFuseTechnique() 
{
 	int nFuseTechnique = m_comboFuseTechnique.GetCurSel();

	switch (nFuseTechnique)
	{
		case CFuse::NearestNeighbour:
		case CFuse::FurthestNeighbour:
			m_nFuseTechnique = nFuseTechnique;
			m_nBeta = 0.0;
			UpdateData(FALSE);
			//m_editBeta.EnableWindow(FALSE);
			break;

		case CFuse::FlexibleWPGMA:
		case CFuse::FlexibleUPGMA:
			if ( m_nFuseTechnique != CFuse::FlexibleWPGMA &&
				 m_nFuseTechnique != CFuse::FlexibleUPGMA )
			{
				m_nBeta = -0.1;
			}
			m_nFuseTechnique = nFuseTechnique;
			UpdateData(FALSE);
			m_editBeta.EnableWindow(TRUE);
			break;
	}
}

void CClassRowsPage::OnSelchangeAlocAssocMeasure() 
{
	if(m_listAlocAssocMeasure.GetCurSel() == -1){
		if(m_pDoc->GetSparcityRatio() < 50)
			m_nAllocAssocMeasure = CAssociation::GowerMetric;
		else
			m_nAllocAssocMeasure = CAssociation::BrayCurtis;
	}else{
		m_nAllocAssocMeasure = m_listAlocAssocMeasure.GetCurSel()+1;	
	}
	if(m_listAlocAssocMeasure.GetCurSel() == 2)
		GetDlgItem(IDC_CONSTANT)->EnableWindow(true);
	else
		GetDlgItem(IDC_CONSTANT)->EnableWindow(false);
}

void CClassRowsPage::OnNone() 
{
	m_listAlocAssocMeasure.EnableWindow(FALSE);
	m_comboFuseTechnique.EnableWindow(FALSE);
	m_editBeta.EnableWindow(FALSE);
	m_editNoOfGroups.EnableWindow(FALSE);
	this->m_constant_box.EnableWindow(false);
	//m_nFuseTechnique = -1;
	//m_nAllocAssocMeasure = -1;
	UpdateData(TRUE);	
}

void CClassRowsPage::OnAloc() 
{
	m_listAlocAssocMeasure.EnableWindow(TRUE);
	//m_listAlocAssocMeasure.SetCurSel(1);
	m_comboFuseTechnique.EnableWindow(FALSE);
	m_editBeta.EnableWindow(FALSE);
	m_editNoOfGroups.EnableWindow(TRUE);
	m_nAllocAssocMeasure = m_listAlocAssocMeasure.GetCurSel()+1;
	//m_nFuseTechnique = -1;
	this->m_listAlocAssocMeasure.EnableWindow(true);
	if(m_listAlocAssocMeasure.GetCurSel() == 2)
		GetDlgItem(IDC_CONSTANT)->EnableWindow(true);
	else
		GetDlgItem(IDC_CONSTANT)->EnableWindow(false);
	
	UpdateData(TRUE);	
}

void CClassRowsPage::OnFuse() 
{
	//m_listAlocAssocMeasure.EnableWindow(FALSE);
	m_comboFuseTechnique.EnableWindow(TRUE);
	m_editBeta.EnableWindow(TRUE);
	m_editNoOfGroups.EnableWindow(TRUE);
 	m_nFuseTechnique = m_comboFuseTechnique.GetCurSel();
	//m_nAllocAssocMeasure = -1;
	GetDlgItem(IDC_CONSTANT)->EnableWindow(false);
	this->m_listAlocAssocMeasure.EnableWindow(false);
	UpdateData(TRUE);	
}

void CClassRowsPage::OnKillfocusBeta() 
{
	UpdateData(TRUE);	
}

void CClassRowsPage::OnUpdateNumGroups() 
{
//	int x;
//	CString strMessage;
//	CString number;
//
//	x = m_nNoOfGroups;
//	UpdateData(TRUE);
//	if(m_nNoOfGroups <= 0 || m_nNoOfGroups >= m_pDoc->m_aRowGroups.GetSize())
//	{	
//		strMessage = "Group number is out of range. \n";
//		number.Format("%d",m_pDoc->m_aRowGroups.GetSize());
//		strMessage += "Must be > 1 and < " + number + ".\n";
//		AfxGetMainWnd()->MessageBox(strMessage,"Invalid Group Number");
//		m_editNoOfGroups.SetSel(0,m_editNoOfGroups.LineLength());
//		number.Format("%d",x);
//		m_editNoOfGroups.ReplaceSel(number);
//		UpdateData(TRUE);
//	}
}
/////////////////////////////////////////////////////////////////////////////
// CClassColsPage property page

IMPLEMENT_DYNCREATE(CClassColsPage, CPropertyPage)

CClassColsPage::CClassColsPage() : CPropertyPage(CClassColsPage::IDD)
{
	//{{AFX_DATA_INIT(CClassColsPage)
	m_nNoOfGroups = 0;
	m_nBeta = -0.1;
	m_nClassStrategy = CClassification::none;
	//}}AFX_DATA_INIT
	m_nFuseTechnique = -1;
}

CClassColsPage::~CClassColsPage()
{
}

void CClassColsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassColsPage)
	DDX_Control(pDX, IDC_FUSE_TECHNIQUE, m_comboFuseTechnique);
	DDX_Control(pDX, IDC_NUM_GROUPS, m_editNoOfGroups);
	DDX_Control(pDX, IDC_BETA, m_editBeta);
	DDX_Text(pDX, IDC_BETA, m_nBeta);
	DDV_MinMaxDouble(pDX, m_nBeta, -1., 1.);
	DDX_Text(pDX, IDC_NUM_GROUPS, m_nNoOfGroups);
	DDX_Radio(pDX, IDC_NONE, m_nClassStrategy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClassColsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CClassColsPage)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	ON_BN_CLICKED(IDC_FUSE, OnFuse)
	ON_CBN_SELCHANGE(IDC_FUSE_TECHNIQUE, OnSelchangeFuseTechnique)
	ON_EN_UPDATE(IDC_NUM_GROUPS, OnUpdateNumGroups)
	ON_EN_KILLFOCUS(IDC_BETA, OnKillfocusBeta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CClassColsPage message handlers

BOOL CClassColsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_comboFuseTechnique.SetCurSel(m_nFuseTechnique);
	UpdateData(FALSE);
	if ( m_nClassStrategy == CClassification::none )
	{
		//m_editNoOfGroups.EnableWindow(FALSE);
		//m_comboFuseTechnique.EnableWindow(FALSE);
		//m_editBeta.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClassColsPage::OnNone() 
{
	//m_comboFuseTechnique.EnableWindow(FALSE);
	//m_editBeta.EnableWindow(FALSE);
	//m_editNoOfGroups.EnableWindow(FALSE);
	m_nFuseTechnique = -1;
	UpdateData(TRUE);	
}

void CClassColsPage::OnFuse() 
{
	m_comboFuseTechnique.EnableWindow(TRUE);
	m_editBeta.EnableWindow(TRUE);
	m_editNoOfGroups.EnableWindow(TRUE);
 	m_nFuseTechnique = m_comboFuseTechnique.GetCurSel();
	UpdateData(TRUE);
}

void CClassColsPage::OnSelchangeFuseTechnique() 
{
 	int nFuseTechnique = m_comboFuseTechnique.GetCurSel();

	switch (nFuseTechnique)
	{
		case CFuse::NearestNeighbour:
		case CFuse::FurthestNeighbour:
			m_nFuseTechnique = nFuseTechnique;
			m_nBeta = 0.0;
			UpdateData(FALSE);
			//m_editBeta.EnableWindow(FALSE);
			break;

		case CFuse::FlexibleWPGMA:
		case CFuse::FlexibleUPGMA:
			if ( m_nFuseTechnique != CFuse::FlexibleWPGMA &&
				 m_nFuseTechnique != CFuse::FlexibleUPGMA )
			{
				m_nBeta = -0.1;
			}
			m_nFuseTechnique = nFuseTechnique;
			UpdateData(FALSE);
			m_editBeta.EnableWindow(TRUE);
			break;
	}
}

void CClassColsPage::OnKillfocusBeta() 
{
	UpdateData(TRUE);	
}

void CClassColsPage::OnUpdateNumGroups() 
{
/*	int x;
	CString strMessage;
	CString number;

	x = m_nNoOfGroups;
	UpdateData(TRUE);
	if(m_nNoOfGroups <= 0 || m_nNoOfGroups >= m_pDoc->m_aColumnGroups.GetSize())
	{	
		strMessage = "Group number is out of range. \n";
		number.Format("%d",m_pDoc->m_aColumnGroups.GetSize());
		strMessage += "Must be > 1 and < " + number + ".\n";
		AfxGetMainWnd()->MessageBox(strMessage,"Invalid Group Number");
		m_editNoOfGroups.SetSel(0,m_editNoOfGroups.LineLength());
		number.Format("%d",x);
		m_editNoOfGroups.ReplaceSel(number);
		UpdateData(TRUE);
	}
*/		
}
/////////////////////////////////////////////////////////////////////////////
// COrdinationPage property page

IMPLEMENT_DYNCREATE(COrdinationPage, CPropertyPage)

COrdinationPage::COrdinationPage() : CPropertyPage(COrdinationPage::IDD)
{
	//{{AFX_DATA_INIT(COrdinationPage)
	m_Cut = 0.9;
	m_Ord3d = -1;
	m_Ordination = FALSE;
	m_NumberOfIterations = 10;
	m_numIterations = 50;
	m_numSeed = 1235;
	m_bWithTunneling = FALSE;
	m_bCalculateStats = FALSE;
	m_movementfactor = 50.0;
	m_numtunnels = 20;
	//}}AFX_DATA_INIT
}

COrdinationPage::~COrdinationPage()
{
}

void COrdinationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrdinationPage)
	DDX_Control(pDX, IDC_CHECK3, m_tunnel3);
	DDX_Control(pDX, IDC_STATIC2, m_tunnel2);
	DDX_Control(pDX, IDC_STATIC1, m_tunnel1);
	DDX_Control(pDX, IDC_EDIT6, m_NumTunnels);
	DDX_Control(pDX, IDC_EDIT5, m_Movement);
	DDX_Control(pDX, IDC_CHECK2, m_withtunneling);
	DDX_Control(pDX, IDC_SEED, m_CtrlSeed);
	DDX_Control(pDX, IDC_ITERATIONS, m_CtrlIterations);
	DDX_Control(pDX, IDC_EDIT2, m_Iterations);
	DDX_Control(pDX, IDC_CHECK1, m_OrdinationCheck);
	DDX_Control(pDX, IDC_EDIT1, m_EditCut);
	DDX_Text(pDX, IDC_EDIT1, m_Cut);
	DDX_Radio(pDX, IDC_3D, m_Ord3d);
	DDX_Check(pDX, IDC_CHECK1, m_Ordination);
	DDX_Text(pDX, IDC_EDIT2, m_NumberOfIterations);
	DDX_Text(pDX, IDC_ITERATIONS, m_numIterations);
	DDX_Text(pDX, IDC_SEED, m_numSeed);
	DDX_Check(pDX, IDC_CHECK2, m_bWithTunneling);
	DDX_Check(pDX, IDC_CHECK3, m_bCalculateStats);
	DDX_Text(pDX, IDC_EDIT5, m_movementfactor);
	DDX_Text(pDX, IDC_EDIT6, m_numtunnels);
	DDV_MinMaxInt(pDX, m_numtunnels, 0, 2147483647);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrdinationPage, CPropertyPage)
	//{{AFX_MSG_MAP(COrdinationPage)
	ON_BN_CLICKED(IDC_CHECK1, OnRandom)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COrdinationPage message handlers

/////////////////////////////////////////////////////////////////////////////
// CAnalysisDlg dialog

CAnalysisDlg::CAnalysisDlg(CPatnDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CAnalysisDlg::IDD, pParent)
{
	m_pDoc = pDoc;
	//{{AFX_DATA_INIT(CAnalysisDlg)
	m_bColumns = FALSE;
	m_bRows = TRUE;
	m_evaluations = FALSE;
	//}}AFX_DATA_INIT
}

BOOL CAnalysisDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	Initialize();
	if ( !m_bColumns )
		m_btnSetParmsColumns.EnableWindow(FALSE);
	if ( m_bRows )
		m_btnSetParmsRows.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalysisDlg::Initialize()
{
	long n;
		
	if ( m_pDoc->IsExtrinsicRows() )			
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	m_AnalysisRecipeRows.nOverride = true;

	if(m_pDoc->m_AnalysisRecipe.nFirstTime){
		double nSparcityRatio = m_pDoc->GetSparcityRatio();
		if ( nSparcityRatio < 50 ){	// less than 40%
			m_AnalysisRecipeRows.nAssocMeasure = CAssociation::GowerMetric;
			m_AnalysisRecipeCols.nAssocMeasure = CAssociation::GowerMetric;
			m_AnalysisRecipeRows.nAllocAssocMeasure = CAssociation::GowerMetric;
		}else{
			m_AnalysisRecipeRows.nAssocMeasure = CAssociation::BrayCurtis;
			m_AnalysisRecipeCols.nAssocMeasure = CAssociation::TwoStep;
			m_AnalysisRecipeRows.nAllocAssocMeasure = CAssociation::TwoStep;
		}
		//m_AnalysisRecipeRows.nAllocAssocMeasure = -1;
		
		//m_AnalysisRecipeCols.nAssocMeasure = CAssociation::none;
		m_AnalysisRecipeRows.nClassStrategy = CClassification::none;
		m_AnalysisRecipeCols.nClassStrategy = CClassification::fuse;
		m_AnalysisRecipeRows.nFuseTechnique = CFuse::FlexibleUPGMA;
		m_AnalysisRecipeCols.nFuseTechnique = CFuse::FlexibleUPGMA;
		m_AnalysisRecipeRows.nBeta = -0.1;
		m_AnalysisRecipeRows.dPower = 2;
		m_AnalysisRecipeCols.nBeta = -0.1;
		m_AnalysisRecipeRows.nOrdination = true;
		m_AnalysisRecipeRows.nOrd3d = 3;
		m_AnalysisRecipeCols.nOrdination = false; //not used!
		m_AnalysisRecipeRows.nCut = 0.9;
		m_AnalysisRecipeRows.nMinkConst = 2.0;
		m_AnalysisRecipeCols.nMinkConst = 2.0;
		m_AnalysisRecipeRows.nOrdIterations = 50;
		m_AnalysisRecipeRows.nOrdRandomStarts = 10;
		m_AnalysisRecipeRows.nOrdSeed = 1235;
		m_AnalysisRecipeRows.bCalculateStats = false;
		m_AnalysisRecipeRows.bWithTunnels = false;
		m_AnalysisRecipeRows.nTunnels = 20;
		m_AnalysisRecipeRows.dMovementFactor = 50.0;
		m_AnalysisRecipeRows.nOverride = true;

		
		if(n <= 100)
			m_AnalysisRecipeRows.nClassStrategy = CClassification::fuse;
		else
		{
			m_AnalysisRecipeRows.nClassStrategy = CClassification::alloc;
			m_AnalysisRecipeRows.nAssocMeasure = CAssociation::none;
			m_AnalysisRecipeRows.nOrdination = false;
			m_AnalysisRecipeRows.nOverride = false;
		}

		double nSqrt = sqrt(n*1.0);
		long nNoOfGroups = (long)nSqrt;
		if ( (nSqrt-nNoOfGroups) > 0 )
			nNoOfGroups++;  // round up
		m_AnalysisRecipeRows.nNoOfGroups = nNoOfGroups;

		if ( m_pDoc->IsExtrinsicColumns() )
			n = m_pDoc->GetIdxLeftExtrinsicColumn();
		else
			n = m_pDoc->GetNoOfColumns();

		nSqrt = sqrt(n*1.0);
		nNoOfGroups = (long)nSqrt;
		if ( (nSqrt-nNoOfGroups) > 0 )
			nNoOfGroups++;  // round up
		m_AnalysisRecipeCols.nNoOfGroups = nNoOfGroups;
		m_pDoc->m_AnalysisRecipe.nFirstTime = false;

		m_pDoc->m_AnalysisRecipe.rows.nAssocMeasure=m_AnalysisRecipeRows.nAssocMeasure;
		m_pDoc->m_AnalysisRecipe.rows.nClassStrategy=m_AnalysisRecipeRows.nClassStrategy;
		m_pDoc->m_AnalysisRecipe.rows.nAllocAssocMeasure=m_AnalysisRecipeRows.nAllocAssocMeasure;
		m_pDoc->m_AnalysisRecipe.rows.nOrdIterations=m_AnalysisRecipeRows.nOrdIterations;
		m_pDoc->m_AnalysisRecipe.rows.nOrdRandomStarts=m_AnalysisRecipeRows.nOrdRandomStarts;
		m_pDoc->m_AnalysisRecipe.rows.nOrdSeed=m_AnalysisRecipeRows.nOrdSeed;
		m_pDoc->m_AnalysisRecipe.rows.nNoOfGroups=m_AnalysisRecipeRows.nNoOfGroups;
		m_pDoc->m_AnalysisRecipe.rows.nFuseTechnique=m_AnalysisRecipeRows.nFuseTechnique;
		m_pDoc->m_AnalysisRecipe.rows.nOrdination=m_AnalysisRecipeRows.nOrdination ;
		m_pDoc->m_AnalysisRecipe.rows.nOrd3d=m_AnalysisRecipeRows.nOrd3d;
		m_pDoc->m_AnalysisRecipe.rows.nCut=m_AnalysisRecipeRows.nCut;
		m_pDoc->m_AnalysisRecipe.rows.nMinkConst=m_AnalysisRecipeRows.nMinkConst;
		m_pDoc->m_AnalysisRecipe.rows.nBeta=m_AnalysisRecipeRows.nBeta;
		m_pDoc->m_AnalysisRecipe.rows.dPower=m_AnalysisRecipeRows.dPower;
		m_pDoc->m_AnalysisRecipe.cols.nAssocMeasure=m_AnalysisRecipeCols.nAssocMeasure;
		m_pDoc->m_AnalysisRecipe.cols.nMinkConst=m_AnalysisRecipeCols.nMinkConst;
		m_pDoc->m_AnalysisRecipe.cols.nNoOfGroups=m_AnalysisRecipeCols.nNoOfGroups;
		m_pDoc->m_AnalysisRecipe.cols.nClassStrategy=m_AnalysisRecipeCols.nClassStrategy;
		m_pDoc->m_AnalysisRecipe.cols.nFuseTechnique=m_AnalysisRecipeCols.nFuseTechnique;
		m_pDoc->m_AnalysisRecipe.cols.nBeta=m_AnalysisRecipeCols.nBeta;

		m_pDoc->m_AnalysisRecipe.rows.bCalculateStats = m_AnalysisRecipeRows.bCalculateStats;
		m_pDoc->m_AnalysisRecipe.rows.bWithTunnels = m_AnalysisRecipeRows.bWithTunnels;
		m_pDoc->m_AnalysisRecipe.rows.nTunnels = m_AnalysisRecipeRows.nTunnels;
		m_pDoc->m_AnalysisRecipe.rows.dMovementFactor = m_AnalysisRecipeRows.dMovementFactor;
		m_pDoc->m_AnalysisRecipe.rows.nOverride = m_AnalysisRecipeRows.nOverride;
	}else{
		m_AnalysisRecipeRows.nAssocMeasure = m_pDoc->m_AnalysisRecipe.rows.nAssocMeasure;
		m_AnalysisRecipeCols.nAssocMeasure = m_pDoc->m_AnalysisRecipe.cols.nAssocMeasure;
		m_AnalysisRecipeRows.nAllocAssocMeasure =m_pDoc->m_AnalysisRecipe.rows.nAllocAssocMeasure;
		m_AnalysisRecipeRows.nClassStrategy = m_pDoc->m_AnalysisRecipe.rows.nClassStrategy;
		m_AnalysisRecipeCols.nClassStrategy = m_pDoc->m_AnalysisRecipe.cols.nClassStrategy;
		m_AnalysisRecipeRows.nFuseTechnique = m_pDoc->m_AnalysisRecipe.rows.nFuseTechnique;
		m_AnalysisRecipeCols.nFuseTechnique = m_pDoc->m_AnalysisRecipe.cols.nFuseTechnique;
		m_AnalysisRecipeRows.nBeta = m_pDoc->m_AnalysisRecipe.rows.nBeta;
		m_AnalysisRecipeRows.dPower = m_pDoc->m_AnalysisRecipe.rows.dPower;
		m_AnalysisRecipeCols.nBeta = m_pDoc->m_AnalysisRecipe.cols.nBeta;
		m_AnalysisRecipeRows.nOrdination = m_pDoc->m_AnalysisRecipe.rows.nOrdination;
		m_AnalysisRecipeRows.nOrd3d = m_pDoc->m_AnalysisRecipe.rows.nOrd3d;

		m_AnalysisRecipeRows.bCalculateStats = m_pDoc->m_AnalysisRecipe.rows.bCalculateStats;
		m_AnalysisRecipeRows.bWithTunnels = m_pDoc->m_AnalysisRecipe.rows.bWithTunnels;
		m_AnalysisRecipeRows.nTunnels = m_pDoc->m_AnalysisRecipe.rows.nTunnels;
		m_AnalysisRecipeRows.dMovementFactor = m_pDoc->m_AnalysisRecipe.rows.dMovementFactor;

		m_AnalysisRecipeRows.nOverride = m_pDoc->m_AnalysisRecipe.rows.nOverride;
		
		m_AnalysisRecipeRows.nCut = m_pDoc->m_AnalysisRecipe.rows.nCut;
		m_AnalysisRecipeRows.nMinkConst = m_pDoc->m_AnalysisRecipe.rows.nMinkConst;
		m_AnalysisRecipeCols.nMinkConst = m_pDoc->m_AnalysisRecipe.cols.nMinkConst;
		m_AnalysisRecipeRows.nOrdIterations = m_pDoc->m_AnalysisRecipe.rows.nOrdIterations;
		m_AnalysisRecipeRows.nOrdRandomStarts = m_pDoc->m_AnalysisRecipe.rows.nOrdRandomStarts;
		m_AnalysisRecipeRows.nOrdSeed = m_pDoc->m_AnalysisRecipe.rows.nOrdSeed;
		m_AnalysisRecipeRows.nNoOfGroups = m_pDoc->m_AnalysisRecipe.rows.nNoOfGroups;
		m_AnalysisRecipeCols.nNoOfGroups = m_pDoc->m_AnalysisRecipe.cols.nNoOfGroups;
		
	}

	long i;
	//check for changes and set accordingly
	if(m_pDoc->m_FuseColChange)
	{
		for(i=0;i<m_pDoc->m_FuseTableCols.GetSize();i++)
			delete m_pDoc->m_FuseTableCols[i];
		m_pDoc->m_FuseTableCols.RemoveAll();
		m_pDoc->m_FuseTableCols.SetSize(0);
	}
	if(m_pDoc->m_FuseRowChange)
	{
		for(i=0;i<m_pDoc->m_FuseTableRows.GetSize();i++)
			delete m_pDoc->m_FuseTableRows[i];
		m_pDoc->m_FuseTableRows.RemoveAll();
		m_pDoc->m_FuseTableRows.SetSize(0);
	}
	if(m_pDoc->m_AssocColChange)
	{
		m_pDoc->m_AssocMatrixCols.Reset();

		for(i=0;i<m_pDoc->m_FuseTableCols.GetSize();i++)
			delete m_pDoc->m_FuseTableCols[i];
		m_pDoc->m_FuseTableCols.RemoveAll();
		m_pDoc->m_FuseTableCols.SetSize(0);
	}
	if(m_pDoc->m_AssocRowChange)
	{
		m_pDoc->m_AssocMatrixRows.Reset();

		for(i=0;i<m_pDoc->m_FuseTableRows.GetSize();i++)
			delete m_pDoc->m_FuseTableRows[i];
		m_pDoc->m_FuseTableRows.RemoveAll();
		m_pDoc->m_FuseTableRows.SetSize(0);

		for(i=0;i<m_pDoc->m_aRowXCoords.GetSize();i++){
			m_pDoc->m_aRowXCoords.SetAt(i,ANALYSIS_VALUE);
			m_pDoc->m_aRowYCoords.SetAt(i,ANALYSIS_VALUE);
			m_pDoc->m_aRowZCoords.SetAt(i,ANALYSIS_VALUE);
		}
		m_pDoc->m_OrdAsso = ""; 
		m_pDoc->m_OrdTime = "";
		m_pDoc->m_OrdValue = 0.0;
	}
	if(m_pDoc->m_OrdChange)
	{
		for(i=0;i<m_pDoc->m_aRowXCoords.GetSize();i++){
			m_pDoc->m_aRowXCoords.SetAt(i,ANALYSIS_VALUE);
			m_pDoc->m_aRowYCoords.SetAt(i,ANALYSIS_VALUE);
			m_pDoc->m_aRowZCoords.SetAt(i,ANALYSIS_VALUE);
		}
		m_pDoc->m_OrdAsso = ""; 
		m_pDoc->m_OrdTime = "";
		m_pDoc->m_OrdValue = 0.0;
	}
}

void CAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalysisDlg)
	DDX_Control(pDX, IDC_SET_PARMS_ROWS, m_btnSetParmsRows);
	DDX_Control(pDX, IDC_SET_PARMS_COLUMNS, m_btnSetParmsColumns);
	DDX_Radio(pDX, IDC_COLUMNS_NO, m_bColumns);
	DDX_Radio(pDX, IDC_ROWS_NO, m_bRows);
	DDX_Check(pDX, IDC_EVALUATION1, m_evaluations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalysisDlg, CDialog)
	//{{AFX_MSG_MAP(CAnalysisDlg)
	ON_BN_CLICKED(IDC_SET_PARMS_ROWS, OnSetParmsRows)
	ON_BN_CLICKED(IDC_SET_PARMS_COLUMNS, OnSetParmsColumns)
	ON_BN_CLICKED(IDC_COLUMNS_NO, OnColumnsNo)
	ON_BN_CLICKED(IDC_COLUMNS_YES, OnColumnsYes)
	ON_BN_CLICKED(IDC_ROWS_NO, OnRowsNo)
	ON_BN_CLICKED(IDC_ROWS_YES, OnRowsYes)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalysisDlg message handlers

void CAnalysisDlg::OnRun() 
{
	long i;
   CString errlog;
   
	m_pDoc->m_AnalysisRecipe.rows.nAssocMeasure=m_AnalysisRecipeRows.nAssocMeasure;
	m_pDoc->m_AnalysisRecipe.rows.nClassStrategy=m_AnalysisRecipeRows.nClassStrategy;
	m_pDoc->m_AnalysisRecipe.rows.nAllocAssocMeasure=m_AnalysisRecipeRows.nAllocAssocMeasure;
	m_pDoc->m_AnalysisRecipe.rows.nOrdIterations=m_AnalysisRecipeRows.nOrdIterations;
	m_pDoc->m_AnalysisRecipe.rows.nOrdRandomStarts=m_AnalysisRecipeRows.nOrdRandomStarts;
	m_pDoc->m_AnalysisRecipe.rows.nOrdSeed=m_AnalysisRecipeRows.nOrdSeed;
	m_pDoc->m_AnalysisRecipe.rows.nNoOfGroups=m_AnalysisRecipeRows.nNoOfGroups;
	m_pDoc->m_AnalysisRecipe.rows.nFuseTechnique=m_AnalysisRecipeRows.nFuseTechnique;
	m_pDoc->m_AnalysisRecipe.rows.nOrdination=m_AnalysisRecipeRows.nOrdination ;
	m_pDoc->m_AnalysisRecipe.rows.nOrd3d=m_AnalysisRecipeRows.nOrd3d;
	m_pDoc->m_AnalysisRecipe.rows.nCut=m_AnalysisRecipeRows.nCut;
	m_pDoc->m_AnalysisRecipe.rows.nMinkConst=m_AnalysisRecipeRows.nMinkConst;
	m_pDoc->m_AnalysisRecipe.rows.nBeta=m_AnalysisRecipeRows.nBeta;
	m_pDoc->m_AnalysisRecipe.rows.dPower=m_AnalysisRecipeRows.dPower;
	m_pDoc->m_AnalysisRecipe.cols.nAssocMeasure=m_AnalysisRecipeCols.nAssocMeasure;
	m_pDoc->m_AnalysisRecipe.cols.nMinkConst=m_AnalysisRecipeCols.nMinkConst;
	m_pDoc->m_AnalysisRecipe.cols.nNoOfGroups=m_AnalysisRecipeCols.nNoOfGroups;
	m_pDoc->m_AnalysisRecipe.cols.nClassStrategy=m_AnalysisRecipeCols.nClassStrategy;
	m_pDoc->m_AnalysisRecipe.cols.nFuseTechnique=m_AnalysisRecipeCols.nFuseTechnique;
	m_pDoc->m_AnalysisRecipe.cols.nBeta=m_AnalysisRecipeCols.nBeta;

	m_pDoc->m_AnalysisRecipe.rows.bCalculateStats = m_AnalysisRecipeRows.bCalculateStats;
	m_pDoc->m_AnalysisRecipe.rows.bWithTunnels = m_AnalysisRecipeRows.bWithTunnels;
	m_pDoc->m_AnalysisRecipe.rows.nTunnels = m_AnalysisRecipeRows.nTunnels;
	m_pDoc->m_AnalysisRecipe.rows.dMovementFactor = m_AnalysisRecipeRows.dMovementFactor;
	m_pDoc->m_AnalysisRecipe.rows.nOverride = m_AnalysisRecipeRows.nOverride;

	//ShowWindow(SW_HIDE);
	//EnableWindow(false);
CString ir,s;
	if(!((CPatnApp*)AfxGetApp())->silent_mode) UpdateData(TRUE);
	if ( m_bRows == FALSE )
	{
		m_AnalysisRecipeRows.nAssocMeasure = CAssociation::none;
		m_AnalysisRecipeRows.nClassStrategy = CClassification::none;
		m_AnalysisRecipeRows.nOrdination = false;
		m_AnalysisRecipeRows.nOrd3d = 3;
	}
	if ( m_bColumns == FALSE )
	{
		m_AnalysisRecipeCols.nAssocMeasure = CAssociation::none;
		m_AnalysisRecipeCols.nClassStrategy = CClassification::none;
		m_AnalysisRecipeCols.nOrdination = false;
		m_AnalysisRecipeCols.nOrd3d = 3;
	}
	
	evaluations = m_evaluations;
	CAnalysisRecipeDlg * dlg = new CAnalysisRecipeDlg(m_pDoc,&m_AnalysisRecipeRows,&m_AnalysisRecipeCols,NULL);
	dlg->m_TableName = m_pDoc->m_TableName;

	CAssociationDlg *assoc = new CAssociationDlg;
	CClassification classif(m_pDoc);
	classif.m_parent = this->GetSafeHwnd();
	COrdination ord(m_pDoc);
	ord.m_parent = this->GetSafeHwnd();
	if ( (((CPatnApp*)AfxGetApp())->silent_mode) || dlg->DoModal() == IDOK )
	
	{
		m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
		m_pDoc->m_log += "\r\n";

		m_pDoc->m_log += dlg->m_str;
		m_pDoc->m_log += "\r\n\r\n";
						
		//CAssociation assoc(m_pDoc);
		
		assoc->Setup(m_pDoc,true);
		CWaitCursor waitCursor;
		/*if( m_AnalysisRecipeRows.nClassStrategy == 
			CClassification::alloc){
			switch (m_AnalysisRecipeRows.nAllocAssocMeasure){
			case 0://CAssociation::BrayCurtis:
				m_AnalysisRecipeRows.nAssocMeasure = CAssociation::BrayCurtis;
				break;
			case 1://CAssociation::GowerMetric:
				m_AnalysisRecipeRows.nAssocMeasure = CAssociation::GowerMetric;
			break;
			}
		}*/
	
		// Rows - Association
		if(m_AnalysisRecipeRows.nAssocMeasure != CAssociation::none && m_AnalysisRecipeRows.nOverride){
			m_pDoc->m_MCAOExists = false;
			for(i=0;i<m_pDoc->m_Vectors.GetSize();i++)
				if(m_pDoc->m_Vectors[i]->GetSize())
					m_pDoc->m_Vectors[i]->SetAt(0,ANALYSIS_VALUE);
			m_pDoc->m_ANOSIMRowsExists = false;
			if(m_pDoc->m_kw_patn_generated_groups == 0)
			{
				m_pDoc->m_KWExists = false;
				m_pDoc->m_KWValues.SetSize(0);
				m_pDoc->m_KWAttributes.SetSize(0);
				m_pDoc->m_KW.SetSize(0);
			}
			m_pDoc->m_FuseRowChange = 1;
			//reset row fusion because its association changed
			for(i=0;i<m_pDoc->m_FuseTableRows.GetSize();i++)
				delete m_pDoc->m_FuseTableRows[i];
			m_pDoc->m_FuseTableRows.RemoveAll();
			m_pDoc->m_FuseTableRows.SetSize(0);
						m_pDoc->m_OrdChange = 1;
			//reset ordination coords because its association changed
			for(i=0;i<m_pDoc->m_aRowXCoords.GetSize();i++){
				m_pDoc->m_aRowXCoords.SetAt(i,ANALYSIS_VALUE);
				m_pDoc->m_aRowYCoords.SetAt(i,ANALYSIS_VALUE);
				m_pDoc->m_aRowZCoords.SetAt(i,ANALYSIS_VALUE);
			}
			m_pDoc->m_OrdAsso = ""; 
			m_pDoc->m_OrdTime = "";
			m_pDoc->m_OrdValue = 0.0;
		}
		clock_t t2 = clock();		
		CString sst; sst.Format("%.2f",(double)(clock()-t2)/CLOCKS_PER_SEC);
		
		if(m_AnalysisRecipeRows.nOverride)
		{
			switch ( m_AnalysisRecipeRows.nAssocMeasure )
			{
				case CAssociation::none:
					m_pDoc->m_AssocRowUpdate = 0;
					break;

				case CAssociation::BrayCurtis:
					//m_pDoc->m_AssocRowUpdate = assoc.BrayCurtisRows();
					assoc->m_type = CAssociation::BrayCurtis;
					assoc->DoModal();
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					if(!(m_pDoc->m_AssocRowUpdate = assoc->m_value)){						
						m_pDoc->m_log += "- Row Association failed " + sst + " \r\n    " + m_pDoc->m_logerror + "\r\n";
             			errlog += "- Row Association failed " +sst+ " \r\n";
						m_pDoc->m_AssocRowUpdate = 0;
					}else{
						m_pDoc->m_log += "+ Row Association succeeded " +sst+ " \r\n";
					}
					//s = ir; ir.Format("%s -ARA BrayCurtis ",s);

					break;
				case CAssociation::GowerMetric:
					//m_pDoc->m_AssocRowUpdate = assoc->GowerMetricRows();
					assoc->m_type = CAssociation::GowerMetric;
					assoc->DoModal();
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					if(!(m_pDoc->m_AssocRowUpdate = assoc->m_value)){
						m_pDoc->m_log += "- Row Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
						errlog += "- Row Association failed " +sst+ " \r\n";
						m_pDoc->m_AssocRowUpdate = 0;
					}else{
							m_pDoc->m_log += "+ Row Association succeeded " +sst+ " \r\n";
					}
					//s = ir; ir.Format("%s -ARA GowerMetric ",s);
					break;

				case CAssociation::MinkowskiSeries:
					//m_pDoc->m_AssocRowUpdate = assoc.MinkowskiSeriesRows();
					assoc->m_type = CAssociation::MinkowskiSeries;
					assoc->DoModal();
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					if(!(m_pDoc->m_AssocRowUpdate = assoc->m_value)){
						m_pDoc->m_log += "Row Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
						m_pDoc->m_AssocRowUpdate = 0;
					}else{
						m_pDoc->m_log += "+ Row Association succeeded " +sst+ " \r\n";
					}
					//s = ir; ir.Format("%s -ARA Minkowski %g ",s,this->m_AnalysisRecipeRows.nMinkConst);
					break;

				case CAssociation::TwoStep:
					//m_pDoc->m_AssocRowUpdate = assoc.TwoStepRows();
					assoc->m_type = CAssociation::TwoStep;
					assoc->DoModal();
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					if(!(m_pDoc->m_AssocRowUpdate = assoc->m_value)){
						m_pDoc->m_log += "- Row Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
						errlog += "- Row Association failed " +sst+ " \r\n";
						m_pDoc->m_AssocRowUpdate = 0;
					}else{
						m_pDoc->m_log += "+ Row Association succeeded " +sst+ " \r\n";
					}
					//s = ir; ir.Format("%s -ARA TwoStep ",s);
					break;

				case CAssociation::Kulzinski:
					//m_pDoc->m_AssocRowUpdate = assoc.KulzinskiRows();
					assoc->m_type = CAssociation::Kulzinski;
					assoc->DoModal();
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					if(!(m_pDoc->m_AssocRowUpdate = assoc->m_value)){
						m_pDoc->m_log += "- Row Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
						errlog += "- Row Association failed " +sst+ " \r\n";
						m_pDoc->m_AssocRowUpdate = 0;
					}else{
						m_pDoc->m_log += "+ Row Association succeeded " +sst+ " \r\n";
					}
					//s = ir; ir.Format("%s -ARA Kulzinski ",s);
					break;
			}
		}
		m_pDoc->m_logerror = "";

		// Columns - Association
		t2 = clock();
		assoc->Setup(m_pDoc,false);
		if(m_AnalysisRecipeCols.nAssocMeasure != CAssociation::none){
			m_pDoc->m_ANOSIMColsExists = false;

			m_pDoc->m_FuseColChange = 1;
			//reset column fusion because its association changed
			for(i=0;i<m_pDoc->m_FuseTableCols.GetSize();i++)
				delete m_pDoc->m_FuseTableCols[i];
			m_pDoc->m_FuseTableCols.RemoveAll();
			m_pDoc->m_FuseTableCols.SetSize(0);
		}

		switch ( m_AnalysisRecipeCols.nAssocMeasure )
		{
			case CAssociation::none:
				m_pDoc->m_AssocColUpdate = 0;
				break;

			case CAssociation::BrayCurtis:
				//m_pDoc->m_AssocColUpdate = assoc.BrayCurtisColumns();
				assoc->m_type = CAssociation::BrayCurtis;
				assoc->DoModal();
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				if(!(m_pDoc->m_AssocColUpdate = assoc->m_value)){
					m_pDoc->m_log += "- Column Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Association failed " +sst+ " \r\n";
					m_pDoc->m_AssocColUpdate = 0;
				}else{
					m_pDoc->m_log += "+ Column Association succeeded " +sst+ " \r\n";
				}
				//s = ir; ir.Format("%s -ACA BrayCurtis ",s);
				break;

			case CAssociation::GowerMetric:
				//m_pDoc->m_AssocColUpdate = assoc->GowerMetricColumns();
				assoc->m_type = CAssociation::GowerMetric;
				assoc->DoModal();
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				if(!(m_pDoc->m_AssocColUpdate = assoc->m_value)){
					m_pDoc->m_log += "- Column Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Association failed " +sst+ " \r\n";
					m_pDoc->m_AssocColUpdate = 0;
				}else{
					m_pDoc->m_log += "+ Column Association succeeded " +sst+ " \r\n";
				}
				//s = ir; ir.Format("%s -ACA GowerMetric ",s);
				break;

			case CAssociation::MinkowskiSeries:
				//m_pDoc->m_AssocColUpdate = assoc->MinkowskiSeriesColumns();
				assoc->m_type = CAssociation::MinkowskiSeries;
				assoc->DoModal();
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				if(!(m_pDoc->m_AssocColUpdate = assoc->m_value)){
					m_pDoc->m_log += "- Column Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Association failed " +sst+ " \r\n";
					m_pDoc->m_AssocColUpdate = 0;
				}else{
					m_pDoc->m_log += "+ Column Association succeeded " +sst+ " \r\n";
				}
				//s = ir; ir.Format("%s -ACA Minkowski %g ",s,this->m_AnalysisRecipeCols.nMinkConst);
				break;

			case CAssociation::TwoStep:
				//m_pDoc->m_AssocColUpdate = assoc.TwoStepColumns();
				assoc->m_type = CAssociation::TwoStep;
				assoc->DoModal();
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				if(!(m_pDoc->m_AssocColUpdate = assoc->m_value)){
					m_pDoc->m_log += "- Column Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Association failed " +sst+ " \r\n";
					m_pDoc->m_AssocColUpdate = 0;
				}else{
					m_pDoc->m_log += "+ Column Association succeeded " +sst+ " \r\n";
				}
				//s = ir; ir.Format("%s -ACA TwoStep ",s);
				break;
			case CAssociation::Kulzinski:
				//m_pDoc->m_AssocColUpdate = assoc->KulzinskiCols();
				assoc->m_type = CAssociation::Kulzinski;
				assoc->DoModal();
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				if(!(m_pDoc->m_AssocColUpdate = assoc->m_value)){
					m_pDoc->m_log += "- Column Association failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Association failed " +sst+ " \r\n";
					m_pDoc->m_AssocColUpdate = 0;
				}else{
					m_pDoc->m_log += "+ Column Association succeeded " +sst+ " \r\n";
				}
				//s = ir; ir.Format("%s -ACA Kulzinski ",s);
				break;
		}
		m_pDoc->m_logerror = "";

		// Rows - Classification
		t2 = clock();
		switch ( m_AnalysisRecipeRows.nClassStrategy )
		{
			case CClassification::none:
				m_pDoc->m_FuseRowUpdate = 0;
				break;

			case CClassification::fuse:
				m_pDoc->m_ANOSIMRowsExists = false;
				if(!(m_pDoc->m_FuseRowUpdate = classif.Fuse(CClassification::rows,
					m_AnalysisRecipeRows.nFuseTechnique,
					m_AnalysisRecipeRows.nNoOfGroups,
					m_AnalysisRecipeRows.nBeta))){
						sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- Row Classification failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Row Classification failed " +sst+ " \r\n";					
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Row Classification succeeded " +sst+ " \r\n";
				}
				if(m_AnalysisRecipeRows.nFuseTechnique==CFuse::NearestNeighbour){
				//	s = ir; ir.Format("%s -ARC FusionNN %d %g ",s,m_AnalysisRecipeRows.nNoOfGroups,m_AnalysisRecipeRows.nBeta);
				}else if(m_AnalysisRecipeRows.nFuseTechnique==CFuse::FurthestNeighbour){
				//	s = ir; ir.Format("%s -ARC FusionFN %d %g ",s,m_AnalysisRecipeRows.nNoOfGroups,m_AnalysisRecipeRows.nBeta);
				}else if(m_AnalysisRecipeRows.nFuseTechnique==CFuse::FlexibleWPGMA){
				//	s = ir; ir.Format("%s -ARC FusionWPGMA %d %g ",s,m_AnalysisRecipeRows.nNoOfGroups,m_AnalysisRecipeRows.nBeta);
				}else if(m_AnalysisRecipeRows.nFuseTechnique==CFuse::FlexibleUPGMA){
				//	s = ir; ir.Format("%s -ARC FusionUPGMA %d %g ",s,m_AnalysisRecipeRows.nNoOfGroups,m_AnalysisRecipeRows.nBeta);
				}
				break;

			case CClassification::alloc:
				m_pDoc->m_ANOSIMRowsExists = false;
				//reset row assoc & fusion & ordin
//				m_pDoc->m_KWValues.SetSize(0);
//				m_pDoc->m_KW.SetSize(0);
				m_pDoc->m_FuseRowChange = 1;
				for(i=0;i<m_pDoc->m_FuseTableRows.GetSize();i++)
					delete m_pDoc->m_FuseTableRows[i];
				m_pDoc->m_FuseTableRows.RemoveAll();
				m_pDoc->m_FuseTableRows.SetSize(0);

//				m_pDoc->m_OrdChange = 1;
//				for(i=0;i<m_pDoc->m_aRowXCoords.GetSize();i++){
//					m_pDoc->m_aRowXCoords.SetAt(i,ANALYSIS_VALUE);
//					m_pDoc->m_aRowYCoords.SetAt(i,ANALYSIS_VALUE);
//					m_pDoc->m_aRowZCoords.SetAt(i,ANALYSIS_VALUE);
//				}
//				m_pDoc->m_OrdAsso = ""; 
//				m_pDoc->m_OrdTime = "";
//				m_pDoc->m_OrdValue = 0.0;

				m_pDoc->m_FuseRowUpdate = 0;
				if(!(classif.Alloc(m_AnalysisRecipeRows.nAllocAssocMeasure,
					m_AnalysisRecipeRows.nNoOfGroups))){
						sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- Row Classification failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Row Classification failed " +sst+ " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Row Classification succeeded " +sst+ " \r\n";
				}

				if(m_AnalysisRecipeRows.nAllocAssocMeasure==CAssociation::BrayCurtis){
				//	s = ir; ir.Format("%s -ARC AlocBrayCurtis %d ",s,m_AnalysisRecipeRows.nNoOfGroups);
				}else if(m_AnalysisRecipeRows.nAllocAssocMeasure==CAssociation::GowerMetric){
				//	s = ir; ir.Format("%s -ARC AlocGowerMetric %d ",s,m_AnalysisRecipeRows.nNoOfGroups);
				}else if(m_AnalysisRecipeRows.nAllocAssocMeasure==CAssociation::MinkowskiSeries){
				//	s = ir; ir.Format("%s -ARC AlocMinkowski %d %g ",s,m_AnalysisRecipeRows.nNoOfGroups,m_AnalysisRecipeRows.dPower);
				}
				break;
		}
		m_pDoc->m_logerror = "";

		// Columns - Classification
		t2 = clock();
		switch ( m_AnalysisRecipeCols.nClassStrategy )
		{
			case CClassification::none:
				m_pDoc->m_FuseColUpdate = 0;
				break;	

			case CClassification::fuse:
				m_pDoc->m_ANOSIMColsExists = false;
				if(!(m_pDoc->m_FuseColUpdate = classif.Fuse(CClassification::columns,
					m_AnalysisRecipeCols.nFuseTechnique,
				 	m_AnalysisRecipeCols.nNoOfGroups,
				 	m_AnalysisRecipeCols.nBeta))){
						sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- Column Classification failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
					errlog += "- Column Classification failed " +sst+ " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Column Classification succeeded " +sst+ " \r\n";
				}

				if(m_AnalysisRecipeCols.nFuseTechnique==CFuse::NearestNeighbour){
				//	s = ir; ir.Format("%s -ACC FusionNN %d %g ",s,m_AnalysisRecipeCols.nNoOfGroups,m_AnalysisRecipeCols.nBeta);
				}else if(m_AnalysisRecipeCols.nFuseTechnique==CFuse::FurthestNeighbour){
				//	s = ir; ir.Format("%s -ACC FusionFN %d %g ",s,m_AnalysisRecipeCols.nNoOfGroups,m_AnalysisRecipeCols.nBeta);
				}else if(m_AnalysisRecipeCols.nFuseTechnique==CFuse::FlexibleWPGMA){
				//	s = ir; ir.Format("%s -ACC FusionWPGMA %d %g ",s,m_AnalysisRecipeCols.nNoOfGroups,m_AnalysisRecipeCols.nBeta);
				}else if(m_AnalysisRecipeCols.nFuseTechnique==CFuse::FlexibleUPGMA){
				//	s = ir; ir.Format("%s -ACC FusionUPGMA %d %g ",s,m_AnalysisRecipeCols.nNoOfGroups,m_AnalysisRecipeCols.nBeta);
				}
				break;
		}
		m_pDoc->m_logerror = "";

		// Rows - Ordination
		t2 = clock();
		bool ordsuccess = false;
		
		if(m_AnalysisRecipeRows.nOrdination)
		{
			m_pDoc->m_OrdUpdate = 0;
			m_pDoc->m_import_ord = false;

			for(i=0;i<m_pDoc->m_Vectors.GetSize();i++)
				if(m_pDoc->m_Vectors[i]->GetSize())
					m_pDoc->m_Vectors[i]->SetAt(0,ANALYSIS_VALUE);

			m_pDoc->m_MCAOExists = false;			
			m_pDoc->m_KW.SetSize(0);
			m_pDoc->m_KWValues.SetSize(0);
			m_pDoc->m_KWAttributes.SetSize(0);
			m_pDoc->m_Ord3d = m_AnalysisRecipeRows.nOrd3d;
			m_pDoc->m_OrdIterations = m_AnalysisRecipeRows.nOrdIterations;
			m_pDoc->m_OrdSeed = m_AnalysisRecipeRows.nOrdSeed;
			m_pDoc->m_OrdRandomStarts = m_AnalysisRecipeRows.nOrdRandomStarts;

			BeginWaitCursor();
			if((ord.SSHRows(m_AnalysisRecipeRows.nCut,
				m_AnalysisRecipeRows.nOrd3d) == 0)){
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- Row Ordination failed " +sst+ " \r\n    " + m_pDoc->m_logerror + "\r\n";
				errlog += "- Row Ordination failed " +sst+ " \r\n";
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				ordsuccess = true;
				m_pDoc->m_log += "+ Row Ordination succeeded " +sst+ " \r\n";
			}

			//s=ir;ir.Format("%s -ARO SSH %d %d %d %d %g ",s,m_AnalysisRecipeRows.nOrd3d,m_AnalysisRecipeRows.nOrdIterations,
			//	m_AnalysisRecipeRows.nOrdRandomStarts,m_AnalysisRecipeRows.nOrdSeed,m_AnalysisRecipeRows.nCut);
			EndWaitCursor();

			m_pDoc->m_OrdUpdate = 1;
		}

		m_pDoc->m_logerror = "";

		if(m_pDoc->m_FuseColUpdate)
			m_pDoc->m_FuseColChange = 0;
		if(m_pDoc->m_FuseRowUpdate)
			m_pDoc->m_FuseRowChange = 0;
		if(m_pDoc->m_AssocColUpdate)
			m_pDoc->m_AssocColChange = 0;
		if(m_pDoc->m_AssocRowUpdate)
			m_pDoc->m_AssocRowChange = 0;
		if(m_pDoc->m_OrdUpdate)
			m_pDoc->m_OrdChange = 0;		

		m_pDoc->UpdateGroups();

		//after analysis, calculate mst, gsta, colr
		//if(m_AnalysisRecipeRows.nOrdination){
			CColr ccolr(m_pDoc);
			ccolr.OnCreate();

			CMst cmst(m_pDoc);
			cmst.OnCreate();
		//}

		if(m_pDoc->m_Groups.GetSize() > 1)
			CGsta cgsta(m_pDoc);

		//automatic evaluation
		if(m_evaluations == 1){
			CAutoEvaluation ae(m_pDoc);
			ae.Run();
			ae.m_parent = this->GetSafeHwnd();
			errlog += ae.m_errlog;
		}else if(m_evaluations == 2){
			CEvaluation e(m_pDoc);
			e.m_parent = this->GetSafeHwnd();
			e.init();
			e.run(NULL);
		}

		POSITION pos = m_pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = m_pDoc->GetNextView(pos);
			pView->RedrawWindow();
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) UpdateData();

		CString text;
		text.Format("Ordination Stress Value: %.4f",m_pDoc->m_OrdStress);
		if(ordsuccess && !(((CPatnApp*)AfxGetApp())->silent_mode)){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(text,MB_OK, 0);
		}

		if(errlog.GetLength() && !((CPatnApp*)AfxGetApp())->silent_mode){
            	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(errlog,MB_OK,0);
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode)CDialog::OnOK();
	}
	delete assoc;
	delete dlg;

	if(m_pDoc->GetIdxLeftExtrinsicColumn()>0 &&
		m_pDoc->GetNoOfColumns() - m_pDoc->GetIdxLeftExtrinsicColumn()>0)
	{
		//s = ir;ir.Format("%s -XC %d",s,(m_pDoc->GetNoOfColumns() - m_pDoc->GetIdxLeftExtrinsicColumn()));
	}

	if(m_pDoc->GetIdxTopExtrinsicRow()>0 &&
		m_pDoc->GetNoOfRows() - m_pDoc->GetIdxTopExtrinsicRow()>0)
	{
		//s = ir;ir.Format("%s -XR %d",s,(m_pDoc->GetNoOfRows() - m_pDoc->GetIdxTopExtrinsicRow()));
		
	}
	//COMMANND LINE TEXT
	//m_pDoc->m_log += ir;

	//ShowWindow(SW_SHOW);
	//this->EnableWindow(true);
	m_pDoc->UpdateAprioriGroupsColours();
	AfxGetMainWnd()->SetForegroundWindow();
	
}

void CAnalysisDlg::OnSetParmsRows() 
{
	CAssociationPage	pageAssociation;
	CClassRowsPage		pageClassification;
	COrdinationPage		pageOrdination;
	CPropertySheet		ps("Set analysis parameters for rows", this);

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;	// remove 'Apply' button

	// association page
	ps.AddPage(&pageAssociation);
	pageAssociation.m_nAssocMeasure		= m_AnalysisRecipeRows.nAssocMeasure;
	pageAssociation.m_nConstant = m_AnalysisRecipeRows.nMinkConst;
	pageAssociation.m_noverride = (m_AnalysisRecipeRows.nOverride)?true:false;
	pageAssociation.m_SparcityRatio = m_pDoc->GetSparcityRatio();

	long n;
	if ( m_pDoc->IsExtrinsicRows() )			
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();
	if(n > 100)
		pageAssociation.m_LargeNoOfObjs = true;
	else
		pageAssociation.m_LargeNoOfObjs = false;

	// classification page
	ps.AddPage(&pageClassification);
	pageClassification.m_nClassStrategy		= m_AnalysisRecipeRows.nClassStrategy;
	pageClassification.m_nFuseTechnique		= m_AnalysisRecipeRows.nFuseTechnique;
	pageClassification.m_nAllocAssocMeasure	= m_AnalysisRecipeRows.nAllocAssocMeasure;
	pageClassification.m_constant			= m_AnalysisRecipeRows.dPower;
	pageClassification.m_nBeta				= m_AnalysisRecipeRows.nBeta;
	pageClassification.m_nNoOfGroups		= m_AnalysisRecipeRows.nNoOfGroups;
	pageClassification.m_pDoc = m_pDoc;

	// link ALOC value
	pageAssociation.m_nAloc = &pageClassification.m_nAllocAssocMeasure;

	// ordination page
	ps.AddPage(&pageOrdination);
	pageOrdination.m_Ordination = m_AnalysisRecipeRows.nOrdination;
	switch(m_AnalysisRecipeRows.nOrd3d)
	{
	case 1:
		pageOrdination.m_Ord3d = 2;
		break;
	case 2:
		pageOrdination.m_Ord3d = 1;
		break;
	case 3:
		pageOrdination.m_Ord3d = 0;
		break;
	default:
		pageOrdination.m_Ord3d = -1;
	}
	pageOrdination.m_Cut = m_AnalysisRecipeRows.nCut;
	pageOrdination.m_NumberOfIterations = m_AnalysisRecipeRows.nOrdRandomStarts;
	pageOrdination.m_numSeed = m_AnalysisRecipeRows.nOrdSeed;
	pageOrdination.m_numIterations = m_AnalysisRecipeRows.nOrdIterations;
	//pageOrdination.m_bCalculateStats = m_AnalysisRecipeRows.bCalculateStats;
	//pageOrdination.m_bWithTunneling = m_AnalysisRecipeRows.bWithTunnels;
	//pageOrdination.m_numtunnels = m_AnalysisRecipeRows.nTunnels;
	//pageOrdination.m_movementfactor = m_AnalysisRecipeRows.dMovementFactor;
	
	if ( ps.DoModal() != IDOK )
		return;

	// set parameters to new values
	m_AnalysisRecipeRows.nAssocMeasure		= pageAssociation.m_nAssocMeasure;
	m_AnalysisRecipeRows.nClassStrategy		= pageClassification.m_nClassStrategy;
	m_AnalysisRecipeRows.nFuseTechnique		= pageClassification.m_nFuseTechnique;
	m_AnalysisRecipeRows.nAllocAssocMeasure = pageClassification.m_nAllocAssocMeasure;
	m_AnalysisRecipeRows.nBeta				= pageClassification.m_nBeta;
	m_AnalysisRecipeRows.dPower = pageClassification.m_constant;
	m_AnalysisRecipeRows.nNoOfGroups		= pageClassification.m_nNoOfGroups;
	m_AnalysisRecipeRows.nOrdination		= !!pageOrdination.m_Ordination;
    //m_AnalysisRecipeRows.nOrd3d				= pageOrdination.m_Ord3d;
	if(pageOrdination.m_Ord3d == 0)
		m_AnalysisRecipeRows.nOrd3d = 3;
	else if(pageOrdination.m_Ord3d == 1)
		m_AnalysisRecipeRows.nOrd3d = 2;
	else
		m_AnalysisRecipeRows.nOrd3d = 1;
	m_AnalysisRecipeRows.nCut				= pageOrdination.m_Cut;
	m_AnalysisRecipeRows.nMinkConst = pageAssociation.m_nConstant;
	m_pDoc->m_AssocRowsMinkConst = pageAssociation.m_nConstant;
	m_AnalysisRecipeRows.nOrdIterations		= pageOrdination.m_numIterations;
	m_AnalysisRecipeRows.nOrdRandomStarts	= pageOrdination.m_NumberOfIterations;
	m_AnalysisRecipeRows.nOrdSeed			= pageOrdination.m_numSeed;
	m_AnalysisRecipeRows.bCalculateStats = pageOrdination.m_bCalculateStats;
	m_AnalysisRecipeRows.bWithTunnels = pageOrdination.m_bWithTunneling;
	m_AnalysisRecipeRows.dMovementFactor = pageOrdination.m_movementfactor;
	m_AnalysisRecipeRows.nTunnels = pageOrdination.m_numtunnels;
	m_AnalysisRecipeRows.nOverride = pageAssociation.m_noverride;
	

}

void CAnalysisDlg::OnSetParmsColumns() 
{
	CAssociationPage	pageAssociation;
	CClassColsPage		pageClassification;
	COrdinationPage		pageOrdination;
	CPropertySheet		ps("Set analysis parameters for columns", this);

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;	// remove 'Apply' button

	// association page
	ps.AddPage(&pageAssociation);
	pageAssociation.m_nAssocMeasure	   = m_AnalysisRecipeCols.nAssocMeasure;
	pageAssociation.m_nConstant = m_AnalysisRecipeCols.nMinkConst;
	pageAssociation.m_LargeNoOfObjs = false;
	pageAssociation.m_noverride = true;

	// classification page
	ps.AddPage(&pageClassification);
	pageClassification.m_nClassStrategy = m_AnalysisRecipeCols.nClassStrategy;
	pageClassification.m_nFuseTechnique = m_AnalysisRecipeCols.nFuseTechnique;
	pageClassification.m_nBeta			= m_AnalysisRecipeCols.nBeta;
	pageClassification.m_nNoOfGroups	= m_AnalysisRecipeCols.nNoOfGroups;
	pageClassification.m_pDoc = m_pDoc;

	// link ALOC value, or, since there is none, fix it;
	int Aloc = -1;
	pageAssociation.m_nAloc = &Aloc;

	if ( ps.DoModal() != IDOK )
		return;

	// set parameters to new values
	m_AnalysisRecipeCols.nAssocMeasure  = pageAssociation.m_nAssocMeasure;
	m_AnalysisRecipeCols.nClassStrategy = pageClassification.m_nClassStrategy;
	m_AnalysisRecipeCols.nFuseTechnique = pageClassification.m_nFuseTechnique;
	m_AnalysisRecipeCols.nBeta			= pageClassification.m_nBeta;
	m_AnalysisRecipeCols.nNoOfGroups    = pageClassification.m_nNoOfGroups;
	m_AnalysisRecipeCols.nMinkConst = pageAssociation.m_nConstant;
	m_pDoc->m_AssocColsMinkConst = pageAssociation.m_nConstant;
}

void CAnalysisDlg::OnColumnsNo() 
{
	m_btnSetParmsColumns.EnableWindow(FALSE);
}

void CAnalysisDlg::OnColumnsYes() 
{
	m_btnSetParmsColumns.EnableWindow(TRUE);
}

void CAnalysisDlg::OnRowsNo() 
{
	m_btnSetParmsRows.EnableWindow(FALSE);
}

void CAnalysisDlg::OnRowsYes() 
{
	m_btnSetParmsRows.EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// CAnalysisRecipeDlg dialog


CAnalysisRecipeDlg::CAnalysisRecipeDlg(CPatnDoc * doc,CAnalysisRecipe* pAnalysisRecipeRows,
                                       CAnalysisRecipe* pAnalysisRecipeCols,
                                       CWnd* pParent /*=NULL*/)
	: CDialog(CAnalysisRecipeDlg::IDD, pParent)
{
	m_created = false;
	m_pDoc = doc;
	m_pAnalysisRecipeRows =	pAnalysisRecipeRows;
	m_pAnalysisRecipeCols =	pAnalysisRecipeCols;
	//{{AFX_DATA_INIT(CAnalysisRecipeDlg)
	m_str = _T("");
	//}}AFX_DATA_INIT
}


void CAnalysisRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalysisRecipeDlg)
	DDX_Control(pDX, IDC_SAVERECIPE, m_Box3);
	DDX_Control(pDX, IDCANCEL, m_Box1);
	DDX_Control(pDX, IDOK, m_Box2);
	DDX_Control(pDX, IDC_ANALYSIS_LAYOUT, m_Box);
	DDX_Text(pDX, IDC_ANALYSIS_LAYOUT, m_str);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalysisRecipeDlg, CDialog)
	//{{AFX_MSG_MAP(CAnalysisRecipeDlg)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAnalysisRecipeDlg message handlers

BOOL CAnalysisRecipeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_created = true;

	CString str1, str2, str3, str4, str5;  // temp storage
	char buffer[20];
	CString tmp;

	CString str_for_input;
	str_for_input = "";

	// Rows ....
	// .... Association
	m_str = "Analysis based on rows -\r\n";
	m_str += "    Association Measure: ";
	switch (m_pAnalysisRecipeRows->nAssocMeasure)
	{
		case CAssociation::none:
			str1 = "None";
			break;
		case CAssociation::BrayCurtis:
			str1.LoadString(ASSOC_BRAY_CURTIS);
			str_for_input += "-ARA BrayCurtis ";
			break;
		case CAssociation::GowerMetric:
			str1.LoadString(ASSOC_GOWER_METRIC);
			str_for_input += "-ARA GowerMetric ";
			break;
		case CAssociation::MinkowskiSeries:
			str1.LoadString(ASSOC_MINKOWSKI_SERIES);
			str1 += "\t";
			str1 += "Constant = ";
			tmp.Format("%.3f",m_pAnalysisRecipeRows->nMinkConst);
			str1 += tmp;
			sprintf(buffer,"%g ",m_pAnalysisRecipeRows->nMinkConst);
			str_for_input += "-ARA Minkowski " ;
					str_for_input +=  buffer;
			break;
		case CAssociation::TwoStep:
			str1.LoadString(ASSOC_TWO_STEP);
			str_for_input += "-ARA TwoStep ";
			break;
		case CAssociation::Kulzinski:
			str1.LoadString(ASSOC_KULZINSKI);
			str_for_input += "-ARA Kulzinski ";
			break;
	};
	if(m_pAnalysisRecipeRows->nClassStrategy == CClassification::alloc){
		switch (m_pAnalysisRecipeRows->nAllocAssocMeasure){
		case 0://CAssociation::BrayCurtis:
			//str1.LoadString(ASSOC_BRAY_CURTIS);
			break;
		case 1://CAssociation::GowerMetric:
			//str1.LoadString(ASSOC_GOWER_METRIC);
		break;
		}
	}
	str1 += "\r\n";
	m_str += str1;

	// .... Classification
	m_str += "    Classification Strategy: ";
	str2 = "        ";
 	sprintf(buffer,"%d",m_pAnalysisRecipeRows->nNoOfGroups);
	str3 = "        Number of groups to produce: ";
	str3 += buffer;
 	str3 += "\r\n";
 	sprintf(buffer,"%0.4f",m_pAnalysisRecipeRows->nBeta);
	str5 = "        Beta: ";
	str5 += buffer;
 	str5 += "\r\n";
	switch (m_pAnalysisRecipeRows->nClassStrategy)
	{
		case CClassification::none:
			str1 = "None";
			str2.Empty();
			str3.Empty();
			str4.Empty();
			str5.Empty();
			break;
		case CClassification::fuse:
			str1.LoadString(CLASS_FUSE);
			str2 += "Technique: ";
			switch (m_pAnalysisRecipeRows->nFuseTechnique)
			{
				case CFuse::NearestNeighbour:
					str4.LoadString(FUSE_NEAREST_NEIGHBOUR);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeRows->nNoOfGroups,m_pAnalysisRecipeRows->nBeta);
					str_for_input += "-ARC FusionNN ";
					str_for_input += buffer;
					break;
				case CFuse::FurthestNeighbour:
					str4.LoadString(FUSE_FURTHEST_NEIGHBOUR);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeRows->nNoOfGroups,m_pAnalysisRecipeRows->nBeta);
					str_for_input += "-ARC FusionFN " ;
					str_for_input +=  buffer;
					break;
				case CFuse::FlexibleWPGMA:
					str4.LoadString(FUSE_FLEXIBLE_WPGMA);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeRows->nNoOfGroups,m_pAnalysisRecipeRows->nBeta);
					str_for_input += "-ARC FusionWPGMA " ;
					str_for_input +=  buffer;
					break;
				case CFuse::FlexibleUPGMA:
					str4.LoadString(FUSE_FLEXIBLE_UPGMA);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeRows->nNoOfGroups,m_pAnalysisRecipeRows->nBeta);
					str_for_input += "-ARC FusionUPGMA " ;
					str_for_input +=  buffer;
					break;
			}
			str2 += str4;
			str2 += "\r\n";
			break;
		case CClassification::alloc:
			str1.LoadString(CLASS_ALLOC);
			if(m_pAnalysisRecipeRows->nAllocAssocMeasure == 3)
			{
				CString s;
				s.Format("%g",m_pAnalysisRecipeRows->dPower);
                str2 += "Power: " + s + "\r\n"+str4;
				
			}
			str2 += "Association measure: ";
			switch (m_pAnalysisRecipeRows->nAllocAssocMeasure)
			{
				case CAssociation::BrayCurtis:
					str4.LoadString(ASSOC_BRAY_CURTIS);
					sprintf(buffer,"%d ",m_pAnalysisRecipeRows->nNoOfGroups);
					str_for_input += "-ARC AlocBrayCurtis ";
					str_for_input +=buffer;
					break;
				case CAssociation::GowerMetric:
					str4.LoadString(ASSOC_GOWER_METRIC);
					sprintf(buffer,"%d ",m_pAnalysisRecipeRows->nNoOfGroups);
					str_for_input += "-ARC AlocGowerMetric ";
					str_for_input += buffer;
					break;
				case 3:
					str4 = "Minkowski";
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeRows->nNoOfGroups,m_pAnalysisRecipeRows->dPower);
					str_for_input += "-ARC Minkowski ";
					str_for_input += buffer;
					break;
			}
			str5.Empty();
			str2 += str4;
			str2 += "\r\n";
			break;
	}
	str1 += "\r\n";
	m_str += str1;
	m_str += str2;
	m_str += str5;
	m_str += str3;

	m_str += "    Ordination Method: ";
	if(m_pAnalysisRecipeRows->nOrdination)
	{
		str1.Format("%.3f",m_pAnalysisRecipeRows->nCut);
		if(m_pAnalysisRecipeRows->nOrd3d == 3)
			str2 = "\r\n        3 Dimensions";
		else if(m_pAnalysisRecipeRows->nOrd3d == 2)
			str2 = "\r\n        2 Dimensions";
		else
			str2 = "\r\n        1 Dimensions";
		str3.Format("\r\n        Number of random starts: %d\r\n        Max iterations: %d\r\n        Random Seed Value: %d",m_pAnalysisRecipeRows->nOrdRandomStarts,m_pAnalysisRecipeRows->nOrdIterations,m_pAnalysisRecipeRows->nOrdSeed);

		m_str += "SSH\r\n        CutOff = ";
		m_str += str1;
		m_str += str2;
		m_str += str3;
		sprintf(buffer,"%d %d %d %d %g ",m_pAnalysisRecipeRows->nOrd3d,m_pAnalysisRecipeRows->nOrdIterations,
			m_pAnalysisRecipeRows->nOrdRandomStarts,m_pAnalysisRecipeRows->nOrdSeed,m_pAnalysisRecipeRows->nCut);
		str_for_input += "-ARO SSH " ;
					str_for_input +=  buffer;
	}else{
		m_str += "None";
	}

	// blank line
	m_str += "\r\n";
	m_str += "\r\n";


	// Columns ....
	// .... Association
	m_str += "Analysis based on columns -\r\n";
	m_str += "    Association Measure: ";
	switch (m_pAnalysisRecipeCols->nAssocMeasure)
	{
		case CAssociation::none:
			str1 = "None";
			break;
		case CAssociation::BrayCurtis:
			str1.LoadString(ASSOC_BRAY_CURTIS);
			str_for_input += "-ACA BrayCurtis ";
			break;
		case CAssociation::GowerMetric:
			str1.LoadString(ASSOC_GOWER_METRIC);
			str_for_input += "-ACA GowerMetric ";
			break;
		case CAssociation::MinkowskiSeries:
			str1.LoadString(ASSOC_MINKOWSKI_SERIES);
			str1 += "\t";
			str1 += "Constant = ";
			tmp.Format("%.3f",m_pAnalysisRecipeCols->nMinkConst);
			str1 += tmp;
			sprintf(buffer,"%g ",m_pAnalysisRecipeRows->nMinkConst);
			str_for_input += "-ACA Minkowski " ;
					str_for_input +=  buffer;
			break;
		case CAssociation::TwoStep:
			str1.LoadString(ASSOC_TWO_STEP);
			str_for_input += "-ACA TwoStep ";
			break;
		case CAssociation::Kulzinski:
			str1.LoadString(ASSOC_KULZINSKI);
			str_for_input += "-ACA Kulzinski ";
			break;
	};
	str1 += "\r\n";
	m_str += str1;

	// .... Classification
	m_str += "    Classification Strategy: ";
	str2 = "        ";
 	sprintf(buffer,"%d",m_pAnalysisRecipeCols->nNoOfGroups);
	str3 = "        Number of groups to produce: ";
	str3 += buffer;
 	str3 += "\r\n";
 	sprintf(buffer,"%0.4f",m_pAnalysisRecipeCols->nBeta);
	str5 = "        Beta: ";
	str5 += buffer;
 	str5 += "\r\n";
	switch (m_pAnalysisRecipeCols->nClassStrategy)
	{
		case CClassification::none:
			str1 = "None";
			str2.Empty();
			str3.Empty();
			str4.Empty();
			str5.Empty();
			break;
		case CClassification::fuse:
			str1.LoadString(CLASS_FUSE);
			str2 += "Technique: ";
			switch (m_pAnalysisRecipeCols->nFuseTechnique)
			{
				case CFuse::NearestNeighbour:
					str4.LoadString(FUSE_NEAREST_NEIGHBOUR);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeCols->nNoOfGroups,m_pAnalysisRecipeCols->nBeta);
					str_for_input += "-ACC FusionNN " ;
					str_for_input +=  buffer;
					break;
				case CFuse::FurthestNeighbour:
					str4.LoadString(FUSE_FURTHEST_NEIGHBOUR);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeCols->nNoOfGroups,m_pAnalysisRecipeCols->nBeta);
					str_for_input += "-ACC FusionFN " ;
					str_for_input +=  buffer;
					break;
				case CFuse::FlexibleWPGMA:
					str4.LoadString(FUSE_FLEXIBLE_WPGMA);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeCols->nNoOfGroups,m_pAnalysisRecipeCols->nBeta);
					str_for_input += "-ACC FusionWPGMA " ;
					str_for_input +=  buffer;
					break;
				case CFuse::FlexibleUPGMA:
					str4.LoadString(FUSE_FLEXIBLE_UPGMA);
					sprintf(buffer,"%d %g ",m_pAnalysisRecipeCols->nNoOfGroups,m_pAnalysisRecipeCols->nBeta);
					str_for_input += "-ACC FusionUPGMA " ;
					str_for_input +=  buffer;
					break;
			}
			str2 += str4;
			str2 += "\r\n";
			break;
		case CClassification::alloc:
			str1.LoadString(CLASS_ALLOC);
			str2 += "Association measure: ";
			switch (m_pAnalysisRecipeCols->nAllocAssocMeasure)
			{
				case CAssociation::BrayCurtis:
					str4.LoadString(ASSOC_BRAY_CURTIS);
					sprintf(buffer,"%g ",m_pAnalysisRecipeRows->nNoOfGroups);
					str_for_input += "-ACC AlocBrayCurtis " ;
					str_for_input +=  buffer;
					break;
				case CAssociation::GowerMetric:
					str4.LoadString(ASSOC_GOWER_METRIC);
					sprintf(buffer,"%g ",m_pAnalysisRecipeRows->nNoOfGroups);
					str_for_input += "-ACC AlocGowerMetric " ;
					str_for_input +=  buffer;
					break;
			}
			str5.Empty();
			str2 += str4;
			str2 += "\r\n";
			break;
	}
 	str1 += "\r\n";
	m_str += str1;
	m_str += str2;
	m_str += str5;
	m_str += str3;

	CAutoEvaluation dlg(m_pDoc);
	dlg.m_parent = this->GetSafeHwnd();
	if(evaluations == 1){
		m_str += dlg.GetRecipeString();
	}else if(evaluations == 2){
		CEvaluation e(m_pDoc);
		e.m_parent = this->GetSafeHwnd();
		e.init();
		CEvaluationRecipe er(m_pDoc,&e.m_params);
		m_str += er.m_Recipe ;
	}
	m_str+= "\r\n";// + str_for_input;
	//get distance from bottom of box
	CRect rect,rect2;
	GetClientRect(rect);
	m_Box.GetClientRect(rect2);
	m_dist = rect.bottom;
	m_dist2 = rect.right;

	UpdateData(FALSE); // display string in edit box
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void COrdinationPage::OnRandom() 
{	
	CButton * d2button = (CButton*)GetDlgItem(IDC_2D);
	CButton * d3button = (CButton*)GetDlgItem(IDC_3D);
	CButton * d1button = (CButton*)GetDlgItem(IDC_1D);
	

	UpdateData(TRUE);
	if(m_Ordination){
		m_Cut = 0.9;
		UpdateData(TRUE);
		m_EditCut.EnableWindow(TRUE);
		m_Iterations.EnableWindow(TRUE);
		m_CtrlIterations.EnableWindow(TRUE);
		m_CtrlSeed.EnableWindow(TRUE);
		d1button->EnableWindow(TRUE);
		d2button->EnableWindow(TRUE);
		d3button->EnableWindow(TRUE);

	}else{
		m_EditCut.EnableWindow(FALSE);
		m_Iterations.EnableWindow(FALSE);
		m_CtrlIterations.EnableWindow(FALSE);
		m_CtrlSeed.EnableWindow(FALSE);
		d1button->EnableWindow(FALSE);
		d2button->EnableWindow(FALSE);
		d3button->EnableWindow(FALSE);
	}
	
}

BOOL COrdinationPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CButton * d2button = (CButton*)GetDlgItem(IDC_2D);
	CButton * d3button = (CButton*)GetDlgItem(IDC_3D);
	CButton * d1button = (CButton*)GetDlgItem(IDC_1D);

	d1button->EnableWindow(FALSE);
	d2button->EnableWindow(FALSE);
	d3button->EnableWindow(FALSE);
	
	//if(m_withtunneling.GetCheck()){
	//	m_Movement.EnableWindow(true);
	//	m_NumTunnels.EnableWindow(true);
	//	m_tunnel3.EnableWindow(true);
	//}else{
	//	m_Movement.EnableWindow(false);
	//	m_NumTunnels.EnableWindow(false);
	//	m_tunnel3.EnableWindow(false);
	//}	

	OnRandom();
	
	return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void CAssociationPage::OnChangeConstant() 
{
	CString text;
	m_Constant.GetWindowText(text);
	if(m_nAssocMeasure == CAssociation::MinkowskiSeries){
		m_nConstant = atof(text);
		if(m_nConstant == 0){
			//get correct number
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Try something other than zero.",
				MB_OK | MB_ICONSTOP, 0);
			m_Constant.SetWindowText("2");
			m_nConstant = 2;
		}
	}	
}

void CAssociationPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	//if(*m_nAloc == -1){
		//set selectionbox active
		//m_listAssocMeasures.EnableWindow();
	//}else if(*m_nAloc == 0){
		//set selectionbox inactive & bray&curtis selected
		//m_listAssocMeasures.SetCurSel(0);
		//m_listAssocMeasures.EnableWindow(FALSE);
	//}else{
		//set selectionbox inactive & gowermetric selected
		//m_listAssocMeasures.SetCurSel(0);
		//m_listAssocMeasures.EnableWindow(FALSE);
	//}	
}

void CAnalysisRecipeDlg::OnSaverecipe() 
{
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Save Analysis Recipe:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"txt","*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"text (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();

delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Recipe.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	//now write string!
	CTime time = CTime::GetCurrentTime();

	f.WriteString("Recipe of analysis to be performed on  ");
	f.WriteString(m_TableName);
	f.WriteString("  at  ");
	f.WriteString(time.Format("%H:%M:%S, %B %d, %Y"));
	
	f.WriteString("\n\n\n");

	f.WriteString(m_str);


	

	f.Close();		
}

void CAnalysisRecipeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(!m_created)
		return;

	CRect rect,rect2,rect3,rect4,rect5;
	GetWindowRect(rect);
	
	m_Box.GetWindowRect(rect2);
	m_Box1.GetWindowRect(rect3);
	m_Box2.GetWindowRect(rect4);
	m_Box3.GetWindowRect(rect5);

	rect2.bottom = rect.bottom - 40;
	rect3.top = rect.bottom - 32;
	rect3.bottom = rect.bottom - 10;
	rect4.top = rect.bottom - 32;
	rect4.bottom = rect.bottom - 10;
	rect5.top = rect.bottom - 32;
	rect5.bottom = rect.bottom - 10;

	rect2.right = rect.right - 15;
	
	m_dist = rect.bottom;
	m_dist2 = rect.right;

	ScreenToClient(rect2);
	m_Box.MoveWindow(rect2);

	ScreenToClient(rect3);
	m_Box1.MoveWindow(rect3);
	ScreenToClient(rect4);
	m_Box2.MoveWindow(rect4);
	ScreenToClient(rect5);
	m_Box3.MoveWindow(rect5);	
}

void COrdinationPage::OnCheck2() 
{
	//if(m_withtunneling.GetCheck()){
		//m_Movement.EnableWindow(true);
		//m_NumTunnels.EnableWindow(true);
		//m_tunnel3.EnableWindow(true);
//	}else{
//		m_Movement.EnableWindow(false);
//		m_NumTunnels.EnableWindow(false);
//		m_tunnel3.EnableWindow(false);
//	}	
}

BOOL COrdinationPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_CHAR){
		if(((int) pMsg->wParam) == 1){//ctrl+a
		//	m_tunnel1.ShowWindow(SW_SHOW);
		//	m_tunnel2.ShowWindow(SW_SHOW);
		//	m_tunnel3.ShowWindow(SW_SHOW);
		//	m_NumTunnels.ShowWindow(SW_SHOW);
		//	m_Movement.ShowWindow(SW_SHOW);
		//	m_withtunneling.ShowWindow(SW_SHOW);
		}
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CAssociationPage::OnBnClickedAllowassoc()
{
	m_noverride = !m_noverride;
	if(m_noverride)
	{
		//enable listbox
		this->m_listAssocMeasures.EnableWindow(true);
		
		
		//if ( m_SparcityRatio < 50 ){	// less than 40% or rows
		//	m_nAssocMeasure = CAssociation::GowerMetric;			
		//}else{
		//	m_nAssocMeasure = CAssociation::TwoStep;			
		//}
	}
	else
	{
		this->m_listAssocMeasures.EnableWindow(false);

		m_nAssocMeasure = CAssociation::none;
	}
}
