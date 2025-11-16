// fuseswnd.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "PickFuseDlg.h"
#include "fuseswnd.h"
#include "fuseview.h"
#include "dendview.h"
#include "tableview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFuseSplitterWnd

IMPLEMENT_DYNCREATE(CFuseSplitterWnd, CMDIChildWnd)

CFuseSplitterWnd::CFuseSplitterWnd()
{
}

CFuseSplitterWnd::~CFuseSplitterWnd()
{

		
}

BOOL CFuseSplitterWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CPatnDoc * pDoc = (CPatnDoc*) pContext->m_pCurrentDoc;
	m_pDoc = pDoc;

	m_pFuseTable = NULL;

	m_IsRow = 0;

	if(m_pDoc->m_LoadingFuseRow && !m_pDoc->m_LoadingFuseCol){
		pDoc->m_FuseRowVisible = 1;
		m_IsRow = 1;
		m_pFuseTable = &pDoc->m_FuseTableRows;
		m_pDendTable = &pDoc->m_DendTableRows;
		m_pDendOrder = &pDoc->m_DendOrderRows;
		m_pGroupDendTable = &pDoc->m_GroupDendTableRows;
		m_pGroupDendOrder = &pDoc->m_GroupDendOrderRows;
		m_pCutOff = &pDoc->m_nCutOffRows;  // TODO: temp code for dendrogram line
	}else if(m_pDoc->m_LoadingFuseCol){
		pDoc->m_FuseColVisible = 1;
		m_pFuseTable = &pDoc->m_FuseTableCols;
		m_pDendTable = &pDoc->m_DendTableCols;
		m_pDendOrder = &pDoc->m_DendOrderCols;
		m_pGroupDendTable = &pDoc->m_GroupDendTableCols;
		m_pGroupDendOrder = &pDoc->m_GroupDendOrderCols;
		m_pCutOff = &pDoc->m_nCutOffCols;  // TODO: temp code for dendrogram line
	}else{

		if ( (pDoc->m_FuseTableRows.GetSize() > 0 &&
				  pDoc->m_FuseTableCols.GetSize() == 0 &&
				  pDoc->m_FuseRowVisible == 0) ||
				  (pDoc->m_FuseTableRows.GetSize() > 0 &&
				  pDoc->m_FuseColVisible == 1 &&
				  pDoc->m_FuseRowVisible == 0))
		{
			pDoc->m_FuseRowVisible = 1;
			m_IsRow = 1;
			m_pFuseTable = &pDoc->m_FuseTableRows;
			m_pDendTable = &pDoc->m_DendTableRows;
			m_pDendOrder = &pDoc->m_DendOrderRows;
			m_pGroupDendTable = &pDoc->m_GroupDendTableRows;
			m_pGroupDendOrder = &pDoc->m_GroupDendOrderRows;
			m_pCutOff = &pDoc->m_nCutOffRows;  // TODO: temp code for dendrogram line
		}
		else if ( (pDoc->m_FuseTableRows.GetSize() == 0 &&
				  pDoc->m_FuseTableCols.GetSize() > 0 &&
				  pDoc->m_FuseColVisible == 0) ||
				  (pDoc->m_FuseTableCols.GetSize() > 0 &&
				  pDoc->m_FuseRowVisible == 1 &&
				  pDoc->m_FuseColVisible == 0))
		{
			pDoc->m_FuseColVisible = 1;
			m_pFuseTable = &pDoc->m_FuseTableCols;
			m_pDendTable = &pDoc->m_DendTableCols;
			m_pDendOrder = &pDoc->m_DendOrderCols;
			m_pGroupDendTable = &pDoc->m_GroupDendTableCols;
			m_pGroupDendOrder = &pDoc->m_GroupDendOrderCols;
			m_pCutOff = &pDoc->m_nCutOffCols;  // TODO: temp code for dendrogram line
		}
		else if ( pDoc->m_FuseTableRows.GetSize() > 0 &&
				  pDoc->m_FuseTableCols.GetSize() > 0 &&
				  (pDoc->m_FuseRowVisible == 0 ||
				  pDoc->m_FuseColVisible == 0))
		{
			CPickFuseTableDlg dlg;
			if ( dlg.DoModal() == IDOK )
			{
				switch (dlg.m_nDataType)
				{
					case CPickFuseTableDlg::ROWS:
						pDoc->m_FuseRowVisible = 1;
						m_IsRow = 1;
						m_pFuseTable = &pDoc->m_FuseTableRows;
						m_pDendTable = &pDoc->m_DendTableRows;
						m_pDendOrder = &pDoc->m_DendOrderRows;
						m_pGroupDendTable = &pDoc->m_GroupDendTableRows;
						m_pGroupDendOrder = &pDoc->m_GroupDendOrderRows;
						m_pCutOff = &pDoc->m_nCutOffRows;  // TODO: temp code for dendrogram line
						break;

					case CPickFuseTableDlg::COLUMNS:
						pDoc->m_FuseColVisible = 1;
						m_pFuseTable = &pDoc->m_FuseTableCols;
						m_pDendTable = &pDoc->m_DendTableCols;
						m_pDendOrder = &pDoc->m_DendOrderCols;
						m_pGroupDendTable = &pDoc->m_GroupDendTableCols;
						m_pGroupDendOrder = &pDoc->m_GroupDendOrderCols;
						m_pCutOff = &pDoc->m_nCutOffCols;  // TODO: temp code for dendrogram line
						break;
				}
			}
		}
	}

	if ( m_pFuseTable == NULL )
		return FALSE;

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	CRect rc;
	this->GetWindowRect(&rc);

	// add the first splitter pane - the default view in column 0
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(rc.Width()/2, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	// add the second splitter pane - a dendrogram view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CDendView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	// activate the dendrogram view
	SetActiveView((CView*)m_wndSplitter.GetPane(0,1));

	return TRUE;
}


BEGIN_MESSAGE_MAP(CFuseSplitterWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CFuseSplitterWnd)
	ON_COMMAND(ID__CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFuseSplitterWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}

BOOL CFuseSplitterWnd::DestroyWindow() 
{
	if(m_IsRow == 1)
		m_pDoc->m_FuseRowVisible = 0;
	else
		m_pDoc->m_FuseColVisible = 0;
	
	return CMDIChildWnd::DestroyWindow();
}


void CFuseSplitterWnd::OnClose() 
{
	DestroyWindow();
	
}

BOOL CFuseSplitterWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;

		cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_DENDOGRAM_ICON));
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CFuseSplitterWnd::GetActiveDocument() 
{
	CDocument *d = CMDIChildWnd::GetActiveDocument();
	CPatnDoc * pDoc = (CPatnDoc*) d;


	{
		CString str;
		str = pDoc->GetTitle();
		if(m_IsRow)
			str.Insert(str.GetLength()," : Row Fusion");
		else
			str.Insert(str.GetLength()," : Column Fusion");
		SetWindowText(str);
	}

	return d;
}
