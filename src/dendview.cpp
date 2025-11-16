// dendview.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "fuseswnd.h"
#include "dendview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CXLOGMM (m_DendMetrics.LogPixelsX*10/254)
#define CYLOGMM (m_DendMetrics.LogPixelsY*10/254)
#define CYLINESPACING m_DendMetrics.cyLineSpacing
#define FONTSCALING m_DendMetrics.LogPixelsY/72.0
#define YSCROLLTOP m_DendMetrics.yScrollTop
#define WIDTH m_DendMetrics.width
#define HEIGHT m_DendMetrics.height
#define TOPMARGIN (15*CYLOGMM)
#define BOTTOMMARGIN (10*CYLOGMM)
#define LEFTMARGIN (25*CXLOGMM)
#define RIGHTMARGIN (25*CXLOGMM)
#define HEADERFOOTERGAP (10*CYLOGMM)
#define LINESPERPAGE m_DendMetrics.nNumLinesPerPage

/////////////////////////////////////////////////////////////////////////////
// CDendView

IMPLEMENT_DYNCREATE(CDendView, CViewExtension)

CDendView::CDendView()
{
	InitializeFonts();
	m_ControlIndicators = false;


}

CDendView::~CDendView()
{
	for(long i=0;i<m_controlindicator.GetSize();i++)
		delete m_controlindicator[i];
}

BEGIN_MESSAGE_MAP(CDendView, CViewExtension)
	//{{AFX_MSG_MAP(CDendView)
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__GROUPSONLY, OnGroupsonly)
	ON_COMMAND(ID__SAVEIMAGE, OnSaveimageas)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID__BWSYMBOLS, OnBwsymbols)
	ON_UPDATE_COMMAND_UI(ID__BWSYMBOLS, OnUpdateBwsymbols)
	ON_COMMAND(ID__CONTROLINDICATOR, OnControlindicator)
	ON_UPDATE_COMMAND_UI(ID__CONTROLINDICATOR, OnUpdateControlindicator)
	ON_COMMAND(ID__LABELS, OnLabels)
	ON_COMMAND(ID__LABELSSYMBOLS, OnLabelsSymbols)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID__FONT, OnOptionsFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDendView drawing

void CDendView::OnDraw(CDC* pDC)
{

	pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	CFuseLine* pFuseLine;
	CDendLine* pDendLine;
	CFont fontHeading, fontObjectLabel, fontScaleLabel;
	CFont *pOldFont;
	CBrush brushGrey,brushGrid,brushBlack;
	CBrush *pOldBrush;
	CBitmap bitmap;
	CPen penHighlight, penShadow, penBlack, penGroupLine, penEvenGroup, penOddGroup;
	CPen penColour;
	CPen penBorder,penThickBorder;	// used when printing
	CPen *pOldPen;
	COLORREF oldTextColor;
	CRect rClip;
	CRect rLabel;
	CString strLabel;
	char buffer[10];
	long nSize;	// size of fusion table
	long x,y;
	long i;
	long xStart,xRight,xEnd,yStart,yEnd;	// coords for each line	joining fused objects
	long nOldBkMode;
	long nLineWidth;

	long fi;
//	HDC met;

	CDC dcMem;
	CBitmap bmp;
	CString strPathName;
	CRect rClient;
	
	static long IN_OnDraw = FALSE;
	if(IN_OnDraw && !m_SaveImage) return;


	if(m_SaveImage){
	//	SaveIt(pDC);
	//	return;
		
		GetClientRect(&rClient);

		CFileDialog* pFileDlg;
		CStdioFile f;
		CFileException fe;
		CString strFileError;
		
		CString strTitle;

		strTitle = "Save Dendrogram:";

		// Get path for the new file
		pFileDlg = new CFileDialog(FALSE,"emf","*.emf",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|Bitmap - Reduced Width (*.bmp)|*.bmp|Enhanced Meta File (*.emf)|*.emf||",NULL);
		pFileDlg->m_ofn.nFilterIndex = 3;
		pFileDlg->m_ofn.lpstrTitle = strTitle;
		if(!IN_OnDraw)
		{
			if ( pFileDlg->DoModal() != IDOK ){
				delete pFileDlg;
				return;
			}
			fi = pFileDlg->m_ofn.nFilterIndex;
		}
		else
		{
			fi = 4;
		}
		strPathName = pFileDlg->GetPathName();

		if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
		else nSize = m_pDendOrder->GetSize();
		y = YSCROLLTOP + CYLINESPACING; // bottom of first label
		y += nSize * m_DendMetrics.cyLineSpacing;

		
		if(fi == 3)
		{
			IN_OnDraw = true;
			DrawToMetaFile(strPathName);
			
			return;
		}
		else if(fi == 2)
		{
			x = 340;
		}
		else if(fi == 4)
		{
			x = rClient.Width();
			if(x %2 == 1) x++;
		}
		else
		{
			x = max(340,80 + nSize * 6);
		}

		delete pFileDlg;

		if(y %2 == 1) y++;

		rClip.top = rClient.top = 0;
		rClip.bottom = rClient.bottom = y;
		rClip.left = rClient.left = 0;
		rClip.right = rClient.right = x; //340;

		m_DendMetrics.width = x;
		m_DendMetrics.height = y;
		
		long m_cxWidth  = rClient.Width();
		long m_cyHeight = rClient.Height();	
		long nWidth = m_cxWidth;	
		long nHeight = m_cyHeight;
		
		// ========================================================================
		
		if(fi == 3)
		{
			CDC * pDC = GetDC();
			CRect rr;
			GetClientRect(rr);
			rr.left = 0;
			rr.right = nWidth;
			rr.top = 0;
			rr.bottom = nHeight;
			pDC->FillSolidRect(rr,0x00ffffff);
			long iWidthMM = GetDeviceCaps(pDC->GetSafeHdc(), HORZSIZE);
			long iHeightMM = GetDeviceCaps(pDC->GetSafeHdc(), VERTSIZE);
			long iWidthPels = GetDeviceCaps(pDC->GetSafeHdc(), HORZRES);
			long iHeightPels = GetDeviceCaps(pDC->GetSafeHdc(), VERTRES);

			// Use iWidthMM, iWidthPels, iHeightMM, and iHeightPels to determine the
			// number of .01-millimeter units per pixel in the x and y directions.

			long iMMPerPelX = (iWidthMM * 100)/iWidthPels;
			long iMMPerPelY = (iHeightMM * 100)/iHeightPels;

			// Retrieve the coordinates of the client rectangle in pixels.
			// Convert client coordinates to .01-mm units. 
			long width = rr.Width();
			long height = rr.Height();

			rr.left = rr.left * iMMPerPelX;
			rr.top = rr.top * iMMPerPelY;
			rr.right = rr.right * iMMPerPelX;
			rr.bottom = rr.bottom * iMMPerPelY;

		//	pDC->TextOut(50,50,"hello");
			//pDC->LineTo(rr.right,rr.bottom);

			//met = CreateEnhMetaFile(pDC->GetSafeHdc(),strPathName,NULL,NULL);	

			//pDC = CDC::FromHandle(met);
		}
		else if(fi != 4)
		{
			dcMem.CreateCompatibleDC(pDC);
			bmp.CreateCompatibleBitmap(pDC,nWidth,nHeight);
			
			CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

			dcMem.BitBlt(0,0,nWidth,nHeight,pDC,0,0,WHITENESS);

			nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
			dcMem.SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);

			pDC = &dcMem;
		}
	}
	else{
		GetClientRect(rClip);
	//	InvalidateRect(rClip,FALSE);
	}

		IN_OnDraw = TRUE;


	//Check for updates
	if(m_pDoc->m_FuseColUpdate &&
		m_pFuseTable == &m_pDoc->m_FuseTableCols)
	{
		m_pDoc->m_FuseColChange = 0;
		m_pDoc->m_FuseColUpdate = 0;
		//redraw all windows
		POSITION pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			pView->RedrawWindow();
		} 
	}else if(m_pDoc->m_FuseRowUpdate &&
		m_pFuseTable == &m_pDoc->m_FuseTableRows)
	{
		m_pDoc->m_FuseRowChange = 0;
		m_pDoc->m_FuseRowUpdate = 0;
		//redraw all windows
		POSITION pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			pView->RedrawWindow();
		} 
	}

