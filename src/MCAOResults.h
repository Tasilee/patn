#if !defined(AFX_MCAORESULTS_H__DA2D1BE6_CBE0_429F_A438_AF6AC16F8172__INCLUDED_)
#define AFX_MCAORESULTS_H__DA2D1BE6_CBE0_429F_A438_AF6AC16F8172__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MCAOResults.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMCAOResults dialog

class CMCAOResults : public CDialog
{
// Construction
public:
	CMCAOResults(CPatnDoc * pDoc,CWnd* pParent = NULL);   // standard constructor

	CPatnDoc * m_pDoc;

// Dialog Data
	//{{AFX_DATA(CMCAOResults)
	enum { IDD = IDD_MCAORESULTS };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCAOResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCAOResults)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCAORESULTS_H__DA2D1BE6_CBE0_429F_A438_AF6AC16F8172__INCLUDED_)
