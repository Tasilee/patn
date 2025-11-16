// Pcc.h: interface for the CPcc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCC_H__BA910581_6930_11D4_A580_984F54F24135__INCLUDED_)
#define AFX_PCC_H__BA910581_6930_11D4_A580_984F54F24135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPcc  
{
public:
	CPcc(CPatnDoc * pDoc);
	virtual ~CPcc();

	long m_NumCoords;

	long OnCreate(long extrinsic);
	double OnCreate2(Row* data);
	
	typedef CArray<double> doublex;
	typedef CArray<doublex*> dbllarray;

	matrix *tmat_mat_mult(matrix *tA, matrix *A);
	vector *tmat_vect_mult(matrix *tA, vector *B);
	vector *mat_vect_mult(matrix *A, vector *B);
	dbllarray* Gauss(dbllarray *A);

	double tvect_vect_mult_n(vector *tA, vector *B);
	double tvect_vect_mult_p(vector *tA, vector *B);

	void ExportPcc(void);

	
	CPatnDoc *m_pDoc;
	vector *m_Vector;

};

#endif // !defined(AFX_PCC_H__BA910581_6930_11D4_A580_984F54F24135__INCLUDED_)

