#if !defined(AFX_SCATFRAME_H__832BB268_7CC3_4186_9316_19D16F93C815__INCLUDED_)
#define AFX_SCATFRAME_H__832BB268_7CC3_4186_9316_19D16F93C815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScatFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScatFrame frame

class CScatFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CScatFrame)
protected:
	CScatFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScatFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScatFrame();

	// Generated message map functions
	//{{AFX_MSG(CScatFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCATFRAME_H__832BB268_7CC3_4186_9316_19D16F93C815__INCLUDED_)
