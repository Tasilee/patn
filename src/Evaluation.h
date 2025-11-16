#if !defined(AFX_EVALUATION_H__865880E4_90EA_456E_96F5_DAF0D4595B71__INCLUDED_)
#define AFX_EVALUATION_H__865880E4_90EA_456E_96F5_DAF0D4595B71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Evaluation.h : header file
//

#include "patndoc.h"
#include "evaluationrecipe.h"

/////////////////////////////////////////////////////////////////////////////
// CEvaluation dialog

class CEvaluation : public CDialog
{
// Construction
public:
	CEvaluation(CPatnDoc * doc,CWnd* pParent = NULL);   // standard constructor

	void run(CEvaluationRecipe * dlg);
	void init();
	EvaluationRecipe m_params;

	HWND m_parent;

protected:
	CPatnDoc * m_pDoc;
	

// Dialog Data
	//{{AFX_DATA(CEvaluation)
	enum { IDD = IDD_EVALUATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvaluation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEvaluation)
	afx_msg void OnSetparms();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVALUATION_H__865880E4_90EA_456E_96F5_DAF0D4595B71__INCLUDED_)
