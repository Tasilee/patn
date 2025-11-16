// analysis.h : header file
//

#pragma once
/////////////////////////////////////////////////////////////////////////////
// CAssociationPage dialog

// TODO: Make sure that only one instance of this modeless dialog box can exist
// at any one time.

#include "afxwin.h"
class CAssociationPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAssociationPage)

// Construction
public:
	
	CAssociationPage();
	~CAssociationPage();
	double m_SparcityRatio;
// Dialog Data
	//{{AFX_DATA(CAssociationPage)
	enum { IDD = IDD_ASSOCIATION_PAGE };
	CEdit	m_Constant;
	CListBox	m_listAssocMeasures;
	int		m_nAssocMeasure;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAssociationPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	double m_nConstant;
	int m_LargeNoOfObjs;
	int * m_nAloc; //point to alloc status, -1=none, 0=bray&curtis, 1=gowermetric

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAssociationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAssocMeasures();
	afx_msg void OnChangeConstant();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_override_defaults;
	bool m_noverride;
	afx_msg void OnBnClickedAllowassoc();
	CStatic m_info;
	CStatic m_info3;
	CStatic m_info2;
};

/////////////////////////////////////////////////////////////////////////////
// CClassRowsPage dialog

class CClassRowsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CClassRowsPage)

// Construction
public:
	CClassRowsPage();
	~CClassRowsPage();
	CPatnDoc* m_pDoc;

// Dialog Data
	//{{AFX_DATA(CClassRowsPage)
	enum { IDD = IDD_CLASSIFICATION_PAGE_ROWS };
	CEdit	m_editNoOfGroups;
	CEdit	m_editBeta;
	CComboBox	m_comboFuseTechnique;
	CListBox	m_listAlocAssocMeasure;
	int		m_nNoOfGroups;
	int		m_nClassStrategy;
	double	m_nBeta;
	//}}AFX_DATA
	int m_nAllocAssocMeasure;
	int m_nFuseTechnique;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CClassRowsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CClassRowsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFuseTechnique();
	afx_msg void OnSelchangeAlocAssocMeasure();
	afx_msg void OnNone();
	afx_msg void OnAloc();
	afx_msg void OnFuse();
	afx_msg void OnUpdateNumGroups();
	afx_msg void OnKillfocusBeta();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	double m_constant;
	CEdit m_constant_box;
};

/////////////////////////////////////////////////////////////////////////////
// CClassColsPage dialog

class CClassColsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CClassColsPage)

// Construction
public:
	CClassColsPage();
	~CClassColsPage();
	CPatnDoc* m_pDoc;

// Dialog Data
	//{{AFX_DATA(CClassColsPage)
	enum { IDD = IDD_CLASSIFICATION_PAGE_COLS };
	CComboBox	m_comboFuseTechnique;
	CEdit	m_editNoOfGroups;
	CEdit	m_editBeta;
	double	m_nBeta;
	int		m_nNoOfGroups;
	int		m_nClassStrategy;
	//}}AFX_DATA
	int m_nFuseTechnique;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassColsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClassColsPage)
	afx_msg void OnNone();
	afx_msg void OnFuse();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFuseTechnique();
	afx_msg void OnUpdateNumGroups();
	afx_msg void OnKillfocusBeta();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COrdinationPage dialog

class COrdinationPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COrdinationPage)

public:
	int m_ordination;
	int m_ord3d;

// Construction
public:
	COrdinationPage();
	~COrdinationPage();

// Dialog Data
	//{{AFX_DATA(COrdinationPage)
	enum { IDD = IDD_ORDINATION_PAGE };
	CButton	m_tunnel3;
	CStatic	m_tunnel2;
	CStatic	m_tunnel1;
	CEdit	m_NumTunnels;
	CEdit	m_Movement;
	CButton	m_withtunneling;
	CEdit	m_CtrlSeed;
	CEdit	m_CtrlIterations;
	CEdit	m_Iterations;
	CButton	m_OrdinationCheck;
	CEdit	m_EditCut;
	double	m_Cut;
	int		m_Ord3d;
	BOOL	m_Ordination;
	int		m_NumberOfIterations;
	int		m_numIterations;
	int		m_numSeed;
	BOOL	m_bWithTunneling;
	BOOL	m_bCalculateStats;
	double	m_movementfactor;
	int		m_numtunnels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COrdinationPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COrdinationPage)
	afx_msg void OnRandom();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CAnalysisDlg dialog

class CAnalysisDlg : public CDialog
{
// Construction
public:
	CAnalysisDlg(CPatnDoc* pDoc, CWnd* pParent = NULL);

protected:
	CPatnDoc* m_pDoc;

public:
	CAnalysisRecipe m_AnalysisRecipeRows, m_AnalysisRecipeCols;
void Initialize();
// Dialog Data
	//{{AFX_DATA(CAnalysisDlg)
	enum { IDD = IDD_ANALYSIS };
	CButton	m_btnSetParmsRows;
	CButton	m_btnSetParmsColumns;
	int		m_bColumns;
	int		m_bRows;
	BOOL	m_evaluations;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalysisDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CAnalysisDlg)
	afx_msg void OnSetParmsRows();
	afx_msg void OnSetParmsColumns();
	afx_msg void OnColumnsNo();
	afx_msg void OnColumnsYes();
	afx_msg void OnRowsNo();
	afx_msg void OnRowsYes();
	virtual BOOL OnInitDialog();
	afx_msg void OnRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
};
/////////////////////////////////////////////////////////////////////////////
// CAnalysisRecipeDlg dialog

class CAnalysisRecipeDlg : public CDialog
{
// Construction
public:
	CAnalysisRecipeDlg(CPatnDoc * pDoc,CAnalysisRecipe* pAnalysisRecipeRows,
                       CAnalysisRecipe* pAnalysisRecipeCols,
                       CWnd* pParent = NULL);

	int m_dist,m_dist2;
	int m_created;
	CString m_TableName;
	CPatnDoc * m_pDoc;

public:
	CAnalysisRecipe *m_pAnalysisRecipeRows, *m_pAnalysisRecipeCols;

// Dialog Data
	//{{AFX_DATA(CAnalysisRecipeDlg)
	enum { IDD = IDD_ANALYSIS_RECIPE };
	CButton	m_Box3;
	CButton	m_Box1;
	CButton	m_Box2;
	CEdit	m_Box;
	CString	m_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalysisRecipeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalysisRecipeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaverecipe();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
