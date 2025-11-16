#if !defined(AFX_PROPERTIESTABLE_H__669D35C9_9D5A_4CA6_AAC8_8C68FFF7DC74__INCLUDED_)
#define AFX_PROPERTIESTABLE_H__669D35C9_9D5A_4CA6_AAC8_8C68FFF7DC74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesTable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTable dialog

class CPropertiesTable : public CDialog
{
// Construction
public:
	CPropertiesTable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesTable)
	enum { IDD = IDD_PROPERTIES_TABLEPROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesTable)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESTABLE_H__669D35C9_9D5A_4CA6_AAC8_8C68FFF7DC74__INCLUDED_)
