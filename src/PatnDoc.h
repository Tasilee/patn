// PatnDoc.h : interface of the CPatnDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _PATNDOC_H_
#define _PATNDOC_H_

#include "DataTransform.h"
#include "Selection.h"

#define D3D_OVERLOADS
#ifdef DIRECT3D_VERSION
#undef DIRECT3D_VERSION
#define STRICT
#endif
#define DIRECT3D_VERSION 0x0700
#include <math.h>
#include <d3d.h>


#define ANALYSIS_VALUE -9999.9999

BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) ;
void sort(CArray<double>* pData, CArray<long> * idxs,long bAscending);

/////////////////////////////////////////////////////////////////////////////
// helper functions

typedef struct {
	float red;
	float green;
	float blue;
} colour;

struct CAnalysisRecipe
{
	long		nAssocMeasure;
	long		nClassStrategy;
	long		nFuseTechnique;
	long		nAllocAssocMeasure;
	double	nBeta;
	long		nNoOfGroups;
	long		nOrdination;
	long		nOrd3d;
	double	nCut;
	double	nMinkConst;
	double		dPower;
	long		nOrdIterations;
	long		nOrdSeed;
	long		nOrdRandomStarts;

	long		nTunnels;
	long		bWithTunnels;
	long		bCalculateStats;
	double	dMovementFactor;

	long		nOverride;
};

void sort(CArray<int>* pData, long bAscending = TRUE);
long is_contiguous(CArray<long>* pData);

typedef struct CAnalysisRecipe2{
	CAnalysisRecipe rows;
	CAnalysisRecipe cols;
	long nFirstTime;
}CAnalysisRecipe2;

/////////////////////////////////////////////////////////////////////////////
// TextBox - for 3dview & scat
typedef struct TextBox
{
	CString contents;
	CRect location;
	long size;
}TextBox;

typedef struct TextBox2
{
	CString contents;
	CRect location;
	LOGFONT font;
	unsigned long colour;
}TextBox2;


typedef CArray<long> Rows;

//////////////////////////////////////////////
// data structures implemented
typedef struct{
	double minimum;
	double first_quartile;
    double median;
	double mean;
	double third_quartile;
	double maximum;
}Stat;

typedef CArray<Stat*> dblStat;
typedef CArray<double> dblarray;
typedef dblarray Row;

#define NO_VERTEX   -1

typedef CArray<long> half;
typedef CArray<half*> edges;
typedef long vertex;
typedef double weight;

typedef struct 
{
	long   in_mst;     // true if the vertex is in U
	vertex nearest;    // If !in_mst, the nearest vertex in U
	weight cost;       // Weight of corresponding edge
} neighbour;

typedef CArray<double> vector;
typedef CArray<vector*> matrix;
	
/////////////////////////////////////////////////////////////////////////////
// class CRowColumnAttributes
class CRowColumnAttributes : public CObject
{

public:
	CRowColumnAttributes();
	DECLARE_SERIAL(CRowColumnAttributes)
public:
	enum /* used by attrib objects to describe the type of information in the row/column */
	{
		Data,
		Sum,
		Minimum,
		Mean,
		Maximum,
		CountPositive,
		CountMissing,
		Group,
		Association,
		XCoord,
		YCoord,
		ZCoord
	};
	
// Attributes
protected:
	CString	m_strLabel;
	long m_strGroup;
	BOOL	m_bIsHidden;
	UINT	m_nType; // what type of information in this row/column
	
public:
	void	SetLabel(const char* strLabel) { m_strLabel = strLabel; }
	void	GetLabel(CString& str) { str = m_strLabel; }
	void	SetGroup(const long strGroup) { m_strGroup = strGroup; }
	void	GetGroup(long* str) { *str = m_strGroup; }
	void	Hide() { m_bIsHidden = TRUE; }
	void	Unhide() { m_bIsHidden = FALSE; }
	BOOL	IsHidden() { return m_bIsHidden; }
	void	SetType(UINT nType);
	UINT	GetType() { return m_nType; }

public:
	virtual void Serialize(CArchive& ar);
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class CRowAttributes
class CRowAttributes : public CRowColumnAttributes
{


public:
	CRowAttributes();
	DECLARE_SERIAL(CRowAttributes)
	
protected:
	
// Attributes
protected:
	double	m_nHeight;
	
public:
	void	SetHeight(double nHeight);
	double	GetHeight() { return m_nHeight; }

public:
	virtual void Serialize(CArchive& ar);
};

#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<CRowAttributes*>(CArchive& ar, CRowAttributes** ppRowAttributes, int nCount);
#else
void SerializeElements( CArchive& ar, CRowAttributes** ppRowAttributes, int nCount );
#endif
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class CColumnAttributes
class CColumnAttributes : public CRowColumnAttributes
{

 
public:
	CColumnAttributes();
	DECLARE_SERIAL(CColumnAttributes)
// Attributes
protected:
	double	m_nWidth;
	BOOL	m_bIsStandardWidth;
	long		m_nDecimalPrecision;
	
public:
	void	SetWidth(double nWidth);
	double	GetWidth() {return m_nWidth; }
	void	SetWidthStandard();
	BOOL	IsStandardWidth() { return m_bIsStandardWidth; }
	void	SetDecimalPrecision(long nDecimalPrecision);
	long		GetDecimalPrecision() { return m_nDecimalPrecision; }

public:
	virtual void Serialize(CArchive& ar);
};

#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<CColumnAttributes*>(CArchive& ar, CColumnAttributes** ppColumnAttributes, int nCount);
#else
void SerializeElements( CArchive& ar, CColumnAttributes** ppColumnAttributes, int nCount );
#endif


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class CAssocMatrix
typedef CArray<double> CAssocMatrixRow;

class CAssocMatrix : public CObject
{
DECLARE_SERIAL(CAssocMatrix)
 
public:
	CAssocMatrix();

public:
	CArray<CAssocMatrixRow*> m_aMatrix;
	CString m_strMeasure;	// Description of measure used
	long m_nDecMax;		// Decimal point pos of max value in matrix (used for setting
						//	column width in screen painting
	CString m_Time;		// Time of creation as string 

public:
	void Reset();

public:
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
// fusion objects
struct CFuseLine : public CObject
{
	long nLeft;
	long nRight;
	double nLevel;
};
typedef CArray<CFuseLine*> CFuseTable;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// dendrogram objects
struct CDendLine : public CObject
{
	double xStart;
	double xRight;
	double xEnd;
	double yStart;
	double yEnd;
};
typedef CArray<CDendLine*> CDendTable;
typedef CArray<long> CDendOrder;
/////////////////////////////////////////////////////////////////////////////

class CPatnSrvrItem;

/////////////////////////////////////////////////////////////////////////////
// class CPatnDoc
typedef CArray<double> ROW;

//for printing spreadsheets
typedef struct PageInfo 
{
	long x,y; //page's grid location
	long left,right,top,bottom; //page's index ranges
}PageInfo;


class CPatnDoc : public COleServerDoc
{
friend class CDataTransform;
friend class CLinearInterpValuesDlg;
friend class CAssociation;

protected:
	CPatnDoc(); // create from serialization only
	BOOL	CreateTableFromExternalSource();
	BOOL	CreateNewTable();
	DECLARE_DYNCREATE(CPatnDoc)

