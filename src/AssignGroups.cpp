// AssignGroups.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "AssignGroups.h"
#include ".\assigngroups.h"


// CAssignGroups dialog

IMPLEMENT_DYNAMIC(CAssignGroups, CDialog)
CAssignGroups::CAssignGroups(CPatnDoc * doc, CWnd* pParent /*=NULL*/)
	: CDialog(CAssignGroups::IDD, pParent)
{
	m_pDoc = doc;
}

CAssignGroups::~CAssignGroups()
{
}

BOOL CAssignGroups::OnInitDialog() {
	CDialog::OnInitDialog();

	m_assoc_measures.AddString("Bray and Curtis");
	m_assoc_measures.AddString("Gower Metric");
	m_assoc_measures.AddString("Minkowski Series");
	m_assoc_measures.AddString("Two-step");
	m_assoc_measures.AddString("Kulczynski");
	m_assoc_measures.SetCurSel(0);

	m_const.SetWindowText("2.0");

	return TRUE;
}

void CAssignGroups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ASSOC_MEASURES, m_assoc_measures);
	DDX_Control(pDX, IDC_CHECK1, m_apriori_groups);
	DDX_Control(pDX, IDC_CONSTANT, m_const);
}


BEGIN_MESSAGE_MAP(CAssignGroups, CDialog)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_CONSTANT, OnEnChangeConstant)
END_MESSAGE_MAP()


// CAssignGroups message handlers

void CAssignGroups::OnBnClickedButton4()
{
	this->OnCancel();
}

void CAssignGroups::OnBnClickedButton1()
{
	//group centroids
	int ncols = m_pDoc->GetIdxLeftExtrinsicColumn();
	if(ncols == -1) ncols = m_pDoc->GetNoOfColumns();
	int ngroups = m_pDoc->m_Groups.GetSize();
	CArray<long> * groups = &m_pDoc->m_aRowGroups;

	if(m_apriori_groups.GetCheck()) {
		ngroups = m_pDoc->m_aAprioriGroupColours.GetSize();
		groups = &m_pDoc->m_AprioriGroups;
	}

	double * centroids = new double[ncols * ngroups];
	long * counts = new long[ncols * ngroups];
	long i,j;
	for(i=0;i<ncols * ngroups;i++) {
		centroids[i] = 0;
		counts[i] = 0;
	}
	long g;
	for(i=0;i<m_pDoc->GetNoOfRows();i++) {
		if((g=groups->GetAt(i)) != -1) {
			for(j=0;j<ncols;j++) {
				if(m_pDoc->GetValueAt(j,i) != m_pDoc->GetMissingValue() ){
					centroids[g*ncols + j] += m_pDoc->GetValueAt(j,i);
					counts[g*ncols + j] ++;
				}
			}
		}
	}
	for(i=0;i<ngroups;i++) {
		for(j=0;j<ncols;j++) {
			if(counts[i*ncols +j] > 0) {
				centroids[i*ncols + j] /= (double) counts[i*ncols + j];
			} else {
				centroids[i*ncols +j] = m_pDoc->GetMissingValue();
			}
		}
	}

	//for each row without a group, test against each centroid and determine closest
	for(i=0;i<m_pDoc->GetNoOfRows();i++) {
		if((g=m_pDoc->m_aRowGroups[i]) == -1) {
			int closest = assocDistance(i,centroids,ncols,ngroups);
			if(closest >= 0) {
				//set group
				groups->SetAt(i,closest);
			}
		}
	}

	m_pDoc->UpdateGroups();
	OnCancel();
}

