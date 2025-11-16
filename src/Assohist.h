// assohist.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAssocHistogram view
#include "viewextension.h"

class CAssocHistogram : public CViewExtension
{
protected:
	CAssocHistogram();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAssocHistogram)

// Attributes
public:
	void OnFont();

public:
	long m_cxWidth, m_cyHeight;	// width & height of area to draw on
	struct HISTOGRAM 
	{
		CString		strHeading;
		CArray<CArray<double>*>*	pMatrix;
	} m_Hist;
	CAssocMatrix* m_pMatrix;
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
	CRect m_rectDraw;
	bool m_SaveImage;

	long m_HRes;
	long m_VRes;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

// Operations
public:
	void CalcHistogram();
	void drawPrint(CDC *pDC);
	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssocHistogram)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAssocHistogram();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CAssocHistogram)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSaveimage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CPatnDoc* GetDocument();
	virtual BOOL Create(LPCSTR lpszClassName,
		LPCSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext /* = NULL */);
	void SaveIt(CDC* pDC);

};

#ifndef _DEBUG  // debug version in assohist.cpp
inline CPatnDoc* CAssocHistogram::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
