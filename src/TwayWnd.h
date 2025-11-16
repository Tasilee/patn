#if !defined(AFX_TWAYWND_H__E80DFD1C_AE4D_423B_A06E_DA0D0D226E9D__INCLUDED_)
#define AFX_TWAYWND_H__E80DFD1C_AE4D_423B_A06E_DA0D0D226E9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TwayWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTwayWnd frame

class CTwayWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTwayWnd)
protected:
	CTwayWnd();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTwayWnd)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTwayWnd();

	// Generated message map functions
	//{{AFX_MSG(CTwayWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWAYWND_H__E80DFD1C_AE4D_423B_A06E_DA0D0D226E9D__INCLUDED_)
