// Permut.h: interface for the CPermut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERMUT_H__C86C4F5A_B228_4AC0_8B42_8C4E9E3388D2__INCLUDED_)
#define AFX_PERMUT_H__C86C4F5A_B228_4AC0_8B42_8C4E9E3388D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//should use templates, but instead, use CArray<double> for now
class CPermut  
{
protected:
	typedef struct Tree{
		long size; //number of branches at this level
		long pos; //position of branch at this level (1<=pos<=size)
		long value; //actual number assigned to this pos
		Tree * next; //pointer to next tree
		Tree * prev; //pointer to previous tree
	}Tree;

	Tree * m_first;
	Tree * m_last;
	CArray<__int64,__int64> m_Factors;

public:
	CPermut();
	virtual ~CPermut();

	void initPermut(long size);

	void nextPermut(CArray<double>*data,bool resetTree);
	void prevPermut(CArray<double>*data,bool resetTree);

	//steps may be positive (nextPermutations) or negative
	// (prevPermutations).
	void Permut(CArray<double>*data,__int64 steps,bool resetTree);

	void SrtShl(CArray<long> *data,CArray<double>* values);

	CArray<long>* expandTree(CArray<long>*data);
};

#endif // !defined(AFX_PERMUT_H__C86C4F5A_B228_4AC0_8B42_8C4E9E3388D2__INCLUDED_)
