#if !defined(AFX_HISTOGRAMFRAME_H__3038D605_ED7F_4002_BD73_8879C4CCCDF8__INCLUDED_)
#define AFX_HISTOGRAMFRAME_H__3038D605_ED7F_4002_BD73_8879C4CCCDF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistogramFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHistogramFrame frame

class CHistogramFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CHistogramFrame)
protected:
	CHistogramFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistogramFrame)
	public:
	virtual CDocument* GetActiveDocument();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHistogramFrame();

	// Generated message map functions
	//{{AFX_MSG(CHistogramFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTOGRAMFRAME_H__3038D605_ED7F_4002_BD73_8879C4CCCDF8__INCLUDED_)
