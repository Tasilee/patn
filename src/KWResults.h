#if !defined(AFX_KWRESULTS_H__24C18BA1_9CEF_426B_9506_95591B737F2D__INCLUDED_)
#define AFX_KWRESULTS_H__24C18BA1_9CEF_426B_9506_95591B737F2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KWResults.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKWResults dialog

class CKWResults : public CDialog
{
// Construction
public:
	CKWResults(CPatnDoc * pDoc,CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;

	bool m_values;

// Dialog Data
	//{{AFX_DATA(CKWResults)
	enum { IDD = IDD_KWRESULTS };
	CListBox	m_Columns;
	CButton		m_ordertable;
	CButton		m_ordervalues;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKWResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKWResults)
	virtual BOOL OnInitDialog();
	afx_msg void OnOrdertable();
	afx_msg void OnOrdervalue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KWRESULTS_H__24C18BA1_9CEF_426B_9506_95591B737F2D__INCLUDED_)
