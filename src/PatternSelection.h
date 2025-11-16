#if !defined(AFX_PATTERNSELECTION_H__F940C9C2_9EBC_11D4_A3D5_525405F5DADA__INCLUDED_)
#define AFX_PATTERNSELECTION_H__F940C9C2_9EBC_11D4_A3D5_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatternSelection.h : header file
//

#include "texture.h"

/////////////////////////////////////////////////////////////////////////////
// CPatternSelection dialog

class CPatternSelection : public CDialog
{
// Construction
public:
	CPatternSelection(CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;
	bool m_ObjectsSelected;
	bool m_apriori;
	CTexture m_tex;

	void RefreshPatterns();

// Dialog Data
	//{{AFX_DATA(CPatternSelection)
	enum { IDD = IDD_PATTERN_ORDER };
	CSliderCtrl	m_ScrollPatterns;
	CSliderCtrl	m_ScrollObjects;
	CButton	m_Assign;
	CButton	m_Reset;
	CButton	m_ObjectsSelection;
	CListBox	m_Patterns;
	CListBox	m_Objects;
	int		m_Radio1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternSelection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPatternSelection)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnSelchangeObjects();
	afx_msg void OnAssign();
	afx_msg void OnReset();
	afx_msg void OnSelchangePatterns();
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATTERNSELECTION_H__F940C9C2_9EBC_11D4_A3D5_525405F5DADA__INCLUDED_)
