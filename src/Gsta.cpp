// Gsta.cpp: implementation of the CGsta class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "stdio.h"
#include "patn.h"
#include "patndoc.h"
#include "Gsta.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

double CGsta::Get_First_Quartile(CArray<double> *values)
{
	if(values->GetSize() == 0) return 0;
	return values->ElementAt(values->GetSize()/4);
}

double CGsta::Get_Maximum(CArray<double> *values)
{
	if(values->GetSize() == 0) return 0;
	return values->ElementAt(values->GetSize()-1);
}

double CGsta::Get_Mimimum(CArray<double> *values)
{
	if(values->GetSize() == 0) return 0;
	return values->ElementAt(0);
}

double CGsta::Get_Median(CArray<double> *values)
{
	if(values->GetSize() == 0) return 0;
	return values->ElementAt(values->GetSize()/2);
}

double CGsta::Get_Mean(CArray<double> *values)
{
	if(values->GetSize() == 0) return 0;
	double tally = 0.0;
	for(long i = 0;i<values->GetSize();i++){
		tally += values->ElementAt(i);
	}
	return (tally / values->GetSize());
}

double CGsta::Get_Third_Quartile(CArray<double> *values)
{
	return values->ElementAt(values->GetSize() / 4 * 3);
}

void SrtSh1(
			double * data,
			long n)
{
	long ii,fm,i;
//	bool skip;
	long j;
	long fn = n;
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,n-m);i++){
			if(data[i-1] > data[i+m-1]){
				tempd = data[i+m-1];
				data[i+m-1] = data[i-1];
				if(i <= m){
					data[i-1] = tempd;
				}else{
					//skip = false;
					for(j=i-m;/*!skip &&*/ j>=1;j-=m){
						if(tempd >= data[j-1]) //skip = true;
							break;
						//if(!skip){
							data[j+m-1] = data[j-1];
						//}
					}
					data[j+m-1] = tempd;
				}
			}
		}
		m = fm/2;
	}
}


CGsta::CGsta(CPatnDoc *pDoc)
{
	long count = 0,o = 0,p = 0;
	double value = 0;

	//CArray <double,double> values;
	
	Stat *a;
	dblStat *b;

	m_pDoc = pDoc;

	long nCols;

	nCols = m_pDoc->GetNoOfColumns();

	for(p=0;p<m_pDoc->m_Gstastats.GetSize();p++){
		for(long o=0;m_pDoc->m_Gstastats[p] != NULL &&
			o<m_pDoc->m_Gstastats[p]->GetSize();o++)
			delete m_pDoc->m_Gstastats[p]->GetAt(o);
		delete m_pDoc->m_Gstastats[p];
	}
	m_pDoc->m_Gstastats.SetSize(m_pDoc->m_Groups.GetSize());

	for(p=0;p<m_pDoc->m_Gstastats_apriori.GetSize();p++){
		for(long o=0;m_pDoc->m_Gstastats_apriori[p] != NULL &&
			o<m_pDoc->m_Gstastats_apriori[p]->GetSize();o++)
			delete m_pDoc->m_Gstastats_apriori[p]->GetAt(o);
		delete m_pDoc->m_Gstastats_apriori[p];
	}	
	m_pDoc->m_Gstastats_apriori.SetSize(m_pDoc->m_RowControlGroups.GetSize());

	long k,i;
	CArray<dblStat*> * gstastats;
	CArray<long> * groups;
	CArray<Rows*> * rowgroups;
	CArray<long> apgrps;
	for(k=0;k<2;k++)
	{
		if(k==0)
		{
			gstastats = &m_pDoc->m_Gstastats;
			groups = &m_pDoc->m_Groups;
			rowgroups = &m_pDoc->m_Rows;
		}
		else
		{
			gstastats = &m_pDoc->m_Gstastats_apriori;
			if(m_pDoc->m_RowControlGroups.GetSize() == 0)
				return;
			apgrps.SetSize(m_pDoc->m_RowControlGroups.GetSize()-1);
			for(i=0;i<apgrps.GetSize();i++)
			{
				apgrps[i]=i-1;
			}
			groups = &apgrps;
			rowgroups = & m_pDoc->m_RowControlGroups;
		}
		if(groups->GetSize() > 1)
		{
			double * values = new double[m_pDoc->GetNoOfRows()];
			long size = 0;

			for(long group=0;group<groups->GetSize();group++){
				b = new dblStat;
				b->SetSize(nCols);
				for(long attrib=0;attrib<nCols;attrib++){
					//values.RemoveAll();
					size = 0;
					double mean = 0;
					for(count=0;group >= 0 && group < rowgroups->GetSize() &&
						count<rowgroups->GetAt(group)->GetSize();count++){
						if(pDoc->GetNoOfRows() > rowgroups->GetAt(group)->GetAt(count) &&
							pDoc->GetNoOfColumns() > attrib &&
							(value = pDoc->GetValueAt(attrib,rowgroups->GetAt(group)->GetAt(count))) != pDoc->GetMissingValue()){
							
							values[size] = value;
							size++;
							mean += value;
						}				
					}
					SrtSh1(values,size);
					if(size == 0){
						b->SetAt(attrib,new Stat);
						b->GetAt(attrib)->first_quartile = m_pDoc->GetMissingValue();
						b->GetAt(attrib)->maximum = m_pDoc->GetMissingValue();
						b->GetAt(attrib)->mean = m_pDoc->GetMissingValue();
						b->GetAt(attrib)->median = m_pDoc->GetMissingValue();
						b->GetAt(attrib)->minimum = m_pDoc->GetMissingValue();
						b->GetAt(attrib)->third_quartile = m_pDoc->GetMissingValue();
						continue;
					}
					mean /= size;
					a = new Stat;
					a->first_quartile = values[size/4];  //Get_First_Quartile(&values);
					a->maximum = values[size-1];  //Get_Maximum(&values);
					a->minimum = values[0];  //Get_Mimimum(&values);
					a->median = values[size/2];  //Get_Median(&values);
					a->mean = mean; //Get_Mean(&values);
					a->third_quartile = values[3*size/4]; //Get_Third_Quartile(&values);
					b->SetAt(attrib,a);
				}
				gstastats->SetAt(group,b);
			}
			m_pDoc->m_GstaAvailable = 1;
			delete [] values;
		}
		else
		{
			b = new dblStat;
			b->SetSize(nCols);
			for(long i=0;i<nCols;i++)
			{
				a = new Stat;
				a->first_quartile = m_pDoc->GetMissingValue();
				a->maximum = m_pDoc->GetMissingValue();
				a->minimum = m_pDoc->GetMissingValue();
				a->median = m_pDoc->GetMissingValue();
				a->mean = m_pDoc->GetMissingValue();
				a->third_quartile = m_pDoc->GetMissingValue();
				b->SetAt(i,a);
			}

			gstastats->SetAt(0,b);		
			m_pDoc->m_GstaAvailable = 0;
		}
	}
}


