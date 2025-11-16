// GroupsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "GroupsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg dialog

CGroupsDlg::CGroupsDlg(CPatnDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CGroupsDlg::IDD, pParent)
{
	m_pDoc = pDoc;
	m_aGroups.SetSize(0);
	m_aGroupAllocations.SetSize(0);
	m_aGroupColours.SetSize(0);
	m_nDataControl = false;
	//{{AFX_DATA_INIT(CGroupsDlg)
	m_Columns = 0;
	m_Num = -1;
	//}}AFX_DATA_INIT
}


void CGroupsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupsDlg)
	DDX_Control(pDX, IDC_GROUPCOLOUR, m_GroupColour);
	DDX_Control(pDX, IDC_GROUPNUM, m_Number);
	DDX_Control(pDX, IDC_SELECTED, m_Selected);
	DDX_Control(pDX, IDC_GROUPS, m_Groups);
	DDX_Control(pDX, IDC_ALL, m_All);
	DDX_Radio(pDX, IDC_COLUMNS, m_Columns);
	DDX_Text(pDX, IDC_GROUPNUM, m_Num);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupsDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupsDlg)
	ON_BN_CLICKED(IDC_ROWS, OnRows)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_COLUMNS, OnColumns)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_DBLCLK(IDC_SELECTED, OnDblclkSelected)
	ON_LBN_DBLCLK(IDC_ALL, OnDblclkAll)
	ON_LBN_SELCHANGE(IDC_ALL, OnSelchangeAll)
	ON_LBN_SELCHANGE(IDC_SELECTED, OnSelchangeSelected)
	ON_LBN_SELCHANGE(IDC_GROUPS, OnSelchangeGroups)
	ON_EN_CHANGE(IDC_GROUPNUM, OnChangeGroupnum)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_GROUPCOLOUR, OnGroupcolour)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupsDlg message handlers