//	pDC->GetClipBox(&rClip);

	nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);

	// Calculate layout -------------------------------------------------------
	if ( !CalcMetrics(pDC))
	{
		IN_OnDraw = FALSE;
		return;
	}
	if(m_SaveImage){
		rClip.top = rClient.top = 0;
		rClip.bottom = rClient.bottom = y;
		rClip.left = rClient.left = 0;
		rClip.right = rClient.right = x; //340;

		m_DendMetrics.width = x;
		m_DendMetrics.height = y;
		
	}

	//long x0 = CYLINESPACING*(m_pDoc->m_RowControlGroups.GetSize()*2+.5)*0.8;/*+m_DendMetrics.xObjectLabelsLeft +
	//	 m_DendMetrics.sizeObjectLabel.cx +	CXLOGMM;*/
	long x0;
	if(m_row && m_GroupsOnly && m_ControlIndicators)
		x0 = (long)(CYLINESPACING*(m_pDoc->m_RowControlGroups.GetSize()*2+.5)*0.8);
	else if(!m_row && m_GroupsOnly && m_ControlIndicators)
		x0 = (long)(CYLINESPACING*(m_pDoc->m_ColControlGroups.GetSize()*2+.5)*0.8);
	else
		x0 = m_DendMetrics.xObjectLabelsLeft + m_DendMetrics.sizeObjectLabel.cx + CXLOGMM;

	if(m_GroupsSymbols == 1){
		x0 = m_DendMetrics.xObjectLabelsLeft + CXLOGMM;
	}else if(m_GroupsSymbols == 2 || m_ControlIndicators){
		x0 += m_DendMetrics.xObjectLabelsLeft*4; 
	}

	const long x1 = (long)max(WIDTH - 7*CXLOGMM,x0 + m_DendMetrics.sizeObjectLabel.cx);
	const long h  = x1 - x0;
	// Set y pos of dend lines to mid-point of labels
	const long y0 = YSCROLLTOP + CYLINESPACING - m_DendMetrics.sizeObjectLabel.cy/2;
	// ========================================================================

	// Create GDI objects -----------------------------------------------------
	if ( pDC->IsPrinting() )
		nLineWidth = (long)(13 * CYLOGMM / 100); // (0.13mm)
	else
		nLineWidth = 0; // (1 pixel)
		
    if ( ! fontHeading.CreateFontIndirect(&m_lfHeading) )
	{
		IN_OnDraw = FALSE;
		return;
	}
    if ( ! fontScaleLabel.CreateFontIndirect(&m_lfScaleLabel) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		return;
	}
    if ( ! fontObjectLabel.CreateFontIndirect(&m_pDoc->m_lfCells) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		return;
	}
	if ( ! brushGrey.CreateSolidBrush(RGB(192,192,192)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		return;
	}
	if ( ! brushBlack.CreateSolidBrush(RGB(0,0,0)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		return;
	}

	WORD aPattern[] = {0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF};
	if ( ! bitmap.CreateBitmap(8,8,1,1,aPattern) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		return;
	}
	if ( ! brushGrid.CreatePatternBrush(&bitmap) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		return;
	}

	if ( ! penHighlight.CreatePen(PS_SOLID,0,RGB(255,255,255)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		return;
	}
	if ( ! penShadow.CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		return;
	}
	if ( ! penBlack.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		return;
	}
	if ( ! penEvenGroup.CreatePen(PS_SOLID,nLineWidth,RGB(250,100,0)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		return;
	}
	if ( ! penOddGroup.CreatePen(PS_SOLID,nLineWidth,RGB(0,100,250)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		return;
	}

	if ( ! penGroupLine.CreatePen(PS_DOT,nLineWidth,RGB(0,0,255)) )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		return;
	}
	if ( pDC->IsPrinting() )
	{
		if ( ! penBorder.CreatePen(PS_SOLID,(long)(CYLOGMM*50/100),RGB(0,0,0)) )
		{
			IN_OnDraw = FALSE;
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			return;
		}
		if ( ! penThickBorder.CreatePen(PS_SOLID,(long)(CYLOGMM),RGB(0,0,0)) )
		{
			IN_OnDraw = FALSE;
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			return;
		}
	}
	// ========================================================================
	
	// Paint heading ----------------------------------------------------------
 	if ( (pOldBrush = pDC->SelectObject(&brushGrey)) == NULL )
	{
		IN_OnDraw = FALSE;
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	if ( (pOldFont = pDC->SelectObject(&fontHeading)) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	if ( !pDC->IsPrinting() )
		pDC->PatBlt(0,0,WIDTH,m_DendMetrics.cyHeadingBar,PATCOPY);
	x  = m_DendMetrics.xObjectLabelsLeft;

	if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
	{
		if(m_pDoc->m_FuseRowChange)
		{
			pDC->TextOut(x,m_DendMetrics.yHeadingBottom,"Row Fusion Data Changed",23);
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}else
			pDC->TextOut(x,m_DendMetrics.yHeadingBottom,"Row Fusion Dendrogram",21);
	}else
	{
		if(m_pDoc->m_FuseColChange)
		{
			pDC->TextOut(x,m_DendMetrics.yHeadingBottom,"Column Fusion Data Changed",26);
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}else
			pDC->TextOut(x,m_DendMetrics.yHeadingBottom,"Column Fusion Dendrogram",24);
	}

	if ( pDC->IsPrinting() )
	{
		if (( pOldPen = pDC->SelectObject(&penThickBorder)) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		pDC->MoveTo(0,m_DendMetrics.cyHeadingBar);
		pDC->LineTo(WIDTH,m_DendMetrics.cyHeadingBar);
	}
	// ========================================================================

	// Paint scale bar rectangle ----------------------------------------------
	if ( !pDC->IsPrinting() )
	{
		pDC->PatBlt(0,m_DendMetrics.cyHeadingBar,WIDTH,m_DendMetrics.cyScaleBar,PATCOPY);
		y = m_DendMetrics.cyScaleBar + m_DendMetrics.cyHeadingBar;
		// Highlight
		if ( (pOldPen = pDC->SelectObject(&penHighlight)) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		pDC->MoveTo(0,y);
		pDC->LineTo(0, y - m_DendMetrics.cyScaleBar);
		pDC->LineTo(WIDTH-1, y - m_DendMetrics.cyScaleBar);
		// Shadow
		if ( pDC->SelectObject(&penShadow) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		pDC->LineTo(WIDTH-1,y);
		pDC->LineTo(0,y);
	}
	// ========================================================================

	// All values will be range standardised to 0.1 - 1.0 ---------------------
	// Looks bad when 0.1-1.0, check it out when m_pFuseTable->GetAt(0)->nLevel > 0.5;
	// So to fix it, the new min is old min -0.1
	double oldmin, oldmax;
	double newmin, newmax;
	double oldrange, newrange;
	double interval;
	if(m_pFuseTable->GetSize() == 0)
		return;
	pFuseLine = m_pFuseTable->GetAt(0);
	if(m_GroupsOnly) oldmin = *m_pCutOff;
	else oldmin = pFuseLine->nLevel;
	pFuseLine = m_pFuseTable->GetAt(m_pFuseTable->GetUpperBound());
	oldmax = pFuseLine->nLevel;
	oldrange = oldmax - oldmin;
	if(m_GroupsOnly) newmin = 0.1;
	else newmin = 0.1;
	newmax = 1.0;
	newrange = newmax - newmin;
	interval = (h-newmin*h)/4;
	// ========================================================================

	// Draw scale lines & grid ------------------------------------------------
	if ( !pDC->IsPrinting() )
	{
		// Scale line is given 3-D effect by double writing with shadow and highlight pens
		if ( pDC->SelectObject(&penShadow) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
	}
	else
	{
		if ( pDC->SelectObject(&penBlack) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
	}
	if ( (pOldBrush = pDC->SelectObject(&brushGrid)) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	// scale line
	y = m_DendMetrics.yScaleLine;
	x =	(long)(x0 + newmin*h);
	pDC->MoveTo(x, y);
	pDC->LineTo(x1, y);

	m_cutoffmin = (long)(x0 + (newmin*h));
	m_cutoffmax = (long)(x0 + (newmin*h+4*interval));
	for ( i = 0; i < 5; i++ )
	{
		// scale mark
		x = (long)(x0 + (newmin*h+i*interval));
		pDC->MoveTo(x, y-2*CYLOGMM);
		if(!m_SaveImage)
			pDC->LineTo(x, y+2*CYLOGMM);
		else
			pDC->LineTo(x, rClient.bottom);
		
		// grid line
		if ( !pDC->IsPrinting() )	
			oldTextColor = pDC->SetTextColor(RGB(192,192,192));
		else
			oldTextColor = pDC->SetTextColor(RGB(0,0,0));
		pDC->PatBlt(x, YSCROLLTOP,max(nLineWidth,1),HEIGHT,PATCOPY);
		pDC->SetTextColor(oldTextColor);
	}

	if ( !pDC->IsPrinting() )
	{
		// Scale line is given 3-D effect by double writing with shadow and highlight pens
		if ( pDC->SelectObject(&penHighlight) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		// scale line
		x =	(long)(x0 + newmin*h);
		pDC->MoveTo(x+1, y+1);
		pDC->LineTo(x1+1, y+1);
		for ( i = 0; i < 5; i++ )
		{
			// scale mark
			x = (long)(x0 + (newmin*h+i*interval));
			pDC->MoveTo(x+1, y-2*CYLOGMM);
			pDC->LineTo(x+1, y+2*CYLOGMM);	
		}
	}
	// ========================================================================

	// Draw scale bar labels --------------------------------------------------
	if ( pDC->SelectObject(&fontScaleLabel) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	y = m_DendMetrics.yScaleLabels;
	x =	(long)(x0 + newmin*h + m_DendMetrics.sizeScaleLabel.cy/2);
	sprintf(buffer,"%0.4f",oldmin);
	if ( !pDC->IsPrinting() )
	{
		// Scale bar labels are given 3-D effect by double writing with shadow and highlight
		oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		pDC->TextOut(x,y,buffer,strlen(buffer));
		pDC->SetTextColor(RGB(255,255,255));
		pDC->TextOut(x+1,y+1,buffer,strlen(buffer));
	}
	else
	{
		oldTextColor = pDC->SetTextColor(RGB(0,0,0));
		pDC->TextOut(x,y,buffer,strlen(buffer));
	}
	pDC->SetTextColor(oldTextColor);
	for ( i = 1; i < 4; i++ )
	{
		x = (long)(x0 + (newmin*h+i*interval) + m_DendMetrics.sizeScaleLabel.cy/2);
		sprintf(buffer,"%0.4f",oldmin + oldrange/4*i);
		if ( !pDC->IsPrinting() )
		{
			// Scale bar labels are given 3-D effect by double writing with shadow and highlight
			oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
			pDC->TextOut(x,y,buffer,strlen(buffer));
			pDC->SetTextColor(RGB(255,255,255));
			pDC->TextOut(x+1,y+1,buffer,strlen(buffer));
		}
		else
		{
			oldTextColor = pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(x,y,buffer,strlen(buffer));
		}
		pDC->SetTextColor(oldTextColor);
	}
	x =	x1 + m_DendMetrics.sizeScaleLabel.cy/2;
	sprintf(buffer,"%0.4f",oldmax);
	if ( !pDC->IsPrinting() )
	{
		// Scale bar labels are given 3-D effect by double writing with shadow and highlight
		oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		pDC->TextOut(x,y,buffer,strlen(buffer));
		pDC->SetTextColor(RGB(255,255,255));
		pDC->TextOut(x+1,y+1,buffer,strlen(buffer));
	}
	else
	{
		oldTextColor = pDC->SetTextColor(RGB(0,0,0));
		pDC->TextOut(x,y,buffer,strlen(buffer));
	}
	pDC->SetTextColor(oldTextColor);


	// ========================================================================

	// Paint objects & labels down left side ----------------------------------
 	if ( pDC->SelectObject(&brushGrey) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}\
	if ( pDC->SelectObject(&fontObjectLabel) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}

	if(m_GroupsOnly && m_ControlIndicators){
		//print control indicators
		x =	CYLINESPACING*2;
		CString str = "a priori Groups";
		if ( !pDC->IsPrinting() )
		{
			// Scale bar labels are given 3-D effect by double writing with shadow and highlight
			oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
			pDC->TextOut(x,y,str,str.GetLength());
			pDC->SetTextColor(RGB(255,255,255));
			pDC->TextOut(x+1,y+1,str,str.GetLength());
		}
		else
		{
			oldTextColor = pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(x,y,str,str.GetLength());
		}
		pDC->SetTextColor(oldTextColor);

		//print symbols
		long xoff = 0;
		y += CYLINESPACING;
		long j;
		if(m_row){
			for(j=1;j<m_pDoc->m_RowControlGroups.GetSize() && j<m_pDoc->m_PatternApriori.GetSize();j++){
				m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j-1+1],pDC->GetSafeHdc(),
					(long)(x+CYLINESPACING*(xoff+0.5)*0.8),
					y-CYLINESPACING*3/4,
					1*CYLINESPACING/14);
				xoff += 2;
			}
		}else{
			for(j=1;j<m_pDoc->m_ColControlGroups.GetSize() && j<=m_pDoc->m_PatternGroups.GetSize();j++){
				m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j-1],pDC->GetSafeHdc(),
					(long)(x+CYLINESPACING*(xoff+0.5)*0.8),
					y-CYLINESPACING*3/4,
					1*CYLINESPACING/14);
				xoff += 2;
			}
		}		
	}

	oldTextColor = pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	long idx, next;
	if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
	else nSize = m_pDendOrder->GetSize();
	x = m_DendMetrics.xObjectLabelsLeft;
	y = YSCROLLTOP + CYLINESPACING; // bottom of first label
	rLabel.left  = x;
	rLabel.right = rLabel.left + m_DendMetrics.sizeObjectLabel.cx;

		// Draw dendrogram lines --------------------------------------------------
	if ( pDC->SelectObject(&penBlack) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	// everything above has already been painted, so prevent dend lines from going too far up
	//rClip.top = max(rClip.top,YSCROLLTOP);
	rClip.top = y0;
	CRect rct;
	if(m_SaveImage){
		rClip.top = y0 - m_DendMetrics.cyLineSpacing/2;
		if(fi != 3)
			pDC->IntersectClipRect(&rClip);
	}else{
		
		GetClientRect(rct);
		rct.top = y0 - m_DendMetrics.cyLineSpacing/2;
		
		if(pDC->IsPrinting())
		{
			rct = m_rectDraw;
			rClip = m_rectDraw;
			rct.top = y0 - m_DendMetrics.cyLineSpacing/2;
		}
		pDC->IntersectClipRect(&rct);
	}



	if(m_GroupsOnly) nSize = m_pGroupDendTable->GetSize();
	else nSize = m_pDendTable->GetSize();

	long nOrigin;
	if ( pDC->IsPrinting() )
		nOrigin = m_idxTopPrintRow;
	else
		nOrigin = m_idxTopRow;
	if(m_GroupsOnly) nOrigin++;
	long Right = -1;
	bool done = false;

	//pDC->SelectObject(&penBlack);

	for ( i = 0; i < nSize; i++ )
	{
		if(m_GroupsOnly){
			pDendLine = m_pGroupDendTable->GetAt(i);
		}else{
			pDendLine = m_pDendTable->GetAt(i);			
		}

		if ( pDendLine->xStart > 0 )
		{
			xStart = (long)(((pDendLine->xStart - oldmin)*newrange/oldrange + newmin)*h + x0);
		}
		else if( pDendLine->xStart == 0 && pDendLine->yStart > (long) pDendLine->yStart)
		{
			xStart = (long)(x0 + newmin*h);
		}
		else
		{
			//xStart = x0;
			xStart = (long)(x-CXLOGMM+CYLINESPACING*1.5);
		}
		xRight = (long)(((pDendLine->xRight - oldmin)*newrange/oldrange + newmin)*h + x0);
		if ( pDendLine->xEnd > 0 )
		{
			xEnd = (long)(((pDendLine->xEnd - oldmin)*newrange/oldrange + newmin)*h + x0);
		}
		else if(pDendLine->xEnd == 0 && pDendLine->yEnd > (long) pDendLine->yEnd)
		{
			xEnd = (long)(x0 + newmin*h);
		}
		else
		{
			//xEnd = x0;
			xEnd = (long)(x-CXLOGMM+CYLINESPACING*1.5);
		}

		yStart = (long)((pDendLine->yStart-nOrigin)*m_DendMetrics.cyLineSpacing + y0);
		yEnd   = (long)((pDendLine->yEnd-nOrigin)*m_DendMetrics.cyLineSpacing + y0);

		if(!m_SaveImage && (yEnd < rct.top || yStart > rct.bottom))
			continue;

		long j1,j2;
		//alternate colour of differing groups 
		if(!pDC->IsPrinting()){			
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				//m_pDoc->GetRowLabel(m_pDendOrder->GetAt(i),strLabel);
				j1 = m_pDoc->GetRowGroups(m_pDendOrder->GetAt((long)pDendLine->yStart));
				j2 = m_pDoc->GetRowGroups(m_pDendOrder->GetAt((long)pDendLine->yEnd));
			}else{
				j1 = m_pDoc->GetColumnGroups(m_pDendOrder->GetAt((long)pDendLine->yStart));
				j2 = m_pDoc->GetColumnGroups(m_pDendOrder->GetAt((long)pDendLine->yEnd));
			}
		}
		if(m_GroupsOnly || pDC->IsPrinting()){
			pDC->SelectObject(&penBlack);
			pDC->MoveTo(xStart,yStart);
			pDC->LineTo(xRight,yStart);
			pDC->LineTo(xRight,yEnd);
			pDC->LineTo(xEnd-1,yEnd);
		}else if(j1 == j2){
			if(pDendLine->xStart > *m_pCutOff){
				pDC->SelectObject(&penBlack);
			}else{				
				if(j1%2 == 0 || m_pFuseTable == &m_pDoc->m_FuseTableRows){
					if(j1-1 >= 0 && j1-1 < m_pDoc->m_aGroupColours.GetSize()){
						penColour.DeleteObject();
						penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
							255*(m_pDoc->m_aGroupColours[j1-1].red),
							255*(m_pDoc->m_aGroupColours[j1-1].green),
							255*(m_pDoc->m_aGroupColours[j1-1].blue)));
						pDC->SelectObject(&penColour);
					}else{
						penColour.DeleteObject();
						penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
					}
					if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
						pDC->SelectObject(&penEvenGroup);
				}else{
					if(j2-1 >= 0 && j2-1 < m_pDoc->m_aGroupColours.GetSize()){
						penColour.DeleteObject();
						penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
							255*(m_pDoc->m_aGroupColours[j2-1].red),
							255*(m_pDoc->m_aGroupColours[j2-1].green),
							255*(m_pDoc->m_aGroupColours[j2-1].blue)));
						pDC->SelectObject(&penColour);
					}else{
						penColour.DeleteObject();
						penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
					}
					if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
						pDC->SelectObject(&penOddGroup);
				}				
			}
			pDC->MoveTo(xStart,yStart);
			pDC->LineTo(xRight,yStart);
			pDC->LineTo(xRight,yEnd);
			pDC->LineTo(xEnd-1,yEnd);
			if(pDendLine->xStart <= *m_pCutOff){
				pDC->SelectObject(&penBlack);
				penColour.DeleteObject();
			}
		}else{
			if(pDendLine->xStart > *m_pCutOff){
				pDC->SelectObject(&penBlack);
			}else if(j1%2 == 0 || m_pFuseTable == &m_pDoc->m_FuseTableRows){
				if(j1-1 >= 0 && j1-1 < m_pDoc->m_aGroupColours.GetSize()){
					penColour.DeleteObject();
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
						255*(m_pDoc->m_aGroupColours[j1-1].red),
						255*(m_pDoc->m_aGroupColours[j1-1].green),
						255*(m_pDoc->m_aGroupColours[j1-1].blue)));
					pDC->SelectObject(&penColour);
				}else{
					penColour.DeleteObject();
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
				}
				if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
					pDC->SelectObject(&penEvenGroup);
			}else{
				if(j2-1 >= 0 && j2-1 < m_pDoc->m_aGroupColours.GetSize()){
					penColour.DeleteObject();
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
						255*(m_pDoc->m_aGroupColours[j2-1].red),
						255*(m_pDoc->m_aGroupColours[j2-1].green),
						255*(m_pDoc->m_aGroupColours[j2-1].blue)));
					pDC->SelectObject(&penColour);
				}else{
					penColour.DeleteObject();
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
				}
				if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
					pDC->SelectObject(&penOddGroup);
			}
			pDC->MoveTo(xStart,yStart);
			pDC->LineTo(xRight,yStart);
			pDC->SelectObject(&penBlack);
			pDC->LineTo(xRight,yEnd);
			if(pDendLine->xEnd > *m_pCutOff){
				pDC->SelectObject(&penBlack);
			}else if(j2%2 != 0 || m_pFuseTable == &m_pDoc->m_FuseTableRows){
				penColour.DeleteObject();
				if(j2-1 >= 0 && j2-1 < m_pDoc->m_aGroupColours.GetSize()){
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
						255*(m_pDoc->m_aGroupColours[j2-1].red),
						255*(m_pDoc->m_aGroupColours[j2-1].green),
						255*(m_pDoc->m_aGroupColours[j2-1].blue)));
					pDC->SelectObject(&penColour);
				}else{
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
				}
				if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
					pDC->SelectObject(&penOddGroup);
			}else{
				penColour.DeleteObject();
				if(j1-1 >= 0 && j1-1 < m_pDoc->m_aGroupColours.GetSize()){
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(
						255*(m_pDoc->m_aGroupColours[j1-1].red),
						255*(m_pDoc->m_aGroupColours[j1-1].green),
						255*(m_pDoc->m_aGroupColours[j1-1].blue)));
					pDC->SelectObject(&penColour);
				}else{
					penColour.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0));
				}
				if(m_pFuseTable != &m_pDoc->m_FuseTableRows)
					pDC->SelectObject(&penEvenGroup);
			}
			pDC->LineTo(xEnd-1,yEnd);

			if(pDendLine->xStart <= *m_pCutOff){
				pDC->SelectObject(&penBlack);
				penColour.DeleteObject();
			}
		}

		//to fix up m_pCutOff !!!
		if(!done && xRight == (long)(((*m_pCutOff - oldmin)*newrange/oldrange + newmin)*h + x0)){
			Right = 0;
			Right -= xRight;
		}else if(!done && Right != -1){
			Right += xRight;
			Right /= 2;
			done = true;
		}
	}
	// ========================================================================

	if ( pDC->IsPrinting() )
		i = m_idxTopPrintRow;
	else
		i = m_idxTopRow;
	if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
	else nSize = m_pDendOrder->GetSize();
	for ( ; i < nSize; i++ )
	{
		if ( y < rClip.top )
		{
			y += m_DendMetrics.cyLineSpacing;
			continue;
		}

		if(m_GroupsOnly){
			next = m_pGroupDendOrder->GetAt(i);
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				m_pDoc->GetRowLabel(m_pDoc->m_Rows.GetAt(next)->GetAt(0),strLabel);
				if(m_GroupsSymbols && m_pDoc->m_Rows.GetSize() > next &&
					m_pDoc->m_Rows[next]){
					if(m_GroupsSymbols == 2)
					{
						strLabel.Format("%d, %s",m_pDoc->m_Rows[next]->GetSize(), strLabel);
					}
					else
					{
						strLabel.Format("%d",m_pDoc->m_Rows[next]->GetSize());
					}
				}
			}else{
				m_pDoc->GetColumnLabel(m_pDoc->m_Cols.GetAt(next)->GetAt(0),strLabel);
				if(m_GroupsSymbols && m_pDoc->m_Cols.GetSize() > next &&
					m_pDoc->m_Cols[next]){
					if(m_GroupsSymbols == 2)
					{
						strLabel.Format("%d, %s",m_pDoc->m_Cols[next]->GetSize(), strLabel);
					}
					else
					{
						strLabel.Format("%d",m_pDoc->m_Cols[next]->GetSize());
					}
				}
			}
		}else{
			next = m_pDendOrder->GetAt(i);
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
				m_pDoc->GetRowLabel(next,strLabel);
			else
				m_pDoc->GetColumnLabel(next,strLabel);
		}
		
		if(m_GroupsOnly && m_ControlIndicators && 
			next < m_controlindicator.GetSize() &&
			next < m_pDoc->m_PatternGroups.GetSize()){
			//draw control label/evaluated group counts

			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			//pDC->ExtTextOut((long)(x+CYLINESPACING*1.5),y,ETO_CLIPPED,&rLabel,strLabel,strLabel.GetLength(),NULL);
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[next],pDC->GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);

			pDC->PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
					(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);

			long xoff = 0;
			for(long j=1;j<m_controlindicator[next]->GetSize();j++){
				if(m_controlindicator[next]->GetAt(j) == 0){
					xoff+=2;
					continue;
				}

				strLabel.Format("%d",m_controlindicator[next]->GetAt(j));
				pDC->ExtTextOut((long)(x+CYLINESPACING*(2 + xoff)*0.8),y,NULL/*ETO_CLIPPED*/,&rLabel,strLabel,strLabel.GetLength(),NULL);
				xoff += 2;
			}

		}else if(m_GroupsOnly && m_GroupsSymbols){
			//draw symbol
				
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			if(m_GroupsSymbols == 2){
				pDC->PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
					(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
				pDC->ExtTextOut((long)(x+CYLINESPACING*1.5),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
			}else{
				pDC->PatBlt((long)(x-CXLOGMM+CYLINESPACING*2.5),y-m_DendMetrics.sizeObjectLabel.cy,
					(long)(x0-(x-CXLOGMM+CYLINESPACING*2.0)),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
				pDC->ExtTextOut((long)(x+CYLINESPACING*1.0),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
			}
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[next],pDC->GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
		}else if((m_GroupsSymbols || m_ControlIndicators) && !m_GroupsOnly){
			
			
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			if(m_ControlIndicators || m_GroupsSymbols == 2){
				pDC->PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
					(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
				pDC->ExtTextOut((long)(x+CYLINESPACING*1.5),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
			}

			if(m_GroupsSymbols && next < m_pDoc->m_PatternObjects.GetSize())
			{
				if(m_pDoc->m_PatternObjects[next] != -1)
					m_tex.GDIPatterns(m_pDoc->m_PatternObjects[next],pDC->GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
				else
					m_tex.GDIPatterns(next,pDC->GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
			}
			else
			{
				//what apriori group does it belong to?
				for(long k=0;k<m_pDoc->m_RowControlGroups.GetSize();k++){
					for(long m=0;m<m_pDoc->m_RowControlGroups[k]->GetSize();m++){
						if(m_pDoc->m_RowControlGroups[k]->GetAt(m) == next)
							break;
					}
					if(m != m_pDoc->m_RowControlGroups[k]->GetSize())
						break;
				}
				if(k != m_pDoc->m_RowControlGroups.GetSize())
					m_tex.GDIPatterns(m_pDoc->m_PatternApriori[k],pDC->GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
			}
		}else{
			pDC->PatBlt(x-CXLOGMM,y-m_DendMetrics.sizeObjectLabel.cy,
					x0-(x-CXLOGMM),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
			
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;
			
			pDC->ExtTextOut(x,y,NULL /*ETO_CLIPPED*/ ,&rLabel,strLabel,strLabel.GetLength(),NULL);
		}

		if ( y > rClip.bottom  && !m_SaveImage)
			break;

		y += CYLINESPACING;
		idx = next;
	}
	pDC->SetTextColor(oldTextColor);
	// ========================================================================

	

 

	// Draw line showing where requested no groups occurred -------------------
	if(!pDC->IsPrinting()){
		if ( pDC->SelectObject(&penGroupLine) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		long nOldROP2;
		if ( !pDC->IsPrinting() )
			nOldROP2 = pDC->SetROP2(R2_NOTXORPEN);
		else
			nOldROP2 = pDC->SetROP2(R2_COPYPEN);
		x = (long)(((*m_pCutOff - oldmin)*newrange/oldrange + newmin)*h + x0)
			;//+ Right;
		m_cutoffX = x;
		m_cutoffoldmin = oldmin;
		m_cutoffoldrange = oldrange;
		m_cutoffnewrange = newrange;
		m_cutoffnewmin = newmin;
		m_cutoffx0 = x0;
		m_cutoffh = h;
		y = YSCROLLTOP;
		pDC->MoveTo(x,y);
		y = HEIGHT;
		pDC->LineTo(x,y);
		pDC->SetROP2(nOldROP2);
	}
	// ========================================================================

	// Draw terminating marker on last fusion ---------------------------------
	if ( pDC->SelectObject(&penBlack) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	if ( pDC->SelectObject(&brushBlack) == NULL )
	{
		IN_OnDraw = FALSE;
		if(!pDC->IsPrinting()){
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldFont);
			pDC->SelectObject(pOldPen);
		}
		fontHeading.DeleteObject();
		fontScaleLabel.DeleteObject();
		fontObjectLabel.DeleteObject();
		brushGrey.DeleteObject();
		brushBlack.DeleteObject();
		bitmap.DeleteObject();
		brushGrid.DeleteObject();
		penHighlight.DeleteObject();
		penShadow.DeleteObject();
		penBlack.DeleteObject();
		penEvenGroup.DeleteObject();
		penOddGroup.DeleteObject();
		penGroupLine.DeleteObject();
		if(pDC->IsPrinting()) penBorder.DeleteObject();
		if(pDC->IsPrinting()) penThickBorder.DeleteObject();
		return;
	}
	x =	xRight;
	y = yStart + (yEnd-yStart)/2;
	pDC->MoveTo(x,y);
	x += 2*CXLOGMM;
	pDC->LineTo(x,y);
	POINT aPoints[] = { x,y, x+2*CXLOGMM,y-2*CYLOGMM, x+2*CXLOGMM,y+2*CYLOGMM };
	long oldPolyFillMode = pDC->SetPolyFillMode(WINDING);
	pDC->MoveTo(x,y);
	pDC->Polygon(aPoints,3);
	pDC->SetPolyFillMode(oldPolyFillMode);
	// ========================================================================

	// Draw border top and bottom ---------------------------------------------
	if ( pDC->IsPrinting() )
	{
		if ( pDC->SelectObject(&penBorder) == NULL )
		{
			IN_OnDraw = FALSE;
			if(!pDC->IsPrinting()){
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldFont);
				pDC->SelectObject(pOldPen);
			}
			fontHeading.DeleteObject();
			fontScaleLabel.DeleteObject();
			fontObjectLabel.DeleteObject();
			brushGrey.DeleteObject();
			brushBlack.DeleteObject();
			bitmap.DeleteObject();
			brushGrid.DeleteObject();
			penHighlight.DeleteObject();
			penShadow.DeleteObject();
			penBlack.DeleteObject();
			penEvenGroup.DeleteObject();
			penOddGroup.DeleteObject();
			penGroupLine.DeleteObject();
			if(pDC->IsPrinting()) penBorder.DeleteObject();
			if(pDC->IsPrinting()) penThickBorder.DeleteObject();
			return;
		}
		pDC->SelectObject(&penBlack);
		pDC->MoveTo(0,YSCROLLTOP);
		pDC->LineTo(WIDTH,YSCROLLTOP);
		pDC->MoveTo(0,HEIGHT);
		pDC->LineTo(WIDTH,HEIGHT);
	}
	//=========================================================================
	
	

	IN_OnDraw = FALSE;
	if(!pDC->IsPrinting()){
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldFont);
		pDC->SelectObject(pOldPen);
	}
	fontHeading.DeleteObject();
	fontScaleLabel.DeleteObject();
	fontObjectLabel.DeleteObject();
	brushGrey.DeleteObject();
	brushBlack.DeleteObject();
	bitmap.DeleteObject();
	brushGrid.DeleteObject();
	penHighlight.DeleteObject();
	penShadow.DeleteObject();
	penBlack.DeleteObject();
	penEvenGroup.DeleteObject();
	penOddGroup.DeleteObject();
	penGroupLine.DeleteObject();
	if(pDC->IsPrinting()) penBorder.DeleteObject();
	if(pDC->IsPrinting()) penThickBorder.DeleteObject();

	if(m_SaveImage){
		if(fi == 3 || fi == 4)
		{
		//	DeleteEnhMetaFile(CloseEnhMetaFile(met));
			m_SaveImage = false;
		}
		else
		{
			m_SaveImage = false;
			SaveBitmapFile(pDC->GetSafeHdc(), (HBITMAP) bmp, strPathName);

			DeleteDC(*pDC);
			bmp.DeleteObject();
		}
	}
	
}

void CDendView::InitializeFonts()
{
	m_nHeadingPointSize = 16;
	m_nScaleLabelPointSize = 9;
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

	m_lfScaleLabel.lfHeight = 0; // to be set when device context is prepared
	m_lfScaleLabel.lfWidth = 0;
	m_lfScaleLabel.lfEscapement = 900;
	m_lfScaleLabel.lfOrientation = 900;
	m_lfScaleLabel.lfWeight = FW_NORMAL;
	m_lfScaleLabel.lfItalic = FALSE;
	m_lfScaleLabel.lfUnderline = FALSE;
	m_lfScaleLabel.lfStrikeOut = 0;
	m_lfScaleLabel.lfCharSet = ANSI_CHARSET;
	m_lfScaleLabel.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfScaleLabel.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfScaleLabel.lfQuality = PROOF_QUALITY;
	m_lfScaleLabel.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
	strcpy(m_lfScaleLabel.lfFaceName, ((CPatnApp*)AfxGetApp())->m_globalFontName);

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

BOOL CDendView::CalcMetrics(CDC* pDC)
{
	CFont font;
	TEXTMETRIC tm;
	
	CFuseSplitterWnd* pSplitterWnd = (CFuseSplitterWnd*)GetParentFrame();
	m_pFuseTable = pSplitterWnd->m_pFuseTable;
	m_pDendTable = pSplitterWnd->m_pDendTable;
	m_pDendOrder = pSplitterWnd->m_pDendOrder;
	m_pGroupDendOrder = pSplitterWnd->m_pGroupDendOrder;
	m_pGroupDendTable = pSplitterWnd->m_pGroupDendTable;
 	m_pCutOff = pSplitterWnd->m_pCutOff;  // TODO: temp code for no of groups line

	if ( !pDC->IsPrinting() )
	{
		// if printing, then the width & height are set in OnPrint() based on m_rectDraw
		CRect rClient;
		GetClientRect(&rClient);
		m_DendMetrics.width = rClient.Width();
		m_DendMetrics.height = rClient.Height();
	}

	// Header & footer (printing) ---------------------------------------------
	if ( pDC->IsPrinting() )
	{
		m_lfHeaderFooter.lfHeight = (long)(-m_nHeaderFooterPointSize * FONTSCALING );
	    if ( ! font.CreateFontIndirect(&m_lfHeaderFooter) )
			return FALSE;
		if ( pDC->SelectObject(&font) == NULL )
			return FALSE;
		pDC->GetTextMetrics(&tm);
		m_DendMetrics.cyHeader = tm.tmHeight + TOPMARGIN + HEADERFOOTERGAP;
		m_DendMetrics.cyFooter = tm.tmHeight + BOTTOMMARGIN + HEADERFOOTERGAP;

		if ( !font.DeleteObject() )
			return FALSE;
	}
	// ========================================================================

	m_DendMetrics.xObjectLabelsLeft = 4 * CXLOGMM;

	// Heading bar ------------------------------------------------------------
	if(pDC->IsPrinting()) 
		m_lfHeading.lfHeight = (long)(-m_nHeadingPointSize * FONTSCALING);
	else
		m_lfHeading.lfHeight = (long)(-m_nHeadingPointSize * FONTSCALING);
    if ( ! font.CreateFontIndirect(&m_lfHeading) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )
		return FALSE;
	pDC->GetTextMetrics(&tm);
	m_DendMetrics.cyHeadingBar = (long)(tm.tmHeight*1.5);
	m_DendMetrics.yHeadingBottom = m_DendMetrics.cyHeadingBar -
								   (m_DendMetrics.cyHeadingBar - tm.tmHeight)/2;

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	// Scale Bar --------------------------------------------------------------
	if(pDC->IsPrinting())
		m_lfScaleLabel.lfHeight = (long)(-m_nScaleLabelPointSize * FONTSCALING);
	else
		m_lfScaleLabel.lfHeight = (long)(-m_nScaleLabelPointSize * FONTSCALING);
    if ( ! font.CreateFontIndirect(&m_lfScaleLabel) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )
		return FALSE;

	// An extra decimal digit is used in following to produce gap to edge of scale bar
	m_DendMetrics.sizeScaleLabel = pDC->GetTextExtent("8.88888", 7);
	m_DendMetrics.cyScaleBar = m_DendMetrics.sizeScaleLabel.cx + 6*CYLOGMM;
	m_DendMetrics.yScaleLine = m_DendMetrics.cyHeadingBar +
							   m_DendMetrics.cyScaleBar -
							   3*CYLOGMM;
	m_DendMetrics.yScaleLabels = m_DendMetrics.cyHeadingBar +
								 m_DendMetrics.sizeScaleLabel.cx;

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	m_DendMetrics.yScrollTop = m_DendMetrics.cyHeadingBar +	m_DendMetrics.cyScaleBar + 1;

	// Objects ----------------------------------------------------------------
	//if(pDC->IsPrinting())
		//m_pDoc->m_lfCells.lfHeight = (long)(-m_pDoc->m_nPointSizeCells * FONTSCALING);
	//else
	//	m_pDoc->m_lfCells.lfHeight = -m_pDoc->m_nPointSizeCells * FONTSCALING;
    if ( ! font.CreateFontIndirect(&m_pDoc->m_lfCells) )
		return FALSE;
	if ( pDC->SelectObject(&font) == NULL )
		return FALSE;

	pDC->GetTextMetrics(&tm);
	m_DendMetrics.cyLineSpacing = (long)(tm.tmHeight*1.2);
	long maxsize = 0;
	long count = 0;

	if(m_pFuseTable == &m_pDoc->m_FuseTableRows){		
		count = m_pDoc->GetNoOfRows();
	}else{
		count = m_pDoc->GetNoOfColumns();
	}

	m_DendMetrics.sizeObjectLabel = pDC->GetTextExtent("WWWWA0000",9);

	for(long i=0;i<count;i++){	
		CString tmp;
		if(m_pFuseTable == &m_pDoc->m_FuseTableRows){		
			m_pDoc->GetRowLabel(i,tmp);
		}else{
			m_pDoc->GetColumnLabel(i,tmp);
		}
		CSize sz = pDC->GetTextExtent(tmp);
		if(sz.cx > m_DendMetrics.sizeObjectLabel.cx){
			m_DendMetrics.sizeObjectLabel.cx = sz.cx;
		}
		if(sz.cy > m_DendMetrics.sizeObjectLabel.cy){
			m_DendMetrics.sizeObjectLabel.cy = sz.cy;
		}
	}	

	if ( !font.DeleteObject() )
		return FALSE;
	// ========================================================================

	return TRUE;
}

BOOL CDendView::Create(LPCSTR lpszClassName,
	LPCSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext /* = NULL */)
{
	dwStyle |= WS_VSCROLL;	// Add vertical scroll bar

	return CWnd::Create(lpszClassName,lpszWindowName, dwStyle,rect,pParentWnd, nID,pContext);
}

/////////////////////////////////////////////////////////////////////////////
// CDendView diagnostics

#ifdef _DEBUG
void CDendView::AssertValid() const
{
	CView::AssertValid();
}

void CDendView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


#ifdef _DEBUG
CPatnDoc* CDendView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDendView message handlers

void CDendView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	CRect rClient;
	CRect rScroll;
	GetClientRect(&rClient);
	rScroll.SetRect(0,YSCROLLTOP,WIDTH,HEIGHT);
	
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
			if ( m_idxTopRow < m_pDendOrder->GetSize()-1 )
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
				if ( nScrollAmount > rScroll.Height() )
				{
					nScrollAmount -= CYLINESPACING;
					m_idxTopRow++;
					break;
				}
			}
			break;
		
		case SB_PAGEDOWN:
			while ( m_idxTopRow < m_pDendOrder->GetSize()-1 )
			{
				m_idxTopRow++;
				nScrollAmount += CYLINESPACING;
					
				// If we've gone too far, back up one column
				if ( nScrollAmount > rScroll.Height() )
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
			//InvalidateRect(rScroll,TRUE);
			//UpdateWindow();
			RedrawWindow();
			break;

		default:
			break;		
	}

	if ( nScrollAmount != 0 )
	{
		//ScrollWindow(0,nScrollAmount,rScroll,rScroll);
		//UpdateWindow();
		RedrawWindow();
	}
	SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
}

int CDendView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_psd = NULL;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_SaveImage = false;
	m_movingcutoff = false;

	m_pDoc = GetDocument();
	ASSERT_VALID(m_pDoc);

	m_menu.LoadMenu(IDR_MENU_DEND);
	m_menu.CheckMenuItem(ID__GROUPSONLY,MF_UNCHECKED);
	m_GroupsOnly = false;

	CFuseSplitterWnd* pSplitterWnd = (CFuseSplitterWnd*)GetParentFrame();
	m_pFuseTable = pSplitterWnd->m_pFuseTable;
	m_pDendTable = pSplitterWnd->m_pDendTable;
	m_pDendOrder = pSplitterWnd->m_pDendOrder;
	m_pGroupDendOrder = pSplitterWnd->m_pGroupDendOrder;
	m_pGroupDendTable = pSplitterWnd->m_pGroupDendTable;

 	m_pCutOff = pSplitterWnd->m_pCutOff;  // TODO: temp code for no of groups line

	m_idxTopRow = 0;
	SetScrollRange(SB_VERT,0,m_pDendOrder->GetSize()-1,TRUE);
	
	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;

	CString patterns;
	HRSRC rsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_TEXT1),"TEXT");
	HGLOBAL h = LoadResource(NULL,rsrc);
	char * c = (char*)h;
	long i=0;//loop until "eee" occurs - last entry
	while(!(c[i] == 'e' && c[i+1] == 'e' && c[i+2] == 'e')){
		patterns.Insert(i,c[i]);
		i++;
	}
	patterns.Insert(i,'e');

	if(!m_tex.LoadPatterns(0,patterns)){
	
	}

	m_GroupsSymbols = 0;

	m_row = (pSplitterWnd->m_IsRow)?true:false;

	//build control indicator information
	//build a list for each item of groupdendorder
	m_controlindicator.SetSize(m_pGroupDendOrder->GetSize());
	for(i=0;i<m_pGroupDendOrder->GetSize();i++){
		m_controlindicator.SetAt(i,new Rows);

		long rowidx = m_pGroupDendOrder->GetAt(i);

		if(m_row)
			m_controlindicator[i]->SetSize(m_pDoc->m_RowControlGroups.GetSize());
		else
			m_controlindicator[i]->SetSize(m_pDoc->m_ColControlGroups.GetSize());

		for(long j=0;j<m_controlindicator[i]->GetSize();j++){
			m_controlindicator[i]->SetAt(j,0);
		}
		
		if(m_row){
			//find m_Rows idx to identify group members		
			for(j=0;j<m_pDoc->m_Rows[rowidx]->GetSize();j++){
				long r_idx = m_pDoc->m_Rows[rowidx]->GetAt(j);

				//attempt to find the r_idx
				for(long ii = 0;ii<m_pDoc->m_RowControlGroups.GetSize();ii++){
					for(long jj=0;jj<m_pDoc->m_RowControlGroups[ii]->GetSize();jj++){
						if(m_pDoc->m_RowControlGroups[ii]->GetAt(jj) == r_idx){
							break;
						}
					}
					if(jj < m_pDoc->m_RowControlGroups[ii]->GetSize())
						break;
				}
				
				//if r_idx was found, set counter
				if(ii < m_pDoc->m_RowControlGroups.GetSize()){
					long value = m_controlindicator[i]->GetAt(ii);
					m_controlindicator[i]->SetAt(ii,value + 1);
				}
			}
		}else {
			//find m_Rows idx to identify group members		
			for(j=0;j<m_pDoc->m_Cols[rowidx]->GetSize();j++){
				long r_idx = m_pDoc->m_Cols[rowidx]->GetAt(j);

				//attempt to find the r_idx
				for(long ii = 0;ii<m_pDoc->m_ColControlGroups.GetSize();ii++){
					for(long jj=0;jj<m_pDoc->m_ColControlGroups[ii]->GetSize();jj++){
						if(m_pDoc->m_ColControlGroups[ii]->GetAt(jj) == r_idx){
							break;
						}
					}
					if(jj < m_pDoc->m_ColControlGroups[ii]->GetSize())
						break;
				}
				
				//if r_idx was found, set counter
				if(ii < m_pDoc->m_ColControlGroups.GetSize()){
					long value = m_controlindicator[i]->GetAt(ii);
					m_controlindicator[i]->SetAt(ii,value + 1);
				}
			}
		}
	}//finished building m_controlindicator (contains member counts [%d %d %d...] for groups 1->n.

	return 0;
}
void CDendView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{

	CView::OnPrepareDC(pDC, pInfo);

	SetMapping(pDC);
}

void CDendView::SetMapping(CDC* pDC)
{
	pDC->SetMapMode(MM_TEXT);
	m_DendMetrics.LogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
	m_DendMetrics.LogPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
}

void CDendView::SaveIt(CDC* pDC)
{
	CFuseLine* pFuseLine;
	CDendLine* pDendLine;
	CFont fontHeading, fontObjectLabel, fontScaleLabel;
	CFont *pOldFont;
	CBrush brushGrey,brushGrid,brushBlack;
	CBrush *pOldBrush;
	CBitmap bitmap;
	CPen penHighlight, penShadow, penBlack, penGroupLine;
	CPen penBorder,penThickBorder;	// used when printing
//	CPen *pOldPen;
	COLORREF oldTextColor;
	CRect rClip;
	CRect rLabel;
	CString strLabel;
	char buffer[10];
	long nSize;	// size of fusion table
	long x,y;
	long i;
	long xStart,xRight,xEnd,yStart,yEnd;	// coords for each line	joining fused objects
	long nOldBkMode;
	long nLineWidth;

	CRect rClient;
	GetClientRect(&rClient);

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	m_SaveImage = false;
	strTitle = "Save Dendrogram:";
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|Bitmap - Reduced Width (*.bmp)|*.bmp|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();

	if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
	else nSize = m_pDendOrder->GetSize();
	y = YSCROLLTOP + CYLINESPACING; // bottom of first label
	y += nSize * m_DendMetrics.cyLineSpacing;

	long fi = pFileDlg->m_ofn.nFilterIndex;
	if(fi == 2){
		x = 340;
	}else{
		x = max(340,80 + nSize * 6);
	}

	delete pFileDlg;

	rClip.top = rClient.top = 0;
	rClip.bottom = rClient.bottom = y;
	rClip.left = rClient.left = 0;
	rClip.right = rClient.right = x; //340;
	
	long m_cxWidth  = rClient.Width();
	long m_cyHeight = rClient.Height();	
	long nWidth = m_cxWidth;	
	long nHeight = m_cyHeight;
	
	// ========================================================================
	CDC dcMem;
	CBitmap bmp;

	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	
	CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

	dcMem.BitBlt(0,0,nWidth,nHeight,pDC,0,0,WHITENESS);

	nOldBkMode = dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);


	
	
	// Calculate layout -------------------------------------------------------
	if ( !CalcMetrics(pDC) )
	{
		return;
	}

//	const long x0 = m_DendMetrics.xObjectLabelsLeft +
//		 m_DendMetrics.sizeObjectLabel.cx +	CXLOGMM;
	long x0;
	if(m_row && m_GroupsOnly && m_ControlIndicators)
		x0 = (long)(CYLINESPACING*(m_pDoc->m_RowControlGroups.GetSize()*2+.5)*0.8);
	else if(!m_row && m_GroupsOnly && m_ControlIndicators)
		x0 = (long)(CYLINESPACING*(m_pDoc->m_ColControlGroups.GetSize()*2+.5)*0.8);
	else
		x0 = m_DendMetrics.xObjectLabelsLeft + m_DendMetrics.sizeObjectLabel.cx + CXLOGMM;

	if(m_GroupsSymbols == 1){
		x0 = m_DendMetrics.xObjectLabelsLeft + CXLOGMM;
	}else if(m_GroupsSymbols == 2 || m_ControlIndicators){
		x0 += m_DendMetrics.xObjectLabelsLeft*4; 
	}

	const long x1 = (long)max(rClient.Width() - 7*CXLOGMM,x0 + m_DendMetrics.sizeObjectLabel.cx);
	const long h  = x1 - x0;
	// Set y pos of dend lines to mid-point of labels
	const long y0 = YSCROLLTOP + CYLINESPACING - m_DendMetrics.sizeObjectLabel.cy/2;
	// ========================================================================

	// Create GDI objects -----------------------------------------------------
	nLineWidth = 0; // (1 pixel)

    if ( ! fontHeading.CreateFontIndirect(&m_lfHeading) )
	{
		return;
	}
    if ( ! fontScaleLabel.CreateFontIndirect(&m_lfScaleLabel) )
	{
		return;
	}
    if ( ! fontObjectLabel.CreateFontIndirect(&m_pDoc->m_lfCells) )
	{
		return;
	}
	if ( ! brushGrey.CreateSolidBrush(RGB(192,192,192)) )
	{
		return;
	}
	if ( ! brushBlack.CreateSolidBrush(RGB(0,0,0)) )
	{
		return;
	}

	WORD aPattern[] = {0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF};
	if ( ! bitmap.CreateBitmap(8,8,1,1,aPattern) )
	{
		return;
	}
	if ( ! brushGrid.CreatePatternBrush(&bitmap) )
	{
		return;
	}

	if ( ! penHighlight.CreatePen(PS_SOLID,0,RGB(255,255,255)) )
	{
		return;
	}
	if ( ! penShadow.CreatePen(PS_SOLID,0,GetSysColor(COLOR_BTNSHADOW)) )
	{
		return;
	}
	if ( ! penBlack.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0)) )
	{
		return;
	}
	if ( ! penGroupLine.CreatePen(PS_DOT,nLineWidth,RGB(0,0,255)) )
	{
		return;
	}
	
	// Paint heading ----------------------------------------------------------
 	if ( (pOldBrush = dcMem.SelectObject(&brushGrey)) == NULL )
	{
		return;
	}
	if ( (pOldFont = dcMem.SelectObject(&fontHeading)) == NULL )
	{
		return;
	}

//	dcMem.PatBlt(0,0,rClient.Width(),m_DendMetrics.cyHeadingBar,PATCOPY);
	x  = m_DendMetrics.xObjectLabelsLeft;

	if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
	{
		dcMem.TextOut(x,m_DendMetrics.yHeadingBottom,"Row Fusion Dendrogram",21);
	}else
	{
		dcMem.TextOut(x,m_DendMetrics.yHeadingBottom,"Column Fusion Dendrogram",24);
	}

	// Paint scale bar rectangle ----------------------------------------------
	//dcMem.PatBlt(0,m_DendMetrics.cyHeadingBar,rClient.Width(),m_DendMetrics.cyScaleBar,PATCOPY);
	y = m_DendMetrics.cyScaleBar + m_DendMetrics.cyHeadingBar;
	// Highlight
//	if ( (pOldPen = dcMem.SelectObject(&penHighlight)) == NULL )
//	{
//		return;
//	}
	dcMem.MoveTo(0,y);
	dcMem.LineTo(0, y - m_DendMetrics.cyScaleBar);
	dcMem.LineTo(rClient.Width()-1, y - m_DendMetrics.cyScaleBar);
	// Shadow
//	if ( dcMem.SelectObject(&penShadow) == NULL )
//	{
//		return;
//	}
	dcMem.LineTo(rClient.Width()-1,y);
	dcMem.LineTo(0,y);
	// ========================================================================

	// All values will be range standardised to 0.1 - 1.0 ---------------------
	double oldmin, oldmax;
	double newmin, newmax;
	double oldrange, newrange;
	double interval;
	pFuseLine = m_pFuseTable->GetAt(0);
	if(m_GroupsOnly) oldmin = *m_pCutOff;
	else oldmin = pFuseLine->nLevel;
	pFuseLine = m_pFuseTable->GetAt(m_pFuseTable->GetUpperBound());
	oldmax = pFuseLine->nLevel;
	oldrange = oldmax - oldmin;
	if(m_GroupsOnly) newmin = 0.1;
//	else newmin = *m_pCutOff - 0.1;
	else newmin = 0.1;
	newmax = 1.0;
	newrange = newmax - newmin;
	interval = (h-newmin*h)/4;
	// ========================================================================

	// Draw scale lines & grid ------------------------------------------------
	// Scale line is given 3-D effect by double writing with shadow and highlight pens
//	if ( dcMem.SelectObject(&penShadow) == NULL )
//	{
//		return;
//	}
//	if ( (pOldBrush = dcMem.SelectObject(&brushGrid)) == NULL )
//	{
//		return;
//	}
	// scale line
	y = m_DendMetrics.yScaleLine;
	x =	(long)(x0 + newmin*h);
	dcMem.MoveTo(x, y);
	dcMem.LineTo(x1, y);

	for ( i = 0; i < 5; i++ )
	{
		// scale mark
		x = (long)(x0 + (newmin*h+i*interval));
		dcMem.MoveTo(x, y-2*CYLOGMM);
		//dcMem.LineTo(x, y+2*CYLOGMM);
		dcMem.LineTo(x, rClient.bottom);
		
		// grid line
		oldTextColor = dcMem.SetTextColor(RGB(192,192,192));
		//dcMem.PatBlt(x, YSCROLLTOP,max(nLineWidth,1),HEIGHT,PATCOPY);
		dcMem.PatBlt(x, YSCROLLTOP,max(nLineWidth,1),rClient.bottom,PATCOPY);
		dcMem.SetTextColor(oldTextColor);
	}

	// Draw scale bar labels --------------------------------------------------
	if ( dcMem.SelectObject(&fontScaleLabel) == NULL )
	{
		return;
	}
	y = m_DendMetrics.yScaleLabels;
	x =	(long)(x0 + newmin*h + m_DendMetrics.sizeScaleLabel.cy/2);
	sprintf(buffer,"%0.4f",oldmin);
	
//	// Scale bar labels are given 3-D effect by double writing with shadow and highlight
//	oldTextColor = dcMem.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
//	dcMem.TextOut(x,y,buffer,strlen(buffer));
	dcMem.SetTextColor(RGB(0,0,0));
	dcMem.TextOut(x,y,buffer,strlen(buffer));
	

	dcMem.SetTextColor(oldTextColor);
	for ( i = 1; i < 4; i++ )
	{
		x = (long)(x0 + (newmin*h+i*interval) + m_DendMetrics.sizeScaleLabel.cy/2);
		sprintf(buffer,"%0.4f",oldmin + oldrange/4*i);
		// Scale bar labels are given 3-D effect by double writing with shadow and highlight
		//oldTextColor = dcMem.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		//dcMem.TextOut(x,y,buffer,strlen(buffer));
		dcMem.SetTextColor(RGB(0,0,0));
		dcMem.TextOut(x+1,y+1,buffer,strlen(buffer));

		dcMem.SetTextColor(oldTextColor);
	}
	x =	x1 + m_DendMetrics.sizeScaleLabel.cy/2;
	sprintf(buffer,"%0.4f",oldmax);
	// Scale bar labels are given 3-D effect by double writing with shadow and highlight
	//oldTextColor = dcMem.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
	//dcMem.TextOut(x,y,buffer,strlen(buffer));
	dcMem.SetTextColor(RGB(0,0,0));
	dcMem.TextOut(x+1,y+1,buffer,strlen(buffer));
	
	dcMem.SetTextColor(oldTextColor);
	// ========================================================================

	// Paint objects & labels down left side ----------------------------------
 	if ( dcMem.SelectObject(&brushGrey) == NULL )
	{
		return;
	}
	if ( dcMem.SelectObject(&fontObjectLabel) == NULL )
	{
		return;
	}

	if(m_GroupsOnly && m_ControlIndicators){
		//print control indicators
		x =	CYLINESPACING*2;
		CString str = "a priori Groups";
		//if ( false)//!dcMem.IsPrinting() )
		//{
			// Scale bar labels are given 3-D effect by double writing with shadow and highlight
		//	oldTextColor = dcMem.SetTextColor(GetSysColor(COLOR_BTNSHADOW));
		//	dcMem.TextOut(x,y,str,str.GetLength());
		//	dcMem.SetTextColor(RGB(255,255,255));
		//	dcMem.TextOut(x+1,y+1,str,str.GetLength());
		//}
		//else
		{
			oldTextColor = dcMem.SetTextColor(RGB(0,0,0));
			dcMem.TextOut(x,y,str,str.GetLength());
		}
		dcMem.SetTextColor(oldTextColor);

		//print symbols
		long xoff = 0;
		y += CYLINESPACING;
		long j;
		if(m_row){
			for(j=1;j<m_pDoc->m_RowControlGroups.GetSize() && j<m_pDoc->m_PatternApriori.GetSize();j++){
				m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j-1+1],dcMem.GetSafeHdc(),
					(long)(x+CYLINESPACING*(xoff+0.5)*0.8),
					y-CYLINESPACING*3/4,
					1*CYLINESPACING/14);
				xoff += 2;
			}
		}else{
			for(j=1;j<m_pDoc->m_ColControlGroups.GetSize() && j<=m_pDoc->m_PatternGroups.GetSize();j++){
				m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j-1],dcMem.GetSafeHdc(),
					(long)(x+CYLINESPACING*(xoff+0.5)*0.8),
					y-CYLINESPACING*3/4,
					1*CYLINESPACING/14);
				xoff += 2;
			}
		}		
	}

	oldTextColor = dcMem.SetTextColor(m_pDoc->m_rgbFontCells);
	long idx, next;
	if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
	else nSize = m_pDendOrder->GetSize();
	x = m_DendMetrics.xObjectLabelsLeft;
	y = YSCROLLTOP + CYLINESPACING; // bottom of first label
	rLabel.left  = x;
	rLabel.right = rLabel.left + m_DendMetrics.sizeObjectLabel.cx;
	
		// Draw dendrogram lines --------------------------------------------------
	if ( dcMem.SelectObject(&penBlack) == NULL )
	{
		return;
	}
	// everything above has already been painted, so prevent dend lines from going too far up
	//rClip.top = max(rClip.top,YSCROLLTOP);
	rClip.top = y0 - m_DendMetrics.cyLineSpacing/2;
	dcMem.IntersectClipRect(&rClip);

	if(m_GroupsOnly) nSize = m_pGroupDendTable->GetSize();
	else nSize = m_pDendTable->GetSize();
	// TODO: restrict to lines which are within invalid rect - just remember to include
	// lines just above and just below invalid rect, so that line are drawn to top and 
	// bottom edges of client area.
	long nOrigin;
	nOrigin = 0;
	if(m_GroupsOnly) nOrigin++;
	long Right = -1;
	bool done = false;
	for ( i = 0; i < nSize; i++ )
	{
		if(m_GroupsOnly) pDendLine = m_pGroupDendTable->GetAt(i);
		else pDendLine = m_pDendTable->GetAt(i);

		if ( pDendLine->xStart > 0 )
		{
			xStart = (long)(((pDendLine->xStart - oldmin)*newrange/oldrange + newmin)*h + x0);
		}
		else if( pDendLine->xStart == 0 && pDendLine->yStart > (long) pDendLine->yStart)
		{
			xStart = (long)(x0 + newmin*h);
		}
		else
		{
			xStart = x0;
			if(m_GroupsSymbols == 1)xStart += m_DendMetrics.xObjectLabelsLeft;
			
		}
		xRight = (long)(((pDendLine->xRight - oldmin)*newrange/oldrange + newmin)*h + x0);
		if ( pDendLine->xEnd > 0 )
		{
			xEnd = (long)(((pDendLine->xEnd - oldmin)*newrange/oldrange + newmin)*h + x0);
		}
		else if(pDendLine->xEnd == 0 && pDendLine->yEnd > (long) pDendLine->yEnd)
		{
			xEnd = (long)(x0 + newmin*h);
		}
		else
		{
			xEnd = x0;
			if(m_GroupsSymbols == 1) xEnd += m_DendMetrics.xObjectLabelsLeft;
		}

		yStart = (long)((pDendLine->yStart-nOrigin)*m_DendMetrics.cyLineSpacing + y0);
		yEnd   = (long)((pDendLine->yEnd-nOrigin)*m_DendMetrics.cyLineSpacing + y0);

		dcMem.MoveTo(xStart,yStart);
		dcMem.LineTo(xRight,yStart);
		dcMem.LineTo(xRight,yEnd);
		dcMem.LineTo(xEnd-1,yEnd);

		//to fix up m_pCutOff !!!
		if(!done && xRight == (long)(((*m_pCutOff - oldmin)*newrange/oldrange + newmin)*h + x0)){
			Right = 0;
			Right -= xRight;
		}else if(!done && Right > -1){
			Right += xRight;
			done = true;
		}
		
	}
	// ========================================================================

	if(m_GroupsOnly) nSize = m_pGroupDendOrder->GetSize();
	else nSize = m_pDendOrder->GetSize();

	i = 0;
	for ( ; i < nSize; i++ )
	{
		if ( y < rClip.top )
		{
			y += m_DendMetrics.cyLineSpacing;
			continue;
		}


		if(m_GroupsOnly){
			next = m_pGroupDendOrder->GetAt(i);
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
				m_pDoc->GetRowLabel(m_pDoc->m_Rows.GetAt(next)->GetAt(0),strLabel);
			}else{
				m_pDoc->GetColumnLabel(m_pDoc->m_Cols.GetAt(next)->GetAt(0),strLabel);
			}

			//if(m_GroupsSymbols && m_pDoc->m_Rows.GetSize() > next &&
			//	m_pDoc->m_Rows[next]){
			//	strLabel.Format("%d",m_pDoc->m_Rows[next]->GetSize());
			//}
		}else{
			next = m_pDendOrder->GetAt(i);
			if(m_pFuseTable == &m_pDoc->m_FuseTableRows)
				m_pDoc->GetRowLabel(next,strLabel);
			else
				m_pDoc->GetColumnLabel(next,strLabel);
		}
		
		if(m_GroupsOnly && m_ControlIndicators && 
			next < m_controlindicator.GetSize() &&
			next < m_pDoc->m_PatternGroups.GetSize()){
			//draw control label/evaluated group counts

			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			//dcMem.ExtTextOut((long)(x+CYLINESPACING*1.5),y,ETO_CLIPPED,&rLabel,strLabel,strLabel.GetLength(),NULL);
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[next],dcMem.GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);

		//	dcMem.PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
		//			(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
		//			m_DendMetrics.sizeObjectLabel.cy,PATCOPY);

			long xoff = 0;
			for(long j=1;j<m_controlindicator[next]->GetSize();j++){
				if(m_controlindicator[next]->GetAt(j) == 0){
					xoff+=2;
					continue;
				}

				strLabel.Format("%d",m_controlindicator[next]->GetAt(j));
				dcMem.ExtTextOut((long)(x+CYLINESPACING*(2 + xoff)*0.8),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
				xoff += 2;
			}

		}else if(m_GroupsOnly && m_GroupsSymbols){
			//draw symbol
			//dcMem.PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
			//		(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
			//		m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
			
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			if(m_GroupsSymbols == 2){
				dcMem.ExtTextOut((long)(x+CYLINESPACING*1.5),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
			}
			m_tex.GDIPatterns(m_pDoc->m_PatternGroups[next],dcMem.GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
		}else if((m_GroupsSymbols || m_ControlIndicators) && !m_GroupsOnly){
		//	dcMem.PatBlt((long)(x-CXLOGMM+CYLINESPACING*1.5),y-m_DendMetrics.sizeObjectLabel.cy,
		//			(long)(x0-(x-CXLOGMM+CYLINESPACING*1.5)),
		//			m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
			
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;

			if(m_GroupsSymbols == 2 || m_ControlIndicators){
				dcMem.ExtTextOut((long)(x+CYLINESPACING*1.5),y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
			}
			if(m_GroupsSymbols && next < m_pDoc->m_PatternObjects.GetSize())
			{
				if(m_pDoc->m_PatternObjects[next] == -1)
					m_tex.GDIPatterns(m_pDoc->m_PatternObjects[next],dcMem.GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
				else
					m_tex.GDIPatterns(next,dcMem.GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
			}
			else
			{
				//what apriori group does it belong to?
				for(long k=0;k<m_pDoc->m_RowControlGroups.GetSize();k++){
					for(long m=0;m<m_pDoc->m_RowControlGroups[k]->GetSize();m++){
						if(m_pDoc->m_RowControlGroups[k]->GetAt(m) == next)
							break;
					}
					if(m != m_pDoc->m_RowControlGroups[k]->GetSize())
						break;
				}
				if(k != m_pDoc->m_RowControlGroups.GetSize())
					m_tex.GDIPatterns(m_pDoc->m_PatternApriori[k],dcMem.GetSafeHdc(),x,y-CYLINESPACING*3/4,1*CYLINESPACING/14);
			}
		}else{
			dcMem.PatBlt(x-CXLOGMM,y-m_DendMetrics.sizeObjectLabel.cy,
					x0-(x-CXLOGMM),
					m_DendMetrics.sizeObjectLabel.cy,PATCOPY);
			
			rLabel.bottom = y;
			rLabel.top = rLabel.bottom - m_DendMetrics.sizeObjectLabel.cy;
			
			dcMem.ExtTextOut(x,y,NULL,&rLabel,strLabel,strLabel.GetLength(),NULL);
		}

		if ( y > rClip.bottom )
			break;

		y += CYLINESPACING;
		idx = next;
	}
	dcMem.SetTextColor(oldTextColor);
	// ========================================================================

 


	// Draw terminating marker on last fusion ---------------------------------
	if ( dcMem.SelectObject(&penBlack) == NULL )
	{
		return;
	}
	if ( dcMem.SelectObject(&brushBlack) == NULL )
	{
		return;
	}
	x =	xRight;
	y = yStart + (yEnd-yStart)/2;
	dcMem.MoveTo(x,y);
	x += 2*CXLOGMM;
	dcMem.LineTo(x,y);
	POINT aPoints[] = { x,y, x+2*CXLOGMM,y-2*CYLOGMM, x+2*CXLOGMM,y+2*CYLOGMM };
	long oldPolyFillMode = dcMem.SetPolyFillMode(WINDING);
	dcMem.MoveTo(x,y);
	dcMem.Polygon(aPoints,3);
	dcMem.SetPolyFillMode(oldPolyFillMode);
	// ========================================================================

	// Draw border top and bottom ---------------------------------------------
	
	SaveBitmapFile(dcMem.GetSafeHdc(), (HBITMAP) bmp, strPathName);

	DeleteDC(dcMem);
	bmp.DeleteObject();

}

void CDendView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());
}

void CDendView::OnGroupsonly() 
{
	if(m_GroupsOnly){
		m_GroupsOnly = false;
		m_menu.CheckMenuItem(ID__GROUPSONLY,MF_UNCHECKED);
		m_idxTopRow = 0;
		SetScrollRange(SB_VERT,0,m_pDendOrder->GetSize()-1,TRUE);
	}else{
		m_GroupsOnly = true;
		m_menu.CheckMenuItem(ID__GROUPSONLY,MF_CHECKED);
		m_idxTopRow = 0;
		SetScrollRange(SB_VERT,0,m_pGroupDendOrder->GetSize()-1,TRUE);
	}
	RedrawWindow();

}

void CDendView::OnOptionsFont() 
{
	LOGFONT lf;
	memcpy(&lf,&(m_pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	lf.lfHeight /= ((long)(FONTSCALING));
	
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	dlg.m_cf.Flags |= CF_LIMITSIZE;
	dlg.m_cf.nSizeMin = 3;
	dlg.m_cf.nSizeMax = 48;
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

BOOL CDendView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog * d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	pInfo->m_pPD->m_pd.Flags |= PD_ALLPAGES;
	pInfo->m_pPD->m_pd.Flags |= PD_HIDEPRINTTOFILE;
	pInfo->m_pPD->m_pd.Flags |= PD_NOSELECTION;
	
	return DoPreparePrinting(pInfo);
}

void CDendView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
//	CRect rect,temp;
//	CPoint point;

//	m_psd->GetMargins(&rect,&temp);
//	point.x = m_psd->GetPaperSize().cx;
//	point.y = m_psd->GetPaperSize().cy;
	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();
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

	long store = m_pDoc->m_lfCells.lfHeight;
	m_pDoc->m_lfCells.lfHeight = (long)(-m_pDoc->m_nPointSizeCells*VPixelsPerInch/72/10);


	SetMapping(pDC);	
	CalcMetrics(pDC);

	CString str;
	CSize size;
	str = m_pDoc->GetTitle();
	str += "     ";
	str += m_pDoc->m_TableName;
	size = pDC->GetTextExtent(str, str.GetLength());	
	long y;

	y = m_rectDraw.top + size.cy*4;	

	y += m_DendMetrics.cyLineSpacing;//YSCROLLTOP + CYLINESPACING

	long cyDend;	// Height of area left on page for drawing actual dendrogram
	long nNumLines;
	long nNumPages;

	// Set up drawing rect to entire page (in logical coordinates)
	// (this is just the same as what the framework does after OnPrepareDC())
	pDC->DPtoLP(&m_rectDraw);

	// Calculate how much of the rect is available for drawing actual dendrogram
	//cyDend = m_rectDraw.Height() -
	//			m_DendMetrics.cyHeader -
	//			m_DendMetrics.cyFooter -
	//			m_DendMetrics.cyHeadingBar -
	//			m_DendMetrics.cyScaleBar - 1 /* YScrollTop */;
	//cyDend = m_rectDraw.Height() -
	//	m_DendMetrics.cyHeader - 
	//	m_DendMetrics.cyScaleBar -1 ;
	cyDend = m_rectDraw.Height() - YSCROLLTOP - CYLINESPACING;

	// Calculate how many pages will be needed to print full dendrogram
	m_DendMetrics.nNumLinesPerPage = (long)(cyDend/(double)(CYLINESPACING*1.1));
	if(m_GroupsOnly)
		nNumLines = m_pGroupDendOrder->GetSize();
	else
		nNumLines = m_pDendOrder->GetSize();
	nNumPages = nNumLines/m_DendMetrics.nNumLinesPerPage;
	if ( nNumLines%m_DendMetrics.nNumLinesPerPage)
		nNumPages++;

	// Set pages info
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(nNumPages);

	m_pDoc->m_lfCells.lfHeight = store;
//	CView::OnBeginPrinting(pDC, pInfo);
}

void CDendView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	//static bool firsttime = true;

	long store = m_pDoc->m_lfCells.lfHeight;
	m_pDoc->m_lfCells.lfHeight = (long)((-m_pDoc->m_nPointSizeCells * FONTSCALING)/10);

	CFont font, *pOldFont;
	CSize size;
	CString str;
	long x,y;
	CRect holdrect;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	holdrect.top = m_rectDraw.top;
	holdrect.bottom = m_rectDraw.bottom;
	holdrect.left = m_rectDraw.left;
	holdrect.right = m_rectDraw.right;

	m_idxTopPrintRow = (pInfo->m_nCurPage - 1) * LINESPERPAGE;
	
	m_lfHeaderFooter.lfHeight = (long)(-m_nHeaderFooterPointSize * FONTSCALING );
	font.CreateFontIndirect(&m_lfHeaderFooter);
	pOldFont = pDC->SelectObject(&font);

	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);

	// Print header -----------------------------------------------------------
	str = pDoc->GetTitle();
	str += "     ";
	str += pDoc->m_TableName;
	size = pDC->GetTextExtent(str, str.GetLength());
	x = m_rectDraw.left + (m_rectDraw.Width() - size.cx)/2;
	y = m_rectDraw.top + size.cy;
	pDC->TextOut(x,y,str);
	x = m_rectDraw.left;
	if(m_pFuseTable == &m_pDoc->m_FuseTableRows){
		str.Format("Fusion Type: %s      Beta = %0.2f",pDoc->m_FuseTypeRows,
			pDoc->m_FuseBetaRows);
		size = pDC->GetTextExtent(str, str.GetLength());
		y += size.cy;
		pDC->TextOut(x,y,str);
		str = "On Association: ";
		str += pDoc->m_FuseAssoRows;
		str += "     Created on: ";
		str += pDoc->m_FuseTimeRows;
		size = pDC->GetTextExtent(str, str.GetLength());
		y += size.cy;
		pDC->TextOut(x,y,str);
	}else{
		str.Format("Fusion Type: %s      Beta = %0.2f",pDoc->m_FuseTypeCols,
			pDoc->m_FuseBetaCols);
		size = pDC->GetTextExtent(str, str.GetLength());
		y += size.cy;
		pDC->TextOut(x,y,str);
		str = "On Association: ";
		str += pDoc->m_FuseAssoCols;
		str += "     Created on: ";
		str += pDoc->m_FuseTimeCols;
		size = pDC->GetTextExtent(str, str.GetLength());
		y += size.cy;
		pDC->TextOut(x,y,str);
	}


	// set m_rectDraw.top to below the header
	m_rectDraw.top += (size.cy + HEADERFOOTERGAP);
	// ========================================================================

	// Print footer -----------------------------------------------------------
	char buffer[10];
	str = "Page ";
	sprintf(buffer,"%d",pInfo->m_nCurPage);
	str += buffer;
	str += " of ";
	sprintf(buffer,"%d",pInfo->GetMaxPage());
	str += buffer;
	size = pDC->GetTextExtent(str, str.GetLength());
	x = m_rectDraw.left + (m_rectDraw.Width() - size.cx)/2;
	y = m_rectDraw.bottom;
	pDC->TextOut(x,y,str);

	//if(firsttime){
		CalcMetrics(pDC);
	//	firsttime = false;
	//}

	m_rectDraw.bottom = m_rectDraw.top + 
		CYLINESPACING * m_DendMetrics.nNumLinesPerPage + 
		 m_DendMetrics.cyHeadingBar +
		m_DendMetrics.cyScaleBar +1;

	// ========================================================================

	pDC->SetWindowOrg(-m_rectDraw.left,-m_rectDraw.top);
	pDC->DPtoLP(&m_rectDraw);
	pDC->IntersectClipRect(m_rectDraw);

	if ( pOldFont != NULL )
		pDC->SelectObject(pOldFont);
	font.DeleteObject();

	m_DendMetrics.width = m_rectDraw.Width();
	m_DendMetrics.height = m_rectDraw.Height();

	OnDraw(pDC);

	m_rectDraw.top = holdrect.top;
	m_rectDraw.bottom = holdrect.bottom;
	m_rectDraw.left = holdrect.left;
	m_rectDraw.right = holdrect.right;

	m_pDoc->m_lfCells.lfHeight = store;
}

