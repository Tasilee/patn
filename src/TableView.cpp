// TableView.cpp : implementation of the CTableView class
//

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "MainFrame.h"
#include "CntrItem.h"
#include "comparerow.h"
#include "TableView.h"
#include "SelectColumnsOrRowsDlg.h"
#include "Analysis.h"
#include "Association.h"                     
#include "Classification.h"
#include "PageColumnOptions.h"
#include "PageRowOptions.h"
#include "Ordination.h"
#include "groupsdlg.h"
#include "colr.h"
#include "fuse.h"
#include "DataTransformDlg.h"
#include "tableproperties.h"
#include "pcc.h"
//#include "3dview.h"
#include "3dxview.h"
//#include "scat.h"
#include "mst.h"
#include "gsta.h"
#include "kruskalwalice.h"
#include "evaluation.h"
//#include "copyselectionsize.h"

#include "seloptions.h"
#include "gsta.h"
#include "autoevaluation.h"
#include "tableframe.h"

#include "propertiesgeneral.h"
#include "propertiesordinationview.h"
#include "propertiesrandom.h"
#include "propertiesvisiblestatistics.h"
#include ".\tableview.h"

#include "ordinationlegend.h"

#include "kruskalwalice.h"
#include "kwthreashold.h"
#include "constancyfidelity.h"

#include "SelectionName.h"
#include "assigngroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// class CUpdateHint

IMPLEMENT_DYNAMIC( CUpdateHint, CObject )

CUpdateHint::CUpdateHint()
{
	m_idxOld = -1;
	m_idxNew = -1;
	m_nCount = 0;
	m_pSelection = NULL;
	m_pItems = NULL;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCell

CCell::CCell()
{
	m_idxRow = -1;
	m_idxColumn = -1;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTableView

IMPLEMENT_DYNCREATE(CTableView, CView)

BEGIN_MESSAGE_MAP(CTableView, CView)
	//{{AFX_MSG_MAP(CTableView)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TABLE_COLUMN_ADD, OnTableColumnAdd)
	ON_COMMAND(ID_TABLE_COLUMN_DELETE, OnTableColumnDelete)
	ON_COMMAND(ID_TABLE_COLUMN_INSERT, OnTableColumnInsert)
	ON_COMMAND(ID_TABLE_COLUMN_MAKEEXTRINSIC, OnTableColumnMakeExtrinsic)
	ON_COMMAND(ID_TABLE_COLUMN_MAKEINTRINSIC, OnTableColumnMakeIntrinsic)
	ON_COMMAND(ID_TABLE_COLUMN_PROPERTIES, OnTableColumnProperties)
	ON_COMMAND(ID_TABLE_ROW_ADD, OnTableRowAdd)
	ON_COMMAND(ID_TABLE_ROW_DELETE, OnTableRowDelete)
	ON_COMMAND(ID_TABLE_ROW_INSERT, OnTableRowInsert)
	ON_COMMAND(ID_TABLE_ROW_MAKEEXTRINSIC, OnTableRowMakeExtrinsic)
	ON_COMMAND(ID_TABLE_ROW_MAKEINTRINSIC, OnTableRowMakeIntrinsic)
	ON_COMMAND(ID_TABLE_ROW_PROPERTIES, OnTableRowProperties)
	ON_COMMAND(ID_TABLE_FONT, OnTableFont)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_COMMAND(ID_TABLE_COLUMN_SELECT, OnTableColumnSelect)
	ON_COMMAND(ID_TABLE_ROW_SELECT, OnTableRowSelect)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_COMMAND(ID_TABLE_COLUMN_HIDE, OnTableColumnHide)
	ON_COMMAND(ID_TABLE_COLUMN_GOTOEXTRINSICS, OnTableColumnGotoExtrinsics)
	ON_COMMAND(ID_TABLE_COLUMN_UNHIDE, OnTableColumnUnhide)
	ON_COMMAND(ID_TABLE_ROW_GOTOEXTRINSICS, OnTableRowGotoExtrinsics)
	ON_COMMAND(ID_TABLE_ROW_HIDE, OnTableRowHide)
	ON_COMMAND(ID_TABLE_ROW_UNHIDE, OnTableRowUnhide)
	ON_COMMAND(ID_TOOLS_DATATRANSFORMATION, OnToolsDataTransformation)
	ON_COMMAND(ID_TOOLS_DATAANALYSIS, OnToolsDataAnalysis)
	ON_COMMAND(ID_TABLE_COLUMN_GROUPS, OnTableColumnGroups)
	ON_COMMAND(ID_TABLE_COLUMN_FIND, OnTableColumnFind)
	ON_COMMAND(ID_TABLE_ROW_FIND, OnTableRowFind)
	ON_COMMAND(ID_TABLE_ROW_GROUPS, OnTableRowGroups)
	ON_UPDATE_COMMAND_UI(ID_TABLE_COLUMN_MAKEEXTRINSIC, OnUpdateTableColumnMakeExtrinsic)
	ON_UPDATE_COMMAND_UI(ID_TABLE_COLUMN_MAKEINTRINSIC, OnUpdateTableColumnMakeIntrinsic)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ROW_MAKEEXTRINSIC, OnUpdateTableRowMakeExtrinsic)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ROW_MAKEINTRINSIC, OnUpdateTableRowMakeIntrinsic)
	ON_UPDATE_COMMAND_UI(ID_TABLE_COLUMN_GOTOEXTRINSICS, OnUpdateTableColumnGotoExtrinsics)
	ON_UPDATE_COMMAND_UI(ID_TABLE_ROW_GOTOEXTRINSICS, OnUpdateTableRowGotoExtrinsics)
	ON_COMMAND(ID_FILE_APPEND, OnFileAppend)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_TOOLS_DATAEVALUATION, OnToolsDataevaluation)
	ON_COMMAND(ID_TABLE_TANSPOSEDATALABELS, OnTableTansposedatalabels)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_OTHERLABELSINCOPYCUT, OnEditOtherlabelsincopycut)
	ON_COMMAND(ID_TABLE_SELECTIONOPTIONS, OnTableSelectionoptions)
	ON_COMMAND(ID_DATA_DELETEDUPLICATEROWS, OnDeleteDuplicates)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RESIZE_EDIT, OnResizeEdit)  // sent by CNumEdit control when need to resize
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
//	ON_WM_HELPINFO()
ON_COMMAND(ID_TEST, ontest)
ON_COMMAND(ID_DATA_SWAPCOLUMNINTRINSICSWITHEXTRINSICS, OnDataSwapcolumnintrinsicswithextrinsics)
//ON_UPDATE_COMMAND_UI(ID_DATA_SWAPCOLUMNINTRINSICSWITHEXTRINSICS, OnUpdateDataSwapcolumnintrinsicswithextrinsics)
ON_COMMAND(ID_DATA_SWAPROWINTRINSICSWITHEXTRINSICS, OnDataSwaprowintrinsicswithextrinsics)
//ON_UPDATE_COMMAND_UI(ID_DATA_SWAPROWINTRINSICSWITHEXTRINSICS, OnUpdateDataSwaprowintrinsicswithextrinsics)
ON_COMMAND(ID_DATA_VARIABLES_KW_TO_EXTRINSICS, OnDataVariablesKwToExtrinsics)
ON_UPDATE_COMMAND_UI(ID_DATA_VARIABLES_KW_TO_EXTRINSICS, OnUpdateDataVariablesKwToExtrinsics)
ON_COMMAND(ID_DATA_CONSTANCYANDFIDELITY, OnDataConstancyandfidelity)
ON_UPDATE_COMMAND_UI(ID_DATA_CONSTANCYANDFIDELITY, OnUpdateDataConstancyandfidelity)
ON_COMMAND(ID_SELECTIONS_CLEAR, OnSelectionsClear)
ON_UPDATE_COMMAND_UI(ID_SELECTIONS_CLEAR, OnUpdateSelectionsClear)
ON_COMMAND(ID_SELECTIONS_SAVE, OnSelectionsSave)
ON_UPDATE_COMMAND_UI(ID_SELECTIONS_SAVE, OnUpdateSelectionsSave)
ON_COMMAND(ID_SELECTION1_ACTIVATE, OnSelection1Activate)
ON_COMMAND(ID_SELECTION2_ACTIVATE, OnSelection2Activate)
ON_COMMAND(ID_SELECTION3_ACTIVATE, OnSelection3Activate)
ON_COMMAND(ID_SELECTION4_ACTIVATE, OnSelection4Activate)
ON_COMMAND(ID_SELECTION5_ACTIVATE, OnSelection5Activate)
ON_COMMAND(ID_SELECTION6_ACTIVATE, OnSelection6Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION1_ACTIVATE, OnUpdateSelection1Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION2_ACTIVATE, OnUpdateSelection2Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION3_ACTIVATE, OnUpdateSelection3Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION4_ACTIVATE, OnUpdateSelection4Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION5_ACTIVATE, OnUpdateSelection5Activate)
ON_UPDATE_COMMAND_UI(ID_SELECTION6_ACTIVATE, OnUpdateSelection6Activate)
ON_COMMAND(ID_SELECTION1_SAVECURRENTSELECTION, OnSelection1SaveCurrentSelection)
ON_COMMAND(ID_SELECTION2_SAVECURRENTSELECTION, OnSelection2SaveCurrentSelection)
ON_COMMAND(ID_SELECTION3_SAVECURRENTSELECTION, OnSelection3SaveCurrentSelection)
ON_COMMAND(ID_SELECTION4_SAVECURRENTSELECTION, OnSelection4SaveCurrentSelection)
ON_COMMAND(ID_SELECTION5_SAVECURRENTSELECTION, OnSelection5SaveCurrentSelection)
ON_COMMAND(ID_SELECTION6_SAVECURRENTSELECTION, OnSelection6SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION1_SAVECURRENTSELECTION, OnUpdateSelection1SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION2_SAVECURRENTSELECTION, OnUpdateSelection2SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION3_SAVECURRENTSELECTION, OnUpdateSelection3SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION4_SAVECURRENTSELECTION, OnUpdateSelection4SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION5_SAVECURRENTSELECTION, OnUpdateSelection5SaveCurrentSelection)
ON_UPDATE_COMMAND_UI(ID_SELECTION6_SAVECURRENTSELECTION, OnUpdateSelection6SaveCurrentSelection)
ON_COMMAND(ID_DATA_ASSIGNGROUPSTONON, OnDataAssigngroupstonon)
ON_UPDATE_COMMAND_UI(ID_DATA_ASSIGNGROUPSTONON, OnUpdateDataAssigngroupstonon)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableView construction/destruction

CTableView::CTableView()
{
	m_pSelection = NULL;
	m_pSelectedColumns = new CColumnSelection(this);
	m_pSelectedRows = new CRowSelection(this);

	m_nRowBtnBarWidth = 0;
	autosizecolumns = false;

	for(int i=0;i<6;i++) {
		m_aSavedSelections.Add(NULL);
	}
}

CTableView::~CTableView()
{
	delete m_pSelectedColumns;
	delete m_pSelectedRows;
	for(long i=0;i < m_aColumns.GetSize();i++)
		delete m_aColumns[i];
	for(i=0;i < m_aRows.GetSize();i++)
		delete m_aRows[i];
	//windows GDI object destruction
	if ( m_fontCells.GetSafeHandle() != NULL ) // currently attached to Windows GDI object
		m_fontCells.DeleteObject();
	m_brushXBar.DeleteObject();
	if(m_fontPrinter.GetSafeHandle() != NULL)
		m_fontPrinter.DeleteObject();
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	for(i=0;i<6;i++) {
		delete m_aSavedSelections.GetAt(i);
	
	}
}

BOOL CTableView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	WNDCLASS wc;
	// register new class if class not already reg'd
	if ( !GetClassInfo(AfxGetInstanceHandle(), "PatnTable", &wc) )
	{
		wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = AfxWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = AfxGetInstanceHandle();
		wc.hIcon = NULL;			
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "PatnTable";
		if ( !AfxRegisterClass(&wc) )
			return FALSE;
	}

	cs.lpszClass = "PatnTable";
	cs.style |= WS_HSCROLL | WS_VSCROLL;	// Add vertical & horizontal scroll bars

	return CView::PreCreateWindow(cs);
}

BOOL CTableView::SetPointerToColumnTotalsView(CColumnTotalsView *pColumnTotalsView)
{
	ASSERT(pColumnTotalsView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)));
	m_pColumnTotalsView = pColumnTotalsView;
	return TRUE;
}

BOOL CTableView::SetPointerToRowTotalsView(CRowTotalsView *pRowTotalsView)
{
	ASSERT(pRowTotalsView->IsKindOf(RUNTIME_CLASS(CRowTotalsView)));
	m_pRowTotalsView = pRowTotalsView;
	return TRUE;
}

void CTableView::OnDraw(CDC* pDC)
{
	SetTableFont();
	if(autosizecolumns)
	{
		AutoSizeColumns();
		autosizecolumns = false;
		AutoSizeRows(pDC);
	}	

			

	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	CString statbartext;
	statbartext.Format("number of objects %d, variables %d",GetDocument()->GetNoOfRows(),GetDocument()->GetNoOfColumns());
	frm->m_wndStatusBar.SetPaneText(1,statbartext);
/*
	CRect rr;
	GetClientRect(rr);
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

	rr.left = rr.left * iMMPerPelX;
	rr.top = rr.top * iMMPerPelY;
	rr.right = rr.right * iMMPerPelX;
	rr.bottom = rr.bottom * iMMPerPelY;

	HDC met;
	met = CreateEnhMetaFile(pDC->GetSafeHdc(),"c:\\out.emf",&rr,NULL);
	if(met != NULL)
		pDC = CDC::FromHandle(met);*/

	CPatnDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	// TODO: add draw code for native data here
	// TODO: also draw all OLE items in the document

	// Draw the selection at an arbitrary position.  This code should be
	//  removed once your real drawing code is implemented.  This position
	//  corresponds exactly to the rectangle returned by CPatnCntrItem,
	//  to give the effect of in-place editing.

	// TODO: remove this code when final draw code is complete.
	pDoc->m_ColSelection = m_pSelectedColumns;
	pDoc->m_RowSelection = m_pSelectedRows;

	if (m_pSelection == NULL)
	{
		POSITION pos = pDoc->GetStartPosition();
		m_pSelection = (CPatnCntrItem*)pDoc->GetNextClientItem(pos);
	}
	if (m_pSelection != NULL)
		m_pSelection->Draw(pDC, CRect(10, 10, 210, 210));

	// native data
	long		i;
	CString	strValue;
	CRect	rText;
	CRect	r, rClip;
	long		idxCol,idxRow;
	long		nColLimit;
	long		nRowLimit;
	CBrush*	pOldBrush;
	CFont*	pOldFont;

	CalcLayout();

	// get invalidated rectangle
	pDC->GetClipBox(rClip);

	pDC->SetBkColor(RGB(192,192,192));
	pDC->SetTextColor(pDoc->m_rgbFontCells);

	// select font into memory DC
	if ( (pOldFont = pDC->SelectObject(&m_fontCells)) == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for table");
	}

	if ( m_nColumnBtnBarHeight >= rClip.top )
		DrawColumnBtnBar(pDC);

	if ( m_nRowBtnBarWidth >= rClip.left )
		DrawRowBtnBar(pDC);
	
	// draw shadows at right & bottom edges of grid
	pOldBrush = (CBrush*)pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(m_xGrid,0,1,m_yGrid + 1,PATCOPY);
	pDC->PatBlt(0,m_yGrid,m_xGrid + 1,1,PATCOPY);

	// extrinsic column bar
	if ( pDoc->IsExtrinsicColumns() &&
		(i = GetColumn(pDoc->GetIdxLeftExtrinsicColumn())) != -1 )
	{
		pDC->SelectObject(&m_brushXBar);
		pDC->PatBlt(m_aColumns[i]->m_xLeft,0,m_aColumns[i]->Width(),m_yGrid,PATCOPY);
		pDC->SelectStockObject(GRAY_BRUSH);
		pDC->PatBlt(m_aColumns[i]->m_xLeft,0,1,m_yGrid,PATCOPY);
		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->PatBlt(m_aColumns[i]->m_xRight,0,1,m_yGrid,PATCOPY);
	}

	// extrinsic row bar
	if ( pDoc->IsExtrinsicRows() &&
		(i = GetRow(pDoc->GetIdxTopExtrinsicRow())) != -1 )
	{
		pDC->SelectObject(&m_brushXBar);
		pDC->PatBlt(0,m_aRows[i]->m_yTop,m_xGrid,m_aRows[i]->Height(),PATCOPY);

		// prevent drawing shadow & highlight lines across extrinsic column bar
		if ( m_rExtrinsicColumnBar.IsRectEmpty() ) // no extrinsic column bar
		{
			pDC->SelectStockObject(GRAY_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yTop,m_xGrid,1,PATCOPY);
			pDC->SelectStockObject(WHITE_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yBottom,m_xGrid,1,PATCOPY);
		}
		else
		{
			pDC->SelectStockObject(GRAY_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yTop,m_rExtrinsicColumnBar.left + 1,1,PATCOPY);
			pDC->PatBlt(m_rExtrinsicColumnBar.right - 1,m_aRows[i]->m_yTop,
				m_xGrid - (m_rExtrinsicColumnBar.right - 1),1,PATCOPY);
			pDC->SelectStockObject(WHITE_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yBottom,m_rExtrinsicColumnBar.left + 1,1,PATCOPY);
			pDC->PatBlt(m_rExtrinsicColumnBar.right - 1,m_aRows[i]->m_yBottom,
				m_xGrid - (m_rExtrinsicColumnBar.right - 1),1,PATCOPY);
		}
	}

	// cell contents
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

	nColLimit = m_aColumns.GetSize();
	nRowLimit = m_aRows.GetSize();

	for ( idxRow = 0; idxRow < nRowLimit; idxRow++ )
	{
		if ( m_aRows[idxRow]->m_bIsHidden || m_aRows[idxRow]->m_bIsExtrinsicBar )
			continue;

		rText.top = m_aRows[idxRow]->m_yTop + g_ctm;
		rText.bottom = m_aRows[idxRow]->m_yBottom - g_cbm;
		for ( idxCol = 0; idxCol < nColLimit; idxCol++ )
		{
			if ( m_aColumns[idxCol]->m_bIsHidden || m_aColumns[idxCol]->m_bIsExtrinsicBar )
				continue;

			rText.left = m_aColumns[idxCol]->m_xLeft + g_clm;
			rText.right = m_aColumns[idxCol]->m_xRight - g_crm;
			if ( ! r.IntersectRect(rText,rClip) )
				continue;

			if ( rText.Width() < m_nWidthUnit ) // can't fit even 1 char in width, so skip
				continue;

			pDoc->GetValueAt(m_aColumns[idxCol]->m_idx,
							 m_aRows[idxRow]->m_idx,strValue);

			if ( strValue.IsEmpty() ) // missing value
				strValue = "..";
			else if ( (pDC->GetTextExtent(strValue,strValue.GetLength())).cx > rText.Width() )
			{
				CString strTemp = '#';
				CString strOut;
				while ( (pDC->GetTextExtent(strTemp,strTemp.GetLength())).cx <= rText.Width() )
				{
					strOut = strTemp;
					strTemp += '#';
				}
				strValue = strOut;
			}

			pDC->ExtTextOut(rText.right,rText.bottom,
							ETO_CLIPPED | ETO_OPAQUE,
							rText,strValue,strValue.GetLength(),NULL);
		}
	}

    // currently selected cell
	DrawFrame(pDC);

	// highlight selected rows/columns
	if ( !m_pSelectedColumns->IsEmpty() )
	{
		for ( i = 0; i < nColLimit; i++ )
		{
			if ( m_pSelectedColumns->IsSelected(m_aColumns[i]->m_idx) )
			{
				//to highlight properly
				if ( i < m_aColumns.GetSize() &&
					m_aColumns[i]->m_idx == pDoc->GetIdxLeftExtrinsicColumn() ) 
					i++;
				HighlightColumn(i, pDC);
			}
		}
	}
	else if ( !m_pSelectedRows->IsEmpty() )
	{
		for ( i = 0; i < nRowLimit; i++ )
		{
			if ( m_pSelectedRows->IsSelected(m_aRows[i]->m_idx) )
			{
				//to highlight properly
				if (i < m_aRows.GetSize() &&
					m_aRows[i]->m_idx == pDoc->GetIdxTopExtrinsicRow() ) 
					i++;
				HighlightRow(i, pDC);
			}
		}
	}
	IN_OnDraw = FALSE;

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);
	m_pColumnTotalsView->Refresh();
	//if(met != NULL)
//		((CDC*)met)->TextOut(10,10,"hello");
	//CloseEnhMetaFile(met);
}

void CTableView::DrawColumnBtnBar(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CBrush*	pOldBrush;
	long		x;
	long		i;
	long		nCount = m_aColumns.GetSize();
	CRect	rText;
	CString	strLabel;
	CString strEllipsis = "...";
	long		cxEllipsis = (pDC->GetTextExtent(strEllipsis)).cx;

	// buttons (highlights and shadows)
	pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,m_xGrid,1,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(0,m_nColumnBtnBarHeight,m_xGrid,1,PATCOPY);

	for ( i = 0; i < nCount; i++ )
	{
		if ( m_aColumns[i]->m_bIsHidden )
		{
			pDC->SelectStockObject(BLACK_BRUSH);
			pDC->PatBlt(m_aColumns[i]->m_xLeft,0,1,m_nColumnBtnBarHeight,PATCOPY);
		}
		else
		{
			pDC->SelectStockObject(WHITE_BRUSH);
			pDC->PatBlt(m_aColumns[i]->m_xLeft,0,1,m_nColumnBtnBarHeight,PATCOPY);
			pDC->SelectStockObject(GRAY_BRUSH);
			pDC->PatBlt(m_aColumns[i]->m_xRight,0,1,m_nColumnBtnBarHeight,PATCOPY);
		}
	}

	// button labels
	rText.top = g_ctm;
	rText.bottom = m_nColumnBtnBarHeight - g_cbm;
    for ( i = 0; i < nCount; i++ )
    {
		if ( m_aColumns[i]->m_bIsHidden )
			continue;

		rText.left = m_aColumns[i]->m_xLeft + g_clm;
		rText.right = m_aColumns[i]->m_xRight - g_crm;
    	if ( m_tm.tmAveCharWidth <= rText.Width() ) // if at least one char can be printed
    	{
			pDoc->GetColumnLabel(m_aColumns[i]->m_idx, strLabel);

			// if it fits, center the label on the button ...
			if ( (pDC->GetTextExtent(strLabel,strLabel.GetLength())).cx <= rText.Width() )
			{
				pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.right - rText.Width()/2;
			}
			// ... otherwise, left-justify and print as much of the label as will fit
			else
			{
				pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.left;

				long cxExtent = 0;
				long cxMax = rText.Width() - cxEllipsis;
				long nCharMax = strLabel.GetLength();
				long nCharCount;
				CString strTemp, strOut;
				for ( nCharCount = 1; nCharCount <= nCharMax; nCharCount++ )
				{
					strTemp = strLabel.Left(nCharCount);
					cxExtent = (pDC->GetTextExtent(strTemp)).cx;
					if ( cxExtent > cxMax )
						break;
					strOut = strTemp;
				}
				strOut += strEllipsis;
				strLabel = strOut;
			}
			pDC->ExtTextOut(x,rText.bottom,
							ETO_CLIPPED | ETO_OPAQUE,
							rText,strLabel,strLabel.GetLength(),NULL);
		}
   }

	pDC->SelectObject(pOldBrush);
}

void CTableView::DrawRowBtnBar(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	CBrush*	pOldBrush;
	long		x;
	long		i;
	long		nCount = m_aRows.GetSize();
	CRect	rText;
	CString	strLabel;
	CString strEllipsis = "...";
	long		cxEllipsis = (pDC->GetTextExtent(strEllipsis)).cx;

	// buttons (highlights and shadows)
	pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,1,m_yGrid,PATCOPY);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(m_nRowBtnBarWidth,0,1,m_yGrid,PATCOPY);

	for ( i = 0; i < nCount; i++ )
	{
		if ( m_aRows[i]->m_bIsHidden )
		{
			pDC->SelectStockObject(BLACK_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yTop,m_nRowBtnBarWidth,1,PATCOPY);
		}
		else
		{
			pDC->SelectStockObject(WHITE_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yTop,m_nRowBtnBarWidth,1,PATCOPY);
			pDC->SelectStockObject(GRAY_BRUSH);
			pDC->PatBlt(0,m_aRows[i]->m_yBottom,m_nRowBtnBarWidth,1,PATCOPY);
		}
	}

	// button labels
	rText.left = g_clm;
	rText.right = m_nRowBtnBarWidth - g_crm;
    for ( i = 0; i < nCount; i++ )
    {
		if ( m_aRows[i]->m_bIsHidden )
			continue;

		rText.top = m_aRows[i]->m_yTop + g_ctm;
		rText.bottom = m_aRows[i]->m_yBottom - g_cbm;
    	if ( m_tm.tmAveCharWidth <= rText.Width() ) // if at least one char can be printed
    	{
			pDoc->GetRowLabel(m_aRows[i]->m_idx, strLabel);

			// if it fits, center the label on the button ...
			if ( (pDC->GetTextExtent(strLabel,strLabel.GetLength())).cx <= rText.Width() )
			{
				pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.right - rText.Width()/2;
			}
			// ... otherwise, left-justify and print as much of the label as will fit
			else
			{
				pDC->SetTextAlign(TA_LEFT | TA_BOTTOM | TA_NOUPDATECP);
				x = rText.left;

				long cxExtent = 0;
				long cxMax = rText.Width() - cxEllipsis;
				long nCharMax = strLabel.GetLength();
				long nCharCount;
				CString strTemp, strOut;
				for ( nCharCount = 1; nCharCount <= nCharMax; nCharCount++ )
				{
					strTemp = strLabel.Left(nCharCount);
					cxExtent = (pDC->GetTextExtent(strTemp)).cx;
					if ( cxExtent > cxMax )
						break;
					strOut = strTemp;
				}
				strOut += strEllipsis;
				strLabel = strOut;
				/****/
			}
			pDC->ExtTextOut(x,rText.bottom,
							ETO_CLIPPED | ETO_OPAQUE,
							rText,strLabel,strLabel.GetLength(),NULL);
		}
   }

	pDC->SelectObject(pOldBrush);
}

void CTableView::HighlightColumn(long idx)
{
	CClientDC dc(this);
	CPatnDoc* pDoc = GetDocument();
	//to highlight properly
	if ( idx < m_aColumns.GetSize() &&
		m_aColumns[idx]->m_idx == pDoc->GetIdxLeftExtrinsicColumn() ) 
		idx++;
	HighlightColumn(idx, &dc);
}

