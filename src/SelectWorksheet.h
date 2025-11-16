#if !defined(AFX_SELECTWORKSHEET_H__BCFD71E9_4CF3_4D5E_A147_2FC415B60951__INCLUDED_)
#define AFX_SELECTWORKSHEET_H__BCFD71E9_4CF3_4D5E_A147_2FC415B60951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectWorksheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheet dialog

class CSelectWorksheet : public CDialog
{
// Construction
public:
	CSelectWorksheet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectWorksheet)
	enum { IDD = IDD_EXCEL_WORKSHEET };
	CListBox	m_tables;
	//}}AFX_DATA

	CArray<CString> tables;
	long m_selectedidx;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectWorksheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectWorksheet)
	afx_msg void OnDblclkTables();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTWORKSHEET_H__BCFD71E9_4CF3_4D5E_A147_2FC415B60951__INCLUDED_)
