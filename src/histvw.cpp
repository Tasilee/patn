// histvw.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "histvw.h"
#include "TableView.h"
#include <stdlib.h>
#include <stdio.h>
#include ".\histvw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CXLOGMM pDC->GetDeviceCaps(LOGPIXELSX)*10/254
#define CYLOGMM pDC->GetDeviceCaps(LOGPIXELSY)*10/254
#define FONTSCALING pDC->GetDeviceCaps(LOGPIXELSY)/72
#define WIDTH m_cxWidth
#define HEIGHT m_cyHeight
#define TOPMARGIN 15*CYLOGMM
#define BOTTOMMARGIN 10*CYLOGMM
#define LEFTMARGIN 25*CXLOGMM
#define RIGHTMARGIN 25*CXLOGMM
#define HEADERFOOTERGAP 10*CYLOGMM
/////////////////////////////////////////////////////////////////////////////
// CHistogram

IMPLEMENT_DYNCREATE(CHistogram, CViewExtension)

CHistogram::CHistogram()
{
}

CHistogram::~CHistogram()
{
	delete m_aArray;
	
	if(m_psd != NULL) delete m_psd;


}

#ifdef _DEBUG
CPatnDoc* CHistogram::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CHistogram, CViewExtension)
	//{{AFX_MSG_MAP(CHistogram)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__SAVEIMAGE, OnSaveimage)
	ON_COMMAND(ID__CLOSE, OnClose)
	ON_WM_CAPTURECHANGED()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID__FONT, OnFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHistogram drawing

