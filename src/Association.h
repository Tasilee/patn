// Association.h
//

#ifndef ASSOCIATION_H
#define ASSOCIATION_H

class CPatnDoc;

class CAssociation 
{
public:
	CAssociation(CPatnDoc* pDoc);

public:
	enum AssociationMeasures
	{
		none,
		BrayCurtis,
		GowerMetric,
		MinkowskiSeries,
		TwoStep,
		Kulzinski
	};

protected:
	CPatnDoc* m_pDoc;


// Saving
public:
	void WriteAssocMatrixToASCII(long nType);

};

#endif // ASSOCIATION_H





