// SelOptions.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "SelOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelOptions dialog


CSelOptions::CSelOptions(CPatnDoc * doc,CWnd* pParent /*=NULL*/)
	: CDialog(CSelOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelOptions)
	m_rows = 0;
	m_sum = 0;
	m_less = 0;
	m_value = 0.0;
	//}}AFX_DATA_INIT

	m_doc = doc;
}


void CSelOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelOptions)
	DDX_Radio(pDX, IDC_ROWS, m_rows);
	DDX_Radio(pDX, IDC_SUM, m_sum);
	DDX_Radio(pDX, IDC_LESS, m_less);
	DDX_Text(pDX, IDC_VALUE, m_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelOptions, CDialog)
	//{{AFX_MSG_MAP(CSelOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelOptions message handlers

void CSelOptions::OnOK() 
{

	UpdateData();

	m_doc->m_ColSelection->RemoveAll();
	m_doc->m_RowSelection->RemoveAll();

	long i,j = 0;
	double value;

	long nrows = m_doc->GetNoOfRows();
	long ncols = m_doc->GetNoOfColumns();
	if(nrows > m_doc->GetIdxTopExtrinsicRow() && m_doc->GetIdxTopExtrinsicRow() != -1)
		nrows = m_doc->GetIdxTopExtrinsicRow();
	if(ncols > m_doc->GetIdxLeftExtrinsicColumn() && m_doc->GetIdxLeftExtrinsicColumn() != -1)
		ncols = m_doc->GetIdxLeftExtrinsicColumn();
	

	if(!m_rows){
		for(i=0;i<nrows;i++){
			//for(long j=0;j<ncols;j++){
			value = GetValue(i,j);
				switch(m_less){
				case 0: //less than
					if(value < m_value)
						m_doc->m_RowSelection->SetAt(i);
					break;
				case 1: //greater than
					if(value > m_value)
						m_doc->m_RowSelection->SetAt(i);
					break;
				case 2: //equal to
					if(value == m_value)
						m_doc->m_RowSelection->SetAt(i);
					break;
				}
			//}
		}
	}else{
		for(i=0;i<ncols;i++){
			//for(long j=0;j<nrows;j++){
				value = GetValue(j,i);
				switch(m_less){
				case 0: //less than
					if(value < m_value)
						m_doc->m_ColSelection->SetAt(i);
					break;
				case 1: //greater than
					if(value > m_value){
						m_doc->m_ColSelection->SetAt(i);
					}
					break;
				case 2: //equal to
					if(value == m_value)
						m_doc->m_ColSelection->SetAt(i);
					break;
				}
			//}
		}
	}

	POSITION pos = m_doc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = m_doc->GetNextView(pos);   
		pView->RedrawWindow();
	} 
	
	CDialog::OnOK();
}


double CSelOptions::GetValue(long row,long col)
{
	long nrows = m_doc->GetNoOfRows();
	long ncols = m_doc->GetNoOfColumns();
	if(nrows < m_doc->GetIdxTopExtrinsicRow())
		nrows = m_doc->GetIdxTopExtrinsicRow();
	if(ncols < m_doc->GetIdxLeftExtrinsicColumn())
		ncols = m_doc->GetIdxLeftExtrinsicColumn();

	//check for the following conditions
/*	if(nrows >= m_doc->m_aIntrinsicRowSums.GetSize() ||
		nrows >= m_doc->m_aIntrinsicRowMaximum.GetSize() ||
		nrows >= m_doc->m_aIntrinsicRowCountPositives.GetSize() ||
		nrows >= m_doc->m_aIntrinsicRowMean.GetSize() ||
		nrows >= m_doc->m_aIntrinsicRowMinimum.GetSize() ||
		nrows >= m_doc->m_aIntrinsicRowCountMissingValues.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnSums.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnMaximum.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnCountPositives.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnMean.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnMinimum.GetSize() ||
		ncols >= m_doc->m_aIntrinsicColumnCountMissingValues.GetSize())
		return 0;*/


	switch(m_sum){
	case 0://sum
		if(!m_rows) return m_doc->GetIntrinsicRowSum(row);
		else return m_doc->GetIntrinsicColumnSum(col);
		break;
	case 1://minimum
		if(!m_rows) return m_doc->GetIntrinsicRowMinimum(row);
		else return m_doc->GetIntrinsicColumnMinimum(col);
		break;
	case 2://mean
		if(!m_rows) return m_doc->GetIntrinsicRowMean(row);
		else return m_doc->GetIntrinsicColumnMean(col);
		break;
	case 3://maximum
		if(!m_rows) return m_doc->GetIntrinsicRowMaximum(row);
		else return m_doc->GetIntrinsicColumnMaximum(col);
		break;
	case 4://>0
		if(!m_rows) return m_doc->GetIntrinsicRowCountPositives(row);
		else return m_doc->GetIntrinsicColumnCountPositives(col);
		break;
	case 5://#missing
		if(!m_rows) return m_doc->GetIntrinsicRowCountMissingValues(row);
		else return m_doc->GetIntrinsicColumnCountMissingValues(col);
		break;
	}

	return m_doc->GetMissingValue();
}
