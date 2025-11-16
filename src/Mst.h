// Mst.h: interface for the CMst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MST_H__E0D862A1_6530_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_MST_H__E0D862A1_6530_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMst  
{
public:
	CMst(CPatnDoc * pDoc);
	virtual ~CMst();

	long OnCreate();

	CPatnDoc* m_pDoc;

	edges m_Mst;


};

#endif // !defined(AFX_MST_H__E0D862A1_6530_11D4_A3D4_525405F5DADA__INCLUDED_)
