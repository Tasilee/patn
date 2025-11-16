#if !defined(AFX_PROPERTIESVISIBLESTATISTICS_H__37EF8B1D_AFFC_4C4F_9B31_0DC34AA34C12__INCLUDED_)
#define AFX_PROPERTIESVISIBLESTATISTICS_H__37EF8B1D_AFFC_4C4F_9B31_0DC34AA34C12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesVisibleStatistics.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesVisibleStatistics dialog

class CPropertiesVisibleStatistics : public CPropertyPage
{
// Construction
public:
	CPropertiesVisibleStatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesVisibleStatistics)
	enum { IDD = IDD_PROPERTIES_VISIBLESTATISTICS };
	CSpinButtonCtrl	m_vis_spin;
	CSpinButtonCtrl	m_col_spin;
	CButton	m_check;
	CEdit	m_col_edit_ctrl;
	BOOL	m_colmax;
	BOOL	m_colmean;
	BOOL	m_colmin;
	BOOL	m_colmissing;
	BOOL	m_colno;
	BOOL	m_colsum;
	BOOL	m_rowmax;
	BOOL	m_rowmean;
	BOOL	m_rowmin;
	BOOL	m_rowmissing;
	BOOL	m_rowno;
	BOOL	m_rowsum;
	int		m_visible_decimals;
	int		m_dec;
	BOOL	m_check_bool;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesVisibleStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesVisibleStatistics)
	afx_msg void OnColdeccheck();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESVISIBLESTATISTICS_H__37EF8B1D_AFFC_4C4F_9B31_0DC34AA34C12__INCLUDED_)
