#include "stdafx.h"
#include "PatnDoc.h"
#include "dend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long END = -1;

CDend::CDend(CFuseTable* pFuseTable, CDendOrder* pDendOrder,
			 CDendOrder* pGroupDendOrder)
{
	Initialize();
	Attach(pFuseTable, pDendOrder,pGroupDendOrder);
}

CDend::CDend()
{
	Initialize();
}

void CDend::Initialize()
{
	m_pFuseTable = NULL;
	m_pDendOrder = NULL;
} 

void CDend::Attach(CFuseTable* pFuseTable, CDendOrder* pDendOrder,
				   CDendOrder* pGroupDendOrder)
{
	m_pFuseTable = pFuseTable;
	m_pDendOrder = pDendOrder;
	m_pGroupDendOrder = pGroupDendOrder;
}

BOOL CDend::SetDendOrder(CFuseTable* pFuseTable, CDendOrder* pDendOrder, 
						 CDendTable* pDendTable, CArray<long> *Groups,
						 CDendOrder* pGroupDendOrder, CDendTable* pGroupDendTable)
{
	Initialize();
	Attach(pFuseTable, pDendOrder, pGroupDendOrder);

	CDendOrder* pTempOrder = new CDendOrder;
	long i;
	long next;
	long nTemp;
	CFuseLine* pFuseLine;
	CDendLine* pDendLine;
	long nUpperBound;
	long nSize;
	CArray<double> X,Y,X2,Y2;
	long key;
	CArray<long> tempGroups;

	tempGroups.SetSize(Groups->GetSize());
	for(i=0;i<Groups->GetSize();i++)
		tempGroups[i] = Groups->GetAt(i);

	if ( (nUpperBound = m_pFuseTable->GetUpperBound()) == -1 )
		return FALSE;

	if ( pDendTable->GetSize() > 0 )
	{
		for ( i = 0; i < pDendTable->GetSize(); i++ )
		{
			delete pDendTable->GetAt(i);
		}
		pDendTable->RemoveAll();
	}
	if ( pGroupDendTable->GetSize() > 0 )
	{
		for ( i = 0; i < pGroupDendTable->GetSize(); i++ )
		{
			delete pGroupDendTable->GetAt(i);
		}
		pGroupDendTable->RemoveAll();
	}

	pTempOrder->SetSize(nUpperBound+2); // Fusion table is 1 less than no. of objects
	
	// Set the first elements in the DendOrder array
	pFuseLine = pFuseTable->GetAt(nUpperBound);
	pTempOrder->SetAt(pFuseLine->nLeft,pFuseLine->nRight);
	pTempOrder->SetAt(pFuseLine->nRight,END);

	// First, work out the links
	for ( i = nUpperBound - 1; i >= 0; i-- )
	{
		pFuseLine = pFuseTable->GetAt(i);
		nTemp = pTempOrder->GetAt(pFuseLine->nLeft);
		pTempOrder->SetAt(pFuseLine->nLeft,pFuseLine->nRight);
		pTempOrder->SetAt(pFuseLine->nRight,nTemp);
	}

	// .... then store in link order
	m_pDendOrder->SetSize(pTempOrder->GetSize());
	i = 0;
	next = 0;
	while ( next != -1 )
	{
		m_pDendOrder->SetAt(i++,next);
		next = pTempOrder->GetAt(next);
	}

	// Create key lookup table;
	nSize = m_pDendOrder->GetSize();
	X.SetSize(nSize);
	X2.SetSize(nSize);
	Y.SetSize(nSize);
	Y2.SetSize(nSize);
	long group = 1;
	for ( i = 0; i < nSize; i++ )
	{
		key = m_pDendOrder->GetAt(i);
		X[key] = 0;
		X2[key] = 0;
		Y[key] = i;

		//also assign groups here
		if(i != 0 && tempGroups[key] != tempGroups[m_pDendOrder->GetAt(i-1)])
			group++;
		Groups->SetAt(key,group);
		
		Y2[key] = group;
	}

	// Create table of dendrogram line structures
	nSize = pFuseTable->GetSize();
	pDendTable->SetSize(nSize);
	for ( i = 0; i < nSize; i++ )
	{
		pDendLine = new CDendLine;
		pFuseLine = pFuseTable->GetAt(i);
		pDendLine->xStart = X[pFuseLine->nLeft];
		pDendLine->xRight = pFuseLine->nLevel;
		pDendLine->yStart = Y[pFuseLine->nLeft];
		pDendLine->xEnd = X[pFuseLine->nRight];
		pDendLine->yEnd = Y[pFuseLine->nRight];
		pDendTable->SetAt(i,pDendLine);
		Y[pFuseLine->nLeft] = (Y[pFuseLine->nLeft] + Y[pFuseLine->nRight])/2;
		X[pFuseLine->nLeft] = pFuseLine->nLevel;
	}

	nSize = pFuseTable->GetSize();
	pGroupDendTable->SetSize(group-1);

	//create groupdend
	for ( i = nSize-group+1; i < nSize; i++ ){
		pDendLine = new CDendLine;
		pFuseLine = pFuseTable->GetAt(i);
		pDendLine->xStart = X2[pFuseLine->nLeft];
		pDendLine->xRight = pFuseLine->nLevel;
		pDendLine->yStart = Y2[pFuseLine->nLeft];
		pDendLine->xEnd = X2[pFuseLine->nRight];
		pDendLine->yEnd = Y2[pFuseLine->nRight];
		pGroupDendTable->SetAt(i-nSize+group-1,pDendLine);
		Y2[pFuseLine->nLeft] = (Y2[pFuseLine->nLeft] + Y2[pFuseLine->nRight])/2;
		X2[pFuseLine->nLeft] = pFuseLine->nLevel;
	}
	pGroupDendOrder->SetSize(group);
	for(i=0;i<group;i++){
		pGroupDendOrder->SetAt(i,i);
	}
		

	delete pTempOrder;

	return TRUE;
}

