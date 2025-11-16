#pragma once


// C3dLegend view

class C3dLegend : public CListView
{
	DECLARE_DYNCREATE(C3dLegend)

public:
	C3dLegend();           // protected constructor used by dynamic creation
	virtual ~C3dLegend();



public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


