// MCAO.h: interface for the CMCAO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCAO_H__B12B48F2_E9A3_4DC3_9CF0_0D35A010FEFE__INCLUDED_)
#define AFX_MCAO_H__B12B48F2_E9A3_4DC3_9CF0_0D35A010FEFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "newdialog.h"

class CMCAO  
{
public:
	CMCAO();
	virtual ~CMCAO();
	bool Create(CPatnDoc *pDoc);
	void SrtShl(Row* data);
	void permut(Row*data,long seed,long idx);
	bool IsElem(CArray<long> *order,long i);


	CPatnDoc *m_pDoc;
	long m_numPermutations;
	long m_Seed;
	bool m_Random;
	CnewDialog * m_dlg;
};

#endif // !defined(AFX_MCAO_H__B12B48F2_E9A3_4DC3_9CF0_0D35A010FEFE__INCLUDED_)
