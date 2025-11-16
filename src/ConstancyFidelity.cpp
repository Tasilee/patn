// ConstancyFidelity.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ConstancyFidelity.h"
#include ".\constancyfidelity.h"


// CConstancyFidelity dialog

IMPLEMENT_DYNAMIC(CConstancyFidelity, CDialog)
CConstancyFidelity::CConstancyFidelity(CPatnDoc * pDoc, CTableView * table, CWnd* pParent /*=NULL*/)
	: CDialog(CConstancyFidelity::IDD, pParent)
	, m_dblThreashold(0)
{
	m_pDoc = pDoc;
	m_table = table;
	m_idxSelected = new long[m_pDoc->GetNoOfColumns()];
}

CConstancyFidelity::~CConstancyFidelity()
{
	delete m_idxSelected;

	delete m_constancy;
	delete m_fidelity;
	delete m_constancyApriori;
	delete m_fidelityApriori;

	delete m_IdxSelectedColumns;
}

BOOL CConstancyFidelity::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pDoc->m_aAprioriGroupColours.GetSize() <= 1) {
		//disable m_ctrlApriori
		m_ctrlApriori.EnableWindow(FALSE);
	} else if( m_pDoc->m_Groups.GetSize() <= 1) {
		m_ctrlApriori.EnableWindow(FALSE);
		m_ctrlApriori.SetState(TRUE);
	}

	long i,j,g;
	
	//identify all columns with 0, 1, missingvalues only
	m_IdxSelectedColumns = new int[m_pDoc->GetNoOfColumns()];
	long count = 0;
	for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
		bool ok = true;
		for(j=0;j<m_pDoc->GetNoOfRows();j++) {
			double v = m_pDoc->GetValueAt(i,j);
			if(v != 0 && v != 1 && v != m_pDoc->GetMissingValue()) {
				ok = false;
				break;
			}
		}
		if(ok){
			count++;
		}
		m_IdxSelectedColumns[i] = ok;
	}

	if(count == 0) {
		AfxMessageBox("No column containing only 0, 1 and missing value");
		OnCancel();
	}

	//calculate constancy and fidelity
	long maxg = 0;
	for(i=0;i<m_pDoc->m_aRowGroups.GetSize();i++) {
		g = m_pDoc->m_aRowGroups[i];
		if(maxg < g) {
			maxg = g;
		}
	}
	m_groupsCount = maxg;
	m_constancy = new double[m_pDoc->GetNoOfColumns() * (maxg)];
	m_fidelity = new double [m_pDoc->GetNoOfColumns() * (maxg)];
	if(maxg+1 >= 2) {
		calculateConstancyFidelity(m_pDoc->m_Groups.GetSize(), &m_pDoc->m_aRowGroups, m_constancy, m_fidelity, 0);
	}

	maxg = m_pDoc->m_RowControlGroups.GetSize()-1;
	m_groupsAprioriCount = maxg;
	m_constancyApriori = new double[m_pDoc->GetNoOfColumns() * (maxg)];
	m_fidelityApriori = new double[m_pDoc->GetNoOfColumns() * (maxg)];
	if(maxg + 1 >=2) {
		calculateConstancyFidelity(maxg, &m_pDoc->m_AprioriGroups, m_constancyApriori, m_fidelityApriori, 0);
	}

	updateLists();

	m_ctrlThreashold.SetWindowText("50");

	return TRUE;
}

void CConstancyFidelity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECTED, m_selectedColumns);
	DDX_Control(pDX, IDC_CHECK1, m_ctrlApriori);
	DDX_Control(pDX, IDC_CHECK4, m_orderByGroup);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlThreashold);
	DDX_Text(pDX, IDC_EDIT1, m_dblThreashold);
}


BEGIN_MESSAGE_MAP(CConstancyFidelity, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK4, OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConstancyFidelity message handlers

void CConstancyFidelity::OnBnClickedButton3()
{
	ExportCSV();
	this->OnCancel();
}

void CConstancyFidelity::updateLists() {
	long i,j;
	
	m_selectedColumns.ResetContent();

	m_data = new CArray<dblarray*>();
	m_colLabels = new CArray<CString>();
	m_rowLabels = new CArray<CString>();
	m_rowLabels->Add("Group population");
	m_rowLabels->Add("");

	for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
		CString s;
		m_pDoc->GetColumnLabel(i,s);

		m_rowLabels->Add(s);
	}

	m_data->SetSize(m_pDoc->GetNoOfColumns()+2);

	if(!m_ctrlApriori.GetCheck()) {
		for(j=0;j<m_data->GetSize();j++) {
			dblarray * a = new dblarray();
			a->SetSize(m_groupsCount);
			for(int i=0;i<a->GetSize();i++) {
				a->SetAt(i,0);
			}
			m_data->SetAt(j, a);
		}
		for(j=0;j<m_groupsCount;j++) {
			CString s;
			s.Format("Group %d",j+1);
			m_colLabels->Add(s);
			m_data->GetAt(0)->SetAt(j,m_pDoc->m_Rows.GetAt(j)->GetSize());
		}
	} else {
		for(j=0;j<m_data->GetSize();j++) {
			dblarray * a = new dblarray();
			a->SetSize(m_groupsAprioriCount);
			for(int i=0;i<a->GetSize();i++) {
				a->SetAt(i,0);
			}
			m_data->SetAt(j, a);
		}
		for(j=0;j<m_groupsAprioriCount;j++) {
			CString s;
			s.Format("Group %d",j+1);
			m_colLabels->Add(s);
			m_data->GetAt(0)->SetAt(j,m_pDoc->m_RowControlGroups.GetAt(j)->GetSize());
		}
	}

					
	//populate all list
	CString formatted,str;
	if(!m_orderByGroup.GetCheck()) {
		for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
			if(!m_IdxSelectedColumns[i]) continue;

			m_pDoc->GetColumnLabel(i,str);
			
			if(!m_ctrlApriori.GetCheck()) {
				for(j=0;j<m_groupsCount;j++) {
					if(m_constancy[i * m_groupsCount + j] == m_constancy[i * m_groupsCount + j]
						&& m_fidelity[i*m_groupsCount+j] == m_fidelity[i*m_groupsCount+j]
						&& m_constancy[i * m_groupsCount +j]>= 0) { //removes groups with no rows assigned and variables with no presence
						formatted.Format("%.4f, %.4f, %d, ,%d, %s",m_constancy[i * m_groupsCount + j], m_fidelity[i * m_groupsCount + j], j+1, i+1, str);
						m_selectedColumns.AddString(formatted);

						m_data->GetAt(i+2)->SetAt(j,m_constancy[i*m_groupsCount + j]);
					}
				}
			} else {
				for(j=0;j<m_groupsAprioriCount;j++){
					if(m_constancyApriori[i * m_groupsAprioriCount + j] == m_constancyApriori[i * m_groupsAprioriCount + j]
						&& m_fidelityApriori[i * m_groupsAprioriCount + j] == m_fidelityApriori[i * m_groupsAprioriCount + j]
						&& m_constancyApriori[i * m_groupsAprioriCount +j]>= 0) { //removes groups with no rows assigned.
						formatted.Format("%.4f, %.4f, %d, %d, %s",m_constancyApriori[i * m_groupsAprioriCount + j], m_fidelityApriori[i * m_groupsAprioriCount + j], j+1, i+1, str);
						m_selectedColumns.AddString(formatted);

						m_data->GetAt(i+2)->SetAt(j,m_constancyApriori[i * m_groupsAprioriCount + j]);
					}
				} 
			}
		}
	} else {
		for(j=0;j<m_groupsCount;j++) {
			for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
				if(!m_IdxSelectedColumns[i]) continue;

				m_pDoc->GetColumnLabel(i,str);
				
				if(!m_ctrlApriori.GetCheck()) {
				
					if(m_constancy[i * m_groupsCount + j] == m_constancy[i * m_groupsCount + j]
						&& m_fidelity[i*m_groupsCount+j] == m_fidelity[i*m_groupsCount+j]
						&& m_constancy[i * m_groupsCount +j]>= 0) { //removes groups with no rows assigned and variables with no presence
						formatted.Format("%.4f, %.4f, %d, %d, %s",m_constancy[i * m_groupsCount + j], m_fidelity[i * m_groupsCount + j], j+1, i+1, str);
						m_selectedColumns.AddString(formatted);

						m_data->GetAt(i+2)->SetAt(j,m_constancy[i*m_groupsCount + j]);
					}
				
				} else {
					if(m_constancyApriori[i * m_groupsAprioriCount + j] == m_constancyApriori[i * m_groupsAprioriCount + j]
						&& m_fidelityApriori[i * m_groupsAprioriCount + j] == m_fidelityApriori[i * m_groupsAprioriCount + j]
						&& m_constancyApriori[i * m_groupsAprioriCount +j]>= 0) { //removes groups with no rows assigned.
					
						formatted.Format("%.4f, %.4f, %d, %d, %s",m_constancyApriori[i * m_groupsAprioriCount + j], m_fidelityApriori[i * m_groupsAprioriCount + j], j+1, i+1, str);
						m_selectedColumns.AddString(formatted);

						m_data->GetAt(i+2)->SetAt(j,m_constancyApriori[i * m_groupsAprioriCount + j]);
					}
				} 
			}
		}
	}
}

