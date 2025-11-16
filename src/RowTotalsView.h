// RowTotalsView.h : header file
//

#ifndef ROWTOTALSVIEW_H
#define ROWTOTALSVIEW_H

#include "TotalsView.h"
#include "tabs.h"

struct CRowTotalsColumn
{
	long m_xLeft;
	long m_xRight;

	long	Width() { return m_xRight - m_xLeft + 1; } 
}; 

/////////////////////////////////////////////////////////////////////////////
// CRowTotalsView view

class CTableView;

class CRowTotalsView : public CTotalsView
{
protected:
	CRowTotalsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRowTotalsView)

	enum{ tabTotals, tabIntrinsicTotals, tabAlloc, tabOrd };
	bool m_intrinsic;

// Attributes
protected:
	CTableView* m_pTableView;
	long m_cxClient, m_cyClient;
	CTabs m_Tabs;
	CArray<CRowTotalsColumn*> m_aRowTotalsColumns, m_aRowAllocColumns, m_aRowOrdColumns;
	CArray<CRowTotalsColumn*>* m_pColumns;  // points to one of the above
	CArray<CColumnAttributes*>* m_pAttributes;// points to doc attrib arrays
	CRect m_rBtnBar;

// row moving/resizing
protected:
	// mouse operations
	enum
	{
		none,
		resizeColumn,
		dragColumn,
	};
	long		m_nMouseAction;
	long		m_xLine;	// x-pos of move/resize line
	long 	m_xStart;	// x-pos of cursor when started to resize column
	long		m_iSelectedColumn;
	CRect	m_rSelectedColumn; 

protected:
	void	ClearMoveColumnLine();
	void	DrawMoveColumnLine(long x);
	void	ClearResizeColumnLine();
	void	DrawResizeColumnLine(long x);
	void	SelectColumn(long idx);
	void	UnselectColumn();
	long		GetColumn(CPoint point);

// scrolling
public:
	void	VScroll(int yAmount); // if xAmount > 0 - scroll down; < 0 - scroll up
	void	Refresh();

// Operations
public:
	BOOL SetPointerToTableView(CTableView* pTableView);
	CPatnDoc* GetDocument();
	void CalcLayout();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRowTotalsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
	void DrawButtonBar(CDC *pDC);
	void DrawTotals(CDC *pDC);
	void DrawAlloc(CDC *pDC);
	void DrawOrd(CDC *pDC);

// Implementation
protected:
	virtual ~CRowTotalsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRowTotalsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnVisibledecimals0();
	afx_msg void OnVisibledecimals1();
	afx_msg void OnVisibledecimals2();
	afx_msg void OnVisibledecimals3();
	afx_msg void OnVisibledecimals4();
	afx_msg void OnVisibledecimals5();
	afx_msg void OnVisibledecimals6();
	afx_msg void OnVisibledecimals7();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RowTotalsView.cpp
inline CPatnDoc* CRowTotalsView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif // ROWTOTALSVIEW_H