void CHistogram::OnDraw(CDC* pDC)
{

	CPatnDoc* pDoc = GetDocument();

	pDC->SetTextColor(pDoc->m_rgbFontCells);

	long mmHorz = pDC->GetDeviceCaps(HORZSIZE);
	long mmVert = pDC->GetDeviceCaps(VERTSIZE);
	float nScreenRatio = ((float)mmHorz/(float)mmVert);
	long nMaxBarHeight;
	long nHistLeft;
	long nHistTop;
	long nHistBottom;
	long nBarWidth;
	long nMaxScale;
	float nScale;
 	long nSegments;
	float nSegmentSize;
	long x;
	long y;
	CRect rectBar;
	CBrush brushBar;
	CPen pen;
	long nLineWidth;
	CFont fontText, fontHeading;
	TEXTMETRIC tm;
 	char buffer[100];
	CRect rText;
	CRect rStats;
	long nCount;
	long i;
	CSize size;
	long cxLabel;
	long cxGap;
	static IN_OnDraw = FALSE;
	
	CFont * oldFont;
	CPen * oldPen;
	CBrush * oldBrush;

	if(pDC->IsPrinting()){
		//drawPrint(pDC);
		//return;
	}	

	if(m_SaveImage){
		SaveIt(pDC);	
		return;
	}

	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;
	//change dimensions of pDC

	// Set up arrays for stats ------------------------------------------------
	CArray<double> aNums;
	aNums.Add(m_nMin);
	aNums.Add(m_nMax);
	aNums.Add(m_nMax-m_nMin);
	aNums.Add(m_nSum);
	aNums.Add(m_nMean);
	CArray<CString,const char*> aLabels;
	aLabels.Add("Number");
	aLabels.Add("Num > 0");
	aLabels.Add("Minimum");
	aLabels.Add("Maximum");
	aLabels.Add("Range");
	aLabels.Add("Sum");
	aLabels.Add("Mean");
	// ========================================================================

	// Get dimensions of client area-------------------------------------------
	if ( !pDC->IsPrinting() )
	{
		// if printing, then the width & height are set in OnPrint() based on m_rectDraw
		CRect rClient;
		GetClientRect(&rClient);
		m_cxWidth  = rClient.Width();
		m_cyHeight = rClient.Height();
		if(m_cyHeight > m_cxWidth / 1.5)
			m_cyHeight = (long)(m_cxWidth / 1.5);
	}
	// ========================================================================

	// Calculate histogram height & bar width ---------------------------------
	// If printing, create a square client area so that the bars appear in same
	// proportions to rest of histogram, regardless of orientation of paper
	nHistTop = HEIGHT / 10;
	nHistBottom = nHistTop + 8 * HEIGHT / 10;
	nBarWidth = (95*(nHistBottom - nHistTop)/100)/m_nNoOfBars;
	if ( pDC->IsPrinting() )
	{
		if ( (HEIGHT/CYLOGMM) > (WIDTH/CXLOGMM) )
		{
			HEIGHT = WIDTH/CXLOGMM*CYLOGMM;
		}
		else if ( (WIDTH/CXLOGMM) > (HEIGHT/CYLOGMM) )
		{
			WIDTH = HEIGHT/CYLOGMM*CXLOGMM;
		}
		HEIGHT = m_rectDraw.Height();
		WIDTH = m_rectDraw.Width();
		long h = HEIGHT;
		long w = WIDTH;
		if(HEIGHT > WIDTH/2)
			HEIGHT = (long)(WIDTH/2);
		else
			WIDTH = (long)(HEIGHT*2);
		nScreenRatio = 1.0;
		nHistTop = HEIGHT / 10;
		nHistBottom = nHistTop + 8 * HEIGHT / 10;
		nBarWidth = (95*(nHistBottom - nHistTop)/100)/m_nNoOfBars;
	}
	
	// ========================================================================

	// Create GDI objects -----------------------------------------------------
	// (fonts are scaled to size of histogram)
    //if ( ! fontText.CreateFont(-(nBarWidth*7/10),0,0,0,FW_LIGHT,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
	//	CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS, "Papyrus") ){
	//	IN_OnDraw = FALSE;
	//	return;
	//}
    //if ( ! fontHeading.CreateFont(-nBarWidth,0,0,0,FW_BOLD,TRUE,TRUE,0,0,OUT_DEFAULT_PRECIS,
	//	CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS, "Papyrus") ){
	//	fontText.DeleteObject();
	//	IN_OnDraw = FALSE;
	//	return;
	//}
	LOGFONT lf;
	memcpy(&lf,&(pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,pDoc->m_lfCells.lfFaceName);

	if(pDC->IsPrinting() && m_rectDraw.Width()/1.3 < m_rectDraw.Height())
		lf.lfHeight = -nBarWidth*7/15;
	else
		lf.lfHeight = -nBarWidth*7/10;
	fontText.CreateFontIndirect(&lf);
	lf.lfHeight = -nBarWidth;
	lf.lfWeight = FW_BOLD; 
    fontHeading.CreateFontIndirect(&lf);

	if ( !brushBar.CreateSolidBrush(RGB(0,255,255)) ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	if ( pDC->IsPrinting() )
		nLineWidth = 13 * CYLOGMM / 100; // (0.13mm)
	else
		nLineWidth = 0; // (1 pixel)

	if ( !pen.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0)) ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}
	// ========================================================================

	// Calculate width of text strings ----------------------------------------
	/*
	Labels on left of histogram are:
					 --
					|	gap
					|	stats_label
					|	gap
		stats block-|	:
					|	gap
					|	stats_num
					|	2 * gap
					 --

					 --
					|	count
					|	gap
		hist labels-|	%
					|	gap
					|	range
					 --
	*/

	if ( (oldFont = pDC->SelectObject(&fontText)) == NULL ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	pDC->GetTextMetrics(&tm);
	cxGap = 2*tm.tmAveCharWidth;

	cxLabel = 0;
	// ....stats labels
	cxLabel += cxGap;
	cxLabel += (pDC->GetTextExtent("Maximum",7)).cx;
	cxLabel += cxGap;
	cxLabel += (pDC->GetTextExtent(":",1)).cy;
	cxLabel += cxGap;

	// ....stats numbers
	sprintf(buffer,"%0.4f",m_nSum);
	cxLabel += (pDC->GetTextExtent(buffer,strlen(buffer))).cx;
	cxLabel += 2*cxGap;

	// ....count
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	cxLabel += max((pDC->GetTextExtent(buffer,strlen(buffer))).cx,(pDC->GetTextExtent("Count",5)).cx);
	// ....gap
	cxLabel += cxGap;
	// ....%
	cxLabel += (pDC->GetTextExtent("00.00)",6)).cx;
	cxLabel += (pDC->GetTextExtent("(",1)).cx;
	// ....gap
	cxLabel += cxGap;
	// ....range
	sprintf(buffer,"%*.4f  -  %*.4f",m_nDecMax+5,m_nMax,m_nDecMax+5,m_nMax);
	cxLabel += (pDC->GetTextExtent(buffer,strlen(buffer))).cx;
	// ========================================================================

	// Calculate left side of histogram	---------------------------------------
	nMaxBarHeight = (long) ((nHistBottom-nHistTop)*nScreenRatio);
	if ( (cxLabel + nMaxBarHeight) <= (9*WIDTH/10) )  // No adjustment needed
	{
		// Center (labels + histogram) horizontally
		nHistLeft = ( (WIDTH - cxLabel - nMaxBarHeight)/2 + cxLabel );
	}
	else
	{
		nMaxBarHeight = 9*WIDTH/10 - cxLabel;
		nMaxBarHeight = max(nMaxBarHeight,96);
		nHistLeft = WIDTH - nMaxBarHeight - 5*WIDTH/100;
	}
	// ========================================================================

	if ( (oldBrush = pDC->SelectObject(&brushBar)) == NULL ){
		pDC->SelectObject(oldFont);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	if ( (oldPen = pDC->SelectObject(&pen)) == NULL ){
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

	// Work out the scale for the X-axis --------------------------------------
	// Initally, try for 4 segments, then adjust if necessary
	nSegments = 4;
	i = 0;
	if ( m_nHighCount > (nSegments - 1)*2 )
	{
		nMaxScale = m_nHighCount/nSegments;
		while ( nMaxScale > 10 )
		{
			nMaxScale /= 10;
			i++;
		}
		nMaxScale++;
		while ( i > 0 )
		{
			nMaxScale *= 10;
			i--;
		}
		nMaxScale *= nSegments;
	}
	else
	{
		nSegments = m_nHighCount;
		nMaxScale = nSegments;
	}
	nScale = (float)m_nHighCount/(float)nMaxScale;
	nSegmentSize = (float)nMaxBarHeight/(float)nSegments;
	// ========================================================================

	// Now that dimensions have been worked out, start painting!

	// Heading ----------------------------------------------------------------
	if ( pDC->SelectObject(&fontHeading) == NULL ){
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
		return;
	}

 	sprintf(buffer,"%s",m_Hist.strHeading);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	if ( size.cx > WIDTH )
	{
 		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
		x =	CXLOGMM;
		rText.left = x;
	}
	else
	{
  		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
		x = WIDTH/2;
		rText.left = x - size.cx/2;
	}
	y = 9*nHistTop/10;
	rText.bottom = y;
	rText.right = rText.left + size.cx;
	rText.top = rText.bottom - size.cy;
	
	pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);
	//=========================================================================

	//PRINTING FUDGING
	if(pDC->IsPrinting() && false){
		//nMaxBarHeight = 700;
		// Draw the Y and X axes --------------------------------------------------
		pDC->MoveTo(nHistLeft,nHistTop);
		pDC->LineTo(nHistLeft,nHistBottom);
		pDC->LineTo(nHistLeft+nMaxBarHeight,nHistBottom);

		// Draw the scale marks on the X-axis
		for ( i = 0; i < nSegments; i++ )
		{
			x = (long) ((((float)i/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);//(long) (nHistLeft + nSegmentSize*(i+1));
			pDC->MoveTo(x,nHistBottom);
			pDC->LineTo(x,nHistBottom+30);
		}
		if ( pDC->SelectObject(&fontText) == NULL ){
			pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
			return;
		}

		// Draw the labels on the X-axis ------------------------------------------
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);

		rText.top = nHistBottom + 4 + tm.tmHeight/2;
		rText.bottom = rText.top + tm.tmHeight;

		for ( i = 0; i <= nSegments; i++ )
		{
			sprintf(buffer,"%d",i*nMaxScale/nSegments);
			x = (long) ((((float)i/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);//(long) (nHistLeft + nSegmentSize*(i+1));
			rText.left = (long) (x - nSegmentSize/2);
			rText.right = (long) (rText.left + nSegmentSize);
			pDC->ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		}
		// ========================================================================
	}else{
		// Draw the Y and X axes --------------------------------------------------
		pDC->MoveTo(nHistLeft,nHistTop);
		pDC->LineTo(nHistLeft,nHistBottom);
		pDC->LineTo(nHistLeft+nMaxBarHeight,nHistBottom);

		// Draw the scale marks on the X-axis
		for ( i = 0; i < nSegments; i++ )
		{
			x = (long) (nHistLeft + nSegmentSize*(i+1));
			pDC->MoveTo(x,nHistBottom);
			pDC->LineTo(x,nHistBottom+4);
		}
		
		if ( pDC->SelectObject(&fontText) == NULL ){
			pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		IN_OnDraw = FALSE;
			return;
		}

		// Draw the labels on the X-axis ------------------------------------------
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);

		rText.top = nHistBottom + 4 + tm.tmHeight/2;
		rText.bottom = rText.top + tm.tmHeight;

		for ( i = 0; i <= nSegments; i++ )
		{
			sprintf(buffer,"%d",i*nMaxScale/nSegments);
			x = (long) (nHistLeft + i*nSegmentSize);
			rText.left = (long) (x - nSegmentSize/2);
			rText.right = (long) (rText.left + nSegmentSize);
			pDC->ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		}
		// ========================================================================

	}
	// ========================================================================

	// Draw the histogram bars ------------------------------------------------
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rectBar.top = y;
	rectBar.left = nHistLeft;

	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rectBar.bottom = rectBar.top + nBarWidth;
		nCount = m_aCount.GetAt(i);
		rectBar.right = (long) ((((float)nCount/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);
		pDC->Rectangle(&rectBar);
		rectBar.top = rectBar.bottom - 1;
	}
	// ========================================================================

	// Paint labels on left of Y-axis -----------------------------------------
	// ....right side of range
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"  %*.4f",m_nDecMax+5,m_nMax);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = nHistLeft - 6;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"  %*.4f",m_nDecMax+5,m_aSegments[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left side of range
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMax);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	rText.top = rText.bottom - size.cy;
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMin);
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	for ( i = 0; i < m_nNoOfBars-1; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_aSegments[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....percentage
	size = pDC->GetTextExtent("00.00)",6);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx - (pDC->GetTextExtent("(",1)).cx;
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
	pDC->ExtTextOut(rText.left+rText.Width()/2,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"%",1,NULL);	
	rText.bottom += nBarWidth;
	rText.left += (pDC->GetTextExtent("(",1)).cx;
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%5.2f)",(float)nCount*100/(float)m_nCount);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left bracket around percentage
	size = pDC->GetTextExtent("(",1);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"(");
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....count
	sprintf(buffer,"Count",m_nDecCount,m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"Count",5,NULL);	
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y += nBarWidth;
	rText.bottom = y;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%*d",m_nDecCount,nCount);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	pDC->MoveTo(rText.left,rText.top);
	pDC->LineTo(rText.right,rText.top);
	rText.left = min(rText.left,rText.right - (pDC->GetTextExtent("Count",5)).cx);
	// ========================================================================

	// Paint stats block ------------------------------------------------------
	// ....numbers
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"%0.4f",m_nSum);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += 3*nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - 2*cxGap;
	rStats.right = rText.right + cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%0.4f",aNums[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%d",m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left + size.cx;
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	sprintf(buffer,"%d",m_nCountPos);
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	

	// ....':'
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,":");
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize()+2; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,":");
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....stats labels
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"Maximum");
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rStats.top = nHistTop;
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	rStats.left = rText.left - cxGap;
	for ( i = 0; i < aLabels.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%s",aLabels[i]);
		pDC->ExtTextOut(rText.left,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// .... draw rect around stats block
	rStats.bottom = rText.bottom;
	pDC->MoveTo(rStats.left,rStats.top);
	pDC->LineTo(rStats.right,rStats.top);
	pDC->LineTo(rStats.right,rStats.bottom);
	pDC->LineTo(rStats.left,rStats.bottom);
	pDC->LineTo(rStats.left,rStats.top);
	// ========================================================================
	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
	fontText.DeleteObject();
	fontHeading.DeleteObject();
	brushBar.DeleteObject();
	pen.DeleteObject();
	IN_OnDraw = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CHistogram diagnostics

#ifdef _DEBUG
void CHistogram::AssertValid() const
{
	CView::AssertValid();
}

void CHistogram::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHistogram message handlers

int CHistogram::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_menu.LoadMenu(IDR_IMAGE);
	
	m_psd = NULL;
	m_SaveImage = false;

	CPatnDoc* pDoc = GetDocument();
	CArray<CArray<double>*> aMatrix;
	CArray<double>* pArray;
	long		nSelCount;
	long		nCount;
	double	nValue;
	double	nMissingValue = pDoc->GetMissingValue();
	long		i,j,k;					   
	long		idx, idxMin = 0;
	CString strLabel;

	m_Hist.strHeading.Empty();

	if ( !pDoc->m_ColSelection->IsEmpty() )
	{
		nCount = pDoc->GetNoOfRows();
		nSelCount = pDoc->m_ColSelection->GetCount();
		aMatrix.SetSize(nSelCount);
		idxMin = idx = pDoc->m_ColSelection->GetFirst();
		for ( i = 0; i < nSelCount && idx != -1; i++ )
		{
			idxMin = min(idxMin,idx);
			pArray = new CArray<double>;
			m_aArray = pArray;
			k = 0;
			pArray->SetSize(nCount);
			for ( j = 0; j < nCount; j++ )
			{
				nValue = pDoc->GetValueAt(idx,j);
				if ( nValue != nMissingValue )
				{
					pArray->SetAt(k,nValue);
					k++;
				}
			}
			pArray->SetSize(k);
			aMatrix.SetAt(i,pArray);

			idx = pDoc->m_ColSelection->GetNext();
		}
		m_Hist.pMatrix = &aMatrix;
		m_Hist.strHeading = "Column: ";
		pDoc->GetColumnLabel(idxMin,strLabel);
		m_Hist.strHeading += strLabel;
		if ( nSelCount > 1 )
			m_Hist.strHeading += "...";
	}
	else if ( !pDoc->m_RowSelection->IsEmpty() )
	{
		nCount = pDoc->GetNoOfColumns();
		nSelCount = pDoc->m_RowSelection->GetCount();
		aMatrix.SetSize(nSelCount);
		idxMin = idx = pDoc->m_RowSelection->GetFirst();
		for ( i = 0; i < nSelCount && idx != -1; i++ )
		{
			idxMin = min(idxMin,idx);
			pArray = new CArray<double>;
			m_aArray = pArray;
			k = 0;
			pArray->SetSize(nCount);
			for ( j = 0; j < nCount; j++ )
			{
				nValue = pDoc->GetValueAt(j,idx);
				if ( nValue != nMissingValue )
				{
					pArray->SetAt(k,nValue);
					k++;
				}
			}
			pArray->SetSize(k);
			aMatrix.SetAt(i,pArray);

			idx = pDoc->m_RowSelection->GetNext();
		}
		m_Hist.pMatrix = &aMatrix;
		m_Hist.strHeading = "Row: ";
		pDoc->GetRowLabel(idxMin,strLabel);
		m_Hist.strHeading += strLabel;
		if ( nSelCount > 1 )
			m_Hist.strHeading += "...";
	}

	if(m_Hist.pMatrix == NULL || 
		m_Hist.pMatrix->GetAt(0)->GetSize() == 0)
		return -1;

	CalcHistogram();

	return 0;
}

void CHistogram::CalcHistogram()
{
	CArray<double>* pA;
	double nValue;
	long i,j;
	long nColCount, nRowCount;
	
	// Get initial min & max values
	nRowCount =	m_Hist.pMatrix->GetSize();
	if ( nRowCount > 0 )
	{
		pA = m_Hist.pMatrix->GetAt(0);
		if(pA->GetSize() > 0)
			nValue = pA->GetAt(0);
		else
			nValue = 0;
		
		m_nMin = nValue;
		m_nMax = nValue;
	}

	m_nSum = 0;
	m_nCount = 0;
	m_nCountPos = 0;

	// Get range, stats
	for ( i = 0; i < nRowCount; i++ )
	{
		pA = m_Hist.pMatrix->GetAt(i);
		nColCount = pA->GetSize();
		for ( j = 0; j < nColCount; j++ )
		{
			nValue = pA->GetAt(j);
			m_nMin = min(m_nMin,nValue);
			m_nMax = max(m_nMax,nValue);
			m_nSum += nValue;
			m_nCount++;
			if ( nValue > 0 )
				m_nCountPos++;
		}
	}
	m_nMean = m_nSum/m_nCount;

	m_nNoOfBars = 18;

	// Divide range into segments (18 segments is as per DOS patn)
	m_aSegments.SetSize(m_nNoOfBars);
	m_aCount.SetSize(m_nNoOfBars);

	double nSegmentSize = (float)(m_nMax - m_nMin)/(float)m_nNoOfBars;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		m_aSegments.SetAt(i, (i+1)*nSegmentSize + m_nMin);
	}

	// Get count of values within each segment range
	long nCount;
	long k;
	for ( i = 0; i < m_nNoOfBars; i++)
	{
		m_aCount.SetAt(i,0);
	}
	for ( i = 0; i < nRowCount; i++ )
	{
		pA = m_Hist.pMatrix->GetAt(i);
		nColCount = pA->GetSize();
		for ( j = 0; j < nColCount; j++ )
		{
			nValue = pA->GetAt(j);
			for ( k = 0; k < m_nNoOfBars; k++ )
			{
				if ( nValue <= m_aSegments.GetAt(k) )
				{
					nCount = m_aCount.GetAt(k);
					nCount++;
					m_aCount.SetAt(k,nCount);
					break;
				}
			}
		}
	}

	// Get highest count
	m_nHighCount = 0;
	m_nLowCount = 0;

	if ( m_aCount.GetSize() > 0 )
	{
		m_nHighCount = m_aCount.GetAt(0);
		m_nLowCount = m_aCount.GetAt(0);
		for ( i = 1; i < m_nNoOfBars; i++ )
		{
			nCount = m_aCount.GetAt(i);
			m_nHighCount = max(m_nHighCount,nCount);
			m_nLowCount  = min(m_nLowCount,nCount);
		}
	}

	// Get largest number of digits before decimal point (used to determine label width)
	int nSign,decmax,deccount;
	decmax = m_nDecMax;
	deccount = m_nDecCount;
	_ecvt(m_nMax,8,&decmax,&nSign);
	decmax = max(0,decmax);
	_ecvt(m_nCount,8,&deccount,&nSign);
	m_nDecCount = max(0,deccount);
	m_nDecMax = decmax;
	m_nDecCount = deccount;
}

