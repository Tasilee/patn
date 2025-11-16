#pragma once

#include "patndoc.h"
#include "assomxvw.h"

// CConstFidView view

class CConstFidView : public CViewExtension
{
	DECLARE_DYNCREATE(CConstFidView)

protected:
	CConstFidView();           // protected constructor used by dynamic creation
	virtual ~CConstFidView();

public:
	void InitializeFonts();
	BOOL CalcMetrics(CDC* pDC);

	CPatnDoc* m_pDoc;
	long m_idxLeftColumn;
	long m_idxTopRow;
	long m_idxTopPrintRow;		// used for printing
	long m_idxLeftPrintColumn;	// used for printing
	LOGFONT m_lfHeading, m_lfHeaderFooter, m_lfData;
	long m_nHeadingPointSize, m_nScaleLabelPointSize, m_nHeaderFooterPointSize;
	CAssocMxMetrics m_AssocMxMetrics;
	
	long m_mousewheelsensitivity;

	long m_tempheight;

	CRect m_rectDraw;
	CRect m_rect;

	bool m_isApriori;
	double m_threashold;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

	CArray<PageInfo*> m_pageinfo;
	long m_printorder;

	CArray<CString> * m_colLabels;
	CArray<CString> * m_rowLabels;
	CArray<dblarray*> * m_data;

	void OnSaveas();
	void CConstFidView::SetMapping(CDC* pDC);
	void OnOptionsFont(); 

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL DestroyWindow();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
};


