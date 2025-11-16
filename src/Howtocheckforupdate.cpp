// Howtocheckforupdate.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "Howtocheckforupdate.h"
#include ".\howtocheckforupdate.h"


// CHowtocheckforupdate dialog

IMPLEMENT_DYNAMIC(CHowtocheckforupdate, CDialog)
CHowtocheckforupdate::CHowtocheckforupdate(CWnd* pParent /*=NULL*/)
	: CDialog(CHowtocheckforupdate::IDD, pParent)
	, m_url(_T(""))
{

}

CHowtocheckforupdate::~CHowtocheckforupdate()
{
}

void CHowtocheckforupdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_url);
}


BEGIN_MESSAGE_MAP(CHowtocheckforupdate, CDialog)
END_MESSAGE_MAP()


// CHowtocheckforupdate message handlers

BOOL CHowtocheckforupdate::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowText("Buy PATN");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
