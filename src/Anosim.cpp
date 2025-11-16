// Anosim.cpp: implementation of the CAnosim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "newdialog.h"
#include "Anosim.h"
#include <math.h>
#include "chooseanosim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


bool IsElem(CArray<long> *order,long i)
{
	for(long n=0;n<order->GetSize();n++){
		if(order->GetAt(n) == i) 
			return true;
	}
	return false;
}

//deal with result structures
void CAnosim::ResetStructures(bool columns)
{
	CArray<CAssocMatrixRow*> * save;
	CArray<CAssocMatrixRow*> * best;
	CArray<CAssocMatrixRow*> * stat;
	if(columns){
		save = &m_pDoc->m_ColPairSave;
		best = &m_pDoc->m_ColPairBest;
		stat = &m_pDoc->m_ColPairStat;
	}else{
		save = &m_pDoc->m_RowPairSave;
		best = &m_pDoc->m_RowPairBest;
		stat = &m_pDoc->m_RowPairStat;
	}

	for(long i=0;i<save->GetSize();i++){
		delete save->GetAt(i);
		delete stat->GetAt(i);
		delete best->GetAt(i);
	}
	save->SetSize(0);
	stat->SetSize(0);
	best->SetSize(0);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnosim::CAnosim(CPatnDoc * pDoc)
{
	m_pDoc = pDoc;
}

CAnosim::~CAnosim()
{

}

bool CAnosim::AnosimRows(CnewDialog * newDialog,long iterations,long seed,bool autoeval,int patn_generated_groups)
{
	//deal with result structures
	ResetStructures(false);

	

	m_dlg = newDialog;

	m_iterations = iterations;
	m_seed = seed;

	CArray<long> order,group,new_group;
	double hits,m2;
	CString buffer;
	long nr,fnr;
	double totw,totb;
	double numw,numb;
	bool ranked = false;
	double rbest = 0.0;
	long n = m_pDoc->GetNoOfRows();
	CArray<CAssocMatrixRow*> *m_aMatrix;

	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();
	
	CArray<long> * grps;
	long enough_groups = 0;
	long p;
	if(patn_generated_groups)
	{
		grps = &m_pDoc->m_AprioriGroups;
		if(grps->GetSize() == 0)
		{
			enough_groups = 0;
		}
		else
		{
			p = grps->GetAt(0);
			for(long i=1;i<grps->GetSize();i++)
			{
				if(p != grps->GetAt(i))
				{
					enough_groups = 2;
					i = grps->GetSize();
				}				
			}
		}
	}
	else
	{
		grps = &m_pDoc->m_aRowGroups;
		enough_groups = m_pDoc->m_Groups.GetSize();
	}

	//randomize number generator
	//srand(m_seed);
	((CPatnApp*)AfxGetApp())->RandomSeed(m_seed);

	//patnold defaults
	long nrand = m_iterations;
	char symb;
	long i,j;
	long ig,jg;
	double as,r,save;
	double stat;
	r = save = 0;

	m_aMatrix = &m_pDoc->m_AssocMatrixRows.m_aMatrix;

	if(m_pDoc->m_AssocRowChange || (m_aMatrix->GetSize() != -1 && m_aMatrix->GetSize() < n-1)){
		//show an error
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Row Association Required for Row ANOSIM.");
		}
		m_pDoc->m_log += "- Row ANOSIM failed.\r\n\r\n";
		return FALSE;
	}

	

	if(enough_groups < 2){
		//show error0
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("More than one object group must exist for Row ANOSIM.");
		}
		m_pDoc->m_log += "- Row ANOSIM failed.\r\n\r\n";
		return FALSE;
	}

	if(m_aMatrix->GetAt(0)->GetAt(0) > 1 || m_aMatrix->GetAt(1)->GetAt(0) > 1 ||
		m_aMatrix->GetAt(n-2)->GetAt(n-3) > 1 || m_aMatrix->GetAt(n-2)->GetAt(n-2) > 1){
		ranked = true;
		symb = '-';
		//assume ranked
	}else{
		ranked = false;
		symb = '/';
		//assume not ranked
	}

	CArray<long> sourceList;

	//setup group
    group.RemoveAll();
	for(i=0;i<n;i++){
		group.Add(i);
	}
	new_group.SetSize(n);
	order.RemoveAll();
	order.Add((long) floor(((CPatnApp*)AfxGetApp)->Rand()*(n-1)));
	while(order.GetSize() < n){
		i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*n);
		if(i < n && !IsElem(&order,i))
			order.Add(i);
	}
	//randomize loop
	long progress = 0;
	long step = nrand/100;

	hits = 0.0;
	m2 = (n*(n-1.0)/2.0)/2.0;

	for(nr=0;nr<nrand;nr++){
		//ini
		fnr = nr;
		totw = 0.0;
		totb = totw;
		numw = 0.0;
		numb = numw;
		//pairs
		for(i=0;i<n-1;i++){
			ig = grps->GetAt(group[i]);//m_pDoc->m_aRowGroups[group[i]];
			for(j=i+1;j<n;j++){
				jg = grps->GetAt(group[j]);//m_pDoc->m_aRowGroups[group[j]]; 
				as = m_aMatrix->GetAt(j-1)->GetAt(i);
				if(ig == jg){
					totw += as;
					numw++;
				}else{
					totb += as;
					numb++;
				}
			}
		}
		//B/W
		totw /= numw;
		totb /= numb;
		if(ranked)
			r = (totb-totw)/(double)m2;
		else
			r = totb/totw;
	
		//store real/update hits
		if(nr == 0){
			save = r;
		}else{
			rbest = max(rbest,r);
			if(save <= r)
				hits++;
		}
		
		stat = hits/(double)(fnr) * 100;

		if(progress == step){
if(!((CPatnApp*)AfxGetApp())->silent_mode)			m_dlg->inc();
			progress = 0;
		}
		progress++;
			
		//randomize
		if(nr != nrand){
			//create another order set
			sourceList.RemoveAll();
			for(i=0;i<n;i++) sourceList.Add(i);

			order.RemoveAll();
			while(order.GetSize() < n){
				i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*(sourceList.GetSize()));
				if(i > sourceList.GetSize()) i--;
				order.Add(sourceList[i]);
				sourceList.RemoveAt(i);
			}
			//complete permitivity (reorder)
			for(i=0;i<order.GetSize();i++) 
				new_group[i] = group[order[i]];
			for(i=0;i<order.GetSize();i++)
				group[i] = new_group[i];

		}
	}

	m_pDoc->m_RowAsim_stat = stat;
	m_pDoc->m_RowAsim_rbest = rbest;
	m_pDoc->m_RowAsim_save = save;
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_RowAsim_time = time.Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_RowAsim_asso = m_pDoc->m_AssocMatrixRows.m_strMeasure + CString(" on ") + CString((!m_pDoc->m_RowAsim_patn_groups)?"PATN generated groups":"a-priori groups");
	m_pDoc->m_RowAsim_iterations = m_iterations;
	m_pDoc->m_RowAsim_seed = m_seed;

	return TRUE;
}

