// RowTotalsView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "RowTotalsView.h"
#include "ColumnTotalsView.h"
#include "TableView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRowTotalsView

IMPLEMENT_DYNCREATE(CRowTotalsView, CView)

CRowTotalsView::CRowTotalsView()
{
	m_pTableView = NULL;
	m_nMouseAction = none;
	m_xLine = -1;
	m_xStart = -1;
	m_iSelectedColumn = -1;
	m_rSelectedColumn.SetRectEmpty();
	m_pColumns = NULL;
}

CRowTotalsView::~CRowTotalsView()
{
	long i;

	for ( i = 0; i < m_aRowTotalsColumns.GetSize(); i++ )
		delete m_aRowTotalsColumns[i];

	for ( i = 0; i < m_aRowAllocColumns.GetSize(); i++ )
		delete m_aRowAllocColumns[i];

	for ( i = 0; i < m_aRowOrdColumns.GetSize(); i++ )
		delete m_aRowOrdColumns[i];
}

BEGIN_MESSAGE_MAP(CRowTotalsView, CView)
	//{{AFX_MSG_MAP(CRowTotalsView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__VISIBLEDECIMALS_0, OnVisibledecimals0)
	ON_COMMAND(ID__VISIBLEDECIMALS_1, OnVisibledecimals1)
	ON_COMMAND(ID__VISIBLEDECIMALS_2, OnVisibledecimals2)
	ON_COMMAND(ID__VISIBLEDECIMALS_3, OnVisibledecimals3)
	ON_COMMAND(ID__VISIBLEDECIMALS_4, OnVisibledecimals4)
	ON_COMMAND(ID__VISIBLEDECIMALS_5, OnVisibledecimals5)
	ON_COMMAND(ID__VISIBLEDECIMALS_6, OnVisibledecimals6)
	ON_COMMAND(ID__VISIBLEDECIMALS_7, OnVisibledecimals7)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRowTotalsView drawing

void CRowTotalsView::OnDraw(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(m_pColumns == NULL)
		return;

	long		i;
	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	CalcLayout();
	m_Tabs.DrawTabs(pDC);

	CRect rect;
	GetClientRect(rect);
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,150));
	pDC->FillRect(rect,&brush);
	pDC->SetBkColor(RGB(255,255,150));

	pDC->SetTextColor(pDoc->m_rgbFontCells);

	CFont* oldFont = pDC->SelectObject(&(m_pTableView->m_fontCells));
	// select font into memory DC
	if ( oldFont == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for row totals");
	}

	DrawButtonBar(pDC);

	// extrinsic bar
	if ( pDoc->IsExtrinsicRows() &&
		(i = m_pTableView->GetRow(pDoc->GetIdxTopExtrinsicRow())) != -1 )
	{
		pDC->SelectObject(&m_pTableView->m_brushXBar);
		pDC->PatBlt(0,m_pTableView->m_aRows[i]->m_yTop,
			m_cxClient,m_pTableView->m_aRows[i]->Height(),PATCOPY);
	}

	switch (m_Tabs.GetIdxActiveTab())
	{
		case tabTotals:
		case tabIntrinsicTotals:
			DrawTotals(pDC);
			break;

		case tabAlloc:
			DrawAlloc(pDC);
			break;

		case tabOrd:
			DrawOrd(pDC);
			break;
	}

	pDC->SelectObject(oldFont);
	IN_OnDraw = FALSE;
//	RedrawWindow();
}

void CRowTotalsView::DrawButtonBar(CDC *pDC)
{
	CPatnDoc* pDoc = GetDocument();
	CBrush*	pOldBrush;
	long		nOldBkColor;
	CRect	rText;
	long		nCount = (long)m_pColumns->GetSize();
	long		x;
	long		i;
	CSize	size;
	CString	str;
	CString strEllipsis = "...";
	long		cxEllipsis = (pDC->GetTextExtent(strEllipsis)).cx;

	if(m_Tabs.GetIdxActiveTab() == tabOrd && !(pDoc->m_Ord3d == 3)) nCount--;

	nOldBkColor = pDC->SetBkColor(RGB(192,192,192));

	// buttons
	if(m_Tabs.GetIdxActiveTab() == tabOrd && !(pDoc->m_Ord3d == 3)) x = m_pColumns->GetAt(m_pColumns->GetUpperBound()-1)->m_xRight + 1;
	else x = m_pColumns->GetAt(m_pColumns->GetUpperBound())->m_xRight + 1;
	pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(0,0,x,m_pTableView->m_nColumnBtnBarHeight,PATCOPY);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,x,1,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(0,m_pTableView->m_nColumnBtnBarHeight,x,1,PATCOPY);

	for ( i = 0; i < nCount; i++ )
	{
		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->PatBlt(m_pColumns->GetAt(i)->m_xLeft,0,
					1,m_pTableView->m_nColumnBtnBarHeight,PATCOPY);

		pDC->SelectStockObject(GRAY_BRUSH);
		pDC->PatBlt(m_pColumns->GetAt(i)->m_xRight,0,
					1,m_pTableView->m_nColumnBtnBarHeight,PATCOPY);
	}

	// button labels
	//nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	rText.top = g_ctm;
	rText.bottom = m_pTableView->m_nColumnBtnBarHeight - g_cbm;
    for ( i = 0; i < nCount; i++ )
    {
		rText.left = m_pColumns->GetAt(i)->m_xLeft + g_clm;
		rText.right = m_pColumns->GetAt(i)->m_xRight - g_crm;
		// if at least one char can be printed
    	if ( m_pTableView->m_tm.tmAveCharWidth <= rText.Width() )
    	{
			m_pAttributes->GetAt(i)->GetLabel(str);
			size = pDC->GetTextExtent(str,str.GetLength());
			// if it fits, center the label on the button ...
			if ( size.cx <= rText.Width() )
			{
				pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.right - rText.Width()/2;
			}
			// ... otherwise, left-justify and print as much of the label as will fit
			else
			{
				pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.left;

				long cxExtent = 0;
				long cxMax = rText.Width() - cxEllipsis;
				long nCharMax = str.GetLength();
				long nCharCount;
				CString strTemp, strOut;
				for ( nCharCount = 1; nCharCount <= nCharMax; nCharCount++ )
				{
					strTemp = str.Left(nCharCount);
					cxExtent = (pDC->GetTextExtent(strTemp)).cx;
					if ( cxExtent > cxMax )
						break;
					strOut = strTemp;
				}
				strOut += strEllipsis;
				str = strOut;
				/****/
			}
			pDC->ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,
							rText,str,str.GetLength(),NULL);
		}
	}

	pDC->SelectObject(pOldBrush);
	pDC->SetBkColor(nOldBkColor);
}

