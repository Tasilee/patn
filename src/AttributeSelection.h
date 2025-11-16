#if !defined(AFX_ATTRIBUTESELECTION_H__F8142F21_9641_11D4_A3D5_525405F5DADA__INCLUDED_)
#define AFX_ATTRIBUTESELECTION_H__F8142F21_9641_11D4_A3D5_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttributeSelection.h : header file
//

#include "patndoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAttributeSelection dialog

class CAttributeSelection : public CDialog
{
// Construction
public:
	CAttributeSelection(CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;
	long m_attrib;
// Dialog Data
	//{{AFX_DATA(CAttributeSelection)
	enum { IDD = IDD_ATTRIBUTE };
	CListBox	m_Labels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttributeSelection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttributeSelection)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLabels();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBUTESELECTION_H__F8142F21_9641_11D4_A3D5_525405F5DADA__INCLUDED_)