void CConstancyFidelity::calculateConstancyFidelity(long cgroups, CArray<long> * rowgroups, double * constancy, double * fidelity, int offset) {
	long i,j;
	long cols = m_pDoc->GetNoOfColumns();
	long rows = m_pDoc->GetNoOfRows();
	long * presenceCounts = new long[m_pDoc->GetNoOfColumns() * cgroups];
	long * absenceCounts = new long[m_pDoc->GetNoOfColumns() * cgroups];
	long * tPresence = new long[cols];
	long * tAbsence = new long[cols];

	for(i=0;i<cols;i++) {
		for(j=0;j<cgroups;j++) {
			presenceCounts[i * cgroups + j] = 0;
			absenceCounts[i * cgroups + j] = 0;
		}
		tPresence[i] = 0;
		tAbsence[i] = 0;
	}
	for(i=0;i<cols;i++) {
		for(j=0;j<rows;j++) {
			if (rowgroups->GetAt(j) + offset >= 0) {
				if(m_pDoc->GetValueAt(i,j) == 1) {
					presenceCounts[i * cgroups + rowgroups->GetAt(j) + offset]++;
					tPresence[i]++;
				}else if(m_pDoc->GetValueAt(i,j) == 0) {
					absenceCounts[i * cgroups + rowgroups->GetAt(j) + offset]++;
					tAbsence[i]++;
				}
			}
		}
	}

	for(i=0;i<cols;i++) {
		for(j=0;j<cgroups;j++) {
			constancy[i * cgroups + j] = 100.0*presenceCounts[i * cgroups + j] / (double)(presenceCounts[i * cgroups + j] + absenceCounts[i * cgroups + j]);
			fidelity[i * cgroups + j] = 100.0*presenceCounts[i * cgroups + j] / (double)(tPresence[i]);
		}
	}

	//delete presenceCounts;
	//delete absenceCounts;
	delete tPresence;
	delete tAbsence;
}

void CConstancyFidelity::OnBnClickedCheck1()
{
	updateLists();
}

void CConstancyFidelity::OnBnClickedCheck4()
{
	updateLists();
}

