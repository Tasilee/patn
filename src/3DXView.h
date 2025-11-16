#if !defined(AFX_3DXVIEW_H__A06D6A68_77D2_4829_A1CC_F57376BCAB15__INCLUDED_)
#define AFX_3DXVIEW_H__A06D6A68_77D2_4829_A1CC_F57376BCAB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DXView.h : header file


#include "viewextension.h"
//#include <d3dtypes.h>
//

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_OBJECTS 4

#define g_PI 3.1415926283f

//(segments-2)*(segments*2+1)*2 + (segments*2+1)*2 +2
#define _NUM_SPHERE_SEGMENTS_ 12
#define _NUM_SPHERE_VERTICIES_ (_NUM_SPHERE_SEGMENTS_-2)*(_NUM_SPHERE_SEGMENTS_*2+1)*2 + (_NUM_SPHERE_SEGMENTS_*2+1)*2 +2

#include "edittext2.h"
#include "texture.h"
#include "writeavi.h"
//#include <ddraw.h>
#include "legendview.h"


/////////////////////////////////////////////////////////////////////////////
// C3DXView view
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );

class C3DXView : public CViewExtension
{
protected:
	C3DXView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C3DXView)

	long g_avi_count;
	CWriteAVI g_wa;

	long m_viewtype;

// Attributes
public:

	void OnPickacolumn();
	int getColourFor(double d, int r1, int g1, int b1, int r2, int g2, int b2);
	
	long m_colColourMap;
	CArray<int> m_colColours;

	CLegendView * legend;

	long m_zoom;

	float x_scale, y_scale, z_scale;

	long m_legendNumberOfRows;
	long m_legendPos;
	CRect m_legendRect;
	long m_legendX;
	long m_legendHeight;
	double m_legendScale;
	
	bool m_firsttime;
	LPDIRECTDRAW7        g_pDD;
	LPDIRECTDRAWSURFACE7 g_pddsPrimary;
	LPDIRECTDRAWSURFACE7 g_pddsBackBuffer;
	LPDIRECTDRAWSURFACE7 g_pddsZBuffer;
	LPDIRECT3D7          g_pD3D;
	LPDIRECT3DDEVICE7    g_pd3dDevice;
	
	DIRECTDRAWCREATEEX   MDirectDrawCreateEx;
	HINSTANCE			 DDHinst;

	RECT                 g_rcScreenRect;

	long m_time;

	long m_mst_size,m_pcc_size;

	long m_infowidth;
	CPoint m_drawres;

	D3DMATRIX m_matView;
	D3DMATRIX m_matWorld; 
	D3DMATRIX m_matProj; 

	long m_gdi_offsetx;

	CPoint m_oldpoint;

	BOOL g_bActive;
	BOOL g_bReady;

	long m_hittop,m_hitbottom;
	long m_SkyFanSize,m_SkyEnd;

	D3DMATRIX g_matLocal[NUM_OBJECTS];
	D3DVERTEX m_sphere[ _NUM_SPHERE_VERTICIES_ ];
	D3DVERTEX m_ring[37];
	D3DVERTEX m_ranges[ 22 ];
	D3DVERTEX * m_mst;
	D3DVERTEX * m_pcc;
	D3DVERTEX * m_line;
	D3DVERTEX * m_line2;
    
	long m_glselect,m_togglerotate,m_Gstaattrib,
		m_saveimage,m_printimage,m_waitforrefresh,
		m_Rotate,m_Labels,m_Legend,m_Mst,m_Axies,
		m_Grouping,m_Usergroupcolours,m_SelOnly,
		m_Centroid,m_Showranges,m_Symbols,
		m_currenttextbox,
		m_currentobject,m_currentcentroid,
		m_compareobject,m_comparecentroid;
	double m_RotationSpeed;
	bool m_showinfo;

	CPatnDoc * m_pDoc;

	long m_currentpage;
	BITMAPINFO m_info;
	PBYTE m_bits;
	HDC m_dc;
	HBITMAP m_bitmap;
	PIXELFORMATDESCRIPTOR m_pfd;

	CRect m_Rect,m_screenrect;
	CPalette * m_pOldPalette;
	CRect m_oldRect;
	CDC * m_pDC;

	CPoint m_rotatepoint;
	long m_settingrotate;

	CArray<TextBox2*> m_TextBoxes;

	TextBox2 m_stressbox;
	D3DVECTOR m_vEyePt;
    D3DVECTOR m_vLookatPt;
    D3DVECTOR m_vUpVec;
	D3DVECTOR m_vRight;

	D3DVECTOR m_vLight;
	D3DLIGHT7 m_light;

	CPoint m_local;

	//variables for operation
	bool m_NoSymbols;
	CArray<long> m_PccVectors;

	long m_selectedobject;
	long m_selectedgroup;
	long m_lastselectedobject;
	long m_lasttime;

	//hint structure
	enum hint {none,objectselection,centroidselection,textboxselection,
		objectcompare,centroidcompare,textboxmove,spaceselection,stressboxselection,stressboxmove} m_hint;

	//hit test return types
	enum hit{h_none,h_space,h_object,h_centroid,h_textbox,h_stressbox};

	CPoint m_comparepoint;

	//items required for infobar
	enum type {object, group};
	typedef CArray<double> gpstat;
	typedef CArray<double> ord;

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
		double mst_length; // holds mst length (in association distance)
	} m_infobar;

	CArray<long> m_KWOrder;
	CArray<double> m_KWValues;

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
	CTexture m_tex;

	CArray<double> m_groupzvalues,m_objectzvalues;
	CArray<long> m_groupzorder,m_objectzorder;

	bool m_printreduction;

	HRESULT CreateEverything( HWND );
	HRESULT Initialize3DEnvironment( HWND, GUID*, const GUID* );
	HRESULT Cleanup3DEnvironment();
	HRESULT Render3DEnvironment();
	HRESULT ShowFrame();
	HRESULT RestoreSurfaces();

	VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE7);
	HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE7);
	HRESULT App_FrameMove( LPDIRECT3DDEVICE7, FLOAT );
	HRESULT App_Render( LPDIRECT3DDEVICE7);
	VOID App_Corner(LPDIRECT3DDEVICE7 pd3dDevice,float *sx,float *sy,float *sz,float x,float y,float z);
	VOID App_Corner_Reverse( LPDIRECT3DDEVICE7 pd3dDevice,float *sx,float *sy,float *sz,float x,float y,float z);


	HRESULT SetViewMatrix( D3DMATRIX& mat, D3DVECTOR& vFrom, D3DVECTOR& vAt,
                       D3DVECTOR& vWorldUp );
	HRESULT TransformVertices( IDirect3D7* pd3dDevice,
                           D3DTLVERTEX* pvVertices, DWORD dwNumVertices );

	static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired );

	void DrawSphere(double x,double y,double z,double red,double green,double blue);
	void InitSphere();
	void InitRing();
	bool HitSphere(float x,float y,float z,CPoint point);
	void DrawSphere2(double x,double y,double z,double red,double green,double blue,bool selected=false, bool grouped = false);
	void InitSphere2();
	bool HitSphere2(float x,float y,float z,CPoint point);
	void SkyValue(D3DVERTEX * v,long q);

	void InitMst(void);
	void DrawMst(void);
	void DrawMst(HDC hdc);
	void InitRanges(void);
	void DrawRanges(HDC hdc);
	void DrawRangesDC(HDC hdc,long pos);
	void InitPcc(void);
	void InitLine(void);
	void DrawPcc(HDC hdc);
	void DrawPcc(HDC hdc,long pos);
	void DrawInfoBar(HDC hdc);
	void DrawTextBoxes(HDC hdc);
	void DrawObjects(HDC hdc);
	void DrawCompare(HDC hdc);
	void DrawLegend(HDC hdc);

	void SaveAVI();

	void DrawOverlap(HDC hdc,long shape,
						   float c1x,float c1y,float c1z,
						   float c2x,float c2y,float c2z,
						   float sx,float sy,float sz,float sr,long height);

	hit HitTest(CPoint point, bool hover=false);

	void UpdateZOrders();

	void SaveIt();

	bool RenderBySize(long x,long y);
	bool RenderLegend(long x,long y);

	void OnObjectsize();
	void OnPlusobjectsize();
	void OnMinusobjectsize();

	void DrawToMetaFile(long usefilename,CString filename);

