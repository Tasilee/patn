// Tway.cpp : implementation file
//

#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "PatnDoc.h"
#include "patn.h"
//#include <gl\glu.h>
//#include <gl\gl.h>

#include "Tway.h"
//#include "CFont.h"
//#include "Glf.h"
#include "bitms.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//GLubyte *ReadDIBitmap(BITMAPINFO **info);


/////////////////////////////////////////////////////////////////////////////
// CTway

IMPLEMENT_DYNCREATE(CTway, CViewExtension)

CTway::CTway()
{
}

CTway::~CTway()
{
	for(long i=0;i<m_Tway.pcolourMatrix->GetSize();i++)
		delete m_Tway.pcolourMatrix->GetAt(i);
	delete m_Tway.pcolourMatrix;
}


BEGIN_MESSAGE_MAP(CTway, CViewExtension)
	//{{AFX_MSG_MAP(CTway)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__FLIPAXES, OnFlipaxes)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID__CLOSE, OnClose)
	ON_COMMAND(ID__FONT, OnFont)
	ON_COMMAND(ID__SAVEIMAGE, OnSaveimage)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTway drawing

void CTway::OnDraw(CDC* pDC)
{
	CPatnDoc*	pDoc = GetDocument();

	pDC->SetTextColor(pDoc->m_rgbFontCells);
	ASSERT_VALID(pDoc);

	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	long vertoffset;

	if(m_ScaleUp && !pDC->IsPrinting()){
		m_ScaleUp = false;
		pDoc->m_lfCells.lfHeight = (long)(pDoc->m_lfCells.lfHeight / (2*m_HScale));
	}

	pDC->SetBkMode(TRANSPARENT);

	long x,y,i,j;
	CRect rct;
	CRect rect;
	long move = 0;
	if(pDC->IsPrinting() || m_SaveImage){
		rect.top = m_rectDraw.top;
		rect.bottom = m_rectDraw.bottom;
		rect.left = m_rectDraw.left;
		rect.right = m_rectDraw.right;
	}else{
		GetClientRect(rect);
		m_HScale = 1;
	}
	GetClientRect(rct);

	
	//fonts!
	CFont horzfont,vertfont,headingfont,legendfont;
	long err = horzfont.CreateFont(pDoc->m_lfCells.lfHeight,pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
		pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
		pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
		pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
		pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
	err += vertfont.CreateFont(pDoc->m_lfCells.lfHeight,pDoc->m_lfCells.lfWidth,900,
		900,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
		pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
		pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
		pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
	err += headingfont.CreateFont(pDoc->m_lfCells.lfHeight*3,pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
		pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
		pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
		pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
		pDoc->m_lfCells.lfPitchAndFamily,"Times");
	err += legendfont.CreateFont(pDoc->m_lfCells.lfHeight*1,pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
		pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
		pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
		pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
		pDoc->m_lfCells.lfPitchAndFamily,"Times");

	m_colwidth = (long)fabs(pDoc->m_lfCells.lfHeight * 1.37);
	m_rowheight = m_colwidth;
	vertoffset = (long)fabs(pDoc->m_lfCells.lfHeight * .8 * 14);//height of legend + some
	m_colindent = (long)fabs(m_colwidth * 4.0);

	long width,height;

	//draw title
	pDC->SelectObject(&headingfont);
	pDC->TextOut(rect.left + m_colindent+m_colwidth,rect.top,"Two-way Table");

	//draw legend
	//draw surrounding rect
	pDC->MoveTo(rect.left + m_colwidth/2,rect.top+0);
	pDC->LineTo((long)(rect.left + m_colindent-m_colwidth*.5),rect.top+0);
	pDC->LineTo((long)(rect.left + m_colindent-m_colwidth*.5),rect.top+vertoffset-m_rowheight);
	pDC->LineTo(rect.left + m_colwidth/2,rect.top+vertoffset-m_rowheight);
	pDC->LineTo(rect.left + m_colwidth/2,rect.top+0);
	pDC->SelectObject(&legendfont);
	pDC->TextOut(rect.left + m_colwidth,rect.top+m_rowheight/5,"Legend");
	
	x = m_colwidth*2;
	y = m_rowheight/5 + m_rowheight;
	CString str;
	for(i=0;i<5;i++){
		str.Format("%1.1f",(1+i)*.2);
		y = (long)(m_rowheight/5 + m_rowheight*(i+1.2));
		pDC->TextOut(rect.left + x,rect.top+y,str);
		if(i == 0){
			pDC->MoveTo(rect.left + m_colwidth,(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2));
			pDC->LineTo(rect.left + m_colwidth + (long)(m_colwidth*.8),(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2));
			pDC->LineTo(rect.left + m_colwidth + (long)(m_colwidth*.8),(long)(rect.top+(i+1.2)*m_rowheight + m_rowheight*.8+m_rowheight*.2));
			pDC->LineTo(rect.left + m_colwidth,(long)(rect.top+(i+1.2)*m_rowheight + m_rowheight*.8+m_rowheight*.2));
			pDC->LineTo(rect.left + m_colwidth,(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2));
		}else{
			pDC->FillSolidRect((long)(rect.left + m_colwidth),(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2),(long)(m_colwidth*.8),(long)(m_rowheight*.8),
				drawColour(!!m_red,!!m_green,!!m_blue,!!m_bw,(CTway::colour)(i*3)));
			pDC->FillSolidRect(rect.left,rect.top,0,0,0x00ffffff);
		}
	}	

	//missing legend item
	y = (long)(m_rowheight/5 + m_rowheight*(i+1.2));
	pDC->TextOut(rect.left + x,rect.top+y,"?");
	if(m_bw){
		CRect rct;
		rct.top = (long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2);
		rct.bottom = (long)(m_rowheight*.8) + (long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2);
		rct.left = (long)(rect.left + m_colwidth);
		rct.right = (long)(rect.left + m_colwidth)+(long)(m_colwidth*.8);

		pDC->MoveTo(rct.right,rct.top);
		pDC->LineTo(rct.left,rct.bottom);
		pDC->MoveTo(rct.left,rct.top);
		pDC->LineTo(rct.right,rct.bottom);
		pDC->LineTo(rct.right,rct.top);
		pDC->LineTo(rct.left,rct.top);
		pDC->LineTo(rct.left,rct.bottom);
		pDC->LineTo(rct.right,rct.bottom);
	//	pDC->FillSolidRect((long)(rect.left + m_colwidth),(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2),(long)(m_colwidth*.8),(long)(m_rowheight*.8),
	//		drawColour(!!m_red,!!m_green,!!m_blue,!!m_bw,col00));
	}else{
		pDC->FillSolidRect((long)(rect.left + m_colwidth),(long)(rect.top+(i+1.2)*m_rowheight+m_rowheight*.2),(long)(m_colwidth*.8),(long)(m_rowheight*.8),
			drawColour(!!m_red,!!m_green,!!m_blue,!!m_bw,col00));
	}
	pDC->FillSolidRect(rect.left,rect.top,0,0,0x00ffffff);

	//get labels width
	rl_width = 0;
	cl_width = 0;
	pDC->SelectObject(&horzfont);
	for(i=0;i<m_Tway.m_pRowLabels.GetSize();i++){
		CSize s = pDC->GetTextExtent(m_Tway.m_pRowLabels[i]);
		if(s.cx > rl_width)
			rl_width = s.cx;		
	}
	for(i=0;i<m_Tway.m_pColumnLabels.GetSize();i++){
		CSize s = pDC->GetTextExtent(m_Tway.m_pColumnLabels[i]);
		if(s.cx > cl_width)
			cl_width = s.cx;		
	}
	//get labels height
	cl_height = 0;
	rl_height = 0;
	pDC->SelectObject(&vertfont);
	for(i=0;i<m_Tway.m_pRowLabels.GetSize();i++){
		CSize s = pDC->GetTextExtent(m_Tway.m_pRowLabels[i]);
		if(s.cx > rl_height)
			rl_height = s.cx;		
	}
	for(i=0;i<m_Tway.m_pColumnLabels.GetSize();i++){
		CSize s = pDC->GetTextExtent(m_Tway.m_pColumnLabels[i]);
		if(s.cx > cl_height)
			cl_height = s.cx;		
	}

	if(!m_Flipped){
		m_colindent = (m_colindent>rl_width+10)?m_colindent:rl_width+10;
		vertoffset = (vertoffset>cl_height+m_rowheight*4)?vertoffset:cl_height+m_rowheight*4;
		m_pRowTextLength = m_colindent;

		//row labels
		pDC->SelectObject(&horzfont);
		rct.left = 0;
		rct.right = (long)m_pRowTextLength;
		y = vertoffset;
		for(i=m_top;y<rect.Height() && i<m_Tway.m_pRowLabels.GetSize();i++){
			rct.top = y;
			rct.bottom = y + m_rowheight;
			pDC->TextOut(rect.left + rct.left,rect.top+rct.top,m_Tway.m_pRowLabels[m_Tway.rowOrder[i]]);//,rct,DT_BOTTOM | DT_LEFT);
		
			y += m_rowheight;
		}
		m_height = i;

		//col labels
		pDC->SelectObject(&vertfont);
		rct.top = 0;
		rct.bottom = vertoffset-m_rowheight;
		x = m_colindent;
		for(i=m_left;x<rect.Width() && i<m_Tway.m_pColumnLabels.GetSize();i++){
			rct.left = x;
			rct.right = x + m_colwidth;
			pDC->TextOut(rect.left + rct.left,rect.top+rct.bottom,m_Tway.m_pColumnLabels[m_Tway.columnOrder[i]]);//,rct,DT_BOTTOM | DT_LEFT);
		
			x += m_colwidth;
			width = x;
		}
		m_width = i;

		//data
		x = m_colindent;
		for(i=m_left;x<rect.Width() && i<m_Tway.m_pColumnLabels.GetSize();i++){
			y = vertoffset;
			for(j=m_top;y<rect.Height() && j<m_Tway.m_pRowLabels.GetSize();j++){
				rct.top = rect.top+y;
				rct.bottom = rect.top+y + m_rowheight;
				rct.left = rect.left + x;
				rct.right = rect.left + x + m_colwidth;

				long ro = m_Tway.rowOrder[j];
				long co = m_Tway.columnOrder[i];
				CTway::colour clr = m_Tway.pcolourMatrix->GetAt(ro)->GetAt(co);

				if(m_bw && clr == col00)
				{//missing in b/w					
					pDC->MoveTo(rct.right,rct.top);
					pDC->LineTo(rct.left,rct.bottom);
					pDC->MoveTo(rct.left,rct.top);
					pDC->LineTo(rct.right,rct.bottom);
					pDC->LineTo(rct.right,rct.top);
					pDC->LineTo(rct.left,rct.top);
					pDC->LineTo(rct.left,rct.bottom);
					pDC->LineTo(rct.right,rct.bottom);
				}
				else
				{
					long draw_colour = drawColour(!!m_red,!!m_green,!!m_blue,!!m_bw,clr);
					if(draw_colour != 0x00ffffff)
						pDC->FillSolidRect(rct,draw_colour);
				}
				
				y += m_rowheight;
				height = y;
			}
			x += m_colwidth;
		}
		//group seperators
		for(i=m_top;i<m_Tway.m_pRowLabels.GetSize();i++){
			for(long j=0;j<m_Tway.rowGroupBarIdx.GetSize();j++){
				long k = m_Tway.rowGroupBarIdx.GetAt(j);
				if(k == i ){
					//make line 4* thick
					//pDC->FillSolidRect(m_colindent+(i-m_top)*m_colwidth,0,4,height,0x00000000);
					pDC->FillSolidRect(rect.left,(long)(rect.top+vertoffset+(i-m_top)*m_rowheight-3*m_HScale),width,(long)(m_HScale*3),0x00000000);
					break;
				}
			}
		}
		for(i=m_left;i<m_Tway.m_pColumnLabels.GetSize();i++){
			for(long j=0;j<m_Tway.columnGroupBarIdx.GetSize();j++){
				long k = m_Tway.columnGroupBarIdx.GetAt(j);
				if(k == i ){
					//make line 4* thick
					pDC->FillSolidRect((long)(rect.left+m_colindent+(i-m_left)*m_colwidth-3*m_HScale),rect.top+vertoffset-cl_height-m_rowheight,(long)(3*m_HScale),height-(vertoffset-cl_height-m_rowheight),0x00000000);
					//pDC->FillSolidRect(0,vertoffset+(i-m_top)*m_rowheight-3,width,3,0x00000000);
					break;
				}
			}
		}
	}else{
		m_colindent = (m_colindent>cl_width+10)?m_colindent:cl_width+10;
		vertoffset = (vertoffset>rl_height+m_rowheight*4)?vertoffset:rl_height+m_rowheight*4;
		m_pRowTextLength = m_colindent;


		//row labels
		pDC->SelectObject(&horzfont);
		rct.left = 0;
		rct.right = (long)m_pRowTextLength;
		y = vertoffset;
		for(i=m_top;y<rect.Height() && i<m_Tway.m_pColumnLabels.GetSize();i++){
			rct.top = y;
			rct.bottom = y + m_rowheight;
			pDC->TextOut(rect.left + rct.left,rect.top+rct.top,m_Tway.m_pColumnLabels[m_Tway.columnOrder[i]]);//,rct,DT_BOTTOM | DT_LEFT);
			
			y += m_rowheight;
			height = y;
		}
		m_height = i;

		//col labels
		pDC->SelectObject(&vertfont);
		rct.top = 0;
		rct.bottom = vertoffset-m_rowheight;
		x = m_colindent;
		for(i=m_left;x<rect.Width() && i<m_Tway.m_pRowLabels.GetSize();i++){
			rct.left = x;
			rct.right = x + m_colwidth;
			pDC->TextOut(rect.left + rct.left,rect.top+rct.bottom,m_Tway.m_pRowLabels[m_Tway.rowOrder[i]]);//,rct,DT_BOTTOM | DT_LEFT);
		
			x += m_colwidth;
			width = x;
		}
		m_width = i;

		//data
		x = m_colindent;
		for(i=m_left;x<rect.Width() && i<m_Tway.m_pRowLabels.GetSize();i++){
			y = vertoffset;
			for(j=m_top;y<rect.Height() && j<m_Tway.m_pColumnLabels.GetSize();j++){
				rct.top = rect.top+y;
				rct.bottom = rect.top+y + m_rowheight;
				rct.left = rect.left + x;
				rct.right = rect.left + x + m_colwidth;
							
				long ro = m_Tway.rowOrder[i];
				long co = m_Tway.columnOrder[j];
				CTway::colour clr = m_Tway.pcolourMatrix->GetAt(ro)->GetAt(co);

				if(m_bw && clr == col00)
				{//missing in b/w					
					pDC->MoveTo(rct.right,rct.top);
					pDC->LineTo(rct.left,rct.bottom);
					pDC->MoveTo(rct.left,rct.top);
					pDC->LineTo(rct.right,rct.bottom);
					pDC->LineTo(rct.right,rct.top);
					pDC->LineTo(rct.left,rct.top);
					pDC->LineTo(rct.left,rct.bottom);
					pDC->LineTo(rct.right,rct.bottom);
				}
				else
				{
					long draw_colour = drawColour(!!m_red,!!m_green,!!m_blue,!!m_bw,clr);
					if(draw_colour != 0x00ffffff)
						pDC->FillSolidRect(rct,draw_colour);
					
				}
				
				y += m_rowheight;
			}
			x += m_colwidth;
		}

		//group seperators
		for(i=m_top;i<m_Tway.m_pColumnLabels.GetSize();i++){
			for(long j=0;j<m_Tway.columnGroupBarIdx.GetSize();j++){
				long k = m_Tway.columnGroupBarIdx.GetAt(j);
				if(k == i ){
					//make line 4* thick
					//pDC->FillSolidRect(m_colindent+(i-m_top)*m_colwidth,0,4,height,0x00000000);
					pDC->FillSolidRect(rect.left,(long)(rect.top+vertoffset+(i-m_top)*m_rowheight-3*m_HScale),width,(long)(3*m_HScale),0x00000000);
					break;
				}
			}
		}
		for(i=m_left;i<m_Tway.m_pRowLabels.GetSize();i++){
			for(long j=0;j<m_Tway.rowGroupBarIdx.GetSize();j++){
				long k = m_Tway.rowGroupBarIdx.GetAt(j);
				if(k == i ){
					//make line 4* thick
					pDC->FillSolidRect((long)(rect.left+m_colindent+(i-m_left)*m_colwidth-3*m_HScale),(long)(rect.top+vertoffset-rl_height-m_rowheight),(long)(3*m_HScale),height-(vertoffset-rl_height-m_rowheight),0x00000000);
					//pDC->FillSolidRect(0,vertoffset+(i-m_top)*m_rowheight-3,width,3,0x00000000);
					break;
				}
			}
		}		
	}

	headingfont.DeleteObject();
	legendfont.DeleteObject();
	horzfont.DeleteObject();
	vertfont.DeleteObject();

	IN_OnDraw = FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CTway diagnostics

#ifdef _DEBUG
void CTway::AssertValid() const
{
	CView::AssertValid();
}

void CTway::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTway message handlers

void CTway::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
	switch(nSBCode)
	{
		case SB_LINEDOWN: // scrolling forward through document
			m_top ++;
			break;
		case SB_LINEUP:	// scrolling backward through document
			m_top --;
			break;
		case SB_PAGEDOWN: // scrolling forward through document by a page
			m_top += m_height;
			break;
		case SB_PAGEUP:	// scrolling backward through document
			m_top -= m_height;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			m_top = (long)nPos;
			break;		
		case SB_ENDSCROLL:
		default:
			break;		
	}

	if(m_top < 0) 
		m_top = 0;
	if(!m_Flipped){
		if(m_top >= m_Tway.m_pRowLabels.GetSize()){
			m_top = m_Tway.m_pRowLabels.GetSize()-1;
		}
	}else{
		if(m_top >= m_Tway.m_pColumnLabels.GetSize()){
			m_top = m_Tway.m_pColumnLabels.GetSize()-1;
		}
	}

	RedrawWindow();

	SetScrollPos(SB_VERT,m_top);
}

void CTway::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	CView::OnHScroll(nSBCode, nPos, pScrollBar);

	switch(nSBCode)
	{
		case SB_LINERIGHT: // scrolling forward through document
			m_left ++;
			break;
		case SB_LINELEFT:	// scrolling backward through document
			m_left --;
			break;
		case SB_PAGERIGHT: // scrolling forward through document by a page
			m_left += m_width;
			break;
		case SB_PAGELEFT:	// scrolling backward through document
			m_left -= m_width;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			m_left = (long)nPos;
			break;		
		case SB_ENDSCROLL:
		default:
			break;		
	}

	if(m_left < 0) 
		m_left = 0;
	if(m_Flipped){
		if(m_left >= m_Tway.m_pRowLabels.GetSize()){
			m_left = m_Tway.m_pRowLabels.GetSize()-1;
		}
	}else{
		if(m_left >= m_Tway.m_pColumnLabels.GetSize()){
			m_left = m_Tway.m_pColumnLabels.GetSize()-1;
		}
	}

	RedrawWindow();

	SetScrollPos(SB_HORZ ,m_left,TRUE);
}

int CTway::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_ScaleUp = false;

	m_top = 0;m_left = 0;
	m_rowheight = 20;
	m_colwidth = 20;
	m_colindent = 50;
	m_height = 0;
	m_width = 0;

	m_printing = false;


	CPatnDoc* pDoc;
	//for setting up row/column order lists
	long idx = 0;	
	long oldIdx = 0;	
	long group = 0;	
	long value = -1;	
	long j = 0, i = 0;
	m_Flipped = false;
	double nglobMin,nglobMax,nglobRange;
	double temp,nNewValue;
	CArray <double, double> ncolRange;
	CArray <double, double> ncolMin;
	CArray <double, double> ncolMax;
	long k;
	CString label;
	m_printing = false;

	m_psd = NULL;


	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_menu.LoadMenu(IDR_MENU_TWAY);

	pDoc = (CPatnDoc*) m_pDocument;
	if(pDoc->GetNoOfColumns() <= 0 || pDoc->GetNoOfRows() <= 0)
		return -1;

	if((m_pLeftExtrinsic = pDoc->GetIdxLeftExtrinsicColumn()) == -1)
		m_pLeftExtrinsic = pDoc->m_aColumnGroups.GetSize();
	
	if((m_pTopExtrinsic = pDoc->GetIdxTopExtrinsicRow()) == -1)
		m_pTopExtrinsic = pDoc->m_aRowGroups.GetSize();

	pDoc->m_pExists = 1;
	//value setting
	m_colStand = pDoc->m_colstand;
	m_PresAbs = pDoc->m_presabs;
	m_nScale = pDoc->m_nScale;
	m_red = pDoc->m_red;
	m_green = pDoc->m_green;
	m_blue = pDoc->m_blue;
	m_bw = pDoc->m_bw;

	if(pDoc->GetMissingValue() >= 0 && pDoc->GetMissingValue() <= 1)
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Missing value is between 0 and 1.  This may cause problems with the Two Way Table results.");
	}

	ncolMin.SetSize(m_pLeftExtrinsic);
	ncolRange.SetSize(m_pLeftExtrinsic);
	ncolMax.SetSize(m_pLeftExtrinsic);
	m_Tway.pcolourMatrix = new CArray<colour_list*>;
	m_Tway.pcolourMatrix->SetSize(m_pTopExtrinsic);
	for(i=0;i<m_pTopExtrinsic;i++)
	{
		//*(m_Tway.pcolourMatrix->GetAt(i)) = new CArray<colour>;
		m_Tway.pcolourMatrix->SetAt(i,getColourList());
		m_Tway.pcolourMatrix->GetAt(i)->SetSize(m_pLeftExtrinsic);
	}

	// standardisations
	if (!m_colStand) {
		//for standardisation by global min max
		nglobMin = 1.7E308;
		nglobMax = -1.7E308;
		for (i = 0; i<m_pLeftExtrinsic; i++) {
			for (j = 0; j<m_pTopExtrinsic; j++) {
				temp = pDoc->GetValueAt(i,j);
				if(temp != pDoc->GetMissingValue()){
					nglobMin = min(nglobMin,temp);
					nglobMax = max(nglobMax,temp);
				}
			}
		}
		nglobRange = nglobMax - nglobMin;
	} else { //for standardisation by column
		for (i = 0; i<m_pLeftExtrinsic; i++) {
			ncolMin.SetAt(i,1.7E308);
			ncolMax.SetAt(i,-1.7E308);
			for (k = 0; k<m_pTopExtrinsic; k++) {
				temp = pDoc->GetValueAt(i,k);
				if(temp != pDoc->GetMissingValue()){
					ncolMin.SetAt(i,min(ncolMin.GetAt(i),temp));
					ncolMax.SetAt(i,max(ncolMax.GetAt(i),temp));
				}
			}
			ncolRange.SetAt(i,ncolMax.GetAt(i) - ncolMin.GetAt(i));
		}
	}

	//standardised matrix formed - pTway:  not yet created
	for (i = 0; i<m_pLeftExtrinsic; i++) {
		for (j = 0; j<m_pTopExtrinsic; j++) {
			if (!m_colStand) {
				if((nNewValue = pDoc->GetValueAt(i,j)) != pDoc->GetMissingValue())
					nNewValue = (pDoc->GetValueAt(i,j) - nglobMin)/(double)nglobRange;
				setMatrix(nglobMin, nNewValue, i, j,pDoc->GetValueAt(i,j));
			} else {
				if((nNewValue = pDoc->GetValueAt(i,j)) != pDoc->GetMissingValue())
					nNewValue = (pDoc->GetValueAt(i,j) - ncolMin.GetAt(i))/(double)ncolRange.GetAt(i);
				setMatrix(ncolMin.GetAt(i), nNewValue, i, j,pDoc->GetValueAt(i,j));
			}
		}
	}

	//setup group order matrix for rows & columns
	m_Tway.columnOrder.SetSize(m_pLeftExtrinsic);
	m_Tway.rowOrder.SetSize(m_pTopExtrinsic);
	for(i=0;pDoc->m_FuseTableRows.GetSize() && i<pDoc->m_DendOrderRows.GetSize();i++)
		m_Tway.rowOrder.SetAt(i,pDoc->m_DendOrderRows[i]);
	if(pDoc->m_DendOrderRows.GetSize() == 0 || 
		pDoc->m_FuseTableRows.GetSize() == 0)
	{
		int c,j;
		c = 0;
		for(i=0;c<m_pTopExtrinsic && i<pDoc->m_Rows.GetSize();i++)
		{
			for(j=0;c<m_pTopExtrinsic && j<pDoc->m_Rows[i]->GetSize();j++)
			{
				m_Tway.rowOrder.SetAt(c++,pDoc->m_Rows[i]->GetAt(j));
			}
		}
	}	
	for(i=0;pDoc->m_FuseTableCols.GetSize() && i<pDoc->m_DendOrderCols.GetSize();i++)
		m_Tway.columnOrder.SetAt(i,pDoc->m_DendOrderCols[i]);
	if(pDoc->m_DendOrderCols.GetSize() == 0 ||
		pDoc->m_FuseTableCols.GetSize() == 0 )
	{
		int c,j;
		c = 0;
		for(i=0;c<m_pLeftExtrinsic && i<pDoc->m_Cols.GetSize();i++)
		{
			for(j=0;c<m_pLeftExtrinsic && j<pDoc->m_Cols[i]->GetSize();j++)
			{
				m_Tway.columnOrder.SetAt(c++,pDoc->m_Cols[i]->GetAt(j));
			}
		}
	}	
	
	//setup rowGroupBarIdx & columnGroupBarIdx for group seperators
	//idx = 0;
	//for(i=0;i<pDoc->m_Rows.GetSize()-1;i++){
		//set by size!
	//	idx += pDoc->m_Rows[i]->GetSize();