bool CAnosim::AnosimCols(CnewDialog * newDialog,long iterations,long seed,bool autoeval)
{
	//deal with result structures
	ResetStructures(true);

	m_dlg = newDialog;
	m_iterations = iterations;
	m_seed = seed;

	CArray<long> order,group,new_group;
	double hits,m2;
	CString buffer;
	long nr,fnr;
	double totw,totb;
	double numw,numb;
	bool ranked = false;
	double rbest = 0.0;
	long n;
	CArray<CAssocMatrixRow*> *m_aMatrix;

	if ( m_pDoc->IsExtrinsicColumns() )
		n = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		n = m_pDoc->GetNoOfColumns();

	//randomize number generator
	//srand(m_seed);
	((CPatnApp*)AfxGetApp())->RandomSeed(m_seed);

	//patnold defaults
	long nrand = m_iterations;
	char symb;
	long i,j;
	long ig,jg;
	double as,r,save;
	double stat;

	m_aMatrix = &m_pDoc->m_AssocMatrixCols.m_aMatrix;

	if(m_pDoc->m_AssocColChange || (m_aMatrix->GetSize() != -1 && m_aMatrix->GetSize() < n-1)){
		//show an error
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Column Association Required for Column ANOSIM.");
		}
		m_pDoc->m_log += "- Column ANOSIM failed.\r\n\r\n";
		return FALSE;
	}

	if(m_pDoc->m_ColGroups.GetSize() < 2){
		//show error
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("More than one attribute group must exist for Column ANOSIM.");
		}
		m_pDoc->m_log += "- Column ANOSIM failed.\r\n\r\n";
		return FALSE;
	}

	if(m_aMatrix->GetAt(0)->GetAt(0) > 1 || m_aMatrix->GetAt(1)->GetAt(0) > 1 ||
		m_aMatrix->GetAt(n-2)->GetAt(n-3) > 1 || m_aMatrix->GetAt(n-2)->GetAt(n-2) > 1){
		ranked = true;
		symb = '-';
		//assume ranked
	}else{
		ranked = false;
		symb = '/';
		//assume not ranked
	}

	CArray<long> sourceList;

	//setup group
    group.RemoveAll();
	for(i=0;i<n;i++){
		group.Add(i);
	}
	new_group.SetSize(n);
	order.RemoveAll();
	order.Add((long) floor(((CPatnApp*)AfxGetApp)->Rand()*(n-1)));
	while(order.GetSize() < n){
		i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*n);
		if(i < n && !IsElem(&order,i))
			order.Add(i);
	}