BOOL CDendView::SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) 
{     
	long         hFile;
	OFSTRUCT    ofReOpenBuff;     
	HBITMAP     hTmpBmp;//, hBmpOld;     
	BOOL        bSuccess;     
	BITMAPFILEHEADER    bfh;     
	PBITMAPINFO pbmi;     
	PBYTE       pBits;     
	BITMAPINFO  bmi;     
	PBYTE pjTmp, pjTmpBmi;     
	ULONG sizBMI;       

	bSuccess = TRUE; 

	if (!hBmp) 
	{         
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("no bitmap to save",NULL, MB_OK);         
		return FALSE;     
	}      

	//     
	// Let the graphics engine to retrieve the dimension of the bitmap for us     
	// GetDIBits uses the size to determine if it's BITMAPCOREINFO or BITMAPINFO     
	// if BitCount != 0, color table will be retrieved     
	//     
	bmi.bmiHeader.biSize = 0x28;       // GDI need this to work     
	bmi.bmiHeader.biBitCount = 0;      // don't get the color table     
	
	if ((GetDIBits(hDC, hBmp, 0, 0, (LPSTR)NULL, &bmi, 
		DIB_RGB_COLORS)) == 0) 
	{        
		return FALSE;     
	}      

	//     
	// Now that we know the size of the image, alloc enough memory to retrieve     
	// the actual bits     
	//     
	if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,                 
		bmi.bmiHeader.biSizeImage)) == NULL) 
	{         
		return FALSE;     
	}      

	//     
	// Note: 24 bits per pixel has no color table.  So, we don't have to     
	// allocate memory for retrieving that.  Otherwise, we do.     
	//     
	pbmi = &bmi; 
                                     
	// assume no color table      
	switch (bmi.bmiHeader.biBitCount) 
	{         
		case 24:  // has color table             
			sizBMI = sizeof(BITMAPINFOHEADER);             
			break;         
		case 16:         
		case 32:             
			sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;             
			break;         
		default:             
			sizBMI = sizeof(BITMAPINFOHEADER)+
				sizeof(RGBQUAD)*(1<<bmi.bmiHeader.biBitCount);             
		break;      
	}      

	//     
	// Allocate memory for color table if it is not 24bpp...     
	//     
	if (sizBMI != sizeof(BITMAPINFOHEADER)) 
	{         
		ULONG       sizTmp;         
		//         
		// I need more memory for the color table         
		//         
		if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | 
			GMEM_ZEROINIT, sizBMI )) == NULL) 
		{             
			bSuccess = FALSE;             
			goto ErrExit1;         
		}         
		
		//        
		// Now that we've a bigger chunk of memory, let's copy the Bitmap         
		// info header data over         
		//         
		pjTmp = (PBYTE)pbmi;         
		pjTmpBmi = (PBYTE)&bmi;         
		sizTmp = sizeof(BITMAPINFOHEADER);          
		while(sizTmp--) 
		{             
			*(((PBYTE)pjTmp)++) = *((pjTmpBmi)++);         
		}     
	}  
    
	//     
	// Let's open the file and get ready for writing     
	//     
	if ((hFile = OpenFile(pszFileName, (LPOFSTRUCT)&ofReOpenBuff,                  
		OF_CREATE | OF_WRITE)) == -1) 
	{         
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("file open failed",NULL, MB_OK);         
		goto ErrExit2;     
	}      

	//     
	// But first, fill in the info for the BitmapFileHeader     
	//     
	bfh.bfType = 0x4D42;                            

	// 'BM'     
	bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizBMI+
	pbmi->bmiHeader.biSizeImage;     
	bfh.bfReserved1 = bfh.bfReserved2 = 0;     
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizBMI;      
	
	//     
	// Write out the file header now     
	//     
	if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1) 
	{         
		bSuccess = FALSE;         
		goto ErrExit3;     
	}      
	
	//     
	// Bitmap can't be selected into a DC when calling GetDIBits     
	// Assume that the hDC is the DC where the bitmap would have been selected     
	// if indeed it has been selected     
	//     
	if (hTmpBmp = CreateCompatibleBitmap(hDC, pbmi->bmiHeader.biWidth, 
		pbmi->bmiHeader.biHeight)) 
	{         
 		SelectObject(hDC, hTmpBmp);         
		if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, 
			(LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0)
		{            
			bSuccess = FALSE;             
			goto ErrExit4;         
		}     
	} else {         
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("bitmap not created.\r\n\r\nImage may be too large.  Try saving with a reduced image width.",NULL, MB_OK);         
		bSuccess = FALSE;         
		goto ErrExit3;     
	}      

	//     
	// Now write out the BitmapInfoHeader and color table, if any     
	//     
	if (_lwrite(hFile, (LPSTR)pbmi, sizBMI) == -1) 
	{         
		bSuccess = FALSE;         
		goto ErrExit4;     
	}      

	//     
	// write the bits also     
	//     
	if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1) 
	{         
		bSuccess = FALSE;         
		goto ErrExit4;     
	}   
	
