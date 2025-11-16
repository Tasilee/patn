// histvw.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "scattplotvw.h"
#include "TableView.h"
#include <stdlib.h>
#include <stdio.h>
#include ".\scattplotvw.h"


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
// CScattplot

IMPLEMENT_DYNCREATE(CScattplot, CViewExtension)

CScattplot::CScattplot()
{
}

CScattplot::~CScattplot()
{
	delete [] data;
	delete [] idxs;
}

#ifdef _DEBUG
CPatnDoc* CScattplot::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CScattplot, CViewExtension)
	//{{AFX_MSG_MAP(CScattplot)
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
	ON_WM_CHAR()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID__FONT, OnFont)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID__DECREASESYMBOLSIZE, OnDecreaseSymbolSize)
	ON_COMMAND(ID__INCREASESIZE, OnIncreaseSymbolSize)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScattplot drawing

void CScattplot::OnDraw(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();

	pDC->SetTextColor(pDoc->m_rgbFontCells);
	CFont font,vfont;
	long err;
	CRect windowSize = m_dimensions;
	double print_scale = 1;

	//get drawing rect
	CRect rect;
	this->GetClientRect(rect);
	if(pDC->IsPrinting())
	{
		rect = this->m_rectDraw;
		//if(rect.Width() > rect.Height())
		//	rect.right = rect.left + rect.Height();
		//if(rect.Height() > rect.Width())
		//	rect.bottom = rect.top + rect.Width();

		//determine the correct scale by using the original 'windowSize'.
		double oldratio,newratio;
		oldratio = windowSize.Height() / (double)windowSize.Width();
		newratio = rect.Height() / (double)rect.Width();

		if(oldratio > newratio)
		{
			//reduce WIDTH of new screen
			rect.right = (long)(rect.left + rect.Height() / oldratio);
			print_scale = rect.Height() / (double)windowSize.Height();
		}
		else if(newratio > oldratio)
		{
			//reduce HEIGHT of new screen
			rect.bottom = (long)(rect.top + rect.Width() * oldratio);
			print_scale = rect.Width() / (double)windowSize.Width();
		}
		else
		{
			print_scale = rect.Height() / (double)windowSize.Height();
		}
	}
	else
	{
		this->GetClientRect(m_dimensions);
		windowSize = m_dimensions;
	}

	if(pDC->IsPrinting())
	{
		err = font.CreateFont((int)(pDoc->m_lfCells.lfHeight*print_scale),pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
			pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
			pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
			pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
			pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
		err = vfont.CreateFont((int)(pDoc->m_lfCells.lfHeight*print_scale),pDoc->m_lfCells.lfWidth,900,
			900,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
			pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
			pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
			pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
	}
	else
	{
		err = font.CreateFont(pDoc->m_lfCells.lfHeight,pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
			pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
			pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
			pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
			pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
		err = vfont.CreateFont(pDoc->m_lfCells.lfHeight,pDoc->m_lfCells.lfWidth,900,
			900,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
			pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
			pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
			pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
		GetWindowRect(windowSize);		
	}
	CFont* oldfont = pDC->SelectObject(&font);
	
	//calc internal drawing region offsets
	//CRect m_offset;
	CString xmax,xmin,ymax,ymin;
	ymax.Format("%g",m_yMax);
	xmax.Format("%g",m_xMax);
	ymin.Format("%g",m_yMin);
	xmin.Format("%g",m_xMin);
	CRect tmp1,tmp2; 
	tmp1 = rect; pDC->DrawText(ymax,tmp1,DT_CALCRECT);
	tmp2 = rect; pDC->DrawText(ymin,tmp2,DT_CALCRECT);
	m_offset = CRect(rect.left + (int)(2*print_scale) + max(tmp1.Width(),tmp2.Width()),rect.top+tmp2.Height(),rect.right-(int)(10*print_scale),rect.bottom - (int)(10*print_scale) - tmp2.Height()*2);

	//draw scale lines
	pDC->MoveTo(m_offset.left,m_offset.top);
	pDC->LineTo(m_offset.left,m_offset.bottom);
	pDC->LineTo(m_offset.right,m_offset.bottom);

	
	pDC->SetBkMode(TRANSPARENT);

	//draw scale labels
	tmp1 = CRect(0,tmp2.Height() / 2 ,(int)(m_offset.left-1*print_scale),(int)(3000*print_scale));
	pDC->DrawText(ymax,tmp1,DT_TOP | DT_RIGHT);	
	tmp1 = CRect(0,m_offset.bottom - tmp2.Height() / 2,(int)(m_offset.left-1*print_scale),(int)(3000*print_scale));
	pDC->DrawText(ymin,tmp1,DT_TOP | DT_RIGHT);	
	tmp1 = CRect((int)(m_offset.left+1*print_scale),(int)(m_offset.bottom + (2*print_scale)),(int)(3000*print_scale),(int)(3000*print_scale));
	pDC->DrawText(xmin,tmp1,DT_TOP | DT_LEFT);	
	tmp1 = CRect(0,(int)(m_offset.bottom + (2*print_scale)),(int)(m_offset.right-(2*print_scale)),(int)(3000*print_scale));
	pDC->DrawText(xmax,tmp1,DT_TOP | DT_RIGHT);	

	//draw x & y labels
	//tmp1 = CRect(0,tmp2.Height(), rect.right,3000);
	CString str,str1,str2;
	if(m_key == 1)
	{
		pDoc->GetRowLabel(m_k1,str1);
		pDoc->GetRowLabel(m_k2,str2);
	}
	else
	{
		pDoc->GetColumnLabel(m_k1,str1);
		pDoc->GetColumnLabel(m_k2,str2);
	}
	//str.Format("%s (x-axis) vs %s (y-axis)",str1,str2);
	//pDC->DrawText(str,tmp1,DT_TOP | DT_CENTER);
	pDC->DrawText(str1,CRect(m_offset.left,m_offset.bottom + tmp2.Height() + (int)(2*print_scale),m_offset.right,(int)(3000*print_scale)),DT_TOP | DT_CENTER);
	pDC->SelectObject(&vfont);
	//pDC->DrawText(str2,CRect(2,m_offset.top,m_offset.left,m_offset.bottom),DT_TOP | DT_CENTER);
	CSize s = pDC->GetTextExtent(str2);
	pDC->TextOut((int)(2*print_scale),m_offset.top+m_offset.Height()/2+s.cx/2,str2);
	pDC->SelectObject(&font);

	//draw points
	int i=0;
	int width = (int)(m_objsize*print_scale);
	
	while(i<size)
	{		
		double x = data[i], y = data[i+1];
		long lx = (long)((x - m_xMin) * m_offset.Width()/ (m_xMax-m_xMin))  + m_offset.left;
		long ly = (long)(m_offset.bottom - (y - m_yMin) * m_offset.Height()/ (m_yMax-m_yMin));
		if(m_sel == i && !pDC->IsPrinting())
		{
			pDC->FillSolidRect(lx-width,ly,width*2+1,1,0x000000ff);
			pDC->FillSolidRect(lx,ly-width,1,width*2+1,0x000000ff);

			tmp1 = CRect(lx+width+1,ly+width+1,3000,3000);
			CString str;
			if(m_key == 0) //rows
				pDoc->GetRowLabel(idxs[i/2],str);
			else
				pDoc->GetColumnLabel(idxs[i/2],str);
			pDC->DrawText(str,tmp1,DT_TOP | DT_LEFT | DT_CALCRECT);
			if(tmp1.right > rect.right-1) tmp1.left -= (tmp1.right - rect.right - 1);
			if(tmp1.bottom > rect.bottom-1) tmp1.top -= (tmp1.bottom - rect.bottom - 1);
			pDC->DrawText(str,tmp1,DT_TOP | DT_LEFT);
		}
		else
		{
			pDC->MoveTo(lx-width,ly);
			pDC->LineTo(lx+width+1,ly);
			pDC->MoveTo(lx,ly-width);
			pDC->LineTo(lx,ly+width+1);
		}		
		i+=2;
	}

	pDC->SelectObject(oldfont);
	font.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CScattplot diagnostics

#ifdef _DEBUG
void CScattplot::AssertValid() const
{
	CView::AssertValid();
}

void CScattplot::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScattplot message handlers

int CScattplot::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_objsize = 4;
	m_sel = -1;
	
	m_menu.LoadMenu(IDR_SCATTPLOT);
	
	m_psd = NULL;
	m_SaveImage = false;

	CPatnDoc* pDoc = GetDocument();
	//CArray<CArray<double>*, CArray<double>*> aMatrix;
	//CArray<double>* pArray;
	long		nSelCount;
	long		nCount;
//	double	nValue;
	double	nMissingValue = pDoc->GetMissingValue();
//	long		i,j,k;					   
	long		idxMin = 0;
	CString strLabel;

//	m_Hist.strHeading.Empty();

	if ( !pDoc->m_ColSelection->IsEmpty() )
	{
		m_key = 0;
		if((nCount = pDoc->GetIdxTopExtrinsicRow()) <= 0) nCount = pDoc->GetNoOfRows();
		data = new double[nCount*2];
		idxs = new long[nCount];
		long pos = 0;

		nSelCount = pDoc->m_ColSelection->GetCount();		
		size = nCount*2;
		if(nSelCount == 2)
		{
			long idx1 = pDoc->m_ColSelection->GetFirst();
			long idx2 = pDoc->m_ColSelection->GetNext();

			m_k1 = min(idx1,idx2);
			m_k2 = max(idx1,idx2);

			for(long j=0;j<nCount && pos < nCount*2;j++)
			{
				double v1 = pDoc->GetValueAt(m_k1,j);
				double v2 = pDoc->GetValueAt(m_k2,j);
				if(v1 == nMissingValue || v2 == nMissingValue) { size -= 2; continue; }

				double v = v1;
				if(v < m_xMin || pos == 0) m_xMin = v; if(v > m_xMax || pos == 0) m_xMax = v;
				data[pos++] = v;
				
				v = v2;
				if(v < m_yMin || pos == 1) m_yMin = v; if(v > m_yMax || pos == 1) m_yMax = v;
				data[pos++] = v;	
				idxs[(pos-2)/2] = j;
			}
		}
	}
	else if ( !pDoc->m_RowSelection->IsEmpty() )
	{
		m_key = 1;
		if((nCount = pDoc->GetIdxLeftExtrinsicColumn()) <= 0) nCount = pDoc->GetNoOfColumns();
		data = new double[nCount*2];
		idxs = new long[nCount];
		long pos = 0;

		nSelCount = pDoc->m_RowSelection->GetCount();

		size = nCount*2;
		if(nSelCount == 2)
		{
			long idx1 = pDoc->m_RowSelection->GetFirst();
			long idx2 = pDoc->m_RowSelection->GetNext();

			m_k1 = min(idx1,idx2);
			m_k2 = max(idx1,idx2);

			for(long j=0;j<nCount && pos < nCount*2;j++)
			{
				double v1 = pDoc->GetValueAt(j,m_k1);
				double v2 = pDoc->GetValueAt(j,m_k2);
				if(v1 == nMissingValue || v2 == nMissingValue) { size -= 2; continue; }

				double v = v1; 
				if(v < m_xMin || pos == 0) m_xMin = v; if(v > m_xMax || pos == 0) m_xMax = v;
				data[pos++] = v;
				v = v2;
				if(v < m_yMin || pos == 1) m_yMin = v; if(v > m_yMax || pos == 1) m_yMax = v;
				data[pos++] = v;	
				idxs[(pos-2)/2] = j;
			}
		}
	}	

	return 0;
}





void CScattplot::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());

}

BOOL CScattplot::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog *d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
	//if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	pInfo->SetMaxPage(1);	

	return DoPreparePrinting(pInfo);
}

void CScattplot::drawPrint(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();


	
}

void CScattplot::SaveIt(CDC* pDC)
{
	
	
}

void CScattplot::OnSaveimage() 
{
	CString strTitle = "Save Histogram:";
	CFileDialog * pFileDlg;
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	CString strPathName = pFileDlg->GetPathName();
	delete pFileDlg;


	// ========================================================================

	CDC dcMem;
	CBitmap bmp;

	//CFont * oldFont;
//	CPen * oldPen;
//	CBrush * oldBrush;

	CRect rect;
	GetClientRect(rect);

	dcMem.CreateCompatibleDC(GetDC());
	bmp.CreateCompatibleBitmap(GetDC(),rect.Width(),rect.Height());
	CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

	dcMem.BitBlt(0,0,rect.Width(),rect.Height(),GetDC(),0,0,WHITENESS);	
	
	OnDraw(&dcMem);

	SaveBitmapFile(dcMem.GetSafeHdc(), (HBITMAP) bmp, strPathName);
	dcMem.SelectObject(pOldBitmap);
	DeleteDC(dcMem);
	pOldBitmap->DeleteObject();
	bmp.DeleteObject();
	
}

void CScattplot::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
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

	m_rectDraw = pInfo->m_rectDraw;
	
//	CView::OnBeginPrinting(pDC, pInfo);
}

