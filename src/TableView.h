// TableView.h : interface of the CTableView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PATN_CTABLEVIEW_
#define _PATN_CTABLEVIEW_

#include "numedit.h"
#include "RowTotalsView.h"
#include "ColumnTotalsView.h"
#include "Selection.h"
#include "comparerow.h"

class CPatnCntrItem;
class CRowTotalsView;
class CColumnTotalsView;
class CColumnSelection;
class CRowSelection;

struct CColumn
{
	long		m_idxStart;		// index of first doc column in hidden block if m_bIsHidden
	long		m_idx;			// index of column in document or last column if m_bIsHidden
	long		m_xLeft;		// left edge of column in view
	long		m_xRight;		// right edge of column in view
	BOOL	m_bIsHidden;	// flags that this column is a block of hidden column(s)
	BOOL	m_bIsExtrinsicBar;

	long		Width() { return m_xRight - m_xLeft + 1; } 
};

struct CRow
{
	long		m_idxStart;		// index of first doc row in hidden block if m_bIsHidden
	long		m_idx;			// index of row in document or last row if m_bIsHidden
	long		m_yTop;			// top edge of row in view
	long		m_yBottom;		// bottom edge of row in view
	BOOL	m_bIsHidden;	// flags that this row is a block of hidden row(s)
	BOOL	m_bIsExtrinsicBar;

	long		Height() { return m_yBottom - m_yTop + 1; } 
};

struct CCell
{
	CCell();
	long		m_idxRow;
	long		m_idxColumn;
};

struct CRange
{
	CCell	m_cellTopLeft;
	CCell	m_cellBottomRight;
};

class CUpdateHint : public CObject
// used to pass update hints to views
{
public:
	CUpdateHint();
	DECLARE_DYNAMIC( CUpdateHint )

public:
	CCell	m_Cell;
	CRange	m_Range;
	long		m_idxOld;
	long		m_idxNew;
	long		m_nCount;
	CSelection* m_pSelection;
	CArray<int>* m_pItems;
	
	// update hints
	enum
	{
		none,
		editCell,
		setFont,
		insertColumn,
		deleteColumn,
		moveColumn,
		addColumn,
		hideColumn,
		unhideColumn,
		resizeColumn,
		makeColumnExtrinsic,
		makeColumnIntrinsic,
		insertRow,
		deleteRow,
		moveRow,
		addRow,
		resizeRow,
		hideRow,
		unhideRow,
		makeRowExtrinsic,
		makeRowIntrinsic,
		changeColumnTotalsOrder,
		changeColumnIntrinsicTotalsOrder,
		changeColumnAllocOrder,
		changeColumnCompareOrder,
		changeColumnPccOrder,
		changeRowTotalsOrder,
		changeRowAllocOrder,
		resizeRowTotalsColumn,
		resizeRowAllocColumn,
		changeRowOrdOrder,
		resizeRowOrdColumn,
		changeIntrinsicColumnTotalsOrder,
		changeIntrinsicRowTotalsOrder,
		resizeIntrinsicRowTotalsColumn,
		changeIntrinsicRowOrdOrder,
		resizeIntrinsicRowOrdColumn		
	};
};

class CTableView : public CView
{
friend class CDataTransformDlg; //****TODO - this really shouldn't be a friend as
friend class CPccSelection;     //it mainly operates on the data in CPatnDoc - it only
								//has to access this view's selected columns/rows to fill
								//the 'selected' listbox ****/

friend class CColumnSelection;	// manages selected columns
friend class CRowSelection;		// manages selected rows
friend class CSelectColumnsOrRowsDlg; // marks columns or rows as selected
friend class CColumnTotalsView;
friend class CRowTotalsView;
friend class CCompareRow;

protected: // create from serialization only
	CTableView();
	DECLARE_DYNCREATE(CTableView)

	// mouse operations
	enum
	{
		none,
		selectColumn,
		selectRow,
		resizeColumn,
		resizeRow,
		dragColumn,
		dragRow,
		unhideColumn,
		unhideRow,
		dragExtrinsicColumnBar,
		dragExtrinsicRowBar
	};

// Attributes
public:
	CPatnDoc* GetDocument();
	// m_pSelection holds the selection to the current CPatnCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CPatnCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CPatnCntrItem* m_pSelection;

	long m_mousewheelsensitivity;

