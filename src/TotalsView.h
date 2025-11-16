// TotalsView.h : header file
//

#ifndef TOTALSVIEW_H
#define TOTALSVIEW_H

/////////////////////////////////////////////////////////////////////////////
// CTotalsView view

class CTotalsView : public CView
{
protected:
	CTotalsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTotalsView)

// Attributes
public:

protected:
	CBrush m_brushWindow;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTotalsView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTotalsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTotalsView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

#endif // TOTALSVIEW_H
