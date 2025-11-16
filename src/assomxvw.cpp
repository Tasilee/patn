// assomxvw.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "MainFrame.h"
#include "assoswnd.h"
#include "assomxvw.h"
#include "association.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CXLOGMM (pDC->GetDeviceCaps(LOGPIXELSX)*10/254)
#define CYLOGMM (pDC->GetDeviceCaps(LOGPIXELSY)*10/254)
#define CYLINESPACING m_AssocMxMetrics.cyLineSpacing
#define CXCOLUMNWIDTH m_AssocMxMetrics.cxColumnWidth
#define LABELWIDTH m_AssocMxMetrics.sizeObjectLabel.cx
#define FONTSCALING m_AssocMxMetrics.LogPixelsY/72
#define YSCROLLTOP m_AssocMxMetrics.yScrollTop
#define WIDTH m_AssocMxMetrics.width
#define HEIGHT m_AssocMxMetrics.height
#define TOPMARGIN (15*CYLOGMM)
#define BOTTOMMARGIN (10*CYLOGMM)
#define LEFTMARGIN (25*CXLOGMM)
#define RIGHTMARGIN (25*CXLOGMM)
#define HEADERFOOTERGAP (10*CYLOGMM)
#define LINESPERPAGE m_AssocMxMetrics.nNumLinesPerPage

/////////////////////////////////////////////////////////////////////////////
// CAssocMatrixView

IMPLEMENT_DYNCREATE(CAssocMatrixView, CViewExtension)

CAssocMatrixView::CAssocMatrixView()
{
	InitializeFonts();
}

CAssocMatrixView::~CAssocMatrixView()
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
}

#ifdef _DEBUG
CPatnDoc* CAssocMatrixView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CAssocMatrixView, CView)
	//{{AFX_MSG_MAP(CAssocMatrixView)
	ON_WM_HSCROLL()
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
// CAssocMatrixView drawing

