#if !defined(AFX_PROPERTIESGENERAL_H__7881DC85_A851_4BDE_867A_D36ADB438DD7__INCLUDED_)
#define AFX_PROPERTIESGENERAL_H__7881DC85_A851_4BDE_867A_D36ADB438DD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesGeneral dialog

class CPropertiesGeneral : public CPropertyPage
{
// Construction
public:
	CPropertiesGeneral(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesGeneral)
	enum { IDD = IDD_PROPERTIES_TABLEPROPERTIES };
	CString	m_tablename;
	double	m_missingvalue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesGeneral)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESGENERAL_H__7881DC85_A851_4BDE_867A_D36ADB438DD7__INCLUDED_)
