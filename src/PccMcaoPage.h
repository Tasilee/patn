#if !defined(AFX_PCCMCAOPAGE_H__1B818334_C820_4CAC_A6BE_2301069DC7B0__INCLUDED_)
#define AFX_PCCMCAOPAGE_H__1B818334_C820_4CAC_A6BE_2301069DC7B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PccMcaoPage.h : header file
//
#include "patndoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPccMcaoPage dialog

class CPccMcaoPage : public CPropertyPage
{
// Construction
public:
	CPccMcaoPage(CPatnDoc * pDoc,CWnd* pParent = NULL);   // standard constructor

protected:
	void FillList();

public:
	CArray<long> m_pcc_indexes;
	CPatnDoc * m_pDoc;

// Dialog Data
	//{{AFX_DATA(CPccMcaoPage)
	enum { IDD = IDD_PCCMCAO_PAGE };
	CEdit	m_McaoIterations;
	CEdit	m_McaoSeed;
	CButton	m_RemoveAll;
	CButton	m_Remove;
	CButton	m_AddAll;
	CButton	m_Add;
	CListBox	m_All;
	CListBox	m_Selected;
	BOOL	m_mcao;
	BOOL	m_pcc;
	int		m_mcao_seed;
	int		m_mcao_iterations;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPccMcaoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPccMcaoPage)
	afx_msg void OnAdd();
	afx_msg void OnAddAll();
	afx_msg void OnDblclkAllcols();
	afx_msg void OnSelchangeAllcols();
	afx_msg void OnMcao();
	afx_msg void OnPcc();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnSelchangeSelectedcols();
	afx_msg void OnDblclkSelectedcols();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCCMCAOPAGE_H__1B818334_C820_4CAC_A6BE_2301069DC7B0__INCLUDED_)
