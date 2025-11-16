#pragma once


// C3dLegendDialog dialog

class C3dLegendDialog : public CDHtmlDialog
{
	DECLARE_DYNCREATE(C3dLegendDialog)

public:
	C3dLegendDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~C3dLegendDialog();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_3DLEGEND, IDH = IDR_HTML_3DLEGENDDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