void CAssocMatrixView::OnDraw(CDC* pDC)
{

	CPatnDoc* pDoc = (CPatnDoc*)GetDocument();

	pDC->SetTextColor(pDoc->m_rgbFontCells);
	
	char buffer[400];
	double nValue;
	long i,j;
	CRect rText(0,0,0,0);
	long x ,y;
	CRect rClip;
	CString strLabel;
	CFont fontHeading, fontObjects, fontData;
	CSize size;
  	CAssocMatrixRow* pAMR;
	long nRows = m_pMatrix->m_aMatrix.GetSize();
	long nCols;
	TEXTMETRIC tm;

	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	rClip = CRect(0,0,0,0);
	pDC->GetClipBox(&rClip);

	if ( !CalcMetrics(pDC) )
	{
		IN_OnDraw = FALSE;
		return;
	}

	LOGFONT lf;
	memcpy(&lf,&(pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,pDoc->m_lfCells.lfFaceName);



	if(pDC->IsPrinting() && m_rectDraw.Width()/1.3 < m_rectDraw.Height())
		lf.lfHeight = (int)(lf.lfHeight*1.5);
	else
		lf.lfHeight = (int)(lf.lfHeight*1.5);
	//fontText.CreateFontIndirect(&lf);
	//lf.lfHeight = -nBarWidth;
	lf.lfWeight = FW_BOLD; 
    //fontHeading.CreateFontIndirect(&lf);

	// Create GDI objects -----------------------------------------------------
    //if ( ! fontHeading.CreateFontIndirect(&m_lfHeading) )
	if ( ! fontHeading.CreateFontIndirect(&lf) )
	{
		IN_OnDraw = FALSE;
		return;
	}
    if ( ! fontObjects.CreateFontIndirect(&m_pDoc->m_lfCells) )
	{
		fontHeading.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}
    if ( ! fontData.CreateFontIndirect(&m_pDoc->m_lfCells) )
	{
		fontHeading.DeleteObject();
		fontObjects.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}
	// ========================================================================

	// Paint heading ----------------------------------------------------------
	CFont* oldFont;
	if ( (oldFont = pDC->SelectObject(&fontHeading)) == NULL )
	{
		fontHeading.DeleteObject();
		fontObjects.DeleteObject();
		fontData.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	// Exit if data changed to prevent crash due to data change
	if(pDoc->m_AssocRowChange &&
		m_pMatrix == &pDoc->m_AssocMatrixRows)
		sprintf(buffer,"New Row Association Required");
	else if(pDoc->m_AssocColChange &&
		m_pMatrix == &pDoc->m_AssocMatrixCols)
		sprintf(buffer,"New Column Association Required");
	else
		sprintf(buffer,"%s",m_pMatrix->m_strMeasure);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	if ( size.cx > WIDTH )
		x = CXLOGMM;
	else
		x = (WIDTH - size.cx)/2;
	y = m_AssocMxMetrics.cyHeadingBar;
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.left = x;
	rText.right = rText.left + size.cx;
	
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
	pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);
	// ========================================================================
	
	//exit if data has changed
	if(pDoc->m_AssocRowChange &&
		m_pMatrix == &pDoc->m_AssocMatrixRows)
	{
		pDC->SelectObject(oldFont);
		fontHeading.DeleteObject();
		fontObjects.DeleteObject();
		fontData.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}
	else if(pDoc->m_AssocColChange &&
		m_pMatrix == &pDoc->m_AssocMatrixCols)
	{
		pDC->SelectObject(oldFont);
		fontHeading.DeleteObject();
		fontObjects.DeleteObject();
		fontData.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	// column labels at top ---------------------------------------------------
	pDC->SelectObject(&fontObjects);
	pDC->GetTextMetrics(&tm);
	//rText.right = CXCOLUMNWIDTH;
	rText.right = this->m_AssocMxMetrics.cx1stColumnWidth;
	rText.top = rText.bottom;
	rText.bottom = rText.top + CYLINESPACING;
	y = rText.bottom;
	if ( y > rClip.top && y < rClip.bottom )
	{
		for ( j = m_idxLeftColumn; j < nRows; j++ )
		{
			rText.left = rText.right;
			rText.right += CXCOLUMNWIDTH;
			x = rText.right - tm.tmAveCharWidth/2;
			if(m_pMatrix == &pDoc->m_AssocMatrixRows)
				pDoc->GetRowLabel(j,strLabel);
			else
				pDoc->GetColumnLabel(j,strLabel);
			CSize sz = pDC->GetTextExtent(strLabel);
			if(sz.cx > CXCOLUMNWIDTH-3)
			{
				strLabel = strLabel.Left(strLabel.GetLength()-5);
				strLabel += "...";
				sz = pDC->GetTextExtent(strLabel);
				while(sz.cx > CXCOLUMNWIDTH-3 && strLabel.GetLength() > 5)
				{
					strLabel = strLabel.Left(strLabel.GetLength()-4) + "...";

					sz = pDC->GetTextExtent(strLabel);
				}
			}
			pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,strLabel,strLabel.GetLength(),NULL);
			if ( rText.right > rClip.right )
				break;
		}
	}
	// ========================================================================

	// Rows: label on left then data fields -----------------------------------
	if ( pDC->IsPrinting() )
		i = m_idxTopPrintRow;
	else
		i = m_idxTopRow;
	for ( ; i < nRows; i++ )
	{

		rText.bottom += CYLINESPACING;
		y = rText.bottom;
		if ( y < rClip.top )
			continue;

		// label at left
		pDC->SelectObject(&fontObjects);
		pDC->GetTextMetrics(&tm);
		rText.top = rText.bottom - CYLINESPACING;
		rText.left = 0;
		rText.right = this->m_AssocMxMetrics.cx1stColumnWidth;
		x = rText.right - tm.tmAveCharWidth/2;
		if(m_pMatrix == &pDoc->m_AssocMatrixRows)
			pDoc->GetRowLabel(i+1,strLabel);
		else
			pDoc->GetColumnLabel(i+1,strLabel);
		pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,strLabel,strLabel.GetLength(),NULL);

		// data
		pDC->SelectObject(&fontData);
		pAMR = (CAssocMatrixRow*)m_pMatrix->m_aMatrix.GetAt(i);
		nCols = pAMR->GetSize();
		if ( pDC->IsPrinting() )
			j = m_idxLeftPrintColumn;
		else
			j = m_idxLeftColumn;
		for ( ; j < nCols; j++ )
		{
			nValue = pAMR->GetAt(j);
			sprintf(buffer,"%0.4f",nValue);
			
			rText.left = rText.right;
			rText.right += CXCOLUMNWIDTH;
			x = rText.right - tm.tmAveCharWidth/2;
			pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);
			if ( x >= rClip.right )
				break;
		}
		if ( y >= rClip.bottom )
			break;
	}
	// ========================================================================
	pDC->SelectObject(oldFont);
	fontHeading.DeleteObject();
	fontObjects.DeleteObject();
	fontData.DeleteObject();
		
	IN_OnDraw = FALSE;
}

