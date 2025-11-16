#if !defined(AFX_INDIVIDUALGROUPS_H__BFF69B8A_C3F8_4F37_BDAD_558DE8F6BFEA__INCLUDED_)
#define AFX_INDIVIDUALGROUPS_H__BFF69B8A_C3F8_4F37_BDAD_558DE8F6BFEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IndividualGroups.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndividualGroups dialog

class CIndividualGroups : public CDialog
{
// Construction
public:
	CIndividualGroups(CPatnDoc * pDoc,bool columns,CArray<long> * groups,CWnd* pParent = NULL);   // standard constructor
	CArray<long> m_aSelectedItems;
	CArray<long> m_aGroups;
	CArray<long> * m_origGroups;
	CPatnDoc * m_pDoc;
	bool m_columns;

	typedef CArray<int> intarray;
	
	bool m_nDataType;

	CArray<intarray*> m_aGroupAllocations;

	void RefreshButtons();
	void FillList();
	void FillWithColumnLabels();
	void FillWithRowLabels();
	void ResetList();
	void SetSelected();
// Dialog Data
	//{{AFX_DATA(CIndividualGroups)
	enum { IDD = IDD_INDIVIDUALGROUPS };
	CButton	m_SetControl;
	CStatic	m_Title;
	CButton	m_GroupColour;
	CEdit	m_Number;
	CListBox	m_Selected;
	CListBox	m_Groups;
	CListBox	m_All;
	int		m_Columns;
	int		m_Num;
	int		m_NumGroups;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndividualGroups)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIndividualGroups)
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
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
	afx_msg void OnDestroy();
	afx_msg void OnChangeEdit1();
	afx_msg void OnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDIVIDUALGROUPS_H__BFF69B8A_C3F8_4F37_BDAD_558DE8F6BFEA__INCLUDED_)
