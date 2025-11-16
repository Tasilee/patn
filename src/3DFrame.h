#if !defined(AFX_3DFRAME_H__938F9A23_DA42_48BE_81A3_E4993BF316EC__INCLUDED_)
#define AFX_3DFRAME_H__938F9A23_DA42_48BE_81A3_E4993BF316EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C3DFrame frame

class C3DFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(C3DFrame)
protected:
	C3DFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3DFrame();

	// Generated message map functions
	//{{AFX_MSG(C3DFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DFRAME_H__938F9A23_DA42_48BE_81A3_E4993BF316EC__INCLUDED_)