BOOL CHistogram::SaveBitmapFile(HDC hDC, HBITMAP hBmp,CString pszFileName) 
{     
	long         hFile;
	OFSTRUCT    ofReOpenBuff;     
	HBITMAP     hTmpBmp, hBmpOld;     
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
 		hBmpOld = (HBITMAP) SelectObject(hDC, hTmpBmp);         
		if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, 
			(LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0)
		{            
			bSuccess = FALSE;             
			goto ErrExit4;         
		}     
	} else {         
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("bitmap not created",NULL, MB_OK);         
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
	SelectObject(hDC, hBmpOld);     
	DeleteObject(hTmpBmp); 

ErrExit3:
	_lclose(hFile); 

ErrExit2:     
	GlobalFree(pbmi); 

ErrExit1:     
	GlobalFree(pBits);     

	return bSuccess; 
}  

HPALETTE CopyPalette (HPALETTE hpal) {     
	PLOGPALETTE ppal;     
	WORD        nNumEntries;    
	
	if (!hpal)      
		return NULL;   
	
	GetObject(hpal,sizeof(INT),(LPSTR)&nNumEntries);    
	
	if (nNumEntries == 0) 
		return NULL;   

	ppal = (PLOGPALETTE)LocalAlloc(LPTR,sizeof(LOGPALETTE) +  
		nNumEntries * sizeof(PALETTEENTRY));   
	
	if (!ppal)     
		return NULL;   
	
	ppal->palVersion    = 0x300; 
    ppal->palNumEntries = nNumEntries;    
	
	GetPaletteEntries(hpal,0,nNumEntries,
		(LPPALETTEENTRY)ppal->palPalEntry);     
	
	hpal = CreatePalette(ppal);      
	
	LocalFree((HANDLE)ppal);   
	
	return hpal;
 } 