void CRowTotalsView::DrawTotals(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = (long)m_pColumns->GetSize();
	long		nRowLimit;// = m_pTableView->m_aRows.GetSize();
	long		iCol, iRow;
	CString	strValue;
	CRow* pRow;

	if(pDoc->GetIdxTopExtrinsicRow() == -1 || !m_intrinsic) nRowLimit = pDoc->GetNoOfRows();
	else nRowLimit = pDoc->GetIdxTopExtrinsicRow();

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	bool redrawthis = false;

	//nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	for ( iCol = 0; iCol < nColLimit; iCol++ )
	{
		rText.left = m_pColumns->GetAt(iCol)->m_xLeft + g_clm;
		rText.right = m_pColumns->GetAt(iCol)->m_xRight - g_crm;

		for ( iRow = 0; iRow < m_pTableView->m_aRows.GetSize() &&
			m_pTableView->m_aRows[iRow]->m_idx < nRowLimit; iRow++ )
		{
			pRow = m_pTableView->m_aRows[iRow];

			if ( pRow->m_bIsHidden || pRow->m_bIsExtrinsicBar )
				continue;

			rText.top = pRow->m_yTop + g_ctm;
			rText.bottom = pRow->m_yBottom - g_cbm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			switch (m_pAttributes->GetAt(iCol)->GetType())
			{
				case CRowColumnAttributes::Sum:
					if(m_intrinsic)	pDoc->GetIntrinsicRowSum(pRow->m_idx,strValue);
					else pDoc->GetRowSum(pRow->m_idx,strValue);
					break;
				case CRowColumnAttributes::Minimum:
					if(m_intrinsic)	pDoc->GetIntrinsicRowMinimum(pRow->m_idx,strValue);
					else pDoc->GetRowMinimum(pRow->m_idx,strValue);
					break;
				case CRowColumnAttributes::Mean:
					if(m_intrinsic)	pDoc->GetIntrinsicRowMean(pRow->m_idx,strValue);
					else pDoc->GetRowMean(pRow->m_idx,strValue);
					break;
				case CRowColumnAttributes::Maximum:
					if(m_intrinsic)	pDoc->GetIntrinsicRowMaximum(pRow->m_idx,strValue);
					else pDoc->GetRowMaximum(pRow->m_idx,strValue);
					break;
				case CRowColumnAttributes::CountPositive:
					if(m_intrinsic)	pDoc->GetIntrinsicRowCountPositives(pRow->m_idx,strValue);
					else pDoc->GetRowCountPositives(pRow->m_idx,strValue);
					break;
				case CRowColumnAttributes::CountMissing:
					if(m_intrinsic)	pDoc->GetIntrinsicRowCountMissingValues(pRow->m_idx,strValue);
					else pDoc->GetRowCountMissingValues(pRow->m_idx,strValue);
					break;
			}

			if ( strValue.IsEmpty() ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
				//increase the width & flag for redraw
				//redrawthis = true;
				m_pColumns->GetAt(iCol)->m_xRight = 
					m_pColumns->GetAt(iCol)->m_xLeft + 
					(pDC->GetTextExtent(strValue,strValue.GetLength())).cx + g_clm + g_crm;
				rText.left = m_pColumns->GetAt(iCol)->m_xLeft + g_clm;
				rText.right = m_pColumns->GetAt(iCol)->m_xRight - g_crm;			
				for(int pp = iCol+1;pp < m_pColumns->GetSize();pp++)
				{
					m_pColumns->GetAt(pp)->m_xRight = 
						m_pColumns->GetAt(pp)->Width() + 1 +
						m_pColumns->GetAt(pp-1)->m_xRight;
					m_pColumns->GetAt(pp)->m_xLeft =
						m_pColumns->GetAt(pp-1)->m_xRight + 1;
				}
				double d = (m_pColumns->GetAt(iCol)->m_xRight - 
					m_pColumns->GetAt(iCol)->m_xLeft - g_clm - g_crm) / (double) m_pTableView->m_nWidthUnit;
				m_pAttributes->GetAt(iCol)->SetWidth(d);
				
				
				CString strTemp = '#';
				CString strOut;
				while ( (pDC->GetTextExtent(strTemp,strTemp.GetLength())).cx <= rText.Width() )
				{
					strOut = strTemp;
					strTemp += '#';
				}
				strValue = strOut;
			}

			pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED,rText,
							strValue,strValue.GetLength(),NULL);
		}
	}

	//if(redrawthis)
	//	RedrawWindow();
	//pDC->SetBkMode(nOldBkMode);
}

