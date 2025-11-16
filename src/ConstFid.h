#pragma once


// CConstFid frame

class CConstFid : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CConstFid)
protected:
	CConstFid();           // protected constructor used by dynamic creation
	virtual ~CConstFid();


	BOOL PreCreateWindow(CREATESTRUCT& cs) ;

	CDocument* GetActiveDocument() ;

protected:
	DECLARE_MESSAGE_MAP()
};