void CAssocMatrixView::InitializeFonts()
{
	m_nHeadingPointSize = 16;
	m_nHeaderFooterPointSize = 10;

	m_lfHeading.lfHeight = 0; // to be set when device context is prepared
	m_lfHeading.lfWidth = 0;
	m_lfHeading.lfEscapement = 0;
	m_lfHeading.lfOrientation = 0;
	m_lfHeading.lfWeight = FW_BOLD;
	m_lfHeading.lfItalic = TRUE;
	m_lfHeading.lfUnderline = TRUE;
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

BOOL CAssocMatrixView::CalcMetrics(CDC* pDC)
{
	CFont font;
	TEXTMETRIC tm;

	if ( !pDC->IsPrinting() )
	{
		// if printing, then the width & height are set in OnPrint() based on m_rectDraw
		CRect rClient;
		GetClientRect(&rClient);
		m_AssocMxMetrics.width = rClient.Width();
		m_AssocMxMetrics.height = rClient.Height();
	}
	CFont * oldFont;

	// Header & footer (printing) ---------------------------------------------
	if ( pDC->IsPrinting() )
	{
		m_lfHeaderFooter.lfHeight = -m_nHeaderFooterPointSize * FONTSCALING;
	    if ( ! font.CreateFontIndirect(&m_lfHeaderFooter) )
			return FALSE;
		oldFont = pDC->SelectObject(&font);
		if ( oldFont == NULL ){
			font.DeleteObject();
			return FALSE;
		}
		pDC->GetTextMetrics(&tm);
		m_AssocMxMetrics.cyHeader = tm.tmHeight + TOPMARGIN + HEADERFOOTERGAP;
		m_AssocMxMetrics.cyFooter = tm.tmHeight + BOTTOMMARGIN + HEADERFOOTERGAP;

		pDC->SelectObject(oldFont);
		if ( !font.DeleteObject() ){
			return FALSE;
		}
	}
	// ========================================================================

	// Heading bar ------------------------------------------------------------
	m_lfHeading.lfHeight = -m_nHeadingPointSize * FONTSCALING;
    if ( ! font.CreateFontIndirect(&m_lfHeading) ){
		pDC->SelectObject(oldFont);
		return FALSE;
	}
	if ( (oldFont = pDC->SelectObject(&font)) == NULL ){
		pDC->SelectObject(oldFont);
		font.DeleteObject();
		return FALSE;
	}
	pDC->GetTextMetrics(&tm);
	m_AssocMxMetrics.cyHeadingBar = 15*tm.tmHeight/10;

	if ( !font.DeleteObject() ){
		pDC->SelectObject(oldFont);
		return FALSE;
	}
	// ========================================================================

	// Line spacing -----------------------------------------------------------
	//m_pDoc->m_lfCells.lfHeight = -(m_pDoc->m_nPointSizeCells * FONTSCALING);
    if ( ! font.CreateFontIndirect(&m_pDoc->m_lfCells) ){
		pDC->SelectObject(oldFont);
		return FALSE;
	}
	if ( pDC->SelectObject(&font) == NULL ){
		pDC->SelectObject(oldFont);
		font.DeleteObject();
		return FALSE;
	}

	pDC->GetTextMetrics(&tm);
	m_AssocMxMetrics.cyLineSpacing = tm.tmHeight*12/10;
	//HERE!!
	m_AssocMxMetrics.sizeObjectLabel = pDC->GetTextExtent("WWWWA0000",9);
	m_AssocMxMetrics.cxColumnWidth = m_AssocMxMetrics.sizeObjectLabel.cx + tm.tmAveCharWidth/2;
	
	CRect re;
	GetClientRect(&re);
	m_AssocMxMetrics.cx1stColumnWidth = m_AssocMxMetrics.cxColumnWidth;
	long nRows = m_pMatrix->m_aMatrix.GetSize();
	int i;
	if ( pDC->IsPrinting() )
		i = m_idxTopPrintRow;
	else
		i = m_idxTopRow;
	int top = 0;
	CPatnDoc * pDoc = GetDocument();CString strLabel;
	for ( ; i < nRows && top < re.Height(); i++ )
	{
		top += CYLINESPACING;
		
		// label at left
		if(m_pMatrix == &pDoc->m_AssocMatrixRows)
			pDoc->GetRowLabel(i+1,strLabel);
		else
			pDoc->GetColumnLabel(i+1,strLabel);
		CSize sz = pDC->GetTextExtent(strLabel);
		if(sz.cx+5 > m_AssocMxMetrics.cx1stColumnWidth)
			m_AssocMxMetrics.cx1stColumnWidth = sz.cx+5;
	}
	
	if ( !font.DeleteObject() ){
		pDC->SelectObject(oldFont);
		return FALSE;
	}
	// ========================================================================

	m_AssocMxMetrics.yScrollTop = m_AssocMxMetrics.cyHeadingBar +
								  CYLINESPACING + 1;

	// Data font --------------------------------------------------------------
	m_lfData.lfHeight = -((m_pDoc->m_nPointSizeCells) * FONTSCALING);
	m_lfData.lfWidth = m_pDoc->m_lfCells.lfWidth;
	m_lfData.lfEscapement = m_pDoc->m_lfCells.lfEscapement;
	m_lfData.lfOrientation = m_pDoc->m_lfCells.lfOrientation;
	m_lfData.lfWeight = FW_NORMAL;
	m_lfData.lfItalic = m_pDoc->m_lfCells.lfItalic;
	m_lfData.lfUnderline = m_pDoc->m_lfCells.lfUnderline;
	m_lfData.lfStrikeOut = m_pDoc->m_lfCells.lfStrikeOut;
	m_lfData.lfCharSet = m_pDoc->m_lfCells.lfCharSet;
	m_lfData.lfOutPrecision = m_pDoc->m_lfCells.lfOutPrecision;
	m_lfData.lfClipPrecision = m_pDoc->m_lfCells.lfClipPrecision;
	m_lfData.lfQuality = m_pDoc->m_lfCells.lfQuality;
	m_lfData.lfPitchAndFamily = m_pDoc->m_lfCells.lfPitchAndFamily;
	strcpy(m_lfData.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	// ========================================================================

	pDC->SelectObject(oldFont);

	return TRUE;
}

BOOL CAssocMatrixView::Create(LPCSTR lpszClassName,
	LPCSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext /* = NULL */)
{
	dwStyle |= WS_VSCROLL | WS_HSCROLL;	// Add vertical & horizontal scroll bars

	return CWnd::Create(lpszClassName,lpszWindowName, dwStyle,rect,pParentWnd, nID,pContext);
}

/////////////////////////////////////////////////////////////////////////////
// CAssocMatrixView diagnostics

#ifdef _DEBUG
void CAssocMatrixView::AssertValid() const
{
	CView::AssertValid();
}

void CAssocMatrixView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAssocMatrixView message handlers

void CAssocMatrixView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
        ::GetScrollInfo(hWndScroll, SB_HORZ, &info);

        nPos = info.nTrackPos;
    }
	// The area to scroll is the client area to the right of the left labels	
	long nScrollAmount = 0;
	CRect rectClient;
	CRect rectScroll;
	GetClientRect(&rectClient);
	//rectScroll.SetRect(CXCOLUMNWIDTH,15*CYLINESPACING/10,rectClient.Width(),rectClient.Height());
	rectScroll.SetRect(this->m_AssocMxMetrics.cx1stColumnWidth,15*CYLINESPACING/10,rectClient.Width(),rectClient.Height());

	switch(nSBCode)
	{
		case SB_LINELEFT:
			if ( m_idxLeftColumn > 0 )
			{
				m_idxLeftColumn--;
				nScrollAmount = CXCOLUMNWIDTH;
			}
			break;
		
		case SB_LINERIGHT:
			if ( m_idxLeftColumn < m_pMatrix->m_aMatrix.GetSize()-1 )
			{
				m_idxLeftColumn++;
				nScrollAmount = -CXCOLUMNWIDTH;
			}
			break;

		case SB_PAGELEFT:
			while ( m_idxLeftColumn > 0 )
			{
				m_idxLeftColumn--;
				nScrollAmount += CXCOLUMNWIDTH;
					
				// If we've gone too far, back up one column
				if ( nScrollAmount > rectScroll.Width() )
				{
					nScrollAmount -= CXCOLUMNWIDTH;
					m_idxLeftColumn++;
					break;
				}
			}
			break;
		
		case SB_PAGERIGHT:
			while ( m_idxLeftColumn < m_pMatrix->m_aMatrix.GetSize()-1 )
			{
				m_idxLeftColumn++;
				nScrollAmount += CXCOLUMNWIDTH;
					
				// If we've gone too far, back up one column
				if ( nScrollAmount > rectScroll.Width() )
				{
					nScrollAmount -= CXCOLUMNWIDTH;
					m_idxLeftColumn--;
					break;
				}
			}
			nScrollAmount = -nScrollAmount;
			break;
		
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			// TODO: Scroll window if less than one page, otherwise just repaint everything
			m_idxLeftColumn = (long)nPos;
			nScrollAmount=0;
			InvalidateRect(rectScroll,TRUE);
			UpdateWindow();
			break;

		default:
			break;		
	}

	if ( nScrollAmount != 0 )
	{
		//ScrollWindow(nScrollAmount,0,rectScroll,rectScroll);
		//UpdateWindow();
		RedrawWindow();
	}

	SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);
}

