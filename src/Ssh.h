// Ssh.h: longerface for the CSsh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSH_H__C792AB83_6426_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_SSH_H__C792AB83_6426_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "newdialog.h"

typedef CArray<double> ldblarray;

typedef double * ldblaray;

class CSsh  
{
public:
	CSsh();
	virtual ~CSsh();

	CPatnDoc *m_pDoc;
	CnewDialog * m_dlg;

bool Ssh1(
		  double * din,		//input assoc
		  double * dout,	//output assoc
		  double * dest,	//estimated assoc
		  long* indx,	//sorted order
		  ldblaray* conf,	//ordination data
		  ldblaray* grad,	//ordination data gradient
		  ldblaray* gral,	//same
		  long nd,			//number of dimensions.
		  long n,
		  CPatnDoc * pDoc,
		  double cut,	//cutoff value
		  long pMaxIterations, //number of random starts
		  double * pStress,
		  ldblaray*saveconf,
		  long pNumberOfRandomStarts,
		  long pRandomSeed,
		  long assocsize);



void Ed(
		ldblaray* conf,	//coordinate data (n,nd)
		double * ass,	//final distance location
		long* indx,	//order of conf to din
		long i92,		//# objects
		long n,			//# ???
		long nd);		//# dimensions

void OrdReg(
			double * dout,	//original ord array
			double * dest,	//to be sorted copy of dout
			long nord);		//size of dout & dest

void Fit(
		 double * din,	//input assoc
		 double * dout,	//output assoc
		 double * dest,	//new assoc
		 long i92,		//size of assoc's
		 long icut,	//cut polong
		 long ireg,		//????
		 double x0,		//constant - find definition 
		 double x1,		//constant - find definition
		 double *u,		//output value
		 double *t);		//output value

void Rank(
		  double * data,	//array to be ranked
		  long n);			//size of data

void Rank2(
		   double * data,	//data to be ranked2
		   long * i1,		//value to update on return
		   long i2,			//loop value
		   double sumrnk,	//calculation value
		   double sum);		//calculation value

void SrtShl(
			double * data,
			long* indx,
			long n);

void LinReg(
			double * x,
			double * y,
			long n,
			long icut,
			long ireg,
			double *r,
			double *x0,
			double *x1);

void SrtSh1(
			double * data,
			long n);

void LinLt(
		   double positn,
		   long *irow,
		   long *icol);

};

#endif // !defined(AFX_SSH_H__C792AB83_6426_11D4_A3D4_525405F5DADA__INCLUDED_)