	void AutoSizeColumns();
	bool autosizecolumns;
	void AutoSizeRows(CDC * dc);

// pointers to other views for easy reference
protected:
	CRowTotalsView *m_pRowTotalsView;
	CColumnTotalsView *m_pColumnTotalsView;

public:
	BOOL SetPointerToColumnTotalsView(CColumnTotalsView *pColumnTotalsView);
	BOOL SetPointerToRowTotalsView(CRowTotalsView *pRowTotalsView);

// data
protected:
	CArray<CColumn*> m_aColumns;
	CArray<CRow*> m_aRows;

//printing
protected:
	CArray<PageInfo*> m_pageinfo;
	long m_printcols;
	long m_printrows;

// drawing-related
protected:
	CFont	m_fontCells, m_fontPrinter;
	TEXTMETRIC	m_tm;
	CBrush	m_brushXBar; // brush for extrinsic bars

public:
	void	SetTableFont();

protected:
	void	DrawColumnBtnBar(CDC* pDC);
	void	DrawRowBtnBar(CDC* pDC);
	void	HighlightColumn(long idx);
	void	HighlightColumn(long idx, CDC* pDC);
	void	HighlightRow(long idx);
	void	HighlightRow(long idx, CDC* pDC);
	void	UnHighlightColumn(long idx);
	void	UnHighlightColumn(long idx, CDC* pDC);
	void	UnHighlightRow(long idx);
	void	UnHighlightRow(long idx, CDC* pDC);
	void	ClearBackground(CRect& rect);
	void	ClearBackground(CRect& rect, CDC* pDC);

// layout, co-ordinates
public:
	long		m_idxLeftColumn;
	long		m_idxTopRow;
	long		m_nRowHeight;  // height of each row of data in the table
	float	m_nWidthUnit;  // unit used for column widths;
	long		m_nColumnBtnBarHeight;
	long		m_nRowBtnBarWidth;
	long		m_nExtrinsicBarHeight;	// extrinsic rows bar
	long		m_nExtrinsicBarWidth;	// extrinsic columns bar
	long		m_cxClient, m_cyClient;
	long		m_xGrid, m_yGrid; // stores edges of grid if smaller than client area
	CRect	m_rGrid; // to right & below btn bars - used for hit-testing
	CRect	m_rColumnBtnBar, m_rRowBtnBar;				// used for hit-testing
	CRect	m_rExtrinsicColumnBar, m_rExtrinsicRowBar;	// used for hit-testing

	void	CalcLayout();
	void	GetCellRect(long idxColumn, long idxRow, CRect& rCell);
	void	GetCellRect(CCell& cell, CRect& rCell);
	long		CalcViewColumnWidth(long idxColumn); // converts doc column width into view units
	double	CalcDocColumnWidth(long nWidth); // converts view column width into doc units
	long 	GetLastVisibleColumn();
	long 	GetLastVisibleRow();
	long 	GetColumn(CPoint point);
	long 	GetRow(CPoint point);
	long 	GetColumn(long idx /* doc idx */);
	long 	GetRow(long idx /* doc idx */);

	CRect m_rectDraw;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog * m_psd;

	

// active cell
protected:
	struct CActiveCell
	{
		CNumEdit m_edit;  // the edit control used for editing cell contents
		long		m_idxColumn, m_idxRow;
		BOOL	m_bIsEditing;
	} m_ActiveCell;

	public:
	void	SetActiveCellAt(long idxColumn,long idxRow,BOOL bScrollIntoView = TRUE);
	void	GetActiveCellRect(CRect& rect);
	void	ScrollActiveCellIntoView();
	void	StartEdit(BOOL bCut = TRUE);
	void	EndEdit();
	void	DrawFrame();
	void	DrawFrame(CDC* pDC);
	void	RemoveFrame();
	void	RemoveFrame(CDC* pDC);
	
// generic mouse operations
protected:
	long		m_nMouseAction; // enum what mouse action is taking place, eg dragColumn
	CPoint	m_ptClick; 

	long		HitTestColumnBtnBar(CPoint point, long& iColumn);
	long		HitTestRowBtnBar(CPoint point, long& iColumn);

// selection
public:
	CColumnSelection* m_pSelectedColumns;
	CRowSelection* m_pSelectedRows;
	bool m_copylabels;

	CArray<Sel*> m_aSavedSelections;

	void SelectionActivate(int n);
	void SelectionSaveCurrentSelection(int n);

// column resizing
protected:
	long 	m_iResizeColumn;	// array idx of column being resized
	long 	m_nStartX;	// X-coord of mouse when resizing started
	long 	m_nOldX;	// X-coord of mouse on last mouse move message

	void DrawResizeColumnLine(long x);
	void ClearResizeColumnLine();
	
// row resizing
protected:
	long 	m_iResizeRow;	// array idx of row being resized
	long 	m_nStartY;		// Y-coord of mouse when resizing started
	long 	m_nOldY;			// Y-coord of mouse on last mouse move message
						protected:
	void DrawResizeRowLine(long y);
	void ClearResizeRowLine();
	void SizeToFit(int idxColumn);
	void SizeToFit(CArray<int>* pItems);

