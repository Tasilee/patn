#if !defined(AFX_NEWDIALOG_H__B782B2BF_79CE_47C1_A410_B62874F8FD54__INCLUDED_)
#define AFX_NEWDIALOG_H__B782B2BF_79CE_47C1_A410_B62874F8FD54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// newDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CnewDialog dialog

class CnewDialog : public CDialog
{
// Construction
public:
	CnewDialog(CWnd* pParent = NULL);   // standard constructor
	bool Create(CString str);

	void inc();

	long * m_progress;
	long id;
	CWnd * parent;
	bool m_Cancel;
	CString m_str;

	SYSTEMTIME m_systime;
	bool m_time;
	double *m_end,*m_start;

// Dialog Data
	//{{AFX_DATA(CnewDialog)
	enum { IDD = IDD_NEWDIALOG };
	CEdit	m_Label;
	CProgressCtrl	m_Progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CnewDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CnewDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnCancelMode();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDIALOG_H__B782B2BF_79CE_47C1_A410_B62874F8FD54__INCLUDED_)
