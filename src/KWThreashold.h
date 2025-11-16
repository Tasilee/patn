#pragma once
#include "afxwin.h"
#include "patndoc.h"


// CKWThreashold dialog

class CKWThreashold : public CDialog
{
	DECLARE_DYNAMIC(CKWThreashold)

public:
	CKWThreashold(CPatnDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CKWThreashold();

	BOOL OnInitDialog();

	CPatnDoc * m_pDoc;

// Dialog Data
	enum { IDD = IDD_KWTHREASHOLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	double m_KWThreashold;
	CListBox m_list;
	afx_msg void OnBnClickedCancel();
};