if(!((CPatnApp*)AfxGetApp())->silent_mode)	m_dlg->m_Progress.SetRange32(0,nrand);

	//randomize loop
	hits = 0.0;
	m2 = (n*(n-1.0)/2.0)/2.0;
	for(nr=0;nr<nrand;nr++){
		//ini
		fnr = nr;
		totw = 0.0;
		totb = totw;
		numw = 0.0;
		numb = numw;
		//pairs
		for(i=0;i<n-1;i++){
			ig = m_pDoc->m_aColumnGroups[group[i]];
			for(j=i+1;j<n;j++){
				jg = m_pDoc->m_aColumnGroups[group[j]];
				as = m_aMatrix->GetAt(j-1)->GetAt(i);
				if(ig == jg){
					totw += as;
					numw++;
				}else{
					totb += as;
					numb++;
				}
			}
		}
		//B/W
		totw /= numw;
		totb /= numb;
		if(ranked)
			r = (totb-totw)/m2;
		else
			r = totb/totw;
	
		//store real/update hits
		if(nr == 0){
			save = r;
		}else{
			rbest = max(rbest,r);
			if(save <= r)
				hits++;
		}

		stat = hits/(fnr)*100;

if(!((CPatnApp*)AfxGetApp())->silent_mode)		m_dlg->inc();
		
		//randomize
		if(nr != nrand){
			//create another order set
			sourceList.RemoveAll();
			for(i=0;i<n;i++) sourceList.Add(i);

			order.RemoveAll();
			while(order.GetSize() < n){
				i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*(sourceList.GetSize()-1));
				order.Add(sourceList[i]);
				sourceList.RemoveAt(i);
			}
			//complete permitivity (reorder)
			for(i=0;i<order.GetSize();i++) 
				new_group[i] = group[order[i]];
			for(i=0;i<order.GetSize();i++)
				group[i] = new_group[i];
		}
	}
	m_pDoc->m_ColumnAsim_stat = stat;
	m_pDoc->m_ColumnAsim_rbest = rbest;
	m_pDoc->m_ColumnAsim_save = save;
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_ColumnAsim_time = time.Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_ColumnAsim_asso = m_pDoc->m_AssocMatrixCols.m_strMeasure;
	m_pDoc->m_ColumnAsim_iterations = m_iterations;
	m_pDoc->m_ColumnAsim_seed = m_seed;

	return TRUE;
}

	                                                               
bool CAnosim::AnosimPairs(bool columns,CnewDialog * newDialog,
		CArray<long> * groups,long iterations,long seed,bool autoeval,int patn_generated_groups)
{
	//check for errors
	if(groups->GetSize() < 2){
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("fewer than 2 objects exist.\nAnosim will not be performed.");
		}
		if(columns)		
			m_pDoc->m_log += "- Column ANOSIM failed.\r\n\r\n";
		else
			m_pDoc->m_log += "- Row ANOSIM failed.\r\n\r\n";
		return false;
	}

	//deal with result structures
	ResetStructures(columns);
	CArray<CAssocMatrixRow*> * Save;
	CArray<CAssocMatrixRow*> * Best;
	CArray<CAssocMatrixRow*> * Stat;
	if(columns){
		Save = &m_pDoc->m_ColPairSave;
		Best = &m_pDoc->m_ColPairBest;
		Stat = &m_pDoc->m_ColPairStat;
	}else{
		Save = &m_pDoc->m_RowPairSave;
		Best = &m_pDoc->m_RowPairBest;
		Stat = &m_pDoc->m_RowPairStat;
	}

	
	//identify number of groups & their idxs
	long numgroups,i;
	CArray<long> grps;
	long j,item;
	for(i=0;i<groups->GetSize();i++){
		if((item = groups->GetAt(i)) == -1) continue;
		//check if in groups already
		for(j=0;j<grps.GetSize();j++){
			if(item == grps[j]) break;
			if(item < grps[j]){
			//insert item
				grps.InsertAt(j,item);
				break;
			}
		}	
		if((j < grps.GetSize() && item != grps[j]) || 
			j >= grps.GetSize()){
			//add
			grps.Add(item);
		}
	}
	numgroups = (long)grps.GetSize();
	if(numgroups < 2){
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("fewer than 2 groups have been assigned.\nAnosim will not be performed.");
		}
		if(columns)		
			m_pDoc->m_log += "- Column ANOSIM failed.\r\n\r\n";
		else
			m_pDoc->m_log += "- Row ANOSIM failed.\r\n\r\n";
		return false;
	}
		
	//set result structure sizes
	for(i=0;i<numgroups-1;i++){
		Save->Add(new CAssocMatrixRow);
		Stat->Add(new CAssocMatrixRow);
		Best->Add(new CAssocMatrixRow);
		Save->GetAt(i)->SetSize(i+1);
		Stat->GetAt(i)->SetSize(i+1);
		Best->GetAt(i)->SetSize(i+1);
	}

	//same as above
	m_dlg = newDialog;
	m_iterations = iterations;
	m_seed = seed;

	CArray<long> order,group,new_group;
	double hits,m2;
	CString buffer;
	long nr,fnr;
	double totw,totb;
	double numw,numb;
	bool ranked = false;
	double rbest = 0.0;
	long n;

	CArray<CAssocMatrixRow*> *m_aMatrix;

	//randomize number generator
	//srand(m_seed);
	((CPatnApp*)AfxGetApp())->RandomSeed(m_seed);

	//patnold defaults
	long nrand = m_iterations;
	char symb;
	long ig,jg;
	double as,r,save;
	double stat;

	CArray<long>* grouplist;

	long grouplistlength = 0;
	if(columns){
		m_aMatrix = &m_pDoc->m_AssocMatrixCols.m_aMatrix;
		grouplist = &m_pDoc->m_aColumnGroups;
		
	}else{
		if(patn_generated_groups)
		{
			grouplist = &m_pDoc->m_AprioriGroups;
		}
		else
		{
			grouplist = &m_pDoc->m_aRowGroups;
		}
		m_aMatrix = &m_pDoc->m_AssocMatrixRows.m_aMatrix;		
	}
	if(grouplist->GetSize() > m_aMatrix->GetSize()+1) 
	{
		grouplistlength = m_aMatrix->GetSize()+1;
	}
	else
	{
		grouplistlength = grouplist->GetSize();
	}		

	n = m_aMatrix->GetSize()+1;

	if(columns){
		if(m_pDoc->m_AssocColChange || (m_aMatrix->GetSize() != -1 && m_aMatrix->GetSize() < n-1)){
			//show an error
			if(!autoeval){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Column Association Required for Column ANOSIM.");
			}
			m_pDoc->m_log += "- Column ANOSIM failed.\r\n\r\n";
			return FALSE;
		}
	}else{
		if(m_pDoc->m_AssocRowChange || (m_aMatrix->GetSize() != -1 && m_aMatrix->GetSize() < n-1)){
			//show an error
			if(!autoeval){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Row Association Required for Row ANOSIM.");
			}
			m_pDoc->m_log += "- Row ANOSIM failed.\r\n\r\n";
			return FALSE;
		}
	}

	if(m_aMatrix->GetAt(0)->GetAt(0) > 1 || m_aMatrix->GetAt(1)->GetAt(0) > 1 ||
		m_aMatrix->GetAt(n-2)->GetAt(n-3) > 1 || m_aMatrix->GetAt(n-2)->GetAt(n-2) > 1){
		ranked = true;
		symb = '-';
		//assume ranked
	}else{
		ranked = false;
		symb = '/';
		//assume not ranked
	}

	CArray<long> sourceList,idxs;

if(!((CPatnApp*)AfxGetApp())->silent_mode)	m_dlg->m_Progress.SetRange32(0,nrand*numgroups*(numgroups-1)/2);

	//beginning of mess (loop for each pair)
	long row,col;
	long g1,g2;
	for(row=1;row<numgroups;row++){
		for(col=0;col<row;col++){
			g1 = grps[row];
			g2 = grps[col];

			rbest = 0.0;

			//build list of current idxs
			idxs.RemoveAll();
			//for(i=0;i<groups->GetSize();i++){
			//	if(groups->GetAt(i) == g1 ||
			//		groups->GetAt(i) == g2)
			//		idxs.Add(i);
			//}
			CArray<long> group1items;

			for(i=0;i<grouplistlength;i++){
				if(g1 == grouplist->GetAt(i)){
					group1items.Add(i);					
					idxs.Add(i);
				}else if(g2 == grouplist->GetAt(i)){
					idxs.Add(i);
				}
			}
			
			//set number of elements
			n = idxs.GetSize();

			//setup group
			group.RemoveAll();
			for(i=0;i<n;i++){
				group.Add(i);
			}
			new_group.SetSize(n);
			order.RemoveAll();
			order.Add((long) floor(((CPatnApp*)AfxGetApp)->Rand()*(n-1)));
			while(order.GetSize() < n){
				i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*n);
				if(i < n && !IsElem(&order,i))
					order.Add(i);
			}

			//randomize loop
			hits = 0.0;
			m2 = (n*(n-1.0)/2.0)/2.0;
			for(nr=0;nr<nrand;nr++){
				//ini
				fnr = nr;
				totw = 0.0;
				totb = totw;
				numw = 0.0;
				numb = numw;
				//pairs
				for(i=0;i<n-1;i++){
					//get idxs in different order
					//ig = groups->GetAt(idxs[group[i]]);
					ig = grouplist->GetAt(idxs[group[i]]);
					//ig = grouplist->GetAt(idxs[i]);
					for(j=i+1;j<n;j++){
						//jg = groups->GetAt(idxs[group[j]]);
						jg = grouplist->GetAt(idxs[group[j]]);
						//jg = groups->GetAt(idxs[j]);
						as = m_aMatrix->GetAt(idxs[j]-1)->GetAt(idxs[i]);
						if(ig == jg){
							totw += as;
							numw++;
						}else{
							totb += as;
							numb++;
						}
					}
				}
				//B/W
				if(numw == 0 || totw == 0 || totb == 0 || numb == 0) 
					totw = totw;

				if(totw == 0){
					r = m_pDoc->GetMissingValue();

					if(ranked) r = totb/m2;

					if(nr == 0){
						save = r;
					}else{
						rbest = r;
						hits++;
					}
				}else{
					totw /= numw;
					if(numb != 0)
						totb /= numb;

					if(ranked)
						r = (totb-totw)/m2;
					else
						r = totb/totw;
				
					//store real/update hits
					if(nr == 0){
						save = r;
					}else{
						rbest = max(rbest,r);
						if(save <= r && save != m_pDoc->GetMissingValue())
						{
							hits++;
							/*FILE * fp = fopen("c:\\hit.txt","a");
							if(fp)
							{
								for(int y = 0;y<group1items.GetSize();y++)
								{
									fprintf(fp,"%d,",group1items[y]);
								}
								fprintf(fp,"\n");

								for(int y = 0;y<group.GetSize();y++)
								{
									fprintf(fp,"%d,",idxs[group[y]]);
								}
								fprintf(fp,"\n");

								for(y=0;y<group.GetSize();y++)
								{
									fprintf(fp,"%d,",group[y]);
								}
								fprintf(fp,"\n");

								for(y=0;y<group.GetSize();y++)
								{
									fprintf(fp,"%d,",grouplist->GetAt(idxs[group[y]]));
								}
								fprintf(fp,"\n");
								
								fclose(fp);
							}*/
						}
					}
				}

				stat = hits/(fnr)*100;

if(!((CPatnApp*)AfxGetApp())->silent_mode)				m_dlg->inc();
				
				//randomize
				if(nr != nrand){
					bool diff = true;
					long eternal_loop_check = 0;
					while(diff && eternal_loop_check < 200){
						eternal_loop_check++;
						//create another order set
						sourceList.RemoveAll();
						for(i=0;i<n;i++) sourceList.Add(i);

						order.RemoveAll();
						while(order.GetSize() < n){
							i = (long) floor(((CPatnApp*)AfxGetApp)->Rand()*(sourceList.GetSize()));
							if(i == sourceList.GetSize()) i = i - 1;
							order.Add(sourceList[i]);
							sourceList.RemoveAt(i);
						}
						//complete permitivity (reorder)
						for(i=0;i<order.GetSize();i++) 
							new_group[i] = group[order[i]];
						for(i=0;i<order.GetSize();i++)
							group[i] = new_group[i];

						//verify that it is different group allocations
						// -> check for same group 1 contents (of 2 groups)
						long foundcount = 0;
						
						long g = -2;
						long gcount = 0;
						long g1count = 0;
						long g2count = 0;
						if(group.GetSize() > 2)
						{
							for(i=0;i<group.GetSize() && diff;i++){
								long j;

								//is the idx[group[]] member in group1items
								for(j=0;j<group1items.GetSize();j++)
								{
									if(group1items[j] == idxs[group[i]])
									{
										//j = group1items.GetSize()+1;
										if(g1 == grouplist->GetAt(idxs[group[i]]) &&
											g1 == grouplist->GetAt(idxs[i]))
										{
											//g = g1;
											gcount++;
										}
										break;
									}
								}

								//if in group 1, add to count & record the group
								//if(j < group1items.GetSize())
								//{
									//setup g
								//	if(g == -2)
								//	{
								//		g = grouplist->GetAt(group1items[j]);
								//	}
									//if this group1 item is in the group, add & continue
								//	if(g == grouplist->GetAt(idxs.GetAt(group[i])))
								//	{
								//		gcount += 1;
								//	}
								//	else
								//	{
										//failed, can skip the rest of the test
								//		diff = false;
								//	}
								//}
								//else if(g != -2 && g == grouplist->GetAt(idxs.GetAt(group[i])))
								//{
								//	diff = false;
								//}
								//if(g2 == grouplist->GetAt(idxs.GetAt(group[i])))
								//{
								//	g2count += 1;
								//}
								//if(g1 == grouplist->GetAt(idxs.GetAt(group[i])))
								//{
								//	g1count += 1;
								//}
							}
							//now, only valid if 
							if((gcount == group1items.GetSize()) || (gcount == 0 && group1items.GetSize()*2 == group.GetSize()))
							{
							}
							else
							{
								diff = false;
							}
						}
						else
						{
							diff = false;
						}
						
					}
				}
			}

			//done for this comparison, add data to result structures
			Best->GetAt(row-1)->SetAt(col,rbest);
			Save->GetAt(row-1)->SetAt(col,save);
			Stat->GetAt(row-1)->SetAt(col,stat);
		}
	}

	CTime time = CTime::GetCurrentTime();
	if(columns){
		m_pDoc->m_ColumnAsim_time = time.Format("%H:%M:%S, %B %d, %Y");
		m_pDoc->m_ColumnAsim_asso = m_pDoc->m_AssocMatrixCols.m_strMeasure;
		m_pDoc->m_ColumnAsim_iterations = m_iterations;
		m_pDoc->m_ColumnAsim_seed = m_seed;
	}else{
		m_pDoc->m_RowAsim_time = time.Format("%H:%M:%S, %B %d, %Y");
		//m_pDoc->m_RowAsim_asso = m_pDoc->m_AssocMatrixRows.m_strMeasure;
		m_pDoc->m_RowAsim_asso = m_pDoc->m_AssocMatrixRows.m_strMeasure + CString(" on ") + CString((!m_pDoc->m_RowAsim_patn_groups)?"PATN generated groups":"a-priori groups");
		m_pDoc->m_RowAsim_iterations = m_iterations;
		m_pDoc->m_RowAsim_seed = m_seed;
	}
	return TRUE;
}

