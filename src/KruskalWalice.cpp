// KruskalWalice.cpp: implementation of the CKruskalWalice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "KruskalWalice.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKruskalWallace::CKruskalWallace()
{

}

CKruskalWallace::~CKruskalWallace()
{

}

bool CKruskalWallace::Create(CnewDialog * dlg,CPatnDoc *pDoc,bool autoeval)
{
	long i;
	Rows colOrder;
	
	m_pDoc = pDoc;
	m_dlg = dlg;

//Now handled in Evaluation module...
//	CKruskalWalliceOptions kwoptions(pDoc);
//
//	if(kwoptions.DoModal() == IDCANCEL ||
//		!pDoc->m_KWAttributes.GetSize())
//		return false;

	//empty patndoc m_KW structure
	//pDoc->m_KWAttributes.RemoveAll();
	pDoc->m_KW.RemoveAll();
	pDoc->m_KWVisible.RemoveAll();
	pDoc->m_KWValues.RemoveAll();

	//function for KW goes here 
	if(!CalcKW(&colOrder,autoeval))
		return false;

	//write back order
	for(i=0;i<colOrder.GetSize();i++){
		pDoc->m_KW.Add(colOrder.GetAt(i));
		pDoc->m_KWVisible.Add(0);
	}

	CTime time = CTime::GetCurrentTime();
	pDoc->m_kwtime = time.Format("%H:%M:%S, %B %d, %Y");

		
	return true;
}

