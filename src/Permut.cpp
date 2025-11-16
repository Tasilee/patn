// Permut.cpp: implementation of the CPermut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "patn.h"
#include "Permut.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPermut::CPermut()
{
	m_first = m_last = NULL;
	m_Factors.Add(1);
	for(long i=2;i<21;i++)
		m_Factors.Add(m_Factors[i-2]*i);
}

CPermut::~CPermut()
{
	m_first = m_first->next;
	while(m_first != NULL){
		delete m_first->prev;
		m_first = m_first->next;
	}
	delete m_last;
}

void CPermut::initPermut(long size)
{
	if(m_first != NULL){
		m_first = m_first->next;
		while(m_first != NULL){
			delete m_first->prev;
			m_first = m_first->next;
		}
	}

	for(long i=0;i<size;i++){
		if(i == 0){
			m_first = m_last = new Tree;
			m_last->prev = NULL;
		}else{
			m_last->next = new Tree;
			m_last->next->prev = m_last;
			m_last = m_last->next;
		}
		m_last->pos = 1;
		m_last->size = size - i;
		m_last->value = i+1;
		m_last->next = NULL;  
	}
}

void CPermut::nextPermut(CArray<double>*data,bool resetTree)
{
	if(resetTree) initPermut(data->GetSize());

	//search from end of tree for pos<size (indicates next branch)
	Tree * branch = m_last;
	CArray<long> collection; //holds ordered values passed over
	CArray<double> sink; //holds data values as per collection

	long pos = data->GetSize();
	while(branch != NULL && branch->pos < branch->size){
		//add branch->value to collection
		collection.Add(branch->value);
		sink.Add(data->GetAt(--pos));
		branch = branch->prev;
	}
	if(branch == NULL){
		SrtShl(expandTree(&collection),data);
		initPermut(m_first->size);
	}else{
		long value = branch->value;
		collection.Add(value);
		sink.Add(data->GetAt(--pos));
		//now sort collection (maintain sink)
		SrtShl(&collection,&sink);

		//find value in collection & use next value as branch->value
		for(long i=0;i<collection.GetSize();i++){
			if(collection[i] == value){
				//found, so now fill in the rest of the tree
				while(branch != NULL){
					if(collection[i] == value)
						branch->pos++;  //inc pos of current branch
					else 
						branch->pos = 1;  //first
					if(i == collection.GetSize()-1)
						i = 0;
					else
						i++;
					branch->value = collection[i];
					data->SetAt(pos++,sink[i]);
					branch = branch->next;
				}
			}
		}
	}
}

void CPermut::prevPermut(CArray<double>*data,bool resetTree)
{
	if(resetTree) initPermut(data->GetSize());

	//search from end of tree for pos<size (indicates next branch)
	Tree * branch = m_last;
	CArray<long> collection; //holds ordered values passed over
	CArray<double> sink; //holds data values as per collection

	long i;
	double tmpvalue;

	long pos = data->GetSize();
	while(branch != NULL && branch->pos > 1){
		//add branch->value to collection
		collection.Add(branch->value);
		sink.Add(data->GetAt(--pos));
		branch = branch->prev;
	}
	if(branch == NULL){
		SrtShl(expandTree(&collection),data);
		//reverse order of data
		for(i=0;i<ceil(data->GetSize()/2.0);i++){
			tmpvalue = data->GetAt(i);
			data->SetAt(i,data->GetAt(data->GetSize()-1-i));
			data->SetAt(data->GetSize()-1-i,tmpvalue);
		}

		//reverse tree
		branch = m_first;
		while(branch != NULL){
			branch->value = branch->size;
			branch->pos = branch->size;
			branch = branch->next;
		}
	}else{
		long value = branch->value;
		collection.Add(value);
		sink.Add(data->GetAt(--pos));
		//now sort collection (maintain sink)
		SrtShl(&collection,&sink);

		//find value in collection & use next value as branch->value
		for(long i=0;i<collection.GetSize();i++){
			if(collection[i] == value){
				//found, so now fill in the rest of the tree
				while(branch != NULL){
					if(collection[i] == value)
						branch->pos--;  //dec pos of current branch
					else 
						branch->pos = branch->size;  //last
					//get previous pos for prevPermut on branch
					if(i == 0)
						i = collection.GetSize()-1;
					else
						i--;
					branch->value = collection[i];
					data->SetAt(pos++,sink[i]);
					branch = branch->next;
				}
			}
		}
	}
}

