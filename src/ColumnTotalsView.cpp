// ColumnTotalsView.cpp : implementation file
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
// CColumnTotalsView

IMPLEMENT_DYNCREATE(CColumnTotalsView, CView)

CColumnTotalsView::CColumnTotalsView()
{
	m_pTableView = NULL;
	m_nMouseAction = none;
	m_yLine = -1;
	m_iSelectedRow = -1;
	m_rSelectedRow.SetRectEmpty();
	m_pRows = NULL;
}

CColumnTotalsView::~CColumnTotalsView()
{
	long i;

	for ( i = 0; i < m_aColumnTotalsRows.GetSize(); i++ )
		delete m_aColumnTotalsRows[i];

	for ( i = 0; i < m_aColumnAllocRows.GetSize(); i++ )
		delete m_aColumnAllocRows[i];

	for ( i = 0; i < m_aColumnCompareRows.GetSize(); i++)
		delete m_aColumnCompareRows[i];

	for ( i = 0; i < m_aColumnPcc.GetSize(); i++)
		delete m_aColumnPcc[i];


}

BEGIN_MESSAGE_MAP(CColumnTotalsView, CView)
	//{{AFX_MSG_MAP(CColumnTotalsView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID__VISIBLEDECIMALS_7, OnVisibledecimals7)
	ON_COMMAND(ID__VISIBLEDECIMALS_6, OnVisibledecimals6)
	ON_COMMAND(ID__VISIBLEDECIMALS_5, OnVisibledecimals5)
	ON_COMMAND(ID__VISIBLEDECIMALS_4, OnVisibledecimals4)
	ON_COMMAND(ID__VISIBLEDECIMALS_3, OnVisibledecimals3)
	ON_COMMAND(ID__VISIBLEDECIMALS_2, OnVisibledecimals2)
	ON_COMMAND(ID__VISIBLEDECIMALS_1, OnVisibledecimals1)
	ON_COMMAND(ID__VISIBLEDECIMALS_0, OnVisibledecimals0)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColumnTotalsView drawing

void CColumnTotalsView::OnDraw(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
		
	if(m_pRows == NULL)
		return;

	long		i;
	CString strLabel;
	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	if(m_Tabs.GetIdxActiveTab() == tabCompare && 
		pDoc->m_RowSelection->GetCount()){
		pDoc->GetRowLabel(pDoc->m_RowSelection->GetFirst(),strLabel);
		m_pAttributes->GetAt(0)->SetLabel(strLabel);
		if(pDoc->m_RowSelection->GetCount() > 1){
			pDoc->GetRowLabel(pDoc->m_RowSelection->GetNext(),strLabel);
			m_pAttributes->GetAt(1)->SetLabel(strLabel);
		}
	}else if(m_Tabs.GetIdxActiveTab() == tabCompare){
		m_pAttributes->GetAt(0)->SetLabel("none");
		m_pAttributes->GetAt(1)->SetLabel("none");
	}

	CalcLayout();
	m_Tabs.DrawTabs(pDC);

	CRect rect;
	GetClientRect(rect);
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,150));
	pDC->FillRect(rect,&brush);
	pDC->SetBkColor(RGB(255,255,150));

	pDC->SetTextColor(pDoc->m_rgbFontCells);

	CFont * oldFont;
	// select font into memory DC
	oldFont = pDC->SelectObject(&(m_pTableView->m_fontCells));
	if ( oldFont == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for column totals");

	}

	DrawButtonBar(pDC);
	
	// extrinsic bar
	if ( pDoc->IsExtrinsicColumns() &&
		(i = m_pTableView->GetColumn(pDoc->GetIdxLeftExtrinsicColumn())) != -1 )
	{
		pDC->SelectObject(&m_pTableView->m_brushXBar);
		pDC->PatBlt(m_pTableView->m_aColumns[i]->m_xLeft,0,
			m_pTableView->m_aColumns[i]->Width(),m_cyClient,PATCOPY);
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

		case tabCompare:
			DrawCompare(pDC);
			break;

		case tabPcc:
			DrawPcc(pDC);
			break;
	}

	pDC->SelectObject(oldFont);

	IN_OnDraw = FALSE;
}