ErrExit4:     

ErrExit3:
	_lclose(hFile); 

ErrExit2:     
	GlobalFree(pbmi); 

ErrExit1:     
	GlobalFree(pBits);     

	return bSuccess; 
}  

void CDendView::OnSaveimageas() 
{
	m_SaveImage = true;
	RedrawWindow();
	m_SaveImage = false;
	RedrawWindow();
}

void CDendView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
}

BOOL CDendView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) return true;
	inMouseWheel = true;

	long x,y,diff;
	
	if(m_mousewheelsensitivity == 0)
		m_mousewheelsensitivity = 40;
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y + x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	long max = m_pDendOrder->GetSize()-1;

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



void CDendView::OnMouseMove(UINT nFlags, CPoint point) 
{
/*	if(m_movingcutoff && !m_GroupsOnly){
		CRect rect;
		GetClientRect(rect);
		
		//calculate new cutoff
		CFuseSplitterWnd* pSplitterWnd = (CFuseSplitterWnd*)GetParentFrame();
		if(point.x <= m_cutoffmin){
			*pSplitterWnd->m_pCutOff = m_cutoffoldmin;
			rect.left = m_cutoffmin;
			rect.right = m_cutoffX;
		}else if(point.x >= m_cutoffmax){
			*pSplitterWnd->m_pCutOff = m_cutoffoldmin+m_cutoffoldrange;
			rect.right = m_cutoffmax;
			rect.left = m_cutoffX;
		}else{
			*pSplitterWnd->m_pCutOff = ((point.x-m_cutoffx0)/m_cutoffh - 
				m_cutoffnewmin) * m_cutoffoldrange / m_cutoffnewrange + 
				m_cutoffoldmin;
			rect.left = min(m_cutoffX,point.x);
			rect.right = max(m_cutoffX,point.x);
		}
		//reassign objects to groups based on the new cutoff
		//TODO

		//redrawwindow		
		rect.left--;
		rect.right++;
		RedrawWindow(rect);


	}
	
	if(point.x-4 < m_cutoffX && point.x+4 > m_cutoffX){
	//change icon to resize column icon
		SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_EW));
	}else{
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}*/
	
	CView::OnMouseMove(nFlags, point);
}