void CHistogram::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());

}

BOOL CHistogram::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog *d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
	//if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	pInfo->SetMaxPage(1);	

	return DoPreparePrinting(pInfo);
}

void CHistogram::drawPrint(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();

	long cxScreen = pDC->GetDeviceCaps(HORZRES);
	int cyScreen = pDC->GetDeviceCaps(VERTRES);
	int mmHorz = pDC->GetDeviceCaps(HORZSIZE);
	int mmVert = pDC->GetDeviceCaps(VERTSIZE);
	float nScreenRatio = ((float)mmHorz/(float)mmVert);
	long nMaxBarHeight;
	long nHistLeft;
	long nHistTop;
	long nHistBottom;
	long nBarWidth;
	long nMaxScale;
	float nScale;
 	long nSegments;
	float nSegmentSize;
	long x;
	long y;
	CRect rectBar;
	CBrush brushBar;
	CPen pen;
	long nLineWidth;
	CFont fontText, fontHeading;
	TEXTMETRIC tm;
 	char buffer[100];
	CRect rText;
	CRect rStats;
	long nCount;
	long i;
	CSize size;
	long cxLabel;
	long cxGap;

	CFont * oldFont;
	CPen * oldPen;
	CBrush * oldBrush;

	// Set up arrays for stats ------------------------------------------------
	CArray<double> aNums;
	aNums.Add(m_nMin);
	aNums.Add(m_nMax);
	aNums.Add(m_nMax-m_nMin);
	aNums.Add(m_nSum);
	aNums.Add(m_nMean);
	CArray<CString,const char*> aLabels;
	aLabels.Add("Number");
	aLabels.Add("Num > 0");
	aLabels.Add("Minimum");
	aLabels.Add("Maximum");
	aLabels.Add("Range");
	aLabels.Add("Sum");
	aLabels.Add("Mean");
	// ========================================================================
	
	// Calculate histogram height & bar width ---------------------------------
	// If printing, create a square client area so that the bars appear in same
	// proportions to rest of histogram, regardless of orientation of paper
	if ( (HEIGHT/CYLOGMM) > (WIDTH/CXLOGMM) )
	{
		HEIGHT = WIDTH/CXLOGMM*CYLOGMM;
	}
	else if ( (WIDTH/CXLOGMM) > (HEIGHT/CYLOGMM) )
	{
		WIDTH = HEIGHT/CYLOGMM*CXLOGMM;
	}
	HEIGHT = m_rectDraw.Height();
	WIDTH = m_rectDraw.Width();
	nScreenRatio = 1.0;
	cxScreen = m_rectDraw.Width();
	cyScreen = m_rectDraw.Height();
	m_cxWidth  = m_rectDraw.Width();//rClient.Width();
	m_cyHeight = m_rectDraw.Height();//rClient.Height();
	HEIGHT = (HEIGHT>WIDTH)?WIDTH:HEIGHT;
	m_cyHeight = (m_cyHeight > m_cxWidth)?m_cxWidth:m_cyHeight;
	nHistTop = HEIGHT / 10;
	nHistBottom = nHistTop + 8 * HEIGHT / 10;
	nBarWidth = (95*(nHistBottom - nHistTop)/100)/m_nNoOfBars;

	
	// ========================================================================

	// Create GDI objects -----------------------------------------------------
	// (fonts are scaled to size of histogram)
    //if ( ! fontText.CreateFont((long)(m_rectDraw.Width()/42.0),0,0,0,FW_LIGHT,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
      //                   CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS, "Papyrus") )
	//{
		//return;
	//}
    //if ( ! fontHeading.CreateFont((long)(m_rectDraw.Width()/15.0),0,0,0,FW_BOLD,TRUE,TRUE,0,0,OUT_DEFAULT_PRECIS,
      //               CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS, "Papyrus") ){
//		fontText.DeleteObject();
//		return;
//	}	
	LOGFONT lf;
	memcpy(&lf,&(pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,pDoc->m_lfCells.lfFaceName);

	lf.lfHeight = -nBarWidth*7/10;
	fontText.CreateFontIndirect(&lf);
	lf.lfHeight = -nBarWidth;
	lf.lfWeight = FW_BOLD; 
    fontHeading.CreateFontIndirect(&lf);

	if ( !brushBar.CreateSolidBrush(RGB(0,255,255)) ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		return;
	}

	nLineWidth = (long)(13 * m_HScale * CYLOGMM / 100); // (0.13mm)


	if ( !pen.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0)) ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		return;
	}	// ========================================================================

	// Calculate width of text strings ----------------------------------------
	/*
	Labels on left of histogram are:
					 --
					|	gap
					|	stats_label
					|	gap
		stats block-|	:
					|	gap
					|	stats_num
					|	2 * gap
					 --

					 --
					|	count
					|	gap
		hist labels-|	%
					|	gap
					|	range
					 --
	*/

	if ( (oldFont = pDC->SelectObject(&fontText)) == NULL ){
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	pDC->GetTextMetrics(&tm);
	cxGap = 2*tm.tmAveCharWidth;

	cxLabel = 0;
	// ....stats labels
	cxLabel += cxGap;
	cxLabel += (pDC->GetTextExtent("Maximum",7)).cx;
	cxLabel += cxGap;
	cxLabel += (pDC->GetTextExtent(":",1)).cy;
	cxLabel += cxGap;

	// ....stats numbers
	sprintf(buffer,"%0.4f",m_nSum);
	cxLabel += (pDC->GetTextExtent(buffer,strlen(buffer))).cx;
	cxLabel += 2*cxGap;

	// ....count
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	cxLabel += max((pDC->GetTextExtent(buffer,strlen(buffer))).cx,(pDC->GetTextExtent("Count",5)).cx);
	// ....gap
	cxLabel += cxGap;
	// ....%
	cxLabel += (pDC->GetTextExtent("00.00)",6)).cx;
	cxLabel += (pDC->GetTextExtent("(",1)).cx;
	// ....gap
	cxLabel += cxGap;
	// ....range
	sprintf(buffer,"%*.4f  -  %*.4f",m_nDecMax+5,m_nMax,m_nDecMax+5,m_nMax);
	cxLabel += (pDC->GetTextExtent(buffer,strlen(buffer))).cx;
	// ========================================================================

	// Calculate left side of histogram	---------------------------------------
	nMaxBarHeight = (long) ((nHistBottom-nHistTop)*nScreenRatio);
	if ( (cxLabel + nMaxBarHeight) <= (9*WIDTH/10) )  // No adjustment needed
	{
		// Center (labels + histogram) horizontally
		nHistLeft = ( (WIDTH - cxLabel - nMaxBarHeight)/2 + cxLabel );
	}
	else
	{
		nMaxBarHeight = 9*WIDTH/10 - cxLabel;
		nMaxBarHeight = max(nMaxBarHeight,96);
		nHistLeft = WIDTH - nMaxBarHeight - 5*WIDTH/100;
	}
	// ========================================================================

	if ( (oldBrush = pDC->SelectObject(&brushBar)) == NULL ){
		pDC->SelectObject(oldFont);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	if ( (oldPen = pDC->SelectObject(&pen)) == NULL ){
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	// Work out the scale for the X-axis --------------------------------------
	// Initally, try for 4 segments, then adjust if necessary
	nSegments = 4;
	i = 0;
	if ( m_nHighCount > (nSegments - 1)*2 )
	{
		nMaxScale = m_nHighCount/nSegments;
		while ( nMaxScale > 10 )
		{
			nMaxScale /= 10;
			i++;
		}
		nMaxScale++;
		while ( i > 0 )
		{
			nMaxScale *= 10;
			i--;
		}
		nMaxScale *= nSegments;
	}
	else
	{
		nSegments = m_nHighCount;
		nMaxScale = nSegments;
	}
	nScale = (float)m_nHighCount/(float)nMaxScale;
	nSegmentSize = (float)nMaxBarHeight/(float)nSegments;
	// ========================================================================

	// Now that dimensions have been worked out, start painting!

	// Heading ----------------------------------------------------------------
	if ( pDC->SelectObject(&fontHeading) == NULL ){
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
 	sprintf(buffer,"%s",m_Hist.strHeading);

	size = pDC->GetTextExtent(buffer,strlen(buffer));

	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
	x = WIDTH/2+m_rectDraw.left;
	rText.left = x - size.cx/2;

	rText.top = m_rectDraw.top;
	y = rText.bottom = rText.top + size.cy;
	rText.right = rText.left + size.cx;
	
	pDC->ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);
	//=========================================================================

	nHistLeft = m_rectDraw.left + m_rectDraw.Width()/3*2;
	nHistBottom = m_rectDraw.bottom*2/3;//m_cxWidth*4/3 + m_rectDraw.top + 100;
	nHistTop = m_rectDraw.top + m_cxWidth*4/3/nSegments-50;
	nMaxBarHeight = m_rectDraw.right - nHistLeft;
	nSegmentSize = (float) (nMaxBarHeight/nSegments);

	// Draw the Y and X axes --------------------------------------------------
	pDC->MoveTo(nHistLeft,nHistTop);
	pDC->LineTo(nHistLeft,nHistBottom);
	pDC->LineTo(nHistLeft+nMaxBarHeight,nHistBottom);
	// Draw the scale marks on the X-axis
	for ( i = 0; i < nSegments; i++ )
	{
		x = (long) ((((float)i/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);//(long) (nHistLeft + nSegmentSize*(i+1));
		pDC->MoveTo(x,nHistBottom);
		pDC->LineTo(x,nHistBottom+30);
	}
	if ( pDC->SelectObject(&fontText) == NULL ){
			pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
			return;
		}
	// Draw the labels on the X-axis ------------------------------------------
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);

	rText.top = nHistBottom + 4 + tm.tmHeight/2;
	rText.bottom = rText.top + tm.tmHeight;
	for ( i = 0; i <= nSegments; i++ )
	{
		sprintf(buffer,"%d",i*nMaxScale/nSegments);
		x = (long) ((((float)i/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);//(long) (nHistLeft + nSegmentSize*(i+1));
		rText.left = (long) (x - nSegmentSize/2);
		rText.right = (long) (rText.left + nSegmentSize);
		pDC->ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	}
	// ========================================================================
	
	// ========================================================================

		// Draw the histogram bars ------------------------------------------------
	nBarWidth = (95*(nHistBottom - nHistTop)/100)/m_nNoOfBars;
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rectBar.top = y;
	rectBar.left = nHistLeft;

	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rectBar.bottom = rectBar.top + nBarWidth;
		nCount = m_aCount.GetAt(i);
		rectBar.right = (long) ((((float)nCount/(float)m_nHighCount)*nMaxBarHeight) + nHistLeft);
		pDC->Rectangle(&rectBar);
		rectBar.top = rectBar.bottom - 1;
	}

	// Paint labels on left of Y-axis -----------------------------------------
	// ....right side of range
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"  %*.4f",m_nDecMax+5,m_nMax);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = nHistLeft - 6;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"  %*.4f",m_nDecMax+5,m_aSegments[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left side of range
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMax);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	rText.top = rText.bottom - size.cy;
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMin);
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	for ( i = 0; i < m_nNoOfBars-1; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_aSegments[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....percentage
	size = pDC->GetTextExtent("00.00)",6);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx - (pDC->GetTextExtent("(",1)).cx;
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
	pDC->ExtTextOut(rText.left+rText.Width()/2,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"%",1,NULL);	
	rText.bottom += nBarWidth;
	rText.left += (pDC->GetTextExtent("(",1)).cx;
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%5.2f)",(float)nCount*100/(float)m_nCount);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left bracket around percentage
	size = pDC->GetTextExtent("(",1);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"(");
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....count
	sprintf(buffer,"Count",m_nDecCount,m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"Count",5,NULL);	
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y += nBarWidth;
	rText.bottom = y;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%*d",m_nDecCount,nCount);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	pDC->MoveTo(rText.left,rText.top);
	pDC->LineTo(rText.right,rText.top);
	rText.left = min(rText.left,rText.right - (pDC->GetTextExtent("Count",5)).cx);
	// ========================================================================

	// Paint stats block ------------------------------------------------------
	// ....numbers
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"%0.4f",m_nSum);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += 3*nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - 2*cxGap;
	rStats.right = rText.right + cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%0.4f",aNums[i]);
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%d",m_nCount);
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left + size.cx;
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	sprintf(buffer,"%d",m_nCountPos);
	rText.top = rText.bottom - size.cy;
	pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	

	// ....':'
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,":");
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize()+2; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,":");
		pDC->ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....stats labels
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"Maximum");
	size = pDC->GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rStats.top = nHistTop;
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	rStats.left = rText.left - cxGap;
	for ( i = 0; i < aLabels.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%s",aLabels[i]);
		pDC->ExtTextOut(rText.left,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// .... draw rect around stats block
	rStats.bottom = rText.bottom;
	pDC->MoveTo(rStats.left,rStats.top);
	pDC->LineTo(rStats.right,rStats.top);
	pDC->LineTo(rStats.right,rStats.bottom);
	pDC->LineTo(rStats.left,rStats.bottom);
	pDC->LineTo(rStats.left,rStats.top);
	// ========================================================================

	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
	fontText.DeleteObject();
	fontHeading.DeleteObject();
	brushBar.DeleteObject();
	pen.DeleteObject();
}

void CHistogram::SaveIt(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	// TODO: add draw code here
	long cxScreen = pDC->GetDeviceCaps(HORZRES);
	long cyScreen = pDC->GetDeviceCaps(VERTRES);
	long mmHorz = pDC->GetDeviceCaps(HORZSIZE);
	long mmVert = pDC->GetDeviceCaps(VERTSIZE);
	float nScreenRatio = ((float)mmHorz/(float)mmVert);
	long nMaxBarHeight;
	long nHistLeft;
	long nHistTop;
	long nHistBottom;
	long nBarWidth;
	long nMaxScale;
	float nScale;
 	long nSegments;
	float nSegmentSize;
	long x;
	long y;
	CRect rectBar;
	CBrush brushBar;
	CPen pen;
	long nLineWidth;
	CFont fontText, fontHeading;
	TEXTMETRIC tm;
 	char buffer[100];
	CRect rText;
	CRect rStats;
	long nCount;
	long i;
	CSize size;
	long cxLabel;
	long cxGap;

	// if printing, then the width & height are set in OnPrint() based on m_rectDraw
	CRect rClient;
	GetClientRect(&rClient);

	rClient.top = 0;
	rClient.bottom = 480;
	rClient.left = 0;
	rClient.right = 640;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	m_SaveImage = false;
	strTitle = "Save Histogram:";
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
	delete pFileDlg;

	m_cxWidth  = rClient.Width();
	m_cyHeight = rClient.Height();	
	long nWidth = m_cxWidth;	
	long nHeight = m_cyHeight;

		if(nWidth % 2 == 1) nWidth = nWidth + 1;
	if(nHeight %2 == 1) nHeight = nHeight + 1;

	
	// ========================================================================

	CDC dcMem;
	CBitmap bmp;

	CFont * oldFont;
	CPen * oldPen;
	CBrush * oldBrush;

	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

	dcMem.BitBlt(0,0,nWidth,nHeight,pDC,0,0,WHITENESS);
	
	// Set up arrays for stats ------------------------------------------------
	CArray<double> aNums;
	aNums.Add(m_nMin);
	aNums.Add(m_nMax);
	aNums.Add(m_nMax-m_nMin);
	aNums.Add(m_nSum);
	aNums.Add(m_nMean);
	CArray<CString,const char*> aLabels;
	aLabels.Add("Number");
	aLabels.Add("Num > 0");
	aLabels.Add("Minimum");
	aLabels.Add("Maximum");
	aLabels.Add("Range");
	aLabels.Add("Sum");
	aLabels.Add("Mean");
	// ========================================================================

	// Calculate histogram height & bar width ---------------------------------
	// If printing, create a square client area so that the bars appear in same
	// proportions to rest of histogram, regardless of orientation of paper
	nHistTop = HEIGHT / 10;
	nHistBottom = nHistTop + 8 * HEIGHT / 10;
	nBarWidth = (95*(nHistBottom - nHistTop)/100)/m_nNoOfBars;

	// ========================================================================

	// Create GDI objects -----------------------------------------------------
	// (fonts are scaled to size of histogram)
    if ( ! fontText.CreateFont(-(nBarWidth*7/10),0,0,0,FW_LIGHT,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS,  ((CPatnApp*)AfxGetApp())->m_globalFontName) )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		return;
	}    if ( ! fontHeading.CreateFont(-nBarWidth,0,0,0,FW_BOLD,TRUE,TRUE,0,0,OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH | FF_SWISS,  ((CPatnApp*)AfxGetApp())->m_globalFontName) )
{
		fontText.DeleteObject();
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		return;
	}	if ( !brushBar.CreateSolidBrush(RGB(0,255,255)) )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		return;
	}
	nLineWidth = 0; // (1 pixel)

	if ( !pen.CreatePen(PS_SOLID,nLineWidth,RGB(0,0,0)) )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		return;
	}	// ========================================================================

	// Calculate width of text strings ----------------------------------------
	/*
	Labels on left of histogram are:
					 --
					|	gap
					|	stats_label
					|	gap
		stats block-|	:
					|	gap
					|	stats_num
					|	2 * gap
					 --

					 --
					|	count
					|	gap
		hist labels-|	%
					|	gap
					|	range
					 --
	*/

	if ( (oldFont = dcMem.SelectObject(&fontText)) == NULL )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	dcMem.GetTextMetrics(&tm);
	cxGap = 2*tm.tmAveCharWidth;

	cxLabel = 0;
	// ....stats labels
	cxLabel += cxGap;
	cxLabel += (dcMem.GetTextExtent("Maximum",7)).cx;
	cxLabel += cxGap;
	cxLabel += (dcMem.GetTextExtent(":",1)).cy;
	cxLabel += cxGap;

	// ....stats numbers
	sprintf(buffer,"%0.4f",m_nSum);
	cxLabel += (dcMem.GetTextExtent(buffer,strlen(buffer))).cx;
	cxLabel += 2*cxGap;

	// ....count
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	cxLabel += max((dcMem.GetTextExtent(buffer,strlen(buffer))).cx,(dcMem.GetTextExtent("Count",5)).cx);
	// ....gap
	cxLabel += cxGap;
	// ....%
	cxLabel += (dcMem.GetTextExtent("00.00)",6)).cx;
	cxLabel += (dcMem.GetTextExtent("(",1)).cx;
	// ....gap
	cxLabel += cxGap;
	// ....range
	sprintf(buffer,"%*.4f  -  %*.4f",m_nDecMax+5,m_nMax,m_nDecMax+5,m_nMax);
	cxLabel += (dcMem.GetTextExtent(buffer,strlen(buffer))).cx;
	// ========================================================================

	// Calculate left side of histogram	---------------------------------------
	nMaxBarHeight = (long) ((nHistBottom-nHistTop)*nScreenRatio);
	if ( (cxLabel + nMaxBarHeight) <= (9*WIDTH/10) )  // No adjustment needed
	{
		// Center (labels + histogram) horizontally
		nHistLeft = ( (WIDTH - cxLabel - nMaxBarHeight)/2 + cxLabel );
	}
	else
	{
		nMaxBarHeight = 9*WIDTH/10 - cxLabel;
		nMaxBarHeight = max(nMaxBarHeight,96);
		nHistLeft = WIDTH - nMaxBarHeight - 5*WIDTH/100;
	}
	// ========================================================================

	if ( (oldBrush = dcMem.SelectObject(&brushBar)) == NULL )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		pDC->SelectObject(oldFont);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	if ( (oldPen = dcMem.SelectObject(&pen)) == NULL )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}
	// Work out the scale for the X-axis --------------------------------------
	// Initally, try for 4 segments, then adjust if necessary
	nSegments = 4;
	i = 0;
	if ( m_nHighCount > (nSegments - 1)*2 )
	{
		nMaxScale = m_nHighCount/nSegments;
		while ( nMaxScale > 10 )
		{
			nMaxScale /= 10;
			i++;
		}
		nMaxScale++;
		while ( i > 0 )
		{
			nMaxScale *= 10;
			i--;
		}
		nMaxScale *= nSegments;
	}
	else
	{
		nSegments = m_nHighCount;
		nMaxScale = nSegments;
	}
	nScale = (float)m_nHighCount/(float)nMaxScale;
	nSegmentSize = (float)nMaxBarHeight/(float)nSegments;
	// ========================================================================

	// Now that dimensions have been worked out, start painting!

	// Heading ----------------------------------------------------------------
	if ( dcMem.SelectObject(&fontHeading) == NULL )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
		pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
		return;
	}

 	sprintf(buffer,"%s",m_Hist.strHeading);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	if ( size.cx > WIDTH )
	{
 		dcMem.SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
		x =	CXLOGMM;
		rText.left = x;
	}
	else
	{
  		dcMem.SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
		x = WIDTH/2;
		rText.left = x - size.cx/2;
	}
	y = 9*nHistTop/10;
	rText.bottom = y;
	rText.right = rText.left + size.cx;
	rText.top = rText.bottom - size.cy;
	
	dcMem.ExtTextOut(x,y,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);
	//=========================================================================

	//PRINTING FUDGING
	// Draw the Y and X axes --------------------------------------------------
	dcMem.MoveTo(nHistLeft,nHistTop);
	dcMem.LineTo(nHistLeft,nHistBottom);
	dcMem.LineTo(nHistLeft+nMaxBarHeight,nHistBottom);

	// Draw the scale marks on the X-axis
	for ( i = 0; i < nSegments; i++ )
	{
	x = (long) (nHistLeft + nSegmentSize*(i+1));
		dcMem.MoveTo(x,nHistBottom);
		dcMem.LineTo(x,nHistBottom+4);
	}
		
	if ( dcMem.SelectObject(&fontText) == NULL )
{
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
			pDC->SelectObject(oldFont);
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
		fontText.DeleteObject();
		fontHeading.DeleteObject();
		brushBar.DeleteObject();
		pen.DeleteObject();
			return;
		}
	// Draw the labels on the X-axis ------------------------------------------
	dcMem.SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);

	rText.top = nHistBottom + 4 + tm.tmHeight/2;
	rText.bottom = rText.top + tm.tmHeight;

	for ( i = 0; i <= nSegments; i++ )
	{
		sprintf(buffer,"%d",i*nMaxScale/nSegments);
		x = (long) (nHistLeft + i*nSegmentSize);
		rText.left = (long) (x - nSegmentSize/2);
		rText.right = (long) (rText.left + nSegmentSize);
		dcMem.ExtTextOut(x,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	}
	// ========================================================================

	
	// ========================================================================

	// Draw the histogram bars ------------------------------------------------
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rectBar.top = y;
	rectBar.left = nHistLeft;

	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rectBar.bottom = rectBar.top + nBarWidth;
		nCount = m_aCount.GetAt(i);
		rectBar.right = (long) ((((float)nCount/(float)m_nHighCount)*nMaxBarHeight)*nScale + nHistLeft);
		dcMem.Rectangle(&rectBar);
		rectBar.top = rectBar.bottom - 1;
	}
	// ========================================================================

	// Paint labels on left of Y-axis -----------------------------------------
	// ....right side of range
	dcMem.SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"  %*.4f",m_nDecMax+5,m_nMax);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = nHistLeft - 6;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"  %*.4f",m_nDecMax+5,m_aSegments[i]);
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left side of range
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMax);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	rText.top = rText.bottom - size.cy;
	sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_nMin);
	dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	for ( i = 0; i < m_nNoOfBars-1; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%*.4f  -",m_nDecMax+5,m_aSegments[i]);
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....percentage
	size = dcMem.GetTextExtent("00.00)",6);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx - (dcMem.GetTextExtent("(",1)).cx;
	dcMem.SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
	dcMem.ExtTextOut(rText.left+rText.Width()/2,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"%",1,NULL);	
	rText.bottom += nBarWidth;
	rText.left += (dcMem.GetTextExtent("(",1)).cx;
	dcMem.SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%5.2f)",(float)nCount*100/(float)m_nCount);
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....left bracket around percentage
	size = dcMem.GetTextExtent("(",1);
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"(");
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....count
	sprintf(buffer,"Count",m_nDecCount,m_nCount);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rText.bottom = y;
	rText.top = rText.bottom - size.cy;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,"Count",5,NULL);	
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y += nBarWidth;
	rText.bottom = y;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < m_nNoOfBars; i++ )
	{
		rText.top = rText.bottom - size.cy;
		nCount = m_aCount.GetAt(i);
		sprintf(buffer,"%*d",m_nDecCount,nCount);
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%*d",m_nDecCount,m_nCount);
	rText.top = rText.bottom - size.cy;
	dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	dcMem.MoveTo(rText.left,rText.top);
	dcMem.LineTo(rText.right,rText.top);
	rText.left = min(rText.left,rText.right - (dcMem.GetTextExtent("Count",5)).cx);
	// ========================================================================

	// Paint stats block ------------------------------------------------------
	// ....numbers
	dcMem.SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"%0.4f",m_nSum);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += 3*nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - 2*cxGap;
	rStats.right = rText.right + cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%0.4f",aNums[i]);
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}
	sprintf(buffer,"%d",m_nCount);
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left + size.cx;
	rText.top = rText.bottom - size.cy;
	dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
	rText.bottom += (nBarWidth - 1);
	sprintf(buffer,"%d",m_nCountPos);
	rText.top = rText.bottom - size.cy;
	dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	

	// ....':'
	dcMem.SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,":");
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	for ( i = 0; i < aNums.GetSize()+2; i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,":");
		dcMem.ExtTextOut(rText.right,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// ....stats labels
	dcMem.SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
	sprintf(buffer,"Maximum");
	size = dcMem.GetTextExtent(buffer,strlen(buffer));
	y = (long) (nHistTop + ((float)(nHistBottom - nHistTop) - (m_nNoOfBars * (nBarWidth-1)))/2.0);
	rStats.top = nHistTop;
	y += nBarWidth;
	rText.bottom = y;
	rText.right = rText.left - cxGap;
	rText.left = rText.right - size.cx;
	rStats.left = rText.left - cxGap;
	for ( i = 0; i < aLabels.GetSize(); i++ )
	{
		rText.top = rText.bottom - size.cy;
		sprintf(buffer,"%s",aLabels[i]);
		dcMem.ExtTextOut(rText.left,rText.bottom,ETO_CLIPPED | ETO_OPAQUE,rText,buffer,strlen(buffer),NULL);	
		rText.bottom += (nBarWidth - 1);
	}

	// .... draw rect around stats block
	rStats.bottom = rText.bottom;
	dcMem.MoveTo(rStats.left,rStats.top);
	dcMem.LineTo(rStats.right,rStats.top);
	dcMem.LineTo(rStats.right,rStats.bottom);
	dcMem.LineTo(rStats.left,rStats.bottom);
	dcMem.LineTo(rStats.left,rStats.top);
	// ========================================================================

	SaveBitmapFile(dcMem.GetSafeHdc(), (HBITMAP) bmp, strPathName);
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
	

	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
	fontText.DeleteObject();
	fontHeading.DeleteObject();
	brushBar.DeleteObject();
	pen.DeleteObject();
	
}