void CRowTotalsView::DrawAlloc(CDC *pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = (long)m_pColumns->GetSize();
	long		nRowLimit = (long)m_pTableView->m_aRows.GetSize();
	long		iCol, iRow;
	CString	strValue;
	CRow* pRow;
	long nGroup;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	//nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	double dist;

	iCol = 0;
	for ( iCol = 0; iCol < nColLimit; iCol++ )
	{
		rText.left = m_pColumns->GetAt(iCol)->m_xLeft + g_clm;
		rText.right = m_pColumns->GetAt(iCol)->m_xRight - g_crm;

		if(iCol == 2 && pDoc->m_aRowGroupsDist.GetSize() && 
			pDoc->m_aRowGroupsDist[0] == pDoc->GetMissingValue())
		{
			//continue;
			}

		for ( iRow = 0; iRow < nRowLimit; iRow++ )
		{
			pRow = m_pTableView->m_aRows[iRow];

			if ( pRow->m_bIsHidden || pRow->m_bIsExtrinsicBar )
				continue;

			rText.top = pRow->m_yTop + g_ctm;
			rText.bottom = pRow->m_yBottom - g_cbm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			if(iCol == 0)
			{
				pDoc->GetRowGroups(pRow->m_idx,&nGroup);
			}
			else if(iCol == 1)
			{
				pDoc->m_AprioriGroups;
				if(pDoc->m_AprioriGroups.GetSize() <= pRow->m_idx)
					nGroup = -1;
				else
					nGroup = pDoc->m_AprioriGroups[pRow->m_idx];
				if(nGroup == 0)
					nGroup = -1;
			}
			else if(iCol == 2)
			{
				if(pDoc->m_aRowGroupsDist.GetSize() <= pRow->m_idx)
					dist = pDoc->GetMissingValue();
				else
					dist = pDoc->m_aRowGroupsDist[pRow->m_idx];
				
			}

			CString format;
			format.Format("%%.%df",pDoc->m_visible_decimals);

			if ( (nGroup == -1 && iCol != 2) ||  (iCol == 2 && dist == pDoc->GetMissingValue())) // missing value
				strValue = "..";
			else if(iCol == 2)
				strValue.Format(format,dist);
			else 
				strValue.Format("%d",nGroup);

			pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED,rText,
							strValue,strValue.GetLength(),NULL);
		}
	}

	//pDC->SetBkMode(nOldBkMode);
}

void CRowTotalsView::DrawOrd(CDC *pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = (long)m_pColumns->GetSize();
	long		nRowLimit = (long)m_pTableView->m_aRows.GetSize();
	long		iCol, iRow;
	CString	strValue;
	CString tmp;
	CRow* pRow;
	double dblValue;

	if(!(pDoc->m_Ord3d == 3)) nColLimit--;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	//nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	for ( iCol = 0; iCol < nColLimit; iCol++ )
	{
		rText.left = m_pColumns->GetAt(iCol)->m_xLeft + g_clm;
		rText.right = m_pColumns->GetAt(iCol)->m_xRight - g_crm;

		for ( iRow = 0; iRow < nRowLimit; iRow++ )
		{
			pRow = m_pTableView->m_aRows[iRow];

			if ( pRow->m_bIsHidden || pRow->m_bIsExtrinsicBar )
				continue;

			rText.top = pRow->m_yTop + g_ctm;
			rText.bottom = pRow->m_yBottom - g_cbm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			switch (m_pAttributes->GetAt(iCol)->GetType())
			{
				case CRowColumnAttributes::XCoord:
					dblValue = pDoc->m_aRowXCoords[pRow->m_idx];
					break;
				case CRowColumnAttributes::YCoord:
					dblValue = pDoc->m_aRowYCoords[pRow->m_idx];
					break;
				case CRowColumnAttributes::ZCoord:
					dblValue = pDoc->m_aRowZCoords[pRow->m_idx];
					break;
			}

			CString format;
			format.Format("%%.%df",pDoc->m_visible_decimals);

			strValue.Format(format,dblValue);
			if ( dblValue == ANALYSIS_VALUE ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
				//increase the width & flag for redraw
				//redrawthis = true;
				m_pColumns->GetAt(iCol)->m_xRight = 
					m_pColumns->GetAt(iCol)->m_xLeft + 
					(pDC->GetTextExtent(strValue,strValue.GetLength())).cx + g_clm + g_crm;
				rText.left = m_pColumns->GetAt(iCol)->m_xLeft + g_clm;
				rText.right = m_pColumns->GetAt(iCol)->m_xRight - g_crm;			
				for(int pp = iCol+1;pp < m_pColumns->GetSize();pp++)
				{
					m_pColumns->GetAt(pp)->m_xRight = 
						m_pColumns->GetAt(pp)->Width() + 1 +
						m_pColumns->GetAt(pp-1)->m_xRight;
					m_pColumns->GetAt(pp)->m_xLeft =
						m_pColumns->GetAt(pp-1)->m_xRight + 1;
				}
				double d = (m_pColumns->GetAt(iCol)->m_xRight - 
					m_pColumns->GetAt(iCol)->m_xLeft - g_clm - g_crm) / (double) m_pTableView->m_nWidthUnit;
				m_pAttributes->GetAt(iCol)->SetWidth(d);

				CString strTemp = '#';
				CString strOut;
				while ( (pDC->GetTextExtent(strTemp,strTemp.GetLength())).cx <= rText.Width() )
				{
					strOut = strTemp;
					strTemp += '#';
				}
				strValue = strOut;
			}

			pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED,rText,
							strValue,strValue.GetLength(),NULL);
		}
	}

	//pDC->SetBkMode(nOldBkMode);
	
}


