// Mst.cpp : implementation file
//

#include "stdafx.h"
#include "PatnDoc.h"
#include "patn.h"
#include "Mst.h"
//#include "gl\gl.h"
//#include "gl\glu.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMst

CMst::CMst(CPatnDoc * pDoc)
{
	m_pDoc = pDoc;
	m_pDoc->m_mst_changed = true;
}

CMst::~CMst()
{
}

long CMst::OnCreate() 
{
	CArray<neighbour*>* selection;
	neighbour *np;
	long i, j, n;
	half * temphalf;
	vertex minVertex, w, sizeofU;
	weight minWeight, wgt;
	weight minTotalCost = (weight) 0;
	
	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	if(m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() == 0 ||
		m_pDoc->m_AssocRowChange)
		return false;

	//initialise MST
	m_Mst.SetSize(n);
	for (i = 0; i < n; i++) {
		temphalf = new half;
		temphalf->SetSize(n);
		//m_Mst[i].SetSize(n);
		m_Mst.SetAt(i,temphalf);
		for (j = 0; j < n; j++) {
			//m_Mst[i][j] = 0;
			temphalf->SetAt(j,0);
		}
	}

	selection = new CArray<neighbour*>;
	selection->SetSize(n);
	for(i=0;i<selection->GetSize();i++){
		np = new neighbour;
		np->in_mst = false;
		np->nearest = NO_VERTEX;
		selection->SetAt(i,np);
	}

	selection->GetAt(0)->in_mst = true;
	for (i = 1; i < n; i++) {
		np = selection->GetAt(i);
		np->nearest = 0;
		np->cost = m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(i-1)->GetAt(0);
	}

	for (sizeofU = 1; sizeofU < n; sizeofU++) {
		minVertex = NO_VERTEX;
		for (w = 0; w < n; w++) {
			np = selection->GetAt(w);
			if (np->in_mst || np->nearest == NO_VERTEX)
				continue;
			if (minVertex == NO_VERTEX || np->cost < minWeight) {
				minVertex = w;
				minWeight = np->cost;
			}
		}

		//update selection array
		np = selection->GetAt(minVertex);
		np->in_mst = true;
		minTotalCost += minWeight;

		m_Mst[np->nearest]->SetAt(minVertex,1);

		for (i = 1; i < n; i++) {
			np = selection->GetAt(i);
			if (np->in_mst)
				continue;
			if (i < minVertex) {
				wgt = m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(minVertex - 1)->GetAt(i);
			} else {
				wgt = m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(i - 1)->GetAt(minVertex);
			}
			if (np->nearest == NO_VERTEX || wgt < np->cost) {
				np->nearest = minVertex;
				np->cost = wgt;
			}
		}
	}

	for (i = 0; i < selection->GetSize(); i++) {
		delete selection->GetAt(i);
	}
	delete selection;
	
	//insert into patndoc
	for(i=0;i<m_pDoc->m_aMst.GetSize();i++)
		delete m_pDoc->m_aMst[i];
	m_pDoc->m_aMst.SetSize(m_Mst.GetSize());
	for(i=0;i<m_Mst.GetSize();i++)
		m_pDoc->m_aMst[i] = m_Mst[i];

	return 0;
}

