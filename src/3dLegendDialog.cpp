// 3dLegendDialog.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "3dLegendDialog.h"


// C3dLegendDialog dialog

IMPLEMENT_DYNCREATE(C3dLegendDialog, CDHtmlDialog)

C3dLegendDialog::C3dLegendDialog(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(C3dLegendDialog::IDD, C3dLegendDialog::IDH, pParent)
{
}

C3dLegendDialog::~C3dLegendDialog()
{
}

void C3dLegendDialog::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL C3dLegendDialog::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(C3dLegendDialog, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(C3dLegendDialog)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// C3dLegendDialog message handlers

HRESULT C3dLegendDialog::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}

HRESULT C3dLegendDialog::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