/////////////////////////////////////////////////////////////////////////////
// CRowTotalsView diagnostics

#ifdef _DEBUG
void CRowTotalsView::AssertValid() const
{
	CView::AssertValid();
}

void CRowTotalsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPatnDoc* CRowTotalsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRowTotalsView message handlers

BOOL CRowTotalsView::SetPointerToTableView(CTableView* pTableView)
{
	ASSERT(pTableView->IsKindOf(RUNTIME_CLASS(CTableView)));
	m_pTableView = pTableView;
	return TRUE;
}

int CRowTotalsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	BeginWaitCursor();

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	m_Tabs.Create(this,CTabs::tabVert);
	m_Tabs.AddTab("Stats");
	m_Tabs.AddTab("Intrinsic Stats");
	m_Tabs.AddTab("Groups");
	m_Tabs.AddTab("Ordination");
	m_intrinsic = false;

	EndWaitCursor();
	
	return 0;
}

void CRowTotalsView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_cxClient = cx;
	m_cyClient = cy;
}

void CRowTotalsView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long idxOldActiveTab, idxNewActiveTab;
	long i;

	// clicked on tab?
	idxOldActiveTab = m_Tabs.GetIdxActiveTab();
	if ( m_Tabs.ClickedOnTab(point) )
	{
		idxNewActiveTab = m_Tabs.GetIdxActiveTab();
		if ( idxNewActiveTab != idxOldActiveTab ) // clicked on a different tab
		{
			switch(idxNewActiveTab)
			{
				case tabTotals:
					m_intrinsic = false;
					m_pColumns = &m_aRowTotalsColumns;
					m_pAttributes = &(pDoc->m_aRowTotalsColumnAttributes);
					break;
				case tabIntrinsicTotals:
					m_intrinsic = true;
					m_pColumns = &m_aRowTotalsColumns;
					m_pAttributes = &(pDoc->m_aRowTotalsColumnAttributes);				
					break;
				case tabAlloc:
					m_pColumns = &m_aRowAllocColumns;
					m_pAttributes = &(pDoc->m_aRowAllocColumnAttributes);
					break;

				case tabOrd:
					m_pColumns = &m_aRowOrdColumns;
					m_pAttributes = &(pDoc->m_aRowOrdColumnAttributes);
					break;
			}
			Invalidate(TRUE);
		}

		return;
	}
	
	// not on tab bar
	m_nMouseAction = none;
	SetCapture();       		// Capture the mouse until button up

	if ( m_rBtnBar.PtInRect(point) )
	{
		i = GetColumn(point);

		long cx = m_pColumns->GetAt(i)->Width() >> 2; // divide by 4
		cx = min(cx,6);

		// resizing column?
		if ( (m_pColumns->GetAt(i)->m_xRight - point.x) < cx )
		{
			m_nMouseAction = resizeColumn;
		}
		else if ( i > 0 && (point.x - m_pColumns->GetAt(i)->m_xLeft + 1) < cx )
		{
			i--; // sizing column on left
			m_nMouseAction = resizeColumn;
		}

		if ( m_nMouseAction == resizeColumn )
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
			m_xStart = m_xLine = point.x;
			DrawResizeColumnLine(m_xLine);
			m_iSelectedColumn = i;
		}
		else
		{
			SelectColumn(i);
			DrawMoveColumnLine(m_rSelectedColumn.left);
			SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_EW));
			m_nMouseAction = dragColumn;
		}
	}
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));

	CView::OnLButtonDown(nFlags, point);
}

void CRowTotalsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	long i;
	long x;

	// left mouse button is down
	if (GetCapture() == this)	// Left mouse button is down
    {
    	switch (m_nMouseAction)
    	{
    		case resizeColumn:
				if ( point.x < 0 || point.x > (m_cxClient - m_Tabs.GetTabWidth()) )
				{
    				ClearResizeColumnLine();
					m_xLine = -1;
					SetCursor(LoadCursor(NULL,IDC_NO));
					break;
				}

				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
				ClearResizeColumnLine();
				DrawResizeColumnLine(point.x);
				break;

    		case dragColumn:
				if ( point.x < 0 || point.x > (m_cxClient - m_Tabs.GetTabWidth()) )
				{
    				ClearMoveColumnLine();
					m_xLine = -1;
					SetCursor(LoadCursor(NULL,IDC_NO));
					break;
				}

				SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_EW));

				if ( (i = GetColumn(point)) != -1 )
				{
					x = m_pColumns->GetAt(i)->m_xLeft;
				}
				else
				{
 					x = m_pColumns->GetAt(m_pColumns->GetUpperBound())->m_xRight;
				}

    			if ( x != m_xLine )
    			{
    				ClearMoveColumnLine();
    				DrawMoveColumnLine(x);
    			}
				break;
		}
	}
	else if ( point.x >= m_cxClient - m_Tabs.GetTabWidth() ) // over tab bar
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	else if ( m_rBtnBar.PtInRect(point) )
	{
		i = GetColumn(point);

		long cx = m_pColumns->GetAt(i)->Width() >> 2; // divide by 4
		cx = min(cx,6);

		if ( (m_pColumns->GetAt(i)->m_xRight - point.x) < cx )
			SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
		else if ( i > 0 && (point.x - m_pColumns->GetAt(i)->m_xLeft + 1) < cx )
		{
			i--; // sizing column on left
			SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
		}
		else
			SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	
	CView::OnMouseMove(nFlags, point);
}

