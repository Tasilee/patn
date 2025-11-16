// Ordination.h: interface for the COrdination class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDINATION_H__6428E3C2_4C6B_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_ORDINATION_H__6428E3C2_4C6B_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ssh.h"

class CPatnDoc;

class COrdination
{
public:
	COrdination(CPatnDoc* pDoc);
	~COrdination();

	void ExportRowOrdination(void);
	void RescaleRadius(ldblaray * conf, long n, long dim);

public:
	HWND m_parent;

protected:
	CPatnDoc* m_pDoc;

// Columns
public:
	BOOL SSHColumns();
	
// Rows
public:	
	BOOL SSHRows(double cut,int ord3d);

};

#endif // !defined(AFX_ORDINATION_H__6428E3C2_4C6B_11D4_A3D4_525405F5DADA__INCLUDED_)

