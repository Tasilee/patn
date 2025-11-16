#if !defined(AFX_KWOPTIONS_H__FCF22ACD_0E4D_4D62_88CE_5942ADA62A12__INCLUDED_)
#define AFX_KWOPTIONS_H__FCF22ACD_0E4D_4D62_88CE_5942ADA62A12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KWOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKWOptions dialog

class CKWOptions : public CDialog
{
// Construction
public:
	CKWOptions(CPatnDoc *pDoc,CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;
// Dialog Data
	//{{AFX_DATA(CKWOptions)
	enum { IDD = IDD_KWOPTIONS };
	CEdit	m_Visible;
	CEdit	m_Cut;
	CSpinButtonCtrl	m_Spin;
	CListBox	m_ColumnList;
	double	m_Cutoff;
	int		m_NumVisible;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKWOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKWOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KWOPTIONS_H__FCF22ACD_0E4D_4D62_88CE_5942ADA62A12__INCLUDED_)
