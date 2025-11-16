#include "stdafx.h"
#include "tabs.h"

CTabs::CTabs()
{
	m_pView = NULL;
	m_idxActiveTab = 0;
}

CTabs::~CTabs()
{
	long i;
	long n = m_aTabs.GetSize();
	CTab *pTab;
	for ( i = 0; i < n; i++ )
	{
		pTab = m_aTabs[i];
		delete pTab;
	}
	m_aTabs.RemoveAll();
	if(m_fontTab.GetSafeHandle() != NULL)
		m_fontTab.DeleteObject();
	if(m_fontActiveTab.GetSafeHandle() != NULL)
		m_fontActiveTab.DeleteObject();
}

void CTabs::Create(CView* pView,long nType)
{
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(CView)));
	m_pView = pView;

	ASSERT(nType == tabVert || nType == tabHorz);
	m_nType = nType;

	long nAngle;

	switch (nType)
	{
		case tabVert:
			m_nWidth = ::GetSystemMetrics(SM_CYHSCROLL);
			nAngle = 900;
			break;

		case tabHorz:
			m_nWidth = ::GetSystemMetrics(SM_CXVSCROLL);
			nAngle = 0;
			break;
	}

	m_fontTab.CreateFont(-11,0,nAngle,nAngle,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH | FF_SWISS,
			"Papyrus");
	m_fontActiveTab.CreateFont(-11,0,nAngle,nAngle,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH | FF_SWISS,
			"Papyrus");
}

void CTabs::AddTab(CString strLabel)
{
	ASSERT(m_pView != NULL);
	
	CTab *pTab = new CTab;
	CClientDC dc(m_pView);
	long x,y;

	pTab->m_strLabel = strLabel;

	CFont * oldFont = dc.SelectObject(&m_fontActiveTab);
	pTab->m_nLength = (dc.GetTextExtent(pTab->m_strLabel,
			pTab->m_strLabel.GetLength())).cx + 16;

	switch(m_nType)
	{
		case tabVert:
			if ( m_aTabs.GetSize() == 0 )
				y = 4;	 // TODO: use some other measure - not hard-coded 4
			else
				y = m_aTabs[m_aTabs.GetUpperBound()]->m_aPoints[2].y;

			pTab->m_aPoints[0].y = y;
			pTab->m_aPoints[1].y = y + 8; // TODO: use some other measure - not hard-coded 8
			pTab->m_aPoints[2].y = y + pTab->m_nLength;
			pTab->m_aPoints[3].y = y + pTab->m_nLength + 8; // ditto
			break;

		case tabHorz:
			if ( m_aTabs.GetSize() == 0 )
				x = 4;	 // TODO: use some other measure - not hard-coded 4
			else
				x = m_aTabs[m_aTabs.GetUpperBound()]->m_aPoints[2].x;

			pTab->m_aPoints[0].x = x;
			pTab->m_aPoints[1].x = x + 8; // TODO: use some other measure - not hard-coded 8
			pTab->m_aPoints[2].x = x + pTab->m_nLength;
			pTab->m_aPoints[3].x = x + pTab->m_nLength + 8; // ditto
			break;
	}

	m_aTabs.Add(pTab);
}

CPoint CTabs::WhereIsTab(long i)
{
	ASSERT(m_pView != NULL);

	CPoint point;

	CRect rClient;
 	CTab *pTab;

	m_pView->GetClientRect(&rClient);

	pTab = m_aTabs[m_aTabs.GetUpperBound()];
	switch(m_nType)
	{
		case tabVert:
			point.x = rClient.right - m_nWidth;
            point.y = 5;
			if(m_aTabs.GetSize() > i && i >= 0)
			{
				pTab = m_aTabs[i];
				point.y = pTab->m_aPoints[3].y - 5;
			}
			break;

		case tabHorz:
			point.y = rClient.bottom - m_nWidth;
			point.x = 5;

			if(m_aTabs.GetSize() > i && i >= 0)
			{
				pTab = m_aTabs[i];
				point.x = pTab->m_aPoints[3].x - 5;
			}
			break;
	}

	return point;

}

BOOL CTabs::ClickedOnTab(CPoint point)
{
	ASSERT(m_pView != NULL);

	CRect rClient;
 	CTab *pTab;
	long i;

	m_pView->GetClientRect(&rClient);

	pTab = m_aTabs[m_aTabs.GetUpperBound()];
	switch(m_nType)
	{
		case tabVert:
			if ( point.x >= rClient.right - m_nWidth &&
				 point.y > 4 && point.y < pTab->m_aPoints[3].y )
			{
				for ( i = 0; i < m_aTabs.GetSize(); i++ )
				{
					pTab = m_aTabs[i];
					if ( point.y < pTab->m_aPoints[3].y - 4 )
					{
						m_idxActiveTab = i;
						break;
					}
				}
				return TRUE;
			}
			break;

		case tabHorz:
			if ( point.y >= rClient.bottom - m_nWidth &&
				 point.x > 4 && point.x < pTab->m_aPoints[3].x )
			{
				for ( i = 0; i < m_aTabs.GetSize(); i++ )
				{
					pTab = m_aTabs[i];
					if ( point.x < pTab->m_aPoints[3].x - 4 )
					{
						m_idxActiveTab = i;
						break;
					}
				}
				return TRUE;
			}
			break;
	}

	return FALSE;
}