//		m_Tway.rowGroupBarIdx.Add(idx);
//	}
	for(i=0;i<m_Tway.rowOrder.GetSize()-1;i++)
	{
		if(pDoc->GetRowGroups(m_Tway.rowOrder[i]) != pDoc->GetRowGroups(m_Tway.rowOrder[i+1]))
			m_Tway.rowGroupBarIdx.Add(i+1);
	}
	//idx = 0;
	//for(i=0;i<pDoc->m_Cols.GetSize()-1;i++){
	//	idx += pDoc->m_Cols[i]->GetSize();
	//	m_Tway.columnGroupBarIdx.Add(idx);
	//}
	for(i=0;i<m_Tway.columnOrder.GetSize()-1;i++)
	{
		if(pDoc->GetColumnGroups(m_Tway.columnOrder[i]) != pDoc->GetColumnGroups(m_Tway.columnOrder[i+1]))
			m_Tway.columnGroupBarIdx.Add(i+1);
	}

	//setup indexes
	m_pRowHeight = 20;
	m_pColWidth = 20;
	m_pRowTextLength = 70;
	m_pColumnTextLength = 70;
	m_pTopIdx = 0;
	m_pLeftIdx = 0;
	m_pBottomIdx = ((long) m_oldRect.bottom / m_pRowHeight) + 1;
	if(m_pBottomIdx > m_pTopExtrinsic)
		m_pBottomIdx = m_pTopExtrinsic;
	m_pRightIdx = ((long) m_oldRect.right / m_pColWidth) + 1;
	if(m_pRightIdx > m_pLeftExtrinsic)
		m_pRightIdx = m_pLeftExtrinsic;

	//setup scroll bars
	SetScrollRange(SB_HORZ,0,m_pLeftExtrinsic - 2);
	SetScrollRange(SB_VERT,0,m_pTopExtrinsic - 2);

	//copy across row/column labels
	for(i=0;i<m_pTopExtrinsic;i++)
	{
		pDoc->GetRowLabel(i,label);
		m_Tway.m_pRowLabels.Add(label);
	}
	for(i=0;i<m_pLeftExtrinsic;i++)
	{
		pDoc->GetColumnLabel(i,label);
		m_Tway.m_pColumnLabels.Add(label);
	}
	m_SaveImage = false;

	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;

	return 0;
}

