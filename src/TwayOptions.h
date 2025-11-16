#if !defined(AFX_TWAYOPTIONS_H__746133A1_2C9A_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_TWAYOPTIONS_H__746133A1_2C9A_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwayOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTwayOptions dialog

class CTwayOptions : public CDialog
{
// Construction
public:
	CTwayOptions(CWnd* pParent = NULL);   // standard constructor
	long m_colstand;
	long m_globstand;
	long m_presabs;
	long m_nScale;
	long m_red;
	long m_green;
	long m_blue;
	long m_bw;
// Dialog Data
	//{{AFX_DATA(CTwayOptions)
	enum { IDD = IDD_TWAY_OPTIONS };
	CButton	m_GlobStd;
	CButton	m_ColStd;
	CButton	m_PresAbs;
	CButton	m_Red;
	CButton	m_Green;
	CButton	m_Blue;
	CButton	m_Bw;
	int		m_Scale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTwayOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTwayOptions)
	afx_msg void OnGlobstd();
	afx_msg void OnColstd();
	virtual BOOL OnInitDialog();
	afx_msg void OnGreen();
	afx_msg void OnRed();
	afx_msg void OnBlue();
	afx_msg void OnBw();
	afx_msg void OnPresabs();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWAYOPTIONS_H__746133A1_2C9A_11D4_A3D4_525405F5DADA__INCLUDED_)
