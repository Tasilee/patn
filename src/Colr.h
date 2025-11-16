// Colr.h: interface for the CColr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLR_H__1CDC9321_5F5A_11D4_A580_E7E37BBCBE36__INCLUDED_)
#define AFX_COLR_H__1CDC9321_5F5A_11D4_A580_E7E37BBCBE36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColr  
{
public:
	//CColr();
	CColr(CPatnDoc* pDoc);
	virtual ~CColr();

	CPatnDoc* m_pDoc;
	
	CArray<colour> *objectColours;
	CArray<colour> *groupColours;

	typedef struct {
		double x;
		double y;
		double z;
	} Centroid;
	Centroid centroid;

public:
	//functions
	int OnCreate(void);
};

#endif // !defined(AFX_COLR_H__1CDC9321_5F5A_11D4_A580_E7E37BBCBE36__INCLUDED_)