void CConstancyFidelity::ExportCSV() {
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Save Constancy and Fidelity:";

	// Get path for the new file
	if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
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
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Constancy and Fidelity " + (m_ctrlApriori.GetCheck()?"PATN generated groups":"Apriori groups") + ".csv";
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

	strBuffer = "";

	//f.WriteString(getExportGroupCompositionHeader(false));
	long i,j;
	CString formatted,str;
	if(!m_orderByGroup.GetCheck()) {
		for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
			if(!m_IdxSelectedColumns[i]) continue;

			m_pDoc->GetColumnLabel(i,str);
			
			if(!m_ctrlApriori.GetCheck()) {
				for(j=0;j<m_groupsCount;j++) {
					if(m_constancy[i * m_groupsCount + j] == m_constancy[i * m_groupsCount + j]
						&& m_fidelity[i*m_groupsCount+j] == m_fidelity[i*m_groupsCount+j]
						&& m_constancy[i * m_groupsCount +j]>= 0) { //removes groups with no rows assigned and variables with no presence
							str.Replace("\"","\"\"");
						formatted.Format("%f,%f,%d,%d,\"%s\"\n",m_constancy[i * m_groupsCount + j], m_fidelity[i * m_groupsCount + j], j+1, i+1, str);
						f.WriteString(formatted);
					}
				}
			} else {
				for(j=0;j<m_groupsAprioriCount;j++){
					if(m_constancyApriori[i * m_groupsAprioriCount + j] == m_constancyApriori[i * m_groupsAprioriCount + j]
						&& m_fidelityApriori[i * m_groupsAprioriCount + j] == m_fidelityApriori[i * m_groupsAprioriCount + j]
						&& m_constancyApriori[i * m_groupsAprioriCount +j]>= 0) { //removes groups with no rows assigned.
							str.Replace("\"","\"\"");
						formatted.Format("%f,%f,%d,%d,\"%s\"\n",m_constancyApriori[i * m_groupsAprioriCount + j], m_fidelityApriori[i * m_groupsAprioriCount + j], j+1, i+1, str);
						f.WriteString(formatted);
					}
				} 
			}
		}
	} else {
		for(j=0;j<m_groupsCount;j++) {
			for(i=0;i<m_pDoc->GetNoOfColumns();i++) {
				if(!m_IdxSelectedColumns[i]) continue;

				m_pDoc->GetColumnLabel(i,str);
				
				if(!m_ctrlApriori.GetCheck()) {
				
					if(m_constancy[i * m_groupsCount + j] == m_constancy[i * m_groupsCount + j]
						&& m_fidelity[i*m_groupsCount+j] == m_fidelity[i*m_groupsCount+j]
						&& m_constancy[i * m_groupsCount +j] >= 0) { //removes groups with no rows assigned and variables with no presence
							str.Replace("\"","\"\"");
						formatted.Format("%f,%f,%d,%d,\"%s\"\n",m_constancy[i * m_groupsCount + j], m_fidelity[i * m_groupsCount + j], j+1, i+1, str);
						f.WriteString(formatted);
					}
				
				} else {
					if(m_constancyApriori[i * m_groupsAprioriCount + j] == m_constancyApriori[i * m_groupsAprioriCount + j]
						&& m_fidelityApriori[i * m_groupsAprioriCount + j] == m_fidelityApriori[i * m_groupsAprioriCount + j]
						&& m_constancyApriori[i * m_groupsAprioriCount +j]  >= 0) { //removes groups with no rows assigned.
							str.Replace("\"","\"\"");
					
						formatted.Format("%f,%f,%d,%d,\"%s\"\n",m_constancyApriori[i * m_groupsAprioriCount + j], m_fidelityApriori[i * m_groupsAprioriCount + j], j+1, i+1, str);
						f.WriteString(formatted);
					}
				} 
			}
		}
	}

	f.Flush();
		
	f.Close();
}
void CConstancyFidelity::OnBnClickedClose()
{
	this->OnCancel();
}

void CConstancyFidelity::OnBnClickedOk()
{
	m_isApriori = m_ctrlApriori.GetCheck()==1;

	this->OnOK();
}
