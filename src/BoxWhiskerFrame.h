#if !defined(AFX_BOXWHISKERFRAME_H__0B4E76B4_F5E3_4938_9D73_F4F3AD2A95BA__INCLUDED_)
#define AFX_BOXWHISKERFRAME_H__0B4E76B4_F5E3_4938_9D73_F4F3AD2A95BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoxWhiskerFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerFrame frame

class CBoxWhiskerFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBoxWhiskerFrame)
protected:
	CBoxWhiskerFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoxWhiskerFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBoxWhiskerFrame();

	// Generated message map functions
	//{{AFX_MSG(CBoxWhiskerFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXWHISKERFRAME_H__0B4E76B4_F5E3_4938_9D73_F4F3AD2A95BA__INCLUDED_)
