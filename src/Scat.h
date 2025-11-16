#if !defined(AFX_SCAT_H__E983C301_7781_11D4_A580_A4C28CF92F35__INCLUDED_)
#define AFX_SCAT_H__E983C301_7781_11D4_A580_A4C28CF92F35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Scat.h : header file
//

#include "glf.h"
#include "texture.h"
#include "cfont.h"
#include "viewextension.h"

/////////////////////////////////////////////////////////////////////////////
// CScat view

class CScat : public CViewExtension
{
protected:
	CScat();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScat)

// Attributes
public:


	CPatnDoc* GetDocument();
	
	long m_currentpage;
	BITMAPINFO m_info;
	PBYTE m_bits;
	HDC m_dc;
	HBITMAP m_bitmap;
	PIXELFORMATDESCRIPTOR m_pfd;
	
	CCFont m_ccfont;
	GLFONT * m_Font;
	CString m_Fonttype;
	long m_italic;
	long m_labelsize;

	bool m_showinfo;
	
	CPalette m_GLPalette;	// Logical Palette
	HDC m_hDC;
	CGlf m_glf;
	long m_font;

	bool m_waitforrefresh;

	// OpenGL Attributes
	HGLRC       m_hRC;

	CRect m_Rect;
	CPalette    *m_pOldPalette;
	CRect       m_oldRect;
	CDC   *m_pDC;

	CPatnDoc * m_pDoc;
	CArray<TextBox*,TextBox*> m_TextBoxes;
	CPoint m_local;
	float wAngleY; 

	double m_ModelMatrix[16],m_ProjMatrix[16];

	// Light values and coordinates
	GLfloat  whiteLight[4];
	GLfloat  sourceLight[4];
	GLfloat	 lightPos[4];

	//variables for operation
	bool m_Rotate,m_Labels,m_Mst,m_Axies,m_Grouping,m_SelOnly;
	bool m_Centroid,m_Usergroupcolours,m_Showranges,m_Symbols;
	bool m_Legend;
	double m_RotationSpeed;
	long m_currenttextbox;
	long m_currentobject,m_compareobject;
	long m_currentcentroid,m_comparecentroid;
	long m_Gstaattrib;
	double m_dist;
	bool m_NoSymbols;
	CTexture m_tex;


	//hint structure
	enum hint {none,objectselection,centroidselection,textboxselection,
		objectcompare,centroidcompare,textboxmove,spaceselection} m_hint;

	//hit test return types
	enum hit{h_none,h_space,h_object,h_centroid,h_textbox};

	CPoint m_comparepoint;

	//items required for infobar
	enum type {object, group};
	typedef CArray<double, double> gpstat;
	typedef CArray<double, double> ord;

	CArray<long,long> m_PccVectors;

	//infobar structure
	struct infobar{
		type item;      // holds info on whether items are object/group
		long itemNr;     // number of items of interest (1 or 2)
		CString name1;  // holds name of first object/group
		CString name2;  // holds name of second object/group
		ord ordin1;     // holds ordination coords for item 1
		ord ordin2;     // holds ordination coords for item 2
		double assoc;   // holds association value of two objects
		gpstat group1;  // holds group stats for group 1
		gpstat group2;  // holds group stats for group 2
		CString attrib; // holds attribute name for group stats
	} m_infobar;
	CArray<long,long> m_KWOrder;
	CArray<double,double> m_KWValues;

	bool m_saveimage;
	CPageSetupDialog *m_psd;
	CRect m_rectDraw;
		long m_HRes;
	long m_VRes;
	long m_HPixelsPerInch;
	long m_VPixelsPerInch;
	double m_Hratio;
	double m_Vratio;
	double m_HScale;
	double m_VScale;

	double m_SaveHeight,m_SaveWidth;
	bool m_printimage;

// Operations
public:
	void SetupRC(HDC hDC);
	void InitializePalette(HDC hDC);
	void DrawScene(void);
	void InitObjSpace(void);
	void DrawObjSpace(void);
	void InitInfoSpace(void);
	void DrawInfoSpace(void);
	void InitLegendSpace(void);
	void DrawLegendSpace(void);
	void InitTextSpace(void);
	void DrawTextSpace(void);
	void DrawMst(void);
	BOOL bSetupPixelFormat(CDC* pDC);
	hit HitTest(CPoint point);
	void SaveIt();
	void OnSaveasimage() ;
	void PrintPatternTable(long nRows,long nCols,long firstIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScat)
	public:
	virtual BOOL DestroyWindow();
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScat();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CScat)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAddtextbox();
	afx_msg void OnDisplaycentroids();
	afx_msg void OnDisplaylabels();
	afx_msg void OnDisplaymst();
	afx_msg void OnShowaxies();
	afx_msg void OnShowgrouping();
	afx_msg void OnShowselectedonly();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnUseusergroupcolours();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnShowranges();
	afx_msg void OnUseblackwhitesymbols();
	afx_msg void OnUpdateUseblackwhitesymbols(CCmdUI* pCmdUI);
	afx_msg void OnSelectgroupstatisticsattribute();
	afx_msg void OnSelectpccvectors();
	afx_msg void OnUpdateDisplaycentroids(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowgrouping(CCmdUI* pCmdUI);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnUpdateKruskalWalliceOptions(CCmdUI* pCmdUI);
	afx_msg void OnKruskalWalliceOptions();
	afx_msg void OnUpdateSelectpccvectors(CCmdUI* pCmdUI);
	afx_msg void OnDisplayLegend();
	afx_msg void OnUpdateUseusergroupcolours(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

inline CPatnDoc* CScat::GetDocument()
   { return (CPatnDoc*)m_pDocument; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCAT_H__E983C301_7781_11D4_A580_A4C28CF92F35__INCLUDED_)
