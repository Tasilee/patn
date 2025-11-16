// MCAO.cpp: implementation of the CMCAO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "pcc.h"
#include "newDialog.h"
#include "MCAO.h"
#include "permut.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMCAO::CMCAO()
{
	m_numPermutations = 100;
	m_Seed = 1245;
	m_Random = false;
}

CMCAO::~CMCAO()
{

}


bool CMCAO::Create(CPatnDoc *pDoc)
{

	WaitMessage();

	//must only operate on columns that have had pcc run on them...
	m_pDoc = pDoc;

	CPcc pcc(pDoc);

	long i,j;//,k;
	Row newCol;
	Row * rList;
	double rsquared;
//	LPMSG lmsg;
	CPermut cpermut;
	long progress = 0;
	long step = m_numPermutations*m_pDoc->m_Vectors.GetSize()/100;
	long top_seed,current_seed;
	long nRows;
	if((nRows = m_pDoc->GetIdxTopExtrinsicRow()) == -1)
		nRows = m_pDoc->GetNoOfRows();

	cpermut.initPermut(nRows);

	for(i=0;i<m_pDoc->m_MCAOTable.GetSize();i++)
		delete m_pDoc->m_MCAOTable[i];
	m_pDoc->m_MCAOTable.RemoveAll();
	m_pDoc->m_MCAOTable.SetSize(m_pDoc->m_Vectors.GetSize());
		
	top_seed = m_Seed;

	if(!((CPatnApp*)AfxGetApp())->silent_mode ) m_dlg->m_Label.SetWindowText("MCAO...");
	
	for(i=0;i<m_pDoc->m_Vectors.GetSize() && (((CPatnApp*)AfxGetApp())->silent_mode  || !m_dlg->m_Cancel);i++){
		current_seed = top_seed;
		if(m_pDoc->m_Vectors[i] == NULL || m_pDoc->m_Vectors[i]->GetAt(0) == ANALYSIS_VALUE){
			m_pDoc->m_MCAOTable.SetAt(i,new Row);
			for(j=0;j<m_numPermutations;j++){
				progress++;
				if(!((CPatnApp*)AfxGetApp())->silent_mode)
				{
					if(progress == step){
						progress = 0;
						m_dlg->inc();					
					}
				}
			}
			continue;
		}
		//build new dataset (nRows x numPermutations)
		//newCol.RemoveAll();
		//for(k=0;k<nRows;k++){
		//	newCol.Add(m_pDoc->GetValueAt(i,k));
		//}	
		newCol.SetSize(nRows);
		rList = new Row;
		for(j=0;j<m_numPermutations && (((CPatnApp*)AfxGetApp())->silent_mode || !m_dlg->m_Cancel);j++){
			//step progress bar...
			progress++;
			if(progress == step){
				progress = 0;
				if(!((CPatnApp*)AfxGetApp())->silent_mode)
				{
					m_dlg->inc();
					if(m_dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "MCAO Cancelled by user.";
						return false;
					}
				}
			}
				
		//	PeekMessage(lmsg,(HWND)AfxGetMainWnd(),0,100,PM_NOREMOVE);
		//	m_dlg->IsDialogMessage(lmsg);

			//sort newCol...
			//if(m_Random)
			if(true)
				//permut(&newCol,m_Seed);
				permut(&newCol,current_seed+j,i);
			else
				cpermut.Permut(&newCol,m_Seed,false);

			//run pcc
			rsquared = pcc.OnCreate2(&newCol);
			rList->Add(rsquared);
		}

		//sort rList
		SrtShl(rList);
		
		//add to patndoc for later data extraction
		m_pDoc->m_MCAOTable.SetAt(i,rList);	
		if(!((CPatnApp*)AfxGetApp())->silent_mode)
		{	
			if(m_dlg->m_Cancel)
				break;
		}
	}
	if(!((CPatnApp*)AfxGetApp())->silent_mode && m_dlg->m_Cancel)
	{
		//cleanup
		for(;i>=0;i--)
		{
			delete m_pDoc->m_MCAOTable[i];
		}
		m_pDoc->m_MCAOTable.RemoveAll();
		return false;
	}


	double total,value;
	CString strBuffer,strLabel;

	if(m_pDoc->m_MCAOTable.GetSize() > m_pDoc->m_Vectors.GetSize()) 
		return false;

	m_pDoc->m_MCAOResults.SetSize(m_pDoc->m_MCAOTable.GetSize());
	for(i=0;i<m_pDoc->m_MCAOTable.GetSize();i++){
		if(m_pDoc->m_Vectors[i]->GetAt(0) == ANALYSIS_VALUE){
			m_pDoc->m_MCAOResults[i] = ANALYSIS_VALUE;
			continue;
		}
		value = m_pDoc->m_Vectors[i]->GetAt(m_pDoc->m_Vectors[i]->GetSize()-1);
		for(j=0,total=0;j<m_pDoc->m_MCAOTable[i]->GetSize();j++){
			if(m_pDoc->m_MCAOTable[i]->GetAt(j) > value){
				total++;
			}	
		}
		total /= (double)(m_pDoc->m_MCAOTable[i]->GetSize());

		m_pDoc->m_MCAOResults[i] = total;
	}


	return true;
}

void CMCAO::SrtShl(Row* data)
{
	long i,j,fm,ii;

	long fn = data->GetSize();
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,fn-m);i++){
			if(data->GetAt(i-1) > data->GetAt(i+m-1)){
				tempd = data->GetAt(i+m-1);
				data->SetAt(i+m-1,data->GetAt(i-1));
				if(i <= m){
					data->SetAt(i-1,tempd);
				}else{
					for(j=i-m;j>=1;j-=m){
						if(tempd >= data->GetAt(j-1)) break;
						data->SetAt(j+m-1,data->GetAt(j-1));
					}
					data->SetAt(j+m-1,tempd);
				}
			}
		}
		m = fm/2;
	}
}

void CMCAO::permut(Row*data,long seed,long idx)
{
	CArray<long> order;
	CArray<long> sourcelist;

	long i,n;
	//Row newdata;

	n = data->GetSize();

	//randomize number generator
	//srand(seed);
	((CPatnApp*)AfxGetApp())->RandomSeed(seed);

	//newdata.SetSize(n);

	//build sourcelist
	for(i=0;i<n;i++) sourcelist.Add(i);

	while(order.GetSize() < n){
		i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*(sourcelist.GetSize()-1));
		order.Add(sourcelist[i]);
		sourcelist.RemoveAt(i);
	}
	//complete permitivity (reorder)
	for(i=0;i<order.GetSize();i++) 
		//newdata.SetAt(i,data->GetAt(order[i]));
		data->SetAt(i,m_pDoc->GetValueAt(idx,order[i]));
	//for(i=0;i<order.GetSize();i++)
	//	data->SetAt(i,newdata[i]);
}


bool CMCAO::IsElem(CArray<long> *order,long i)
{
	for(long n=0;n<order->GetSize();n++){
		if(order->GetAt(n) == i) 
			return true;
	}
	return false;
}
