#if !defined(AFX_OBJECTSIZE_H__A855DCB7_9FF7_4205_B96C_F16AA8017794__INCLUDED_)
#define AFX_OBJECTSIZE_H__A855DCB7_9FF7_4205_B96C_F16AA8017794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectSize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectSize dialog

class CObjectSize : public CDialog
{
// Construction
public:
	CObjectSize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectSize)
	enum { IDD = IDD_OBJECTSIZE };
	CStatic	m_label;
	double	m_objectsize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectSize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectSize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTSIZE_H__A855DCB7_9FF7_4205_B96C_F16AA8017794__INCLUDED_)