void CRowTotalsView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double	nWidth;		// new column width in pixels
	CRect	rInvalid(0,0,0,0);	// the rect that needs to be repainted
	long i;
	long iTo;
	CColumnAttributes *pTemp, *pAttrib;
	LPARAM lHint = 0;
	CUpdateHint UpdateHint;

	if (GetCapture() == this)	// Left mouse button is down
    {
    	switch (m_nMouseAction)
    	{
    		case resizeColumn:
    			ClearResizeColumnLine();

				if ( point.x < 0 || point.x > (m_cxClient - m_Tabs.GetTabWidth()) )
				{
					m_xStart = m_xLine = -1;
					break;
				}

				// set the rect to be invalidated
    			rInvalid.left = m_pColumns->GetAt(m_iSelectedColumn)->m_xLeft;
    			rInvalid.top = 0;
    			rInvalid.right = m_cxClient - m_Tabs.GetTabWidth();
    			rInvalid.bottom = m_cyClient;
    			
				// first, calc new width in pixels ...
				nWidth = m_pColumns->GetAt(m_iSelectedColumn)->Width() + (point.x - m_xStart);
				// ... then convert to doc units
				nWidth = m_pTableView->CalcDocColumnWidth((long)nWidth);

				if ( nWidth <= 0.0 )
					nWidth = 0.01; // keep width > 0.0
				m_pAttributes->GetAt(m_iSelectedColumn)->SetWidth(nWidth);

				// prepare update hint
				UpdateHint.m_idxOld = m_iSelectedColumn;
				switch ( m_Tabs.GetIdxActiveTab() )
				{
					case tabTotals:
						lHint = CUpdateHint::resizeRowTotalsColumn;
						break;

					case tabIntrinsicTotals:
						lHint = CUpdateHint::resizeIntrinsicRowTotalsColumn;
						break;

					case tabAlloc:
						lHint = CUpdateHint::resizeRowAllocColumn;
						break;

					case tabOrd:
						lHint = CUpdateHint::resizeRowOrdColumn;
						break;
				}

 				InvalidateRect(rInvalid);
				UpdateWindow();
				pDoc->UpdateAllViews(this,lHint,&UpdateHint);
				pDoc->SetModifiedFlag(TRUE);
				break;
     			
   		case dragColumn:
    			ClearMoveColumnLine();

				if ( point.x < 0 || point.x > (m_cxClient - m_Tabs.GetTabWidth()) )
				{
 					UnselectColumn();
					break;
				}

				if ( (iTo = GetColumn(point)) == -1 )
 					iTo = (long)m_pColumns->GetSize();

				// adjust order
				pTemp = m_pAttributes->GetAt(m_iSelectedColumn);
				if ( iTo > m_iSelectedColumn + 1)
				{
					for ( i = m_iSelectedColumn; i < (iTo - 1); i++ )
					{
						pAttrib = m_pAttributes->GetAt(i + 1);
						m_pAttributes->SetAt(i, pAttrib);
					}
					m_pAttributes->SetAt(iTo - 1, pTemp);
    				rInvalid.left = m_pColumns->GetAt(m_iSelectedColumn)->m_xLeft;
					UpdateHint.m_idxOld = m_iSelectedColumn;
				}
				else if ( iTo < m_iSelectedColumn )
				{
					for ( i = m_iSelectedColumn; i > iTo; i-- )
					{
						pAttrib = m_pAttributes->GetAt(i - 1);
						m_pAttributes->SetAt(i, pAttrib);
					}
					m_pAttributes->SetAt(iTo, pTemp);
    				rInvalid.left = m_pColumns->GetAt(iTo)->m_xLeft;
					UpdateHint.m_idxOld = iTo;
				}
				else
				{
 					UnselectColumn();
					break;
				}

				// set the rect to be invalidated
    			rInvalid.top = 0;
    			rInvalid.right = m_cxClient - m_Tabs.GetTabWidth();
    			rInvalid.bottom = m_cyClient;
    			
				// prepare update hint
				switch ( m_Tabs.GetIdxActiveTab() )
				{
					case tabTotals:
						lHint = CUpdateHint::changeRowTotalsOrder;
						break;

					case tabIntrinsicTotals:
						lHint = CUpdateHint::changeIntrinsicRowTotalsOrder;
						break;

					case tabAlloc:
						lHint = CUpdateHint::changeRowAllocOrder;
						break;

					case tabOrd:
						lHint = CUpdateHint::changeRowOrdOrder;
						break;
				}

				UnselectColumn();
 				InvalidateRect(rInvalid);
				UpdateWindow();
				pDoc->UpdateAllViews(this,lHint,&UpdateHint);
				pDoc->SetModifiedFlag(TRUE);
				break;
		}


		m_nMouseAction = none;	// Whatever mouse action was in progress is now finished
		m_xStart = m_xLine = -1;
		ReleaseCapture();   	// Release the mouse capture established at OnLButtonDown()
	}

	CView::OnLButtonUp(nFlags, point);

	OnMouseMove(nFlags,point); // so that cursor is set to correct type for point

	m_pTableView->SetFocus();
}

