// numedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumEdit window

#ifndef _PATN_CNUMEDIT_
#define _PATN_CNUMEDIT_

class CNumEdit : public CEdit
{
// Construction
public:
	CNumEdit();

// Attributes
public:
	CString strLast;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNumEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChange();
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif