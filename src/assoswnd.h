// assoswnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAssocSplitterWnd frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CAssocSplitterWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CAssocSplitterWnd)
protected:
	CAssocSplitterWnd();           // protected constructor used by dynamic creation

// Attributes
protected:

public:
	CAssocMatrix* m_pAssocMatrix;
	CSplitterWnd    m_wndSplitter;

// Operations
public:
	long m_IsRow;
	CPatnDoc* m_pDoc;
	bool m_created_successfully;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssocSplitterWnd)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAssocSplitterWnd();

	// Generated message map functions
	//{{AFX_MSG(CAssocSplitterWnd)
	afx_msg void OnFileExport();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

