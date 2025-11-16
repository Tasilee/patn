// Association.cpp
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "Association.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CAssociation::CAssociation(CPatnDoc* pDoc)
{
	ASSERT( pDoc->IsKindOf(RUNTIME_CLASS(CPatnDoc)) );
	m_pDoc = pDoc;
}

void CAssociation::WriteAssocMatrixToASCII(long nType)
{
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

	switch (nType)
	{
		case (CPatnDoc::columns):
			strTitle = "Save Association Matrix (Columns):";
			pMatrix = &m_pDoc->m_AssocMatrixCols.m_aMatrix;
			break;

		case (CPatnDoc::rows):
			strTitle = "Save Association Matrix (Rows):";
			pMatrix = &m_pDoc->m_AssocMatrixRows.m_aMatrix;
			break;

		default:
			return;
	}
if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{

	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"csv","*.csv",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CSV File (*.csv)|*.csv|All Files (*.*)|*.*||",NULL);
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
		if(nType == CPatnDoc::rows)
			strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Analysis Row Association Matrix.csv";
		else
			strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Analysis Column Association Matrix.csv";
	
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

	//write header
	if(nType == CPatnDoc::rows){
		tmp.Format("Project Comment: %s\n",m_pDoc->m_TableName);tmp.Replace(","," ");
		f.WriteString(tmp);
		tmp.Format("Association Option: %s\n",m_pDoc->m_AssocMatrixRows.m_strMeasure);tmp.Replace(","," ");
		f.WriteString(tmp);
		tmp.Format("Created Date/Time: %s\n",m_pDoc->m_AssocMatrixRows.m_Time);tmp.Replace(","," ");
		f.WriteString(tmp);
	}else{
		tmp.Format("Project Comment: %s\n",m_pDoc->m_TableName);tmp.Replace(","," ");
		f.WriteString(tmp);
		tmp.Format("Association Option: %s\n",m_pDoc->m_AssocMatrixCols.m_strMeasure);tmp.Replace(","," ");
		f.WriteString(tmp);
		tmp.Format("Created Date/Time: %s\n",m_pDoc->m_AssocMatrixCols.m_Time);tmp.Replace(","," ");
		f.WriteString(tmp);
	}

	// File created OK, write to it

	//write horizontal labels
	f.WriteString(",");
	for(i=0;i<pMatrix->GetSize();i++){
		if(nType == CPatnDoc::rows)
			m_pDoc->GetRowLabel(i,tmp);
		else
			m_pDoc->GetColumnLabel(i,tmp);
		tmp.Replace(","," ");
		if(i < pMatrix->GetSize())
			tmp += ",";
		f.WriteString(tmp);
	}
	f.WriteString("\n");

	for ( i = 0; i < pMatrix->GetSize(); i++ )
	{
		pAMR = pMatrix->GetAt(i);
		if(nType == CPatnDoc::rows)
			m_pDoc->GetRowLabel(i+1,tmp);
		else
			m_pDoc->GetColumnLabel(i+1,tmp);
		tmp.Replace(","," ");
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
}