void CAnosim::Save(bool columns)
{
	//choose type (stat,best,save)
	CString type;

	CChooseAnosim obj;
	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
		if(obj.DoModal() == IDCANCEL)
			return;
		if(obj.m_Type == 0)
			type = "Best";
		else if(obj.m_Type == 1)
			type = "Stat";
		else
			type = "Save";
	}
	else
	{
		type = "Best";
	}
forsilentmode:
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CArray<CAssocMatrixRow*>* pMatrix;
	CAssocMatrixRow* pAMR;

	long i,j;
	CString tmp;

	if(columns){
		if(type == "Stat"){
			strTitle = "Save Column Anosim with Group Pairs (% Best > Real)";
			pMatrix = &m_pDoc->m_ColPairStat;
		}else if(type == "Save"){
			strTitle = "Save Column Anosim with Group Pairs (Real f-ratio)";
			pMatrix = &m_pDoc->m_ColPairSave;
		}else{
			strTitle = "Save Column Anosim with Group Pairs (Best f-ratio)";
			pMatrix = &m_pDoc->m_ColPairBest;
		}
	}else{
		if(type == "Stat"){
			strTitle = "Save Row Anosim with Group Pairs (% Best > Real)";
			pMatrix = &m_pDoc->m_RowPairStat;
		}else if(type == "Save"){
			strTitle = "Save Row Anosim with Group Pairs (Real f-ratio)";
			pMatrix = &m_pDoc->m_RowPairSave;
		}else{
			strTitle = "Save Row Anosim with Group Pairs (Best f-ratio)";
			pMatrix = &m_pDoc->m_RowPairBest;
		}
	}

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
	delete pFileDlg;
}
	else
	{
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + "ANOSIM " + type + " Recipe.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
		

		return;
	}

	long grp;
	CArray<long> * grpidxs;
	CArray<long> GrpIdxs;

	//set default view as "stat", & load up group indexes
	if(columns){
		grpidxs = &m_pDoc->m_AnosimColGroups;
	}else{
		grpidxs = &m_pDoc->m_AnosimRowGroups;
	}
	for(i=0;i<grpidxs->GetSize();i++){
		if((grp = grpidxs->GetAt(i)) <= 0) continue;
		//check if already in m_GrpIdxs
		for(j=0;j<GrpIdxs.GetSize();j++){
			if(grp == GrpIdxs[j]){
				j = -1;
				break;
			}else if(grp < GrpIdxs[j]){
				GrpIdxs.InsertAt(j,grp);
				j = -1;
				break;
			}					
		}
		//check if still not present & add
		if(j != -1){
			GrpIdxs.Add(grp);
		}			
	}

	//write header
	tmp.Format("Project Comment: %s\n",m_pDoc->m_TableName);tmp.Replace(","," ");
	f.WriteString(tmp);
	tmp = strTitle.Right(strTitle.GetLength() - 5);tmp.Replace(","," ");
	f.WriteString(tmp);
	f.WriteString("\n\n\n");
	
	// File created OK, write to it

	//write horizontal labels
	f.WriteString(",");
	for(i=0;i<pMatrix->GetSize();i++){
		tmp.Format("Group %d",GrpIdxs[i]);
		if(i < pMatrix->GetSize())
			tmp += ",";
		f.WriteString(tmp);
	}
	f.WriteString("\n");

	for ( i = 0; i < pMatrix->GetSize(); i++ )
	{
		pAMR = pMatrix->GetAt(i);
		tmp.Format("Group %d",GrpIdxs[i+1]);

		tmp += ",";
		f.WriteString(tmp);
		for ( j = 0; j < pAMR->GetSize(); j++ )
		{
			tmp.Format("%8.4f",pAMR->GetAt(j));
			if(j+1 < pAMR->GetSize())
				tmp += ",";
			f.WriteString(tmp);
		}
		f.WriteString("\n");
	}

	

	// Close the file
	f.Close();

	if(((CPatnApp*)AfxGetApp())->silent_mode)
	{
		if(type == "Best")
		{
			type = "Stat";
		}
		else if(type == "Stat")
		{
			type = "Save";
		}
		else
		{
			return;
		}
		goto forsilentmode;
	}
	
}

