#if !defined(AFX_PROPERTIESFONT_H__1E697406_0C17_4C3E_8D4A_07D94A0BE467__INCLUDED_)
#define AFX_PROPERTIESFONT_H__1E697406_0C17_4C3E_8D4A_07D94A0BE467__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesFont.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesFont dialog

class CPropertiesFont : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertiesFont)

// Construction
public:
	CPropertiesFont();
	~CPropertiesFont();

	CPatnDoc * pDoc;

// Dialog Data
	//{{AFX_DATA(CPropertiesFont)
	enum { IDD = IDD_PROPERTIES_FONT };
	CStatic	m_text_example;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesFont)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertiesFont)
	afx_msg void OnFont();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESFONT_H__1E697406_0C17_4C3E_8D4A_07D94A0BE467__INCLUDED_)
