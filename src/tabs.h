#if !defined(TABS_H)

class CTab 
{
friend class CTabs;

protected:
	CString m_strLabel;
	long m_nLength;
  	POINT m_aPoints[4];
};

class CTabs 
{
public:
	CTabs();
	~CTabs();
	void	Create(CView *pView, long nType);
	CPoint	WhereIsTab(long i);


public:
	enum { tabVert, tabHorz };

protected:
	CArray<CTab*> m_aTabs;
	long		m_idxActiveTab;
	CView	*m_pView;
	long		m_nWidth;
	long		m_nType;
	CFont	m_fontTab;
	CFont	m_fontActiveTab;

public:
	void	AddTab(CString strLabel);
	BOOL	ClickedOnTab(CPoint point);  // returns TRUE if point is on one of the tabs
	long		GetIdxActiveTab() { return m_idxActiveTab; }
	void	DrawTabs(CDC *pDC);
	long		GetTabWidth() { return m_nWidth + 1; }

protected:
	void	DrawTabsVert(CDC *pDC);
	void	DrawTabsHorz(CDC *pDC);
};

#define TABS_H
#endif