// Operations
public:
	bool m_legend_instance;
	void * m_legend_view;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DXView)
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3DXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DXView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDisplaymst();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDisplaycentroids();
	afx_msg void OnShowgrouping();
	afx_msg void OnShowranges();
	afx_msg void OnShowselectedonly();
	afx_msg void OnUseusergroupcolours();
	afx_msg void OnSelectpccvectors();
	afx_msg void OnResetRotation();
	afx_msg void OnUpdateResetRotation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectpccvectors(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnAddtextbox();
	afx_msg void OnDisplaylegend();
	afx_msg void OnUseblackwhitesymbols();
	afx_msg void OnUpdateUseblackwhitesymbols(CCmdUI* pCmdUI);
	afx_msg void OnDisplaylabels();
	afx_msg void OnFont();
	afx_msg void OnRotate();
	afx_msg void OnUpdateRotate(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMinusrotationspeed();
	afx_msg void OnPlusrotationspeed();
	afx_msg void OnUpdatePrintpreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetRotationyz();
	afx_msg void OnSetRotationxy();
	afx_msg void OnSetRotationxz();
	afx_msg void OnZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGroupAnalysisgroupsandcentroids();
	afx_msg void OnGroupApriorigroupsancentroids();
	afx_msg void OnSymbolsizeDecreasepgdn();
	afx_msg void OnSymbolsizeIncreasepgup();
	afx_msg void OnUpdateGroupApriorigroupsandcentroids(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUseusergroupcolours(CCmdUI *pCmdUI);
	afx_msg void OnPickacolumnButton();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DXVIEW_H__A06D6A68_77D2_4829_A1CC_F57376BCAB15__INCLUDED_)
