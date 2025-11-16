#if !defined(AFX_EVALUATIONRECIPE_H__18CD6034_1495_45BB_823F_EECEDDAB1A8F__INCLUDED_)
#define AFX_EVALUATIONRECIPE_H__18CD6034_1495_45BB_823F_EECEDDAB1A8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EvaluationRecipe.h : header file
//

typedef struct EvaluationRecipe{
	//ANOSIM - rows
	bool anosim_rows;
	long anosim_rows_seed;
	long anosim_rows_iterations;
	bool anosim_rows_pairs;
	CArray<long> anosim_rows_groups;
	long anosim_patn_generated_groups;
	//ANOSIM - cols
	bool anosim_cols;
	long anosim_cols_seed;
	long anosim_cols_iterations;
	bool anosim_cols_pairs;
	CArray<long> anosim_cols_groups;
	//Kruskal-Wallis
	bool kw;
	// actively stored in patndoc
	//PCC
	bool pcc;
	CArray<long> pcc_indexes;
	//MCAO	
	bool mcao;
	long mcao_seed;
	long mcao_iterations;
	long kw_patn_generated_groups;
}EvaluationRecipe;


/////////////////////////////////////////////////////////////////////////////
// CEvaluationRecipe dialog

class CEvaluationRecipe : public CDialog
{
// Construction
public:
	CEvaluationRecipe(CPatnDoc * doc,EvaluationRecipe * recipe,CWnd* pParent = NULL);   // standard constructor

public:
	CPatnDoc * m_pDoc;

	long m_dist,m_dist2,m_created;

// Dialog Data
	//{{AFX_DATA(CEvaluationRecipe)
	enum { IDD = IDD_EVALUATIONRECIPE };
	CButton	m_Box3;
	CButton	m_Box2;
	CButton	m_Box1;
	CEdit	m_Box;
	CString	m_Recipe;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvaluationRecipe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEvaluationRecipe)
	afx_msg void OnSaverecipe();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVALUATIONRECIPE_H__18CD6034_1495_45BB_823F_EECEDDAB1A8F__INCLUDED_)