void CTabs::DrawTabs(CDC *pDC)
{
	switch(m_nType)
	{
		case tabVert:
			DrawTabsVert(pDC);
			break;

		case tabHorz:
			DrawTabsHorz(pDC);
			break;
	}
}

void CTabs::DrawTabsVert(CDC *pDC)
{
	ASSERT(m_pView != NULL);

	long x, y;
  	CRect rClient;
	long i;
	CTab *pTab;
	CPen penGray,penLtGray;
	CPen*	pOldPen;
	CBrush*	pOldBrush;
	CBrush*	pBrush;
	long		nOldTextAlign, nOldBkMode;
	COLORREF crOldTextColor, crOldBkColor;
 	TEXTMETRIC tm;

	m_pView->GetClientRect(rClient);
	x = rClient.right - m_nWidth;
	y = 4;
 
	// paint background
	crOldTextColor = pDC->SetTextColor(RGB(192,192,192));
	crOldBkColor = pDC->SetBkColor(RGB(128,128,128));
	pBrush = pDC->GetHalftoneBrush();
	pOldBrush = pDC->SelectObject(pBrush);
	pDC->PatBlt(x,0,m_nWidth,rClient.Height(),PATCOPY);
	pDC->SetTextColor(RGB(0,0,0));

	// paint tabs
	penGray.CreatePen(PS_SOLID,0,RGB(128,128,128));
	penLtGray.CreatePen(PS_SOLID,0,RGB(192,192,192));

	for ( i = m_aTabs.GetUpperBound(); i >= 0; i-- )
	{
		pTab = m_aTabs[i];
		pTab->m_aPoints[0].x = rClient.right - m_nWidth;
		pTab->m_aPoints[1].x = rClient.right - 1;
		pTab->m_aPoints[2].x = rClient.right - 1;
		pTab->m_aPoints[3].x = rClient.right - m_nWidth;

		pDC->SelectStockObject(LTGRAY_BRUSH);
		pOldPen = pDC->SelectObject(&penLtGray);
		pDC->Polygon(&pTab->m_aPoints[0],4);
		pDC->SelectObject(&penGray);
		pDC->MoveTo(pTab->m_aPoints[2].x, pTab->m_aPoints[2].y-1);
		pDC->LineTo(pTab->m_aPoints[3].x, pTab->m_aPoints[3].y-1);
		pDC->SelectStockObject(BLACK_PEN);
		pDC->Polyline(&pTab->m_aPoints[0],4);
	}

	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->SelectStockObject(WHITE_PEN);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->Polygon(&pTab->m_aPoints[0],4);
	pDC->SelectStockObject(BLACK_PEN);
	pDC->Polyline(&pTab->m_aPoints[0],4);

	// paint tab labels ...
	nOldTextAlign = pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	nOldBkMode = pDC->SetBkMode(TRANSPARENT);

	// ... active tab
	pDC->SelectObject(&m_fontActiveTab);
	pDC->GetTextMetrics(&tm);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->ExtTextOut(rClient.right - (m_nWidth - tm.tmHeight)/2,
					pTab->m_aPoints[0].y + 4 + pTab->m_nLength/2,
					0,NULL,
					pTab->m_strLabel,
					pTab->m_strLabel.GetLength(),NULL);

	// ... the background tabs
	pDC->SelectObject(&m_fontTab);
	for ( i = 0; i < m_aTabs.GetSize(); i++ )
	{
		if ( i != m_idxActiveTab )
		{
			pTab = m_aTabs[i];
			pDC->ExtTextOut(rClient.right - (m_nWidth - tm.tmHeight)/2,
							pTab->m_aPoints[0].y + 4 + pTab->m_nLength/2,
							0,NULL,
							pTab->m_strLabel,pTab->m_strLabel.GetLength(),NULL);
		}
	}

	// paint line at base of tabs
	pDC->SelectStockObject(BLACK_PEN);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->MoveTo(x,0);
	pDC->LineTo(x,pTab->m_aPoints[0].y);
	pDC->MoveTo(x,pTab->m_aPoints[3].y);
	pDC->LineTo(x,rClient.bottom);
	pDC->SelectObject(&penGray);
	pDC->MoveTo(x-1,0);
	pDC->LineTo(x-1,rClient.bottom);

	// prevent subsequent client area painting from overwriting tabs
	rClient.right -= (m_nWidth + 1);
	pDC->IntersectClipRect(&rClient);

	// restore CDC attributes
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(crOldTextColor);
	pDC->SetBkColor(crOldBkColor);
	pDC->SetTextAlign(nOldTextAlign);
	pDC->SetBkMode(nOldBkMode);
}