bool CKruskalWallace::CalcKW(Rows* order,bool autoeval)
{
	CArray<double> valList;

	long nCols;

	if((nCols = m_pDoc->GetIdxLeftExtrinsicColumn()) == -1)
		nCols = m_pDoc->GetNoOfColumns();

	long nRows;

	if((nRows = m_pDoc->GetIdxTopExtrinsicRow()) == -1)
		nRows = m_pDoc->GetNoOfRows();
		
	double valmis = m_pDoc->GetMissingValue();

//	long i,j,ip,nn,rn,ik,mk;
//	double kw1,kw2,kw3,kruwal,sumrnk,tie,tiesum,d,val,grnk,fnigt;

	CArray<double> rnk;
	CArray<double> datsrt;
	CArray<long> isort;
	CArray<double> data;
	CArray<long double> group;
	CArray<double> rank;

	int i;

	//if no columns selected, assume that they all should be selected (or only extrinsics).
	if(m_pDoc->m_KWAttributes.GetSize() == 0){
		m_pDoc->m_KWAttributes.RemoveAll();
		for(i=m_pDoc->GetIdxLeftExtrinsicColumn();i!=-1 && i<m_pDoc->GetNoOfColumns();i++){
			m_pDoc->m_KWAttributes.Add(i);
		}				
	}
	if(m_pDoc->m_KWAttributes.GetSize() == 0){
		m_pDoc->m_KWAttributes.RemoveAll();
		for(i=0;i<nCols;i++){
			m_pDoc->m_KWAttributes.Add(i);
		}
	}

	if(!((CPatnApp*)AfxGetApp())->silent_mode && m_dlg != NULL) m_dlg->m_Progress.SetRange32(0,m_pDoc->m_KWAttributes.GetSize());

	long col;

	for(i=0;i<m_pDoc->m_KWAttributes.GetSize();i++){
		col = m_pDoc->m_KWAttributes[i];

		data.RemoveAll();
		rank.RemoveAll();
		group.RemoveAll();
		isort.RemoveAll();

		CArray<long> idx;
		idx.SetSize(nRows);
		data.SetSize(nRows);
		isort.SetSize(nRows);
		long d = 0;
		for(int k=0;k<nRows;k++)
		{
			double ld = m_pDoc->GetValueAt(col,k);
			if(ld != valmis)
			{
				idx[d] = d;			
				data[d] = ld;
				if(m_pDoc->m_kw_patn_generated_groups) 
				{
					if(m_pDoc->m_AprioriGroups.GetSize() <= k)
						isort[d] = -1;
					else
						isort[d] = m_pDoc->m_AprioriGroups.GetAt(k);
				}
				else
					isort[d] = m_pDoc->GetRowGroups(k);//row groups are 1->n
				d++;
			}
		}
		if(d <= 0)
		{
			valList.Add(m_pDoc->GetMissingValue());
			continue;
		}
		idx.SetSize(d);
		data.SetSize(d);
		isort.SetSize(d);
		SrtShl(&data,&idx,d);
	
		//VERIFY
		for(long k=0;k<d-1;k++)
		{
			if(data[k] > data[k+1])
				return false;
		}

		//RANK values
		rank.SetSize(d);
		for(long k=0;k<d;k++)
		{
			//test for tie
			long startpos = k;
			long endpos = k;
			long ranksum = k+1;
            for(int p=k+1;p<d && data[p]==data[k];p++)
			{
				ranksum += p+1;
				endpos = p;
				k = k+1;
			}
			for(p=startpos;p<=endpos;p++)
			{
                rank[idx[p]] = ranksum / (double)(endpos-startpos+1);
			}
		}

		//REFLECT isort group values correctly
		bool continuing = true;
		long currentg = -1;
		long ming;
		CArray<long> groupcount;
		while(continuing)
		{
			ming = 99999999; //obviously this is only an assumption.

			//get next lowest group number
			for(k=0;k<d;k++)
			{
				if(isort[k] < ming && isort[k] > currentg)
				{
					ming = isort[k];
				}
			}
			if(ming == 99999999)
			{
				continuing = false;
				continue;
			}

			//set all of these values to currentg+1;
			currentg++;
			long sumg = 0;
			for(k=0;k<d;k++)
			{
				if(isort[k] == ming)
				{
					isort[k] = currentg;
					sumg++;
				}
			}
			groupcount.Add(sumg);
		}

		//calculate group totals
		//loop for each group
		group.SetSize(groupcount.GetSize());
		for(k=0;k<group.GetSize();k++)
		{
			group[k] = 0;
		}		

		for(k=0;k<d && isort[k]<group.GetSize() && isort[k]<groupcount.GetSize();k++)
		{
			//also check for overflow...
			long double check = group[isort[k]];
			group[isort[k]] += rank[k];
			if(check > group[isort[k]])
			{
				//error		
				m_pDoc->m_log += "- Box & Whisker failed\r\n    too many objects - number overflow on group_sum.  Default value of 1.7E308 used.\r\n";
				group[isort[k]] = 1.7E308;
				k = d;
				//return false;
			}			
		}
		
		

		//evaluate
		long double totalsum = 0;
		for(k=0;k<group.GetSize();k++)
		{
			long double check = totalsum;
			totalsum += (group[k] / (long double) groupcount[k]) * (group[k] / (long double)((long double)d*((long double)d+1)));
			if(check > totalsum)
			{
				//error		
				m_pDoc->m_log += "- Box & Whisker failed\r\n    too many objects - number overflow on total_sum.  Default value of 1.7E308 used.\r\n";
				totalsum = 1.7e308;
				k = group.GetSize();
				//return false;
			}			
		}

		//finish up
		double H = (12.0 * totalsum) - 3 * (d + 1);		
		
		valList.Add(H);
		if(!((CPatnApp*)AfxGetApp())->silent_mode && m_dlg != NULL)m_dlg->inc();
	}

	
		/*
		//reset datsrt,isort & rnk
		rnk.RemoveAll();
		datsrt.RemoveAll();
		isort.RemoveAll();

		nn = 0;
		for(k=0;k<nRows;k++){
			d = m_pDoc->GetValueAt(col,k);
			if(d != valmis){
				nn++;
				datsrt.Add(d);
				isort.Add(k);
			}
		}

		rn = nn;
		if(rn == 0){//not supposed to occur! All missing values.
			valList.Add(valmis);
			m_dlg->inc();
			continue;
		}

		kw1 = 12.0/((double) rn*(rn+1));
		kw2 = 3.0*(rn+1);

		Rank(&datsrt,&isort,nn,1);

/*		kw3 = rn*rn*rn - rn;

		tie = 0;
		Rank(&datsrt,&isort,nn,1);
		tie = 1;
		tiesum = 0;

		rnk.SetSize(nRows);

		val = datsrt.GetAt(0);
		ik = isort.GetAt(k);
		rnd.SetAt(0,val);
		for(k=1;k<nn;k++){
			ik = isort.GetAt(k);
			rnk.SetAt(ik,datsrt.GetAt(k));
			if(datsrt.GetAt(k) == val){
				tie++;
			}else{
				if(tie > 1) tiesum += tie*tie*tie-tie;
				val = datsrt.GetAt(k);
				tie = 1;
			}			
		}
		if(tie > 1) tiesum += tie*tie*tie - tie;
		
		//group loop
		sumrnk = 0;
		double sumg, sumsqg;
		for(j=0;j<m_pDoc->m_Rows.GetSize();j++){
			sumg = 0;
			sumsqg = 0;
			ip = 0;
			grnk = 0;			
			for(k=0;k<m_pDoc->m_Rows.GetAt(j)->GetSize();k++){
				mk = m_pDoc->m_Rows.GetAt(j)->GetAt(k);
				d = m_pDoc->GetValueAt(col,mk);
				if(d != valmis){					
					//d> 0->1's? NO!
					datsrt.SetAt(ip,d);
					sumsqg += d*d;
					ip++;
					sumg += d;

					grnk += rnk.GetAt(mk);

				}
			}
			if(ip > 0){
				fnigt = (double) ip;
				sumrnk += grnk*grnk/fnigt;
			}
		}

		kruwal = (kw1 * sumrnk - kw2) / (1.0 - tiesum/kw3);
		if(kw3 == 0 || (tiesum == kw3)) //check it anyway... same as from above...
			kruwal = valmis;

		valList.Add(kruwal);

		m_dlg->inc();
	}
*/ 

	//all values added to 'valList', sort for 'order'
	//init 'order'
	for(i=0;i<valList.GetSize();i++){
		order->Add(i);
	}

	if(order->GetSize()){
		//sort it then...
		SrtShl(&valList,order,m_pDoc->m_KWAttributes.GetSize());

		//write back values
		for(i=0;i<valList.GetSize();i++){
			m_pDoc->m_KWValues.Add(valList[i]);
		}
	}else{
		if(!autoeval){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("More than one Attribute must be selected\nfor Kruskal-Wallis Statistics.");
		}
		m_pDoc->m_log += "Kruskal-Wallis Statistics failed.\r\n\r\n";
	}


	return true;
}


