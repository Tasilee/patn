#if !defined(AFX_CHOOSEANOSIM_H__BFEA2C47_3E19_40E9_AF97_27668BB41438__INCLUDED_)
#define AFX_CHOOSEANOSIM_H__BFEA2C47_3E19_40E9_AF97_27668BB41438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseAnosim.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseAnosim dialog

class CChooseAnosim : public CDialog
{
// Construction
public:
	CChooseAnosim(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseAnosim)
	enum { IDD = IDD_CHOOSEANOSIM };
	int		m_Type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseAnosim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseAnosim)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEANOSIM_H__BFEA2C47_3E19_40E9_AF97_27668BB41438__INCLUDED_)