	void SetMaxWidth(CString filename);

public:
	enum
	{
		columns,
		rows
	};

	long    AppendExternalFile();

// Attributes
public:
	CPatnSrvrItem* GetEmbeddedItem()
		{ return (CPatnSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

	CString m_TableName;
	CString m_LastModified;

	CArray<CColumnAttributes*> m_aColumnAttributes;

public:
	long m_nRandomSeed;
	double	m_nMissingValue;  // value to signify missing values in data
	long		m_idxLeftExtrinsicColumn;
	long		m_idxTopExtrinsicRow;
	CArray<ROW*> m_aData;
	CArray<CRowAttributes*> m_aRowAttributes;
	CArray<double> m_aColumnSums, m_aRowSums,m_aIntrinsicColumnSums, m_aIntrinsicRowSums;
	CArray<long> m_aColumnCountPositives, m_aRowCountPositives,m_aIntrinsicColumnCountPositives, m_aIntrinsicRowCountPositives;
	CArray<double> m_aColumnMinimum, m_aRowMinimum,m_aIntrinsicColumnMinimum, m_aIntrinsicRowMinimum;
	CArray<double> m_aColumnMean, m_aRowMean,m_aIntrinsicColumnMean, m_aIntrinsicRowMean;
	CArray<double> m_aColumnMaximum, m_aRowMaximum,m_aIntrinsicColumnMaximum, m_aIntrinsicRowMaximum;
	CArray<long> m_aColumnCountMissingValues, m_aRowCountMissingValues,m_aIntrinsicColumnCountMissingValues, m_aIntrinsicRowCountMissingValues;

//Log (results etc)
public:
	CString m_log,m_logerror;

	void RetrieveViewContents();

// Display settings
public:
	LOGFONT	m_lfCells;
	long		m_nPointSizeCells; // in tenths of a point
	COLORREF m_rgbFontCells;

	CArray<long> m_aRowGroups;
	CArray<double> m_aRowGroupsDist;
	CArray<long> m_aColumnGroups;

	long m_TwayChange;
	long m_TwayVisible;

public:
	CAssocMatrix m_AssocMatrixRows;
	CAssocMatrix m_AssocMatrixCols;
	//LOGFONT m_lfAssocMx;			// LOGFONT for association matrix text
	//long m_nAssocMxFontPointSize;	// point size of font for association matrix text
	//COLORREF m_rgbAssocMxText;		// rgb of font for association matrix text
	long m_AssocRowChange;
	long m_AssocColChange;
	long m_AssocRowVisible;
	long m_AssocColVisible;
	long m_AssocRowUpdate;
	long m_AssocColUpdate;
	double m_AssocRowsMinkConst; 
	double m_AssocColsMinkConst;

	long m_3ddefault;
	long CreateTableFromRelationalDataUserColumnSelect(CString strFile);
public:
	CFuseTable m_FuseTableRows;
	CFuseTable m_FuseTableCols;
	//LOGFONT m_lfFuse;			// LOGFONT for fusion table text
	//long m_nFuseFontPointSize;	// point size of font for fusion table text
	//COLORREF m_rgbFuseText;		// rgb of font for fusion table text
	long m_FuseRowChange;
	long m_FuseColChange;
	long m_FuseRowVisible;
	long m_FuseColVisible;
	long m_FuseRowUpdate;
	long m_FuseColUpdate;
	CString m_FuseTimeRows;
	CString m_FuseTimeCols;
	CString m_FuseTypeRows;
	CString m_FuseTypeCols;
	double m_FuseBetaRows;
	double m_FuseBetaCols;
	CString m_FuseAssoRows;
	CString m_FuseAssoCols;

public:
	CDendTable m_DendTableRows;
	CDendTable m_DendTableCols;
	CDendOrder m_DendOrderRows;
	CDendOrder m_DendOrderCols;
	CDendTable m_GroupDendTableRows;
	CDendTable m_GroupDendTableCols;
	CDendOrder m_GroupDendOrderRows;
	CDendOrder m_GroupDendOrderCols;
	//LOGFONT m_lfDend;			// LOGFONT for dendrogram object labels
	//long m_nDendFontPointSize;	// point size of font for dendrogram object labels
	//COLORREF m_rgbDendText;		// rgb of font for dendrogram object labels

public:
	CColumnSelection* m_ColSelection;
	CRowSelection* m_RowSelection;
	struct HISTOGRAM 
	{
		CString		strHeading;
		CArray<CArray<double>*>*	pMatrix;
	}* m_Hist;

public:	//tway options
	long m_nScale;
	long m_pExists;
	long m_pIsRowLabel;
	long m_colstand;
	long m_presabs;
	long m_globstand;
	long m_red;
	long m_green;
	long m_blue;
	long m_bw;
	//LOGFONT m_twayfont;
	//LOGFONT m_gstafont;
	//LOGFONT m_BWFont;
	//LOGFONT m_3dfont;

	long m_loading;

	void SerializeV2_12(CArchive &ar,double version);
	void SerializeV2_17(CArchive &ar,double version);
	void SerializeV2_18(CArchive &ar,double version);
	void SerializeV2_19(CArchive &ar,double version);
	void SerializeV2_24(CArchive &ar,double version);
	void SerializeV2_25(CArchive &ar,double version);
	void SerializeV2_30(CArchive& ar,double version);


public: //MST data
	edges m_aMst;
	int m_mst_changed;
//	CArray<neighbour*>* m_aMstSelection;

public: //Colr data - please ignore the mess above
	CArray<colour> m_aObjectColours,m_aObjectColours2; //+3dviewstuff
	CArray<colour> m_aGroupColours,m_aGroupColours2, m_aAprioriGroupColours; //+3dviewstuff
	CArray<colour> m_Vectors2,m_r;//3dview stuff
	CArray<colour> m_UserGroupColours;
	//CArray<colour> m_aPrioriUserGroupColours;

	void UpdateAprioriGroupsColours();

public: //Gsta
	void RunRecipe(CString recipe);
	CArray<dblStat*> m_Gstastats;
	CArray<dblStat*> m_Gstastats_apriori;
	//Group Numbers
	CArray<long> m_Groups;
	CArray<long> m_AprioriGroups;

	CArray<long> m_ColGroups;
	//Row numbers belonging to each group index
	CArray<Rows*> m_Rows;
	CArray<Rows*> m_Cols;
	CArray<Rows*> m_RowControlGroups,m_ColControlGroups;
	//Group Colours (set by group function)
	CArray<colour> m_GroupColours;	
	//pattern vs group
	CArray<long> m_PatternGroups;

	//pattern vs rows (objects)
	CArray<long> m_PatternObjects;

	//pattern vs rows (apriori groups)
	CArray<long> m_PatternApriori;

	long m_GstaAvailable;

	//ANALYSIS 
	CAnalysisRecipe2 m_AnalysisRecipe;

public: //Ansim
	double m_RowAsim_stat;
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
	long m_RowAsim_patn_groups;

	//to facilitate view creation
	long m_AnosimPairView;
	long m_AnosimColView; //is view visible
	long m_AnosimRowView; //is view visible

	//something for anosim pair results...
	CArray<CAssocMatrixRow*> m_RowPairStat; //% random values better than save
	CArray<CAssocMatrixRow*> m_RowPairBest; //best random value
	CArray<CAssocMatrixRow*> m_RowPairSave; //value assigned by group
	CArray<CAssocMatrixRow*> m_ColPairStat; //% random values better than save
	CArray<CAssocMatrixRow*> m_ColPairBest; //best random value
	CArray<CAssocMatrixRow*> m_ColPairSave; //value assigned by group
	CArray<long> m_AnosimRowGroups,m_AnosimColGroups;

public: //Pcc
	CArray<vector*> m_Vectors;

	long m_pGstaVisible;

public: //Kruskal & Wallace place holders
	//array per group...
	CArray<long> m_KW;
	CArray<double> m_KWValues;
	CArray<long> m_KWAttributes;
	CArray<long> m_KWVisible;
	long m_KWNumVisible;
	long m_KWOption;
	double m_KWCutoff;
	long m_KWExists;
	long m_kw_patn_generated_groups;

	long m_MCAOExists;
	CArray<double> m_MCAOResults;
	long m_MCAOSeed;
	long m_MCAOIterations;

	long m_ANOSIMRowsExists;
	long m_ANOSIMColsExists;

	long m_createKW;
	long m_createANOSIMRows;
	long m_createANOSIMCols;
	long m_createMCAO;
	long m_createPCC;

public: //Import function

	long ODBCConnect();

	//double NumFromRk (long rk); //for excel import
	//double NumFromIEEE(long rk,long rk2,long rk3,long rk4); //for excel import

	long m_visible_decimals;


	//long CreateTableFromExcelFile(CString strFile);
//	long CreateTableFromExcelFileTransposed(CString strFile);
	long CreateTableFromTextFile(CString strFile);
//	long CreateTableFromTextFileTransposed(CString strFile);

	long CreateTableFromExcelFileByExcel(CString strFile,long xoff=0,long yoff=0,CArray<CString>* row_labels = NULL,CArray<CString>* col_labels = NULL,bool has_row_labels=true,bool has_col_labels=true);	
	long GetCell(long c,long r,long co,long ro,IDispatch * pSheet,CArray<CString> * labels = NULL);

	long CreateTableFromDOSPRM(CString strFile);
	long CreateTableFromDOSARC(CString strFile);

	long CreateTableFromRelationalData(CString strFile);
	long CreateTableFromAssociationMatrix(CString strFile);

	long AppendExcelFile(CString strFile);


	void ImportanalysisdataRowlower(CString strFile);

	enum
	{
		excel,
		text,
		dosprm,
		dosarc,
		relational,
		associationmatrix
	};


	WINDOWPLACEMENT m_wndplace_tway;
	WINDOWPLACEMENT m_wndplace_gstaview;
	WINDOWPLACEMENT m_wndplace_tableview;
	WINDOWPLACEMENT m_wndplace_3dview;
	WINDOWPLACEMENT m_wndplace_boxwhisker;
	WINDOWPLACEMENT m_wndplace_anosimcol;
	WINDOWPLACEMENT m_wndplace_anosimrow;
	WINDOWPLACEMENT m_wndplace_assoccol;
	WINDOWPLACEMENT m_wndplace_assocrow;
	WINDOWPLACEMENT m_wndplace_fuserow;
	WINDOWPLACEMENT m_wndplace_fusecol;
	WINDOWPLACEMENT m_wndplace_dendrow;
	WINDOWPLACEMENT m_wndplace_dendcol;
	WINDOWPLACEMENT m_wndplace_rowtotals;
	WINDOWPLACEMENT m_wndplace_coltotals;
	WINDOWPLACEMENT m_wndplace_histrow;
	WINDOWPLACEMENT m_wndplace_histcol;
	
public:
	CArray<CRowAttributes*> m_aColumnTotalsRowAttributes;
	CArray<CColumnAttributes*> m_aRowTotalsColumnAttributes;
	CArray<CRowAttributes*> m_aColumnAllocRowAttributes;
	CArray<CColumnAttributes*> m_aRowAllocColumnAttributes;
	CArray<CRowAttributes*> m_aColumnCompareRowAttributes;
	CArray<CRowAttributes*> m_aColumnPccAttributes;
	CArray<CColumnAttributes*> m_aRowOrdColumnAttributes;

public://ssh ordination
	CArray<double> m_aRowXCoords, m_aRowYCoords, m_aRowZCoords;
	CString m_OrdAsso;
	CString m_OrdTime;
	double m_OrdValue;
	long m_Ord3d;
	long m_OrdVisible,m_OrdXVisible;
	long m_OrdUpdate;
	long m_OrdChange;
	double m_OrdXmin;
	double m_OrdXmax;
	double m_OrdYmin;
	double m_OrdYmax;
	double m_OrdZmin;
	double m_OrdZmax;
	double m_OrdStress;
	long m_OrdIterations;
	long m_OrdSeed;
	long m_OrdRandomStarts;
	//kw
	CString m_kwtime;
	
	void OnFileTableAndInfo();

	void UpdateGroups();
	long isExistingGroup(long group,long rows);

public: //MCAO
	CArray<Row*> m_MCAOTable;

public: //New loading stuff
	long m_LoadingFuseRow, m_LoadingFuseCol;
	long m_LoadingAssocRow, m_LoadingAssocCol;
	long m_boxwhiskervisible,m_constfidvisible;
	long m_LoadingOrd,m_LoadingXOrd;
	long m_LoadingTable,m_LoadingTway;
	long m_LoadingGsta,m_LoadingAnosimRow;
	long m_LoadingAnosimCol,m_LoadingBoxWhisker;

	//rects
	CRect mr_tableview,mr_rowtotals,mr_coltotals;
	CRect mr_fuserow,mr_fusecol,mr_fuserowdend,mr_fusecoldend;
	CRect mr_assocrowmx,mr_assoccolmx;
	CRect mr_assocrowhist,mr_assoccolhist;
	CRect mr_gsta,mr_tway;
	CRect mr_ord;
	CRect mr_anosimrow,mr_anosimcol;
	CRect mr_boxwhisker;

	//ord & scat
	long m_Rotate;
	long m_Labels;
	long m_Legend;
	long m_Mst;
	long m_Axies;
	long m_Grouping;
	long m_Usergroupcolours;
	long m_SelOnly;
	long m_Centroid;
	long m_Showranges;
	long m_Symbols;
	double m_RotationSpeed;
	CArray<TextBox*> m_TextBoxes;
	CArray<TextBox2*> m_TextBoxes2;
	TextBox2 m_stressbox;
	D3DVECTOR m_vEyePt;
	D3DVECTOR m_vUpVec;
	D3DVECTOR m_vRight;
	float wAngleY; 
	long m_NoSymbols;
	CArray<long> m_PccVectors;

	//box&whisker
	long m_boxwhisker_source; //apriori = 1, patn generated groups = 0


// Operations
public:

	void	UpdateIntrinsicTotals();
	void	UpdateAllTotals();


	void SetupWindowsAfterLoad();

	// add,delete,move columns
	void	InsertColumnAt(long idxColumn, long nCount=1);
	void	RemoveColumnAt(long idxColumn, long nCount=1);
	void	AddColumn();
	void	MoveColumn(long idxFrom, long idxTo, long nCount=1);

	// add,delete,move rows
	void	InsertRowAt(long idxRow, long nCount=1);
	void	RemoveRowAt(long idxRow, long nCount=1);
	void	AddRow();
	void	MoveRow(long idxFrom, long idxTo, long nCount=1);

	// set,get cell values
	void	SetValueAt(long idxColumn, long idxRow, CString strNewValue);
	void	SetValueAt(long idxColumn, long idxRow, double nNewValue);
	void	GetValueAt(long idxColumn, long idxRow, CString& strValue);
	double	GetValueAt(long idxColumn, long idxRow);

	// set,get column stats
	double 	GetColumnSum(long idxColumn);
	void 	GetColumnSum(long idxColumn, CString& strValue);
	long		GetColumnCountPositives(long idxColumn);
	void	GetColumnCountPositives(long idxColumn, CString& strValue);
	double	GetColumnMinimum(long idxColumn);
	void	GetColumnMinimum(long idxColumn, CString& strValue);
	double	GetColumnMean(long idxColumn);
	void	GetColumnMean(long idxColumn, CString& strValue);
	double	GetColumnMaximum(long idxColumn);
	void	GetColumnMaximum(long idxColumn, CString& strValue);
	long		GetColumnGroups(long idxRow);
	void	GetColumnGroups(long idxRow, long* strValue);
	long		ColumnGroupsEmpty(void);
	long		GetColumnCountMissingValues(long idxColumn);
	void	GetColumnCountMissingValues(long idxColumn, CString& strValue);

	// get row stats
	double 	GetRowSum(long idxRow);
	void 	GetRowSum(long idxRow, CString& strValue);
	long		GetRowCountPositives(long idxRow);
	void	GetRowCountPositives(long idxRow, CString& strValue);
	double	GetRowMinimum(long idxRow);
	void	GetRowMinimum(long idxRow, CString& strValue);
	double	GetRowMean(long idxRow);
	void	GetRowMean(long idxRow, CString& strValue);
	double	GetRowMaximum(long idxRow);
	void	GetRowMaximum(long idxRow, CString& strValue);
	long		GetRowGroups(long idxRow);
	void	GetRowGroups(long idxRow, long* strValue);
	long		RowGroupsEmpty(void);
	long		GetRowCountMissingValues(long idxRow);
	void	GetRowCountMissingValues(long idxRow, CString& strValue);
	void	GetRowXCoord(long idxRow, CString& strValue);
	void	GetRowYCoord(long idxRow, CString& strValue);
	void	GetRowZCoord(long idxRow, CString& strValue);

	double 	GetIntrinsicRowSum(long idxRow);
	void 	GetIntrinsicRowSum(long idxRow, CString& strValue);
	long		GetIntrinsicRowCountPositives(long idxRow);
	void	GetIntrinsicRowCountPositives(long idxRow, CString& strValue);
	double	GetIntrinsicRowMinimum(long idxRow);
	void	GetIntrinsicRowMinimum(long idxRow, CString& strValue);
	double	GetIntrinsicRowMean(long idxRow);
	void	GetIntrinsicRowMean(long idxRow, CString& strValue);
	double	GetIntrinsicRowMaximum(long idxRow);
	void	GetIntrinsicRowMaximum(long idxRow, CString& strValue);
	long		GetIntrinsicRowCountMissingValues(long idxRow);
	void	GetIntrinsicRowCountMissingValues(long idxRow, CString& strValue);

	double 	GetIntrinsicColumnSum(long idxCol);
	void 	GetIntrinsicColumnSum(long idxCol, CString& strValue);
	long		GetIntrinsicColumnCountPositives(long idxCol);
	void	GetIntrinsicColumnCountPositives(long idxCol, CString& strValue);
	double	GetIntrinsicColumnMinimum(long idxCol);
	void	GetIntrinsicColumnMinimum(long idxCol, CString& strValue);
	double	GetIntrinsicColumnMean(long idxCol);
	void	GetIntrinsicColumnMean(long idxCol, CString& strValue);
	double	GetIntrinsicColumnMaximum(long idxCol);
	void	GetIntrinsicColumnMaximum(long idxCol, CString& strValue);
	long		GetIntrinsicColumnCountMissingValues(long idxCol);
	void	GetIntrinsicColumnCountMissingValues(long idxCol, CString& strValue);


	// get missing value
	void	SetMissingValue(CString strMissingValue);
	void	SetMissingValue(double nMissingValue);
	double	GetMissingValue() { return m_nMissingValue; }

	// set,get column attributes
	void	SetColumnWidth(long idxColumn, double nWidth);
	double	GetColumnWidth(long idxColumn);
	void	SetColumnLabel(long idxColumn, CString str);
	void	GetColumnLabel(long idxColumn, CString& str);
	void	SetColumnDecimalPrecision(long idxColumn, long nDecimalPrecision);
	long		GetColumnDecimalPrecision(long idxColumn);
	void	HideColumn(long idxColumn);
	void	HideColumns(CArray<long>* pItems);
	void	UnhideColumn(long idxColumn);
	void	UnhideColumns(CArray<long>* pItems);
	long	IsColumnHidden(long idxColumn);
	void	AdjustColumnWidthToFit(long idxColumn, CDC* pDC);
	void	AdjustColumnWidthToFit(CArray<long>* pItems, CDC* pDC);

	// set,get row attributes
	void	SetRowHeight(long idxRow, double nHeight);
	double	GetRowHeight(long idxRow);
	void	SetRowLabel(long idxRow, CString str);
	void	GetRowLabel(long idxRow, CString& str);
	void	SetColumnGroups(long idxColumn, long grp);
	void	SetRowGroups(long idxRow, long grp, double dist=0);
	
	void	HideRow(long idxRow);
	void	HideRows(CArray<long>* pItems);
	void	UnhideRow(long idxRow);
	void	UnhideRows(CArray<long>* pItems);
	long	IsRowHidden(long idxRow);

	// table dimensions, layout
	long 	GetUpperBoundColumns()	{ return (long)m_aColumnAttributes.GetUpperBound(); }
	long 	GetUpperBoundRows()		{ return (long)m_aData.GetUpperBound(); }
	long 	GetNoOfColumns()		{ return (long)m_aColumnAttributes.GetSize(); }
	long 	GetNoOfRows()			{ return (long)m_aData.GetSize(); }
	long	IsColumnInTable(long idxColumn);
	long	IsRowInTable(long idxRow);
	long	IsCellInTable(long idxColumn, long idxRow);
	long		GetIdxNextVisibleColumn(long idxColumn);
	long		GetIdxPreviousVisibleColumn(long idxColumn);
	long		GetIdxNextVisibleRow(long idxRow);
	long		GetIdxPreviousVisibleRow(long idxRow);

	// extrinsic columns
	long	IsExtrinsicColumns()
			{ return (m_idxLeftExtrinsicColumn >= 0 || m_idxLeftExtrinsicColumn >= GetNoOfColumns()); }
	long	IsIntrinsicColumns()
			{ return (m_idxLeftExtrinsicColumn > 0 || m_idxLeftExtrinsicColumn == -1); }
	void	MakeColumnsExtrinsic(CArray<int>* pItems);
	void	MakeColumnsIntrinsic(CArray<int>* pItems);
	void	MakeAllColumnsExtrinsic();
	void	MakeAllColumnsIntrinsic();
	void	SetIdxLeftExtrinsicColumn(long idxColumn);
	long		GetIdxLeftExtrinsicColumn() { return m_idxLeftExtrinsicColumn; }

	// extrinsic rows
	long	IsExtrinsicRows() { return (m_idxTopExtrinsicRow >= 0 || m_idxTopExtrinsicRow >= GetNoOfRows()); }
	long	IsIntrinsicRows()
			{ return (m_idxTopExtrinsicRow > 0 || m_idxTopExtrinsicRow == -1); }
	void	MakeRowsExtrinsic(CArray<int>* pItems);
	void	MakeRowsIntrinsic(CArray<int>* pItems);
	void	MakeAllRowsExtrinsic();
	void	MakeAllRowsIntrinsic();
	void	SetIdxTopExtrinsicRow(long idxRow);
	long		GetIdxTopExtrinsicRow() { return m_idxTopExtrinsicRow; }

	//so the source of the ordination can be determined (0 == analysis, 1 == import)
	long m_import_ord;

	// fonts
	void	SetDefaultFontsAll();
	void	SetDefaultFontCells();
	//void	SetDefaultFontAssocMatrix();
	//void	SetDefaultFontFusionTable();
	//void	SetDefaultFontDendrogram();
	//void	SetDefaultFontTway();
	//void	SetDefaultFontGsta();
	//void	SetDefaultFontBW();
	//void	SetDefaultFont3d();

	CString getExportGroupCompositionHeader(long isrowdependant);

	// association matrix
	long	IsAssocMatrix() { return ( (m_AssocMatrixRows.m_aMatrix.GetSize() > 0 && !m_AssocRowChange) ||
									   (m_AssocMatrixCols.m_aMatrix.GetSize() > 0 && !m_AssocColChange) ); }

	// fusion
	long	IsFuseTable() { return ( (m_FuseTableRows.GetSize() > 0 && !m_FuseRowChange) ||
									 (m_FuseTableCols.GetSize() > 0 && !m_FuseColChange) ); }

	double m_nCutOffRows, m_nCutOffCols; // TODO: temp to facilitate no of groups in dendrogram


	// sparcity ratio
	double	GetSparcityRatio();

	void OnGo();

public:
	void	SetSizes(long nNoOfColumns, long nNoOfRows);
	void	SetInitialColumnAttributes(long nNoOfColumns, double nWidth, long nDecPrecision);
	void	SetInitialRowAttributes(long nNoOfRows, double nHeight);
	void	SetInitialColumnTotalsRowAttributes();
	void	SetInitialRowTotalsColumnAttributes(long nDecPrecision);

protected:
	ROW*	GetRowAt(long idxRow);
	void	SetColumnAverage(long idxColumn);
	void	SetRowAverage(long idxRow);
	
	void	FillTableWithZeros(long nNoOfColumns, long nNoOfRows);
	void	FillTableWithMissingValues(long nNoOfColumns, long nNoOfRows);
	void	FillTableWithRandomData(long nNoOfColumns, long nNoOfRows);
	
	void	assertCell(long row,long col);
	void	addString(long row,long col,CString string);
	void	addNumber(long row,long col,long double number);

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatnDoc)
	public:
	virtual int OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual int OnOpenDocument(LPCTSTR lpszPathName);
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual void OnFrameWindowActivate(int bActivate);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnView3dgraphDirectx70a_forward()
	{
		OnView3dgraphDirectx70a();
	};
	virtual ~CPatnDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//afx_msg void OnViewGsta();
// Generated message map functions
	//afx_msg void OnUpdateViewGsta(CCmdUI* pCmdUI);
protected:
	//{{AFX_MSG(CPatnDoc)
	afx_msg void OnUpdateViewAssociationMatrix(CCmdUI* pCmdUI);
	afx_msg void OnViewAssociationMatrix();
	afx_msg void OnViewDataTable();
	afx_msg void OnViewFusionDetails();
	afx_msg void OnUpdateViewFusionDetails(CCmdUI* pCmdUI);
	afx_msg void OnToolsHistogram();
	afx_msg void OnUpdateToolsHistogram(CCmdUI* pCmdUI);
	afx_msg void OnViewHistogram();
	afx_msg void OnUpdateViewHistogram(CCmdUI* pCmdUI);
	afx_msg void OnViewTwaytable();
	afx_msg void OnUpdateViewTwaytable(CCmdUI* pCmdUI);		
	afx_msg void OnFileOdbcconnection();
	afx_msg void OnFileExportColumnaffinityvector();
	afx_msg void OnUpdateFileExportColumnaffinityvector(CCmdUI* pCmdUI);
	afx_msg void OnFileExportColumnanismresults();
	afx_msg void OnUpdateFileExportColumnanismresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportColumnassociationmatrix();
	afx_msg void OnUpdateFileExportColumnassociationmatrix(CCmdUI* pCmdUI);
	afx_msg void OnFileExportColumnfusiontable();
	afx_msg void OnUpdateFileExportColumnfusiontable(CCmdUI* pCmdUI);
	afx_msg void OnFileExportOrdinationcoordinates();
	afx_msg void OnUpdateFileExportOrdinationcoordinates(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExportPccresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportPccresults();
	afx_msg void OnFileExportRowaffinityvector();
	afx_msg void OnUpdateFileExportRowaffinityvector(CCmdUI* pCmdUI);
	afx_msg void OnFileExportRowanismresults();
	afx_msg void OnUpdateFileExportRowanismresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportRowassocationmatrix();
	afx_msg void OnUpdateFileExportRowassocationmatrix(CCmdUI* pCmdUI);
	afx_msg void OnFileExportRowfusiontable();
	afx_msg void OnUpdateFileExportRowfusiontable(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadColumnaffinityvector();
	afx_msg void OnUpdateFileLoadColumnaffinityvector(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadRowaffinityvector();
	afx_msg void OnUpdateFileLoadRowaffinityvector(CCmdUI* pCmdUI);
	afx_msg void OnFileExportTableandinfotabs();
	afx_msg void OnFileLoadOrdination();
	afx_msg void OnTablePatternsobjects();
	afx_msg void OnFileExportGroupstatistics();
	afx_msg void OnUpdateFileExportGroupstatistics(CCmdUI* pCmdUI);
	afx_msg void OnViewKruskalwalliceorder();
	afx_msg void OnUpdateViewKruskalwalliceorder(CCmdUI* pCmdUI);
	afx_msg void OnFileExportKruskalwallicestats();
	afx_msg void OnUpdateFileExportKruskalwallicestats(CCmdUI* pCmdUI);
	afx_msg void OnFileExportMcaorawdata();
	afx_msg void OnUpdateFileExportMcaorawdata(CCmdUI* pCmdUI);
	afx_msg void OnViewKruskalwallisresults();
	afx_msg void OnUpdateViewKruskalwallisresults(CCmdUI* pCmdUI);
	afx_msg void OnViewMcao();
	afx_msg void OnUpdateViewMcao(CCmdUI* pCmdUI);
	afx_msg void OnViewAnosimresults();
	afx_msg void OnUpdateViewAnosimresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportMcaoresults();
	afx_msg void OnUpdateFileExportMcaoresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportHistory();
	afx_msg void OnViewHistory();
	afx_msg void OnFileExportAnosimgrouppairaffinityvectors();
	afx_msg void OnUpdateFileExportAnosimgrouppairaffinityvectors(CCmdUI* pCmdUI);
	afx_msg void OnFileExportAnosimgrouppairresults();
	afx_msg void OnUpdateFileExportAnosimgrouppairresults(CCmdUI* pCmdUI);
	afx_msg void OnFileExportSitecount();
	afx_msg void OnViewBoxwhiskerstatistics();
	afx_msg void OnUpdateViewBoxwhiskerstatistics(CCmdUI* pCmdUI);
	afx_msg void OnView3dgraphDirectx70a();
	afx_msg void OnUpdateView3dgraphDirectx70a(CCmdUI* pCmdUI);
	afx_msg void OnView3dgraphOpengl();
	afx_msg void OnUpdateView3dgraphOpengl(CCmdUI* pCmdUI);
	afx_msg void OnContextmenu();
	afx_msg void OnUpdateContextmenu(CCmdUI* pCmdUI);
	afx_msg void OnToolsOptions();
	afx_msg void OnTableProperties();
	afx_msg void OnDataControlgroups();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewPccresults();
	afx_msg void OnUpdateViewPccresults(CCmdUI *pCmdUI);
	afx_msg void OnViewScatterplot33034();
	afx_msg void OnUpdateViewScatterplot33034(CCmdUI *pCmdUI);
	afx_msg void OnImportApriorirowgroupcomposition();
	afx_msg void OnExportanalysisdataApriorirowgroupcomposition();
	afx_msg void OnUpdateExportanalysisdataApriorirowgroupcomposition(CCmdUI *pCmdUI);
	virtual void OnCloseDocument();
	afx_msg void OnUpdateImportanalysisdataColumnlower(CCmdUI *pCmdUI);
	afx_msg void OnImportanalysisdataColumnlower();
	afx_msg void OnImportanalysisdataRowlower();
	afx_msg void OnUpdateImportanalysisdataRowlower(CCmdUI *pCmdUI);
	afx_msg void OnImportanalysisdataColumndendogramorder();
	afx_msg void OnImportanalysisdataRowdendogramorder();
	afx_msg void OnExportevaluationdataColumngroupstatistics();
	afx_msg void OnDataCreatenewvariablesrepresentinggroups();
protected:
	virtual BOOL SaveModified();
public:
	afx_msg void OnViewConstfid();
	afx_msg void OnUpdateViewConstfid(CCmdUI *pCmdUI);
};

#if _MSC_VER > 1020
template <> void AFXAPI SerializeElements<ROW*>(CArchive& ar, ROW** ppRow, int nCount);
#else
void SerializeElements( CArchive& ar, ROW** ppRow, int nCount );
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// inline functions

// get data values
inline void CPatnDoc::GetValueAt(long idxColumn, long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nDec = m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
	double nValue = GetValueAt(idxColumn,idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}

inline double CPatnDoc::GetValueAt(long idxColumn, long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	//ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	ROW* pRow = m_aData.GetAt(idxRow);
	return (pRow->GetAt(idxColumn));
}

// get column attributes
inline double CPatnDoc::GetColumnWidth(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnAttributes[idxColumn]->GetWidth();
}

inline void CPatnDoc::GetColumnLabel(long idxColumn, CString& str)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	m_aColumnAttributes[idxColumn]->GetLabel(str);
}

inline long CPatnDoc::GetColumnDecimalPrecision(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
}

inline long CPatnDoc::IsColumnHidden(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnAttributes[idxColumn]->IsHidden();
}

// get row attributes
inline double CPatnDoc::GetRowHeight(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowAttributes[idxRow]->GetHeight();
}

inline void CPatnDoc::GetRowLabel(long idxRow, CString& str)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	m_aRowAttributes[idxRow]->GetLabel(str);
}

inline long CPatnDoc::IsRowHidden(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowAttributes[idxRow]->IsHidden();
}

// get column stats
inline double CPatnDoc::GetColumnSum(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnSums[idxColumn];
}

inline void CPatnDoc::GetColumnSum(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nDec = m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
	double nValue = GetColumnSum(idxColumn);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}

inline long CPatnDoc::GetColumnCountPositives(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnCountPositives[idxColumn];
}

inline void CPatnDoc::GetColumnCountPositives(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nValue = GetColumnCountPositives(idxColumn);
	strValue.Format("%d",nValue);
}

inline double CPatnDoc::GetColumnMinimum(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnMinimum[idxColumn];
}

inline void CPatnDoc::GetColumnMinimum(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nDec = m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
	double nValue = GetColumnMinimum(idxColumn);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}
inline double CPatnDoc::GetColumnMean(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnMean[idxColumn];
}

inline void CPatnDoc::GetColumnMean(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nDec = m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
	double nValue = GetColumnMean(idxColumn);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}
inline double CPatnDoc::GetColumnMaximum(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnMaximum[idxColumn];
}

inline void CPatnDoc::GetColumnMaximum(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nDec = m_aColumnAttributes[idxColumn]->GetDecimalPrecision();
	double nValue = GetColumnMaximum(idxColumn);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}

inline long CPatnDoc::GetColumnCountMissingValues(long idxColumn)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	return m_aColumnCountMissingValues[idxColumn];
}

inline void CPatnDoc::GetColumnCountMissingValues(long idxColumn, CString& strValue)
{
	ASSERT(idxColumn >= 0 && idxColumn < m_aColumnAttributes.GetSize());
	long nValue = GetColumnCountMissingValues(idxColumn);
	strValue.Format("%d",nValue);
}

// get row stats
inline double CPatnDoc::GetRowSum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowSums[idxRow];
}

inline void CPatnDoc::GetRowSum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetRowSum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline long CPatnDoc::GetRowCountPositives(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowCountPositives[idxRow];
}

inline void CPatnDoc::GetRowCountPositives(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	long nValue = GetRowCountPositives(idxRow);
	strValue.Format("%d",nValue);
}

inline double CPatnDoc::GetRowMinimum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowMinimum[idxRow];
}

inline void CPatnDoc::GetRowMinimum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetRowMinimum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}
inline double CPatnDoc::GetRowMean(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowMean[idxRow];
}

inline void CPatnDoc::GetRowMean(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetRowMean(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}
inline double CPatnDoc::GetRowMaximum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowMaximum[idxRow];
}

inline void CPatnDoc::GetRowMaximum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetRowMaximum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline long CPatnDoc::GetRowGroups(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowGroups[idxRow];
}

inline void CPatnDoc::GetRowGroups(long idxRow, long* strValue)
{
	ASSERT(idxRow >= 0 && idxRow< m_aData.GetSize());
/*	long nValue = GetRowGroups(idxRow);
	if ( nValue == -1 )
		strValue.Empty();
	else
		strValue.Format("%d",nValue);
*/
	*strValue = GetRowGroups(idxRow);
}

inline long CPatnDoc::RowGroupsEmpty(void)
{
	for(long i=(long)m_aRowGroups.GetSize()-1;i>=0;i--)
		if( (m_idxTopExtrinsicRow == -1 ||
			i < m_idxTopExtrinsicRow) && 
			m_aRowGroups.GetAt(i) == -1 ) return 1;
	return 0;
}

inline long CPatnDoc::GetColumnGroups(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < m_aColumnGroups.GetSize());
	return m_aColumnGroups[idxCol];
}

