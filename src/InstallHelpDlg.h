#pragma once
#include "afxwin.h"


// CInstallHelpDlg dialog

class CInstallHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CInstallHelpDlg)

public:
	CInstallHelpDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstallHelpDlg();

// Dialog Data
	enum { IDD = IDD_INSTALLHELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
	CStatic m_pic;
	virtual BOOL OnInitDialog();
};