void CRowTotalsView::DrawMoveColumnLine(long x)
{
	CClientDC dc(this);
	CBrush* pOldBrush;
	CBrush brush;
	CBitmap bitmap;
	long aPattern[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
	
	if ( ! bitmap.CreateBitmap(8,8,1,1,aPattern) )
		return;
	if ( ! brush.CreatePatternBrush(&bitmap) )
		return;
	if ( (pOldBrush = dc.SelectObject(&brush)) == NULL )
		return;
	
	dc.PatBlt(x-1,0,3,m_cyClient,PATINVERT);
	m_xLine = x;

	dc.SelectObject(pOldBrush);
	bitmap.DeleteObject();
}

void CRowTotalsView::ClearMoveColumnLine()
{
	if ( m_xLine >= 0 )
		DrawMoveColumnLine(m_xLine);
}

void CRowTotalsView::DrawResizeColumnLine(long x)
{
	CClientDC dc(this);
	CPen* pOldPen;
	CPen pen;
	long nOldROP2;
	
	if ( ! pen.CreatePen(PS_DOT,0,RGB(0,0,0)) )
		return;
	if ( (pOldPen = dc.SelectObject(&pen)) == NULL )
		return;
	
	nOldROP2 = dc.SetROP2(R2_NOT);
	//nOldBkMode = dc.SetBkMode(TRANSPARENT);
	
	dc.MoveTo(x,0);
	dc.LineTo(x,m_cyClient);
	m_xLine = x;

	dc.SetROP2(nOldROP2);
	//dc.SetBkMode(nOldBkMode);	
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CRowTotalsView::ClearResizeColumnLine()
{
	if ( m_xLine >= 0 )
		DrawResizeColumnLine(m_xLine);
}

void CRowTotalsView::SelectColumn(long i)
{
	CClientDC dc(this);

	m_iSelectedColumn = i;
	m_rSelectedColumn.SetRect(m_pColumns->GetAt(i)->m_xLeft,0,
						   m_pColumns->GetAt(i)->m_xRight + 1,m_cyClient);
	dc.InvertRect(m_rSelectedColumn);
}

void CRowTotalsView::UnselectColumn()
{
	CClientDC dc(this);

	if ( !m_rSelectedColumn.IsRectEmpty() )
		dc.InvertRect(m_rSelectedColumn);

	m_iSelectedColumn = -1;
	m_xLine = -1;
	m_rSelectedColumn.SetRectEmpty();
}

long CRowTotalsView::GetColumn(CPoint point)
{
	long i;

	for ( i = 0; i < m_pColumns->GetSize(); i++ )
	{
		if ( point.x >= m_pColumns->GetAt(i)->m_xLeft &&
			 point.x <= m_pColumns->GetAt(i)->m_xRight )
		{
			return i;
		}
	}

	// point not on valid column
	return -1;
}

void CRowTotalsView::OnInitialUpdate() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CView::OnInitialUpdate();

	long i;

	m_aRowTotalsColumns.SetSize(6);
	for ( i = 0; i < m_aRowTotalsColumns.GetSize(); i++ )
		m_aRowTotalsColumns.SetAt(i, new CRowTotalsColumn);

	m_aRowAllocColumns.SetSize(3);
	for ( i = 0; i < m_aRowAllocColumns.GetSize(); i++ )
		m_aRowAllocColumns.SetAt(i, new CRowTotalsColumn);

	m_aRowOrdColumns.SetSize(3);
	for ( i = 0; i < m_aRowOrdColumns.GetSize(); i++ )
		m_aRowOrdColumns.SetAt(i, new CRowTotalsColumn);

	switch ( m_Tabs.GetIdxActiveTab() )
	{
		case tabTotals:
			m_intrinsic = false;
			m_pColumns = &m_aRowTotalsColumns;
			m_pAttributes = &(pDoc->m_aRowTotalsColumnAttributes);
			break;

		case tabIntrinsicTotals:
			m_intrinsic = true;
			m_pColumns = &m_aRowTotalsColumns;
			m_pAttributes = &(pDoc->m_aRowTotalsColumnAttributes);
			break;

		case tabAlloc:
			m_pColumns = &m_aRowAllocColumns;
			m_pAttributes = &(pDoc->m_aRowAllocColumnAttributes);
			break;

		case tabOrd:
			m_pColumns = &m_aRowOrdColumns;
			m_pAttributes = &(pDoc->m_aRowOrdColumnAttributes);
			break;
	}
}

void CRowTotalsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CRect	rInvalid(0,0,0,0);
	long		i, j;
	long		idx;
	CUpdateHint* pH;
	CRowSelection* pRS;

	// if can't use hint then repaint all
	if ( pHint == NULL || !pHint->IsKindOf(RUNTIME_CLASS(CUpdateHint)) )
	{
		Invalidate();
		UpdateWindow();
		return;
	}

	pH = (CUpdateHint*)pHint;

	switch ( lHint )
	{
		case CUpdateHint::editCell:
			if ( (i = m_pTableView->GetRow(pH->m_Cell.m_idxRow)) != -1 ) // in view?
			{
				rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
				rInvalid.bottom = m_pTableView->m_aRows[i]->m_yBottom;
			}
			break;

		case CUpdateHint::resizeRow:
			// resizing of rows is not supported by views in this version
			break;

		case CUpdateHint::hideRow:
		case CUpdateHint::unhideRow:
			// if the row is part of a selection, are any rows in view?
			if ( pH->m_pSelection != NULL )
			{
				pRS = (CRowSelection*)pH->m_pSelection;
				idx = pRS->GetFirst();
				while ( idx != -1 )
				{
					if ( (i = m_pTableView->GetRow(idx)) != -1 ) // in view?
					{
						if ( rInvalid.top == 0 ||
							 m_pTableView->m_aRows[i]->m_yTop < rInvalid.top )
							rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
							rInvalid.bottom = m_cyClient;
					}
					idx = pRS->GetNext();
				}
			}
			else // not part of selection, so just check if affected row is in view
			{
				if ( (i = m_pTableView->GetRow(pH->m_idxOld)) != -1 ) // in view?
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					rInvalid.bottom = m_cyClient;
				}
			}
			break;

		case CUpdateHint::moveRow:
			// check whether any part of 'from' block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetRow(idx)) != -1 )
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					rInvalid.bottom = m_cyClient;
					break;
				}
			}

			// check whether any part of destination is in view
			for ( idx = pH->m_idxNew; idx <= (pH->m_idxNew + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetRow(idx)) != -1 )
				{
					if ( rInvalid.top == 0 || m_pTableView->m_aRows[i]->m_yTop < rInvalid.top )
					{
						rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
						rInvalid.bottom = m_cyClient;
						break;
					}
				}
			}
			break;

		case CUpdateHint::makeRowExtrinsic:
			// first, check whether bottom edge of table is in view
			//(that's where the rows move) ...
			if(pH->m_pItems == NULL) break;

			// check whether any part of 'from' block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetRow(idx)) != -1 )
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					rInvalid.bottom = m_cyClient;
					break;
				}
			}

			// check whether any part of destination is in view
			for ( idx = pH->m_idxNew; idx <= (pH->m_idxNew + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetRow(idx)) != -1 )
				{
					if ( rInvalid.top == 0 || m_pTableView->m_aRows[i]->m_yTop < rInvalid.top )
					{
						rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
						rInvalid.bottom = m_cyClient;
						break;
					}
				}
			}
			break;

		case CUpdateHint::makeRowIntrinsic:
			if(pH->m_pItems == NULL) break;

			// first check whether any of the rows to be made intrinsic are in view ...
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = m_pTableView->GetRow(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					break;
				}
			}

			// ... then check whether previous extrinsic row is in view
			// (that's where the rows move) ...
			if ( (i = m_pTableView->GetRow(pH->m_idxOld)) != -1 )
			{
				if ( rInvalid.top == 0 )
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
				else
					rInvalid.top = min(rInvalid.top, m_pTableView->m_aRows[i]->m_yTop);
			}
			break;

		case CUpdateHint::insertRow:
			// check whether any part of block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetRow(idx)) != -1 )
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					rInvalid.bottom = m_cyClient;
					break;
				}
			}
			break;

		case CUpdateHint::deleteRow:
			// iterate through selection, determining rect to repaint
			for ( j = 0; j < pH->m_pItems->GetSize(); j++ )
			{
				if ( (i = m_pTableView->GetRow(pH->m_pItems->GetAt(j))) != -1 )
				{
					rInvalid.top = m_pTableView->m_aRows[i]->m_yTop;
					rInvalid.bottom = m_cyClient;
					break;
				}
			}
			break;

		case CUpdateHint::addRow:
			if ( m_pTableView->m_yGrid < m_cyClient )
			{
				rInvalid.top = m_pTableView->m_yGrid;
				rInvalid.bottom = m_cyClient;
			}
			break;

		// none of the following change data, so nothing needs to be repainted
		case CUpdateHint::moveColumn:
		case CUpdateHint::hideColumn:
		case CUpdateHint::unhideColumn:
		case CUpdateHint::resizeColumn:
		case CUpdateHint::changeColumnTotalsOrder:
		case CUpdateHint::changeColumnAllocOrder:
		case CUpdateHint::makeColumnExtrinsic:
		case CUpdateHint::makeColumnIntrinsic:
			break;

		case CUpdateHint::setFont:
			Invalidate(TRUE);
			break;

		case CUpdateHint::resizeRowTotalsColumn:
		case CUpdateHint::changeRowTotalsOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabTotals )
			{
				rInvalid.left = m_pColumns->GetAt(pH->m_idxOld)->m_xLeft;
				rInvalid.top = 0;
				rInvalid.bottom = m_cyClient;
			}
			break;

		case CUpdateHint::resizeIntrinsicRowTotalsColumn:
		case CUpdateHint::changeIntrinsicRowTotalsOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabIntrinsicTotals )
			{
				rInvalid.left = m_pColumns->GetAt(pH->m_idxOld)->m_xLeft;
				rInvalid.top = 0;
				rInvalid.bottom = m_cyClient;
			}
			break;
		
		case CUpdateHint::resizeRowAllocColumn:
		case CUpdateHint::changeRowAllocOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabAlloc )
			{
				rInvalid.left = m_pColumns->GetAt(pH->m_idxOld)->m_xLeft;
				rInvalid.top = 0;
				rInvalid.bottom = m_cyClient;
			}
			break;

		case CUpdateHint::resizeRowOrdColumn:
		case CUpdateHint::changeRowOrdOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabOrd )
			{
				rInvalid.left = m_pColumns->GetAt(pH->m_idxOld)->m_xLeft;
				rInvalid.top = 0;
				rInvalid.bottom = m_cyClient;
			}
			break;

		case CUpdateHint::insertColumn:
		case CUpdateHint::deleteColumn:
		case CUpdateHint::addColumn:
			if ( m_Tabs.GetIdxActiveTab() == tabTotals )
				rInvalid.SetRect(0,m_rBtnBar.bottom + 1,
								 m_cxClient - m_Tabs.GetTabWidth(),m_cyClient);
			break;

		default:  // not a hint that we recognise
			Invalidate(TRUE);
			UpdateWindow();
			break;
	}

	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.right = m_cxClient - m_Tabs.GetTabWidth();
		InvalidateRect(rInvalid,TRUE);
	}
	RedrawWindow();
}

