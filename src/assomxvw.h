// assomxvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAssocMatrixView view

#ifndef _CASSOCMATRIXVIEW_
#define _CASSOCMATRIXVIEW_

class CAssocMxMetrics 
{
public:
	long cyHeader, cyFooter; // only for printing
	CSize sizeObjectLabel;	// size of labels for objects
	long cyHeadingBar;
	long yScrollTop;			// top of scrollable area
	long cyLineSpacing;
	long cxColumnWidth;
	long LogPixelsX,LogPixelsY;  // pixels per inch in X and Y directions (depends on DC)
	long width,height;			// width & height of area to draw on
	long nNumLinesPerPage;	// only used for printing		
	long cx1stColumnWidth;
};

#include "viewextension.h"

class CAssocMatrixView : public CViewExtension
{
protected:
	CAssocMatrixView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAssocMatrixView)

// Attributes
public:
	CAssocMatrix* m_pMatrix;

protected:
	CPatnDoc* m_pDoc;
	long m_idxLeftColumn;
	long m_idxTopRow;
	long m_idxTopPrintRow;		// used for printing
	long m_idxLeftPrintColumn;	// used for printing
	LOGFONT m_lfHeading, m_lfHeaderFooter, m_lfData;
	long m_nHeadingPointSize, m_nScaleLabelPointSize, m_nHeaderFooterPointSize;
	CAssocMxMetrics m_AssocMxMetrics;
	
	long m_mousewheelsensitivity;

	long m_tempheight;

	CRect m_rectDraw;
	CRect m_rect;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

// Operations
public:

protected:
	void InitializeFonts();
	BOOL CalcMetrics(CDC* pDC);
	void SetMapping(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssocMatrixView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAssocMatrixView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CAssocMatrixView)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
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

protected:
	CArray<PageInfo*> m_pageinfo;
	long m_printorder;

};

#ifndef _DEBUG  // debug version in fuseview.cpp
inline CPatnDoc* CAssocMatrixView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////

#endif