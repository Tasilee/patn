// DataTransform.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "DataTransform.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CDataTransform::CDataTransform(CPatnDoc* pDoc)
{
	ASSERT( pDoc->IsKindOf(RUNTIME_CLASS(CPatnDoc)) );
	m_pDoc = pDoc;
}

void CDataTransform::RangeColumns(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double nOldValue, nNewValue;
	double nMinValue = 0, nMaxValue = 0;
	double nRange = 0;
	CString strLabel;
	CString strMessage;
//	long nResult;
	BOOL bValueFound;
	
	for ( i = 0; i < nCount; i++ )
	{
		// Find the first non-missing value
		bValueFound = FALSE;
		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nMinValue = nOldValue;
				nMaxValue = nOldValue;
				bValueFound = TRUE;
				break;
			}
		}
		if ( ! bValueFound )
		{
			m_pDoc->GetColumnLabel(i, strLabel);
			strMessage  = "The column labelled\n";
			strMessage += strLabel;
			strMessage += '\n';
			strMessage += "contains only missing values.\n";
			strMessage += "Range standardisation cannot\n";
			strMessage += "be performed on that column.\n\n";
			if ( (i+1) < nCount )
			{
				strMessage += "Do you wish to continue with\n";
				strMessage += "range standardisation on the\n";
				strMessage += "remaining selected columns?";
				if(((CPatnApp*)AfxGetApp())->silent_mode)
				{
					waitCursor.Restore();
					continue;
				}
				else if ( AfxMessageBox(strMessage,MB_YESNO | MB_ICONEXCLAMATION) == IDNO )
					return;
				else
				{
					waitCursor.Restore();
					continue;
				}
			}
			else
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
				return;
			}
		}
		// Find max and min values in this column
		for ( ; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nMinValue = min(nMinValue,nOldValue);
				nMaxValue = max(nMaxValue,nOldValue);
			}
		}
		nRange = nMaxValue - nMinValue;
		if ( nRange == 0 )
		{
			m_pDoc->GetColumnLabel(i, strLabel);
			strMessage  = "Range in column labelled\n";
			strMessage += strLabel;
			strMessage += '\n';
			strMessage += "is equal to zero.\n";
			strMessage += "Range standardisation cannot\n";
			strMessage += "be performed on that column.\n\n";
			if ( (i+1) < nCount )
			{
				strMessage += "Do you wish to continue with\n";
				strMessage += "range standardisation on the\n";
				strMessage += "remaining selected columns?";
				if(((CPatnApp*)AfxGetApp())->silent_mode) 
				{
					waitCursor.Restore();
					continue;
				}
				else if ( AfxMessageBox(strMessage,MB_YESNO | MB_ICONEXCLAMATION) == IDNO )
				{
					return;
				}
				else
				{
					waitCursor.Restore();
					continue;
				}
			}
			else
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
				return;
			}
		}

		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue == m_pDoc->GetMissingValue() )
				continue;
			nNewValue = (nOldValue - nMinValue)/nRange;
			m_pDoc->SetValueAt(paItems->GetAt(i),row,nNewValue);
		}
	}
	
	//Log
	CString buffer;
	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Range Columns: ";
	m_pDoc->m_log += "\r\n\tOn Columns: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::RangeRows(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double nOldValue, nNewValue;
	double nMinValue = 0, nMaxValue = 0;
	double nRange = 0;
	CString strLabel;
	CString strMessage;