void CColumnTotalsView::DrawButtonBar(CDC *pDC)
{
	CPatnDoc * pDoc = (CPatnDoc*) GetDocument();
	CBrush*	pOldBrush;
	long		nOldBkMode, nOldBkColor;
	CRect	rText;
	long		nCount = m_pAttributes->GetSize();
	long		y;
	long		x;
	long		i;
	CSize	size;
	CString	str;
	CString strEllipsis = "...";
	long		cxEllipsis = (pDC->GetTextExtent(strEllipsis)).cx;

	nOldBkColor = pDC->SetBkColor(RGB(192,192,192));

	// buttons
	y = m_pRows->GetAt(m_pRows->GetUpperBound())->m_yBottom + 1;
	pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(0,0,m_pTableView->m_nRowBtnBarWidth,y,PATCOPY);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,1,y,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(m_pTableView->m_nRowBtnBarWidth,0,1,y,PATCOPY);

	for ( i = 0; i < nCount; i++ )
	{
		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->PatBlt(0,m_pRows->GetAt(i)->m_yTop,
					m_pTableView->m_nRowBtnBarWidth,1,PATCOPY);

		pDC->SelectStockObject(GRAY_BRUSH);
		pDC->PatBlt(0,m_pRows->GetAt(i)->m_yBottom,
					m_pTableView->m_nRowBtnBarWidth,1,PATCOPY);
	}

	// button labels
	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	rText.left = g_clm;
	rText.right = m_pTableView->m_nRowBtnBarWidth - g_crm;
    for ( i = 0; i < nCount; i++ )
    {
		rText.top = m_pRows->GetAt(i)->m_yTop + g_ctm;
		rText.bottom = m_pRows->GetAt(i)->m_yBottom - g_cbm;

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
			}
			pDC->ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,
							rText,str,str.GetLength(),NULL);
		}
	}

	pDC->SelectObject(pOldBrush);
	pDC->SetBkMode(nOldBkMode);
	pDC->SetBkColor(nOldBkColor);
}

