// Anosim.h: interface for the CAnosim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANOSIM_H__0287BD31_17D2_4DEC_B63C_D3665E704801__INCLUDED_)
#define AFX_ANOSIM_H__0287BD31_17D2_4DEC_B63C_D3665E704801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "newdialog.h"

class CAnosim  
{
public:
	CAnosim(CPatnDoc*pDoc);
	virtual ~CAnosim();

	bool AnosimRows(CnewDialog * m_dlg,long iterations,long seed,bool autoeval,int patn_generated_groups);
	bool AnosimCols(CnewDialog * m_dlg,long iterations,long seed,bool autoeval);
	bool AnosimPairs(bool columns,CnewDialog * m_dlg,
		CArray<long> * groups,long iterations,long seed,bool autoeval,int patn_generated_groups);
	void ResetStructures(bool columns);
	void Save(bool columns);
	void SaveVector(bool columns);

protected:
	CPatnDoc * m_pDoc;
	CnewDialog * m_dlg;	
	long m_iterations;
	long m_seed;

};

#endif // !defined(AFX_ANOSIM_H__0287BD31_17D2_4DEC_B63C_D3665E704801__INCLUDED_)
