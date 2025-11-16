// fuseswnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFuseSplitterWnd frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFuseSplitterWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFuseSplitterWnd)
protected:
	CFuseSplitterWnd();           // protected constructor used by dynamic creation

// Attributes
public:
	CSplitterWnd    m_wndSplitter;
	long m_IsRow;
	CPatnDoc *m_pDoc;
public:
	CFuseTable* m_pFuseTable;
	CDendTable* m_pDendTable;
	CDendOrder* m_pDendOrder;
	CDendOrder* m_pGroupDendOrder;
	CDendTable* m_pGroupDendTable;

	double* m_pCutOff; // TODO: temp code for no of groups line in dendrogram

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFuseSplitterWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL DestroyWindow();
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFuseSplitterWnd();

	// Generated message map functions
	//{{AFX_MSG(CFuseSplitterWnd)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
