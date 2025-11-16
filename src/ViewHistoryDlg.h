#include "afxwin.h"
#if !defined(AFX_VIEWHISTORYDLG_H__543A9A69_E350_4A45_98DF_ECCB71BA123E__INCLUDED_)
#define AFX_VIEWHISTORYDLG_H__543A9A69_E350_4A45_98DF_ECCB71BA123E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewHistoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewHistoryDlg dialog

class CViewHistoryDlg : public CDialog
{
// Construction
public:
	CViewHistoryDlg(CWnd* pParent = NULL);   // standard constructor

	long m_dist,m_dist2;
	long m_created;

	CString * m_plog;

// Dialog Data
	//{{AFX_DATA(CViewHistoryDlg)
	enum { IDD = IDD_VIEWHISTORY };
	CButton	m_Box3;
	CEdit	m_Box2;
	CButton	m_Box1;
	CString	m_history;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewHistoryDlg)
	afx_msg void OnButton2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnClearHistory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_box4;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWHISTORYDLG_H__543A9A69_E350_4A45_98DF_ECCB71BA123E__INCLUDED_)