void CTableView::HighlightColumn(long idx, CDC* pDC)
{
	CRect	rRect;
	long		i;
	CPatnDoc* pDoc = GetDocument();

	if ( idx < 0 || idx > m_aColumns.GetUpperBound() || m_aColumns[idx]->m_bIsHidden )
		return;

	// prevent inverting extrinsic row bar
	if ( GetDocument()->IsExtrinsicRows() &&
		(i = GetRow(GetDocument()->GetIdxTopExtrinsicRow())) != -1 &&
		!m_rExtrinsicRowBar.IsRectEmpty())
	{
		rRect.SetRect(m_aColumns[idx]->m_xLeft,0,
					  m_aColumns[idx]->m_xRight + 1,m_rExtrinsicRowBar.top);
		pDC->InvertRect(rRect);
		rRect.SetRect(m_aColumns[idx]->m_xLeft,m_rExtrinsicRowBar.bottom,
					  m_aColumns[idx]->m_xRight + 1,m_yGrid);
		pDC->InvertRect(rRect);
	}
	else
	{
		rRect.SetRect(m_aColumns[idx]->m_xLeft,0,
					  m_aColumns[idx]->m_xRight + 1,m_yGrid);
		pDC->InvertRect(rRect);
	}

}

void CTableView::HighlightRow(long idx)
{
	CClientDC dc(this);
	CPatnDoc* pDoc = GetDocument();
	//to highlight properly
	if (idx < m_aRows.GetSize() &&
		m_aRows[idx]->m_idx == pDoc->GetIdxTopExtrinsicRow() ) 
		idx++;
	HighlightRow(idx, &dc);
}

void CTableView::HighlightRow(long idx, CDC* pDC)
{
	CRect	rRect;
	long		i;

	if ( idx < 0 || idx > m_aRows.GetUpperBound() || m_aRows[idx]->m_bIsHidden )
		return;

	// prevent inverting extrinsic column bar
	if ( GetDocument()->IsExtrinsicColumns() &&
		(i = GetColumn(GetDocument()->GetIdxLeftExtrinsicColumn())) != -1 &&
		!m_rExtrinsicColumnBar.IsRectEmpty())
	{
		rRect.SetRect(0,m_aRows[idx]->m_yTop,
					  m_rExtrinsicColumnBar.left,m_aRows[idx]->m_yBottom + 1);
		pDC->InvertRect(rRect);
		rRect.SetRect(m_rExtrinsicColumnBar.right,m_aRows[idx]->m_yTop,
					  m_xGrid,m_aRows[idx]->m_yBottom + 1);
		pDC->InvertRect(rRect);
	}
	else
	{
	rRect.SetRect(0,m_aRows[idx]->m_yTop,
				  m_xGrid,m_aRows[idx]->m_yBottom + 1);
	pDC->InvertRect(rRect);
	}
}


void CTableView::UnHighlightColumn(long idx)
{
	CClientDC dc(this);
	CPatnDoc *pDoc = GetDocument();
	if (m_aColumns[idx]->m_idx == pDoc->GetIdxLeftExtrinsicColumn()) 
	{
		idx++;		
		if(idx < m_aColumns.GetSize() &&
			m_aColumns[idx]->m_idx != pDoc->GetIdxLeftExtrinsicColumn())
		{
			idx--;
			return;
		}
	}
	if(idx < m_aColumns.GetSize())
		UnHighlightColumn(idx, &dc);

}

void CTableView::UnHighlightColumn(long idx, CDC* pDC)
{
	HighlightColumn(idx, pDC);
}

void CTableView::UnHighlightRow(long idx)
{
	CClientDC dc(this);
	CPatnDoc *pDoc = GetDocument();
	if (m_aRows[idx]->m_idx == pDoc->GetIdxTopExtrinsicRow()) 
	{
		idx++;		
		if(idx < m_aRows.GetSize() && 
			m_aRows[idx]->m_idx != pDoc->GetIdxTopExtrinsicRow())
		{
			idx--;
			return;
		}
	}
	if(idx < m_aRows.GetSize())
		UnHighlightRow(idx, &dc);
}

void CTableView::UnHighlightRow(long idx, CDC* pDC)
{
	HighlightRow(idx, pDC);
}

void CTableView::ClearBackground(CRect& rect)
{
	CClientDC dc(this);
	ClearBackground(rect, &dc);
}

void CTableView::ClearBackground(CRect& rect, CDC* pDC)
{
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(rect.left,rect.top,rect.Width(),rect.Height(),PATCOPY);
	pDC->SelectObject(pOldBrush);
}

void CTableView::CalcLayout()
{
	// NOTE: some metrics that change only when the font changes are calculated in SetTableFont()

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long x, y;
	long idx;
	long nNoOfColumns = pDoc->GetNoOfColumns();
	long nNoOfRows = pDoc->GetNoOfRows();
	SCROLLINFO ScrollInfo;
	long nPageSize;
	CColumn* pColumn;
	CRow* pRow;

	// set to empty all the rects used for hit-testing
	m_rGrid.SetRectEmpty();
	m_rColumnBtnBar.SetRectEmpty();
	m_rRowBtnBar.SetRectEmpty();
	m_rExtrinsicColumnBar.SetRectEmpty();
	m_rExtrinsicRowBar.SetRectEmpty();

	// setup columns visible in view
	for ( idx = 0; idx < m_aColumns.GetSize(); idx++ )
		delete m_aColumns[idx];
	m_aColumns.RemoveAll();

	x = m_nRowBtnBarWidth + 1;
	idx = m_idxLeftColumn;
	while ( x < m_cxClient && idx < nNoOfColumns )
	{
		// if the column is extrinsic, then add a bar to the array of columns in the view
		if ( idx == pDoc->GetIdxLeftExtrinsicColumn() )
		{
			pColumn = new CColumn;
			pColumn->m_idxStart = -1;
			pColumn->m_idx = idx;
			pColumn->m_xLeft = x;
			x += m_nExtrinsicBarWidth;
			pColumn->m_xRight = x - 1;
			pColumn->m_bIsHidden = FALSE;
			pColumn->m_bIsExtrinsicBar = TRUE;
			m_aColumns.Add(pColumn);
			// the following hit-testing rect is set to m_xRight + 1 because CRect.PtInRect()
			// does not count the right and bottom edges as being within the rect
			m_rExtrinsicColumnBar.SetRect(pColumn->m_xLeft,0,pColumn->m_xRight + 1,m_cyClient);

			if ( x >= m_cxClient )
				break;
		}

		pColumn = new CColumn;
		pColumn->m_idx = idx;
		pColumn->m_xLeft = x;
		pColumn->m_bIsExtrinsicBar = FALSE;

		if ( pDoc->IsColumnHidden(idx) )
		{
			pColumn->m_idxStart = idx;
			x++;
			pColumn->m_bIsHidden = TRUE;

			// find next non-hidden column or extrinsic column
			while ( ++idx < nNoOfColumns )
			{
				if ( !pDoc->IsColumnHidden(idx) || idx == pDoc->GetIdxLeftExtrinsicColumn() )
				{
					pColumn->m_idx = idx - 1; // the one before this one
					break;
				}
			}
		}
		else
		{
			pColumn->m_idxStart = -1;
			x += CalcViewColumnWidth(idx);
			pColumn->m_bIsHidden = FALSE;
			idx++;
		}
		pColumn->m_xRight = x - 1;
		m_aColumns.Add(pColumn);
	}
	m_xGrid = x; // store in case right edge of grid is within client area

	// setup rows visible in view
	for ( idx = 0; idx < m_aRows.GetSize(); idx++ )
		delete m_aRows[idx];
	m_aRows.RemoveAll();

	y = m_nColumnBtnBarHeight + 1;
	idx = m_idxTopRow;
	while ( y < m_cyClient && idx < nNoOfRows )
	{
		// if the row is extrinsic, then add a bar to the array of rows in the view
		if ( idx == pDoc->GetIdxTopExtrinsicRow() )
		{
			pRow = new CRow;
			pRow->m_idxStart = -1;
			pRow->m_idx = idx;
			pRow->m_yTop = y;
			y += m_nExtrinsicBarHeight;
			pRow->m_yBottom = y - 1;
			pRow->m_bIsHidden = FALSE;
			pRow->m_bIsExtrinsicBar = TRUE;
			m_aRows.Add(pRow);
			// the following hit-testing rect is set to m_yBottom + 1 because CRect.PtInRect()
			// does not count the right and bottom edges as being within the rect
			m_rExtrinsicRowBar.SetRect(0,pRow->m_yTop,m_cxClient,pRow->m_yBottom + 1);
			if ( y >= m_cyClient )
				break;
		}

		pRow = new CRow;
		pRow->m_idx = idx;
		pRow->m_yTop = y;
		pRow->m_bIsExtrinsicBar = FALSE;

		if ( pDoc->IsRowHidden(idx) )
		{
			pRow->m_idxStart = idx;
			y++;
			pRow->m_bIsHidden = TRUE;

			// find next non-hidden row or extrinsic row
			while ( ++idx < nNoOfRows )
			{
				if ( !pDoc->IsRowHidden(idx) || idx == pDoc->GetIdxTopExtrinsicRow() )
				{
					pRow->m_idx = idx - 1; // the one before this one
					break;
				}
			}
		}
		else
		{
			pRow->m_idxStart = -1;
			y += m_nRowHeight;
			pRow->m_bIsHidden = FALSE;
			idx++;
		}
		pRow->m_yBottom = y - 1;
		m_aRows.Add(pRow);
	}
	m_yGrid = y; // store in case bottom edge of grid is within client area

	// some of the following hit-testing rects have 1 added because CRect.PtInRect()
	// does not count the right and bottom edges as being within the rect
	m_rGrid.SetRect(m_nRowBtnBarWidth + 1,m_nColumnBtnBarHeight + 1,m_xGrid,m_yGrid);
	m_rColumnBtnBar.SetRect(m_nRowBtnBarWidth + 1,0,m_xGrid,m_nColumnBtnBarHeight + 1);
	m_rRowBtnBar.SetRect(0,m_nColumnBtnBarHeight + 1,m_nRowBtnBarWidth + 1,m_yGrid);

	// set page size for proportional scroll box
	ScrollInfo.fMask = SIF_PAGE;
	nPageSize = m_aRows.GetSize();
	if ( m_yGrid > m_cyClient )
		nPageSize--;
	ScrollInfo.nPage = nPageSize;
	SetScrollInfo(SB_VERT,&ScrollInfo,TRUE);
	nPageSize = m_aColumns.GetSize();
	if ( m_xGrid > m_cxClient )
		nPageSize--;
	ScrollInfo.nPage = nPageSize;
	SetScrollInfo(SB_HORZ,&ScrollInfo,TRUE);
}

void CTableView::GetActiveCellRect(CRect& rCell)
{
	GetCellRect(m_ActiveCell.m_idxColumn, m_ActiveCell.m_idxRow, rCell);
}

void CTableView::GetCellRect(CCell& cell, CRect& rCell)
{
	GetCellRect(cell.m_idxColumn, cell.m_idxRow, rCell);
}

void CTableView::GetCellRect(long idxColumn, long idxRow, CRect& rCell)
{
	long iCol, iRow;

	// check whether cell is in view
	if ( (iCol = GetColumn(idxColumn)) == -1 ||
		 (iRow = GetRow(idxRow)) == -1 )
	{
		rCell.SetRectEmpty();
		return;
	}

	// if editing, then return the edit window rect
	if ( idxColumn == m_ActiveCell.m_idxColumn && idxRow == m_ActiveCell.m_idxRow &&
		 m_ActiveCell.m_bIsEditing )
	{
		m_ActiveCell.m_edit.GetWindowRect(rCell);
		ScreenToClient(rCell);
		// adjust rCell to account for frame around window
		rCell.top--;
		rCell.left--;
		return;
	}

	// check whether column is hidden or extrinsic bar
	if ( m_aColumns[iCol]->m_bIsHidden)// || m_aColumns[iCol]->m_bIsExtrinsicBar)
	{
		rCell.SetRectEmpty();
		return;
	}

	if(m_aColumns[iCol]->m_bIsExtrinsicBar){
		iCol++;
	}
	if(m_aRows[iRow]->m_bIsExtrinsicBar){
		iRow++;
	}

	// check whether row is hidden or extrinsic bar
	if ( m_aRows[iRow]->m_bIsHidden)// || m_aRows[iRow]->m_bIsExtrinsicBar )
	{
		rCell.SetRectEmpty();
		return;
	}

	rCell.SetRect(m_aColumns[iCol]->m_xLeft, m_aRows[iRow]->m_yTop,
				  m_aColumns[iCol]->m_xRight, m_aRows[iRow]->m_yBottom);
}

double CTableView::CalcDocColumnWidth(long nWidth)
{
	return (nWidth - (g_clm + g_crm))/m_nWidthUnit;
}

void CTableView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_pSelection = NULL;    // initialize selection

	m_idxLeftColumn = 0;
	m_idxTopRow = 0;
	m_nMouseAction = none;
	SetTableFont();
	CalcLayout();	// need to call now so that table layout is know before
					// calling SetActiveCellAt(0,0) (below)
	m_ActiveCell.m_edit.ShowWindow(SW_HIDE);
	m_ActiveCell.m_bIsEditing = FALSE;
	SetActiveCellAt(0,0);

	CPatnDoc* pDoc = GetDocument();
	pDoc->OnGo();

}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CTableView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CPatnCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	return pDocItem == m_pSelection;
}

void CTableView::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CPatnCntrItem object.
/*	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CPatnCntrItem* pItem = NULL;
	TRY
	{
		// Create new item connected to this document.
		CPatnDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CPatnCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item from the dialog data.
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);

		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);

		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted.

		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();*/
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void CTableView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CTableView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);
	
	GetParentFrame()->SetActiveView(this);

	//do status bar;
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	CString statbartext;
	statbartext.Format("number of objects %d, variables %d",GetDocument()->GetNoOfRows(),GetDocument()->GetNoOfColumns());
	frm->m_wndStatusBar.SetPaneText(1,statbartext);

	//this->autosizecolumns = true;
	//RedrawWindow();
}

void CTableView::OnSize(UINT nType, int cx, int cy)
{
	CPatnDoc* pDoc = GetDocument();

	CView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();

	m_cxClient = cx;
	m_cyClient = cy;
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server support

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the server (not the container) causes the deactivation.
void CTableView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTableView diagnostics

#ifdef _DEBUG
void CTableView::AssertValid() const
{
	CView::AssertValid();
}

void CTableView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPatnDoc* CTableView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPatnDoc)));
	return (CPatnDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTableView message handlers

void CTableView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here

	CMenu menu;
	CPoint local = point;
	ScreenToClient(&local);
	long i;

	if ( m_rColumnBtnBar.PtInRect(local) )
	{
		i = GetColumn(local);
		if ( !m_pSelectedColumns->IsSelected(m_aColumns[i]->m_idx) )
		{
			m_pSelectedRows->RemoveAll();
			m_pSelectedColumns->StartActiveSelection(m_aColumns[i]->m_idx);
		}

		if (menu.LoadMenu(IDR_MENU_COLUMN))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y,
				AfxGetMainWnd());
		}
	}
	else if ( m_rRowBtnBar.PtInRect(local) )
	{
		i = GetRow(local);
		if ( !m_pSelectedRows->IsSelected(m_aRows[i]->m_idx) )
		{
			m_pSelectedColumns->RemoveAll();
			m_pSelectedRows->StartActiveSelection(m_aRows[i]->m_idx);
		}

		if (menu.LoadMenu(IDR_MENU_ROW))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y,
				AfxGetMainWnd());
		}
	}
}

void CTableView::OnTableColumnAdd() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWaitCursor waitCursor;
	CUpdateHint UpdateHint;

	pDoc->AddColumn();

	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Column Added\r\n\r\n";
	
	// scroll to new column
	SendMessage(WM_HSCROLL,SB_THUMBPOSITION | pDoc->GetUpperBoundColumns() << 16,NULL);
	SetScrollRange(SB_HORZ,0,pDoc->GetUpperBoundColumns(),TRUE);

	RedrawWindow();
	pDoc->UpdateAllViews(this, CUpdateHint::addColumn, &UpdateHint);
	pDoc->SetModifiedFlag();
}

void CTableView::OnTableColumnDelete() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	CArray<int> aItems;
	long i, j;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	// retrieve selected items
	m_pSelectedColumns->GetItems(&aItems);

	// iterate through selection, deleting each column
	//->set first unselected column visible
	if((i = aItems[0] - 1) < 0) i = 0;
	SendMessage(WM_HSCROLL,
		SB_THUMBPOSITION | i << 16,NULL);

	CString buffer;
	
	/*pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Removed Columns: ";
	for(i=0;i<aItems.GetUpperBound();i++){
		pDoc->GetColumnLabel(aItems[i],buffer);
		pDoc->m_log += buffer;
		if(i < aItems.GetUpperBound()-1)
			pDoc->m_log += ",";
	}
	pDoc->m_log += "\r\n\r\n";*/
pDoc->m_loading = true;
	for ( i = aItems.GetUpperBound(); i >= 0; i-- )
		pDoc->RemoveColumnAt(aItems[i]);
pDoc->m_loading = false;
pDoc->UpdateIntrinsicTotals();
	pDoc->UpdateAllTotals();
	// now that columns are deleted, clear selection
	m_pSelectedColumns->RemoveAll();

	// if necessary, repaint window
	for ( j = 0; j < aItems.GetSize(); j++ )
	{
		if ( (i = GetColumn(aItems[j])) != -1 ) // visible in view?
		{
			rInvalid.SetRect(m_aColumns[i]->m_xLeft,0,m_cxClient,m_cyClient);
			InvalidateRect(rInvalid,TRUE);
			UpdateWindow();
			break;
		}
	}

	// update document and all other views
	
	
	SetActiveCellAt(0,0,false);

	m_idxLeftColumn = pDoc->GetUpperBoundColumns();
	SetScrollPos(SB_HORZ,pDoc->GetUpperBoundColumns(),true);

	SetScrollRange(SB_HORZ,0,pDoc->GetUpperBoundColumns(),TRUE);

	pDoc->SetModifiedFlag();
	UpdateHint.m_pItems = &aItems;
	pDoc->UpdateAllViews(this, CUpdateHint::deleteColumn, &UpdateHint);
}

void CTableView::OnTableColumnInsert() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	long nSelCount;
	CUpdateHint UpdateHint;
	long i;
	long idx;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		OnTableColumnAdd();
		return;
	}

	// don't insert if selection not contiguous
	if ( !m_pSelectedColumns->IsContiguous() )
	{
		MessageBox("Can't insert on non-contiguous selection",
				   "Insert columns",MB_OK | MB_ICONSTOP);
		return;
	}

	// insert new columns just before leftmost selected column
	nSelCount = m_pSelectedColumns->GetCount();
	idx = m_pSelectedColumns->GetMinValue();
	if ( (i = GetColumn(idx)) == -1 )
	{
		SendMessage(WM_HSCROLL,SB_THUMBPOSITION | idx << 16,NULL);
		rInvalid.left = m_aColumns[0]->m_xLeft;
	}
	else
	{
		rInvalid.left = m_aColumns[i]->m_xLeft;
	}
	
	CString buffer;
	
	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	buffer.Format("Inserted %d Columns\n\n",nSelCount);
	pDoc->m_log += buffer;

	pDoc->InsertColumnAt(idx,nSelCount);

	// shift selection right so that it stays with previously selected columns
	m_pSelectedColumns->Shift(nSelCount);

	// repaint this view
	rInvalid.right = m_cxClient;
	rInvalid.bottom = m_cyClient;
	InvalidateRect(rInvalid);
	UpdateWindow();
	
	// repaint all other views
	SetScrollRange(SB_HORZ,0,pDoc->GetUpperBoundColumns(),TRUE);
	UpdateHint.m_idxOld = idx;
	UpdateHint.m_nCount = nSelCount;
	pDoc->UpdateAllViews(this,CUpdateHint::insertColumn,&UpdateHint);
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnTableColumnMakeExtrinsic() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CArray<int> aColumns;
	long nSelCount;
	CUpdateHint UpdateHint;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	nSelCount = m_pSelectedColumns->GetCount();
	m_pSelectedColumns->GetItems(&aColumns); // fills array sorted in ascending order

	CString buffer;
	long i;

	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Made Columns Extrinsic: ";
	for(i=0;i<aColumns.GetSize();i++){
		pDoc->GetColumnLabel(aColumns[i],buffer);
		pDoc->m_log += buffer;
		if(i < aColumns.GetSize()-1)
			pDoc->m_log += ",";
	}
	pDoc->m_log += "\r\n\r\n";

	// check whether all the selected columns are already extrinsic
	if ( pDoc->IsExtrinsicColumns() && !(aColumns[0] <= pDoc->GetIdxLeftExtrinsicColumn()) )
		return;

	// find the view rect to invalidate
	// first, check whether right edge of table is in view (that's where the columns move) ...
	if ( m_xGrid < m_cxClient )
		rInvalid.left = m_xGrid;
	
	m_pSelectedColumns->RemoveAll();
	pDoc->MakeColumnsExtrinsic(&aColumns);

	// repaint this view
	if ( !rInvalid.IsRectNull())
	{
		rInvalid.right = m_cxClient;
		rInvalid.bottom = m_cyClient;
		InvalidateRect(rInvalid);
		if(!GetDocument()->m_loading) {
			UpdateWindow();
		}
	}
	
	// repaint all other views
	UpdateHint.m_pItems = &aColumns;
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL && !GetDocument()->m_loading)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	} 
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnUpdateTableColumnMakeExtrinsic(CCmdUI* pCmdUI) 
{
	bool enable = false;
	
	CArray<int> list;
	m_pSelectedColumns->GetItems(&list);
	for(long i=0;i<list.GetSize();i++){
		if(GetDocument()->GetIdxLeftExtrinsicColumn() == -1 || list[i] < GetDocument()->GetIdxLeftExtrinsicColumn()) {
			enable = true;
			break;
		}
	}

	pCmdUI->Enable(enable);
}

void CTableView::OnTableColumnMakeIntrinsic() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CArray<int> aColumns;
	long nSelCount;
	CUpdateHint UpdateHint;
	long i;
	long idxLeftExtrinsicColumn = pDoc->GetIdxLeftExtrinsicColumn();
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	UpdateHint.m_idxOld = idxLeftExtrinsicColumn; // save old idx
	nSelCount = m_pSelectedColumns->GetCount();
	m_pSelectedColumns->GetItems(&aColumns); // fills array sorted in ascending order

	CString buffer;

	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Made Columns Intrinsic: ";
	for(i=0;i<aColumns.GetSize();i++){
		pDoc->GetColumnLabel(aColumns[i],buffer);
		pDoc->m_log += buffer;
		if(i < aColumns.GetSize()-1)
			pDoc->m_log += ",";
	}
	pDoc->m_log += "\r\n\r\n";

	// check whether all the selected columns are already intrinsic
	if ( !pDoc->IsExtrinsicColumns() ||
			aColumns[aColumns.GetUpperBound()] < idxLeftExtrinsicColumn )
		return;

	// find the view rect to invalidate
	// first, check whether extrinsic column is in view (that's where the columns move) ...
	if ( (i = GetColumn(idxLeftExtrinsicColumn)) != -1 )
		rInvalid.left = m_aColumns[i]->m_xLeft;

	m_pSelectedColumns->RemoveAll();
	pDoc->MakeColumnsIntrinsic(&aColumns);

	// repaint this view
	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.right = m_cxClient;
		rInvalid.bottom = m_cyClient;
		InvalidateRect(rInvalid);
		UpdateWindow();
	}
	
	// repaint all other views
	UpdateHint.m_pItems = &aColumns;
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	}
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnUpdateTableColumnMakeIntrinsic(CCmdUI* pCmdUI) 
{
	bool enable = false;

	CArray<int> list;
	m_pSelectedColumns->GetItems(&list);
	for(long i=0;i<list.GetSize();i++){
		if(GetDocument()->GetIdxLeftExtrinsicColumn() != -1 && 
			list[i] >= GetDocument()->GetIdxLeftExtrinsicColumn()) {
				enable = true;
				break;
			}
	}
	
	pCmdUI->Enable(enable);
}

void CTableView::OnTableColumnProperties() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWaitCursor waitCursor;
	CArray<int> aItems;
	long i,j;
	long idx;
	long nWidth;
	CPropertySheet ps("Column Properties",NULL,0);
	CPageColumnOptions pageColumnOptions;
	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	CRect rInvalid(0,0,0,0);

	pageColumnOptions.m_pDoc = pDoc;

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	m_pSelectedColumns->GetItems(&aItems);

	// multiple columns selected?
	if ( m_pSelectedColumns->GetCount() == 1 ) // single column selected
	{
		idx = aItems[0];

		// label
		pDoc->GetColumnLabel(idx,pageColumnOptions.m_strLabel);
		pDoc->GetColumnGroups(idx,&pageColumnOptions.m_strGroup);
		
		// hidden?
		if ( pDoc->IsColumnHidden(idx) )
			pageColumnOptions.m_nHide = 1; // checked
		else
			pageColumnOptions.m_nHide = 0; // unchecked

		// width
		if ( pDoc->GetColumnWidth(idx) == g_nStandardColumnWidth )
		{
			pageColumnOptions.m_nWhichColumnWidth = CPageColumnOptions::standard;
			pageColumnOptions.m_nColumnWidth = g_nStandardColumnWidth;
		}
		else
		{
			pageColumnOptions.m_nWhichColumnWidth = CPageColumnOptions::custom;
			// restrict to 2 decimal places
			nWidth = (long)(pDoc->GetColumnWidth(idx) * 100);
			pageColumnOptions.m_nColumnWidth = (double)(nWidth / 100.0);
		}

		// decimal precision
		pageColumnOptions.m_nDecimalPrecision = pDoc->GetColumnDecimalPrecision(idx);
	}
	else // multiple columns selected
	{
		pageColumnOptions.m_bMultiple = TRUE;

		// decimal precision
		pageColumnOptions.m_nDecimalPrecision = g_nDefaultDecimalPrecision;
		pageColumnOptions.m_nColumnWidth = g_nStandardColumnWidth;
		pageColumnOptions.m_strGroup = -1;

	}

	ps.AddPage(&pageColumnOptions);

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// display options dialog box
	if ( ps.DoModal() == IDCANCEL )
		return;

	waitCursor.Restore(); // restore wait cursor after dialog box

	// label
	if ( pageColumnOptions.m_bChangedLabel )
		for ( i = 0; i < aItems.GetSize(); i++ )
		{
			pDoc->SetColumnLabel(aItems[i],pageColumnOptions.m_strLabel);
		}

	if ( pageColumnOptions.m_bChangedGroup  && aItems.GetSize() == 1)
		pDoc->SetColumnGroups(aItems[0],pageColumnOptions.m_strGroup);
		
			

	// hidden?
	if ( pageColumnOptions.m_bChangedHidden )
	{
		switch ( pageColumnOptions.m_nHide )
		{
			case 0:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->UnhideColumn(aItems[i]);
				break;

			case 1:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->HideColumn(aItems[i]);
				break;
		}
	}

	// width
	if ( pageColumnOptions.m_bChangedColumnWidth )
	{
		switch ( pageColumnOptions.m_nWhichColumnWidth )
		{
			case CPageColumnOptions::standard:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->SetColumnWidth(aItems[i],g_nStandardColumnWidth);
				break;

			case CPageColumnOptions::custom:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->SetColumnWidth(aItems[i],pageColumnOptions.m_nColumnWidth);
				break;

			case CPageColumnOptions::sizetofit:
				SizeToFit(&aItems);
				break;
		}
	}

	// decimal precision
	if ( pageColumnOptions.m_bChangedDecimalPrecision )
		for ( i = 0; i < aItems.GetSize(); i++ )
			pDoc->SetColumnDecimalPrecision(aItems[i],pageColumnOptions.m_nDecimalPrecision);

	// if necessary, repaint window
	for ( j = 0; j < aItems.GetSize(); j++ )
	{
		if ( (i = GetColumn(aItems[j])) != -1 ) // visible in view?
		{
			rInvalid.SetRect(m_aColumns[i]->m_xLeft,0,m_cxClient,m_cyClient);
			InvalidateRect(rInvalid,TRUE);
			UpdateWindow();
			break;
		}
	}

	//this->autosizecolumns = true;
	RedrawWindow();

	// update document and all other views
	pDoc->SetModifiedFlag();
	// for updating, use any relevant hint that causes columns to be repainted
	UpdateHint.m_pSelection = m_pSelectedColumns;
	pDoc->UpdateAllViews(this, CUpdateHint::resizeColumn, &UpdateHint);

	pDoc->UpdateGroups();
}