void CKruskalWallace::Rank(CArray<double> *values,CArray<long>*order,long nn,long iop)
{
	long i,j;
	//sort data
	SrtShl(values, order,nn);

	double ties = 1;
	double rnk = 0;
	double sumrnk = 1;
	long istart = 0;
	bool run = false;
	double last = values->GetAt(0);

	//loop on data
	for(i=1;i<nn;i++){
		//tie
		if(values->GetAt(i) == last){
			if(!run){
				run = true;
				istart = i-1;
			}
			ties++;
			sumrnk += (double) (i+1);
		}

		//no-tie or last
		if(values->GetAt(i) != last || i == (nn-1)){

			if(iop == 1) rnk = sumrnk/ties;
			else rnk++;
			//need to check for ties == 0, should never occur...
			if(ties == 0 && iop == 1)
				rnk = sumrnk;//do something... this will cause the analysis to be inaccurate

			if(i != nn-1) last = values->GetAt(i);

			//run: fill to i-1 or iend;
			if(run){
				for(j=istart;j<i;j++){
					values->SetAt(j,rnk);
				}
			}else{ 
				//no run: rank i-1
				values->SetAt(i-1,rnk);
			}

			//last: rank i
			if(i == nn-1){
				if(values->GetAt(i) != last){
					if(iop == 1){
						values->SetAt(nn-1,(double) (i+1));
					}else{
						values->SetAt(nn-1,rnk + 1);
					}
				}else{
					values->SetAt(nn-1,rnk);
				}		
			}	
			//re-initialize
			run = false;
			ties = 1;
			istart = i;
			sumrnk = (double) (i+1);
		}
	}
}

void CKruskalWallace::SrtShl(
				  CArray<double>*  data,
				  CArray<long>* indx,
				  long n)
{
	long i,j,fm,ii;
	//setup index
	if(n == 0){
		indx->SetAt(0,1);
		return;
	}else if(indx->GetAt(0) == -1){
		for(i=0;i<n;i++){
			indx->SetAt(i,i);
		}
	}

	long fn = n;
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;
	long tempi;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,n-m);i++){
			if(data->GetAt(i-1) > data->GetAt(i+m-1)){
				tempd = data->GetAt(i+m-1);
				tempi = indx->GetAt(i+m-1);
				data->SetAt(i+m-1,data->GetAt(i-1));
				indx->SetAt(i+m-1,indx->GetAt(i-1));
				if(i <= m){
					data->SetAt(i-1,tempd);
					indx->SetAt(i-1,tempi);
				}else{
					for(j=i-m;j>=1;j-=m){
						if(tempd >= data->GetAt(j-1)) break;
						data->SetAt(j+m-1,data->GetAt(j-1));
						indx->SetAt(j+m-1,indx->GetAt(j-1));
					}
					data->SetAt(j+m-1,tempd);
					indx->SetAt(j+m-1,tempi);
				}
			}
		}
		m = fm/2;
	}

	//take out missing values (put at start?)
	long moved = 0;
	for(i=data->GetSize()-2;i>=moved;i--){
		if(data->GetAt(i) == m_pDoc->GetMissingValue()){
			data->InsertAt(0,data->GetAt(i));
			indx->InsertAt(0,indx->GetAt(i));
			data->RemoveAt(i+1);
			indx->RemoveAt(i+1);
			moved++;
		}
	}
}


