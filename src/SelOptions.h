#if !defined(AFX_SELOPTIONS_H__781B0E5C_B797_4C33_AA14_0139145FC3E2__INCLUDED_)
#define AFX_SELOPTIONS_H__781B0E5C_B797_4C33_AA14_0139145FC3E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelOptions.h : header file
//

#include "patndoc.h"

/////////////////////////////////////////////////////////////////////////////
// CSelOptions dialog

class CSelOptions : public CDialog
{
// Construction
public:
	CSelOptions(CPatnDoc * doc,CWnd* pParent = NULL);   // standard constructor

	CPatnDoc *m_doc;

	double GetValue(long row,long col);
	
	// Dialog Data
	//{{AFX_DATA(CSelOptions)
	enum { IDD = IDD_SELECTOPTIONS };
	int		m_rows;
	int		m_sum;
	int		m_less;
	double	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelOptions)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELOPTIONS_H__781B0E5C_B797_4C33_AA14_0139145FC3E2__INCLUDED_)
