#if !defined(AFX_ANOSIMRESULTS_H__A99141ED_EAAC_4579_B68C_29843DA6D414__INCLUDED_)
#define AFX_ANOSIMRESULTS_H__A99141ED_EAAC_4579_B68C_29843DA6D414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnosimResults.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnosimResults dialog

class CAnosimResults : public CDialog
{
// Construction
public:
	CAnosimResults(CPatnDoc * pDoc, CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;

// Dialog Data
	//{{AFX_DATA(CAnosimResults)
	enum { IDD = IDD_ANOSIMRESULTS };
	CButton	m_RowPairs;
	CButton	m_ColPairs;
	double	m_Best;
	double	m_Bw;
	int		m_Iterations;
	double	m_Stat;
	double	m_2Best;
	double	m_2Bw;
	double	m_2Iterations;
	double	m_2Stat;
	int		m_2Seed;
	int		m_Seed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnosimResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnosimResults)
	virtual BOOL OnInitDialog();
	afx_msg void OnRowpairs();
	afx_msg void OnColpairs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeStat();
	afx_msg void OnStnClicked1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANOSIMRESULTS_H__A99141ED_EAAC_4579_B68C_29843DA6D414__INCLUDED_)
