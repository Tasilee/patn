// histvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScattplot view
#ifndef _CSCATTPLOT_
#define _CSCATTPLOT_

#include "viewextension.h"

class CScattplot : public CViewExtension
{
protected:
	CScattplot();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScattplot)

// Attributes
public:
	CRect m_dimensions;
protected:
	long m_cxWidth, m_cyHeight;	// width & height of area to draw on
	//CArray<double>* m_aArray;
	CRect m_rectDraw;
	CRect m_offset;

	//struct HISTOGRAM 
	//{
	//	CString		strHeading;
	//	CArray<CArray<double>*,CArray<double>*>*	pMatrix;
	//} m_Hist;
	//CArray<double> m_aSegments;
	//CArray<long> m_aCount;
	//long m_nNoOfBars;
	//double m_nMax, m_nMin;
	//double m_nSum;
	//long m_nCount;
	//long m_nCountPos;
	//double m_nMean;
	//long m_nLowCount, m_nHighCount;
	//long m_nDecMax;	 // width in chars (before decimal point) of m_nMax
	//long m_nDecCount; // width in chars of m_nCount
	
	bool m_SaveImage;

	double m_xMax,m_xMin,m_yMax,m_yMin;
	double * data;
	long * idxs;
	long size;
	enum { SCAT_XY };
	int m_key;
	int m_k1,m_k2;

	

// Operations
public:
	//void Calcscattplot();
//	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName);
	void drawPrint(CDC *pDC);
	void SaveIt(CDC *pDC);

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog * m_psd;
	int m_objsize;
	int m_sel;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScattplot)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScattplot();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CScattplot)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveimage();
	afx_msg void OnClose();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CPatnDoc* GetDocument();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFont();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDecreaseSymbolSize();
	afx_msg void OnIncreaseSymbolSize();
};

#ifndef _DEBUG  // debug version in histvw.cpp
inline CPatnDoc* CScattplot::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif