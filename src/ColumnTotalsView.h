// ColumnTotalsView.h : header file
//

#ifndef COLUMNTOTALSVIEW_H
#define COLUMNTOTALSVIEW_H

#include "TotalsView.h"
#include "tabs.h"

struct CColumnTotalsRow
{
	long m_yTop;
	long m_yBottom;

	long		Height() { return m_yBottom - m_yTop + 1; } 
}; 

/////////////////////////////////////////////////////////////////////////////
// CColumnTotalsView view

class CTableView;

class CColumnTotalsView : public CTotalsView
{
protected:
	CColumnTotalsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CColumnTotalsView)

	enum{ tabTotals, tabIntrinsicTotals, tabAlloc, tabCompare, tabPcc};

// Attributes
protected:
	CTableView* m_pTableView;
	long m_cxClient, m_cyClient;
	
	CArray<CColumnTotalsRow*> m_aColumnTotalsRows, m_aColumnAllocRows, m_aColumnCompareRows, m_aColumnPcc;
	CArray<CColumnTotalsRow*>* m_pRows;  // points to one of the above
	CArray<CRowAttributes*>* m_pAttributes; // points to doc attrib arrays
	CRect m_rBtnBar;
public:
	CTabs m_Tabs;
	void ButtonAt(CPoint p)
	{
		this->OnLButtonDown(0,p);
		this->OnLButtonUp(0,p);
	}

// row moving/resizing
protected:
	// mouse operations
	enum
	{
		none,
		resizeRow,
		dragRow,
	};
	long		m_nMouseAction;
	long		m_yLine;	// y-pos of move line
	long		m_iSelectedRow;
	CRect	m_rSelectedRow; 

	bool m_intrinsic;

protected:
	void	ClearMoveRowLine();
	void	DrawMoveRowLine(long y);
	void	ClearResizeRowLine();
	void	DrawResizeRowLine(long y);
	void	SelectRow(long idx);
	void	UnselectRow();
	long		GetRow(CPoint point);
public:
	void	CheckPcc();

// scrolling
public:
	void	HScroll(long xAmount); // if xAmount > 0 - scroll right; < 0 - scroll left
	void	Refresh();

// Operations
public:
	BOOL SetPointerToTableView(CTableView *pTableView);
	CPatnDoc* GetDocument();
	void CalcLayout();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColumnTotalsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
	void DrawButtonBar(CDC *pDC);
	void DrawTotals(CDC *pDC);
	void DrawAlloc(CDC *pDC);
	void DrawCompare(CDC *pDC);
	void DrawPcc(CDC *pDC);

// Implementation
protected:
	virtual ~CColumnTotalsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CColumnTotalsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVisibledecimals7();
	afx_msg void OnVisibledecimals6();
	afx_msg void OnVisibledecimals5();
	afx_msg void OnVisibledecimals4();
	afx_msg void OnVisibledecimals3();
	afx_msg void OnVisibledecimals2();
	afx_msg void OnVisibledecimals1();
	afx_msg void OnVisibledecimals0();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ColumnTotalsView.cpp
inline CPatnDoc* CColumnTotalsView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // COLUMNTOTALSVIEW_H
