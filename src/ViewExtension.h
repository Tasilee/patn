#if !defined(AFX_VIEWEXTENSION_H__7287C8D8_C68C_468B_B6DF_4B7CAA3DDBA1__INCLUDED_)
#define AFX_VIEWEXTENSION_H__7287C8D8_C68C_468B_B6DF_4B7CAA3DDBA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewExtension.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewExtension view

class CViewExtension : public CView
{
protected:
	CViewExtension();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewExtension)

// Attributes
public:

// Operations
public:

	CMenu m_menu;
	virtual CMenu * GetMenu();

	bool m_activated;
	virtual bool IsActivated();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewExtension)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewExtension();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewExtension)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWEXTENSION_H__7287C8D8_C68C_468B_B6DF_4B7CAA3DDBA1__INCLUDED_)
