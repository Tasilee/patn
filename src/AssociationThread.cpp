// AssociationThread.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "AssociationThread.h"
#include "associationdlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAssociationThread

//IMPLEMENT_DYNCREATE(CAssociationThread, CWinThread)
IMPLEMENT_DYNAMIC(CAssociationThread, CWinThread)

BEGIN_MESSAGE_MAP(CAssociationThread, CWinThread)
	//{{AFX_MSG_MAP(CGDIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAssociationThread::CAssociationThread()
{
	m_hEventKillAssociationThread = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CAssociationThread::~CAssociationThread()
{
	CloseHandle(m_hEventKillAssociationThread);
}

BOOL CAssociationThread::InitInstance()
{
	*m_value = Go();

	return FALSE;
}

int CAssociationThread::ExitInstance()
{
	SetEvent(((CAssociationDlg*)m_dlg)->m_hEventKilledAssociationThread);
	if(!((CPatnApp*)AfxGetApp())->silent_mode) m_dlg->PostMessage(WM_CLOSE, 0, 0L);	

	return CWinThread::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CAssociationThread message handlers

BOOL CAssociationThread::BrayCurtisRows()
{
	CWaitCursor waitCursor;

	long i,j,k;
	double nVali, nValj;
	double nBCN, nBCD;	// Numerator and denominator
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	long messagecount = 0;

	m_pDoc->m_AssocMatrixRows.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();
	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrisic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	for ( i = 1; i < nRows; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}

			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			for ( k = 0; k < nCols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = m_pDoc->GetValueAt(k,j);
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					if ( nVali < 0 || nValj < 0 )
					{
						// TODO: improve error message
						long kk;
						for(kk=0;kk<i-1;kk++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[kk];
						m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
						delete pAMR;

						long c,r; CString str1,str2;
						if(nVali < 0){
							m_pDoc->GetColumnLabel(k,str2);
							c = k;
							m_pDoc->GetRowLabel(i,str1);
							r = i;
						}else{
							m_pDoc->GetColumnLabel(k,str2);
							c = k;
							m_pDoc->GetRowLabel(j,str1);
							r = j;
						}
						m_pDoc->m_logerror.Format("Bray & Curtis: Negative data at %s (%dth row):%s (%dth column)",
							str1,r,str2,c);

						if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);

						return FALSE;
					}
					nBCN += fabs(nVali - nValj);
					nBCD += (nVali + nValj);
				}
				else
				{
					nCountMissing++;
				}
			}
			if ( nCountMissing == nCols )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Bray & Curtis: 2 Objects with all missing data (%s (%dth row) and %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				
				return FALSE;
			}
			if ( nBCD == 0 )
			{
				nAssoc = 0;
			}
			else
			{
				nAssoc = nBCN/nBCD;
			}
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixRows.m_strMeasure = "Bray & Curtis (Rows)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}

BOOL CAssociationThread::GowerMetricRows()
{
	CWaitCursor waitCursor;

	long i,j,k;
	double nVali, nValj;
	double nAssoc;
	double nMin, nMax;
	double nRange;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	BOOL bDataFound = FALSE;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	CArray<double> aRanges;
	long messagecount = 0;

	m_pDoc->m_AssocMatrixRows.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();

	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}


	aRanges.SetSize(nCols);
	m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	// Calculate range for each column
	for ( k = 0; k < nCols; k++ )
	{
		if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
			== WAIT_OBJECT_0){
			m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
			m_pDoc->m_logerror = "Canceled by User";
			return FALSE; // Terminate this thread by existing the proc.
		}else if(messagecount > m_Step){
			//m_dlg->SendMessage(WM_USER_UPDATE_NUMBERS,0,0L);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
			messagecount = 0;
		}else{
			messagecount++;
		}

		bDataFound = FALSE;
		nRange = 0;
		for ( i = 0; i < nRows; i++ )
		{
			nVali = m_pDoc->GetValueAt(k,i);
			
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
			// TODO: Improve error message
			CString str1;
			m_pDoc->GetColumnLabel(k,str1);
			m_pDoc->m_logerror.Format("Gower Metric: Column contains all missing values %s (%dth column)",str1,k);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_ICONSTOP | MB_OK, 0);
			m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
			
			return FALSE;
		}
		nRange = nMax-nMin;
		if ( nRange == 0 )
		{
			// TODO: Improve error message
			CString str1;
			m_pDoc->GetColumnLabel(k,str1);
			m_pDoc->m_logerror.Format("Gower Metric: Range is equal to zero in %s (%dth column)",str1,k);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_ICONSTOP | MB_OK, 0);
			m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
			return FALSE;
		}
		aRanges.SetAt(k,nRange);
	}

	for ( i = 1; i < nRows; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nCols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = m_pDoc->GetValueAt(k,j);
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}
				nAssoc += fabs(nVali - nValj)/aRanges.GetAt(k);
			}
			if ( nCountMissing == nCols )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Gower Metric: 2 Objects with all missing data, %s (%dth row) and %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				return FALSE;
			}
			nAssoc /= (nCols - nCountMissing);
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}

	m_pDoc->m_AssocMatrixRows.m_strMeasure = "Gower Metric (Rows)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}