//for inter-group kw
bool CKruskalWallace::Create2(CPatnDoc *pDoc,CArray<long>*order,CArray<double>*values,
							  long group1,long group2)
{
	m_pDoc = pDoc;
	
	if(!pDoc->m_KWAttributes.GetSize()) return false;

	//function for KW goes here 
	if(!CalcKW2(order,values,group1,group2))
		return false;

	return true;
}



bool CKruskalWallace::CalcKW2(CArray<long>* order,CArray<double>*values,
							  long group1,long group2)
{
	long nRows;

	CArray<double> rnk;
	CArray<double> datsrt;
	CArray<long> isort;
	CArray<double> data;
	CArray<long double> group;
	CArray<double> rank;

	if((nRows = m_pDoc->GetIdxTopExtrinsicRow()) == -1)
		nRows = m_pDoc->GetNoOfRows();
		
	double valmis = m_pDoc->GetMissingValue();

	values->RemoveAll();
	order->RemoveAll();

	//long i,j,k,ip,nn,rn,ik,mk;
	int i;
	//double kw1,kw2,kw3,kruwal,sumrnk,tie,tiesum,d,val,grnk,fnigt;

	//CArray<double> rnk;
	//CArray<double> datsrt;
	//CArray<long> isort;

	long col;
	for(i=0;i<m_pDoc->m_KWAttributes.GetSize() && i<m_pDoc->GetNoOfColumns();i++){
		col = m_pDoc->m_KWAttributes[i];

		//reset datsrt,isort & rnk
		data.RemoveAll();
		rank.RemoveAll();
		group.RemoveAll();
		isort.RemoveAll();

		CArray<long> idx;
		idx.SetSize(nRows);
		data.SetSize(nRows);
		isort.SetSize(nRows);
		long d = 0;
		for(int k=0;k<nRows;k++)
		{
			double ld = m_pDoc->GetValueAt(col,k);
			if(m_pDoc->m_kw_patn_generated_groups)
			{
				if(ld != valmis 
					&& 				
					(
					(group1 >= 0 && group1 < m_pDoc->m_RowControlGroups.GetSize() && isMember(m_pDoc->m_RowControlGroups[group1],k)) ||
					(group2 >= 0 && group2 < m_pDoc->m_RowControlGroups.GetSize() && isMember(m_pDoc->m_RowControlGroups[group2],k)) 
					))
				{
					idx[d] = d;			
					data[d] = ld;
					if(m_pDoc->m_AprioriGroups.GetSize() <= k)
						isort[d] = -1;
					else
						isort[d] = m_pDoc->m_AprioriGroups[k];					
					d++;
				}
			}
			else
			{
				if(ld != valmis 
					&& 				
					(
					(group1 >= 0 && group1 < m_pDoc->m_Rows.GetSize() && isMember(m_pDoc->m_Rows[group1],k)) ||
					(group2 >= 0 && group2 < m_pDoc->m_Rows.GetSize() && isMember(m_pDoc->m_Rows[group2],k)) 
					))
				{
					idx[d] = d;			
					data[d] = ld;
					isort[d] = m_pDoc->GetRowGroups(k);//row groups are 1->n
					d++;
				}
			}
		}
		if(d==0) return false;
		idx.SetSize(d);
		data.SetSize(d);
		isort.SetSize(d);
		SrtShl(&data,&idx,d);
	
		//VERIFY
		for(long k=0;k<d-1;k++)
		{
			if(data[k] > data[k+1])
				return false;
		}

		//RANK values
		rank.SetSize(d);
		for(long k=0;k<d;k++)
		{
			//test for tie
			long startpos = k;
			long endpos = k;
			long ranksum = k+1;
            for(int p=k+1;p<d && data[p]==data[k];p++)
			{
				ranksum += p+1;
				endpos = p;
				k = k+1;
			}
			for(p=startpos;p<=endpos;p++)
			{
                rank[idx[p]] = ranksum / (double)(endpos-startpos+1);
			}
		}

		//REFLECT isort group values correctly
		bool continuing = true;
		long currentg = -1;
		long ming;
		CArray<long> groupcount;
		while(continuing)
		{
			ming = 99999999; //obviously this is only an assumption.

			//get next lowest group number
			for(k=0;k<d;k++)
			{
				if(isort[k] < ming && isort[k] > currentg)
				{
					ming = isort[k];
				}
			}
			if(ming == 99999999)
			{
				continuing = false;
				continue;
			}

			//set all of these values to currentg+1;
			currentg++;
			long sumg = 0;
			for(k=0;k<d;k++)
			{
				if(isort[k] == ming)
				{
					isort[k] = currentg;
					sumg++;
				}
			}
			groupcount.Add(sumg);
		}

		//calculate group totals
		//loop for each group
		group.SetSize(groupcount.GetSize());
		for(k=0;k<group.GetSize();k++)
		{
			group[k] = 0;
		}
		for(k=0;k<d && isort[k]<group.GetSize() && isort[k]<groupcount.GetSize();k++)
		{
			//also check for overflow...
			long double check = group[isort[k]];
			group[isort[k]] += rank[k];
			if(check > group[isort[k]])
			{
				//error				
				return false;
			}			
		}
		//evaluate
		long double totalsum = 0;
		for(k=0;k<group.GetSize();k++)
		{
			totalsum += (group[k] / (long double) groupcount[k]) * (group[k] / (long double)(d*(d+1)));
		}

		//finish up
		double H = (12.0 * totalsum) - 3 * (d + 1);		
		
		values->Add(H);
	//	m_dlg->inc();
	}

/*		rn = nn;
		if(rn == 0){//should not occur... do something/anything! (or just handle it later)
			values->Add(valmis);
			continue;
		}

		kw1 = 12.0/((double) rn*(rn+1));
		kw2 = 3.0*(rn+1);
		kw3 = rn*rn*rn - rn;

		tie = 0;
		Rank(&datsrt,&isort,nn,1);
		tie = 1;
		tiesum = 0;

		rnk.SetSize(nRows);
		if(datsrt.GetSize())
			val = datsrt.GetAt(0);
		for(k=0;k<nn && k<isort.GetSize() && k < datsrt.GetSize();k++){
			ik = isort.GetAt(k);
			if(ik < rnk.GetSize() && ik >= 0)
				rnk.SetAt(ik,datsrt.GetAt(k));

			if(k>0){
				if(datsrt.GetAt(k) == val){
					tie ++;
				}else{
					if(tie > 1) tiesum += tie*tie*tie-tie;
					val = datsrt.GetAt(k);
					tie = 1;
				}
			}
		}
		if(tie > 1) 
			tiesum += tie*tie*tie - tie;
		
		//group loop, only 2 groups now
		sumrnk = 0;
		long grp;
		for(j=0;j<2;j++){
			if(j==0) grp = group1;
			else grp = group2;
			ip = 0;
			grnk = 0;
			for(k=0;grp >= 0 && grp < m_pDoc->m_Rows.GetSize() &&
				k<m_pDoc->m_Rows.GetAt(grp)->GetSize();k++){
				mk = m_pDoc->m_Rows.GetAt(grp)->GetAt(k);
				d = m_pDoc->GetValueAt(i,mk);
				if(d != valmis && mk >= 0 && mk < rnk.GetSize()){
					ip++;
					//d> 0->1's?
					grnk += rnk.GetAt(mk);
				}
			}
			if(ip > 0){
				fnigt = (double) ip;
				sumrnk += grnk*grnk/fnigt;
			}
		}

		kruwal = (kw1 * sumrnk - kw2) / (1.0 - tiesum/kw3);
		if(kw3 == 0 || tiesum == kw3) //check it anyway
			kruwal = valmis;

		values->Add(kruwal);
	}
*/
	//all values added to 'valList', sort for 'order'
	//init 'order'
	for(i=0;i<values->GetSize();i++){
		order->Add(i);
	}

	//sort it then...
	SrtShl(values,order,m_pDoc->m_KWAttributes.GetSize());

	return true;
}

bool CKruskalWallace::isMember(CArray<long>*list,long k)
{
	for(long i=0;i<list->GetSize();i++)
		if(list->GetAt(i) == k) return true;
	return false;
}