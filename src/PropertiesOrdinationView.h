#if !defined(AFX_PROPERTIESORDINATIONVIEW_H__7EEF4F84_D7CD_4E6F_A27D_6690A3C8ABC6__INCLUDED_)
#define AFX_PROPERTIESORDINATIONVIEW_H__7EEF4F84_D7CD_4E6F_A27D_6690A3C8ABC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesOrdinationView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesOrdinationView dialog

class CPropertiesOrdinationView : public CPropertyPage
{
// Construction
public:
	CPropertiesOrdinationView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesOrdinationView)
	enum { IDD = IDD_PROPERTIES_ORDINATIONDISPLAYTYPE };
	int		m_ordtype;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesOrdinationView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesOrdinationView)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESORDINATIONVIEW_H__7EEF4F84_D7CD_4E6F_A27D_6690A3C8ABC6__INCLUDED_)