void CTway::OnDestroy() 
{
	CPatnDoc* pDoc = GetDocument();

	pDoc->m_pExists = 0;

	CView::OnDestroy();
}

BOOL CTway::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CView::PreCreateWindow(cs);
}

COLORREF CTway::drawColour(bool red, bool green, bool blue, bool bw,colour input)
{
	COLORREF clr;

	switch (input) {
	case col00:
		clr = 0x0000ffff;//yellow
		break;
	case col0:
		clr = 0x00ffffff;
		break;
	case col1:
		if (blue)  clr = 0x00ffffc0;
		else if (green)  clr = 0x00c0ffff;
		else if (red)  clr = 0x00ffc0ff;
		else clr = 0x00f3f3f3;
		break;
	case col2:
		if (blue) clr = 0x00ffff80;
		else if (green)  clr = 0x0000ffff;
		else if (red)  clr = 0x00ff00ff;
		else clr = 0x00e3e3e3;
		break;
	case col3:
		if (blue)  clr = 0x00ffff00;
		else if (green) clr = 0x00ffffcc;
		else if (red) clr = 0x00cc00ff;
		else clr = 0x00cfcfcf;
		break;
	case col4:
		if (blue) clr = 0x00ffff40;
		else if (green)  clr = 0x0000cccc;
		else if (red)  clr = 0x00cc00cc;
		else clr = 0x00b8b8b8;
		break;
	case col5:
		if (blue)  clr = 0x00cccc00;
		else if (green) clr = 0x0000ff99;
		else if (red)  clr = 0x009900ff;
		else clr = 0x00a1a1a1;
		break;
	case col6:
		if (blue)  clr = 0x00ffcc00;
		else if (green)  clr = 0x0000ff66;
		else if (red)  clr = 0x006600ff;
		else clr = 0x008a8a8a;
		break;
	case col7:
		if (blue)  clr = 0x00ff9900;
		else if (green)  clr = 0x0000ff33;
		else if (red)  clr = 0x003300ff;
		else clr = 0x00737373;
		break;
	case col8:
		if (blue) clr = 0x00ff6600;
		else if (green)  clr = 0x0000ff00;
		else if (red)  clr = 0x000000ff;
		else clr = 0x005c5c5c;
		break;
	case col9:
		if (blue) clr = 0x00808000;
		else if (green) clr = 0x0000c000;
		else if (red)  clr = 0x000000c0;
		else clr = 0x00454545;
		break;
	case col10:
		if (blue) clr = 0x00ff0000;
		else if (green) clr = 0x00008000;
		else if (red) clr = 0x00000080;
		else clr = 0x002e2e2e;
		break;
	case col11:
		if (blue) clr = 0x00990000;
		else if (green) clr = 0x00008080;
		else if (red) clr = 0x00800080;
		else clr = 0x00171717;
		break;
	//case col12:
	default:
		clr = clr = 0x00000000;
	}
	return clr;
}


long CTway::chooseColour(long temp)
{
	switch (m_nScale) 
	{
	case 2:
		return (temp * 12);
	case 3:
		return (temp * 6);
	case 4:
		return (temp * 4);
	case 5:
		return (temp * 3);
	case 6:
		if (temp == 5) return 12;
		else if (temp == 4) return 9;
		else return (temp * 2);
	case 7:
		return (temp * 2);
	case 8:
		if (temp < 3) return temp;
		else return (temp * 2 - 2);
	case 9:
		if (temp == 8) return 12;
		else if (temp == 7) return 10;
		else if (temp == 6) return 8;
		else if (temp == 5) return 6;
		else return temp;
	case 10:
		if (temp == 9) return 12;
		else if (temp == 8) return 10;
		else if (temp == 7) return 8;
		else return temp;
	case 11:
		if (temp == 10) return 12;
		else if (temp == 9) return 10;
		else return temp;
	case 12:
		if (temp == 11) return 12;
		else return temp;
	case 13:
		return temp;
	}
	return temp;
}

CTway::colour_list* CTway::getColourList(void)
{
	colour_list* clist;
	clist = new CArray<colour>;
	return clist;
}

void CTway::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
//	CRect rect,temp;
//	CPoint point;

//	m_psd->GetMargins(&rect,&temp);
//	point.x = m_psd->GetPaperSize().cx;
//	point.y = m_psd->GetPaperSize().cy;

	// default preparation
	long i;
	
	//////////////////////////////////////////////////////////////
	//device dependant info for regions & size & aspect retaining
	//////////////////////////////////////////////////////////////
	long m_HRes = pDC->GetDeviceCaps(HORZRES);
	long m_VRes = pDC->GetDeviceCaps(VERTRES);
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

	long x,y,vertoffset;

	//increase size
	//fonts
	m_ScaleUp = true;
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
	pDoc->m_lfCells.lfHeight = (long)(pDoc->m_lfCells.lfHeight * 2*m_HScale);

	m_printleftidxs.RemoveAll();
	m_printtopidxs.RemoveAll();
	
	m_colwidth = (long)fabs(pDoc->m_lfCells.lfHeight * 1.37);
	m_rowheight = m_colwidth;
	vertoffset = (long)fabs(pDoc->m_lfCells.lfHeight * .8 * 14);//height of legend + some
	m_colindent = (long)fabs(m_colwidth * 4.0);

	if(!m_Flipped){
		//row labels
		if(m_Tway.m_pRowLabels.GetSize())
			m_printtopidxs.Add(0);
		y = vertoffset;
		for(i=0;i<m_Tway.m_pRowLabels.GetSize();i++){
			if(y >= m_rectDraw.Height()){
				m_printtopidxs.Add(i);
				y = vertoffset;
			}else{
				y += m_rowheight;
			}
		}
		
		//col labels
		if(m_Tway.m_pColumnLabels.GetSize())
			m_printleftidxs.Add(0);
		x = m_colindent;
		for(i=0;i<m_Tway.m_pColumnLabels.GetSize();i++){
			if(x >= m_rectDraw.Width()){
				x = m_colindent;
				m_printleftidxs.Add(i);
			}else{		
				x += m_colwidth;
			}
		}
	}else{
		//row labels
		if(m_Tway.m_pColumnLabels.GetSize())
			m_printtopidxs.Add(0);
		y = vertoffset;
		for(i=0;i<m_Tway.m_pColumnLabels.GetSize();i++){
			if(y >= m_rectDraw.Height()){
				m_printtopidxs.Add(i);
				y = vertoffset;
			}else{
				y += m_rowheight;
			}
		}

		//col labels
		if(m_Tway.m_pRowLabels.GetSize())
			m_printleftidxs.Add(0);
		x = m_colindent;
		for(i=0;i<m_Tway.m_pRowLabels.GetSize();i++){
			if(x >= m_rectDraw.Width()){
				m_printleftidxs.Add(i);
				x = m_colindent;
			}else{
				x += m_colwidth;
			}
		}
	}
	
	pInfo->SetMaxPage(m_printtopidxs.GetSize() * m_printleftidxs.GetSize());	
}

