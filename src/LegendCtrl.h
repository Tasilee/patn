#pragma once


// CLegendCtrl

class CLegendCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CLegendCtrl)

public:
	CLegendCtrl();
	virtual ~CLegendCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnNcPaint();
};