//	long nResult;
	BOOL bValueFound;
	
	for ( i = 0; i < nCount; i++ )
	{
		// Find the first non-missing value
		bValueFound = FALSE;
		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nMinValue = nOldValue;
				nMaxValue = nOldValue;
				bValueFound = TRUE;
				break;
			}
		}
		if ( ! bValueFound )
		{
			m_pDoc->GetRowLabel(i, strLabel);
			strMessage  = "The row labelled\n";
			strMessage += strLabel;
			strMessage += '\n';
			strMessage += "contains only missing values.\n";
			strMessage += "Range standardisation cannot\n";
			strMessage += "be performed on that row.\n\n";
			if ( (i+1) < nCount )
			{
				strMessage += "Do you wish to continue with\n";
				strMessage += "range standardisation on the\n";
				strMessage += "remaining selected rows?";
				if(((CPatnApp*)AfxGetApp())->silent_mode) 
				{
					waitCursor.Restore();
					continue;

				}
				else if (AfxMessageBox(strMessage,MB_YESNO | MB_ICONEXCLAMATION) == IDNO )
				{
					return;
				}
				else
				{
					
					waitCursor.Restore();
					continue;
				}
			}
			else
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
				return;
			}
		}
		// Find max and min values in this column
		for ( ; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nMinValue = min(nMinValue,nOldValue);
				nMaxValue = max(nMaxValue,nOldValue);
			}
		}
		nRange = nMaxValue - nMinValue;
		if ( nRange == 0 )
		{
			m_pDoc->GetRowLabel(i, strLabel);
			strMessage = "Range in row labelled\n";
			strMessage += strLabel;
			strMessage += '\n';
			strMessage += "is equal to zero.\n";
			strMessage += "Range standardisation cannot\n";
			strMessage += "be performed on that row.\n\n";
			if ( (i+1) < nCount )
			{
				strMessage += "Do you wish to continue with\n";
				strMessage += "range standardisation on the\n";
				strMessage += "remaining selected rows?";
				if(((CPatnApp*)AfxGetApp())->silent_mode) {
				}
				else if ( AfxMessageBox(strMessage,MB_YESNO | MB_ICONEXCLAMATION) == IDNO ){
					return;}
				else
				{
					waitCursor.Restore();
					continue;
				}
			}
			else
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
				return;
			}
		}

		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue == m_pDoc->GetMissingValue() )
				continue;
			nNewValue = (nOldValue - nMinValue)/nRange;
			m_pDoc->SetValueAt(col,paItems->GetAt(i),nNewValue);
		}
	}

	//Log
	CString buffer;
	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Range Rows: ";
	m_pDoc->m_log += "\r\n\tOn Rows: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::LogeColumns(CArray<long>* paItems, double nConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double nOldValue, nNewValue;
	CString temp;
	CString strMessage;
	
	for ( i = 0; i < nCount; i++ )
	{
		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nOldValue += nConstant;
				if ( nOldValue > 0 )
				{
					nNewValue = log(nOldValue);
					m_pDoc->SetValueAt(paItems->GetAt(i),row,nNewValue);
				}else
				{
					strMessage = "The attribute ";
					m_pDoc->GetColumnLabel(paItems->GetAt(i),temp);
					strMessage += temp + " at site ";
					m_pDoc->GetRowLabel(row,temp);
					strMessage += temp + " with constant ";
					temp.Format("%e",nConstant);
					strMessage += temp + " creates value < 0.\n";
					if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
					
				}
			}
		}
	}

	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Log (base e) Columns: \r\n";
	m_pDoc->m_log += "\tConstant = ";
	buffer.Format("%f\r\n\tOn Columns: ",nConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::LogeRows(CArray<long>* paItems, double nConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double nOldValue, nNewValue;
	CString strMessage;
	CString temp;

	for ( i = 0; i < nCount; i++ )
	{
		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nOldValue += nConstant;
				if ( nOldValue > 0 )
				{
					nNewValue = log(nOldValue);
					m_pDoc->SetValueAt(col,paItems->GetAt(i),nNewValue);
				}else
				{
					strMessage = "The attribute ";
					m_pDoc->GetColumnLabel(col,temp);
					strMessage += temp + " at site ";
					m_pDoc->GetRowLabel(paItems->GetAt(i),temp);
					strMessage += temp + " with constant ";
					temp.Format("%e",nConstant);
					strMessage += temp + " creates value < 0.\n";
					if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
					
				}
			}
		}
	}
	
	//Log
	CString buffer;
	
	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Log (base e) Rows: \r\n";
	m_pDoc->m_log += "\tConstant = ";
	buffer.Format("%f\r\n\tOn Rows: ",nConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";


	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::Log10Columns(CArray<long>* paItems, double nConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double nOldValue, nNewValue;
	CString strMessage;
	CString temp;

	for ( i = 0; i < nCount; i++ )
	{
		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nOldValue += nConstant;
				if ( nOldValue > 0 )
				{
					nNewValue = log10(nOldValue);
					m_pDoc->SetValueAt(paItems->GetAt(i),row,nNewValue);
				}else
				{
					strMessage = "The attribute ";
					m_pDoc->GetColumnLabel(paItems->GetAt(i),temp);
					strMessage += temp + " at site ";
					m_pDoc->GetRowLabel(row,temp);
					strMessage += temp + " with constant ";
					temp.Format("%e",nConstant);
					strMessage += temp + " creates value < 0.\n";
					if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
					
				}
			}
		}
	}

	//Log
	CString buffer;
	
	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Log (base 10) Columns: ";
	m_pDoc->m_log += "\r\n\tConstant = ";
	buffer.Format("%f\r\n\tOn Columns: ",nConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";


	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::Log10Rows(CArray<long>* paItems, double nConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double nOldValue, nNewValue;
	CString strMessage;
	CString temp;

	for ( i = 0; i < nCount; i++ )
	{
		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				nOldValue += nConstant;
				if ( nOldValue > 0 )
				{
					nNewValue = log10(nOldValue);
					m_pDoc->SetValueAt(col,paItems->GetAt(i),nNewValue);
				}else
				{
					strMessage = "The attribute ";
					m_pDoc->GetColumnLabel(col,temp);
					strMessage += temp + " at site ";
					m_pDoc->GetRowLabel(paItems->GetAt(i),temp);
					strMessage += temp + " with constant ";
					temp.Format("%e",nConstant);
					strMessage += temp + " creates value < 0.\n";
					if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strMessage,MB_OK | MB_ICONEXCLAMATION);
					
				}
			}
		}
	}
	
	//Log
	CString buffer;
	
	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Log (base 10) Rows: ";
	m_pDoc->m_log += "\r\n\tConstant = ";
	buffer.Format("%f\r\nOn Rows: ",nConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::PowerColumns(CArray<long>* paItems, double nExponent)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double nOldValue, nNewValue;
	double nInteger, nFraction;
	BOOL bInvalid = FALSE;
	
	for ( i = 0; i < nCount; i++ )
	{
		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				if ( nOldValue == 0 && nExponent <= 0 )
					bInvalid = TRUE;
				else if ( nOldValue < 0 && (nFraction = modf(nExponent,&nInteger)) )
					bInvalid = TRUE;
				if ( !bInvalid )
				{
					if ( (nNewValue = pow(nOldValue,nExponent)) == HUGE_VAL )
					{
						if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Overflow occurred - result is too large");
						return;
					}
					m_pDoc->SetValueAt(paItems->GetAt(i),row,nNewValue);
				}
			}
		}
	}

	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Power Columns: ";
	m_pDoc->m_log += "\r\n\tExponent = ";
	buffer.Format("%f\r\n\tOn Columns: ",nExponent);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	
	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::PowerRows(CArray<long>* paItems, double nExponent)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double nOldValue, nNewValue;
	double nInteger, nFraction;
	BOOL bInvalid = FALSE;
	
	for ( i = 0; i < nCount; i++ )
	{
		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != m_pDoc->GetMissingValue() )
			{
				if ( nOldValue == 0 && nExponent <= 0 )
					bInvalid = TRUE;
				else if ( nOldValue < 0 && (nFraction = modf(nExponent,&nInteger)) )
					bInvalid = TRUE;
				if ( !bInvalid )
				{
					if ( (nNewValue = pow(nOldValue,nExponent)) == HUGE_VAL )
					{
						if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Overflow occurred - result is too large");
						return;
					}
					m_pDoc->SetValueAt(col,paItems->GetAt(i),nNewValue);
				}
			}
		}
	}
	
	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Power Rows: ";
	m_pDoc->m_log += "\r\n\tExponent = ";
	buffer.Format("%f\r\n\tOn Rows: ",nExponent);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}


