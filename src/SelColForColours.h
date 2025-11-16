#pragma once
#include "afxwin.h"
#include "patndoc.h"


// CSelColForColours dialog

class CSelColForColours : public CDialog
{
	DECLARE_DYNAMIC(CSelColForColours)

public:
	CSelColForColours(CPatnDoc * doc, long sel, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelColForColours();

	BOOL OnInitDialog();

	int m_colour1, m_colour2;
	CPatnDoc * m_pDoc;

	int m_sel;
	long m_inputSel;

	void updateC();

// Dialog Data
	enum { IDD = IDD_COL_COLOURS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnSelchangeList1();
	CListBox m_colourPresets;
	bool m_logScale;
	CListBox m_list;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnPaint();
	CEdit m_c1;
	CEdit m_c2;
};
