#if !defined(AFX_FUSEDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_FUSEDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FusionDlg.h : header file
//

#include "fusionthread.h"

/////////////////////////////////////////////////////////////////////////////
// CFusionDlg dialog

class CFusionDlg : public CDialog
{
// Construction
public:
	CFusionDlg(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CFusionDlg)
	enum { IDD = IDD_NEWDIALOG }; //samedlg as assoc progress
	CEdit	m_Label;
	CProgressCtrl	m_Progress;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFusionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void Setup(CPatnDoc * pDoc,bool rows,
			CMatrix* pMatrix,
            CFuseTable* pFuseTable,
            long nFuseTechnique,
			double* pCutOff,  // TODO; temp code to facilitate no of groups line in dendrogram
            long nNoOfGroups,
            double nBeta,
			CArray<long>* pGroups);

	enum
	{
		NearestNeighbour,
		FurthestNeighbour,
		FlexibleWPGMA,
		FlexibleUPGMA,
	};

	long m_type;
	bool m_value;
	bool m_rows;

	CPatnDoc* m_pDoc;

	CFusionThread *m_fusionthread;

	HANDLE m_hEventKilledFusionThread;

	void StepIt();

	CMatrix* m_pMatrix;
	CFuseTable* m_pFuseTable;
	long m_nFuseTechnique;
	double* m_pCutOff;  
	long m_nNoOfGroups;
	double m_nBeta;
	CArray<long>* m_pGroups;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFusionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUSIONDLG_H__31ACC601_702E_11D4_A3D4_525405F5DADA__INCLUDED_)
