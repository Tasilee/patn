// ImportRelationDataPicker.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ImportRelationDataPicker.h"
#include ".\importrelationdatapicker.h"


// CImportRelationDataPicker dialog

IMPLEMENT_DYNAMIC(CImportRelationDataPicker, CDialog)
CImportRelationDataPicker::CImportRelationDataPicker(CStringArray* labels, CWnd* pParent /*=NULL*/)
	: CDialog(CImportRelationDataPicker::IDD, pParent)
	, m_colSel(0)
	, m_RowSel(0)
	, m_DataSel(0)
{
	m_labels = labels;
}

CImportRelationDataPicker::~CImportRelationDataPicker()
{
}

void CImportRelationDataPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Column);
	DDX_Control(pDX, IDC_COMBO2, m_Row);
	DDX_Control(pDX, IDC_COMBO3, m_Data);
}


BEGIN_MESSAGE_MAP(CImportRelationDataPicker, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CImportRelationDataPicker message handlers

BOOL CImportRelationDataPicker::OnInitDialog()
{
	int i;
	CDialog::OnInitDialog();

	for(i=0;i<m_labels->GetSize();i++) {
		m_Column.AddString(m_labels->GetAt(i));
		m_Row.AddString(m_labels->GetAt(i));
		m_Data.AddString(m_labels->GetAt(i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportRelationDataPicker::OnBnClickedOk()
{
	m_colSel = m_Column.GetCurSel();
	m_RowSel = m_Row.GetCurSel();
	m_DataSel = m_Data.GetCurSel();

	OnOK();
}
