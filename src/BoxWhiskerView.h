#if !defined(AFX_BOXWHISKERVIEW_H__0590A102_0F45_4683_9F0A_493CCF09C05B__INCLUDED_)
#define AFX_BOXWHISKERVIEW_H__0590A102_0F45_4683_9F0A_493CCF09C05B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BoxWhiskerView.h : header file
//

#include "patndoc.h"
#include "viewextension.h"

/////////////////////////////////////////////////////////////////////////////
// CBoxWhiskerView view

class CBoxWhiskerView : public CViewExtension
{
protected:
	CBoxWhiskerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBoxWhiskerView)

// Attributes
public:
	CPatnDoc * m_pDoc;

	CArray<dblStat*> * m_gstastats;
	CArray<long> * m_groups;
	CArray<Rows*> * m_rowgroups;

	double m_max,m_min;
	long pos;

	long m_NumPerPage;
	CRect m_rectDraw;
	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

	long m_saveimage;
	long m_sortbyattribute;

	CArray<long> m_alphatable;

	long m_mousewheelsensitivity;

	void SaveIt();
	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName) ;

	void DrawToMetaFile(CString filename); //if false, must deletemetafile g_emf after use

// Operations
public:
	void OnFont();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoxWhiskerView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBoxWhiskerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBoxWhiskerView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveimage();
	afx_msg void OnClose();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSortalphabetically();
	afx_msg void OnSortbykruskalwallis();
	afx_msg void OnSortbytableorder();
	afx_msg void OnUpdateSortbykruskalwallis(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXWHISKERVIEW_H__0590A102_0F45_4683_9F0A_493CCF09C05B__INCLUDED_)
