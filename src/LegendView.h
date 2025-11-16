#pragma once


// CLegendView view

class CLegendView : public CView
{
	DECLARE_DYNCREATE(CLegendView)

public:
	CLegendView();           // protected constructor used by dynamic creation
	virtual ~CLegendView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


