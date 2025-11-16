#pragma once


// CHowtocheckforupdate dialog

class CHowtocheckforupdate : public CDialog
{
	DECLARE_DYNAMIC(CHowtocheckforupdate)

public:
	CHowtocheckforupdate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHowtocheckforupdate();

// Dialog Data
	enum { IDD = IDD_UPDATE_GUIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_url;
	virtual BOOL OnInitDialog();
};