/**
returns the group number for the nearest group, or nMissingValue if none
*/
int CAssignGroups::assocDistance(long i, double * centroids, long ncols, long ngroups) {
	//for row i, get distance against centriods row j

	//return 9999 if not comparison

	long k,ii,j,nRows;
	double nVali, nValj;
	double nBCN, nBCD;	// Numerator and denominator
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	double nMin, nMax;
	double nRange;
	BOOL bDataFound = FALSE;
	CArray<double> aRanges;
	typedef CArray<double> CMatrixRow;
	CMatrixRow* pMR;
	CArray<CMatrixRow*> aMatrix;
	double vsmall = 0.00001;
	long fm;
	double sumin;
	CArray<double> rowsum;

	CString text;
	m_const.GetWindowText(text);
	double p = atof(text); //Minkowski constant

	double minassoc = 9999;
	int mingroup = -1;

	switch(m_assoc_measures.GetCurSel()+1) {
		case CAssociation::BrayCurtis:
		for(j=0;j<ngroups;j++) {
			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			nAssoc = 0;
			for ( k = 0; k < ncols && nAssoc != 9999; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = centroids[j*ncols + k];
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					if ( nVali < 0 || nValj < 0 )
					{
						return -1;		
					}
					nBCN += fabs(nVali - nValj);
					nBCD += (nVali + nValj);
				}
				else
				{
					nCountMissing++;
				}
			}
			if(nAssoc == 0) {
				if ( nCountMissing == ncols )
				{
					//nAssoc = 9999; //error condition
					//return -1;
					continue;
				} else if ( nBCD == 0 )
				{
					nAssoc = 0;
				}
				else
				{
					nAssoc = nBCN/nBCD;
				}
			}
			if(minassoc > nAssoc) {
				minassoc= nAssoc;
				mingroup = j;
			}
		}
	break;
		case CAssociation::GowerMetric:
	aRanges.SetSize(ncols);

	// Calculate range for each column
	for ( k = 0; k < ncols; k++ )
	{
		
		bDataFound = FALSE;
		nRange = 0;
		for ( ii = -1; ii < ngroups; ii++ )
		{
			if(ii <0) {
				//get input value
				nVali = m_pDoc->GetValueAt(k,ii);
			} else {
				nVali = centroids[ii * ncols + k];
			}
			if ( nVali == nMissingValue )
				continue;

			if ( !bDataFound )
			{
				// The first non-missing value
				nMin = nVali;
				nMax = nVali;
				bDataFound = TRUE;
			}
			else
			{
				nMin = min(nMin,nVali);
				nMax = max(nMax,nVali);
			}
		}
		if ( !bDataFound )
		{
			continue;
		}
		nRange = nMax-nMin;
		if ( nRange == 0 )
		{
			continue;
		}
		aRanges.SetAt(k,nRange);
	}

	for(j=0;j<ngroups;j++) {
			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < ncols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = centroids[j*ncols + k];
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}
				nAssoc += fabs(nVali - nValj)/aRanges.GetAt(k);
			}
			if ( nCountMissing == ncols )
			{
				continue;
			}
			nAssoc /= (ncols - nCountMissing);
			if(nAssoc < minassoc) {
				minassoc = nAssoc;
				mingroup = j;
			}
		}
	
	break;
		case CAssociation::MinkowskiSeries:

		for(j=0;j<ngroups;j++) {
	nCountMissing = 0;
	nMax = 0;
			nAssoc = 0;
			for ( k = 0; k < ncols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = centroids[j*ncols + k];
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}
				nAssoc += pow(fabs(nVali - nValj),p);
			}
			if ( nCountMissing == ncols )
			{
				// TODO: improve error message
				continue;
			}
			nAssoc = pow(nAssoc,1/p)/(ncols - nCountMissing);
			nMax = max(nMax,nAssoc);
			if(nAssoc < minassoc) {
				minassoc = nAssoc;
				mingroup = j;
			}
		}
		break;
		case CAssociation::TwoStep:
		// 1st step - produce square matrix
		
	for ( ii = -1; ii < ngroups; ii++ )
	{
		pMR = new CMatrixRow();
		pMR->SetSize(ngroups+1);
		for ( j = -1; j < ngroups; j++ )
		{
			
			if(i == j) {
				pMR->SetAt(j+1,0.0);
				continue;
			}
			
			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			
			for ( k = 0; k < ncols; k++ )
			{
				nVali = (ii==-1)?m_pDoc->GetValueAt(k,i):centroids[ii*ncols + k];
				nValj = (j==-1)?m_pDoc->GetValueAt(k,i):centroids[j*ncols + k];
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					if(nVali > 0){
						nBCN += fabs(nVali - nValj);
						nBCD += (nVali + nValj);
					}
				}
				else
				{
					nCountMissing++;
				}
			}
			if ( nCountMissing == ncols )
			{
				delete pMR;
				return -1;
			}
			if ( nBCD == 0 )
			{
				delete pMR;
				return -1;
			}
			nAssoc = nBCN/nBCD;
			pMR->SetAt(j,nAssoc);
		}
		aMatrix.SetAt(i,(CAssocMatrixRow*)pMR);
	}

	// 2nd step - reprocess square matrix produced in 1st step
	nRows = aMatrix.GetSize();
	for ( ii = 1; ii < nRows; ii++ )
	{
		j = 0; //only process first in the sq mx because it is the new row
		
		{
			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nRows; k++ ) // square matrix nRows X nRows
			{
				nVali = aMatrix[ii]->GetAt(k);
				nValj = aMatrix[j]->GetAt(k);
				nAssoc += fabs(nVali - nValj);
			}
			nAssoc /= nRows;
			if(minassoc > nAssoc){
				minassoc = nAssoc;
				mingroup = ii-1;
			}
		}
	}
break;
		case CAssociation::Kulzinski:
		for(k=0;k<ngroups+1;i++){
			rowsum.Add(0.0);
			if(k == 0) {
				for(long jj=0;jj<ncols;jj++) {
					if(m_pDoc->GetValueAt(jj,i) != nMissingValue) {
						rowsum[k] += m_pDoc->GetValueAt(jj,i);
					}
				}
			} else {
				for(long jj=0;jj<ncols;jj++){
					if(centroids[(k-1)*ncols + jj] != nMissingValue)
						rowsum[k] += centroids[(k-1)*ncols+jj];
				}
			}
		}
		for(j=0;j<ngroups;j++) {
		sumin = 0.0;
			fm = ncols;
			for ( k = 0; k < ncols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = centroids[j*ncols + k];
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					sumin += min(nVali,nValj);
				}else{
					fm--;
				}
			}
			if ( fm > vsmall )
			{
				nAssoc = 1.0 - 0.5*(sumin/rowsum[i]+sumin/rowsum[j]);
			}else{
				//error
				continue;
			}
			if(minassoc > nAssoc) {
				minassoc = nAssoc;
				mingroup = j;
			}
		}

	break;
	}
	return mingroup;
}

void CAssignGroups::OnEnChangeConstant()
{
CString text;
	m_const.GetWindowText(text);
	if(m_assoc_measures.GetCurSel()-1 == CAssociation::MinkowskiSeries){
		double c = atof(text);
		if(c == 0){
			//get correct number
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Try something other than zero.",
				MB_OK | MB_ICONSTOP, 0);
			m_const.SetWindowText("2");
		}
	}	
}
