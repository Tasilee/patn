#if !defined(AFX_APPENDOPTIOSN_H__6FD32A0F_43C8_4DC1_8E65_0D74F9BBAA4E__INCLUDED_)
#define AFX_APPENDOPTIOSN_H__6FD32A0F_43C8_4DC1_8E65_0D74F9BBAA4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppendOptiosn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppendOptiosn dialog

class CAppendOptiosn : public CDialog
{
// Construction
public:
	CAppendOptiosn(CWnd* pParent = NULL);   // standard constructor

	bool m_selb;
	CBitmap b,bp,l,lp,r,c,f;


// Dialog Data
	//{{AFX_DATA(CAppendOptiosn)
	enum { IDD = IDD_APPENDOPTIONS };
	CStatic	m_r;
	CButton	m_rl;
	CButton	m_m;
	CStatic	m_f;
	CStatic	m_c;
	CButton	m_cl;
	CStatic	m_lp;
	CStatic	m_l;
	CStatic	m_bp;
	CStatic	m_b;
	BOOL	m_columnlabels;
	BOOL	m_rowlabels;
	BOOL	m_match;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppendOptiosn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAppendOptiosn)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMatch();
	afx_msg void OnRl();
	afx_msg void OnCl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPENDOPTIOSN_H__6FD32A0F_43C8_4DC1_8E65_0D74F9BBAA4E__INCLUDED_)
