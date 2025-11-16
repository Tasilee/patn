// fuseview.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "fuseswnd.h"
#include "fuseview.h"
#include "fuse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CXLOGMM (m_FuseMetrics.LogPixelsX*10/254)
#define CYLOGMM (m_FuseMetrics.LogPixelsY*10/254)
#define CYLINESPACING m_FuseMetrics.cyLineSpacing
#define LABELWIDTH m_FuseMetrics.sizeObjectLabel.cx
#define FONTSCALING m_FuseMetrics.LogPixelsY/72.0
#define YSCROLLTOP m_FuseMetrics.yScrollTop
#define WIDTH m_FuseMetrics.width
#define HEIGHT m_FuseMetrics.height
#define TOPMARGIN (15*CYLOGMM)
#define BOTTOMMARGIN (10*CYLOGMM)
#define LEFTMARGIN (25*CXLOGMM)
#define RIGHTMARGIN (25*CXLOGMM)
#define HEADERFOOTERGAP (10*CYLOGMM)
#define LINESPERPAGE m_FuseMetrics.nNumLinesPerPage
#define LEFT x[0]

/////////////////////////////////////////////////////////////////////////////
// CFuseView

IMPLEMENT_DYNCREATE(CFuseView, CViewExtension)

CFuseView::CFuseView()
{
	InitializeFonts();
}

CFuseView::~CFuseView()
{
}

#ifdef _DEBUG
CPatnDoc* CFuseView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CFuseView, CViewExtension)
	//{{AFX_MSG_MAP(CFuseView)
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_COMMAND(ID_OPTIONS_FONT, OnOptionsFont)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__SAVEAS, OnSaveas)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID__FONT, OnOptionsFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFuseView drawing