void CAssocMatrixView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	rectScroll.SetRect(0,m_AssocMxMetrics.yScrollTop,rectClient.Width(),rectClient.Height());
	
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
			if ( m_idxTopRow < m_pMatrix->m_aMatrix.GetSize()-1 )
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
			while ( m_idxTopRow < m_pMatrix->m_aMatrix.GetSize()-1 )
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
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			m_idxTopRow = (long)nPos;
			nScrollAmount=0;
		//	InvalidateRect(rectScroll,TRUE);
			UpdateWindow();
			break;

		default:
			break;		
	}

	if ( nScrollAmount != 0 )
	{
		//ScrollWindow(0,nScrollAmount,rectScroll,rectScroll);
		//ScrollWindow(0,nScrollAmount,rectScroll,rectScroll);
		//UpdateWindow();
		RedrawWindow();
	}
	SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
	
//	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

int CAssocMatrixView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_psd = NULL;

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_menu.LoadMenu(IDR_TABLE);
	
	// TODO: Add your specialized creation code here
	m_pDoc = GetDocument();
	ASSERT_VALID(m_pDoc);

	CAssocSplitterWnd* pSplitterWnd = (CAssocSplitterWnd*)GetParentFrame();
	m_pMatrix = pSplitterWnd->m_pAssocMatrix;
	
	SetScrollRange(SB_VERT,0,m_pMatrix->m_aMatrix.GetSize()-1,TRUE);
	SetScrollRange(SB_HORZ,0,m_pMatrix->m_aMatrix.GetSize()-1,TRUE);

	m_idxLeftColumn = 0;
	m_idxTopRow = 0;

	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;
	
	//setup row column width

	return 0;
}

