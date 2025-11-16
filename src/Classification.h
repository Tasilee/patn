// Classification.h
//

#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

class CPatnDoc;

class CClassification 
{
public:
	CClassification(CPatnDoc* pDoc);

public:
	enum ClassStrategies
	{
		none,
		fuse,
		alloc
	};

	enum
	{
		columns,
		rows
	};
	double radius;
	long groupsFormed;

	HWND m_parent;
protected:
	CPatnDoc* m_pDoc;

		

public:
	BOOL Fuse(long nType, long nFuseTechnique, long nNoOfGroups, double nBeta);
	BOOL Alloc(long nAssocMeasure, long nNoOfGroups);
    bool Allocate(long nNoOfGroups);
	CArray<double> seeds;
	long counter;
	long notInGroup;
	long inGroupNr;

};

#endif // CLASSIFICATION_H