void CColumnTotalsView::DrawTotals(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		nOldBkMode;
	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit;// = m_pTableView->m_aColumns.GetSize();
	long		nRowLimit = m_pRows->GetSize();
	long		iCol, iRow;
	CString	strValue;
	CColumn* pCol;

	if(pDoc->GetIdxLeftExtrinsicColumn() == -1 || !m_intrinsic) nColLimit = pDoc->GetNoOfColumns();
	else nColLimit = pDoc->GetIdxLeftExtrinsicColumn();

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	for ( iRow = 0; iRow < nRowLimit; iRow++ )
	{
		rText.top = m_pRows->GetAt(iRow)->m_yTop + g_ctm;
		rText.bottom = m_pRows->GetAt(iRow)->m_yBottom - g_cbm;

		for ( iCol = 0; iCol < m_pTableView->m_aColumns.GetSize() &&
			m_pTableView->m_aColumns[iCol]->m_idx < nColLimit; iCol++ )
		{
			pCol = m_pTableView->m_aColumns[iCol];

			if ( pCol->m_bIsHidden || pCol->m_bIsExtrinsicBar )
				continue;

			rText.left = pCol->m_xLeft + g_clm;
			rText.right = pCol->m_xRight - g_crm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			switch (m_pAttributes->GetAt(iRow)->GetType())
			{
				case CRowColumnAttributes::Sum:
					if(m_intrinsic)	pDoc->GetIntrinsicColumnSum(pCol->m_idx,strValue);
					else pDoc->GetColumnSum(pCol->m_idx,strValue);
					break;
				case CRowColumnAttributes::Minimum:
					if(m_intrinsic) pDoc->GetIntrinsicColumnMinimum(pCol->m_idx,strValue);
					else pDoc->GetColumnMinimum(pCol->m_idx,strValue);
					break;
				case CRowColumnAttributes::Mean:
					if(m_intrinsic) pDoc->GetIntrinsicColumnMean(pCol->m_idx,strValue);
					else pDoc->GetColumnMean(pCol->m_idx,strValue);
					break;
				case CRowColumnAttributes::Maximum:
					if(m_intrinsic) pDoc->GetIntrinsicColumnMaximum(pCol->m_idx,strValue);
					else pDoc->GetColumnMaximum(pCol->m_idx,strValue);
					break;
				case CRowColumnAttributes::CountPositive:
					if(m_intrinsic)	pDoc->GetIntrinsicColumnCountPositives(pCol->m_idx,strValue);
					else pDoc->GetColumnCountPositives(pCol->m_idx,strValue);
					break;
				case CRowColumnAttributes::CountMissing:
					if(m_intrinsic) pDoc->GetIntrinsicColumnCountMissingValues(pCol->m_idx,strValue);
					else pDoc->GetColumnCountMissingValues(pCol->m_idx,strValue);
					break;
			}

			if ( strValue.IsEmpty() ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
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

	pDC->SetBkMode(nOldBkMode);
}

void CColumnTotalsView::DrawAlloc(CDC *pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		nOldBkMode;
	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = m_pTableView->m_aColumns.GetSize();
	long		nRowLimit = m_pRows->GetSize();
	long		iCol, iRow;
	CString	strValue;
	CColumn* pCol;
	long nGroup;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	iRow = 0;
	rText.top = m_pRows->GetAt(iRow)->m_yTop + g_ctm;
	rText.bottom = m_pRows->GetAt(iRow)->m_yBottom - g_cbm;

	for ( iCol = 0; iCol < nColLimit; iCol++ )
	{
		pCol = m_pTableView->m_aColumns[iCol];

		if ( pCol->m_bIsHidden || pCol->m_bIsExtrinsicBar )
			continue;

		rText.left = pCol->m_xLeft + g_clm;
		rText.right = pCol->m_xRight - g_crm;

		if ( ! r.IntersectRect(rText,rClip) )
			continue;

		pDoc->GetColumnGroups(pCol->m_idx,&nGroup);

		if ( nGroup == -1 ) // missing value
			strValue = "..";
		else
			strValue.Format("%d",nGroup);
		
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED,rText,
						strValue,strValue.GetLength(),NULL);
	}

	pDC->SetBkMode(nOldBkMode);

}

void CColumnTotalsView::DrawCompare(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		nOldBkMode;
	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = m_pTableView->m_aColumns.GetSize();
	long		nRowLimit = m_pRows->GetSize();
	long		iCol, iRow;
	CString	strValue;
	CColumn* pCol;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	for ( iRow = 0; iRow < nRowLimit; iRow++ )
	{
		rText.top = m_pRows->GetAt(iRow)->m_yTop + g_ctm;
		rText.bottom = m_pRows->GetAt(iRow)->m_yBottom - g_cbm;

		for ( iCol = 0; iCol < nColLimit; iCol++ )
		{
			pCol = m_pTableView->m_aColumns[iCol];

			if ( pCol->m_bIsHidden || pCol->m_bIsExtrinsicBar )
				continue;

			rText.left = pCol->m_xLeft + g_clm;
			rText.right = pCol->m_xRight - g_crm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			if(pDoc->m_RowSelection->GetCount() != 2 &&
				pDoc->m_RowSelection->GetCount() != 1){
				strValue = "..";
			}else{
				CString format;
				long dec = pDoc->m_aColumnAttributes[pCol->m_idx]->GetDecimalPrecision();
				format.Format("%%.%df",dec);
				switch (m_pAttributes->GetAt(iRow)->GetType())
				{
					case 0:
						strValue.Format(format,pDoc->GetValueAt(pCol->m_idx,
							pDoc->m_RowSelection->GetFirst()));
						break;
					case 1:
						if(pDoc->m_RowSelection->GetCount() == 1){
							strValue = "..";
						}else{
							pDoc->m_RowSelection->GetFirst();
							strValue.Format(format,pDoc->GetValueAt(pCol->m_idx,
							pDoc->m_RowSelection->GetNext()));
						}						
						break;
					case 2:
						if(pDoc->m_RowSelection->GetCount() == 1) {
							strValue = "..";
						}else{
							strValue.Format(format,pDoc->GetValueAt(pCol->m_idx,pDoc->m_RowSelection->GetFirst()) - 
							pDoc->GetValueAt(pCol->m_idx,pDoc->m_RowSelection->GetNext()));
						}
						
						break;
				}
			}

			if ( strValue.IsEmpty() ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
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

	pDC->SetBkMode(nOldBkMode);
}

void CColumnTotalsView::DrawPcc(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		nOldBkMode;
	CRect	rText;
 	CRect	r, rClip;
	long		nColLimit = m_pTableView->m_aColumns.GetSize();
	long		nRowLimit = m_pAttributes->GetSize();
	long		iCol, iRow;
	CString	strValue;
	CColumn* pCol;
	static bool error = false;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	for ( iRow = 0; iRow < nRowLimit; iRow++ )
	{
		rText.top = m_pRows->GetAt(iRow)->m_yTop + g_ctm;
		rText.bottom = m_pRows->GetAt(iRow)->m_yBottom - g_cbm;

		for ( iCol = 0; iCol < nColLimit; iCol++ )
		{
			pCol = m_pTableView->m_aColumns[iCol];

			if ( pCol->m_bIsHidden || pCol->m_bIsExtrinsicBar )
				continue;

			rText.left = pCol->m_xLeft + g_clm;
			rText.right = pCol->m_xRight - g_crm;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			if(pDoc->m_Ord3d == 1 && iRow == 1)
			{
				strValue = "";
			}
			else if(pDoc->m_Ord3d == 1 && iRow == 2)
			{
				CString format;

				format.Format("%%.%df",pDoc->m_visible_decimals);

				if(pDoc->m_Vectors[pCol->m_idx]->GetAt(0) == ANALYSIS_VALUE){
					strValue = "...";
				}else if(m_pAttributes->GetAt(iRow)->GetType() != 2 || pDoc->m_Ord3d == 3){
					strValue.Format(format,pDoc->m_Vectors[pCol->m_idx]->GetAt(iRow-1));
				}else{
					//if(!error){
					//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error due to removal of an m_pAttributes value.\nThis message appears only once.\nWhen this error occurs data is filled with 'error3'.",
					//		MB_OK | MB_ICONSTOP, 0);
					//	error = true;
					//}
					//strValue = "error3";
					strValue = "";
				}
			}
			else if(pDoc->m_Vectors.GetSize() <= pCol->m_idx){
				//report error due to construction of m_Vectors in data table
				//if(!error){
				//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error due to construction of m_Vectors in data table.\nThis message appears only once.\nWhen this error occurs, data is filled with 'error1'.",
				//		MB_OK | MB_ICONSTOP, 0);
				//	error = true;
				//}
				//strValue = "error1";
				strValue = "";
			}else if(pDoc->m_Vectors[pCol->m_idx]->GetSize() <= iRow){
				//report error due to 2d & 3d data conversion - (recorded instance when 2d ord redone for 3d... vectors reset, not resized)
				//if(!error){
				//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error due to 2d to/from 3d data conversion.\nThis message appears only once.\nWhen this error occurs data is filled with 'error2'.",
				//		MB_OK | MB_ICONSTOP, 0);
				//	error = true;
				//}
				//strValue = "error2";
				strValue = "";
			}else{
				CString format;

				format.Format("%%.%df",pDoc->m_visible_decimals);

				if(pDoc->m_Vectors[pCol->m_idx]->GetAt(0) == ANALYSIS_VALUE){
					strValue = "...";
				}else if(m_pAttributes->GetAt(iRow)->GetType() != 2 || pDoc->m_Ord3d == 3){
					strValue.Format(format,pDoc->m_Vectors[pCol->m_idx]->GetAt(iRow));
				}else{
					//if(!error){
					//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error due to removal of an m_pAttributes value.\nThis message appears only once.\nWhen this error occurs data is filled with 'error3'.",
					//		MB_OK | MB_ICONSTOP, 0);
					//	error = true;
					//}
					//strValue = "error3";
					strValue = "";
				}
			}
			
			if ( strValue.IsEmpty() ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
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

	pDC->SetBkMode(nOldBkMode);
}

/////////////////////////////////////////////////////////////////////////////
// CColumnTotalsView diagnostics

#ifdef _DEBUG
void CColumnTotalsView::AssertValid() const
{
	CView::AssertValid();
}

void CColumnTotalsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPatnDoc* CColumnTotalsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColumnTotalsView message handlers

BOOL CColumnTotalsView::SetPointerToTableView(CTableView* pTableView)
{
	ASSERT(pTableView->IsKindOf(RUNTIME_CLASS(CTableView)));
	m_pTableView = pTableView;
	return TRUE;
}

int CColumnTotalsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_intrinsic = false;

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	BeginWaitCursor();

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	m_Tabs.Create(this,CTabs::tabHorz);
	m_Tabs.AddTab("Stats");
	m_Tabs.AddTab("Intrinsic Stats");
	m_Tabs.AddTab("Groups");
	m_Tabs.AddTab("Compare Rows");
	m_Tabs.AddTab("PCC");

	EndWaitCursor();
	
	return 0;
}

void CColumnTotalsView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_cxClient = cx;
	m_cyClient = cy;
}

void CColumnTotalsView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long idxOldActiveTab, idxNewActiveTab;

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
					m_pRows = &m_aColumnTotalsRows;
					m_pAttributes = &(pDoc->m_aColumnTotalsRowAttributes);
					break;

				case tabIntrinsicTotals:
					m_intrinsic = true;
					m_pRows = &m_aColumnTotalsRows;
					m_pAttributes = &(pDoc->m_aColumnTotalsRowAttributes);
					break;

				case tabAlloc:
					m_pRows = &m_aColumnAllocRows;
					m_pAttributes = &(pDoc->m_aColumnAllocRowAttributes);
					break;

				case tabCompare:
					m_pRows = &m_aColumnCompareRows;
					m_pAttributes = &(pDoc->m_aColumnCompareRowAttributes);
					break;

				case tabPcc:
					m_pRows = &m_aColumnPcc;
					m_pAttributes = &(pDoc->m_aColumnPccAttributes);
					if(pDoc->m_Ord3d == 3 && m_pAttributes->GetSize() < 4){
						CRowAttributes * pRowAttributes = new CRowAttributes;
						pRowAttributes->SetLabel("Z");
						pRowAttributes->SetType(2);
						pRowAttributes->SetHeight(-1);
						m_pAttributes->InsertAt(2,pRowAttributes);
						
						m_pRows->InsertAt(2,new CColumnTotalsRow);
					}else if(!(pDoc->m_Ord3d == 3) && m_pAttributes->GetSize() > 3){
						//find 'Z'
						for(long i=0;i<m_pAttributes->GetSize();i++){
							if(m_pAttributes->GetAt(i)->GetType() == 2){
								delete m_pAttributes->GetAt(i);
								m_pAttributes->RemoveAt(i);
								delete m_pRows->GetAt(i);
								m_pRows->RemoveAt(i);
							}
						}			
					}
						
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
		m_nMouseAction = dragRow;
		SelectRow(GetRow(point));
		DrawMoveRowLine(m_rSelectedRow.top);
		SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_NS));
		return;
	}

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	CView::OnLButtonDown(nFlags, point);
}

void CColumnTotalsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	long i;
	long y;

	// left mouse button is down
	if (GetCapture() == this)	// Left mouse button is down
    {
    	switch (m_nMouseAction)
    	{
    		case resizeRow:
				// resizing of rows not supported
				break;

    		case dragRow:
				if ( point.y < 0 || point.y > (m_cyClient - m_Tabs.GetTabWidth()) )
				{
    				ClearMoveRowLine();
					m_yLine = -1;
					SetCursor(LoadCursor(NULL,IDC_NO));
					break;
				}

				SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_NS));

				if ( (i = GetRow(point)) != -1 )
				{
					y = m_pRows->GetAt(i)->m_yTop;
				}
				else
				{
 					y = m_pRows->GetAt(m_pRows->GetUpperBound())->m_yBottom;
				}

    			if ( y != m_yLine )
    			{
    				ClearMoveRowLine();
    				DrawMoveRowLine(y);
    			}
				break;
		}
	}
	else if ( point.y >= m_cyClient - m_Tabs.GetTabWidth() ) // over tab bar
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	
	CView::OnMouseMove(nFlags, point);
}

