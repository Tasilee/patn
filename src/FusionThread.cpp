// AsimRowThread.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "FusionThread.h"
#include "fusiondlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFusionThread

//IMPLEMENT_DYNCREATE(CFusionThread, CWinThread)
IMPLEMENT_DYNAMIC(CFusionThread, CWinThread)

BEGIN_MESSAGE_MAP(CFusionThread, CWinThread)
	//{{AFX_MSG_MAP(CGDIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFusionThread::CFusionThread()
{
	m_hEventKillFusionThread = CreateEvent(NULL, FALSE, FALSE, NULL);
	Initialize();
}

CFusionThread::~CFusionThread()
{
	CloseHandle(m_hEventKillFusionThread);
	for(long i=0;i<m_Matrix.GetSize();i++)
		delete m_Matrix[i];
}

BOOL CFusionThread::InitInstance()
{
	*m_value = !!Fuse();

	return FALSE;
}

int CFusionThread::ExitInstance()
{
	SetEvent(((CFusionDlg*)m_dlg)->m_hEventKilledFusionThread);
	m_dlg->PostMessage(WM_CLOSE, 0, 0L);

	return CWinThread::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CFusionThread message handlers

bool CFusionThread::Go()
{
	long value = 0;
	if(!m_rows){
	}else{
	}
	return !!value;
}


void CFusionThread::Initialize()
{
	m_pMatrix = NULL;
	m_pFuseTable = NULL;
	m_nAlpha1 = 0.5;
	m_nAlpha2 = 0.5;
	m_nBeta   = -0.1;
	m_nGamma  = 0.0;
	m_nOMB    = 0.0;
	m_nFuseTechnique = FlexibleUPGMA;
} // end CFusionThread::Initialize()

BOOL CFusionThread::Attach(CMatrix* pMatrix,
            CFuseTable* pFuseTable,
            long nFuseTechnique,
			double* pCutOff,  // TODO; temp code to facilitate no of groups line in dendrogram
            long nNoOfGroups,
            double nBeta,
			CArray<long>* pGroups)
{
	if ( m_pMatrix != NULL ){
      m_pDoc->m_logerror = "Unknown error";
		return FALSE;
   }

	m_pMatrix = pMatrix;
	m_pFuseTable = pFuseTable;
	m_pGroups = pGroups;

	if ( !SetFuseTechnique(nFuseTechnique) ){
   	m_pDoc->m_logerror = "Invalid Fusion Technique";
		return FALSE;
   }

	if ( !SetNoOfGroups(nNoOfGroups) )
	{
		// TODO: improve handling of the following error condition
		// - perhaps notify user at analysis dialog box time
		//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Number of groups must be greater than 2\nand less than number of objects",MB_OK | MB_ICONSTOP);
      m_pDoc->m_logerror = "number of groups to create must be >2 and <totalnumberofobjects";
		return FALSE;
	}

	m_pCutOff = pCutOff; // TODO: temp code to facilitate groups line in dendrogram
	m_nBeta = nBeta;  //TODO: make sure that nBeta is in range -1.0 to 1.0

	// Make a copy of the matrix (so that original in CTable is not altered)
	long nCount, nCount1;
	long i,j;
	CADouble *pRow, *pNewRow;
	double nValue;

	nCount = m_pMatrix->GetSize();
	m_Matrix.SetSize(nCount);
	for ( i = 0; i < nCount; i++ )
	{
		pRow = m_pMatrix->GetAt(i);
		nCount1 = pRow->GetSize();
		pNewRow = new CADouble;
		pNewRow->SetSize(nCount1);
		for ( j = 0; j < nCount1; j++ )
		{

			nValue = pRow->GetAt(j);
			pNewRow->SetAt(j,nValue);
		}
		m_Matrix.SetAt(i,pNewRow);
	}

	return TRUE;
} // end CFusionThread::Attach()

BOOL CFusionThread::SetFuseTechnique(long nFuseTechnique)
{
	switch (nFuseTechnique)
	{
		case NearestNeighbour:
		case FurthestNeighbour:
		case FlexibleWPGMA:
		case FlexibleUPGMA:
			m_nFuseTechnique = nFuseTechnique;
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

BOOL CFusionThread::SetNoOfGroups(long nNoOfGroups)
{
	if ( nNoOfGroups <= 0 || nNoOfGroups > m_pMatrix->GetSize() )
	{
		return FALSE;
	}

	m_nNoOfGroups = nNoOfGroups;

	return TRUE;
}

BOOL CFusionThread::Fuse()
{
	long i;

	ASSERT(m_pMatrix != NULL);

	if ( m_pMatrix == NULL )
	{
   	m_pDoc->m_logerror = "Unknown Error";
		return FALSE;
	}

	if ( m_pFuseTable->GetSize() > 0 )
	{
		for ( i = 0; i < m_pFuseTable->GetSize(); i++ )
		{
			delete m_pFuseTable->GetAt(i);
		}
		m_pFuseTable->RemoveAll();
	}

	m_pFuseTable->SetSize(m_pMatrix->GetSize());

	CFuseLine* pFuseLine;
	NN* pNN;
	NN* pMinNN;
	NN* pNNL;
	NN* pNNM;
	long key;
	POSITION pos;
	long L,M; 	// The key values of the two objects which are nearest neighbours
	double DLM;	// Distance, dissimilarity, difference between L & M objects
	double DL;  // Distance to from current object to L
	double DM;  // Distance to from current object to M
	long nNIG;   // No of objects in group
	long nNoOfGroups;
	long messagecount = 0;
	long k;
	long count = 0;
	if(!CreateNNMap()){
   		//m_pDoc->m_logerror = "Unknown Error:  (fusionthread:210)";
		//it now means that the user canceled the fusion process
   		return false;
	}
	DLM = 0;
	i = 0;
	while ( (nNoOfGroups = m_mapNN.GetCount()) > 1 ) // while there are objects to be fused
	{
		if ( nNoOfGroups == m_nNoOfGroups )
			*m_pCutOff = DLM;	// TODO: temp code to facilitate no of groups in dendrogram
			
		// Find closest neighbours
		pMinNN = GetMinNN();
		L = min(pMinNN->key,pMinNN->keyNN);
		M = max(pMinNN->key,pMinNN->keyNN);
		DLM = pMinNN->nLevel;

		// Store pointers to L and M NN objects;
		if ( !m_mapNN.Lookup(L,pNNL) || !m_mapNN.Lookup(M,pNNM) ){ // TODO: Error condition
      	m_pDoc->m_logerror = "Unknown Error:	(fusionthread:228)";
 			return FALSE;
      }

		// Remove L & M from map (to avoid self-comparisons)
		if ( !m_mapNN.RemoveKey(L) || !m_mapNN.RemoveKey(M) ){ // TODO: Error condition
      	m_pDoc->m_logerror = "Unknown Error:	(fusionthread:234)";
			return FALSE;
      }

		// Fuse the two objects
		pFuseLine = new CFuseLine;
		pFuseLine->nLeft = L;
		pFuseLine->nRight = M;
		pFuseLine->nLevel = DLM;
		m_pFuseTable->SetAt(i,pFuseLine);
		i++;

		// Compute new distances between objects and fused group
		pos = m_mapNN.GetStartPosition();
		while ( pos != NULL )
		{
			if (WaitForSingleObject(m_hEventKillFusionThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<m_pFuseTable->GetSize();k++) 
					delete m_pFuseTable->GetAt(k);
				m_pFuseTable->SetSize(0);
				pos = m_mapNN.GetStartPosition();
				while(pos != NULL){
					m_mapNN.GetNextAssoc(pos,k,pNN);
					delete pNN;
				}	
				delete pNNL;
				delete pNNM;
				m_mapNN.RemoveAll();
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CFusionDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			count ++;
			m_mapNN.GetNextAssoc(pos,(long&)key,(NN*&)pNN);

			// Get current distances from key to L & M
			DL = GetAssocAt(key,L);
			DM = GetAssocAt(key,M);
	
			switch (m_nFuseTechnique)
			{
				case NearestNeighbour:
					m_nAlpha1 = 0.5;
					m_nAlpha2 = 0.5;
					m_nBeta   = 0.0;
					m_nGamma  = -0.5;
					break;

				case FurthestNeighbour:
					m_nAlpha1 = 0.5;
					m_nAlpha2 = 0.5;
					m_nBeta   = 0.0;
					m_nGamma  = 0.5;
					break;

				case FlexibleWPGMA:
					m_nAlpha1 = (1.0 - m_nBeta)/2.0;
					m_nAlpha2 = m_nAlpha1;
					m_nGamma  = 0.0;
					break;

				case FlexibleUPGMA:
					nNIG = pNNL->nNbrInGroup + pNNM->nNbrInGroup;
					m_nOMB = 1.0 - m_nBeta;
					m_nAlpha1 = m_nOMB*pNNL->nNbrInGroup/nNIG;
					m_nAlpha2 = m_nOMB*pNNM->nNbrInGroup/nNIG;
					break;
			}

			// DL becomes the distance from key to the new L (ie, distance to fused L + M)
			DL = m_nAlpha1*DL + m_nAlpha2*DM + m_nBeta*DLM + m_nGamma*(fabs(DL - DM));

			// Update association matrix with distance from key to new L
			SetAssocAt(key,L,DL);

			if ( m_mapNN.GetCount() > 1 )
			{
				if ( pNN->keyNN == L || pNN->keyNN == M ) // One of the fused objects was NN
				{
					pNN->keyNN = FindNNFor(key);
					pNN->nLevel = GetAssocAt(pNN->key, pNN->keyNN);
				}
				else if ( DL < pNN->nLevel ) // The new group is a closer neighbour than current NN
				{
					pNN->keyNN = L;
					pNN->nLevel = DL;
				}
			}
			else // only one object left - has to be L's nearest neighbour
			{
				pNN->keyNN = L;
				pNN->nLevel = DL;
			}
		}
		 
		if ( m_mapNN.GetCount() > 0 )  // There are still objects to be fused
		{
			pNNL->keyNN = FindNNFor(L);
			pNNL->nLevel = GetAssocAt(pNNL->key, pNNL->keyNN);
			pNNL->nNbrInGroup += pNNM->nNbrInGroup; 
			m_mapNN.SetAt(L,pNNL);
		}
		else  // All objects have been fused
		{
			delete pNNL;
		}

		delete pNNM;
	}

	if(m_pFuseTable != NULL)
		AssignGroups();
	
	return TRUE;
} // end CFusionThread::Fuse()

bool CFusionThread::CreateNNMap()
{
	ASSERT(m_pMatrix != NULL);

	if ( m_pMatrix == NULL ) {
   	m_pDoc->m_logerror = "Unknown Error:	(fusionthread:359)";
		return false;
   }

	long nSize, nRowSize;
	long i,j;
	CADouble* pRow;
	NN *pNN, *pNN1;
	double nLevel;

	long messagecount = 0;
	long k = 0;
	POSITION pos;
	nSize = m_pMatrix->GetSize();

	// Set map entry for row 0 - 1st row in Assoc Matrix is row 1 but index starts at 0
	if ( nSize > 0 )
	{
		pRow = m_pMatrix->GetAt(0);
		pNN = new NN;
		pNN->key = 0;
		pNN->keyNN = 1;
		pNN->nLevel = pRow->GetAt(0);
		pNN->nNbrInGroup = 1;
		m_mapNN.SetAt(0,pNN);
	}

	for ( i = 0; i < nSize; i++ )
	{
		pRow = m_pMatrix->GetAt(i);
		nRowSize = pRow->GetSize();

		pNN = new NN;
		pNN->key = i + 1; // 1st row in Assoc Matrix is row 1 but index starts at 0

		// Initially, set the NN object's nearest neighbour to the first object in the row
		pNN->keyNN = 0;
		pNN->nLevel = pRow->GetAt(0);
		pNN->nNbrInGroup = 1;

		for ( j = 1; j < nRowSize; j++ )
		{
			if (WaitForSingleObject(m_hEventKillFusionThread, 0)
				== WAIT_OBJECT_0){
				pos = m_mapNN.GetStartPosition();
				delete pNN;
				while(pos != NULL){
					m_mapNN.GetNextAssoc(pos,k,pNN);
					delete pNN;
				}					
				m_mapNN.RemoveAll();
				m_pFuseTable->SetSize(0);
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CFusionDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nLevel = pRow->GetAt(j);

			// Is object in the row this object's nearest neighbour?
			if ( nLevel < pNN->nLevel )
			{
				pNN->nLevel = nLevel;
				pNN->keyNN = j;
			}

			// Is this object the nearest neighbour to the object in the row?
			if ( m_mapNN.Lookup(j,pNN1) && nLevel < pNN1->nLevel )
			{
				pNN1->nLevel = nLevel;
				pNN1->keyNN = i + 1;
			}
		}
		m_mapNN.SetAt(pNN->key,pNN);
	}
	return true;
} // end CFusionThread::CreateNNMap()

NN* CFusionThread::GetMinNN()
{
	double nMinAssoc;
	NN* pNN;
	NN* pMinNN;
	long key;
	double nLevel;
	POSITION pos;

	pos = m_mapNN.GetStartPosition();
	if ( pos != NULL )
	{
		m_mapNN.GetNextAssoc(pos,(long&)key,(NN*&)pNN);
		nMinAssoc = pNN->nLevel;
		pMinNN = pNN;
	}

	while ( pos != NULL )
	{
		m_mapNN.GetNextAssoc(pos,(long&)key,(NN*&)pNN);
		nLevel = pNN->nLevel;
		if ( nLevel < nMinAssoc )
		{
			nMinAssoc = nLevel;
			pMinNN = pNN;
		}
	} 

	return pMinNN;
} // end CFusionThread::GetMinNN()

long CFusionThread::FindNNFor(long key)
{
	double nMinAssoc, nAssoc;
	NN* pNN;
	long keyTemp, keyNN = -1;
	POSITION pos;

	pos = m_mapNN.GetStartPosition();
	if ( pos != NULL )
	{
		do
		{
			m_mapNN.GetNextAssoc(pos,(long&)keyNN,(NN*&)pNN);
		} while ( keyNN == key && pos != NULL );

		if ( pos != NULL )
			nMinAssoc = GetAssocAt(key,keyNN);
	}

	while ( pos != NULL )
	{
		m_mapNN.GetNextAssoc(pos,(long&)keyTemp,(NN*&)pNN);
		if ( keyTemp != key )
		{
			nAssoc = GetAssocAt(key,keyTemp);
			if ( nAssoc < nMinAssoc )
			{
				nMinAssoc = nAssoc;
				keyNN = keyTemp;
			}
		}
	} 

	return keyNN;
} // end CFusionThread::FindNNFor()

double CFusionThread::GetAssocAt(long i,long j)
{
	double nAssoc;
	CADouble* pRow;

	if ( j > i )
	{
		long temp = i;
		i = j;
		j = temp;
	}

	i--;  // 1st row in Assoc Matrix is row 1, but index starts at 0
	pRow = m_Matrix.GetAt(i);
	nAssoc = pRow->GetAt(j);

	return nAssoc;
} // end CFusionThread::GetAssocAt()

void CFusionThread::SetAssocAt(long i, long j, double nLevel)
{
	CADouble* pRow;

	if ( j > i )
	{
		long temp = i;
		i = j;
		j = temp;
	}

	i--;  // 1st row in Assoc Matrix is row 1, but index starts at 0
	pRow = m_Matrix.GetAt(i);
	pRow->SetAt(j,nLevel);
} // end CFusionThread::SetAssocAt()

long CFusionThread::Full(void)
{
	long n = 0;
	while(m_pGroups->GetAt(n) != -1 && 
		n < m_pGroups->GetSize())
		n++;

	return n;
}

void CFusionThread::AssignGroups(void)
{
	long group = 0;
	long n = 0;
	long i = 0;
	long p = 0;
	long idx;

	//reset group values
	for(i=0;i<m_pGroups->GetSize();i++){
		m_pGroups->SetAt(i,-1);
	}

	//for each group, AllocateGroups()
	while(p < m_pFuseTable->GetSize()){
		p = 0;
		idx = m_pFuseTable->GetAt(p)->nLeft;
		while(p < m_pFuseTable->GetSize() &&
			m_pGroups->GetAt(idx) != -1){
			p++;
			if(p < m_pFuseTable->GetSize())
				idx = m_pFuseTable->GetAt(p)->nLeft;
		}
		n = p;
		
		if(n == m_pFuseTable->GetSize()){
			p = 0;
			idx = m_pFuseTable->GetAt(p)->nLeft;
			while(p < m_pFuseTable->GetSize() &&
				m_pGroups->GetAt(idx) != -1){
				p++;
				if(p < m_pFuseTable->GetSize())
					idx = m_pFuseTable->GetAt(p)->nLeft;
			}
			n = p;
		}

		AllocateGroups(n,group);
		group++;
	}
	group--;
	//Allocate remaining groups based on order that they occur in m_pFuseTable
	p=0;
	while(p < m_pFuseTable->GetSize()){
		idx = m_pFuseTable->GetAt(p)->nLeft;
		if(m_pGroups->GetAt(idx) == -1){
			m_pGroups->SetAt(idx,group);
			group++;
		}
		idx = m_pFuseTable->GetAt(p)->nRight;
		if(m_pGroups->GetAt(idx) == -1){
			m_pGroups->SetAt(idx,group);
			group++;
		}
		p++;
	}
}


void CFusionThread::AllocateGroups(long idx,long group)
{
	//Find all elements of the 'group' that m_pFuseTable['idx'] 
	//belongs to.  Assign 'group' to their group value.
	long i,n,p;
	while(idx < m_pFuseTable->GetSize()){
		//check if left branch is done
		n = m_pFuseTable->GetAt(idx)->nLeft;
		if(m_pGroups->GetAt(n) == -1){
			//search back (before idx) to find instance of n
			p = n;
			i = 0;
			while(i < m_pFuseTable->GetSize() &&
				i < idx &&
				n != m_pFuseTable->GetAt(i)->nLeft &&
				n != m_pFuseTable->GetAt(i)->nRight){
				i++;
			}
			if(i != idx){
				//add current group
				AllocateGroups(i,group);
			}
			m_pGroups->SetAt(n,group);
		}
		
		//check if right branch is done
		n = m_pFuseTable->GetAt(idx)->nRight;
		if(m_pGroups->GetAt(n) == -1 &&
			m_pFuseTable->GetAt(idx)->nLevel <= *m_pCutOff){
			//search back (before idx) to find instance of n
			p = n;
			i = 0;
			while(i < m_pFuseTable->GetSize() &&
				i < idx &&
				n != m_pFuseTable->GetAt(i)->nLeft &&
				n != m_pFuseTable->GetAt(i)->nRight){
				i++;
			}
			if(i != idx){
				//add current group
				AllocateGroups(i,group);
			}
			m_pGroups->SetAt(n,group);
		}			

		//find next instance of m_pFuseTable->GetAt(idx)->nLeft
		p = idx+1;
		n = m_pFuseTable->GetAt(idx)->nLeft;
		while(p < m_pFuseTable->GetSize() &&
			m_pFuseTable->GetAt(p)->nLeft != n &&
			m_pFuseTable->GetAt(p)->nRight != n){
			p++;
		}
		idx = p;
		//check if next instance of m_pFuseTable is > cutoff
		//note: m_pFuseTable is in asending cutoff order.
		if(p >= m_pFuseTable->GetSize() || m_pFuseTable->GetAt(p)->nLevel > *m_pCutOff){
			idx = m_pFuseTable->GetSize();
		}
	}
}