void CFuseView::OnDraw(CDC* pDC)
{
	
	pDC->SetTextColor(m_pDoc->m_rgbFontCells);

    CString strLeft, strRight;
	char buffer[40];
	long i;
	long nSize;
	CFuseLine* pFuseLine;
	long tw[3];	// text width
	long x[7],y;
	CFont fontHeading, fontObjects, fontText;
	CPen penBorder,penThickBorder;	// used when printing
	CBrush brushGrey;
	CRect rLabel;
	CRect rClip;
	const char *Text[3] = {	" fused with:  ", " at:  ", " to form new:  "	};

	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	pDC->GetClipBox(&rClip);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);

	if ( !CalcMetrics(pDC) )
	{
		IN_OnDraw = FALSE;
		return;
	}

	// Create GDI objects -----------------------------------------------------
    if ( ! fontHeading.CreateFontIndirect(&m_lfHeading) )
			{
		IN_OnDraw = FALSE;
		return;
	}
    if ( ! fontObjects.CreateFontIndirect(&m_pDoc->m_lfCells) )
			{
		IN_OnDraw = FALSE;
		return;
	}
    if ( ! fontText.CreateFontIndirect(&m_lfText) )
			{
		IN_OnDraw = FALSE;
		return;
	}

	if ( ! brushGrey.CreateSolidBrush(RGB(192,192,192)) )
			{
		IN_OnDraw = FALSE;
		return;
	}

	if ( pDC->IsPrinting() )
	{
		if ( ! penBorder.CreatePen(PS_SOLID,CYLOGMM*50/100,RGB(0,0,0)) )
				{
		IN_OnDraw = FALSE;
		return;
		}

		if ( ! penThickBorder.CreatePen(PS_SOLID,CYLOGMM,RGB(0,0,0)) )
				{
		IN_OnDraw = FALSE;
		return;
		}

	}
	// ========================================================================

	// Calculate layout -------------------------------------------------------
	if ( pDC->SelectObject(&fontObjects) == NULL )
			{
		IN_OnDraw = FALSE;
		return;
	}

	for ( i = 0; i < 3; i++ )
	{
		tw[i] = pDC->GetTextExtent(Text[i], strlen(Text[i])).cx;
	}

	x[0] = 4 * CXLOGMM;			// left side of first object's label
	x[1] = x[0] + LABELWIDTH;	// left of " fused with "
	x[2] = x[1] + tw[0];///FONTSCALING;		// left side of second object's label
	x[3] = x[2] + LABELWIDTH;	// left side of " at "
	x[4] = x[3] + tw[1];//FONTSCALING; 		// left side of fusion value
	x[5] = x[4] + LABELWIDTH;//(pDC->GetTextExtent("99.9999", 7)).cx;	// left side of " to form new "
	x[6] = x[5] + tw[2];//FONTSCALING; 		// left side of new object's label
	// ========================================================================
	
	// Paint heading ----------------------------------------------------------
 	if ( pDC->SelectObject(&brushGrey) == NULL )
			{
		IN_OnDraw = FALSE;
		return;
	}

	if ( pDC->SelectObject(&fontHeading) == NULL )
			{
		IN_OnDraw = FALSE;
		return;
	}

	if ( !pDC->IsPrinting() )
		pDC->PatBlt(0,0,WIDTH,m_FuseMetrics.cyHeadingBar,PATCOPY);
	if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
		if(m_pDoc->m_FuseRowChange)
		{
			pDC->TextOut(LEFT,m_FuseMetrics.yHeadingBottom,"Row Fusion Data Changed",23);
			IN_OnDraw = FALSE;
			return;
		}else
		{
			pDC->TextOut(LEFT,m_FuseMetrics.yHeadingBottom,"Row Fusion History",18);
		}
	else
	{
		if(m_pDoc->m_FuseColChange)
		{
			pDC->TextOut(LEFT,m_FuseMetrics.yHeadingBottom,"Column Fusion Data Changed",26);
			IN_OnDraw = FALSE;
			return;
		}else
		{
			pDC->TextOut(LEFT,m_FuseMetrics.yHeadingBottom,"Column Fusion History",21);
		}
	}
	if ( pDC->IsPrinting() )
	{
		if ( pDC->SelectObject(&penThickBorder) == NULL )
				{
		IN_OnDraw = FALSE;
		return;
	}
		pDC->MoveTo(0,m_FuseMetrics.cyHeadingBar);
		pDC->LineTo(WIDTH,m_FuseMetrics.cyHeadingBar);
	}
	// ========================================================================

	// Paint the table --------------------------------------------------------
	pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	long idx;
	nSize = m_pFuseTable->GetSize();
	y = YSCROLLTOP + CYLINESPACING; // bottom of first label
	if ( pDC->IsPrinting() )
		i = m_idxTopPrintRow;
	else
		i = m_idxTopRow;
	for ( ; i < nSize; i++ )
	{
		if ( y < rClip.top )
		{
			y += CYLINESPACING;
			continue;
		}
		pFuseLine = (CFuseLine*)m_pFuseTable->GetAt(i);
		idx = pFuseLine->nLeft;
		
		if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
			m_pDoc->GetRowLabel(idx,strLeft);
		else
			m_pDoc->GetColumnLabel(idx,strLeft);
		idx = pFuseLine->nRight;
		if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
			m_pDoc->GetRowLabel(idx,strRight);
		else
			m_pDoc->GetColumnLabel(idx,strRight);
		sprintf(buffer,"%0.4f",pFuseLine->nLevel);

		if ( pDC->SelectObject(&fontObjects) == NULL )
				{
		IN_OnDraw = FALSE;
		return;
		}


		rLabel.bottom = y;
		rLabel.top = rLabel.bottom - m_FuseMetrics.sizeObjectLabel.cy;

		// first object's label
		rLabel.left = LEFT;
		rLabel.right = rLabel.left + m_FuseMetrics.sizeObjectLabel.cx;
		//get text extent
		CSize sz;
		
		if(pDC->GetTextExtent(strLeft).cx > rLabel.Width())
			strLeft = strLeft.Left(strLeft.GetLength()-2) + "..";
		while((sz = pDC->GetTextExtent(strLeft)).cx > rLabel.Width() && strLeft.GetLength() > 3)
		{
			strLeft = strLeft.Left(strLeft.GetLength()-3) + "..";
		}
		pDC->ExtTextOut(x[0],y,ETO_CLIPPED,&rLabel,strLeft,strLeft.GetLength(),NULL);

		// second object's label
		rLabel.left = x[2];
		rLabel.right = rLabel.left + m_FuseMetrics.sizeObjectLabel.cx;
		if(pDC->GetTextExtent(strRight).cx > rLabel.Width())
			strRight = strRight.Left(strRight.GetLength()-2) + "..";
		while((sz = pDC->GetTextExtent(strRight)).cx > rLabel.Width() && strRight.GetLength() > 3)
		{
			strRight = strRight.Left(strRight.GetLength()-3) + "..";
		}
		pDC->ExtTextOut(x[2],y,ETO_CLIPPED,&rLabel,strRight,strRight.GetLength(),NULL);

		// fusion level
		rLabel.left = x[4];
		rLabel.right = rLabel.left + m_FuseMetrics.sizeObjectLabel.cx;
		pDC->ExtTextOut(x[4],y,ETO_CLIPPED,&rLabel,buffer,strlen(buffer),NULL);

		// new object's label
		rLabel.left = x[6];
		rLabel.right = rLabel.left + m_FuseMetrics.sizeObjectLabel.cx;
		pDC->ExtTextOut(x[6],y,ETO_CLIPPED,&rLabel,strLeft,strLeft.GetLength(),NULL);

		rLabel.left = x[1];
		rLabel.right = x[2];
		pDC->ExtTextOut(x[1],y,ETO_CLIPPED,&rLabel,Text[0],strlen(Text[0]),NULL);
		rLabel.left = x[3];
		rLabel.right = x[4];
		pDC->ExtTextOut(x[3],y,ETO_CLIPPED,&rLabel,Text[1],strlen(Text[1]),NULL);
		rLabel.left = x[5];
		rLabel.right = x[6];
		pDC->ExtTextOut(x[5],y,ETO_CLIPPED,&rLabel,Text[2],strlen(Text[2]),NULL);

		if ( y > rClip.bottom )
			break;
		y += CYLINESPACING;
	}
	// ========================================================================
	// Draw border top and bottom ---------------------------------------------
	if ( pDC->IsPrinting() )
	{
  		if ( pDC->SelectObject(&penBorder) == NULL )
				{
		IN_OnDraw = FALSE;
		return;
		}

		pDC->MoveTo(0,YSCROLLTOP);
		pDC->LineTo(WIDTH,YSCROLLTOP);
		pDC->MoveTo(0,HEIGHT);
		pDC->LineTo(WIDTH,HEIGHT);
	}
	//=========================================================================
	IN_OnDraw = FALSE;	
}