BOOL CDend::SetDendTable(CFuseTable* pFuseTable, CDendOrder* pDendOrder, 
						 CDendTable* pDendTable)
{
	Initialize();
	Attach(pFuseTable, pDendOrder, NULL);

	CDendOrder* pTempOrder = new CDendOrder;
	long i;
	CFuseLine* pFuseLine;
	CDendLine* pDendLine;
	long nSize;
	CArray<double> X,Y,X2,Y2;
	long key;
	CArray<long> tempGroups;

	if ( pDendTable->GetSize() > 0 )
	{
		for ( i = 0; i < pDendTable->GetSize(); i++ )
		{
			delete pDendTable->GetAt(i);
		}
		pDendTable->RemoveAll();
	}

	
	// Create key lookup table;
	nSize = m_pDendOrder->GetSize();
	X.SetSize(nSize);
	Y.SetSize(nSize);
	long group = 1;
	for ( i = 0; i < nSize; i++ )
	{
		key = m_pDendOrder->GetAt(i);
		X[key] = 0;
		Y[key] = i;
	}

	// Create table of dendrogram line structures
	nSize = pFuseTable->GetSize();
	pDendTable->SetSize(nSize);
	for ( i = 0; i < nSize; i++ )
	{
		pDendLine = new CDendLine;
		pFuseLine = pFuseTable->GetAt(i);
		pDendLine->xStart = X[pFuseLine->nLeft];
		pDendLine->xRight = pFuseLine->nLevel;
		pDendLine->yStart = Y[pFuseLine->nLeft];
		pDendLine->xEnd = X[pFuseLine->nRight];
		pDendLine->yEnd = Y[pFuseLine->nRight];
		pDendTable->SetAt(i,pDendLine);
		Y[pFuseLine->nLeft] = (Y[pFuseLine->nLeft] + Y[pFuseLine->nRight])/2;
		X[pFuseLine->nLeft] = pFuseLine->nLevel;
	}
	
	return TRUE;
}

