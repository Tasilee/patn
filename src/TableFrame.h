
// TableFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTableFrame frame


class CTableFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTableFrame)
public:
	CTableFrame();

// Attributes
//protected:
public:
	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;
	BOOL m_bSplittersCreated;
	long m_cx, m_cy;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableFrame)
	public:
	virtual CDocument* GetActiveDocument();
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTableFrame();

	// Generated message map functions
	//{{AFX_MSG(CTableFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
