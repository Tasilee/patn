// assoswnd.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "PickAMxDlg.h"
#include "assoswnd.h"
#include "assohist.h"
#include "assomxvw.h"
#include "association.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssocSplitterWnd

IMPLEMENT_DYNCREATE(CAssocSplitterWnd, CMDIChildWnd)

CAssocSplitterWnd::CAssocSplitterWnd()
{
	m_created_successfully = false;
}

CAssocSplitterWnd::~CAssocSplitterWnd()
{
	if(m_created_successfully){
		if(m_IsRow)
			m_pDoc->m_AssocRowVisible = 0;
		else
			m_pDoc->m_AssocColVisible = 0;
	}
}

BOOL CAssocSplitterWnd::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	CPatnDoc* pDoc = (CPatnDoc*)pContext->m_pCurrentDoc;
	ASSERT_VALID(pDoc);

	m_pDoc = pDoc;
	m_pAssocMatrix = NULL;

	m_IsRow = 0;
	if(pDoc->m_LoadingAssocRow && !pDoc->m_LoadingAssocCol){
		m_IsRow = 1;
		pDoc->m_AssocRowVisible = 1;
		m_pAssocMatrix = &pDoc->m_AssocMatrixRows;	
	}else if(pDoc->m_LoadingAssocCol){
		pDoc->m_AssocColVisible = 1;
		m_pAssocMatrix = &pDoc->m_AssocMatrixCols;
	}else {
		if ( (pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocMatrixCols.m_aMatrix.GetSize() == 0 &&
				  pDoc->m_AssocRowVisible == 0) ||
				  (pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocColVisible == 1 &&
				  pDoc->m_AssocRowVisible == 0))
		{
			m_IsRow = 1;
			pDoc->m_AssocRowVisible = 1;
			m_pAssocMatrix = &pDoc->m_AssocMatrixRows;
		}
		else if ( (pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() == 0 &&
				  pDoc->m_AssocMatrixCols.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocColVisible == 0) ||
				  (pDoc->m_AssocMatrixCols.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocRowVisible == 1 &&
				  pDoc->m_AssocColVisible == 0))
		{
			pDoc->m_AssocColVisible = 1;
			m_pAssocMatrix = &pDoc->m_AssocMatrixCols;
		}
		else if ( pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocMatrixCols.m_aMatrix.GetSize() > 0 &&
				  pDoc->m_AssocRowVisible == 0 &&
				  pDoc->m_AssocColVisible == 0)
		{
			CPickAssocMatrixDlg dlg;
			if ( dlg.DoModal() == IDOK )
			{
				switch (dlg.m_nDataType)
				{
					case CPickAssocMatrixDlg::ROWS:
						m_IsRow = 1;
						pDoc->m_AssocRowVisible = 1;
						m_pAssocMatrix = &pDoc->m_AssocMatrixRows;
						break;

					case CPickAssocMatrixDlg::COLUMNS:
						pDoc->m_AssocColVisible = 1;
						m_pAssocMatrix = &pDoc->m_AssocMatrixCols;
						break;
				}
			}
		}
	}
	if ( m_pAssocMatrix == NULL )
		return FALSE;

	// create a splitter with 2 rows, 1 columns
	if (!m_wndSplitter.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	// add the first splitter pane - the default view in column 0
	int mx = 300;
	int my = 50;
	CRect tr;
	this->GetWindowRect(&tr);
	my = tr.Height() / 2;
	
	if (!m_wndSplitter.CreateView(1, 0,
		pContext->m_pNewViewClass, CSize(mx,my), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}


	// add the second splitter pane - a histogram view in column 1
	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CAssocHistogram), CSize(0, my), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	// activate the histogram view
	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));

	m_created_successfully = true;
	return TRUE;
}


BEGIN_MESSAGE_MAP(CAssocSplitterWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CAssocSplitterWnd)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID__CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CAssocSplitterWnd::OnFileExport() 
{
	CAssociation asso(m_pDoc);
	if(m_pAssocMatrix == &m_pDoc->m_AssocMatrixCols)
		asso.WriteAssocMatrixToASCII(CPatnDoc::columns);
	else
		asso.WriteAssocMatrixToASCII(CPatnDoc::rows);
}

void CAssocSplitterWnd::OnClose() 
{
	DestroyWindow();
	
}

BOOL CAssocSplitterWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;


	cs.lpszClass = AfxRegisterWndClass(0,
		::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_ASSOC_ICON));

	return CMDIChildWnd::PreCreateWindow(cs);
}

CDocument* CAssocSplitterWnd::GetActiveDocument() 
{
	CPatnDoc * pDoc = (CPatnDoc*) CMDIChildWnd::GetActiveDocument();

	{
		CString str;
		str = pDoc->GetTitle();
		if(m_IsRow)
			str.Insert(str.GetLength()," : Row Association Matrix");
		else
			str.Insert(str.GetLength()," : Column Association Matrix");
		SetWindowText(str);

	
	}
		
	return CMDIChildWnd::GetActiveDocument();
}