void CFuseView::InitializeFonts()
{
	m_nHeadingPointSize = 16;
	m_nHeaderFooterPointSize = 10;

	m_lfHeading.lfHeight = 0; // to be set when device context is prepared
	m_lfHeading.lfWidth = 0;
	m_lfHeading.lfEscapement = 0;
	m_lfHeading.lfOrientation = 0;
	m_lfHeading.lfWeight = FW_BOLD;
	m_lfHeading.lfItalic = FALSE;
	m_lfHeading.lfUnderline = FALSE;
	m_lfHeading.lfStrikeOut = 0;
	m_lfHeading.lfCharSet = ANSI_CHARSET;
	m_lfHeading.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfHeading.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfHeading.lfQuality = PROOF_QUALITY;
	m_lfHeading.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
	strcpy(m_lfHeading.lfFaceName, ((CPatnApp*)AfxGetApp())->m_globalFontName);

	m_lfHeaderFooter.lfHeight = 0; // to be set when device context is prepared
	m_lfHeaderFooter.lfWidth = 0;
	m_lfHeaderFooter.lfEscapement = 0;
	m_lfHeaderFooter.lfOrientation = 0;
	m_lfHeaderFooter.lfWeight = FW_NORMAL;
	m_lfHeaderFooter.lfItalic = FALSE;
	m_lfHeaderFooter.lfUnderline = FALSE;
	m_lfHeaderFooter.lfStrikeOut = 0;
	m_lfHeaderFooter.lfCharSet = ANSI_CHARSET;
	m_lfHeaderFooter.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfHeaderFooter.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfHeaderFooter.lfQuality = PROOF_QUALITY;
	m_lfHeaderFooter.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
	strcpy(m_lfHeaderFooter.lfFaceName, ((CPatnApp*)AfxGetApp())->m_globalFontName);
}

