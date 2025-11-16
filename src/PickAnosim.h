#if !defined(AFX_PICKANOSIM_H__C9FCF041_16A5_4672_9005_34A59120EAE4__INCLUDED_)
#define AFX_PICKANOSIM_H__C9FCF041_16A5_4672_9005_34A59120EAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PickAnosim.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPickAnosim dialog

class CPickAnosim : public CDialog
{
// Construction
public:
	CPickAnosim(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPickAnosim)
	enum { IDD = IDD_PICK_ANOSIM };
	int		m_columns;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickAnosim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickAnosim)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICKANOSIM_H__C9FCF041_16A5_4672_9005_34A59120EAE4__INCLUDED_)