BOOL CAssociationThread::MinkowskiSeriesRows()
{
	long i,j,k;
	double p;
	double nVali, nValj;
	double nMax = 0;
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	long messagecount = 0;

	CWaitCursor waitCursor;

	//p = m_pDoc->m_AssocRowsMinkConst;
	p = m_pDoc->m_AnalysisRecipe.rows.nMinkConst;

	m_pDoc->m_AssocMatrixRows.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	for ( i = 1; i < nRows; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nCols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = m_pDoc->GetValueAt(k,j);
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}
				nAssoc += pow(fabs(nVali - nValj),p);
			}
			if ( nCountMissing == nCols )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Minkowski: 2 Objects with all missing data, %s (%dth row), %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				return FALSE;
			}
			nAssoc = pow(nAssoc,1/p)/(nCols - nCountMissing);
			nMax = max(nMax,nAssoc);
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}

	m_pDoc->m_AssocMatrixRows.m_strMeasure = "Minkowski Series (Rows)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}


BOOL CAssociationThread::TwoStepRows()
{
	CWaitCursor waitCursor;

	long i,j,k;
	double nVali, nValj;
	double nBCN, nBCD;	// Numerator and denominator
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	typedef CArray<double> CMatrixRow;
	CMatrixRow* pMR;
	CArray<CMatrixRow*> aMatrix;
	long messagecount = 0;

	m_pDoc->m_AssocMatrixRows.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	aMatrix.SetSize(nRows);
	m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	// 1st step - produce square matrix
	for ( i = 0; i < nRows; i++ )
	{
		pMR = new CMatrixRow();
		pMR->SetSize(nRows);
		for ( j = 0; j < nRows; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i;k++) delete aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			if(i == j) {
				pMR->SetAt(j,0.0);
				continue;
			}
			
			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			
			for ( k = 0; k < nCols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = m_pDoc->GetValueAt(k,j);
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
			if ( nCountMissing == nCols )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Two-Step: 2 Objects with all missing data, %s (%dth row), %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i;k++) delete aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pMR;
				return FALSE;
			}
			if ( nBCD == 0 )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Two-Step: Sum of 2 objects <= 0, %s (%dth row) %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i;k++) delete aMatrix[k];

				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pMR;
				return FALSE;
			}
			nAssoc = nBCN/nBCD;
			pMR->SetAt(j,nAssoc);
		}
		aMatrix.SetAt(i,(CAssocMatrixRow*)pMR);
	}

	// 2nd step - reprocess square matrix produced in 1st step
	nRows = aMatrix.GetSize();
	for ( i = 1; i < nRows; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				for(i=0;i<aMatrix.GetSize();i++)delete aMatrix[i];
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}

			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nRows; k++ ) // square matrix nRows X nRows
			{
				nVali = aMatrix[i]->GetAt(k);
				nValj = aMatrix[j]->GetAt(k);
				nAssoc += fabs(nVali - nValj);
			}
			nAssoc /= nRows;
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixRows.m_strMeasure = "Two-Step (Rows)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	for(i=0;i<aMatrix.GetSize();i++)
		delete aMatrix[i];

	return TRUE;
}


//----- Columns ---------------------------------------------------------------