void CAssocMatrixView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	m_rect.left = pDC->GetDeviceCaps(PHYSICALOFFSETX);//150;//pInfo->m_rectDraw.left;
	m_rect.right = pDC->GetDeviceCaps(PHYSICALOFFSETX) + pDC->GetDeviceCaps(HORZRES);//1500;//pInfo->m_rectDraw.right;
	m_rect.top = pDC->GetDeviceCaps(PHYSICALOFFSETY);//120;//pInfo->m_rectDraw.top;
	m_rect.bottom = pDC->GetDeviceCaps(PHYSICALOFFSETY) + pDC->GetDeviceCaps(VERTRES);//1700;//pInfo->m_rectDraw.bottom;

	CView::OnPrepareDC(pDC, pInfo);
	SetMapping(pDC);
}

void CAssocMatrixView::SetMapping(CDC* pDC)
{
	pDC->SetMapMode(MM_TEXT);
	m_AssocMxMetrics.LogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	m_AssocMxMetrics.LogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
}

BOOL CAssocMatrixView::DestroyWindow() 
{
	return CView::DestroyWindow();
}

void CAssocMatrixView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	CRect rect,temp;
//	CPoint point;

//	m_psd->GetMargins(&rect,&temp);
//	point.x = m_psd->GetPaperSize().cx;
//	point.y = m_psd->GetPaperSize().cy;

	// default preparation
	long i,j;
	
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
	
	//USED 300 'CAUSE THAT'S WHAT IT WAS WRITTEN ON
