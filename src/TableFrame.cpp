// TableFrame.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "TableFrame.h"
#include "TableView.h"
#include "comparerow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableFrame

IMPLEMENT_DYNCREATE(CTableFrame, CMDIChildWnd)

CTableFrame::CTableFrame()
{
	m_bSplittersCreated = FALSE;
}

CTableFrame::~CTableFrame()
{
}


BEGIN_MESSAGE_MAP(CTableFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CTableFrame)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableFrame message handlers

BOOL CTableFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// create a splitter with 2 rows, 1 columns
	if (!m_wndSplitter1.CreateStatic(this, 2, 1,
		WS_CHILD | WS_VISIBLE /* | WS_HSCROLL */))
	{
		TRACE0("Failed to create static splitter\n");
		return FALSE;
	}

	//if(lpcs->cy*2/3 < 15)
	//	m_wndSplitter1.SetRowInfo(1,15,15);
	//else
		m_wndSplitter1.SetRowInfo(0,lpcs->cy*2/3,15);

	// add the second splitter pane - which is a nested splitter with 2 columns
	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter1,     // our parent window is the first splitter
		1, 2,               // the new splitter is 2 columns, 1 row
		WS_CHILD | WS_VISIBLE | WS_BORDER /* | WS_VSCROLL */,  // style, WS_BORDER is needed
		m_wndSplitter1.IdFromRowCol(0, 0)
			// new splitter is in the first row, first column of first splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

//	if(lpcs->cy*2/3 < 15)
//		m_wndSplitter2.SetColumnInfo(1,15,15);
//	else
//		m_wndSplitter2.SetColumnInfo(1,lpcs->cy*2/3,15);

	m_bSplittersCreated = TRUE;

	// now create the two views inside the nested splitter
	long cx = lpcs->cx*2/3;    // width of table pane
	long cy = lpcs->cy*2/3;    // height of table pane

	if (!m_wndSplitter2.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(cx,cy), pContext))
	{
		TRACE0("Failed to create table pane\n");
		return FALSE;
	}
	if (!m_wndSplitter2.CreateView(0, 1,
		RUNTIME_CLASS(CRowTotalsView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create row totals pane\n");
		return FALSE;
	}

	// create the column totals view inside the second row of the first splitter
	if (!m_wndSplitter1.CreateView(1, 0,
		RUNTIME_CLASS(CColumnTotalsView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create column totals pane\n");
		return FALSE;
	}
	
	// set pointers to the views so that they can communicate with each other
	CTableView *pTableView = (CTableView*)(m_wndSplitter2.GetPane(0,0));
	CRowTotalsView *pRowTotalsView = (CRowTotalsView*)(m_wndSplitter2.GetPane(0,1));
	CColumnTotalsView *pColumnTotalsView = (CColumnTotalsView*)(m_wndSplitter1.GetPane(1,0));
	if ( !pTableView->SetPointerToRowTotalsView(pRowTotalsView) ||
		 !pTableView->SetPointerToColumnTotalsView(pColumnTotalsView))
	{
		TRACE0("Failed to set pointers to totals views\n");
		return FALSE;
	}
	if ( !pRowTotalsView->SetPointerToTableView(pTableView) ||
		 !pColumnTotalsView->SetPointerToTableView(pTableView))
	{
		TRACE0("Failed to set pointers to table view\n");
		return FALSE;
	}


	return TRUE;
}

void CTableFrame::OnSize(UINT nType, int cx, int cy) 
{
	int cyCur, cyMin;
	int cyNew;
	int cxCur, cxMin;
	int cxNew;

	if ( m_bSplittersCreated )
	{
		m_wndSplitter1.GetRowInfo(0,cyCur,cyMin);
		m_wndSplitter2.GetColumnInfo(0,cxCur,cxMin);
		CMDIChildWnd::OnSize(nType, cx, cy);
		cyMin = 15;
		cxMin = 15;

		cyNew = cy - (m_cy - cyCur); if ( cyNew < cyMin ) cyNew = cyMin;
		cxNew = cx - (m_cx - cxCur); if ( cxNew < cxMin ) cxNew = cxMin;
		m_wndSplitter1.SetRowInfo(0,cyNew,cyMin);
		m_wndSplitter2.SetColumnInfo(0,cxNew,cxMin);
		m_wndSplitter1.RecalcLayout();
		m_wndSplitter2.RecalcLayout();



	}
	else
		CMDIChildWnd::OnSize(nType, cx, cy);

	m_cx = cx;
	m_cy = cy;
}

BOOL CTableFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CTableFrame::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();
	// TODO: Add your specialized code here and/or call the base class

	CPatnDoc * pDoc = (CPatnDoc*) d;
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Data Table");
		SetWindowText(str);
	}

	return d;
}

BOOL CTableFrame::DestroyWindow() 
{
	CPatnDoc * pDoc = (CPatnDoc*)CMDIChildWnd::GetActiveDocument();
		
	bool closed = false;

	while(pDoc != NULL){
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL){
			CView* pView = pDoc->GetNextView(pos);   
			
			if(!pView->IsKindOf(RUNTIME_CLASS(CTableView)) &&
				!pView->IsKindOf(RUNTIME_CLASS(CRowTotalsView)) &&
				!pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)) &&
				!pView->IsKindOf(RUNTIME_CLASS(CTableFrame))){
				pView->GetParentFrame()->SendMessage(WM_CLOSE);
				closed = true;
				break;
			}
		} 
		if(!closed) 
			break;	
		closed = false;
	}	
	return CMDIChildWnd::DestroyWindow();
}