BOOL CFuseView::CalcMetrics(CDC* pDC)
{
	CFont font;
	TEXTMETRIC tm;

	if ( !pDC->IsPrinting() )
	{
		// if printing, then the width & height are set in OnPrint() based on m_rectDraw
		CRect rClient;
		GetClientRect(&rClient);
		m_FuseMetrics.width = rClient.Width();
		m_FuseMetrics.height = rClient.Height();
	}

	// Header & footer (printing) ---------------------------------------------
	if ( pDC->IsPrinting() )
	{
		//m_lfHeaderFooter.lfHeight = -m_nHeaderFooterPointSize * FONTSCALING;
	    if ( ! font.CreateFontIndirect(&m_lfHeaderFooter) )
			return FALSE;
		if ( pDC->SelectObject(&font) == NULL )
			return FALSE;
		pDC->GetTextMetrics(&tm);
		m_FuseMetrics.cyHeader = tm.tmHeight + TOPMARGIN + HEADERFOOTERGAP;
		m_FuseMetrics.cyFooter = tm.tmHeight + BOTTOMMARGIN + HEADERFOOTERGAP;

		if ( !font.DeleteObject() )
			return FALSE;
	}
	// ========================================================================

	// Heading bar ------------------------------------------------------------
	//m_lfHeading.lfHeight = -m_nHeadingPointSize * FONTSCALING;
    if ( ! font.CreateFontIndirect(&m_lfHeading) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )
		return FALSE;
	pDC->GetTextMetrics(&tm);
	m_FuseMetrics.cyHeadingBar = (long)(tm.tmHeight*1.5);
	m_FuseMetrics.yHeadingBottom = m_FuseMetrics.cyHeadingBar -
								   (m_FuseMetrics.cyHeadingBar - tm.tmHeight)/2;

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	m_FuseMetrics.yScrollTop = m_FuseMetrics.cyHeadingBar + 1;

	// Objects ----------------------------------------------------------------
	//m_pDoc->m_lfCells.lfHeight = -(m_pDoc->m_nPointSizeCells * FONTSCALING);
    if ( ! font.CreateFontIndirect(&m_pDoc->m_lfCells) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )
		return FALSE;

	pDC->GetTextMetrics(&tm);
	m_FuseMetrics.cyLineSpacing = (long)(tm.tmHeight*1.2);
	m_FuseMetrics.sizeObjectLabel = pDC->GetTextExtent("WWWW0000",8);

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	// Text -------------------------------------------------------------------
	m_lfText.lfHeight = (long)(-((m_pDoc->m_nPointSizeCells) * FONTSCALING));
	m_lfText.lfWidth = m_pDoc->m_lfCells.lfWidth;
	m_lfText.lfEscapement = m_pDoc->m_lfCells.lfEscapement;
	m_lfText.lfOrientation = m_pDoc->m_lfCells.lfOrientation;
	m_lfText.lfWeight = FW_NORMAL;
	m_lfText.lfItalic = m_pDoc->m_lfCells.lfItalic;
	m_lfText.lfUnderline = m_pDoc->m_lfCells.lfUnderline;
	m_lfText.lfStrikeOut = m_pDoc->m_lfCells.lfStrikeOut;
	m_lfText.lfCharSet = m_pDoc->m_lfCells.lfCharSet;
	m_lfText.lfOutPrecision = m_pDoc->m_lfCells.lfOutPrecision;
	m_lfText.lfClipPrecision = m_pDoc->m_lfCells.lfClipPrecision;
	m_lfText.lfQuality = m_pDoc->m_lfCells.lfQuality;
	m_lfText.lfPitchAndFamily = m_pDoc->m_lfCells.lfPitchAndFamily;
	strcpy(m_lfText.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
    if ( ! font.CreateFontIndirect(&m_lfText) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	return TRUE;
}

BOOL CFuseView::Create(LPCSTR lpszClassName,
	LPCSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext /* = NULL */)
{
	dwStyle |= WS_VSCROLL;	// Add vertical & horizontal scroll bars

	return CWnd::Create(lpszClassName,lpszWindowName, dwStyle,rect,pParentWnd, nID,pContext);
}

/////////////////////////////////////////////////////////////////////////////
// CFuseView diagnostics

#ifdef _DEBUG
void CFuseView::AssertValid() const
{
	CView::AssertValid();
}

void CFuseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFuseView message handlers

void CFuseView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
    {
    // First determine if the user scrolled a scroll bar control
    // on the window or scrolled the window itself
        HWND hWndScroll;
        if ( pScrollBar == NULL )
            hWndScroll = m_hWnd;
        else
            hWndScroll = pScrollBar->m_hWnd;

        SCROLLINFO info;
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_TRACKPOS;
        ::GetScrollInfo(hWndScroll, SB_VERT, &info);

        nPos = info.nTrackPos;
    }
	// The area to scroll is the client area below the top labels	
	long nScrollAmount = 0;
	CRect rectClient;
	CRect rectScroll;
	GetClientRect(&rectClient);
	rectScroll.SetRect(0,m_FuseMetrics.yScrollTop,rectClient.Width(),rectClient.Height());
	
	switch(nSBCode)
	{
		case SB_LINEUP:
			if ( m_idxTopRow > 0 )
			{
				m_idxTopRow--;
				nScrollAmount = CYLINESPACING;
			}
			break;
		
		case SB_LINEDOWN:
			if ( m_idxTopRow < m_pFuseTable->GetSize()-1 )
			{
				m_idxTopRow++;
				nScrollAmount = -CYLINESPACING;
			}
			break;

		case SB_PAGEUP:
			while ( m_idxTopRow > 0 )
			{
				m_idxTopRow--;
				nScrollAmount += CYLINESPACING;
					
				// If we've gone too far, back up one column
				if ( nScrollAmount > rectScroll.Height() )
				{
					nScrollAmount -= CYLINESPACING;
					m_idxTopRow++;
					break;
				}
			}
			break;
		
		case SB_PAGEDOWN:
			while ( m_idxTopRow < m_pFuseTable->GetSize()-1 )
			{
				m_idxTopRow++;
				nScrollAmount += CYLINESPACING;
					
				// If we've gone too far, back up one column
				if ( nScrollAmount > rectScroll.Height() )
				{
					nScrollAmount -= CYLINESPACING;
					m_idxTopRow--;
					break;
				}
			}
			nScrollAmount = -nScrollAmount;
			break;
		
		case SB_THUMBPOSITION:
			m_idxTopRow = (long)nPos;
			nScrollAmount=0;
			InvalidateRect(rectScroll,TRUE);
			UpdateWindow();
			break;

		default:
			break;		
	}

	if ( nScrollAmount != 0 )
	{
		ScrollWindow(0,nScrollAmount,rectScroll,rectScroll);
		UpdateWindow();
	}
	SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
}

int CFuseView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_psd = NULL;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_menu.LoadMenu(IDR_TABLE);
	
	m_pDoc = GetDocument();
	ASSERT_VALID(m_pDoc);

	CFuseSplitterWnd* pSplitterWnd = (CFuseSplitterWnd*)GetParentFrame();
	m_pFuseTable = pSplitterWnd->m_pFuseTable;
	
	m_idxTopRow = 0;
	SetScrollRange(SB_VERT,0,m_pFuseTable->GetSize()-1,TRUE);

	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;

	return 0;
}

