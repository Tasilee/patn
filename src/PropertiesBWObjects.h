#if !defined(AFX_PROPERTIESBWOBJECTS_H__13D7860D_F8F9_4B4E_99CE_37F01C34CED2__INCLUDED_)
#define AFX_PROPERTIESBWOBJECTS_H__13D7860D_F8F9_4B4E_99CE_37F01C34CED2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesBWObjects.h : header file
//
#include "patndoc.h"
#include "texture.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertiesBWObjects dialog

class CPropertiesBWObjects : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertiesBWObjects)

// Construction
public:
	CPropertiesBWObjects();
	~CPropertiesBWObjects();

	CPatnDoc * m_pDoc;
	bool m_ObjectsSelected;
	bool m_apriori;
	CTexture m_tex;

	void RefreshPatterns();

// Dialog Data
	//{{AFX_DATA(CPropertiesBWObjects)
	enum { IDD = IDD_PROPERTIES_BWOBJECTS };
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
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesBWObjects)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertiesBWObjects)
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

#endif // !defined(AFX_PROPERTIESBWOBJECTS_H__13D7860D_F8F9_4B4E_99CE_37F01C34CED2__INCLUDED_)