void CTabs::DrawTabsHorz(CDC *pDC)
{
	ASSERT(m_pView != NULL);

	long		x, y;
  	CRect	rClient;
	long		i;
	CTab*	pTab;
	CPen	penGray,penLtGray;
	CPen*	pOldPen;
	CBrush*	pOldBrush;
	CBrush*	pBrush;
	long		nOldTextAlign, nOldBkMode;
	COLORREF crOldTextColor, crOldBkColor;
 	TEXTMETRIC tm;

	m_pView->GetClientRect(rClient);
	y = rClient.bottom - m_nWidth;
	x = 4;

	// paint background
	crOldTextColor = pDC->SetTextColor(RGB(192,192,192));
	crOldBkColor = pDC->SetBkColor(RGB(128,128,128));
	pBrush = pDC->GetHalftoneBrush();
	pOldBrush = pDC->SelectObject(pBrush);
	pDC->PatBlt(0,y,rClient.Width(),m_nWidth,PATCOPY);
	pDC->SetTextColor(RGB(0,0,0));

	// paint tabs
	penGray.CreatePen(PS_SOLID,0,RGB(128,128,128));
	penLtGray.CreatePen(PS_SOLID,0,RGB(192,192,192));

	for ( i = m_aTabs.GetUpperBound(); i >= 0; i-- )
	{
		pTab = m_aTabs[i];
		pTab->m_aPoints[0].y = rClient.bottom - m_nWidth;
		pTab->m_aPoints[1].y = rClient.bottom - 1;
		pTab->m_aPoints[2].y = rClient.bottom - 1;
		pTab->m_aPoints[3].y = rClient.bottom - m_nWidth;

		pDC->SelectStockObject(LTGRAY_BRUSH);
		pOldPen = pDC->SelectObject(&penLtGray);
		pDC->Polygon(&pTab->m_aPoints[0],4);
		pDC->SelectObject(&penGray);
		pDC->MoveTo(pTab->m_aPoints[2].x-1, pTab->m_aPoints[2].y);
		pDC->LineTo(pTab->m_aPoints[3].x-1, pTab->m_aPoints[3].y);
		pDC->SelectStockObject(BLACK_PEN);
		pDC->Polyline(&pTab->m_aPoints[0],4);
	}

	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->SelectStockObject(WHITE_PEN);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->Polygon(&pTab->m_aPoints[0],4);
	pDC->SelectStockObject(BLACK_PEN);
	pDC->Polyline(&pTab->m_aPoints[0],4);

	// paint tab labels ...
	nOldTextAlign = pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	nOldBkMode = pDC->SetBkMode(TRANSPARENT);

	// ... active tab
	pDC->SelectObject(&m_fontActiveTab);
	pDC->GetTextMetrics(&tm);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->ExtTextOut(pTab->m_aPoints[0].x + 4 + pTab->m_nLength/2,
					rClient.bottom - (m_nWidth - tm.tmHeight)/2,
					0,NULL,
					pTab->m_strLabel,pTab->m_strLabel.GetLength(),NULL);

	// ... the background tabs
	pDC->SelectObject(&m_fontTab);
	for ( i = 0; i < m_aTabs.GetSize(); i++ )
	{
		if ( i != m_idxActiveTab )
		{
			pTab = m_aTabs[i];
			pDC->ExtTextOut(pTab->m_aPoints[0].x + 4 + pTab->m_nLength/2,
							rClient.bottom - (m_nWidth - tm.tmHeight)/2,
							0,NULL,
							pTab->m_strLabel,pTab->m_strLabel.GetLength(),NULL);
		}
	}

	// paint line at base of tabs
	pDC->SelectStockObject(BLACK_PEN);
	pTab = m_aTabs[m_idxActiveTab];
	pDC->MoveTo(0,y);
	pDC->LineTo(pTab->m_aPoints[0].x,y);
	pDC->MoveTo(pTab->m_aPoints[3].x,y);
	pDC->LineTo(rClient.right,y);
	pDC->SelectObject(&penGray);
	pDC->MoveTo(0,y-1);
	pDC->LineTo(rClient.right,y-1);

	// prevent subsequent client area painting from overwriting tabs
	rClient.bottom -= (m_nWidth + 1);
	pDC->IntersectClipRect(&rClient);

	// restore CDC attributes
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(crOldTextColor);
	pDC->SetBkColor(crOldBkColor);
	pDC->SetTextAlign(nOldTextAlign);
	pDC->SetBkMode(nOldBkMode);
}
