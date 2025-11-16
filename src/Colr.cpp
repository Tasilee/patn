// Colr1.cpp: implementation of the CColr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
//CPatnDoc class needs this
#include "patndoc.h"
#include "Colr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColr::CColr(CPatnDoc* pDoc)
{
	m_pDoc = pDoc;
}

CColr::~CColr()
{

}

int CColr::OnCreate(void)
{
	long i, j, m, n;
	long count;
	double x, y, z;
	colour max,min;

	//Don't need this as "new" operator not used
	//for(i=0;i<m_pDoc->m_GroupColours.GetSize();i++)
	//	delete m_pDoc->m_GroupColours[i];
	//for(i=0;i<m_pDoc->m_ObjectColours.GetSize();i++)
	//	delete m_pDoc->m_ObjectColours[i];
	m_pDoc->m_aGroupColours.RemoveAll();
	m_pDoc->m_aObjectColours.RemoveAll();
	
	groupColours = &m_pDoc->m_aGroupColours;
	objectColours = &m_pDoc->m_aObjectColours;

	//See above for this
	//m_pDoc = GetDocument();

	//n = m_pDoc->GetNoOfRows();
	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	//Shouldn't the colours be ranged between 0 & 255?
	min.red = max.red =(float) m_pDoc->m_aRowXCoords[0];
	if(m_pDoc->m_Ord3d < 2)
	{min.green = 0; max.green = 1;}
	else
		min.green = max.green = (float) m_pDoc->m_aRowYCoords[0];
	if(m_pDoc->m_Ord3d < 3)
	{min.blue = 0; max.blue = 1;}
	else
		min.blue = max.blue = (float) m_pDoc->m_aRowZCoords[0];

	objectColours->SetSize(n);
	for (i = 0; i < n; i++) {
		(*objectColours)[i].red = (float) m_pDoc->m_aRowXCoords[i];
		if(m_pDoc->m_Ord3d < 2) (*objectColours)[i].green = .5;
		else (*objectColours)[i].green = (float) m_pDoc->m_aRowYCoords[i];
		if(m_pDoc->m_Ord3d < 3) (*objectColours)[i].blue = .5;
		else (*objectColours)[i].blue = (float) m_pDoc->m_aRowZCoords[i];

		if((*objectColours)[i].red < min.red) min.red = (*objectColours)[i].red;
		if((*objectColours)[i].red > max.red) max.red = (*objectColours)[i].red;
		if(m_pDoc->m_Ord3d > 1)
		{
			if((*objectColours)[i].green < min.green) min.green = (*objectColours)[i].green;
			if((*objectColours)[i].green > max.green) max.green = (*objectColours)[i].green;
		}
		
		if(m_pDoc->m_Ord3d > 2)
		{
			if((*objectColours)[i].blue < min.blue) min.blue = (*objectColours)[i].blue;
			if((*objectColours)[i].blue > max.blue) max.blue = (*objectColours)[i].blue;
		}
	}

	//range all
	for (i = 0; i < n; i++) {
		(*objectColours)[i].red = ((*objectColours)[i].red - min.red) / 
			(max.red - min.red);
		(*objectColours)[i].green = ((*objectColours)[i].green - min.green) / 
			(max.green - min.green);
		if (max.blue == min.blue) {
			(*objectColours)[i].blue = 0;
		} else {
			(*objectColours)[i].blue = ((*objectColours)[i].blue - min.blue) / 
				(max.blue - min.blue);
		}
	}

	//initializing groupColours
	m = m_pDoc->m_Groups.GetSize();
	groupColours->SetSize(m);
	for (j = 0; j < m; j++) {
		x = 0; y = 0; z = 0; count = 0;
		for (i = 0; i < m_pDoc->m_Rows[j]->GetSize(); i++) {
			//if (m_pDoc->m_aRowGroups.GetRowGroups(i) == (j + 1)) {
			//changed as lowest group value possible is zero
			if(m_pDoc->m_Rows[j]->GetAt(i) < m_pDoc->m_aRowXCoords.GetSize())
			{
				x += m_pDoc->m_aRowXCoords[m_pDoc->m_Rows[j]->GetAt(i)];
				y += m_pDoc->m_aRowYCoords[m_pDoc->m_Rows[j]->GetAt(i)];
				z += m_pDoc->m_aRowZCoords[m_pDoc->m_Rows[j]->GetAt(i)];
			}			
			count++;
		}	
		centroid.x = x / (double) count;
		if(m_pDoc->m_Ord3d < 2)	centroid.y = 0.5;
		else centroid.y = y / (double) count;
		if(m_pDoc->m_Ord3d < 3) centroid.z = 0.5;
		else centroid.z = z / (double) count;
		
		//range group colours as it goes
		(*groupColours)[j].red = (float) ((centroid.x - min.red) / 
			(max.red - min.red));
		(*groupColours)[j].green = (float) ((centroid.y - min.green) / 
			(max.green - min.green));
		(*groupColours)[j].blue = (float) ((centroid.z - min.blue) / 
			(max.blue - min.blue));
	}
	//write back min&max's
	m_pDoc->m_OrdXmin = min.red;
	m_pDoc->m_OrdXmax = max.red;
	m_pDoc->m_OrdYmin = min.green;
	m_pDoc->m_OrdYmax = max.green;
	m_pDoc->m_OrdZmin = min.blue;
	m_pDoc->m_OrdZmax = max.blue;


	return 0;
}