void CDendView::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	if(point.x-4 < m_cutoffX && point.x+4 > m_cutoffX){
//		SetCapture();
//
//		m_movingcutoff = true;
//	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CDendView::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	ReleaseCapture();
//
//	m_movingcutoff = false;
//	
	CView::OnLButtonUp(nFlags, point);
}

void CDendView::OnBwsymbols() 
{
	//if(m_GroupsSymbols){
	//	m_GroupsSymbols = false;
	//	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_UNCHECKED);
	//}else{
	//	m_GroupsSymbols = true;
	//	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_CHECKED);
	//}
	m_GroupsSymbols = 1;
	m_ControlIndicators = false;
	
	m_menu.CheckMenuItem(ID__LABELS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_CHECKED);
	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__LABELSSYMBOLS,MF_UNCHECKED);

	RedrawWindow();	
}

void CDendView::OnUpdateBwsymbols(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pFuseTable == &m_pDoc->m_FuseTableRows);
	
}

void CDendView::OnControlindicator() 
{
	//if(m_ControlIndicators){
	//	m_ControlIndicators = false;
	//	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_UNCHECKED);
	//}else{
	//	m_ControlIndicators = true;
	//	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_CHECKED);
	//}
	m_GroupsSymbols = 0;	
	m_ControlIndicators = true;


	m_menu.CheckMenuItem(ID__LABELS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_CHECKED);
	m_menu.CheckMenuItem(ID__LABELSSYMBOLS,MF_UNCHECKED);

	RedrawWindow();	
}

