// Fuse.h
//

#ifndef FUSE_H
#define FUSE_H

#include "patndoc.h"

class CAssocMatrix;

struct NN
{
	long 	key;
	long 	keyNN;
	double	nLevel;
	long 	nNbrInGroup;
};
typedef struct NN NN;

typedef CArray<double> CADouble;
typedef CArray<CADouble*> CMatrix;

class CFuse 
{
public:
	CFuse(CPatnDoc * doc);
	~CFuse();

	CPatnDoc * m_pDoc;

public:
	enum
	{
		NearestNeighbour,
		FurthestNeighbour,
		FlexibleWPGMA,
		FlexibleUPGMA,
	};

public:
	double	m_nAlpha1;
	double	m_nAlpha2;
	double	m_nBeta;
	double	m_nGamma;
	double* m_pCutOff;	// TODO: temp code to facilitate groups line in dendrogram

protected:
	long		m_nFuseTechnique;
	long		m_nNoOfGroups;
	CMatrix* m_pMatrix, m_Matrix;
	CFuseTable* m_pFuseTable;
	double	m_nOMB;
	CMap<long,long,NN*,NN*> m_mapNN;
	CArray<long> *m_pGroups;

public:
	BOOL Attach(CMatrix* pMatrix,
	            CFuseTable* pFuseTable,
	            long nFuseTechnique,
				double* pCutOff,
	            long nNoOfGroups = 1,
	            double nBeta = -0.1,
				CArray<long>* pGroups = NULL);
	BOOL	Fuse();
	BOOL	SetFuseTechnique(long nFuseTechnique);
	BOOL	CFuse::SetNoOfGroups(long nNoOfGroups);
	void AssignGroups(void);

	void WriteFuseTableToASCII(CPatnDoc * m_pDoc,long nType);

protected:
	void	Initialize();
	void	CreateNNMap();
	NN*		GetMinNN();
	long		FindNNFor(long key);
	double	GetAssocAt(long i,long j);
	void	SetAssocAt(long i, long j, double nLevel);
	long		Full(void);
	void	AllocateGroups(long idx,long group);

};
#endif // FUSE_H