//steps may be positive (nextPermutations) or negative
// (prevPermutations).
//__int64 used so that upto 20 values (20!) can be varied 
// in a single step
void CPermut::Permut(CArray<double>*data,__int64 steps,bool resetTree)
{
	CArray<long> collection;
	CArray<double> sink;
	CArray<long> numFactors;

	__int64 savesteps = steps;
	long i,level,pos = data->GetSize();
	long value,lastpos;
	Tree * branch = m_last;
	bool skip = false;

	if(steps > 0){ //nextPermut
		//determine factor composition
		for(i=m_Factors.GetSize()-1;i>=0;i--){
			numFactors.InsertAt(0,(long) floor(steps/((double)m_Factors[i])));
			steps %= m_Factors[i];
		}

		//check if a revolution occurs (back to start)
		steps = 0;
		if(branch != NULL) branch = branch->prev;
		for(i=0;branch != NULL && i<20;i++){
			if(i == 19){
				if(branch->size-branch->pos > 7) skip = true;
			}else{
				steps += m_Factors[i] * (branch->size - branch->pos);
			}
			branch = branch->prev;
		}
		if(!skip && savesteps > steps){
			SrtShl(expandTree(&collection),data);
			initPermut(data->GetSize());
			Permut(data,savesteps - steps,false);
			return;
		}
		branch = m_last;

		level = 0;
		//determine level & update pos
		branch = branch->prev;
		for(i=0;branch != NULL && i<m_Factors.GetSize();i++){
			//update pos & level
			if(numFactors[i]){
				if(level < i){
					level = i;
					lastpos = branch->pos;
					value = branch->value;
				}
				if((branch->pos += numFactors[i]) > branch->size){
					lastpos = branch->prev->pos++;
					value = branch->prev->value;
					level++;
					branch->pos -= branch->size;
					if(branch->pos > branch->size){ //dblcheck
						branch->pos -= branch->size;
					}
				}
			}
			branch = branch->prev;
		}
		//check branches > level for invalid pos
		while(branch != NULL && branch->pos > branch->size){
			level++;
			branch->pos -= branch->size;
			lastpos = branch->prev->pos++;
			value = branch->prev->value;
			branch = branch->prev;
		}
		level+=2;
		
		//store collection (maintain sink)
		branch = m_last; 
		for(i=0;branch != NULL && i<level;i++){
			collection.Add(branch->value);
			sink.Add(data->GetAt(--pos));
			branch = branch->prev;
		}	
		if(branch != NULL) branch = branch->next;
		else branch = m_first;
		
		//sort collection
		SrtShl(&collection,&sink);

		//update values (pos already done).
		// find value in collection & use next value as branch->value
		for(i=0;i<collection.GetSize();i++){
			if(collection[i] == value){
				i += branch->pos - lastpos;
				//found, so now fill in the rest of the tree
				while(branch != NULL){
					while(i != 0 && i >= collection.GetSize())
						i -= collection.GetSize();
					branch->value = collection[i];
					data->SetAt(pos++,sink[i]);
					collection.RemoveAt(i);
					sink.RemoveAt(i);
					branch = branch->next;
					i++;
				}
				continue;
			}
		}		
	}else if(steps < 0){ //prevPermut
		savesteps = steps = steps * -1;

		//determine factor composition
		for(i=m_Factors.GetSize()-1;i>=0;i--){
			numFactors.InsertAt(0,(long) floor(steps/((double)m_Factors[i])));
			steps %= m_Factors[i];
		}

		//check if a revolution occurs (back to start)
		steps = 0;
		if(branch != NULL) branch = branch->prev;
		for(i=0;branch != NULL && i<20;i++){
			if(i == 19){
				if(branch->pos-1 > 7) skip = true;
			}else{
				steps += m_Factors[i] * (branch->pos - 1);
			}
			branch = branch->prev;
		}
		if(!skip && savesteps > steps){
			SrtShl(expandTree(&collection),data);
			Permut(data,steps - savesteps,false);
			return;
		}
		branch = m_last;

		//determine level & update pos
		level = 0;
		for(i=0;branch != NULL && i<m_Factors.GetSize();i++){
			//update pos & level
			if(level < i){
				level = i;
				lastpos = branch->pos;
				value = branch->value;
			}
			if((branch->pos -= numFactors[i]) < 1){
				lastpos = branch->prev->pos--;
				value = branch->prev->value;
				level++;
				branch->pos += branch->size;
				if(branch->pos < 1){ //dblcheck
					branch->pos += branch->size;
				}
			}
			branch = branch->prev;
		}
		//check branches > level for invalid pos
		while(branch != NULL && branch->pos < 1){
			level++;
			branch->pos += branch->size;
			lastpos = branch->prev->pos--;
			value = branch->prev->value;
			branch = branch->prev;
		}
		
		//store collection (maintain sink)
		branch = m_last; 
		for(i=0;branch != NULL && i<level;i++){
			collection.Add(branch->value);
			sink.Add(data->GetAt(--pos));
			branch = branch->prev;
		}	
		if(branch != NULL) branch = branch->next;
		
		//sort collection
		SrtShl(&collection,&sink);

		//update values (pos already done).
		// find value in collection & use next value as branch->value
		for(i=0;i<collection.GetSize();i++){
			if(collection[i] == value){
				//found, so now fill in the rest of the tree
				while(branch != NULL){
					if(collection[i] == value)
						i -= (branch->pos - lastpos);
					else 
						i--;
					if(i == 0)
						i = branch->size;
					branch->value = collection[i];
					data->SetAt(pos++,sink[i]);
					collection.RemoveAt(i);
					sink.RemoveAt(i);
					branch = branch->next;
				}
			}
		}

	}

}

