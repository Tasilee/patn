#if !defined(AFX_TABLEPROPERTIES_H__4D8951E1_7A7E_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_TABLEPROPERTIES_H__4D8951E1_7A7E_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTableProperties dialog

class CTableProperties : public CDialog
{
// Construction
public:
	CTableProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTableProperties)
	enum { IDD = IDD_TABLEPROPERTIES };
	CString	m_TableName;
	double	m_MissingValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableProperties)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEPROPERTIES_H__4D8951E1_7A7E_11D4_A3D4_525405F5DADA__INCLUDED_)
