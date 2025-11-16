// BoxWhiskerView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "BoxWhiskerView.h"
#include "gsta.h"

#include "bitms.h"                         


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerView

bool g_scrolling = false;

IMPLEMENT_DYNCREATE(CBoxWhiskerView, CViewExtension)

CBoxWhiskerView::CBoxWhiskerView()
{
	m_psd = NULL;
}

CBoxWhiskerView::~CBoxWhiskerView()
{
	m_pDoc->m_boxwhiskervisible = 0;
	if(m_groups && m_pDoc->m_kw_patn_generated_groups)
	{
		delete m_groups;
	}
}


BEGIN_MESSAGE_MAP(CBoxWhiskerView, CViewExtension)
	//{{AFX_MSG_MAP(CBoxWhiskerView)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__SAVEIMAGE, OnSaveimage)
	ON_COMMAND(ID__CLOSE, OnClose)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID__SORTALPHABETICALLY, OnSortalphabetically)
	ON_COMMAND(ID__SORTBYKRUSKALWALLIS, OnSortbykruskalwallis)
	ON_COMMAND(ID__SORTBYTABLEORDER, OnSortbytableorder)
	ON_UPDATE_COMMAND_UI(ID__SORTBYKRUSKALWALLIS, OnUpdateSortbykruskalwallis)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID__FONT, OnFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerView drawing

