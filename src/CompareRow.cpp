// CompareRow.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "CompareRow.h"
#include "tableview.h"
#include "tableframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompareRow

IMPLEMENT_DYNCREATE(CCompareRow, CView)

CCompareRow::CCompareRow()
{
}

CCompareRow::~CCompareRow()
{
}


BEGIN_MESSAGE_MAP(CCompareRow, CView)
	//{{AFX_MSG_MAP(CCompareRow)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompareRow drawing

void CCompareRow::OnDraw(CDC* pDC)
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
	

	pDC->SetTextColor(pDoc->m_rgbFontCells);

	// select font into memory DC
	CFont * oldFont = pDC->SelectObject(&(m_pTableView->m_fontCells));
	if ( oldFont == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for column totals");
	}

	CBrush*	pOldBrush;
	int		nOldBkMode, nOldBkColor;
	CRect	rText;
	long		nCount = 1;
	long		y;
	long		x;
	CSize	size;
	CString	str;
	CString strEllipsis = "...";
	long		cxEllipsis = (pDC->GetTextExtent(strEllipsis)).cx;

	nOldBkColor = pDC->SetBkColor(RGB(192,192,192));

	// buttons
	y = m_pTableView->m_nRowHeight;
	pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(0,0,m_pTableView->m_nRowBtnBarWidth,y,PATCOPY);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,1,y,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(m_pTableView->m_nRowBtnBarWidth,0,1,y,PATCOPY);
	y = m_pTableView->m_nRowHeight*2;
	pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(0,y/2,m_pTableView->m_nRowBtnBarWidth,y,PATCOPY);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,y/2,1,y,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(m_pTableView->m_nRowBtnBarWidth,0,1,y,PATCOPY);
	
	// button labels
	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	rText.left = 1;
	rText.right = m_pTableView->m_nRowBtnBarWidth;

	rText.top = 1;
	rText.bottom = m_pTableView->m_nRowHeight;
	// if at least one char can be printed
    if ( m_pTableView->m_tm.tmAveCharWidth <= rText.Width() )
    {
		pDoc->GetRowLabel(0,str);
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
	rText.left = 1;
	rText.right = m_pTableView->m_nRowBtnBarWidth;
	rText.top = m_pTableView->m_nRowHeight;
	rText.bottom = m_pTableView->m_nRowHeight * 2;
	if ( m_pTableView->m_tm.tmAveCharWidth <= rText.Width() )
    {
		str = "Difference";
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


	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldFont);
	pDC->SetBkMode(nOldBkMode);
	pDC->SetBkColor(nOldBkColor);


 	CRect	r, rClip;
	long		nColLimit = m_pTableView->m_aColumns.GetSize();
	long		nRowLimit = 2;
	long		iCol, iRow;
	CString	strValue;
	CColumn* pCol;

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
 	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	rText.top = 0;
	rText.bottom = 20;
	for ( iRow = 0; iRow < nRowLimit; iRow++ )
	{
		rText.left = m_pTableView->m_nRowBtnBarWidth;
		rText.right = m_pTableView->m_nRowBtnBarWidth;
		for ( iCol = m_pTableView->m_idxLeftColumn; iCol < m_pTableView->m_idxLeftColumn+nColLimit; iCol++ )
		{
			pCol = m_pTableView->m_aColumns[iCol-m_pTableView->m_idxLeftColumn];

			if ( pCol->m_bIsHidden || pCol->m_bIsExtrinsicBar )
				continue;

			rText.left = rText.right;
			rText.right = rText.left + pCol->m_xRight - pCol->m_xLeft;

			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			if(iRow == 0)
				strValue.Format("%.2lf",pDoc->GetValueAt(iCol,0));
			else
				strValue.Format("%.2lf",pDoc->GetValueAt(iCol,1) - pDoc->GetValueAt(iCol,0));

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
		rText.top += 20;
		rText.bottom += 20;
	}

	pDC->SetBkMode(nOldBkMode);

}

/////////////////////////////////////////////////////////////////////////////
// CCompareRow diagnostics

#ifdef _DEBUG
void CCompareRow::AssertValid() const
{
	CView::AssertValid();
}

void CCompareRow::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCompareRow message handlers

void CCompareRow::OnSize(UINT nType, int cx, int cy) 
{
	long m_Compare = 1;
	static first = 0;

	CView::OnSize(nType, cx, cy);	
	
}

BOOL CCompareRow::SetPointerToTableView(CTableView* pTableView)
{
	ASSERT(pTableView->IsKindOf(RUNTIME_CLASS(CTableView)));
	m_pTableView = pTableView;
	return TRUE;
}

void CCompareRow::Refresh()
{
	CRect r;
	// The area to refresh is the client area to the right of button bar & above tabs
	r.SetRect(1,0,2000,2000);
	InvalidateRect(r);

	// NOTE: UpdateWindow() cannot be called here because CTableView has not yet
	// called UpdateWindow() -> OnDraw() -> CalcLayout().  Therefore, UpdateWindow()
	// is called from CTableView::OnHScroll() after CTableView calls UpdateWindow()
}