void CRowTotalsView::CalcLayout()
{
	long x = 0;
	long i;
	long nCount = (long)m_pColumns->GetSize();

	//test visible widths & alter accordingly


	for ( i = 0; i < nCount; i++ )
	{
		m_pColumns->GetAt(i)->m_xLeft = x;
		if(m_pAttributes->GetSize() > i && !m_pAttributes->GetAt(i)->IsHidden())
			x += (long)(m_pAttributes->GetAt(i)->GetWidth() * m_pTableView->m_nWidthUnit) + 
				   g_clm + g_crm;
		m_pColumns->GetAt(i)->m_xRight = x;
		x++;
	}

	m_rBtnBar.SetRect(0,0,min(x - 1,m_cxClient),m_pTableView->m_nColumnBtnBarHeight);
}

void CRowTotalsView::VScroll(int cyScroll)
{
	CRect	rScroll;
	CRect	rInvalid;
	
	if ( cyScroll == 0 )
		return;

	// The area to scroll is the client area to the right of button bar & above tabs
	rScroll.SetRect(0,m_rBtnBar.bottom + 1,
					m_cxClient - m_Tabs.GetTabWidth(),m_pTableView->m_yGrid - 1);
	ScrollWindow(0,cyScroll,rScroll,rScroll);

	// mark bottom of client area for repainting so as to cater for varying row heights
	// eg, new bottom row may extend further down than previous bottom row
	rInvalid.SetRect(0,min(m_pTableView->m_cyClient,m_pTableView->m_yGrid - 1),
					 m_cxClient - m_Tabs.GetTabWidth(),m_cyClient);
	InvalidateRect(rInvalid);

	// NOTE: UpdateWindow() cannot be called here because CTableView has not yet
	// called UpdateWindow() -> OnDraw() -> CalcLayout().  Therefore, UpdateWindow()
	// is called from CTableView::OnHScroll() after CTableView calls UpdateWindow()
}