void CAnosim::SaveVector(bool columns)
{
	//choose type (stat,best,save)
	CString type;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CArray<long> * aMatrix;

	long i;
	CString tmp;

	if(columns){
		strTitle = "Save Column Anosim with Group Pairs group composition";
		aMatrix = &m_pDoc->m_AnosimColGroups;
		pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.caf|All Files (*.*)|*.*||",NULL);
	}else{
		strTitle = "Save Row Anosim with Group Pairs group composition";
		aMatrix = &m_pDoc->m_AnosimRowGroups;
		pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	}
	

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
	// Get path for the new file
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();

	delete pFileDlg;
	}
	else
	{
		if(!columns) strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation Row ANOSIM Results.csv";
		else strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation Column ANOSIM Results.csv";
	}
	
	// Create the file
	if ( !f.Open(strPathName,CFile::modeCreate | CFile::modeWrite | CFile::typeText,&fe) )
	{
		strFileError = strPathName;
		strFileError += '\n';
		strFileError = "File Error";
		switch (fe.m_cause)
		{
			case CFileException::diskFull:
				strFileError += " - Disk full";
		}

		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strFileError,MB_OK | MB_ICONSTOP);
	
		return;
	}
	
	for(i=0;i<aMatrix->GetSize();i++){
		tmp.Format("%d",aMatrix->GetAt(i));
		f.WriteString(tmp);
		if(columns && i<aMatrix->GetSize()-1)
			f.WriteString(",");
		else if(i<aMatrix->GetSize()-1)
			f.WriteString("\n");
	}



	// Close the file
	f.Close();
}