CGsta::~CGsta()
{

}

bool CGsta::SaveGsta(CArray<dblStat*> * in)
{
	bool row = false;
	if(in == NULL)
	{
		row = true;
		in = &m_pDoc->m_Gstastats;
	}

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	long i;

	if(in->GetSize() < 1) return false;
	
	if(row)
		strTitle = "Save Group Statistics Table (Rows)";
	else
		strTitle = "Save Group Statistics Table (Columns)";

	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{

	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return false;
	}
	strPathName = pFileDlg->GetPathName();
delete pFileDlg;
}
	else
	{
		if(row) 
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation Row Group Statistics.csv";
		else
			strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation Column Group Statistics.csv";
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
		
		return false;
	}

	CString str;
	CTime time = CTime::GetCurrentTime();	
	if(row)
	{
		str.Format("Project Comment:  %s\nAssociation Option: %s\nClassification Option: %s\nCreation Date/Time: %s\n",
				m_pDoc->m_TableName,
				m_pDoc->m_FuseAssoRows,			
				m_pDoc->m_FuseTypeRows,
				time.Format("%H:%M:%S, %B %d, %Y")		
				);
	}
	else
	{
		str.Format("Project Comment:  %s\nAssociation Option: %s\nClassification Option: %s\nCreation Date/Time: %s\n",
				m_pDoc->m_TableName,
				m_pDoc->m_FuseAssoCols,			
				m_pDoc->m_FuseTypeCols,
				time.Format("%H:%M:%S, %B %d, %Y")		
				);
	}
	f.WriteString(str);

	//write group titles across the first row
	f.WriteString("Variable,Group,Minimum,1st Quartile,Median,Mean,3rd Quartile, Maximum\n");

	long j;
	for(i=0;i<in->GetAt(0)->GetSize();i++){
		for(j=0;j<in->GetSize();j++){
			if(row) m_pDoc->GetColumnLabel(i,strBuffer);
			else m_pDoc->GetRowLabel(i,strBuffer);
			f.WriteString(strBuffer);
//			if(j == 0) f.WriteString(",");
			if(row) strBuffer.Format(",%d",m_pDoc->m_Groups.GetAt(j));
			else strBuffer.Format(",%d",m_pDoc->m_ColGroups.GetAt(j));
			
			f.WriteString(strBuffer);
			
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->minimum);
			f.WriteString(strBuffer);
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->first_quartile);
			f.WriteString(strBuffer);
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->median);
			f.WriteString(strBuffer);
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->mean);
			f.WriteString(strBuffer);
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->third_quartile);
			f.WriteString(strBuffer);
			strBuffer.Format(",%.4f",in->GetAt(j)->GetAt(i)->maximum);
			f.WriteString(strBuffer);
			
			f.WriteString("\n");
			//if(j < in->GetSize()-1)
			//	f.WriteString(",");
		}
	}
	

	f.Close();	

	return true;
}

