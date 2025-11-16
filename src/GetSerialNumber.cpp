// GetSerialNumber.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "GetSerialNumber.h"
#include ".\getserialnumber.h"


// CGetSerialNumber dialog

IMPLEMENT_DYNAMIC(CGetSerialNumber, CDialog)
CGetSerialNumber::CGetSerialNumber(CWnd* pParent /*=NULL*/)
	: CDialog(CGetSerialNumber::IDD, pParent)
	, m_serialnumber(_T(""))
{
}

CGetSerialNumber::~CGetSerialNumber()
{
}

void CGetSerialNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUMBER, m_serialnumber);
}


BEGIN_MESSAGE_MAP(CGetSerialNumber, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CGetSerialNumber message handlers

void CGetSerialNumber::OnBnClickedOk()
{

	OnOK();
}
