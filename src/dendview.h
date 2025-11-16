// dendview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDendView view
#include "viewextension.h"

#include "texture.h"

class CDendMetrics 
{
public:
	long cyHeader, cyFooter; // only for printing
	long xObjectLabelsLeft; 	// left side of object labels
	long cyHeadingBar;
	long yHeadingBottom;
	CSize sizeScaleLabel;
	long cyScaleBar;
	long yScaleLine;
	long yScaleLabels;
	long yScrollTop;			// top of scrollable area
	CSize sizeObjectLabel;	// size of labels for objects
	long cyLineSpacing;
	long LogPixelsX,LogPixelsY;  // pixels per inch in X and Y directions (depends on DC)
	long width,height;			// width & height of area to draw on
	long nNumLinesPerPage;	// only used for printing		
};

class CDendView : public CViewExtension
{
protected:
	CDendView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDendView) 

// Attributes
public:
	double* m_pCutOff;  // TODO: temp code for no of groups line

	CTexture m_tex;
	long m_GroupsSymbols; //0 == labels, 1 == symbols, 2 == symbols+labels

public:
	CPatnDoc* m_pDoc;
	long m_idxTopRow;		// used for screen display/scrolling
	long m_idxTopPrintRow;	// used for printing
	LOGFONT m_lfHeading, m_lfScaleLabel, m_lfHeaderFooter;
	long m_nHeadingPointSize, m_nScaleLabelPointSize, m_nHeaderFooterPointSize;
	CDendMetrics m_DendMetrics;
	CFuseTable* m_pFuseTable;
	CDendTable* m_pDendTable;
	CDendOrder* m_pDendOrder;
	CDendOrder * m_pGroupDendOrder;
	CDendTable * m_pGroupDendTable;
	bool m_GroupsOnly;
	CRect m_rectDraw;
	bool m_SaveImage;

	long m_cutoffX,m_cutoffmax,m_cutoffmin,m_movingcutoff;
	double m_cutoffoldmin,m_cutoffoldrange,m_cutoffnewrange,m_cutoffnewmin,m_cutoffx0,m_cutoffh;

	long m_mousewheelsensitivity;
	
	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

	CArray<Rows*> m_controlindicator;
	bool m_row;
	bool m_ControlIndicators;

// Operations
protected:
	void InitializeFonts();
	BOOL CalcMetrics(CDC* pDC);
	void SetMapping(CDC* pDC);
	void SaveIt(CDC* pDC);
	void OnOptionsFont();
	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) ;

	void DrawToMetaFile(CString filename);



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDendView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDendView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDendView)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGroupsonly();
	afx_msg void OnSaveimageas();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBwsymbols();
	afx_msg void OnUpdateBwsymbols(CCmdUI* pCmdUI);
	afx_msg void OnControlindicator();
	afx_msg void OnUpdateControlindicator(CCmdUI* pCmdUI);
	afx_msg void OnLabels();
	afx_msg void OnLabelsSymbols();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CPatnDoc* GetDocument();
	BOOL CDendView::Create(LPCSTR lpszClassName,
		LPCSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext /* = NULL */);
};

#ifndef _DEBUG  // debug version in dendview.cpp
inline CPatnDoc* CDendView::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