//	m_rectDraw.left = (long)((rect.left)/((double) point.x)*m_HRes);
//	m_rectDraw.right = (long)((point.x - rect.right)/((double) point.x)*m_HRes);
//	m_rectDraw.top = (long)((rect.top)/((double) point.y)*m_VRes);
//	m_rectDraw.bottom = (long)((point.y - rect.bottom)/((double) point.y)*m_VRes);
	//////////////////////////////////////////////////////////////
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = m_HRes;
	m_rectDraw.bottom = m_VRes;

	//resize so more visible...
	m_AssocMxMetrics.cyLineSpacing *= (long)(2);//*m_VScale);
	m_AssocMxMetrics.cxColumnWidth *= (long)(2);//*m_HScale);
	m_AssocMxMetrics.cx1stColumnWidth *= (long)(2);
	m_tempheight = m_lfData.lfHeight;
	m_lfData.lfHeight = (long) (m_tempheight*m_HScale*2);	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	if(m_lfData.lfHeight >0) m_lfData.lfHeight *=-1;
	
	PageInfo *info;
	CArray<PageInfo*> tempcolPages,temprowPages;
	long count;

	if(m_pMatrix == &pDoc->m_AssocMatrixRows){
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

	//find column range & indexes
	long idx = -1;
	long startidx = idx;
	double xpos = CXCOLUMNWIDTH*m_HScale;
	xpos = this->m_AssocMxMetrics.cx1stColumnWidth*m_HScale;
	while(startidx < count-1){
		xpos += m_rectDraw.left + CXCOLUMNWIDTH*m_HScale;
		while(xpos <= m_rectDraw.right && idx < count-2){
			idx++;
			if(idx+1 < count-1)
				xpos += CXCOLUMNWIDTH*m_HScale;
		}
		info = new PageInfo;

		info->left = startidx;
		info->right = idx;
		tempcolPages.Add(info);

		startidx = idx+1;
		xpos = 0;
	}
	//find row range & indexes
	idx = -1;
	startidx = idx;
	double ypos = CYLINESPACING*6*m_VScale; //header info space
	while(startidx < count - 1){
		ypos += m_rectDraw.top + CYLINESPACING*m_VScale;
		while(ypos <= m_rectDraw.bottom  && idx < count-2){
			idx++;
			if(idx+1 < count)
				ypos += CYLINESPACING*m_VScale;
		}
		info = new PageInfo;

		info->top = startidx;
		info->bottom = idx;
		temprowPages.Add(info);

		startidx = idx+1;
		ypos = 0;
	}

	//create m_pageinfo structure for all pages
	m_pageinfo.SetSize(0);
	for(i=0;i<tempcolPages.GetSize();i++){
		for(j=0;j<temprowPages.GetSize();j++){
			info = new PageInfo;
			info->x = i;
			info->y = j;
			info->top = temprowPages[j]->top;
			info->bottom = temprowPages[j]->bottom;
			info->left = tempcolPages[i]->left;
			info->right = tempcolPages[i]->right;
			m_pageinfo.Add(info);
		}
	}
	m_printorder = i;

	pInfo->SetMaxPage(i*j);

	for(i=0;i<tempcolPages.GetSize();i++)
		delete tempcolPages[i];
	for(i=0;i<temprowPages.GetSize();i++)
		delete temprowPages[i];

	m_lfData.lfHeight = m_tempheight;
	if(m_lfData.lfHeight >0) m_lfData.lfHeight *=-1;
	m_AssocMxMetrics.cyLineSpacing /= (long)(2);//*m_VScale);
	m_AssocMxMetrics.cxColumnWidth /= (long)(2);//*m_HScale);
	m_AssocMxMetrics.cx1stColumnWidth /= (long)(2);
}


