#if !defined(AFX_EDITTEXT2_H__356AF347_EF39_4C97_ADCF_2387B04C7AF0__INCLUDED_)
#define AFX_EDITTEXT2_H__356AF347_EF39_4C97_ADCF_2387B04C7AF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditText2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditText2 dialog

class CEditText2 : public CDialog
{
// Construction
public:
	CEditText2(CWnd* pParent = NULL);   // standard constructor
	bool m_delete;
// Dialog Data
	//{{AFX_DATA(CEditText2)
	enum { IDD = IDD_EDITTEXT2 };
	CString	m_text;
	//}}AFX_DATA

	LOGFONT m_font;

	unsigned long m_colour;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditText2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditText2)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTEXT2_H__356AF347_EF39_4C97_ADCF_2387B04C7AF0__INCLUDED_)
