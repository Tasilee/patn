#pragma once
#include "patndoc.h"
#include "afxwin.h"
#include "association.h"

// CAssignGroups dialog

class CAssignGroups : public CDialog
{
	DECLARE_DYNAMIC(CAssignGroups)

public:
	CAssignGroups(CPatnDoc * doc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAssignGroups();

	BOOL OnInitDialog();

	CPatnDoc * m_pDoc;

	int assocDistance(long i, double * centroids, long ncols, long ngroups);

// Dialog Data
	enum { IDD = IDD_ASSIGNGROUPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_assoc_measures;
	CButton m_apriori_groups;
	CEdit m_const;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeConstant();
};
