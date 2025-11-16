#pragma once
#include "afxwin.h"

// CImportRelationDataPicker dialog

class CImportRelationDataPicker : public CDialog
{
	DECLARE_DYNAMIC(CImportRelationDataPicker)

public:
	CImportRelationDataPicker(CStringArray * labels,CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportRelationDataPicker();

	CStringArray * m_labels;

// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Column;
	CComboBox m_Row;
	CComboBox m_Data;
	virtual BOOL OnInitDialog();
	int m_colSel;
	int m_RowSel;
	int m_DataSel;
	afx_msg void OnBnClickedOk();
};
