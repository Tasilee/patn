#if !defined(AFX_GROUPSDLG_H__AA3EBB82_575D_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_GROUPSDLG_H__AA3EBB82_575D_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg dialog

class CGroupsDlg : public CDialog
{
// Construction
public:
	CGroupsDlg(CPatnDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
	CPatnDoc * m_pDoc;
	CArray<long> m_aSelectedItems;
	CArray<long> m_aGroups;
	CArray<colour> m_aGroupColours;

	typedef CArray<int> intarray;
	
	bool m_nDataType;
	bool m_nDataControl;

	CArray<intarray*> m_aGroupAllocations;
	void aPrioriGroupcolour();
	void RefreshButtons();
	void FillList();
	void FillWithColumnLabels();
	void FillWithRowLabels();
	void ResetList();
	void SetSelected();
// Dialog Data
	//{{AFX_DATA(CGroupsDlg)
	enum { IDD = IDD_GROUPS };
	CButton	m_GroupColour;
	CEdit	m_Number;
	CListBox	m_Selected;
	CListBox	m_Groups;
	CListBox	m_All;
	int		m_Columns;
	int		m_Num;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupsDlg)
	afx_msg void OnRows();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnColumns();
	afx_msg void OnAddAll();
	afx_msg void OnAdd();
	afx_msg void OnDblclkSelected();
	afx_msg void OnDblclkAll();
	afx_msg void OnSelchangeAll();
	afx_msg void OnSelchangeSelected();
	afx_msg void OnSelchangeGroups();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeGroupnum();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnGroupcolour();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPSDLG_H__AA3EBB82_575D_11D4_A3D4_525405F5DADA__INCLUDED_)