	void AssignRowsToGroups();
	int assocDistance(long i, double * centroids, long ncols, long ngroups);
	
// column moving
protected:
	long 	m_idxMoveColumn;	// doc idx of column being moved

	void DrawMoveColumnLine(long x);
	void ClearMoveColumnLine();
	
// row moving
protected:
	long 	m_idxMoveRow;	// doc idx of row being moved

	void DrawMoveRowLine(long y);
	void ClearMoveRowLine();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTableView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTableView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTableColumnAdd();
	afx_msg void OnTableColumnDelete();
	afx_msg void OnTableColumnInsert();
	afx_msg void OnTableColumnMakeExtrinsic();
	afx_msg void OnTableColumnMakeIntrinsic();
	afx_msg void OnTableColumnProperties();
	afx_msg void OnTableRowAdd();
	afx_msg void OnTableRowDelete();
	afx_msg void OnTableRowInsert();
	afx_msg void OnTableRowMakeExtrinsic();
	afx_msg void OnTableRowMakeIntrinsic();
	afx_msg void OnTableRowProperties();
	afx_msg void OnTableFont();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTableColumnSelect();
	afx_msg void OnTableRowSelect();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTableColumnHide();
	afx_msg void OnTableColumnGotoExtrinsics();
	afx_msg void OnTableColumnUnhide();
	afx_msg void OnTableRowGotoExtrinsics();
	afx_msg void OnTableRowHide();
	afx_msg void OnTableRowUnhide();
	afx_msg void OnToolsDataTransformation();
	afx_msg void OnToolsDataAnalysis();
	afx_msg void OnTableColumnGroups();
	afx_msg void OnTableColumnFind();
	afx_msg void OnTableRowFind();
	afx_msg void OnTableRowGroups();
	afx_msg void OnUpdateTableColumnMakeExtrinsic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableColumnMakeIntrinsic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableRowMakeExtrinsic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableRowMakeIntrinsic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableColumnGotoExtrinsics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableRowGotoExtrinsics(CCmdUI* pCmdUI);
	afx_msg void OnFileAppend();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnToolsDataevaluation();
	afx_msg void OnTableTansposedatalabels();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditOtherlabelsincopycut();
	afx_msg void OnTableSelectionoptions();
	afx_msg void OnDeleteDuplicates();
	//}}AFX_MSG
	afx_msg LRESULT OnResizeEdit(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
//	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	afx_msg void ontest();
	afx_msg void OnDataSwapcolumnintrinsicswithextrinsics();
//	afx_msg void OnUpdateDataSwapcolumnintrinsicswithextrinsics(CCmdUI *pCmdUI);
	afx_msg void OnDataSwaprowintrinsicswithextrinsics();
//	afx_msg void OnUpdateDataSwaprowintrinsicswithextrinsics(CCmdUI *pCmdUI);
	afx_msg void OnDataVariablesKwToExtrinsics();
	afx_msg void OnUpdateDataVariablesKwToExtrinsics(CCmdUI *pCmdUI);
	afx_msg void OnDataConstancyandfidelity();
	afx_msg void OnUpdateDataConstancyandfidelity(CCmdUI *pCmdUI);
	afx_msg void OnSelectionsClear();
	afx_msg void OnUpdateSelectionsClear(CCmdUI *pCmdUI);
	afx_msg void OnSelectionsSave();
	afx_msg void OnUpdateSelectionsSave(CCmdUI *pCmdUI);
	afx_msg void OnSelection1Activate();
	afx_msg void OnSelection2Activate();
	afx_msg void OnSelection3Activate();
	afx_msg void OnSelection4Activate();
	afx_msg void OnSelection5Activate();
	afx_msg void OnSelection6Activate();
	afx_msg void OnUpdateSelection1Activate(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection2Activate(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection3Activate(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection4Activate(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection5Activate(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection6Activate(CCmdUI * pCmdUI);
	afx_msg void OnSelection1SaveCurrentSelection();
	afx_msg void OnSelection2SaveCurrentSelection();
	afx_msg void OnSelection3SaveCurrentSelection();
	afx_msg void OnSelection4SaveCurrentSelection();
	afx_msg void OnSelection5SaveCurrentSelection();
	afx_msg void OnSelection6SaveCurrentSelection();
	afx_msg void OnUpdateSelection1SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection2SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection3SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection4SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection5SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnUpdateSelection6SaveCurrentSelection(CCmdUI * pCmdUI);
	afx_msg void OnDataAssigngroupstonon();
	afx_msg void OnUpdateDataAssigngroupstonon(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in TableView.cpp
inline CPatnDoc* CTableView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// inline functions

inline long CTableView::CalcViewColumnWidth(long idxColumn)
{
	return (long)(GetDocument()->GetColumnWidth(idxColumn) * m_nWidthUnit) + g_clm + g_crm;
}

#endif