void CTableView::OnTableRowAdd() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWaitCursor waitCursor;
	CUpdateHint UpdateHint;

	GetDocument()->AddRow();

	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Row Added\r\n\r\n";
	
	// scroll to new row
	SendMessage(WM_VSCROLL,SB_THUMBPOSITION | pDoc->GetUpperBoundRows() << 16,NULL);

	SetScrollRange(SB_VERT,0,pDoc->GetUpperBoundRows(),TRUE);

	RedrawWindow();
	pDoc->UpdateAllViews(this, CUpdateHint::addRow, &UpdateHint);
	pDoc->SetModifiedFlag();
}

void CTableView::OnTableRowDelete() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	CArray<int> aItems;
	long i, j;
	CRect rInvalid(0,0,0,0);

	// if there are no rows already selected, then get user to select rows now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no rows selected
			return;
	}

	// retrieve selected items
	m_pSelectedRows->GetItems(&aItems);

	CString buffer;

/*	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Removed Rows: ";
	for(i=0;i<aItems.GetUpperBound();i++){
		pDoc->GetRowLabel(aItems[i],buffer);
		pDoc->m_log += buffer;
		if(i < aItems.GetUpperBound()-1)
			pDoc->m_log += ",";
	}
	pDoc->m_log += "\r\n\r\n";*/

	// iterate through selection, deleting each row
	//->set first unselected row visible
	if((i = aItems[0] - 1) < 0) i = 0;
	SendMessage(WM_VSCROLL,
		SB_THUMBPOSITION | i << 16,NULL);
	pDoc->m_loading = true;
	for ( i = aItems.GetUpperBound(); i >= 0; i-- )
		pDoc->RemoveRowAt(aItems[i]);
	pDoc->m_loading = false;

	// now that rows are deleted, clear selection
	m_pSelectedRows->RemoveAll();

	// if necessary, repaint window
	for ( j = 0; j < aItems.GetSize(); j++ )
	{
		if ( (i = GetRow(aItems[j])) != -1 ) // visible in view?
		{
			rInvalid.SetRect(0,m_aRows[i]->m_yTop,m_cxClient,m_cyClient);
			InvalidateRect(rInvalid,TRUE);
			UpdateWindow();
			break;
		}
	}

	// update document and all other views
	SetActiveCellAt(0,0,false);

	m_idxTopRow = pDoc->GetUpperBoundRows();
	SetScrollPos(SB_VERT,pDoc->GetUpperBoundRows(),true);

		SetScrollRange(SB_VERT,0,pDoc->GetUpperBoundRows(),TRUE);


	pDoc->UpdateIntrinsicTotals();
	pDoc->UpdateAllTotals();
	pDoc->SetModifiedFlag();
	UpdateHint.m_pItems = &aItems;
	pDoc->UpdateAllViews(this, CUpdateHint::deleteRow, &UpdateHint);
}

void CTableView::OnTableRowInsert() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	long nSelCount;
	CUpdateHint UpdateHint;
	long i;
	long idx;
	CRect rInvalid(0,0,0,0);

	//add to end if no rows already selected
	if(m_pSelectedRows->IsEmpty()){
		OnTableRowAdd();
		return;
	}

	// don't insert if selection not contiguous
	if ( !m_pSelectedRows->IsContiguous() )
	{
		MessageBox("Can't insert on non-contiguous selection",
				   "Insert rows",MB_OK | MB_ICONSTOP);
		return;
	}

	// insert new rows just before topmost selected row
	nSelCount = m_pSelectedRows->GetCount();
	idx = m_pSelectedRows->GetMinValue();
	if ( (i = GetRow(idx)) == -1 )
	{
		SendMessage(WM_VSCROLL,SB_THUMBPOSITION | idx << 16,NULL);
		rInvalid.top = m_aRows[0]->m_yTop;
	}
	else
	{
		rInvalid.top = m_aRows[i]->m_yTop;
	}
	
	pDoc->InsertRowAt(idx,nSelCount);

	CString buffer;
	buffer.Format("Inserted %d Rows\r\n\r\n",nSelCount);

	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += buffer;

	// shift selection down so that it stays with previously selected rows
	m_pSelectedRows->Shift(nSelCount);

	// repaint this view
	rInvalid.right = m_cxClient;
	rInvalid.bottom = m_cyClient;
	InvalidateRect(rInvalid);
	UpdateWindow();
	
	// repaint all other views
	UpdateHint.m_idxOld = idx;
	UpdateHint.m_nCount = nSelCount;

	SetScrollRange(SB_VERT,0,pDoc->GetUpperBoundRows(),TRUE);

	pDoc->UpdateAllViews(this,CUpdateHint::insertRow,&UpdateHint);
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnTableRowMakeExtrinsic() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CArray<int> aRows;
	long nSelCount;
	CUpdateHint UpdateHint;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no columns selected
			return;
	}

	nSelCount = m_pSelectedRows->GetCount();
	m_pSelectedRows->GetItems(&aRows); // fills array sorted in ascending order

	CString buffer;
	pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	pDoc->m_log += "\r\n";

	pDoc->m_log += "Made Rows Extrinsic: ";
	long i;
	for(i=0;i<aRows.GetSize();i++){
		pDoc->GetRowLabel(aRows[i],buffer);
		pDoc->m_log += buffer;
		if(i < aRows.GetSize()-1)
			pDoc->m_log += ",";
	}
	pDoc->m_log += "\r\n\r\n";


	// check whether all the selected columns are already extrinsic
	if ( pDoc->IsExtrinsicRows() && !(aRows[0] < pDoc->GetIdxTopExtrinsicRow()) )
		return;

	// find the view rect to invalidate
	// first, check whether right edge of table is in view (that's where the columns move) ...
	if ( m_yGrid < m_cyClient )
		rInvalid.top = m_yGrid;
	
	m_pSelectedRows->RemoveAll();
	pDoc->MakeRowsExtrinsic(&aRows);

	// repaint this view
	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.right = m_cxClient;
		rInvalid.bottom = m_cyClient;
		InvalidateRect(rInvalid);
		UpdateWindow();
	}
	
	// repaint all other views
	UpdateHint.m_pItems = &aRows;
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL && !GetDocument()->m_loading)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	}
	pDoc->SetModifiedFlag(TRUE);

}

void CTableView::OnUpdateTableRowMakeExtrinsic(CCmdUI* pCmdUI) 
{
	bool enable = false;
	CArray<int> list;
	m_pSelectedRows->GetItems(&list);
	for(long i=0;i<list.GetSize();i++){
		if(GetDocument()->GetIdxTopExtrinsicRow() == -1 || list[i] < GetDocument()->GetIdxTopExtrinsicRow()) {
			enable = true;
			break;
		}
	}

	pCmdUI->Enable(enable);
}

void CTableView::OnTableRowMakeIntrinsic() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CArray<int> aRows;
	long nSelCount;
	CUpdateHint UpdateHint;
	long i;
	long idxTopExtrinsicRow = pDoc->GetIdxTopExtrinsicRow();
	CRect rInvalid(0,0,0,0);

	// if there are no rows already selected, then get user to select rows now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no rows selected
			return;
	}

	UpdateHint.m_idxOld = idxTopExtrinsicRow; // save old idx
	nSelCount = m_pSelectedRows->GetCount();
	m_pSelectedRows->GetItems(&aRows); // fills array sorted in ascending order

	// check whether all the selected rows are already intrinsic
	if ( !pDoc->IsExtrinsicRows() ||
			aRows[aRows.GetUpperBound()] < idxTopExtrinsicRow )
		return;

	// find the view rect to invalidate
	// first, check whether extrinsic row is in view (that's where the rows move) ...
	if ( (i = GetRow(idxTopExtrinsicRow)) != -1 )
		rInvalid.top = m_aRows[i]->m_yTop;

	m_pSelectedRows->RemoveAll();
	pDoc->MakeRowsIntrinsic(&aRows);

	// repaint this view
	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.right = m_cxClient;
		rInvalid.bottom = m_cyClient;
		InvalidateRect(rInvalid);
		UpdateWindow();
	}
	
	// repaint all other views
	UpdateHint.m_pItems = &aRows;
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL && !GetDocument()->m_loading)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	}
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnUpdateTableRowMakeIntrinsic(CCmdUI* pCmdUI) 
{
	bool enable = false;

	CArray<int> list;
	m_pSelectedRows->GetItems(&list);
	for(long i=0;i<list.GetSize();i++){
		if(GetDocument()->GetIdxTopExtrinsicRow() != -1 &&
			list[i] >= GetDocument()->GetIdxTopExtrinsicRow()) {
				enable = true;
				break;
			}
	}

	pCmdUI->Enable(enable);
}

void CTableView::OnTableRowProperties() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CWaitCursor waitCursor;
	CArray<int> aItems;
	long i;
	long idx;
	CPropertySheet ps("Row Properties",NULL,0);
	CPageRowOptions pageRowOptions;
	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	CRect rInvalid(0,0,0,0);

	pageRowOptions.m_pDoc = pDoc;

	// if there are no rows already selected, then get user to select rows now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no rows selected
			return;
	}

	m_pSelectedRows->GetItems(&aItems);

	// multiple rows selected?
	if ( m_pSelectedRows->GetCount() == 1 ) // single row selected
	{
		idx = aItems[0];

		// label
		pDoc->GetRowLabel(idx,pageRowOptions.m_strLabel);
		pDoc->GetRowGroups(idx,&pageRowOptions.m_strGroup);

		// hidden?
		if ( pDoc->IsRowHidden(idx) )
			pageRowOptions.m_nHide = 1; // checked
		else
			pageRowOptions.m_nHide = 0; // unchecked
	}
	else // multiple rows selected
	{
		pageRowOptions.m_bMultiple = TRUE;
		pageRowOptions.m_strGroup = -1;
	}

	ps.AddPage(&pageRowOptions);

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// display options dialog box
	if ( ps.DoModal() == IDCANCEL )
		return;

	waitCursor.Restore(); // restore wait cursor after dialog box

	// label
	if ( pageRowOptions.m_bChangedLabel )
		for ( i = 0; i < aItems.GetSize(); i++ )
		{
			pDoc->SetRowLabel(aItems[i],pageRowOptions.m_strLabel);
		}

	if ( pageRowOptions.m_bChangedGroup && aItems.GetSize() == 1)
		pDoc->SetRowGroups(aItems[0],pageRowOptions.m_strGroup);
		


	// hidden?
	if ( pageRowOptions.m_bChangedHidden )
	{
		switch ( pageRowOptions.m_nHide )
		{
			case 0:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->UnhideRow(aItems[i]);
				break;

			case 1:
				for ( i = 0; i < aItems.GetSize(); i++ )
					pDoc->HideRow(aItems[i]);
				break;
		}
	}

	// if necessary, repaint window
	//for ( j = 0; j < aItems.GetSize(); j++ )
	//{
//		if ( (i = GetRow(aItems[j])) != -1 ) // visible in view?
		//{
			//rInvalid.SetRect(0,m_aRows[i]->m_yTop,m_cxClient,m_cyClient);
			//InvalidateRect(rInvalid,TRUE);
			//UpdateWindow();
			//break;
		//}
	//}
	//autosizecolumns = true;
	RedrawWindow();

	// update document and all other views
	pDoc->SetModifiedFlag();
	// for updating, use any relevant hint that causes rows to be repainted
	UpdateHint.m_pSelection = m_pSelectedRows;
	pDoc->UpdateAllViews(this, CUpdateHint::resizeRow, &UpdateHint);

	pDoc->UpdateGroups();
}

void CTableView::OnTableColumnSelect() 
{
	CSelectColumnsOrRowsDlg dlg(this, GetDocument());

	dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
	dlg.DoModal();

	CPatnDoc* pDoc = (CPatnDoc*) GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
			pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)) ||
			pView->IsKindOf(RUNTIME_CLASS(C3DXView)))// ||
			//pView->IsKindOf(RUNTIME_CLASS(CScat)))
			pView->RedrawWindow();
	}   
}

void CTableView::OnTableRowSelect() 
{
	CSelectColumnsOrRowsDlg dlg(this, GetDocument());
	
	dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
	dlg.DoModal();
	
	CPatnDoc* pDoc = (CPatnDoc*) GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
			pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)) ||
			pView->IsKindOf(RUNTIME_CLASS(C3DXView)))// ||
//			pView->IsKindOf(RUNTIME_CLASS(CScat)))
			pView->RedrawWindow();
	}    
}

void CTableView::OnTableColumnHide() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	long i;
	long idx;
	long xLeft;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	// itereate through selection, hiding each column and determining rect to repaint
	xLeft = m_cxClient;
	idx = m_pSelectedColumns->GetFirst();
	while ( idx != -1 )
	{
		pDoc->HideColumn(idx);

		if ( (i = GetColumn(idx)) != -1 ) // any need to update view?
			xLeft = min(xLeft, m_aColumns[i]->m_xLeft);

		idx = m_pSelectedColumns->GetNext();
	}

	// if necessary, repaint window
	if ( xLeft < m_cxClient )
	{
		rInvalid.SetRect(xLeft,0,m_cxClient,m_cyClient);
		InvalidateRect(rInvalid,TRUE);
		UpdateWindow();
	}

	// update document and all other views
	pDoc->SetModifiedFlag();
	UpdateHint.m_pSelection = m_pSelectedColumns;
	GetDocument()->UpdateAllViews(this, CUpdateHint::hideColumn, &UpdateHint);

	// now that columns are hidden, clear selection
	m_pSelectedColumns->RemoveAll();
}

void CTableView::OnTableColumnGotoExtrinsics() 
{
	//SendMessage(WM_HSCROLL,
	//		SB_THUMBPOSITION | GetDocument()->GetIdxLeftExtrinsicColumn() << 16,NULL);
	
	if(GetDocument()->GetIdxLeftExtrinsicColumn() >= 0)
	{
		m_idxLeftColumn = GetDocument()->GetIdxLeftExtrinsicColumn();
		SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);
		RedrawWindow();
		m_pRowTotalsView->RedrawWindow();
		this->m_pColumnTotalsView->RedrawWindow();
	}
}

void CTableView::OnUpdateTableColumnGotoExtrinsics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( GetDocument()->IsExtrinsicColumns() );
}

void CTableView::OnTableColumnUnhide() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	long i;
	long idx;
	long xLeft;
	CRect rInvalid(0,0,0,0);

	// if there are no columns already selected, then get user to select columns now
	if ( m_pSelectedColumns->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::columns;
		dlg.DoModal();
		if ( m_pSelectedColumns->IsEmpty() ) // still no columns selected
			return;
	}

	// itereate through selection, unhiding each column and determining rect to repaint
	xLeft = m_cyClient;
	idx = m_pSelectedColumns->GetFirst();
	while ( idx != -1 )
	{
		pDoc->UnhideColumn(idx);
		//also check adjacent columns
		long t = idx-1;
		while(t >= 0 && t < m_aColumns.GetSize())
		{
			if(m_aColumns[t]->m_bIsHidden)
			{
				pDoc->UnhideColumn(t);
				t = t -1;
			}
			else
			{
				break;
			}
		}
		t = idx+1;
		while(t >= 0 && t < m_aColumns.GetSize())
		{
			if(m_aColumns[t]->m_bIsHidden)
			{
				pDoc->UnhideColumn(t);
				t = t + 1;
			}
			else
			{
				break;
			}
		}
		

		if ( (i = GetColumn(idx)) != -1 ) // any need to update view?
			xLeft = min(xLeft, m_aColumns[i]->m_xLeft);

		idx = m_pSelectedColumns->GetNext();
	}

	// if necessary, repaint window
	if ( xLeft < m_cyClient )
	{
		rInvalid.SetRect(xLeft,0,m_cxClient,m_cyClient);
		InvalidateRect(rInvalid,TRUE);
		UpdateWindow();
	}

	// update document and all other views
	pDoc->SetModifiedFlag();
	UpdateHint.m_pSelection = m_pSelectedColumns;
	GetDocument()->UpdateAllViews(this, CUpdateHint::unhideColumn, &UpdateHint);
}

void CTableView::OnTableRowGotoExtrinsics() 
{
	//SendMessage(WM_VSCROLL,
	//		SB_THUMBPOSITION | GetDocument()->GetIdxTopExtrinsicRow() << 16,NULL);
	if(GetDocument()->GetIdxTopExtrinsicRow() >= 0)
	{
		m_idxTopRow = GetDocument()->GetIdxTopExtrinsicRow();
		SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
		RedrawWindow();
		m_pRowTotalsView->RedrawWindow();
		this->m_pColumnTotalsView->RedrawWindow();
	}
}

void CTableView::OnUpdateTableRowGotoExtrinsics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( GetDocument()->IsExtrinsicRows() );
}

void CTableView::OnTableRowHide() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	long i;
	long idx;
	long yTop;
	CRect rInvalid(0,0,0,0);

	// if there are no rows already selected, then get user to select rows now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no rows selected
			return;
	}

	// itereate through selection, hiding each rows and determining rect to repaint
	yTop = m_cyClient;
	idx = m_pSelectedRows->GetFirst();
	while ( idx != -1 )
	{
		pDoc->HideRow(idx);

		if ( (i = GetRow(idx)) != -1 ) // any need to update view?
			yTop = min(yTop, m_aRows[i]->m_yTop);

		idx = m_pSelectedRows->GetNext();
	}

	// if necessary, repaint window
	if ( yTop < m_cyClient )
	{
		rInvalid.SetRect(0,yTop,m_cxClient,m_cyClient);
		InvalidateRect(rInvalid,TRUE);
		UpdateWindow();
	}

	// update document and all other views
	pDoc->SetModifiedFlag();
	UpdateHint.m_pSelection = m_pSelectedRows;
	GetDocument()->UpdateAllViews(this, CUpdateHint::hideRow, &UpdateHint);

	// now that rows are hidden, clear selection
	m_pSelectedRows->RemoveAll();
}

void CTableView::OnTableRowUnhide() 
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSelectColumnsOrRowsDlg dlg(this, pDoc);
	CUpdateHint UpdateHint;
	long i;
	long idx;
	long yTop;
	CRect rInvalid(0,0,0,0);

	// if there are no rows already selected, then get user to select rows now
	if ( m_pSelectedRows->IsEmpty() )
	{
		dlg.m_nDataType = CSelectColumnsOrRowsDlg::rows;
		dlg.DoModal();
		if ( m_pSelectedRows->IsEmpty() ) // still no rows selected
			return;
	}

	// itereate through selection, unhiding each rows and determining rect to repaint
	yTop = m_cyClient;
	idx = m_pSelectedRows->GetFirst();
	while ( idx != -1 )
	{
		pDoc->UnhideRow(idx);
		//check adjacent
		long t = idx-1;
		while(t >= 0 && t < m_aRows.GetSize())
		{
			if(m_aRows[t]->m_bIsHidden)
			{
				pDoc->UnhideRow(t);
				t = t - 1;
			}
			else
			{
				break;
			}
		}
		t = idx + 1;
		while(t >= 0 && t < m_aRows.GetSize())
		{
			if(m_aRows[t]->m_bIsHidden)
			{
				pDoc->UnhideRow(t);
				t = t + 1;
			}
			else
			{
				break;
			}
		}

		if ( (i = GetRow(idx)) != -1 ) // any need to update view?
			yTop = min(yTop, m_aRows[i]->m_yTop);

		idx = m_pSelectedRows->GetNext();
	}

	// if necessary, repaint window
	if ( yTop < m_cyClient )
	{
		rInvalid.SetRect(0,yTop,m_cxClient,m_cyClient);
		InvalidateRect(rInvalid,TRUE);
		UpdateWindow();
	}

	// update document and all other views
	pDoc->SetModifiedFlag();
	UpdateHint.m_pSelection = m_pSelectedRows;
	GetDocument()->UpdateAllViews(this, CUpdateHint::unhideRow, &UpdateHint);
}

void CTableView::OnTableFont() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFontDialog dlg(&(pDoc->m_lfCells));
	dlg.m_cf.rgbColors = pDoc->m_rgbFontCells;
	
	if ( dlg.DoModal() != IDOK )
		return;

	pDoc->m_nPointSizeCells = dlg.GetSize();
	pDoc->m_rgbFontCells = dlg.GetColor();

	pDoc->SetModifiedFlag();
	SetTableFont();
	pDoc->UpdateAllViews(this,CUpdateHint::setFont,NULL);
}

void CTableView::SetTableFont()
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if(pDoc->m_loading)
		return;

	LOGFONT* plf = &(pDoc->m_lfCells);
	CClientDC dc(this);
	CFont* pOldFont;

	// note: pDoc->m_nPointSizeCells is in tenths of a point, hence divide by ten
	//plf->lfHeight = -(MulDiv(pDoc->m_nPointSizeCells, dc.GetDeviceCaps(LOGPIXELSY), 72))/10;

	// create font
	if ( m_fontCells.GetSafeHandle() != NULL ) // currently attached to Windows GDI object
		m_fontCells.DeleteObject();
	if ( !m_fontCells.CreateFontIndirect(plf) ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not create required font for table");
	}
	if ( (pOldFont = dc.SelectObject(&m_fontCells)) == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for table");
	}

	// set same font for active cell edit window
	m_ActiveCell.m_edit.SetFont(&m_fontCells);

	// use new font to calculate fontsize-dependent metrics
	// calc row height based on font size
	dc.GetTextMetrics(&m_tm);
	m_nRowHeight = m_tm.tmHeight + g_ctm + g_cbm;

	// calc column width unit based on average width of digits
	
	m_nWidthUnit = (float)((dc.GetTextExtent("0123456789",9)).cx / 10.0);
	//m_nWidthUnit = (float)m_tm.tmMaxCharWidth;

	// calc size of extrinsic bars
	m_nExtrinsicBarWidth = (long)m_nWidthUnit;
	m_nExtrinsicBarHeight = m_nExtrinsicBarWidth
						  * dc.GetDeviceCaps(LOGPIXELSY) / dc.GetDeviceCaps(LOGPIXELSX);
	
	// calc size of button bars
	m_nColumnBtnBarHeight = m_nRowHeight;
	if(m_nRowBtnBarWidth < (long)(11 * m_nWidthUnit))
		m_nRowBtnBarWidth = (long)(11 * m_nWidthUnit);
	//if(m_nRowBtnBarWidth > 200)
	//	m_nRowBtnBarWidth = 200;

	// force repaint of entire client area so that new font takes effect
	//Invalidate(TRUE);
	//UpdateWindow();

	dc.SelectObject(pOldFont);
}