void CColumnTotalsView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rInvalid(0,0,0,0);	// the rect that needs to be repainted
	long i;
	long iTo;
	CRowAttributes *pTemp, *pAttrib;
	LPARAM lHint = 0;
	CUpdateHint UpdateHint;

	if (GetCapture() == this)	// Left mouse button is down
    {
    	switch (m_nMouseAction)
    	{
    		case resizeRow:
				// resizing of rows not supported
				break;

    		case dragRow:
    			ClearMoveRowLine();

				if ( point.y < 0 || point.y > (m_cyClient - m_Tabs.GetTabWidth()) )
				{
 					UnselectRow();
					break;
				}

				if ( (iTo = GetRow(point)) == -1 )
 					iTo = m_pRows->GetSize();

				// adjust order
				pTemp = m_pAttributes->GetAt(m_iSelectedRow);
				if ( iTo > m_iSelectedRow + 1)
				{
					for ( i = m_iSelectedRow; i < (iTo - 1); i++ )
					{
						pAttrib = m_pAttributes->GetAt(i + 1);
						m_pAttributes->SetAt(i, pAttrib);
					}
					m_pAttributes->SetAt(iTo - 1, pTemp);
    				rInvalid.top = m_pRows->GetAt(m_iSelectedRow)->m_yTop;
					UpdateHint.m_idxOld = m_iSelectedRow;
				}
				else if ( iTo < m_iSelectedRow )
				{
					for ( i = m_iSelectedRow; i > iTo; i-- )
					{
						pAttrib = m_pAttributes->GetAt(i - 1);
						m_pAttributes->SetAt(i, pAttrib);
					}
					m_pAttributes->SetAt(iTo, pTemp);
    				rInvalid.top = m_pRows->GetAt(iTo)->m_yTop;
					UpdateHint.m_idxOld = iTo;
				}
				else
				{
 					UnselectRow();
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
						lHint = CUpdateHint::changeColumnTotalsOrder;
						break;

					case tabIntrinsicTotals:
						lHint = CUpdateHint::changeColumnIntrinsicTotalsOrder;
						break;

					case tabAlloc:
						lHint = CUpdateHint::changeColumnAllocOrder;
						break;

					case tabCompare:
						lHint = CUpdateHint::changeColumnCompareOrder;
						break;
					
					case tabPcc:
						lHint = CUpdateHint::changeColumnPccOrder;
						break;
				}

 				UnselectRow();
 				InvalidateRect(rInvalid);
				UpdateWindow();
				pDoc->UpdateAllViews(this,lHint,&UpdateHint);
				pDoc->SetModifiedFlag(TRUE);
				break;
		}

		m_nMouseAction = none;	// Whatever mouse action was in progress is now finished
		m_yLine = -1;
		ReleaseCapture();   	// Release the mouse capture established at OnLButtonDown()
	}

	CView::OnLButtonUp(nFlags, point);

	OnMouseMove(nFlags,point); // so that cursor is set to correct type for point

	m_pTableView->SetFocus();
}