BOOL CAssocMatrixView::OnPreparePrinting(CPrintInfo* pInfo)
{
	m_pageinfo.SetSize(0);
	
	CPageSetupDialog * d =  new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	

	return DoPreparePrinting(pInfo);
}

void CAssocMatrixView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	long i;
	for(i=0;i<m_pageinfo.GetSize();i++)
		delete m_pageinfo.GetAt(i);
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;

	
}

void CAssocMatrixView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//current page being printed == pInfo->m_nCurSel
	long cPage = pInfo->m_nCurPage - 1;
	long i,j;
	CRect rect;
	CString text;
	CFont fontData;
	CFont*	pOldFont;

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(pDoc->m_rgbFontCells);

	//resize so more visible...
	m_AssocMxMetrics.cyLineSpacing *= (long)(2);//*m_VScale);
	m_AssocMxMetrics.cxColumnWidth *= (long)(2);//*m_HScale);
	m_AssocMxMetrics.cx1stColumnWidth *= (long)(2);
	long tempheight = m_lfData.lfHeight;
	m_lfData.lfHeight = (long) (m_lfData.lfHeight*m_HScale*2)/10;
	if(m_lfData.lfHeight >0) m_lfData.lfHeight *=-1;
	fontData.CreateFontIndirect(&m_lfData);
	m_lfData.lfHeight = tempheight;
	if(m_lfData.lfHeight >0) m_lfData.lfHeight *=-1;
	// select font into memory DC
	if ( (pOldFont = pDC->SelectObject(&fontData)) == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for table");
	}
	
	rect.left = rect.right = m_rectDraw.left;
	
	for(i=m_pageinfo.GetAt(cPage)->left;i<=m_pageinfo.GetAt(cPage)->right;i++){
		if(i == -1){
			rect.top = m_rectDraw.top+1;
			rect.bottom = rect.top + (long) (CYLINESPACING*m_VScale);
			if(cPage == 0){
				rect.left = m_rectDraw.left;
				rect.right = m_rectDraw.right;
				//draw header here
				if(m_pMatrix == &pDoc->m_AssocMatrixRows){
					text.Format("Data Table: %s",pDoc->m_TableName);
				}else{
					text.Format("Data Table: %s",pDoc->m_TableName);
				}

				pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
				pDC->ExtTextOut(rect.left,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);
				rect.top = rect.bottom+1;
				rect.bottom += (long)(CYLINESPACING*m_VScale);

				if(m_pMatrix == &pDoc->m_AssocMatrixRows){
					text.Format("Association Type: %s",pDoc->m_AssocMatrixRows.m_strMeasure);
				}else{
					text.Format("Association Type: %s",pDoc->m_AssocMatrixCols.m_strMeasure);
				}
				pDC->ExtTextOut(rect.left,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);
				rect.top = rect.bottom+1;
				rect.bottom += (long)(CYLINESPACING*m_VScale);

				if(m_pMatrix == &pDoc->m_AssocMatrixRows){
					text.Format("Created: %s",pDoc->m_AssocMatrixRows.m_Time);
				}else{
					text.Format("Created: %s",pDoc->m_AssocMatrixCols.m_Time);
				}
				pDC->ExtTextOut(rect.left,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);
				rect.top = m_rectDraw.top+1+(long)(CYLINESPACING*4*m_VScale);
				rect.bottom = rect.top + (long)(CYLINESPACING*2*m_VScale);	
			}else{
				rect.top = m_rectDraw.top+1+(long)(CYLINESPACING*5*m_VScale);
				rect.bottom = rect.top + (long)(CYLINESPACING*m_VScale);
			}
			rect.left = rect.right = m_rectDraw.left;
			rect.right += (long)(CXCOLUMNWIDTH*m_HScale);
		}else{
			rect.left = rect.right;
			rect.right += (long)(CXCOLUMNWIDTH*m_HScale);
			rect.top = m_rectDraw.top+1+(long)(CYLINESPACING*5*m_VScale);
			rect.bottom = rect.top + (long)(CYLINESPACING*m_VScale);
		}

		for(j=m_pageinfo.GetAt(cPage)->top;j<=m_pageinfo.GetAt(cPage)->bottom;j++){
			if(i == -1 && j != -1){
				//printrow title
				if(m_pMatrix == &pDoc->m_AssocMatrixRows){
					pDoc->GetRowLabel(j+1,text);
				}else{
					pDoc->GetColumnLabel(j+1,text);
				}

				//call a function to modify 'text' to fit
				// in width of 'rect.right - rect.left'
				pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
				//pDC->ExtTextOut((rect.right-rect.left)/2+rect.left,rect.bottom, ETO_CLIPPED, 
				//	rect,text,text.GetLength(),NULL);
				pDC->ExtTextOut(rect.left,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);

				//draw surrounding rect
				//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
			}else if(j == i-1){ //-1 && i != -1){
				//printcol title
				if(m_pMatrix == &pDoc->m_AssocMatrixRows){
					pDoc->GetRowLabel(i,text);
				}else{
					pDoc->GetColumnLabel(i,text);
				}

				//call a function to modify 'text' to fit
				// in width of 'rect.right - rect.left'
				pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
		
				//pDC->ExtTextOut((rect.right-rect.left)/2+rect.left,rect.bottom, ETO_CLIPPED, 
				//	rect,text,text.GetLength(),NULL);
				pDC->ExtTextOut(rect.right-2,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);
	
				//draw surroundiyng rect
				//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
			}else if(j != -1 && i != -1){
				//print cell contents
				
				if(j >= i) {
					//text.Format("%.2f",pDoc->GetValueAt(i,j));
					text.Format("%.4f",m_pMatrix->m_aMatrix.GetAt(j)->GetAt(i));
	
					//call a function to modify 'text' to fit
					// in width of 'rect.right - rect.left'
					pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
					pDC->ExtTextOut(rect.right-2,rect.bottom, ETO_CLIPPED,
						rect,text,text.GetLength(),NULL);
				
					//draw surrounding rect
					//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
				}
			}
			rect.top = rect.bottom;
			rect.bottom += (long)(CYLINESPACING*m_VScale);
		}
	}

	pDC->SelectObject(pOldFont);
	fontData.DeleteObject();

	m_AssocMxMetrics.cyLineSpacing /= (long)(2);//*m_VScale);
	m_AssocMxMetrics.cxColumnWidth /= (long)(2);//*m_HScale);
	m_AssocMxMetrics.cx1stColumnWidth /= (long) (2);
}


