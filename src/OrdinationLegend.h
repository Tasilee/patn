#pragma once
#include "afxcmn.h"

#include "legendctrl.h"


// COrdinationLegend dialog

class COrdinationLegend : public CDialog
{
	DECLARE_DYNAMIC(COrdinationLegend)

public:
	COrdinationLegend(CWnd* pParent = NULL);   // standard constructor
	virtual ~COrdinationLegend();

// Dialog Data
	enum { IDD = IDD_3DLEGEND };

	CPatnDoc * m_pDoc;

	void OnPopulate(int type = 1); //where type 0 is B&W and 1 is coloured circle
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list;
};
