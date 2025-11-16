#if !defined(AFX_ASIMDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_ASIMDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsimDlg.h : header file
//

#include "associationthread.h"

/////////////////////////////////////////////////////////////////////////////
// CAssociationDlg dialog

class CAssociationDlg : public CDialog
{
// Construction
public:
	CAssociationDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CAssociationDlg)
	enum { IDD = IDD_NEWDIALOG };
	CEdit	m_Label;
	CProgressCtrl	m_Progress;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssociationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void Setup(CPatnDoc * pDoc,bool rows);
	enum AssociationMeasures
	{
		none,
		BrayCurtis,
		GowerMetric,
		MinkowskiSeries,
		TwoStep,
		Kulzinski
	};

	int m_type;
	bool m_rows;
	bool m_value;

	CPatnDoc* m_pDoc;

	CAssociationThread *m_associationthread;

	HANDLE m_hEventKilledAssociationThread;

	void StepIt();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAssociationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASIMDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_)
