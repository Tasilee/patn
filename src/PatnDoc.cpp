// PatnDoc.cpp : implementation of the CPatnDoc class
//

#include "stdafx.h"
#include "patn.h"

#include "mainframe.h"

#include "PatnDoc.h"
#include "CntrItem.h"
//#include "SrvrItem.h"
#include "PageNewTableOptions.h"
#include "TwayOptions.h"
//#include "GstaOptions.h"
#include "Colr.h"
#include "Mst.h"
#include "Gsta.h"
#include "dend.h"
#include "odbcconnection.h"
#include "association.h"
#include "pcc.h"
#include "fuse.h"
#include "ordination.h"
#include "patternselection.h"
#include "kruskalwalice.h"
#include "kwoptions.h"
#include "tableview.h"
#include "MCAOResults.h"
#include "KWResults.h"
#include "AnosimResults.h"
#include "viewhistorydlg.h"
#include <math.h>
#include "anosim.h"
#include "pickanosim.h"
#include "tableframe.h"
//#include "gstaview.h"
#include "tway.h"
//#include "3dview.h"
#include "3dxview.h"
//#include "scat.h"
#include "boxwhiskerview.h"
#include "anosimview.h"
#include "assoswnd.h"
#include "fuseview.h"
#include "fuseswnd.h"
#include "assohist.h"
#include "dendview.h"
#include "appendoptiosn.h"

#include "selectworksheet.h"

//#include "tableproperties.h"
#include "propertiesgeneral.h"
#include "propertiesvisiblestatistics.h"
#include "propertiesordinationview.h"
#include "propertiesfont.h"
#include "propertiesbwobjects.h"

#include "groupsdlg.h"
#include "autoevaluation.h"
#include "evaluation.h"
#include "classification.h"
#include <ddraw.h>
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>
#include <dmusici.h>
#include ".\patndoc.h"

#include "analysis.h"

#include "constfidview.h"
#include "constancyfidelity.h"
#include "tableview.h"
#include "importrelationdatapicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// helper functions


long Record(long value)
{
	long ret = false;

	switch(value){
	case 0x0221: //ARRAY
	case 0x0201: //BLANK	
	case 0x0205: //BOOLERR
	case 0x0406: //FORMULA
	case 0x0204: //LABEL
	case 0x0203: //NUMBER -- not sure how to interpret IEEE numbers yet.
	case 0x00be: //MULBLANK
	case 0x00bd: //MULRK
	case 0x027e: //RK
	case 0x00d6: //RSTRING
	case 0x00bc: //SHRFMLA
	case 0x0207: //STRING -- called after formula?
	case 0x00fd: //STRING -- undocumented in BIFF7
	case 0x0006: //????
		ret = true;
		break;
	default: //Code not recognized!
		break;
	}	
	return ret;	
}

void sort(CArray<int>* pData, long bAscending /*= TRUE*/)
{
	long i, j, pos;
	long N = pData->GetSize();
	long temp;
	
	for ( i = 0; i < (N - 1); i++ )
	{
		pos = i;
		for ( j = i+1; j < N; j++ )
		{
			switch(bAscending)
			{
				case TRUE:
					if ( pData->GetAt(j) < pData->GetAt(pos) )
						pos = j;
					break;
				case FALSE:
					if ( pData->GetAt(j) > pData->GetAt(pos) )
						pos = j;
					break;
			}
		}

		if ( pos != i )
		{
			temp = pData->GetAt(i);
			pData->SetAt(i, pData->GetAt(pos));
			pData->SetAt(pos, temp);
		}
	}
}

void sort(CArray<double>* pData, CArray<long> * idxs,long bAscending)
{
	long i, j, pos;
	long N = pData->GetSize();
	long temp;
	
	for ( i = 0; i < (N - 1); i++ )
	{
		pos = i;
		for ( j = i+1; j < N; j++ )
		{
			switch(bAscending)
			{
				case TRUE:
					if ( pData->GetAt(idxs->GetAt(j)) < pData->GetAt(idxs->GetAt(pos)) )
						pos = j;
					break;
				case FALSE:
					if ( pData->GetAt(idxs->GetAt(j)) > pData->GetAt(idxs->GetAt(pos)) )
						pos = j;
					break;
			}
		}

		if ( pos != i )
		{
			temp = idxs->GetAt(i);
			idxs->SetAt(i, idxs->GetAt(pos));
			idxs->SetAt(pos, temp);
		}
	}
}

long is_contiguous(CArray<long>* pData)
{
	long i;
	long N = pData->GetSize();
	
	for ( i = 0; i < (N - 1); i++ )
	{
		if ( pData->GetAt(i + 1) != pData->GetAt(i) + 1 )
			return FALSE;
	}

	return TRUE;
}

                                                                              
	
/////////////////////////////////////////////////////////////////////////////
// class CRowColumnAttributes

IMPLEMENT_SERIAL(CRowColumnAttributes,CObject,0)

CRowColumnAttributes::CRowColumnAttributes()
{
	m_bIsHidden = FALSE;
	m_nType = CRowColumnAttributes::Data;
}

void CRowColumnAttributes::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_bIsHidden;
		ar << m_nType;
		ar << m_strLabel;
		ar << m_strGroup;
	}
	else
	{
		ar >> m_bIsHidden;
		ar >> m_nType;
		ar >> m_strLabel;
		ar >> m_strGroup;
	}
}

void CRowColumnAttributes::SetType(UINT nType)
{
	m_nType = nType;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class CRowAttributes
#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<CRowAttributes*>(CArchive& ar,CRowAttributes** ppRowAttributes, int nCount)
#else
void SerializeElements(CArchive& ar, CRowAttributes** ppRowAttributes, int nCount)
#endif
{
	if ( ar.IsStoring() )
	{
		for ( long i = 0; i < nCount; i++, ppRowAttributes++ )
		{
			CRowAttributes* pRowAttributes = *(ppRowAttributes);
			pRowAttributes->Serialize( ar );
		}
	}
	else
	{
		for ( long i = 0; i < nCount; i++, ppRowAttributes++ )
		{
			CRowAttributes* pRowAttributes = new CRowAttributes;
			*(ppRowAttributes) = pRowAttributes;
			pRowAttributes->Serialize( ar );
		}
	}
}

IMPLEMENT_SERIAL(CRowAttributes,CObject,0)  

CRowAttributes::CRowAttributes()
{
	m_nHeight = 12.75; // not used - height is set by view based on font size
}

void CRowAttributes::SetHeight(double nHeight)
{
	if ( nHeight <= 0.0 )
		m_bIsHidden = TRUE;
	else
		m_nHeight = nHeight;
}

void CRowAttributes::Serialize(CArchive& ar)
{
	CRowColumnAttributes::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_nHeight;
	}
	else
	{
		ar >> m_nHeight;
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class CColumnAttributes

#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<CColumnAttributes*>(CArchive& ar,CColumnAttributes** ppColumnAttributes, int nCount)
#else
void SerializeElements(CArchive& ar, CColumnAttributes** ppColumnAttributes, int nCount)
#endif
{
	if ( ar.IsStoring() )
	{
		for ( long i = 0; i < nCount; i++, ppColumnAttributes++ )
		{
			CColumnAttributes* pColumnAttributes = *(ppColumnAttributes);
			pColumnAttributes->Serialize( ar );
		}
	}
	else
	{
		for ( long i = 0; i < nCount; i++, ppColumnAttributes++ )
		{
			CColumnAttributes* pColumnAttributes = new CColumnAttributes;
			*(ppColumnAttributes) = pColumnAttributes;
			pColumnAttributes->Serialize( ar );
		}
	}
}

IMPLEMENT_SERIAL(CColumnAttributes,CObject,0)  
CColumnAttributes::CColumnAttributes()
{
	m_bIsStandardWidth = TRUE;
	m_nWidth = g_nStandardColumnWidth;
	m_nDecimalPrecision = g_nDefaultDecimalPrecision;
}

void CColumnAttributes::SetWidth(double nWidth)
{
	if ( nWidth <= 0.0 )
		m_bIsHidden = TRUE;
	else
	{
		m_nWidth = nWidth;
		if ( m_nWidth == g_nStandardColumnWidth )
			m_bIsStandardWidth = TRUE;
		else
			m_bIsStandardWidth = FALSE;
	}
}

void CColumnAttributes::SetWidthStandard()
{
	m_bIsStandardWidth = TRUE;
	m_nWidth = g_nStandardColumnWidth;
}

void CColumnAttributes::SetDecimalPrecision(long nDecimalPrecision)
{
	if ( nDecimalPrecision < 0 )
		nDecimalPrecision = 0;

	m_nDecimalPrecision = nDecimalPrecision;
}

void CColumnAttributes::Serialize(CArchive& ar)
{
	CRowColumnAttributes::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_nWidth;
		ar << m_bIsStandardWidth;
		ar << m_nDecimalPrecision;
	}
	else
	{
		ar >> m_nWidth;
		ar >> m_bIsStandardWidth;
		ar >> m_nDecimalPrecision;
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CAssocMatrix

IMPLEMENT_SERIAL(CAssocMatrix,CObject,1)

CAssocMatrix::CAssocMatrix()
{
	m_nDecMax = 0;
	m_strMeasure.Empty();
}

void CAssocMatrix::Reset()
{
	CAssocMatrixRow* pAMR;
	long i;

	for ( i = 0; i < m_aMatrix.GetSize(); i++ )
	{
		pAMR = m_aMatrix[i];
		if(pAMR != NULL)
		{
			pAMR->RemoveAll();
			delete pAMR;
		}
		//m_aMatrix.RemoveAt(i);
	}
	m_aMatrix.SetSize(0);
	m_nDecMax = 0;
	m_strMeasure.Empty();
}


void CAssocMatrix::Serialize(CArchive& ar)
{
	CAssocMatrixRow* pAMR;
	long nRowCount, nColCount;
	double nValue;
	long i,j;

	CObject::Serialize(ar);

	if ( ar.IsStoring())
	{
		nRowCount = m_aMatrix.GetSize();
		ar << nRowCount;
		for ( i = 0; i < nRowCount; i++ )
		{
			pAMR = m_aMatrix.GetAt(i);
			nColCount = pAMR->GetSize();
			ar << nColCount;
			for	( j = 0; j < nColCount; j++ )
			{
				ar << pAMR->GetAt(j);
			}
		}
		ar << m_nDecMax;
		ar << m_strMeasure;
		ar << m_Time;
	}
	else
	{
		ar >> nRowCount;
		m_aMatrix.SetSize(nRowCount);
		for ( i = 0; i < nRowCount; i++ )
		{
			pAMR = new CAssocMatrixRow;
			ar >> nColCount;
			pAMR->SetSize(nColCount);
			for ( j = 0; j < nColCount; j++ )
			{
				ar >> nValue;
				pAMR->SetAt(j,nValue);
			}
			m_aMatrix.SetAt(i,pAMR);
		}
		ar >> m_nDecMax;
		ar >> m_strMeasure;
		ar >> m_Time;
	}
}
/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<ROW*>(CArchive& ar,ROW** ppRow, int nCount)
#else
void SerializeElements(CArchive& ar, ROW** ppRow, int nCount)
#endif
{
	if ( ar.IsStoring() )
	{
		for ( long i = 0; i < nCount; i++, ppRow++ )
		{
			ROW* pRow = *(ppRow);
			pRow->Serialize(ar);
		}
	}
	else
	{
		for ( long i = 0; i < nCount; i++, ppRow++ )
		{
			ROW* pRow = new ROW;
			*(ppRow) = pRow;
			pRow->Serialize(ar);
		}
	}
}
  
double CPatnDoc::GetSparcityRatio()
{
	CWaitCursor waitCursor;

	long col, row;
	long nCols,nRows;
	long nCountOfZeros;
	double nSparcityRatio;

	if ( IsExtrinsicColumns() )
		nCols = m_idxLeftExtrinsicColumn;
	else
		nCols = GetNoOfColumns();

	if ( IsExtrinsicRows() )
		nRows = m_idxTopExtrinsicRow;
	else
		nRows = GetNoOfRows();

	nCountOfZeros = 0;
	for ( col = 0; col < nCols; col++ )
	{
		for ( row = 0; row < nRows; row++ )
		{
			if ( GetValueAt(col,row) == 0.0 && 0.0 != GetMissingValue() )
				nCountOfZeros++;
		}
	}

	nSparcityRatio = (double)nCountOfZeros*100/(nCols*nRows);

	return nSparcityRatio;
}

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc
//ON_COMMAND(ID_VIEW_GSTA, OnViewGsta)ON_UPDATE_COMMAND_UI(ID_VIEW_GSTA, OnUpdateViewGsta)
IMPLEMENT_DYNCREATE(CPatnDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CPatnDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CPatnDoc)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ASSOCIATIONMATRIX, OnUpdateViewAssociationMatrix)
	ON_COMMAND(ID_VIEW_ASSOCIATIONMATRIX, OnViewAssociationMatrix)
	ON_COMMAND(ID_SHOWTABLE, OnViewDataTable)
	ON_COMMAND(ID_VIEW_FUSIONDETAILS, OnViewFusionDetails)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FUSIONDETAILS, OnUpdateViewFusionDetails)
	ON_COMMAND(ID_TOOLS_HISTOGRAM, OnToolsHistogram)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HISTOGRAM, OnUpdateToolsHistogram)
	ON_COMMAND(ID_VIEW_HISTOGRAM, OnViewHistogram)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTOGRAM, OnUpdateViewHistogram)
	ON_COMMAND(ID_VIEW_TWAYTABLE, OnViewTwaytable)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TWAYTABLE, OnUpdateViewTwaytable)	
	ON_COMMAND(ID_FILE_ODBCCONNECTION, OnFileOdbcconnection)
	ON_COMMAND(ID_FILE_EXPORT_COLUMNAFFINITYVECTOR, OnFileExportColumnaffinityvector)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_COLUMNAFFINITYVECTOR, OnUpdateFileExportColumnaffinityvector)
	ON_COMMAND(ID_FILE_EXPORT_COLUMNANISMRESULTS, OnFileExportColumnanismresults)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_COLUMNANISMRESULTS, OnUpdateFileExportColumnanismresults)
	ON_COMMAND(ID_FILE_EXPORT_COLUMNASSOCIATIONMATRIX, OnFileExportColumnassociationmatrix)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_COLUMNASSOCIATIONMATRIX, OnUpdateFileExportColumnassociationmatrix)
	ON_COMMAND(ID_FILE_EXPORT_COLUMNFUSIONTABLE, OnFileExportColumnfusiontable)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_COLUMNFUSIONTABLE, OnUpdateFileExportColumnfusiontable)
	ON_COMMAND(ID_FILE_EXPORT_ORDINATIONCOORDINATES, OnFileExportOrdinationcoordinates)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ORDINATIONCOORDINATES, OnUpdateFileExportOrdinationcoordinates)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_PCCRESULTS, OnUpdateFileExportPccresults)
	ON_COMMAND(ID_FILE_EXPORT_PCCRESULTS, OnFileExportPccresults)
	ON_COMMAND(ID_FILE_EXPORT_ROWAFFINITYVECTOR, OnFileExportRowaffinityvector)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ROWAFFINITYVECTOR, OnUpdateFileExportRowaffinityvector)
	ON_COMMAND(ID_FILE_EXPORT_ROWANISMRESULTS, OnFileExportRowanismresults)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ROWANISMRESULTS, OnUpdateFileExportRowanismresults)
	ON_COMMAND(ID_FILE_EXPORT_ROWASSOCATIONMATRIX, OnFileExportRowassocationmatrix)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ROWASSOCATIONMATRIX, OnUpdateFileExportRowassocationmatrix)
	ON_COMMAND(ID_FILE_EXPORT_ROWFUSIONTABLE, OnFileExportRowfusiontable)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ROWFUSIONTABLE, OnUpdateFileExportRowfusiontable)
	ON_COMMAND(ID_FILE_LOAD_COLUMNAFFINITYVECTOR, OnFileLoadColumnaffinityvector)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_COLUMNAFFINITYVECTOR, OnUpdateFileLoadColumnaffinityvector)
	ON_COMMAND(ID_FILE_LOAD_ROWAFFINITYVECTOR, OnFileLoadRowaffinityvector)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_ROWAFFINITYVECTOR, OnUpdateFileLoadRowaffinityvector)
	ON_COMMAND(ID_FILE_EXPORT_TABLEANDINFOTABS, OnFileExportTableandinfotabs)
	ON_COMMAND(ID_FILE_LOAD_ORDINATION, OnFileLoadOrdination)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_ORDINATION, OnUpdateFileLoadColumnaffinityvector)
	ON_COMMAND(ID_TABLE_PATTERNSOBJECTS, OnTablePatternsobjects)
	ON_COMMAND(ID_FILE_EXPORT_GROUPSTATISTICS, OnFileExportGroupstatistics)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_GROUPSTATISTICS, OnUpdateFileExportGroupstatistics)
	ON_COMMAND(ID_FILE_EXPORT_KRUSKALWALLICESTATS, OnFileExportKruskalwallicestats)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_KRUSKALWALLICESTATS, OnUpdateFileExportKruskalwallicestats)
	ON_COMMAND(ID_FILE_EXPORT_MCAORAWDATA, OnFileExportMcaorawdata)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_MCAORAWDATA, OnUpdateFileExportMcaorawdata)
	ON_COMMAND(ID_VIEW_KRUSKALWALLISRESULTS, OnViewKruskalwallisresults)
	ON_UPDATE_COMMAND_UI(ID_VIEW_KRUSKALWALLISRESULTS, OnUpdateViewKruskalwallisresults)
	ON_COMMAND(ID_VIEW_MCAO, OnViewMcao)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MCAO, OnUpdateViewMcao)
	ON_COMMAND(ID_VIEW_ANOSIMRESULTS, OnViewAnosimresults)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANOSIMRESULTS, OnUpdateViewAnosimresults)
	ON_COMMAND(ID_FILE_EXPORT_MCAORESULTS, OnFileExportMcaoresults)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_MCAORESULTS, OnUpdateFileExportMcaoresults)
	ON_COMMAND(ID_FILE_EXPORT_HISTORY, OnFileExportHistory)
	ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
	ON_COMMAND(ID_FILE_EXPORT_ANOSIMGROUPPAIRAFFINITYVECTORS, OnFileExportAnosimgrouppairaffinityvectors)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ANOSIMGROUPPAIRAFFINITYVECTORS, OnUpdateFileExportAnosimgrouppairaffinityvectors)
	ON_COMMAND(ID_FILE_EXPORT_ANOSIMGROUPPAIRRESULTS, OnFileExportAnosimgrouppairresults)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_ANOSIMGROUPPAIRRESULTS, OnUpdateFileExportAnosimgrouppairresults)
	ON_COMMAND(ID_FILE_EXPORT_SITECOUNT, OnFileExportSitecount)
	ON_COMMAND(ID_VIEW_BOXWHISKERSTATISTICS, OnViewBoxwhiskerstatistics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOXWHISKERSTATISTICS, OnUpdateViewBoxwhiskerstatistics)
	ON_COMMAND(ID_VIEW_3DGRAPH_DIRECTX70A, OnView3dgraphDirectx70a)
	ON_UPDATE_COMMAND_UI(ID_VIEW_3DGRAPH_DIRECTX70A, OnUpdateView3dgraphDirectx70a)
	ON_COMMAND(ID_CONTEXTMENU, OnContextmenu)
	ON_UPDATE_COMMAND_UI(ID_CONTEXTMENU, OnUpdateContextmenu)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_TABLE_PROPERTIES, OnTableProperties)
	ON_COMMAND(ID_DATA_CONTROLGROUPS, OnDataControlgroups)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
	ON_COMMAND(ID_VIEW_PCCRESULTS, OnViewPccresults)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PCCRESULTS, OnUpdateViewPccresults)
	ON_COMMAND(ID_VIEW_SCATTERPLOT33034, OnViewScatterplot33034)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCATTERPLOT33034, OnUpdateViewScatterplot33034)
	ON_COMMAND(ID_IMPORT_APRIORIROWGROUPCOMPOSITION, OnImportApriorirowgroupcomposition)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_APRIORIROWGROUPCOMPOSITION, OnUpdateImportanalysisdataColumnlower)
	ON_COMMAND(ID_EXPORTANALYSISDATA_APRIORIROWGROUPCOMPOSITION, OnExportanalysisdataApriorirowgroupcomposition)
	ON_UPDATE_COMMAND_UI(ID_EXPORTANALYSISDATA_APRIORIROWGROUPCOMPOSITION, OnUpdateExportanalysisdataApriorirowgroupcomposition)
	ON_UPDATE_COMMAND_UI(ID_IMPORTANALYSISDATA_COLUMNLOWER, OnUpdateImportanalysisdataColumnlower)
	ON_COMMAND(ID_IMPORTANALYSISDATA_COLUMNLOWER, OnImportanalysisdataColumnlower)
	ON_COMMAND(ID_IMPORTANALYSISDATA_ROWLOWER, OnImportanalysisdataRowlower)
	ON_UPDATE_COMMAND_UI(ID_IMPORTANALYSISDATA_ROWLOWER, OnUpdateImportanalysisdataRowlower)
	ON_COMMAND(ID_IMPORTANALYSISDATA_COLUMNDENDOGRAMORDER, OnImportanalysisdataColumndendogramorder)
	ON_COMMAND(ID_IMPORTANALYSISDATA_ROWDENDOGRAMORDER, OnImportanalysisdataRowdendogramorder)
	ON_UPDATE_COMMAND_UI(ID_IMPORTANALYSISDATA_COLUMNDENDOGRAMORDER, OnUpdateImportanalysisdataColumnlower)
	ON_UPDATE_COMMAND_UI(ID_IMPORTANALYSISDATA_ROWDENDOGRAMORDER, OnUpdateImportanalysisdataColumnlower)
	ON_COMMAND(ID_EXPORTEVALUATIONDATA_COLUMNGROUPSTATISTICS, OnExportevaluationdataColumngroupstatistics)
	ON_COMMAND(ID_DATA_CREATENEWVARIABLESREPRESENTINGGROUPS, OnDataCreatenewvariablesrepresentinggroups)
	ON_COMMAND(ID_VIEW_CONSTFID, OnViewConstfid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONSTFID, OnUpdateViewConstfid)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc construction/destruction

CPatnDoc::CPatnDoc()
{
	// Use OLE compound files
	EnableCompoundFile();

	m_visible_decimals = 2;

	m_ColSelection = NULL;
	m_RowSelection = NULL;
	m_Hist = NULL;
	m_pExists = 0;
	m_FuseRowVisible = 0;
	m_FuseColVisible = 0;
	m_AssocRowVisible = 0;
	m_AssocColVisible = 0;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowUpdate = 0;
	m_FuseColUpdate = 0;
	m_AssocRowUpdate = 0;
	m_AssocColUpdate = 0;
	m_OrdChange = 1;
	m_OrdUpdate = 0;
	m_OrdVisible = 0;
	m_OrdXVisible = 0;
	m_pGstaVisible = 0;
	m_GstaAvailable = 0;
	m_KWNumVisible = 5;
	m_KWOption = 2;
	m_KWCutoff = 0.5;
	m_KWExists = false;
	m_MCAOExists = false;
	m_ANOSIMRowsExists = false;
	m_ANOSIMColsExists = false;
	m_RowAsim_iterations = 100;
	m_ColumnAsim_iterations = 100;
	m_RowAsim_seed = 1245;
	m_ColumnAsim_seed = 1245;
	m_ColumnAsim_pairs = false;
	m_RowAsim_pairs = false;
	m_RowAsim_patn_groups = false;
	m_AnosimRowView = false;
	m_AnosimColView = false;
	m_boxwhiskervisible = 0;
	m_constfidvisible = 0;

	m_kw_patn_generated_groups = 0;

	m_LoadingFuseCol = m_LoadingFuseRow = 0;
	m_LoadingAssocCol = m_LoadingAssocRow = 0;
	m_LoadingOrd = 0;
	m_LoadingXOrd = 0;
	m_LoadingTable = 0;
	m_LoadingTway = 0;
	m_LoadingGsta = m_LoadingAnosimRow = 0;
	m_LoadingAnosimCol = m_LoadingBoxWhisker =0;
	
	m_MCAOIterations = 100;
	m_MCAOSeed = 1245;

	m_AnalysisRecipe.nFirstTime = true;

	m_3ddefault = false;
	m_loading = false;

	m_createKW = false;
	m_createMCAO = false;
	m_createANOSIMRows = false;
	m_createANOSIMCols = false;
	m_createPCC = false;

	m_mst_changed = false;

	m_import_ord = false;
}

CPatnDoc::~CPatnDoc()
{
	for(long i=0;i<m_aData.GetSize();i++)
		delete m_aData[i];
	for(i=0;i<m_aRowAttributes.GetSize();i++)
		delete m_aRowAttributes[i];
	for(i=0;i<m_aColumnAttributes.GetSize();i++)
		delete m_aColumnAttributes[i];
	for(i=0;i<m_aColumnTotalsRowAttributes.GetSize();i++)
		delete m_aColumnTotalsRowAttributes[i];
	for(i=0;i<m_aColumnAllocRowAttributes.GetSize();i++)
		delete m_aColumnAllocRowAttributes[i];
	for(i=0;i<m_aColumnCompareRowAttributes.GetSize();i++)
		delete m_aColumnCompareRowAttributes[i];
	for(i=0;i<m_aColumnPccAttributes.GetSize();i++)
		delete m_aColumnPccAttributes[i];
	for(i=0;i<m_aRowTotalsColumnAttributes.GetSize();i++)
		delete m_aRowTotalsColumnAttributes[i];
	for(i=0;i<m_aRowAllocColumnAttributes.GetSize();i++)
		delete m_aRowAllocColumnAttributes[i];
	for(i=0;i<m_aRowOrdColumnAttributes.GetSize();i++)
		delete m_aRowOrdColumnAttributes[i];
	for(i=0;i<m_Vectors.GetSize();i++)
		delete m_Vectors[i];

	for(i=0;i<m_AssocMatrixRows.m_aMatrix.GetSize();i++)
		delete m_AssocMatrixRows.m_aMatrix[i];

	for(i=0;i<m_AssocMatrixCols.m_aMatrix.GetSize();i++)
		delete m_AssocMatrixCols.m_aMatrix[i];

	for(i=0;i<m_DendTableCols.GetSize();i++)
		delete m_DendTableCols[i];
	for(i=0;i<m_DendTableRows.GetSize();i++)
		delete m_DendTableRows[i];
	for(i=0;i<m_GroupDendTableCols.GetSize();i++)
		delete m_GroupDendTableCols[i];
	for(i=0;i<m_GroupDendTableRows.GetSize();i++)
		delete m_GroupDendTableRows[i];
	for(i=0;i<m_FuseTableRows.GetSize();i++)
		delete m_FuseTableRows[i];
	for(i=0;i<m_FuseTableCols.GetSize();i++)
		delete m_FuseTableCols[i];
	
	for(i=0;i<m_aMst.GetSize();i++)
		delete m_aMst.GetAt(i);

	for(i=0;i<m_Rows.GetSize();i++)
		delete m_Rows[i];
	for(i=0;i<m_Cols.GetSize();i++)
		delete m_Cols[i];
	for(i=0;i<m_RowControlGroups.GetSize();i++)
		delete m_RowControlGroups[i];
	for(i=0;i<m_ColControlGroups.GetSize();i++)
		delete m_ColControlGroups[i];

	
	for(i=0;i<m_Gstastats.GetSize();i++){
		for(long o=0;o<m_Gstastats[i]->GetSize();o++)
			delete m_Gstastats[i]->GetAt(o);
		delete m_Gstastats[i];	
	}

	for(i=0;i<m_MCAOTable.GetSize();i++)
		delete m_MCAOTable[i];

	for(i=0;i<m_RowPairBest.GetSize();i++){
		delete m_RowPairBest[i];
		delete m_RowPairStat[i];
		delete m_RowPairSave[i];
	}
	for(i=0;i<m_ColPairBest.GetSize();i++){
		delete m_ColPairBest[i];
		delete m_ColPairStat[i];
		delete m_ColPairSave[i];
	}
}

int CPatnDoc::OnNewDocument()
{
	
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_idxLeftExtrinsicColumn = -1;
	m_idxTopExtrinsicRow = -1;
	m_nMissingValue = ANALYSIS_VALUE;
	m_pIsRowLabel = 0;
	m_TwayChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_OrdChange = 1;


	// import data from external source?
	switch(((CPatnApp*)AfxGetApp())->IsGetExternalData())
	{
		case TRUE:
			if(((CPatnApp*)AfxGetApp())->IsGetOdbc()){
				if(ODBCConnect())
					return COleServerDoc::OnNewDocument();		
				else
					return FALSE;
			}else{
				if ( !CreateTableFromExternalSource() )
				{
					//COleServerDoc::OnCloseDocument();
					return FALSE;
				}
			}
			break;

		case FALSE:
			if ( !CreateNewTable() )
			{
				//COleServerDoc::OnCloseDocument();
				return FALSE;
			}
			break;
	}

	// create fonts
	SetDefaultFontsAll();

	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		pView->RedrawWindow();
	}   

	UpdateGroups();
	CGsta cgsta(this);	

	if (!COleServerDoc::OnNewDocument())
		return FALSE;


	return TRUE;
}

int CPatnDoc::CreateNewTable()
{
	long nNoOfColumns, nNoOfRows;
	double nColumnWidth;
	long nWhichColumnWidth;
	long nWhichInitialValues;
	long nDecimalPrecision;

	m_TwayChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_OrdChange = 1;

	m_loading = 1;

	// prepare dialog box to get options for creation of table
	CPropertySheet * ps = new CPropertySheet("Create new PATN table",NULL,0);
	CPageNewTableOptions * pageNewTableOptions = new CPageNewTableOptions();

	pageNewTableOptions->m_nMissingValue = m_nMissingValue;
	pageNewTableOptions->m_nNoOfRows = 50;
	pageNewTableOptions->m_nNoOfColumns = 20;
	pageNewTableOptions->m_nWhichColumnWidth = CPageNewTableOptions::standard;
	pageNewTableOptions->m_nColumnWidth = g_nStandardColumnWidth;
	pageNewTableOptions->m_nWhichInitialValues = CPageNewTableOptions::missingValue;
	pageNewTableOptions->m_nDecimalPrecision = g_nDefaultDecimalPrecision;

	ps->AddPage(pageNewTableOptions);

	ps->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// display options dialog box
	if ( ps->DoModal() == IDCANCEL )
	{
		ps->RemovePage(0);
		delete ps;
		delete pageNewTableOptions;
		return FALSE;
	}

	m_nRandomSeed = pageNewTableOptions->m_random_seed;

	// retrieve values from dialog box
	m_nMissingValue = pageNewTableOptions->m_nMissingValue;

	nNoOfRows = pageNewTableOptions->m_nNoOfRows;
	if ( nNoOfRows < 1 )
		nNoOfRows = 1;

	nNoOfColumns = pageNewTableOptions->m_nNoOfColumns;
	if ( nNoOfColumns < 1 )
		nNoOfColumns = 1;

	nWhichColumnWidth = pageNewTableOptions->m_nWhichColumnWidth;
	switch (nWhichColumnWidth)
	{
		case CPageNewTableOptions::standard:
			nColumnWidth = g_nStandardColumnWidth;
			break;

		case CPageNewTableOptions::custom:
			nColumnWidth = pageNewTableOptions->m_nColumnWidth;
			break;
	}

	nWhichInitialValues = pageNewTableOptions->m_nWhichInitialValues;

	nDecimalPrecision = pageNewTableOptions->m_nDecimalPrecision;
	if ( nDecimalPrecision < 0 )
		nDecimalPrecision = 0;

	ps->RemovePage(0);
	delete ps;
	delete pageNewTableOptions;

	BeginWaitCursor();

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	// fill table with data
	switch (nWhichInitialValues)
	{
		case CPageNewTableOptions::zero:
			FillTableWithZeros(nNoOfColumns, nNoOfRows);
			break;

		case CPageNewTableOptions::missingValue:
			FillTableWithMissingValues(nNoOfColumns, nNoOfRows);
			break;

		case CPageNewTableOptions::random:
			FillTableWithRandomData(nNoOfColumns, nNoOfRows);
			break;
	}

	EndWaitCursor();
	m_loading = 0;
	
	UpdateIntrinsicTotals();
	UpdateAllTotals();
	return TRUE;
}

int CPatnDoc::CreateTableFromExternalSource()
{

	CString strFile;
	CString extension;
	long nfileType;
	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		CFileDialog dlg(TRUE,NULL,"*.csv;*.xls;*.xlsx",OFN_HIDEREADONLY,
			"All Supported Types (*.csv,*.xls,*.xlsx)|*.csv;*.xls;*.xlsx|Comma Seperated Files (*.csv)|*.csv|Excel Files (*.xls,*.xlsx)|*.xls;*.xlsx|Relational Data (*.csv)|*.csv|Association Matric (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
		if ( dlg.DoModal() != IDOK )
			return FALSE;
		strFile = dlg.GetPathName();

		
		if(dlg.m_pOFN->nFilterIndex == 4)
		{
			nfileType = relational;
		}

		if(dlg.m_pOFN->nFilterIndex == 5)
		{
			nfileType = associationmatrix;
		}

	}
	else
	{
		strFile = ((CPatnApp*)AfxGetApp())->sm_data ;
	}
	CWaitCursor waitcursor;

	extension = strFile.Right(3);
	extension.MakeLower();

	if(extension.CompareNoCase("xls") == 0 || extension.CompareNoCase("lsx") == 0){
		nfileType = excel;
	}
	if(extension.CompareNoCase("txt") == 0){
		nfileType = text;
	}	
	if(extension.CompareNoCase("prm") == 0)
	{
		nfileType = dosprm;
	}
	if(extension.CompareNoCase("arc") == 0)
	{
		nfileType = dosarc;
	}
	//if(dlg.m_pOFN->nFilterIndex == 6)
	//{
	//	nfileType = relational;
	//}

		
	switch (nfileType)
	{

		case (excel):
			//if (!CreateTableFromExcelFile(strFile)){
			if(!CreateTableFromExcelFileByExcel(strFile)){
				return FALSE;
			}
			break;
		case (dosprm):
			if(!CreateTableFromDOSPRM(strFile))
			{
				return FALSE;
			}
			break;
		case (dosarc):
			if(!CreateTableFromDOSARC(strFile))
			{
				return FALSE;
			}
			break;
		case (relational):
			if(!CreateTableFromRelationalDataUserColumnSelect(strFile))
			{
				return FALSE;
			}
			break;
		case (associationmatrix):
			if (!CreateTableFromAssociationMatrix(strFile)) 
			{
				return FALSE;
			}
			break;
		default:
			if (!CreateTableFromTextFile(strFile)){
					return FALSE;
			}
			break;
	}

	if(((CPatnApp*)AfxGetApp())->silent_mode)
	{
		long find_it, fi2;

		CString xc, xr;
		long cc,rr;
		
			
		if((find_it = ((CPatnApp*)AfxGetApp())->sm_recipe.Find("-XC ")) >=0 )
		{
			fi2 = ((CPatnApp*)AfxGetApp())->sm_recipe.Find(" ",find_it+4);
			if(fi2 >= 0)
			{
				xc = ((CPatnApp*)AfxGetApp())->sm_recipe.Mid(find_it+3,fi2-find_it-3);
				if((cc = atoi(xc))>0)
				{
					this->SetIdxLeftExtrinsicColumn(this->GetNoOfColumns() - cc);
					
				}
			}
		}
		if((find_it = ((CPatnApp*)AfxGetApp())->sm_recipe.Find("-XR ")) >=0 )
		{
			fi2 = ((CPatnApp*)AfxGetApp())->sm_recipe.Find(" ",find_it+4);
			if(fi2 >= 0)
			{
				xr = ((CPatnApp*)AfxGetApp())->sm_recipe.Mid(find_it+3,fi2-find_it-3);
				if((rr = atoi(xr))>0)
				{
					this->SetIdxTopExtrinsicRow(this->GetNoOfRows() - rr);
				}
			}
		}
			
		
		//import the recipe settings
		//TODO CORRECTLY
		m_AnalysisRecipe.nFirstTime = false;
		CAnalysisDlg dlg(this,NULL); //to initalize		
		//CRecipeHandler rh(&m_AnalysisRecipe,NULL);
		RunRecipe(((CPatnApp*)AfxGetApp())->sm_recipe);

		//run analysis & evaluation
        dlg.Initialize();
		dlg.m_bColumns = true;
		dlg.OnRun();
		
		if(((CPatnApp*)AfxGetApp())->sm_recipe.Find("-EA ") >=0 )
		{
			CAutoEvaluation ae(this);
			ae.Run();
			
		}else{
			CEvaluation e(this);
			e.init();
			e.run(NULL);
		}

		//export everything
		//-general
		this->OnFileExportTableandinfotabs();
		this->OnFileExportSitecount();
		this->OnFileExportColumnaffinityvector();
		this->OnFileExportRowaffinityvector();
		this->OnFileExportColumnassociationmatrix();
		this->OnFileExportRowassocationmatrix();
		this->OnFileExportColumnfusiontable();
		this->OnFileExportRowfusiontable();
		this->OnFileExportOrdinationcoordinates();
		this->OnFileExportColumnanismresults();
		this->OnFileExportRowanismresults();
		this->OnFileExportKruskalwallicestats();
		this->OnFileExportPccresults();
		this->OnFileExportMcaorawdata();
		this->OnFileExportMcaoresults();
		this->OnFileExportGroupstatistics();
		this->OnFileExportHistory();
		
		//assoc mx
		

//		fprintf(stdout, "%s",this->m_log);

		//done!
		return FALSE;
	}

	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
		{
			((CTableView*)pView)->autosizecolumns = true;
		}
		pView->RedrawWindow();
	}   

	 return TRUE;
}


long CPatnDoc::CreateTableFromTextFile(CString strFile)
{
	m_loading = true;
	CStdioFile io;
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long i;
	int left, right;
	long nColumnCount, nRowCount;
	CWaitCursor waitcursor;

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	nRowCount = 0;
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		nRowCount++;
	}

	if ( nRowCount < 2 )
	{
		AfxGetMainWnd()->MessageBox("No data rows in external file","Record count",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	io.SeekToBegin();
	io.ReadString(strRow);

	long plusone = 1;
	if(0 < strRow.GetLength() &&
		(strRow[0] == '\"' || strRow[0] == '\'' || strRow[0] == ',' || strRow[0] == ';' ||
		strRow[0] == '\t') ) plusone = 0;
		
	right = left = 0;
	while ( right < strRow.GetLength() )
	{
		while ( left < strRow.GetLength() &&
			(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
			strRow[left] == '\t' || strRow[left] == ';') )
			left++;

		if ( left >= strRow.GetLength() ) // last field has been processed
			break;

		right = left;
		while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
			strRow[right] != '\t' && strRow[right] != ';'))
			right++;

		strLabel = strRow.Mid(left, right - left);
		aLabels.Add(strLabel);
		left = right;
	}

	nColumnCount = aLabels.GetSize();

	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = nRowCount - 1;
	long		nNoOfColumns = nColumnCount - plusone;
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	if(nNoOfColumns == 0 || nNoOfRows == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("No rows and/or columns found");
		io.Close();
		m_loading = false;
		return false;
	}

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	// replace the default labels that were generated above
	for ( i = 0; i < nNoOfColumns; i++ )
	{
		SetColumnLabel(i,aLabels[i+plusone]); // aLabels[0] is the row labels column
	}


	double nValue;
	long idxCol, idxRow;
	ROW* pRow;
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill table with data
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = m_aData[idxRow];
		io.ReadString(strRow);

		// get the row label
		right = left = 0;
 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t' || strRow[left] == ';') )
			left++;
		right = left;
		while ( strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
			strRow[right] != '\t'&& strRow[right] != ';')
			right++;

		strLabel = strRow.Mid(left, right - left);
		SetRowLabel(idxRow,strLabel);
		left = right;

 		while (strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t'|| strRow[left] == ';')
			left++;

		for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
		{
			left = ++right;
			while ( right < strRow.GetLength() && 
				(strRow[right] != '\"' && strRow[right] != '\'' &&
				strRow[right] != ',' && strRow[right] != '\t' && strRow[right] != ';') )
				right++;

			if(right == left)
				continue;

			if((nValue = atof(strRow.Mid(left, right - left))) != m_nMissingValue){

				pRow->SetAt(idxCol,nValue);

				// totals
				m_aRowSums[idxRow] += nValue;
				m_aColumnSums[idxCol] += nValue;
				if ( nValue > 0.0 )
				{
					m_aRowCountPositives[idxRow]++;
					m_aColumnCountPositives[idxCol]++;
				}
				if(nValue > m_aRowMaximum[idxRow] || m_aRowMaximum[idxRow] == m_nMissingValue)
					m_aRowMaximum[idxRow] = nValue;
				if(nValue < m_aRowMinimum[idxRow] || m_aRowMinimum[idxRow] == m_nMissingValue)
					m_aRowMinimum[idxRow] = nValue;
				if(nValue > m_aColumnMaximum[idxCol] || m_aColumnMaximum[idxCol] == m_nMissingValue)
					m_aColumnMaximum[idxCol] = nValue;
				if(nValue < m_aColumnMinimum[idxCol] || m_aColumnMinimum[idxCol] == m_nMissingValue)
					m_aColumnMinimum[idxCol] = nValue;
			}else{
				m_aRowCountMissingValues[idxRow]++;
				m_aColumnCountMissingValues[idxCol]++;
				pRow->SetAt(idxCol,nValue);
			}
		}
		// row mean
		if(m_aRowSums[idxRow] != 0)
			m_aRowMean[idxRow] = (double)(m_aRowSums[idxRow]/(nNoOfColumns-m_aRowCountMissingValues[idxRow]));
		else
			m_aRowMean[idxRow] = GetMissingValue();
	}

	// column mean
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		if(m_aColumnSums[idxCol] != 0)
			m_aColumnMean[idxCol] = (double)(m_aColumnSums[idxCol]/(nNoOfRows-m_aColumnCountMissingValues[idxCol]));
		else
			m_aColumnMean[idxCol] = GetMissingValue();
	}

	io.Close();

	m_loading = false;

	UpdateIntrinsicTotals();
	UpdateAllTotals();

	return TRUE;
}

long CPatnDoc::CreateTableFromRelationalData(CString strFile)
{
	m_loading = true;
	CStdioFile io;
	CString strRow;
	CString strLabel;
	CStringArray aRowLabels, aColLabels;
	dblarray aData;
	long i;
	CWaitCursor waitcursor;

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

		CString rl, cl;
		//io.ReadString(strRow); //read the header
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		else
		{
			//get row & col labels
			cl = "";
			rl = "";
			for(i = 0; i< strRow.GetLength();i++)
			{
				if(cl.GetLength() == 0 && strRow.Mid(i,1) == ",")
				{
					cl = strRow.Left(i);
				}
				else if(strRow.Mid(i,1) == ",")
				{
					rl = strRow.Mid(cl.GetLength()+1,i - cl.GetLength()-1);
					i = strRow.GetLength();
				}
			}

			//add to column label
			i = 0;
			while(i < aColLabels.GetSize() && cl != aColLabels[i] ) i++;
			if(i == aColLabels.GetSize() && cl.GetLength() > 0)
				aColLabels.Add(cl);

			//add to row label
			i = 0;
			while(i < aRowLabels.GetSize() && rl != aRowLabels[i]) i++;
			if(i == aRowLabels.GetSize() && rl.GetLength() > 0)
				aRowLabels.Add(rl);
		}	
	}

	io.SeekToBegin();
		
	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = aRowLabels.GetSize();
	long		nNoOfColumns = aColLabels.GetSize();
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	if(nNoOfColumns == 0 || nNoOfRows == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("No rows and/or columns found");
		io.Close();
		m_loading = false;
		return false;
	}

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	
	
	long idxCol, idxRow;	
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill table with data
	for(i = 0;i<aRowLabels.GetSize();i++)
		SetRowLabel(i,aRowLabels[i]);
	for(i=0;i<aColLabels.GetSize();i++)
		SetColumnLabel(i,aColLabels[i]);
//io.ReadString(strRow); //read the header
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		
		//get data
		CString data;
		long comma_count = 0;
		for(i = 0; i< strRow.GetLength();i++)
		{
			if(strRow.Mid(i,1) == ",")
			{
				comma_count = comma_count + 1;
				if(comma_count == 1)
					cl = strRow.Left(i);
				if(comma_count == 2)
				{
					rl = strRow.Mid(cl.GetLength() + 1,i - cl.GetLength()-1);
					data = strRow.Right(strRow.GetLength() - i-1);
				}
			}
		}
	
		long ri, ci;
		//get the row & column indexes
		for(ri = 0;ri < aRowLabels.GetSize() && rl != aRowLabels[ri];ri++);
		for(ci = 0;ci < aColLabels.GetSize() && cl != aColLabels[ci];ci++);

		if(aRowLabels.GetSize() > ri && aColLabels.GetSize() > ci) 
			SetValueAt(ci,ri,atof(data));
	}

	io.Close();

	m_loading = false;

	UpdateIntrinsicTotals();
	UpdateAllTotals();

	return TRUE;
}

long CPatnDoc::CreateTableFromRelationalDataUserColumnSelect(CString strFile)
{
	m_loading = true;
	CStdioFile io;
	CString strRow;
	CString strLabel;
	CStringArray aRowLabels, aColLabels, aOriginalLabels, aRowData;
	dblarray aData;
	long i;
	CWaitCursor waitcursor;

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

		CString rl, cl,dv;

		long end;

	//get col headers so a choice can be presented
	if ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
		{
		}
		else
		{
			//get col labels
			cl = "";
			long previ = 0;
			for(i = 0; i< strRow.GetLength();i++)
			{
				if(i + 1 == strRow.GetLength() || strRow.Mid(i,1) == ",")
				{
					if(i + 1 == strRow.GetLength()) {
						end = i - previ + 1;
					} else {
						end = i - previ;
					}

					cl = strRow.Mid(previ,end);
					//add to column label
					aOriginalLabels.Add(cl);
					previ = i + 1;;

				}
			}

			
		}	
	}

	io.SeekToBegin();

	//present x and y label choices from aColLabels list
	long colIdx, rowIdx, dataIdx;
	colIdx = 0; 
	rowIdx = 1;
	dataIdx = 2;

	CImportRelationDataPicker dlg(&aOriginalLabels);
	if(dlg.DoModal() == IDCANCEL) {
		return 0;
	}

	colIdx = dlg.m_colSel;
	rowIdx = dlg.m_RowSel;
	dataIdx = dlg.m_DataSel;
	bool sumExtrinsics = true;

	//include data value selection column
	aColLabels.RemoveAll();
	io.ReadString(strRow); //read the header
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		else
		{
			//get row & col labels
			cl = "";
			rl = "";
			dv = "";
			long j = 0;
			long previ = 0;
			for(i = 0; i <= strRow.GetLength();i++)
			{
				if(i +1 == strRow.GetLength() || strRow.Mid(i,1) == ",")
				{
					if(i + 1 == strRow.GetLength()) {
						end = i - previ + 1;
					} else {
						end = i - previ;
					}
					if (j == colIdx) {
						cl = strRow.Mid(previ,end);
					} else if(j == rowIdx) {
						rl = strRow.Mid(previ,end);
					} else if(j == dataIdx) {
						dv = strRow.Mid(previ,end);
					}
					previ = i + 1;
					j++;
				}
			}

			//add to column label
			i = 0;
			while(i < aColLabels.GetSize() && cl != aColLabels[i] ) i++;
			if(i == aColLabels.GetSize() && cl.GetLength() > 0)
				aColLabels.Add(cl);

			//add to row label
			i = 0;
			while(i < aRowLabels.GetSize() && rl != aRowLabels[i]) i++;
			if(i == aRowLabels.GetSize() && rl.GetLength() > 0)
				aRowLabels.Add(rl);
		}	
	}

	io.SeekToBegin();
		
	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = aRowLabels.GetSize();
	long		nNoOfColumns = aColLabels.GetSize();
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	if(nNoOfColumns == 0 || nNoOfRows == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("No rows and/or columns found");
		io.Close();
		m_loading = false;
		return false;
	}

	SetSizes(nNoOfColumns + aOriginalLabels.GetSize() - 3, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns + aOriginalLabels.GetSize() - 3, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	
	
	long idxCol, idxRow;	
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns + aOriginalLabels.GetSize() - 3; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill table with data
	for(i = 0;i<aRowLabels.GetSize();i++)
		SetRowLabel(i,aRowLabels[i]);
	for(i=0;i<aColLabels.GetSize();i++)
		SetColumnLabel(i,aColLabels[i]);
	long ii = 0;
	for(i=0;i<aOriginalLabels.GetSize();i++) {
		if(i != rowIdx && i != colIdx && i != dataIdx) {
			if(sumExtrinsics) {
				SetColumnLabel(nNoOfColumns + ii,"Sum of " + aOriginalLabels.GetAt(i));
			} else {
				SetColumnLabel(nNoOfColumns + ii,"First of " + aOriginalLabels.GetAt(i));
			}
			ii ++;
		}
	}

	for(i=0;i<nNoOfColumns;i++) {
		for(long j=0;j<nNoOfRows;j++) {
			SetValueAt(i,j,m_nMissingValue);
		}
	}

	//if(io.ReadString(strRow))
	while ( io.ReadString(strRow) )
	{
		if (  strRow.IsEmpty() )
			break;
		

		aRowData.RemoveAll();

		//get data
		CString data;
		long comma_count = 0;
		long j = 0;
		long previ = 0;
		cl = "";rl = "";data = "";
		for(i = 0; i< strRow.GetLength();i++)
		{

				if(i + 1 == strRow.GetLength() || strRow.Mid(i,1) == ",")
				{
					if(i + 1 == strRow.GetLength()) {
						end = i - previ + 1;
					} else {
						end = i - previ;
					}

					if(end > 0){
						if (j == colIdx) {
							cl = strRow.Mid(previ,end);
						} else if(j == rowIdx) {
							rl = strRow.Mid(previ,end);
						} else if(j == dataIdx) {
							data = strRow.Mid(previ,end);
						}

						aRowData.Add(strRow.Mid(previ,end));
					}
					previ = i + 1;
					j++;
				}
			
		}
	
		long ri, ci;
		//get the row & column indexes
		for(ri = 0;ri < aRowLabels.GetSize() && rl != aRowLabels[ri];ri++);
		for(ci = 0;ci < aColLabels.GetSize() && cl != aColLabels[ci];ci++);

		if(aRowLabels.GetSize() > ri && aColLabels.GetSize() > ci) {
			SetValueAt(ci,ri,atof(data));

			ci = 0;
			for(i=0;i< aOriginalLabels.GetSize();i++) {
				if(i != colIdx && i!= rowIdx && i != dataIdx) {
					if(i < aRowData.GetSize() && aRowData.GetAt(ci).GetLength() > 0) {
						//sum for column extrinsics
						if(sumExtrinsics) {
							SetValueAt(nNoOfColumns + ci,ri,atof(aRowData.GetAt(i))+GetValueAt(nNoOfColumns + ci,ri));
						} else {
							SetValueAt(nNoOfColumns + ci,ri,atof(aRowData.GetAt(i)));
						}
					} 
					ci++;
				}
			}
		}
	}

	io.Close();

	m_loading = false;

	m_idxLeftExtrinsicColumn = nNoOfColumns;

	UpdateIntrinsicTotals();
	UpdateAllTotals();

	return TRUE;
}

long CPatnDoc::CreateTableFromAssociationMatrix(CString strFile)
{
	m_loading = true;
	CStdioFile io;
	CString strRow;
	CString strLabel;
	CStringArray aRowLabels, aColLabels;
	CString strFirstRowLabel;
	dblarray aData;
	long i;
	CWaitCursor waitcursor;

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

		CString rl, cl;
		//io.ReadString(strRow); //read the header
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		else
		{
			//get row & col labels
			cl = "";
			rl = "";
			for(i = 0; i< strRow.GetLength();i++)
			{
				if(cl.GetLength() == 0 && strRow.Mid(i,1) == ",")
				{
					cl = strRow.Left(i);
				}
				else if(strRow.Mid(i,1) == ",")
				{
					rl = strRow.Mid(cl.GetLength()+1,i - cl.GetLength()-1);
					i = strRow.GetLength();
				}
			}

			//add to column label
			aColLabels.Add(cl);
		}	
	}

	io.SeekToBegin();
		
	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = aColLabels.GetSize();
	long		nNoOfColumns = 3;
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	if(nNoOfColumns == 0 || nNoOfRows == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("No rows and/or columns found");
		io.Close();
		m_loading = false;
		return false;
	}

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);
	
	long idxCol, idxRow;	
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill table with data
	for(i = 0;i<aColLabels.GetSize();i++)
		SetRowLabel(i,aColLabels[i]);	
	for(i=0;i<nNoOfColumns;i++) {
		CString str;
		str.Format("column %d", i + 1);
		SetColumnLabel(i,str);
	}

	io.Close();

	// import association matrix
	ImportanalysisdataRowlower(strFile);
	

	m_loading = false;

	UpdateIntrinsicTotals();
	UpdateAllTotals();

	return TRUE;
}



/*long CPatnDoc::CreateTableFromTextFileTransposed(CString strFile)
{
	CStdioFile io;
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long i;
	long left, right;
	long nColumnCount, nRowCount;
	CWaitCursor waitcursor;

	m_loading = true;

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	nColumnCount = 0;
	while ( io.ReadString(strRow) )
	{
		if ( strRow.IsEmpty() )
			break;
		nColumnCount++;
	}

	if ( nColumnCount < 2 )
	{
		AfxGetMainWnd()->MessageBox("No data rows in external file","Record count",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	io.SeekToBegin();
	io.ReadString(strRow);

	long plusone = 1;
	if(0 < strRow.GetLength() &&
		(strRow[0] == '\"' || strRow[0] == '\'' || strRow[0] == ',' ||
		strRow[0] == '\t') ) plusone = 0;
		
	right = left = 0;
	while ( right < strRow.GetLength() )
	{
		while ( left < strRow.GetLength() &&
			(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
			strRow[left] == '\t') )
			left++;

		if ( left >= strRow.GetLength() ) // last field has been processed
			break;

		right = left;
		while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
			strRow[right] != '\t'))
			right++;

		strLabel = strRow.Mid(left, right - left);
		aLabels.Add(strLabel);
		left = right;
	}

	nRowCount = aLabels.GetSize();

	nRowCount++;
	nColumnCount--;

	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = nRowCount - 1;
	long		nNoOfColumns = nColumnCount - plusone;
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	SetSizes(nNoOfColumns,nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 );//dummy value - not used by view

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	// replace the default labels that were generated above
	for ( i = 0; i < nNoOfRows; i++ )
	{
		SetRowLabel(i,aLabels[i+plusone]); // aLabels[0] is the row labels column
	}


	double nValue;
	long idxCol, idxRow;
	ROW* pRow;
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill table with data
	for ( idxRow = 0; idxRow < nNoOfColumns; idxRow++ )
	{
		//pRow = m_aData[idxRow];
		io.ReadString(strRow);

		// get the row label
		right = left = 0;
 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t') )
			left++;
		right = left;
		while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
			strRow[right] != '\t'))
			right++;

		if(right < strRow.GetLength()){
			strLabel = strRow.Mid(left, right - left);
			SetColumnLabel(idxRow,strLabel);
		}
		
		left = right;

 		while (left < strRow.GetLength() && (strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t'))
			left++;

		for ( idxCol = 0; idxCol < nNoOfRows; idxCol++ )
		{
			left = ++right;
			while ( right < strRow.GetLength() && 
				(strRow[right] != '\"' && strRow[right] != '\'' &&
				strRow[right] != ',' && strRow[right] != '\t') )
				right++;

			if((nValue = atof(strRow.Mid(left, right - left))) != m_nMissingValue){
				pRow = m_aData[idxCol];
				pRow->SetAt(idxRow,nValue);

				// totals
				m_aColumnSums[idxRow] += nValue;
				m_aRowSums[idxCol] += nValue;
				if ( nValue > 0.0 )
				{
					m_aColumnCountPositives[idxRow]++;
					m_aRowCountPositives[idxCol]++;
				}
				if(nValue > m_aColumnMaximum[idxRow] || m_aColumnMaximum[idxRow] == m_nMissingValue)
					m_aColumnMaximum[idxRow] = nValue;
				if(nValue < m_aColumnMinimum[idxRow] || m_aColumnMinimum[idxRow] == m_nMissingValue)
					m_aColumnMinimum[idxRow] = nValue;
				if(nValue > m_aRowMaximum[idxCol] || m_aRowMaximum[idxCol] == m_nMissingValue)
					m_aRowMaximum[idxCol] = nValue;
				if(nValue < m_aRowMinimum[idxCol] || m_aRowMinimum[idxCol] == m_nMissingValue)
					m_aRowMinimum[idxCol] = nValue;
			}else{
				m_aColumnCountMissingValues[idxRow]++;
				m_aRowCountMissingValues[idxCol]++;
			}
		}
		// row mean
		m_aColumnMean[idxRow] = (double)(m_aColumnSums[idxRow]/(nNoOfColumns-m_aColumnCountMissingValues[idxRow]));
	}

	// column mean
	for ( idxCol = 0; idxCol < nNoOfRows; idxCol++ )
	{
		m_aRowMean[idxCol] = (double)(m_aRowSums[idxCol]/(nNoOfRows-m_aRowCountMissingValues[idxCol]));
	}

	io.Close();

	m_loading = false;

	UpdateIntrinsicTotals();
	UpdateAllTotals();
	return TRUE;
}*/

long CPatnDoc::CreateTableFromExcelFileByExcel(CString strFile,long xoff,long yoff,CArray<CString> * row_labels,CArray<CString> * col_labels,bool has_row_labels,bool has_col_labels)
{
	m_loading = true;
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	
	CFileException e;
	CFile excelFile;
	
	long err = false;

	//do new table thing... ... .. .
	long nNoOfColumns, nNoOfRows;
	double nColumnWidth;
	long nDecimalPrecision;

	m_TwayChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_OrdChange = 1;

	if(!xoff && !yoff){
		m_nMissingValue = m_nMissingValue;

		nNoOfRows = 1;
		nNoOfColumns = 1;
		nColumnWidth = g_nStandardColumnWidth;

		nDecimalPrecision = 2;

		SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

		// set row & column attributes
		SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
		SetInitialRowAttributes(nNoOfRows, 1 /*dummy value - not used by view*/);

		// set row & column totals & alloc attributes
		SetInitialColumnTotalsRowAttributes();
		SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

		FillTableWithZeros(nNoOfColumns, nNoOfRows);
	}
	
//	UpdateIntrinsicTotals();
	

	//TRY init ptr to excel
	::CoInitialize(NULL);

	HRESULT hr;
	OLECHAR FAR* szFunction;
	DISPID functionID;
	VARIANT vResult;
	DISPPARAMS dpNoArgs = {NULL, NULL, 0, 0};

	IUnknown* pUnk;
	IDispatch* pExcelApplication;
	IDispatch* pWorkbooks;
	IDispatch* pWorkbook;
	IDispatch* pSheets;
	IDispatch * pSheet;
//	IDispatch * cell;

	VARIANT vArgs[2];
	DISPPARAMS input;
	input.rgvarg = vArgs;

	CLSID clsid;
	hr = CLSIDFromProgID(L"Excel.Application", &clsid);  

	long pos = 0;

	while(true || hr == S_OK){
	    
		hr = ::CoCreateInstance( clsid, NULL, CLSCTX_SERVER,IID_IUnknown, (void**) &pUnk);
		
		if(hr != S_OK)
			break;

		pos++;
		
		hr = pUnk->QueryInterface(IID_IDispatch, (void**)&pExcelApplication);
		
		if(hr != S_OK)		
			break;

		pos++;

		//get WORKBOOKS object
		szFunction = OLESTR("Workbooks");
		hr = pExcelApplication->GetIDsOfNames(IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
		
		if(hr != S_OK)			
			break;
		hr = pExcelApplication->Invoke(functionID,IID_NULL,LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,&dpNoArgs, &vResult, NULL, NULL);
		
		if(hr != S_OK)
			break;

		pWorkbooks = vResult.pdispVal;

		pos++;

		//attempt to open file
		szFunction = OLESTR("Open");
		hr = pWorkbooks->GetIDsOfNames(IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);				
		if(hr != S_OK)
			break;

		BSTR bstrTemp = strFile.AllocSysString();
		vArgs[0].vt = VT_BSTR;
		vArgs[0].bstrVal = bstrTemp;
		input.cArgs = 1;
		input.cNamedArgs = 0;		

		hr = pWorkbooks->Invoke(functionID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &input, &vResult, NULL, NULL);

		::SysFreeString(bstrTemp);

		if(hr != S_OK)
			break;
		pWorkbook = vResult.pdispVal;		

		pos++;

		//get sheets
		szFunction = OLESTR("Sheets");
		hr = pWorkbook->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
		if(hr != S_OK)
			break;
		hr = pWorkbook->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &dpNoArgs, &vResult, NULL, NULL);

		if(hr != S_OK)
			break;
		pSheets = vResult.pdispVal;

		pos++; 
		
		//get sheet count
		szFunction = OLESTR("Count");
		hr = pSheets->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);

		if(hr != S_OK)
			break;
		hr = pSheets->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &dpNoArgs, &vResult, NULL, NULL);

		if(hr != S_OK)
			break;
		long sheet_count = vResult.lVal;

		CArray<CString> names;
		names.SetSize(sheet_count);

		//get sheet names

		for(long i=0;i<sheet_count;i++){

			vArgs[0].vt = VT_I4;
			vArgs[0].lVal = i+1;

			szFunction = OLESTR("Item");
			hr = pSheets->GetIDsOfNames(IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
			if(hr != S_OK)
				break;
			hr = pSheets->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &input, &vResult, NULL, NULL);
			if(hr != S_OK)
				break;
			pSheet = vResult.pdispVal;

			pos++;

			szFunction = OLESTR("Name");
			hr = pSheet->GetIDsOfNames (IID_NULL, &szFunction, 1,LOCALE_USER_DEFAULT, &functionID);
			if(hr != S_OK)
				break;
			hr = pSheet->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &dpNoArgs, &vResult, NULL, NULL);	
			if(hr != S_OK)
				break;
			names.SetAt(i,CString(vResult.bstrVal));

			pSheet->Release();
			pos--;
		}

		//get user sheet selection id
		CSelectWorksheet dlg(NULL);
		for(i=0;i<names.GetSize();i++){
			dlg.tables.Add(names[i]);
		}
		if(names.GetSize() > 1){
			dlg.DoModal();
		}else{
			dlg.m_selectedidx = 0;
		}

		long selectionID = dlg.m_selectedidx+1;

		//get user sheet
		vArgs[0].vt = VT_I4;
		vArgs[0].lVal = selectionID;

		szFunction = OLESTR("Item");
		hr = pSheets->GetIDsOfNames(IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
		if(hr != S_OK)
			break;

		hr = pSheets->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &input, &vResult, NULL, NULL);
		if(hr != S_OK)
			break;

		pSheet = vResult.pdispVal;

		pos++;

		//loop for labels contained in the first row, then column to determine table size
		if(row_labels != NULL){
			GetCell(1,1,0,0,pSheet,row_labels);
		}else if(col_labels != NULL){
			GetCell(1,1,0,0,pSheet,col_labels);
		}
		
		
		//row labels
		long row_count = -1;
		for(long r=0;r<650000 && row_count < 0;r++){	
			if(!GetCell(1,r+1+has_col_labels,xoff,r+1+yoff - !has_col_labels,pSheet,row_labels)){
				row_count = r;
				assertCell(row_count,1);
				if(row_labels) row_labels->RemoveAt(row_labels->GetSize()-1);
				break;
			}
		}
		
		//column labels
		long col_count = -1;
		for(long c=0;c<650000 && col_count < 0;c++){
			if(!GetCell(c+1+has_row_labels,1,c+1+xoff - !has_row_labels,yoff,pSheet,col_labels)){
				col_count = c;
				assertCell(1,col_count);
				if(col_labels) col_labels->RemoveAt(col_labels->GetSize()-1);
				break;
			}
		}

	//	SetSizes(col_count,row_count);
			
		//get all cell values in range col_count,row_count
		for(r=has_col_labels;r<row_count+1;r++){
			CString str;
			str.Format("Loading Row %d of %d",r,row_count);
			((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetWindowText(str);
			for(c=has_row_labels;c<col_count+1;c++){
				GetCell(c+1,r+1,c+xoff,r+yoff,pSheet);	
			}
		}
		break;
		
	}
	
	//cleanup

	VARIANT vFalse[2];
	DISPPARAMS dpClose;

	switch(pos){
//	case 7: cell->Release();
	case 6: pSheet->Release();
	case 5: pSheets->Release();
	case 4: 
			dpClose.cArgs = 1;
			dpClose.cNamedArgs = 0;
			dpClose.rgvarg = vFalse;	
			vFalse[0].vt = VT_BOOL;
			vFalse[0].boolVal = FALSE;

			szFunction = OLESTR("Close");
			hr = pWorkbook->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
			hr = pWorkbook->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &dpClose, &vResult, NULL, NULL);	
			pWorkbook->Release();
	case 3: pWorkbooks->Release();
	case 2: 
		szFunction = OLESTR("Quit");
		hr = pExcelApplication->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
		hr = pExcelApplication->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,
                           DISPATCH_METHOD, &dpNoArgs, NULL, NULL, NULL);	
		pExcelApplication->Release();
	case 1: pUnk->Release();
	};

	::CoUninitialize();
			

	m_loading = false;

	UpdateAllTotals();
	UpdateIntrinsicTotals();    

	return true;
}

long CPatnDoc::GetCell(long c,long r,long co,long ro,IDispatch * pSheet,CArray<CString> * labels)
{
	HRESULT hr;
	OLECHAR FAR* szFunction;
	DISPID functionID;
	VARIANT vResult;
	DISPPARAMS dpNoArgs = {NULL, NULL, 0, 0};

	IDispatch * cell;

	VARIANT vArgs[2];
	DISPPARAMS input;
	input.rgvarg = vArgs;

	vArgs [0].vt = VT_I4;
	vArgs [0].lVal = c;
	vArgs [1].vt = VT_I4;
	vArgs [1].lVal = r;
	input.cArgs = 2;
	input.cNamedArgs = 0;
	input.rgvarg = vArgs;

	szFunction = OLESTR("Cells");
	
	hr = pSheet->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
	if(hr != S_OK)
		return false;

	hr = pSheet->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &input, &vResult, NULL, NULL);
	if(hr != S_OK)
		return false;
	
	cell = vResult.pdispVal;

	szFunction = OLESTR("Value");

	hr = cell->GetIDsOfNames (IID_NULL, &szFunction, 1, LOCALE_USER_DEFAULT, &functionID);
	if(hr != S_OK){
		cell->Release();
		return false;
	}

	hr = cell->Invoke (functionID, IID_NULL, LOCALE_USER_DEFAULT,DISPATCH_PROPERTYGET, &dpNoArgs, &vResult, NULL, NULL);	
	if(hr != S_OK){
		cell->Release();
		return false;
	}

	double value = 0;	
	BSTR str;			
	long type = 1;

	switch(vResult.vt){
	case VT_I2: value = vResult.iVal; break;
	case VT_I4: value = vResult.lVal; break;
	case VT_R4: value = vResult.fltVal; break;
	case VT_R8: value = vResult.dblVal; break;
	case VT_BSTR: str = vResult.bstrVal; type = 2; break;
	case VT_ERROR: case 0: type = -1; break;
	default: type = 0;
	}

	CString tmp;

	if(labels == NULL){
		if(type == 1){//number;
			assertCell(ro,co);
			addNumber(ro,co,value);
		}else if(type == 2){//string
			assertCell(ro,co);
			addString(ro,co,CString(str));
		}else if(type == -1){//error, break
			//col_count = c;
			cell->Release();
			return false;		
		}
	}else{
		if(type == 1){//number;
			tmp.Format("%g",value);
		}else if(type == 2){//string
			tmp = str;
		}else if(type == -1){//error, break
			//col_count = c;
			labels->Add("");
			cell->Release();
			return false;		
		}
		labels->Add(str);
	}

	cell->Release();
	return true;
}


/*
long CPatnDoc::CreateTableFromExcelFile(CString strFile)
{
	m_loading = true;
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long fileLength;
	long nRowCount;
	long nColumnCount;
	CFileException e;
	CFile excelFile;
	
	long err = false;

	long idxStringList;

	// open the file specified
	if (!excelFile.Open( strFile, CFile::modeRead | CFile::shareExclusive | CFile::typeBinary, &e ) ) 
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e.m_cause << "\n";
		#endif
		AfxGetMainWnd()->MessageBox("File could not be opened ","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	//do new table thing... ... .. .
	long nNoOfColumns, nNoOfRows;
	double nColumnWidth;
	long nDecimalPrecision;

	m_TwayChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_OrdChange = 1;

	m_nMissingValue = m_nMissingValue;

	nNoOfRows = 1;
	nNoOfColumns = 1;
	nColumnWidth = g_nStandardColumnWidth;

	nDecimalPrecision = 2;

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 );

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	FillTableWithZeros(nNoOfColumns, nNoOfRows);
	
//	UpdateIntrinsicTotals();
	
	//continue excel file thing... .. .
	fileLength = excelFile.GetLength();
    unsigned char *pbuf;
	unsigned short value;
	pbuf = new unsigned char [fileLength];
	// read the file to the buffer
	excelFile.Read(pbuf,fileLength);
	nRowCount = 0;
	nColumnCount = 0;
	long j;
	long rk;
	double number;    

	CArray<double> excelFileValues;
	long lastRow;
	long firstRow;
	long BOF = 0;
	long INDEX = 0;
	long nDBCELL = 0;
	long i,k;
	CArray<long> DBCELL;

	//function contents to read excel files (well, 
	// the essential data anyway...)
	long idx;

	//open file

	//find start of file (0x0908), & store idx
	long idxStart = -1;
	long idxSheet = -1;
	CString sheetName;
	CArray<CString> stringList;

	//estimate BOF at 200h and verify
	i = 512;
	value = pbuf[i] + 256 * pbuf[i+1];
	long version;
	if(value == 0x0809){
		idxStart = i;
		version = pbuf[i+4] + 256*pbuf[i+5];
		if(version != 0x0600 && version != 0x0500){
			if(IDNO == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Unknown version.\n\nDo you wish to continue loading this file?",IDYES | IDNO)){
				delete pbuf;
				excelFile.Close();
				return false;
			}
		}
	}else{
		if(if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("There may be a problem reading this excel file.  Attempt to read it anyway?", MB_YESNO) == IDYES){
			for(i=513;i<fileLength-2;i++){
				value = pbuf[i] + 256 * pbuf[i+1];
				if(value == 0x0809){
					idxStart = i;
					version = pbuf[i+4] + 256*pbuf[i+5];
					if(version != 0x0600 && version != 0x0500){
						if(IDNO == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Unknown version.\n\nDo you wish to continue loading this file?",IDYES | IDNO)){
							delete pbuf;
							excelFile.Close();
							return false;
						}
					}
					break;
				}
			}
			if(i == fileLength-2){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Invalid Excel file.");
				delete pbuf;
				excelFile.Close();
				return false;
			}
		}else{
			delete pbuf;
			excelFile.Close();
			return false;
		}
	}
	idxStringList = -1;
    long length,found = 0;
	for(;i<fileLength-4; ){
		value = pbuf[i] + 256 * pbuf[i+1];
		length = pbuf[i+2] + 256 * pbuf[i+3];
		if(value == 0x0085 && !found){
			//get sheet idx
			idxSheet = idxStart + pbuf[i+4] + 256 * (pbuf[i+5] + 256 * (
				pbuf[i+6] + 256 * pbuf[i+7]));

			//find spreadsheet name
			long nameLength;
			if(version == 0x0500){
				nameLength = pbuf[i+10];
				for(j=0;j<nameLength;j++){
					sheetName.Insert(j,pbuf[i+11+j]);				
				}				
			}else{				
				nameLength = pbuf[i+10] + 256 * pbuf[i+11];
				for(j=0;j<nameLength;j++){
					sheetName.Insert(j,pbuf[i+12+j]);				
				}
			}			
			found = 1;
		}else if(value == 0x00fc){//store 0xFC record idx (beginning of str list)
			idxStringList = i;
		}else if(value == 0x000a){
			//end of workbook global records
			i = fileLength;
		}
		i = i + 4 + length;
	}	

	m_TableName = sheetName;
	SetValueAt(0,0,m_nMissingValue);

	unsigned char *pbufold;
	pbufold = NULL;	
	pbufold = (unsigned char*) malloc(sizeof(unsigned char*) * fileLength);
	if(pbufold == NULL){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("out of memory");
		return false;
	}
	long row,col;
	CString string;
	long oldRow,oldCol;	
	//build stringList
	i = idxStringList;
	CArray<long> conrecpos;
	if(i != -1){
		long recordlength = pbuf[i+2] + 256*pbuf[i+3]-8;
		long numStrings = pbuf[i+8] + 256 * (pbuf[i+9] + 
			256 * (pbuf[i+10] + 256 * pbuf[i+11]));
		i += 12; //move to string records
		found = 0;
	
		long pos = 0;

		//create new buffer to remove CONTINUE records and
		// record their positions
		{
			while(!found){
				for(j=0;j<recordlength;j++){
					pbufold[pos++] = pbuf[i+j];
				}
				i += j;
				value = pbuf[i] + 256*pbuf[i+1];
				if(value == 0x3c){//continue record
					conrecpos.Add(pos); //the record at 'pos' may be part of an extended continue record
					recordlength = pbuf[i+2] + 256*pbuf[i+3];
					i +=4;//size of continue record???
				}else{
					found = true;
				}
			}		
		}	
		
		found = 0;
		j = 0;
		long ii = 0;
		stringList.SetSize(numStrings);
		while(found<numStrings && ii < pos-2){
			string.Empty();
			{
				//remove conrecpos entry when it occurs between
				// records.
				if(conrecpos.GetSize() && ii == conrecpos[0])
					conrecpos.RemoveAt(0);

				length = pbufold[ii] + 256 * pbufold[ii+1]; //number of characters ONLY
				unsigned flag = pbufold[ii+2];

				unsigned long highbyte = flag & 0x01;
				unsigned long extstr = flag & 0x04;
				unsigned long richstr = flag & 0x08;

				if(highbyte){
					//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("some characters may be missing in the output");
				}

				long crun = 0;
				long addlength = 0;
				long dist = 0;
				if(extstr && richstr){
					addlength = pbufold[ii+3] + 256*(pbufold[ii+4] + 
						256*(pbufold[ii+5] + 256*pbufold[ii+6]));
					dist = 7;
				}else if(extstr && !richstr){
					crun = pbufold[ii+3] + 256*pbufold[ii+4];
					addlength = pbufold[ii+5] + 256*(pbufold[ii+6] + 
						256*(pbufold[ii+7] + 256*pbufold[ii+8]));
					dist = 9;
				}else if(richstr){
					crun = pbufold[ii+3] + 256*pbufold[ii+4];
					dist = 5;
				}else{
					dist = 3;
				}

				//only need to check for continue records when reading the actual string

				if(highbyte){//read in as '_'
					if(conrecpos.GetSize() && ii+dist+length > conrecpos[0]){
						ii++; //increment for the extended continue record						
						conrecpos.RemoveAt(0);
					}
					for(k=0;k<length;k++){
						string.Insert(k,'_');
					}
				}else{//read as is
					for(k=0;k<length;k++){
						if(conrecpos.GetSize() && ii+dist+k == conrecpos[0]){
							ii++; //increment for the extended continue record
							conrecpos.RemoveAt(0);
						}
						string.Insert(k,pbufold[ii+dist+k]);
					}
				}
				stringList.InsertAt(found++,string);
				ii += dist + 8*crun + addlength + length;
			}
		}
	}
	
	long rk2;
	
	
	//goto spreadsheet list & get list of indexes 
	// for all cell lists (32 rows/per idx...)
	CArray<long> DBCellList;
	long position;
	length = 0;
	for(i=idxSheet;i<fileLength-20;i+=4+length){
		value = pbuf[i] + 256 * pbuf[i+1];
		length= pbuf[i+2] + 256 * pbuf[i+3];
		//find INDEX location
		if(value == 0x0809){ // get sheet version
			version = pbuf[i+4] + 256*pbuf[i+5];
			if(version != 0x0600 && version != 0x0500){
				if(IDNO == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Unknown version.\n\nDo you wish to continue loading this file?",IDYES | IDNO)){
					delete pbuf;
					excelFile.Close();
					return false;
				}
			}
		}else if(value == 0x020b){ 
			if(version == 0x0500){
				//get row info
				firstRow = pbuf[i+8] + 256 * pbuf[i+9];
				lastRow = pbuf[i+10] + 256 * pbuf[i+11];
				position = 16;	
			}else{
				firstRow = pbuf[i+8] + 256 * (pbuf[i+9] + 
					256 * (pbuf[i+10] + 256 * pbuf[i+11]));
				lastRow = pbuf[i+12] + 256 * (pbuf[i+13] + 
					256 * (pbuf[i+14] + 256 * pbuf[i+15]));
				position = 20;				
			}

			//now build DBCellList
			long numRows = (lastRow-firstRow);
			long numDBCells = (long) (ceil(numRows/32.0));
			
			for(j=0;j<numDBCells && position + j * 4 < 4 + length;j++){
				long k = i + + position + j * 4;
				idx = pbuf[k] + 256 * (pbuf[k+1] + 256 * 
					(pbuf[k+2] + 256 * pbuf[k+3]));
				DBCellList.Add(idx + idxStart);
			}
			i = fileLength;
		}
	}
	row = col = 0;

	long rk3,rk4;
	
	//for each DBCellList entry, goto ROW record, 
	// jump to cell entries, & get data until 
	// DBCellList entry idx reached.
	for(i=0;i<DBCellList.GetSize();i++){
		//jump...
		idx = DBCellList[i];
		j = idx - (pbuf[idx+4] + 256 * (pbuf[idx+5] + 256 * 
			(pbuf[idx+6] + 256 * pbuf[idx+7]))) + pbuf[idx+8] +
			256 * pbuf[idx+9] + 20;

		//sort through cell records for current DBCELL
		while(j > 0 && j < idx){
			value = pbuf[j] + 256 * pbuf[j+1];
			length = pbuf[j+2] + 256*pbuf[j+3];
			oldRow = row;
			oldCol = col;
			row = pbuf[j+4] + 256 * pbuf[j+5];
			col = pbuf[j+6] + 256 * pbuf[j+7];
			long pos;

			//check for continue record...
			found = false;
			long maxlength = 4;
			long newvalue;
			while(!found){
				j += 4;
				for(k=0;k<length;k++){
					pbufold[maxlength++] = pbuf[j+k];
				}
				j += length;
				newvalue = pbuf[j] + 256*pbuf[j+1];
				if(newvalue == 0x3c){//continue record
					length = pbuf[j+2] + 256*pbuf[j+3];
					j += 4;//size of continue record???
				}else{
					found = true;
				}
			}	

			switch(value){
			case 0x0221: //ARRAY
				//not supported, skip
				//j += 18 + pbufold[16] + 256 * pbufold[17];				
				break;
			case 0x0201: //BLANK	
				//do nothing, skip				
				//j += 10;
				break;
			case 0x0205: //BOOLERR
				//treat as pres/abs (1 or 0).				
				assertCell(row,col);
				if(pbufold[11] == 0 && pbufold[10] == 1){
					//set value as 1

					//add value!!
					addNumber(row,col,1);

				}else{
					//set value as 0

					//add value!!
					addNumber(row,col,0);

				}
				//j += 12;
				break;
			case 0x0204: //LABEL
				//get string length
				assertCell(row,col);
				pos = pbufold[10] + 256 * pbufold[11];
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[12+k]);
				
				//add string!!
				addString(row,col,string);

				//j += 12 + length;
				break;
			case 0x0203: //NUMBER -- not sure how to interpret IEEE numbers yet.
				//number exists from [10] to [17]
				assertCell(row,col);
				
				//use last 8bytes, detail not yet supported for the rest...
				rk = pbufold[17] + 256 * pbufold[16];
				rk2 = pbufold[15] + 256 * pbufold[14];
				rk3 = pbufold[13] + 256 * pbufold[12];
				rk4 = pbufold[11] + 256 * pbufold[10];

				number = NumFromIEEE(rk,rk2,rk3,rk4);

				//add number!!				
				addNumber(row,col,number);
			
				//j += 18;
				break;
			case 0x00be: //MULBLANK
				//do nothing, skip
				//j += 12;
				break;
			case 0x00bd: //MULRK
				k = 0;
				//=8;
				//loop until exit condition satisfied.
				// MAY TERMINATE BEFORE COMPLETION, but should be OK
				pos = 8;
				while(pos < idx && pos+2 < maxlength){
					//(pbufold[pos]+256*pbufold[pos+1] != k+col-1 || 
					//!Record(pbufold[pos+2]+256*pbufold[pos+3]))){
					rk = pbufold[pos+2] + 256 * (pbufold[pos+3] + 256 * 
						(pbufold[pos+4] + 256 * pbufold[pos+5]));
					//if(rk != 655575 && rk != 4456663 && 
					//	rk != 2490583 && rk != 3277015){
					//ONLY TO BE ANNOYING...
					// all ending with 11010111 (0xD7)  ignored (????)
					if((rk & 0xff) != 0xd7){
						number = NumFromRk(rk);
						assertCell(row,col+k);
						addNumber(row,col+k,number);
					}
					
					//=6;
					pos += 6;
					k++;
				}
				//=2;			
				break;
			case 0x027e: //RK
				//number exists from [10] to [13]
				assertCell(row,col);
				rk = pbufold[10] + 256 * (pbufold[11] + 256 *
					(pbufold[12] + 256 * pbufold[13]));
				number = NumFromRk(rk);
				//add number!!
				addNumber(row,col,number);

				//j += 14;
				break;
			case 0x00d6: //RSTRING
				//get string length
				pos = pbufold[10] + 256 * pbufold[11];
				assertCell(row,col);
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[12+k]);
				
				//add string!!
				addString(row,col,string);

				//j += 12 + length + k;
				break;
			case 0x0207: //STRING -- called after formula?
				assertCell(oldRow,oldCol);
				//get string length
				pos = pbufold[4] + 256 * pbufold[5];
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[6+k]);
				
				//add string!!
				addString(oldRow,oldCol,string);

				//j += 6 + pbufold[4] + 256 * pbufold[5];
				break;
			case 0x00fd: //STRING -- undocumented in BIFF7
				assertCell(row,col);
				//get string position in string list
				position = pbufold[10] + 256 * (pbufold[11] +
					256 * (pbufold[12] + 256 * pbufold[13]));
				
				if(position < 0 || position >= stringList.GetSize())
					string = "error";
				else
					string = stringList.GetAt(position);
				
				//add string!!
				addString(row,col,string);

				//j += 14;
				break;
			default: //Code not recognized!
				//continue though buffer...
				if(!err) if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("cells containing formula, etc, may be present.\nFile may not successfully load!");
				err = true;
				//+;
				break;
			}
		}
	}

	delete pbuf;
	free(pbufold);


	excelFile.Close();

	m_loading = false;

	UpdateAllTotals();
	UpdateIntrinsicTotals();

	return true;

}*/


long CPatnDoc::AppendExternalFile()
{
	CString strFile;
	CString extension;
	long nfileType;
	CFileDialog dlg(TRUE,NULL,"*.xls;*.xlsx",OFN_HIDEREADONLY,"Excel Files (*.xls,*.xlsx)|*.xls;*.xlsx|All Files (*.*)|*.*||",NULL);
	if ( dlg.DoModal() != IDOK )
		return FALSE;
	CWaitCursor waitcursor;
	strFile = dlg.GetPathName();

	extension = strFile.Right(3);
	extension.MakeLower();
	if(extension == "xls" || extension == "lsx"){
		nfileType = excel;
	}else {
		nfileType = text;
	}		
	switch (nfileType)
	{

		case (excel):
			if (!AppendExcelFile(strFile))
				return FALSE;
			break;
		default:
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("File type not supported.");
			return FALSE;
			break;
	}
	
	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		pView->RedrawWindow();
	}   

	return TRUE;
}

/*
double CPatnDoc::NumFromRk(long rk)
{
	double num;

	if(rk & 0x02){
		num = (double) (rk >> 2);        
	}else{
		// hi words of IEEE num        
		*((long *)&num+1) = rk & 0xfffffffc;
		*((long *)&num) = 0;        
	}    
	if(rk & 0x01)        // divide by 100
		num /= 100;   
	return num;    
}

double CPatnDoc::NumFromIEEE(long rk,long rk2,long rk3,long rk4)
{
	double num;

	long sign;
	long exponent;
	long double top;
	double base,base2,base3,base4;

	long specialCase = 0;

	//zero? (specialCase = 1)
	if(rk == 0 && rk2 == 0 && rk3 == 0 && rk4 == 0)
		specialCase = 1;
	//top change .. see below
	else if((rk & 0x007f) == 0x003f)
		specialCase = 2;
	else if((rk & 0x007f) == 0x0040)
		specialCase = 3;
	
	if(specialCase == 1){
		num = 0;
	}else{
		//get sign
		if(rk & 0x80) sign = -1;
		else sign = 1;

		//get exponent
		exponent = ((rk & 0x7f)<<4) - 1023; 

		//get top
		top = 0;
		if(specialCase == 2){
			top = 0.000030517578125;
			if(rk & 0x1000) top *= 2;
			if(rk & 0x2000) top *= 4;
			if(rk & 0x4000) top *= 16;
			if(rk & 0x8000) top *= 256;
		}else if(specialCase == 3){
			top = 2;
			if(rk & 0x1000) top *= 2;
			if(rk & 0x2000) top *= 4;
			if(rk & 0x4000) top *= 16;
			if(rk & 0x8000) top *= 256;
		}else{
			if(rk & 0x1000) top += 2;
			if(rk & 0x2000) top += 4;
			if(rk & 0x4000) top += 16;
			if(rk & 0x8000) top += 256;
		}

		//build base - ok, in a lazy fashion...
		// 32bits remain to be examined, but program has
		// not decent facility to display them.  This seems to 
		// be accurate to at least 6 decimal places.
		base = 0;
		if(rk & 0x100) base += top/16.0;
		if(rk & 0x200) base += top/8.0;
		if(rk & 0x400) base += top/4.0;
		if(rk & 0x800) base += top/2.0;

		//use rk2
		base2 = 0;
		if(rk2 & 0x1) base2 += top*((double) pow(2,-12));
		if(rk2 & 0x2) base2 += top*((double) pow(2,-11));
		if(rk2 & 0x4) base2 += top*((double) pow(2,-10));
		if(rk2 & 0x8) base2 += top*((double) pow(2,-9));
		if(rk2 & 0x10) base2 += top*((double) pow(2,-8));
		if(rk2 & 0x20) base2 += top*((double) pow(2,-7));
		if(rk2 & 0x40) base2 += top*((double) pow(2,-6));
		if(rk2 & 0x80) base2 += top*((double) pow(2,-5));
		if(rk2 & 0x100) base2 += top*((double) pow(2,-20));
		if(rk2 & 0x200) base2 += top*((double) pow(2,-19));
		if(rk2 & 0x400) base2 += top*((double) pow(2,-18));
		if(rk2 & 0x800) base2 += top*((double) pow(2,-17));
		if(rk2 & 0x1000) base2 += top*((double) pow(2,-16));
		if(rk2 & 0x2000) base2 += top*((double) pow(2,-15));
		if(rk2 & 0x4000) base2 += top*((double) pow(2,-14));
		if(rk2 & 0x8000) base2 += top*((double) pow(2,-13));

		//use rk3
		base3 = 0;
		if(rk3 & 0x1) base3 += top*((double) pow(2,-28));
		if(rk3 & 0x2) base3 += top*((double) pow(2,-27));
		if(rk3 & 0x4) base3 += top*((double) pow(2,-26));
		if(rk3 & 0x8) base3 += top*((double) pow(2,-25));
		if(rk3 & 0x10) base3 += top*((double) pow(2,-24));
		if(rk3 & 0x20) base3 += top*((double) pow(2,-23));
		if(rk3 & 0x40) base3 += top*((double) pow(2,-22));
		if(rk3 & 0x80) base3 += top*((double) pow(2,-21));
		if(rk3 & 0x100) base3 += top*((double) pow(2,-36));
		if(rk3 & 0x200) base3 += top*((double) pow(2,-35));
		if(rk3 & 0x400) base3 += top*((double) pow(2,-34));
		if(rk3 & 0x800) base3 += top*((double) pow(2,-33));
		if(rk3 & 0x1000) base3 += top*((double) pow(2,-32));
		if(rk3 & 0x2000) base3 += top*((double) pow(2,-31));
		if(rk3 & 0x4000) base3 += top*((double) pow(2,-30));
		if(rk3 & 0x8000) base3 += top*((double) pow(2,-29));

		//use rk4
		base4 = 0;
		if(rk4 & 0x1) base4 += top*((double) pow(2,-44));
		if(rk4 & 0x2) base4 += top*((double) pow(2,-43));
		if(rk4 & 0x4) base4 += top*((double) pow(2,-42));
		if(rk4 & 0x8) base4 += top*((double) pow(2,-41));
		if(rk4 & 0x10) base4 += top*((double) pow(2,-40));
		if(rk4 & 0x20) base4 += top*((double) pow(2,-39));
		if(rk4 & 0x40) base4 += top*((double) pow(2,-38));
		if(rk4 & 0x80) base4 += top*((double) pow(2,-37));
		if(rk4 & 0x100) base4 += top*((double) pow(2,-52));
		if(rk4 & 0x200) base4 += top*((double) pow(2,-51));
		if(rk4 & 0x400) base4 += top*((double) pow(2,-50));
		if(rk4 & 0x800) base4 += top*((double) pow(2,-49));
		if(rk4 & 0x1000) base4 += top*((double) pow(2,-48));
		if(rk4 & 0x2000) base4 += top*((double) pow(2,-47));
		if(rk4 & 0x4000) base4 += top*((double) pow(2,-46));
		if(rk4 & 0x8000) base4 += top*((double) pow(2,-45));

		//calculate num
		if(specialCase > 1){
			num = sign*(top+base+base2+base3+base4);		
		}else{
			num = sign*(top+base+base2+base3+base4)*pow(2,exponent);
		}
	}
	return num;
}*/

void CPatnDoc::SetSizes(long nNoOfColumns, long nNoOfRows)
{
	ASSERT(nNoOfColumns > 0 && nNoOfRows > 0);

	long idxRow;
	ROW* pRow;

	//for(idxRow=0;idxRow<GetNoOfRows();idxRow++)
	{
		//delete m_aData[idxRow];
	}

	m_aData.SetSize(nNoOfRows); // set number of ROWs in table
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = new ROW; // create new ROW
		pRow->SetSize(nNoOfColumns); // set number of columns in ROW
		m_aData[idxRow] = pRow; // add ROW to table
	}
	m_aColumnAttributes.SetSize(nNoOfColumns);
	m_aRowAttributes.SetSize(nNoOfRows);
	m_aColumnSums.SetSize(nNoOfColumns);
	m_aColumnCountPositives.SetSize(nNoOfColumns);
	m_aColumnMinimum.SetSize(nNoOfColumns);
	m_aColumnMean.SetSize(nNoOfColumns);
	m_aColumnMaximum.SetSize(nNoOfColumns);
	m_aColumnCountMissingValues.SetSize(nNoOfColumns);
	m_aColumnGroups.SetSize(nNoOfColumns);
	
	if(m_Vectors.GetSize() == 0)
	{
		long j;
		
		for(j=0;j<nNoOfColumns;j++)
		{
			vector * nVector  = new vector;
			for(long i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
			m_Vectors.Add(nVector);
		}
	}
	else
	{
		for(long j=m_Vectors.GetSize()-1;j>=nNoOfColumns;j--)
			delete m_Vectors[j];
		if(nNoOfColumns > m_Vectors.GetSize())
			m_Vectors.InsertAt(m_Vectors.GetSize(),NULL,nNoOfColumns - m_Vectors.GetSize());
		m_Vectors.SetSize(nNoOfColumns);
	}
	

	m_aRowSums.SetSize(nNoOfRows);
	m_aRowCountPositives.SetSize(nNoOfRows);
	m_aRowMinimum.SetSize(nNoOfRows);
	m_aRowMean.SetSize(nNoOfRows);
	m_aRowMaximum.SetSize(nNoOfRows);
	m_aRowCountMissingValues.SetSize(nNoOfRows);

	m_aRowGroups.SetSize(nNoOfRows);
	m_aRowGroupsDist.SetSize(nNoOfRows);
	m_aRowXCoords.SetSize(nNoOfRows);
	m_aRowYCoords.SetSize(nNoOfRows);
	m_aRowZCoords.SetSize(nNoOfRows);
	m_PatternObjects.SetSize(nNoOfRows);
}


int CPatnDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(CString("OnOpenDocument") + CString(lpszPathName));
	if (!COleServerDoc::OnOpenDocument(lpszPathName))
		return FALSE;


	//if(IDYES == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Restore Window Positions?\n\nNote: only window size and dimensions are \nrestored.  Window order may change.",MB_YESNO | MB_ICONQUESTION))
		

	UpdateAllViews(NULL);

//	OnGo();

	return TRUE;
}

void CPatnDoc::SetInitialColumnAttributes(long nNoOfColumns, double nWidth, long nDecPrecision)
{
	long idx;
	CString strLabel;
	CColumnAttributes* pColumnAttributes;

	for ( idx = 0; idx < nNoOfColumns; idx++ )
	{
		pColumnAttributes = new CColumnAttributes;
		m_aColumnAttributes.SetAt(idx,pColumnAttributes);
		SetColumnWidth(idx,nWidth);
		SetColumnDecimalPrecision(idx,nDecPrecision);

		// make up an initial label
		strLabel.Format("Var %d",idx + 1);
		SetColumnLabel(idx,strLabel);
	}
}

void CPatnDoc::SetInitialRowAttributes(long nNoOfRows, double nHeight)
{
	long idx;
	CString strLabel;
	CRowAttributes* pRowAttributes;

	for ( idx = 0; idx < nNoOfRows; idx++ )
	{
		pRowAttributes = new CRowAttributes;
		m_aRowAttributes.SetAt(idx,pRowAttributes);
		SetRowHeight(idx,nHeight);

		// make up an initial label
		strLabel.Format("Object %d",idx + 1);
		SetRowLabel(idx,strLabel);
	}
}

void CPatnDoc::SetInitialColumnTotalsRowAttributes()
{
	CRowAttributes* pRowAttributes;

	// totals
	m_aColumnTotalsRowAttributes.SetSize(6);
	// Sums
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Sum");
	pRowAttributes->SetType(CRowColumnAttributes::Sum);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Hide();
	m_aColumnTotalsRowAttributes.SetAt(0,pRowAttributes);
	// Minimum
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Minimum");
	pRowAttributes->SetType(CRowColumnAttributes::Minimum);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnTotalsRowAttributes.SetAt(1,pRowAttributes);
	// Mean
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Mean");
	pRowAttributes->SetType(CRowColumnAttributes::Mean);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Hide();
	m_aColumnTotalsRowAttributes.SetAt(2,pRowAttributes);
	// Maximum
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Maximum");
	pRowAttributes->SetType(CRowColumnAttributes::Maximum);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnTotalsRowAttributes.SetAt(3,pRowAttributes);
	// No. > 0
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("No. > 0");
	pRowAttributes->SetType(CRowColumnAttributes::CountPositive);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnTotalsRowAttributes.SetAt(4,pRowAttributes);
	// No. missing
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("No. missing");
	pRowAttributes->SetType(CRowColumnAttributes::CountMissing);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Hide();
	m_aColumnTotalsRowAttributes.SetAt(5,pRowAttributes);

	// alloc
	m_aColumnAllocRowAttributes.SetSize(1);
	// Group
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Group");
	pRowAttributes->SetType(CRowColumnAttributes::Group);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnAllocRowAttributes.SetAt(0,pRowAttributes);

	// compare
	m_aColumnCompareRowAttributes.SetSize(3);
	// row label 1
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Group");
	pRowAttributes->SetType(0);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnCompareRowAttributes.SetAt(0,pRowAttributes);
	// row label 2
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Group");
	pRowAttributes->SetType(1);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnCompareRowAttributes.SetAt(1,pRowAttributes);
	// row differences
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Difference");
	pRowAttributes->SetType(2);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnCompareRowAttributes.SetAt(2,pRowAttributes);
	
	// pcc
	m_aColumnPccAttributes.SetSize(4);
	// x
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("X");
	pRowAttributes->SetType(0);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnPccAttributes.SetAt(0,pRowAttributes);
	// y
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Y");
	pRowAttributes->SetType(1);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnPccAttributes.SetAt(1,pRowAttributes);
	// z
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("Z");
	pRowAttributes->SetType(2);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnPccAttributes.SetAt(2,pRowAttributes);

	// c
	pRowAttributes = new CRowAttributes;
	pRowAttributes->SetLabel("rSquared");
	pRowAttributes->SetType(4);
	pRowAttributes->SetHeight(-1);
	pRowAttributes->Unhide();
	m_aColumnPccAttributes.SetAt(3,pRowAttributes);
}


void CPatnDoc::SetInitialRowTotalsColumnAttributes(long nDecPrecision)
{
	CColumnAttributes* pColumnAttributes;

	// totals
	m_aRowTotalsColumnAttributes.SetSize(6);

	// Sums
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Sum");
	pColumnAttributes->SetType(CRowColumnAttributes::Sum);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(nDecPrecision);
	pColumnAttributes->Hide();
	m_aRowTotalsColumnAttributes.SetAt(0,pColumnAttributes);

	// Minimum
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Minimum");
	pColumnAttributes->SetType(CRowColumnAttributes::Minimum);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(nDecPrecision);
	pColumnAttributes->Unhide();
	m_aRowTotalsColumnAttributes.SetAt(1,pColumnAttributes);

	// Mean
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Mean");
	pColumnAttributes->SetType(CRowColumnAttributes::Mean);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(nDecPrecision);
	pColumnAttributes->Hide();
	m_aRowTotalsColumnAttributes.SetAt(2,pColumnAttributes);

	// Maximum
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Maximum");
	pColumnAttributes->SetType(CRowColumnAttributes::Maximum);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(nDecPrecision);
	pColumnAttributes->Unhide();
	m_aRowTotalsColumnAttributes.SetAt(3,pColumnAttributes);

	// No. > 0
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("No. > 0");
	pColumnAttributes->SetType(CRowColumnAttributes::CountPositive);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowTotalsColumnAttributes.SetAt(4,pColumnAttributes);

	// No. missing
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("No. missing");
	pColumnAttributes->SetType(CRowColumnAttributes::CountMissing);
	pColumnAttributes->SetWidth(12);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Hide();
	m_aRowTotalsColumnAttributes.SetAt(5,pColumnAttributes);

	// alloc
	m_aRowAllocColumnAttributes.SetSize(3);

	// Group (row analysis)
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Analysis");
	pColumnAttributes->SetType(CRowColumnAttributes::Group);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowAllocColumnAttributes.SetAt(0,pColumnAttributes);
	// Group (row a priori)
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("A Priori");
	pColumnAttributes->SetType(CRowColumnAttributes::Group);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowAllocColumnAttributes.SetAt(1,pColumnAttributes);
	// Group (row alloc dist)
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("Distance to centroid");
	pColumnAttributes->SetType(CRowColumnAttributes::Group);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowAllocColumnAttributes.SetAt(2,pColumnAttributes);

	// ord
	m_aRowOrdColumnAttributes.SetSize(3);

	// x coord
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("x Coord");
	pColumnAttributes->SetType(CRowColumnAttributes::XCoord);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowOrdColumnAttributes.SetAt(0,pColumnAttributes);

	//y coord
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("y Coord");
	pColumnAttributes->SetType(CRowColumnAttributes::YCoord);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowOrdColumnAttributes.SetAt(1,pColumnAttributes);

	//z coord
	pColumnAttributes = new CColumnAttributes;
	pColumnAttributes->SetLabel("z Coord");
	pColumnAttributes->SetType(CRowColumnAttributes::ZCoord);
	pColumnAttributes->SetWidth(g_nStandardColumnWidth);
	pColumnAttributes->SetDecimalPrecision(0);
	pColumnAttributes->Unhide();
	m_aRowOrdColumnAttributes.SetAt(2,pColumnAttributes);

}

void CPatnDoc::FillTableWithZeros(long nNoOfColumns, long nNoOfRows)
{
	long idxCol, idxRow;
	ROW* pRow;

	// fill data in ROWs
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = m_aData[idxRow];
		for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
			pRow->SetAt(idxCol,0.0);
	}
	
	long i;
	vector* nVector;
	// column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = 0.0;
		m_aColumnMean[idxCol] = 0.0;
		m_aColumnMaximum[idxCol] = 0.0;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = 0.0;
		m_aRowMean[idxRow] = 0.0;
		m_aRowMaximum[idxRow] = 0.0;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::FillTableWithMissingValues(long nNoOfColumns, long nNoOfRows)
{
	long idxCol, idxRow;
	ROW* pRow;

	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = m_aData[idxRow];
		for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
			pRow->SetAt(idxCol,m_nMissingValue);
	}
	
	vector* nVector;
	long i;

	// column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = m_nMissingValue;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = nNoOfRows;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = m_nMissingValue;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = nNoOfColumns;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::FillTableWithRandomData(long nNoOfColumns, long nNoOfRows)
{
	// NOTE: as totals are manually set (ie, by-passing SetValueAt(), which automatically
	// checks for missing values while adjusting totals, this function must discard any
	// random values that equal the value used for missing data

	double nValue;
	long idxCol, idxRow;
	ROW* pRow;

	vector* nVector;
	long i;

	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	//srand(m_nRandomSeed); // sets the starting point for generating a series of random numbers
	((CPatnApp*)AfxGetApp())->RandomSeed(m_nRandomSeed);

	this->m_TableName = "Random Data";
	// fill table with data
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = m_aData[idxRow];
		for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
		{
			while ( (nValue = (double)((CPatnApp*)AfxGetApp)->Rand()*10) == m_nMissingValue );
			pRow->SetAt(idxCol,nValue);

			// totals
			m_aRowSums[idxRow] += nValue;
			m_aColumnSums[idxCol] += nValue;
			if ( nValue > 0.0 )
			{
				m_aRowCountPositives[idxRow]++;
				m_aColumnCountPositives[idxCol]++;
			}
			if(nValue > m_aRowMaximum[idxRow] || m_aRowMaximum[idxRow] == m_nMissingValue)
				m_aRowMaximum[idxRow] = nValue;
			if(nValue < m_aRowMinimum[idxRow] || m_aRowMinimum[idxRow] == m_nMissingValue)
				m_aRowMinimum[idxRow] = nValue;
			if(nValue > m_aColumnMaximum[idxCol] || m_aColumnMaximum[idxCol] == m_nMissingValue)
				m_aColumnMaximum[idxCol] = nValue;
			if(nValue < m_aColumnMinimum[idxCol] || m_aColumnMinimum[idxCol] == m_nMissingValue)
				m_aColumnMinimum[idxCol] = nValue;
		}
		// row mean
		if(m_aRowSums[idxRow] != GetMissingValue())
			m_aRowMean[idxRow] = (double)(m_aRowSums[idxRow]/nNoOfColumns);
		else
			m_aRowMean[idxRow] = GetMissingValue();
	}

	// column averages
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		if(m_aColumnSums[idxCol] != GetMissingValue())
			m_aColumnMean[idxCol] = (double)(m_aColumnSums[idxCol]/nNoOfRows);
		else
			m_aColumnMean[idxCol] = GetMissingValue();
	}
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc server implementation

COleServerItem* CPatnDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	//CPatnSrvrItem* pItem = new CPatnSrvrItem(this);
	//ASSERT_VALID(pItem);
	return NULL;//pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc operations

// add,delete,move columns
void CPatnDoc::InsertColumnAt(long idxColumn, long nCount /* = 1 */)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());
	ASSERT(nCount > 0);

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;

	}

	if ( idxColumn < 0 || idxColumn > GetUpperBoundColumns() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

	ROW* pRow;
	CColumnAttributes* pColumnAttributes;
	long i;
	long idxRow;
	long nNoOfRows = GetNoOfRows();
	
	for ( i = 0; i < nCount; i++ )
	{
		// create a new CColumnAttributes object for each column to be inserted
		pColumnAttributes = new CColumnAttributes();
		m_aColumnAttributes.InsertAt(idxColumn, pColumnAttributes);
	}

	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		// insert new cell element(s) in each row of data
		pRow = GetRowAt(idxRow);
		pRow->InsertAt(idxColumn,m_nMissingValue,nCount);

		// adjust row totals
		m_aRowCountMissingValues[idxRow] += nCount;
	}
	
	vector* nVector;
	
	// insert new element(s) in each of the totals arrays
	m_aColumnSums.InsertAt(idxColumn,0,nCount);
	m_aColumnCountPositives.InsertAt(idxColumn,0,nCount);
	m_aColumnMinimum.InsertAt(idxColumn,m_nMissingValue,nCount);
	m_aColumnMean.InsertAt(idxColumn,m_nMissingValue,nCount);
	m_aColumnMaximum.InsertAt(idxColumn,m_nMissingValue,nCount);
	m_aColumnCountMissingValues.InsertAt(idxColumn,nNoOfRows,nCount);
	m_aColumnGroups.InsertAt(idxColumn,-1,nCount);

	for(long j=0;j<nCount;j++){
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_Vectors.InsertAt(idxColumn+j,nVector);
	}
	
	// adjust index of extrinsic columns if necessary
	if ( idxColumn <= m_idxLeftExtrinsicColumn )
		m_idxLeftExtrinsicColumn += nCount;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::RemoveColumnAt(long idxColumn, long nCount /* = 1 */)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());
	ASSERT(nCount > 0);

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && ( AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	if ( idxColumn < 0 || idxColumn > GetUpperBoundColumns() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

	ROW* pRow;
	CColumnAttributes* pColumnAttributes;
	long i;
	long idxRow;
	double nValue;
	long nNoOfRows = GetNoOfRows();
	
	for ( i = 0; i < nCount; i++ )
	{
		// delete CColumnAttributes object for each column to be removed
		pColumnAttributes = m_aColumnAttributes[idxColumn + i];
		delete pColumnAttributes;
		
		// adjust index of extrinsic columns if necessary
		if ( idxColumn < m_idxLeftExtrinsicColumn )
			m_idxLeftExtrinsicColumn--;
	}
	m_aColumnAttributes.RemoveAt(idxColumn,nCount);

	// for each row of data ....
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		pRow = GetRowAt(idxRow);  // get pointer to current row

		// adjust row totals for current row
		for ( i = 0; i < nCount; i++ )
		{
			nValue = pRow->GetAt(idxColumn + i);
			if ( nValue == m_nMissingValue )
				m_aRowCountMissingValues[idxRow]--;
			else
			{
				if(m_aRowSums[idxRow] == m_nMissingValue)
					m_aRowSums[idxRow] = 0;
				m_aRowSums[idxRow] -= nValue;
				if ( nValue > 0 )
					m_aRowCountPositives[idxRow]--;
			}
		}

		// remove cell elements from current row
		pRow->RemoveAt(idxColumn,nCount);

		
	}


	// remove element(s) in each of the totals arrays
	m_aColumnSums.RemoveAt(idxColumn,nCount);
	m_aColumnCountPositives.RemoveAt(idxColumn,nCount);
	m_aColumnMinimum.RemoveAt(idxColumn,nCount);
	m_aColumnMean.RemoveAt(idxColumn,nCount);
	m_aColumnMaximum.RemoveAt(idxColumn,nCount);
	m_aColumnCountMissingValues.RemoveAt(idxColumn,nCount);
	m_aColumnGroups.RemoveAt(idxColumn,nCount);
	for(i=0;i<nCount;i++){
		delete m_Vectors[idxColumn+i];
	}
	m_Vectors.RemoveAt(idxColumn,nCount);

	// if there are no remaining extrinsic columns, then set index to -1
	if ( m_idxLeftExtrinsicColumn > GetUpperBoundColumns() )
		m_idxLeftExtrinsicColumn = -1;

	SetModifiedFlag();
	
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ ){
		// recalculate average for all rows
		SetRowAverage(idxRow);
	}

	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::AddColumn()
{
	ROW* pRow;
	CColumnAttributes* pColumnAttributes;
	long idxRow;
	long nNoOfRows = GetNoOfRows();

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}
	
	// create a new CColumnAttributes object
	pColumnAttributes = new CColumnAttributes();
	m_aColumnAttributes.Add(pColumnAttributes);

	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		// add new cell element to each row of data
		pRow = GetRowAt(idxRow);
		pRow->Add(m_nMissingValue);

		// adjust row totals
		m_aRowCountMissingValues[idxRow]++;
	}

	vector* nVector;
	nVector  = new vector;
	long i;
	for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);

	// add new element(s) to each of the totals arrays
	m_aColumnSums.Add(0);
	m_aColumnCountPositives.Add(0);
	m_aColumnMinimum.Add(m_nMissingValue);
	m_aColumnMean.Add(m_nMissingValue);
	m_aColumnMaximum.Add(m_nMissingValue);
	m_aColumnCountMissingValues.Add(nNoOfRows);
	m_aColumnGroups.Add(-1);
	m_Vectors.Add(nVector);
}

void CPatnDoc::MoveColumn(long idxFrom, long idxTo, long nCount /* = 1 */)
{
	ASSERT(idxFrom >= 0 && idxFrom <= GetUpperBoundColumns());
	ASSERT(idxTo >= 0 && idxTo <= GetNoOfColumns());
	ASSERT(nCount > 0);
	long n = 0;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	if ( idxFrom < 0 || idxFrom > GetUpperBoundColumns() )	// invalid index
		return;
	if ( idxTo < 0 || idxTo > GetNoOfColumns() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

		
	BeginWaitCursor();
		
	ROW* pRow;
	CColumnAttributes* pColumnAttributes;
	long i;
	long idxRow;
	long nNoOfRows = GetNoOfRows();
	double nValue;
	double nSum;
	long nCountPositives;
	double nMinimum,nMaximum,nMean;
	long nCountMissingValues;
	long nGroups;
	vector *nVector;

	if ( idxTo < idxFrom )
	{
		for ( i = 0; i < nCount; i++ )
		{
			// move column attributes
			pColumnAttributes = m_aColumnAttributes[idxFrom + i];
			m_aColumnAttributes.InsertAt(idxTo + i,pColumnAttributes);
			m_aColumnAttributes.RemoveAt(idxFrom + i + 1);

			// move cell elements in each row
			for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
			{
				pRow = GetRowAt(idxRow);
				nValue = pRow->GetAt(idxFrom + i);
				pRow->InsertAt(idxTo + i,nValue);
				pRow->RemoveAt(idxFrom + i + 1);
			}

			// move column totals
			nSum = m_aColumnSums[idxFrom + i];
			m_aColumnSums.InsertAt(idxTo + i,nSum);
			m_aColumnSums.RemoveAt(idxFrom + i + 1);

			nCountPositives = m_aColumnCountPositives[idxFrom + i];
			m_aColumnCountPositives.InsertAt(idxTo + i,nCountPositives);
			m_aColumnCountPositives.RemoveAt(idxFrom + i + 1);

			nMinimum = m_aColumnMinimum[idxFrom + i];
			m_aColumnMinimum.InsertAt(idxTo + i,nMinimum);
			m_aColumnMinimum.RemoveAt(idxFrom + i + 1);

			nMean = m_aColumnMean[idxFrom + i];
			m_aColumnMean.InsertAt(idxTo + i,nMean);
			m_aColumnMean.RemoveAt(idxFrom + i + 1);

			nMaximum = m_aColumnMaximum[idxFrom + i];
			m_aColumnMaximum.InsertAt(idxTo + i,nMaximum);
			m_aColumnMaximum.RemoveAt(idxFrom + i + 1);

			nCountMissingValues = m_aColumnCountMissingValues[idxFrom + i];
			m_aColumnCountMissingValues.InsertAt(idxTo + i,nCountMissingValues);
			m_aColumnCountMissingValues.RemoveAt(idxFrom + i + 1);

			nGroups = m_aColumnGroups[idxFrom + i];
			m_aColumnGroups.InsertAt(idxTo + i,nGroups);
			m_aColumnGroups.RemoveAt(idxFrom + i + 1);

			nVector = m_Vectors[idxFrom + i];
			m_Vectors.InsertAt(idxTo + i,nVector);
			m_Vectors.RemoveAt(idxFrom + i + 1);

		}
	}
	else
	{
		if ( idxTo <= (idxFrom + nCount) )	// No point in moving to same spot
			return;

		for ( i = 0; i < nCount; i++ )
		{
			// move column attributes
			pColumnAttributes = m_aColumnAttributes[idxFrom];
			m_aColumnAttributes.InsertAt(idxTo,pColumnAttributes);
			m_aColumnAttributes.RemoveAt(idxFrom);

			// move cell elements in each row
			for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
			{
				pRow = GetRowAt(idxRow);
				nValue = pRow->GetAt(idxFrom);
				pRow->InsertAt(idxTo,nValue);
				pRow->RemoveAt(idxFrom);
			}

			// move column totals
			nSum = m_aColumnSums[idxFrom];
			m_aColumnSums.InsertAt(idxTo,nSum);
			m_aColumnSums.RemoveAt(idxFrom);

			nCountPositives = m_aColumnCountPositives[idxFrom];
			m_aColumnCountPositives.InsertAt(idxTo,nCountPositives);
			m_aColumnCountPositives.RemoveAt(idxFrom);

			nMinimum = m_aColumnMinimum[idxFrom];
			m_aColumnMinimum.InsertAt(idxTo,nMinimum);
			m_aColumnMinimum.RemoveAt(idxFrom);

			nMean = m_aColumnMean[idxFrom];
			m_aColumnMean.InsertAt(idxTo,nMean);
			m_aColumnMean.RemoveAt(idxFrom);

			nMaximum = m_aColumnMaximum[idxFrom];
			m_aColumnMaximum.InsertAt(idxTo,nMaximum);
			m_aColumnMaximum.RemoveAt(idxFrom);

			nCountMissingValues = m_aColumnCountMissingValues[idxFrom];
			m_aColumnCountMissingValues.InsertAt(idxTo,nCountMissingValues);
			m_aColumnCountMissingValues.RemoveAt(idxFrom);

			nGroups = m_aColumnGroups[idxFrom];
			m_aColumnGroups.InsertAt(idxTo,nGroups);
			m_aColumnGroups.RemoveAt(idxFrom);

			nVector = m_Vectors[idxFrom];
			m_Vectors.InsertAt(idxTo,nVector);
			m_Vectors.RemoveAt(idxFrom);

		}
	}
	
	if ( IsExtrinsicColumns() )
	{
		if ( idxFrom < m_idxLeftExtrinsicColumn && idxTo >= m_idxLeftExtrinsicColumn )
		{
			if ( (idxFrom + nCount - 1) >= m_idxLeftExtrinsicColumn )
				m_idxLeftExtrinsicColumn -= (m_idxLeftExtrinsicColumn - idxFrom);
			else
				m_idxLeftExtrinsicColumn -= nCount;
		}
		else if ( (idxFrom + nCount - 1) >= m_idxLeftExtrinsicColumn && idxTo < m_idxLeftExtrinsicColumn )
		{
			if ( idxFrom >= m_idxLeftExtrinsicColumn )
				m_idxLeftExtrinsicColumn += nCount;
			else
				m_idxLeftExtrinsicColumn += nCount - (m_idxLeftExtrinsicColumn - idxFrom);
		}
		
		if ( m_idxLeftExtrinsicColumn >= GetNoOfColumns() )
			m_idxLeftExtrinsicColumn = -1;
	}

	EndWaitCursor();	

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

// add,delete,move rows
void CPatnDoc::InsertRowAt(long idxRow, long nCount /* = 1 */)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());
	ASSERT(nCount > 0);

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}
	if ( idxRow < 0 || idxRow > GetUpperBoundRows() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

	ROW* pRow;
	CRowAttributes* pRowAttributes;
	long i;
	long idxColumn;
	long nNoOfColumns = GetNoOfColumns();
	
	for ( i = 0; i < nCount; i++ )
	{
		// create a new CRowAttributes object for each row to be inserted
		pRowAttributes = new CRowAttributes();
		m_aRowAttributes.InsertAt(idxRow, pRowAttributes);

		// create a new ROW for each row to be inserted
		pRow = new ROW;
		pRow->SetSize(nNoOfColumns);
		for ( idxColumn = 0; idxColumn < nNoOfColumns; idxColumn++ )
		{
			// set value for each cell in row
			pRow->SetAt(idxColumn,m_nMissingValue);
			// adjust column totals
			m_aColumnCountMissingValues[idxColumn]++;
		}
		m_aData.InsertAt(idxRow,pRow);
	}

	// adjust row totals
	m_aRowSums.InsertAt(idxRow,0,nCount);
	m_aRowCountPositives.InsertAt(idxRow,0,nCount);
	m_aRowMinimum.InsertAt(idxRow,m_nMissingValue,nCount);
	m_aRowMean.InsertAt(idxRow,m_nMissingValue,nCount);
	m_aRowMaximum.InsertAt(idxRow,m_nMissingValue,nCount);
	m_aRowCountMissingValues.InsertAt(idxRow,nNoOfColumns,nCount);
	m_aRowGroups.InsertAt(idxRow,-1,nCount);
	m_aRowGroupsDist.InsertAt(idxRow,m_nMissingValue,nCount);
	m_aRowXCoords.InsertAt(idxRow,ANALYSIS_VALUE,nCount);
	m_aRowYCoords.InsertAt(idxRow,ANALYSIS_VALUE,nCount);
	m_aRowZCoords.InsertAt(idxRow,ANALYSIS_VALUE,nCount);
	m_PatternObjects.InsertAt(idxRow,-1,nCount);

	// adjust index of extrinsic rows if necessary
	if ( idxRow <= m_idxTopExtrinsicRow )
		m_idxTopExtrinsicRow += nCount;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::RemoveRowAt(long idxRow, long nCount /* = 1 */)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());
	ASSERT(nCount > 0);

	if((!m_TwayChange || /*!m_AssocRowChange ||*/
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		//m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	if ( idxRow < 0 || idxRow > GetUpperBoundRows() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

	ROW* pRow;
	CRowAttributes* pRowAttributes;
	long i;
	long idxColumn;
	long nNoOfColumns = GetNoOfColumns();
	double nValue;
	
	for ( i = 0; i < nCount; i++ )
	{
		// delete CRowAttributes object for each Row to be removed
		pRowAttributes = m_aRowAttributes[idxRow + i];
		delete pRowAttributes;
		
		// adjust index of extrinsic Rows if necessary
		if ( idxRow < m_idxTopExtrinsicRow )
			m_idxTopExtrinsicRow--;
	}
	m_aRowAttributes.RemoveAt(idxRow,nCount);

	// for each row of data to be deleted ....
	for ( i = 0; i < nCount; i++ )
	{
		pRow = GetRowAt(idxRow + i);  // get pointer to row

		// adjust column totals (Note - averages adjusted later in this function)
		for ( idxColumn = 0; idxColumn < nNoOfColumns; idxColumn++ )
		{
			nValue = pRow->GetAt(idxColumn);

			if ( nValue == m_nMissingValue )
				m_aColumnCountMissingValues[idxColumn]--;
			else
			{
				if(m_aColumnSums[idxColumn] == m_nMissingValue)
					m_aColumnSums[idxColumn] = 0;
				m_aColumnSums[idxColumn] -= nValue;
				if ( nValue > 0 )
					m_aColumnCountPositives[idxColumn]--;
			}
		}

		// remove cell elements from current row
		pRow->RemoveAll();
	}

	// remove ROWs from data
	ROW* tmprow = m_aData.GetAt(idxRow);
	for(i=0;i<nCount;i++){
		delete m_aData[idxRow+i];
	}
	m_aData.RemoveAt(idxRow,nCount);
	
	// remove element(s) in each of the totals arrays
	m_aRowSums.RemoveAt(idxRow,nCount);
	m_aRowCountPositives.RemoveAt(idxRow,nCount);
	m_aRowMaximum.RemoveAt(idxRow,nCount);
	m_aRowMean.RemoveAt(idxRow,nCount);
	m_aRowMinimum.RemoveAt(idxRow,nCount);
	m_aRowCountMissingValues.RemoveAt(idxRow,nCount);
	m_aRowGroups.RemoveAt(idxRow,nCount);
	m_aRowGroupsDist.RemoveAt(idxRow,nCount);
	m_aRowXCoords.RemoveAt(idxRow,nCount);
	m_aRowYCoords.RemoveAt(idxRow,nCount);
	m_aRowZCoords.RemoveAt(idxRow,nCount);
	m_PatternObjects.RemoveAt(idxRow,nCount);

	// adjust averages for columns
	for ( idxColumn = 0; idxColumn < nNoOfColumns; idxColumn++ )
	{
		SetColumnAverage(idxColumn);
	}

	// if there are no remaining extrinsic Rows, then set index to -1
	if ( m_idxTopExtrinsicRow > GetUpperBoundRows() )
		m_idxTopExtrinsicRow = -1;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();

	//association matrix update
	//remove column values
	for(i=0;i<m_AssocMatrixRows.m_aMatrix.GetSize();i++) {
		if(m_AssocMatrixRows.m_aMatrix.GetAt(i)->GetSize() > idxRow) {
			m_AssocMatrixRows.m_aMatrix.GetAt(i)->RemoveAt(idxRow);
		}
	}
	//remove row record
	if(idxRow > 0) {
		if (m_AssocMatrixRows.m_aMatrix.GetSize() > idxRow-1) {
			delete m_AssocMatrixRows.m_aMatrix.GetAt(idxRow-1);
			m_AssocMatrixRows.m_aMatrix.RemoveAt(idxRow-1);
		}
	} else if(idxRow == 0) {
		if (m_AssocMatrixRows.m_aMatrix.GetSize() > idxRow) {
			delete m_AssocMatrixRows.m_aMatrix.GetAt(idxRow);
			m_AssocMatrixRows.m_aMatrix.RemoveAt(idxRow);
		}
	}
		
}

void CPatnDoc::AddRow()
{
	ROW* pRow;
	CRowAttributes* pRowAttributes;
	long idxColumn;
	long nNoOfColumns = GetNoOfColumns();

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}
	
	// create a new CRowAttributes object
	pRowAttributes = new CRowAttributes();
	m_aRowAttributes.Add(pRowAttributes);

	// create a new ROW in the data
	pRow = new ROW;
	pRow->SetSize(nNoOfColumns);
	for ( idxColumn = 0; idxColumn < nNoOfColumns; idxColumn++ )
	{
		// initialize each cell element of the new row of data
		pRow->SetAt(idxColumn, m_nMissingValue);

		// adjust column totals
		m_aColumnCountMissingValues[idxColumn]++;
	}
	m_aData.Add(pRow);

	// add new element(s) to each of the totals arrays
	m_aRowSums.Add(0);
	m_aRowCountPositives.Add(0);
	m_aRowMinimum.Add(m_nMissingValue);
	m_aRowMean.Add(m_nMissingValue);
	m_aRowMaximum.Add(m_nMissingValue);
	m_aRowCountMissingValues.Add(nNoOfColumns);
	m_aRowGroups.Add(-1);
	m_aRowGroupsDist.Add(m_nMissingValue);
	m_aRowXCoords.Add(ANALYSIS_VALUE);
	m_aRowYCoords.Add(ANALYSIS_VALUE);
	m_aRowZCoords.Add(ANALYSIS_VALUE);
	m_PatternObjects.Add(-1);

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::MoveRow(long idxFrom, long idxTo, long nCount /* = 1 */)
{
	ASSERT(idxFrom >= 0 && idxFrom <= GetUpperBoundRows());
	ASSERT(idxTo >= 0 && idxTo <= GetNoOfRows());
	ASSERT(nCount > 0);

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	if ( idxFrom < 0 || idxFrom > GetUpperBoundRows() )	// invalid index
		return;
	if ( idxTo < 0 || idxTo > GetNoOfRows() )	// invalid index
		return;
	if ( nCount < 1 )  // invalid count
		return;

	BeginWaitCursor();
		
	ROW* pRow;
	CRowAttributes* pRowAttributes;
	long i;
	long nNoOfRows = GetNoOfRows();
	double nSum;
	long nCountPositives;
	double nMinimum,nMean,nMaximum;
	long nCountMissingValues;
	long nGroups;
	double nXCoord;
	double nYCoord;
	double nZCoord;

	if ( idxTo < idxFrom )
	{
		for ( i = 0; i < nCount; i++ )
		{
			// move row attributes
			pRowAttributes = m_aRowAttributes[idxFrom + i];
			m_aRowAttributes.InsertAt(idxTo + i,pRowAttributes);
			m_aRowAttributes.RemoveAt(idxFrom + i + 1);

			// move ROWs
			pRow = GetRowAt(idxFrom + i);
			m_aData.InsertAt(idxTo + i,pRow);
			m_aData.RemoveAt(idxFrom + i + 1);

			// move row totals
			nSum = m_aRowSums[idxFrom + i];
			m_aRowSums.InsertAt(idxTo + i,nSum);
			m_aRowSums.RemoveAt(idxFrom + i + 1);

			nCountPositives = m_aRowCountPositives[idxFrom + i];
			m_aRowCountPositives.InsertAt(idxTo + i,nCountPositives);
			m_aRowCountPositives.RemoveAt(idxFrom + i + 1);

			nMinimum = m_aRowMinimum[idxFrom + i];
			m_aRowMinimum.InsertAt(idxTo + i,nMinimum);
			m_aRowMinimum.RemoveAt(idxFrom + i + 1);

			nMean = m_aRowMean[idxFrom + i];
			m_aRowMean.InsertAt(idxTo + i,nMean);
			m_aRowMean.RemoveAt(idxFrom + i + 1);

			nMaximum = m_aRowMaximum[idxFrom + i];
			m_aRowMaximum.InsertAt(idxTo + i,nMaximum);
			m_aRowMaximum.RemoveAt(idxFrom + i + 1);

			nCountMissingValues = m_aRowCountMissingValues[idxFrom + i];
			m_aRowCountMissingValues.InsertAt(idxTo + i,nCountMissingValues);
			m_aRowCountMissingValues.RemoveAt(idxFrom + i + 1);

			nGroups = m_aRowGroups[idxFrom + i];
			m_aRowGroups.InsertAt(idxTo + i,nGroups);
			m_aRowGroups.RemoveAt(idxFrom + i + 1);

			nMaximum = m_aRowGroupsDist[idxFrom + i];
			m_aRowGroupsDist.InsertAt(idxTo + i,nMaximum);
			m_aRowGroupsDist.RemoveAt(idxFrom +i + 1);

			nXCoord = m_aRowXCoords[idxFrom + i];
			m_aRowXCoords.InsertAt(idxTo + i,nXCoord);
			m_aRowXCoords.RemoveAt(idxFrom + i + 1);

			nYCoord = m_aRowYCoords[idxFrom + i];
			m_aRowYCoords.InsertAt(idxTo + i,nYCoord);
			m_aRowYCoords.RemoveAt(idxFrom + i + 1);

			nZCoord = m_aRowZCoords[idxFrom + i];
			m_aRowZCoords.InsertAt(idxTo + i,nZCoord);
			m_aRowZCoords.RemoveAt(idxFrom + i + 1);

			{
				long nObject = m_PatternObjects[idxFrom + i];
				m_PatternObjects.InsertAt(idxTo + i,nObject);
				m_PatternObjects.RemoveAt(idxFrom + i + 1);
			}
		}
	}
	else
	{
		if ( idxTo <= (idxFrom + nCount) )	// No point in moving to same spot
			return;

		for ( i = 0; i < nCount; i++ )
		{
			// move row attributes
			pRowAttributes = m_aRowAttributes[idxFrom];
			m_aRowAttributes.InsertAt(idxTo,pRowAttributes);
			m_aRowAttributes.RemoveAt(idxFrom);
		
			// move ROWs
			pRow = GetRowAt(idxFrom);
			m_aData.InsertAt(idxTo,pRow);
			m_aData.RemoveAt(idxFrom);

			// move row totals
			nSum = m_aRowSums[idxFrom];
			m_aRowSums.InsertAt(idxTo,nSum);
			m_aRowSums.RemoveAt(idxFrom);

			nCountPositives = m_aRowCountPositives[idxFrom];
			m_aRowCountPositives.InsertAt(idxTo,nCountPositives);
			m_aRowCountPositives.RemoveAt(idxFrom);

			nMinimum = m_aRowMinimum[idxFrom];
			m_aRowMinimum.InsertAt(idxTo,nMinimum);
			m_aRowMinimum.RemoveAt(idxFrom);

			nMean = m_aRowMean[idxFrom];
			m_aRowMean.InsertAt(idxTo,nMean);
			m_aRowMean.RemoveAt(idxFrom);

			nMaximum = m_aRowMaximum[idxFrom];
			m_aRowMaximum.InsertAt(idxTo,nMaximum);
			m_aRowMaximum.RemoveAt(idxFrom);

			nCountMissingValues = m_aRowCountMissingValues[idxFrom];
			m_aRowCountMissingValues.InsertAt(idxTo,nCountMissingValues);
			m_aRowCountMissingValues.RemoveAt(idxFrom);

			nGroups = m_aRowGroups[idxFrom];
			m_aRowGroups.InsertAt(idxTo,nGroups);
			m_aRowGroups.RemoveAt(idxFrom);

			nMaximum = m_aRowGroupsDist[idxFrom];
			m_aRowGroupsDist.InsertAt(idxTo,nMaximum);
			m_aRowGroupsDist.RemoveAt(idxFrom);

			nXCoord = m_aRowXCoords[idxFrom + i];
			m_aRowXCoords.InsertAt(idxTo + i,nXCoord);
			m_aRowXCoords.RemoveAt(idxFrom + i + 1);

			nYCoord = m_aRowYCoords[idxFrom + i];
			m_aRowYCoords.InsertAt(idxTo + i,nYCoord);
			m_aRowYCoords.RemoveAt(idxFrom + i + 1);

			nZCoord = m_aRowZCoords[idxFrom + i];
			m_aRowZCoords.InsertAt(idxTo + i,nZCoord);
			m_aRowZCoords.RemoveAt(idxFrom + i + 1);
	
			{
				long nObject = m_PatternObjects[idxFrom + i];
				m_PatternObjects.InsertAt(idxTo + i,nObject);
				m_PatternObjects.RemoveAt(idxFrom + i + 1);
			}
		}
	}
	
	if ( IsExtrinsicRows() )
	{
		if ( idxFrom < m_idxTopExtrinsicRow && idxTo >= m_idxTopExtrinsicRow )
		{
			if ( (idxFrom + nCount - 1) >= m_idxTopExtrinsicRow )
				m_idxTopExtrinsicRow -= (m_idxTopExtrinsicRow - idxFrom);
			else
				m_idxTopExtrinsicRow -= nCount;
		}
		else if ( (idxFrom + nCount - 1) >= m_idxTopExtrinsicRow && idxTo < m_idxTopExtrinsicRow )
		{
			if ( idxFrom >= m_idxTopExtrinsicRow )
				m_idxTopExtrinsicRow += nCount;
			else
				m_idxTopExtrinsicRow += nCount - (m_idxTopExtrinsicRow - idxFrom);
		}
		
		if ( m_idxTopExtrinsicRow >= nNoOfRows )
			m_idxTopExtrinsicRow = -1;
	}

	EndWaitCursor();

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

// set cell values
void CPatnDoc::SetValueAt(long idxColumn, long idxRow, CString strNewValue)
{
	double nNewValue = atof(strNewValue);
	SetValueAt(idxColumn,idxRow,nNewValue); 

	SetModifiedFlag();
}

void CPatnDoc::SetValueAt(long idxColumn, long idxRow, double nNewValue)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	ROW* pRow;
	double nOldValue;

	nOldValue = GetValueAt(idxColumn,idxRow);
	if ( nNewValue == nOldValue ) // no changes required
		return;

	if(!m_loading) {
		if((!m_TwayChange || !m_AssocRowChange ||
			!m_AssocColChange || !m_FuseRowChange ||
			!m_FuseColChange || !m_OrdChange) &&
			(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
			MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
		{
			return;
		}else{
			//Graphs invalid
			m_TwayChange = 1;
			m_AssocRowChange = 1;
			m_AssocColChange = 1;
			m_FuseRowChange = 1;
			m_FuseColChange = 1;
			m_OrdChange = 1;
		}
	}

	// change cell value;
	pRow = GetRowAt(idxRow);
	pRow->SetAt(idxColumn,nNewValue);

	if(!m_loading){
		if ( nNewValue == m_nMissingValue )
		{
			// Adjust column totals
			if(m_aColumnSums[idxColumn] == m_nMissingValue)
				m_aColumnSums[idxColumn] = 0;
			m_aColumnSums[idxColumn] -= nOldValue;
			if ( nOldValue > 0 )
				m_aColumnCountPositives[idxColumn]--;
			m_aColumnCountMissingValues[idxColumn]++;

			// check whether this has resulted in all column values being set to missing value
			if ( m_aColumnCountMissingValues[idxColumn] == GetNoOfRows() )
				m_aColumnSums[idxColumn] = m_nMissingValue;

			SetColumnAverage(idxColumn);
		
			// Adjust row totals
			if(m_aRowSums[idxRow] == m_nMissingValue)
				m_aRowSums[idxRow] = 0;
			m_aRowSums[idxRow] -= nOldValue;
			if ( nOldValue > 0 )
				m_aRowCountPositives[idxRow]--;
			m_aRowCountMissingValues[idxRow]++;

			// check whether this has resulted in all row values being set to missing value
			if ( m_aRowCountMissingValues[idxColumn] == GetNoOfColumns() )
				m_aRowSums[idxColumn] = m_nMissingValue;

			SetRowAverage(idxRow);
		}
		else if ( nOldValue == m_nMissingValue )
		{
			// Adjust column totals
			// check whether the column previously contained all missing values
			if ( m_aColumnSums[idxColumn] == m_nMissingValue )
				m_aColumnSums[idxColumn] = nNewValue;
			else
				m_aColumnSums[idxColumn] += nNewValue;

			if ( nNewValue > 0 )
				m_aColumnCountPositives[idxColumn]++;
			m_aColumnCountMissingValues[idxColumn]--;
			SetColumnAverage(idxColumn);

			// Adjust row totals
			// check whether the row previously contained all missing values
			if ( m_aRowSums[idxRow] == m_nMissingValue )
				m_aRowSums[idxRow] = nNewValue;
			else
				m_aRowSums[idxRow] += nNewValue;

			if ( nNewValue > 0 )
				m_aRowCountPositives[idxRow]++;
			m_aRowCountMissingValues[idxRow]--;
			SetRowAverage(idxRow);
		}
		else   // No missing values to worry about so just update totals
		{                   
			// Adjust column totals
			m_aColumnSums[idxColumn] += nNewValue - nOldValue;
			if ( nOldValue <= 0 && nNewValue > 0 )
				m_aColumnCountPositives[idxColumn]++;
			else if ( nOldValue > 0 && nNewValue <= 0 )
				m_aColumnCountPositives[idxColumn]--;
			SetColumnAverage(idxColumn);

			// Adjust row totals
			m_aRowSums[idxRow] += nNewValue - nOldValue;
			if ( nOldValue <= 0 && nNewValue > 0 )
				m_aRowCountPositives[idxRow]++;
			else if ( nOldValue > 0 && nNewValue <= 0 )
				m_aRowCountPositives[idxRow]--;
			SetRowAverage(idxRow);
		}

		SetModifiedFlag();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
	}
}

void CPatnDoc::SetColumnAverage(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	double nSum = m_aColumnSums[idxColumn];
	long nCountNonMissing = GetNoOfRows() - m_aColumnCountMissingValues[idxColumn];
	if ( nCountNonMissing != 0 )
		m_aColumnMean[idxColumn] = nSum/nCountNonMissing;
	else
		m_aColumnMean[idxColumn] = m_nMissingValue;

	long i;
	double value;
	for(i=0;i<GetNoOfRows() && !m_loading;i++){
		value = GetValueAt(idxColumn,i);
		if(m_aColumnMinimum[idxColumn] > value || m_aColumnMinimum[idxColumn] == m_nMissingValue)
			m_aColumnMinimum[idxColumn] = value;
		if(m_aColumnMaximum[idxColumn] < value || m_aColumnMaximum[idxColumn] == m_nMissingValue)
			m_aColumnMaximum[idxColumn] = value;
	}
}

void CPatnDoc::SetRowAverage(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	double nSum = m_aRowSums[idxRow];
	long nCountNonMissing = GetNoOfColumns() - m_aRowCountMissingValues[idxRow];
	if ( nCountNonMissing != 0 )
		m_aRowMean[idxRow] = nSum/nCountNonMissing;
	else
		m_aRowMean[idxRow] = m_nMissingValue;
	
	long i;
	double value;
	for(i=0;i<GetNoOfColumns() && !m_loading;i++){
		value = GetValueAt(i,idxRow);
		if(m_aRowMinimum[idxRow] > value || m_aRowMinimum[idxRow] == m_nMissingValue)
			m_aRowMinimum[idxRow] = value;
		if(m_aRowMaximum[idxRow] < value || m_aRowMaximum[idxRow] == m_nMissingValue)
			m_aRowMaximum[idxRow] = value;
	}
}

void CPatnDoc::SetMissingValue(CString strMissingValue)
{
	double nMissingValue = atof(strMissingValue);
	SetMissingValue(nMissingValue); 

	SetModifiedFlag();
//	UpdateAllTotals();
//	UpdateIntrinsicTotals();
}

// set missing value
void CPatnDoc::SetMissingValue(double nMissingValue)
{
	if ( nMissingValue == m_nMissingValue ) // no changes required
		return;

//	long nRow, nCol;
	long nNoOfRows = GetNoOfRows();
	long nNoOfColumns = GetNoOfColumns();
//	double nValue;

	BeginWaitCursor();

/*	for ( nRow = 0; nRow < nNoOfRows; nRow++ )
	{
		for ( nCol = 0; nCol < nNoOfColumns; nCol++ )
		{
			nValue = GetValueAt(nCol,nRow);

			if ( nValue == m_nMissingValue ) // == old missing value
			{
				// check whether the column previously contained all missing values
				if ( m_aColumnSums[nCol] == nMissingValue )
					m_aColumnSums[nCol] = nValue;
				else
					m_aColumnSums[nCol] += nValue;

				// check whether the row previously contained all missing values
				if ( m_aRowSums[nRow] == nMissingValue )
					m_aRowSums[nRow] = nValue;
				else
					m_aRowSums[nRow] += nValue;

				if ( nValue > 0 )
				{
					m_aColumnCountPositives[nCol]++;
					m_aRowCountPositives[nRow]++;
				}
				m_aColumnCountMissingValues[nCol]--;
				m_aRowCountMissingValues[nRow]--;
			}
			else if ( nValue == nMissingValue ) // == new missing value
			{
				m_aColumnCountMissingValues[nCol]++;
				m_aRowCountMissingValues[nRow]++;

				// check whether the column now contains all missing values
				if ( m_aColumnCountMissingValues[nCol] == GetNoOfRows() )
					m_aColumnSums[nCol] = nMissingValue;
				else
					m_aColumnSums[nCol] -= nValue;

				// check whether the column now contains all missing values
				if ( m_aRowCountMissingValues[nCol] == GetNoOfRows() )
					m_aRowSums[nRow] = nMissingValue;
				else
					m_aRowSums[nRow] -= nValue;

				if ( nValue > 0 )
				{
					m_aColumnCountPositives[nCol]--;
					m_aRowCountPositives[nRow]--;
				}
			}
		}
	}

	// adjust averages for rows
	for ( nRow = 0; nRow < nNoOfRows; nRow++ )
		SetRowAverage(nRow);

	// adjust averages for columns
	for ( nCol = 0; nCol < nNoOfColumns; nCol++ )
		SetColumnAverage(nCol);
*/
	// finally, replace the old missing value with the new missing value
	m_nMissingValue = nMissingValue;

	EndWaitCursor();

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

// set column attributes
void CPatnDoc::SetColumnWidth(long idxColumn, double nWidth)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	if ( nWidth <= 0.0 )
	{
		HideColumn(idxColumn);
		return;
	}

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];
	pColumnAttributes->SetWidth(nWidth);

	//SetModifiedFlag();
	//UpdateIntrinsicTotals();
	//UpdateAllTotals();
}

void CPatnDoc::SetColumnLabel(long idxColumn, CString str)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];
	pColumnAttributes->SetLabel(str);

	SetModifiedFlag();
	if(!m_loading) {UpdateIntrinsicTotals();UpdateAllTotals();}
}

void CPatnDoc::SetColumnDecimalPrecision(long idxColumn, long nDecimalPrecision)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());
	ASSERT(nDecimalPrecision >= 0);

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];

	if ( nDecimalPrecision >= 0 )
		pColumnAttributes->SetDecimalPrecision(nDecimalPrecision);

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::HideColumn(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];
	pColumnAttributes->Hide();

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::HideColumns(CArray<long>* pItems)
{
	long nCount = pItems->GetSize();
	long i;
	long idxColumn;

	for ( i = 0; i < nCount; i++ )
	{
		idxColumn = pItems->GetAt(i);
		HideColumn(idxColumn);
	}
}

void CPatnDoc::UnhideColumn(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];
	pColumnAttributes->Unhide();

	SetModifiedFlag();
	UpdateIntrinsicTotals();UpdateAllTotals();
}

void CPatnDoc::UnhideColumns(CArray<long>* pItems)
{
	long nCount = pItems->GetSize();
	long i;
	long idxColumn;

	for ( i = 0; i < nCount; i++ )
	{
		idxColumn = pItems->GetAt(i);
		UnhideColumn(idxColumn);
	}
}

void CPatnDoc::AdjustColumnWidthToFit(long idxColumn, CDC* pDC)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	CColumnAttributes* pColumnAttributes = m_aColumnAttributes[idxColumn];

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::AdjustColumnWidthToFit(CArray<long>* pItems, CDC* pDC)
{
	long nCount = pItems->GetSize();
	long i;
	long idxColumn;

	for ( i = 0; i < nCount; i++ )
	{
		idxColumn = pItems->GetAt(i);
		AdjustColumnWidthToFit(idxColumn, pDC);
	}
}

// set,get row attributes
void CPatnDoc::SetRowHeight(long idxRow, double nHeight)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	if ( nHeight <= 0.0 )
	{
		HideRow(idxRow);
		return;
	}

	CRowAttributes* pRowAttributes = m_aRowAttributes[idxRow];
	pRowAttributes->SetHeight(nHeight);

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::SetRowLabel(long idxRow, CString str)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	CRowAttributes* pRowAttributes = m_aRowAttributes[idxRow];
	pRowAttributes->SetLabel(str);

	SetModifiedFlag();
	if(!m_loading) {UpdateIntrinsicTotals();UpdateAllTotals();}

}

void CPatnDoc::SetRowGroups(long idxRow, long grp, double dist)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	m_aRowGroups[idxRow] = grp;
	m_aRowGroupsDist[idxRow] = dist;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::SetColumnGroups(long idxColumn, long grp)
{
	ASSERT(idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());

	m_aColumnGroups[idxColumn] = grp;
	
	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}


void CPatnDoc::HideRow(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	CRowAttributes* pRowAttributes = m_aRowAttributes[idxRow];
	pRowAttributes->Hide();

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::HideRows(CArray<long>* pItems)
{
	long nCount = pItems->GetSize();
	long i;
	long idxRow;

	for ( i = 0; i < nCount; i++ )
	{
		idxRow = pItems->GetAt(i);
		HideRow(idxRow);
	}
}

void CPatnDoc::UnhideRow(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow <= GetUpperBoundRows());

	CRowAttributes* pRowAttributes = m_aRowAttributes[idxRow];
	pRowAttributes->Unhide();

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::UnhideRows(CArray<long>* pItems)
{
	long nCount = pItems->GetSize();
	long i;
	long idxRow;

	for ( i = 0; i < nCount; i++ )
	{
		idxRow = pItems->GetAt(i);
		UnhideRow(idxRow);
	}
}

// table dimensions
long CPatnDoc::IsColumnInTable(long idxColumn)
{
	return (idxColumn >= 0 && idxColumn <= GetUpperBoundColumns());
}

long CPatnDoc::IsRowInTable(long idxRow)
{
	return (idxRow >= 0 && idxRow <= GetUpperBoundRows());
}

long CPatnDoc::IsCellInTable(long idxColumn, long idxRow)
{
	return (IsColumnInTable(idxColumn) && IsRowInTable(idxRow));
}

long CPatnDoc::GetIdxNextVisibleColumn(long idxColumn)
{
	long i = idxColumn;
	long idx = 0;
	long nCount = GetNoOfColumns();

	while ( ++i < nCount )
	{
		if ( !IsColumnHidden(i) )
		{
			idx = i;
			break;
		}
	}

	return idx;
}

long CPatnDoc::GetIdxPreviousVisibleColumn(long idxColumn)
{
	long i = idxColumn;
	long idx = 0;

	while ( --i >=0 )
	{
		if ( !IsColumnHidden(i) )
		{
			idx = i;
			break;
		}
	}

	return idx;
}

long CPatnDoc::GetIdxNextVisibleRow(long idxRow)
{
	long i = idxRow;
	long idx = 0;
	long nCount = GetNoOfRows();

	while ( ++i < nCount )
	{
		if ( !IsRowHidden(i) )
		{
			idx = i;
			break;
		}
	}

	return idx;
}

long CPatnDoc::GetIdxPreviousVisibleRow(long idxRow)
{
	long i = idxRow;
	long idx = 0;

	while ( --i >=0 )
	{
		if ( !IsRowHidden(i) )
		{
			idx = i;
			break;
		}
	}

	return idx;
}

// extrinsic columns
void CPatnDoc::MakeColumnsExtrinsic(CArray<int>* pItems)
{
	long nCount = pItems->GetSize();
	long idxFrom, idxTo;
	long i;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}


//	::sort(pItems);
	if((!IsExtrinsicColumns() || m_idxLeftExtrinsicColumn >= GetNoOfColumns()) && 
		nCount == 1 && pItems->GetAt(0) == GetNoOfColumns()-1)
		m_idxLeftExtrinsicColumn = GetNoOfColumns()-1;
	else if ( !IsExtrinsicColumns() )
		m_idxLeftExtrinsicColumn = GetNoOfColumns();
	
	
	idxTo = GetNoOfColumns();

	for ( i = 0; i < nCount; i++ )
	{
		idxFrom = pItems->GetAt(i) - i;
		if ( idxFrom >= m_idxLeftExtrinsicColumn )
			break;

		MoveColumn(idxFrom,idxTo);
	}

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::MakeColumnsIntrinsic(CArray<int>* pItems)
{
	long nCount = pItems->GetSize();
	long idxFrom;
	long i;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	if ( !IsExtrinsicColumns() ) // there are no extrinsic columns to make intrinsic
		return;

//	::sort(pItems);

	for ( i = 0; i < nCount; i++ )
	{
		idxFrom = pItems->GetAt(i);
		if ( idxFrom >= m_idxLeftExtrinsicColumn )
		{
			MoveColumn(idxFrom,m_idxLeftExtrinsicColumn);
			m_idxLeftExtrinsicColumn++;
		}
	}	
		
	if ( m_idxLeftExtrinsicColumn >= GetNoOfColumns() )
		m_idxLeftExtrinsicColumn = -1;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::MakeAllColumnsExtrinsic()
{
	if ( m_idxLeftExtrinsicColumn == 0 ) // already all extrinsic
		return;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	m_idxLeftExtrinsicColumn = 0;
	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::MakeAllColumnsIntrinsic()
{
	if ( m_idxLeftExtrinsicColumn == -1 ) // already all intrinsic
		return;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}
	m_idxLeftExtrinsicColumn = -1;
	SetModifiedFlag();
	UpdateIntrinsicTotals();
	UpdateAllTotals();
}

void CPatnDoc::SetIdxLeftExtrinsicColumn(long idxColumn)
{
	if ( !IsColumnInTable(idxColumn) )
		return;

	if ( !(m_idxLeftExtrinsicColumn == idxColumn) )
	{
		m_idxLeftExtrinsicColumn = idxColumn;
		SetModifiedFlag();
		UpdateIntrinsicTotals();
	}
}

// extrinsic rows
void CPatnDoc::MakeRowsExtrinsic(CArray<int>* pItems)
{
	long nCount = pItems->GetSize();
	long idxFrom, idxTo;
	long i;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}
m_loading = true;
	if((!IsExtrinsicRows() || m_idxTopExtrinsicRow >= GetNoOfRows()) && 
		nCount == 1 && pItems->GetAt(0) == GetNoOfRows()-1)
		m_idxTopExtrinsicRow = GetNoOfRows()-1;
	else if ( !IsExtrinsicRows() )
		m_idxTopExtrinsicRow = GetNoOfRows();
	
	idxTo = GetNoOfRows();

	for ( i = 0; i < nCount; i++ )
	{
		idxFrom = pItems->GetAt(i) - i;
		if ( idxFrom >= m_idxTopExtrinsicRow )
			break;

		MoveRow(idxFrom,idxTo);
	}
m_loading = false;
	SetModifiedFlag();
	UpdateIntrinsicTotals();

}

void CPatnDoc::MakeRowsIntrinsic(CArray<int>* pItems)
{
	long nCount = pItems->GetSize();
	long idxFrom;
	long i;
	
	if ( !IsExtrinsicRows() ) // there are no extrinsic rows to make intrinsic
		return;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	for ( i = 0; i < nCount; i++ )
	{
		idxFrom = pItems->GetAt(i);
		if ( idxFrom >= m_idxTopExtrinsicRow )
		{
			MoveRow(idxFrom,m_idxTopExtrinsicRow);
			m_idxTopExtrinsicRow++;
		}
	}	
		
	if ( m_idxTopExtrinsicRow >= GetNoOfRows() )
		m_idxTopExtrinsicRow = -1;

	SetModifiedFlag();
	UpdateIntrinsicTotals();
}

void CPatnDoc::MakeAllRowsExtrinsic()
{
	if ( m_idxTopExtrinsicRow == 0 ) // already all extrinsic
		return;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange || !m_OrdChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	m_idxTopExtrinsicRow = 0;
	SetModifiedFlag();
	UpdateIntrinsicTotals();
}

void CPatnDoc::MakeAllRowsIntrinsic()
{
	if ( m_idxTopExtrinsicRow == -1 ) // already all intrinsic
		return;

	if((!m_TwayChange || !m_AssocRowChange ||
		!m_AssocColChange || !m_FuseRowChange ||
		!m_FuseColChange) &&
		(!((CPatnApp*)AfxGetApp())->silent_mode) && (AfxMessageBox("This will invalidate anaysis data",
		MB_OKCANCEL | MB_ICONSTOP )) == IDCANCEL)
	{
		return;
	}else{
		//Graphs invalid
		m_TwayChange = 1;
		m_AssocRowChange = 1;
		m_AssocColChange = 1;
		m_FuseRowChange = 1;
		m_FuseColChange = 1;
		m_OrdChange = 1;
	}

	m_idxTopExtrinsicRow = -1;
	SetModifiedFlag();
	UpdateIntrinsicTotals();
}

void CPatnDoc::SetIdxTopExtrinsicRow(long idxRow)
{
	if ( !IsRowInTable(idxRow) )
		return;

	if ( !(m_idxTopExtrinsicRow == idxRow ))
	{
		m_idxTopExtrinsicRow = idxRow;
		SetModifiedFlag();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
	}
}

void CPatnDoc::SetDefaultFontsAll()
{
	SetDefaultFontCells();
	//SetDefaultFontAssocMatrix();
	//SetDefaultFontFusionTable();
	//SetDefaultFontDendrogram();
	//SetDefaultFontTway();
	//SetDefaultFontGsta();
	//SetDefaultFontBW();
	//SetDefaultFont3d();
}

void CPatnDoc::SetDefaultFontCells()
{
	m_nPointSizeCells = 100; // in tenths of a point
	m_rgbFontCells = RGB(0,0,0);

	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_lfCells.lfHeight = -13;
	m_lfCells.lfWidth = 0;
	m_lfCells.lfEscapement = 0;
	m_lfCells.lfOrientation = 0;
	m_lfCells.lfWeight = FW_NORMAL;
	m_lfCells.lfItalic = FALSE;
	m_lfCells.lfUnderline = FALSE;
	m_lfCells.lfStrikeOut = FALSE;
	m_lfCells.lfCharSet = ANSI_CHARSET;
	m_lfCells.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfCells.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfCells.lfQuality = PROOF_QUALITY;
	m_lfCells.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

	strcpy(m_lfCells.lfFaceName, ((CPatnApp*)AfxGetApp())->m_globalFontName);
}
/*
void CPatnDoc::SetDefaultFontTway()
{
	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_twayfont.lfHeight = -16;
	m_twayfont.lfWidth = 0;
	m_twayfont.lfEscapement = 0;
	m_twayfont.lfOrientation = 0;
	m_twayfont.lfWeight = FW_NORMAL;
	m_twayfont.lfItalic = FALSE;
	m_twayfont.lfUnderline = FALSE;
	m_twayfont.lfStrikeOut = FALSE;
	m_twayfont.lfCharSet = ANSI_CHARSET;
	m_twayfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_twayfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_twayfont.lfQuality = PROOF_QUALITY;
	m_twayfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_twayfont.lfFaceName,"Arial");
}

void CPatnDoc::SetDefaultFontBW()
{
	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_BWFont.lfHeight = -16;
	m_BWFont.lfWidth = 0;
	m_BWFont.lfEscapement = 0;
	m_BWFont.lfOrientation = 0;
	m_BWFont.lfWeight = FW_NORMAL;
	m_BWFont.lfItalic = FALSE;
	m_BWFont.lfUnderline = FALSE;
	m_BWFont.lfStrikeOut = FALSE;
	m_BWFont.lfCharSet = ANSI_CHARSET;
	m_BWFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_BWFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_BWFont.lfQuality = PROOF_QUALITY;
	m_BWFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_BWFont.lfFaceName,"Arial");
}
void CPatnDoc::SetDefaultFontGsta()
{
	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_gstafont.lfHeight = -16;
	m_gstafont.lfWidth = 0;
	m_gstafont.lfEscapement = 0;
	m_gstafont.lfOrientation = 0;
	m_gstafont.lfWeight = FW_NORMAL;
	m_gstafont.lfItalic = FALSE;
	m_gstafont.lfUnderline = FALSE;
	m_gstafont.lfStrikeOut = FALSE;
	m_gstafont.lfCharSet = ANSI_CHARSET;
	m_gstafont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_gstafont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_gstafont.lfQuality = PROOF_QUALITY;
	m_gstafont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_gstafont.lfFaceName,"Arial");
}
void CPatnDoc::SetDefaultFont3d()
{
	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_3dfont.lfHeight = -15;
	m_3dfont.lfWidth = 0;
	m_3dfont.lfEscapement = 0;
	m_3dfont.lfOrientation = 0;
	m_3dfont.lfWeight = 400;//FW_NORMAL;
	m_3dfont.lfItalic = FALSE;
	m_3dfont.lfUnderline = FALSE;
	m_3dfont.lfStrikeOut = FALSE;
	m_3dfont.lfCharSet = 0;//ANSI_CHARSET;
	m_3dfont.lfOutPrecision = 3;//OUT_DEFAULT_PRECIS;
	m_3dfont.lfClipPrecision = 2;//CLIP_DEFAULT_PRECIS;
	m_3dfont.lfQuality = 1;//PROOF_QUALITY;
	m_3dfont.lfPitchAndFamily = 18;//VARIABLE_PITCH | FF_SWISS;
	strcpy(m_3dfont.lfFaceName,"Times");
}



void CPatnDoc::SetDefaultFontAssocMatrix()
{
	m_nAssocMxFontPointSize = 10; ///****TODO - change to tenths of a point ***
	m_rgbAssocMxText = RGB(0,0,0);

	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_lfAssocMx.lfHeight = 0;
	m_lfAssocMx.lfWidth = 0;
	m_lfAssocMx.lfEscapement = 0;
	m_lfAssocMx.lfOrientation = 0;
	m_lfAssocMx.lfWeight = FW_NORMAL;
	m_lfAssocMx.lfItalic = FALSE;
	m_lfAssocMx.lfUnderline = FALSE;
	m_lfAssocMx.lfStrikeOut = FALSE;
	m_lfAssocMx.lfCharSet = ANSI_CHARSET;
	m_lfAssocMx.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfAssocMx.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfAssocMx.lfQuality = PROOF_QUALITY;
	m_lfAssocMx.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_lfAssocMx.lfFaceName,"Arial");
}

void CPatnDoc::SetDefaultFontFusionTable()
{
	m_nFuseFontPointSize = 10; ///***TODO - change to tenths of a point ****
	m_rgbFuseText = RGB(0,0,0);

	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_lfFuse.lfHeight = 0;
	m_lfFuse.lfWidth = 0;
	m_lfFuse.lfEscapement = 0;
	m_lfFuse.lfOrientation = 0;
	m_lfFuse.lfWeight = FW_NORMAL;
	m_lfFuse.lfItalic = FALSE;
	m_lfFuse.lfUnderline = FALSE;
	m_lfFuse.lfStrikeOut = FALSE;
	m_lfFuse.lfCharSet = ANSI_CHARSET;
	m_lfFuse.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfFuse.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfFuse.lfQuality = PROOF_QUALITY;
	m_lfFuse.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_lfFuse.lfFaceName,"Arial");
}

void CPatnDoc::SetDefaultFontDendrogram()
{
	m_nDendFontPointSize = 9; ///****TODO - change to tenths of a point ***
	m_rgbDendText = RGB(0,0,0);

	// lfHeight to be set by view using m_nPointSizeCells and based on pixels per logical inch
	m_lfDend.lfHeight = 0;
	m_lfDend.lfWidth = 0;
	m_lfDend.lfEscapement = 0;
	m_lfDend.lfOrientation = 0;
	m_lfDend.lfWeight = FW_NORMAL;
	m_lfDend.lfItalic = FALSE;
	m_lfDend.lfUnderline = FALSE;
	m_lfDend.lfStrikeOut = FALSE;
	m_lfDend.lfCharSet = ANSI_CHARSET;
	m_lfDend.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lfDend.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lfDend.lfQuality = PROOF_QUALITY;
	m_lfDend.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	strcpy(m_lfDend.lfFaceName,"Arial");
}*/

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc serialization


/*	double m_RowAsim_stat;
	double m_RowAsim_rbest;
	double m_RowAsim_save;
	double m_ColumnAsim_stat;
	double m_ColumnAsim_rbest;
	double m_ColumnAsim_save;
	CString m_RowAsim_time;
	CString m_ColumnAsim_time;
	CString m_RowAsim_asso;
	CString m_ColumnAsim_asso;
	long m_RowAsim_iterations;
	long m_ColumnAsim_iterations;
	long m_RowAsim_seed;
	long m_ColumnAsim_seed;
	long m_ColumnAsim_pairs;
	long m_RowAsim_pairs;
*/

void CPatnDoc::Serialize(CArchive& ar)
{
	//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//update visible window rect regions
	POSITION pos = this->GetFirstViewPosition();
	while(pos){
		CView * v = this->GetNextView(pos);
		v->RedrawWindow();
	}
	if(ar.IsStoring())
		RetrieveViewContents();

	//double version = 3.031;
	double version = 3.13;
	if(ar.IsStoring())
		//ar << (double) 2.09; //current version number
		ar << version;
	else
		ar >> version;

	//maintain existing backwards compatability
	if(version < 2.31){
		SerializeV2_30(ar,version);

		if(m_aRowAllocColumnAttributes.GetSize() < 2)
		{
			// Group (row a priori)
			CColumnAttributes * pColumnAttributes = new CColumnAttributes;
			pColumnAttributes->SetLabel("A Priori");
			pColumnAttributes->SetType(CRowColumnAttributes::Group);
			pColumnAttributes->SetWidth(g_nStandardColumnWidth);
			pColumnAttributes->SetDecimalPrecision(0);
			pColumnAttributes->Unhide();
			m_aRowAllocColumnAttributes.SetSize(2);
			m_aRowAllocColumnAttributes.SetAt(1,pColumnAttributes);
		}

		if(	m_Ord3d ) m_Ord3d = 3;
		else m_Ord3d = 2;
		if(m_AnalysisRecipe.rows.nOrd3d) m_AnalysisRecipe.rows.nOrd3d = 3;
		else m_AnalysisRecipe.rows.nOrd3d = 2;

		if(m_idxLeftExtrinsicColumn > GetNoOfColumns())
			m_idxLeftExtrinsicColumn = GetNoOfColumns();

		if(m_idxTopExtrinsicRow > GetNoOfRows())
			m_idxTopExtrinsicRow = GetNoOfRows();

		this->m_AnalysisRecipe.rows.dPower = 0.5;

		UpdateAprioriGroupsColours();

		return;
	}

	
	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	
	m_aColumnGroups.Serialize(ar);

	m_aRowGroups.Serialize(ar);
	if(version >= 3.1)
	{
        m_aRowGroupsDist.Serialize(ar);
		
	}
	else if(ar.IsLoading())
	{
		m_aRowGroupsDist.SetSize(m_aRowGroups.GetSize());
		for(i=0;i<m_aRowGroupsDist.GetSize();i++)
		{
			m_aRowGroupsDist[i] = GetMissingValue();
		}
	}
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	
	m_aRowTotalsColumnAttributes.Serialize(ar);
	
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	if(m_aRowAllocColumnAttributes.GetSize() < 3)
	{
		// Group (row a priori)
		CColumnAttributes * pColumnAttributes = new CColumnAttributes;
		pColumnAttributes->SetLabel("Aloc Distance");
		pColumnAttributes->SetType(CRowColumnAttributes::Group);
		pColumnAttributes->SetWidth(g_nStandardColumnWidth);
		pColumnAttributes->SetDecimalPrecision(0);
		pColumnAttributes->Unhide();
		m_aRowAllocColumnAttributes.SetSize(3);
		m_aRowAllocColumnAttributes.SetAt(2,pColumnAttributes);
	}

	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(ar.IsStoring()){
		ar << m_MCAOTable.GetSize();
		for(i=0;i<m_MCAOTable.GetSize();i++){
			ar << m_MCAOTable[i]->GetSize();
			for(j=0;j<m_MCAOTable[i]->GetSize();j++)
				ar << m_MCAOTable[i]->GetAt(j);
		}
	}else{
		ar >> nCount;m_MCAOTable.SetSize(nCount);
		for(i=0;i<m_MCAOTable.GetSize();i++){
			m_MCAOTable[i] = new CArray<double>;
			ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
			}
		}
	}
	
	m_aColumnSums.SetSize(GetNoOfColumns());
	m_aColumnCountMissingValues.SetSize(GetNoOfColumns());
	m_aColumnMaximum.SetSize(GetNoOfColumns());
	m_aColumnMinimum.SetSize(GetNoOfColumns());
	m_aColumnCountPositives.SetSize(GetNoOfColumns());
	m_aColumnMean.SetSize(GetNoOfColumns());
	
	m_aRowSums.SetSize(GetNoOfRows());
	m_aRowCountMissingValues.SetSize(GetNoOfRows());
	m_aRowMaximum.SetSize(GetNoOfRows());
	m_aRowMinimum.SetSize(GetNoOfRows());
	m_aRowCountPositives.SetSize(GetNoOfRows());
	m_aRowMean.SetSize(GetNoOfRows());
	
	long nSize;

	if (ar.IsStoring())
	{
		ar << m_GstaAvailable;
		ar << m_AssocRowsMinkConst;
		ar << m_AssocColsMinkConst;
		nCount = m_Vectors.GetSize();
		ar << nCount;
		nSize = m_Vectors[0]->GetSize();
		ar << nSize;
		for ( i = 0; i < nCount; i++ )
		{
			for(j=0;j<nSize;j++){
				if(j >= m_Vectors[i]->GetSize()){
					ar << GetMissingValue();
				}else{
					ar << m_Vectors[i]->GetAt(j);
				}
			}
		}

		nCount = m_UserGroupColours.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_UserGroupColours[i].red;
			ar << m_UserGroupColours[i].green;
			ar << m_UserGroupColours[i].blue;			
		}
		if(version >= 3.1){
			//nCount = m_aPrioriUserGroupColours.GetSize();
			//ar << nCount;
			//for ( i = 0; i < nCount; i++ )
			//{
			//	ar << m_aPrioriUserGroupColours[i].red;
			//	ar << m_aPrioriUserGroupColours[i].green;
			//	ar << m_aPrioriUserGroupColours[i].blue;			
			//}	
			nCount = this->m_AprioriGroups.GetSize();
			ar << nCount;
			for(i=0;i<nCount;i++){
				ar << m_AprioriGroups[i];
			}
		}
		ar << m_TwayChange;
		ar << m_AssocRowChange;
		ar << m_AssocColChange;
		ar << m_FuseRowChange;
		ar << m_FuseColChange;
		ar << m_OrdChange;
		ar << m_nMissingValue;
		ar << m_idxLeftExtrinsicColumn;
		ar << m_idxTopExtrinsicRow;

		nCount = m_FuseTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableRows.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}
		nCount = m_FuseTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableCols.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}

		nCount = m_DendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_DendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_DendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderRows[i];
		}
		nCount = m_DendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderCols[i];
		}

		nCount = m_GroupDendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_GroupDendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_GroupDendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderRows[i];
		}
		nCount = m_GroupDendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderCols[i];
		}
		

		ar.Write(&m_lfCells,sizeof(LOGFONT));
		ar << m_nPointSizeCells;
		ar << m_rgbFontCells;

		ar << m_nCutOffRows;
		ar << m_nCutOffCols;

		ar << m_FuseBetaRows;
		ar << m_FuseBetaCols;
		ar << m_FuseTimeRows;
		ar << m_FuseTimeCols;
		ar << m_FuseTypeRows;
		ar << m_FuseTypeCols;
		ar << m_FuseAssoRows;
		ar << m_FuseAssoCols;

		ar << m_OrdAsso;
		ar << m_OrdTime;
		ar << m_OrdValue;
		ar << m_Ord3d;
		ar << m_OrdStress;

		ar << m_TableName;
		ar << m_LastModified;

		ar << m_KWNumVisible;
		ar << m_KWOption;
		ar << m_KWCutoff;

		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_iterations;
		ar << m_RowAsim_seed;
		ar << m_ColumnAsim_seed;
		ar << m_KWExists;
		ar << m_MCAOExists;
		ar << m_ANOSIMRowsExists;
		ar << m_ANOSIMColsExists;
		ar << m_MCAOIterations;
		ar << m_MCAOSeed;

		ar << m_log;

		//write all new anosim values
		i = (m_ColumnAsim_pairs)?1:0; ar << i;
		i = (m_RowAsim_pairs)?1:0; ar << i;
		
		ar << m_RowPairStat.GetSize();
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar << m_RowPairStat[i]->GetSize();
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar << m_RowPairStat[i]->GetAt(j);
				ar << m_RowPairBest[i]->GetAt(j);
				ar << m_RowPairSave[i]->GetAt(j);
			}
		}
		ar << m_ColPairStat.GetSize();
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar << m_ColPairStat[i]->GetSize();
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar << m_ColPairStat[i]->GetAt(j);
				ar << m_ColPairBest[i]->GetAt(j);
				ar << m_ColPairSave[i]->GetAt(j);
			}
		}
	
		ar << m_AnosimRowGroups.GetSize();
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar << m_AnosimRowGroups[i];
		
		ar << m_AnosimColGroups.GetSize();
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar << m_AnosimColGroups[i];

		ar << m_RowAsim_stat;
		ar << m_RowAsim_rbest;
		ar << m_RowAsim_save;
		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_stat;
		ar << m_ColumnAsim_rbest;
		ar << m_ColumnAsim_save;
		ar << m_ColumnAsim_iterations;
		ar << m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.cols.nAssocMeasure;
		ar << m_AnalysisRecipe.cols.nBeta;
		ar << m_AnalysisRecipe.cols.nClassStrategy;
		ar << m_AnalysisRecipe.cols.nCut;
		ar << m_AnalysisRecipe.cols.nFuseTechnique;
		ar << m_AnalysisRecipe.cols.nMinkConst;
		ar << m_AnalysisRecipe.cols.nNoOfGroups;
		ar << m_AnalysisRecipe.cols.nOrd3d;
		ar << m_AnalysisRecipe.cols.nOrdination;
		ar << m_AnalysisRecipe.cols.nOrdIterations;
		ar << m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar << m_AnalysisRecipe.cols.nOrdSeed;
		ar << m_AnalysisRecipe.nFirstTime;
		ar << m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.rows.nAssocMeasure;
		ar << m_AnalysisRecipe.rows.nBeta;
		ar << m_AnalysisRecipe.rows.nClassStrategy;
		ar << m_AnalysisRecipe.rows.nCut;
		ar << m_AnalysisRecipe.rows.nFuseTechnique;
		ar << m_AnalysisRecipe.rows.nMinkConst;
		ar << m_AnalysisRecipe.rows.nNoOfGroups;
		ar << m_AnalysisRecipe.rows.nOrd3d;
		ar << m_AnalysisRecipe.rows.nOrdination;
		ar << m_AnalysisRecipe.rows.nOrdIterations;
		ar << m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar << m_AnalysisRecipe.rows.nOrdSeed;
	
		ar << m_pGstaVisible;
		ar << m_AssocRowVisible;
		ar << m_AssocColVisible;
		ar << m_AssocRowUpdate;
		ar << m_AssocColUpdate;
		ar << m_FuseRowVisible;
		ar << m_FuseColVisible;
		ar << m_FuseRowUpdate;
		ar << m_FuseColUpdate;
		ar << m_nScale;
		ar << m_pExists;
		ar << m_pIsRowLabel;
		ar << m_colstand;
		ar << m_presabs;
		ar << m_globstand;
		ar << m_red;
		ar << m_green;
		ar << m_blue;
		ar << m_bw;
		ar << m_AnosimPairView;
		ar << m_AnosimColView; 
		ar << m_AnosimRowView; 
		ar << m_OrdVisible;
		ar << m_OrdXVisible;
		ar << m_OrdUpdate;
		ar << m_boxwhiskervisible;
		ar << mr_tableview;
		ar << mr_rowtotals;
		ar << mr_coltotals;
		ar << mr_anosimrow;
		ar << mr_anosimcol;
		ar << mr_fuserow;
		ar << mr_fusecol;
		ar << mr_fuserowdend;
		ar << mr_fusecoldend;
		ar << mr_assocrowmx;
		ar << mr_assoccolmx;
		ar << mr_assocrowhist;
		ar << mr_assoccolhist;
		ar << mr_ord;
		ar << mr_tway;
		ar << mr_gsta;
		ar << mr_boxwhisker;
		
		ar << m_Rotate;
		ar << m_Labels;
		ar << m_Legend;
		ar << m_Mst;
		ar << m_Axies;
		ar << m_Grouping;
		ar << m_Usergroupcolours;
		ar << m_SelOnly;
		ar << m_Centroid;
		ar << m_Showranges;
		ar << m_Symbols;
		ar << m_RotationSpeed;
		ar << m_TextBoxes.GetSize();
		for(i=0;i<m_TextBoxes.GetSize();i++){
			ar << m_TextBoxes[i]->contents;
			ar << m_TextBoxes[i]->size;
			ar << m_TextBoxes[i]->location;
		}
		ar << m_TextBoxes2.GetSize();
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			ar << m_TextBoxes2[i]->contents;
			ar << m_TextBoxes2[i]->location;
			ar << m_TextBoxes2[i]->colour;
			ar.Write(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}
		ar << m_stressbox.contents;
		ar << m_stressbox.location;
		ar << m_stressbox.colour;
		ar.Write(&m_stressbox.font,sizeof(LOGFONT));

		ar << m_vEyePt.x;ar << m_vEyePt.y;ar << m_vEyePt.z;
		ar << m_vUpVec.x;ar << m_vUpVec.y;ar << m_vUpVec.z;
		ar << m_vRight.x;ar << m_vRight.y;ar << m_vRight.z;
		
		ar << wAngleY; 
		ar << m_NoSymbols;
		ar << m_PccVectors.GetSize();
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar << m_PccVectors[i];
		}

		nCount = m_aGroupColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aGroupColours2[i].red;
			ar << m_aGroupColours2[i].green;
			ar << m_aGroupColours2[i].blue;			
		}
		nCount = m_aObjectColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aObjectColours2[i].red;
			ar << m_aObjectColours2[i].green;
			ar << m_aObjectColours2[i].blue;			
		}
		nCount = m_r.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_r[i].red;
			ar << m_r[i].green;
			ar << m_r[i].blue;			
		}
		nCount = m_Vectors2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_Vectors2[i].red;
			ar << m_Vectors2[i].green;
			ar << m_Vectors2[i].blue;			
		}			
	
		//2.18 (some is above)
		m_RowSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}
		m_ColSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}		

		//2.19
		i = m_createKW;ar << i;
		i = m_createANOSIMRows;ar << i;
		i = m_createANOSIMCols;ar << i;
		i = m_createMCAO;ar << i;
		i = m_createPCC;ar << i;

		//2.25
		ar << m_visible_decimals;
		ar.Write(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

		//2.26
		//row and col control groups
		ar << m_RowControlGroups.GetSize();
		for(i=0;i<m_RowControlGroups.GetSize();i++){
			ar << m_RowControlGroups[i]->GetSize();
			for(long j=0;j<m_RowControlGroups[i]->GetSize();j++){
				ar << m_RowControlGroups[i]->GetAt(j);
			}
		}
		ar << m_ColControlGroups.GetSize();
		for(i=0;i<m_ColControlGroups.GetSize();i++){
			ar << m_ColControlGroups[i]->GetSize();
			for(long j=0;j<m_ColControlGroups[i]->GetSize();j++){
				ar << m_ColControlGroups[i]->GetAt(j);
			}
		}

		//2.27
		//apriori group patterns (rowcontrolgroup vs pattern)
		ar << m_PatternApriori.GetSize();
		for(i=0;i<m_PatternApriori.GetSize();i++)
			ar << m_PatternApriori[i];

		//2.31
		ar << m_RowAsim_time;
		ar << m_ColumnAsim_time;
		ar << m_RowAsim_asso;
		ar << m_ColumnAsim_asso;
		ar << m_import_ord;

		//3.01
		ar << m_AnalysisRecipe.rows.nOverride;

		//3.03
		ar << m_AnalysisRecipe.rows.dPower;

		//3.04
		i = (m_RowAsim_patn_groups)?1:0; ar << i;

		//3.041
		i = (m_kw_patn_generated_groups)?1:0; ar << i;

		//3.1
		if(version >= 3.1)
			ar << m_kwtime;
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		CArray<colour> aPrioriUserGroupColours;
		if(version >= 3.1 && version < 3.13){
			ar >> nCount;
			aPrioriUserGroupColours.SetSize(nCount);
			for ( i = 0; i < nCount; i++ )
			{
				ar >> aPrioriUserGroupColours[i].red;
				ar >> aPrioriUserGroupColours[i].green;
				ar >> aPrioriUserGroupColours[i].blue;			
			}
			
		}
		if(version >= 3.1)
		{
			ar >> nCount;m_AprioriGroups.SetSize(nCount);			
			for(i=0;i<nCount;i++){
				ar >> m_AprioriGroups[i];
			}
		}
		
		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		if(m_idxLeftExtrinsicColumn > GetNoOfColumns())
			m_idxLeftExtrinsicColumn = GetNoOfColumns();

		ar >> m_idxTopExtrinsicRow;
		if(m_idxTopExtrinsicRow > GetNoOfRows())
			m_idxTopExtrinsicRow = GetNoOfRows();

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		if(version < 3.02)
		{
			if(	m_Ord3d ) m_Ord3d = 3;
			else m_Ord3d = 2;
		}
		
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;
		
		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		ar >> m_RowAsim_stat;
		ar >> m_RowAsim_rbest;
		ar >> m_RowAsim_save;
		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_stat;
		ar >> m_ColumnAsim_rbest;
		ar >> m_ColumnAsim_save;
		ar >> m_ColumnAsim_iterations;
		ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nBeta;
		ar >> m_AnalysisRecipe.cols.nClassStrategy;
		ar >> m_AnalysisRecipe.cols.nCut;
		ar >> m_AnalysisRecipe.cols.nFuseTechnique;
		ar >> m_AnalysisRecipe.cols.nMinkConst;
		ar >> m_AnalysisRecipe.cols.nNoOfGroups;
		ar >> m_AnalysisRecipe.cols.nOrd3d;
		ar >> m_AnalysisRecipe.cols.nOrdination;
		ar >> m_AnalysisRecipe.cols.nOrdIterations;
		ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.cols.nOrdSeed;
		ar >> m_AnalysisRecipe.nFirstTime;
		ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nBeta;
		ar >> m_AnalysisRecipe.rows.nClassStrategy;
		ar >> m_AnalysisRecipe.rows.nCut;
		ar >> m_AnalysisRecipe.rows.nFuseTechnique;
		ar >> m_AnalysisRecipe.rows.nMinkConst;
		ar >> m_AnalysisRecipe.rows.nNoOfGroups;
		ar >> m_AnalysisRecipe.rows.nOrd3d;
		if(version < 3.02)
		{
			if(m_AnalysisRecipe.rows.nOrd3d) m_AnalysisRecipe.rows.nOrd3d = 3;
			else m_AnalysisRecipe.rows.nOrd3d = 2;
		}
		ar >> m_AnalysisRecipe.rows.nOrdination;
		ar >> m_AnalysisRecipe.rows.nOrdIterations;
		ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.rows.nOrdSeed;
		
		ar >> i;m_pGstaVisible = (i)?true:false;
		ar >> m_AssocRowVisible;
		ar >> m_AssocColVisible;
		ar >> m_AssocRowUpdate;
		ar >> m_AssocColUpdate;
		ar >> m_FuseRowVisible;
		ar >> m_FuseColVisible;
		ar >> m_FuseRowUpdate;
		ar >> m_FuseColUpdate;
		ar >> m_nScale;
		ar >> m_pExists;
		ar >> m_pIsRowLabel;
		ar >> m_colstand;
		ar >> m_presabs;
		ar >> m_globstand;
		ar >> m_red;
		ar >> m_green;
		ar >> m_blue;
		ar >> m_bw;
		ar >> i;m_AnosimPairView = (i)?true:false;
		ar >> i;m_AnosimColView = (i)?true:false; 
		ar >> i;m_AnosimRowView = (i)?true:false; 
		ar >> i;m_OrdVisible = (i)?true:false;
		ar >> i;m_OrdXVisible = (i)?true:false;
		ar >> i;m_OrdUpdate = (i)?true:false;
		ar >> m_boxwhiskervisible;
		ar >> mr_tableview;
		ar >> mr_rowtotals;
		ar >> mr_coltotals;
		ar >> mr_anosimrow;
		ar >> mr_anosimcol;
		ar >> mr_fuserow;
		ar >> mr_fusecol;
		ar >> mr_fuserowdend;
		ar >> mr_fusecoldend;
		ar >> mr_assocrowmx;
		ar >> mr_assoccolmx;
		ar >> mr_assocrowhist;
		ar >> mr_assoccolhist;
		ar >> mr_ord;
		ar >> mr_tway;
		ar >> mr_gsta;
		ar >> mr_boxwhisker;

		ar >> i;m_Rotate = (i)?true:false;
		ar >> i;m_Labels = (i)?true:false;
		ar >> i;m_Legend = (i)?true:false;
		ar >> i;m_Mst = (i)?true:false;
		ar >> i;m_Axies = (i)?true:false;
		ar >> i;m_Grouping = (i)?true:false;
		ar >> i;m_Usergroupcolours = (i)?true:false;
		ar >> i;m_SelOnly = (i)?true:false;
		ar >> i;m_Centroid = (i)?true:false;
		ar >> i;m_Showranges = (i)?true:false;
		ar >> i;m_Symbols = (i)?true:false;
		ar >> m_RotationSpeed;
		long size;
		ar >> size;m_TextBoxes.SetSize(size);
		for(i=0;i<m_TextBoxes.GetSize();i++){
			m_TextBoxes[i] = new TextBox;
			ar >> m_TextBoxes[i]->contents;
			ar >> m_TextBoxes[i]->size;
			ar >> m_TextBoxes[i]->location;
		}			
		ar >> size;m_TextBoxes2.SetSize(size);
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			m_TextBoxes2[i] = new TextBox2;
			ar >> m_TextBoxes2[i]->contents;
			ar >> m_TextBoxes2[i]->location;
			ar >> m_TextBoxes2[i]->colour;
			ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}			
		ar >> m_stressbox.contents;
		ar >> m_stressbox.location;
		ar >> m_stressbox.colour;
		ar.Read(&m_stressbox.font,sizeof(LOGFONT));
		
		ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
		ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
		ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

		ar >> wAngleY; 
		ar >> i;m_NoSymbols = (i)?true:false;
		ar >> size;m_PccVectors.SetSize(size);
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar >> m_PccVectors[i];
		}

		ar >> nCount;
		m_aGroupColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aGroupColours2[i].red;
			ar >> m_aGroupColours2[i].green;
			ar >> m_aGroupColours2[i].blue;			
		}
		ar >> nCount;
		m_aObjectColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aObjectColours2[i].red;
			ar >> m_aObjectColours2[i].green;
			ar >> m_aObjectColours2[i].blue;			
		}
		ar >> nCount;
		m_r.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_r[i].red;
			ar >> m_r[i].green;
			ar >> m_r[i].blue;			
		}
		ar >> nCount;
		m_Vectors2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_Vectors2[i].red;
			ar >> m_Vectors2[i].green;
			ar >> m_Vectors2[i].blue;			
		}
		m_3ddefault = true;

		//2.18 (some is above)
		POSITION pos = this->GetFirstViewPosition();
		CView * pView;
		while(pos != NULL){
			pView = this->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
				break;
			}
			pView = NULL;
		}

		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedRows->SetAt(j);
		}	
		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
		}	

		//2.19
		ar >> i;m_createKW = (i)?true:false;
		ar >> i;m_createANOSIMRows = (i)?true:false;
		ar >> i;m_createANOSIMCols = (i)?true:false;
		ar >> i;m_createMCAO = (i)?true:false;
		ar >> i;m_createPCC = (i)?true:false;

		//2.25
		ar >> m_visible_decimals;
		ar.Read(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

		//2.26
		//row and col control groups
		ar >> i;m_RowControlGroups.SetSize(i);
		for(i=0;i<m_RowControlGroups.GetSize();i++){
			m_RowControlGroups[i] = new Rows;
			long j; ar >> j;m_RowControlGroups[i]->SetSize(j);			
			for(j=0;j<m_RowControlGroups[i]->GetSize();j++){
				long k; ar >> k; m_RowControlGroups[i]->SetAt(j,k);
			}
		}
		ar >> i;m_ColControlGroups.SetSize(i);
		for(i=0;i<m_ColControlGroups.GetSize();i++){
			m_ColControlGroups[i] = new Rows;
			long j; ar >> j;m_ColControlGroups[i]->SetSize(j);			
			for(j=0;j<m_ColControlGroups[i]->GetSize();j++){
				long k; ar >> k; m_ColControlGroups[i]->SetAt(j,k);
			}
		}

		//2.27
		if(version >= 2.27){
			//apriori group patterns (rowcontrolgroup vs pattern)
			ar >> i; m_PatternApriori.SetSize(i);
			for(i=0;i<m_PatternApriori.GetSize();i++){
				long j;
				ar >> j; m_PatternApriori[i] = j;
			}
		}
		//fix for pre 3.12 versions
		if(version < 3.12)
		{
			for(i=0;i<m_PatternApriori.GetSize()-1;i++)
			{
				m_PatternApriori[i] = m_PatternApriori[i+1];
			}
		}

		//2.31
		ar >> m_RowAsim_time;
		ar >> m_ColumnAsim_time;
		ar >> m_RowAsim_asso;
		ar >> m_ColumnAsim_asso;
		ar >> m_import_ord;

		//3.01
		if(version >= 3.01)
		{
			ar >> m_AnalysisRecipe.rows.nOverride;
		}
		else
		{
			m_AnalysisRecipe.rows.nOverride = true;
		}

		//3.03
		if(version >= 3.03)
		{
			ar >> m_AnalysisRecipe.rows.dPower;
		}
		else
		{
			m_AnalysisRecipe.rows.dPower = 0.5;
		}

		//3.031...
		if(version < 3.031 && m_aRowAllocColumnAttributes.GetSize() < 2)
		{
			// Group (row a priori)
			CColumnAttributes * pColumnAttributes = new CColumnAttributes;
			pColumnAttributes->SetLabel("A Priori");
			pColumnAttributes->SetType(CRowColumnAttributes::Group);
			pColumnAttributes->SetWidth(g_nStandardColumnWidth);
			pColumnAttributes->SetDecimalPrecision(0);
			pColumnAttributes->Unhide();
			m_aRowAllocColumnAttributes.SetSize(2);
			m_aRowAllocColumnAttributes.SetAt(1,pColumnAttributes);
		}

		//3.04
		if(version >= 3.04)
		{
			ar >> i;m_RowAsim_patn_groups = (i)?true:false;
		}
		else
		{
			m_RowAsim_patn_groups = false;
		}

		//3.041
		if(version >= 3.041)
		{
			ar >> i;m_kw_patn_generated_groups = (i)?true:false;

			if(version >= 3.1)
				ar>>m_kwtime;
			
		}
		else
		{
			m_kw_patn_generated_groups = false;
		}
m_loading = true;
		//functions to set up stuff not stored
		UpdateGroups();
m_loading = false;
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)

		CGsta cgsta(this);

		UpdateAprioriGroupsColours();

		SetupWindowsAfterLoad();

		SetModifiedFlag(false);
		int pp = IsModified();
	}


}


void CPatnDoc::SerializeV2_30(CArchive& ar,double version)
{
	//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//update visible window rect regions
	/*POSITION pos = this->GetFirstViewPosition();
	while(pos){
		CView * v = this->GetNextView(pos);
		v->RedrawWindow();
	}
	if(ar.IsStoring())
		RetrieveViewContents();

	//double version = 2.27;
	if(ar.IsStoring())
		//ar << (double) 2.09; //current version number
		ar << version;
	else
		ar >> version;
*/
	if(version < 2.26){
		SerializeV2_25(ar,version);

		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	
	m_aColumnGroups.Serialize(ar);

	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	
	m_aRowTotalsColumnAttributes.Serialize(ar);
	
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(ar.IsStoring()){
		ar << m_MCAOTable.GetSize();
		for(i=0;i<m_MCAOTable.GetSize();i++){
			ar << m_MCAOTable[i]->GetSize();
			for(j=0;j<m_MCAOTable[i]->GetSize();j++)
				ar << m_MCAOTable[i]->GetAt(j);
		}
	}else{
		ar >> nCount;m_MCAOTable.SetSize(nCount);
		for(i=0;i<m_MCAOTable.GetSize();i++){
			m_MCAOTable[i] = new CArray<double>;
			ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
			}
		}
	}
	
	m_aColumnSums.SetSize(GetNoOfColumns());
	m_aColumnCountMissingValues.SetSize(GetNoOfColumns());
	m_aColumnMaximum.SetSize(GetNoOfColumns());
	m_aColumnMinimum.SetSize(GetNoOfColumns());
	m_aColumnCountPositives.SetSize(GetNoOfColumns());
	m_aColumnMean.SetSize(GetNoOfColumns());
	
	m_aRowSums.SetSize(GetNoOfRows());
	m_aRowCountMissingValues.SetSize(GetNoOfRows());
	m_aRowMaximum.SetSize(GetNoOfRows());
	m_aRowMinimum.SetSize(GetNoOfRows());
	m_aRowCountPositives.SetSize(GetNoOfRows());
	m_aRowMean.SetSize(GetNoOfRows());
	
	long nSize;

	if (ar.IsStoring())
	{
		ar << m_GstaAvailable;
		ar << m_AssocRowsMinkConst;
		ar << m_AssocColsMinkConst;
		nCount = m_Vectors.GetSize();
		ar << nCount;
		nSize = m_Vectors[0]->GetSize();
		ar << nSize;
		for ( i = 0; i < nCount; i++ )
		{
			for(j=0;j<nSize;j++){
				if(j >= m_Vectors[i]->GetSize()){
					ar << GetMissingValue();
				}else{
					ar << m_Vectors[i]->GetAt(j);
				}
			}
		}

		nCount = m_UserGroupColours.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_UserGroupColours[i].red;
			ar << m_UserGroupColours[i].green;
			ar << m_UserGroupColours[i].blue;			
		}

		ar << m_TwayChange;
		ar << m_AssocRowChange;
		ar << m_AssocColChange;
		ar << m_FuseRowChange;
		ar << m_FuseColChange;
		ar << m_OrdChange;
		ar << m_nMissingValue;
		ar << m_idxLeftExtrinsicColumn;
		ar << m_idxTopExtrinsicRow;

		nCount = m_FuseTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableRows.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}
		nCount = m_FuseTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableCols.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}

		nCount = m_DendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_DendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_DendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderRows[i];
		}
		nCount = m_DendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderCols[i];
		}

		nCount = m_GroupDendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_GroupDendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_GroupDendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderRows[i];
		}
		nCount = m_GroupDendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderCols[i];
		}
		

		ar.Write(&m_lfCells,sizeof(LOGFONT));
		ar << m_nPointSizeCells;
		ar << m_rgbFontCells;

		ar << m_nCutOffRows;
		ar << m_nCutOffCols;

		ar << m_FuseBetaRows;
		ar << m_FuseBetaCols;
		ar << m_FuseTimeRows;
		ar << m_FuseTimeCols;
		ar << m_FuseTypeRows;
		ar << m_FuseTypeCols;
		ar << m_FuseAssoRows;
		ar << m_FuseAssoCols;

		ar << m_OrdAsso;
		ar << m_OrdTime;
		ar << m_OrdValue;
		ar << m_Ord3d;
		ar << m_OrdStress;

		ar << m_TableName;
		ar << m_LastModified;

		ar << m_KWNumVisible;
		ar << m_KWOption;
		ar << m_KWCutoff;

		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_iterations;
		ar << m_RowAsim_seed;
		ar << m_ColumnAsim_seed;
		ar << m_KWExists;
		ar << m_MCAOExists;
		ar << m_ANOSIMRowsExists;
		ar << m_ANOSIMColsExists;
		ar << m_MCAOIterations;
		ar << m_MCAOSeed;

		ar << m_log;

		//write all new anosim values
		ar << m_ColumnAsim_pairs;
		ar << m_RowAsim_pairs;

		ar << m_RowPairStat.GetSize();
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar << m_RowPairStat[i]->GetSize();
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar << m_RowPairStat[i]->GetAt(j);
				ar << m_RowPairBest[i]->GetAt(j);
				ar << m_RowPairSave[i]->GetAt(j);
			}
		}
		ar << m_ColPairStat.GetSize();
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar << m_ColPairStat[i]->GetSize();
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar << m_ColPairStat[i]->GetAt(j);
				ar << m_ColPairBest[i]->GetAt(j);
				ar << m_ColPairSave[i]->GetAt(j);
			}
		}
	
		ar << m_AnosimRowGroups.GetSize();
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar << m_AnosimRowGroups[i];
		
		ar << m_AnosimColGroups.GetSize();
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar << m_AnosimColGroups[i];

		ar << m_RowAsim_stat;
		ar << m_RowAsim_rbest;
		ar << m_RowAsim_save;
		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_stat;
		ar << m_ColumnAsim_rbest;
		ar << m_ColumnAsim_save;
		ar << m_ColumnAsim_iterations;
		ar << m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.cols.nAssocMeasure;
		ar << m_AnalysisRecipe.cols.nBeta;
		ar << m_AnalysisRecipe.cols.nClassStrategy;
		ar << m_AnalysisRecipe.cols.nCut;
		ar << m_AnalysisRecipe.cols.nFuseTechnique;
		ar << m_AnalysisRecipe.cols.nMinkConst;
		ar << m_AnalysisRecipe.cols.nNoOfGroups;
		ar << m_AnalysisRecipe.cols.nOrd3d;
		ar << m_AnalysisRecipe.cols.nOrdination;
		ar << m_AnalysisRecipe.cols.nOrdIterations;
		ar << m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar << m_AnalysisRecipe.cols.nOrdSeed;
		ar << m_AnalysisRecipe.nFirstTime;
		ar << m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.rows.nAssocMeasure;
		ar << m_AnalysisRecipe.rows.nBeta;
		ar << m_AnalysisRecipe.rows.nClassStrategy;
		ar << m_AnalysisRecipe.rows.nCut;
		ar << m_AnalysisRecipe.rows.nFuseTechnique;
		ar << m_AnalysisRecipe.rows.nMinkConst;
		ar << m_AnalysisRecipe.rows.nNoOfGroups;
		ar << m_AnalysisRecipe.rows.nOrd3d;
		ar << m_AnalysisRecipe.rows.nOrdination;
		ar << m_AnalysisRecipe.rows.nOrdIterations;
		ar << m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar << m_AnalysisRecipe.rows.nOrdSeed;
	
		ar << m_pGstaVisible;
		ar << m_AssocRowVisible;
		ar << m_AssocColVisible;
		ar << m_AssocRowUpdate;
		ar << m_AssocColUpdate;
		ar << m_FuseRowVisible;
		ar << m_FuseColVisible;
		ar << m_FuseRowUpdate;
		ar << m_FuseColUpdate;
		ar << m_nScale;
		ar << m_pExists;
		ar << m_pIsRowLabel;
		ar << m_colstand;
		ar << m_presabs;
		ar << m_globstand;
		ar << m_red;
		ar << m_green;
		ar << m_blue;
		ar << m_bw;
		ar << m_AnosimPairView;
		ar << m_AnosimColView; 
		ar << m_AnosimRowView; 
		ar << m_OrdVisible;
		ar << m_OrdXVisible;
		ar << m_OrdUpdate;
		ar << m_boxwhiskervisible;
		ar << mr_tableview;
		ar << mr_rowtotals;
		ar << mr_coltotals;
		ar << mr_anosimrow;
		ar << mr_anosimcol;
		ar << mr_fuserow;
		ar << mr_fusecol;
		ar << mr_fuserowdend;
		ar << mr_fusecoldend;
		ar << mr_assocrowmx;
		ar << mr_assoccolmx;
		ar << mr_assocrowhist;
		ar << mr_assoccolhist;
		ar << mr_ord;
		ar << mr_tway;
		ar << mr_gsta;
		ar << mr_boxwhisker;
		
		ar << m_Rotate;
		ar << m_Labels;
		ar << m_Legend;
		ar << m_Mst;
		ar << m_Axies;
		ar << m_Grouping;
		ar << m_Usergroupcolours;
		ar << m_SelOnly;
		ar << m_Centroid;
		ar << m_Showranges;
		ar << m_Symbols;
		ar << m_RotationSpeed;
		ar << m_TextBoxes.GetSize();
		for(i=0;i<m_TextBoxes.GetSize();i++){
			ar << m_TextBoxes[i]->contents;
			ar << m_TextBoxes[i]->size;
			ar << m_TextBoxes[i]->location;
		}
		ar << m_TextBoxes2.GetSize();
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			ar << m_TextBoxes2[i]->contents;
			ar << m_TextBoxes2[i]->location;
			ar << m_TextBoxes2[i]->colour;
			ar.Write(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}
		ar << m_stressbox.contents;
		ar << m_stressbox.location;
		ar << m_stressbox.colour;
		ar.Write(&m_stressbox.font,sizeof(LOGFONT));

		ar << m_vEyePt.x;ar << m_vEyePt.y;ar << m_vEyePt.z;
		ar << m_vUpVec.x;ar << m_vUpVec.y;ar << m_vUpVec.z;
		ar << m_vRight.x;ar << m_vRight.y;ar << m_vRight.z;
		
		ar << wAngleY; 
		ar << m_NoSymbols;
		ar << m_PccVectors.GetSize();
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar << m_PccVectors[i];
		}

		nCount = m_aGroupColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aGroupColours2[i].red;
			ar << m_aGroupColours2[i].green;
			ar << m_aGroupColours2[i].blue;			
		}
		nCount = m_aObjectColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aObjectColours2[i].red;
			ar << m_aObjectColours2[i].green;
			ar << m_aObjectColours2[i].blue;			
		}
		nCount = m_r.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_r[i].red;
			ar << m_r[i].green;
			ar << m_r[i].blue;			
		}
		nCount = m_Vectors2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_Vectors2[i].red;
			ar << m_Vectors2[i].green;
			ar << m_Vectors2[i].blue;			
		}			
	
		//2.18 (some is above)
		m_RowSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}
		m_ColSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}		

		//2.19
		i = m_createKW;ar << i;
		i = m_createANOSIMRows;ar << i;
		i = m_createANOSIMCols;ar << i;
		i = m_createMCAO;ar << i;
		i = m_createPCC;ar << i;

		//2.25
		ar << m_visible_decimals;
		ar.Write(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

		//2.26
		//row and col control groups
		ar << m_RowControlGroups.GetSize();
		for(i=0;i<m_RowControlGroups.GetSize();i++){
			ar << m_RowControlGroups[i]->GetSize();
			for(long j=0;j<m_RowControlGroups[i]->GetSize();j++){
				ar << m_RowControlGroups[i]->GetAt(j);
			}
		}
		ar << m_ColControlGroups.GetSize();
		for(i=0;i<m_ColControlGroups.GetSize();i++){
			ar << m_ColControlGroups[i]->GetSize();
			for(long j=0;j<m_ColControlGroups[i]->GetSize();j++){
				ar << m_ColControlGroups[i]->GetAt(j);
			}
		}

		//2.27
		//apriori group patterns (rowcontrolgroup vs pattern)
		ar << m_PatternApriori.GetSize();
		for(i=0;i<m_PatternApriori.GetSize();i++)
			ar << m_PatternApriori[i];
		
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		ar >> m_RowAsim_stat;
		ar >> m_RowAsim_rbest;
		ar >> m_RowAsim_save;
		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_stat;
		ar >> m_ColumnAsim_rbest;
		ar >> m_ColumnAsim_save;
		ar >> m_ColumnAsim_iterations;
		ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nBeta;
		ar >> m_AnalysisRecipe.cols.nClassStrategy;
		ar >> m_AnalysisRecipe.cols.nCut;
		ar >> m_AnalysisRecipe.cols.nFuseTechnique;
		ar >> m_AnalysisRecipe.cols.nMinkConst;
		ar >> m_AnalysisRecipe.cols.nNoOfGroups;
		ar >> m_AnalysisRecipe.cols.nOrd3d;
		ar >> m_AnalysisRecipe.cols.nOrdination;
		ar >> m_AnalysisRecipe.cols.nOrdIterations;
		ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.cols.nOrdSeed;
		ar >> m_AnalysisRecipe.nFirstTime;
		ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nBeta;
		ar >> m_AnalysisRecipe.rows.nClassStrategy;
		ar >> m_AnalysisRecipe.rows.nCut;
		ar >> m_AnalysisRecipe.rows.nFuseTechnique;
		ar >> m_AnalysisRecipe.rows.nMinkConst;
		ar >> m_AnalysisRecipe.rows.nNoOfGroups;
		ar >> m_AnalysisRecipe.rows.nOrd3d;
		ar >> m_AnalysisRecipe.rows.nOrdination;
		ar >> m_AnalysisRecipe.rows.nOrdIterations;
		ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.rows.nOrdSeed;
		
		ar >> i;m_pGstaVisible = (i)?true:false;
		ar >> m_AssocRowVisible;
		ar >> m_AssocColVisible;
		ar >> m_AssocRowUpdate;
		ar >> m_AssocColUpdate;
		ar >> m_FuseRowVisible;
		ar >> m_FuseColVisible;
		ar >> m_FuseRowUpdate;
		ar >> m_FuseColUpdate;
		ar >> m_nScale;
		ar >> m_pExists;
		ar >> m_pIsRowLabel;
		ar >> m_colstand;
		ar >> m_presabs;
		ar >> m_globstand;
		ar >> m_red;
		ar >> m_green;
		ar >> m_blue;
		ar >> m_bw;
		ar >> i;m_AnosimPairView = (i)?true:false;
		ar >> i;m_AnosimColView = (i)?true:false; 
		ar >> i;m_AnosimRowView = (i)?true:false; 
		ar >> i;m_OrdVisible = (i)?true:false;
		ar >> i;m_OrdXVisible = (i)?true:false;
		ar >> i;m_OrdUpdate = (i)?true:false;
		ar >> m_boxwhiskervisible;
		ar >> mr_tableview;
		ar >> mr_rowtotals;
		ar >> mr_coltotals;
		ar >> mr_anosimrow;
		ar >> mr_anosimcol;
		ar >> mr_fuserow;
		ar >> mr_fusecol;
		ar >> mr_fuserowdend;
		ar >> mr_fusecoldend;
		ar >> mr_assocrowmx;
		ar >> mr_assoccolmx;
		ar >> mr_assocrowhist;
		ar >> mr_assoccolhist;
		ar >> mr_ord;
		ar >> mr_tway;
		ar >> mr_gsta;
		ar >> mr_boxwhisker;

		ar >> i;m_Rotate = (i)?true:false;
		ar >> i;m_Labels = (i)?true:false;
		ar >> i;m_Legend = (i)?true:false;
		ar >> i;m_Mst = (i)?true:false;
		ar >> i;m_Axies = (i)?true:false;
		ar >> i;m_Grouping = (i)?true:false;
		ar >> i;m_Usergroupcolours = (i)?true:false;
		ar >> i;m_SelOnly = (i)?true:false;
		ar >> i;m_Centroid = (i)?true:false;
		ar >> i;m_Showranges = (i)?true:false;
		ar >> i;m_Symbols = (i)?true:false;
		ar >> m_RotationSpeed;
		long size;
		ar >> size;m_TextBoxes.SetSize(size);
		for(i=0;i<m_TextBoxes.GetSize();i++){
			m_TextBoxes[i] = new TextBox;
			ar >> m_TextBoxes[i]->contents;
			ar >> m_TextBoxes[i]->size;
			ar >> m_TextBoxes[i]->location;
		}			
		ar >> size;m_TextBoxes2.SetSize(size);
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			m_TextBoxes2[i] = new TextBox2;
			ar >> m_TextBoxes2[i]->contents;
			ar >> m_TextBoxes2[i]->location;
			ar >> m_TextBoxes2[i]->colour;
			ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}			
		ar >> m_stressbox.contents;
		ar >> m_stressbox.location;
		ar >> m_stressbox.colour;
		ar.Read(&m_stressbox.font,sizeof(LOGFONT));
		
		ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
		ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
		ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

		ar >> wAngleY; 
		ar >> i;m_NoSymbols = (i)?true:false;
		ar >> size;m_PccVectors.SetSize(size);
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar >> m_PccVectors[i];
		}

		ar >> nCount;
		m_aGroupColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aGroupColours2[i].red;
			ar >> m_aGroupColours2[i].green;
			ar >> m_aGroupColours2[i].blue;			
		}
		ar >> nCount;
		m_aObjectColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aObjectColours2[i].red;
			ar >> m_aObjectColours2[i].green;
			ar >> m_aObjectColours2[i].blue;			
		}
		ar >> nCount;
		m_r.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_r[i].red;
			ar >> m_r[i].green;
			ar >> m_r[i].blue;			
		}
		ar >> nCount;
		m_Vectors2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_Vectors2[i].red;
			ar >> m_Vectors2[i].green;
			ar >> m_Vectors2[i].blue;			
		}
		m_3ddefault = true;

		//2.18 (some is above)
		POSITION pos = this->GetFirstViewPosition();
		CView * pView;
		while(pos != NULL){
			pView = this->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
				break;
			}
			pView = NULL;
		}

		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedRows->SetAt(j);
		}	
		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
		}	

		//2.19
		ar >> i;m_createKW = (i)?true:false;
		ar >> i;m_createANOSIMRows = (i)?true:false;
		ar >> i;m_createANOSIMCols = (i)?true:false;
		ar >> i;m_createMCAO = (i)?true:false;
		ar >> i;m_createPCC = (i)?true:false;

		//2.25
		ar >> m_visible_decimals;
		ar.Read(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

		//2.26
		//row and col control groups
		ar >> i;m_RowControlGroups.SetSize(i);
		for(i=0;i<m_RowControlGroups.GetSize();i++){
			m_RowControlGroups[i] = new Rows;
			long j; ar >> j;m_RowControlGroups[i]->SetSize(j);			
			for(j=0;j<m_RowControlGroups[i]->GetSize();j++){
				long k; ar >> k; m_RowControlGroups[i]->SetAt(j,k);
			}
		}
		ar >> i;m_ColControlGroups.SetSize(i);
		for(i=0;i<m_ColControlGroups.GetSize();i++){
			m_ColControlGroups[i] = new Rows;
			long j; ar >> j;m_ColControlGroups[i]->SetSize(j);			
			for(j=0;j<m_ColControlGroups[i]->GetSize();j++){
				long k; ar >> k; m_ColControlGroups[i]->SetAt(j,k);
			}
		}

		//2.27
		if(version >= 2.27){
			//apriori group patterns (rowcontrolgroup vs pattern)
			ar >> i; m_PatternApriori.SetSize(i);
			for(i=0;i<m_PatternApriori.GetSize();i++){
				long j;
				ar >> j; m_PatternApriori[i] = j;
			}
		}


		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)

		CGsta cgsta(this);

		SetupWindowsAfterLoad();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc diagnostics

#ifdef _DEBUG
void CPatnDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CPatnDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

void CPatnDoc::OnViewAssociationMatrix() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	pApp->OpenAssocView(this);
}

void CPatnDoc::OnViewDataTable()
{
	POSITION pos = GetFirstViewPosition();
	while(pos){
		CView * view = GetNextView(pos);
		if(view->IsKindOf(RUNTIME_CLASS(CTableView))){
			view->GetParentFrame()->BringWindowToTop();
			//view->GetParentFrame()->ShowWindow(SW_SHOWNORMAL );
		}
	}
}


void CPatnDoc::OnUpdateViewAssociationMatrix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsAssocMatrix() );
}

void CPatnDoc::OnViewFusionDetails() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	pApp->OpenFuseView(this);
}

void CPatnDoc::OnUpdateViewFusionDetails(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsFuseTable()  );
}

void CPatnDoc::OnViewHistogram() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	pApp->OpenHistogram(this);
}

void CPatnDoc::OnUpdateViewHistogram(CCmdUI* pCmdUI) 
{
	if(m_ColSelection == NULL || m_RowSelection == NULL){
		pCmdUI->Enable(false);
	}else{
		pCmdUI->Enable( !m_ColSelection->IsEmpty() || !m_RowSelection->IsEmpty() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPatnDoc commands

void CPatnDoc::OnToolsHistogram() 
{
	// TODO: Add your command handler code here
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	pApp->OpenHistogram(this);

}

void CPatnDoc::OnUpdateToolsHistogram(CCmdUI* pCmdUI) 
{
	if(m_ColSelection == NULL  || m_RowSelection == NULL){
		pCmdUI->Enable(false);
	}else{
		pCmdUI->Enable( !m_ColSelection->IsEmpty() || !m_RowSelection->IsEmpty() );
	}
	
}

void CPatnDoc::OnViewTwaytable() 
{
	//**PROTO1**
	//Intialization for CTway class. (see above for examples)
	CTwayOptions cTwayOptions;
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();

	if(m_pExists) 
	{
		//AfxGetMainWnd()->MessageBox("Only one Two way table can exist \n at a time.","Message");
		//goto it
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(view->IsKindOf(RUNTIME_CLASS(CTway))){
				view->GetParentFrame()->BringWindowToTop();
				//view->GetParentFrame()->ShowWindow(SW_RESTORE );
				break;
			}
		}
        return;		
	}

	if(cTwayOptions.DoModal() == IDOK)
	{
		m_colstand = cTwayOptions.m_colstand;
		m_presabs = cTwayOptions.m_presabs;
		m_nScale = cTwayOptions.m_Scale;
		m_globstand = cTwayOptions.m_globstand;
		m_red = cTwayOptions.m_red;
		m_green = cTwayOptions.m_green;
		m_blue = cTwayOptions.m_blue;
		m_bw = cTwayOptions.m_bw;
		pApp->OpenTway(this);
	}
}

void CPatnDoc::OnUpdateViewTwaytable(CCmdUI* pCmdUI) 
{
//	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	//View menu item activation criteria (see above functions).
	pCmdUI->Enable(!RowGroupsEmpty() &&
		!ColumnGroupsEmpty());
	
}

/*void CPatnDoc::OnViewGsta() 
{
	//Intialization for CGsta class. (see above for examples)
	CGstaOptions cGstaOptions;
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();

	if(m_pGstaVisible) 
	{
		//AfxGetMainWnd()->MessageBox("Only one GSTA table can exist \n at a time.","Message");
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(view->IsKindOf(RUNTIME_CLASS(CGstaView))){
				view->GetParentFrame()->BringWindowToTop();
				view->GetParentFrame()->ShowWindow(SW_RESTORE );
				break;
			}
		}
        return;		
	}

	if(cGstaOptions.DoModal() == IDOK)
	{
		m_presabs = cGstaOptions.m_presabs;
		pApp->OpenGstaView(this);
	}
}*/

/*void CPatnDoc::OnUpdateViewGsta(CCmdUI* pCmdUI) 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	//View menu item activation criteria (see above functions).
	pCmdUI->Enable(m_GstaAvailable);

}*/

void CPatnDoc::UpdateGroups()
{
	m_GstaAvailable = 0; //groups changed.
	if(m_kw_patn_generated_groups == 0 && !m_loading) m_KWExists = false;

	long i,n,o,p,numRows,group;
	Rows * row;

	//set anosim groups as none!
	//m_AnosimColGroups.RemoveAll();
	//m_AnosimRowGroups.RemoveAll();

	if((numRows = m_idxTopExtrinsicRow) == -1) numRows = GetNoOfRows();
	
	m_Groups.RemoveAll();
	for(i=0;i<m_Rows.GetSize();i++){
		delete m_Rows[i];
	}
	m_Rows.RemoveAll();

	//Define Row Group settings
	for(i=0;i<numRows;i++){
	group = GetRowGroups(i);
		//if it is a new group, add to a new group
		if((n = isExistingGroup(group,true)) != -1){
			m_Rows.GetAt(n)->Add(i);
		}else{//if it is an existing group, add to existing group
			row = new Rows;
			row->Add(i);
			o = m_Groups.GetSize();
			if(o == 0){
				m_Groups.Add(group);
				m_Rows.Add(row);
			}else{
				for(p=0;p<o;p++){
					if(group < m_Groups.GetAt(p)){
						m_Groups.InsertAt(p,group);
						m_Rows.InsertAt(p,row);
						p = m_Groups.GetSize();
					}
				}
				if(o == p){
					m_Groups.Add(group);
					m_Rows.Add(row);
				}
			}
		}
	}
	// set size of user groups
	colour temp;
	temp.red = temp.blue = temp.green = 0;
	// add new empty elements if necessary
	for(i=m_Groups.GetSize();i>m_UserGroupColours.GetSize();){
		m_UserGroupColours.Add(temp);
	}
	for(i=this->m_AprioriGroups.GetSize();i>m_aGroupColours2.GetSize();){
		m_aGroupColours2.Add(temp);
	}
	// remove excessive elements if necessary
	m_UserGroupColours.SetSize(m_Groups.GetSize());
	//m_aPrioriUserGroupColours.SetSize(m_AprioriGroups.GetSize());

	long numCols;
	Rows * col;

	if((numCols = m_idxLeftExtrinsicColumn) == -1) numCols = GetNoOfColumns();
	 
	m_ColGroups.RemoveAll();
	for(i=0;i<m_Cols.GetSize();i++){
		delete m_Cols[i];
	}
	m_Cols.RemoveAll();

	//Define Column Group settings
	for(i=0;i<numCols && i<m_aColumnGroups.GetSize();i++){
		group = GetColumnGroups(i);
		//if it is a new group, add to a new group
		if((n = isExistingGroup(group,false)) != -1){
			m_Cols.GetAt(n)->Add(i);
		}else{//if it is an existing group, add to existing group
			col = new Rows;
			col->Add(i);
			o = m_ColGroups.GetSize();
			if(o == 0){
				m_ColGroups.Add(group);
				m_Cols.Add(col);
			}else{
				for(p=0;p<o;p++){
					if(group < m_ColGroups.GetAt(p)){
						m_ColGroups.InsertAt(p,group);
						m_Cols.InsertAt(p,col);
						p = m_ColGroups.GetSize();
					}
				}
				if(o == p){
					m_ColGroups.Add(group);
					m_Cols.Add(col);
				}
			}
		}
	}

	m_PatternGroups.SetSize(m_Groups.GetSize());
	for(i=0;i<m_PatternGroups.GetSize();i++){
		m_PatternGroups[i] = i;
	}


	CColr ccolr(this);
	ccolr.OnCreate();

	CMst cmst(this);
	cmst.OnCreate();

	CGsta gsta(this);
}

long CPatnDoc::isExistingGroup(long group,long rows)
{
	long n,i;
	if(rows){
		n = -1;
		for(i=0;i<m_Groups.GetSize();i++){
			if(m_Groups.GetAt(i) == group)
				n = i;
		}
	}else{
		n = -1;
		for(i=0;i<m_ColGroups.GetSize();i++){
			if(m_ColGroups.GetAt(i) == group)
				n = i;
		}
	}
	return n;
}


/*
long CPatnDoc::AppendExcelFile(CString strFile)
{
	m_loading = true;

	long existingRows = GetNoOfRows();
	long existingCols = GetNoOfColumns();

	long err = false;

	//long nContinue;
	//CString strMessage;
	//strMessage  = "Do you want to append this file to the currently active file?\n\nThis excel file must contain row & column labels (can be empty).";
	//strMessage += "\n";
	//nContinue = AfxGetMainWnd()->MessageBox(strMessage,"Append External Data",
	//		MB_ICONQUESTION | MB_YESNO );
	//if ( nContinue != IDYES )
	//	return FALSE;
	CAppendOptiosn dlg;
	if(dlg.DoModal() != IDOK){
		return FALSE;
	}else{
		if(dlg.m_selb && dlg.m_match && !dlg.m_columnlabels)
			dlg.m_match = false;
		if(!dlg.m_selb && dlg.m_match && !dlg.m_rowlabels)
			dlg.m_match = false;
	}

	long fileLength;
	CFileException e;
	CFile excelFile;

	// open the file specified
	if (!excelFile.Open( strFile, CFile::modeRead | CFile::shareExclusive | CFile::typeBinary, &e ) ) 
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e.m_cause << "\n";
		#endif
		AfxGetMainWnd()->MessageBox("File could not be opened ","Read external data file",
				MB_ICONSTOP | MB_OK );
		return FALSE;
	}

	//continue excel file thing... .. .
	fileLength = excelFile.GetLength();
    unsigned char *pbuf;
	unsigned short value;
	pbuf = new unsigned char [fileLength];
	// read the file to the buffer
	excelFile.Read(pbuf,fileLength);

	long j;
	long rk;
	double number;    

	CArray<double> excelFileValues;
	long lastRow;
	long firstRow;
	long BOF = 0;
	long INDEX = 0;
	long nDBCELL = 0;
	long i,k;
	CArray<long> DBCELL;
	long idxStringList;

	//function contents to read excel files (well, 
	// the essential data anyway...)
	long idx;

	//open file

	//find start of file (0x0908), & store idx
	long idxStart = -1;
	long idxSheet = -1;
	CString sheetName;
	CArray<CString> stringList;

	//estimate BOF at 200h and verify
	i = 512;
	value = pbuf[i] + 256 * pbuf[i+1];
	long version;
	if(value == 0x0809){
		idxStart = i;
		version = pbuf[i+4] + 256*pbuf[i+5];
		if(version != 0x0600 && version != 0x0500){
			if(IDNO == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Unknown version.\n\nDo you wish to continue loading this file?",IDYES | IDNO)){
				delete pbuf;
				excelFile.Close();
				return false;
			}
		}
	}else{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Invalid Excel file.");
		delete pbuf;
		excelFile.Close();
		return false;
	}
	idxStringList = -1;
    long length,found = 0;
	for(;i<fileLength-4; ){
		value = pbuf[i] + 256 * pbuf[i+1];
		length = pbuf[i+2] + 256 * pbuf[i+3];
		if(value == 0x0085 && !found){
			//get sheet idx
			idxSheet = idxStart + pbuf[i+4] + 256 * (pbuf[i+5] + 256 * (
				pbuf[i+6] + 256 * pbuf[i+7]));

			//find spreadsheet name
			long nameLength;
			if(version == 0x0500){
				nameLength = pbuf[i+10];
				for(j=0;j<nameLength;j++){
					sheetName.Insert(j,pbuf[i+11+j]);				
				}				
			}else{				
				nameLength = pbuf[i+10] + 256 * pbuf[i+11];
				for(j=0;j<nameLength;j++){
					sheetName.Insert(j,pbuf[i+12+j]);				
				}
			}			
			found = 1;
		}else if(value == 0x00fc){//store 0xFC record idx (beginning of str list)
			idxStringList = i;
		}else if(value == 0x000a){
			//end of workbook global records
			i = fileLength;
		}
		i = i + 4 + length;
	}	

	m_TableName = sheetName;

	unsigned char *pbufold;
	pbufold = NULL;	
	pbufold = (unsigned char*) malloc(sizeof(unsigned char*) * fileLength/2);
	if(pbufold == NULL){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("out of memory");
		return false;
	}
	long row,col;
	CString string;
	long oldRow,oldCol;	
	//build stringList
	i = idxStringList;
	CArray<long> conrecpos;
	if(i != -1){
		long recordlength = pbuf[i+2] + 256*pbuf[i+3]-8;
		long numStrings = pbuf[i+8] + 256 * (pbuf[i+9] + 
			256 * (pbuf[i+10] + 256 * pbuf[i+11]));
		i += 12; //move to string records
		found = 0;
	
		long pos = 0;

		//create new buffer to remove CONTINUE records
		{
			while(!found){
				for(j=0;j<recordlength;j++){
					pbufold[pos++] = pbuf[i+j];
				}
				i += j;
				value = pbuf[i] + 256*pbuf[i+1];
				if(value == 0x3c){//continue record
					conrecpos.Add(pos); //the record at 'pos' may be part of an extended continue record
					recordlength = pbuf[i+2] + 256*pbuf[i+3];
					i +=4;//size of continue record???
				}else{
					found = true;
				}
			}		
		}	
		
		found = 0;
		j = 0;
		long ii = 0;
		stringList.SetSize(numStrings);
		while(found<numStrings && ii < pos-2){
			string.Empty();
			{
				//remove conrecpos entry when it occurs between
				// records.
				if(conrecpos.GetSize() && ii == conrecpos[0])
					conrecpos.RemoveAt(0);

				length = pbufold[ii] + 256 * pbufold[ii+1]; //number of characters ONLY
				unsigned flag = pbufold[ii+2];

				unsigned long highbyte = flag & 0x01;
				unsigned long extstr = flag & 0x04;
				unsigned long richstr = flag & 0x08;

				if(highbyte){
					//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("some characters may be missing in the output");
				}

				long crun = 0;
				long addlength = 0;
				long dist = 0;
				if(extstr && richstr){
					addlength = pbufold[ii+3] + 256*(pbufold[ii+4] + 
						256*(pbufold[ii+5] + 256*pbufold[ii+6]));
					dist = 7;
				}else if(extstr && !richstr){
					crun = pbufold[ii+3] + 256*pbufold[ii+4];
					addlength = pbufold[ii+5] + 256*(pbufold[ii+6] + 
						256*(pbufold[ii+7] + 256*pbufold[ii+8]));
					dist = 9;
				}else if(richstr){
					crun = pbufold[ii+3] + 256*pbufold[ii+4];
					dist = 5;
				}else{
					dist = 3;
				}

				if(highbyte){//read in as '_'
					if(conrecpos.GetSize() && ii+dist+length > conrecpos[0]){
						ii++; //increment for the extended continue record						
						conrecpos.RemoveAt(0);
					}
					for(k=0;k<length;k++){
						string.Insert(k,'_');
					}
				}else{//read as is
					for(k=0;k<length;k++){
						if(conrecpos.GetSize() && ii+dist+k == conrecpos[0]){
							ii++; //increment for the extended continue record
							conrecpos.RemoveAt(0);
						}
						string.Insert(k,pbufold[ii+dist+k]);
					}
				}
				stringList.InsertAt(found++,string);
				ii += dist + 8*crun + addlength + length;
			}
		}
	}
	
	long rk2;
	
	
	//goto spreadsheet list & get list of indexes 
	// for all cell lists (32 rows/per idx...)
	CArray<long> DBCellList;
	long position;
	length = 0;
	for(i=idxSheet;i<fileLength-20;i+=4+length){
		value = pbuf[i] + 256 * pbuf[i+1];
		length= pbuf[i+2] + 256 * pbuf[i+3];
		//find INDEX location
		if(value == 0x0809){ // get sheet version
			version = pbuf[i+4] + 256*pbuf[i+5];
			if(version != 0x0600 && version != 0x0500){
				if(IDNO == if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Unknown version.\n\nDo you wish to continue loading this file?",IDYES | IDNO)){
					delete pbuf;
					excelFile.Close();
					return false;
				}
			}
		}else if(value == 0x020b){ 
			if(version == 0x0500){
				//get row info
				firstRow = pbuf[i+8] + 256 * pbuf[i+9];
				lastRow = pbuf[i+10] + 256 * pbuf[i+11];
				position = 16;	
			}else{
				firstRow = pbuf[i+8] + 256 * (pbuf[i+9] + 
					256 * (pbuf[i+10] + 256 * pbuf[i+11]));
				lastRow = pbuf[i+12] + 256 * (pbuf[i+13] + 
					256 * (pbuf[i+14] + 256 * pbuf[i+15]));
				position = 20;				
			}

			//now build DBCellList
			long numRows = (lastRow-firstRow);
			long numDBCells = (long) (ceil(numRows/32.0));
			
			for(j=0;j<numDBCells && position + j * 4 < 4 + length;j++){
				long k = i + + position + j * 4;
				idx = pbuf[k] + 256 * (pbuf[k+1] + 256 * 
					(pbuf[k+2] + 256 * pbuf[k+3]));
				DBCellList.Add(idx + idxStart);
			}
			i = fileLength;
		}
	}
	row = col = 0;

	long rk3,rk4;
	
	//matching labels list!
	CArray<CString> matchlabels;
	long mlabels,*midx;

	//for each DBCellList entry, goto ROW record, 
	// jump to cell entries, & get data until 
	// DBCellList entry idx reached.
	for(i=0;i<DBCellList.GetSize();i++){
		//jump...
		idx = DBCellList[i];
		j = idx - (pbuf[idx+4] + 256 * (pbuf[idx+5] + 256 * 
			(pbuf[idx+6] + 256 * pbuf[idx+7]))) + pbuf[idx+8] +
			256 * pbuf[idx+9] + 20;

		//sort through cell records for current DBCELL
		while(j > 0 && j < idx){
			value = pbuf[j] + 256 * pbuf[j+1];
			length = pbuf[j+2] + 256*pbuf[j+3];
			oldRow = row;
			oldCol = col;
			row = pbuf[j+4] + 256 * pbuf[j+5];
			col = pbuf[j+6] + 256 * pbuf[j+7];

			if(!dlg.m_selb){
				col += existingCols;
			}else{
				row += existingRows;
			}
			if(!dlg.m_rowlabels){
				col++;
			}
			if(!dlg.m_columnlabels){
				row++;
			}

			//ready the building of matchlabels
			if(!dlg.m_selb && col == existingCols && row != 0 && dlg.m_rowlabels){
				mlabels = 1;
				midx = &row;
			}else if(dlg.m_selb && row == existingRows && col != 0 && dlg.m_columnlabels){
				mlabels = 2;
				midx = &col;
			}else{
				mlabels = 0;
			}

			long pos;

			//check for continue record...
			found = false;
			long maxlength = 4;
			long newvalue;
			while(!found){
				j += 4;
				for(k=0;k<length;k++){
					pbufold[maxlength++] = pbuf[j+k];
				}
				j += length;
				newvalue = pbuf[j] + 256*pbuf[j+1];
				if(newvalue == 0x3c){//continue record
					length = pbuf[j+2] + 256*pbuf[j+3];
					j += 4;//size of continue record???
				}else{
					found = true;
				}
			}	

			switch(value){
			case 0x0221: //ARRAY
				//not supported, skip
				//j += 18 + pbufold[j+16] + 256 * pbufold[j+17];				
				break;
			case 0x0201: //BLANK	
				//do nothing, skip				
				//j += 10;
				break;
			case 0x0205: //BOOLERR
				//treat as pres/abs (1 or 0).				
				assertCell(row,col);
				if(pbufold[11] == 0 && pbufold[10] == 1){
					//set value as 1
					//add value!!
					if(!dlg.m_selb && col != existingCols){
						addNumber(row,col,1);
					}else if(dlg.m_selb && row != existingRows){
						addNumber(row,col,1);
					}
					if(mlabels){
						if(matchlabels.GetSize() <= *midx)
							matchlabels.SetSize(*midx);
						matchlabels.SetAt(*midx-1,"1");
					}
				}else{
					//set value as 0
					//dd value!!
					if(!dlg.m_selb && col != existingCols) addNumber(row,col,0);
					else if(dlg.m_selb && row != existingRows) addNumber(row,col,0);
					if(mlabels){
						if(matchlabels.GetSize() <= *midx)
							matchlabels.SetSize(*midx);
						matchlabels.SetAt(*midx-1,"0");
					}
				}
				//j += 12;
				break;
			case 0x0406: //FORMULA
				//not supported, skip
				//j += 26 + pbufold[24] + 256 * pbufold[25];
				break;
			case 0x0204: //LABEL
				//get string pos
				assertCell(row,col);
				pos = pbufold[10] + 256 * pbufold[11];
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[10+k]);
				
				//add string!!
				if(!dlg.m_selb && col != existingCols) addString(row,col,string);
				else if(dlg.m_selb && row != existingRows) addString(row,col,string);
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					matchlabels.SetAt(*midx-1,string);
				}
				
				//j += 12 + pos;
				break;
			case 0x0203: //NUMBER -- not sure how to interpret IEEE numbers
				//number exists from [10] to [17]
				assertCell(row,col);
				
				//get number in 4 parts... it seems to work
				rk = pbufold[17] + 256 * pbufold[16];
				rk2 = pbufold[15] + 256 * pbufold[14];
				rk3 = pbufold[13] + 256 * pbufold[12];
				rk4 = pbufold[11] + 256 * pbufold[10];					

				number = NumFromIEEE(rk,rk2,rk3,rk4);

				//add number!!				
				if(!dlg.m_selb && col != existingCols) addNumber(row,col,number);
				else if(dlg.m_selb && row != existingRows) addNumber(row,col,number);
			
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					string.Format("%f",number);
					matchlabels.SetAt(*midx-1,string);
				}
			//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Numbers with too much detail \nwill appear as missing values");
				//j += 18;
				break;
			case 0x00be: //MULBLANK
				//do nothing, skip
				//j += 12;
				break;
			case 0x00bd: //MULRK
				k = 0;
				pos=8;
				//loop until exit condition satisfied.
				// MAY TERMINATE BEFORE COMPLETION, but should be OK
				while(pos < idx && pos+2 < maxlength){
					//(pbufold[pos]+256*pbufold[pos+1] != k+col-1-existingCols || 
					//!Record(pbufold[pos+2]+256*pbufold[pos+3]))){
					rk = pbufold[pos+2] + 256 * (pbufold[pos+3] + 256 * 
						(pbufold[pos+4] + 256 * pbufold[pos+5]));
					//JUST TO BE ANNOYING... (see CreateTableFromExcelFile())
					// all ending with 11010111 (0xD7)  ignored (????)
					if((rk & 0xff) != 0xd7){
						number = NumFromRk(rk);
						assertCell(row,col+k);
						if(!dlg.m_selb && col+k != existingCols) addNumber(row,col+k,number);
						else if(dlg.m_selb && row != existingRows) addNumber(row,col+k,number);
						if(mlabels == 1){
							if(matchlabels.GetSize() <= *midx)
								matchlabels.SetSize(*midx);
							string.Format("%f",number);
							matchlabels.SetAt(*midx-1,string);
						}else if(mlabels == 2){
							if(matchlabels.GetSize() <= *midx + k)
								matchlabels.SetSize(*midx+k-1);
							string.Format("%f",number);
							matchlabels.SetAt(*midx+k-1,string);
						}
					}
					pos+=6;
					k++;
				}
				//=2;			
				break;
			case 0x027e: //RK
				//number exists from [10] to [13]
				assertCell(row,col);
				rk = pbufold[10] + 256 * (pbufold[11] + 256 *
					(pbufold[12] + 256 * pbufold[13]));
				number = NumFromRk(rk);
				
				//add number!!
				if(!dlg.m_selb && col != existingCols) addNumber(row,col,number);
				else if(dlg.m_selb && row != existingRows) addNumber(row,col,number);
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					string.Format("%f",number);
					matchlabels.SetAt(*midx-1,string);
				}
				
			//	j += 14;
				break;
			case 0x00d6: //RSTRING
				//get string pos
				pos = pbufold[10] + 256 * pbufold[11];
				assertCell(row,col);
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[10+k]);
				
				//add string!!
				if(!dlg.m_selb && col != existingCols) addString(row,col,string);
				else if(dlg.m_selb && row != existingRows) addString(row,col,string);
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					matchlabels.SetAt(*midx-1,string);
				}
				//j += 12 + pos + k;
				break;
			case 0x00bc: //SHRFMLA
				//not supported, skip
				//j += 14 + pbufold[12] + 256 * pbufold[13];
				break;
			case 0x0207: //STRING -- called after formula?
				assertCell(oldRow,oldCol);
				//get string pos
				pos = pbufold[4] + 256 * pbufold[5];
				string.Empty();
				for(k=0;k<pos;k++) string.Insert(k,pbufold[6+k]);
				
				//add string!!
				if(!dlg.m_selb && col != existingCols) addString(oldRow,oldCol,string);
				else if(dlg.m_selb && row != existingRows) addString(oldRow,oldCol,string);
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					matchlabels.SetAt(*midx-1,string);
				}
				
				//j += 6 + pbufold[4] + 256 * pbufold[5];
				break;
			case 0x00fd: //STRING -- undocumented in BIFF7
				assertCell(row,col);
				//get string position in string list
				position = pbufold[10] + 256 * (pbufold[11] +
					256 * (pbufold[12] + 256 * pbufold[13]));

				if(position < 0 || position >= stringList.GetSize())
					string = "error";
				else
					string = stringList.GetAt(position);
				
				//add string!!
				if(!dlg.m_selb && col != existingCols) addString(row,col,string);
				else if(dlg.m_selb && row != existingRows) addString(row,col,string);
				if(mlabels){
					if(matchlabels.GetSize() <= *midx)
						matchlabels.SetSize(*midx);
					matchlabels.SetAt(*midx-1,string);
				}
			
			//	j += 14;
				break;
			default: //Code not recognized!
				//continue though buffer...
				if(!err) if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("cells containing formula, etc, may be present.\nFile may not successfully load!");
				err = true;
			//	+;
				break;
			}
		}
	}
	
	free(pbufold);
	delete pbuf;


	//check for sorting!
	CArray<long> matchorder;
	double mv;
	if(dlg.m_match && !dlg.m_selb){
		//find indexs for correct placement or matchlabels VS rowlabels
		for(i=0;i<existingRows;i++){
			found = 0;
			GetRowLabel(i,string);
			for(j=i;j<matchlabels.GetSize();j++){				
				if(string == matchlabels[j]){
					//matchorder.Add(j);
					//swap them
					for(k=existingCols;k<GetNoOfColumns();k++){
						mv = GetValueAt(k,i);
						SetValueAt(k,i,GetValueAt(k,j));
						SetValueAt(k,j,mv);
					}
					matchlabels[j] = matchlabels[i];
					matchlabels[i] = string;
					found = 1;
					j = matchlabels.GetSize();
				}
			}
			if(!found && i < matchlabels.GetSize()){//not found - add as new row to both table and matchlabels
				AddRow();
				j = GetNoOfRows()-1;
				//swap them
				for(k=existingCols;k<GetNoOfColumns();k++){
					mv = GetValueAt(k,j);
					SetValueAt(k,j,GetValueAt(k,i));
					SetValueAt(k,i,mv);
				}
				if(j >= matchlabels.GetSize()){
					matchlabels.SetSize(j+1);
				}
				matchlabels.SetAt(j,matchlabels[i]);				
			}				
		}
		//done matching against existing rows - 
		// remove empty rows >= existing rows -
		// else - set the row label
		for(i=GetNoOfRows()-1;i>=existingRows;i--){
			found = 0;
			for(j=existingCols;j<GetNoOfColumns();j++){
				if(GetValueAt(j,i) != GetMissingValue())
					found++; break;
			}
			if(found == 0){
				RemoveRowAt(i);
			}else{
				SetRowLabel(i,matchlabels[i]);
			}
		}		
	}else if(dlg.m_match && dlg.m_selb){
		//find indexs for correct placement or matchlabels VS collabels
		for(i=0;i<existingCols;i++){
			found = 0;
			GetColumnLabel(i,string);
			for(j=i;j<matchlabels.GetSize();j++){				
				if(string == matchlabels[j]){
					//matchorder.Add(j);
					//swap them
					for(k=existingRows;k<GetNoOfRows();k++){
						mv = GetValueAt(i,k);
						SetValueAt(i,k,GetValueAt(j,k));
						SetValueAt(j,k,mv);
					}
					matchlabels[j] = matchlabels[i];
					matchlabels[i] = string;
					found = 1;
					j = matchlabels.GetSize();
				}
			}
			if(!found && i<matchlabels.GetSize()){//not found - add as new col to both table and matchlabels
				AddColumn();
				j = GetNoOfColumns()-1;
				//swap them
				for(k=existingRows;k<GetNoOfRows();k++){
					mv = GetValueAt(j,k);
					SetValueAt(j,k,GetValueAt(i,k));
					SetValueAt(i,k,mv);
				}
				if(j >= matchlabels.GetSize()){
					matchlabels.SetSize(j+1);
				}
				matchlabels.SetAt(j,matchlabels[i]);				
			}				
		}
		//done matching against existing col - 
		// remove empty cols >= existing cols -
		// else - set the col label
		for(i=GetNoOfColumns()-1;i>=existingCols;i--){
			found = 0;
			for(j=existingRows;j<GetNoOfRows();j++){
				if(GetValueAt(i,j) != GetMissingValue())
					found++; break;
			}
			if(found == 0){
				RemoveColumnAt(i);
			}else{
				SetColumnLabel(i,matchlabels[i]);
			}
		}
	}

	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
			pView->RedrawWindow();
			pView->SetScrollRange(SB_HORZ,0,GetUpperBoundColumns(),TRUE);
			pView->SetScrollRange(SB_VERT,0,GetUpperBoundRows(),TRUE);
		}		
	}

	excelFile.Close();

	m_loading = false;

	UpdateAllTotals();
	UpdateIntrinsicTotals();
	UpdateGroups();
	CGsta cgsta(this);

	return TRUE;
}*/

long CPatnDoc::AppendExcelFile(CString strFile)
{
	m_loading = true;

	long existingRows = GetNoOfRows();
	long existingCols = GetNoOfColumns();

	long err = false;

	CAppendOptiosn dlg;
	if(dlg.DoModal() != IDOK){
		return FALSE;
	}else{
		if(dlg.m_selb && dlg.m_match && !dlg.m_columnlabels)
			dlg.m_match = false;
		if(!dlg.m_selb && dlg.m_match && !dlg.m_rowlabels)
			dlg.m_match = false;
	}

	//open file as normal
	CArray<CString> other_labels;

	if(dlg.m_selb){
		CreateTableFromExcelFileByExcel(strFile,!dlg.m_rowlabels,GetNoOfRows()+!dlg.m_columnlabels,NULL,&other_labels,dlg.m_rowlabels,dlg.m_columnlabels);		
	}else{
		CreateTableFromExcelFileByExcel(strFile,GetNoOfColumns()+!dlg.m_rowlabels,!dlg.m_columnlabels,&other_labels,NULL,dlg.m_rowlabels,dlg.m_columnlabels);		
	}

	m_loading = true;

	//attached with/without labels, now do matching (swapping rows/cols)
	if(dlg.m_match && !dlg.m_selb){//row matching
		if(dlg.m_columnlabels)
			other_labels.RemoveAt(0);

		//for each original row label, find idx of the row to match
		for(long row=0;row<existingRows && row<other_labels.GetSize();row++){
			CString rowlabel;
			GetRowLabel(row,rowlabel);
			long newrow;

			//match with new labels
			for(newrow=row;newrow<other_labels.GetSize();newrow++){
				if(other_labels[newrow] == rowlabel){
					break;
				}
			}

			//if ! found
			if(newrow == other_labels.GetSize()){
				//move newrow to end
				CString lbl1 = other_labels[row];
                other_labels.Add(lbl1);
				other_labels[row] = "";
				
		//		this->AddRow();

				for(long c=existingCols;c<GetNoOfColumns();c++){										
					assertCell(newrow+1,c);
					SetValueAt(c,newrow,this->GetValueAt(c,row));
					SetValueAt(c,row,GetMissingValue());
				}
			}else{//found - swap
				CString tmpstr = other_labels[row];
				other_labels[row] = other_labels[newrow];
				other_labels[newrow] = tmpstr;

				for(long c=existingCols;c<GetNoOfColumns();c++){					
					double value = GetValueAt(c,row);
					SetValueAt(c,row,GetValueAt(c,newrow));
					SetValueAt(c,newrow,value);
				}
			}
		}

		//remove last row?
		//this->RemoveRowAt(this->GetNoOfRows()-1);
		
	}
	//if new rows added, set labels
	for(long i = existingRows;i<GetNoOfRows() && i+!dlg.m_columnlabels<other_labels.GetSize() && !dlg.m_selb;i++){
		if(!dlg.m_rowlabels) {
			CString str2;
			str2.Format("%d",i+1);
			SetRowLabel(i,str2);
		} else {
			SetRowLabel(i,other_labels[i+!dlg.m_columnlabels]);
		}
	}

	if(dlg.m_match && dlg.m_selb){//col matching
		if(dlg.m_rowlabels)
			other_labels.RemoveAt(0);

		//for each original row label, find idx of the row to match
		for(long col=0;col<existingCols && col < other_labels.GetSize();col++){
			CString collabel;
			GetColumnLabel(col,collabel);
			long newcol;

			//match with new labels
			for(newcol=col;newcol<other_labels.GetSize();newcol++){
				if(other_labels[newcol] == collabel){
					break;
				}
			}

			//if ! found
			if(newcol == other_labels.GetSize()){
				//move newrow to end
				CString lbl = other_labels[col];
				other_labels.Add(lbl);
				other_labels[col] = "";
				
		//		this->AddRow();

				for(long r=existingRows;r<GetNoOfRows();r++){										
					assertCell(r,newcol+1);
					SetValueAt(newcol,r,this->GetValueAt(col,r));
					SetValueAt(col,r,GetMissingValue());
				}
			}else{//found - swap
				CString tmpstr = other_labels[col];
				other_labels[col] = other_labels[newcol];
				other_labels[newcol] = tmpstr;

				for(long r=existingRows;r<GetNoOfRows();r++){					
					double value = GetValueAt(col,r);
					SetValueAt(col,r,GetValueAt(newcol,r));
					SetValueAt(newcol,r,value);
				}
			}
		}
	
		//remove last column?
		//this->RemoveColumnAt(this->GetNoOfColumns()-1);
	}
	//if new rows added, set labels
	for(i = existingCols;i<GetNoOfColumns() && i+!dlg.m_rowlabels<other_labels.GetSize() && dlg.m_selb;i++){
		if(!dlg.m_columnlabels) {
			CString str2;
			str2.Format("%d",i+1);
			SetColumnLabel(i,str2);
		} else {
			SetColumnLabel(i,other_labels[i+!dlg.m_rowlabels]);
		}
	}

	//ready the building of matchlabels
	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
			pView->RedrawWindow();
			pView->SetScrollRange(SB_HORZ,0,GetUpperBoundColumns(),TRUE);
			pView->SetScrollRange(SB_VERT,0,GetUpperBoundRows(),TRUE);
		}		
	}

	m_loading = false;

	UpdateAllTotals();
	UpdateIntrinsicTotals();
	UpdateGroups();
	CGsta cgsta(this);

	return TRUE;
}


long CPatnDoc::ODBCConnect()
{
	//if (!COleServerDoc::OnNewDocument())
	//	return FALSE;

	CDatabase m_dbMine;
	CRecordset rs(&m_dbMine);
	CODBCConnection dbcon;
	CString strSQL;
	CString colField;
	CString rowField;
	CString dataField;
	CString tableName;
	CString dsn;
	// prompt user to enter database details
	CString strMessage;
	//long nContinue;
	strMessage  = "ODBC Connection\r\n\r\nDo you want to search for the DSN?\n";
	strMessage += "\n";
	if ( !((CPatnApp*)AfxGetApp())->silent_mode && AfxMessageBox(strMessage,MB_ICONQUESTION | MB_YESNO,131254)== IDYES ){
		//blankout DSN field in the dialogbox
		dbcon.m_dsn = "search";
		if((m_dbMine.OpenEx(NULL, CDatabase::openReadOnly))==0)
			return FALSE;
	}
	// get table name, fieldnames of columns, rows and data
	long notvalidData = TRUE;
	if(dbcon.DoModal() == IDOK){ //||(dbcon.DoModal() != IDCANCEL)){
		dsn = "DSN=";
		dsn += dbcon.m_dsn;
		tableName = dbcon.m_table_name;
		colField = dbcon.m_col_label_field;
		rowField = dbcon.m_row_label_field;
		dataField = dbcon.m_data_field;
	}else{
		return FALSE;
	}
	//if(nContinue == IDNO){
//		m_dbMine.OpenEx(dsn, CDatabase::openReadOnly);
	//}
	// get the number of rows and columns
	CString strSQLgetNrCols;
	strSQLgetNrCols = "SELECT DISTINCT ";
	strSQLgetNrCols += colField;
	strSQLgetNrCols +=  " ";
	strSQLgetNrCols += "FROM ";
	strSQLgetNrCols += tableName;
	CString strSQLgetNrRows;
	strSQLgetNrRows = "SELECT DISTINCT ";
	strSQLgetNrRows += rowField;
	strSQLgetNrRows +=  " ";
	strSQLgetNrRows += "FROM ";
	strSQLgetNrRows += tableName;

	long nRowCount;
	long nColumnCount;
	rs.Open(CRecordset::forwardOnly,strSQLgetNrCols,CRecordset::none);
	while(!rs.IsEOF()){
		rs.MoveNext();
	}
	nColumnCount = rs.GetRecordCount();
	rs.Close();
	rs.Open(CRecordset::forwardOnly,strSQLgetNrRows,CRecordset::none);
	while(!rs.IsEOF()){
		rs.MoveNext();
	}
	nRowCount = rs.GetRecordCount();
	rs.Close();
	
	long idxCol;
	long idxRow;
	m_nMissingValue = m_nMissingValue;
	long		nNoOfRows = nRowCount;
	long		nNoOfColumns = nColumnCount;
	long		nWhichColumnWidth = CPageNewTableOptions::standard;
	double	nColumnWidth = g_nStandardColumnWidth;
	long		nDecimalPrecision = g_nDefaultDecimalPrecision;

	SetSizes(nNoOfColumns, nNoOfRows); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(nNoOfColumns, nColumnWidth, nDecimalPrecision);
	SetInitialRowAttributes(nNoOfRows, 1 );

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(nDecimalPrecision);

	vector* nVector;
	long i;
	// initialize column totals
	for ( idxCol = 0; idxCol < nNoOfColumns; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < nNoOfRows; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}

	// fill in the labels
	CString strSQLgetLabels;
	strSQLgetLabels =  "SELECT ";
	strSQLgetLabels +=  colField;
	strSQLgetLabels +=  ",";
	strSQLgetLabels +=  rowField;
	strSQLgetLabels +=  " ";
	strSQLgetLabels +=  "FROM ";
	strSQLgetLabels +=  tableName;
	strSQLgetLabels +=  " ";
	strSQLgetLabels +=  "ORDER BY ";
	strSQLgetLabels +=  colField;
	strSQLgetLabels +=  ",";
	strSQLgetLabels +=  rowField;

	CString colLabel = " ";
	CString rowLabel = " ";
	CString tempColLabel;
	
	int nfield;
	idxCol=0;
	idxRow=0;
	nfield=0;

	rs.Open(CRecordset::forwardOnly,strSQLgetLabels,CRecordset::none);
	while( !rs.IsEOF( ) ){
		tempColLabel = colLabel;
		rs.GetFieldValue(nfield,colLabel);
		if(colLabel != tempColLabel){
			SetColumnLabel(idxCol,colLabel);
			idxCol++;
		}
		if(idxRow<nNoOfRows){
			rs.GetFieldValue(nfield+1,rowLabel);
			SetRowLabel(idxRow,rowLabel);
			idxRow++;
		}
		rs.MoveNext( );
	}
	rs.Close( );

	// fill in the data
	strSQL =  "SELECT ";
	strSQL +=  colField;
	strSQL +=  ",";
	strSQL +=  rowField;
	strSQL +=  ",";
	strSQL +=  dataField;
	strSQL +=  " ";
	strSQL +=  "FROM ";
	strSQL +=  tableName;
	strSQL +=  " ";
	strSQL +=  "ORDER BY ";
	strSQL +=  colField;
	strSQL +=  ",";
	strSQL +=  rowField;

	rs.Open(CRecordset::forwardOnly,strSQL,CRecordset::none);
	CString nValue;
	colLabel = " ";
	nfield=0;
	idxCol=0;
	idxRow=0;
	while( !rs.IsEOF( ) ){
		tempColLabel = colLabel;
		rs.GetFieldValue(nfield,colLabel);
		if(colLabel != tempColLabel){ // new column
			idxCol++;
			idxRow=0;
		}
		rs.GetFieldValue(nfield+2,nValue);
		assertCell(idxRow+1,idxCol-1+1);
		SetValueAt(idxCol-1,idxRow,nValue);
		idxRow++;
		rs.MoveNext( );
	}
	rs.Close( );

	UpdateIntrinsicTotals();
	UpdateAllTotals();
	UpdateGroups();
	CGsta cgsta(this);

	return TRUE;
}

void CPatnDoc::OnFileOdbcconnection() 
{
	//ODBCConnect();
	m_loading = true;
	((CPatnApp*)AfxGetApp())->OpenODBC();
	m_loading = false;
}



void CPatnDoc::OnFileExportColumnaffinityvector() 
{
	if(!(m_ColGroups.GetSize() > 1))
		return;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	long n;

	strTitle = "Save Column Group Composition:";

	// Get path for the new file
	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Analysis Column Group Composition.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	strBuffer = "";

	f.WriteString(getExportGroupCompositionHeader(false));
	
	for(n=0;n<m_aColumnGroups.GetSize();n++){
		CString str;
		GetColumnLabel(n,str);
		if(m_aColumnGroups[n] == -1)
			strLabel.Format("%s,0\n",str);
		else
			strLabel.Format("%s,%d\n",str,m_aColumnGroups[n]);
		f.WriteString(strLabel);	
	}
		
	

	f.Close();
		
}

void CPatnDoc::OnUpdateFileExportColumnaffinityvector(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ColGroups.GetSize() > 1);	
}

void CPatnDoc::OnFileExportColumnanismresults() 
{
	if(!m_ANOSIMColsExists)
		return;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	CAnosim obj(this);
	//determine if row anosim pairwise between groups occured
	if(m_ColumnAsim_pairs && m_AnosimColGroups.GetSize() > 1){
		//group pair results
		obj.Save(true);
		return;
	}else{	
		//overall results
		
	}

	strTitle = "Save Column ANOSIM Results:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Column Anosim.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	strBuffer.Format("Project Comment: %s\n",m_TableName);strBuffer.Replace(","," ");
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",m_ColumnAsim_asso);
	f.WriteString(strBuffer);
	strBuffer.Format("Classification Option: %s\n",m_FuseTypeCols);
	f.WriteString(strBuffer);
	strBuffer.Format("Created: %s\nParameter,Value\n",m_ColumnAsim_time);
	f.WriteString(strBuffer);
	strBuffer.Format("Real f-ratio,%.6f \nBest f-ratio,%.6f \n%% Best > Real,%.6f",
		m_ColumnAsim_save,m_ColumnAsim_rbest,
		m_ColumnAsim_stat);
	f.WriteString(strBuffer);

	

	f.Close();	
}

void CPatnDoc::OnUpdateFileExportColumnanismresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ANOSIMColsExists);
}

void CPatnDoc::OnFileExportColumnassociationmatrix() 
{
	if(!(m_AssocMatrixCols.m_aMatrix.GetSize() > 0 && !m_AssocColChange)) return;
	CAssociation casso(this);
	casso.WriteAssocMatrixToASCII(columns);
}

void CPatnDoc::OnUpdateFileExportColumnassociationmatrix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_AssocMatrixCols.m_aMatrix.GetSize() > 0 && !m_AssocColChange);	
}

void CPatnDoc::OnFileExportColumnfusiontable() 
{
	if(!(m_FuseTableCols.GetSize() > 0 && !m_FuseColChange)) return;
	CFuse fuse(this);
	if(m_FuseTableCols.GetSize() > 0)
	{
		fuse.WriteFuseTableToASCII(this,columns);	
	}
}

void CPatnDoc::OnUpdateFileExportColumnfusiontable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_FuseTableCols.GetSize() > 0 && !m_FuseColChange);	
}

void CPatnDoc::OnFileExportOrdinationcoordinates() 
{
	if(!(m_aRowXCoords[0] != ANALYSIS_VALUE)) return;
	COrdination ord(this);

	ord.ExportRowOrdination();
}

void CPatnDoc::OnUpdateFileExportOrdinationcoordinates(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_aRowXCoords[0] != ANALYSIS_VALUE);		
}

void CPatnDoc::OnUpdateFileExportPccresults(CCmdUI* pCmdUI) 
{
	bool enable = false;	
	for(long i=0;i<GetNoOfColumns();i++){
		if(m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
			enable = true;
			return;
		}
	}
	pCmdUI->Enable(enable);
}

void CPatnDoc::OnFileExportPccresults() 
{
	for(long i=0;i<GetNoOfColumns();i++){
		if(m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
			CPcc pcc(this);
			pcc.ExportPcc();
			return;
		}
	}
	
}

void CPatnDoc::OnFileExportRowaffinityvector() 
{
	if(!(m_Groups.GetSize() > 1))
		return;
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	long n;

	strTitle = "Save Row Group Composition:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Analysis Row Group Composition.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}
	
	f.WriteString(getExportGroupCompositionHeader(true));

	for(n=0;n<m_aRowGroups.GetSize();n++){
		CString str;
		GetRowLabel(n,str);
		if(m_aRowGroups[n] == -1)
			strLabel.Format("%s,0,0\n",str);
		else
			strLabel.Format("%s,%d,%g\n",str,m_aRowGroups[n],m_aRowGroupsDist[n]);
		f.WriteString(strLabel);
	}

	

	f.Close();	
}

void CPatnDoc::OnUpdateFileExportRowaffinityvector(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Groups.GetSize() > 1);	
}

void CPatnDoc::OnFileExportRowanismresults() 
{
	if(!m_ANOSIMRowsExists) return;
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	CAnosim obj(this);

	//determine if row anosim pairwise between groups occured
	if(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1){
		//group pair results
		obj.Save(false);
		return;
	}else{	
		//overall results
		
	}

	strTitle = "Save Row ANOSIM results:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Row ANOSIM.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	strBuffer.Format("Project Comment: %s\n",m_TableName);strBuffer.Replace(","," ");
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",m_RowAsim_asso);
	f.WriteString(strBuffer);
	strBuffer.Format("Classification Option: %s\n",m_FuseTypeRows);
	f.WriteString(strBuffer);
	strBuffer.Format("Creation Date/Time: %s\n",m_RowAsim_time);
	f.WriteString(strBuffer);

	f.WriteString("Parameter,Value\n");

	strBuffer.Format("Real f-ratio,%.6f \nBest f-ratio,%.6f \n%% Best > Real,%.6f",
		m_RowAsim_save,m_RowAsim_rbest,m_RowAsim_stat);
	f.WriteString(strBuffer);

	

	f.Close();	
}

void CPatnDoc::OnUpdateFileExportRowanismresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ANOSIMRowsExists);
	
}

void CPatnDoc::OnFileExportRowassocationmatrix() 
{
	CAssociation casso(this);
	casso.WriteAssocMatrixToASCII(rows);
}

void CPatnDoc::OnUpdateFileExportRowassocationmatrix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_AssocMatrixRows.m_aMatrix.GetSize() > 0 &&
		!m_AssocRowChange);	
}

void CPatnDoc::OnFileExportRowfusiontable() 
{
	CFuse fuse(this);
	if(m_FuseTableRows.GetSize() > 0)
		fuse.WriteFuseTableToASCII(this,rows);
}

void CPatnDoc::OnUpdateFileExportRowfusiontable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_FuseTableRows.GetSize() > 0 &&
		!m_FuseRowChange);	
}

void CPatnDoc::OnFileLoadColumnaffinityvector() 
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"Column Group Composition\t*.csv|*.csv|All Files\t*.*|*.*||",NULL);
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long count;
	int left, right;

	if ( dlg.DoModal() != IDOK )
		return;

	CWaitCursor waitcursor;

	strFile = dlg.GetPathName();

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_YESNO );
		return;
	}

	// fill table with data
	count = 0;
	bool found = false;
	while(io.ReadString(strRow) &&
		count < m_aColumnGroups.GetSize()){

		//check for patn saved file
		if(strRow.Find("Project Comment",0) == 0){
			io.ReadString(strRow); //assoc method
			io.ReadString(strRow); //class method
			io.ReadString(strRow); //time
			io.ReadString(strRow); //column headers
			found = true;
			continue;
		}

		right = left = 0;
		long rowcount = 0;
		while(left < strRow.GetLength() && 
			count < m_aColumnGroups.GetSize()){
			// get each element
	 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' || 
				strRow[left] == '\t')) 
				left++;
			right = left;
			while ( right < strRow.GetLength() && strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t')
				right++;

			if(right <= strRow.GetLength()){
				strLabel = strRow.Mid(left, right - left);
				if(found && rowcount == 0)
				{
					rowcount  ++;
				}
				else
				{
					m_aColumnGroups[count] = atoi(strLabel);
					if(m_aColumnGroups[count] == 0)
						m_aColumnGroups[count] = -1;
					count++;
					rowcount = 0;
				}
			}

			left = right+1;
			
			
		}
	}

	m_FuseAssoCols = "None";
	m_FuseTypeCols.Format("Imported file %s",strFile);
	m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
		
	io.Close();

	m_log += "\r\nColumn group composition import from file: \r\n\t" + strFile + "\r\n\r\n";

	UpdateGroups();

	CGsta gsta(this);

	this->UpdateAllViews(NULL);

	return ;	

}

void CPatnDoc::OnUpdateFileLoadColumnaffinityvector(CCmdUI* pCmdUI) 
{
}

void CPatnDoc::OnFileLoadRowaffinityvector() 
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long count;
	int left, right;

	if ( dlg.DoModal() != IDOK )
		return ;

	CWaitCursor waitcursor;

	strFile = dlg.GetPathName();

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_YESNO );
		return ;
	}

	// fill table with data
	count = 0;
	left = 0;
	count = (long)io.GetLength();
	count = 0;
	bool found = false;
	while(io.ReadString(strRow) &&
		count < m_aRowGroups.GetSize()){

		//check for patn saved file
		if(strRow.Find("Project Comment",0) == 0){
			io.ReadString(strRow); //assoc method
			io.ReadString(strRow); //class method
			io.ReadString(strRow); //time
			io.ReadString(strRow); //column headers
			found = true;
			continue;
		}

		right = left = 0;
		long rowcount = 0;
		while(left <= strRow.GetLength() && 
			count < m_aRowGroups.GetSize()){
			// get each element			
	 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' || 
				strRow[left] == '\t')) 
				left++;
			right = left;
			while ( right < strRow.GetLength() && strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t')
				right++;

			if(right <= strRow.GetLength()){
				strLabel = strRow.Mid(left, right - left);
				if(found && rowcount == 0)
				{
					rowcount++;
				}
				else
				{
					m_aRowGroups[count] = atoi(strLabel);
					if(m_aRowGroups[count] == 0)
						m_aRowGroups[count] = -1;
					count++;
					rowcount = 0;
				}
			}

			left = right+1;
		}
	}

	m_FuseAssoRows = "None";
	m_FuseTypeRows.Format("Imported file %s",strFile);
	m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");

	io.Close();

	m_log += "\r\nRow a priori group composition import from file: \r\n\t" + strFile + "\r\n\r\n";

	UpdateGroups();

	CColr ccolr(this);
	ccolr.OnCreate();

	CMst cmst(this);
	cmst.OnCreate();

	CGsta gsta(this);

	this->UpdateAllViews(NULL);

	return;	
}

void CPatnDoc::OnUpdateFileLoadRowaffinityvector(CCmdUI* pCmdUI) 
{
	
}
/*
void CPatnDoc::OnViewColumnanism() 
{
	CAsimDlg asimdlg;
	asimdlg.Setup(this,false);
	asimdlg.DoModal();	
}

void CPatnDoc::OnUpdateViewColumnanism(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!ColumnGroupsEmpty() && 
		m_AssocMatrixCols.m_aMatrix.GetSize() > 0 && !m_AssocColChange);
}

void CPatnDoc::OnViewRowanism() 
{
	CAsimDlg asimdlg;
	asimdlg.Setup(this,true);
	asimdlg.DoModal();	
}

void CPatnDoc::OnUpdateViewRowanism(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!RowGroupsEmpty() && 
		m_AssocMatrixRows.m_aMatrix.GetSize() > 0 && !m_AssocRowChange);	
}
*/
void CPatnDoc::UpdateIntrinsicTotals()
{
	if(m_loading)
		return;
	long left,top,i,j;
	double value;

	if(m_idxLeftExtrinsicColumn == -1 || m_idxLeftExtrinsicColumn >= GetNoOfColumns()) left = GetNoOfColumns();
	else left = m_idxLeftExtrinsicColumn;
	if(m_idxTopExtrinsicRow == -1 || m_idxTopExtrinsicRow >= GetNoOfRows()) top = GetNoOfRows();
	else top = m_idxTopExtrinsicRow;

	m_aIntrinsicColumnSums.SetSize(GetNoOfColumns());
	m_aIntrinsicColumnCountPositives.SetSize(GetNoOfColumns());
	m_aIntrinsicColumnMinimum.SetSize(GetNoOfColumns());
	m_aIntrinsicColumnMean.SetSize(GetNoOfColumns());
	m_aIntrinsicColumnMaximum.SetSize(GetNoOfColumns());
	m_aIntrinsicColumnCountMissingValues.SetSize(GetNoOfColumns());

	m_aIntrinsicRowSums.SetSize(GetNoOfRows());
	m_aIntrinsicRowCountPositives.SetSize(GetNoOfRows());
	m_aIntrinsicRowMinimum.SetSize(GetNoOfRows());
	m_aIntrinsicRowMean.SetSize(GetNoOfRows());
	m_aIntrinsicRowMaximum.SetSize(GetNoOfRows());
	m_aIntrinsicRowCountMissingValues.SetSize(GetNoOfRows());

	for(j=0;j<left;j++){
		m_aIntrinsicColumnSums[j] = 0.0;
		m_aIntrinsicColumnCountPositives[j] = 0;
		m_aIntrinsicColumnCountMissingValues[j] = 0;
		m_aIntrinsicColumnMinimum[j] = m_nMissingValue;
		m_aIntrinsicColumnMaximum[j] = m_nMissingValue;
	}

	for(i=0;i<top;i++){
		m_aIntrinsicRowSums[i] = 0.0;
		m_aIntrinsicRowCountPositives[i] = 0;
		m_aIntrinsicRowCountMissingValues[i] = 0;
		m_aIntrinsicRowMaximum[i] = m_nMissingValue;
		m_aIntrinsicRowMinimum[i] = m_nMissingValue;
		for(j=0;j<left;j++){
			if((value = GetValueAt(j,i)) == GetMissingValue()){
				m_aIntrinsicColumnCountMissingValues[j]++;
				m_aIntrinsicRowCountMissingValues[i]++;
			}else{
				m_aIntrinsicColumnSums[j] += value;
				m_aIntrinsicRowSums[i] += value;
				if(value > 0){
					m_aIntrinsicColumnCountPositives[j]++;
					m_aIntrinsicRowCountPositives[i]++;
				}
				if(value > m_aIntrinsicRowMaximum[i] || m_aIntrinsicRowMaximum[i] == m_nMissingValue)
					m_aIntrinsicRowMaximum[i] = value;
				if(value < m_aIntrinsicRowMinimum[i] || m_aIntrinsicRowMinimum[i] == m_nMissingValue)
					m_aIntrinsicRowMinimum[i] = value;
				if(value > m_aIntrinsicColumnMaximum[j] || m_aIntrinsicColumnMaximum[j] == m_nMissingValue)
					m_aIntrinsicColumnMaximum[j] = value;
				if(value < m_aIntrinsicColumnMinimum[j] || m_aIntrinsicColumnMinimum[j] == m_nMissingValue)
					m_aIntrinsicColumnMinimum[j] = value;
			}
		}
		if( m_aIntrinsicRowSums[i] != 0)
			m_aIntrinsicRowMean[i] = m_aIntrinsicRowSums[i] / (double)(left-m_aIntrinsicRowCountMissingValues[i]);
		else
			m_aIntrinsicRowMean[i] = GetMissingValue();
	}
	for(;i<GetNoOfRows();i++){
		m_aIntrinsicRowSums[i] = m_nMissingValue;
		m_aIntrinsicRowCountPositives[i] = 0;
		m_aIntrinsicRowCountMissingValues[i] = 0;
		m_aIntrinsicRowMaximum[i] = m_nMissingValue;
		m_aIntrinsicRowMinimum[i] = m_nMissingValue;		
		m_aIntrinsicRowMean[i] = m_nMissingValue;
	}

	for(j=0;j<left;j++) 
	{
		if(m_aIntrinsicColumnSums[j] != 0)
			m_aIntrinsicColumnMean[j] = m_aIntrinsicColumnSums[j] / (double)(top-m_aIntrinsicColumnCountMissingValues[j]);
		else
			m_aIntrinsicColumnMean[j] = GetMissingValue();

	}
	for(j=left;j<GetNoOfColumns();j++){
		m_aIntrinsicColumnSums[j] = m_nMissingValue;
		m_aIntrinsicColumnCountPositives[j] =0;
		m_aIntrinsicColumnCountMissingValues[j] = 0;
		m_aIntrinsicColumnMaximum[j] = m_nMissingValue;
		m_aIntrinsicColumnMinimum[j] = m_nMissingValue;		
		m_aIntrinsicColumnMean[j] = m_nMissingValue;
	}

}

void CPatnDoc::UpdateAllTotals()
{
	if(m_loading)
		return;
	long left,top,i,j;
	double value;

	left = GetNoOfColumns();
	top = GetNoOfRows();

	m_aColumnSums.SetSize(left);
	m_aColumnCountPositives.SetSize(left);
	m_aColumnMinimum.SetSize(left);
	m_aColumnMean.SetSize(left);
	m_aColumnMaximum.SetSize(left);
	m_aColumnCountMissingValues.SetSize(left);

	m_aRowSums.SetSize(top);
	m_aRowCountPositives.SetSize(top);
	m_aRowMinimum.SetSize(top);
	m_aRowMean.SetSize(top);
	m_aRowMaximum.SetSize(top);
	m_aRowCountMissingValues.SetSize(top);

	for(j=0;j<left;j++){
		m_aColumnSums[j] = 0.0;
		m_aColumnCountPositives[j] = 0;
		m_aColumnCountMissingValues[j] = 0;
		m_aColumnMinimum[j] = m_nMissingValue;
		m_aColumnMaximum[j] = m_nMissingValue;
	}

	for(i=0;i<top;i++){
		m_aRowSums[i] = 0.0;
		m_aRowCountPositives[i] = 0;
		m_aRowCountMissingValues[i] = 0;
		m_aRowMaximum[i] = m_nMissingValue;
		m_aRowMinimum[i] = m_nMissingValue;
		for(j=0;j<left;j++){
			if((value = GetValueAt(j,i)) == GetMissingValue()){
				m_aColumnCountMissingValues[j]++;
				m_aRowCountMissingValues[i]++;
			}else{
				m_aColumnSums[j] += value;
				m_aRowSums[i] += value;
				if(value > 0){
					m_aColumnCountPositives[j]++;
					m_aRowCountPositives[i]++;
				}
				if(value > m_aRowMaximum[i] || m_aRowMaximum[i] == m_nMissingValue)
					m_aRowMaximum[i] = value;
				if(value < m_aRowMinimum[i] || m_aRowMinimum[i] == m_nMissingValue)
					m_aRowMinimum[i] = value;
				if(value > m_aColumnMaximum[j] || m_aColumnMaximum[j] == m_nMissingValue)
					m_aColumnMaximum[j] = value;
				if(value < m_aColumnMinimum[j] || m_aColumnMinimum[j] == m_nMissingValue)
					m_aColumnMinimum[j] = value;
			}			
		}
		if(m_aRowSums[i] != 0)
			m_aRowMean[i] = m_aRowSums[i] / (double)(left-m_aRowCountMissingValues[i]);
		else
			m_aRowMean[i] = GetMissingValue();

	}
	for(j=0;j<left;j++) 
	{
		if(m_aColumnSums[j] != 0)
			m_aColumnMean[j] = m_aColumnSums[j] / (double)(top-m_aColumnCountMissingValues[j]);
		else
			m_aColumnMean[j] = GetMissingValue();
	}

}

void CPatnDoc::OnFileTableAndInfo() 
{
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Export Table:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Comma Seperated File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Data Table.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}	

	//header
	CString st;
	st.Format("Project Comment: %s\n",this->m_TableName);
	f.WriteString(st);
	CTime time = CTime::GetCurrentTime();	
	st.Format("Creation Date/Time: %s\n",time.Format("%H:%M:%S, %B %d, %Y"));
	f.WriteString(st);

	//write column labels
	long i;
	f.WriteString(",");
	for(i=0;i<GetNoOfColumns();i++){
		if(i == m_idxLeftExtrinsicColumn)
			f.WriteString(",");
		GetColumnLabel(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
	}
		
	//totals sum,totals average,totals #>0,totals #missing
	f.WriteString(",totals sum,totals minimum,totals mean,totals maximum,totals #>0,totals #missing");

	//intrinsics sum,intrinsics average,intrinsics #>0,intrinsics #missing
	f.WriteString(",intrinsics sum,intrinsics minimum,intrinsics mean,intrinsics maximum,intrinsics #>0,intrinsics #missing");
	
	//Groups
	f.WriteString(",groups, aloc distances");

	//Ordination
	if(m_aRowXCoords[0] != GetMissingValue())
	{
		f.WriteString(",X coord");
		if(m_Ord3d >= 2) f.WriteString(",Y coord");
		if(m_Ord3d == 3) f.WriteString(",Z coord");
	}

	//dendogram order
	if(m_FuseTableRows.GetSize() > 0 && !m_FuseRowChange)
	{
		f.WriteString(",dendrogram order");
	}

	f.WriteString("\n");

	long j;
	//data loop
	for(i=0;i<GetNoOfRows();i++){
		if(i == m_idxTopExtrinsicRow)
			f.WriteString("\n");
		
		//write label
		GetRowLabel(i,strLabel);
		f.WriteString(strLabel);
		
		for(j=0;j<GetNoOfColumns();j++){
			if(j == m_idxLeftExtrinsicColumn)
				f.WriteString(",");	
			strLabel.Format(",%.8g",GetValueAt(j,i));
			f.WriteString(strLabel);
		
		}

		//write totals
		f.WriteString(",");
		GetRowSum(i,strLabel);
		strBuffer = ",";
		strBuffer += strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
		GetRowMinimum(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
		GetRowMean(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
		GetRowMaximum(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
		GetRowCountPositives(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);
		GetRowCountMissingValues(i,strLabel);
		strBuffer = strLabel;
		strBuffer += ",";
		f.WriteString(strBuffer);

		//write intrinsics
		if(m_idxTopExtrinsicRow == -1 || i < m_idxTopExtrinsicRow){
			GetIntrinsicRowSum(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
			GetIntrinsicRowMinimum(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
			GetIntrinsicRowMean(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
			GetIntrinsicRowMaximum(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
			GetIntrinsicRowCountPositives(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
			GetIntrinsicRowCountMissingValues(i,strLabel);
			strBuffer = strLabel;
			strBuffer += ",";
			f.WriteString(strBuffer);
		}else{
			f.WriteString(",,,,");
		}

		//write group
		if(GetRowGroups(i) == -1)
		{
			
			if(i<m_aRowGroupsDist.GetSize())
			{
				strLabel.Format("0,%f,",m_aRowGroupsDist[i]);
			}
			else
			{
				strLabel = "0,0,";
			}
		}
		else
		{
			
			if(i<m_aRowGroupsDist.GetSize())
			{
				strLabel.Format("%d,%f,",GetRowGroups(i),m_aRowGroupsDist[i]);
			}
			else
			{
				strLabel.Format("%d,0",GetRowGroups(i));
			}
		}
		f.WriteString(strLabel);

		//write ordination
		if(m_aRowXCoords[0] != GetMissingValue())
		{
			strLabel.Format("%.8g",m_aRowXCoords[i]);
			f.WriteString(strLabel);
			if(m_Ord3d >= 2){
				strLabel.Format(",%.8g",m_aRowYCoords[i]);
				f.WriteString(strLabel);
			}
			if(m_Ord3d == 3){
				strLabel.Format(",%.8g",m_aRowZCoords[i]);
				f.WriteString(strLabel);
			}
		}	

		//write row dendogram order
		if(m_FuseTableRows.GetSize() > 0 && !m_FuseRowChange)
		{
			for(int k=0;k<m_DendOrderRows.GetSize();k++)
			{
				if(m_DendOrderRows[k] == i)
				{
					strLabel.Format(",%d",k+1);
					f.WriteString(strLabel);
					break;
				}
			}
		}		
		
		f.WriteString("\n");
	}

	//loop for each column to build up required row vectors
	CString buffer1 = "Totals sum";
	CString buffer2a = "Totals minimum";
	CString buffer2b = "Totals mean";
	CString buffer2c = "Totals maximum";
	CString buffer3 = "Totals #>0";
	CString buffer4 = "Totals #missing";
	CString buffer5 = "Intrinsics sum";
	CString buffer6a = "Intrinsics minimum";
	CString buffer6b = "Intrinsics mean";
	CString buffer6c = "Intrinsics maximum";
	CString buffer7 = "Intrinsic #>0";
	CString buffer8 = "Intrinsics # missing";
	CString buffer9 = "Groups";	
	CString buffer10 = "PCC X";
	CString buffer11 = "PCC Y";
	CString buffer12,buffer13;
	CString buffer14;
	if(m_Ord3d == 3){
		buffer12 = "PCC Z";
		buffer13 = "PCC rSquared";
	}else if(m_Ord3d == 2){
		buffer12 = "PCC rSquared";
	}else
	{
		buffer11 = "PCC rSquared";
	}
	buffer14 = "Dendrogram Order";

	for(i=0;i<GetNoOfColumns();i++){
		//write totals
		GetColumnSum(i,strLabel);
		buffer1 += ",";
		buffer1 += strLabel;
		GetColumnMinimum(i,strLabel);
		buffer2a += ",";
		buffer2a += strLabel;
		GetColumnMean(i,strLabel);
		buffer2b += ",";
		buffer2b += strLabel;
		GetColumnMaximum(i,strLabel);
		buffer2c += ",";
		buffer2c += strLabel;
		GetColumnCountPositives(i,strLabel);
		buffer3 += ",";
		buffer3 += strLabel;
		GetColumnCountMissingValues(i,strLabel);
		buffer4 += ",";
		buffer4 += strLabel;

		//write intrinsics
		if(m_idxLeftExtrinsicColumn == -1 || i < m_idxLeftExtrinsicColumn){
			GetIntrinsicColumnSum(i,strLabel);
			buffer5 += ",";
			buffer5 += strLabel;
			GetIntrinsicColumnMinimum(i,strLabel);
			buffer6a += ",";
			buffer6a += strLabel;
			GetIntrinsicColumnMean(i,strLabel);
			buffer6b += ",";
			buffer6b += strLabel;
			GetIntrinsicColumnMaximum(i,strLabel);
			buffer6c += ",";
			buffer6c += strLabel;
			GetIntrinsicColumnCountPositives(i,strLabel);
			buffer7 += ",";	
			buffer7 += strLabel;
			GetIntrinsicColumnCountMissingValues(i,strLabel);
			buffer8 += ",";
			buffer8 += strLabel;
		}else{
			buffer5 += ",";
			buffer6a += ",";
			buffer6b += ",";
			buffer6c += ",";
			buffer7 += ",";	
			buffer8 += ",";
		}
			
		//write group
		if(GetColumnGroups(i) == -1)
			strLabel = ",0";
		else
			strLabel.Format(",%d",GetColumnGroups(i));
		buffer9 += strLabel;

		//write pcc
		strLabel.Format(",%.8g",m_Vectors[i]->GetAt(0));
		buffer10 += strLabel;
		strLabel.Format(",%.8g",m_Vectors[i]->GetAt(1));
		buffer11 += strLabel;
		strLabel.Format(",%.8g",m_Vectors[i]->GetAt(2));
		buffer12 += strLabel;
		if(m_Vectors[i]->GetSize() == 4){
			strLabel.Format(",%.8g",m_Vectors[i]->GetAt(3));
			buffer13 += strLabel;
		}

		//column dendogram
		if(m_FuseTableCols.GetSize() > 0 && !m_FuseColChange)
		{
			for(int k=0;k<m_DendOrderCols.GetSize();k++)
			{
				if(m_DendOrderCols[k] == i)
				{
					strLabel.Format(",%d",k+1);
					buffer14 += strLabel;
					break;
				}
			}
		}		
					
		if(i == m_idxLeftExtrinsicColumn){
			buffer1 += ",";
			buffer2a += ",";
			buffer2b += ",";
			buffer2c += ",";
			buffer3 += ",";
			buffer4 += ",";
			buffer5 += ",";
			buffer6a += ",";
			buffer6b += ",";
			buffer6c += ",";
			buffer7 += ",";
			buffer8 += ",";
			buffer9 += ",";
			buffer10 += ",";
			buffer11 += ",";
			buffer12 += ",";
			buffer13 += ",";
			buffer14 += ",";
		}
	}
	
	//write row vectors
	f.WriteString("\n");
	f.WriteString(buffer1);
	f.WriteString("\n");
	f.WriteString(buffer2a);
	f.WriteString("\n");
	f.WriteString(buffer2b);
	f.WriteString("\n");
	f.WriteString(buffer2c);
	f.WriteString("\n");
	f.WriteString(buffer3);
	f.WriteString("\n");
	f.WriteString(buffer4);
	f.WriteString("\n");
	f.WriteString(buffer5);
	f.WriteString("\n");
	f.WriteString(buffer6a);
	f.WriteString("\n");
	f.WriteString(buffer6b);
	f.WriteString("\n");
	f.WriteString(buffer6c);
	f.WriteString("\n");
	f.WriteString(buffer7);
	f.WriteString("\n");
	f.WriteString(buffer8);
	f.WriteString("\n");
	f.WriteString(buffer9);
	f.WriteString("\n");
	if(m_Vectors[0]->GetAt(0) != GetMissingValue())
	{
		f.WriteString(buffer10);
		f.WriteString("\n");
		f.WriteString(buffer11);
		if(m_Ord3d >= 2){
			f.WriteString("\n");
			f.WriteString(buffer12);
		}
		if(m_Ord3d == 3){
			f.WriteString("\n");
			f.WriteString(buffer13);
		}
		f.WriteString("\n");
	}

	if(m_FuseTableCols.GetSize() > 0 && !m_FuseColChange)
	{		
		f.WriteString(buffer14);		
	}
		
	

	f.Close();
}

void CPatnDoc::OnFileExportTableandinfotabs() 
{
	OnFileTableAndInfo();
}

void CPatnDoc::OnFileLoadOrdination() 
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"Ordination Vectors\t*.csv|*.csv|All Files\t*.*|*.*||",NULL);
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long count;
	int left, right;

	if ( dlg.DoModal() != IDOK )
		return;

	CWaitCursor waitcursor;

	strFile = dlg.GetPathName();

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_YESNO );
		return;
	}

	// fill table with data
	count = 0;
	long firsttime = true;
	long num_dimensions = 3;
	if(m_aRowXCoords.GetSize() <= 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("No Rows.");
		return;
	}

	for(long i=0;i<m_aRowXCoords.GetSize();i++){
		m_aRowXCoords[i] = ANALYSIS_VALUE;
		m_aRowYCoords[i] = ANALYSIS_VALUE;
		m_aRowZCoords[i] = ANALYSIS_VALUE;
	}

	this->m_OrdStress = GetMissingValue();

	bool found = false;

	while(io.ReadString(strRow) &&
		(count/num_dimensions) < m_aRowXCoords.GetSize()){

		if(strRow.Find("Project Comment") >= 0)
		{
			io.ReadString(strRow);
			io.ReadString(strRow);
			io.ReadString(strRow);
			io.ReadString(strRow);
			io.ReadString(strRow);
			found = true;
		}

		right = left = 0;
		int pos = 0;
		while(left < strRow.GetLength()){
			// get each element
	 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' || 
				strRow[left] == '\t')) 
				left++;
			right = left;
			while ( right < strRow.GetLength() && strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t')
				right++;

			if(right <= strRow.GetLength()){
				strLabel = strRow.Mid(left, right - left);
				if(found && pos < 2)
				{
					pos++;
					left = right+1;
					continue;
				}
				switch(count%num_dimensions){
				case 0:
					m_aRowXCoords[count/num_dimensions] = atof(strLabel);
					break;
				case 1:
					m_aRowYCoords[count/num_dimensions] = atof(strLabel);
					break;
				default:
					m_aRowZCoords[count/num_dimensions] = atof(strLabel);
					break;
				}
				count++;
			}

			left = right+1;
		}
		if(firsttime){
			//will probably kill the program if scat/3dview already open.
			firsttime = false;
			if(m_aRowYCoords[0] == ANALYSIS_VALUE){
				m_Ord3d = 1;
				num_dimensions = 1;
			}else if(m_aRowZCoords[0] == ANALYSIS_VALUE){
				m_Ord3d = 2;
				num_dimensions = 2;
			}else{
				m_Ord3d = 3;
				num_dimensions = 3;
			}
		}
	}

	//allow viewing of ordination
	m_import_ord = true;

	CColr ccolr(this);
	ccolr.OnCreate();

	CMst cmst(this);
	cmst.OnCreate();
		

	m_log += "\r\nImport ordination coordinates from file:\r\n\t" + strFile + "\r\n\r\n";
	UpdateAllViews(NULL);

	io.Close();

	m_OrdChange = 0;

	
	return;	
}

void CPatnDoc::OnTablePatternsobjects() 
{
//	CPatternSelection patternselection;
//	patternselection.m_pDoc = this;
//	patternselection.DoModal();
	
}

void CPatnDoc::OnFileExportGroupstatistics() 
{
	if(!m_GstaAvailable) return;
	CGsta gsta(this);
	
	gsta.SaveGsta();	
}

void CPatnDoc::OnUpdateFileExportGroupstatistics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_GstaAvailable);
}

void CPatnDoc::OnViewKruskalwalliceorder() 
{
	CKWOptions kwo(this);
	kwo.DoModal();
}

void CPatnDoc::OnUpdateViewKruskalwalliceorder(CCmdUI* pCmdUI) 
{
	//enable iff row groups exist (>1)
	pCmdUI->Enable(m_KW.GetSize());
}

void CPatnDoc::assertCell(long row,long col)
{
	//if row does not exist, add rows until it does
	while(row > GetNoOfRows()) 
		AddRow();

	//if col does not exist, add cols until it does
	while(col > GetNoOfColumns()) 
		AddColumn();

}

void CPatnDoc::addString(long row,long col,CString string)
{
	//if row == 0 && col != 0, add column label
	if(row == 0 && col != 0){
		SetColumnLabel(col-1,string);
	}else if(row != 0 && col == 0){
	//else if row != 0 && col == 0, add row label
		SetRowLabel(row-1,string);
	}else if(row != 0 && col != 0){
	//else report error! (or try to add into a cell)
		SetValueAt(col-1,row-1,atof(string));
	}
}


void CPatnDoc::addNumber(long row,long col,long double number)
{
	CString string;

	//if row == 0 && col != 0, add as column label
	if(row == 0 && col != 0){
		string.Format("%g",number);
		SetColumnLabel(col-1,string);
	//else if row != 0 && col == 0, add row label
	}else if(row != 0 && col == 0){
		string.Format("%g",number);
		SetRowLabel(row-1,string);
	//else if row != 0 && col != 0, add as data element
	}else if(row != 0 && col != 0){
		SetValueAt(col-1,row-1,number);
	//else report error! (or not)
	}else{

	}

}


void CPatnDoc::OnFileExportKruskalwallicestats() 
{
	if(!m_KW.GetSize()) return;
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Save Kruskal-Wallis Statistics:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation Kruskal-Wallis Stats.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	strBuffer.Format("Project Comment: %s\n",m_TableName);strBuffer.Replace(","," ");
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",this->m_FuseAssoRows);
	f.WriteString(strBuffer);
	strBuffer.Format("Classification Option: %s\n",m_FuseTypeRows);
	f.WriteString(strBuffer);
	strBuffer.Format("Creation Date/Time: %s\n",m_kwtime);
	f.WriteString(strBuffer);
	strBuffer.Format("Variable,ID,Kruskal-Wallis\n");
	f.WriteString(strBuffer);

	//write to file
	long i;
	for(i=0;i<GetNoOfColumns();i++)
	{
		for(long j=0;j<m_KW.GetSize();j++)
		{
			if(i == m_KWAttributes[m_KW[j]]) 
				break;
		}
		if(i < GetNoOfColumns() && j < m_KW.GetSize())
		{
			GetColumnLabel(i,strBuffer);			
			f.WriteString(strBuffer);

			strBuffer.Format(",%d,%.6f",i+1,m_KWValues[j]);
			f.WriteString(strBuffer);		
			
			f.WriteString("\n");
		}
	}
	
	

	f.Close();

}

void CPatnDoc::OnUpdateFileExportKruskalwallicestats(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_KW.GetSize());		
}

void CPatnDoc::OnViewMcao() 
{
	CMCAOResults dlg(this);
	dlg.DoModal();
}

void CPatnDoc::OnUpdateViewMcao(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_MCAOExists && !m_OrdChange);	
}

void CPatnDoc::OnFileExportMcaorawdata() 
{
	if(!m_MCAOTable.GetSize()) return;
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Save MCAO Raw Data:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " MCAO Raw Data.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	//write to file
	long i,j;
	for(i=0;i<m_MCAOTable.GetSize();i++){
		if(m_MCAOTable[i] != NULL){
			GetColumnLabel(i,strBuffer);strBuffer.Replace(","," ");
			strBuffer += ",";
			f.WriteString(strBuffer);

			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				strBuffer.Format("%.4f,",m_MCAOTable[i]->GetAt(j));				
				f.WriteString(strBuffer);
			}
			f.WriteString("\n");		
		}
	}
	
	

	f.Close();
		
}

void CPatnDoc::OnUpdateFileExportMcaorawdata(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_MCAOTable.GetSize());
}

void CPatnDoc::OnViewKruskalwallisresults() 
{
	CKWResults dlg(this);

	dlg.DoModal();	
}

void CPatnDoc::OnUpdateViewKruskalwallisresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_KW.GetSize());
}



void CPatnDoc::OnViewAnosimresults() 
{
	CAnosimResults dlg(this);

	dlg.DoModal();	
}

void CPatnDoc::OnUpdateViewAnosimresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ANOSIMRowsExists || m_ANOSIMColsExists);	
}

void CPatnDoc::OnFileExportMcaoresults() 
{
	if(!m_MCAOExists) return;

	long i,j;
	double total,value;
	CString strBuffer,strLabel;

	if(m_MCAOTable.GetSize() > m_Vectors.GetSize()) 
		return;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save MCAO Results:";
if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " MCAO.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	m_TableName.Replace(","," ");
	strBuffer.Format("Project Comment: %s.\n",m_TableName);
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",m_ColumnAsim_asso);
	f.WriteString(strBuffer);
	strBuffer.Format("Classification Option: %s\n",m_FuseTypeCols);
	f.WriteString(strBuffer);
	strBuffer.Format("Cut Threshold: %0.2f,Number of Random Starts: %ld,",m_OrdValue,m_AnalysisRecipe.rows.nOrdRandomStarts);
	f.WriteString(strBuffer);
	strBuffer.Format("Random Number Seed: %ld,Maximum Iterations: %ld,Stress: %g\n",m_AnalysisRecipe.rows.nOrdSeed,m_AnalysisRecipe.rows.nOrdIterations, this->m_OrdStress);
	f.WriteString(strBuffer);
	CTime time = CTime::GetCurrentTime();	
	
	strBuffer.Format("Creation Date/Time: %s\n",time.Format("%H:%M:%S, %B %d, %Y"));
	f.WriteString(strBuffer);

	f.WriteString("Variable,ID,%Permuted r-squared > Actual\n");

	for(i=0;i<m_MCAOTable.GetSize();i++){
		if(m_Vectors[i]->GetAt(0) == ANALYSIS_VALUE) 
			continue;
		value = m_Vectors[i]->GetAt(m_Vectors[i]->GetSize()-1);
		for(j=0,total=0;j<m_MCAOTable[i]->GetSize();j++){
			if(m_MCAOTable[i]->GetAt(j) > value){
				total++;
			}	
		}
		total /= (double)(m_MCAOTable[i]->GetSize());

		GetColumnLabel(i,strLabel);
		strBuffer.Format("%s,%d,%.4f\n",strLabel,i+1,total*100.0);

		f.WriteString(strBuffer);
	}

	f.Close();
	
}

void CPatnDoc::OnUpdateFileExportMcaoresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_MCAOExists);	
}

void CPatnDoc::OnFileExportHistory() 
{
	CString strBuffer,strLabel;

	if(m_MCAOTable.GetSize() > m_Vectors.GetSize()) 
		return;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save History:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"txt","*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text File (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " History.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	f.WriteString(m_log);

	f.Close();
	
	
}

void CPatnDoc::OnViewHistory() 
{
	CViewHistoryDlg obj;
	obj.m_plog = &m_log;
	obj.m_history = m_log;
	obj.DoModal();	
}

void CPatnDoc::OnFileExportAnosimgrouppairaffinityvectors() 
{
	if(!((m_ColumnAsim_pairs && m_AnosimColGroups.GetSize() > 1) || 
		(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1))) return;

	CAnosim obj(this);
	CPickAnosim obj2;

	if((m_ColumnAsim_pairs && m_AnosimColGroups.GetSize() > 1) || 
		(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1)){
		//offer choice
		obj2.DoModal();
		obj.SaveVector((obj2.m_columns)?true:false);
	}else if(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1){
		//only rows available
		obj.SaveVector(false);
	}else{
		//must be cols
		obj.Save(true);
	}
}

void CPatnDoc::OnUpdateFileExportAnosimgrouppairaffinityvectors(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_ColumnAsim_pairs && m_ColPairBest.GetSize() > 0) || 
		(m_RowAsim_pairs && m_RowPairBest.GetSize() > 0));
}

void CPatnDoc::OnFileExportAnosimgrouppairresults() 
{
	if(!((m_ColumnAsim_pairs && m_AnosimColGroups.GetSize() > 1) || 
		(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1))) return;
	CAnosim obj(this);
	CPickAnosim obj2;

	if((m_ColumnAsim_pairs && m_ColPairBest.GetSize() > 0) && 
		(m_RowAsim_pairs && m_RowPairBest.GetSize() > 0)){
		//offer choice of save...
		obj2.DoModal();
		obj.Save((obj2.m_columns)?true:false);
	}else if(m_RowAsim_pairs && m_RowPairBest.GetSize() > 0){
		//save rows
		obj.Save(false);
	}else{
		//must be save cols
		obj.Save(true);
	}
}

void CPatnDoc::OnUpdateFileExportAnosimgrouppairresults(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_ColumnAsim_pairs && m_AnosimColGroups.GetSize() > 1) || 
		(m_RowAsim_pairs && m_AnosimRowGroups.GetSize() > 1));
}

void CPatnDoc::OnFileExportSitecount() 
{
	CString str,strLabel;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save Site Count:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Data Table Site Count.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}

	CArray<long> obj;
	
	long nrows = GetNoOfRows();
	long ncols = GetNoOfColumns();
	if(nrows < GetIdxTopExtrinsicRow())
		nrows = GetIdxTopExtrinsicRow();
	if(ncols < GetIdxLeftExtrinsicColumn())
		ncols = GetIdxLeftExtrinsicColumn();

	long count;
	for(long i=0;i<ncols;i++){
		count = 0;
		for(long j=0;j<nrows;j++){
			//count;
			if(GetValueAt(i,j) > 0 && 
				GetValueAt(i,j) != GetMissingValue()){
				count++;
			}
			
		}
		if(count >= obj.GetSize())
			obj.SetSize(count+1);
		obj[count]++;
	}

	str.Format("Project Comment: %s\n",this->m_TableName);
	f.WriteString(str);
	CTime time = CTime::GetCurrentTime();	
	str.Format("Creation Date/Time: %s\n",time.Format("%H:%M:%S, %B %d, %Y"));
	f.WriteString(str);

	f.WriteString("Number of Variables,>0 on this number of objects\n");
	for(i=0;i<obj.GetSize();i++){
		if(obj[i] == 0) continue;
		str.Format("%4d,%4d\n",obj[i],i);	
		f.WriteString(str);
	}
	f.Close();
	
		
}

void CPatnDoc::OnViewBoxwhiskerstatistics() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	if(m_boxwhiskervisible){
		//AfxGetMainWnd()->MessageBox("Only one Box & Whisker Graph can exist \n at a time.","Message");
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(view->IsKindOf(RUNTIME_CLASS(CBoxWhiskerView))){
				view->GetParentFrame()->BringWindowToTop();
				//view->GetParentFrame()->ShowWindow(SW_RESTORE );
				break;
			}
		}
		return;
	}
	pApp->OpenBoxWhiskerView(this);
		
}

void CPatnDoc::OnUpdateViewBoxwhiskerstatistics(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(m_GstaAvailable);
	pCmdUI->Enable(m_KW.GetSize() && m_KWExists);
}

void CPatnDoc::SetupWindowsAfterLoad()
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();

	m_LoadingTable = 1;

	//open tway table
	if(m_pExists){
		m_LoadingTway = 1;
		pApp->OpenTway(this);
	}

	//open fusion windows
	if(m_FuseRowVisible){
		m_LoadingFuseRow = 1;
		pApp->OpenFuseView(this);
	}
	if(m_FuseColVisible){
		m_LoadingFuseCol = 1;
		pApp->OpenFuseView(this);
	}

	//open association windows
	if(m_AssocRowVisible){
		m_LoadingAssocRow = 1;
		pApp->OpenAssocView(this);
	}
	if(m_AssocColVisible){
		m_LoadingAssocCol = 1;
		pApp->OpenAssocView(this);
	}

	//open gsta table
	if(m_pGstaVisible) {
		m_LoadingGsta = 1;
//		pApp->OpenGstaView(this);
	}

	//open box & whisker
	if(m_boxwhiskervisible){
		m_LoadingBoxWhisker = 1;
		pApp->OpenBoxWhiskerView(this);
	}
	
	//open 3d/scat
//	if(m_OrdVisible) {
	//	m_LoadingOrd = 1;
	//	if(m_Ord3d == 3)
	//		pApp->Open3DView(this);
	//	else
	//		pApp->OpenScat(this);		
	//}else{
	//	m_3ddefault = false;
//	}

	if(m_OrdXVisible){
		m_LoadingXOrd = 1;
		pApp->Open3DXView(this);
	}

	//anosim
	if(m_AnosimRowView){
		m_LoadingAnosimRow = 1;
		m_AnosimPairView = false;
		m_AnosimRowView = true;
		pApp->OpenAnosimView(this);
	}
	if(m_AnosimColView){
		m_LoadingAnosimCol = 1;
		m_AnosimPairView = true;
		m_AnosimColView = true;
		pApp->OpenAnosimView(this);
	}
}

void CPatnDoc::OnGo() 
{
	CRect nrect;
	AfxGetMainWnd()->GetWindowRect(nrect);

	POSITION pos = GetFirstViewPosition();
	while(pos != NULL){
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)) && m_LoadingTable){
			CRect rect;
			//pView->GetParentFrame()->MoveWindow(mr_tableview,true);
			pView->GetParentFrame()->GetWindowRect(&rect);
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_tableview);

			//((CTableFrame*)pView->GetParentFrame())->m_wndSplitter2.SetColumnInfo(0,rect.Width()-(mr_rowtotals.Width()<15)?15:mr_rowtotals.Width() ,0);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter2.SetColumnInfo(1,(mr_rowtotals.Width()<15)?15:mr_rowtotals.Width(),0);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter2.RecalcLayout();	
			
			//((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.SetRowInfo(0,rect.Height()-(mr_coltotals.Height()<15)?15:mr_coltotals.Height(),0);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.SetRowInfo(1,(mr_coltotals.Height()<15)?15:mr_coltotals.Height(),0);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.RecalcLayout();

			
			
			m_LoadingTable = 0;
//		}else if(pView->IsKindOf(RUNTIME_CLASS(CGstaView))){
			//pView->GetParentFrame()->MoveWindow(mr_gsta);
	//		pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_gstaview);
	//		m_LoadingGsta = 0;
		}else if(pView->IsKindOf(RUNTIME_CLASS(CTway))){
			//pView->GetParentFrame()->MoveWindow(mr_tway);
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_tway);
			m_LoadingTway = 0;
/*		}else if(pView->IsKindOf(RUNTIME_CLASS(C3DView)) && m_LoadingOrd){
			//pView->GetParentFrame()->MoveWindow(mr_ord,true);
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_3dview);

			C3DView * pv = (C3DView*)pView;
			pv->m_Rotate = (m_Rotate)?true:false;
			pv->m_Labels = (m_Labels)?true:false;
			pv->m_Legend = (m_Legend)?true:false;
			pv->m_Mst = (m_Mst)?true:false;
			pv->m_Axies = (m_Axies)?true:false;
			pv->m_Grouping = (m_Grouping)?true:false;
			pv->m_Usergroupcolours = (m_Usergroupcolours)?true:false;
			pv->m_SelOnly = (m_SelOnly)?true:false;
			pv->m_Centroid = (m_Centroid)?true:false;
			pv->m_Showranges = (m_Showranges)?true:false;
			pv->m_Symbols = (m_Symbols)?true:false;
			pv->m_RotationSpeed = m_RotationSpeed;
			for(long i=0;i<m_TextBoxes.GetSize();i++){
				pv->m_TextBoxes.Add(new TextBox);
				pv->m_TextBoxes[i]->contents = m_TextBoxes[i]->contents;
				pv->m_TextBoxes[i]->size = m_TextBoxes[i]->size;
				pv->m_TextBoxes[i]->location = m_TextBoxes[i]->location;
			}			
			pv->wAngleY = wAngleY; 
			pv->m_NoSymbols = (m_NoSymbols)?true:false;
			for(i=0;i<m_PccVectors.GetSize();i++){
				pv->m_PccVectors.Add(m_PccVectors[i]);
			}
			m_LoadingOrd = 0;
		}else if(pView->IsKindOf(RUNTIME_CLASS(CScat))&& m_LoadingOrd){
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_3dview);

			//pView->GetParentFrame()->MoveWindow(mr_ord,true);
			CScat * pv = (CScat*)pView;
			pv->m_Rotate = (m_Rotate)?true:false;
			pv->m_Labels = (m_Labels)?true:false;
			pv->m_Legend = (m_Legend)?true:false;
			pv->m_Mst = (m_Mst)?true:false;
			pv->m_Axies = (m_Axies)?true:false;
			pv->m_Grouping = (m_Grouping)?true:false;
			pv->m_Usergroupcolours = (m_Usergroupcolours)?true:false;
			pv->m_SelOnly = (m_SelOnly)?true:false;
			pv->m_Centroid = (m_Centroid)?true:false;
			pv->m_Showranges = (m_Showranges)?true:false;
			pv->m_Symbols = (m_Symbols)?true:false;
			for(long i=0;i<m_TextBoxes.GetSize();i++){
				pv->m_TextBoxes.Add(new TextBox);
				pv->m_TextBoxes[i]->contents = m_TextBoxes[i]->contents;
				pv->m_TextBoxes[i]->size = m_TextBoxes[i]->size;
				pv->m_TextBoxes[i]->location = m_TextBoxes[i]->location;
			}			
			pv->m_NoSymbols = (m_NoSymbols)?true:false;
			for(i=0;i<m_PccVectors.GetSize();i++){
				pv->m_PccVectors.Add(m_PccVectors[i]);
			}
			m_LoadingOrd = 0;*/
		}else if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)) && m_LoadingXOrd && !((C3DXView*)pView)->m_legend_instance){
			//pView->GetParentFrame()->MoveWindow(mr_ord,true);
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_3dview);
			C3DXView * pv = (C3DXView*)pView;
			pv->m_Rotate = (m_Rotate)?true:false;
			if(pv->m_Rotate) pv->m_menu.CheckMenuItem(ID__ROTATE,MF_CHECKED);

			pv->m_Labels = (m_Labels)?true:false;
			if(pv->m_Labels) pv->m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_CHECKED);

			pv->m_Legend = false; //(m_Legend)?true:false;
			//if(pv->m_Legend) pv->m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_CHECKED);

			pv->m_Mst = (m_Mst)?true:false;
			if(pv->m_Mst) pv->m_menu.CheckMenuItem(ID__DISPLAYMST,MF_CHECKED);

			pv->m_Axies = (m_Axies)?true:false;
			
			pv->m_Grouping = (m_Grouping)?true:false;
			if(pv->m_Grouping) pv->m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_CHECKED);

			pv->m_Usergroupcolours = (m_Usergroupcolours)?true:false;
			if(pv->m_Usergroupcolours) pv->m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_CHECKED);

			pv->m_SelOnly = (m_SelOnly)?true:false;
			if(pv->m_SelOnly) pv->m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_CHECKED);

			pv->m_Centroid = (m_Centroid)?true:false;
			if(pv->m_Centroid) pv->m_menu.CheckMenuItem(ID__DISPLAYCENTROIDS,MF_CHECKED);

			pv->m_Showranges = (m_Showranges)?true:false;
			if(pv->m_Showranges) pv->m_menu.CheckMenuItem(ID__SHOWRANGES,MF_CHECKED);

			pv->m_Symbols = (m_Symbols)?true:false;
			if(pv->m_Symbols) pv->m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_CHECKED);

			pv->m_RotationSpeed = m_RotationSpeed;
			for(long i=0;i<m_TextBoxes2.GetSize();i++){
				pv->m_TextBoxes.Add(new TextBox2);
				pv->m_TextBoxes[i]->contents = m_TextBoxes2[i]->contents;
				memcpy(&pv->m_TextBoxes[i]->font,&(m_TextBoxes2[i]->font),sizeof(LOGFONT));
				strcpy(pv->m_TextBoxes[i]->font.lfFaceName,m_TextBoxes2[i]->font.lfFaceName);
				pv->m_TextBoxes[i]->location = m_TextBoxes2[i]->location;
				pv->m_TextBoxes[i]->colour = m_TextBoxes2[i]->colour;
			}			
			pv->m_stressbox.contents = m_stressbox.contents;
			memcpy(&pv->m_stressbox.font,&(m_stressbox.font),sizeof(LOGFONT));
			strcpy(pv->m_stressbox.font.lfFaceName,m_stressbox.font.lfFaceName);
			pv->m_stressbox.location = m_stressbox.location;
			pv->m_stressbox.colour = m_stressbox.colour;

			pv->m_vEyePt.x = m_vEyePt.x;
			pv->m_vEyePt.y = m_vEyePt.y;
			pv->m_vEyePt.z = m_vEyePt.z;
			pv->m_vUpVec.x = m_vUpVec.x;
			pv->m_vUpVec.y = m_vUpVec.y;
			pv->m_vUpVec.z = m_vUpVec.z;
			pv->m_vRight.x = m_vRight.x;
			pv->m_vRight.y = m_vRight.y;
			pv->m_vRight.z = m_vRight.z;

			pv->m_NoSymbols = (m_NoSymbols)?true:false;
			if(pv->m_Labels) pv->m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_CHECKED);

			for(i=0;i<m_PccVectors.GetSize();i++){
				pv->m_PccVectors.Add(m_PccVectors[i]);
			}
			pv->InitPcc();
			m_LoadingXOrd = 0;
		}else if(pView->IsKindOf(RUNTIME_CLASS(CBoxWhiskerView)) && m_LoadingBoxWhisker){
			//pView->GetParentFrame()->MoveWindow(mr_boxwhisker);
			pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_boxwhisker);
			m_LoadingBoxWhisker = 0;
		}else if(pView->IsKindOf(RUNTIME_CLASS(CAnosimView)) && (m_LoadingAnosimRow || m_LoadingAnosimCol)){
			if(((CAnosimView*)pView)->m_columns){
				//pView->GetParentFrame()->MoveWindow(mr_anosimcol);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_anosimcol);
				m_LoadingAnosimCol = 0;
			}else{
				//pView->GetParentFrame()->MoveWindow(mr_anosimrow);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_anosimrow);
				m_LoadingAnosimRow = 0;
			}			
		}else if(pView->IsKindOf(RUNTIME_CLASS(CAssocMatrixView)) && (m_LoadingAssocRow || m_LoadingAssocCol)){
			if(((CAssocMatrixView*)pView)->m_pMatrix == &m_AssocMatrixRows){
				//pView->GetParentFrame()->MoveWindow(mr_assocrowmx);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_assocrow);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.SetColumnInfo(0,mr_assocrowmx.Width(),0);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.RecalcLayout();
				m_LoadingAssocRow = 0;
			}else if(((CAssocMatrixView*)pView)->m_pMatrix == &m_AssocMatrixCols){
				//pView->GetParentFrame()->MoveWindow(mr_assoccolmx);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_assoccol);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.SetColumnInfo(0,mr_assoccolmx.Width(),0);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.RecalcLayout();
				m_LoadingAssocCol = 0;
			}
		}else if(pView->IsKindOf(RUNTIME_CLASS(CFuseView))&& (m_LoadingFuseRow || m_LoadingFuseCol)){
			if(((CFuseView*)pView)->m_pFuseTable == &m_FuseTableRows){
				//pView->GetParentFrame()->MoveWindow(mr_fuserow);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_fuserow);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.SetColumnInfo(0,mr_fuserow.Width(),0);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.RecalcLayout();
				m_LoadingFuseRow = 0;
			}else if(((CFuseView*)pView)->m_pFuseTable == &m_FuseTableCols){
				//pView->GetParentFrame()->MoveWindow(mr_fusecol);
				pView->GetParentFrame()->SetWindowPlacement(&m_wndplace_fusecol);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.SetColumnInfo(0,mr_fusecol.Width(),0);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.RecalcLayout();
				m_LoadingFuseCol = 0;
			}			
		}
	}
	
	pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		pView->RedrawWindow();
	}
}


void CPatnDoc::SetMaxWidth(CString filename)
{
	//read in file

	long extpos,pos,num = 0;

	extpos = 0;
	pos = 0;
	while((pos = filename.Find(".",pos+1)) != -1) extpos = pos;

	CStdioFile in;
	CArray<CStdioFile*> out;
	out.Add(new CStdioFile);

	in.Open(filename,CFile::modeRead);
	CString snum;
	snum.Format("%d",num++);
	CString snew;
	snew = filename;
	snew.Insert(extpos,snum);
	out[0]->Open(snew,CFile::modeWrite);

	CString buf,word,write;
	long count,f,newpos,line = 0;
	while(in.ReadString(buf)){
		count = 0;
		pos = 0;f = 0;
		write = "";
		while((pos =  buf.Find(",",pos)) != -1){
			newpos = buf.Find(",",pos+1);
			if(newpos == -1)
				word = buf.Mid(pos+1,buf.GetLength());
			else
				word = buf.Mid(pos+1,newpos-1);
			if(count < 256){
				if(write == "")
					write += word;
				else
					write += "," + word;
				count++;
			}
			if(count == 256){
				if(f >= out.GetSize()){
					out.Add(new CStdioFile);
					snum.Format("%d",num++);
					snew = filename;
					snew.Insert(extpos,snum);
					out[f]->Open(snew,CFile::modeWrite);
					for(long i=0;i<line;i++){
						out[f]->WriteString("\r\n");
					}
				}					
				write += "\r\n";
				out[f]->WriteString(write);
				f++;
				count = 0;
				write = "";
			}
		}
		line++;
	}
	
	for(long i=0;i<out.GetSize();i++){
		out[i]->Close();
		delete out[i];
	}
	in.Close();

}


void CPatnDoc::SerializeV2_12(CArchive &ar,double version)
{

//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//double version = 2.12;
	//if(ar.IsStoring())
	//	//ar << (double) 2.09; //current version number
	//	ar << version;
	//else
	//	ar >> version;

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	m_aColumnSums.Serialize(ar);
	m_aColumnCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMinimum.Serialize(ar);
	}else{
		m_aColumnMinimum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMinimum.GetSize();i++){
			m_aColumnMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) < m_aColumnMinimum[i] || m_aColumnMinimum[i] == m_nMissingValue)
					m_aColumnMinimum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnMean.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMaximum.Serialize(ar);
	}else{
		m_aColumnMaximum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMaximum.GetSize();i++){
			m_aColumnMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) > m_aColumnMaximum[i] || m_aColumnMaximum[i] == m_nMissingValue)
					m_aColumnMaximum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnCountMissingValues.Serialize(ar);
	m_aColumnGroups.Serialize(ar);

	m_aRowSums.Serialize(ar);
	m_aRowCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aRowMinimum.Serialize(ar);
	}else{
		m_aRowMinimum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMinimum.GetSize();i++){
			m_aRowMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) < m_aRowMinimum[i] || m_aRowMinimum[i] == m_nMissingValue)
					m_aRowMinimum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowMean.Serialize(ar);
	if(version >= 2.11){
		m_aRowMaximum.Serialize(ar);
	}else{
		m_aRowMaximum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMaximum.GetSize();i++){
			m_aRowMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) > m_aRowMaximum[i] || m_aRowMaximum[i] == m_nMissingValue)
					m_aRowMaximum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowCountMissingValues.Serialize(ar);
	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	for(i=0;i<m_aColumnTotalsRowAttributes.GetSize();i++)
		m_aColumnTotalsRowAttributes[i]->Unhide();
	CRowAttributes * attribs;
	CColumnAttributes * cattribs;
	if(version < 2.11){
		attribs = new CRowAttributes;
		attribs->SetLabel("Minimum");
		attribs->SetType(CRowColumnAttributes::Minimum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(1,attribs);
	
		m_aColumnTotalsRowAttributes[2]->SetLabel("Mean");
		m_aColumnTotalsRowAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		attribs = new CRowAttributes;
		attribs->SetLabel("Maximum");
		attribs->SetType(CRowColumnAttributes::Maximum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(3,attribs);
	}
	m_aRowTotalsColumnAttributes.Serialize(ar);
	for(i=0;i<m_aRowTotalsColumnAttributes.GetSize();i++)
		m_aRowTotalsColumnAttributes[i]->Unhide();

	if(version < 2.11){
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Minimum");
		cattribs->SetType(CRowColumnAttributes::Minimum);
		m_aRowTotalsColumnAttributes.InsertAt(1,cattribs);
	
		m_aRowTotalsColumnAttributes[2]->SetLabel("Mean");
		m_aRowTotalsColumnAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Maximum");
		cattribs->SetType(CRowColumnAttributes::Maximum);
		m_aRowTotalsColumnAttributes.InsertAt(3,cattribs);
	}
	m_aColumnAllocRowAttributes.Serialize(ar);
	for(i=0;i<m_aColumnAllocRowAttributes.GetSize();i++)
		m_aColumnAllocRowAttributes[i]->Unhide();
	m_aColumnCompareRowAttributes.Serialize(ar);
	for(i=0;i<m_aColumnCompareRowAttributes.GetSize();i++)
		m_aColumnCompareRowAttributes[i]->Unhide();
	m_aColumnPccAttributes.Serialize(ar);
	for(i=0;i<m_aColumnPccAttributes.GetSize();i++)
		m_aColumnPccAttributes[i]->Unhide();
	m_aRowAllocColumnAttributes.Serialize(ar);
	for(i=0;i<m_aRowAllocColumnAttributes.GetSize();i++)
		m_aRowAllocColumnAttributes[i]->Unhide();
	m_aRowOrdColumnAttributes.Serialize(ar);
	for(i=0;i<m_aRowOrdColumnAttributes.GetSize();i++)
		m_aRowOrdColumnAttributes[i]->Unhide();

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);
	m_KWVisible.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(version >= 2.12){
		if(ar.IsStoring()){
			ar << m_MCAOTable.GetSize();
			for(i=0;i<m_MCAOTable.GetSize();i++){
				ar << m_MCAOTable[i]->GetSize();
				for(j=0;j<m_MCAOTable[i]->GetSize();j++)
					ar << m_MCAOTable[i]->GetAt(j);
			}
		}else{
			ar >> nCount;m_MCAOTable.SetSize(nCount);
			for(i=0;i<m_MCAOTable.GetSize();i++){
				m_MCAOTable[i] = new CArray<double>;
				ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
				for(j=0;j<m_MCAOTable[i]->GetSize();j++){
					ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
				}
			}
		}	
	}

	
	long nSize;

	if (ar.IsStoring())
	{
		
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		LOGFONT tmpfont;
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		if(version >= 2.111){
			ar >> m_RowAsim_stat;
			ar >> m_RowAsim_rbest;
			ar >> m_RowAsim_save;
			ar >> m_RowAsim_iterations;
			ar >> m_ColumnAsim_stat;
			ar >> m_ColumnAsim_rbest;
			ar >> m_ColumnAsim_save;
			ar >> m_ColumnAsim_iterations;
		}
		if(version >= 2.12){
			ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nBeta;
			ar >> m_AnalysisRecipe.cols.nClassStrategy;
			ar >> m_AnalysisRecipe.cols.nCut;
			ar >> m_AnalysisRecipe.cols.nFuseTechnique;
			ar >> m_AnalysisRecipe.cols.nMinkConst;
			ar >> m_AnalysisRecipe.cols.nNoOfGroups;
			ar >> m_AnalysisRecipe.cols.nOrd3d;
			ar >> m_AnalysisRecipe.cols.nOrdination;
			ar >> m_AnalysisRecipe.cols.nOrdIterations;
			ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.cols.nOrdSeed;
			ar >> m_AnalysisRecipe.nFirstTime;
			ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nBeta;
			ar >> m_AnalysisRecipe.rows.nClassStrategy;
			ar >> m_AnalysisRecipe.rows.nCut;
			ar >> m_AnalysisRecipe.rows.nFuseTechnique;
			ar >> m_AnalysisRecipe.rows.nMinkConst;
			ar >> m_AnalysisRecipe.rows.nNoOfGroups;
			ar >> m_AnalysisRecipe.rows.nOrd3d;
			ar >> m_AnalysisRecipe.rows.nOrdination;
			ar >> m_AnalysisRecipe.rows.nOrdIterations;
			ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.rows.nOrdSeed;
		}else{
			m_AnalysisRecipe.nFirstTime = true;
		}


		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)
			CGsta cgsta(this);
	}
}

void CPatnDoc::SerializeV2_17(CArchive& ar,double version)
{
	//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//update visible window rect regions
	
	if(version <= 2.12){
		SerializeV2_12(ar,version);

//		SetDefaultFontTway();
//		SetDefaultFontGsta();
//		SetDefaultFontBW();

		m_3ddefault = false;
		m_AssocRowVisible = 0;
		m_AssocColVisible = 0;
		m_AssocRowUpdate = 0;
		m_AssocColUpdate = 0;
		m_FuseRowVisible = 0;
		m_FuseColVisible = 0;
		m_FuseRowUpdate = 0;
		m_FuseColUpdate = 0;
		m_nScale = 0;
		m_pExists = 0;
		m_pIsRowLabel = 0;
		m_colstand = 0;
		m_presabs = 0;
		m_globstand = 0;
		m_red = 0;
		m_green = 0;
		m_blue = 0;
		m_bw = 0;
		m_AnosimPairView = 0;
		m_AnosimColView = 0; 
		m_AnosimRowView = 0; 
		m_OrdVisible = 0;
		m_OrdUpdate = 0;
		m_boxwhiskervisible = 0;

		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	m_aColumnSums.Serialize(ar);
	m_aColumnCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMinimum.Serialize(ar);
	}else{
		m_aColumnMinimum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMinimum.GetSize();i++){
			m_aColumnMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) < m_aColumnMinimum[i] || m_aColumnMinimum[i] == m_nMissingValue)
					m_aColumnMinimum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnMean.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMaximum.Serialize(ar);
	}else{
		m_aColumnMaximum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMaximum.GetSize();i++){
			m_aColumnMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) > m_aColumnMaximum[i] || m_aColumnMaximum[i] == m_nMissingValue)
					m_aColumnMaximum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnCountMissingValues.Serialize(ar);
	m_aColumnGroups.Serialize(ar);

	m_aRowSums.Serialize(ar);
	m_aRowCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aRowMinimum.Serialize(ar);
	}else{
		m_aRowMinimum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMinimum.GetSize();i++){
			m_aRowMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) < m_aRowMinimum[i] || m_aRowMinimum[i] == m_nMissingValue)
					m_aRowMinimum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowMean.Serialize(ar);
	if(version >= 2.11){
		m_aRowMaximum.Serialize(ar);
	}else{
		m_aRowMaximum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMaximum.GetSize();i++){
			m_aRowMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) > m_aRowMaximum[i] || m_aRowMaximum[i] == m_nMissingValue)
					m_aRowMaximum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowCountMissingValues.Serialize(ar);
	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	CRowAttributes * attribs;
	CColumnAttributes * cattribs;
	if(version < 2.11){
		attribs = new CRowAttributes;
		attribs->SetLabel("Minimum");
		attribs->SetType(CRowColumnAttributes::Minimum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(1,attribs);
	
		m_aColumnTotalsRowAttributes[2]->SetLabel("Mean");
		m_aColumnTotalsRowAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		attribs = new CRowAttributes;
		attribs->SetLabel("Maximum");
		attribs->SetType(CRowColumnAttributes::Maximum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(3,attribs);
	}
	m_aRowTotalsColumnAttributes.Serialize(ar);
	if(version < 2.11){
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Minimum");
		cattribs->SetType(CRowColumnAttributes::Minimum);
		m_aRowTotalsColumnAttributes.InsertAt(1,cattribs);
	
		m_aRowTotalsColumnAttributes[2]->SetLabel("Mean");
		m_aRowTotalsColumnAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Maximum");
		cattribs->SetType(CRowColumnAttributes::Maximum);
		m_aRowTotalsColumnAttributes.InsertAt(3,cattribs);
	}
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);
	m_KWVisible.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(version >= 2.12){
		if(ar.IsStoring()){
			ar << m_MCAOTable.GetSize();
			for(i=0;i<m_MCAOTable.GetSize();i++){
				ar << m_MCAOTable[i]->GetSize();
				for(j=0;j<m_MCAOTable[i]->GetSize();j++)
					ar << m_MCAOTable[i]->GetAt(j);
			}
		}else{
			ar >> nCount;m_MCAOTable.SetSize(nCount);
			for(i=0;i<m_MCAOTable.GetSize();i++){
				m_MCAOTable[i] = new CArray<double>;
				ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
				for(j=0;j<m_MCAOTable[i]->GetSize();j++){
					ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
				}
			}
		}	
	}

	long nSize;

	if (ar.IsStoring())
	{
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		LOGFONT tmpfont;
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		if(version >= 2.16){
			ar.Read(&tmpfont,sizeof(LOGFONT));
			ar.Read(&tmpfont,sizeof(LOGFONT));
			ar.Read(&tmpfont,sizeof(LOGFONT));
		}else{
		//	SetDefaultFontTway();
		//	SetDefaultFontGsta();
		//	SetDefaultFontBW();
		}

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		if(version >= 2.111){
			ar >> m_RowAsim_stat;
			ar >> m_RowAsim_rbest;
			ar >> m_RowAsim_save;
			ar >> m_RowAsim_iterations;
			ar >> m_ColumnAsim_stat;
			ar >> m_ColumnAsim_rbest;
			ar >> m_ColumnAsim_save;
			ar >> m_ColumnAsim_iterations;
		}
		if(version >= 2.12){
			ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nBeta;
			ar >> m_AnalysisRecipe.cols.nClassStrategy;
			ar >> m_AnalysisRecipe.cols.nCut;
			ar >> m_AnalysisRecipe.cols.nFuseTechnique;
			ar >> m_AnalysisRecipe.cols.nMinkConst;
			ar >> m_AnalysisRecipe.cols.nNoOfGroups;
			ar >> m_AnalysisRecipe.cols.nOrd3d;
			ar >> m_AnalysisRecipe.cols.nOrdination;
			ar >> m_AnalysisRecipe.cols.nOrdIterations;
			ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.cols.nOrdSeed;
			ar >> m_AnalysisRecipe.nFirstTime;
			ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nBeta;
			ar >> m_AnalysisRecipe.rows.nClassStrategy;
			ar >> m_AnalysisRecipe.rows.nCut;
			ar >> m_AnalysisRecipe.rows.nFuseTechnique;
			ar >> m_AnalysisRecipe.rows.nMinkConst;
			ar >> m_AnalysisRecipe.rows.nNoOfGroups;
			ar >> m_AnalysisRecipe.rows.nOrd3d;
			ar >> m_AnalysisRecipe.rows.nOrdination;
			ar >> m_AnalysisRecipe.rows.nOrdIterations;
			ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.rows.nOrdSeed;
		}else{
			m_AnalysisRecipe.nFirstTime = true;
		}
		if(version >= 2.15){
			ar >> i;m_pGstaVisible = (i)?true:false;
			ar >> m_AssocRowVisible;
			ar >> m_AssocColVisible;
			ar >> m_AssocRowUpdate;
			ar >> m_AssocColUpdate;
			ar >> m_FuseRowVisible;
			ar >> m_FuseColVisible;
			ar >> m_FuseRowUpdate;
			ar >> m_FuseColUpdate;
			ar >> m_nScale;
			ar >> m_pExists;
			ar >> m_pIsRowLabel;
			ar >> m_colstand;
			ar >> m_presabs;
			ar >> m_globstand;
			ar >> m_red;
			ar >> m_green;
			ar >> m_blue;
			ar >> m_bw;
			ar >> i;m_AnosimPairView = (i)?true:false;
			ar >> i;m_AnosimColView = (i)?true:false; 
			ar >> i;m_AnosimRowView = (i)?true:false; 
			ar >> i;m_OrdVisible = (i)?true:false;
			ar >> i;m_OrdUpdate = (i)?true:false;
			ar >> m_boxwhiskervisible;
			ar >> mr_tableview;
			ar >> mr_rowtotals;
			ar >> mr_coltotals;
			ar >> mr_anosimrow;
			ar >> mr_anosimcol;
			ar >> mr_fuserow;
			ar >> mr_fusecol;
			ar >> mr_fuserowdend;
			ar >> mr_fusecoldend;
			ar >> mr_assocrowmx;
			ar >> mr_assoccolmx;
			ar >> mr_assocrowhist;
			ar >> mr_assoccolhist;
			ar >> mr_ord;
			ar >> mr_tway;
			ar >> mr_gsta;
			ar >> mr_boxwhisker;

			ar >> i;m_Rotate = (i)?true:false;
			ar >> i;m_Labels = (i)?true:false;
			ar >> i;m_Legend = (i)?true:false;
			ar >> i;m_Mst = (i)?true:false;
			ar >> i;m_Axies = (i)?true:false;
			ar >> i;m_Grouping = (i)?true:false;
			ar >> i;m_Usergroupcolours = (i)?true:false;
			ar >> i;m_SelOnly = (i)?true:false;
			ar >> i;m_Centroid = (i)?true:false;
			ar >> i;m_Showranges = (i)?true:false;
			ar >> i;m_Symbols = (i)?true:false;
			ar >> m_RotationSpeed;
			long size;
			ar >> size;m_TextBoxes.SetSize(size);
			long i;
			for(i=0;i<m_TextBoxes.GetSize();i++){
				m_TextBoxes[i] = new TextBox;
				ar >> m_TextBoxes[i]->contents;
				ar >> m_TextBoxes[i]->size;
				ar >> m_TextBoxes[i]->location;
			}			
			ar >> wAngleY; 
			ar >> i;m_NoSymbols = (i)?true:false;
			ar >> size;m_PccVectors.SetSize(size);
			for(i=0;i<m_PccVectors.GetSize();i++){
				ar >> m_PccVectors[i];
			}

			ar >> nCount;
			m_aGroupColours2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_aGroupColours2[i].red;
				ar >> m_aGroupColours2[i].green;
				ar >> m_aGroupColours2[i].blue;			
			}
			ar >> nCount;
			m_aObjectColours2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_aObjectColours2[i].red;
				ar >> m_aObjectColours2[i].green;
				ar >> m_aObjectColours2[i].blue;			
			}
			ar >> nCount;
			m_r.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_r[i].red;
				ar >> m_r[i].green;
				ar >> m_r[i].blue;			
			}
			ar >> nCount;
			m_Vectors2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_Vectors2[i].red;
				ar >> m_Vectors2[i].green;
				ar >> m_Vectors2[i].blue;			
			}
			m_3ddefault = true;
		}else{
			m_3ddefault = false;
			m_AssocRowVisible = 0;
			m_AssocColVisible = 0;
			m_AssocRowUpdate = 0;
			m_AssocColUpdate = 0;
			m_FuseRowVisible = 0;
			m_FuseColVisible = 0;
			m_FuseRowUpdate = 0;
			m_FuseColUpdate = 0;
			m_nScale = 0;
			m_pExists = 0;
			m_pIsRowLabel = 0;
			m_colstand = 0;
			m_presabs = 0;
			m_globstand = 0;
			m_red = 0;
			m_green = 0;
			m_blue = 0;
			m_bw = 0;
			m_AnosimPairView = 0;
			m_AnosimColView = 0; 
			m_AnosimRowView = 0; 
			m_OrdVisible = 0;
			m_OrdUpdate = 0;
			m_boxwhiskervisible = 0;
		}
		
		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)
			CGsta cgsta(this);

		//SetupWindowsAfterLoad();
			m_pExists = 0;
		m_FuseRowVisible = 0;
		m_FuseColVisible = 0;
		m_AssocRowVisible = 0;
		m_AssocColVisible = 0;
		m_pGstaVisible = 0;
		m_boxwhiskervisible = 0;
		m_OrdVisible = 0;
		m_OrdXVisible = 0;
		m_AnosimRowView = 0;
		m_AnosimColView = 0;
	}
}

//-----------------------------------------------------------------------------
// File: GetDXVer.cpp
//
// Desc: Demonstrates how applications can detect what version of DirectX
//       is installed.
//
//    (C) Copyright 1995-1997 Microsoft Corp.  All rights reserved.
//-----------------------------------------------------------------------------

typedef HRESULT(WINAPI * DIRECTDRAWCREATE)( GUID*, LPDIRECTDRAW*, IUnknown* );
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );
typedef HRESULT(WINAPI * DIRECTINPUTCREATE)( HINSTANCE, DWORD, LPDIRECTINPUT*,
											 IUnknown* );

//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns two arguments:
//          dwDXVersion:
//            0x0000 = No DirectX installed
//            0x0100 = DirectX version 1 installed
//            0x0200 = DirectX 2 installed
//            0x0300 = DirectX 3 installed
//            0x0500 = At least DirectX 5 installed.
//            0x0600 = At least DirectX 6 installed.
//            0x0601 = At least DirectX 6.1 installed.
//            0x0700 = At least DirectX 7 installed.
//          dwDXPlatform:
//            0                          = Unknown (This is a failure case)
//            VER_PLATFORM_WIN32_WINDOWS = Windows 9X platform
//            VER_PLATFORM_WIN32_NT      = Windows NT platform
// 
//          Please note that this code is intended as a general guideline. Your
//          app will probably be able to simply query for functionality (via
//          QueryInterface) for one or two components.
//
//          Please also note:
//            "if (dxVer != 0x500) return FALSE;" is BAD. 
//            "if (dxVer < 0x500) return FALSE;" is MUCH BETTER.
//          to ensure your app will run on future releases of DirectX.
//-----------------------------------------------------------------------------
VOID GetDXVersion( DWORD* pdwDXVersion, DWORD* pdwDXPlatform )
{
//	HRESULT              hr;
	HINSTANCE            DDHinst = 0;
	HINSTANCE            DIHinst = 0;
	LPDIRECTDRAW         pDDraw  = 0;
	LPDIRECTDRAW2        pDDraw2 = 0;
	DIRECTDRAWCREATE     DirectDrawCreate   = 0;
	DIRECTDRAWCREATEEX   DirectDrawCreateEx = 0;
	DIRECTINPUTCREATE    DirectInputCreate  = 0;
//	OSVERSIONINFO        osVer;
	LPDIRECTDRAWSURFACE  pSurf  = 0;
	LPDIRECTDRAWSURFACE3 pSurf3 = 0;
	LPDIRECTDRAWSURFACE4 pSurf4 = 0;

	//CUT IT DOWN TO REDUCE ERRORS
	/*
	// First get the windows platform
	osVer.dwOSVersionInfoSize = sizeof(osVer);
	if( !GetVersionEx( &osVer ) )
	{
		(*pdwDXPlatform) = 0;
		(*pdwDXVersion)  = 0;
		return;
	}

	if( osVer.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		(*pdwDXPlatform) = VER_PLATFORM_WIN32_NT;

		// NT is easy... NT 4.0 is DX2, 4.0 SP3 is DX3, 5.0 is DX5
		// and no DX on earlier versions.
		if( osVer.dwMajorVersion < 4 )
		{
			(*pdwDXVersion) = 0; // No DX on NT3.51 or earlier
			return;
		}

		if( osVer.dwMajorVersion == 4 )
		{
			// NT4 up to SP2 is DX2, and SP3 onwards is DX3, so we are at least DX2
			(*pdwDXVersion) = 0x200;

			// We're not supposed to be able to tell which SP we're on, so check for dinput
			DIHinst = LoadLibrary( "DINPUT.DLL" );
			if( DIHinst == 0 )
			{
				// No DInput... must be DX2 on NT 4 pre-SP3
				OutputDebugString( "Couldn't LoadLibrary DInput\r\n" );
				return;
			}

			DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( DIHinst,
																 "DirectInputCreateA" );
			FreeLibrary( DIHinst );

			if( DirectInputCreate == 0 )
			{
				// No DInput... must be pre-SP3 DX2
				OutputDebugString( "Couldn't GetProcAddress DInputCreate\r\n" );
				return;
			}

			// It must be NT4, DX2
			(*pdwDXVersion) = 0x300;  // DX3 on NT4 SP3 or higher
			return;
		}
		// Else it's NT5 or higher, and it's DX5a or higher: Drop through to
		// Win9x tests for a test of DDraw (DX6 or higher)
	}
	else
	{
		// Not NT... must be Win9x
		(*pdwDXPlatform) = VER_PLATFORM_WIN32_WINDOWS;
	}

	// Now we know we are in Windows 9x (or maybe 3.1), so anything's possible.
	// First see if DDRAW.DLL even exists.
	DDHinst = LoadLibrary( "DDRAW.DLL" );
	if( DDHinst == 0 )
	{
		(*pdwDXVersion)  = 0;
		(*pdwDXPlatform) = 0;
		FreeLibrary( DDHinst );
		return;
	}

	// See if we can create the DirectDraw object.
	DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress( DDHinst, "DirectDrawCreate" );
	if( DirectDrawCreate == 0 )
	{
		(*pdwDXVersion)  = 0;
		(*pdwDXPlatform) = 0;
		FreeLibrary( DDHinst );
		OutputDebugString( "Couldn't LoadLibrary DDraw\r\n" );
		return;
	}

	hr = DirectDrawCreate( NULL, &pDDraw, NULL );
	if( FAILED(hr) )
	{
		(*pdwDXVersion)  = 0;
		(*pdwDXPlatform) = 0;
		FreeLibrary( DDHinst );
		OutputDebugString( "Couldn't create DDraw\r\n" );
		return;
	}

	// So DirectDraw exists.  We are at least DX1.
	(*pdwDXVersion) = 0x100;

	// Let's see if IID_IDirectDraw2 exists.
	hr = pDDraw->QueryInterface( IID_IDirectDraw2, (VOID**)&pDDraw2 );
	if( FAILED(hr) )
	{
		// No IDirectDraw2 exists... must be DX1
		pDDraw->Release();
		FreeLibrary( DDHinst );
		OutputDebugString( "Couldn't QI DDraw2\r\n" );
		return;
	}

	// IDirectDraw2 exists. We must be at least DX2
	pDDraw2->Release();
	(*pdwDXVersion) = 0x200;


	///////////////////////////////////////////////////////////////////////////
	// DirectX 3.0 Checks
	///////////////////////////////////////////////////////////////////////////

	// DirectInput was added for DX3
	DIHinst = LoadLibrary( "DINPUT.DLL" );
	if( DIHinst == 0 )
	{
		// No DInput... must not be DX3
		OutputDebugString( "Couldn't LoadLibrary DInput\r\n" );
		pDDraw->Release();
		FreeLibrary( DDHinst );
		return;
	}

	DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( DIHinst,
														"DirectInputCreateA" );
	if( DirectInputCreate == 0 )
	{
		// No DInput... must be DX2
		FreeLibrary( DIHinst );
		FreeLibrary( DDHinst );
		pDDraw->Release();
		OutputDebugString( "Couldn't GetProcAddress DInputCreate\r\n" );
		return;
	}

	// DirectInputCreate exists. We are at least DX3
	(*pdwDXVersion) = 0x300;
	FreeLibrary( DIHinst );

	// Can do checks for 3a vs 3b here


	///////////////////////////////////////////////////////////////////////////
	// DirectX 5.0 Checks
	///////////////////////////////////////////////////////////////////////////

	// We can tell if DX5 is present by checking for the existence of
	// IDirectDrawSurface3. First, we need a surface to QI off of.
	DDSURFACEDESC ddsd;
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize         = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	hr = pDDraw->SetCooperativeLevel( NULL, DDSCL_NORMAL );
	if( FAILED(hr) )
	{
		// Failure. This means DDraw isn't properly installed.
		pDDraw->Release();
		FreeLibrary( DDHinst );
		(*pdwDXVersion) = 0;
		OutputDebugString( "Couldn't Set coop level\r\n" );
		return;
	}

	hr = pDDraw->CreateSurface( &ddsd, &pSurf, NULL );
	if( FAILED(hr) )
	{
		// Failure. This means DDraw isn't properly installed.
		pDDraw->Release();
		FreeLibrary( DDHinst );
		*pdwDXVersion = 0;
		OutputDebugString( "Couldn't CreateSurface\r\n" );
		return;
	}

	// Query for the IDirectDrawSurface3 interface
	if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface3,
									   (VOID**)&pSurf3 ) ) )
	{
		pDDraw->Release();
		FreeLibrary( DDHinst );
		return;
	}

	// QI for IDirectDrawSurface3 succeeded. We must be at least DX5
	(*pdwDXVersion) = 0x500;


	///////////////////////////////////////////////////////////////////////////
	// DirectX 6.0 Checks
	///////////////////////////////////////////////////////////////////////////

	// The IDirectDrawSurface4 interface was introduced with DX 6.0
	if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface4,
									   (VOID**)&pSurf4 ) ) )
	{
		pDDraw->Release();
		FreeLibrary( DDHinst );
		return;
	}

	// IDirectDrawSurface4 was create successfully. We must be at least DX6
	(*pdwDXVersion) = 0x600;
	pSurf->Release();
	pDDraw->Release();


	///////////////////////////////////////////////////////////////////////////
	// DirectX 6.1 Checks
	///////////////////////////////////////////////////////////////////////////

	// Check for DMusic, which was introduced with DX6.1
	LPDIRECTMUSIC pDMusic = NULL;
	CoInitialize( NULL );
	hr = CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
						   IID_IDirectMusic, (VOID**)&pDMusic );
	if( FAILED(hr) )
	{
		OutputDebugString( "Couldn't create CLSID_DirectMusic\r\n" );
		FreeLibrary( DDHinst );
		return;
	}

	// DirectMusic was created successfully. We must be at least DX6.1
	(*pdwDXVersion) = 0x601;
	pDMusic->Release();
	CoUninitialize();

*/
	///////////////////////////////////////////////////////////////////////////
	// DirectX 7.0 Checks
	///////////////////////////////////////////////////////////////////////////
	DDHinst = LoadLibrary( "DDRAW.DLL" );
	(*pdwDXVersion) = 0x000;
	

	// Check for DirectX 7 by creating a DDraw7 object
	LPDIRECTDRAW7 pDD7;
	DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( DDHinst,
													   "DirectDrawCreateEx" );
	if( NULL == DirectDrawCreateEx )
	{
		FreeLibrary( DDHinst );
		return;
	}

	if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
									NULL ) ) )
	{
		FreeLibrary( DDHinst );
		return;
	}

	// DDraw7 was created successfully. We must be at least DX7.0
	(*pdwDXVersion) = 0x700;
	pDD7->Release();


	///////////////////////////////////////////////////////////////////////////
	// End of checks
	///////////////////////////////////////////////////////////////////////////

	// Close open libraries and return
	FreeLibrary( DDHinst );

	return;
}


void CPatnDoc::OnView3dgraphDirectx70a() 
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	
	long found = false;
	long foundLegend = false;
	if(m_OrdVisible || m_OrdXVisible) 
	{
		//AfxGetMainWnd()->MessageBox("Only one 3D Graph can exist \n at a time.","Message");
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(//view->IsKindOf(RUNTIME_CLASS(C3DView)) ||
				view->IsKindOf(RUNTIME_CLASS(C3DXView)) && ((C3DXView*)view)->m_legend_view != (void*)1){// ||
				//view->IsKindOf(RUNTIME_CLASS(CScat)) ){
				view->GetParentFrame()->BringWindowToTop();
				//view->GetParentFrame()->ShowWindow(SW_RESTORE );
				found = true;
			}
				if(//view->IsKindOf(RUNTIME_CLASS(C3DView)) ||
				view->IsKindOf(RUNTIME_CLASS(C3DXView)) && ((C3DXView*)view)->m_legend_view == (void*)1){// ||
				//view->IsKindOf(RUNTIME_CLASS(CScat)) ){
				view->GetParentFrame()->BringWindowToTop();
				//view->GetParentFrame()->ShowWindow(SW_RESTORE );
				foundLegend = true;
				break;
			}
		}
		if(found && foundLegend) {
			//close legend

			

			return;
		}
	}


	DWORD version,platform;
	GetDXVersion( &version,&platform );
	CString str;
	switch( version )
	{
		case 0x000:
			str = "None";
			break;
		case 0x100:
			str = "1";
			break;
		case 0x200:
			str = "2";
			break;
		case 0x300:
			str = "3";
			break;
		case 0x500:
			str = "5";
			break;
		case 0x600:
			str = "6";
			break;
		case 0x601:
			str = "6.1";
			break;
		case 0x700:
			str = "7";
			break;
		default:
			str = "unknown";
			break;
	}
	if(version < 0x700){
		//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("DirectX 7 or better is required to use this view.\n\r\n\rUsing OpenGL instead.");
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("DirectX7 not supported.");
		//pApp->SetOrdType(0);
	}else{
		pApp->Open3DXView(this);
	}
	
}

void CPatnDoc::OnUpdateView3dgraphDirectx70a(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_aRowXCoords[0] != ANALYSIS_VALUE && (!m_OrdChange || m_import_ord));		
}

void CPatnDoc::RetrieveViewContents()
{
	POSITION pos = GetFirstViewPosition();
	pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		pView->RedrawWindow();
	}
	
	CRect nrect;
	AfxGetMainWnd()->GetWindowRect(nrect);
	
	pos = GetFirstViewPosition();

	int id,mi;

	while(pos != NULL){
		CView* pView = GetNextView(pos);pView->GetParentFrame()->BringWindowToTop();   
		//pView->GetParentFrame()->ShowWindow(SW_RESTORE );
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
			CTableView * pv = (CTableView*)pView;
			
			//pView->GetParentFrame()->GetWindowRect(mr_tableview);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.GetRowInfo(0,id,mi);
			mr_tableview.left = mr_tableview.top = 0;
			mr_tableview.bottom = id;
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter2.GetColumnInfo(0,id,mi);
			mr_tableview.right = id;

			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.GetRowInfo(1,id,mi);
			mr_coltotals.top = 0;
			mr_coltotals.bottom = id;

			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter2.GetColumnInfo(1,id,mi);
			mr_rowtotals.left = 0;
			mr_rowtotals.right = id;

			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_tableview.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_tableview.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_tableview.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_tableview.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			

			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_tableview);
			
//		}else if(pView->IsKindOf(RUNTIME_CLASS(CGstaView))){
			
	//		CGstaView * pv = (CGstaView*)pView;
	//		pView->GetParentFrame()->GetWindowRect(mr_gsta);
			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_gsta.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_gsta.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_gsta.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_gsta.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			

		//	pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_gstaview);
			
		}else if(pView->IsKindOf(RUNTIME_CLASS(CTway))){
			CTway * pv = (CTway*) pView;
			pView->GetParentFrame()->GetWindowRect(mr_tway);
			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_tway.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_tway.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_tway.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_tway.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			
			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_tway);

/*		}else if(pView->IsKindOf(RUNTIME_CLASS(C3DView))){
			C3DView * pv = (C3DView*)pView;
			pView->GetParentFrame()->GetWindowRect(mr_ord);
			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_ord.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_ord.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			

			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_3dview);

			//GetWindowRect(mr_ord);

			m_Rotate = pv->m_Rotate;
			m_Labels = pv->m_Labels;
			m_Legend = pv->m_Legend;
			m_Mst = pv->m_Mst;
			m_Axies = pv->m_Axies;
			m_Grouping = pv->m_Grouping;
			m_Usergroupcolours = pv->m_Usergroupcolours;
			m_SelOnly = pv->m_SelOnly;
			m_Centroid = pv->m_Centroid;
			m_Showranges = pv->m_Showranges;
			m_Symbols = pv->m_Symbols;
			m_RotationSpeed = pv->m_RotationSpeed;
			for(long i=0;i<m_TextBoxes.GetSize();i++)
				delete m_TextBoxes[i];
			m_TextBoxes.RemoveAll();
			for(i=0;i<pv->m_TextBoxes.GetSize();i++){
				m_TextBoxes.Add(new TextBox);
				m_TextBoxes[i]->contents = pv->m_TextBoxes[i]->contents;
				m_TextBoxes[i]->size = pv->m_TextBoxes[i]->size;
				m_TextBoxes[i]->location = pv->m_TextBoxes[i]->location;
			}
			wAngleY = pv->wAngleY; 
			m_NoSymbols = pv->m_NoSymbols;
			m_PccVectors.RemoveAll();
			for(i=0;i<pv->m_PccVectors.GetSize();i++){
				m_PccVectors.Add(pv->m_PccVectors[i]);
			}

		}else if(pView->IsKindOf(RUNTIME_CLASS(CScat))){
			CScat * pv = (CScat*)pView;
			pView->GetParentFrame()->GetWindowRect(mr_ord);
			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_ord.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_ord.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			
			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_3dview);

			m_Rotate = pv->m_Rotate;
			m_Labels = pv->m_Labels;
			m_Legend = pv->m_Legend;
			m_Mst = pv->m_Mst;
			m_Axies = pv->m_Axies;
			m_Grouping = pv->m_Grouping;
			m_Usergroupcolours = pv->m_Usergroupcolours;
			m_SelOnly = pv->m_SelOnly;
			m_Centroid = pv->m_Centroid;
			m_Showranges = pv->m_Showranges;
			m_Symbols = pv->m_Symbols;
			for(long i=0;i<m_TextBoxes.GetSize();i++)
				delete m_TextBoxes[i];
			m_TextBoxes.RemoveAll();
			for(i=0;i<pv->m_TextBoxes.GetSize();i++){
				m_TextBoxes.Add(new TextBox);
				m_TextBoxes[i]->contents = pv->m_TextBoxes[i]->contents;
				m_TextBoxes[i]->size = pv->m_TextBoxes[i]->size;
				m_TextBoxes[i]->location = pv->m_TextBoxes[i]->location;
			}
			m_NoSymbols = pv->m_NoSymbols;
			m_PccVectors.RemoveAll();
			for(i=0;i<pv->m_PccVectors.GetSize();i++){
				m_PccVectors.Add(pv->m_PccVectors[i]);
			}*/
		}else if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)) && !((C3DXView*)pView)->m_legend_instance){
			C3DXView * pv = (C3DXView*)pView;
			pView->GetParentFrame()->GetWindowRect(mr_ord);
			//CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_ord.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_ord.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_ord.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			

			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_3dview);

			//GetWindowRect(mr_ord);

			m_Rotate = pv->m_Rotate;
			m_Labels = pv->m_Labels;
			m_Legend = pv->m_Legend;
			m_Mst = pv->m_Mst;
			m_Axies = pv->m_Axies;
			m_Grouping = pv->m_Grouping;
			m_Usergroupcolours = pv->m_Usergroupcolours;
			m_SelOnly = pv->m_SelOnly;
			m_Centroid = pv->m_Centroid;
			m_Showranges = pv->m_Showranges;
			m_Symbols = pv->m_Symbols;
			m_RotationSpeed = pv->m_RotationSpeed;
			for(long i=0;i<m_TextBoxes2.GetSize();i++)
				delete m_TextBoxes2[i];
			m_TextBoxes2.RemoveAll();
			for(i=0;i<pv->m_TextBoxes.GetSize();i++){
				m_TextBoxes2.Add(new TextBox2);
				m_TextBoxes2[i]->contents = pv->m_TextBoxes[i]->contents;
				m_TextBoxes2[i]->location = pv->m_TextBoxes[i]->location;
				memcpy(&m_TextBoxes2[i]->font,&(pv->m_TextBoxes[i]->font),sizeof(LOGFONT));
				strcpy(m_TextBoxes2[i]->font.lfFaceName,pv->m_TextBoxes[i]->font.lfFaceName);
				m_TextBoxes2[i]->colour = pv->m_TextBoxes[i]->colour;

			}
			m_stressbox.contents = pv->m_stressbox.contents;
			m_stressbox.location = pv->m_stressbox.location;
			m_stressbox.colour = pv->m_stressbox.colour;
			memcpy(&m_stressbox.font,&(pv->m_stressbox.font),sizeof(LOGFONT));
			strcpy(m_stressbox.font.lfFaceName,pv->m_stressbox.font.lfFaceName);		

			m_vEyePt.x = pv->m_vEyePt.x;m_vEyePt.y = pv->m_vEyePt.y;m_vEyePt.z = pv->m_vEyePt.z;
			m_vUpVec.x = pv->m_vUpVec.x;m_vUpVec.y = pv->m_vUpVec.y;m_vUpVec.z = pv->m_vUpVec.z;
			m_vRight.x = pv->m_vRight.x;m_vRight.y = pv->m_vRight.y;m_vRight.z = pv->m_vRight.z;

			//wAngleY = wAngleY; 
			m_NoSymbols = pv->m_NoSymbols;
			m_PccVectors.RemoveAll();
			for(i=0;i<pv->m_PccVectors.GetSize();i++){
				m_PccVectors.Add(pv->m_PccVectors[i]);
			}
		}else if(pView->IsKindOf(RUNTIME_CLASS(CBoxWhiskerView))){
			CBoxWhiskerView * pv = (CBoxWhiskerView*)pView;
			pView->GetParentFrame()->GetWindowRect(mr_boxwhisker);
			CRect nrect,nrect2;
			//AfxGetMainWnd()->GetWindowRect(nrect);
			//AfxGetMainWnd()->GetClientRect(nrect2);
			//mr_boxwhisker.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_boxwhisker.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
			//mr_boxwhisker.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			//mr_boxwhisker.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
			
			pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_boxwhisker);

		}else if(pView->IsKindOf(RUNTIME_CLASS(CAnosimView))){
			CAnosimView* pv = (CAnosimView*)pView;
			if(pv->m_columns){
				pView->GetParentFrame()->GetWindowRect(mr_anosimcol);
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_anosimcol.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_anosimcol.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_anosimcol.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_anosimcol.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_anosimcol);
			}else {
				pView->GetParentFrame()->GetWindowRect(mr_anosimrow);
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_anosimrow.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_anosimrow.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_anosimrow.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_anosimrow.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_anosimrow);
			}
		}else if(pView->IsKindOf(RUNTIME_CLASS(CAssocMatrixView))){
			CAssocMatrixView * pv = (CAssocMatrixView*)pView;
			if(pv->m_pMatrix == &m_AssocMatrixRows){
				//pView->GetParentFrame()->GetWindowRect(mr_assocrowmx);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.GetColumnInfo(0,id,mi);
				mr_assocrowmx.left = 0;
				mr_assocrowmx.right= id;
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_assocrowmx.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_assocrowmx.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_assocrowmx.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_assocrowmx.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_assocrow);
			}else if(pv->m_pMatrix == &m_AssocMatrixCols){
				//pView->GetParentFrame()->GetWindowRect(mr_assoccolmx);
				((CAssocSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.GetColumnInfo(0,id,mi);
				mr_assoccolmx.left = 0;
				mr_assoccolmx.right= id;
				
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_assoccolmx.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_assoccolmx.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_assoccolmx.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_assoccolmx.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_assoccol);
			}

		}else if(pView->IsKindOf(RUNTIME_CLASS(CFuseView))){
			CFuseView * pv = (CFuseView*)pView;
			if(pv->m_pFuseTable == &m_FuseTableRows){
				//pView->GetParentFrame()->GetWindowRect(mr_fuserow);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.GetColumnInfo(0,id,mi);
				mr_fuserow.left = 0;
				mr_fuserow.right = id;
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_fuserow.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_fuserow.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_fuserow.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_fuserow.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_fuserow);

			}else if(pv->m_pFuseTable == &m_FuseTableCols){
				//pView->GetParentFrame()->GetWindowRect(mr_fusecol);
				((CFuseSplitterWnd*)pView->GetParentFrame())->m_wndSplitter.GetColumnInfo(0,id,mi);
				mr_fusecol.left = 0;
				mr_fusecol.right = id;
				//CRect nrect,nrect2;
				//AfxGetMainWnd()->GetWindowRect(nrect);
				//AfxGetMainWnd()->GetClientRect(nrect2);
				//mr_fusecol.left -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_fusecol.right -= (nrect.left + nrect.Width()-nrect2.Width() - 2);
				//mr_fusecol.top -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				//mr_fusecol.bottom -= (nrect.top + nrect.Height()-nrect2.Height() + 28);
				
				pView->GetParentFrame()->GetWindowPlacement(&m_wndplace_fusecol);
			}
			
		}else if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView))){
		//	pView->GetWindowRect(mr_coltotals);

			pView->GetWindowPlacement(&m_wndplace_coltotals);
		}else if(pView->IsKindOf(RUNTIME_CLASS(CRowTotalsView))){
		//	pView->GetWindowRect(mr_rowtotals);
			pView->GetWindowPlacement(&m_wndplace_rowtotals);
		}else if(pView->IsKindOf(RUNTIME_CLASS(CAssocHistogram))){
			CAssocHistogram * pv = (CAssocHistogram*) pView;
			if(pv->m_pMatrix == &m_AssocMatrixRows){
				pView->GetWindowRect(mr_assocrowhist);
				pView->GetWindowPlacement(&m_wndplace_histrow);
			}else{
				pView->GetWindowRect(mr_assoccolhist);
				pView->GetWindowPlacement(&m_wndplace_histcol);
			}
		}else if(pView->IsKindOf(RUNTIME_CLASS(CDendView))){
			CDendView * pv = (CDendView*)pView;
			if(pv->m_pFuseTable == &m_FuseTableRows){
				pView->GetWindowRect(mr_fuserowdend);		
				pView->GetWindowPlacement(&m_wndplace_dendrow);
			}else if(pv->m_pFuseTable == &m_FuseTableCols){
				pView->GetWindowRect(mr_fusecoldend);
				pView->GetWindowPlacement(&m_wndplace_dendcol);
			}
		}
	}		
}
void CPatnDoc::SerializeV2_18(CArchive& ar,double version)
{
	if(version < 2.18){
		SerializeV2_17(ar,version);

//		SetDefaultFont3d();
		
		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	m_aColumnSums.Serialize(ar);
	m_aColumnCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMinimum.Serialize(ar);
	}else{
		m_aColumnMinimum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMinimum.GetSize();i++){
			m_aColumnMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) < m_aColumnMinimum[i] || m_aColumnMinimum[i] == m_nMissingValue)
					m_aColumnMinimum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnMean.Serialize(ar);
	if(version >= 2.11){
		m_aColumnMaximum.Serialize(ar);
	}else{
		m_aColumnMaximum.SetSize(m_aData[0]->GetSize());
		for(i=0;i<m_aColumnMaximum.GetSize();i++){
			m_aColumnMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData.GetSize();j++){
				if(GetValueAt(i,j) > m_aColumnMaximum[i] || m_aColumnMaximum[i] == m_nMissingValue)
					m_aColumnMaximum[i] = GetValueAt(i,j);
			}
		}
	}
	m_aColumnCountMissingValues.Serialize(ar);
	m_aColumnGroups.Serialize(ar);

	m_aRowSums.Serialize(ar);
	m_aRowCountPositives.Serialize(ar);
	if(version >= 2.11){
		m_aRowMinimum.Serialize(ar);
	}else{
		m_aRowMinimum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMinimum.GetSize();i++){
			m_aRowMinimum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) < m_aRowMinimum[i] || m_aRowMinimum[i] == m_nMissingValue)
					m_aRowMinimum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowMean.Serialize(ar);
	if(version >= 2.11){
		m_aRowMaximum.Serialize(ar);
	}else{
		m_aRowMaximum.SetSize(m_aData.GetSize());
		for(i=0;i<m_aRowMaximum.GetSize();i++){
			m_aRowMaximum[i] = m_nMissingValue;
			for(j=0;j<m_aData[0]->GetSize();j++){
				if(GetValueAt(j,i) > m_aRowMaximum[i] || m_aRowMaximum[i] == m_nMissingValue)
					m_aRowMaximum[i] = GetValueAt(j,i);
			}
		}
	}
	m_aRowCountMissingValues.Serialize(ar);
	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	CRowAttributes * attribs;
	CColumnAttributes * cattribs;
	if(version < 2.11){
		attribs = new CRowAttributes;
		attribs->SetLabel("Minimum");
		attribs->SetType(CRowColumnAttributes::Minimum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(1,attribs);
	
		m_aColumnTotalsRowAttributes[2]->SetLabel("Mean");
		m_aColumnTotalsRowAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		attribs = new CRowAttributes;
		attribs->SetLabel("Maximum");
		attribs->SetType(CRowColumnAttributes::Maximum);
		attribs->SetHeight(-1);
		m_aColumnTotalsRowAttributes.InsertAt(3,attribs);
	}
	m_aRowTotalsColumnAttributes.Serialize(ar);
	if(version < 2.11){
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Minimum");
		cattribs->SetType(CRowColumnAttributes::Minimum);
		m_aRowTotalsColumnAttributes.InsertAt(1,cattribs);
	
		m_aRowTotalsColumnAttributes[2]->SetLabel("Mean");
		m_aRowTotalsColumnAttributes[2]->SetType(CRowColumnAttributes::Mean);
	
		cattribs = new CColumnAttributes;
		cattribs->SetLabel("Maximum");
		cattribs->SetType(CRowColumnAttributes::Maximum);
		m_aRowTotalsColumnAttributes.InsertAt(3,cattribs);
	}
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);
	m_KWVisible.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(version >= 2.12){
		if(ar.IsStoring()){
			ar << m_MCAOTable.GetSize();
			for(i=0;i<m_MCAOTable.GetSize();i++){
				ar << m_MCAOTable[i]->GetSize();
				for(j=0;j<m_MCAOTable[i]->GetSize();j++)
					ar << m_MCAOTable[i]->GetAt(j);
			}
		}else{
			ar >> nCount;m_MCAOTable.SetSize(nCount);
			for(i=0;i<m_MCAOTable.GetSize();i++){
				m_MCAOTable[i] = new CArray<double>;
				ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
				for(j=0;j<m_MCAOTable[i]->GetSize();j++){
					ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
				}
			}
		}	
	}

	long nSize;

	if (ar.IsStoring())
	{
		
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		LOGFONT tmpfont;
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		if(version >= 2.16){
			ar.Read(&tmpfont,sizeof(LOGFONT));
			ar.Read(&tmpfont,sizeof(LOGFONT));
			ar.Read(&tmpfont,sizeof(LOGFONT));
			ar.Read(&tmpfont,sizeof(LOGFONT));
		}else{
			//SetDefaultFontTway();
			//SetDefaultFontGsta();
			//SetDefaultFontBW();
			//SetDefaultFont3d();
		}

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		if(version >= 2.111){
			ar >> m_RowAsim_stat;
			ar >> m_RowAsim_rbest;
			ar >> m_RowAsim_save;
			ar >> m_RowAsim_iterations;
			ar >> m_ColumnAsim_stat;
			ar >> m_ColumnAsim_rbest;
			ar >> m_ColumnAsim_save;
			ar >> m_ColumnAsim_iterations;
		}
		if(version >= 2.12){
			ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nAssocMeasure;
			ar >> m_AnalysisRecipe.cols.nBeta;
			ar >> m_AnalysisRecipe.cols.nClassStrategy;
			ar >> m_AnalysisRecipe.cols.nCut;
			ar >> m_AnalysisRecipe.cols.nFuseTechnique;
			ar >> m_AnalysisRecipe.cols.nMinkConst;
			ar >> m_AnalysisRecipe.cols.nNoOfGroups;
			ar >> m_AnalysisRecipe.cols.nOrd3d;
			ar >> m_AnalysisRecipe.cols.nOrdination;
			ar >> m_AnalysisRecipe.cols.nOrdIterations;
			ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.cols.nOrdSeed;
			ar >> m_AnalysisRecipe.nFirstTime;
			ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nAssocMeasure;
			ar >> m_AnalysisRecipe.rows.nBeta;
			ar >> m_AnalysisRecipe.rows.nClassStrategy;
			ar >> m_AnalysisRecipe.rows.nCut;
			ar >> m_AnalysisRecipe.rows.nFuseTechnique;
			ar >> m_AnalysisRecipe.rows.nMinkConst;
			ar >> m_AnalysisRecipe.rows.nNoOfGroups;
			ar >> m_AnalysisRecipe.rows.nOrd3d;
			ar >> m_AnalysisRecipe.rows.nOrdination;
			ar >> m_AnalysisRecipe.rows.nOrdIterations;
			ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
			ar >> m_AnalysisRecipe.rows.nOrdSeed;
		}else{
			m_AnalysisRecipe.nFirstTime = true;
		}
		if(version >= 2.15){
			ar >> i;m_pGstaVisible = (i)?true:false;
			ar >> m_AssocRowVisible;
			ar >> m_AssocColVisible;
			ar >> m_AssocRowUpdate;
			ar >> m_AssocColUpdate;
			ar >> m_FuseRowVisible;
			ar >> m_FuseColVisible;
			ar >> m_FuseRowUpdate;
			ar >> m_FuseColUpdate;
			ar >> m_nScale;
			ar >> m_pExists;
			ar >> m_pIsRowLabel;
			ar >> m_colstand;
			ar >> m_presabs;
			ar >> m_globstand;
			ar >> m_red;
			ar >> m_green;
			ar >> m_blue;
			ar >> m_bw;
			ar >> i;m_AnosimPairView = (i)?true:false;
			ar >> i;m_AnosimColView = (i)?true:false; 
			ar >> i;m_AnosimRowView = (i)?true:false; 
			ar >> i;m_OrdVisible = (i)?true:false;
			ar >> i;m_OrdXVisible = (i)?true:false;
			ar >> i;m_OrdUpdate = (i)?true:false;
			ar >> m_boxwhiskervisible;
			ar >> mr_tableview;
			ar >> mr_rowtotals;
			ar >> mr_coltotals;
			ar >> mr_anosimrow;
			ar >> mr_anosimcol;
			ar >> mr_fuserow;
			ar >> mr_fusecol;
			ar >> mr_fuserowdend;
			ar >> mr_fusecoldend;
			ar >> mr_assocrowmx;
			ar >> mr_assoccolmx;
			ar >> mr_assocrowhist;
			ar >> mr_assoccolhist;
			ar >> mr_ord;
			ar >> mr_tway;
			ar >> mr_gsta;
			ar >> mr_boxwhisker;

			ar >> i;m_Rotate = (i)?true:false;
			ar >> i;m_Labels = (i)?true:false;
			ar >> i;m_Legend = (i)?true:false;
			ar >> i;m_Mst = (i)?true:false;
			ar >> i;m_Axies = (i)?true:false;
			ar >> i;m_Grouping = (i)?true:false;
			ar >> i;m_Usergroupcolours = (i)?true:false;
			ar >> i;m_SelOnly = (i)?true:false;
			ar >> i;m_Centroid = (i)?true:false;
			ar >> i;m_Showranges = (i)?true:false;
			ar >> i;m_Symbols = (i)?true:false;
			ar >> m_RotationSpeed;
			long size;
			ar >> size;m_TextBoxes.SetSize(size);
			for(i=0;i<m_TextBoxes.GetSize();i++){
				m_TextBoxes[i] = new TextBox;
				ar >> m_TextBoxes[i]->contents;
				ar >> m_TextBoxes[i]->size;
				ar >> m_TextBoxes[i]->location;
			}			
			ar >> size;m_TextBoxes2.SetSize(size);
			for(i=0;i<m_TextBoxes2.GetSize();i++){
				m_TextBoxes2[i] = new TextBox2;
				ar >> m_TextBoxes2[i]->contents;
				ar >> m_TextBoxes2[i]->location;
				ar >> m_TextBoxes2[i]->colour;
				ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
			}			
			ar >> m_stressbox.contents;
			ar >> m_stressbox.location;
			ar >> m_stressbox.colour;
			ar.Read(&m_stressbox.font,sizeof(LOGFONT));
			
			ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
			ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
			ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

			ar >> wAngleY; 
			ar >> i;m_NoSymbols = (i)?true:false;
			ar >> size;m_PccVectors.SetSize(size);
			for(i=0;i<m_PccVectors.GetSize();i++){
				ar >> m_PccVectors[i];
			}

			ar >> nCount;
			m_aGroupColours2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_aGroupColours2[i].red;
				ar >> m_aGroupColours2[i].green;
				ar >> m_aGroupColours2[i].blue;			
			}
			ar >> nCount;
			m_aObjectColours2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_aObjectColours2[i].red;
				ar >> m_aObjectColours2[i].green;
				ar >> m_aObjectColours2[i].blue;			
			}
			ar >> nCount;
			m_r.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_r[i].red;
				ar >> m_r[i].green;
				ar >> m_r[i].blue;			
			}
			ar >> nCount;
			m_Vectors2.SetSize(nCount);			
			for ( i = 0; i < nCount; i++ )
			{
				ar >> m_Vectors2[i].red;
				ar >> m_Vectors2[i].green;
				ar >> m_Vectors2[i].blue;			
			}
			m_3ddefault = true;

			//2.18 (some is above)
			POSITION pos = this->GetFirstViewPosition();
			CView * pView;
			while(pos != NULL){
				pView = this->GetNextView(pos);
				if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
					break;
				}
				pView = NULL;
			}

			ar >> nCount;
			for(i=0;i<nCount;i++){
				ar >> j;
				if(pView != NULL)
					((CTableView*)pView)->m_pSelectedRows->SetAt(j);
			}	
			ar >> nCount;
			for(i=0;i<nCount;i++){
				ar >> j;
				if(pView != NULL)
					((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
			}	

		}else{
			m_3ddefault = false;
			m_AssocRowVisible = 0;
			m_AssocColVisible = 0;
			m_AssocRowUpdate = 0;
			m_AssocColUpdate = 0;
			m_FuseRowVisible = 0;
			m_FuseColVisible = 0;
			m_FuseRowUpdate = 0;
			m_FuseColUpdate = 0;
			m_nScale = 0;
			m_pExists = 0;
			m_pIsRowLabel = 0;
			m_colstand = 0;
			m_presabs = 0;
			m_globstand = 0;
			m_red = 0;
			m_green = 0;
			m_blue = 0;
			m_bw = 0;
			m_AnosimPairView = 0;
			m_AnosimColView = 0; 
			m_AnosimRowView = 0; 
			m_OrdVisible = 0;
			m_OrdXVisible = 0;
			m_OrdUpdate = 0;
			m_boxwhiskervisible = 0;
		}
		
		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)
			CGsta cgsta(this);

		//SetupWindowsAfterLoad();
			m_pExists = 0;
		m_FuseRowVisible = 0;
		m_FuseColVisible = 0;
		m_AssocRowVisible = 0;
		m_AssocColVisible = 0;
		m_pGstaVisible = 0;
		m_boxwhiskervisible = 0;
		m_OrdVisible = 0;
		m_OrdXVisible = 0;
		m_AnosimRowView = 0;
		m_AnosimColView = 0;
	}
}


void CPatnDoc::SerializeV2_19(CArchive& ar,double version)
{
	//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//update visible window rect regions
//	POSITION pos = this->GetFirstViewPosition();
//	while(pos){
//		CView * v = this->GetNextView(pos);
//		v->RedrawWindow();
//	}
//	if(ar.IsStoring())
//		RetrieveViewContents();

	//double version = 2.19;
	//if(ar.IsStoring())
		//ar << (double) 2.09; //current version number
	//	ar << version;
	//else
	//	ar >> version;

	if(version < 2.19){
		SerializeV2_18(ar,version);

//		SetDefaultFont3d();
		
		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	//m_aColumnSums.Serialize(ar);
	//.Serialize(ar);
	//if(version >= 2.11){
	//	m_aColumnMinimum.Serialize(ar);
	//}else{
//		m_aColumnMinimum.SetSize(m_aData[0]->GetSize());
///		for(i=0;i<m_aColumnMinimum.GetSize();i++){
//			m_aColumnMinimum[i] = m_nMissingValue;
//			for(j=0;j<m_aData.GetSize();j++){
//				if(GetValueAt(i,j) < m_aColumnMinimum[i] || m_aColumnMinimum[i] == m_nMissingValue)
//					[i] = GetValueAt(i,j);
//			}
//		}
//	}
//	.Serialize(ar);
//	if(version >= 2.11){
//		m_aColumnMaximum.Serialize(ar);
//	}else{
//		m_aColumnMaximum.SetSize(m_aData[0]->GetSize());
//		for(i=0;i<m_aColumnMaximum.GetSize();i++){
//			m_aColumnMaximum[i] = m_nMissingValue;
//			for(j=0;j<m_aData.GetSize();j++){
//				if(GetValueAt(i,j) > m_aColumnMaximum[i] || m_aColumnMaximum[i] == m_nMissingValue)
//					[i] = GetValueAt(i,j);
//			}
//		}
//	}
//	m_aColumnCountMissingValues.Serialize(ar);
	

	m_aColumnGroups.Serialize(ar);

//	m_aRowSums.Serialize(ar);
//	m_aRowCountPositives.Serialize(ar);
//	if(version >= 2.11){
//		m_aRowMinimum.Serialize(ar);
//	}else{
//		m_aRowMinimum.SetSize(m_aData.GetSize());
//		for(i=0;i<m_aRowMinimum.GetSize();i++){
//			m_aRowMinimum[i] = m_nMissingValue;
//			for(j=0;j<m_aData[0]->GetSize();j++){
//				if(GetValueAt(j,i) < m_aRowMinimum[i] || m_aRowMinimum[i] == m_nMissingValue)
//					m_aRowMinimum[i] = GetValueAt(j,i);
//			}
//		}
//	}
//	m_aRowMean.Serialize(ar);
//	if(version >= 2.11){
//		m_aRowMaximum.Serialize(ar);
//	}else{
//		m_aRowMaximum.SetSize(m_aData.GetSize());
//		for(i=0;i<m_aRowMaximum.GetSize();i++){
//			m_aRowMaximum[i] = m_nMissingValue;
//			for(j=0;j<m_aData[0]->GetSize();j++){
//				if(GetValueAt(j,i) > m_aRowMaximum[i] || m_aRowMaximum[i] == m_nMissingValue)
//					m_aRowMaximum[i] = GetValueAt(j,i);
//			}
//		}
//	}
//	m_aRowCountMissingValues.Serialize(ar);
	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	
	m_aRowTotalsColumnAttributes.Serialize(ar);
	
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);
//	m_KWVisible.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(ar.IsStoring()){
		ar << m_MCAOTable.GetSize();
		for(i=0;i<m_MCAOTable.GetSize();i++){
			ar << m_MCAOTable[i]->GetSize();
			for(j=0;j<m_MCAOTable[i]->GetSize();j++)
				ar << m_MCAOTable[i]->GetAt(j);
		}
	}else{
		ar >> nCount;m_MCAOTable.SetSize(nCount);
		for(i=0;i<m_MCAOTable.GetSize();i++){
			m_MCAOTable[i] = new CArray<double>;
			ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
			}
		}
	}
	
	m_aColumnSums.SetSize(GetNoOfColumns());
	m_aColumnCountMissingValues.SetSize(GetNoOfColumns());
	m_aColumnMaximum.SetSize(GetNoOfColumns());
	m_aColumnMinimum.SetSize(GetNoOfColumns());
	m_aColumnCountPositives.SetSize(GetNoOfColumns());
	m_aColumnMean.SetSize(GetNoOfColumns());
	
	m_aRowSums.SetSize(GetNoOfRows());
	m_aRowCountMissingValues.SetSize(GetNoOfRows());
	m_aRowMaximum.SetSize(GetNoOfRows());
	m_aRowMinimum.SetSize(GetNoOfRows());
	m_aRowCountPositives.SetSize(GetNoOfRows());
	m_aRowMean.SetSize(GetNoOfRows());
	
	long nSize;

	if (ar.IsStoring())
	{
		
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		LOGFONT tmpfont;
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar >> i;
		ar >> i;

		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar.Read(&tmpfont,sizeof(LOGFONT));
		ar.Read(&tmpfont,sizeof(LOGFONT));
		
		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		ar >> m_RowAsim_stat;
		ar >> m_RowAsim_rbest;
		ar >> m_RowAsim_save;
		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_stat;
		ar >> m_ColumnAsim_rbest;
		ar >> m_ColumnAsim_save;
		ar >> m_ColumnAsim_iterations;
		ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nBeta;
		ar >> m_AnalysisRecipe.cols.nClassStrategy;
		ar >> m_AnalysisRecipe.cols.nCut;
		ar >> m_AnalysisRecipe.cols.nFuseTechnique;
		ar >> m_AnalysisRecipe.cols.nMinkConst;
		ar >> m_AnalysisRecipe.cols.nNoOfGroups;
		ar >> m_AnalysisRecipe.cols.nOrd3d;
		ar >> m_AnalysisRecipe.cols.nOrdination;
		ar >> m_AnalysisRecipe.cols.nOrdIterations;
		ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.cols.nOrdSeed;
		ar >> m_AnalysisRecipe.nFirstTime;
		ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nBeta;
		ar >> m_AnalysisRecipe.rows.nClassStrategy;
		ar >> m_AnalysisRecipe.rows.nCut;
		ar >> m_AnalysisRecipe.rows.nFuseTechnique;
		ar >> m_AnalysisRecipe.rows.nMinkConst;
		ar >> m_AnalysisRecipe.rows.nNoOfGroups;
		ar >> m_AnalysisRecipe.rows.nOrd3d;
		ar >> m_AnalysisRecipe.rows.nOrdination;
		ar >> m_AnalysisRecipe.rows.nOrdIterations;
		ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.rows.nOrdSeed;
		
		ar >> i;m_pGstaVisible = (i)?true:false;
		ar >> m_AssocRowVisible;
		ar >> m_AssocColVisible;
		ar >> m_AssocRowUpdate;
		ar >> m_AssocColUpdate;
		ar >> m_FuseRowVisible;
		ar >> m_FuseColVisible;
		ar >> m_FuseRowUpdate;
		ar >> m_FuseColUpdate;
		ar >> m_nScale;
		ar >> m_pExists;
		ar >> m_pIsRowLabel;
		ar >> m_colstand;
		ar >> m_presabs;
		ar >> m_globstand;
		ar >> m_red;
		ar >> m_green;
		ar >> m_blue;
		ar >> m_bw;
		ar >> i;m_AnosimPairView = (i)?true:false;
		ar >> i;m_AnosimColView = (i)?true:false; 
		ar >> i;m_AnosimRowView = (i)?true:false; 
		ar >> i;m_OrdVisible = (i)?true:false;
		ar >> i;m_OrdXVisible = (i)?true:false;
		ar >> i;m_OrdUpdate = (i)?true:false;
		ar >> m_boxwhiskervisible;
		ar >> mr_tableview;
		ar >> mr_rowtotals;
		ar >> mr_coltotals;
		ar >> mr_anosimrow;
		ar >> mr_anosimcol;
		ar >> mr_fuserow;
		ar >> mr_fusecol;
		ar >> mr_fuserowdend;
		ar >> mr_fusecoldend;
		ar >> mr_assocrowmx;
		ar >> mr_assoccolmx;
		ar >> mr_assocrowhist;
		ar >> mr_assoccolhist;
		ar >> mr_ord;
		ar >> mr_tway;
		ar >> mr_gsta;
		ar >> mr_boxwhisker;

		ar >> i;m_Rotate = (i)?true:false;
		ar >> i;m_Labels = (i)?true:false;
		ar >> i;m_Legend = (i)?true:false;
		ar >> i;m_Mst = (i)?true:false;
		ar >> i;m_Axies = (i)?true:false;
		ar >> i;m_Grouping = (i)?true:false;
		ar >> i;m_Usergroupcolours = (i)?true:false;
		ar >> i;m_SelOnly = (i)?true:false;
		ar >> i;m_Centroid = (i)?true:false;
		ar >> i;m_Showranges = (i)?true:false;
		ar >> i;m_Symbols = (i)?true:false;
		ar >> m_RotationSpeed;
		long size;
		ar >> size;m_TextBoxes.SetSize(size);
		for(i=0;i<m_TextBoxes.GetSize();i++){
			m_TextBoxes[i] = new TextBox;
			ar >> m_TextBoxes[i]->contents;
			ar >> m_TextBoxes[i]->size;
			ar >> m_TextBoxes[i]->location;
		}			
		ar >> size;m_TextBoxes2.SetSize(size);
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			m_TextBoxes2[i] = new TextBox2;
			ar >> m_TextBoxes2[i]->contents;
			ar >> m_TextBoxes2[i]->location;
			ar >> m_TextBoxes2[i]->colour;
			ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}			
		ar >> m_stressbox.contents;
		ar >> m_stressbox.location;
		ar >> m_stressbox.colour;
		ar.Read(&m_stressbox.font,sizeof(LOGFONT));
		
		ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
		ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
		ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

		ar >> wAngleY; 
		ar >> i;m_NoSymbols = (i)?true:false;
		ar >> size;m_PccVectors.SetSize(size);
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar >> m_PccVectors[i];
		}

		ar >> nCount;
		m_aGroupColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aGroupColours2[i].red;
			ar >> m_aGroupColours2[i].green;
			ar >> m_aGroupColours2[i].blue;			
		}
		ar >> nCount;
		m_aObjectColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aObjectColours2[i].red;
			ar >> m_aObjectColours2[i].green;
			ar >> m_aObjectColours2[i].blue;			
		}
		ar >> nCount;
		m_r.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_r[i].red;
			ar >> m_r[i].green;
			ar >> m_r[i].blue;			
		}
		ar >> nCount;
		m_Vectors2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_Vectors2[i].red;
			ar >> m_Vectors2[i].green;
			ar >> m_Vectors2[i].blue;			
		}
		m_3ddefault = true;

		//2.18 (some is above)
		POSITION pos = this->GetFirstViewPosition();
		CView * pView;
		while(pos != NULL){
			pView = this->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
				break;
			}
			pView = NULL;
		}

		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedRows->SetAt(j);
		}	
		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
		}	

		//2.19
		ar >> i;m_createKW = (i)?true:false;
		ar >> i;m_createANOSIMRows = (i)?true:false;
		ar >> i;m_createANOSIMCols = (i)?true:false;
		ar >> i;m_createMCAO = (i)?true:false;
		ar >> i;m_createPCC = (i)?true:false;


		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)
			CGsta cgsta(this);

		//SetupWindowsAfterLoad();
			m_pExists = 0;
		m_FuseRowVisible = 0;
		m_FuseColVisible = 0;
		m_AssocRowVisible = 0;
		m_AssocColVisible = 0;
		m_pGstaVisible = 0;
		m_boxwhiskervisible = 0;
		m_OrdVisible = 0;
		m_OrdXVisible = 0;
		m_AnosimRowView = 0;
		m_AnosimColView = 0;
	}
}

void CPatnDoc::OnContextmenu() 
{
	//get the top window
	long i = 0;

//	CWnd * wnd = this->GetForegroundWindow();

	POSITION pos = GetFirstViewPosition();   
	while(pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsWindowEnabled()){
			if(pView->IsKindOf(RUNTIME_CLASS(CViewExtension))){
				if(((CViewExtension*)pView)->IsActivated()){
					CViewExtension * ve = (CViewExtension*)pView;
				
					CMenu * m = ve->GetMenu();
					CMenu* pPopup = m->GetSubMenu(0);

					CMainFrame * mf = (CMainFrame*) AfxGetMainWnd();

					CRect r1,r2;
					mf->m_wndToolBar.GetItemRect(
						mf->m_wndToolBar.CommandToIndex(ID_CONTEXTMENU),r1);
					mf->m_wndToolBar.GetWindowRect(r2);

					pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						r1.left + r2.left,r1.bottom+r2.top,AfxGetMainWnd());	

					return;
				}
			}
		}
	}   
//
//	CString strTypeName = "";
//	POSITION pos;
//	 
//	CPatnApp * app = (CPatnApp)AfxGetApp();
//	pos = app->GetFirstDocTemplatePosition();
//	if( pos != NULL )
//	{
//		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
//		
//	}

	//CView * view = this->GetActiveView();
	//if(view != NULL)
	//	view->PostMessage(WM_CONTEXTMENU);
  	
}

void CPatnDoc::OnUpdateContextmenu(CCmdUI* pCmdUI) 
{

	POSITION pos = GetFirstViewPosition();   
	while(pos != NULL)   
	{
		CView* pView = GetNextView(pos); 

		if(pView->IsWindowEnabled()){
			if(pView->IsKindOf(RUNTIME_CLASS(CViewExtension))){
				if(((CViewExtension*)pView)->IsActivated()){
					pCmdUI->Enable(true);
					return;
				}
			}
		}
	}	
	pCmdUI->Enable(false);
}

void CPatnDoc::OnToolsOptions() 
{
	CPropertySheet ps("Options", AfxGetMainWnd());

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;	// remove 'Apply' button

	CPropertiesVisibleStatistics pstats;
	//CPropertiesOrdinationView pordview;
	CPropertiesFont pfont;
	CPropertiesBWObjects pbwobjects;
	CPropertiesGeneral tableproperties;

	pbwobjects.m_pDoc = this;
	pfont.pDoc = this;

	//pordview.m_ordtype = ((CPatnApp*)AfxGetApp())->m_ordtype;

	pstats.m_visible_decimals = m_visible_decimals;
	pstats.m_dec = 2;
	for(long i=0;i<m_aColumnTotalsRowAttributes.GetSize();i++){
		switch (((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->GetType())
		{
			case CRowColumnAttributes::Sum:
				pstats.m_colsum = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Minimum:
				pstats.m_colmin = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Mean:
				pstats.m_colmean = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Maximum:
				pstats.m_colmax = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::CountPositive:
				pstats.m_colno = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::CountMissing:
				pstats.m_colmissing = (!((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->IsHidden());
				break;
		}
	}
	for(i=0;i<m_aRowTotalsColumnAttributes.GetSize();i++){
		switch (((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->GetType())
		{
			case CRowColumnAttributes::Sum:
				pstats.m_rowsum = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Minimum:
				pstats.m_rowmin = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Mean:
				pstats.m_rowmean = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::Maximum:
				pstats.m_rowmax = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::CountPositive:
				pstats.m_rowno = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
			case CRowColumnAttributes::CountMissing:
				pstats.m_rowmissing = (!((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->IsHidden());
				break;
		}
	}
	
	tableproperties.m_tablename = m_TableName;
	tableproperties.m_missingvalue = GetMissingValue();

	ps.AddPage((CPropertyPage*)&tableproperties);
	ps.AddPage((CPropertyPage*)&pstats);
	//ps.AddPage((CPropertyPage*)&pordview);
	ps.AddPage((CPropertyPage*)&pfont);
	ps.AddPage((CPropertyPage*)&pbwobjects);
	
	if(ps.DoModal() == IDOK){
		//((CPatnApp*)AfxGetApp())->SetOrdType(pordview.m_ordtype);
		
		m_TableName = tableproperties.m_tablename;
		SetMissingValue(tableproperties.m_missingvalue);

		if(pstats.m_check_bool){
			for(i=0;i<this->m_aColumnAttributes.GetSize();i++){
				m_aColumnAttributes[i]->SetDecimalPrecision(pstats.m_dec);
			}
		}

		m_visible_decimals = pstats.m_visible_decimals;
		for(i=0;i<m_aColumnTotalsRowAttributes.GetSize();i++){
			switch (((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->GetType())
			{
				case CRowColumnAttributes::Sum:
					if(!pstats.m_colsum)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Minimum:
					if(!pstats.m_colmin)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Mean:
					if(!pstats.m_colmean)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Maximum:
					if(!pstats.m_colmax)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::CountPositive:
					if(!pstats.m_colno)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::CountMissing:
					if(!pstats.m_colmissing)
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aColumnTotalsRowAttributes.GetAt(i))->Unhide();
					break;
			}
		}
		for(i=0;i<m_aRowTotalsColumnAttributes.GetSize();i++){
			switch (((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->GetType())
			{
				case CRowColumnAttributes::Sum:
					if(!pstats.m_rowsum)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Minimum:
					if(!pstats.m_rowmin)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Mean:
					if(!pstats.m_rowmean)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::Maximum:
					if(!pstats.m_rowmax)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::CountPositive:
					if(!pstats.m_rowno)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
				case CRowColumnAttributes::CountMissing:
					if(!pstats.m_rowmissing)
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Hide();
					else
						((CRowColumnAttributes*)m_aRowTotalsColumnAttributes.GetAt(i))->Unhide();
					break;
			}
		}
	}
	
	UpdateIntrinsicTotals();
	UpdateAllTotals();
	UpdateAllViews(NULL);
}

void CPatnDoc::OnTableProperties() 
{
//	CTableProperties tableproperties;

//	tableproperties.m_TableName = m_TableName;
//	tableproperties.m_MissingValue = GetMissingValue();

//	if(tableproperties.DoModal() == IDOK){
//		m_TableName = tableproperties.m_TableName;
//		SetMissingValue(tableproperties.m_MissingValue);
//	}	
	
}

void CPatnDoc::SerializeV2_24(CArchive & ar,double version)
{
//first thing first, save the Program Version Number!!!!
	// (supports program additions & removals)
	//update visible window rect regions
	POSITION pos = this->GetFirstViewPosition();
	while(pos){
		CView * v = this->GetNextView(pos);
		v->RedrawWindow();
	}
//	if(ar.IsStoring())
//		RetrieveViewContents();

	//double version = 2.22;
//	if(ar.IsStoring())
//		//ar << (double) 2.09; //current version number
//		ar << version;
//	else
//		ar >> version;

	if(version < 2.22){
		SerializeV2_19(ar,version);

		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	
	m_aColumnGroups.Serialize(ar);

	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	
	m_aRowTotalsColumnAttributes.Serialize(ar);
	
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(ar.IsStoring()){
		ar << m_MCAOTable.GetSize();
		for(i=0;i<m_MCAOTable.GetSize();i++){
			ar << m_MCAOTable[i]->GetSize();
			for(j=0;j<m_MCAOTable[i]->GetSize();j++)
				ar << m_MCAOTable[i]->GetAt(j);
		}
	}else{
		ar >> nCount;m_MCAOTable.SetSize(nCount);
		for(i=0;i<m_MCAOTable.GetSize();i++){
			m_MCAOTable[i] = new CArray<double>;
			ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
			}
		}
	}
	
	m_aColumnSums.SetSize(GetNoOfColumns());
	m_aColumnCountMissingValues.SetSize(GetNoOfColumns());
	m_aColumnMaximum.SetSize(GetNoOfColumns());
	m_aColumnMinimum.SetSize(GetNoOfColumns());
	m_aColumnCountPositives.SetSize(GetNoOfColumns());
	m_aColumnMean.SetSize(GetNoOfColumns());
	
	m_aRowSums.SetSize(GetNoOfRows());
	m_aRowCountMissingValues.SetSize(GetNoOfRows());
	m_aRowMaximum.SetSize(GetNoOfRows());
	m_aRowMinimum.SetSize(GetNoOfRows());
	m_aRowCountPositives.SetSize(GetNoOfRows());
	m_aRowMean.SetSize(GetNoOfRows());
	
	long nSize;

	if (ar.IsStoring())
	{
		ar << m_GstaAvailable;
		ar << m_AssocRowsMinkConst;
		ar << m_AssocColsMinkConst;
		nCount = m_Vectors.GetSize();
		ar << nCount;
		nSize = m_Vectors[0]->GetSize();
		ar << nSize;
		for ( i = 0; i < nCount; i++ )
		{
			for(j=0;j<nSize;j++){
				ar << m_Vectors[i]->GetAt(j);
			}
		}

		nCount = m_UserGroupColours.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_UserGroupColours[i].red;
			ar << m_UserGroupColours[i].green;
			ar << m_UserGroupColours[i].blue;			
		}

		ar << m_TwayChange;
		ar << m_AssocRowChange;
		ar << m_AssocColChange;
		ar << m_FuseRowChange;
		ar << m_FuseColChange;
		ar << m_OrdChange;
		ar << m_nMissingValue;
		ar << m_idxLeftExtrinsicColumn;
		ar << m_idxTopExtrinsicRow;

		nCount = m_FuseTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableRows.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}
		nCount = m_FuseTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableCols.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}

		nCount = m_DendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_DendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_DendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderRows[i];
		}
		nCount = m_DendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderCols[i];
		}

		nCount = m_GroupDendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_GroupDendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_GroupDendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderRows[i];
		}
		nCount = m_GroupDendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderCols[i];
		}
		

		ar.Write(&m_lfCells,sizeof(LOGFONT));
		ar << m_nPointSizeCells;
		ar << m_rgbFontCells;

		ar << m_nCutOffRows;
		ar << m_nCutOffCols;

		ar << m_FuseBetaRows;
		ar << m_FuseBetaCols;
		ar << m_FuseTimeRows;
		ar << m_FuseTimeCols;
		ar << m_FuseTypeRows;
		ar << m_FuseTypeCols;
		ar << m_FuseAssoRows;
		ar << m_FuseAssoCols;

		ar << m_OrdAsso;
		ar << m_OrdTime;
		ar << m_OrdValue;
		ar << m_Ord3d;
		ar << m_OrdStress;

		ar << m_TableName;
		ar << m_LastModified;

		ar << m_KWNumVisible;
		ar << m_KWOption;
		ar << m_KWCutoff;

		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_iterations;
		ar << m_RowAsim_seed;
		ar << m_ColumnAsim_seed;
		ar << m_KWExists;
		ar << m_MCAOExists;
		ar << m_ANOSIMRowsExists;
		ar << m_ANOSIMColsExists;
		ar << m_MCAOIterations;
		ar << m_MCAOSeed;

		ar << m_log;

		//write all new anosim values
		ar << m_ColumnAsim_pairs;
		ar << m_RowAsim_pairs;

		ar << m_RowPairStat.GetSize();
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar << m_RowPairStat[i]->GetSize();
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar << m_RowPairStat[i]->GetAt(j);
				ar << m_RowPairBest[i]->GetAt(j);
				ar << m_RowPairSave[i]->GetAt(j);
			}
		}
		ar << m_ColPairStat.GetSize();
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar << m_ColPairStat[i]->GetSize();
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar << m_ColPairStat[i]->GetAt(j);
				ar << m_ColPairBest[i]->GetAt(j);
				ar << m_ColPairSave[i]->GetAt(j);
			}
		}
	
		ar << m_AnosimRowGroups.GetSize();
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar << m_AnosimRowGroups[i];
		
		ar << m_AnosimColGroups.GetSize();
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar << m_AnosimColGroups[i];

		ar << m_RowAsim_stat;
		ar << m_RowAsim_rbest;
		ar << m_RowAsim_save;
		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_stat;
		ar << m_ColumnAsim_rbest;
		ar << m_ColumnAsim_save;
		ar << m_ColumnAsim_iterations;
		ar << m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.cols.nAssocMeasure;
		ar << m_AnalysisRecipe.cols.nBeta;
		ar << m_AnalysisRecipe.cols.nClassStrategy;
		ar << m_AnalysisRecipe.cols.nCut;
		ar << m_AnalysisRecipe.cols.nFuseTechnique;
		ar << m_AnalysisRecipe.cols.nMinkConst;
		ar << m_AnalysisRecipe.cols.nNoOfGroups;
		ar << m_AnalysisRecipe.cols.nOrd3d;
		ar << m_AnalysisRecipe.cols.nOrdination;
		ar << m_AnalysisRecipe.cols.nOrdIterations;
		ar << m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar << m_AnalysisRecipe.cols.nOrdSeed;
		ar << m_AnalysisRecipe.nFirstTime;
		ar << m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.rows.nAssocMeasure;
		ar << m_AnalysisRecipe.rows.nBeta;
		ar << m_AnalysisRecipe.rows.nClassStrategy;
		ar << m_AnalysisRecipe.rows.nCut;
		ar << m_AnalysisRecipe.rows.nFuseTechnique;
		ar << m_AnalysisRecipe.rows.nMinkConst;
		ar << m_AnalysisRecipe.rows.nNoOfGroups;
		ar << m_AnalysisRecipe.rows.nOrd3d;
		ar << m_AnalysisRecipe.rows.nOrdination;
		ar << m_AnalysisRecipe.rows.nOrdIterations;
		ar << m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar << m_AnalysisRecipe.rows.nOrdSeed;
	
		ar << m_pGstaVisible;
		ar << m_AssocRowVisible;
		ar << m_AssocColVisible;
		ar << m_AssocRowUpdate;
		ar << m_AssocColUpdate;
		ar << m_FuseRowVisible;
		ar << m_FuseColVisible;
		ar << m_FuseRowUpdate;
		ar << m_FuseColUpdate;
		ar << m_nScale;
		ar << m_pExists;
		ar << m_pIsRowLabel;
		ar << m_colstand;
		ar << m_presabs;
		ar << m_globstand;
		ar << m_red;
		ar << m_green;
		ar << m_blue;
		ar << m_bw;
		ar << m_AnosimPairView;
		ar << m_AnosimColView; 
		ar << m_AnosimRowView; 
		ar << m_OrdVisible;
		ar << m_OrdXVisible;
		ar << m_OrdUpdate;
		ar << m_boxwhiskervisible;
		ar << mr_tableview;
		ar << mr_rowtotals;
		ar << mr_coltotals;
		ar << mr_anosimrow;
		ar << mr_anosimcol;
		ar << mr_fuserow;
		ar << mr_fusecol;
		ar << mr_fuserowdend;
		ar << mr_fusecoldend;
		ar << mr_assocrowmx;
		ar << mr_assoccolmx;
		ar << mr_assocrowhist;
		ar << mr_assoccolhist;
		ar << mr_ord;
		ar << mr_tway;
		ar << mr_gsta;
		ar << mr_boxwhisker;
		
		ar << m_Rotate;
		ar << m_Labels;
		ar << m_Legend;
		ar << m_Mst;
		ar << m_Axies;
		ar << m_Grouping;
		ar << m_Usergroupcolours;
		ar << m_SelOnly;
		ar << m_Centroid;
		ar << m_Showranges;
		ar << m_Symbols;
		ar << m_RotationSpeed;
		ar << m_TextBoxes.GetSize();
		for(i=0;i<m_TextBoxes.GetSize();i++){
			ar << m_TextBoxes[i]->contents;
			ar << m_TextBoxes[i]->size;
			ar << m_TextBoxes[i]->location;
		}
		ar << m_TextBoxes2.GetSize();
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			ar << m_TextBoxes2[i]->contents;
			ar << m_TextBoxes2[i]->location;
			ar << m_TextBoxes2[i]->colour;
			ar.Write(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}
		ar << m_stressbox.contents;
		ar << m_stressbox.location;
		ar << m_stressbox.colour;
		ar.Write(&m_stressbox.font,sizeof(LOGFONT));

		ar << m_vEyePt.x;ar << m_vEyePt.y;ar << m_vEyePt.z;
		ar << m_vUpVec.x;ar << m_vUpVec.y;ar << m_vUpVec.z;
		ar << m_vRight.x;ar << m_vRight.y;ar << m_vRight.z;
		
		ar << wAngleY; 
		ar << m_NoSymbols;
		ar << m_PccVectors.GetSize();
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar << m_PccVectors[i];
		}

		nCount = m_aGroupColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aGroupColours2[i].red;
			ar << m_aGroupColours2[i].green;
			ar << m_aGroupColours2[i].blue;			
		}
		nCount = m_aObjectColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aObjectColours2[i].red;
			ar << m_aObjectColours2[i].green;
			ar << m_aObjectColours2[i].blue;			
		}
		nCount = m_r.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_r[i].red;
			ar << m_r[i].green;
			ar << m_r[i].blue;			
		}
		nCount = m_Vectors2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_Vectors2[i].red;
			ar << m_Vectors2[i].green;
			ar << m_Vectors2[i].blue;			
		}			
	
		//2.18 (some is above)
		m_RowSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}
		m_ColSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}		

		//2.19
		i = m_createKW;ar << i;
		i = m_createANOSIMRows;ar << i;
		i = m_createANOSIMCols;ar << i;
		i = m_createMCAO;ar << i;
		i = m_createPCC;ar << i;
	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		ar >> m_RowAsim_stat;
		ar >> m_RowAsim_rbest;
		ar >> m_RowAsim_save;
		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_stat;
		ar >> m_ColumnAsim_rbest;
		ar >> m_ColumnAsim_save;
		ar >> m_ColumnAsim_iterations;
		ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nBeta;
		ar >> m_AnalysisRecipe.cols.nClassStrategy;
		ar >> m_AnalysisRecipe.cols.nCut;
		ar >> m_AnalysisRecipe.cols.nFuseTechnique;
		ar >> m_AnalysisRecipe.cols.nMinkConst;
		ar >> m_AnalysisRecipe.cols.nNoOfGroups;
		ar >> m_AnalysisRecipe.cols.nOrd3d;
		ar >> m_AnalysisRecipe.cols.nOrdination;
		ar >> m_AnalysisRecipe.cols.nOrdIterations;
		ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.cols.nOrdSeed;
		ar >> m_AnalysisRecipe.nFirstTime;
		ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nBeta;
		ar >> m_AnalysisRecipe.rows.nClassStrategy;
		ar >> m_AnalysisRecipe.rows.nCut;
		ar >> m_AnalysisRecipe.rows.nFuseTechnique;
		ar >> m_AnalysisRecipe.rows.nMinkConst;
		ar >> m_AnalysisRecipe.rows.nNoOfGroups;
		ar >> m_AnalysisRecipe.rows.nOrd3d;
		ar >> m_AnalysisRecipe.rows.nOrdination;
		ar >> m_AnalysisRecipe.rows.nOrdIterations;
		ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.rows.nOrdSeed;
		
		ar >> i;m_pGstaVisible = (i)?true:false;
		ar >> m_AssocRowVisible;
		ar >> m_AssocColVisible;
		ar >> m_AssocRowUpdate;
		ar >> m_AssocColUpdate;
		ar >> m_FuseRowVisible;
		ar >> m_FuseColVisible;
		ar >> m_FuseRowUpdate;
		ar >> m_FuseColUpdate;
		ar >> m_nScale;
		ar >> m_pExists;
		ar >> m_pIsRowLabel;
		ar >> m_colstand;
		ar >> m_presabs;
		ar >> m_globstand;
		ar >> m_red;
		ar >> m_green;
		ar >> m_blue;
		ar >> m_bw;
		ar >> i;m_AnosimPairView = (i)?true:false;
		ar >> i;m_AnosimColView = (i)?true:false; 
		ar >> i;m_AnosimRowView = (i)?true:false; 
		ar >> i;m_OrdVisible = (i)?true:false;
		ar >> i;m_OrdXVisible = (i)?true:false;
		ar >> i;m_OrdUpdate = (i)?true:false;
		ar >> m_boxwhiskervisible;
		ar >> mr_tableview;
		ar >> mr_rowtotals;
		ar >> mr_coltotals;
		ar >> mr_anosimrow;
		ar >> mr_anosimcol;
		ar >> mr_fuserow;
		ar >> mr_fusecol;
		ar >> mr_fuserowdend;
		ar >> mr_fusecoldend;
		ar >> mr_assocrowmx;
		ar >> mr_assoccolmx;
		ar >> mr_assocrowhist;
		ar >> mr_assoccolhist;
		ar >> mr_ord;
		ar >> mr_tway;
		ar >> mr_gsta;
		ar >> mr_boxwhisker;

		ar >> i;m_Rotate = (i)?true:false;
		ar >> i;m_Labels = (i)?true:false;
		ar >> i;m_Legend = (i)?true:false;
		ar >> i;m_Mst = (i)?true:false;
		ar >> i;m_Axies = (i)?true:false;
		ar >> i;m_Grouping = (i)?true:false;
		ar >> i;m_Usergroupcolours = (i)?true:false;
		ar >> i;m_SelOnly = (i)?true:false;
		ar >> i;m_Centroid = (i)?true:false;
		ar >> i;m_Showranges = (i)?true:false;
		ar >> i;m_Symbols = (i)?true:false;
		ar >> m_RotationSpeed;
		long size;
		ar >> size;m_TextBoxes.SetSize(size);
		for(i=0;i<m_TextBoxes.GetSize();i++){
			m_TextBoxes[i] = new TextBox;
			ar >> m_TextBoxes[i]->contents;
			ar >> m_TextBoxes[i]->size;
			ar >> m_TextBoxes[i]->location;
		}			
		ar >> size;m_TextBoxes2.SetSize(size);
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			m_TextBoxes2[i] = new TextBox2;
			ar >> m_TextBoxes2[i]->contents;
			ar >> m_TextBoxes2[i]->location;
			ar >> m_TextBoxes2[i]->colour;
			ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}			
		ar >> m_stressbox.contents;
		ar >> m_stressbox.location;
		ar >> m_stressbox.colour;
		ar.Read(&m_stressbox.font,sizeof(LOGFONT));
		
		ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
		ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
		ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

		ar >> wAngleY; 
		ar >> i;m_NoSymbols = (i)?true:false;
		ar >> size;m_PccVectors.SetSize(size);
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar >> m_PccVectors[i];
		}

		ar >> nCount;
		m_aGroupColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aGroupColours2[i].red;
			ar >> m_aGroupColours2[i].green;
			ar >> m_aGroupColours2[i].blue;			
		}
		ar >> nCount;
		m_aObjectColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aObjectColours2[i].red;
			ar >> m_aObjectColours2[i].green;
			ar >> m_aObjectColours2[i].blue;			
		}
		ar >> nCount;
		m_r.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_r[i].red;
			ar >> m_r[i].green;
			ar >> m_r[i].blue;			
		}
		ar >> nCount;
		m_Vectors2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_Vectors2[i].red;
			ar >> m_Vectors2[i].green;
			ar >> m_Vectors2[i].blue;			
		}
		m_3ddefault = true;

		//2.18 (some is above)
		POSITION pos = this->GetFirstViewPosition();
		CView * pView;
		while(pos != NULL){
			pView = this->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
				break;
			}
			pView = NULL;
		}

		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedRows->SetAt(j);
		}	
		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
		}	

		//2.19
		ar >> i;m_createKW = (i)?true:false;
		ar >> i;m_createANOSIMRows = (i)?true:false;
		ar >> i;m_createANOSIMCols = (i)?true:false;
		ar >> i;m_createMCAO = (i)?true:false;
		ar >> i;m_createPCC = (i)?true:false;


		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)

		CGsta cgsta(this);

		m_pExists = 0;
		m_FuseRowVisible = 0;
		m_FuseColVisible = 0;
		m_AssocRowVisible = 0;
		m_AssocColVisible = 0;
		m_pGstaVisible = 0;
		m_boxwhiskervisible = 0;
		m_OrdVisible = 0;
		m_OrdXVisible = 0;
		m_AnosimRowView = 0;
		m_AnosimColView = 0;

		//SetupWindowsAfterLoad();
	}
}

void CPatnDoc::OnFrameWindowActivate(int bActivate) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	COleServerDoc::OnFrameWindowActivate(bActivate);
}

void CPatnDoc::OnDataControlgroups() 
{
	//find & close row dendogram
	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CDendView)))
		{
			CDendView * dv = (CDendView*) pView;
			if(dv->m_row)
				dv->PostMessage(WM_CLOSE);

		}
	}	

	UpdateAprioriGroupsColours();

	CGroupsDlg groupsdlg(this);
	groupsdlg.m_nDataType = true;
	groupsdlg.m_nDataControl = true;
	groupsdlg.DoModal();

	UpdateAprioriGroupsColours();
	
	//update all views
	pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		pView->RedrawWindow();
	}	
	
}

void CPatnDoc::SerializeV2_25(CArchive& ar,double version)
{
	if(version < 2.25){
		SerializeV2_24(ar,version);

		return;
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);

 	CFuseLine* pFuseLine;
	CDendLine* pDendLine;

	CArray<int> intarray;

	vector * nVector;

	double dbl;
	long nCount;
	long i,j;

	m_aData.Serialize(ar);
	
	m_aColumnGroups.Serialize(ar);

	m_aRowGroups.Serialize(ar);
	m_aRowXCoords.Serialize(ar);
	m_aRowYCoords.Serialize(ar);
	m_aRowZCoords.Serialize(ar);
	m_PatternObjects.Serialize(ar);

	m_aColumnAttributes.Serialize(ar);
	m_aRowAttributes.Serialize(ar);

	m_aColumnTotalsRowAttributes.Serialize(ar);
	
	m_aRowTotalsColumnAttributes.Serialize(ar);
	
	m_aColumnAllocRowAttributes.Serialize(ar);
	m_aColumnCompareRowAttributes.Serialize(ar);
	m_aColumnPccAttributes.Serialize(ar);
	m_aRowAllocColumnAttributes.Serialize(ar);
	m_aRowOrdColumnAttributes.Serialize(ar);

	m_AssocMatrixRows.Serialize(ar);
	m_AssocMatrixCols.Serialize(ar);

	m_KWValues.Serialize(ar);
	m_KWAttributes.Serialize(ar);
	m_KW.Serialize(ar);

	m_MCAOResults.Serialize(ar);
	if(ar.IsStoring()){
		ar << m_MCAOTable.GetSize();
		for(i=0;i<m_MCAOTable.GetSize();i++){
			ar << m_MCAOTable[i]->GetSize();
			for(j=0;j<m_MCAOTable[i]->GetSize();j++)
				ar << m_MCAOTable[i]->GetAt(j);
		}
	}else{
		ar >> nCount;m_MCAOTable.SetSize(nCount);
		for(i=0;i<m_MCAOTable.GetSize();i++){
			m_MCAOTable[i] = new CArray<double>;
			ar >> nCount;m_MCAOTable[i]->SetSize(nCount);				
			for(j=0;j<m_MCAOTable[i]->GetSize();j++){
				ar >> dbl;m_MCAOTable[i]->SetAt(j,dbl);
			}
		}
	}
	
	m_aColumnSums.SetSize(GetNoOfColumns());
	m_aColumnCountMissingValues.SetSize(GetNoOfColumns());
	m_aColumnMaximum.SetSize(GetNoOfColumns());
	m_aColumnMinimum.SetSize(GetNoOfColumns());
	m_aColumnCountPositives.SetSize(GetNoOfColumns());
	m_aColumnMean.SetSize(GetNoOfColumns());
	
	m_aRowSums.SetSize(GetNoOfRows());
	m_aRowCountMissingValues.SetSize(GetNoOfRows());
	m_aRowMaximum.SetSize(GetNoOfRows());
	m_aRowMinimum.SetSize(GetNoOfRows());
	m_aRowCountPositives.SetSize(GetNoOfRows());
	m_aRowMean.SetSize(GetNoOfRows());
	
	long nSize;

	if (ar.IsStoring())
	{
		ar << m_GstaAvailable;
		ar << m_AssocRowsMinkConst;
		ar << m_AssocColsMinkConst;
		nCount = m_Vectors.GetSize();
		ar << nCount;
		nSize = m_Vectors[0]->GetSize();
		ar << nSize;
		for ( i = 0; i < nCount; i++ )
		{
			for(j=0;j<nSize;j++){
				ar << m_Vectors[i]->GetAt(j);
			}
		}

		nCount = m_UserGroupColours.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_UserGroupColours[i].red;
			ar << m_UserGroupColours[i].green;
			ar << m_UserGroupColours[i].blue;			
		}

		ar << m_TwayChange;
		ar << m_AssocRowChange;
		ar << m_AssocColChange;
		ar << m_FuseRowChange;
		ar << m_FuseColChange;
		ar << m_OrdChange;
		ar << m_nMissingValue;
		ar << m_idxLeftExtrinsicColumn;
		ar << m_idxTopExtrinsicRow;

		nCount = m_FuseTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableRows.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}
		nCount = m_FuseTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = m_FuseTableCols.GetAt(i);
			ar << pFuseLine->nLeft;
			ar << pFuseLine->nRight;
			ar << pFuseLine->nLevel;
		}

		nCount = m_DendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_DendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_DendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_DendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderRows[i];
		}
		nCount = m_DendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_DendOrderCols[i];
		}

		nCount = m_GroupDendTableRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableRows.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}
		nCount = m_GroupDendTableCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = m_GroupDendTableCols.GetAt(i);
			ar << pDendLine->xStart;
			ar << pDendLine->xRight;
			ar << pDendLine->xEnd;
			ar << pDendLine->yStart;
			ar << pDendLine->yEnd;
		}

		nCount = m_GroupDendOrderRows.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderRows[i];
		}
		nCount = m_GroupDendOrderCols.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_GroupDendOrderCols[i];
		}
		

		ar.Write(&m_lfCells,sizeof(LOGFONT));
		ar << m_nPointSizeCells;
		ar << m_rgbFontCells;

		ar << m_nCutOffRows;
		ar << m_nCutOffCols;

		ar << m_FuseBetaRows;
		ar << m_FuseBetaCols;
		ar << m_FuseTimeRows;
		ar << m_FuseTimeCols;
		ar << m_FuseTypeRows;
		ar << m_FuseTypeCols;
		ar << m_FuseAssoRows;
		ar << m_FuseAssoCols;

		ar << m_OrdAsso;
		ar << m_OrdTime;
		ar << m_OrdValue;
		ar << m_Ord3d;
		ar << m_OrdStress;

		ar << m_TableName;
		ar << m_LastModified;

		ar << m_KWNumVisible;
		ar << m_KWOption;
		ar << m_KWCutoff;

		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_iterations;
		ar << m_RowAsim_seed;
		ar << m_ColumnAsim_seed;
		ar << m_KWExists;
		ar << m_MCAOExists;
		ar << m_ANOSIMRowsExists;
		ar << m_ANOSIMColsExists;
		ar << m_MCAOIterations;
		ar << m_MCAOSeed;

		ar << m_log;

		//write all new anosim values
		ar << m_ColumnAsim_pairs;
		ar << m_RowAsim_pairs;

		ar << m_RowPairStat.GetSize();
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar << m_RowPairStat[i]->GetSize();
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar << m_RowPairStat[i]->GetAt(j);
				ar << m_RowPairBest[i]->GetAt(j);
				ar << m_RowPairSave[i]->GetAt(j);
			}
		}
		ar << m_ColPairStat.GetSize();
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar << m_ColPairStat[i]->GetSize();
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar << m_ColPairStat[i]->GetAt(j);
				ar << m_ColPairBest[i]->GetAt(j);
				ar << m_ColPairSave[i]->GetAt(j);
			}
		}
	
		ar << m_AnosimRowGroups.GetSize();
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar << m_AnosimRowGroups[i];
		
		ar << m_AnosimColGroups.GetSize();
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar << m_AnosimColGroups[i];

		ar << m_RowAsim_stat;
		ar << m_RowAsim_rbest;
		ar << m_RowAsim_save;
		ar << m_RowAsim_iterations;
		ar << m_ColumnAsim_stat;
		ar << m_ColumnAsim_rbest;
		ar << m_ColumnAsim_save;
		ar << m_ColumnAsim_iterations;
		ar << m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.cols.nAssocMeasure;
		ar << m_AnalysisRecipe.cols.nBeta;
		ar << m_AnalysisRecipe.cols.nClassStrategy;
		ar << m_AnalysisRecipe.cols.nCut;
		ar << m_AnalysisRecipe.cols.nFuseTechnique;
		ar << m_AnalysisRecipe.cols.nMinkConst;
		ar << m_AnalysisRecipe.cols.nNoOfGroups;
		ar << m_AnalysisRecipe.cols.nOrd3d;
		ar << m_AnalysisRecipe.cols.nOrdination;
		ar << m_AnalysisRecipe.cols.nOrdIterations;
		ar << m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar << m_AnalysisRecipe.cols.nOrdSeed;
		ar << m_AnalysisRecipe.nFirstTime;
		ar << m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar << m_AnalysisRecipe.rows.nAssocMeasure;
		ar << m_AnalysisRecipe.rows.nBeta;
		ar << m_AnalysisRecipe.rows.nClassStrategy;
		ar << m_AnalysisRecipe.rows.nCut;
		ar << m_AnalysisRecipe.rows.nFuseTechnique;
		ar << m_AnalysisRecipe.rows.nMinkConst;
		ar << m_AnalysisRecipe.rows.nNoOfGroups;
		ar << m_AnalysisRecipe.rows.nOrd3d;
		ar << m_AnalysisRecipe.rows.nOrdination;
		ar << m_AnalysisRecipe.rows.nOrdIterations;
		ar << m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar << m_AnalysisRecipe.rows.nOrdSeed;
	
		ar << m_pGstaVisible;
		ar << m_AssocRowVisible;
		ar << m_AssocColVisible;
		ar << m_AssocRowUpdate;
		ar << m_AssocColUpdate;
		ar << m_FuseRowVisible;
		ar << m_FuseColVisible;
		ar << m_FuseRowUpdate;
		ar << m_FuseColUpdate;
		ar << m_nScale;
		ar << m_pExists;
		ar << m_pIsRowLabel;
		ar << m_colstand;
		ar << m_presabs;
		ar << m_globstand;
		ar << m_red;
		ar << m_green;
		ar << m_blue;
		ar << m_bw;
		ar << m_AnosimPairView;
		ar << m_AnosimColView; 
		ar << m_AnosimRowView; 
		ar << m_OrdVisible;
		ar << m_OrdXVisible;
		ar << m_OrdUpdate;
		ar << m_boxwhiskervisible;
		ar << mr_tableview;
		ar << mr_rowtotals;
		ar << mr_coltotals;
		ar << mr_anosimrow;
		ar << mr_anosimcol;
		ar << mr_fuserow;
		ar << mr_fusecol;
		ar << mr_fuserowdend;
		ar << mr_fusecoldend;
		ar << mr_assocrowmx;
		ar << mr_assoccolmx;
		ar << mr_assocrowhist;
		ar << mr_assoccolhist;
		ar << mr_ord;
		ar << mr_tway;
		ar << mr_gsta;
		ar << mr_boxwhisker;
		
		ar << m_Rotate;
		ar << m_Labels;
		ar << m_Legend;
		ar << m_Mst;
		ar << m_Axies;
		ar << m_Grouping;
		ar << m_Usergroupcolours;
		ar << m_SelOnly;
		ar << m_Centroid;
		ar << m_Showranges;
		ar << m_Symbols;
		ar << m_RotationSpeed;
		ar << m_TextBoxes.GetSize();
		for(i=0;i<m_TextBoxes.GetSize();i++){
			ar << m_TextBoxes[i]->contents;
			ar << m_TextBoxes[i]->size;
			ar << m_TextBoxes[i]->location;
		}
		ar << m_TextBoxes2.GetSize();
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			ar << m_TextBoxes2[i]->contents;
			ar << m_TextBoxes2[i]->location;
			ar << m_TextBoxes2[i]->colour;
			ar.Write(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}
		ar << m_stressbox.contents;
		ar << m_stressbox.location;
		ar << m_stressbox.colour;
		ar.Write(&m_stressbox.font,sizeof(LOGFONT));

		ar << m_vEyePt.x;ar << m_vEyePt.y;ar << m_vEyePt.z;
		ar << m_vUpVec.x;ar << m_vUpVec.y;ar << m_vUpVec.z;
		ar << m_vRight.x;ar << m_vRight.y;ar << m_vRight.z;
		
		ar << wAngleY; 
		ar << m_NoSymbols;
		ar << m_PccVectors.GetSize();
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar << m_PccVectors[i];
		}

		nCount = m_aGroupColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aGroupColours2[i].red;
			ar << m_aGroupColours2[i].green;
			ar << m_aGroupColours2[i].blue;			
		}
		nCount = m_aObjectColours2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_aObjectColours2[i].red;
			ar << m_aObjectColours2[i].green;
			ar << m_aObjectColours2[i].blue;			
		}
		nCount = m_r.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_r[i].red;
			ar << m_r[i].green;
			ar << m_r[i].blue;			
		}
		nCount = m_Vectors2.GetSize();
		ar << nCount;
		for ( i = 0; i < nCount; i++ )
		{
			ar << m_Vectors2[i].red;
			ar << m_Vectors2[i].green;
			ar << m_Vectors2[i].blue;			
		}			
	
		//2.18 (some is above)
		m_RowSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}
		m_ColSelection->GetItems(&intarray);
		nCount = intarray.GetSize();
		ar << nCount;
		for(i=0;i<nCount;i++){
			ar << intarray[i];
		}		

		//2.19
		i = m_createKW;ar << i;
		i = m_createANOSIMRows;ar << i;
		i = m_createANOSIMCols;ar << i;
		i = m_createMCAO;ar << i;
		i = m_createPCC;ar << i;

		//2.25
		ar << m_visible_decimals;
		ar.Write(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Write(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

	}
	else
	{	
		ar >> m_GstaAvailable;
		ar >> m_AssocRowsMinkConst;
		ar >> m_AssocColsMinkConst;
		ar >> nCount;	
		m_Vectors.SetSize(nCount);
		ar >> nSize;
		for ( i = 0; i < nCount; i++ )
		{
			nVector = new vector;
			nVector->SetSize(nSize);
			for(j=0;j<nSize;j++){
				ar >> dbl;
				nVector->SetAt(j,dbl);
			}
			m_Vectors[i] = nVector;
		}
		
		ar >> nCount;
		m_UserGroupColours.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_UserGroupColours[i].red;
			ar >> m_UserGroupColours[i].green;
			ar >> m_UserGroupColours[i].blue;			
		}

		ar >> m_TwayChange;
		ar >> m_AssocRowChange;
		ar >> m_AssocColChange;
		ar >> m_FuseRowChange;
		ar >> m_FuseColChange;
		ar >> m_OrdChange;
		ar >> m_nMissingValue;
		ar >> m_idxLeftExtrinsicColumn;
		ar >> m_idxTopExtrinsicRow;

		ar >> nCount;
		m_FuseTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableRows.SetAt(i,pFuseLine);
		}
		ar >> nCount;
		m_FuseTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pFuseLine = new CFuseLine;
			ar >> pFuseLine->nLeft;
			ar >> pFuseLine->nRight;
			ar >> pFuseLine->nLevel;
			m_FuseTableCols.SetAt(i,pFuseLine);
		}

 		ar >> nCount;
		m_DendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_DendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_DendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_DendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderRows[i];
		}
		ar >> nCount;
		m_DendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_DendOrderCols[i];
		}
 		ar >> nCount;
		
		m_GroupDendTableRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableRows.SetAt(i,pDendLine);
		}
		ar >> nCount;
		m_GroupDendTableCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			pDendLine = new CDendLine;
			ar >> pDendLine->xStart;
			ar >> pDendLine->xRight;
			ar >> pDendLine->xEnd;
			ar >> pDendLine->yStart;
			ar >> pDendLine->yEnd;
			m_GroupDendTableCols.SetAt(i,pDendLine);
		}

		ar >> nCount;
		m_GroupDendOrderRows.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderRows[i];
		}
		ar >> nCount;
		m_GroupDendOrderCols.SetSize(nCount);
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_GroupDendOrderCols[i];
		}

		ar.Read(&m_lfCells,sizeof(LOGFONT));
		ar >> m_nPointSizeCells;
		ar >> m_rgbFontCells;

		ar >> m_nCutOffRows;
		ar >> m_nCutOffCols;

		ar >> m_FuseBetaRows;
		ar >> m_FuseBetaCols;
		ar >> m_FuseTimeRows;
		ar >> m_FuseTimeCols;
		ar >> m_FuseTypeRows;
		ar >> m_FuseTypeCols;
		ar >> m_FuseAssoRows;
		ar >> m_FuseAssoCols;

		ar >> m_OrdAsso;
		ar >> m_OrdTime;
		ar >> m_OrdValue;
		ar >> m_Ord3d;
		ar >> m_OrdStress;

		ar >> m_TableName;
		ar >> m_LastModified;

		ar >> m_KWNumVisible;
		ar >> m_KWOption;
		ar >> m_KWCutoff;

		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_iterations;
		ar >> m_RowAsim_seed;
		ar >> m_ColumnAsim_seed;
		ar >> i;m_KWExists = (i)?true:false;
		ar >> i;m_MCAOExists = (i)?true:false;
		ar >> i;m_ANOSIMRowsExists = (i)?true:false;
		ar >> i;m_ANOSIMColsExists = (i)?true:false;
		ar >> m_MCAOIterations;
		ar >> m_MCAOSeed;

		ar >> m_log;

		//write all new anosim values
		ar >> i;m_ColumnAsim_pairs = (i)?true:false;
		ar >> i;m_RowAsim_pairs = (i)?true:false;

		ar >> i;
		m_RowPairStat.SetSize(i);
		m_RowPairBest.SetSize(i);
		m_RowPairSave.SetSize(i);
		for(i=0;i<m_RowPairStat.GetSize();i++){
			ar >> j;
			m_RowPairStat[i] = new CArray<double>;
			m_RowPairStat[i]->SetSize(j);
			m_RowPairBest[i] = new CArray<double>;
			m_RowPairBest[i]->SetSize(j);
			m_RowPairSave[i] = new CArray<double>;
			m_RowPairSave[i]->SetSize(j);
			for(j=0;j<m_RowPairStat[i]->GetSize();j++){
				ar >> dbl;m_RowPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_RowPairSave[i]->SetAt(j,dbl);
			}
		}
		ar >> i;
		m_ColPairStat.SetSize(i);
		m_ColPairBest.SetSize(i);
		m_ColPairSave.SetSize(i);
		for(i=0;i<m_ColPairStat.GetSize();i++){
			ar >> j;
			m_ColPairStat[i] = new CArray<double>;
			m_ColPairStat[i]->SetSize(j);
			m_ColPairBest[i] = new CArray<double>;
			m_ColPairBest[i]->SetSize(j);
			m_ColPairSave[i] = new CArray<double>;
			m_ColPairSave[i]->SetSize(j);
			for(j=0;j<m_ColPairStat[i]->GetSize();j++){
				ar >> dbl;m_ColPairStat[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairBest[i]->SetAt(j,dbl);
				ar >> dbl;m_ColPairSave[i]->SetAt(j,dbl);
			}
		}
	
		ar >> i;m_AnosimRowGroups.SetSize(i);
		for(i=0;i<m_AnosimRowGroups.GetSize();i++)
			ar >> m_AnosimRowGroups[i];
		
		ar >> i;m_AnosimColGroups.SetSize(i);
		for(i=0;i<m_AnosimColGroups.GetSize();i++)
			ar >> m_AnosimColGroups[i];

		ar >> m_RowAsim_stat;
		ar >> m_RowAsim_rbest;
		ar >> m_RowAsim_save;
		ar >> m_RowAsim_iterations;
		ar >> m_ColumnAsim_stat;
		ar >> m_ColumnAsim_rbest;
		ar >> m_ColumnAsim_save;
		ar >> m_ColumnAsim_iterations;
		ar >> m_AnalysisRecipe.cols.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nAssocMeasure;
		ar >> m_AnalysisRecipe.cols.nBeta;
		ar >> m_AnalysisRecipe.cols.nClassStrategy;
		ar >> m_AnalysisRecipe.cols.nCut;
		ar >> m_AnalysisRecipe.cols.nFuseTechnique;
		ar >> m_AnalysisRecipe.cols.nMinkConst;
		ar >> m_AnalysisRecipe.cols.nNoOfGroups;
		ar >> m_AnalysisRecipe.cols.nOrd3d;
		ar >> m_AnalysisRecipe.cols.nOrdination;
		ar >> m_AnalysisRecipe.cols.nOrdIterations;
		ar >> m_AnalysisRecipe.cols.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.cols.nOrdSeed;
		ar >> m_AnalysisRecipe.nFirstTime;
		ar >> m_AnalysisRecipe.rows.nAllocAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nAssocMeasure;
		ar >> m_AnalysisRecipe.rows.nBeta;
		ar >> m_AnalysisRecipe.rows.nClassStrategy;
		ar >> m_AnalysisRecipe.rows.nCut;
		ar >> m_AnalysisRecipe.rows.nFuseTechnique;
		ar >> m_AnalysisRecipe.rows.nMinkConst;
		ar >> m_AnalysisRecipe.rows.nNoOfGroups;
		ar >> m_AnalysisRecipe.rows.nOrd3d;
		ar >> m_AnalysisRecipe.rows.nOrdination;
		ar >> m_AnalysisRecipe.rows.nOrdIterations;
		ar >> m_AnalysisRecipe.rows.nOrdRandomStarts;
		ar >> m_AnalysisRecipe.rows.nOrdSeed;
		
		ar >> i;m_pGstaVisible = (i)?true:false;
		ar >> m_AssocRowVisible;
		ar >> m_AssocColVisible;
		ar >> m_AssocRowUpdate;
		ar >> m_AssocColUpdate;
		ar >> m_FuseRowVisible;
		ar >> m_FuseColVisible;
		ar >> m_FuseRowUpdate;
		ar >> m_FuseColUpdate;
		ar >> m_nScale;
		ar >> m_pExists;
		ar >> m_pIsRowLabel;
		ar >> m_colstand;
		ar >> m_presabs;
		ar >> m_globstand;
		ar >> m_red;
		ar >> m_green;
		ar >> m_blue;
		ar >> m_bw;
		ar >> i;m_AnosimPairView = (i)?true:false;
		ar >> i;m_AnosimColView = (i)?true:false; 
		ar >> i;m_AnosimRowView = (i)?true:false; 
		ar >> i;m_OrdVisible = (i)?true:false;
		ar >> i;m_OrdXVisible = (i)?true:false;
		ar >> i;m_OrdUpdate = (i)?true:false;
		ar >> m_boxwhiskervisible;
		ar >> mr_tableview;
		ar >> mr_rowtotals;
		ar >> mr_coltotals;
		ar >> mr_anosimrow;
		ar >> mr_anosimcol;
		ar >> mr_fuserow;
		ar >> mr_fusecol;
		ar >> mr_fuserowdend;
		ar >> mr_fusecoldend;
		ar >> mr_assocrowmx;
		ar >> mr_assoccolmx;
		ar >> mr_assocrowhist;
		ar >> mr_assoccolhist;
		ar >> mr_ord;
		ar >> mr_tway;
		ar >> mr_gsta;
		ar >> mr_boxwhisker;

		ar >> i;m_Rotate = (i)?true:false;
		ar >> i;m_Labels = (i)?true:false;
		ar >> i;m_Legend = (i)?true:false;
		ar >> i;m_Mst = (i)?true:false;
		ar >> i;m_Axies = (i)?true:false;
		ar >> i;m_Grouping = (i)?true:false;
		ar >> i;m_Usergroupcolours = (i)?true:false;
		ar >> i;m_SelOnly = (i)?true:false;
		ar >> i;m_Centroid = (i)?true:false;
		ar >> i;m_Showranges = (i)?true:false;
		ar >> i;m_Symbols = (i)?true:false;
		ar >> m_RotationSpeed;
		long size;
		ar >> size;m_TextBoxes.SetSize(size);
		for(i=0;i<m_TextBoxes.GetSize();i++){
			m_TextBoxes[i] = new TextBox;
			ar >> m_TextBoxes[i]->contents;
			ar >> m_TextBoxes[i]->size;
			ar >> m_TextBoxes[i]->location;
		}			
		ar >> size;m_TextBoxes2.SetSize(size);
		for(i=0;i<m_TextBoxes2.GetSize();i++){
			m_TextBoxes2[i] = new TextBox2;
			ar >> m_TextBoxes2[i]->contents;
			ar >> m_TextBoxes2[i]->location;
			ar >> m_TextBoxes2[i]->colour;
			ar.Read(&m_TextBoxes2[i]->font,sizeof(LOGFONT));
		}			
		ar >> m_stressbox.contents;
		ar >> m_stressbox.location;
		ar >> m_stressbox.colour;
		ar.Read(&m_stressbox.font,sizeof(LOGFONT));
		
		ar >> m_vEyePt.x;ar >> m_vEyePt.y;ar >> m_vEyePt.z;
		ar >> m_vUpVec.x;ar >> m_vUpVec.y;ar >> m_vUpVec.z;
		ar >> m_vRight.x;ar >> m_vRight.y;ar >> m_vRight.z;

		ar >> wAngleY; 
		ar >> i;m_NoSymbols = (i)?true:false;
		ar >> size;m_PccVectors.SetSize(size);
		for(i=0;i<m_PccVectors.GetSize();i++){
			ar >> m_PccVectors[i];
		}

		ar >> nCount;
		m_aGroupColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aGroupColours2[i].red;
			ar >> m_aGroupColours2[i].green;
			ar >> m_aGroupColours2[i].blue;			
		}
		ar >> nCount;
		m_aObjectColours2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_aObjectColours2[i].red;
			ar >> m_aObjectColours2[i].green;
			ar >> m_aObjectColours2[i].blue;			
		}
		ar >> nCount;
		m_r.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_r[i].red;
			ar >> m_r[i].green;
			ar >> m_r[i].blue;			
		}
		ar >> nCount;
		m_Vectors2.SetSize(nCount);			
		for ( i = 0; i < nCount; i++ )
		{
			ar >> m_Vectors2[i].red;
			ar >> m_Vectors2[i].green;
			ar >> m_Vectors2[i].blue;			
		}
		m_3ddefault = true;

		//2.18 (some is above)
		POSITION pos = this->GetFirstViewPosition();
		CView * pView;
		while(pos != NULL){
			pView = this->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
				break;
			}
			pView = NULL;
		}

		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedRows->SetAt(j);
		}	
		ar >> nCount;
		for(i=0;i<nCount;i++){
			ar >> j;
			if(pView != NULL)
				((CTableView*)pView)->m_pSelectedColumns->SetAt(j);
		}	

		//2.19
		ar >> i;m_createKW = (i)?true:false;
		ar >> i;m_createANOSIMRows = (i)?true:false;
		ar >> i;m_createANOSIMCols = (i)?true:false;
		ar >> i;m_createMCAO = (i)?true:false;
		ar >> i;m_createPCC = (i)?true:false;

		//2.25
		ar >> m_visible_decimals;
		ar.Read(&m_wndplace_tableview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_gstaview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_tway,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_dendcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fuserow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_fusecol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assocrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_assoccol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histrow,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_histcol,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_rowtotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_coltotals,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_3dview,sizeof(WINDOWPLACEMENT));
		ar.Read(&m_wndplace_boxwhisker,sizeof(WINDOWPLACEMENT));

		//functions to set up stuff not stored
		UpdateGroups();
		UpdateIntrinsicTotals();
		UpdateAllTotals();
		
		CColr ccolr(this);
		ccolr.OnCreate();
	
		CMst cmst(this);
		cmst.OnCreate();

		//if(m_Groups.GetSize() > 1)

		CGsta cgsta(this);

		SetupWindowsAfterLoad();
	}
}


CString CPatnDoc::getExportGroupCompositionHeader(long isrowdependant)
{
	CString str;

	if(isrowdependant)
	{
		str.Format("Project Comment:  %s\nAssociation Option: %s\nClassification Option: %s\nCreation Date/Time: %s\n",
			m_TableName,
			m_FuseAssoRows,			
			m_FuseTypeRows,
			m_FuseTimeRows
			);
	}
	else
	{
		str.Format("Project Comment:  %s\nAssociation Option: %s\nClassification Option: %s\nCreation Date/Time: %s\n",
			m_TableName,
			m_FuseAssoCols,			
			m_FuseTypeCols,
			m_FuseTimeCols
			);
	}

	str += "Label,Group,Distance to centroid\n";

	return str;
}

long CPatnDoc::CreateTableFromDOSPRM(CString strFile)
{
	
	this->m_loading = true;

	//open the file in old format
	FILE * fp;
	fp = fopen(strFile,"r");
	if(!fp)
		return 0;

//	int header; //4byte;
	char comment[81]; //80 bytes;
	char name[44]; //43 bytes;
	int rows,cols; //12byte * 2;
	float missingvalue; //12.5byte;
	int unknown; //don't care, 3byte;
	char crap[8];

	fseek(fp,0,SEEK_END);
	fpos_t len;
	long length;
	fgetpos(fp,&len);
	length = long(len);

	fseek(fp,0,SEEK_SET);

	if(length == 155) //new DOSPATN
		fread(crap,sizeof(char),4,fp);
	else //old DOSPATN
		fread(crap,sizeof(char),3,fp);
	fread(comment,sizeof(char),80,fp); comment[80] = 0;
	fread(name,sizeof(char),43,fp); name[43] = 0;
	fread(&rows,sizeof(int),1,fp);
	fread(&cols,sizeof(int),1,fp);
	fread(&unknown,sizeof(int),1,fp);
	fread(&unknown,sizeof(int),1,fp);
	fread(&unknown,sizeof(int),1,fp);
	fread(&missingvalue,sizeof(float),1,fp);
	fread(&unknown,sizeof(int),1,fp);

	//

	fclose(fp);

	SetSizes(1, 1); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(1, 12, 2);
	SetInitialRowAttributes(1, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(2);

	//set missing value
	this->SetMissingValue(missingvalue);
;
	long idxCol, idxRow;
//	ROW* pRow;
	vector* nVector;
	long i = 0;
	// initialize column totals
	for ( idxCol = 0; idxCol < 1; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < 1; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}
	
	//open data file
	//get path & attach filename
	fp = fopen(name,"r");	
	if(!fp)
	{
		CString sname = CString(name);
		sname = sname.Trim();
		sname = sname + CString(".dat");
		fp = fopen(sname,"r");
		if(!fp)
			return 0;
	}

	//read crap at start of file
	if(length == 155) //new DOSPATN
		fread(&unknown,sizeof(int),1,fp);
	else
		fread(crap,sizeof(char),2,fp);

	//for each row, then column
	for(long i=0;i<rows;i++)
	{
		for(long j=0;j<cols;j++)
		{
			//assert cell
			this->assertCell(i+1,j+1);
			float value;
			fread(&value,sizeof(float),1,fp);
			this->SetValueAt(j,i,value);
			long err;
			if(length == 155)
				err = fseek(fp,4*(i*cols+j)+4+4,SEEK_SET);
			else
				err = fseek(fp,4*(i*cols+j)+2+4,SEEK_SET);
		}		
	}

	fclose(fp);

	//open row labels
	CString newname = name;
	newname = newname.Trim();
	newname = newname.Left(newname.GetLength()-3);
	newname += "rlb";

	char rl[9];
	rl[8] = 0;

	fp = fopen(newname,"r");
	if(!fp)
		return 0;
	//crap
	if(length == 155)
		fread(&unknown,sizeof(int),1,fp);
	else
		fread(crap,sizeof(char),5,fp);

    for(long i=0;i<rows;i++)
	{
		fread(rl,sizeof(char),8,fp);
		this->SetRowLabel(i,rl);
		fseek(fp,0,SEEK_CUR);
	}
	fclose(fp);

	newname = newname.Left(newname.GetLength()-3);
	newname += "clb";

	char cl[9];
	cl[8] = 0;

	fp = fopen(newname,"r");
	if(!fp)
		return 0;
	//crap
	if(length == 155)
		fread(&unknown,sizeof(int),1,fp);
	else
		fread(crap,sizeof(char),5,fp);
	for(long i=0;i<cols;i++)
	{
		fread(cl,sizeof(char),8,fp);
		this->SetColumnLabel(i,cl);
		fseek(fp,0,SEEK_CUR);
	}
	fclose(fp);

	this->m_loading = false;
	UpdateIntrinsicTotals();
	UpdateAllTotals();
	return 1;
}

long CPatnDoc::CreateTableFromDOSARC(CString strFile)
{
	this->m_loading = true;
	//open the file in old format
	FILE * fp;
	fp = fopen(strFile,"r");
	if(!fp)
		return 0;

	char comment[83]; //80 bytes; +2
	char name[46]; //43 bytes; +2
	long rows,cols; //12byte * 2;
	float missingvalue; //12.5byte;
	long unknown; //don't care, 3byte;

	fgets(comment,82,fp);
	fgetc(fp);
	fgets(name,45,fp);
	fgetc(fp);
	long length;
	char line[200];
	fgets(line,199,fp);
	fgetc(fp);
	long offset = CString(line).Find("(")+1;
	
	//now reading free format
	//read = sscanf(line+offset,"%dF%d.%d",&count,&length,&decimals);
	//char c;
	//while((c=fgetc(fp)) != EOF && c != '\n');

	//if(read == 2)
	//	decimals = 0;
int i;
	//read line?
	fgets(line,7,fp);line[7] = 0;
	rows = atoi(line);
	fgets(line,7,fp);line[7] = 0;
	cols = atoi(line);
	fgets(line,7,fp);line[7] = 0;
	unknown = atoi(line);
	fgets(line,7,fp);line[7] = 0;
	unknown = atoi(line);
	fgets(line,7,fp);line[7] = 0;
	unknown = atoi(line);
	fgets(line,80,fp);
	missingvalue = (float) atof(line);

	SetSizes(1, 1); // set sizes for the various arrays

	// set row & column attributes
	SetInitialColumnAttributes(1, 12, 2);
	SetInitialRowAttributes(1, 1 /*dummy value - not used by view*/);

	// set row & column totals & alloc attributes
	SetInitialColumnTotalsRowAttributes();
	SetInitialRowTotalsColumnAttributes(2);

	//set missing value
	this->SetMissingValue(missingvalue);

//	double nValue;
	long idxCol, idxRow;
//	ROW* pRow;
	vector* nVector;

	// initialize column totals
	for ( idxCol = 0; idxCol < 1; idxCol++ )
	{
		nVector  = new vector;
		for(i=0;i<4;i++) nVector->Add(ANALYSIS_VALUE);
		m_aColumnSums[idxCol] = 0.0;
		m_aColumnCountPositives[idxCol] = 0;
		m_aColumnMinimum[idxCol] = m_nMissingValue;
		m_aColumnMean[idxCol] = m_nMissingValue;
		m_aColumnMaximum[idxCol] = m_nMissingValue;
		m_aColumnCountMissingValues[idxCol] = 0;
		m_aColumnGroups[idxCol] = -1;
		if(m_Vectors[idxCol])
			delete m_Vectors[idxCol];
		m_Vectors[idxCol] = nVector;
	}

	// initialize row totals
	for ( idxRow = 0; idxRow < 1; idxRow++ )
	{
		m_aRowSums[idxRow] = 0.0;
		m_aRowCountPositives[idxRow] = 0;
		m_aRowMinimum[idxRow] = m_nMissingValue;
		m_aRowMean[idxRow] = m_nMissingValue;
		m_aRowMaximum[idxRow] = m_nMissingValue;
		m_aRowCountMissingValues[idxRow] = 0;
		m_aRowGroups[idxRow] = -1;
		m_aRowGroupsDist[idxRow] = m_nMissingValue;
		m_aRowXCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowYCoords[idxRow] = ANALYSIS_VALUE;
		m_aRowZCoords[idxRow] = ANALYSIS_VALUE;
		m_PatternObjects[idxRow] = -1;
	}
	
	
	//for each row, then column
	char * s;
	length = 256;
	s = new char[length+1];
	s[length] = 0;

	for(long i=0;i<rows;i++)
	{
		//read new line char
		for(long j=0;j<cols;j++)
		{
			//assert cell
			this->assertCell(i+1,j+1);
			float value;

			bool found = false;
			for(long k=0;k<length && (k == 0 || s[k] != EOF);k++)
			{
				s[k] = fgetc(fp);
				//if(s[k] == '\n')
				//{
				//	s[k] = fgetc(fp);
				//}			
				if(s[k] != ' ' && s[k] != '\n') found = true;
				else if((s[k] == ' ' || s[k] == '\n') && found){s[k] = '\0'; break;}
			}
	
			value = (float)atof(s);

			//determine how to use the value
			CString str = s;
			if(str.Find(".") < 0)
			{
				//value /= (float)pow(10,decimals);
			}		

			this->SetValueAt(j,i,value);
		}	

		fseek(fp,0,SEEK_CUR);
	}

//	fgetc(fp);//newline char
	//read size 8 labels
	char label[9];
	label[8] = 0;
	label[0] = 1;
	for(long i=0;i<rows && label[0] != EOF;i++)
	{
		for(long j=0;j<8;j++)
		{
			label[j] = fgetc(fp);
			if(label[j] == '\n')
				label[j] = fgetc(fp);
		}
		this->SetRowLabel(i,label);
	}
	//fgetc(fp);//newline char
	//read size 8 labels
	for(long i=0;i<cols && label[0] != EOF;i++)
	{
		for(long j=0;j<8;j++)
		{
			label[j] = fgetc(fp);
			if(label[j] == '\n')
				label[j] = fgetc(fp);
		}
		this->SetColumnLabel(i,label);
	}

	fclose(fp);
/*
	//open row labels
	CString newname = name;
	newname = newname.Trim();
	newname = newname.Left(newname.GetLength()-3);
	newname += "rlb";

	char rl[9];
	rl[8] = 0;

//	fp = fopen(newname,"r");
//	if(!fp)
//		return 0;
	//crap
	fread(&unknown,sizeof(long),1,fp);
	for(i=0;i<rows;i++)
	{
		fread(rl,sizeof(char),8,fp);
		this->SetRowLabel(i,rl);
		fseek(fp,0,SEEK_CUR);
	}
//	fclose(fp);

	newname = newname.Left(newname.GetLength()-3);
	newname += "clb";

	char cl[9];
	cl[8] = 0;

//	fp = fopen(newname,"r");
//	if(!fp)
//		return 0;
	//crap
	fread(&unknown,sizeof(long),1,fp);
	for(i=0;i<cols;i++)
	{
		fread(cl,sizeof(char),8,fp);
		this->SetColumnLabel(i,cl);
		fseek(fp,0,SEEK_CUR);
	}
//	fclose(fp);
*/
	this->m_loading = false;
	UpdateIntrinsicTotals();
	UpdateAllTotals();
	return 1;
}


void CPatnDoc::OnViewPccresults()
{
	//get tableview window - get tableview frame - get lower frame - set focus to PCC tab

	POSITION pos = this->GetFirstViewPosition();
	CView * pView = NULL;
	while(pos != NULL){
		pView = this->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
			break;
		}
		pView = NULL;
	}
	if(pView != NULL)
	{
		CFrameWnd * fw = pView->GetParentFrame();
		if(fw != NULL && fw->IsKindOf(RUNTIME_CLASS(CTableFrame)))
		{
			CTableFrame * tf = (CTableFrame*) fw;
//			((CTableFrame*)tf->GetParentFrame())->m_wndSplitter1.SetRowInfo(1,100,0);
			CColumnTotalsView *pColumnTotalsView = (CColumnTotalsView*)(tf->m_wndSplitter1.GetPane(1,0));
			CPoint p = pColumnTotalsView->m_Tabs.WhereIsTab(4);
			pColumnTotalsView->ButtonAt(p);
			fw->BringWindowToTop();
			fw->SetFocus();
			
			
		}
	}
	pos = GetFirstViewPosition();
	while(pos != NULL){
		pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)) ){
			CRect rect;
			pView->GetParentFrame()->GetWindowRect(&rect);
			long rh;
			rh = ((CTableView*)pView)->m_nRowHeight;
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.SetRowInfo(0,rect.Height()-(int)(rh*7.5),0);
			//((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.SetRowInfo(1,mr_coltotals.Height(),0);
			((CTableFrame*)pView->GetParentFrame())->m_wndSplitter1.RecalcLayout();
		}
	}

	
}

void CPatnDoc::OnUpdateViewPccresults(CCmdUI *pCmdUI)
{
	if(m_PccVectors.GetSize() && this->m_Vectors.GetSize() > m_PccVectors[0])
	{
		pCmdUI->Enable(true);
	}
	else
	{
		pCmdUI->Enable(false);
	}
}

void CPatnDoc::OnViewScatterplot33034()
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	pApp->OpenScatterplot(this);
}

void CPatnDoc::OnUpdateViewScatterplot33034(CCmdUI *pCmdUI)
{
	if(m_RowSelection != NULL && (this->m_RowSelection->GetCount() == 2 || m_ColSelection->GetCount() == 2))
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) 
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
void CPatnDoc::OnImportApriorirowgroupcomposition()
{

	if(m_kw_patn_generated_groups) m_KWExists = false;

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	CString strRow;
	CString strLabel;
	CStringArray aLabels;
	long count;
	int left, right;

	if ( dlg.DoModal() != IDOK )
		return ;

	CWaitCursor waitcursor;

	strFile = dlg.GetPathName();

	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_YESNO );
		return ;
	}

	// fill table with data
	count = 0;
	left = 0;
	count = (long)io.GetLength();
	count = 0;

	CArray<long> tmp;
	tmp.SetSize(m_aRowGroups.GetSize());

	bool found = false;

	while(io.ReadString(strRow) &&
		count < m_aRowGroups.GetSize()){

		//check for patn saved file
		if(strRow.Find("Project Comment",0) == 0){
			io.ReadString(strRow); //assoc method
			io.ReadString(strRow); //class method
			io.ReadString(strRow); //time
			io.ReadString(strRow); //column headers
			found = true;
			continue;
		}

		right = left = 0;
		int rowcount = 0;
		while(left <= strRow.GetLength() && 
			count < m_aRowGroups.GetSize()){
			// get each element			
	 		while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' || 
				 strRow[left] == '\t')) 
				left++;
			right = left;
			while ( right < strRow.GetLength() && strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t')
				right++;

            if(right <= strRow.GetLength()){
				strLabel = strRow.Mid(left, right - left);

				if(rowcount == 0 && found)
				{
					rowcount ++;
				}
				else
				{
					tmp[count] = atoi(strLabel);
					if(tmp[count] == 0)
						tmp[count] = -1;
					count++;
					rowcount = 0;
				}
			}

			left = right+1;
		}
	}
	io.Close();
	long len = 0;
	for(long i=0;i<tmp.GetSize();i++)
	{
		if(tmp[i] > len)
			len = tmp[i];
	}
	len++;

	for(i=0;i<m_RowControlGroups.GetSize();i++)
		delete m_RowControlGroups[i];
	m_RowControlGroups.SetSize(len);
	for(i=0;i<len;i++)
		m_RowControlGroups[i] = new Rows;
	
	for(i=0;i<tmp.GetSize();i++)
	{
		if(tmp[i] > 0 && tmp[i] <= m_RowControlGroups.GetSize())
			m_RowControlGroups[tmp[i]-1]->Add(i);
	}

	//m_FuseAssoRows = "None";
	//m_FuseTypeRows.Format("Imported file %s",strFile);
	//m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");

	m_log += "\r\nRow a priori group composition import from file: \r\n\t" + strFile + "\r\n\r\n";

	UpdateAprioriGroupsColours();

	this->UpdateAllViews(NULL);



	return;	
}

void CPatnDoc::OnExportanalysisdataApriorirowgroupcomposition()
{
	UpdateAprioriGroupsColours();

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	long n;

	strTitle = "Save Row A Priori Group Composition:";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
	}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " APriori Groups.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		
		return;
	}
	
	//f.WriteString(getExportGroupCompositionHeader(true));
	CString str;
	str.Format("Project Comment:  %s\nAssociation Option: N/A\nClassification Option: N/A\nCreation Date/Time: N/A\n",
			m_TableName);
	f.WriteString(str);
	f.WriteString("Label,Group\n");

	for(n=0;n<m_AprioriGroups.GetSize();n++){		
		GetRowLabel(n,str);
		strLabel.Format("%s,%d\n",str,m_AprioriGroups[n]);
		f.WriteString(strLabel);
	}

	

	f.Close();	

	this->UpdateAllViews(NULL);
}

void CPatnDoc::UpdateAprioriGroupsColours()
{	
	m_AprioriGroups.SetSize(m_aRowGroups.GetSize());
	for(long i=0;i<m_RowControlGroups.GetSize();i++)
	{
		for(long j=0;j<m_RowControlGroups[i]->GetSize();j++)
		{
			long p = m_RowControlGroups[i]->GetAt(j);
			if(p >= 0 && p < m_AprioriGroups.GetSize())
			{
				m_AprioriGroups[p] = i+1;
				if(m_AprioriGroups[p] == 0)
					m_AprioriGroups[p] = -1;
			}
		}
	}

	m_aAprioriGroupColours.SetSize(m_RowControlGroups.GetSize());

	for(i=0;i<m_aAprioriGroupColours.GetSize();i++)
	{
		m_aAprioriGroupColours[i].red = m_aAprioriGroupColours[i].blue = m_aAprioriGroupColours[i].green =0;
	}

	colour clr;
	clr.red = clr.blue = clr.green = 0;

	for(i=m_aGroupColours2.GetSize();i<m_RowControlGroups.GetSize();i++)
	{
		m_aGroupColours2.Add(clr);
	}
	//m_aPrioriUserGroupColours.SetSize(m_RowControlGroups.GetSize());

	for(i=0;i<this->m_aObjectColours.GetSize() && i < m_AprioriGroups.GetSize();i++)
	{
		if(m_aObjectColours[i].red == GetMissingValue())
			break;

		if(m_AprioriGroups[i] >= 0 && m_AprioriGroups[i] <= m_aAprioriGroupColours.GetSize() &&
			m_AprioriGroups[i] > 0)
		{
			m_aAprioriGroupColours[m_AprioriGroups[i]-1].red += (float)m_aObjectColours[i].red;
			m_aAprioriGroupColours[m_AprioriGroups[i]-1].green += (float)m_aObjectColours[i].green;
			m_aAprioriGroupColours[m_AprioriGroups[i]-1].blue += (float)m_aObjectColours[i].blue;
		}        		
	}	
	for(i=0;i<m_aAprioriGroupColours.GetSize() && i < m_RowControlGroups.GetSize();i++)
	{
		m_aAprioriGroupColours[i].red /= (float)m_RowControlGroups[i]->GetSize();
		m_aAprioriGroupColours[i].green /= (float)m_RowControlGroups[i]->GetSize();
		m_aAprioriGroupColours[i].blue /= (float)m_RowControlGroups[i]->GetSize();
	}

	while(m_PatternApriori.GetSize() < m_RowControlGroups.GetSize())
		m_PatternApriori.Add(m_PatternApriori.GetSize());
	m_PatternApriori.SetSize(m_RowControlGroups.GetSize());

	this->UpdateAllViews(NULL);
}
void CPatnDoc::OnUpdateExportanalysisdataApriorirowgroupcomposition(CCmdUI *pCmdUI)
{
	if(m_RowControlGroups.GetSize() > 1)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(false);
}

void CPatnDoc::OnCloseDocument()
{

	COleServerDoc::OnCloseDocument();
}

void CPatnDoc::OnUpdateImportanalysisdataColumnlower(CCmdUI *pCmdUI)
{

}

void CPatnDoc::OnImportanalysisdataColumnlower()
{

	
	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);

	if ( dlg.DoModal() != IDOK )
		return ;

	strFile = dlg.GetPathName();

	//setup
	long i,j;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	CArray<double> aRanges;

	m_AssocColChange = 1;
	m_AssocMatrixCols.Reset();

	if ( IsExtrinsicColumns() )
		nCols = GetIdxLeftExtrinsicColumn();
	else
		nCols = GetNoOfColumns();

	if ( IsExtrinsicRows() )
		nRows = GetIdxTopExtrinsicRow();
	else
		nRows = GetNoOfRows();


	aRanges.SetSize(nCols);
	m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	for(i=1;i<nCols;i++)
	{
		pAMR = new CAssocMatrixRow();

		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{			
			pAMR->SetAt(j,0);
		}
		m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}

	//read 
	CString strLabel,strRow;
	int left, right;
		
	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return;
	}

	i = 0; j = 0;
	bool emptycol = false;
	while(i<m_AssocMatrixCols.m_aMatrix.GetSize() && io.ReadString(strRow))
	{
		if(i == 0 && j == 0)
		{//first time, check if exported from here
			if(strRow.Find("Project Comment") == 0)
			{
				io.ReadString(strRow);
				io.ReadString(strRow);
				io.ReadString(strRow);
				emptycol = true;
				io.ReadString(strRow);
			}
		}

		right = left = 0;
		while (i<m_AssocMatrixCols.m_aMatrix.GetSize() &&  right < strRow.GetLength() )
		{
			while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t' || strRow[left] == ';') )
				left++;

			if ( left >= strRow.GetLength() ) // last field has been processed
				break;

			right = left;
			while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t' && strRow[right] != ';'))
				right++;

			strLabel = strRow.Mid(left, right - left);
			if(!emptycol || left != 0)
			{
				//add value
				m_AssocMatrixCols.m_aMatrix[i]->SetAt(j,atof(strLabel));
				//increament counters
				if(j == i)
				{
					//new line
					i = i + 1;
					j = 0;
				}
				else
				{
					//same line
					j = j + 1;
				}
			}

			left = right;
		}
		
	}
	io.Close();

	m_ANOSIMColsExists = true;
	m_FuseColChange = 1;
	//reset column fusion because its association changed
	for(i=0;i<m_FuseTableCols.GetSize();i++)
		delete m_FuseTableCols[i];
	m_FuseTableCols.RemoveAll();
	m_FuseTableCols.SetSize(0);

	m_log += "\r\nColumn association matrix import from file: \r\n\t" + strFile + "\r\n\r\n";
	m_AssocMatrixCols.m_strMeasure = "Imported (Columns)";
	CTime time = CTime::GetCurrentTime();
	m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");
	m_AssocColChange = 0;

}

void CPatnDoc::OnImportanalysisdataRowlower()
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);

	if ( dlg.DoModal() != IDOK )
		return ;

	ImportanalysisdataRowlower(dlg.GetPathName());
}

void CPatnDoc::ImportanalysisdataRowlower(CString strFile)
{
	CStdioFile io;

	//setup
	long i,j;
	long nRows, nCols;
	CAssocMatrixRow* pAMR;
	CArray<double> aRanges;

	m_AssocRowChange = 1;
	m_AssocMatrixRows.Reset();

	if ( IsExtrinsicColumns() )
		nCols = GetIdxLeftExtrinsicColumn();
	else
		nCols = GetNoOfColumns();

	if ( IsExtrinsicRows() )
		nRows = GetIdxTopExtrinsicRow();
	else
		nRows = GetNoOfRows();


	aRanges.SetSize(nRows);
	m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	for(i=1;i<nRows;i++)
	{
		pAMR = new CAssocMatrixRow();

		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{			
			pAMR->SetAt(j,0);
		}
		m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}

	//read 
	CString strLabel,strRow;
	int left, right;
		
	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return;
	}

	i = 0; j = 0;	
	bool emptycol = false;
	while(i<m_AssocMatrixRows.m_aMatrix.GetSize() && io.ReadString(strRow))
	{
		if(i == 0 && j == 0)
		{//first time, check if exported from here
			if(strRow.Find("Project Comment") == 0)
			{
				io.ReadString(strRow);
				io.ReadString(strRow);
				io.ReadString(strRow);
				emptycol = true;
				io.ReadString(strRow);
			} else if (strRow.GetAt(0) == ',' || strRow.GetAt(0) == '\t') {
				//read first row as labels, flag to ignore first column.
				io.ReadString(strRow);
				emptycol = true;
			}
		}
		right = left = 0;
		while (i<m_AssocMatrixRows.m_aMatrix.GetSize() &&  right < strRow.GetLength() )
		{
			while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t' || strRow[left] == ';') )
				left++;

			if ( left >= strRow.GetLength() ) // last field has been processed
				break;

			right = left;
			while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t' && strRow[right] != ';'))
				right++;

			strLabel = strRow.Mid(left, right - left);
			if(!emptycol || left != 0)
			{
				
				//add value
				m_AssocMatrixRows.m_aMatrix[i]->SetAt(j,atof(strLabel));
				//increament counters
				if(j == i)
				{
					//new line
					i = i + 1;
					j = 0;
				}
				else
				{
					//same line
					j = j + 1;
				}
			}

			left = right;
		}
		
	}
	io.Close();

	m_ANOSIMRowsExists = true;
	m_FuseRowChange = 1;
	//reset column fusion because its association changed
	for(i=0;i<m_FuseTableRows.GetSize();i++)
		delete m_FuseTableRows[i];
	m_FuseTableRows.RemoveAll();
	m_FuseTableRows.SetSize(0);

	m_log += "\r\nRow association matrix import from file: \r\n\t" + strFile + "\r\n\r\n";
	m_AssocMatrixRows.m_strMeasure = "Imported (Rows)";
	CTime time = CTime::GetCurrentTime();
	m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");
	m_AssocRowChange = 0;

}

void CPatnDoc::OnUpdateImportanalysisdataRowlower(CCmdUI *pCmdUI)
{

}

void CPatnDoc::OnImportanalysisdataColumndendogramorder()
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);

	if ( dlg.DoModal() != IDOK )
		return ;

	strFile = dlg.GetPathName();

	//setup
	long i,j;
	long nRows, nCols;
	

	if ( IsExtrinsicColumns() )
		nCols = GetIdxLeftExtrinsicColumn();
	else
		nCols = GetNoOfColumns();

	if ( IsExtrinsicRows() )
		nRows = GetIdxTopExtrinsicRow();
	else
		nRows = GetNoOfRows();


	m_DendOrderCols.SetSize(nCols);
	for(i=0;i<nCols;i++)
		m_DendOrderCols[i] = i;

	//read 
	CString strLabel,strRow;
	int left, right;
		
	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return;
	}

	i = 0; j = 0;	
	while(i<m_DendOrderCols.GetSize() && io.ReadString(strRow))
	{
		right = left = 0;
		while (i<m_DendOrderCols.GetSize() &&  right < strRow.GetLength() )
		{
			while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t' || strRow[left] == ';') )
				left++;

			if ( left >= strRow.GetLength() ) // last field has been processed
				break;

			right = left;
			while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t' && strRow[right] != ';'))
				right++;

			strLabel = strRow.Mid(left, right - left);
			//add value
			if(m_DendOrderCols.GetSize() > atoi(strLabel)-1 && atoi(strLabel)-1 >= 0)
			{
				m_DendOrderCols[atoi(strLabel)-1] = i++;
			}			

			left = right;
		}
		
	}
	io.Close();

	CDend dend;
	if(!m_FuseColChange && this->m_FuseTableCols.GetSize())
	{
		dend.SetDendTable(&m_FuseTableCols,
			&m_DendOrderCols,
			&m_DendTableCols);
	}

}

void CPatnDoc::OnImportanalysisdataRowdendogramorder()
{

	CStdioFile io;
	CString strFile;
	CFileDialog dlg(TRUE,NULL,"*.csv",OFN_HIDEREADONLY,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);

	if ( dlg.DoModal() != IDOK )
		return ;

	strFile = dlg.GetPathName();

	//setup
	long i,j;
	long nRows, nCols;
	

	if ( IsExtrinsicColumns() )
		nCols = GetIdxLeftExtrinsicColumn();
	else
		nCols = GetNoOfColumns();

	if ( IsExtrinsicRows() )
		nRows = GetIdxTopExtrinsicRow();
	else
		nRows = GetNoOfRows();


	m_DendOrderRows.SetSize(nRows);
	for(i=0;i<nRows;i++)
		m_DendOrderRows[i] = i;

	//read 
	CString strLabel,strRow;
	int left, right;
		
	if ( !io.Open(strFile,CFile::modeRead) )
	{
		AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
				MB_ICONSTOP | MB_OK );
		return;
	}

	i = 0; j = 0;	
	while(i<m_DendOrderRows.GetSize() && io.ReadString(strRow))
	{		
		right = left = 0;
		while (i<m_DendOrderRows.GetSize() &&  right < strRow.GetLength() )
		{
			while ( left < strRow.GetLength() &&
				(strRow[left] == '\"' || strRow[left] == '\'' || strRow[left] == ',' ||
				strRow[left] == '\t' || strRow[left] == ';') )
				left++;

			if ( left >= strRow.GetLength() ) // last field has been processed
				break;

			right = left;
			while ( right < strRow.GetLength() && (strRow[right] != '\"' && strRow[right] != '\'' && strRow[right] != ',' &&
				strRow[right] != '\t' && strRow[right] != ';'))
				right++;

			strLabel = strRow.Mid(left, right - left);
			//add value
			if(m_DendOrderRows.GetSize() > atoi(strLabel)-1 && atoi(strLabel)-1 >= 0)
			{
				m_DendOrderRows[atoi(strLabel)-1] = i++;
			}			

			left = right;
		}
		
	}
	io.Close();

	CDend dend;
	if(!m_FuseRowChange && this->m_FuseTableRows.GetSize())
	{
		dend.SetDendTable(&m_FuseTableRows,
			&m_DendOrderRows,
			&m_DendTableRows);
	}

}

void CPatnDoc::OnExportevaluationdataColumngroupstatistics()
{
	CGsta gsta(this);
	gsta.SaveColumnGsta(this);
}


void CPatnDoc::OnDataCreatenewvariablesrepresentinggroups()
{
	long TwayChange = m_TwayChange;
	long AssocRowChange = m_AssocRowChange;
	long AssocColChange = m_AssocColChange;
	long FuseRowChange = m_FuseRowChange;
	long FuseColChange = m_FuseColChange;
	long OrdChange = m_OrdChange;
	
	//make data invalid - to be reversed at end
	m_TwayChange = 1;
	m_AssocRowChange = 1;
	m_AssocColChange = 1;
	m_FuseRowChange = 1;
	m_FuseColChange = 1;
	m_OrdChange = 1;
	

	//for each group in m_ColGroups, sum the values to generate new columns.  
	long i;
	double sum;
	long non_missing;

	m_loading = true;

	for(i=0;i<m_Cols.GetSize();i++)
	{
		//append column
		this->AddColumn();
		long col = GetNoOfColumns()-1;

		for(long j=0;j<GetNoOfRows();j++)
		{
			non_missing = 0;
			sum = 0;			
			for(long k=0;k<m_Cols[i]->GetSize();k++)
			{
				if(GetValueAt(m_Cols[i]->GetAt(k),j) != GetMissingValue())
				{
					sum += GetValueAt(m_Cols[i]->GetAt(k),j);
					non_missing++;
				}
			}
			SetValueAt(col,j,sum/(double)((non_missing)?non_missing:1));
		}		

		CString str;
		str.Format("Mean of Group %d",m_ColGroups[i]);
		SetColumnLabel(col,str);
	}

	//make extrinsic
	if(m_idxLeftExtrinsicColumn < 0 || m_idxLeftExtrinsicColumn > GetNoOfColumns() - i)
	{
	//	m_idxLeftExtrinsicColumn = GetNoOfColumns() - i;
		CArray<int> items;
		for(long j=GetNoOfColumns()-i;j<GetNoOfColumns();j++)
			items.Add(j);
		MakeColumnsExtrinsic(&items);
	}
	
	m_loading = false;
	UpdateIntrinsicTotals();
	UpdateAllTotals();

	//update display
	UpdateAllViews(NULL);		

	POSITION pos = GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
		{
			((CTableView*)pView)->autosizecolumns = true;
		}
		pView->RedrawWindow();
	}   

	//revert back to the original settings
	m_TwayChange = TwayChange;
	m_AssocRowChange = AssocRowChange;
	m_AssocColChange = AssocColChange;
	m_FuseRowChange = FuseRowChange;
	m_FuseColChange = FuseColChange;
	m_OrdChange = OrdChange;
}

BOOL CPatnDoc::SaveModified()
{
	return COleServerDoc::SaveModified();
}

void CPatnDoc::RunRecipe(CString recipe)
{
	//first setup the defaults for the analysis recipe, don't automatically run anything

	//apply settings
	long i,p,n;
	i = -1;
	CString ws[10];
	
	//find -ARA
	i = recipe.Find("-ARA ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "GowerMetric")
			{
				m_AnalysisRecipe.rows.nAssocMeasure = CAssociation::GowerMetric;
				
			}else if(ws[0] == "BrayCurtis")
			{	m_AnalysisRecipe.rows.nAssocMeasure = CAssociation::BrayCurtis;
			
			}else if(ws[0] == "Minkowksi")
			{	m_AnalysisRecipe.rows.nAssocMeasure = CAssociation::MinkowskiSeries;
				if(p>1) m_AnalysisRecipe.rows.nMinkConst = atof(ws[1]);
				else m_AnalysisRecipe.rows.nMinkConst = 2;
				if(m_AnalysisRecipe.rows.nMinkConst == 0)
				{
					m_AnalysisRecipe.rows.nMinkConst = 2;
				}
			
			}else if(ws[0] == "TwoStep")
			{	m_AnalysisRecipe.rows.nAssocMeasure = CAssociation::TwoStep;
			
			}else if(ws[0] == "Kulzinski")
			{	m_AnalysisRecipe.rows.nAssocMeasure = CAssociation::Kulzinski;
			
			}else
			{
				m_AnalysisRecipe.rows.nAssocMeasure = 0;
			}			
		}
	}

	//find -ACC
	i = recipe.Find("-ACC ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "FusionNN")
			{
				m_AnalysisRecipe.cols.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.cols.nFuseTechnique = CFuse::NearestNeighbour;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.cols.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.cols.nBeta= 8;
				
			}else if(ws[0] == "FusionFN")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.cols.nFuseTechnique = CFuse::FurthestNeighbour;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
				
				if(p>2) m_AnalysisRecipe.cols.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.cols.nBeta= 8;
				

			
			}else if(ws[0] == "FusionUPGMA")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.cols.nFuseTechnique = CFuse::FlexibleUPGMA;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.cols.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.cols.nBeta= 8;
			
			}else if(ws[0] == "FusionWPGMA")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.cols.nFuseTechnique = CFuse::FlexibleWPGMA;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.cols.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.cols.nBeta= 8;
			
			}else if(ws[0] == "AlocBrayCurtis")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.cols.nAllocAssocMeasure = CAssociation::BrayCurtis;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocGowerMetric")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.cols.nAllocAssocMeasure = CAssociation::GowerMetric;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocMinkowski")
			{	m_AnalysisRecipe.cols.nClassStrategy = CClassification::alloc;
			m_AnalysisRecipe.cols.nAllocAssocMeasure = CAssociation::MinkowskiSeries;
				if(p>1) m_AnalysisRecipe.cols.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.cols.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.cols.dPower = atof(ws[2]);
				else m_AnalysisRecipe.cols.dPower = 2;
			
			}else
			{
				m_AnalysisRecipe.cols.nClassStrategy = CClassification::none;
			}			
		}
	}
	if(m_AnalysisRecipe.cols.nNoOfGroups < 2)
		m_AnalysisRecipe.cols.nNoOfGroups = 8;
	if(m_AnalysisRecipe.cols.dPower == 0)
		m_AnalysisRecipe.cols.dPower = 2;

	//find -ARC
	i = recipe.Find("-ARC ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "FusionNN")
			{
				m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::NearestNeighbour;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
				
			}else if(ws[0] == "FusionFN")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FurthestNeighbour;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
				

			
			}else if(ws[0] == "FusionUPGMA")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FlexibleUPGMA;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
			
			}else if(ws[0] == "FusionWPGMA")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FlexibleWPGMA;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
			
			}else if(ws[0] == "AlocBrayCurtis")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::BrayCurtis;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocGowerMetric")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::GowerMetric;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocMinkowski")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
			m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::MinkowskiSeries;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.dPower = atof(ws[2]);
				else m_AnalysisRecipe.rows.dPower = 2;
			
			}else
			{
				m_AnalysisRecipe.rows.nClassStrategy = CClassification::none;
			}			
		}
	}
	if(m_AnalysisRecipe.rows.nNoOfGroups < 2)
		m_AnalysisRecipe.rows.nNoOfGroups = 8;
	if(m_AnalysisRecipe.rows.dPower == 0)
		m_AnalysisRecipe.rows.dPower = 2;

	//find -ARC
	i = recipe.Find("-ARC ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n+1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "FusionNN")
			{
				m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::NearestNeighbour;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
				
			}else if(ws[0] == "FusionFN")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FurthestNeighbour;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
			
			}else if(ws[0] == "FusionUPGMA")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FlexibleUPGMA;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
			
			}else if(ws[0] == "FusionWPGMA")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::fuse;
				m_AnalysisRecipe.rows.nFuseTechnique = CFuse::FlexibleWPGMA;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.nBeta= atof(ws[2]);
				else m_AnalysisRecipe.rows.nBeta= 8;
			
			}else if(ws[0] == "AlocBrayCurtis")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::BrayCurtis;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocGowerMetric")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
				m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::GowerMetric;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
			
			}else if(ws[0] == "AlocMinkowski")
			{	m_AnalysisRecipe.rows.nClassStrategy = CClassification::alloc;
			m_AnalysisRecipe.rows.nAllocAssocMeasure = CAssociation::MinkowskiSeries;
				if(p>1) m_AnalysisRecipe.rows.nNoOfGroups= atoi(ws[1]);
				else m_AnalysisRecipe.rows.nNoOfGroups = 8;
				if(p>2) m_AnalysisRecipe.rows.dPower = atof(ws[2]);
				else m_AnalysisRecipe.rows.dPower = 2;
			
			}else
			{
				m_AnalysisRecipe.rows.nClassStrategy = CClassification::none;
			}			
		}
	}
	if(m_AnalysisRecipe.rows.nNoOfGroups < 2)
		m_AnalysisRecipe.rows.nNoOfGroups = 8;
	if(m_AnalysisRecipe.rows.dPower == 0)
		m_AnalysisRecipe.rows.dPower = 2;

	//find -ARO
	i = recipe.Find("-ARO ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "SSH")
			{
				m_AnalysisRecipe.rows.nOrdination = true;
				if(p>1) m_AnalysisRecipe.rows.nOrd3d = atoi(ws[1]);
				else m_AnalysisRecipe.rows.nOrd3d = 3;
				if(p>2) m_AnalysisRecipe.rows.nOrdIterations = atoi(ws[2]);
				else m_AnalysisRecipe.rows.nOrdIterations= 50;
				if(p>3) m_AnalysisRecipe.rows.nOrdRandomStarts = atoi(ws[3]);
				else m_AnalysisRecipe.rows.nOrdRandomStarts= 10;
				if(p>4) m_AnalysisRecipe.rows.nOrdSeed = atoi(ws[4]);
				else m_AnalysisRecipe.rows.nOrdSeed = 1234;
				if(p>5) m_AnalysisRecipe.rows.nCut = atof(ws[5]);
				else m_AnalysisRecipe.rows.nCut = .05;
			}else
			{	m_AnalysisRecipe.rows.nOrdination = false;
			}						
		}
	}
	else
	{
			m_AnalysisRecipe.rows.nOrdination = false;
	}

	//find -ACA	
	i = recipe.Find("-ACA ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			if(ws[0] == "GowerMetric")
			{
				m_AnalysisRecipe.cols.nAssocMeasure = CAssociation::GowerMetric;
				
			}else if(ws[0] == "BrayCurtis")
			{	m_AnalysisRecipe.cols.nAssocMeasure = CAssociation::BrayCurtis;
			
			}else if(ws[0] == "Minkowski")
			{	m_AnalysisRecipe.cols.nAssocMeasure = CAssociation::MinkowskiSeries;
				if(p>1) m_AnalysisRecipe.cols.nMinkConst = atof(ws[1]);
				else m_AnalysisRecipe.cols.nMinkConst = 2;
				if(m_AnalysisRecipe.cols.nMinkConst == 0)
					m_AnalysisRecipe.cols.nMinkConst = 2;

			
			}else if(ws[0] == "TwoStep")
			{	m_AnalysisRecipe.cols.nAssocMeasure = CAssociation::TwoStep;
			
			}else if(ws[0] == "Kulzinski")
			{	m_AnalysisRecipe.cols.nAssocMeasure = CAssociation::Kulzinski;
			
			}else
			{
				m_AnalysisRecipe.cols.nAssocMeasure = 0;
			}			
		}
	}


	
	//init by user columns selection OTHERWISE intrinsics
	
	{
		//add extrinsics to pcc && kw
		//if(m_pDoc->m_PccVectors.GetSize() == 0)
		if(!(GetIdxLeftExtrinsicColumn() < 0 || 
			GetIdxLeftExtrinsicColumn() >=GetNoOfColumns())){
		
			m_PccVectors.RemoveAll();
			m_KWAttributes.RemoveAll();
			for(i=GetIdxLeftExtrinsicColumn();i<GetNoOfColumns();i++){
				m_PccVectors.Add(i);
				m_KWAttributes.Add(i);
			}
		}
	}	

	
	//find -EAC
	i = recipe.Find("-EAC ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			m_createANOSIMCols = true;
			if(p>0)
			{
				m_ColumnAsim_iterations = atoi(ws[0]);
			}
			else
			{
				m_ColumnAsim_iterations = 50;
			}
					
			if(p>1)
			{
				m_ColumnAsim_seed= atoi(ws[1]);
			}
			else
			{
				m_ColumnAsim_seed = 1235;
			}
			if(p>2)
			{
				m_ColumnAsim_pairs = atoi(ws[2]);
			}
			else
			{
				m_ColumnAsim_pairs= false;
			}
			

		}
		else
		{
			m_createANOSIMCols = false;
		}
	}
	//find -EAR
	i = recipe.Find("-EAR ");
	if(i>=0)
	{
		p = 0;
		i = i + 5;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			m_createANOSIMRows = true;
			if(p>0)
			{
				m_RowAsim_iterations = atoi(ws[0]);
			}
			else
			{
				m_RowAsim_iterations = 50;
			}
					
			if(p>1)
			{
				m_RowAsim_seed= atoi(ws[1]);
			}
			else
			{
				m_RowAsim_seed = 1235;
			}
			if(p>2)
			{
				m_RowAsim_pairs = atoi(ws[2]);
			}
			else
			{
				m_RowAsim_pairs= false;
			}
			if(p>4)
			{
				m_RowAsim_patn_groups = atoi(ws[4]);
			}
			else
			{
				m_RowAsim_patn_groups = false;
			}
		}
		else
		{
			m_createANOSIMRows = false;
		}
	}			

	
	//find -EM
	i = recipe.Find("-EM ");
	if(i>=0)
	{
		m_createMCAO = true;
		p = 0;
		i = i + 4;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{

			if(p>0)
			{
				m_MCAOIterations = atoi(ws[0]);
			}
			else
			{
				m_MCAOIterations= 50;
			}
			if(p>1)
			{
				m_MCAOSeed = atoi(ws[1]);
			}
			else
			{
				m_MCAOSeed= 1235;
			}
					
		}
		
	}	
	else
	{
		m_createMCAO= false;
	}


	//find -EP
	i = recipe.Find("-EP ");
	if(i>=0)
	{
		m_createPCC = true;
		p = 0;
		i = i + 4;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			m_createPCC = true;
			
					
		}
		else
		{
			//m_createPCC= false;
		}
	}			


	//find -EK
	i = recipe.Find("-EK ");
	if(i>=0)
	{
					m_createKW = true;
		p = 0;
		i = i + 4;
		n = recipe.Find(" ",i);
		if(n==-1) n=recipe.GetLength();
		while(p<10 && i>=0 && i<recipe.GetLength() && n>i && n+1 < recipe.GetLength() && recipe.GetAt(n+1) != '-' )
		{
			ws[p] = recipe.Mid(i,n-i);
			i = n + 1;
			if(i<recipe.GetLength())
				n = recipe.Find(" ",i)	;
			p = p++;
		}
		if(p<10 &&  i< recipe.GetLength() && n>i)
		{
			ws[p] = recipe.Mid(i,n-i);
			p = p++;
		}
		if(p>0)
		{
			m_createKW = true;
			if(p>0)
			{
				m_kw_patn_generated_groups = atoi(ws[0]);
			}
			else
			{
				m_kw_patn_generated_groups = false;
			}
					
		}
		else
		{
			//m_createKW= false;
		}
	}			

}
void CPatnDoc::OnViewConstfid()
{
	CPatnApp* pApp = (CPatnApp*)AfxGetApp();
	if(m_constfidvisible){
		//AfxGetMainWnd()->MessageBox("Only one Box & Whisker Graph can exist \n at a time.","Message");
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(view->IsKindOf(RUNTIME_CLASS(CConstFidView))){
				view->GetParentFrame()->BringWindowToTop();
				//view->GetParentFrame()->ShowWindow(SW_RESTORE );
				break;
			}
		}
		return;
	}

	{
		POSITION pos = this->GetFirstViewPosition();
		while(pos){
			CView *view = this->GetNextView(pos);
			if(view->IsKindOf(RUNTIME_CLASS(CTableView))){
				CConstancyFidelity dlg(this, (CTableView*) view);
				
				if(dlg.DoModal() == IDOK) {
				
					pApp->OpenConstFidView(this);

					{
						pos = this->GetFirstViewPosition();
						while(pos){
							CView *view = this->GetNextView(pos);
							if(view->IsKindOf(RUNTIME_CLASS(CConstFidView))){
								((CConstFidView*)view)->m_data = dlg.m_data;
								((CConstFidView*)view)->m_colLabels = dlg.m_colLabels;
								((CConstFidView*)view)->m_rowLabels = dlg.m_rowLabels;
								((CConstFidView*)view)->m_isApriori = dlg.m_isApriori;
								((CConstFidView*)view)->m_threashold = dlg.m_dblThreashold;

								break;
							}
						}
					}
				}
				break;
			}
		}
	}
	
}

void CPatnDoc::OnUpdateViewConstfid(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_Groups.GetSize() > 1 || m_aAprioriGroupColours.GetSize() > 1);	
}
