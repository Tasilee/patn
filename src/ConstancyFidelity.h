#pragma once
#include "afxwin.h"
#include "patndoc.h"
#include "tableview.h"


// CConstancyFidelity dialog

class CConstancyFidelity : public CDialog
{
	DECLARE_DYNAMIC(CConstancyFidelity)

public:
	CConstancyFidelity(CPatnDoc * doc, CTableView * table, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConstancyFidelity();

	CPatnDoc * m_pDoc;
	CTableView * m_table;

	long * m_idxSelected;
	double * m_constancy;
	double * m_fidelity;
	double * m_constancyApriori;
	double * m_fidelityApriori;
	int * m_IdxSelectedColumns;

	CArray<CString> * m_colLabels;
	CArray<CString> * m_rowLabels;
	CArray<dblarray*> * m_data;

	long m_groupsCount;
	long m_groupsAprioriCount;

	bool m_isApriori;

	BOOL OnInitDialog();

	void ExportCSV();

	void calculateConstancyFidelity(long cgroups, CArray<long> * rowgroups, double * constancy, double * fidelity,int offset);
	void updateLists();

// Dialog Data
	enum { IDD = IDD_CONSTANCY_FIDELITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_selectedColumns;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedAddAll();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedRemoveAll();
	afx_msg void OnBnClickedCheck1();
	CButton m_ctrlApriori;
	CButton m_orderByGroup;
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedOk();
	CEdit m_ctrlThreashold;
	double m_dblThreashold;
};