inline void CPatnDoc::GetColumnGroups(long idxCol, long* strValue)
{
	ASSERT(idxCol >= 0 && idxCol< m_aColumnGroups.GetSize());
/*	long nValue = GetColumnGroups(idxCol);
	if ( nValue == -1 )
		strValue.Empty();
	else
		strValue.Format("%d",nValue);
*/
	*strValue = GetColumnGroups(idxCol);
}

inline long CPatnDoc::ColumnGroupsEmpty(void)
{
 	for(long i=(long)m_aColumnGroups.GetSize()-1;i>=0;i--)
		if( (m_idxLeftExtrinsicColumn == -1 ||
			i < m_idxLeftExtrinsicColumn) &&
			m_aColumnGroups.GetAt(i) == -1) return 1;
	return 0;
}

inline long CPatnDoc::GetRowCountMissingValues(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aRowCountMissingValues[idxRow];
}

inline void CPatnDoc::GetRowCountMissingValues(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	long nValue = GetRowCountMissingValues(idxRow);
	strValue.Format("%d",nValue);
}

inline void CPatnDoc::GetRowXCoord(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow <m_aData.GetSize());
	double nValue = m_aRowXCoords[idxRow];
	strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline void	CPatnDoc::GetRowYCoord(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow <m_aData.GetSize());
	double nValue = m_aRowYCoords[idxRow];
	strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline void CPatnDoc::GetRowZCoord(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow <m_aData.GetSize());
	double nValue = m_aRowZCoords[idxRow];
	strValue.Format("%1.*f",m_visible_decimals,nValue);
}

