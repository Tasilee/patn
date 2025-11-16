// fuseview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFuseView view

class CFuseMetrics 
{
public:
	long cyHeader, cyFooter; // only for printing
	CSize sizeObjectLabel;	// size of labels for objects
	long cyHeadingBar;
	long yHeadingBottom;
	long yScrollTop;			// top of scrollable area
	long cyLineSpacing;
	long LogPixelsX,LogPixelsY;  // pixels per inch in X and Y directions (depends on DC)
	long width,height;			// width & height of area to draw on
};

#include "viewextension.h"

class CFuseView : public CViewExtension
{
protected:
	CFuseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFuseView)

// Attributes
public:
	CArray<PageInfo*> m_pageinfo;
protected:
	CPatnDoc* m_pDoc;
	long m_idxTopRow;
	long m_idxTopPrintRow;	// used for printing
	LOGFONT m_lfHeading, m_lfHeaderFooter, m_lfText;
	long m_nHeadingPointSize, m_nHeaderFooterPointSize;
	CFuseMetrics m_FuseMetrics;
public:
	CFuseTable* m_pFuseTable;
	CRect m_rectDraw;

	long m_mousewheelsensitivity;

	long m_pointsize;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

// Operations
protected:
	void InitializeFonts();
	BOOL CalcMetrics(CDC* pDC);
	void SetMapping(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFuseView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFuseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFuseView)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOptionsFont();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveas();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CPatnDoc* GetDocument();
	virtual BOOL Create(LPCSTR lpszClassName,
		LPCSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext /* = NULL */);
};

#ifndef _DEBUG  // debug version in fuseview.cpp
inline CPatnDoc* CFuseView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
