#if !defined(AFX_EDITTEXT_H__A245DB82_6206_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_EDITTEXT_H__A245DB82_6206_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditText dialog

class CEditText : public CDialog
{
// Construction
public:
	CEditText(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditText)
	enum { IDD = IDD_EDITTEXT };
	CSpinButtonCtrl	m_Spin;
	CString	m_text;
	int		m_Fontsize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditText)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTEXT_H__A245DB82_6206_11D4_A3D4_525405F5DADA__INCLUDED_)