void CHistogram::OnSaveimage() 
{
	m_SaveImage = true;
	RedrawWindow();
	m_SaveImage = false;
	RedrawWindow();
	
}

void CHistogram::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
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
	
	//USED 300 'CAUSE THAT'S WHAT IT WAS WRITTEN ON
//	m_rectDraw.left = (long)((rect.left)/((double) point.x)*m_HRes);
//	m_rectDraw.right = (long)((point.x - rect.right)/((double) point.x)*m_HRes);
//	m_rectDraw.top = (long)((rect.top)/((double) point.y)*m_VRes);
//	m_rectDraw.bottom = (long)((point.y - rect.bottom)/((double) point.y)*m_VRes);
	//////////////////////////////////////////////////////////////
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = m_HRes;
	m_rectDraw.bottom = m_VRes;

	pInfo->SetMaxPage(1);	
	
//	CView::OnBeginPrinting(pDC, pInfo);
}

void CHistogram::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	
	CView::OnEndPrinting(pDC, pInfo);
}

void CHistogram::OnClose() 
{
	((CFrameWnd*)GetParent())->DestroyWindow();
	
}



void CHistogram::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CViewExtension::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CHistogram::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	
	
	return CView::OnNotify(wParam, lParam, pResult);
}

void CHistogram::OnCaptureChanged(CWnd *pWnd) 
{
	
	CView::OnCaptureChanged(pWnd);
}