void CGsta::SaveColumnGsta(CPatnDoc * pDoc)
{

	//create
	long count = 0,o = 0,p = 0;
	double value = 0;

	//CArray <double,double> values;
	
	Stat *a;
	dblStat *b;

	m_pDoc = pDoc;

	long nCols;
	nCols = m_pDoc->GetNoOfColumns();
	long nRows;
	nRows = m_pDoc->GetNoOfRows();

	CArray<dblStat *> Gstastats;

	
	Gstastats.SetSize(m_pDoc->m_ColGroups.GetSize());

	if(m_pDoc->m_ColGroups.GetSize() > 1)
	{
		double * values = new double[nCols];
		long size = 0;

		for(long group=0;group<m_pDoc->m_ColGroups.GetSize();group++){
			b = new dblStat;
			b->SetSize(nRows);
			for(long attrib=0;attrib<nRows;attrib++){
				//values.RemoveAll();
				size = 0;
				double mean = 0;
				for(count=0;group >= 0 && group < m_pDoc->m_Cols.GetSize() &&
					count<m_pDoc->m_Cols[group]->GetSize();count++){
					if(nCols > m_pDoc->m_Cols[group]->GetAt(count) &&
						nRows > attrib &&
						(value = pDoc->GetValueAt(m_pDoc->m_Cols[group]->GetAt(count),attrib)) != pDoc->GetMissingValue()){
						/*o = values.GetSize();					
						for(p=0;p<o;p++){
							if(values[p] > value){
								values.InsertAt(p,value);
								p = values.GetSize();
							}
						}
						if(o == p) values.Add(value);			*/
						values[size] = value;
						size++;
						mean += value;
					}				
				}
				SrtSh1(values,size);
				if(size == 0){
					b->SetAt(attrib,new Stat);
					b->GetAt(attrib)->first_quartile = m_pDoc->GetMissingValue();
					b->GetAt(attrib)->maximum = m_pDoc->GetMissingValue();
					b->GetAt(attrib)->mean = m_pDoc->GetMissingValue();
					b->GetAt(attrib)->median = m_pDoc->GetMissingValue();
					b->GetAt(attrib)->minimum = m_pDoc->GetMissingValue();
					b->GetAt(attrib)->third_quartile = m_pDoc->GetMissingValue();
					continue;
				}
				mean /= size;
				a = new Stat;
				a->first_quartile = values[size/4];  //Get_First_Quartile(&values);
				a->maximum = values[size-1];  //Get_Maximum(&values);
				a->minimum = values[0];  //Get_Mimimum(&values);
				a->median = values[size/2];  //Get_Median(&values);
				a->mean = mean; //Get_Mean(&values);
				a->third_quartile = values[3*size/4]; //Get_Third_Quartile(&values);
				b->SetAt(attrib,a);
			}
			Gstastats.SetAt(group,b);
		}		
		delete [] values;
	}
	else
	{
		b = new dblStat;
		b->SetSize(nRows);
		for(long i=0;i<nRows;i++)
		{
			a = new Stat;
			a->first_quartile = m_pDoc->GetMissingValue();
			a->maximum = m_pDoc->GetMissingValue();
			a->minimum = m_pDoc->GetMissingValue();
			a->median = m_pDoc->GetMissingValue();
			a->mean = m_pDoc->GetMissingValue();
			a->third_quartile = m_pDoc->GetMissingValue();
			b->SetAt(i,a);
		}

		Gstastats.SetAt(0,b);				
	}

	//save
	this->SaveGsta(&Gstastats);

	//delete
	for(p=0;p<Gstastats.GetSize();p++){
		for(long o=0;Gstastats[p] != NULL &&
			o<Gstastats[p]->GetSize();o++)
			delete Gstastats[p]->GetAt(o);
		delete Gstastats[p];
	}
}