void CGroupsDlg::OnRows() 
{
	m_GroupColour.EnableWindow(true);

	long i,n,group;
	//save m_aGroupAllocations to pdoc
	colour temp;
	temp.red = temp.blue = temp.green = 0;

	if(m_nDataControl){
		if(!m_Columns){
			for(i=0;i<m_pDoc->m_ColControlGroups.GetSize();i++){
				delete m_pDoc->m_ColControlGroups[i];
			}
			m_pDoc->m_ColControlGroups.SetSize(0);
		}else{
			for(i=0;i<m_pDoc->m_RowControlGroups.GetSize();i++){
				delete m_pDoc->m_RowControlGroups[i];
			}
			m_pDoc->m_RowControlGroups.SetSize(0);
			m_pDoc->m_PatternApriori.SetSize(0);
		}
	}

	// set size of user group colours
	if(m_Columns && !m_nDataControl){
		// add new empty elements if necessary
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_UserGroupColours.GetSize();){
			m_pDoc->m_UserGroupColours.Add(temp);
			
		}
		// remove excessive elements if necessary
		m_pDoc->m_UserGroupColours.SetSize(m_aGroups.GetSize()-1);
		
	}
	else if(m_Columns && m_nDataControl){
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_aGroupColours2.GetSize();){
			m_pDoc->m_aGroupColours2.Add(temp);
		}
		//m_pDoc->m_aGroupColours2.SetSize(m_aGroups.GetSize()-1);
	}


	for(i=0;i<m_aGroupAllocations.GetSize();i++){
		group = m_aGroups[i];

		if(m_nDataControl){
			if(!m_Columns){
				m_pDoc->m_ColControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_ColControlGroups.SetAt(i,new Rows);
				m_pDoc->m_ColControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}else{
				m_pDoc->m_RowControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_PatternApriori.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_PatternApriori.SetAt(i,i);
				m_pDoc->m_RowControlGroups.SetAt(i,new Rows);
				m_pDoc->m_RowControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}
		}

		for(n=0;n<m_aGroupAllocations[i]->GetSize();n++){
			if(!m_nDataControl){
				if(!m_Columns){
					m_pDoc->m_aColumnGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}else{
					if(i != 0){
						m_pDoc->m_UserGroupColours[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_UserGroupColours[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_UserGroupColours[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_aRowGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}
			}else{
				if(!m_Columns){
					m_pDoc->m_ColControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}else{
					if(i != 0){
						m_pDoc->m_aGroupColours2[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_aGroupColours2[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_aGroupColours2[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_RowControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}
			}
		}
	}	
		

	ResetList();
	m_Groups.SetCurSel(0);
	RefreshButtons();
}

void CGroupsDlg::OnNew() 
{
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}

	CString strLabel;
	long n = m_Groups.GetCount();

	colour temp;
	temp.red = temp.green = temp.blue = 0;

	UpdateData(TRUE);

	strLabel.Format("%d",m_Num);
	if(m_Groups.FindString(-1,strLabel) == LB_ERR){
		CArray<int> *tmp = new CArray<int>;
		tmp->SetSize(0);
		for(long i=0;i<m_Groups.GetCount();i++){
			if(m_Num < m_aGroups[i]) {
				//add
				if(m_Columns) m_aGroupColours.InsertAt(i,temp);
				m_aGroups.InsertAt(i,m_Num);
				strLabel.Format("%d",m_Num);
				m_Groups.InsertString(i,strLabel);
				m_aGroupAllocations.InsertAt(i,tmp);
				i = m_Groups.GetCount();
			}
		}
		if(n == m_Groups.GetCount()){
			//add
			if(m_Columns) m_aGroupColours.Add(temp);
			m_aGroups.Add(m_Num);
			strLabel.Format("%d",m_Num);
			m_Groups.AddString(strLabel);
			m_aGroupAllocations.Add(tmp);
		}	
	}
	RefreshButtons();
}

void CGroupsDlg::OnDelete() 
{	
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}

	long i;

	if(m_Groups.GetCurSel() != 0){
		i = m_Groups.GetCurSel();
		long nSelCount = m_Selected.GetSelCount();
		CArray<int> aItems;
		long group = m_Groups.GetCurSel();
		long o,p,n,sel;
		long value;

		for ( sel = m_aGroupAllocations[group]->GetSize()-1; sel >= 0 ; sel-- )
		{
			//remove from the current group
			value = m_aGroupAllocations[group]->GetAt(sel);
			m_aGroupAllocations[group]->RemoveAt(sel);
			
			//add to '-1' group
			o = p = m_aGroupAllocations[0]->GetSize();
			for(n=0;n<p;n++){
				if(value<m_aGroupAllocations[0]->GetAt(n)){
					m_aGroupAllocations[0]->InsertAt(n,value);
					n = p;
				}
			}
			if(o == m_aGroupAllocations[0]->GetSize())
				m_aGroupAllocations[0]->Add(value);
		}

		if(m_Columns ) m_aGroupColours.RemoveAt(m_Groups.GetCurSel()-1);
		m_Groups.DeleteString(m_Groups.GetCurSel());
		m_aGroups.RemoveAt(m_Groups.GetCurSel()+1);
		delete m_aGroupAllocations[m_Groups.GetCurSel()+1];
		m_aGroupAllocations.RemoveAt(m_Groups.GetCurSel()+1);
	}

	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;
	RedrawWindow(rect);
	RefreshButtons();
}

void CGroupsDlg::OnRemove() 
{
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}

	long nSelCount = m_Selected.GetSelCount();
	CArray<int> aItems;
	long group = m_Groups.GetCurSel();
	long o,p,n,sel;
	long value;

	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_Selected.GetSelItems(nSelCount, &aItems[0]);
		     
		long i;
		for ( i = aItems.GetSize()-1; i >= 0 ; i-- )
		{
			sel = aItems[i];

			//remove from the current group
			value = m_aGroupAllocations[group]->GetAt(sel);
			m_aGroupAllocations[group]->RemoveAt(sel);
			
			//add to '-1' group
			o = p = m_aGroupAllocations[0]->GetSize();
			for(n=0;n<p;n++){
				if(value<m_aGroupAllocations[0]->GetAt(n)){
					m_aGroupAllocations[0]->InsertAt(n,value);
					n = p;
				}
			}
			if(o == m_aGroupAllocations[0]->GetSize())
				m_aGroupAllocations[0]->Add(value);
		}
	}

	UpdateData(FALSE);	
	SetSelected();
	RefreshButtons();
}

void CGroupsDlg::OnRemoveAll() 
{
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}

	long nSelCount = m_Selected.GetSelCount();
	CArray<int> aItems;
	long group = m_Groups.GetCurSel();
	long o,p,n,sel;
	long value;

	for ( sel = m_aGroupAllocations[group]->GetSize()-1; sel >= 0 ; sel-- )
	{
		//remove from the current group
		value = m_aGroupAllocations[group]->GetAt(sel);
		m_aGroupAllocations[group]->RemoveAt(sel);
		
		//add to '-1' group
		o = p = m_aGroupAllocations[0]->GetSize();
		for(n=0;n<p;n++){
			if(value<m_aGroupAllocations[0]->GetAt(n)){
				m_aGroupAllocations[0]->InsertAt(n,value);
				n = p;
			}
		}
		if(o == m_aGroupAllocations[0]->GetSize())
			m_aGroupAllocations[0]->Add(value);
	}

	UpdateData(FALSE);	
	SetSelected();
	RefreshButtons();
}

void CGroupsDlg::OnColumns() 
{
	m_GroupColour.EnableWindow(false);
	long i,n,group;
	//save m_aGroupAllocations to pdoc
	colour temp;
	temp.red = temp.blue = temp.green = 0;

	if(m_nDataControl){
		if(!m_Columns){
			for(i=0;i<m_pDoc->m_ColControlGroups.GetSize();i++){
				delete m_pDoc->m_ColControlGroups[i];
			}
			m_pDoc->m_ColControlGroups.SetSize(0);
		}else{
			for(i=0;i<m_pDoc->m_RowControlGroups.GetSize();i++){
				delete m_pDoc->m_RowControlGroups[i];
			}
			m_pDoc->m_RowControlGroups.SetSize(0);
		}
	}

	// set size of user group colours
	if(m_Columns && !m_nDataControl){
		// add new empty elements if necessary
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_UserGroupColours.GetSize();){
			m_pDoc->m_UserGroupColours.Add(temp);
		
		}
		// remove excessive elements if necessary
		m_pDoc->m_UserGroupColours.SetSize(m_aGroups.GetSize()-1);
		
	}
	else if(m_Columns && m_nDataControl){
		// add new empty elements if necessary
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_aGroupColours2.GetSize();){
		
			m_pDoc->m_aGroupColours2.Add(temp);
		}
		// remove excessive elements if necessary
		
		//m_pDoc->m_aPrioriUserGroupColours.SetSize(m_aGroups.GetSize()-1);
	}

	for(i=0;i<m_aGroupAllocations.GetSize();i++){
		group = m_aGroups[i];
		if(m_nDataControl){
			if(!m_Columns){
				m_pDoc->m_ColControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_ColControlGroups.SetAt(i,new Rows);
				m_pDoc->m_ColControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}else{
				m_pDoc->m_RowControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_RowControlGroups.SetAt(i,new Rows);
				m_pDoc->m_RowControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}
		}
		for(n=0;n<m_aGroupAllocations[i]->GetSize();n++){
			if(!m_nDataControl){
				if(!m_Columns){
					m_pDoc->m_aColumnGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}else{
					if(i != 0){
						m_pDoc->m_UserGroupColours[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_UserGroupColours[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_UserGroupColours[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_aRowGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}
			}else{
				if(!m_Columns){
					m_pDoc->m_ColControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}else{
					if(i != 0){
						m_pDoc->m_aGroupColours2[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_aGroupColours2[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_aGroupColours2[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_RowControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}
			}
		}
	}
	ResetList();
	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;
	RedrawWindow(rect);
	m_Groups.SetCurSel(0);
	RefreshButtons();
}

void CGroupsDlg::OnAddAll() 
{
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}

	long group = m_Groups.GetCurSel();
	long i;
	
	//Remove all entries 
	for(i=0;i<m_aGroupAllocations.GetSize();i++){
		m_aGroupAllocations[i]->RemoveAll();
	}
		
	//Add all to m_Groups current selection
	for(i=0;i<m_All.GetCount();i++){
		m_aGroupAllocations[group]->InsertAt(i,i);		
	}
	
	m_All.SetCurSel(-1);

	UpdateData(FALSE);
	SetSelected();
	RefreshButtons();
}

void CGroupsDlg::OnAdd() 
{ 
	if(m_Columns == 0)
	{ //columns
		m_pDoc->m_FuseAssoCols = "None";
		m_pDoc->m_FuseTypeCols = "User Defined";
		m_pDoc->m_FuseTimeCols = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}
	else
	{ //rows
		m_pDoc->m_FuseAssoRows = "None";
		m_pDoc->m_FuseTypeRows = "User Defined";
		m_pDoc->m_FuseTimeRows = CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	}


	long nSelCount = m_All.GetSelCount();
	CString str;
	long group = m_Groups.GetCurSel();
	long n,p,o,sel;
	CArray<int> aItems;
	
	if ( nSelCount > 0 )
	{
		aItems.SetSize(nSelCount);
		m_All.GetSelItems(nSelCount, &aItems[0]);
		     
		long i;
		for ( i = aItems.GetSize()-1; i >= 0 ; i-- )
		{
			sel = aItems[i];

			//Remove from previous allocation
			p = m_aGroupAllocations.GetSize();
			for(n=0;n<p;n++){
				for(o=0;o<m_aGroupAllocations[n]->GetSize();o++){
					if(m_aGroupAllocations[n]->GetAt(o) == sel){
						//is present, remove from here
						m_aGroupAllocations[n]->RemoveAt(o);
						o = m_aGroupAllocations[n]->GetSize();
					}
				}
			}
		
			//Add to m_GroupAllocations.GetAt(group)
			o = p = m_aGroupAllocations[group]->GetSize();
			for(n=0;n<p;n++){
				if(sel<m_aGroupAllocations[group]->GetAt(n)){
					m_aGroupAllocations[group]->InsertAt(n,sel);
					n = p;
				}
			}
			if(o == m_aGroupAllocations[group]->GetSize())
				m_aGroupAllocations[group]->Add(sel);
	
		}
	}
	
	m_All.SetCurSel(-1);
		
	UpdateData(FALSE);
	SetSelected();
	RefreshButtons();
}

void CGroupsDlg::OnDblclkSelected() 
{
	OnRemove();
	RefreshButtons();
}

void CGroupsDlg::OnDblclkAll() 
{
	OnAdd();	
	RefreshButtons();
}

void CGroupsDlg::OnSelchangeAll() 
{
	RefreshButtons();	
}

void CGroupsDlg::OnSelchangeSelected() 
{
	RefreshButtons();	
}

void CGroupsDlg::OnSelchangeGroups() 
{
	CString strLabel;
	long idx = m_Groups.GetCurSel();

	m_Groups.SetCurSel(idx);
	strLabel.Format("%d",m_aGroups[idx]);
	m_Number.SetWindowText(strLabel);

	if(idx != 0 && m_Columns){
		m_GroupColour.EnableWindow(true);
	}else{
		m_GroupColour.EnableWindow(false);
	}

	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;
	RedrawWindow(rect);

	//reset m_All
	RefreshButtons();
	SetSelected();

}

BOOL CGroupsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_GroupColour.EnableWindow(false);
	CArray<int> aItems;

	m_Columns = m_nDataType;

	CButton * pButton1 = (CButton*)GetDlgItem(IDC_COLUMNS);
	CButton * pButton2 = (CButton*)GetDlgItem(IDC_ROWS);
	if(m_Columns){
		pButton1->SetCheck(false);
		pButton2->SetCheck(true);
	}else{
		pButton1->SetCheck(true);
		pButton2->SetCheck(false);
	}

	FillList();
	m_All.SetCurSel(-1);
	UpdateData(FALSE);
	
	m_Groups.SetCurSel(0);
	
	RefreshButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}

void CGroupsDlg::ResetList()
{
	long nOldDataType = m_Columns;
	UpdateData(TRUE);
	if ( m_Columns != nOldDataType )
	{
		m_All.ResetContent();
		m_Selected.ResetContent();
		m_aSelectedItems.RemoveAll();
		m_All.SetCurSel(-1);
		m_Groups.SetCurSel(-1);
		
		FillList();
		UpdateData(FALSE);
	}
	RefreshButtons();
}

void CGroupsDlg::FillList()
{
	  CString strLabel,tmpLabel;
	long group,idx,n,i;
	CArray<int> *tmp;
    
	//Insert group numbers in m_Groups ListBox
	//free m_aGroupAllocations
	for(i=0;i<m_aGroupAllocations.GetSize();i++){
		delete m_aGroupAllocations[i];
	}
	m_aGroupAllocations.RemoveAll();
	m_aGroupAllocations.FreeExtra();
	m_Groups.ResetContent();
	m_aGroups.RemoveAll();
	m_aGroups.FreeExtra();
	
	m_Groups.AddString("-1");
	m_aGroups.Add(-1);
	tmp = new CArray<int>;
	tmp->RemoveAll();
	m_aGroupAllocations.Add(tmp);

	switch (m_Columns){
		case 0:
			FillWithColumnLabels();

			if(!m_nDataControl){
				for(i=0;i<m_pDoc->GetNoOfColumns();i++){
					group = m_pDoc->GetColumnGroups(i);
					strLabel.Format("%d",group);
					if((idx = m_Groups.FindStringExact(-1,strLabel)) == LB_ERR){
						//find correct insert point
						for(n=0;n<m_Groups.GetCount();n++){
							m_Groups.GetText(n,tmpLabel);
							if(atoi(strLabel) < atoi(tmpLabel)){
								idx = n;
								n = m_Groups.GetCount();
							}else 
								idx = n+1;
						}
						
						if(idx == -1) idx = m_aGroups.GetSize();
						if(idx == -1) idx = 0;
				
						m_Groups.InsertString(idx,strLabel);
						m_aGroups.InsertAt(idx,group);
						tmp = new CArray<int>;
						tmp->Add(i);
						m_aGroupAllocations.InsertAt(idx,tmp);
					}else{
						m_aGroupAllocations.GetAt(idx)->Add(i);
						
					}
				}
			}else{//control groupsfor(i=0;i<m_pDoc->GetNoOfColumns();i++){
				for(i=0;i<m_pDoc->m_ColControlGroups.GetSize();i++){
					for(long j=0;j<m_pDoc->m_ColControlGroups[i]->GetSize();j++){
						long colidx = m_pDoc->m_ColControlGroups[i]->GetAt(j);
						group = i;
						strLabel.Format("%d",group);
						if((idx = m_Groups.FindStringExact(-1,strLabel)) == LB_ERR){
							//find correct insert point
							for(n=0;n<m_Groups.GetCount();n++){
								m_Groups.GetText(n,tmpLabel);
								if(atoi(strLabel) < atoi(tmpLabel)){
									idx = n;
									n = m_Groups.GetCount();
								}else 
									idx = n+1;
							}
							
							if(idx == -1) idx = m_aGroups.GetSize();
							if(idx == -1) idx = 0;
					
							m_Groups.InsertString(idx,strLabel);
							m_aGroups.InsertAt(idx,group);
							tmp = new CArray<int>;
							tmp->Add(colidx);
							m_aGroupAllocations.InsertAt(idx,tmp);
						}else{
							m_aGroupAllocations.GetAt(idx)->Add(colidx);							
						}
					}
				}
			}
			SetSelected();
			break;
			
		case 1:
			FillWithRowLabels();
		
			if(!m_nDataControl){
				for(i=0;i<m_pDoc->GetNoOfRows();i++){
					group = m_pDoc->GetRowGroups(i);
					strLabel.Format("%d",group);
					if((idx = m_Groups.FindStringExact(-1,strLabel)) == LB_ERR){
						//find correct insert point
						for(n=0;n<m_Groups.GetCount();n++){
							m_Groups.GetText(n,tmpLabel);
							if(atoi(strLabel) < atoi(tmpLabel)){
								idx = n;
								n = m_Groups.GetCount();
							}else 
								idx = n+1;
						}

						if(idx == -1) idx = m_aGroups.GetSize();
						if(idx == -1) idx = 0;

						m_Groups.InsertString(idx,strLabel);
						m_aGroups.InsertAt(idx,group);
						tmp = new CArray<int>;
						tmp->Add(i);
						m_aGroupAllocations.InsertAt(idx,tmp);
					}else{
						m_aGroupAllocations.GetAt(idx)->Add(i);
					}
				}
				

				m_aGroupColours.SetSize(m_pDoc->m_UserGroupColours.GetSize());
				for(i=0;i<m_pDoc->m_UserGroupColours.GetSize()-1;i++){
					m_aGroupColours[i].red = m_pDoc->m_UserGroupColours[i].red;
					m_aGroupColours[i].green = m_pDoc->m_UserGroupColours[i].green;
					m_aGroupColours[i].blue = m_pDoc->m_UserGroupColours[i].blue;
				}
			}else{//control groupsfor(i=0;i<m_pDoc->GetNoOfColumns();i++){
				for(i=0;i<m_pDoc->m_RowControlGroups.GetSize();i++){
					for(long j=0;j<m_pDoc->m_RowControlGroups[i]->GetSize();j++){
						long colidx = m_pDoc->m_RowControlGroups[i]->GetAt(j);
						group = i;
						strLabel.Format("%d",group);
						if((idx = m_Groups.FindStringExact(-1,strLabel)) == LB_ERR){
							//find correct insert point
							for(n=0;n<m_Groups.GetCount();n++){
								m_Groups.GetText(n,tmpLabel);
								if(atoi(strLabel) < atoi(tmpLabel)){
									idx = n;
									n = m_Groups.GetCount();
								}else 
									idx = n+1;
							}
							
							if(idx == -1) idx = m_aGroups.GetSize();
							if(idx == -1) idx = 0;
					
							m_Groups.InsertString(idx,strLabel);
							m_aGroups.InsertAt(idx,group);
							tmp = new CArray<int>;
							tmp->Add(colidx);
							m_aGroupAllocations.InsertAt(idx,tmp);
						}else{
							m_aGroupAllocations.GetAt(idx)->Add(colidx);							
						}
					}
				}
				m_aGroupColours.SetSize(m_pDoc->m_aGroupColours2.GetSize());
				for(i=0;i<m_pDoc->m_aGroupColours2.GetSize()-1;i++){
					m_aGroupColours[i].red = m_pDoc->m_aGroupColours2[i].red;
					m_aGroupColours[i].green = m_pDoc->m_aGroupColours2[i].green;
					m_aGroupColours[i].blue = m_pDoc->m_aGroupColours2[i].blue;
				}
			}
			SetSelected();
			break;
			
		default:
			break;
	}
	RefreshButtons();
}

void CGroupsDlg::FillWithColumnLabels()
{   
	long		i;
    CString	strLabel;
	long		nCount = m_pDoc->GetNoOfColumns();
    
    for ( i = 0; i < nCount; i++ )
    {
		m_pDoc->GetColumnLabel(i, strLabel);
		m_All.AddString(strLabel);
    }
	RefreshButtons();
}

void CGroupsDlg::FillWithRowLabels()
{   
	long		i;
    CString	strLabel;
	long		nCount = m_pDoc->GetNoOfRows();
    
    for ( i = 0; i < nCount; i++ )
    {
		m_pDoc->GetRowLabel(i, strLabel);
		m_All.AddString(strLabel);
    }
	RefreshButtons();
}

void CGroupsDlg::SetSelected()
{
	long i,n;
	CString strLabel;

	m_Selected.ResetContent();

	i = m_Groups.GetCurSel();
	if(i >= 0){
		if(!m_Columns){
			for(n=0;n<m_aGroupAllocations.GetAt(i)->GetSize();n++)
			{
				m_pDoc->GetColumnLabel(m_aGroupAllocations.GetAt(i)->GetAt(n),strLabel);
				m_Selected.AddString(strLabel);
			}
		}else{
			for(n=0;n<m_aGroupAllocations.GetAt(i)->GetSize();n++)
			{
				m_pDoc->GetRowLabel(m_aGroupAllocations.GetAt(i)->GetAt(n),strLabel);
				m_Selected.AddString(strLabel);
			}
		}			
	}
	RefreshButtons();
}

void CGroupsDlg::OnChangeGroupnum() 
{
	RefreshButtons();
}

void CGroupsDlg::OnOK() 
{
	//save m_aGroupAllocations to pdoc
	long i,n,group;
	colour temp;
	temp.red = temp.blue = temp.green = 0;

	// set size of user group colours
	if(m_Columns && !m_nDataControl){
		// add new empty elements if necessary
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_UserGroupColours.GetSize();){
			m_pDoc->m_UserGroupColours.Add(temp);
		}
		// remove excessive elements if necessary
		m_pDoc->m_UserGroupColours.SetSize(m_aGroups.GetSize()-1);
	}
	if(m_Columns && m_nDataControl){
		// add new empty elements if necessary
		for(i=m_aGroups.GetSize()-1;i>m_pDoc->m_aGroupColours2.GetSize();){
			m_pDoc->m_aGroupColours2.Add(temp);
		}
		// remove excessive elements if necessary
		//m_pDoc->m_aPrioriUserGroupColours.SetSize(m_aGroups.GetSize()-1);
	}

	if(m_nDataControl){
		if(!m_Columns){
			for(i=0;i<m_pDoc->m_ColControlGroups.GetSize();i++){
				delete m_pDoc->m_ColControlGroups[i];
			}
			m_pDoc->m_ColControlGroups.SetSize(0);
		}else{
			for(i=0;i<m_pDoc->m_RowControlGroups.GetSize();i++){
				delete m_pDoc->m_RowControlGroups[i];
			}
			m_pDoc->m_RowControlGroups.SetSize(0);
			m_pDoc->m_PatternApriori.SetSize(0);
		}
	}

	for(i=0;i<m_aGroupAllocations.GetSize();i++){
		group = m_aGroups[i];

		if(m_nDataControl){
			if(!m_Columns){
				m_pDoc->m_ColControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_ColControlGroups.SetAt(i,new Rows);
				m_pDoc->m_ColControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}else{
				m_pDoc->m_RowControlGroups.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_PatternApriori.SetSize(m_aGroupAllocations.GetSize());
				m_pDoc->m_PatternApriori.SetAt(i,i);
				m_pDoc->m_RowControlGroups.SetAt(i,new Rows);
				m_pDoc->m_RowControlGroups[i]->SetSize(m_aGroupAllocations[i]->GetSize());
			}
		}

		for(n=0;n<m_aGroupAllocations[i]->GetSize();n++){
			if(!m_nDataControl){
				if(!m_Columns){
					m_pDoc->m_aColumnGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}else{
					if(i != 0){
						m_pDoc->m_UserGroupColours[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_UserGroupColours[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_UserGroupColours[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_aRowGroups[m_aGroupAllocations[i]->GetAt(n)] = group;
				}
			}else{
				if(!m_Columns){
					m_pDoc->m_ColControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}else{
					if(i != 0){
						m_pDoc->m_aGroupColours2[i-1].red = m_aGroupColours[i-1].red;
						m_pDoc->m_aGroupColours2[i-1].green = m_aGroupColours[i-1].green;
						m_pDoc->m_aGroupColours2[i-1].blue = m_aGroupColours[i-1].blue;
					}
					m_pDoc->m_RowControlGroups[i]->SetAt(n,m_aGroupAllocations[i]->GetAt(n));
				}
			}
		}
	}

	CDialog::OnOK();
	RefreshButtons();
}

void CGroupsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	long idx = m_Groups.GetCurSel();
	//return if group -1 or none selected
	if(idx <= 0 || !m_Columns ) return;

	long r = (long) (m_aGroupColours[idx-1].red*255);
	long g = (long) (m_aGroupColours[idx-1].green*255);
	long b = (long) (m_aGroupColours[idx-1].blue*255);

	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;

	CBrush brush;
	brush.CreateSolidBrush(RGB(r, g, b));

	dc.FillRect(rect,&brush);

	RefreshButtons();
}

void CGroupsDlg::OnGroupcolour() 
{
	long red = (long)(m_aGroupColours[m_Groups.GetCurSel()-1].red*0xff);
	long green = (long)(m_aGroupColours[m_Groups.GetCurSel()-1].green*0xff);
	long blue = (long)(m_aGroupColours[m_Groups.GetCurSel()-1].blue*0xff);
	unsigned long clr = (unsigned long)(red + 0x100*(green+0x100*blue));
	CColorDialog dlg(clr);
	if(dlg.DoModal()){
		clr = dlg.GetColor();
		m_aGroupColours[m_Groups.GetCurSel()-1].red = GetRValue(clr)/255.0f;
		m_aGroupColours[m_Groups.GetCurSel()-1].green = GetGValue(clr)/255.0f;
		m_aGroupColours[m_Groups.GetCurSel()-1].blue = GetBValue(clr)/255.0f;
	}

	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;
	RedrawWindow(rect);
	RefreshButtons();
}

void CGroupsDlg::RefreshButtons()
{
	//add & add all
	// active only if a group selected
	if(m_Groups.GetCurSel() >= 0){
		//active
		CButton* pButton;
		if(m_All.GetCurSel() >= 0){
			pButton = (CButton*)GetDlgItem(IDC_ADD);
			if ( pButton != NULL )
				pButton->EnableWindow(true);
		}else{
			pButton = (CButton*)GetDlgItem(IDC_ADD);
			if ( pButton != NULL )
				pButton->EnableWindow(false);
		}
		pButton = (CButton*)GetDlgItem(IDC_ADD_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(true);
	}else{
		//false
		CButton* pButton;
		pButton = (CButton*)GetDlgItem(IDC_ADD);
		if ( pButton != NULL )
			pButton->EnableWindow(false);
		pButton = (CButton*)GetDlgItem(IDC_ADD_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(false);
	}

	//remove & remove all
	// active only if elements in selected box
	if(m_Selected.GetCount() > 0){
		//active
		CButton* pButton;
		if(m_Selected.GetCurSel() > 0){
			pButton = (CButton*)GetDlgItem(IDC_REMOVE);
			if ( pButton != NULL )
				pButton->EnableWindow(true);
		}else{
			pButton = (CButton*)GetDlgItem(IDC_REMOVE);
			if ( pButton != NULL )
				pButton->EnableWindow(false);
		}
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(true);
	}else{
		//false
		CButton* pButton;
		pButton = (CButton*)GetDlgItem(IDC_REMOVE);
		if ( pButton != NULL )
			pButton->EnableWindow(false);
		pButton = (CButton*)GetDlgItem(IDC_REMOVE_ALL);
		if ( pButton != NULL )
			pButton->EnableWindow(false);
	}

	//new
	// active only if textbox contains a valid new group number
	UpdateData(TRUE);
	CString strLabel;
	strLabel.Format("%d",m_Num);
	if(m_Groups.FindString(-1,strLabel) == LB_ERR && m_Num > 0){
		//active
		CButton* pButton = (CButton*)GetDlgItem(IDC_NEW);
		if ( pButton != NULL )
			pButton->EnableWindow(true);
	}else{
		//false
		CButton* pButton = (CButton*)GetDlgItem(IDC_NEW);
		if ( pButton != NULL )
			pButton->EnableWindow(false);
	}

	//delete
	// active only if a group is selected
	if(m_Groups.GetCurSel() > 0){
		//active
		CButton* pButton = (CButton*)GetDlgItem(IDC_DELETE);
		if ( pButton != NULL )
			pButton->EnableWindow(true);
	}else{
		//false
		CButton* pButton = (CButton*)GetDlgItem(IDC_DELETE);
		if ( pButton != NULL )
			pButton->EnableWindow(false);

	}

	if(m_nDataControl){
		GetDlgItem(IDC_COLUMNS)->EnableWindow(false);

//		m_GroupColour.EnableWindow(false);

	}

}

void CGroupsDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	for(long i=0;i<m_aGroupAllocations.GetSize();i++)
		delete m_aGroupAllocations[i];
	
}
