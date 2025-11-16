#if !defined(AFX_PROPERTIESRANDOM_H__FC1BA49A_F20F_48DC_8A9C_AA4CDDF09D67__INCLUDED_)
#define AFX_PROPERTIESRANDOM_H__FC1BA49A_F20F_48DC_8A9C_AA4CDDF09D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesRandom.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesRandom dialog

class CPropertiesRandom : public CPropertyPage
{
// Construction
public:
	CPropertiesRandom(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesRandom)
	enum { IDD = IDD_PROPERTIES_RANDOMTYPE };
	CEdit	m_about_method;
	int		m_randomtype;
	long	m_randomseed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesRandom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesRandom)
	afx_msg void OnExport();
	virtual BOOL OnInitDialog();
	afx_msg void OnUniformrandom();
	afx_msg void OnLuxury();
	afx_msg void OnMar();
	afx_msg void OnEnu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESRANDOM_H__FC1BA49A_F20F_48DC_8A9C_AA4CDDF09D67__INCLUDED_)