void CDendView::OnUpdateControlindicator(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pDoc->m_PatternApriori.GetSize() && m_row);
}

void CDendView::OnLabels() 
{
	//uncheck ID__BWSYMBOLS && ID__CONTROLINDICATOR

	m_GroupsSymbols = 0;
	m_ControlIndicators = false;

	m_menu.CheckMenuItem(ID__LABELS,MF_CHECKED);
	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__LABELSSYMBOLS,MF_UNCHECKED);
	
	RedrawWindow();
}

void CDendView::OnLabelsSymbols()
{
	m_GroupsSymbols = 2;
	m_ControlIndicators = false;

	m_menu.CheckMenuItem(ID__LABELSSYMBOLS,MF_CHECKED);
	m_menu.CheckMenuItem(ID__LABELS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__BWSYMBOLS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__CONTROLINDICATOR,MF_UNCHECKED);
	
	RedrawWindow();
}

void CDendView::DrawToMetaFile(CString filename) //if false, must deletemetafile g_emf after use
{
	
	CDC * pDC = GetDC();
	CRect rr;
	GetClientRect(rr);
	pDC->FillSolidRect(rr,0x00ffffff);
	long iWidthMM = GetDeviceCaps(pDC->GetSafeHdc(), HORZSIZE);
	long iHeightMM = GetDeviceCaps(pDC->GetSafeHdc(), VERTSIZE);
	long iWidthPels = GetDeviceCaps(pDC->GetSafeHdc(), HORZRES);
	long iHeightPels = GetDeviceCaps(pDC->GetSafeHdc(), VERTRES);

	// Use iWidthMM, iWidthPels, iHeightMM, and iHeightPels to determine the
	// number of .01-millimeter units per pixel in the x and y directions.

	long iMMPerPelX = (iWidthMM * 100)/iWidthPels;
	long iMMPerPelY = (iHeightMM * 100)/iHeightPels;

	// Retrieve the coordinates of the client rectangle in pixels.
	// Convert client coordinates to .01-mm units. 
	long width = rr.Width();
	long height = rr.Height();

	rr.left = rr.left * iMMPerPelX;
	rr.top = rr.top * iMMPerPelY;
	rr.right = rr.right * iMMPerPelX;
	rr.bottom = rr.bottom * iMMPerPelY;

	HDC met;
			
	met = CreateEnhMetaFile(pDC->GetSafeHdc(),filename,NULL,NULL);	

	pDC = CDC::FromHandle(met);
	
	/*{
		//draw a bounding rect for metafile to recognise as the boundary
		if(width-120 < height){
			height = width-120;
		}else{
			width = height+120;
		}
		m_drawres.x = width;
		m_drawres.y = height;
		pDC->SelectStockObject(WHITE_PEN);
		pDC->MoveTo(0,0);
		pDC->LineTo(width,0);
		pDC->LineTo(width,height);
		pDC->SelectStockObject(BLACK_PEN);
	}*/

	//draw using met or pDC?
	OnDraw(pDC);


	m_SaveImage = false;

	//g_emf = CloseEnhMetaFile(met);
	DeleteEnhMetaFile(CloseEnhMetaFile(met));

}