void CDataTransform::NonZeroConstantColumns(CArray<long>* paItems, double nNonZeroConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double nOldValue;
	
	for ( i = 0; i < nCount; i++ )
	{
		for ( row = 0; row < m_pDoc->GetNoOfRows(); row++ )
		{
			nOldValue = m_pDoc->GetValueAt(paItems->GetAt(i),row);
			if ( nOldValue != 0 && nOldValue != m_pDoc->GetMissingValue() )
				m_pDoc->SetValueAt(paItems->GetAt(i),row,nNonZeroConstant);
		}
	}
	
	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Non-zero to Constant Columns: ";
	m_pDoc->m_log += "\r\n\tNonZeroConstant = ";
	buffer.Format("%f\r\n\tOn Columns: ",nNonZeroConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";
	
	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::NonZeroConstantRows(CArray<long>* paItems, double nNonZeroConstant)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double nOldValue;
	
	for ( i = 0; i < nCount; i++ )
	{
		for ( col = 0; col < m_pDoc->GetNoOfColumns(); col++ )
		{
			nOldValue = m_pDoc->GetValueAt(col,paItems->GetAt(i));
			if ( nOldValue != 0 && nOldValue != m_pDoc->GetMissingValue() )
				m_pDoc->SetValueAt(col,paItems->GetAt(i),nNonZeroConstant);
		}
	}
	
	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Non-zero to Constant Rows: ";
	m_pDoc->m_log += "\r\n\tNonZeroConstant = ";
	buffer.Format("%f\r\n\tOn Rows: ",nNonZeroConstant);
	m_pDoc->m_log += buffer;
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}


void CDataTransform::SqrtSosRows(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double factor;
	double sum;

	for ( i = 0; i < nCount; i++ ){
		//find sum of squares for row data
		sum = 0;
		for(col = 0;col < m_pDoc->GetNoOfColumns(); col++){
			if(m_pDoc->GetValueAt(col,paItems->GetAt(i)) !=
				m_pDoc->GetMissingValue())
				sum += pow(m_pDoc->GetValueAt(col,paItems->GetAt(i)),2);
		}
		
		factor = sqrt(sum);

		for(col = 0;col < m_pDoc->GetNoOfColumns(); col++){
			if(m_pDoc->GetValueAt(col,paItems->GetAt(i)) !=
				m_pDoc->GetMissingValue())
				m_pDoc->SetValueAt(col,paItems->GetAt(i),
					m_pDoc->GetValueAt(col,paItems->GetAt(i))/factor);
		}
	}
	
	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Squareroot Sum of Squares Rows: ";
	m_pDoc->m_log += "\r\n\tOn Rows: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::SqrtSosCols(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double factor;
	double sum;

	for ( i = 0; i < nCount; i++ ){
		//find sum of squares for row data
		sum = 0;
		for(row = 0;row < m_pDoc->GetNoOfRows(); row++){
			if(m_pDoc->GetValueAt(paItems->GetAt(i),row) !=
				m_pDoc->GetMissingValue())
				sum += pow(m_pDoc->GetValueAt(paItems->GetAt(i),row),2);
		}
		
		factor = sqrt(sum);

		for(row = 0;row < m_pDoc->GetNoOfRows(); row++){
			if(m_pDoc->GetValueAt(paItems->GetAt(i),row) !=
				m_pDoc->GetMissingValue())
				m_pDoc->SetValueAt(paItems->GetAt(i),row,
					m_pDoc->GetValueAt(paItems->GetAt(i),row)/factor);
		}
	}
	
	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Squareroot Sum of Squares Columns: ";
	m_pDoc->m_log += "\r\n\tOn Columns: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::SumRows(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, col;
	double sum;

	for ( i = 0; i < nCount; i++ ){
		//find sum of squares for row data
		sum = 0;
		for(col = 0;col < m_pDoc->GetNoOfColumns(); col++){
			if(m_pDoc->GetValueAt(col,paItems->GetAt(i)) !=
				m_pDoc->GetMissingValue())
				sum += m_pDoc->GetValueAt(col,paItems->GetAt(i));
		}
		
		for(col = 0;col < m_pDoc->GetNoOfColumns(); col++){
			if(m_pDoc->GetValueAt(col,paItems->GetAt(i)) !=
				m_pDoc->GetMissingValue())
				m_pDoc->SetValueAt(col,paItems->GetAt(i),
					m_pDoc->GetValueAt(col,paItems->GetAt(i))/sum);
		}
	}

	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Sum Rows: ";
	m_pDoc->m_log += "\r\n\tOn Rows: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetRowLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}

void CDataTransform::SumCols(CArray<long>* paItems)
{
	CWaitCursor waitCursor;
	
	long nCount = paItems->GetSize();
	long i, row;
	double sum;

	for ( i = 0; i < nCount; i++ ){
		//find sum of squares for row data
		sum = 0;
		for(row = 0;row < m_pDoc->GetNoOfRows(); row++){
			if(m_pDoc->GetValueAt(paItems->GetAt(i),row) !=
				m_pDoc->GetMissingValue())
				sum += m_pDoc->GetValueAt(paItems->GetAt(i),row);
		}

		for(row = 0;row < m_pDoc->GetNoOfRows(); row++){
			if(m_pDoc->GetValueAt(paItems->GetAt(i),row) !=
				m_pDoc->GetMissingValue())
				m_pDoc->SetValueAt(paItems->GetAt(i),row,
					m_pDoc->GetValueAt(paItems->GetAt(i),row)/sum);
		}
	}

	//Log
	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	m_pDoc->m_log += "Sum Columns: ";
	m_pDoc->m_log += "\r\n\tOn Columns: ";
	for(i=0;i<paItems->GetSize();i++){
		m_pDoc->GetColumnLabel(paItems->GetAt(i),buffer);
		m_pDoc->m_log += buffer;
		if(i<paItems->GetSize()-1) m_pDoc->m_log += ",";
	}
	m_pDoc->m_log += "\r\n\r\n";

	m_pDoc->SetModifiedFlag(TRUE);
	m_pDoc->UpdateAllViews(NULL,0,NULL);
}
