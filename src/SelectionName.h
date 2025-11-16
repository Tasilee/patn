#pragma once
#include "afxwin.h"


// CSelectionName dialog

class CSelectionName : public CDialog
{
	DECLARE_DYNAMIC(CSelectionName)

public:
	CSelectionName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectionName();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_name;
};
