#pragma once


// CGetSerialNumber dialog

class CGetSerialNumber : public CDialog
{
	DECLARE_DYNAMIC(CGetSerialNumber)

public:
	CGetSerialNumber(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGetSerialNumber();

// Dialog Data
	enum { IDD = IDD_GETSERIALNUMBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_serialnumber;
	afx_msg void OnBnClickedOk();
};
