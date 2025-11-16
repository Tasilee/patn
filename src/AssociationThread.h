#if !defined(AFX_ASIMTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_ASIMTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsimThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAssociationThread thread

class CAssociationThread : public CWinThread
{
public:
	DECLARE_DYNAMIC(CAssociationThread)
	CAssociationThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CPatnDoc * m_pDoc;
	HANDLE m_hEventKillAssociationThread;
	HWND m_hWnd;
enum AssociationMeasures
	{
		none,
		BrayCurtis,
		GowerMetric,
		MinkowskiSeries,
		TwoStep,
		Kulzinski
	};

	long m_type;
	bool m_rows;
	bool * m_value;
	long m_Step;

	CDialog * m_dlg;

// Columns, 
public:
	BOOL BrayCurtisColumns();
	BOOL GowerMetricColumns();
	BOOL MinkowskiSeriesColumns();
	BOOL TwoStepColumns();
	BOOL KulzinskiCols();

// Rows, 
public:	
	BOOL BrayCurtisRows();
	BOOL GowerMetricRows();
	BOOL MinkowskiSeriesRows();
	BOOL TwoStepRows();
	BOOL KulzinskiRows();

public:
	bool Go();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssociationThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAssociationThread();

	// Generated message map functions
	//{{AFX_MSG(CAssociationThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASIMTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_)