// row access
inline ROW* CPatnDoc::GetRowAt(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow <= m_aData.GetSize());
	return m_aData[idxRow];
}

/////////////////////////////////////////////////////////////////////////////

// get row stats
inline double CPatnDoc::GetIntrinsicRowSum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowSums[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowSum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetIntrinsicRowSum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline long CPatnDoc::GetIntrinsicRowCountPositives(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowCountPositives[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowCountPositives(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	long nValue = GetIntrinsicRowCountPositives(idxRow);
	strValue.Format("%d",nValue);
}

inline double CPatnDoc::GetIntrinsicRowMinimum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowMinimum[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowMinimum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetIntrinsicRowMinimum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}
inline double CPatnDoc::GetIntrinsicRowMean(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowMean[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowMean(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetIntrinsicRowMean(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}
inline double CPatnDoc::GetIntrinsicRowMaximum(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowMaximum[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowMaximum(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	double nValue = GetIntrinsicRowMaximum(idxRow);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",m_visible_decimals,nValue);
}

inline long CPatnDoc::GetIntrinsicRowCountMissingValues(long idxRow)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	return m_aIntrinsicRowCountMissingValues[idxRow];
}

inline void CPatnDoc::GetIntrinsicRowCountMissingValues(long idxRow, CString& strValue)
{
	ASSERT(idxRow >= 0 && idxRow < m_aData.GetSize());
	long nValue = GetIntrinsicRowCountMissingValues(idxRow);
	strValue.Format("%d",nValue);
}

// get column stats
inline double CPatnDoc::GetIntrinsicColumnSum(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnSums[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnSum(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nDec = m_aColumnAttributes[idxCol]->GetDecimalPrecision();
	double nValue = GetIntrinsicColumnSum(idxCol);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}

inline long CPatnDoc::GetIntrinsicColumnCountPositives(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnCountPositives[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnCountPositives(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nValue = GetIntrinsicColumnCountPositives(idxCol);
	strValue.Format("%d",nValue);
}

inline double CPatnDoc::GetIntrinsicColumnMinimum(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnMinimum[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnMinimum(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nDec = m_aColumnAttributes[idxCol]->GetDecimalPrecision();
	double nValue = GetIntrinsicColumnMinimum(idxCol);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}
inline double CPatnDoc::GetIntrinsicColumnMean(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnMean[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnMean(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nDec = m_aColumnAttributes[idxCol]->GetDecimalPrecision();
	double nValue = GetIntrinsicColumnMean(idxCol);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}
inline double CPatnDoc::GetIntrinsicColumnMaximum(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnMaximum[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnMaximum(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nDec = m_aColumnAttributes[idxCol]->GetDecimalPrecision();
	double nValue = GetIntrinsicColumnMaximum(idxCol);
	if ( nValue == m_nMissingValue )
		strValue.Empty();
	else	
		strValue.Format("%1.*f",nDec,nValue);
}


inline long CPatnDoc::GetIntrinsicColumnCountMissingValues(long idxCol)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	return m_aIntrinsicColumnCountMissingValues[idxCol];
}

inline void CPatnDoc::GetIntrinsicColumnCountMissingValues(long idxCol, CString& strValue)
{
	ASSERT(idxCol >= 0 && idxCol < GetNoOfColumns());
	long nValue = GetIntrinsicColumnCountMissingValues(idxCol);
	strValue.Format("%d",nValue);
}

#endif _PATNDOC_H_
