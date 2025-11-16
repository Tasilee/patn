#if !defined(AFX_KRUSKALWALLIS_H__8D608F53_8709_4CC1_96C9_A0877F2439D3__INCLUDED_)
#define AFX_KRUSKALWALLIS_H__8D608F53_8709_4CC1_96C9_A0877F2439D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KruskalWallis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKruskalWallis dialog

#include "patndoc.h"


class CKruskalWallis : public CPropertyPage
{
// Construction
public:
	CKruskalWallis(CPatnDoc * pDoc,CWnd* pParent = NULL);   // standard constructor

	CPatnDoc* m_pDoc;
	CArray<long> m_KWAttributes;
// Dialog Data
	//{{AFX_DATA(CKruskalWallis)
	enum { IDD = IDD_KW_PAGE };
	CButton	m_AddAll;
	CButton	m_Add;
	CButton	m_RemoveAll;
	CButton	m_Remove;
	CListBox	m_Selected;
	CListBox	m_All;
	BOOL	m_kw;
	int		m_patn_generated_groups;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKruskalWallis)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKruskalWallis)
	afx_msg void OnDblclkAll();
	afx_msg void OnAdd();
	afx_msg void OnAddAll();
	afx_msg void OnDblclkSelected();
	afx_msg void OnRemoveAll();
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnKw();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KRUSKALWALLIS_H__8D608F53_8709_4CC1_96C9_A0877F2439D3__INCLUDED_)