void CColumnTotalsView::DrawMoveRowLine(long y)
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
	
	dc.PatBlt(0,y-1,m_cxClient,3,PATINVERT);
	m_yLine = y;

	dc.SelectObject(pOldBrush);
	brush.DeleteObject();
}

void CColumnTotalsView::ClearMoveRowLine()
{
	if ( m_yLine >= 0 )
		DrawMoveRowLine(m_yLine);
}

void CColumnTotalsView::DrawResizeRowLine(long y)
{
	CClientDC dc(this);
	CPen* pOldPen;
	CPen pen;
	long nOldROP2;
	long nOldBkMode;
	
	if ( ! pen.CreatePen(PS_DOT,0,RGB(0,0,0)) )
		return;
	if ( (pOldPen = dc.SelectObject(&pen)) == NULL )
		return;
	
	nOldROP2 = dc.SetROP2(R2_NOT);
	nOldBkMode = dc.SetBkMode(TRANSPARENT);
	
	dc.MoveTo(0,y);
	dc.LineTo(m_cxClient,y);
	m_yLine = y;

	dc.SetROP2(nOldROP2);
	dc.SetBkMode(nOldBkMode);	
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CColumnTotalsView::ClearResizeRowLine()
{
	ASSERT(m_yLine >= 0);
	DrawResizeRowLine(m_yLine);
}

void CColumnTotalsView::SelectRow(long i)
{
	CClientDC dc(this);

	m_iSelectedRow = i;
	m_rSelectedRow.SetRect(0,m_pRows->GetAt(i)->m_yTop,
						   m_cxClient,m_pRows->GetAt(i)->m_yBottom + 1);
	dc.InvertRect(m_rSelectedRow);
}

void CColumnTotalsView::UnselectRow()
{
	CClientDC dc(this);

	if ( !m_rSelectedRow.IsRectEmpty() )
		dc.InvertRect(m_rSelectedRow);

	m_iSelectedRow = -1;
	m_yLine = -1;
	m_rSelectedRow.SetRectEmpty();
}

long CColumnTotalsView::GetRow(CPoint point)
{
	long i;

	for ( i = 0; i < m_pRows->GetSize(); i++ )
	{
		if ( point.y >= m_pRows->GetAt(i)->m_yTop &&
			 point.y <= m_pRows->GetAt(i)->m_yBottom )
		{
			return i;
		}
	}

	// point not on valid row
	return -1;
}

void CColumnTotalsView::OnInitialUpdate() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CView::OnInitialUpdate();

	long i;

	m_aColumnTotalsRows.SetSize(6);
	for ( i = 0; i < m_aColumnTotalsRows.GetSize(); i++ )
		m_aColumnTotalsRows.SetAt(i, new CColumnTotalsRow);

	m_aColumnAllocRows.SetSize(1);
	for ( i = 0; i < m_aColumnAllocRows.GetSize(); i++ )
		m_aColumnAllocRows.SetAt(i, new CColumnTotalsRow);

	m_aColumnCompareRows.SetSize(3);
	for ( i = 0; i < m_aColumnCompareRows.GetSize(); i++ )
		m_aColumnCompareRows.SetAt(i, new CColumnTotalsRow);

	m_aColumnPcc.SetSize(4);
	for ( i = 0; i < m_aColumnPcc.GetSize(); i++ )
		m_aColumnPcc.SetAt(i, new CColumnTotalsRow);

	switch ( m_Tabs.GetIdxActiveTab() )
	{
		case tabTotals:
		case tabIntrinsicTotals:
			m_pRows = &m_aColumnTotalsRows;
			m_pAttributes = &(pDoc->m_aColumnTotalsRowAttributes);
			break;

		case tabAlloc:
			m_pRows = &m_aColumnAllocRows;
			m_pAttributes = &(pDoc->m_aColumnAllocRowAttributes);
			break;

		case tabCompare:
			m_pRows = &m_aColumnCompareRows;
			m_pAttributes = &(pDoc->m_aColumnCompareRowAttributes);
			break;

		case tabPcc:
			m_pRows = &m_aColumnPcc;
			m_pAttributes = &(pDoc->m_aColumnPccAttributes);
			if(pDoc->m_Ord3d == 3 && m_pAttributes->GetSize() < 4){
				CRowAttributes * pRowAttributes = new CRowAttributes;
				pRowAttributes->SetLabel("Z");
				pRowAttributes->SetType(2);
				pRowAttributes->SetHeight(-1);
				m_pAttributes->InsertAt(2,pRowAttributes);

				m_pRows->InsertAt(2,new CColumnTotalsRow);
			}else if(!(pDoc->m_Ord3d == 3) && m_pAttributes->GetSize() > 3){
				//find 'Z'
				for(long i=0;i<m_pAttributes->GetSize();i++){
					if(m_pAttributes->GetAt(i)->GetType() == 2){
						delete m_pAttributes->GetAt(i);
						m_pAttributes->RemoveAt(i);
						delete m_pRows->GetAt(i);
						m_pRows->RemoveAt(i);
					}
				}			
			}
			break;
	}
}

void CColumnTotalsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CRect	rInvalid(0,0,0,0);
	long		i, j;
	long		idx;
	CUpdateHint* pH;
	CColumnSelection* pCS;

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
			if ( (i = m_pTableView->GetColumn(pH->m_Cell.m_idxColumn)) != -1 ) // in view?
			{
				rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
				rInvalid.right = m_pTableView->m_aColumns[i]->m_xRight;
			}
			break;

		case CUpdateHint::resizeColumn:
		case CUpdateHint::hideColumn:
		case CUpdateHint::unhideColumn:
			// if the column is part of a selection, are any columns in view?
			if ( pH->m_pSelection != NULL )
			{
				pCS = (CColumnSelection*)pH->m_pSelection;
				idx = pCS->GetFirst();
				while ( idx != -1 )
				{
					if ( (i = m_pTableView->GetColumn(idx)) != -1 ) // in view?
					{
						if ( rInvalid.left == 0 ||
							 m_pTableView->m_aColumns[i]->m_xLeft < rInvalid.left )
						{
							rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
							rInvalid.right = m_cxClient;
						}
					}
					idx = pCS->GetNext();
				}
			}
			else // not part of selection, so just check if affected column is in view
			{
				if ( (i = m_pTableView->GetColumn(pH->m_idxOld)) != -1 ) // in view?
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					rInvalid.right = m_cxClient;
				}
			}
			break;

		case CUpdateHint::moveColumn:
			// check whether any part of 'from' block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetColumn(idx)) != -1 )
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					rInvalid.right = m_cxClient;
					break;
				}
			}

			// check whether any part of destination is in view
			for ( idx = pH->m_idxNew; idx <= (pH->m_idxNew + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetColumn(idx)) != -1 )
				{
					if ( rInvalid.left == 0 ||
						 m_pTableView->m_aColumns[i]->m_xLeft < rInvalid.left )
					{
						rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
						rInvalid.right = m_cxClient;
						break;
					}
				}
			}
			break;
 
		case CUpdateHint::makeColumnExtrinsic:
			// first, check whether right edge of table is in view
			// (that's where the columns move) ...
			if ( m_pTableView->m_xGrid < m_cxClient )
				rInvalid.left = m_pTableView->m_xGrid;
			
			if(pH->m_pItems == NULL) break;

			// ... then check whether any of the columns to be made extrinsic are in view
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = m_pTableView->GetColumn(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					break;
				}
			}
			break;

		case CUpdateHint::makeColumnIntrinsic:
			if(pH->m_pItems == NULL) break;
			
			// first check whether any of the columns to be made intrinsic are in view ...
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = m_pTableView->GetColumn(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					break;
				}
			}

			// ... then check whether previous extrinsic column is in view
			// (that's where the columns move) ...
			if ( (i = m_pTableView->GetColumn(pH->m_idxOld)) != -1 )
			{
				if ( rInvalid.left == 0 )
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
				else
					rInvalid.left = min(rInvalid.left, m_pTableView->m_aColumns[i]->m_xLeft);
			}
			break;

		case CUpdateHint::insertColumn:
			// check whether any part of block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = m_pTableView->GetColumn(idx)) != -1 )
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					rInvalid.right = m_cxClient;
					break;
				}
			}
			break;

		case CUpdateHint::deleteColumn:
			// iterate through selection, determining rect to repaint
			for ( j = 0; j < pH->m_pItems->GetSize(); j++ )
			{
				if ( (i = m_pTableView->GetColumn(pH->m_pItems->GetAt(j))) != -1 )
				{
					rInvalid.left = m_pTableView->m_aColumns[i]->m_xLeft;
					rInvalid.right = m_cxClient;
					break;
				}
			}
			break;

		case CUpdateHint::addColumn:
			if ( m_pTableView->m_xGrid < m_cxClient )
			{
				rInvalid.left = m_pTableView->m_xGrid;
				rInvalid.right = m_cxClient;
			}
			break;

		// none of the following change data, so nothing needs to be repainted
		case CUpdateHint::moveRow:
		case CUpdateHint::hideRow:
		case CUpdateHint::unhideRow:
		case CUpdateHint::resizeRow:
		case CUpdateHint::makeRowExtrinsic:
		case CUpdateHint::makeRowIntrinsic:
		case CUpdateHint::changeRowTotalsOrder:
		case CUpdateHint::changeRowAllocOrder:
		case CUpdateHint::resizeRowTotalsColumn:
		case CUpdateHint::resizeRowAllocColumn:
		case CUpdateHint::resizeRowOrdColumn:
		case CUpdateHint::changeRowOrdOrder:
			break;

		case CUpdateHint::setFont:
			Invalidate(TRUE);
			break;

		case CUpdateHint::changeColumnTotalsOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabTotals )
			{
				rInvalid.left = 0;
				rInvalid.top = m_pRows->GetAt(pH->m_idxOld)->m_yTop;
				rInvalid.right = m_cxClient;
			}
			break;

		case CUpdateHint::changeColumnIntrinsicTotalsOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabIntrinsicTotals )
			{
				rInvalid.left = 0;
				rInvalid.top = m_pRows->GetAt(pH->m_idxOld)->m_yTop;
				rInvalid.right = m_cxClient;
			}
			break;

		case CUpdateHint::changeColumnAllocOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabAlloc )
			{
				rInvalid.left = 0;
				rInvalid.top = m_pRows->GetAt(pH->m_idxOld)->m_yTop;
				rInvalid.right = m_cxClient;
			}
			break;

		case CUpdateHint::changeColumnCompareOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabCompare )
			{
				rInvalid.left = 0;
				rInvalid.top = m_pRows->GetAt(pH->m_idxOld)->m_yTop;
				rInvalid.right = m_cxClient;
			}
			break;

		case CUpdateHint::changeColumnPccOrder:
			if ( m_Tabs.GetIdxActiveTab() == tabPcc )
			{
				rInvalid.left = 0;
				rInvalid.top = m_pRows->GetAt(pH->m_idxOld)->m_yTop;
				rInvalid.right = m_cxClient;
			}
			break;

		case CUpdateHint::insertRow:
		case CUpdateHint::deleteRow:
		case CUpdateHint::addRow:
			if ( m_Tabs.GetIdxActiveTab() == tabTotals )
				rInvalid.SetRect(m_rBtnBar.right + 1,0,
								 m_cxClient,m_cyClient - m_Tabs.GetTabWidth());
			break;

		default:  // not a hint that we recognise
			Invalidate(TRUE);
			UpdateWindow();
			break;
	}

	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.bottom = m_cyClient - m_Tabs.GetTabWidth();
		InvalidateRect(rInvalid,TRUE);
	}

	RedrawWindow();
}

