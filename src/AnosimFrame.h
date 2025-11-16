#if !defined(AFX_ANOSIMFRAME_H__8D198392_5395_4FB9_8BEA_B99CB77A744C__INCLUDED_)
#define AFX_ANOSIMFRAME_H__8D198392_5395_4FB9_8BEA_B99CB77A744C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnosimFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnosimFrame frame

class CAnosimFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CAnosimFrame)
protected:
	CAnosimFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnosimFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAnosimFrame();

	// Generated message map functions
	//{{AFX_MSG(CAnosimFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANOSIMFRAME_H__8D198392_5395_4FB9_8BEA_B99CB77A744C__INCLUDED_)
