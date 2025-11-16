#if !defined(AFX_ANOSIMPAGE_H__3BD09EDF_AC71_4304_A5AD_CB9183481BEA__INCLUDED_)
#define AFX_ANOSIMPAGE_H__3BD09EDF_AC71_4304_A5AD_CB9183481BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnosimPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnosimPage dialog

class CAnosimPage : public CPropertyPage
{
// Construction
public:
	CAnosimPage(CPatnDoc * pDoc,CWnd* pParent = NULL);   // standard constructor
	CPatnDoc * m_pDoc;
	CArray<long> m_rows_groups;
	CArray<long> m_cols_groups;
	//bool m_rows_pairs,m_cols_pairs;

// Dialog Data
	//{{AFX_DATA(CAnosimPage)
	enum { IDD = IDD_ANOSIM_PAGE };
	CButton	m_SetRowGroups;
	CButton	m_SetColGroups;
	CEdit	m_RowSeed;
	CEdit	m_RowIterations;
	CEdit	m_ColumnIterations;
	CEdit	m_ColumnSeed;
	BOOL	m_cols;
	int		m_cols_iterations;
	int		m_cols_seed;
	int		m_rows_iterations;
	int		m_rows_seed;
	BOOL	m_rows;
	int		m_cols_pairs;
	int		m_rows_pairs;
	int		m_patn_generated_groups;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnosimPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnosimPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRows();
	afx_msg void OnColumns();
	afx_msg void OnRowpairgroups();
	afx_msg void OnRowallgroups();
	afx_msg void OnSetcolgroups();
	afx_msg void OnSetrowgroups();
	afx_msg void OnColpairgroups();
	afx_msg void OnColallgroups();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANOSIMPAGE_H__3BD09EDF_AC71_4304_A5AD_CB9183481BEA__INCLUDED_)