void CFuseView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	
	CView::OnPrepareDC(pDC, pInfo);
	SetMapping(pDC);
}

void CFuseView::SetMapping(CDC* pDC)
{
	pDC->SetMapMode(MM_TEXT);
	m_FuseMetrics.LogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	m_FuseMetrics.LogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
}


void CFuseView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
//	CRect rect,temp;
//	CPoint point;

//	m_psd->GetMargins(&rect,&temp);
//	point.x = m_psd->GetPaperSize().cx;
//	point.y = m_psd->GetPaperSize().cy;	
	
	//////////////////////////////////////////////////////////////
	//device dependant info for regions & size & aspect retaining
	//////////////////////////////////////////////////////////////
	m_HRes = pDC->GetDeviceCaps(HORZRES);
	m_VRes = pDC->GetDeviceCaps(VERTRES);
	//below to determine xscale
	long HPixelsPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	m_HScale = HPixelsPerInch / 300.0;
	//below to determine yscale
	long VPixelsPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	m_VScale = VPixelsPerInch / 300.0;

	m_pointsize = m_pDoc->m_lfCells.lfHeight; // iPointSize is in 1/10s
	m_pDoc->m_lfCells.lfHeight = (long)(m_pDoc->m_lfCells.lfHeight * m_HScale * 2);
	if(m_pDoc->m_lfCells.lfHeight > 0) m_pDoc->m_lfCells.lfHeight *=-1;
	m_FuseMetrics.cyLineSpacing = (long) (m_FuseMetrics.cyLineSpacing * 2*m_VScale);

	//USED 300 'CAUSE THAT'S WHAT IT WAS WRITTEN ON
