

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistogramFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScattplotFrame frame

class CScattplotFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CScattplotFrame)
protected:
	CScattplotFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScattplotFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScattplotFrame();

	// Generated message map functions
	//{{AFX_MSG(CScattplotFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_HISTOGRAMFRAME_H__3038D605_ED7F_4002_BD73_8879C4CCCDF8__INCLUDED_)