void CPermut::SrtShl(CArray<long> *data,CArray<double> *values)
{
	long i,j,fm,ii;

	long fn = data->GetSize();
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	long tempd;
	double tempi;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,fn-m);i++){
			if(data->GetAt(i-1) > data->GetAt(i+m-1)){
				tempd = data->GetAt(i+m-1);
				tempi = values->GetAt(i+m-1);
				data->SetAt(i+m-1,data->GetAt(i-1));
				values->SetAt(i+m-1,values->GetAt(i-1));
				if(i <= m){
					data->SetAt(i-1,tempd);
					values->SetAt(i-1,tempi);
				}else{
					for(j=i-m;j>=1;j-=m){
						if(tempd >= data->GetAt(j-1)) break;
						data->SetAt(j+m-1,data->GetAt(j-1));
						values->SetAt(j+m-1,values->GetAt(j-1));
					}
					data->SetAt(j+m-1,tempd);
					values->SetAt(j+m-1,tempi);
				}
			}
		}
		m = fm/2;
	}

	/*
	long i;
	long tempi;
	double tempd;
	long n = data->GetSize();

	//bubble sort, i think
	long count = 1;
	long max = n-1;
	while(count > 0){
		count = 0;
		for(i=0;i<max;i++){
			if(data->GetAt(i) > data->GetAt(i+1)){
				//swap them
				tempi = data->GetAt(i);
				tempd = values->GetAt(i);
				data->SetAt(i,data->GetAt(i+1));
				values->SetAt(i,values->GetAt(i+1));
				data->SetAt(i+1,tempi);
				values->SetAt(i+1,tempd);
				count++;
			}
		}
		max--;
	}
	*/
}

CArray<long>* CPermut::expandTree(CArray<long>*data)
{
	Tree * branch = m_first;
	data->SetSize(branch->size);
	for(long i=0;i<branch->size;i++){
		data->SetAt(i,branch->value);
		branch = branch->next;
	}

	return data;
}