void CTableView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rInvalid(0,0,0,0);
	long		i,j;
	long		idx;
	CUpdateHint* pH;
	CColumnSelection* pCS;
	CRowSelection* pRS;
	long		idxOldLeftColumn = m_idxLeftColumn;
	long		idxOldTopRow = m_idxTopRow;
	
	SetScrollRange(SB_VERT,0,pDoc->GetUpperBoundRows(),TRUE);
	SetScrollRange(SB_HORZ,0,pDoc->GetUpperBoundColumns(),TRUE);

	// if can't use hint then repaint all
	if ( pHint == NULL || !pHint->IsKindOf(RUNTIME_CLASS(CUpdateHint)) )
	{
		if (m_idxLeftColumn >= pDoc->GetNoOfColumns() ) {
			m_idxLeftColumn = 0;
		}

		if (m_idxTopRow >= pDoc->GetNoOfRows() ) {
			m_idxTopRow = 0;
		}

		Invalidate();
		UpdateWindow();
		return;
	}

	pH = (CUpdateHint*)pHint;

	switch ( lHint )
	{
		case CUpdateHint::editCell:
			GetCellRect(pH->m_Cell,rInvalid);
			break;

		case CUpdateHint::resizeColumn:
		case CUpdateHint::hideColumn:
		case CUpdateHint::unhideColumn:
			// if the column is part of a selection, are any columns in view?
			if ( pH->m_pSelection != NULL )
			{
				pCS = (CColumnSelection*)pH->m_pSelection;
				idx = pCS->GetFirst();
				while ( idx != -1 )
				{
					if ( (i = GetColumn(idx)) != -1 ) // in view?
					{
						if ( rInvalid.left == 0 || m_aColumns[i]->m_xLeft < rInvalid.left )
							rInvalid.left = m_aColumns[i]->m_xLeft;
					}
					idx = pCS->GetNext();
				}
			}
			else // not part of selection, so just check if affected column is in view
			{
				if ( (i = GetColumn(pH->m_idxOld)) != -1 ) // in view?
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
				}
			}
			break;

		case CUpdateHint::resizeRow:
			// resizing of rows is not supported by views in this version of CTableView
			break;

		case CUpdateHint::hideRow:
		case CUpdateHint::unhideRow:
			// if the row is part of a selection, are any rows in view?
			if ( pH->m_pSelection != NULL )
			{
				pRS = (CRowSelection*)pH->m_pSelection;
				idx = pRS->GetFirst();
				while ( idx != -1 )
				{
					if ( (i = GetRow(idx)) != -1 ) // in view?
					{
						if ( rInvalid.top == 0 || m_aRows[i]->m_yTop < rInvalid.top )
							rInvalid.top = m_aRows[i]->m_yTop;
					}
					idx = pRS->GetNext();
				}
			}
			else // not part of selection, so just check if affected row is in view
			{
				if ( (i = GetRow(pH->m_idxOld)) != -1 ) // in view?
				{
					rInvalid.top = m_aRows[i]->m_yTop;
				}
			}
			break;

		case CUpdateHint::moveColumn:
			// adjust m_idxLeftColumn so that the current left column remains where it is
			if ( m_idxLeftColumn > (pH->m_idxOld + pH->m_nCount - 1) &&
				 m_idxLeftColumn < pH->m_idxNew )
			{
				m_idxLeftColumn -= pH->m_nCount;
			}
			else if ( m_idxLeftColumn < pH->m_idxOld && m_idxLeftColumn >= pH->m_idxNew )
			{
				m_idxLeftColumn += pH->m_nCount;
			}
			else if ( m_idxLeftColumn >= pH->m_idxOld &&
					  m_idxLeftColumn <= (pH->m_idxOld + pH->m_nCount - 1) )
			{
				if ( m_idxLeftColumn < pH->m_idxNew )
				{
					m_idxLeftColumn += pH->m_idxNew - (pH->m_idxOld + pH->m_nCount);
				}
				else
				{
					m_idxLeftColumn -= pH->m_idxOld - pH->m_idxNew;
				}
			}

			// check whether any part of 'from' block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetColumn(idx)) != -1 )
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}

			// check whether any part of destination is in view
			for ( idx = pH->m_idxNew; idx <= (pH->m_idxNew + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetColumn(idx)) != -1 )
				{
					if ( rInvalid.left == 0 || m_aColumns[i]->m_xLeft < rInvalid.left )
						rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}
			break;
 
		case CUpdateHint::moveRow:
			// adjust m_idxTopRow so that the current top row remains where it is
			if ( m_idxTopRow > (pH->m_idxOld + pH->m_nCount - 1) &&
				 m_idxTopRow < pH->m_idxNew )
			{
				m_idxTopRow -= pH->m_nCount;
			}
			else if ( m_idxTopRow < pH->m_idxOld && m_idxTopRow >= pH->m_idxNew )
			{
				m_idxTopRow += pH->m_nCount;
			}
			else if ( m_idxTopRow >= pH->m_idxOld &&
					  m_idxTopRow <= (pH->m_idxOld + pH->m_nCount - 1) )
			{
				if ( m_idxTopRow < pH->m_idxNew )
				{
					m_idxTopRow += pH->m_idxNew - (pH->m_idxOld + pH->m_nCount);
				}
				else
				{
					m_idxTopRow -= pH->m_idxOld - pH->m_idxNew;
				}
			}

			// check whether any part of 'from' block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetRow(idx)) != -1 )
				{
					rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}

			// check whether any part of destination is in view
			for ( idx = pH->m_idxNew; idx <= (pH->m_idxNew + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetRow(idx)) != -1 )
				{
					if ( rInvalid.top == 0 || m_aRows[i]->m_yTop < rInvalid.top )
						rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}
			break;

		case CUpdateHint::makeColumnExtrinsic:
			// first, check whether right edge of table is in view
			// (that's where the columns move) ...
			if ( m_xGrid < m_cxClient )
				rInvalid.left = m_xGrid;
			
			// ... then check whether any of the columns to be made extrinsic are in view
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = GetColumn(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}
			break;

		case CUpdateHint::makeColumnIntrinsic:
			// first check whether any of the columns to be made intrinsic are in view ...
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = GetColumn(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}

			// ... then check whether previous extrinsic column is in view
			// (that's where the columns move) ...
			if ( (i = GetColumn(pH->m_idxOld)) != -1 )
			{
				if ( rInvalid.left == 0 )
					rInvalid.left = m_aColumns[i]->m_xLeft;
				else
					rInvalid.left = min(rInvalid.left, m_aColumns[i]->m_xLeft);
			}
			break;

		case CUpdateHint::makeRowExtrinsic:
			// first, check whether bottom edge of table is in view
			//(that's where the rows move) ...
			if ( m_yGrid < m_cyClient )
				rInvalid.top = m_yGrid;
			
			// ... then check whether any of the rows to be made extrinsic are in view
			for ( i = 0; i <= pH->m_nCount; i++ )
			{
				if ( (i = GetColumn(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}
			break;

		case CUpdateHint::makeRowIntrinsic:
			// first check whether any of the rows to be made intrinsic are in view ...
			for ( i = 0; i <= pH->m_pItems->GetSize(); i++ )
			{
				if ( (i = GetRow(pH->m_pItems->GetAt(i))) != -1 )
				{
					rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}

			// ... then check whether previous extrinsic row is in view
			// (that's where the rows move) ...
			if ( (i = GetRow(pH->m_idxOld)) != -1 )
			{
				if ( rInvalid.top == 0 )
					rInvalid.top = m_aRows[i]->m_yTop;
				else
					rInvalid.top = min(rInvalid.top, m_aRows[i]->m_yTop);
			}
			break;

		case CUpdateHint::insertColumn:
			// check whether any part of block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetColumn(idx)) != -1 )
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}
			break;

		case CUpdateHint::deleteColumn:
			// iterate through selection, determining rect to repaint
			for ( j = 0; j < pH->m_pItems->GetSize(); j++ )
			{
				if ( (i = GetColumn(pH->m_pItems->GetAt(j))) != -1 ) // visible in view?
				{
					rInvalid.left = m_aColumns[i]->m_xLeft;
					break;
				}
			}
			break;

		case CUpdateHint::insertRow:
			// check whether any part of block is within view
			for ( idx = pH->m_idxOld; idx <= (pH->m_idxOld + pH->m_nCount - 1); idx++ )
			{
				if ( (i = GetRow(idx)) != -1 )
				{
					rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}
			break;

		case CUpdateHint::deleteRow:
			// iterate through selection, determining rect to repaint
			for ( j = 0; j < pH->m_pItems->GetSize(); j++ )
			{
				if ( (i = GetRow(pH->m_pItems->GetAt(j))) != -1 ) // visible in view?
				{
					rInvalid.top = m_aRows[i]->m_yTop;
					break;
				}
			}
			break;

		case CUpdateHint::addColumn:
			if ( m_xGrid < m_cxClient )
				rInvalid.left = m_xGrid;
			break;

		case CUpdateHint::addRow:
			//if ( m_yGrid < m_cyClient )
			//	rInvalid.top = m_yGrid;
			rInvalid.right = m_cxClient;
			rInvalid.bottom = m_cyClient;
			break;

		case CUpdateHint::setFont:
			SetTableFont();
			break;

		// none of the following cause changes to this view, so do nothing
		case CUpdateHint::changeColumnTotalsOrder:
		case CUpdateHint::changeColumnAllocOrder:
		case CUpdateHint::changeRowTotalsOrder:
		case CUpdateHint::changeRowAllocOrder:
		case CUpdateHint::resizeRowTotalsColumn:
		case CUpdateHint::resizeRowAllocColumn:
		case CUpdateHint::resizeRowOrdColumn:
		case CUpdateHint::changeRowOrdOrder:
			break;

		default:  // not a hint that we recognise, so repaint all
			Invalidate(TRUE);
			UpdateWindow();
			break;
	}

	if ( !rInvalid.IsRectNull() )
	{
		rInvalid.right = m_cxClient;
		rInvalid.bottom = m_cyClient;
		InvalidateRect(rInvalid);
		UpdateWindow();
	}

	if ( m_idxLeftColumn != idxOldLeftColumn )
		SetScrollPos(SB_HORZ,m_idxLeftColumn);

	if ( m_idxTopRow != idxOldTopRow )
		SetScrollPos(SB_VERT,m_idxTopRow);

	pDoc->UpdateAllTotals();
	pDoc->UpdateIntrinsicTotals();
}

void CTableView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_idxTopRow >= pDoc->GetNoOfRows()) {
		m_idxTopRow= 0;
		SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
	
		UpdateWindow();
		m_pRowTotalsView->RedrawWindow();
	}
	
	long		i;
	long		idx;
	long		idxNewTopRow;
	long		idxExtrinsicRow = pDoc->GetIdxTopExtrinsicRow();
	long		cyScroll = 0;
	long		cy;
	long oldidx = m_idxTopRow;
	CRect	rScroll(0,m_nColumnBtnBarHeight + 1,m_cxClient,m_cyClient);
	
	if ( m_ActiveCell.m_bIsEditing )
		EndEdit();


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


	if(((int)nPos) >= pDoc->GetNoOfRows())
	{
		m_idxTopRow= 0;
		SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
	
		UpdateWindow();
		m_pRowTotalsView->RedrawWindow();
	}
	switch(nSBCode)
	{
		case SB_LINEDOWN: // scrolling forward through document
			if ( m_idxTopRow < (pDoc->GetUpperBoundRows()) )
			{
				// find the index of the next non-hidden row in the document
				if ( (idxNewTopRow = pDoc->GetIdxNextVisibleRow(m_idxTopRow)) == -1 )
					// no more non-hidden rows in document
					break;

				// if the next visible row is past the bottom of the view, then
				// just repaint all rows by invalidating the entire scroll rect
				i = GetLastVisibleRow();
				if ( idxNewTopRow > m_aRows[i]->m_idx )
				{
					InvalidateRect(rScroll,TRUE);
					m_pRowTotalsView->Refresh();
				}
				// otherwise, calc the amount to scroll the window
				else
				{
					i = 0;
					do // scroll past any hidden rows, etc
					{
						cyScroll -= m_aRows[i]->Height();
						i++;
					} while (i<m_aRows.GetSize() && m_aRows[i]->m_idx != idxNewTopRow );
					if(i == m_aRows.GetSize())
					{
						i = i;
					}
				}
				m_idxTopRow = idxNewTopRow;
			}
			break;

		case SB_LINEUP:	// scrolling backward through document
			if ( m_idxTopRow > 0 )
			{
				// find the index of the previous non-hidden row in the document
				if ( (idxNewTopRow = pDoc->GetIdxPreviousVisibleRow(m_idxTopRow)) == -1 )
					// no previous non-hidden row in document
					break;

				// first, scroll by the height of the new top row ...
				cyScroll += m_nRowHeight;

				// ... and then check whether hidden rows and/or the extrinsic bar
				// are in between the current top row and the new top row - if so,
				// then scroll by the required additional amounts:
				if ( m_idxTopRow - idxNewTopRow > 1 )	// ie, separated by hidden rows
					cyScroll += 1;						// width of block of hidden rows = 1

				if ( idxExtrinsicRow >= idxNewTopRow && idxExtrinsicRow < m_idxTopRow )
				{
					cyScroll += m_nExtrinsicBarHeight;
					if ( idxExtrinsicRow > idxNewTopRow + 1 )
						// hidden rows split by extrinsic bar - ie, 2 sets of hidden rows
						cyScroll += 1; // width of block of hidden rows = 1
				}

				m_idxTopRow = idxNewTopRow;
			}
			break;
		
		case SB_PAGEDOWN: // scrolling forward through document
			// find the index of the next non-hidden row in the document
			if ( (idx = pDoc->GetIdxNextVisibleRow(m_idxTopRow)) == -1 )
				// no more non-hidden rows in document, so can't scroll
				break;

			// first, find the element that contains the last visible row in the view
			i = m_aRows.GetUpperBound();
			while ( i >= 0 && i < m_aRows.GetSize())
			{
				if ( !m_aRows[i]->m_bIsHidden && !m_aRows[i]->m_bIsExtrinsicBar )
					break;
				i--;
			}

			// if the next visible row is past last visible row in view, then
			// just repaint all rows by invalidating the entire scroll rect
			if ( idx > m_aRows[i]->m_idx )
			{
				m_idxTopRow = idx;
				InvalidateRect(rScroll,TRUE);
				m_pRowTotalsView->Refresh();
				break;
			}

			// calc initial scroll amount ...
			cyScroll = -(m_aRows[i]->m_yTop - rScroll.top);
			idxNewTopRow = m_aRows[i]->m_idx;

			// ... if the last visible row in the view is partially past the bottom
			// of the view, then locate the last fully-visible row in the view and
			// adjust the scroll amount so as to scroll the fully-visible row to the
			// top of the view.  however, if the last fully-visible row is the current
			// top row, then scroll the last row (ie, the partially obscured row) to the top,
			// otherwise no scrolling would occur.
			if ( m_aRows[i]->m_yBottom > rScroll.bottom ) // partially obscured at bottom
			{
				i--;
				while ( i >= 0 && i < m_aRows.GetSize()) // find the last fully-visible row in the view
				{
					if ( !m_aRows[i]->m_bIsHidden && !m_aRows[i]->m_bIsExtrinsicBar )
						break;
					i--;
				}
				if (i >= 0 && i < m_aRows.GetSize() &&  m_aRows[i]->m_idx > m_idxTopRow )
				{
					cyScroll = -(m_aRows[i]->m_yTop - rScroll.top);
					idxNewTopRow = m_aRows[i]->m_idx;
				}
			}
			m_idxTopRow = idxNewTopRow;

			// if extrinsic, roll back so that extrinsic bar remains visible
			if ( cyScroll != 0 && m_idxTopRow == idxExtrinsicRow )
				cyScroll += m_nExtrinsicBarHeight;

			break;
		
		case SB_PAGEUP:	// scrolling backward through document
			// first find element containing the current top visible row in the view
			i = 0;
			while (i < m_aRows.GetSize() &&  m_aRows[i]->m_idx != m_idxTopRow )
				i++;

			idx = idxNewTopRow = m_idxTopRow;

			while ( idx > 0 )
			{
				// find the index of the previous non-hidden row in the document
				if ( (idx = pDoc->GetIdxPreviousVisibleRow(idxNewTopRow)) == -1 )
					// no previous non-hidden row in document
					break;

				// first, scroll by the height of the new row ...
				cy = m_nRowHeight;

				// ... and then check whether hidden rows and/or the extrinsic bar
				// are in between the current top row and the new row - if so,
				// then scroll by the required additional amounts:
				if ( idxNewTopRow - idx > 1 )	// ie, separated by hidden rows
					cy += 1;				// width of block of hidden rows = 1

				if ( idxExtrinsicRow >= idx && idxExtrinsicRow < idxNewTopRow )
				{
					cy += m_nExtrinsicBarHeight;
					if ( idxExtrinsicRow > idx + 1 )
						// hidden rows split by extrinsic bar - ie, 2 sets of hidden rows
						cy += 1; // width of block of hidden rows = 1
				}

				if (i >= 0 && i <m_aRows.GetSize() && ((cyScroll + cy) > (rScroll.bottom - m_aRows[i]->m_yBottom) &&
					 idxNewTopRow < m_idxTopRow ))
					break;

				cyScroll += cy;
				idxNewTopRow = idx;
			}

			m_idxTopRow = idxNewTopRow;

			break;
		
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			if ( nPos > (UINT)m_idxTopRow ) // scrolling forward through document
			{
				// try to find the first non-hidden row at nPos or after - if none found,
				// then try to find the previous non-hidden row from nPos
				if ( (idxNewTopRow = pDoc->GetIdxNextVisibleRow(nPos - 1)) == -1 )
					idxNewTopRow = pDoc->GetIdxPreviousVisibleRow(nPos);
				if ( idxNewTopRow == m_idxTopRow ) // ie, no visible rows after m_idxTopRow
					break;

				m_idxTopRow = idxNewTopRow;

				// calc amount to scroll
				i = GetLastVisibleRow();
				if ( idxNewTopRow > m_aRows[i]->m_idx )	// past bottom of view,
				{
					InvalidateRect(rScroll,TRUE);			// so just repaint all rows
					m_pRowTotalsView->Refresh();
					break;
				}
				else
				{
					i = 0;
					/****TODO: the following does not take into account hidden rows ****/
					do
					{
						cyScroll -= m_aRows[i]->Height();
						i++;
					} while (i<m_aRows.GetSize() && m_aRows[i]->m_idx != idxNewTopRow );
				}

				// if extrinsic, roll back so that extrinsic bar remains visible
				if ( cyScroll != 0 && m_aRows[i]->m_idx == idxExtrinsicRow )
					cyScroll += m_nExtrinsicBarHeight;
			}
			else if ( nPos < (UINT)m_idxTopRow ) // scrolling backward through document
			{
				// try to find the first non-hidden row at nPos or before - if none found,
				// then try to find the next non-hidden row after nPos
				if ( (idxNewTopRow = pDoc->GetIdxPreviousVisibleRow((long)nPos + 1)) == -1 )
					idxNewTopRow = pDoc->GetIdxNextVisibleRow((long)nPos);
				if ( idxNewTopRow == m_idxTopRow ) // ie, no visible rows before m_idxTopRow
					break;

				while ( m_idxTopRow > idxNewTopRow )
				{
					// find the index of the previous non-hidden row in the document
					idx = pDoc->GetIdxPreviousVisibleRow(m_idxTopRow);

					// first, scroll by the height of the new row ...
					cyScroll += m_nRowHeight;

					// ... and then check whether hidden rows and/or the extrinsic bar
					// are in between the current top row and the new row - if so,
					// then scroll by the required additional amounts:
					if ( m_idxTopRow - idx > 1 )	// ie, separated by hidden rows
						cyScroll += 1;				// width of block of hidden rows = 1

					if ( idxExtrinsicRow >= idx && idxExtrinsicRow < m_idxTopRow )
					{
						cyScroll += m_nExtrinsicBarHeight;
						if ( idxExtrinsicRow > idx + 1 )
							// hidden rows split by extrinsic bar - ie, 2 sets of hidden rows
							cyScroll += 1; // width of block of hidden rows = 1
					}

					m_idxTopRow = idx;

					if ( cyScroll > rScroll.Height() )	// more than window height to scroll
					{
						m_idxTopRow = idxNewTopRow;
						cyScroll = 0;					// don't scroll window,
						InvalidateRect(rScroll,TRUE);	// just repaint all rows
						m_pRowTotalsView->Refresh();
						break;
					}
				}
			}
			break;

		
		case SB_ENDSCROLL:
		default:
			break;		
	}

	if ( cyScroll != 0 )
	{
		ScrollWindow(0,cyScroll,rScroll,rScroll);
		m_pRowTotalsView->VScroll(cyScroll);
	}

	SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
	
	if(oldidx != m_idxTopRow)
	{
		UpdateWindow();
		m_pRowTotalsView->RedrawWindow();
		// RedrawWindow();
	}

	
}

void CTableView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (m_idxLeftColumn >= pDoc->GetNoOfColumns()) {
		m_idxLeftColumn = 0;
		SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);

		UpdateWindow();
		m_pColumnTotalsView->RedrawWindow();
	}
	
	long		i;
	long		idx;
	long		idxNewLeftColumn;
	long		idxExtrinsicColumn = pDoc->GetIdxLeftExtrinsicColumn();
	long		cxScroll = 0;
	long		cx;
	long idxold = m_idxLeftColumn;
	CRect	rScroll(m_nRowBtnBarWidth + 1,0,m_cxClient,m_cyClient);
	
	if ( m_ActiveCell.m_bIsEditing )
		EndEdit();

	if(((int)nPos) >= pDoc->GetNoOfColumns())
	{
		m_idxLeftColumn = 0;
		SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);

		UpdateWindow();
		m_pColumnTotalsView->RedrawWindow();
	}
	switch(nSBCode)
	{
		case SB_LINERIGHT: // scrolling forward through document
			if ( m_idxLeftColumn < (pDoc->GetUpperBoundColumns()) )
			{
				// find the index of the next non-hidden column in the document
				idxNewLeftColumn = pDoc->GetIdxNextVisibleColumn(m_idxLeftColumn);
				if ( idxNewLeftColumn == -1 )
					// no more non-hidden columns in document
					break;

				// calc amount to scroll
				i = GetLastVisibleColumn();
				if ( idxNewLeftColumn > m_aColumns[i]->m_idx )
				{
					InvalidateRect(rScroll,TRUE);
					//m_pColumnTotalsView->Refresh();
				}
				else
				{
					i = 0;
					do
					{
						cxScroll -= m_aColumns[i]->Width();
						i++;
					} while ( i < m_aColumns.GetSize() && m_aColumns[i]->m_idx != idxNewLeftColumn );
				}
				m_idxLeftColumn = idxNewLeftColumn;
			}
			break;

		case SB_LINELEFT:	// scrolling backward through document
			if ( m_idxLeftColumn > 0 )
			{
				// find the index of the previous non-hidden column in the document
				idxNewLeftColumn = pDoc->GetIdxPreviousVisibleColumn(m_idxLeftColumn);
				if ( idxNewLeftColumn == -1 )
					// no previous non-hidden column in document
					break;

				// first, scroll by the height of the new left column ...
				cxScroll += CalcViewColumnWidth(idxNewLeftColumn);

				// ... and then check whether hidden columns and/or the extrinsic bar
				// are in between the current left column and the new left column - if so,
				// then scroll by the required additional amounts:
				if ( m_idxLeftColumn - idxNewLeftColumn > 1 ) // separated by hidden columns
					cxScroll += 1;					// width of block of hidden columns = 1

				if ( idxExtrinsicColumn >= idxNewLeftColumn &&
					 idxExtrinsicColumn < m_idxLeftColumn )
				{
					cxScroll += m_nExtrinsicBarWidth;
					if ( idxExtrinsicColumn > idxNewLeftColumn + 1 )
						// hidden columns split by extrinsic bar - 2 sets of hidden columns
						cxScroll += 1; // width of block of hidden columns = 1
				}

				m_idxLeftColumn = idxNewLeftColumn;

				//m_pColumnTotalsView->Refresh();
			}
			break;
		
		case SB_PAGERIGHT: // scrolling forward through document
			// find the index of the next non-hidden column in the document
			if ( (idx = pDoc->GetIdxNextVisibleColumn(m_idxLeftColumn)) == -1 )
				// no more non-hidden columns in document, so can't scroll
				break;

			// first, find the element that contains the last visible column in the view
			i = m_aColumns.GetUpperBound();
			while ( i >= 0 )
			{
				if ( i < m_aColumns.GetSize() && !m_aColumns[i]->m_bIsHidden && !m_aColumns[i]->m_bIsExtrinsicBar )
					break;
				i--;
			}

			// if the next visible column is past last visible column in view, then
			// just repaint all columns by invalidating the entire scroll rect
			if ( idx > m_aColumns[i]->m_idx )
			{
				m_idxLeftColumn = idx;
				InvalidateRect(rScroll,TRUE);
				//m_pColumnTotalsView->Refresh();
				break;
			}

			// calc initial scroll amount ...
			cxScroll = -(m_aColumns[i]->m_xLeft - rScroll.left);
			idxNewLeftColumn = m_aColumns[i]->m_idx;

			// ... if the last visible column in the view is partially past the right edge
			// of the view, then locate the last fully-visible column in the view and
			// adjust the scroll amount so as to scroll the fully-visible column to the
			// left of the view.  however, if the last fully-visible column is the current
			// left column, then scroll the last column (ie, the partially obscured column)
			// to the left side, otherwise no scrolling would occur.
			if ( m_aColumns[i]->m_xRight > rScroll.right ) // partially obscured at right edge
			{
				i--;
				while ( i >= 0 ) // find the last fully-visible column in the view
				{
					if ( i < m_aColumns.GetSize() && !m_aColumns[i]->m_bIsHidden && !m_aColumns[i]->m_bIsExtrinsicBar )
						break;
					i--;
				}
				if ( m_aColumns[i]->m_idx > m_idxLeftColumn )
				{
					cxScroll = -(m_aColumns[i]->m_xLeft - rScroll.left);
					idxNewLeftColumn = m_aColumns[i]->m_idx;
				}
			}
			m_idxLeftColumn = idxNewLeftColumn;

			// if extrinsic, roll back so that extrinsic bar remains visible
			if ( cxScroll != 0 && m_idxLeftColumn == idxExtrinsicColumn )
				cxScroll += m_nExtrinsicBarWidth;

			break;
		
		case SB_PAGELEFT:	// scrolling backward through document
			// first find element containing the current leftmost visible column in the view
			i = 0;
			while ( i < m_aColumns.GetSize() && m_aColumns[i]->m_idx != m_idxLeftColumn )
				i++;

			idx = idxNewLeftColumn = m_idxLeftColumn;

			while ( idx > 0 )
			{
				// find the index of the previous non-hidden column in the document
				if ( (idx = pDoc->GetIdxPreviousVisibleColumn(idxNewLeftColumn)) == -1 )
					// no previous non-hidden column in document
					break;

				// first, scroll by the height of the new column ...
				cx = CalcViewColumnWidth(idx);

				// ... and then check whether hidden columns and/or the extrinsic bar
				// are in between the current left column and the new column - if so,
				// then scroll by the required additional amounts:
				if ( idxNewLeftColumn - idx > 1 )	// ie, separated by hidden columns
					cx += 1;				// width of block of hidden columns = 1

				if ( idxExtrinsicColumn >= idx && idxExtrinsicColumn < idxNewLeftColumn )
				{
					cx  += m_nExtrinsicBarWidth;
					if ( idxExtrinsicColumn > idx + 1 )
						// hidden columns split by extrinsic bar - 2 sets of hidden columns
						cx  += 1; // width of block of hidden columns = 1
				}

				if ( (cxScroll + cx) > (rScroll.right - m_aColumns[i]->m_xRight) &&
					 idxNewLeftColumn < m_idxLeftColumn )
					break;

				cxScroll += cx;
				idxNewLeftColumn = idx;
			}


			m_idxLeftColumn = idxNewLeftColumn;
			//m_pColumnTotalsView->Refresh();

			break;
		
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			if ( nPos > (UINT)m_idxLeftColumn ) // scrolling forward through document
			{
				// try to find the first non-hidden column at nPos or after - if none found,
				// then try to find the previous non-hidden column from nPos
				if ( (idxNewLeftColumn = pDoc->GetIdxNextVisibleColumn(nPos - 1)) == -1 )
					idxNewLeftColumn = pDoc->GetIdxPreviousVisibleColumn(nPos);
				if ( idxNewLeftColumn == m_idxLeftColumn )
					// ie, no visible columns after m_idxLeftColumn
					break;

				m_idxLeftColumn = idxNewLeftColumn;

				// calc amount to scroll
				i = GetLastVisibleColumn();
				if (i < m_aColumns.GetSize() &&  idxNewLeftColumn > m_aColumns[i]->m_idx )// past right edge of view,	
				{
					InvalidateRect(rScroll,TRUE);			// so just repaint all columns
					//m_pColumnTotalsView->Refresh();
					break;
				}
				else
				{
					i = 0;
					/****TODO: the following does not take into account hidden columns ****/
					do
					{
						cxScroll -= m_aColumns[i]->Width();
						i++;
					} while ( i < m_aColumns.GetSize() && m_aColumns[i]->m_idx != idxNewLeftColumn );
				}

				// if extrinsic, roll back so that extrinsic bar remains visible
				if ( cxScroll != 0 && m_aColumns[i]->m_idx == idxExtrinsicColumn )
					cxScroll += m_nExtrinsicBarWidth;
				//m_pColumnTotalsView->Refresh();
			}
			else if ( nPos < (UINT)m_idxLeftColumn ) // scrolling backward through document
			{
				// try to find the first non-hidden column at nPos or before - if none found,
				// then try to find the next non-hidden column after nPos
				if ( (idxNewLeftColumn = pDoc->GetIdxPreviousVisibleColumn(nPos + 1)) == -1 )
					idxNewLeftColumn = pDoc->GetIdxNextVisibleColumn(nPos);
				if ( idxNewLeftColumn == m_idxLeftColumn )
					// ie, no visible columns before m_idxLeftColumn
					break;

				while ( m_idxLeftColumn > idxNewLeftColumn )
				{
					// find the index of the previous non-hidden column in the document
					idx = pDoc->GetIdxPreviousVisibleColumn(m_idxLeftColumn);

					// first, scroll by the width of the new column ...
					cxScroll += CalcViewColumnWidth(idx);

					// ... and then check whether hidden columns and/or the extrinsic bar
					// are in between the current left column and the new column - if so,
					// then scroll by the required additional amounts:
					if ( m_idxLeftColumn - idx > 1 )	// ie, separated by hidden columns
						cxScroll += 1;				// width of block of hidden columns = 1

					if ( idxExtrinsicColumn >= idx && idxExtrinsicColumn < m_idxLeftColumn )
					{
						cxScroll += m_nExtrinsicBarWidth;
						if ( idxExtrinsicColumn > idx + 1 )
							// hidden columns split by extrinsic bar - 2 sets of hidden columns
 							cxScroll += 1; // width of block of hidden columns = 1
					}

					m_idxLeftColumn = idx;

					if ( cxScroll > rScroll.Width() )	// more than window width to scroll
					{
						m_idxLeftColumn = idxNewLeftColumn;
						cxScroll = 0;					// don't scroll window,
						InvalidateRect(rScroll,TRUE);	// just repaint all columns
						m_pColumnTotalsView->Refresh();
						break;
					}
				}
				//m_pColumnTotalsView->Refresh();
			}
			break;

		
		case SB_ENDSCROLL:
		default:
			break;		
	}

	
	if ( cxScroll != 0 )
	{
		ScrollWindow(cxScroll,0,rScroll,rScroll);
		m_pColumnTotalsView->HScroll(cxScroll);
	}



	SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);

	if(idxold != m_idxLeftColumn)
	{
		UpdateWindow();
		m_pColumnTotalsView->RedrawWindow();
	}
}

