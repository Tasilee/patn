// histvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHistogram view
#if _MSC_VER > 1000
#pragma once
#endif
#include "viewextension.h"

class CHistogram : public CViewExtension
{
protected:
	CHistogram();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHistogram)

// Attributes
public:

protected:
	long m_cxWidth, m_cyHeight;	// width & height of area to draw on
	CArray<double>* m_aArray;
	CRect m_rectDraw;

	struct HISTOGRAM 
	{
		CString		strHeading;
		CArray<CArray<double>*>*	pMatrix;
	} m_Hist;
	CArray<double> m_aSegments;
	CArray<long> m_aCount;
	long m_nNoOfBars;
	double m_nMax, m_nMin;
	double m_nSum;
	long m_nCount;
	long m_nCountPos;
	double m_nMean;
	long m_nLowCount, m_nHighCount;
	long m_nDecMax;	 // width in chars (before decimal point) of m_nMax
	long m_nDecCount; // width in chars of m_nCount
	bool m_SaveImage;

// Operations
public:
	void CalcHistogram();
	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName);
	void drawPrint(CDC *pDC);
	void SaveIt(CDC *pDC);

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog * m_psd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistogram)
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
	virtual ~CHistogram();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistogram)
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
	afx_msg void OnFont();
};

#ifndef _DEBUG  // debug version in histvw.cpp
inline CPatnDoc* CHistogram::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