void CTway::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
}

BOOL CTway::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog * d =  new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	return DoPreparePrinting(pInfo);
}

void CTway::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	long page = pInfo->m_nCurPage-1;
	if(page < m_printtopidxs.GetSize()*m_printleftidxs.GetSize()){
		m_top = m_printtopidxs[page/m_printleftidxs.GetSize()];
		m_left = m_printleftidxs[page%m_printleftidxs.GetSize()];
		CView::OnPrint(pDC, pInfo);
	}
	m_top = 0;
	m_left = 0;
}

void CTway::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());
	
}

void CTway::OnFlipaxes() 
{
	m_top = 0;
	m_left = 0;

	if(m_Flipped){
		m_Flipped = false;
		//setup scroll bars
		SetScrollPos(SB_HORZ,0);
		SetScrollRange(SB_HORZ,0,m_pLeftExtrinsic - 2);
		SetScrollPos(SB_VERT,0);
		SetScrollRange(SB_VERT,0,m_pTopExtrinsic - 2);
	}else{
		m_Flipped = true;
		//setup scroll bars
		SetScrollPos(SB_HORZ,0);
		SetScrollRange(SB_HORZ,0,m_pTopExtrinsic - 2);
		SetScrollPos(SB_VERT,0);
		SetScrollRange(SB_VERT,0,m_pLeftExtrinsic - 2);
	}
	RedrawWindow();

}

