#if !defined(AFX_ANOSIMVIEW_H__2E10BE75_FFB4_460B_974A_AAA4735CD649__INCLUDED_)
#define AFX_ANOSIMVIEW_H__2E10BE75_FFB4_460B_974A_AAA4735CD649__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnosimView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnosimView view

#include "assomxvw.h"
#include "viewextension.h"

class CAnosimView : public CViewExtension
{
protected:
	CAnosimView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAnosimView)

	CPatnDoc * m_pDoc;
	bool m_columns;

	long m_idxLeftColumn;
	long m_idxTopRow;
	long m_idxTopPrintRow;		// used for printing
	long m_idxLeftPrintColumn;	// used for printing
	LOGFONT m_lfHeading, m_lfHeaderFooter, m_lfData;
	long m_nHeadingPointSize, m_nScaleLabelPointSize, m_nHeaderFooterPointSize;
	CAssocMxMetrics m_AssocMxMetrics;
	CArray<CAssocMatrixRow*> * m_pMatrix;

	long m_mousewheelsensitivity;

	long m_tempheight;

	CRect m_rectDraw;
	CRect m_rect;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

	CArray<long> m_GrpIdxs;

protected:
	CArray<PageInfo*> m_pageinfo;
	long m_printorder;

// Operations
public:

protected:
	void InitializeFonts();
	BOOL CalcMetrics(CDC* pDC);
	void SetMapping(CDC* pDC);
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnosimView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAnosimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnosimView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnOptionsFont();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveas();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBest();
	afx_msg void OnStat();
	afx_msg void OnSave();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANOSIMVIEW_H__2E10BE75_FFB4_460B_974A_AAA4735CD649__INCLUDED_)