void CTableView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		idx;

	switch(nChar)
	{
		case VK_LEFT:
			idx = pDoc->GetIdxPreviousVisibleColumn(m_ActiveCell.m_idxColumn);
			if ( idx != -1 )
				SetActiveCellAt(idx,m_ActiveCell.m_idxRow);
			break;
			
		case VK_RIGHT:
			idx = pDoc->GetIdxNextVisibleColumn(m_ActiveCell.m_idxColumn);
			if ( idx != -1 )
				SetActiveCellAt(idx,m_ActiveCell.m_idxRow);
			break;
			
		case VK_UP:            
			idx = pDoc->GetIdxPreviousVisibleRow(m_ActiveCell.m_idxRow);
			if ( idx != -1 )
				SetActiveCellAt(m_ActiveCell.m_idxColumn,idx);
			break;
			
		case VK_DOWN:
			idx = pDoc->GetIdxNextVisibleRow(m_ActiveCell.m_idxRow);
			if ( idx != -1 )
				SetActiveCellAt(m_ActiveCell.m_idxColumn,idx);
			break;
			
		case VK_HOME:
			SendMessage(WM_HSCROLL,SB_THUMBPOSITION | 0 << 16,NULL);
			SendMessage(WM_VSCROLL,SB_THUMBPOSITION | 0 << 16,NULL);
			SetActiveCellAt(0,0);
			break;
			
		case VK_DELETE:
//*			OnEditDelete();
			break;
			
		case VK_PRIOR:
			SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
			break;
		          
		case VK_NEXT:
			SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
			break;

		case VK_RETURN:
			SetActiveCellAt(m_ActiveCell.m_idxColumn, m_ActiveCell.m_idxRow);
			
		case VK_ESCAPE:
			// TODO: Do I want to handle esc if not editing, eg, to clear current selection
			// in table
			break;

		default:
			break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTableView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case '-':
			StartEdit(/*cut contents =*/TRUE);  // cut contents
			m_ActiveCell.m_edit.SendMessage(WM_CHAR,nChar,nRepCnt | nFlags << 16);
			break;
			
		default:
			break;
	}
	
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CTableView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	POSITION pos = pDoc->GetFirstViewPosition();   

	int  pp;
	pp = pDoc->IsModified();
	
	long		idxColumn, idxRow;
	long		i;

	m_nMouseAction = none;	// clear any previous mouse operation
	m_ptClick = point;
	SetCapture();      		// capture the mouse until button up.

	// if the mouse button down had been inside the edit box of the active cell, then
	// the message would have gone to the edit box's message proc and this view would
	// not have got this WM_LBUTTONDOWN message - ie, the user has clicked outside the
	// active cell and therefore the editing session should be ended
	if ( m_ActiveCell.m_bIsEditing )
	{
		EndEdit();
		
	}

	if ( m_rExtrinsicColumnBar.PtInRect(point) )
	{
		//m_nMouseAction = dragExtrinsicColumnBar;
		//SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_EW));
		//DrawMoveColumnLine(m_rExtrinsicColumnBar.left);
	}
	else if ( m_rExtrinsicRowBar.PtInRect(point) )
	{
		//m_nMouseAction = dragExtrinsicRowBar;
		//SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_NS));
		//DrawMoveRowLine(m_rExtrinsicRowBar.top);
	}
	else if ( m_rGrid.PtInRect(point) )
	{
		m_pSelectedColumns->RemoveAll();
		m_pSelectedRows->RemoveAll();

		CPatnDoc* pDoc = (CPatnDoc*) GetDocument();
		POSITION pos = pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = pDoc->GetNextView(pos);   
			if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
				//pView->IsKindOf(RUNTIME_CLASS(CScat)) ||
			pView->IsKindOf(RUNTIME_CLASS(C3DXView)))
				pView->RedrawWindow();
		}   

		idxColumn = m_aColumns[GetColumn(point)]->m_idx;
		idxRow = m_aRows[GetRow(point)]->m_idx;
	
		if ( idxColumn == m_ActiveCell.m_idxColumn && idxRow == m_ActiveCell.m_idxRow )
		// clicked on current active cell so start edit session
		{
			StartEdit(/*cut contents =*/FALSE);
			// send left mouse button click to edit so as to position caret
			ClientToScreen(&point);
			m_ActiveCell.m_edit.ScreenToClient(&point);
			m_ActiveCell.m_edit.SendMessage(WM_LBUTTONDOWN,nFlags,point.x | point.y << 16);
			m_ActiveCell.m_edit.SendMessage(WM_LBUTTONUP,nFlags,point.x | point.y << 16);
			m_ActiveCell.m_edit.SetSel(-1,0,FALSE);
		}
		else	// new cell clicked so make it the active cell (without starting edit)
		{
			SetActiveCellAt(idxColumn, idxRow, FALSE);
			SetCursor(LoadCursor(NULL,IDC_IBEAM));
		}
	}
	else if ( (m_nMouseAction = HitTestColumnBtnBar(point, i)) != none )
	{
		switch ( m_nMouseAction )
		{
			case selectColumn:
				m_pSelectedRows->RemoveAll();

				if ( !(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) )
				{
					m_pSelectedColumns->StartActiveSelection(m_aColumns[i]->m_idx);
				}
				else if ( nFlags & MK_SHIFT && !(nFlags & MK_CONTROL) )
				{
					m_pSelectedColumns->UpdateActiveSelection(m_aColumns[i]->m_idx);
				}
				else if ( nFlags & MK_CONTROL && !(nFlags & MK_SHIFT) )
				{
					m_pSelectedColumns->StartActiveSelection(m_aColumns[i]->m_idx,FALSE);
				}
				else if ( (nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) )
				{
					m_pSelectedColumns->UpdateActiveSelection(m_aColumns[i]->m_idx,FALSE);
				}

				while (pos != NULL)   
				{
					CView* pView = pDoc->GetNextView(pos);   
					if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
						pView->IsKindOf(RUNTIME_CLASS(C3DXView))) //||
						//pView->IsKindOf(RUNTIME_CLASS(CScat)))
						pView->RedrawWindow();
				}   

				break;

			case resizeColumn:
				m_nStartX = point.x;
				m_iResizeColumn = i;
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
				DrawResizeColumnLine(m_aColumns[i]->m_xRight);
				break;

			case dragColumn:
				m_idxMoveColumn = m_aColumns[i]->m_idx;
				SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_EW));
				DrawMoveColumnLine(m_aColumns[i]->m_xLeft);
				break;

			case unhideColumn:
				m_nStartX = point.x;
				m_iResizeColumn = i;
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_COLUMN));
				DrawResizeColumnLine(m_aColumns[i]->m_xRight);
				break;

			case dragExtrinsicColumnBar:
				/**** point on extrinsic bar is handled earlier ****/
				break;
		}
	}
	else if ( (m_nMouseAction = HitTestRowBtnBar(point, i)) != none )
	{
		switch ( m_nMouseAction )
		{
			case selectRow:
				m_pSelectedColumns->RemoveAll();

				if ( !(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL) )
				{

					m_pSelectedRows->StartActiveSelection(m_aRows[i]->m_idx);
				}
				else if ( nFlags & MK_SHIFT && !(nFlags & MK_CONTROL) )
				{
					m_pSelectedRows->UpdateActiveSelection(m_aRows[i]->m_idx);
				}
				else if ( nFlags & MK_CONTROL && !(nFlags & MK_SHIFT) )
				{
					m_pSelectedRows->StartActiveSelection(m_aRows[i]->m_idx,FALSE);
				}
				else if ( (nFlags & MK_SHIFT) && (nFlags & MK_CONTROL) )
				{
					m_pSelectedRows->UpdateActiveSelection(m_aRows[i]->m_idx,FALSE);
				}
  
				break;

			case resizeRow:
				// resizing of rows is not supported by views in this version of CTableView
				break;

			case dragRow:
				m_idxMoveRow = m_aRows[i]->m_idx;
				SetCursor(AfxGetApp()->LoadCursor(IDC_DRAGGING_NS));
				DrawMoveRowLine(m_aRows[i]->m_yTop);
				break;

			case unhideRow:
				m_nStartY = point.y;
				m_iResizeRow = i;
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_ROW));
				DrawResizeRowLine(m_aRows[i]->m_yBottom);
				break;

			case dragExtrinsicRowBar:
				/**** point on extrinsic bar is handled earlier ****/
				break;
		}
	}
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
			pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)) ||
			pView->IsKindOf(RUNTIME_CLASS(C3DXView)))// ||
			//pView->IsKindOf(RUNTIME_CLASS(CScat)))
			pView->RedrawWindow();
	} 
	

	

	CView::OnLButtonDown(nFlags, point);
}

void CTableView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	long		nMouseAction; // flags what sort of mouse action is possible based on point
	CRect	rActiveCell;
	GetActiveCellRect(rActiveCell);
	long		nStartTime;
	long		idx;
	long		i;
	long		x, y;
	MSG		msg;

	
	if (GetCapture() == this)	// left mouse button was pressed over this view
    {
    	switch (m_nMouseAction)
		{
			case selectColumn:
    			if ( point.x > m_cxClient )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),
							WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_xGrid >= m_cxClient )
						{
	    					OnHScroll(SB_LINERIGHT,0,NULL);
							if ( m_aColumns[m_aColumns.GetUpperBound()]->m_bIsExtrinsicBar )
								idx = pDoc->GetIdxLeftExtrinsicColumn() - 1;
							else
								idx = m_aColumns[m_aColumns.GetUpperBound()]->m_idx;
    						m_pSelectedColumns->UpdateActiveSelection(idx,FALSE);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime*1.0) < 150 );
						}
    				}
    			}
    			else if ( point.x < m_rGrid.left )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),
							WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_idxLeftColumn > 0 )
						{
	    					OnHScroll(SB_LINELEFT,0,NULL);
	    					m_pSelectedColumns->UpdateActiveSelection(m_idxLeftColumn,FALSE);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime*1.0) < 150 );
						}
    				}
    			}
    			else
    			{
					if ( (i = GetColumn(point)) == -1 ) // not on column
						break;

					// don't do anything if still on same idx
					if ( m_pSelectedColumns->GetIdxActiveEnd() == m_aColumns[i]->m_idx )
						break;

					m_pSelectedColumns->UpdateActiveSelection(m_aColumns[i]->m_idx,FALSE);
				}
				break;

			case selectRow:
    			if ( point.y > m_cyClient )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),
							WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_yGrid >= m_cyClient )
						{
	    					OnVScroll(SB_LINEDOWN,0,NULL);
							if ( m_aRows[m_aRows.GetUpperBound()]->m_bIsExtrinsicBar )
								idx = pDoc->GetIdxTopExtrinsicRow() - 1;
							else
								idx = m_aRows[m_aRows.GetUpperBound()]->m_idx;
    						m_pSelectedRows->UpdateActiveSelection(idx,FALSE);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime*1.0) < 150 );
						}
    				}
    			}
    			else if ( point.y < m_rGrid.top )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),
							WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_idxTopRow > 0 )
						{
	    					OnVScroll(SB_LINEUP,0,NULL);
	    					m_pSelectedRows->UpdateActiveSelection(m_idxTopRow,FALSE);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime*1.0) < 150 );
						}
    				}
    			}
    			else
    			{
					if ( (i = GetRow(point)) == -1 ) // not on row
						break;

					// don't do anything if still on same idx
					if ( m_pSelectedRows->GetIdxActiveEnd() == m_aRows[i]->m_idx )
						break;

					m_pSelectedRows->UpdateActiveSelection(m_aRows[i]->m_idx,FALSE);
				}
				break;

			case resizeColumn:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
    			ClearResizeColumnLine();
				DrawResizeColumnLine(point.x);
				break;

			case resizeRow:
				// resizing of rows is not supported by views in this version of CTableView
				break;

			case dragExtrinsicColumnBar:
			case dragColumn:
    			if ( point.x > m_cxClient )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),WM_MOUSEFIRST,WM_MOUSELAST,
							             PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_xGrid >= m_cxClient )
						{
	    					ClearMoveColumnLine();
	    					OnHScroll(SB_LINERIGHT,0,NULL);
	    					x = min(m_cxClient - 1,m_xGrid);
	    					DrawMoveColumnLine(x);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime*1.0) < 150 );
						}
    				}
    			}
    			else if ( point.x < m_rGrid.left )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_idxLeftColumn > 0 )
						{
	    					ClearMoveColumnLine();
	    					OnHScroll(SB_LINELEFT,0,NULL);
	    					x = max(1,m_rGrid.left);
	    					DrawMoveColumnLine(x);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime) < 100 );
						}
    				}
    			}
    			else
    			{
    				if (  point.x >= m_xGrid )
    					x = m_xGrid;
    				else
					{
 						i = GetColumn(point);
		    			x = m_aColumns[i]->m_xLeft;
					}
		    			
		    		if ( x != m_nOldX )
		    		{
		    			ClearMoveColumnLine();
		    			DrawMoveColumnLine(x);
		    		}
    			}
    			break;

			case dragExtrinsicRowBar:
			case dragRow:
				if ( point.y > m_cyClient )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),WM_MOUSEFIRST,WM_MOUSELAST,
							             PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_yGrid >= m_cyClient )
						{
	    					ClearMoveRowLine();
	    					OnVScroll(SB_LINEDOWN,0,NULL);
	    					y = min(m_cyClient - 1,m_yGrid);
	    					DrawMoveRowLine(y);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime) < 150 );
						}
    				}
    			}
    			else if ( point.y < m_rGrid.top )
    			{
    				while ( TRUE )
    				{
    					if ( PeekMessage(&msg, GetSafeHwnd(),WM_MOUSEFIRST,WM_MOUSELAST,PM_NOYIELD | PM_NOREMOVE) )
    						if ( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP )
    							break;

						if ( m_idxTopRow > 0 )
						{
	    					ClearMoveRowLine();
	    					OnVScroll(SB_LINEUP,0,NULL);
	    					y = max(1,m_rGrid.top);
	    					DrawMoveRowLine(y);
	    					nStartTime = ::GetTickCount();
	    					while ( abs((double)GetTickCount() - nStartTime) < 100 );
						}
    				}
    			}
    			else
    			{
    				if (  point.y >= m_yGrid )
    					y = m_yGrid;
    				else
					{
 						i = GetRow(point);
		    			y = m_aRows[i]->m_yTop;
					}
		    			
		    		if ( y != m_nOldY )
		    		{
		    			ClearMoveRowLine();
		    			DrawMoveRowLine(y);
		    		}
    			}
    			break;

			case unhideColumn:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_COLUMN));
    			ClearResizeColumnLine();
				DrawResizeColumnLine(point.x);
				break;

			case unhideRow:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_ROW));
    			ClearResizeRowLine();
				DrawResizeRowLine(point.y);
				break;
		}
	}
	else if ( rActiveCell.PtInRect(point) )
	{
		SetCursor(LoadCursor(NULL,IDC_IBEAM));
	}
	else if ( m_rExtrinsicColumnBar.PtInRect(point) )
	{
		//SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_EW));
	}
	else if ( m_rExtrinsicRowBar.PtInRect(point) )
	{
		//SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_NS));
	}
	else if ( (nMouseAction = HitTestColumnBtnBar(point, i)) != none )
	{
		switch ( nMouseAction )
		{
			case selectColumn:
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				break;

			case resizeColumn:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_COLUMN));
				break;

			case dragColumn:
				SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_EW));
				break;

			case unhideColumn:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_COLUMN));
				break;

			case dragExtrinsicColumnBar:
				SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_EW));
				break;

			default:
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				break;
		}
	}
	else if ( (nMouseAction = HitTestRowBtnBar(point, i)) != none )
	{
		switch ( nMouseAction )
		{
			case selectRow:
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				break;

			case resizeRow:
				// resizing of rows is not supported by views in this version of CTableView
				break;

			case dragRow:
				SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_NS));
				break;

			case unhideRow:
				SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_HIDDEN_ROW));
				break;

			case dragExtrinsicRowBar:
				SetCursor(AfxGetApp()->LoadCursor(IDC_CAN_DRAG_NS));
				break;

			default:
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				break;
		}
	}
	else
		SetCursor(LoadCursor(NULL,IDC_ARROW));

	CView::OnMouseMove(nFlags, point);
}

void CTableView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double	nWidth;		// new column width in pixels
	CRect	rInvalid(0,0,0,0);	// the rect that needs to be repainted
	long		idx;
	CUpdateHint UpdateHint;
	long		i;
	long idxFrom, idxTo, nCount;
	
	if (GetCapture() == this)
    {
		BeginWaitCursor();

    	switch (m_nMouseAction)
		{
			case selectColumn:
				// nothing needs to be done - handled by OnLButtonDown() and OnMouseMove()
				break;

			case selectRow:
				// nothing needs to be done - handled by OnLButtonDown() and OnMouseMove()
				break;

			case dragColumn:
    			ClearMoveColumnLine();

 				if ( point == m_ptClick ) // just clicked on column button
				{
					m_pSelectedColumns->RemoveAll();
					break;
				}

				// from where are we moving them?
				idxFrom = m_pSelectedColumns->GetMinValue();

				// how many are we moving?
				nCount = m_pSelectedColumns->GetCount();

				// to where are we moving them?
    			if ( point.x <= m_rGrid.left ) // left of 1st column
				{
					idxTo = m_idxLeftColumn;
					rInvalid.left = m_rGrid.left;
				}
				else if ( point.x >= m_xGrid ) // right of last column
				{
					idxTo = pDoc->GetNoOfColumns();
					rInvalid.left = m_xGrid;
				}
				else // on one of the columns
				{
					i = GetColumn(point);

					// if the selection has been dragged onto a hidden column block, then
					// make the first idx in the block the destination
					if ( m_aColumns[i]->m_bIsHidden )
						idx = m_aColumns[i]->m_idxStart;
					else // on a visible column - make it the destination
						idxTo = m_aColumns[i]->m_idx;

					rInvalid.left = m_aColumns[i]->m_xLeft;
				}

				// if attempting to move selection onto itself, then don't do anything
    			if ( idxTo < (idxFrom + nCount + 1) && idxTo >= idxFrom )
					break;

				// determine final extents of rect to invalidate
				rInvalid.top = 0;
				rInvalid.right = m_cxClient;
				rInvalid.bottom = m_cyClient;

				// if the selection has been moved to the right, then check whether any
				// of the source columns are still in view (may have scrolled out of view)
				// and, if so, make the leftmost source column in view the rInvalid.left
				if ( idxFrom < idxTo )
				{
					for ( idx = idxFrom; idx <= m_pSelectedColumns->GetMaxValue(); idx++ )
					{
						if ( (i = GetColumn(idx)) != -1 )
						{
							rInvalid.left = m_aColumns[i]->m_xLeft;
							break;
						}
					}
				}

				// adjust m_idxLeftColumn
				if ( m_idxLeftColumn > (idxFrom + nCount - 1) && m_idxLeftColumn < idxTo )
				{
					m_idxLeftColumn -= nCount;
				}
				else if ( m_idxLeftColumn >= idxFrom &&
						  m_idxLeftColumn <= (idxFrom + nCount - 1) )
				{
					m_idxLeftColumn -= (m_idxLeftColumn - idxFrom);
				}

				UpdateHint.m_idxOld = idxFrom;
				UpdateHint.m_idxNew = idxTo;
				UpdateHint.m_nCount = nCount;

				// move the columns within the document
				pDoc->MoveColumn(idxFrom, idxTo, nCount);

				// repaint the invalidated area of the view
				InvalidateRect(rInvalid);
				UpdateWindow();

				// move the selection highlight with the columns in the view
				m_pSelectedColumns->RemoveAll();
				if ( idxTo > idxFrom) { idxTo -= nCount; }
				m_pSelectedColumns->StartActiveSelection(idxTo);
				m_pSelectedColumns->UpdateActiveSelection(idxTo + nCount - 1);

				pDoc->UpdateAllViews(this, CUpdateHint::moveColumn, &UpdateHint);
				pDoc->SetModifiedFlag(TRUE);
				break;

			case dragRow:
    			ClearMoveRowLine();

				if ( point == m_ptClick ) // just clicked on column button
				{
					m_pSelectedRows->RemoveAll();
					break;
				}

				// from where are we moving them?
				idxFrom = m_pSelectedRows->GetMinValue();

				// how many are we moving?
				nCount = m_pSelectedRows->GetCount();

				// to where are we moving them?
    			if ( point.y <= m_rGrid.top ) // above 1st row
				{
					idxTo = m_idxTopRow;
					rInvalid.top = m_rGrid.top;
				}
				else if ( point.y >= m_yGrid ) // below last row
				{
					idxTo = pDoc->GetNoOfRows();
					rInvalid.top = m_yGrid;
				}
				else // on one of the rows
				{
					i = GetRow(point);

					// if the selection has been dragged onto a hidden row block, then
					// make the first idx in the block the destination
					if ( m_aRows[i]->m_bIsHidden )
						idx = m_aRows[i]->m_idxStart;
					else // on a visible row - make it the destination
						idxTo = m_aRows[i]->m_idx;

					rInvalid.top = m_aRows[i]->m_yTop;
				}

				// if attempting to move selection onto itself, then don't do anything
    			if ( idxTo < (idxFrom + nCount + 1) && idxTo >= idxFrom )
					break;

				// determine final extents of rect to invalidate
				rInvalid.left = 0;
				rInvalid.right = m_cxClient;
				rInvalid.bottom = m_cyClient;

				// if the selection has been moved downwards, then check whether any
				// of the source rows are still in view (may have scrolled out of view)
				// and, if so, make the topmost source row in view the rInvalid.top
				if ( idxFrom < idxTo )
				{
					for ( idx = idxFrom; idx <= m_pSelectedRows->GetMaxValue(); idx++ )
					{
						if ( (i = GetRow(idx)) != -1 )
						{
							rInvalid.top = m_aRows[i]->m_yTop;
							break;
						}
					}
				}

				// adjust m_idxTopRow
				if ( m_idxTopRow > (idxFrom + nCount - 1) && m_idxTopRow < idxTo )
				{
					m_idxTopRow -= nCount;
				}
				else if ( m_idxTopRow >= idxFrom &&
						  m_idxTopRow <= (idxFrom + nCount - 1) )
				{
					m_idxTopRow -= (m_idxTopRow - idxFrom);
				}

				UpdateHint.m_idxOld = idxFrom;
				UpdateHint.m_idxNew = idxTo;
				UpdateHint.m_nCount = nCount;

				// move the rows within the document
				pDoc->MoveRow(idxFrom, idxTo, nCount);

				// repaint the invalidated area of the view
				InvalidateRect(rInvalid);
				UpdateWindow();

				// move the selection highlight with the rows in the view
				m_pSelectedRows->RemoveAll();
				if ( idxTo > idxFrom) { idxTo -= nCount; }
				m_pSelectedRows->StartActiveSelection(idxTo);
				m_pSelectedRows->UpdateActiveSelection(idxTo + nCount - 1);

				pDoc->UpdateAllViews(this, CUpdateHint::moveRow, &UpdateHint);
				pDoc->SetModifiedFlag(TRUE);
				break;

			case resizeColumn:
			case unhideColumn:
    			ClearResizeColumnLine();

				// set the rect to be invalidated
    			rInvalid.left = m_aColumns[m_iResizeColumn]->m_xLeft;
    			rInvalid.top = 0;
    			rInvalid.right = m_cxClient;
    			rInvalid.bottom = m_cyClient;
    			
				nWidth = CalcDocColumnWidth(m_aColumns[m_iResizeColumn]->Width() +
											(point.x - m_nStartX));
				if(pDoc->m_pIsRowLabel)
				{
					m_nRowBtnBarWidth = point.x;
					pDoc->m_pIsRowLabel = 0;
					RedrawWindow();
				}else
				// if the column is part of a selection, set same width for all selected cols
				if ( m_pSelectedColumns->IsSelected(m_aColumns[m_iResizeColumn]->m_idx) )
				{
					UpdateHint.m_pSelection = m_pSelectedColumns;
					idx = m_pSelectedColumns->GetFirst();
					while ( idx != -1 )
					{
						if ( nWidth <= 0.0 )
							pDoc->HideColumn(idx);
						else
						{
							pDoc->SetColumnWidth(idx,nWidth);
							pDoc->UnhideColumn(idx);
						}

						if ( (i = GetColumn(idx)) != -1 ) // any need to update view?
							rInvalid.left = min(rInvalid.left,m_aColumns[i]->m_xLeft);

						idx = m_pSelectedColumns->GetNext();
					}
				}
				else // not part of selection, so just set new width for column being unhidden
				{
					idx = m_aColumns[m_iResizeColumn]->m_idx;
					UpdateHint.m_idxOld = idx;

					if ( nWidth <= 0.0 )
						pDoc->HideColumn(idx);
					else
					{
						pDoc->SetColumnWidth(idx,nWidth);
						pDoc->UnhideColumn(idx);
					}
				}

				// repaint the affected area
    			InvalidateRect(&rInvalid,TRUE);
    			UpdateWindow();
				GetDocument()->UpdateAllViews(this,CUpdateHint::resizeColumn,&UpdateHint);
				GetDocument()->SetModifiedFlag(TRUE);
				break;

			case resizeRow:
				// resizing of rows is not supported by views in this version of CTableView
				break;

			case unhideRow:
    			ClearResizeRowLine();

				// set the rect to be invalidated
    			rInvalid.top = m_aRows[m_iResizeRow]->m_yTop;
    			rInvalid.left = 0;
    			rInvalid.right = m_cxClient;
    			rInvalid.bottom = m_cyClient;
    			
				// if the row is part of a selection, unhide all selected cols
				if ( m_pSelectedRows->IsSelected(m_aRows[m_iResizeRow]->m_idx) )
				{
					UpdateHint.m_pSelection = m_pSelectedRows;
					idx = m_pSelectedRows->GetFirst();
					while ( idx != -1 )
					{
						pDoc->UnhideRow(idx);
						if ( (i = GetRow(idx)) != -1 ) // any need to update view?
							rInvalid.top = min(rInvalid.left,m_aRows[i]->m_yTop);

						idx = m_pSelectedRows->GetNext();
					}
				}
				else // not part of selection, so just unhide row
				{
					pDoc->UnhideRow(m_aRows[m_iResizeRow]->m_idx);
					UpdateHint.m_idxOld = m_aRows[m_iResizeRow]->m_idx;
				}

				// repaint the affected area
    			InvalidateRect(&rInvalid,TRUE);
    			UpdateWindow();
				GetDocument()->UpdateAllViews(this,CUpdateHint::unhideRow,&UpdateHint);
				GetDocument()->SetModifiedFlag(TRUE);
				break;

			case dragExtrinsicColumnBar:
    			ClearMoveColumnLine();
				UpdateHint.m_idxOld = pDoc->GetIdxLeftExtrinsicColumn();

    			if ( point.x <= m_rGrid.left ) // left of 1st column
				{
					pDoc->MakeAllColumnsExtrinsic();
					rInvalid.left = m_rGrid.left;
				}
				else if ( point.x >= m_xGrid ) // right of last column
				{
					pDoc->MakeAllColumnsIntrinsic();
					rInvalid.left = m_xGrid;
				}
				else // on one of the columns
				{
					i = GetColumn(point);

					// if the bar has been dragged onto of itself, then do nothing
					if ( m_aColumns[i]->m_bIsExtrinsicBar )
						break;

					// if the bar has been dragged onto a hidden column block, then	make
					// the first idx in the block the left extrinsic column
					if ( m_aColumns[i]->m_bIsHidden )
						pDoc->SetIdxLeftExtrinsicColumn(m_aColumns[i]->m_idxStart);
					else // on a visible column - make it the left extrinsic column
						pDoc->SetIdxLeftExtrinsicColumn(m_aColumns[i]->m_idx);

					rInvalid.left = m_aColumns[i]->m_xLeft;
				}

				if ( !m_rExtrinsicColumnBar.IsRectEmpty() ) // previous pos of bar in view?
				{
					rInvalid.left = min(rInvalid.left, m_rExtrinsicColumnBar.left);
				}
				rInvalid.top = 0;
				rInvalid.right = m_cxClient;
				rInvalid.bottom = m_cyClient;

	    		InvalidateRect(&rInvalid);
	    		UpdateWindow();
				UpdateHint.m_idxNew = pDoc->GetIdxLeftExtrinsicColumn();
				if ( UpdateHint.m_idxOld > pDoc->GetIdxLeftExtrinsicColumn() )
					GetDocument()->UpdateAllViews(this,CUpdateHint::makeColumnExtrinsic,
											  &UpdateHint);
				else
					GetDocument()->UpdateAllViews(this,CUpdateHint::makeColumnIntrinsic,
											  &UpdateHint);
				GetDocument()->SetModifiedFlag(TRUE);
				break;
			
			case dragExtrinsicRowBar:
    			ClearMoveRowLine();
				UpdateHint.m_idxOld = pDoc->GetIdxTopExtrinsicRow();

    			if ( point.y <= m_rGrid.top ) // above 1st row
				{
					pDoc->MakeAllRowsExtrinsic();
					rInvalid.top = m_rGrid.top;
				}
				else if ( point.y >= m_yGrid ) // below last row
				{
					pDoc->MakeAllRowsIntrinsic();
					rInvalid.top = m_yGrid;
				}
				else // on one of the rows
				{
					i = GetRow(point);

					// if the bar has been dragged onto of itself, then do nothing
					if ( m_aRows[i]->m_bIsExtrinsicBar )
						break;

					// if the bar has been dragged onto a hidden row block, then make
					// the first idx in the block the top extrinsic row
					if ( m_aRows[i]->m_bIsHidden )
						pDoc->SetIdxTopExtrinsicRow(m_aRows[i]->m_idxStart);
					else // on a visible row - make it the top extrinsic row
						pDoc->SetIdxTopExtrinsicRow(m_aRows[i]->m_idx);

					rInvalid.top = m_aRows[i]->m_yTop;
				}

				if ( !m_rExtrinsicRowBar.IsRectEmpty() ) // previous pos of bar in view?
				{
					rInvalid.top = min(rInvalid.top, m_rExtrinsicRowBar.top);
				}
				rInvalid.left = 0;
				rInvalid.right = m_cxClient;
				rInvalid.bottom = m_cyClient;

	    		InvalidateRect(&rInvalid);
	    		UpdateWindow();
				if ( UpdateHint.m_idxOld > pDoc->GetIdxTopExtrinsicRow() )
					GetDocument()->UpdateAllViews(this,CUpdateHint::makeRowExtrinsic,
											  &UpdateHint);
				else
					GetDocument()->UpdateAllViews(this,CUpdateHint::makeRowIntrinsic,
											  &UpdateHint);
				GetDocument()->SetModifiedFlag(TRUE);
				break;
		}
		m_nMouseAction = none;	// whatever mouse action was in progress is now finished
		ReleaseCapture();   	// release the mouse capture established at OnLButtonDown()

		EndWaitCursor();
	}

	CView::OnLButtonUp(nFlags, point);
	//do status bar;
	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	CString statbartext;
	statbartext.Format("number of objects %d, variables %d",GetDocument()->GetNoOfRows(),GetDocument()->GetNoOfColumns());
	frm->m_wndStatusBar.SetPaneText(1,statbartext);
}

