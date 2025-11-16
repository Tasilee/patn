// KruskalWalice.h: interface for the CKruskalWalice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRUSKALWALLACE_H__9B670127_8F52_4A5F_BF9E_A97295C0BDEE__INCLUDED_)
#define AFX_KRUSKALWALLACE_H__9B670127_8F52_4A5F_BF9E_A97295C0BDEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "newdialog.h"

class CKruskalWallace  
{
public:
	CKruskalWallace();
	virtual ~CKruskalWallace();
	bool Create(CnewDialog * dlg,CPatnDoc *pDoc,bool autoeval);
	bool CalcKW(Rows* order,bool autoeval);
	void Rank(CArray<double>*values,CArray<long>*order,long nn,long iop);
	void SrtShl(CArray<double>*  data,
				CArray<long>* indx,
				long n);
	bool Create2(CPatnDoc *pDoc,CArray<long>* order,CArray<double>*values,
		long group1,long group2);
	bool CalcKW2(CArray<long>* order,CArray<double>*values,
		long group1,long group2);
	bool isMember(CArray<long>*list,long k);


	CPatnDoc *m_pDoc;
	CnewDialog *m_dlg;
};

#endif // !defined(AFX_KRUSKALWALLACE_H__9B670127_8F52_4A5F_BF9E_A97295C0BDEE__INCLUDED_)