void CHistogram::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	{//set window name
		CFrameWnd* fw = (CFrameWnd*)GetParent();
		CString str;
		fw->GetWindowText(str);
		str = str.Left(str.Find(":")+1);
		str.Insert(str.GetLength()," Histogram");
		fw->SetWindowText(str);
	}
	
}

void CHistogram::OnFont()
{
CPatnDoc * pDoc = (CPatnDoc*) GetDocument();
	LOGFONT lf;
	memcpy(&lf,&(pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,pDoc->m_lfCells.lfFaceName);
		
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	dlg.m_cf.Flags |= CF_LIMITSIZE;
	dlg.m_cf.nSizeMin = 3;
	dlg.m_cf.nSizeMax = 48;
	dlg.m_cf.rgbColors = pDoc->m_rgbFontCells;

	if ( dlg.DoModal() == IDOK )
	{
		memcpy(&(pDoc->m_lfCells),&lf,sizeof(LOGFONT));
		strcpy(pDoc->m_lfCells.lfFaceName,lf.lfFaceName);
		pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
		pDoc->m_nPointSizeCells = dlg.m_cf.iPointSize;///10; // iPointSize is in 1/10s
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();
		UpdateWindow();

		POSITION pos = pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = pDoc->GetNextView(pos);   
			pView->RedrawWindow();
		} 
	}
}