void CTableView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPatnDoc* pDoc = GetDocument();
	pDoc->OnGo();

	
	CView::OnLButtonDblClk(nFlags, point);
}

long CTableView::HitTestColumnBtnBar(CPoint point, long& i)
{
	CPatnDoc* pDoc = GetDocument();
	long		cx; // used to test for distance of point from btn re-sizing edge
	long		idxUpper = m_aColumns.GetUpperBound();

	pDoc->m_pIsRowLabel = 0;

	if ( idxUpper == -1 )	// no columns (empty table)
		return none;
	
	if ( !m_rColumnBtnBar.PtInRect(point) )
		return none;

	i = GetColumn(point);

	if ( m_aColumns[i]->m_bIsHidden )
	{
		// if the point is on a hidden column block, then use the column to the left
		// of the hidden column block to do the hit-testing.  however, if there are
		// no columns to the left of the hidden column block or if the column to the
		// left of the hidden column block is the extrinsic column bar, then unhiding
		if ( i == 0 || m_aColumns[i - 1]->m_bIsExtrinsicBar )
		{
			return unhideColumn;
		}

		i--;
	}

	cx = m_aColumns[i]->Width() >> 2; // divide by 4
	cx = min(cx,6);

	// check whether point is at right side of btn near resizing edge
	if ( (m_aColumns[i]->m_xRight - point.x)-4 < cx )
		return resizeColumn;

	// not at right side of btn so check whether point is at the left side of btn near
	// resizing edge.  if so, then if the column to the left is the extrinsic column bar,
	// no resizing can take place.  however, if the column on the left is a hidden column
	// block, then, depending on distance of point from edge of btn, either unhiding the
	// hidden column or resizing column to left of hidden column block (as long as the
	// column to the left of the hidden column block is not the extrinsic column bar).
	if ( i > 0 && (point.x - m_aColumns[i]->m_xLeft + 1) < cx )
	{
		if ( m_aColumns[i - 1]->m_bIsExtrinsicBar )
			return selectColumn;

		if ( m_aColumns[i - 1]->m_bIsHidden )
		{
			if ( i > 1 && !m_aColumns[i - 2]->m_bIsExtrinsicBar &&
				 (point.x - m_aColumns[i]->m_xLeft + 1) < cx/2 )
			{
				i -= 2;
				return resizeColumn;
			}
			i--;
			return unhideColumn;
		}
		else // resizing adjacent column on left
		{
			i--;
			return resizeColumn;
		}
	}
	else if ( m_pSelectedColumns->IsSelected(m_aColumns[i]->m_idx) &&
			  m_pSelectedColumns->IsContiguous() )
	{
		return dragColumn;
	}else if(m_nRowBtnBarWidth+10 > point.x)
	{// check if resizing row labels column
		pDoc->m_pIsRowLabel = 1;
		return resizeColumn;
	}else // nothing else, so must be selecting button
		return selectColumn;
}

long CTableView::HitTestRowBtnBar(CPoint point, long& i)
{
	long		cy; // used to test for distance of point from btn re-sizing edge
	long		idxUpper = m_aRows.GetUpperBound();

	if ( idxUpper == -1 )	// no rows (empty table)
		return none;

	if ( !m_rRowBtnBar.PtInRect(point) )
		return none;

	i = GetRow(point);

	if ( m_aRows[i]->m_bIsHidden )
	{
		return unhideRow;
	}

	cy = m_aRows[i]->Height() >> 2; // divide by 4
	cy = min(cy,4);

	if ( i > 0 && (point.y - m_aRows[i]->m_yTop + 1) < cy &&
		 m_aRows[i - 1]->m_bIsHidden )
	{
			i--;
			return unhideRow;
	}

	
	if ( m_pSelectedRows->IsSelected(m_aRows[i]->m_idx) &&
			  m_pSelectedRows->IsContiguous() )
	{
		return dragRow;
	}
	else // nothing else, so must be selecting button
		return selectRow;
}

void CTableView::DrawResizeColumnLine(long x)
{
	CClientDC dc(this);
	CPen* pOldPen;
	CPen pen;
	long nOldROP2;
	long nOldBkMode;
	
	if ( ! pen.CreatePen(PS_DOT,0,RGB(0,0,0)) )
		return;
	if ( (pOldPen = dc.SelectObject(&pen)) == NULL )
		return;
	
	nOldROP2 = dc.SetROP2(R2_NOT);
	nOldBkMode = dc.SetBkMode(TRANSPARENT);
	
	dc.MoveTo(x,0);
	dc.LineTo(x,m_rGrid.bottom);
	m_nOldX = x;

	dc.SetROP2(nOldROP2);
	dc.SetBkMode(nOldBkMode);	
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CTableView::ClearResizeColumnLine()
{
	DrawResizeColumnLine(m_nOldX);
}

void CTableView::DrawResizeRowLine(long y)
{
	CClientDC dc(this);
	CPen* pOldPen;
	CPen pen;
	long nOldROP2;
	long nOldBkMode;
	
	if ( ! pen.CreatePen(PS_DOT,0,RGB(0,0,0)) )
		return;
	if ( (pOldPen = dc.SelectObject(&pen)) == NULL )
		return;
	
	nOldROP2 = dc.SetROP2(R2_NOT);
	nOldBkMode = dc.SetBkMode(TRANSPARENT);
	
	dc.MoveTo(0,y);
	dc.LineTo(m_rGrid.right,y);
	m_nOldY = y;

	dc.SetROP2(nOldROP2);
	dc.SetBkMode(nOldBkMode);	
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CTableView::ClearResizeRowLine()
{
	DrawResizeRowLine(m_nOldY);
}

void CTableView::DrawMoveColumnLine(long x)
{
	CClientDC dc(this);
	CBrush* pOldBrush;
	CBrush brush;
	CBitmap bitmap;
	long aPattern[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
	
	if ( ! bitmap.CreateBitmap(8,8,1,1,aPattern) )
		return;
	if ( ! brush.CreatePatternBrush(&bitmap) )
		return;
	if ( (pOldBrush = dc.SelectObject(&brush)) == NULL )
		return;
	
	dc.PatBlt(x-1,0,3,m_rGrid.bottom,PATINVERT);
	m_nOldX = x;

	dc.SelectObject(pOldBrush);
	brush.DeleteObject();
	bitmap.DeleteObject();
}

void CTableView::ClearMoveColumnLine()
{
	DrawMoveColumnLine(m_nOldX);
}

void CTableView::DrawMoveRowLine(long y)
{
	CClientDC dc(this);
	CBrush* pOldBrush;
	CBrush brush;
	CBitmap bitmap;
	WORD aPattern[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
	
	if ( ! bitmap.CreateBitmap(8,8,1,1,aPattern) )
		return;
	if ( ! brush.CreatePatternBrush(&bitmap) )
		return;
	if ( (pOldBrush = dc.SelectObject(&brush)) == NULL )
		return;
	
	dc.PatBlt(0,y-1,m_rGrid.right,3,PATINVERT);
	m_nOldY = y;

	dc.SelectObject(pOldBrush);
	bitmap.DeleteObject();
	brush.DeleteObject();
}

void CTableView::ClearMoveRowLine()
{
	DrawMoveRowLine(m_nOldY);
}

long CTableView::GetLastVisibleColumn()
{
	long		i = m_aColumns.GetUpperBound();

	// try to find (and return) last visible column in view
	while ( i >= 0 )
	{
		if ( !m_aColumns[i]->m_bIsHidden && !m_aColumns[i]->m_bIsExtrinsicBar )
			return i;

		i--;
	}

	// no visible columns in view
	return -1;
}

void CTableView::SizeToFit(int idxColumn)
{
	CPatnDoc * pDoc = GetDocument();

//	SetTableFont();

	//for ( int idxCol = 0; idxCol < pDoc->GetNoOfColumns(); idxCol++ )
	{
		int idxCol = idxColumn;
		int len_col,len_max;

		CRect rText;
		
		CString strValue;

		pDoc->GetColumnLabel(idxCol, strValue);
		len_col = GetDC()->GetTextExtent(strValue).cx;

		pDoc->GetColumnMaximum(idxCol,strValue);
		len_max = GetDC()->GetTextExtent(strValue).cx;
		
		int max = (len_max > len_col)?len_max:len_col;

		//set column with to max width value + 3
		pDoc->SetColumnWidth(idxCol,max/(double) m_nWidthUnit);
	}

	
}



void CTableView::SizeToFit(CArray<int>* pItems)
{
	CPatnDoc * pDoc = GetDocument();

//	SetTableFont();

	for ( int i = 0; i < pItems->GetSize() ; i++ )
	{
		int idxCol = pItems->GetAt(i);
		int len_col,len_max;

		CRect rText;
		
		CString strValue;

		pDoc->GetColumnLabel(idxCol, strValue);
		len_col = GetDC()->GetTextExtent(strValue).cx;

		pDoc->GetColumnMaximum(idxCol,strValue);
		len_max = GetDC()->GetTextExtent(strValue).cx;
		
		int max = (len_max > len_col)?len_max:len_col;

		//set column with to max width value + 3
		pDoc->SetColumnWidth(idxCol,max/(double) m_nWidthUnit);
	}

	
}



long CTableView::GetLastVisibleRow()
{
	long		i = m_aRows.GetUpperBound();

	// try to find (and return) last visible row in view
	while ( i >= 0 )
	{
		if ( !m_aRows[i]->m_bIsHidden && !m_aRows[i]->m_bIsExtrinsicBar )
			return i;
		i--;
	}

	// no visible rows in view
	return -1;
}

long CTableView::GetColumn(CPoint point)
{
	long i;

	for ( i = 0; i < m_aColumns.GetSize(); i++ )
	{
		if ( point.x >= m_aColumns[i]->m_xLeft && point.x <= m_aColumns[i]->m_xRight )
			return i;
	}

	// point not on valid column
	return -1;
}

long CTableView::GetRow(CPoint point)
{
	long i;

	for ( i = 0; i < m_aRows.GetSize(); i++ )
	{
		if ( point.y >= m_aRows[i]->m_yTop && point.y <= m_aRows[i]->m_yBottom )
		{
			return i;
		}
	}

	// point not on valid row
	return -1;
}

long CTableView::GetColumn(long idx)
{
	long		i;
	long		nCount = m_aColumns.GetSize();

	for ( i = 0; i < nCount; i++ )
	{
		if ( m_aColumns[i]->m_bIsHidden )
		{
			if ( idx >= m_aColumns[i]->m_idxStart && idx <= m_aColumns[i]->m_idx )
			{
				return i;
			}
		}
		else if ( m_aColumns[i]->m_idx == idx )
		{
			return i;
		}
	}

	return -1;
}

long CTableView::GetRow(long idx)
{
	long		i;
	long		nCount = m_aRows.GetSize();

	for ( i = 0; i < nCount; i++ )
	{
		if ( m_aRows[i]->m_bIsHidden )
		{
			if ( idx >= m_aRows[i]->m_idxStart && idx <= m_aRows[i]->m_idx )
			{
				return i;
			}
		}
		else if ( m_aRows[i]->m_idx == idx )
		{
			return i;
		}
	}

	return -1;
}

void CTableView::SetActiveCellAt(long idxColumn, long idxRow, BOOL bScrollIntoView /*= TRUE*/)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// make sure new cell is within valid range of rows & columns
	if ( !pDoc->IsCellInTable(idxColumn, idxRow) )
		return;

	// make sure new cell is not hidden
	if ( pDoc->IsColumnHidden(idxColumn) || pDoc->IsRowHidden(idxRow) )
		return;

    // Clear cell at old coordinates
	if ( m_ActiveCell.m_bIsEditing )
	{
		EndEdit();
		
	}
	else
	{
		RemoveFrame();
	}

	// Set cell at new coordinates
	m_ActiveCell.m_idxColumn = idxColumn;
	m_ActiveCell.m_idxRow = idxRow;
	m_ActiveCell.m_bIsEditing = FALSE;
	DrawFrame();

	// scroll active cell fully into view?
	if ( bScrollIntoView )
		ScrollActiveCellIntoView();
}

void CTableView::ScrollActiveCellIntoView()
{
	// this function scrolls the active cell fully into view at the nearest
	// edge of the view, eg: if the active cell is out of view on the right side,
	// then it will be scrolled so that it is fully visible on the right side of view.
	
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	long		cx, cy;
	long		idx;
	long		iCol, iRow;
	long		idxNewLeftColumn, idxNewTopRow;
	const long cxMax = m_cxClient - (m_nRowBtnBarWidth + 1);
	const long cyMax = m_cyClient - (m_nColumnBtnBarHeight + 1);
	const long idxExtrinsicColumn = pDoc->GetIdxLeftExtrinsicColumn();
	const long idxExtrinsicRow = pDoc->GetIdxTopExtrinsicRow();

	// column
	if ( (iCol = GetColumn(m_ActiveCell.m_idxColumn)) == -1 ) // not in view
	{
		if ( m_ActiveCell.m_idxColumn < m_idxLeftColumn ) { // out of view on left side
			//SetScrollPos(SB_HORZ,m_ActiveCell.m_idxColumn,TRUE); //			SendMessage(WM_HSCROLL,SB_THUMBPOSITION | m_ActiveCell.m_idxColumn << 16,NULL);
			//SendMessage(WM_HSCROLL,SB_THUMBPOSITION | m_ActiveCell.m_idxColumn,NULL);
            while(m_ActiveCell.m_idxColumn < m_idxLeftColumn && m_idxLeftColumn  > 0)
				SendMessage(WM_HSCROLL,SB_LINELEFT | 0L,NULL);
		}
		else // must be out of view on right side
		{
			/****TODO: this assumes that m_ActiveCell.m_idxColumn is not hidden ****/
			cx = CalcViewColumnWidth(m_ActiveCell.m_idxColumn);
			idx = m_ActiveCell.m_idxColumn;
			do
			{
				idxNewLeftColumn = idx;

				// find the index of the previous non-hidden column in the document
				idx = pDoc->GetIdxPreviousVisibleColumn(idx);

				if(idx < 0)
				{
					cx += 1;
					continue;
				}

				// first, add the width of the new column ...
				cx += CalcViewColumnWidth(idx);

				// ... and then check whether hidden columns and/or the extrinsic bar
				// are in between the current left column and the new column - if so,
				// then add the required additional amounts:
				if ( idxNewLeftColumn - idx > 1 )	// ie, separated by hidden columns
					cx += 1;				// width of block of hidden columns = 1

				if ( idxExtrinsicColumn > idx && idxExtrinsicColumn <= idxNewLeftColumn )
				{
					cx += m_nExtrinsicBarWidth;
					if ( idxExtrinsicColumn > (idx + 1) &&
						 idxExtrinsicColumn < idxNewLeftColumn )
						// hidden columns split by extrinsic bar - 2 sets of hidden columns
 						cx += 1; // width of block of hidden columns = 1
				}
			} while ( cx <= cxMax );
			//SendMessage(WM_HSCROLL,SB_THUMBPOSITION | idxNewLeftColumn << 16,NULL);
			long l = 0;
			while(idxNewLeftColumn > m_idxLeftColumn && l < 10000)
			{
				l = l + 1;
				SendMessage(WM_HSCROLL,SB_LINERIGHT | 0L,NULL);
			}

		}
	}
	else if ( m_aColumns[iCol]->m_xRight > m_cxClient ) // overlaps right edge of view
		SendMessage(WM_HSCROLL,SB_LINERIGHT | 0L,NULL);

	// row
	if ( (iRow = GetRow(m_ActiveCell.m_idxRow)) == -1 ) // not in view
	{
		if ( m_ActiveCell.m_idxRow < m_idxTopRow ) // out of view at top
		{
			SendMessage(WM_VSCROLL,SB_THUMBPOSITION | m_ActiveCell.m_idxRow << 16,NULL);
			while(m_ActiveCell.m_idxRow < m_idxTopRow && m_idxTopRow > 0)
				SendMessage(WM_VSCROLL,SB_LINEUP | 0L,NULL);
		}
		else // must be out of view at bottom
		{
			/****TODO: this assumes that m_ActiveCell.m_idxRow is not hidden ****/
			cy = m_nRowHeight;
			idx = m_ActiveCell.m_idxRow;
			do
			{
				idxNewTopRow = idx;

				// find the index of the previous non-hidden row in the document
				idx = pDoc->GetIdxPreviousVisibleRow(idx);

				// first, add the height of the new row ...
				cy += m_nRowHeight;

				// ... and then check whether hidden rows and/or the extrinsic bar
				// are in between the current top row and the new row - if so,
				// then add the required additional amounts:
				if ( idxNewTopRow - idx > 1 )	// ie, separated by hidden rows
					cy += 1;				// height of block of hidden rows = 1

				if ( idxExtrinsicRow > idx && idxExtrinsicRow <= idxNewTopRow )
				{
					cy += m_nExtrinsicBarHeight;
					if ( idxExtrinsicRow > (idx + 1) && idxExtrinsicRow < idxNewTopRow )
						// hidden rows split by extrinsic bar - 2 sets of hidden rows
 						cy += 1; // height of block of hidden rows = 1
				}
			} while ( cy <= cyMax );
			//SendMessage(WM_VSCROLL,SB_THUMBPOSITION | idxNewTopRow << 16,NULL);
			long l = 0;
			while(idxNewTopRow > m_idxTopRow && l < 10000)
			{
				l = l + 1;
				SendMessage(WM_VSCROLL,SB_LINEDOWN | 0L,NULL);
			}
		}
	}
	else if ( m_aRows[iRow]->m_yBottom > m_cyClient ) // overlaps bottom edge of view
		SendMessage(WM_VSCROLL,SB_LINEDOWN | 0L,NULL);

}

HBRUSH CTableView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if ( nCtlColor == CTLCOLOR_EDIT && pWnd == &m_ActiveCell.m_edit )
	{
		// Set the text background color.
		pDC->SetBkColor(RGB(192,192,192));
		// Set the text foreground color.
		pDC->SetTextColor(GetDocument()->m_rgbFontCells);
		// Return the control background brush.
 		return (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		// return GetStockObject(LTGRAY_BRUSH);
	}
	else
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

void CTableView::StartEdit(BOOL bCut/* = TRUE*/)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    CRect rCell;
	CRect rFormat;
	CString strValue;

	// make sure active is cell fully in view
	ScrollActiveCellIntoView();

	// get cell rect
	GetActiveCellRect(rCell);
	if ( rCell.IsRectEmpty() )
		return;

	if ( bCut )
		m_ActiveCell.m_edit.SetWindowText("");
	else
	{
		strValue.Format("%f",pDoc->GetValueAt(m_ActiveCell.m_idxColumn, m_ActiveCell.m_idxRow));
		m_ActiveCell.m_edit.SetWindowText(strValue);
	}

	// set initial edit window size & pos
	rCell.top++; // adjust rCell so as not to overwrite frame around cell
	rCell.left++;
	m_ActiveCell.m_edit.MoveWindow(rCell,FALSE);

	// adjust top of edit window if row height is too low to fit font
	if ( ((rCell.Height() + 1) - (g_ctm + g_cbm - 2)) < m_tm.tmHeight )
	{
		rCell.top = rCell.bottom - g_cbm - m_tm.tmHeight;
		rCell.top = max(rCell.top,m_rColumnBtnBar.bottom + 1);
		m_ActiveCell.m_edit.MoveWindow(rCell,FALSE);
	}

	// vertically align text in edit window with text in cell
	m_ActiveCell.m_edit.GetRect(&rFormat);
	ClientToScreen(&rCell);
	m_ActiveCell.m_edit.ScreenToClient(&rCell); // convert to edit window's coordinates
	rFormat.bottom = rCell.bottom - g_cbm;
	rFormat.top = rFormat.bottom - m_tm.tmHeight;
	m_ActiveCell.m_edit.SetRect(&rFormat);

	// size window to fit as many lines as needed/possible
	SendMessage(WM_RESIZE_EDIT,0,0);

	// flag that edit window is activated
	m_ActiveCell.m_bIsEditing = TRUE;

	// everything's ready to display edit window & hand the focus to it
	m_ActiveCell.m_edit.ShowWindow(SW_SHOW);
	m_ActiveCell.m_edit.SetFocus();
}

void CTableView::EndEdit()
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rCell;
	CString strValue;
	CUpdateHint UpdateHint;


	if ( m_ActiveCell.m_bIsEditing )
	{
		if ( m_ActiveCell.m_edit.GetModify() )
		{
			m_ActiveCell.m_edit.GetWindowText(strValue);
			pDoc->SetValueAt(m_ActiveCell.m_idxColumn,m_ActiveCell.m_idxRow,strValue);
		}
		RemoveFrame(); // Clear frame before edit window is hidden
		GetActiveCellRect(rCell);
		m_ActiveCell.m_edit.MoveWindow(0,0,0,0,FALSE);
		m_ActiveCell.m_edit.ShowWindow(SW_HIDE);
		SetFocus(); // return focus to CTableView
		m_ActiveCell.m_bIsEditing = FALSE; // flag that edit window is inactive

		// update views
		InvalidateRect(&rCell,TRUE); // repaint area uncovered by edit window
		UpdateWindow();
		UpdateHint.m_Cell.m_idxColumn = m_ActiveCell.m_idxColumn;
		UpdateHint.m_Cell.m_idxRow = m_ActiveCell.m_idxRow;
		pDoc->UpdateAllViews(this,CUpdateHint::editCell,&UpdateHint);
	}
	//this->autosizecolumns = true;
	RedrawWindow();

}

LRESULT CTableView::OnResizeEdit(WPARAM wParam, LPARAM)
{
	CRect	rCell;
	CRect	rFormat;
	long		iCol, iRow;
	long		nLineCount;
	long		yTopAlign;
	CPoint	point;

	// get current coordinates
	m_ActiveCell.m_edit.GetWindowRect(&rCell);
	m_ActiveCell.m_edit.GetRect(&rFormat);
	yTopAlign = rFormat.top;
	ScreenToClient(&rCell);
	point.x	= rCell.left;
	point.y = rCell.bottom;

	// if edit window size is too small to fit contents on one line, expand to the left first
	iCol = GetColumn(point);
	while ( iCol > 0 && m_ActiveCell.m_edit.GetLineCount() > 1 )
	{
		iCol--;
		rCell.left = m_aColumns[iCol]->m_xLeft + 1;
		m_ActiveCell.m_edit.MoveWindow(rCell,FALSE);
		m_ActiveCell.m_edit.GetRect(&rFormat);
		rFormat.top = yTopAlign;
		rFormat.bottom = rFormat.top + m_tm.tmHeight;
		m_ActiveCell.m_edit.SetRect(&rFormat);
	}

	// if more space needed, adjust vertical size
	nLineCount = m_ActiveCell.m_edit.GetLineCount();
	m_ActiveCell.m_edit.GetRect(&rFormat);
	if ( nLineCount * m_tm.tmHeight > (rFormat.Height() + 1) )
	{
		// as a start, calc a height which is a multiple of the no of lines
		// that need to be displayed in the edit window
		m_ActiveCell.m_edit.GetWindowRect(&rCell);
		ScreenToClient(&rCell);
		m_ActiveCell.m_edit.ClientToScreen(&rFormat);
		ScreenToClient(&rFormat);
		rFormat.bottom = rFormat.top + m_tm.tmHeight * nLineCount;

		// if the bottom of the new rectangle is still within table grid area, then
		// align bottom with bottom of underlying row
		if (rFormat.bottom < m_yGrid )
		{
			point.x	= rFormat.left;
			point.y = rFormat.bottom;
			iRow = GetRow(point);
			rFormat.bottom = m_aRows[iRow]->m_yBottom - 1;
		}

		// make sure the bottom (+ 1 pixel at bottom for frame) remains visible within view
		if ( rFormat.bottom > m_cyClient - 2 )
			rFormat.bottom = m_cyClient - 2;

		// set edit window to the new height
		rCell.bottom = rFormat.bottom;
		m_ActiveCell.m_edit.MoveWindow(rCell,FALSE);
		ClientToScreen(&rFormat);
		m_ActiveCell.m_edit.ScreenToClient(&rFormat);
		m_ActiveCell.m_edit.SetRect(&rFormat);
	}

	DrawFrame();

	return 0L;
}

void CTableView::DrawFrame()
{
	CClientDC dc(this);
	DrawFrame(&dc);
}
    
void CTableView::DrawFrame(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rCell;

//	GetCellRect(m_ActiveCell.m_idxColumn, m_ActiveCell.m_idxRow, rCell);
	GetActiveCellRect(rCell);
	if ( rCell.IsRectEmpty() )
		return;

	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(rCell.left+1,rCell.bottom,rCell.Width(),1,PATCOPY);
	pDC->PatBlt(rCell.right,rCell.top,1,rCell.Height(),PATCOPY);

	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->PatBlt(rCell.left,rCell.top,rCell.Width() + 1,1,PATCOPY);
	pDC->PatBlt(rCell.left,rCell.top,1,rCell.Height() + 1,PATCOPY);

	pDC->SelectObject(pOldBrush);
}

void CTableView::RemoveFrame()
{
	CClientDC dc(this);
	RemoveFrame(&dc);
}

void CTableView::RemoveFrame(CDC* pDC)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect	rCell;

	GetActiveCellRect(rCell);
	if ( rCell.IsRectEmpty() )
		return;

	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->PatBlt(rCell.left,rCell.bottom,rCell.Width() + 1,1,PATCOPY);
	pDC->PatBlt(rCell.left,rCell.top,rCell.Width() + 1,1,PATCOPY);
	pDC->PatBlt(rCell.left,rCell.top,1,rCell.Height(),PATCOPY);
	pDC->PatBlt(rCell.right,rCell.top,1,rCell.Height(),PATCOPY);

	pDC->SelectObject(pOldBrush);
}

int CTableView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	autosizecolumns = true;

	m_psd = NULL;

	CRect rect(0,0,0,0);
	m_ActiveCell.m_edit.Create(WS_CHILD |
							   ES_MULTILINE |
							   ES_AUTOHSCROLL |
							   ES_AUTOVSCROLL |
							   ES_RIGHT,
							   rect,this,0);
	
	m_brushXBar.CreateSolidBrush(RGB(255,0,0)); // red brush for extrinsic bars


	m_mousewheelsensitivity = ((CPatnApp*)AfxGetApp())->m_mousewheelsensitivity;

	CPatnDoc * pDoc = (CPatnDoc*) GetDocument();
	{
		CString str;
		str = pDoc->GetTitle();
		
		str.Insert(str.GetLength()," : Table");
		SetWindowText(str);
	}

	return 0;
}

void CTableView::OnToolsDataTransformation() 
{
	CDataTransformDlg dlg(GetDocument(), this);
	dlg.DoModal();

	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
	pDoc->UpdateAllTotals();
	pDoc->UpdateIntrinsicTotals();
	pDoc->UpdateGroups();
	pDoc->UpdateAllViews(NULL);
}

void CTableView::OnToolsDataAnalysis() 
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CAnalysisDlg dlg(pDoc,this);
	if ( dlg.DoModal() != IDOK )
		return;

	m_pColumnTotalsView->CheckPcc();

	//refresh all
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	} 
	pDoc->SetModifiedFlag(TRUE);
}


void CTableView::OnTableColumnGroups() 
{
	// TODO: Add your command handler code here
	CPatnDoc *pDoc = GetDocument();
	CGroupsDlg groupsdlg(pDoc);
	groupsdlg.m_nDataType = false;
	groupsdlg.DoModal();
	pDoc->UpdateGroups();

	CColr c(pDoc);
	c.OnCreate();

	CMst m(pDoc);
	m.OnCreate();

	//update column tab
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	}
}

void CTableView::OnTableColumnFind() 
{
	// TODO: Add your command handler code here
	
}

void CTableView::OnTableRowFind() 
{
	// TODO: Add your command handler code here

	
}

void CTableView::OnTableRowGroups() 
{
	// TODO: Add your command handler code here
	CPatnDoc *pDoc = GetDocument();
	CGroupsDlg groupsdlg(pDoc);
	groupsdlg.m_nDataType = true;
	groupsdlg.DoModal();
	pDoc->UpdateGroups();
	//update all views
	CColr c(pDoc);
	c.OnCreate();

	CMst m(pDoc);
	m.OnCreate();

	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	}

}


// CTableView printing

BOOL CTableView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog * d =  new CPageSetupDialog(PSD_DISABLEORIENTATION);
	//if(d->DoModal() == IDCANCEL) {delete d;return false;}
	m_psd = d;

	return DoPreparePrinting(pInfo);
}

void CTableView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	//setup new table font...
	LOGFONT* plf = &(pDoc->m_lfCells);
	
	//same as in SetTableFont(), but *5 (it is too small at the moment);
	plf->lfHeight = -(MulDiv(pDoc->m_nPointSizeCells, pDC->GetDeviceCaps(LOGPIXELSY), 72))/10;
	// create font
	m_fontCells.DeleteObject();
	m_fontCells.CreateFontIndirect(plf);

	//CRect rect,temp;
	//CPoint point;

	//m_psd->GetMargins(&rect,&temp);
	//point.x = m_psd->GetPaperSize().cx;
	//point.y = m_psd->GetPaperSize().cy;

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
	
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = m_HRes;
	m_rectDraw.bottom = m_VRes;
	
	m_nRowHeight *= 3* (long) ceil(m_VScale);
	m_nWidthUnit *= 3* (long) ceil(m_HScale);
	m_nRowBtnBarWidth *= 3* (long) ceil(m_HScale);

	// default preparation
	//double Lmost = (rect.left)/10.0;//pInfo->m_rectDraw.left;
	//double Rmost = (point.x - rect.right)/10.0;//pInfo->m_rectDraw.right;
	//double Tmost = (rect.top)/10.0;//pInfo->m_rectDraw.top;
	//double Bmost = (point.y - rect.bottom)/10.0;//pInfo->m_rectDraw.bottom;
 	double Lmost = m_rectDraw.left;//pInfo->m_rectDraw.left;
	double Rmost = m_rectDraw.right;//pInfo->m_rectDraw.right;
	double Tmost = m_rectDraw.top;//pInfo->m_rectDraw.top;
	double Bmost = m_rectDraw.bottom;//pInfo->m_rectDraw.bottom;
	PageInfo *info;
	CArray<PageInfo*> tempcolPages,temprowPages;

	//find column range & indexes
	long idx = -1;
	long startidx = idx;
	double xpos = m_nRowBtnBarWidth;
	while(startidx < pDoc->GetNoOfColumns()){
		if(!pDoc->IsColumnHidden(idx+1))
			xpos += Lmost + CalcViewColumnWidth(idx+1);
		while(xpos <= Rmost && idx < pDoc->GetNoOfColumns()-1){
			idx++;
			if(idx+1 < pDoc->GetNoOfColumns() && !pDoc->IsColumnHidden(idx+1))
				xpos += CalcViewColumnWidth(idx+1);
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
	double ypos = m_nRowHeight;
	while(startidx < pDoc->GetNoOfRows()){
		if(!pDoc->IsRowHidden(idx+1))
			ypos += Tmost + m_nRowHeight;
		while(ypos <= Bmost  && idx < pDoc->GetNoOfRows()-1){
			idx++;
			if(idx+1 < pDoc->GetNoOfRows() && !pDoc->IsRowHidden(idx+1))
				ypos += m_nRowHeight;
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
	m_printcols = i;
	m_printrows = j;

	pInfo->SetMaxPage(i*j);

	//tidy up...
	m_nRowHeight /= (3* (long) ceil(m_VScale));
	m_nWidthUnit /= (3* (long) ceil(m_HScale));
	m_nRowBtnBarWidth /= (3* (long) ceil(m_HScale));
	for(i=0;i<tempcolPages.GetSize();i++)
		delete tempcolPages[i];
	for(i=0;i<temprowPages.GetSize();i++)
		delete temprowPages[i];

}

void CTableView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	long i;
	for(i=0;i<m_pageinfo.GetSize();i++)
		delete m_pageinfo.GetAt(i);
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;

	//restore table font
	CPatnDoc * pDoc = (CPatnDoc*) GetDocument();
	LOGFONT* plf = &(pDoc->m_lfCells);
	m_fontCells.DeleteObject();
	CClientDC dc(this);
	// note: pDoc->m_nPointSizeCells is in tenths of a point, hence divide by ten
	plf->lfHeight = -(MulDiv(pDoc->m_nPointSizeCells, dc.GetDeviceCaps(LOGPIXELSY), 72))/10;
	m_fontCells.CreateFontIndirect(plf);

	
}

void CTableView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_nRowHeight *= 3* (long) ceil(m_VScale);
	m_nWidthUnit *= 3* (long) ceil(m_HScale);
	m_nRowBtnBarWidth *= 3* (long) ceil(m_HScale);

	//current page being printed == pInfo->m_nCurSel
	long cPage = pInfo->m_nCurPage - 1;
	long i,j;
	CRect rect;
	CString text;

//	pDC->MoveTo(0,0);
//	pDC->LineTo(1000,0);
//	pDC->MoveTo(0,0);
//	pDC->LineTo(0,1000);
//	pDC->MoveTo(m_rectDraw.right,m_rectDraw.bottom);
//	pDC->LineTo(m_rectDraw.right,0);
//	pDC->MoveTo(m_rectDraw.right,m_rectDraw.bottom);
//	pDC->LineTo(0,m_rectDraw.bottom);


	CFont*	pOldFont;

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(pDoc->m_rgbFontCells);

	CFont pfontCells;

	// select font into memory DC
	if ( (pOldFont = pDC->SelectObject(&m_fontCells)) == NULL ){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not select font for table");
	}

//	rect.left = rect.right = pInfo->m_rectDraw.left;
	rect.left = rect.right = m_rectDraw.left;
	
	for(i=m_pageinfo.GetAt(cPage)->left;i<=m_pageinfo.GetAt(cPage)->right;i++){
		if(i == -1){
			rect.right += m_nRowBtnBarWidth;
		}else{
			rect.left = rect.right;
			rect.right += CalcViewColumnWidth(i);
		}
		//rect.top = pInfo->m_rectDraw.top+1;
		rect.top = m_rectDraw.top + 1;
		rect.bottom = rect.top + m_nRowHeight;

		for(j=m_pageinfo.GetAt(cPage)->top;j<=m_pageinfo.GetAt(cPage)->bottom;j++){
			if(i == -1 && j != -1){
				//printrow title
			
				//continue if hidden
				if(pDoc->IsRowHidden(j)) continue;

				pDoc->GetRowLabel(j,text);
				//call a function to modify 'text' to fit
				// in width of 'rect.right - rect.left'
				pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
				
				pDC->ExtTextOut((rect.right-rect.left)/2+rect.left,rect.bottom, ETO_CLIPPED, 
					rect,text,text.GetLength(),NULL);

				//draw surrounding rect
				//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
			}else if(j == -1 && i != -1){
				//printcol title

				//continue if hidden
				if(pDoc->IsColumnHidden(i)) continue;

				pDoc->GetColumnLabel(i,text);

				//call a function to modify 'text' to fit
				// in width of 'rect.right - rect.left'
				pDC->SetTextAlign(TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);

				pDC->ExtTextOut((rect.right-rect.left)/2+rect.left,rect.bottom, ETO_CLIPPED,
					rect,text,text.GetLength(),NULL);

				//draw surrounding rect
				//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
			}else if(j != -1 && i != -1){
				//print cell contents

				//continue if hidden
				if(pDoc->IsRowHidden(j) || pDoc->IsColumnHidden(i))
					continue;

				pDoc->GetValueAt(i,j,text);				

				//call a function to modify 'text' to fit
				// in width of 'rect.right - rect.left'

				pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP);

				pDC->ExtTextOut(rect.right-2,rect.bottom, ETO_CLIPPED,
					rect,text,text.GetLength(),NULL);

				//draw surrounding rect
				//pDC->DrawEdge(rect,EDGE_RAISED,BF_RECT);
			}
			rect.top = rect.bottom;
			rect.bottom += m_nRowHeight;
		}
	}
	pDC->SelectObject(pOldFont);

	m_nRowHeight /= (3* (long) ceil(m_VScale));
	m_nWidthUnit /= (3* (long) ceil(m_HScale));
	m_nRowBtnBarWidth /= (3* (long) ceil(m_HScale));
}


void CTableView::OnFileAppend() 
{
	(CPatnDoc*)GetDocument()->AppendExternalFile();
	SetScrollPos(SB_VERT,m_idxTopRow,TRUE);
	SetScrollPos(SB_HORZ,m_idxLeftColumn,TRUE);

}


BOOL CTableView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	static bool inMouseWheel = false;
	if(inMouseWheel) return true;
	inMouseWheel = true;

	if(m_mousewheelsensitivity == 0)
		m_mousewheelsensitivity = 40;

	long x,y,diff;
	
	x = zDelta / m_mousewheelsensitivity;
	y = GetScrollPos(SB_VERT);

	diff = y - x;

	CPatnDoc *pDoc = (CPatnDoc*) GetDocument();

	long max = pDoc->GetUpperBoundRows();

	for(long i=y;i != diff;(diff>y)?i++:i--){
		if(i <= 0 && i >= max) break;
		if(x < 0)
			OnVScroll(SB_LINEDOWN,0,NULL);
		else
			OnVScroll(SB_LINEUP,0,NULL);

	}
	inMouseWheel = false;

	return true;//CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CTableView::OnToolsDataevaluation() 
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();
	CEvaluation dlg(pDoc);
	dlg.m_parent = this->GetSafeHwnd();
	dlg.DoModal();
}

void CTableView::OnTableTansposedatalabels() 
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();

	BeginWaitCursor();

	//transposes data & labels

	//18.12.2005 corrects error when transposing and activated cell & scrolling position
	// will soon not exist.
	if(pDoc->GetNoOfRows() > 0 && pDoc->GetNoOfColumns() > 0)
	{		
		m_idxLeftColumn = 0;
		m_idxTopRow = 0;
		//OnHScroll(SB_THUMBPOSITION,0,NULL);
		//OnVScroll(SB_THUMBPOSITION,0,NULL);
		SetScrollPos(SB_HORZ,0,true);
		SetScrollPos(SB_VERT,0,true);
		SetActiveCellAt(0,0,false);
	}
	//end 18.12.2005

	pDoc->m_loading = true;

	//setup, unhide all, remove extrinsic settings
	for(long idx = 0; idx < pDoc->GetNoOfColumns(); idx++)
        pDoc->UnhideColumn(idx);
	for(idx = 0; idx < pDoc->GetNoOfRows(); idx++)
        pDoc->UnhideRow(idx);

	pDoc->m_idxLeftExtrinsicColumn = -1;
	pDoc->m_idxTopExtrinsicRow = -1;
	
	//make changes
	if((!pDoc->m_TwayChange || !pDoc->m_AssocRowChange ||
		!pDoc->m_AssocColChange || !pDoc->m_FuseRowChange ||
		!pDoc->m_FuseColChange || !pDoc->m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("Warning: This step will invalidate the current analyses.",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		pDoc->m_TwayChange = 1;
		pDoc->m_AssocRowChange = 1;
		pDoc->m_AssocColChange = 1;
		pDoc->m_FuseRowChange = 1;
		pDoc->m_FuseColChange = 1;
		pDoc->m_OrdChange = 1;

		pDoc->m_ANOSIMColsExists = false;
		pDoc->m_ANOSIMRowsExists = false;

		for(long i=0;i<pDoc->m_Vectors.GetSize();i++)
			if(pDoc->m_Vectors[i]->GetSize())
				pDoc->m_Vectors[i]->SetAt(0,ANALYSIS_VALUE);
		pDoc->m_MCAOExists = false;		
		pDoc->m_KWExists = false;
		pDoc->m_KW.SetSize(0);
		pDoc->m_KWValues.SetSize(0);
		pDoc->m_KWAttributes.SetSize(0);
		
		pDoc->m_AssocMatrixCols.Reset();
		pDoc->m_AssocMatrixRows.Reset();
	}

	BeginWaitCursor();
	/*
	long i,rows,cols,j;
	double value;
	CString buffer;

	rows = pDoc->GetNoOfRows();
	cols = pDoc->GetNoOfColumns();

	//set new size
	for(i=0;i<cols;i++)	pDoc->AddRow();
	for(i=0;i<rows;i++) pDoc->AddColumn();

	//set data and labels
	for(i=0;i<cols;i++){
		pDoc->GetColumnLabel(i,buffer);
		pDoc->SetRowLabel(i+rows,buffer);
		for(j=0;j<rows;j++){
			if(!i){
				pDoc->GetRowLabel(j,buffer);
				pDoc->SetColumnLabel(j+cols,buffer);
			}
			value = pDoc->GetValueAt(i,j);
			pDoc->SetValueAt(j+cols,i+rows,value);
		}
	}	

	//remove redundant values
	pDoc->RemoveColumnAt(0,cols);
	pDoc->RemoveRowAt(0,rows);
*/

	//faster transpose!!!
/*	pDoc->m_loading = true;
	double tmp1,tmp2;
	CString str1,str2;
	long i,j,rows,cols,max;

	OnHScroll(SB_THUMBPOSITION,0,NULL);
	OnVScroll(SB_THUMBPOSITION,0,NULL);

	rows = pDoc->GetNoOfRows();
	cols = pDoc->GetNoOfColumns();

	(cols>rows)?max=cols:max=rows;

	//set new size (make it square)
	for(i=rows;i<max;i++) pDoc->AddRow();
	for(i=cols;i<max;i++) pDoc->AddColumn();

	for(i=0;i<max;i++){
		if(i<cols) pDoc->GetColumnLabel(i,str1);		
		if(i<rows) pDoc->GetRowLabel(i,str2);
		if(i<rows) pDoc->SetColumnLabel(i,str2);	
		if(i<cols) pDoc->SetRowLabel(i,str1);
		
		for(j=i+1;j<max;j++){
			if(i<cols) tmp1 = pDoc->GetValueAt(i,j);			
			if(i<rows) tmp2 = pDoc->GetValueAt(j,i);
			if(i<rows) pDoc->SetValueAt(i,j,tmp2);		
			if(i<cols) pDoc->SetValueAt(j,i,tmp1);
		}

	}

	//remove redundant rows/cols
	if(max > rows)
		pDoc->RemoveColumnAt(rows,max-rows);
	if(max > cols)
		pDoc->RemoveRowAt(cols,max-cols);
*/

	//18.12.2005 better transpose
	//make a copy of labels
	CArray<CString> rlabels,clabels;
	long i;
	long nr = pDoc->GetNoOfRows();
	long nc = pDoc->GetNoOfColumns();
    rlabels.SetSize(nr);
	for(i=0;i<nr;i++)
		pDoc->GetRowLabel(i,rlabels[i]);
	clabels.SetSize(nc);
	for(i=0;i<nc;i++)
		pDoc->GetColumnLabel(i,clabels[i]);
	//copy data
	double * data = new double[nr*nc];
	int j;
	for(i=0;i<nr;i++)
	{
		for(j=0;j<nc;j++)
		{
			data[i*nc+j] = pDoc->GetValueAt(j,i);
		}
	}

	//delete all data
	

	m_pSelectedColumns->StartActiveSelection(0);
	m_pSelectedColumns->Shift(nc-1);
	OnTableColumnDelete();
	m_pSelectedRows->StartActiveSelection(0);
	m_pSelectedRows->Shift(nr-1);
	OnTableRowDelete();
    
	//put data back (swap rows & cols)
	for(i=0;i<pDoc->m_aData.GetSize();i++)
		delete pDoc->m_aData[i];
	for(i=0;i<pDoc->m_aRowAttributes.GetSize();i++)
		delete pDoc->m_aRowAttributes[i];
	for(i=0;i<pDoc->m_aColumnAttributes.GetSize();i++)
		delete pDoc->m_aColumnAttributes[i];
	for(i=0;i<pDoc->m_aColumnTotalsRowAttributes.GetSize();i++)
		delete pDoc->m_aColumnTotalsRowAttributes[i];
	for(i=0;i<pDoc->m_aColumnAllocRowAttributes.GetSize();i++)
		delete pDoc->m_aColumnAllocRowAttributes[i];
	for(i=0;i<pDoc->m_aColumnCompareRowAttributes.GetSize();i++)
		delete pDoc->m_aColumnCompareRowAttributes[i];
	for(i=0;i<pDoc->m_aColumnPccAttributes.GetSize();i++)
		delete pDoc->m_aColumnPccAttributes[i];
	for(i=0;i<pDoc->m_aRowTotalsColumnAttributes.GetSize();i++)
		delete pDoc->m_aRowTotalsColumnAttributes[i];
	for(i=0;i<pDoc->m_aRowAllocColumnAttributes.GetSize();i++)
		delete pDoc->m_aRowAllocColumnAttributes[i];
	for(i=0;i<pDoc->m_aRowOrdColumnAttributes.GetSize();i++)
		delete pDoc->m_aRowOrdColumnAttributes[i];

	for(i=0;i<pDoc->m_Vectors.GetSize();i++)
		delete pDoc->m_Vectors[i];
	pDoc->m_Vectors.SetSize(0);

	pDoc->m_loading = true;

	pDoc->SetSizes(nr, nc); // set sizes for the various arrays

	// set row & column attributes
	pDoc->SetInitialColumnAttributes(nr, 12, 2);
	pDoc->SetInitialRowAttributes(nc, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	pDoc->SetInitialColumnTotalsRowAttributes();
	pDoc->SetInitialRowTotalsColumnAttributes(2);
	
	// initialize column totals
	for ( i = 0; i < nr; i++ )
	{
		pDoc->m_aColumnSums[i] = 0.0;
		pDoc->m_aColumnCountPositives[i] = 0;
		pDoc->m_aColumnMinimum[i] = pDoc->m_nMissingValue;
		pDoc->m_aColumnMean[i] = pDoc->m_nMissingValue;
		pDoc->m_aColumnMaximum[i] = pDoc->m_nMissingValue;
		pDoc->m_aColumnCountMissingValues[i] = 0;
		pDoc->m_aColumnGroups[i] = -1;
	}

	// initialize row totals
	for ( i = 0; i < nc; i++ )
	{
		pDoc->m_aRowSums[i] = 0.0;
		pDoc->m_aRowCountPositives[i] = 0;
		pDoc->m_aRowMinimum[i] = pDoc->m_nMissingValue;
		pDoc->m_aRowMean[i] = pDoc->m_nMissingValue;
		pDoc->m_aRowMaximum[i] = pDoc->m_nMissingValue;
		pDoc->m_aRowCountMissingValues[i] = 0;
		pDoc->m_aRowGroups[i] = -1;
		pDoc->m_aRowGroupsDist[i] = pDoc->m_nMissingValue;
		pDoc->m_aRowXCoords[i] = ANALYSIS_VALUE;
		pDoc->m_aRowYCoords[i] = ANALYSIS_VALUE;
		pDoc->m_aRowZCoords[i] = ANALYSIS_VALUE;
		pDoc->m_PatternObjects[i] = -1;
	}

	pDoc->m_loading = true;
	for(i=0;i<nr;i++)
	{
		pDoc->SetColumnLabel(i,rlabels[i]);
	}
	for(i=0;i<nc;i++)
	{
		pDoc->SetRowLabel(i,clabels[i]);
	}	
	for(i=0;i<nc;i++)
	{
		for(j=0;j<nr;j++)
		{
			pDoc->SetValueAt(j,i,data[j*nc+i]);
		}
	}
	delete data;

	pDoc->m_loading = false;

	SetScrollRange(SB_HORZ,0,pDoc->GetUpperBoundColumns(),TRUE);
	SetScrollRange(SB_VERT,0,pDoc->GetUpperBoundRows(),TRUE);

	EndWaitCursor();

	RedrawWindow();
	CUpdateHint UpdateHint;
	pDoc->UpdateAllViews(this, CUpdateHint::addColumn, &UpdateHint);
	pDoc->SetModifiedFlag();

	pDoc->m_loading = false;
	pDoc->UpdateAllTotals();
	pDoc->UpdateIntrinsicTotals();

	//update box&whisker data
	for(i=0;i<pDoc->m_aRowGroups.GetSize();i++)
	{
		pDoc->m_aRowGroups[i] = -1;
		pDoc->m_aRowGroupsDist[i] = pDoc->m_nMissingValue;
	}
	for(i=0;i<pDoc->m_aColumnGroups.GetSize();i++)
		pDoc->m_aColumnGroups[i] = -1;
	
	pDoc->UpdateGroups();		

	CColr ccolr(pDoc);
	ccolr.OnCreate();

	CMst cmst(pDoc);
	cmst.OnCreate();

	CGsta gsta(pDoc);

	//this->autosizecolumns = true;
	//RedrawWindow();
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	} 

	EndWaitCursor();
}

void CTableView::OnEditCut() 
{
/*	//copy selection
	OnEditCopy();

	//delete selection
	if(!m_pSelectedRows->IsEmpty()){
		OnTableRowDelete();
	}else if(!m_pSelectedColumns->IsEmpty()){
		OnTableColumnDelete();
	}*/
}

void CTableView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(!m_pSelectedRows->IsEmpty() || !m_pSelectedColumns->IsEmpty());	
}

