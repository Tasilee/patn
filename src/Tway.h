#if !defined(AFX_TWAY_H__934F2966_31B4_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_TWAY_H__934F2966_31B4_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tway.h : header file
//

//#include "glf.h"
//#include "cfont.h"
#include "viewextension.h"

/////////////////////////////////////////////////////////////////////////////
// CTway view

class CTway : public CViewExtension
{
protected:
	CTway();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTway)

// Attributes
public:
	CPatnDoc* GetDocument();

	long m_Height,m_Width;

	bool m_printing;
	long m_currentpage;
	BITMAPINFO m_info;
	PBYTE m_bits;
	HDC m_dc;
	HBITMAP m_bitmap;
	PIXELFORMATDESCRIPTOR m_pfd;
	long m_mousewheelsensitivity;

	BITMAPINFO* m_pbi;
	LPVOID m_lpBits;
	bool m_SaveImage;

	long rl_width;
	long rl_height;
	long cl_height;
	long cl_width;

	CArray<PageInfo*> m_pageinfo;

	//to be determined by dialog	long m_colStand;
	long m_pLeftExtrinsic;
	long m_pTopExtrinsic;
	long m_colStand;
	long m_PresAbs;
	long m_nScale;
	double m_pRowTextLength;
	double m_pColumnTextLength;

	CArray<long> m_printtopidxs,m_printleftidxs;

	

	bool m_Flipped;

	long m_red,m_green,m_blue,m_bw;

	typedef enum {col00=-1,col0, col1, col2, col3, col4, col5, col6, col7, col8, col9,
		col10, col11, col12} colour;  // colours for two-way cells
	typedef CArray<colour> colour_list;
	long m_pTopIdx, m_pBottomIdx, m_pLeftIdx, m_pRightIdx;
	CRect m_rectDraw;

	struct TWAY
	{
		CString		strHeading;
		CArray<colour_list*>* pcolourMatrix;
		bool presAbs;  // True if Tway represents presence/absence data
		long scale;     // Number of representative colours
		CArray<long> columnOrder;
		CArray<long> rowOrder;
		CArray<long> columnGroupBarIdx;
		CArray<long> rowGroupBarIdx;
		CArray<CString> m_pRowLabels;
		CArray<CString> m_pColumnLabels;
	} m_Tway;
	// TO DO:  Add in any other variables that may be needed/used
	long			m_pColWidth;
	long			m_pRowHeight;
	CPalette    *m_pOldPalette;
	CRect       m_oldRect;

	long m_HRes;
	long m_VRes;
	long m_HPixelsPerInch;
	long m_VPixelsPerInch;
	double m_Hratio;
	double m_Vratio;
	double m_HScale;
	double m_VScale;
	CPageSetupDialog *m_psd;

	bool m_ScaleUp;
	long m_top,m_left;
	long m_rowheight;
	long m_colwidth;
	long m_colindent;
	long m_height,m_width;

	CFont m_font;
	CFont m_headingfont;
	CFont m_legendfont;

// Operations
public:
	void DrawRect (float top,float left, float bot,float right, float col);
	COLORREF drawColour(bool red, bool green, bool blue, bool bw,colour input);
	void bwdraw(double top, double bottom, double right, double left, colour input);
	void drawLegend(double left, double right, double top, double bottom, bool red, bool green, bool blue, bool bw);
	void drawbwLegend(double left, double right, double top, double bottom);
	void drawSquare(double left, double right, double top, double bottom);
	long chooseColour(long temp);
	colour_list* getColourList(void);
	void MyPrint();
	void Initialization();
	void RenderScene(void);
	void drawScale(double x, double y, long pos);
	void setMatrix(double min, double value, long i, long j,double original);

	BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, CString pszFileName);
	
	void DrawToMetaFile(CString filename);
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTway)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTway();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTway)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFlipaxes();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnClose();
	afx_msg void OnFont();
	afx_msg void OnSaveimage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//#ifndef _DEBUG  // debug version in Tway.cpp
inline CPatnDoc* CTway::GetDocument()
   { return (CPatnDoc*)m_pDocument; }
//#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWAY_H__934F2966_31B4_11D4_A3D4_525405F5DADA__INCLUDED_)