BOOL CTway::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) 
		return true;
	inMouseWheel = true;

	if(m_mousewheelsensitivity == 0)
		m_mousewheelsensitivity = 40;

	long x,y,diff;
	
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y + x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	long max = m_pTopExtrinsic - 2;

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

void CTway::OnClose() 
{
	((CFrameWnd*)GetParent())->DestroyWindow();
	
}

void CTway::setMatrix(double min, double value, long i, long j,double original)
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
	long temp, col;
	double increment;
	colour_list* clist;
	
	clist = m_Tway.pcolourMatrix->GetAt(j);
	if (m_PresAbs) 
	{  // presence absence two-way
		value = pDoc->GetValueAt(i,j);
		if (original == pDoc->GetMissingValue())
		{
			clist->SetAt(i,col00);
		}else if(value == 0) {
			clist->SetAt(i,col0);
		} else {
			clist->SetAt(i,col12);
		}
	} else  
	{ // scale number set
		if(original == pDoc->GetMissingValue())
		{		
			clist->SetAt(i,col00);
		}
		else
		{
			increment = 1.0 / (1.0*m_nScale);
			temp = (long)(value/increment);
			col = chooseColour(temp);
			clist->SetAt(i,(colour) col);
		}
	}
}

void CTway::OnFont() 
{
	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	CFontDialog dlg(&pDoc->m_lfCells);
	dlg.m_cf.rgbColors = pDoc->m_rgbFontCells;

	if(dlg.DoModal() == IDOK){
		dlg.GetCurrentFont(&pDoc->m_lfCells);
		pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
	}	

	RedrawWindow();
}