void CColumnTotalsView::CalcLayout()
{
	long y = 0;
	long i;
	long nCount = m_pRows->GetSize();

	for ( i = 0; i < nCount; i++ )
	{
		m_pRows->GetAt(i)->m_yTop = y;
		if(i < m_pAttributes->GetSize() && !m_pAttributes->GetAt(i)->IsHidden())
			y += m_pTableView->m_nRowHeight;
		m_pRows->GetAt(i)->m_yBottom = y;
		y++;
	}

	m_rBtnBar.SetRect(0,0,m_pTableView->m_nRowBtnBarWidth,min(y - 1,m_cyClient));
}

void CColumnTotalsView::HScroll(long cxScroll)
{
	CRect	rScroll;
	CRect	rInvalid;
	
	if ( cxScroll == 0 )
		return;

	// The area to scroll is the client area to the right of button bar & above tabs
	rScroll.SetRect(m_rBtnBar.right + 1, 0,
					m_pTableView->m_xGrid - 1, m_cyClient - m_Tabs.GetTabWidth());
	ScrollWindow(cxScroll,0,rScroll,rScroll);

	// mark right side of client area for repainting so as to cater for varying column widths
	// eg, new right column may extend further to right than previous right column
	rInvalid.SetRect(min(m_pTableView->m_cxClient,m_pTableView->m_xGrid - 1),0,
					 m_cxClient,m_cyClient - m_Tabs.GetTabWidth());
	InvalidateRect(rInvalid);

	// NOTE: UpdateWindow() cannot be called here because CTableView has not yet
	// called UpdateWindow() -> OnDraw() -> CalcLayout().  Therefore, UpdateWindow()
	// is called from CTableView::OnHScroll() after CTableView calls UpdateWindow()
}