void CScattplot::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	
	CView::OnEndPrinting(pDC, pInfo);
}

void CScattplot::OnClose() 
{
	((CFrameWnd*)GetParent())->DestroyWindow();
	
}



void CScattplot::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CViewExtension::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CScattplot::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	
	
	return CView::OnNotify(wParam, lParam, pResult);
}

void CScattplot::OnCaptureChanged(CWnd *pWnd) 
{
	
	CView::OnCaptureChanged(pWnd);
}

void CScattplot::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	{//set window name
		CFrameWnd* fw = (CFrameWnd*)GetParent();
		CString str;
		fw->GetWindowText(str);
		str = str.Left(str.Find(":")+1);
		str.Insert(str.GetLength()," Scatterplot");
		fw->SetWindowText(str);
	}
	
}

void CScattplot::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CViewExtension::OnChar(nChar, nRepCnt, nFlags);
}

void CScattplot::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	m_rectDraw = pInfo->m_rectDraw;
	CViewExtension::OnPrint(pDC, pInfo);
}

void CScattplot::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPatnDoc* pDoc = GetDocument();

/*	CFont font;
	long err = font.CreateFont(pDoc->m_lfCells.lfHeight,pDoc->m_lfCells.lfWidth,pDoc->m_lfCells.lfEscapement,
		pDoc->m_lfCells.lfOrientation,pDoc->m_lfCells.lfWeight,pDoc->m_lfCells.lfItalic,
		pDoc->m_lfCells.lfUnderline,pDoc->m_lfCells.lfStrikeOut,pDoc->m_lfCells.lfCharSet,
		pDoc->m_lfCells.lfOutPrecision,pDoc->m_lfCells.lfClipPrecision,pDoc->m_lfCells.lfQuality,
		pDoc->m_lfCells.lfPitchAndFamily,pDoc->m_lfCells.lfFaceName);
	CDC * pDC = this->GetDC();
	if(pDC == NULL) return;
	
	CFont* oldfont = GetDC()->SelectObject(&font);

	//get drawing rect
	CRect rect;
	this->GetClientRect(rect);

	

	//calc internal drawing region offsets
	CRect m_offset;
	CString xmax,xmin,ymax,ymin;
	ymax.Format("%g",m_yMax);
	xmax.Format("%g",m_xMax);
	ymin.Format("%g",m_yMin);
	xmin.Format("%g",m_xMin);
	CRect tmp1,tmp2; 
	tmp1 = rect; pDC->DrawText(ymax,tmp1,DT_CALCRECT);
	tmp2 = rect; pDC->DrawText(ymin,tmp2,DT_CALCRECT);
	m_offset = CRect(rect.left + 2 + max(tmp1.Width(),tmp2.Width()),rect.top+tmp2.Height()*3,rect.right-10,rect.bottom - 10 - tmp2.Height());
*/
	//find closest point & select
	int i=0;
	int width = m_objsize;
	
	m_sel = -1;
	double dist = 200;

	while(i<size)
	{
		double x = data[i], y = data[i+1];
		x = ((x - m_xMin) * m_offset.Width()/ (m_xMax-m_xMin))  + m_offset.left;
		y = m_offset.bottom - ((y - m_yMin) * m_offset.Height()/ (m_yMax-m_yMin));
		
		double d = sqrt((x - point.x)*(x-point.x) + (y-point.y)*(y-point.y));
		if(d < dist) 
		{ 
			dist = d; 
			m_sel = i;
		}
		
		i += 2;
	}

	//update tableview

	POSITION pos;
	CView *pView;
	pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL) {
		pView = pDoc->GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	} 
	if(pView != NULL)
	{
		long nCount;
		if(m_key == 0)
		{
            if((nCount = pDoc->GetIdxTopExtrinsicRow()) <= 0) nCount = pDoc->GetNoOfRows()*2;
		}
		else
		{
			
			if((nCount = pDoc->GetIdxLeftExtrinsicColumn()) <= 0) nCount = pDoc->GetNoOfColumns()*2;
		}

		((CTableView*)pView)->m_pSelectedColumns->RemoveAll();
		((CTableView*)pView)->m_pSelectedRows->RemoveAll();
		if(m_key == 0)//columns
		{
			
			if(m_sel >=0 && m_sel % 2 == 0 && m_sel < nCount && 
				idxs[m_sel/2] >= 0 && idxs[m_sel/2] < pDoc->GetNoOfRows())
			{
				((CTableView*)pView)->m_pSelectedRows->SetAt(idxs[m_sel/2]);
				((CTableView*)pView)->SetActiveCellAt(0,idxs[m_sel/2],true);
			}
		}
		else
		{
			
			if(m_sel >=0 && m_sel % 2 == 0 && m_sel < nCount && 
				idxs[m_sel/2] >= 0 && idxs[m_sel/2] < pDoc->GetNoOfColumns())
			{
				((CTableView*)pView)->m_pSelectedColumns->SetAt(idxs[m_sel/2]);
				((CTableView*)pView)->SetActiveCellAt(idxs[m_sel/2],0,true);
			}
		}
		pos = pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			pView = pDoc->GetNextView(pos);   
			if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
				pView->RedrawWindow();
		}
	}

//	pDC->SelectObject(oldfont);
//	font.DeleteObject();

	RedrawWindow();

	CViewExtension::OnLButtonUp(nFlags, point);
}

void CScattplot::OnFont()
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
	}
}

void CScattplot::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{			
		case VK_PRIOR:
		case 46:
			if(m_objsize < 20) m_objsize ++;
			RedrawWindow();
			break;		          		
		case VK_NEXT:
		case 44:
			if(m_objsize > 0) m_objsize --;
			RedrawWindow();
			break;
	}


	CViewExtension::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CScattplot::OnDecreaseSymbolSize()
{
	
	if(m_objsize > 0) m_objsize --;
		RedrawWindow();
}

void CScattplot::OnIncreaseSymbolSize()
{
	if(m_objsize < 20) m_objsize ++;
		RedrawWindow();
}
