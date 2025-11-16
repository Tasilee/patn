// SelectionName.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "SelectionName.h"


// CSelectionName dialog

IMPLEMENT_DYNAMIC(CSelectionName, CDialog)
CSelectionName::CSelectionName(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectionName::IDD, pParent)
{
}

CSelectionName::~CSelectionName()
{
}

void CSelectionName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_name);
}


BEGIN_MESSAGE_MAP(CSelectionName, CDialog)
END_MESSAGE_MAP()


// CSelectionName message handlers
