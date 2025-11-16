#if !defined(AFX_COMPAREROW_H__32805A47_69FA_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_COMPAREROW_H__32805A47_69FA_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompareRow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompareRow view

class CCompareRow : public CView
{
protected:
	CCompareRow();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCompareRow)

// Attributes
public:
	CTableView* m_pTableView;
	BOOL SetPointerToTableView(CTableView* pTableView);
	void Refresh();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareRow)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCompareRow();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCompareRow)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAREROW_H__32805A47_69FA_11D4_A3D4_525405F5DADA__INCLUDED_)