void CAssocMatrixView::OnOptionsFont() 
{
	// TODO: Add your command handler code here
	LOGFONT lf;
	memcpy(&lf,&(m_pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	lf.lfHeight /= FONTSCALING;
	
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	dlg.m_cf.Flags |= CF_LIMITSIZE;
	dlg.m_cf.nSizeMin = 3;
	dlg.m_cf.nSizeMax = 32;
	dlg.m_cf.rgbColors = m_pDoc->m_rgbFontCells;

	if ( dlg.DoModal() == IDOK )
	{
		lf.lfHeight *= FONTSCALING;
		memcpy(&(m_pDoc->m_lfCells),&lf,sizeof(LOGFONT));
		strcpy(m_pDoc->m_lfCells.lfFaceName,lf.lfFaceName);
		m_pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
		m_pDoc->m_nPointSizeCells = dlg.m_cf.iPointSize;///10; // iPointSize is in 1/10s
		m_pDoc->SetModifiedFlag(TRUE);
		Invalidate();
		UpdateWindow();

		POSITION pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			pView->RedrawWindow();
		} 
	}
}

void CAssocMatrixView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());	
}

void CAssocMatrixView::OnSaveas() 
{
	CAssociation casso(m_pDoc);
	if(m_pMatrix == &m_pDoc->m_AssocMatrixRows){
		casso.WriteAssocMatrixToASCII(CPatnDoc::rows);
	}else{
		casso.WriteAssocMatrixToASCII(CPatnDoc::columns);
	}
}

BOOL CAssocMatrixView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) return true;
	inMouseWheel = true;

	long x,y,diff;
	
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y + x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	long max = m_pMatrix->m_aMatrix.GetSize()-1;

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



