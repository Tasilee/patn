#if !defined(AFX_PCCVECTORSELECTION_H__F8142F22_9641_11D4_A3D5_525405F5DADA__INCLUDED_)
#define AFX_PCCVECTORSELECTION_H__F8142F22_9641_11D4_A3D5_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PccVectorSelection.h : header file
//
#include "patndoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPccVectorSelection dialog

class CPccVectorSelection : public CDialog
{
// Construction
public:
	CPccVectorSelection(CWnd* pParent = NULL);   // standard constructor
	
	CPatnDoc *m_pDoc;
	CArray<long> * m_PccVectors;

// Dialog Data
	//{{AFX_DATA(CPccVectorSelection)
	enum { IDD = IDD_PCCVECTORSELECTION };
	CListBox	m_Labels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPccVectorSelection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPccVectorSelection)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectnone();
	afx_msg void OnSelectall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCCVECTORSELECTION_H__F8142F22_9641_11D4_A3D5_525405F5DADA__INCLUDED_)