void CColumnTotalsView::Refresh()
{
	CRect r;
	// The area to refresh is the client area to the right of button bar & above tabs
	//r.SetRect(1,0,m_cxClient,m_cyClient - m_Tabs.GetTabWidth());
	//set area to include buttonbar
	r.SetRect(1,0,0,m_cyClient - m_Tabs.GetTabWidth());
	InvalidateRect(r);

	// NOTE: UpdateWindow() cannot be called here because CTableView has not yet
	// called UpdateWindow() -> OnDraw() -> CalcLayout().  Therefore, UpdateWindow()
	// is called from CTableView::OnHScroll() after CTableView calls UpdateWindow()
}

void CColumnTotalsView::CheckPcc()
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	if(m_Tabs.GetIdxActiveTab() == tabPcc){
		if(pDoc->m_Ord3d == 3 && m_pAttributes->GetSize() < 4){
			CRowAttributes * pRowAttributes = new CRowAttributes;
			pRowAttributes->SetLabel("Z");
			pRowAttributes->SetType(2);
			pRowAttributes->SetHeight(-1);
			m_pAttributes->InsertAt(2,pRowAttributes);
			m_pRows->InsertAt(2,new CColumnTotalsRow);
		}else if(!(pDoc->m_Ord3d == 3) && m_pAttributes->GetSize() > 3){
			//find 'Z'
			for(long i=0;i<m_pAttributes->GetSize();i++){
				if(m_pAttributes->GetAt(i)->GetType() == 2){
					delete m_pAttributes->GetAt(i);
					m_pAttributes->RemoveAt(i);
					delete m_pRows->GetAt(i);
					m_pRows->RemoveAt(i);
				}
			}			
		}
	}
}

void CColumnTotalsView::OnVisibledecimals7() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(7);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals6() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(6);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals5() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(5);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals4() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(4);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals3() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(3);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals2() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(2);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals1() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(1);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnVisibledecimals0() 
{
	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();
	for(long i=0;i<pDoc->m_aColumnAttributes.GetSize();i++){
		pDoc->m_aColumnAttributes[i]->SetDecimalPrecision(0);
	}
	pDoc->UpdateAllViews(NULL);
}

void CColumnTotalsView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	CPoint local = point;
	ScreenToClient(&local);

	if (menu.LoadMenu(IDR_TABLETABS)){
		CMenu* pPopup = menu.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y,AfxGetMainWnd());
	}	
}