void CRowTotalsView::Refresh()
{
	CRect r;
	// The area to refresh is the client area below button bar & left of tabs
	r.SetRect(0,m_rBtnBar.bottom + 1,m_cyClient - m_Tabs.GetTabWidth(),m_cyClient);
	InvalidateRect(r);

	// NOTE: UpdateWindow() cannot be called here because CTableView has not yet
	// called UpdateWindow() -> OnDraw() -> CalcLayout().  Therefore, UpdateWindow()
	// is called from CTableView::OnHScroll() after CTableView calls UpdateWindow()
}

void CRowTotalsView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	CPoint local = point;
	ScreenToClient(&local);

	if (menu.LoadMenu(IDR_TABLETABS)){
		CMenu* pPopup = menu.GetSubMenu(0);
	
		CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
		pPopup->CheckMenuItem(pDoc->m_visible_decimals,MF_BYPOSITION | MF_CHECKED);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y,AfxGetMainWnd());
	}	

}

void CRowTotalsView::OnVisibledecimals0() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 0;
	pDoc->UpdateAllViews(NULL);
	
}

void CRowTotalsView::OnVisibledecimals1() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 1;
	pDoc->UpdateAllViews(NULL);
}

void CRowTotalsView::OnVisibledecimals2() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 2;
	pDoc->UpdateAllViews(NULL);
}

void CRowTotalsView::OnVisibledecimals3() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 3;
	pDoc->UpdateAllViews(NULL);
}

void CRowTotalsView::OnVisibledecimals4() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 4;
	pDoc->UpdateAllViews(NULL);
}

void CRowTotalsView::OnVisibledecimals5() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 5;
	pDoc->UpdateAllViews(NULL);
}

void CRowTotalsView::OnVisibledecimals6() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 6;
	pDoc->UpdateAllViews(NULL);
	
}

void CRowTotalsView::OnVisibledecimals7() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_visible_decimals = 7;

	pDoc->UpdateAllViews(NULL);
	
}