void CTway::OnSaveimage() 
{
	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save Two-Way Table:";
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"emf","*.emf",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Enhanced Meta File (*.emf)|*.emf|Bitmap (*.bmp)|*.bmp||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
	long fi = pFileDlg->m_ofn.nFilterIndex;
	delete pFileDlg;

	m_colwidth = (long)fabs(pDoc->m_lfCells.lfHeight * 1.37);
	m_rowheight = m_colwidth;

	long vertoffset = (long)fabs(pDoc->m_lfCells.lfHeight * .8 * 14);//height of legend + some
	m_colindent = (long)fabs(m_colwidth * 4.0);

	long nWidth=0,nHeight=0;
	if(!m_Flipped){
		m_colindent = (m_colindent>rl_width+10)?m_colindent:rl_width+10;
		vertoffset = (vertoffset>cl_height+m_rowheight*4)?vertoffset:cl_height+m_rowheight*4;
		m_pRowTextLength = m_colindent;

		//row labels
		nHeight = vertoffset + m_Tway.m_pRowLabels.GetSize() * m_rowheight;
		//col labels
		nWidth = m_colindent + m_Tway.m_pColumnLabels.GetSize() * m_colwidth;
	}else{

		m_colindent = (m_colindent>cl_width+10)?m_colindent:cl_width+10;
		vertoffset = (vertoffset>rl_height+m_rowheight*4)?vertoffset:rl_height+m_rowheight*4;
		m_pRowTextLength = m_colindent;

		//row labels
		nHeight = vertoffset + m_Tway.m_pColumnLabels.GetSize() * m_rowheight;
		//col labels
		nWidth = m_colindent + m_Tway.m_pRowLabels.GetSize() * m_colwidth;
	}

	if(nWidth % 2 == 1) nWidth = nWidth + 1;
	if(nHeight %2 == 1) nHeight = nHeight + 1;

	m_top = 0;
	m_left = 0;
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = nWidth; m_rectDraw.bottom =nHeight;

	if(fi == 1)
	{
		this->DrawToMetaFile(strPathName);
	}
	else
	{
		CDC dcMem;
		CBitmap bmp;

		dcMem.CreateCompatibleDC(GetDC());

		bmp.CreateCompatibleBitmap(GetDC(),nWidth,nHeight);
		
		CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

		dcMem.BitBlt(0,0,nWidth,nHeight,GetDC(),0,0,WHITENESS);

		dcMem.SetBkMode(TRANSPARENT);
	
	

		m_SaveImage = true;
		OnDraw(&dcMem);
		m_SaveImage = false;

		SaveBitmapFile(dcMem.GetSafeHdc(), (HBITMAP) bmp, strPathName);

		DeleteDC(dcMem);
		bmp.DeleteObject();	
	}
	RedrawWindow();
}

BOOL CTway::SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) 
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

ErrExit3:
	_lclose(hFile); 

ErrExit2:     
	GlobalFree(pbmi); 

ErrExit1:     
	GlobalFree(pBits);     

	return bSuccess; 
}  

void CTway::DrawToMetaFile(CString filename) //if false, must deletemetafile g_emf after use
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
	
	//draw using met or pDC?

	m_SaveImage = true;
	OnDraw(pDC);
	m_SaveImage = false;

	//g_emf = CloseEnhMetaFile(met);
	DeleteEnhMetaFile(CloseEnhMetaFile(met));

}