//	m_rectDraw.left = (long)((rect.left)/((double) point.x)*m_HRes);
//	m_rectDraw.right = (long)((point.x - rect.right)/((double) point.x)*m_HRes);
//	m_rectDraw.top = (long)((rect.top)/((double) point.y)*m_VRes);
//	m_rectDraw.bottom = (long)((point.y - rect.bottom)/((double) point.y)*m_VRes);
	///////////////////////////////////////////////////////////////
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = m_HRes;
	m_rectDraw.bottom = m_VRes;

	// default preparation
	PageInfo *info;
	long count;

	long cyprintline = (long) (-1*m_pDoc->m_lfCells.lfHeight*m_HScale*2);

	if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
		if ( m_pDoc->IsExtrinsicRows() )
			count = m_pDoc->GetIdxTopExtrinsicRow();
		else
			count = m_pDoc->GetNoOfRows();
	}else{
		if ( m_pDoc->IsExtrinsicColumns() )
			count = m_pDoc->GetIdxLeftExtrinsicColumn();
		else
			count = m_pDoc->GetNoOfColumns();
	}

	//find row range & indexes
	long idx = -1;
	long startidx = idx;
	double ypos = 6*cyprintline; //header allocation
	m_pageinfo.SetSize(0);
	while(startidx < count-1){
		ypos += m_rectDraw.top + cyprintline;
		while(ypos <= m_rectDraw.bottom  && idx < count-2){
			idx++;
			if(idx+1 < count) ypos += cyprintline;
		}
		info = new PageInfo;

		info->top = startidx;
		info->bottom = idx ;
		m_pageinfo.Add(info);

		startidx = idx+1;
		ypos = m_rectDraw.top;
	}



	pInfo->SetMaxPage(m_pageinfo.GetSize());

}


BOOL CFuseView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog *d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	return DoPreparePrinting(pInfo);
}


void CFuseView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	long i;
	for(i=0;i<m_pageinfo.GetSize();i++)
		delete m_pageinfo.GetAt(i);
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	m_pDoc->m_lfCells.lfHeight = m_pointsize;
	if(m_pDoc->m_lfCells.lfHeight >0) m_pDoc->m_lfCells.lfHeight *=-1;

	m_FuseMetrics.cyLineSpacing = (long) (m_FuseMetrics.cyLineSpacing/(3*m_VScale));
}