BOOL CAssociationThread::BrayCurtisColumns()
{
	CWaitCursor waitCursor;
	long messagecount = 0;

	long i,j,k;
	double nVali, nValj;
	double nBCN, nBCD;	// Numerator and denominator
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;

	m_pDoc->m_AssocMatrixCols.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrisic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	for ( i = 1; i < nCols; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			for ( k = 0; k < nRows; k++ )
			{
				nVali = m_pDoc->GetValueAt(i,k);
				nValj = m_pDoc->GetValueAt(j,k);
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					if ( nVali < 0 || nValj < 0 )
					{
						// TODO: improve error message
						CString str1,str2;
						if(nVali < 0){
							m_pDoc->GetColumnLabel(i,str1);
							m_pDoc->GetRowLabel(k,str2);
							m_pDoc->m_logerror.Format("Bray & Curtis: Cannot have negative data, %s (%dth row):%s (%dth column)",
								str2,k,str1,i);
						}else{
							m_pDoc->GetColumnLabel(j,str1);
							m_pDoc->GetRowLabel(k,str2);
							m_pDoc->m_logerror.Format("Bray & Curtis: Cannot have negative data, %s (%dth row):%s (%dth column)",
								str2,k,str1,j);
						}
						if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
						for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
						m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
						delete pAMR;
						return FALSE;
					}
					nBCN += fabs(nVali - nValj);
					nBCD += (nVali + nValj);
				}
				else
				{
					nCountMissing++;
				}
			}
			if ( nCountMissing == nRows )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Bray & Curtis: 2 Objects with all missing data, %s (%dth column), %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				return FALSE;
			}
			if ( nBCD == 0 )
			{
				nAssoc = 0;
			}
			else
			{
				nAssoc = nBCN/nBCD;
			}
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixCols.m_strMeasure = "Bray & Curtis (Columns)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}

BOOL CAssociationThread::GowerMetricColumns()
{
	CWaitCursor waitCursor;
	long messagecount = 0;

	long i,j,k;
	double nVali, nValj;
	double nAssoc;
	double nMin, nMax;
	double nRange;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	BOOL bDataFound = FALSE;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	CArray<double> aRanges;

	m_pDoc->m_AssocMatrixCols.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	aRanges.SetSize(nRows);
	m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	// Calculate range for each row
	for ( k = 0; k < nRows; k++ )
	{
		if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
		== WAIT_OBJECT_0){
			m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
			m_pDoc->m_logerror = "Canceled by User";
			return FALSE; // Terminate this thread by existing the proc.
		}else if(messagecount > m_Step){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
			messagecount = 0;
		}else{
			messagecount++;
		}
		bDataFound = FALSE;
		nRange = 0;
		for ( i = 0; i < nCols; i++ )
		{
			nVali = m_pDoc->GetValueAt(i,k);
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
			// TODO: Improve error message
			CString str;
			m_pDoc->GetRowLabel(k,str);
			m_pDoc->m_logerror.Format("Gower Metric: Row contains all missing values, %s (%dth row)",str,k);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_ICONSTOP | MB_OK, 0);
			m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);

			return FALSE;
		}
		nRange = nMax-nMin;
		if ( nRange == 0 )
		{
			// TODO: Improve error message
			CString str;
			m_pDoc->GetRowLabel(k,str);
			m_pDoc->m_logerror.Format("Gower Metric: Range is equal to zero, %s (%dth row)",str,k);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_ICONSTOP | MB_OK, 0);
			m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
			return FALSE;
		}
		aRanges.SetAt(k,nRange);
	}

	for ( i = 1; i < nCols; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}

			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nRows; k++ )
			{
				nVali = m_pDoc->GetValueAt(i,k);
				nValj = m_pDoc->GetValueAt(j,k);
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}
				nAssoc += fabs(nVali - nValj)/aRanges.GetAt(k);
			}
			if ( nCountMissing == nRows )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Gower Metric: 2 Variables with all missing data, %s (%dth column), %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
			
				return FALSE;
			}
			nAssoc /= (nRows - nCountMissing);
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixCols.m_strMeasure = "Gower Metric (Columns)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}


BOOL CAssociationThread::MinkowskiSeriesColumns()
{
	long i,j,k;
	double p;
	double nVali, nValj;
	double nMax = 0;
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	long messagecount = 0;

	CWaitCursor waitCursor;

//	p = m_pDoc->m_AssocColsMinkConst;
	p = m_pDoc->m_AnalysisRecipe.cols.nMinkConst;

	m_pDoc->m_AssocMatrixCols.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	for ( i = 1; i < nCols; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				m_pDoc->m_logerror = "Canceled by User";
				delete pAMR;
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}

			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nRows; k++ )
			{
				nVali = m_pDoc->GetValueAt(i,k);
				nValj = m_pDoc->GetValueAt(j,k);
				if ( nVali == nMissingValue || nValj == nMissingValue )
				{
					nCountMissing++;
					continue;
				}

				nAssoc += pow(fabs(nVali - nValj),p);
			}
			if ( nCountMissing == nRows )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Minkowski: 2 Variables with all missing data, %s (%dth column), %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;

				return FALSE;
			}
			nAssoc = pow(nAssoc,1/p)/(nRows - nCountMissing);
			nMax = max(nMax,nAssoc);
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}

	m_pDoc->m_AssocMatrixCols.m_strMeasure = "Minkowski Series (Columns)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}