void CBoxWhiskerView::OnDraw(CDC* pDC)
{
	if(!m_pDoc->m_KWExists) 
	{
		this->GetParentFrame()->PostMessage(WM_CLOSE);
	}
	if(g_scrolling) return;

	CRect rect;
	long div = 3;
	long width = 30,height = 20;
	long xstart,ystart,xend,yend;
	xstart = 150;ystart = 10;xend = 50;yend = 0;
	
	pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	long print = 1;
	if(m_saveimage){
		rect.top = m_rectDraw.top;
		rect.bottom = m_rectDraw.bottom;
		rect.left = m_rectDraw.left;
		rect.right = m_rectDraw.right;

		div = 3;
		width = abs((long)(m_pDoc->m_lfCells.lfHeight* 1.37 * 2.0));height = abs((long)(m_pDoc->m_lfCells.lfHeight * 1.37));
		xstart = width*5 + rect.left;ystart = height+width+rect.top;xend = 10;yend = 0;
	}else if(pDC->IsPrinting()){
		print = 1;

		width = abs((long)(m_pDoc->m_lfCells.lfHeight* 1.37 * 2.0));height = abs((long)(m_pDoc->m_lfCells.lfHeight * 1.37));

		rect.top = m_rectDraw.top;
		rect.bottom = m_rectDraw.top + height*(m_NumPerPage+5);//*m_pDoc->GetNoOfColumns();
		rect.left = m_rectDraw.left;
		rect.right = m_rectDraw.right;

		div = (long)(3*m_VScale*2);
		xstart = width*5 + rect.left;ystart = height*3+rect.top;xend = 10;yend = 0;	
	}else {
		GetClientRect(rect);
  		width = abs((long)(m_pDoc->m_lfCells.lfHeight* 1.37 * 2.0));height = abs((long)(m_pDoc->m_lfCells.lfHeight * 1.37));
		//xstart = width*5 + rect.left;ystart = width+height+rect.top;xend = 10;yend = 0;
		xstart = width*5 + rect.left;ystart = height+rect.top;xend = 10;yend = 0;
	}

   CFont newfont;
	long err = newfont.CreateFont(m_pDoc->m_lfCells.lfHeight,m_pDoc->m_lfCells.lfWidth,m_pDoc->m_lfCells.lfEscapement,
		m_pDoc->m_lfCells.lfOrientation,m_pDoc->m_lfCells.lfWeight,m_pDoc->m_lfCells.lfItalic,
		m_pDoc->m_lfCells.lfUnderline,m_pDoc->m_lfCells.lfStrikeOut,m_pDoc->m_lfCells.lfCharSet,
		m_pDoc->m_lfCells.lfOutPrecision,m_pDoc->m_lfCells.lfClipPrecision,m_pDoc->m_lfCells.lfQuality,
		m_pDoc->m_lfCells.lfPitchAndFamily,m_pDoc->m_lfCells.lfFaceName);
	CFont* oldfont = pDC->SelectObject(&newfont);
	//lines
	//pDC->MoveTo(rect.right - xend,ystart - height*print);
	//pDC->LineTo(rect.right - xend,ystart - height/2*print);
	//pDC->LineTo(xstart,ystart - height/2*print);
	//pDC->LineTo(xstart,ystart - height*print);
	//pDC->MoveTo(xstart,ystart);
	//pDC->LineTo(xstart,rect.bottom - yend);
	//pDC->MoveTo((rect.right - xend - xstart) / 2 + xstart,ystart - height*print);
	//pDC->LineTo((rect.right - xend - xstart) / 2 + xstart,ystart - height/2*print);

	//min - max labels
	//CRect rct;
	//rct.top = ystart-width*print;rct.bottom = ystart-height/2*print;
	//rct.left = xstart;rct.right = rect.right-xend;
	//CString str;
	//str.Format("%.3f",m_max);
	//pDC->DrawText(str,rct,DT_BOTTOM|DT_RIGHT);
	//str.Format("%.3f",m_min);
	//pDC->DrawText(str,rct,DT_BOTTOM|DT_LEFT);
	//str.Format("%.3f",(m_max+m_min)/2.0);
	//pDC->DrawText(str,rct,DT_BOTTOM|DT_CENTER);
	
	//group/mean labels
	//if(pDC->IsPrinting()) print = 2*m_VScale;
	//rct.left = xstart - 2.5*print*width;
	//rct.right = xstart - 5*print*width/40;
	//rct.bottom += 10*print*width/40;
	//rct.top += 15*print*width/40;
	//str.Format("Group    Mean");
	//pDC->DrawText(str,rct,DT_LEFT|DT_BOTTOM);
	//print = 1;

	long ypos;

	//col labels
	long count = 0;Stat * s;
	//rct.top = rect.Height() - 48;rct.bottom = rect.Height() - 28;

	long minc;
	if(m_gstastats->GetSize() >= 4){
		minc = 1;
	}else if(m_gstastats->GetSize() >= 3){
		minc = 2;
	}else{
		minc = 0;
	}
	minc = 0;
	
	//add extra row at bottom

	pDC->SetBkMode(TRANSPARENT);
	long j,idx;
	CRect rct;
	CString str;
	ypos = ystart;
	long istart = 0;
	long iend = 99999;
	if(pDC->IsPrinting()){
		if((m_groups->GetSize()+4)/(double)m_NumPerPage >= 1)
		{
			istart = (long)(pos/ceil((m_groups->GetSize()+4)/(double)m_NumPerPage));
			iend = pos + m_NumPerPage;
			count = (pos%(long)ceil((m_groups->GetSize()+4)/(double)m_NumPerPage))*m_NumPerPage;
			iend = istart+1;
		}
		else
		{
			istart = pos*m_NumPerPage/(m_groups->GetSize()+4);
			iend = istart + m_NumPerPage/(m_groups->GetSize()+4);
			//count = (pos%(long)ceil((m_pDoc->m_Groups.GetSize()+4)/(double)m_NumPerPage))*m_NumPerPage;
			//iend = istart+1;
			count = 0;
		}
		//pos = count;
	}
	//auto
	if(m_sortbyattribute == 2 && m_pDoc->m_KW.GetSize() == 0){
		pDC->TextOut(20,20,"No KW Values");
		this->GetParentFrame()->PostMessage(WM_CLOSE);
	}
	for(long i=istart;i < iend && m_gstastats->GetSize() && //((i<m_pDoc->m_Gstastats[0]->GetSize() && m_sortbyattribute != 2) || 
		//(m_sortbyattribute == 2 && i<m_pDoc->m_KW.GetSize())) && 
		i<m_pDoc->m_KW.GetSize() && 
		ypos < rect.bottom;i++){
		if(m_sortbyattribute == 2){
			if(i >= 0 && i < m_pDoc->m_KW.GetSize() && 
				m_pDoc->m_KW[m_pDoc->m_KW.GetSize()-i-1] < m_pDoc->m_KWAttributes.GetSize() &&
				m_pDoc->m_KW[m_pDoc->m_KW.GetSize()-i-1] >= 0)
				idx = m_pDoc->m_KWAttributes[m_pDoc->m_KW[m_pDoc->m_KW.GetSize()-i-1]];
		}else if(m_sortbyattribute == 1){
			//idx = i;
			idx = m_pDoc->m_KWAttributes[i];
		}else{
			//get the i'th table (by name order)
			idx = m_alphatable[i];
		}

		//find local (to this attribute) max,min
		if(idx >= 0 && m_gstastats->GetAt(0) != NULL &&
			idx < m_gstastats->GetAt(0)->GetSize()){
			m_max = m_gstastats->GetAt(0)->GetAt(idx)->maximum;
			m_min = m_gstastats->GetAt(0)->GetAt(idx)->minimum;
			for(j=1;j<m_gstastats->GetSize() && m_gstastats->GetAt(j) != NULL &&
				idx < m_gstastats->GetAt(j)->GetSize() &&
				j < m_groups->GetSize();j++){
				double im = m_gstastats->GetAt(j)->GetAt(idx)->maximum;
				double ix = m_gstastats->GetAt(j)->GetAt(idx)->minimum;
				if(im > m_max && im != m_pDoc->GetMissingValue())
					m_max = im;
				if(ix < m_min && ix != m_pDoc->GetMissingValue())
					m_min = ix;
			}
			if(m_min == m_pDoc->GetMissingValue())
				m_min = 0;
			if(m_max == m_pDoc->GetMissingValue())
				m_max = 0;
		}

		long right_margin;
		long group_x,mean_x;

		//lines
		bool in;
		in = false;
		long ylabel = 0;
		if(count+m_gstastats->GetSize() > pos){
			in = true;

			ypos += height*2;

			//min - max labels
			rct.top = ypos+height*2-width*print;rct.bottom = ypos+height*2-height/2*print;
			rct.bottom += 10*print*width/40-height;
			rct.top += 15*print*width/40-height;

			//group/mean labels
			rct.left = //(long)(xstart - 2.5*print*width);
				print*width;
		//	rct.right = (long)(xstart - 5*print*width/40);
			str.Format("Group   Mean");
			//get the metrics for the group mean label
			right_margin = pDC->GetOutputTextExtent(str+"000000000").cx + rct.left;
			rct.right = right_margin;

			group_x = pDC->GetOutputTextExtent(" ").cx + rct.left;
			mean_x = pDC->GetOutputTextExtent("Group   ").cx + rct.left;

			pDC->DrawText(str,rct,DT_LEFT|DT_TOP);

			pDC->MoveTo(rect.right - xend,ypos+height - height*print);
			pDC->LineTo(rect.right - xend,ypos+height - height/2*print);
			pDC->LineTo(right_margin,ypos+height - height/2*print);
			pDC->LineTo(right_margin,ypos+height - height*print);
			//pDC->MoveTo(xstart,ystart);
			//pDC->LineTo(xstart,rect.bottom - yend);
			pDC->MoveTo((rect.right - xend - right_margin) / 2 + right_margin,ypos+height+7 - height*print);
			pDC->LineTo((rect.right - xend - right_margin) / 2 + right_margin,ypos+height+6  - height/2*print);

			//min - max labels
			rct.top = ypos+height*2-width*print;rct.bottom = ypos+height*2-height/2*print;
			rct.left = right_margin;rct.right = rect.right-xend;
			rct.bottom += 10*print*width/40-height;
			rct.top += 15*print*width/40-height;

			CString formatstr;
			if(m_max != m_pDoc->GetMissingValue()){
				if(m_max-m_min < 10){
					formatstr = "%.2f";
				}else if(m_max-m_min < 100){
					formatstr = "%.1f";
				}else{
					formatstr = "%.0f";
				}
				str.Format(formatstr + " ",m_max);
				rct.bottom -= 2;
				rct.top -= 2;
				pDC->DrawText(str,rct,DT_TOP|DT_RIGHT);
				str.Format(" " + formatstr,m_min);
				pDC->DrawText(str,rct,DT_TOP|DT_LEFT);
				long len = formatstr.GetLength();			
				//for(long space=0;space<len;space++)
				//	formatstr = " " + formatstr;
				str.Format("    " + formatstr,(m_max+m_min)/2.0);
				rct.left += 4;
				pDC->DrawText(str,rct,DT_TOP|DT_CENTER);
			}

			
			print = 1;
			

			rct.left = (long)(height/2.0*print+rect.left);//rct.right = xstart-(width+height)*print+rect.left;
			rct.right = (long)(rect.right);
	//		rct.top = ypos;rct.bottom = ypos + height;
			if(idx < m_pDoc->GetNoOfColumns())
				m_pDoc->GetColumnLabel(idx,str);
			
			rct.top -= height*2;
			rct.bottom -= height*2;
			pDC->DrawText(str,rct,DT_LEFT|DT_TOP);

			rct.top = ypos;rct.bottom = ypos + height;

			ylabel = rct.top+height;

			rct.top += (minc) * height;
			rct.bottom += (minc) * height;
		}
			
		long st = 0;
		if(pDC->IsPrinting())
			st =  (pos%(long)ceil((m_groups->GetSize()+4)/(double)m_NumPerPage))*m_NumPerPage;
		for(j=st;j<m_gstastats->GetSize() && m_gstastats->GetAt(j) != NULL &&
			idx >=0 && idx < m_gstastats->GetAt(j)->GetSize() &&
			j < m_groups->GetSize();j++){			
			s = m_gstastats->GetAt(j)->GetAt(idx);
			count++;
			if((count > pos || pDC->IsPrinting()) && ypos < rect.bottom){
				ypos += height;
				//rct.left = xpos;rct.right = xpos + width;
				
				//if(pDC->IsPrinting()) {
				//	print = 2*m_VScale;
				//	rct.left = xstart - 2.5*print*width;rct.right = xstart-5*print*width/40;
				//	rct.top = ypos;rct.bottom = ypos + height;
				//	print = 1;					
				//}else{
					//rct.left = (long)(xstart - 2.5*print*width);rct.right = (long)(xstart-5*print*width/40);
					rct.left = (long)(xstart - 2.5*print*width);rct.right = right_margin; //rct.right = (long)(xstart - 5*print*width/40);
					rct.top = (long)(ypos);rct.bottom = (long)(ypos + height);
				//}

				long numdec = (long)log10(fabs(s->mean));
				str.Format("%3d",j+1);
				rct.left = group_x;				
				pDC->DrawText(str,rct,DT_LEFT|DT_BOTTOM);

				CString formatstr;
				if(numdec <= 2){
					formatstr.Format("%%.%df",2-numdec);
					str.Format(formatstr,s->mean);
				}else{
					str.Format("%g",s->mean);
				}

				//str.Format("  %3d%12.2f",j+1,s->mean);
				rct.left = mean_x;

				if(s->mean == m_pDoc->GetMissingValue()){
					rct.right = rect.right;
					pDC->DrawText("Unavailable",rct,DT_LEFT|DT_BOTTOM);
				}else{
					pDC->DrawText(str,rct,DT_LEFT|DT_BOTTOM);
				

					//rct.left = (long)(xstart - 3*print*width);
					rct.right = (long)(xstart-5*print*width/40.0);
					rct.left = right_margin;
					rct.top = ypos;rct.bottom = ypos + height;								

					//draw box & whisker

					if(m_max-m_min == 0){//range == 0
						pDC->Rectangle(right_margin,ypos+div,(rect.right-right_margin-xend)+right_margin,ypos+height-div);
					}else{
						//top bar
						pDC->MoveTo((long)(((s->maximum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						pDC->LineTo((long)(((s->maximum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height-div);
						//bottom bar
						pDC->MoveTo((long)(((s->minimum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						pDC->LineTo((long)(((s->minimum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height-div);
						//mean bar
						pDC->MoveTo((long)(((s->median-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						pDC->LineTo((long)(((s->median-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height-div);
						//box	
						pDC->MoveTo((long)(((s->first_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						pDC->LineTo((long)(((s->first_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height-div);
						pDC->LineTo((long)(((s->third_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height-div);
						pDC->LineTo((long)(((s->third_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						pDC->LineTo((long)(((s->first_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+div);
						//high line
						pDC->MoveTo((long)(((s->maximum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height/2);
						pDC->LineTo((long)(((s->third_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height/2);
						//low line
						pDC->MoveTo((long)(((s->minimum-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height/2);
						pDC->LineTo((long)(((s->first_quartile-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin),ypos+height/2);
						//mean dot
						pDC->SetPolyFillMode(WINDING);
						long px = (long)(((s->mean-m_min)/(m_max-m_min))*(rect.right-right_margin-xend)+right_margin);
						pDC->Ellipse(px-div,ypos-div+height/2,px+div,ypos+div+height/2);
					}
				}

				//xpos += width;
				//ypos += height;
			}			
			//bool qn = j+1<m_gstastats->GetSize();
			//bool nn = m_gstastats->GetAt(j+1) != NULL;
			//bool mn = idx < m_gstastats->GetAt(j+1)->GetSize();
			//bool rn = j+1 < m_groups->GetSize();
		
		}

		if(in){
			ypos += height;

			if(m_sortbyattribute != 2){
				//find idx in m_pDoc->m_KW(idx) & use idx			
				//for(j=0;j<m_pDoc->m_KW.GetSize();j++){
				//	if(idx == m_pDoc->m_KW[j]) 
				//		break;
				//}
				//if(j < m_pDoc->m_KW.GetSize() && minc){
				//	str.Format("Kruskal-Wallis");
				//	pDC->DrawText(str,rct,DT_LEFT|DT_BOTTOM);
				//	rct.top += height;
				//	rct.bottom += height;
				//	str.Format("    %3.4f",m_pDoc->m_KWValues[j]);
					//pDC->DrawText(str,rct,DT_LEFT|DT_BOTTOM);
				//}

				//find i in m_pDoc->m_KW(idx) & use idx
				for(j=0;j<m_pDoc->m_KW.GetSize();j++){
					if(idx == m_pDoc->m_KWAttributes[m_pDoc->m_KW[j]]) 
						break;
				}
				if(j >= 0 && j < m_pDoc->m_KWAttributes.GetSize() && !minc && 
					count >= pos && j < m_pDoc->m_KWValues.GetSize()){
					//if(pDC->IsPrinting()) {
					//	print = 2*m_VScale;
					//	rct.left = xstart-2.5*print*width;rct.right = xstart;
					//	rct.top = ypos;rct.bottom = ypos + height;
					//	print = 1;					
					//}else{
		//				rct.left = xstart-2.5*print*width;rct.right = xstart;
		//				rct.top = ypos;rct.bottom = ypos + height;
					//}							
					//pDC->FillSolidRect(rct.left,rct.top,4*width*print,height,0x00ffffff);
		//			ypos += height;
		//			rct.right += 4*width*print;
					
					
					//str.Format("Kruskal-Wallis Value = %4.3f",m_pDoc->m_KWValues[j]);
					long numdec = (long)log10(m_pDoc->m_KWValues[j]);
					CString formatstr;
					if(numdec <= 4)
						formatstr.Format("KW = %%.%df",4-numdec);
					else
						formatstr = "KW = %.0f";
					str.Format(formatstr,m_pDoc->m_KWValues[j]);
					//rct.left = xstart - 2.5*print*width;rct.right = rect.right;
					//rct.top = ypos;rct.bottom = ypos + height;
					
					rct.left = (long)(rect.left+print*width);//rct.right = xstart-(width+height)*print+rect.left;
					rct.right = (long)(rect.right);
					rct.top = (long)(ylabel - height*2.3);rct.bottom = ylabel+ height;

					pDC->DrawText(str,rct,DT_LEFT|DT_TOP);
					rct.top = ylabel;
				}
			}else{
				if(i>=0 && i<m_pDoc->m_KWValues.GetSize()){
					//str.Format("Kruskal-Wallis Value = %4.3f",m_pDoc->m_KWValues[m_pDoc->m_KWValues.GetSize()-i-1]);
					long numdec = (long)log10(m_pDoc->m_KWValues[m_pDoc->m_KWValues.GetSize()-i-1]);
					CString formatstr;
					if(numdec <= 4)
						formatstr.Format("KW = %%.%df",4-numdec);
					else
						formatstr = "KW = %.0f";
					str.Format(formatstr,m_pDoc->m_KWValues[m_pDoc->m_KWValues.GetSize()-i-1]);
				}
				
				//rct.left = xstart - 2.5*print*width;rct.right = rect.right;
				//rct.top = ypos;rct.bottom = ypos + height;
				rct.left = (long)(rect.left+print*width);//rct.right = xstart-(width+height)*print+rect.left;
				rct.right = (long)(rect.right);
				rct.top = (long)(ylabel - height*2.3);rct.bottom = ylabel+ height;
								
				pDC->DrawText(str,rct,DT_LEFT|DT_TOP);
				rct.top = ylabel;
			}	
			
			//ypos += height;
		}
		
		if(count > pos){
			//if(pDC->IsPrinting()) {
			//	print = 2*m_VScale;
			//	rct.left = xstart-2.5*print*width;rct.right = xstart;
			//	rct.top = ypos;rct.bottom = ypos + height;
			//	print = 1;					
			//}else{
				rct.left = (long)(xstart-2.5*print*width);rct.right = xstart;
				rct.top = ypos;rct.bottom = ypos + height;
			//}							
		//	pDC->FillSolidRect(rct.left,rct.top,4*width*print,2*height,0x00ffffff);
			count ++;
			ypos += height;
		}
		if(count > pos && ypos < rect.Height()){
			//column seperators
			//pDC->MoveTo(xstart - width/2.0*print,ypos);
			//pDC->LineTo(xstart,ypos);
		}

	}

   pDC->SelectObject(oldfont);
}



/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerView diagnostics

#ifdef _DEBUG
void CBoxWhiskerView::AssertValid() const
{
	CView::AssertValid();
}

void CBoxWhiskerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerView message handlers

int CBoxWhiskerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDoc = (CPatnDoc*)GetDocument();

	//build m_alphatable
	long i,j;
	CString str1,str2;
	CArray<long> oldints;
	//for(i=0;i<m_pDoc->GetNoOfColumns();i++){
		//oldints.Add(i);
	//}
	for(i=0;i<m_pDoc->m_KW.GetSize();i++)
	{
		oldints.Add(m_pDoc->m_KWAttributes[m_pDoc->m_KW[m_pDoc->m_KW.GetSize()-i-1]]);
	}
	while(oldints.GetSize() > 0){
		j = 0;
		m_pDoc->GetColumnLabel(oldints[j],str1);
		for(i=1;i<oldints.GetSize();i++){
			m_pDoc->GetColumnLabel(oldints[i],str2);
			if(str2 < str1){
				str1 = str2;j = i;
			}
		}
		m_alphatable.Add(oldints[j]);
		oldints.RemoveAt(j);
	}
	
	
	m_pDoc->m_boxwhiskervisible = 1;

	m_sortbyattribute = 1;

	CGsta gsta(m_pDoc);
	CArray<long> * apgrps = new CArray<long>;

	if(!m_pDoc->m_kw_patn_generated_groups)
	{
		m_gstastats = &m_pDoc->m_Gstastats;
		m_groups = &m_pDoc->m_Groups;
		m_rowgroups = &m_pDoc->m_Rows;
	}
	else
	{
		m_gstastats = &m_pDoc->m_Gstastats_apriori;
		apgrps->SetSize(m_pDoc->m_RowControlGroups.GetSize());
		for(i=0;i<apgrps->GetSize();i++)
		{
			apgrps->SetAt(i,i);
		}
		m_groups = apgrps;
		m_rowgroups = &m_pDoc->m_RowControlGroups;
	}

	if(m_gstastats->GetSize() && m_gstastats->GetAt(0) != NULL &&
		m_gstastats->GetAt(0)->GetSize()){
		m_max = m_gstastats->GetAt(0)->GetAt(0)->maximum;
		m_min = m_gstastats->GetAt(0)->GetAt(0)->minimum;
	}
	long count = 0;
	for(i=0;i<m_gstastats->GetSize();i++){
		for(j=0;m_gstastats->GetAt(i) != NULL && j<m_gstastats->GetAt(i)->GetSize();j++){
			if(m_max == m_pDoc->GetMissingValue() || 
				(m_max < m_gstastats->GetAt(i)->GetAt(j)->maximum &&
				m_gstastats->GetAt(i)->GetAt(j)->maximum != m_pDoc->GetMissingValue()))
				m_max = m_gstastats->GetAt(i)->GetAt(j)->maximum;
			if(m_min == m_pDoc->GetMissingValue() ||
				(m_min > m_gstastats->GetAt(i)->GetAt(j)->minimum &&
				m_gstastats->GetAt(i)->GetAt(j)->minimum != m_pDoc->GetMissingValue()))
				m_min = m_gstastats->GetAt(i)->GetAt(j)->minimum;
			count++;
		}
	}
	if(m_min == m_pDoc->GetMissingValue())
		m_min = 0;
	if(m_max == m_pDoc->GetMissingValue())
		m_max = 0;

	pos = 0;

	SetScrollRange(SB_VERT,0,count-1);

	m_saveimage = false;
	m_menu.LoadMenu(IDR_BW);

	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;

	if(m_pDoc->m_KWAttributes.GetSize()){
		OnSortbykruskalwallis();
	}else{
		OnSortbytableorder();
	}

	return 0;
}

void CBoxWhiskerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

	long count = 0;long i;
	if(m_sortbyattribute != 2){
		for(i=0;i<m_gstastats->GetSize();i++){
			for(long j=0;m_gstastats->GetAt(i) != NULL && j<m_gstastats->GetAt(i)->GetSize();j++){
				count++;
			}
		}	
	}else{
		count = m_pDoc->m_KW.GetSize()*m_groups->GetSize();
	}

	SetScrollRange(SB_VERT,0,count-1);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);

	int min,max;

	GetScrollRange(SB_VERT,&min,&max);
	
	switch(nSBCode){
		case SB_LINEDOWN: // scrolling forward through document
			if(pos < max) pos++;
			break;
		case SB_LINEUP:	// scrolling backward through document
			if(pos > min) pos--;
			break;
		case SB_PAGEDOWN: // scrolling forward through document by a page
			for(i=0;i<10;i++)
				if(pos < max) pos++;
			break;
		case SB_PAGEUP:	// scrolling backward through document
			for(i=0;i<10;i++)
				if(pos > min) pos--;
			break;
		case SB_THUMBPOSITION:
			pos = (long)nPos;
			break;
		case SB_THUMBTRACK:
			pos = (long)nPos;
			break;
		case SB_ENDSCROLL:
		default:
			break;		
	}

	SetScrollPos(SB_VERT ,pos,TRUE);
	RedrawWindow();
}

void CBoxWhiskerView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//if(m_psd != NULL) delete m_psd;	m_psd = NULL;

	m_pDoc->m_lfCells.lfHeight = (long)(m_pDoc->m_lfCells.lfHeight / (2*m_VScale));

	CView::OnEndPrinting(pDC, pInfo);
}

BOOL CBoxWhiskerView::OnPreparePrinting(CPrintInfo* pInfo) 
{
//	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
//	CPageSetupDialog *d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL){delete d; return false;}
//	m_psd = d;
	
	return DoPreparePrinting(pInfo);

/*	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog * d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
	if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	pInfo->m_pPD->m_pd.Flags |= PD_ALLPAGES;
	pInfo->m_pPD->m_pd.Flags |= PD_HIDEPRINTTOFILE;
	pInfo->m_pPD->m_pd.Flags |= PD_NOSELECTION;
	
	return DoPreparePrinting(pInfo);*/
}

void CBoxWhiskerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	pos = pInfo->m_nCurPage-1;//m_NumPerPage * (pInfo->m_nCurPage-1);
	//pos -= ((long)(pos/(m_pDoc->m_Groups.GetSize()+3))) * 2 + 1;

	//if(pos % m_pDoc->m_Groups.GetSize()+3 == m_pDoc->m_Groups.GetSize())
	//	pos++;
	//if(pos % m_pDoc->m_Groups.GetSize()+3 == m_pDoc->m_Groups.GetSize()+1)
	//	pos++;

	OnDraw(pDC);

	CView::OnPrint(pDC, pInfo);
}

void CBoxWhiskerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
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

	m_pDoc->m_lfCells.lfHeight = (long)(m_pDoc->m_lfCells.lfHeight * 2*m_VScale);
	
	m_NumPerPage = (m_rectDraw.Height() - abs((long)(m_pDoc->m_lfCells.lfHeight* 1.37*2))) / 
		(abs((long)((m_pDoc->m_lfCells.lfHeight)* 1.37)) /*55*/);
	
	long s = m_pDoc->m_KWAttributes.GetSize();
	long p = m_groups->GetSize()+4;

	pInfo->SetMinPage(1);
	//long fix; ((s*p)%m_NumPerPage > 0)?fix=1:fix=0;
	//m_NumPerPage = ceil(m_NumPerPage/(double)p);
	m_NumPerPage -= 4;

	//pInfo->SetMaxPage((s*p)/m_NumPerPage + fix);
	long pages = (long)ceil(s*p/(double)m_NumPerPage);
	pInfo->SetMaxPage(pages);

	
	//CView::OnBeginPrinting(pDC, pInfo);

}

void CBoxWhiskerView::SaveIt()
{
	//m_SaveHeight = m_Rect.bottom;
	//m_SaveWidth = m_Rect.right;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save Box & Whisker Diagram:";
	
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

	long s = m_pDoc->m_KWAttributes.GetSize();
	long p = m_groups->GetSize()+4;
	long height = abs((long)(m_pDoc->m_lfCells.lfHeight * 1.37));
	long width = (long)(height * 3.0/2.0);
	long nWidth = width*20;
	long nHeight = (height*s*p)+height * 4;
		if(nWidth % 2 == 1) nWidth = nWidth + 1;
	if(nHeight %2 == 1) nHeight = nHeight + 1;


	m_rectDraw.left = 0;
	m_rectDraw.right = nWidth+0;
	m_rectDraw.top = 0;
	m_rectDraw.bottom = nHeight;

	pos = 0;

	if(fi == 1)
	{
		DrawToMetaFile(strPathName);
	}
	else
	{
		CDC dcMem;
		CBitmap bmp;

		dcMem.CreateCompatibleDC(GetDC());
		bmp.CreateBitmap(nWidth+0,nHeight,1,1,NULL);
		CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

		dcMem.BitBlt(0,0,nWidth,nHeight,GetDC(),0,0,WHITENESS);

		m_saveimage = true;
		OnDraw(&dcMem);
		m_saveimage = false;

		SaveBitmapFile(dcMem.GetSafeHdc(), (HBITMAP) bmp, strPathName);

		//Destroy all memory device storage
		//wglDeleteContext(rc);
		DeleteObject(bmp);
		DeleteDC(dcMem);
	}

	RedrawWindow();
}

void CBoxWhiskerView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,AfxGetMainWnd());	
}

void CBoxWhiskerView::OnSaveimage() 
{
	SaveIt();
	
}

BOOL CBoxWhiskerView::SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) 
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

void CBoxWhiskerView::OnClose() 
{
	((CFrameWnd*)GetParent())->DestroyWindow();

	
}

BOOL CBoxWhiskerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) return true;
	inMouseWheel = true;

	if(m_mousewheelsensitivity == 0)
		m_mousewheelsensitivity = 40;

	long x,y,diff;
	
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y + x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	for(long i=y;i != diff;(diff>y)?i++:i--){
		if(x < 0)
			OnVScroll(SB_LINEDOWN,0,NULL);
		else
			OnVScroll(SB_LINEUP,0,NULL);

	}
	inMouseWheel = false;
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CBoxWhiskerView::OnFont()
{
 	CFontDialog dlg(&m_pDoc->m_lfCells);
	dlg.m_cf.rgbColors = m_pDoc->m_rgbFontCells;

	if(dlg.DoModal() == IDOK){
		dlg.GetCurrentFont(&m_pDoc->m_lfCells);
		m_pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
		m_pDoc->m_nPointSizeCells = dlg.m_cf.iPointSize;///10; // iPointSize is in 1/10s
	}	

	RedrawWindow();
}

void CBoxWhiskerView::OnSortalphabetically() 
{
	m_sortbyattribute = 0;

	m_menu.CheckMenuItem(ID__SORTALPHABETICALLY,MF_CHECKED);
	m_menu.CheckMenuItem(ID__SORTBYTABLEORDER,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__SORTBYKRUSKALWALLIS,MF_UNCHECKED);	

	g_scrolling = true;
	long pos = GetScrollLimit(SB_VERT);
	for(long i=0;i<pos;i++)
		OnVScroll(SB_LINEUP,0,NULL);
	g_scrolling = false;
	RedrawWindow();

}

void CBoxWhiskerView::OnSortbykruskalwallis() 
{
	m_sortbyattribute = 2;

	m_menu.CheckMenuItem(ID__SORTALPHABETICALLY,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__SORTBYTABLEORDER,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__SORTBYKRUSKALWALLIS,MF_CHECKED);	

	g_scrolling = true;
	long pos = GetScrollLimit(SB_VERT);
	for(long i=0;i<pos;i++)
		OnVScroll(SB_LINEUP,0,NULL);
	g_scrolling = false;
	RedrawWindow();
}

void CBoxWhiskerView::OnSortbytableorder() 
{
	m_sortbyattribute = 1;

	m_menu.CheckMenuItem(ID__SORTALPHABETICALLY,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID__SORTBYTABLEORDER,MF_CHECKED);
	m_menu.CheckMenuItem(ID__SORTBYKRUSKALWALLIS,MF_UNCHECKED);	

	g_scrolling = true;
	long pos = GetScrollLimit(SB_VERT);
	for(long i=0;i<pos;i++)
		OnVScroll(SB_LINEUP,0,NULL);
	g_scrolling = false;
	RedrawWindow();
}

void CBoxWhiskerView::OnUpdateSortbykruskalwallis(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pDoc->m_KW.GetSize());	
}


void CBoxWhiskerView::DrawToMetaFile(CString filename) //if false, must deletemetafile g_emf after use
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

	m_saveimage = true;
	OnDraw(pDC);
	m_saveimage = false;

	//g_emf = CloseEnhMetaFile(met);
	DeleteEnhMetaFile(CloseEnhMetaFile(met));

}