void CFuseView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//current page being printed == pInfo->m_nCurSel
	long cPage = pInfo->m_nCurPage - 1;
	long j;
	CRect rect;
	CString text;
	CFont fontData;
	CFont*	pOldFont;
	CString temp;
	
	long cyprintline = (long)(-1*m_pDoc->m_lfCells.lfHeight * m_HScale * 2);

	fontData.CreateFontIndirect(&m_pDoc->m_lfCells);

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(pDoc->m_rgbFontCells);

	// select font into memory DC
	if ( (pOldFont = pDC->SelectObject(&fontData)) == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for table");
	}
	
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);

	rect.left = rect.right = m_rectDraw.left;
	rect.right = m_rectDraw.right;	
	rect.top = m_rectDraw.top+1;
	rect.bottom = rect.top + cyprintline;

	double last = 0.0;
	double level;
	long b1,b2;
	CString buffer1,buffer2;

	for(j=m_pageinfo.GetAt(cPage)->top;j<=m_pageinfo.GetAt(cPage)->bottom;j++){
		if(j == -1){
			//printheader (should be +5 lines), see notes
			text.Format("Source Table Name: %s",m_pDoc->m_TableName);
			pDC->ExtTextOut(rect.left+2,rect.bottom, ETO_CLIPPED,
				rect,text,text.GetLength(),NULL);
			rect.top = rect.bottom;
			rect.bottom += cyprintline;
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				text.Format("Derived From Association of type: %s",
					m_pDoc->m_FuseAssoRows);
			}else{
				text.Format("Derived From Association of type: %s",
					m_pDoc->m_FuseAssoCols);
			}

			pDC->ExtTextOut(rect.left+2,rect.bottom, ETO_CLIPPED,
				rect,text,text.GetLength(),NULL);
			rect.top = rect.bottom;
			rect.bottom += cyprintline;
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				text.Format("Fusion Type: %s     Beta = %0.2f",
					m_pDoc->m_FuseTypeRows,m_pDoc->m_FuseBetaRows);
			}else{
				text.Format("Fusion Type: %s     Beta = %0.2f",
					m_pDoc->m_FuseTypeCols,m_pDoc->m_FuseBetaCols);
			}

			pDC->ExtTextOut(rect.left+2,rect.bottom, ETO_CLIPPED,
				rect,text,text.GetLength(),NULL);
			rect.top = rect.bottom;
			rect.bottom += cyprintline;

			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				text.Format("Created: %s",pDoc->m_FuseTimeRows);
			}else{
				text.Format("Created: %s",pDoc->m_FuseTimeCols);
			}
			pDC->ExtTextOut(rect.left+2,rect.bottom, ETO_CLIPPED,
				rect,text,text.GetLength(),NULL);
			rect.top = rect.bottom+cyprintline;
			rect.bottom += cyprintline*2;
			
			text = "Groups       First Object             Second Object        New Object      Level     Stress Diff";
		}else{
			//print ...
			// " (#) group1 (#) group2 (#) group1 (level) (stressdiff) "
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				pDoc->GetRowLabel(b1=m_pFuseTable->GetAt(j)->nLeft,buffer1);
				pDoc->GetRowLabel(b2=m_pFuseTable->GetAt(j)->nLeft,buffer2);
			}else{
				pDoc->GetColumnLabel(b1=m_pFuseTable->GetAt(j)->nLeft,buffer1);
				pDoc->GetColumnLabel(b2=m_pFuseTable->GetAt(j)->nLeft,buffer2);
			}

			level = m_pFuseTable->GetAt(j)->nLevel;
			text.Format("%4d %15s (%4d) + %15s (%4d)       =GP(%4d)-      %2.3lf       %2.3lf",
				j,buffer1,b1,buffer2,b2,b1,level,level-last);
			last = level;
		}
		pDC->ExtTextOut(rect.left+2,rect.bottom, ETO_CLIPPED,
			rect,text,text.GetLength(),NULL);
		
		rect.top = rect.bottom;
		rect.bottom += cyprintline;
	}

}

void CFuseView::OnOptionsFont() 
{
	LOGFONT lf;
	memcpy(&lf,&(m_pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	lf.lfHeight /= (long)(FONTSCALING);
	
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	dlg.m_cf.Flags |= CF_LIMITSIZE;
	dlg.m_cf.nSizeMin = 3;
	dlg.m_cf.nSizeMax = 32;
	dlg.m_cf.rgbColors = m_pDoc->m_rgbFontCells;

	if ( dlg.DoModal() == IDOK )
	{
		lf.lfHeight *= (long)(FONTSCALING);
		memcpy(&(m_pDoc->m_lfCells),&lf,sizeof(LOGFONT));
		strcpy(m_pDoc->m_lfCells.lfFaceName,lf.lfFaceName);
		m_pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
		m_pDoc->m_nPointSizeCells = dlg.m_cf.iPointSize;///10; // iPointSize is in 1/10s
		m_pDoc->SetModifiedFlag(TRUE);
		Invalidate();
		UpdateWindow();
	}
}

void CFuseView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());
}

void CFuseView::OnSaveas() 
{
	CFuse fuse(m_pDoc);
	if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
		fuse.WriteFuseTableToASCII(m_pDoc,CPatnDoc::rows);	
	}else{
		fuse.WriteFuseTableToASCII(m_pDoc,CPatnDoc::columns);	
	}
}

BOOL CFuseView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) return true;
	inMouseWheel = true;

	long x,y,diff;
	
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y + x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	long max = m_pFuseTable->GetSize()-1;

	for(long i=y;i != diff;(diff>y)?i++:i--){
		if(i <= 0 && i >= max) break;
		if(x < 0)
			OnVScroll(SB_LINEDOWN,0,NULL);
		else
			OnVScroll(SB_LINEUP,0,NULL);

	}
	inMouseWheel = false;

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