BOOL CAssociationThread::TwoStepColumns()
{
	CWaitCursor waitCursor;
	long messagecount = 0;

	long i,j,k;
	double nVali, nValj;
	double nBCN, nBCD;	// Numerator and denominator
	double nAssoc;
	double nMissingValue = m_pDoc->GetMissingValue();
	long nCountMissing;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	typedef CArray<double> CMatrixRow;
	CMatrixRow* pMR;
	CArray<CMatrixRow*> aMatrix;

	m_pDoc->m_AssocMatrixCols.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	aMatrix.SetSize(nCols);
	m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	// 1st step - produce square matrix
	for ( i = 0; i < nCols; i++ )
	{
		pMR = new CMatrixRow();
		pMR->SetSize(nCols);
		for ( j = 0; j < nCols; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i;k++) delete aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nCountMissing = 0;
			nBCN = 0;
			nBCD = 0;
			for ( k = 0; k < nRows; k++ )
			{
				nVali = m_pDoc->GetValueAt(i,k);
				nValj = m_pDoc->GetValueAt(j,k);
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
			if ( nCountMissing == nRows )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Two-Step: 2 Variables with all missing data, %s (%dth column), %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i;k++) delete aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pMR;

				return FALSE;
			}
			if ( nBCD == 0 )
			{
				// TODO: improve error message
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Two-Step: Sum of 2 Variables <= 0, %s (%dth column), %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,
				              MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i;k++) delete aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pMR;
				return FALSE;
			}
			nAssoc = nBCN/nBCD;
			pMR->SetAt(j,nAssoc);
		}
		aMatrix.SetAt(i,(CAssocMatrixRow*)pMR);
	}

	// 2nd step - reprocess square matrix produced in 1st step
	nCols = aMatrix.GetSize();
	for ( i = 1; i < nCols; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				for(i=0;i<aMatrix.GetSize();i++)delete aMatrix[i];
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			nCountMissing = 0;
			nAssoc = 0;
			for ( k = 0; k < nCols; k++ ) // square matrix nRows X nRows
			{
				nVali = aMatrix[i]->GetAt(k);
				nValj = aMatrix[j]->GetAt(k);
				nAssoc += fabs(nVali - nValj);
			}
			nAssoc /= nCols;
			pAMR->SetAt(j,nAssoc);
		}
		m_pDoc->m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixCols.m_strMeasure = "Two-Step (Columns)";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	for(i=0;i<aMatrix.GetSize();i++)
		delete aMatrix[i];

	return TRUE;
}

BOOL CAssociationThread::KulzinskiCols()
{
	CWaitCursor waitCursor;
	long messagecount = 0;

	double vsmall = 0.00001;
	long i,j,k;
	double nVali, nValj;
	double nMissingValue = m_pDoc->GetMissingValue();
	long fm;
	double sumin;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	CArray<double> colsum;

	m_pDoc->m_AssocMatrixCols.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(nCols-1);

	for(i=0;i<nCols;i++){
		colsum.Add(0.0);
		for(j=0;j<nRows;j++){
			if(m_pDoc->GetValueAt(i,j) != nMissingValue)
				colsum[i] += m_pDoc->GetValueAt(i,j);
		}
	}

	for ( i = 1; i < nCols; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			sumin = 0.0;
			fm = nRows;
			for ( k = 0; k < nRows; k++ )
			{
				nVali = m_pDoc->GetValueAt(i,k);
				nValj = m_pDoc->GetValueAt(j,k);
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					sumin += min(nVali,nValj);
				}else{
					fm--;
				}
			}
			if ( fm > vsmall )
			{
				pAMR->SetAt(j,1.0 - 0.5*(sumin/colsum[i]+sumin/colsum[j]));
			}else{
				//report an error
				CString str1,str2;
				m_pDoc->GetColumnLabel(i,str1);
				m_pDoc->GetColumnLabel(j,str2);
				m_pDoc->m_logerror.Format("Kulczynski: Attribute containing only missing values, %s (%dth column) OR %s (%dth column)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixCols.m_aMatrix[k];
				m_pDoc->m_AssocMatrixCols.m_aMatrix.SetSize(0);
				delete pAMR;
				return FALSE;

			}
		}
		m_pDoc->m_AssocMatrixCols.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixCols.m_strMeasure = "Kulczynski Columns";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixCols.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}

