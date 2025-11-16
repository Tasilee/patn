#if !defined(AFX_GETUSERIDENTITY_H__271B499B_89BA_4213_B7DF_E0248588BBC8__INCLUDED_)
#define AFX_GETUSERIDENTITY_H__271B499B_89BA_4213_B7DF_E0248588BBC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetUserIdentity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetUserIdentity dialog

class CGetUserIdentity : public CDialog
{
// Construction
public:
	CGetUserIdentity(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetUserIdentity)
	enum { IDD = IDD_GETUSERIDENTITY };
	CString	m_user_name;
	CString	m_user_key;
	CString	m_user_org;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetUserIdentity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetUserIdentity)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETUSERIDENTITY_H__271B499B_89BA_4213_B7DF_E0248588BBC8__INCLUDED_)