void CTableView::OnEditCopy() 
{
/*	if(!OpenClipboard()){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("failed to open clipboard");
		return;
	}
	
	char * data = (char*) GetClipboardData(CF_TEXT);
	EmptyClipboard();

	CCopySelectionSize dlg;
	
	CString buffer,tmpstring;
	long i,j;
	char * text;

	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();

	CArray<int> aItems;
	
	if(!m_pSelectedRows->IsEmpty()){
		m_pSelectedRows->GetItems(&aItems);
		
		//column labels first
		if(((CPatnApp*)AfxGetApp())->m_copylabels){
			for(i=0;i<pDoc->GetNoOfColumns();i++){
				buffer += "\t";
				pDoc->GetColumnLabel(i,tmpstring);
				buffer += tmpstring;
			}
			buffer += "\r\n";
		}

		//copy row label followed by data
		for(i=0;i<aItems.GetSize();i++){
			pDoc->GetRowLabel(aItems[i],tmpstring);
			buffer += tmpstring;
			buffer += "\t";
			for(j=0;j<pDoc->GetNoOfColumns();j++){
				tmpstring.Format("%.4f",pDoc->GetValueAt(j,aItems[i]));
				buffer += tmpstring;
				if(j+1 < pDoc->GetNoOfColumns())
					buffer += "\t";
			}
			if(i+1 < aItems.GetSize())
				buffer += "\r\n";
		}
	}else if(!m_pSelectedColumns->IsEmpty()){
		m_pSelectedColumns->GetItems(&aItems);
		
		//column labels first
		for(i=0;i<aItems.GetSize();i++){
			if(((CPatnApp*)AfxGetApp())->m_copylabels || i > 0)
				buffer += "\t";
			pDoc->GetColumnLabel(aItems[i],tmpstring);
			buffer += tmpstring;
		}
		buffer += "\r\n";
		for(i=0;i<pDoc->GetNoOfRows();i++){
			//data for each column
			if(((CPatnApp*)AfxGetApp())->m_copylabels){
				pDoc->GetRowLabel(i,tmpstring);
				buffer += tmpstring;
				buffer += "\t";
			}
			for(j=0;j<aItems.GetSize();j++){
				tmpstring.Format("%.4f",pDoc->GetValueAt(aItems[j],i));
				buffer += tmpstring;
				if(j+1 < aItems.GetSize())
					buffer += "\t";
			}
			if(i+1 < pDoc->GetNoOfRows())
				buffer += "\r\n";
		}
	}else{ //selected cell
		dlg.DoModal();
		
		//col labels first...
		if(((CPatnApp*)AfxGetApp())->m_copylabels){
			for(i=m_ActiveCell.m_idxColumn;i<pDoc->GetNoOfColumns() && 
				i<dlg.m_cols+m_ActiveCell.m_idxColumn;i++){
				buffer += "\t";
				pDoc->GetColumnLabel(i,tmpstring);
				buffer += tmpstring;
			}
		}
		buffer += "\r\n";

		//rest - RowLabel\tdata
		for(i=m_ActiveCell.m_idxRow;i<pDoc->GetNoOfRows() && 
			i<dlg.m_rows+m_ActiveCell.m_idxRow;i++){
			if(((CPatnApp*)AfxGetApp())->m_copylabels){
				pDoc->GetRowLabel(i,tmpstring);
				buffer += tmpstring;
				buffer += "\t";
			}
			for(j=m_ActiveCell.m_idxColumn;j<pDoc->GetNoOfColumns() && 
				j<dlg.m_cols+m_ActiveCell.m_idxColumn;j++){
				tmpstring.Format("%.4f",pDoc->GetValueAt(j,i));
				buffer += tmpstring;				
				if(j+1<pDoc->GetNoOfColumns() && 
					j+1<dlg.m_cols+m_ActiveCell.m_idxColumn)
					buffer += "\t";
			}
			if(i+1<pDoc->GetNoOfRows() && 
				i+1<dlg.m_rows+m_ActiveCell.m_idxRow)
				buffer += "\r\n";		
		}
	}
	text = (char*) GlobalAlloc(GMEM_DDESHARE,
		sizeof(char)*buffer.GetLength()+1);
	strcpy(text,buffer);
	text[buffer.GetLength()] = '\0';
	SetClipboardData(CF_TEXT,(HANDLE)text);

	CloseClipboard();
*/	
}

void CTableView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(true);

}

void CTableView::OnEditPaste() 
{
	
/*	if(!OpenClipboard()){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Cannot open clipboard.");
		return;
	}

	char * data = (char*) GetClipboardData(CF_TEXT);


	//3 cases:
	// a) no selection, add as columns
	// b) column selection, add as columns BEFORE selection
	// c) row selection, add as rows BEFORE selection

	//before this, write to col labels, row labels, data/

	if(!m_pSelectedRows->IsEmpty()){
		
	}else if(!m_pSelectedColumns->IsEmpty()){

	}else{

	}



	CloseClipboard();
	*/
}

void CTableView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
/*	if(!OpenClipboard()){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Cannot open clipboard.");
		return;
	}
	
	pCmdUI->Enable(false);

	if(IsClipboardFormatAvailable(CF_TEXT)){
		pCmdUI->Enable(true);
	}

	CloseClipboard();	
	*/
}

void CTableView::OnEditOtherlabelsincopycut() 
{
/*	((CPatnApp*)AfxGetApp())->m_copylabels = !((CPatnApp*)AfxGetApp())->m_copylabels;

	if(((CPatnApp*)AfxGetApp())->m_copylabels){
		AfxGetMainWnd()->GetMenu()->CheckMenuItem(ID_EDIT_OTHERLABELSINCOPYCUT,MF_CHECKED);
	}else{		
		AfxGetMainWnd()->GetMenu()->CheckMenuItem(ID_EDIT_OTHERLABELSINCOPYCUT,MF_UNCHECKED);
	}	*/
}


void CTableView::OnTableSelectionoptions() 
{
	CPatnDoc * pDoc = (CPatnDoc*)GetDocument();

	CSelOptions so(pDoc);
	so.DoModal();
}

void CTableView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	((CPatnDoc*)GetDocument())->UpdateAllViews(NULL);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//BOOL CTableView::OnHelpInfo(HELPINFO* pHelpInfo)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
//   {
//      //return ::HtmlHelp(
//        // (HWND)pHelpInfo->hItemHandle,
//         //"\\patn.chm",
//         //HH_TP_HELP_WM_HELP,
//         //(DWORD)(LPVOID)myarray) 
//         //!= NULL;
//	   return ::HtmlHelp((HWND)GetDesktopWindow(),"patn.chm",HH_HELP_CONTEXT,131257) == NULL;
//	}
//	
//
//	return true;
//	return CView::OnHelpInfo(pHelpInfo);
//   
//}

//void CTableView::WinHelp(DWORD dwData, UINT nCmd)
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	CView::WinHelp(dwData, nCmd);
//}

void CTableView::AutoSizeColumns()
{
	CPatnDoc * pDoc = GetDocument();

	CMainFrame * frm = (CMainFrame*)AfxGetMainWnd();
	CString statbartext;
	statbartext.Format("calculating column widths...");
	frm->m_wndStatusBar.SetPaneText(1,statbartext);

//	SetTableFont();

	for ( int idxCol = 0; idxCol < pDoc->GetNoOfColumns(); idxCol++ )
	{
		int len_col,len_max;

		CRect rText;
		
		CString strValue;

		pDoc->GetColumnLabel(idxCol, strValue);
		CDC * dc = GetDC();
		if(dc != NULL)
		{			
			len_col = dc->GetTextExtent(strValue).cx;

			pDoc->GetColumnMaximum(idxCol,strValue);
			
			len_max = dc->GetTextExtent(strValue).cx;
			int max = (len_max > len_col)?len_max:len_col;
			//set column with to max width value + 3
			pDoc->SetColumnWidth(idxCol,max/(double) m_nWidthUnit);
		}
	}

	
}

void CTableView::AutoSizeRows(CDC * dc)
{
	CPatnDoc * pDoc = GetDocument();
	int max = 10;
	for ( int r = 0 ; r < pDoc->GetNoOfRows() ; r++ )
	{	
		CString strValue;

		pDoc->GetRowLabel(r, strValue);
		if(max < dc->GetTextExtent(strValue,strValue.GetLength()).cx)
			max = dc->GetTextExtent(strValue,strValue.GetLength()).cx;		
		
	}
	if(max > m_nRowBtnBarWidth)
	{
		m_nRowBtnBarWidth = max;
	}
}

void CTableView::OnDeleteDuplicates()
{
	//1. select duplicates
	//2. delete duplicates - this could just as easily be make extrinsic

	//sort row sums
	CPatnDoc * pDoc = GetDocument();
	CArray<long> rowidxs;
	rowidxs.SetSize(pDoc->m_aRowSums.GetSize());
	for(long i=0;i<rowidxs.GetSize();i++)
	{
		rowidxs[i] = i;
	}
	sort(&pDoc->m_aRowSums,&rowidxs,true);

    //for each identical row sum compare all rows & select
	m_pSelectedRows->RemoveAll();
	for(i=0;i<rowidxs.GetSize();i++)
	{
		long j = i + 1;
		while(j < rowidxs.GetSize() && 
			pDoc->m_aRowSums.GetAt(rowidxs[j]) == pDoc->m_aRowSums.GetAt(rowidxs[i]))
		{
			//is identical?
			for(int k=0;k<pDoc->GetNoOfColumns();k++)
			{
				if(pDoc->GetValueAt(k,rowidxs[j]) != pDoc->GetValueAt(k,rowidxs[i]))
				{
					break;
				}
			}
			if(k == pDoc->GetNoOfColumns())
			{
				//identical, select
				m_pSelectedRows->StartActiveSelection(rowidxs[j],FALSE);
				break;
			}
			j = j + 1;
		}
	}

    //delete
	CString str;
	str.Format("%d rows have been selected for deletion.\r\n\r\nDo you want to delete these?",m_pSelectedRows->GetCount());
	if(m_pSelectedRows->GetCount() > 0)
	{
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode && AfxMessageBox(str,MB_OKCANCEL) == IDOK)
		{
			this->OnTableRowDelete();	
		}
	}
	else
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode)  AfxMessageBox("No duplicate rows found.");
	}
}


void CTableView::ontest()
{
	//going to test the listview C3dLegend here - see if it can be generated as a modeless dialog box.
	//COrdinationLegend * legend = new COrdinationLegend();
	//legend->Create(IDD_3DLEGEND);
	//legend->m_pDoc = (CPatnDoc*)GetDocument();
	//legend->OnPopulate();
	//legend->ShowWindow(TRUE);	

}

void CTableView::OnDataSwapcolumnintrinsicswithextrinsics()
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int numberOfPreviousExtrinsics = pDoc->GetNoOfColumns() - pDoc->GetIdxLeftExtrinsicColumn();
	
	//select column instrinsics
	if(pDoc->GetIdxLeftExtrinsicColumn() != 0) {
		m_pSelectedColumns->RemoveAll();
		m_pSelectedColumns->StartActiveSelection(0);
		m_pSelectedColumns->UpdateActiveSelection((pDoc->GetIdxLeftExtrinsicColumn()==-1)?pDoc->GetNoOfColumns()-1:pDoc->GetIdxLeftExtrinsicColumn());

		//make them extrinsic
		OnTableColumnMakeExtrinsic();
	}

	//select the previous extrinsics
	if (numberOfPreviousExtrinsics >= 1 && numberOfPreviousExtrinsics <= pDoc->GetNoOfColumns()) {
		m_pSelectedColumns->RemoveAll();
		m_pSelectedColumns->StartActiveSelection(0);
		m_pSelectedColumns->UpdateActiveSelection(numberOfPreviousExtrinsics-1);

		//make them intrinsic
		OnTableColumnMakeIntrinsic();
	}

	m_pSelectedColumns->RemoveAll();

	UpdateWindow();
	
	// repaint all other views
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	} 
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnDataSwaprowintrinsicswithextrinsics()
{
	CWaitCursor waitCursor;

	CPatnDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int numberOfPreviousExtrinsics = pDoc->GetNoOfRows() - pDoc->GetIdxTopExtrinsicRow();
	
	//select Row instrinsics
	if(pDoc->GetIdxTopExtrinsicRow() != 0) {
		m_pSelectedRows->RemoveAll();
		m_pSelectedRows->StartActiveSelection(0);
		m_pSelectedRows->UpdateActiveSelection((pDoc->GetIdxTopExtrinsicRow()==-1)?pDoc->GetNoOfRows()-1:pDoc->GetIdxTopExtrinsicRow());

		//make them extrinsic
		OnTableRowMakeExtrinsic();
	}

	//select the previous extrinsics
	if (numberOfPreviousExtrinsics >= 1 && numberOfPreviousExtrinsics <= pDoc->GetNoOfRows()) {
		m_pSelectedRows->RemoveAll();
		m_pSelectedRows->StartActiveSelection(0);
		m_pSelectedRows->UpdateActiveSelection(numberOfPreviousExtrinsics-1);

		//make them intrinsic
		OnTableRowMakeIntrinsic();
	}

	m_pSelectedRows->RemoveAll();

	UpdateWindow();
	
	// repaint all other views
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		pView->RedrawWindow();
	} 
	pDoc->SetModifiedFlag(TRUE);
}

void CTableView::OnDataVariablesKwToExtrinsics()
{
	CPatnDoc * pDoc = (CPatnDoc*) GetDocument();
	double threashold = 5.0;
	long idx_to_move;
	bool moved = true;
	int i;

	CArray<long> t_KW;
	t_KW.SetSize(pDoc->m_KW.GetSize());
	for(i = 0 ;i<pDoc->m_KW.GetSize();i++) {
		t_KW.SetAt(i,pDoc->m_KW.GetAt(i));
	}
	CArray<double> t_KWValues;
	t_KWValues.SetSize(pDoc->m_KWValues.GetSize());
	for(i = 0;i<pDoc->m_KWValues.GetSize();i++) {
		t_KWValues.SetAt(i,pDoc->m_KWValues.GetAt(i));
	}
	CArray<long> t_KWAttributes;
	t_KWAttributes.SetSize(pDoc->m_KWAttributes.GetSize());
	for(i=0;i<pDoc->m_KWAttributes.GetSize();i++) {
		t_KWAttributes.SetAt(i,pDoc->m_KWAttributes.GetAt(i));
	}
	CArray<long> t_KWVisible;
	t_KWVisible.SetSize(pDoc->m_KWVisible.GetSize());
	for(i=0;i<pDoc->m_KWValues.GetSize();i++) {
		t_KWVisible.SetAt(i,pDoc->m_KWVisible.GetAt(i));
	}
	long t_KWNumVisible = pDoc->m_KWNumVisible;
	long t_KWOption = pDoc->m_KWOption;
	double t_KWCutoff = pDoc->m_KWCutoff;
	long t_KWExists = pDoc->m_KWExists;
	long t_kw_patn_generated_groups = pDoc->m_kw_patn_generated_groups;
	long move_count = 0;
	//save kw values

	//select intrinsic columns
	pDoc->m_KWAttributes.RemoveAll();
	for(i=0;i<pDoc->GetNoOfColumns() && (pDoc->GetIdxLeftExtrinsicColumn() == -1 || i < pDoc->GetIdxLeftExtrinsicColumn());i++){
		pDoc->m_KWAttributes.Add(i);
	}

	CKruskalWallace kwStart;
	kwStart.Create(NULL,pDoc,true);
	CKWThreashold dlgKWThreashold(GetDocument());
	if(dlgKWThreashold.DoModal() != IDOK) {
		
		//restore values
		pDoc->m_KW.SetSize(t_KW.GetSize());
		for(i=0;i<t_KW.GetSize();i++) pDoc->m_KW.SetAt(i,t_KW.GetAt(i));
		pDoc->m_KWValues.SetSize(t_KWValues.GetSize());
		for(i=0;i<t_KWValues.GetSize();i++) pDoc->m_KWValues.SetAt(i,t_KWValues.GetAt(i));
		pDoc->m_KWVisible.SetSize(t_KWVisible.GetSize());
		for(i=0;i<t_KWVisible.GetSize();i++) pDoc->m_KWVisible.SetAt(i,t_KWVisible.GetAt(i));
		pDoc->m_KWAttributes.SetSize(t_KWAttributes.GetSize());
		for(i=0;i<t_KWAttributes.GetSize();i++) pDoc->m_KWAttributes.SetAt(i,t_KWAttributes.GetAt(i));

		pDoc->m_kw_patn_generated_groups = t_kw_patn_generated_groups;
		pDoc->m_KWCutoff = t_KWCutoff;
		pDoc->m_KWExists = t_KWExists;
		pDoc->m_KWNumVisible = t_KWNumVisible;
		pDoc->m_KWOption = t_KWOption;

		return;
	}
	threashold = dlgKWThreashold.m_KWThreashold;

	pDoc->m_loading = true;

	long lastMoved = -1;
	m_pSelectedColumns->RemoveAll();

		//move smallest value < threshold to extrinsic
		for(i= 0;i < pDoc->m_KWValues.GetSize();i++) {
			if(pDoc->m_KWValues.GetAt(i) < threashold) {
				move_count++; 
				m_pSelectedColumns->SetAt(pDoc->m_KW[i]);
			}
		}

	if(move_count > 0) {
		OnTableColumnMakeExtrinsic();
	}

	//restore kw values if there was no move
	if(move_count > 0) {
		//invalidate analysis
	} else {
		//restore values
		pDoc->m_KW.SetSize(t_KW.GetSize());
		for(i=0;i<t_KW.GetSize();i++) pDoc->m_KW.SetAt(i,t_KW.GetAt(i));
		pDoc->m_KWValues.SetSize(t_KWValues.GetSize());
		for(i=0;i<t_KWValues.GetSize();i++) pDoc->m_KWValues.SetAt(i,t_KWValues.GetAt(i));
		pDoc->m_KWVisible.SetSize(t_KWVisible.GetSize());
		for(i=0;i<t_KWVisible.GetSize();i++) pDoc->m_KWVisible.SetAt(i,t_KWVisible.GetAt(i));
		pDoc->m_KWAttributes.SetSize(t_KWAttributes.GetSize());
		for(i=0;i<t_KWAttributes.GetSize();i++) pDoc->m_KWAttributes.SetAt(i,t_KWAttributes.GetAt(i));

		pDoc->m_kw_patn_generated_groups = t_kw_patn_generated_groups;
		pDoc->m_KWCutoff = t_KWCutoff;
		pDoc->m_KWExists = t_KWExists;
		pDoc->m_KWNumVisible = t_KWNumVisible;
		pDoc->m_KWOption = t_KWOption;
	}

	pDoc->m_loading = false;

	RedrawWindow();
}

void CTableView::OnUpdateDataVariablesKwToExtrinsics(CCmdUI *pCmdUI)
{
	long rows = GetDocument()->GetIdxTopExtrinsicRow();
	if(rows == -1) {
		rows = GetDocument()->GetNoOfRows();
	}
	for(long i=0;i<rows;i++) {
		if(GetDocument()->m_aRowGroups[i] < 0) {
			pCmdUI->Enable(false);
			return;
		}
	}
	pCmdUI->Enable(true);
}

void CTableView::OnDataConstancyandfidelity()
{
	CConstancyFidelity dlg(GetDocument(), this);

	dlg.DoModal();
}
void CTableView::OnUpdateDataConstancyandfidelity(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_Groups.GetSize() > 1 || GetDocument()->m_aAprioriGroupColours.GetSize() > 1);	
}

void CTableView::OnSelectionsClear()
{
	m_pSelectedColumns->RemoveAll();
	m_pSelectedRows->RemoveAll();
}

void CTableView::OnUpdateSelectionsClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}
void CTableView::OnSelectionsSave()
{

	//CSelectionName dlg;
	//if(dlg.DoModal() == IDCANCEL) {
//		return;
//	}

	Sel * s = new Sel();
	s->c = m_pSelectedColumns->Copy();
	s->r = m_pSelectedRows->Copy();
//	s->name = "test";

//	CString name;
//	dlg.m_name.GetWindowText(name);

//	if(m_aSavedSelections.Size() == 6) {
//		delete m_aSavedSelections[0]->c;
//		delete m_aSavedSelections(0]->r;
//		m_aSavedSelections.RemoveAt(0);
//	}

	m_aSavedSelections.Add(s);
}

void CTableView::OnUpdateSelectionsSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}

void CTableView::SelectionActivate(int n){
	//delete current selection
	delete m_pSelectedColumns;
	delete m_pSelectedRows;

	m_pSelectedColumns = m_aSavedSelections.GetAt(n)->c->Copy();
	m_pSelectedRows = m_aSavedSelections.GetAt(n)->r->Copy();

	//redraw
	RedrawWindow();
	CPatnDoc* pDoc = (CPatnDoc*) GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = pDoc->GetNextView(pos);   
		if(//pView->IsKindOf(RUNTIME_CLASS(C3DView)) ||
			pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)) ||
			pView->IsKindOf(RUNTIME_CLASS(C3DXView)))// ||
			//pView->IsKindOf(RUNTIME_CLASS(CScat)))
			pView->RedrawWindow();
	}   
}
void CTableView::OnSelection1Activate()
{
	if(m_aSavedSelections.GetAt(0) != NULL) {
		SelectionActivate(0);
	}
}

void CTableView::OnUpdateSelection1Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(0) != NULL);
}

void CTableView::OnSelection2Activate()
{
	if(m_aSavedSelections.GetAt(1) != NULL) {
		SelectionActivate(1);
	}
}

void CTableView::OnUpdateSelection2Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(1) != NULL);
}
void CTableView::OnSelection3Activate()
{
	if(m_aSavedSelections.GetAt(2) != NULL) {
		SelectionActivate(2);
	}
}

void CTableView::OnUpdateSelection3Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(2) != NULL);
}
void CTableView::OnSelection4Activate()
{
	if(m_aSavedSelections.GetAt(3) != NULL) {
		SelectionActivate(3);
	}
}

void CTableView::OnUpdateSelection4Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(3) != NULL);
}
void CTableView::OnSelection5Activate()
{
	if(m_aSavedSelections.GetAt(4) != NULL) {
		SelectionActivate(4);
	}
}

void CTableView::OnUpdateSelection5Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(4) != NULL);
}
void CTableView::OnSelection6Activate()
{
	if(m_aSavedSelections.GetAt(5) != NULL) {
		SelectionActivate(5);
	}
}

void CTableView::OnUpdateSelection6Activate(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_aSavedSelections.GetAt(5) != NULL);
}

void CTableView::OnSelection1SaveCurrentSelection()
{
	SelectionSaveCurrentSelection(0);
}

void CTableView::OnUpdateSelection1SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}

void CTableView::OnSelection2SaveCurrentSelection()
{
		SelectionSaveCurrentSelection(1);

}

void CTableView::OnUpdateSelection2SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}
void CTableView::OnSelection3SaveCurrentSelection()
{
		SelectionSaveCurrentSelection(2);
}

void CTableView::OnUpdateSelection3SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}
void CTableView::OnSelection4SaveCurrentSelection()
{
		SelectionSaveCurrentSelection(3);
}

void CTableView::OnUpdateSelection4SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}
void CTableView::OnSelection5SaveCurrentSelection()
{
		SelectionSaveCurrentSelection(4);
}

void CTableView::OnUpdateSelection5SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}
void CTableView::OnSelection6SaveCurrentSelection()
{
		SelectionSaveCurrentSelection(5);
}

void CTableView::OnUpdateSelection6SaveCurrentSelection(CCmdUI * pCmdUI) {
	pCmdUI->Enable(m_pSelectedColumns->GetCount() || m_pSelectedRows->GetCount());
}

void CTableView::SelectionSaveCurrentSelection(int n) {
	if(m_aSavedSelections.GetAt(n) != NULL) {
		delete m_aSavedSelections.GetAt(n)->c;
		delete m_aSavedSelections.GetAt(n)->r;
	}
	Sel * s = new Sel();
	s->c = m_pSelectedColumns->Copy();
	s->r = m_pSelectedRows->Copy();
	m_aSavedSelections.SetAt(n,s);
}

void CTableView::OnDataAssigngroupstonon()
{
	CAssignGroups dlg(GetDocument());
	dlg.DoModal();
}

void CTableView::OnUpdateDataAssigngroupstonon(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_Groups.GetSize() > 1);
}