BOOL CAssociationThread::KulzinskiRows()
{
	CWaitCursor waitCursor;
	long messagecount = 0;

	double vsmall = 0.00001;
	long i,j,k;
	double nVali, nValj;
	double nMissingValue = m_pDoc->GetMissingValue();
	long fm;
	double sumin;
	long nCols, nRows;
	CAssocMatrixRow* pAMR;
	CArray<double> rowsum;

	m_pDoc->m_AssocMatrixRows.Reset();

	if ( m_pDoc->IsExtrinsicColumns() )
		nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
	else
		nCols = m_pDoc->GetNoOfColumns();

	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();
	if(nRows == 0 || nCols == 0){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Table contains no intrinsic values",MB_ICONSTOP | MB_OK, 0);
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
		m_pDoc->m_logerror = "Table contains no intrinsic values";
		return FALSE;
	}

	m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(nRows-1);

	for(i=0;i<nRows;i++){
		rowsum.Add(0.0);
		for(j=0;j<nCols;j++){
			if(m_pDoc->GetValueAt(j,i) != nMissingValue)
				rowsum[i] += m_pDoc->GetValueAt(j,i);
		}
	}

	for ( i = 1; i < nRows; i++ )
	{
		pAMR = new CAssocMatrixRow();
		pAMR->SetSize(i);
		for ( j = 0; j < i; j++ )
		{
			if (WaitForSingleObject(m_hEventKillAssociationThread, 0)
				== WAIT_OBJECT_0){
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				m_pDoc->m_logerror = "Canceled by User";
				return FALSE; // Terminate this thread by existing the proc.
			}else if(messagecount > m_Step){
				if(!((CPatnApp*)AfxGetApp())->silent_mode) ((CAssociationDlg*)m_dlg)->StepIt();
				messagecount = 0;
			}else{
				messagecount++;
			}
			sumin = 0.0;
			fm = nCols;
			for ( k = 0; k < nCols; k++ )
			{
				nVali = m_pDoc->GetValueAt(k,i);
				nValj = m_pDoc->GetValueAt(k,j);
				if ( nVali != nMissingValue && nValj != nMissingValue )
				{
					sumin += min(nVali,nValj);
				}else{
					fm--;
				}
			}
			if ( fm > vsmall )
			{
				pAMR->SetAt(j,1.0 - 0.5*(sumin/rowsum[i]+sumin/rowsum[j]));
			}else{
				//report an error
				CString str1,str2;
				m_pDoc->GetRowLabel(i,str1);
				m_pDoc->GetRowLabel(j,str2);
				m_pDoc->m_logerror.Format("Kulczynski: Object containing only missing values, %s (%dth row) OR %s (%dth row)",
					str1,i,str2,j);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_pDoc->m_logerror,MB_OK | MB_ICONSTOP, 0);
				for(k=0;k<i-1;k++) delete m_pDoc->m_AssocMatrixRows.m_aMatrix[k];
				m_pDoc->m_AssocMatrixRows.m_aMatrix.SetSize(0);
				delete pAMR;
				return FALSE;
			}
		}
		m_pDoc->m_AssocMatrixRows.m_aMatrix.SetAt(i-1,(CAssocMatrixRow*)pAMR);
	}
	m_pDoc->m_AssocMatrixRows.m_strMeasure = "Kulczynski Rows";
	CTime time = CTime::GetCurrentTime();
	m_pDoc->m_AssocMatrixRows.m_Time = time.Format("%H:%M:%S, %B %d, %Y");

	return TRUE;
}

bool CAssociationThread::Go()
{
	long value;
	if(!m_rows){
		switch(m_type){
		case BrayCurtis:
			value = BrayCurtisColumns();
			break;
		case GowerMetric:	
			value = GowerMetricColumns();
			break;
		case MinkowskiSeries:	
			value = MinkowskiSeriesColumns();
			break;
		case TwoStep:	
			value = TwoStepColumns();
			break;
		case Kulzinski:
			value = KulzinskiCols();
			break;
		}
	}else{
		switch(m_type){
		case BrayCurtis:
			value = BrayCurtisRows();
			break;
		case GowerMetric:	
			value = GowerMetricRows();
			break;
		case MinkowskiSeries:	
			value = MinkowskiSeriesRows();
			break;
		case TwoStep:	
			value = TwoStepRows();
			break;
		case Kulzinski:
			value = KulzinskiRows();
			break;
		}
	}
	return !!value;
}