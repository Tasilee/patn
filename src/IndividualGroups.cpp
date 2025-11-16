// IndividualGroups.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "IndividualGroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndividualGroups dialog


CIndividualGroups::CIndividualGroups(CPatnDoc * pDoc,bool columns,CArray<long> * groups,CWnd* pParent /*=NULL*/)
	: CDialog(CIndividualGroups::IDD, pParent)
{
	for(long i=0;i<groups->GetSize();i++)
		m_aGroups.Add(groups->GetAt(i));

	m_origGroups = groups;
	m_aGroupAllocations.SetSize(0);
	m_pDoc = pDoc;
	m_columns = columns;
	//{{AFX_DATA_INIT(CIndividualGroups)
	m_Columns = 0;
	m_Num = -1;
	m_NumGroups = 0;
	//}}AFX_DATA_INIT
}


void CIndividualGroups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndividualGroups)
	DDX_Control(pDX, IDC_SET, m_SetControl);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDC_GROUPNUM, m_Number);
	DDX_Control(pDX, IDC_SELECTED, m_Selected);
	DDX_Control(pDX, IDC_GROUPS, m_Groups);
	DDX_Control(pDX, IDC_ALL, m_All);
	DDX_Text(pDX, IDC_GROUPNUM, m_Num);
	DDX_Text(pDX, IDC_EDIT1, m_NumGroups);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIndividualGroups, CDialog)
	//{{AFX_MSG_MAP(CIndividualGroups)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_ADD_ALL, OnAddAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_DBLCLK(IDC_SELECTED, OnDblclkSelected)
	ON_LBN_DBLCLK(IDC_ALL, OnDblclkAll)
	ON_LBN_SELCHANGE(IDC_ALL, OnSelchangeAll)
	ON_LBN_SELCHANGE(IDC_SELECTED, OnSelchangeSelected)
	ON_LBN_SELCHANGE(IDC_GROUPS, OnSelchangeGroups)
	ON_EN_CHANGE(IDC_GROUPNUM, OnChangeGroupnum)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndividualGroups message handlers

void CIndividualGroups::OnNew() 
{
	CString strLabel,tmplabel;
	long n = m_Groups.GetCount();
	long i;

	UpdateData(true);

	strLabel.Format("%d",m_Num);
	if(m_Groups.FindString(-1,strLabel) == LB_ERR){
		CArray<int> *tmp = new CArray<int>;
		tmp->SetSize(0);

		//add
		for(i=1;i<m_Groups.GetCount();i++){
			m_Groups.GetText(i,tmplabel);
			if(m_Num < atoi(tmplabel)){
				//less than current label, add here
				m_Groups.InsertString(i,strLabel);
				m_aGroupAllocations.InsertAt(i,tmp);
				break;
			}
		}
		if(i >= m_Groups.GetCount()){
			//if ! added
			strLabel.Format("%d",m_Num);
			m_Groups.AddString(strLabel);
			m_aGroupAllocations.Add(tmp);	
		}
	}
	RefreshButtons();
}

void CIndividualGroups::OnDelete() 
{	
	long i;

	if(m_Groups.GetCurSel() != 0){
		i = m_Groups.GetCurSel();
		long nSelCount = m_Selected.GetSelCount();
		CArray<int> aItems;
		long group = m_Groups.GetCurSel();
		long p,n,sel;
		long value;

		for ( sel = m_aGroupAllocations[group]->GetSize()-1; sel >= 0 ; sel-- )
		{
			//remove from the current group
			value = m_aGroupAllocations[group]->GetAt(sel);
			m_aGroupAllocations[group]->RemoveAt(sel);

			m_aGroups.SetAt(value,-1);
			
			//add to '-1' group
			p = m_aGroupAllocations[0]->GetSize();
			for(n=0;n<p;n++){
				if(value<m_aGroupAllocations[0]->GetAt(n)){
					m_aGroupAllocations[0]->InsertAt(n,value);
					n = p;
				}
			}
			if(p == m_aGroupAllocations[0]->GetSize())
				m_aGroupAllocations[0]->Add(value);
		}

		m_Groups.DeleteString(m_Groups.GetCurSel());
		
		delete m_aGroupAllocations[group];
		m_aGroupAllocations.RemoveAt(group);
	}

	m_Groups.SetCurSel(0);

	CRect rect;
	rect.top = 225;
	rect.bottom = 240;
	rect.left = 330;
	rect.right = 380;
	RedrawWindow(rect);
	RefreshButtons();
}

void CIndividualGroups::OnRemove() 
{
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

void CIndividualGroups::OnRemoveAll() 
{
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

void CIndividualGroups::OnAddAll() 
{
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

void CIndividualGroups::OnAdd() 
{
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

void CIndividualGroups::OnDblclkSelected() 
{
	OnRemove();
	RefreshButtons();
}

void CIndividualGroups::OnDblclkAll() 
{
	OnAdd();	
	RefreshButtons();
}

void CIndividualGroups::OnSelchangeAll() 
{
	RefreshButtons();	
}

void CIndividualGroups::OnSelchangeSelected() 
{
	RefreshButtons();	
}

void CIndividualGroups::OnSelchangeGroups() 
{
	CString strLabel;
	long idx = m_Groups.GetCurSel();

//	m_Groups.SetCurSel(idx);
//	strLabel.Format("%d",m_aGroups.GetAt(idx));
//	m_Number.SetWindowText(strLabel);
	m_Groups.GetText(idx,strLabel);
	m_Number.SetWindowText(strLabel);

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

BOOL CIndividualGroups::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_columns){
		m_Title.SetWindowText("Set Column Groups for Anosim");
	}else{
		m_Title.SetWindowText("Set Row Groups for Anosim");
	}

	FillList();
	
	m_All.SetCurSel(-1);
	
	UpdateData(FALSE);
	
	m_Groups.SetCurSel(0);

	m_SetControl.EnableWindow(false);
	
	RefreshButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}

void CIndividualGroups::FillList()
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
	
	m_Groups.AddString("-1");
	tmp = new CArray<int>;
	tmp->RemoveAll();
	m_aGroupAllocations.Add(tmp);

	if(m_columns){
		FillWithColumnLabels();
	}else{
		FillWithRowLabels();
	}
	//use m_aGroups to fill m_aGroupAssociations
	for(i=0;i<m_aGroups.GetSize();i++){
		group = m_aGroups.GetAt(i);
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
			tmp = new CArray<int>;
			tmp->Add(i);
			m_aGroupAllocations.InsertAt(idx,tmp);
		}else{
			m_aGroupAllocations.GetAt(idx)->Add(i);
		}
	}
	SetSelected();
	RefreshButtons();
}

void CIndividualGroups::FillWithColumnLabels()
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

void CIndividualGroups::FillWithRowLabels()
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

void CIndividualGroups::SetSelected()
{
	long i,n;
	CString strLabel;

	m_Selected.ResetContent();

	i = m_Groups.GetCurSel();
	if(i >= 0){
		if(m_columns){
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

void CIndividualGroups::OnChangeGroupnum() 
{
	RefreshButtons();
}

void CIndividualGroups::OnOK() 
{
	//save changes to m_origGroups
	for(long i=0;i<m_aGroups.GetSize();i++)
		m_origGroups->SetAt(i,m_aGroups[i]);

	CDialog::OnOK();
}

void CIndividualGroups::RefreshButtons()
{
	UpdateData(true);

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

	//Set
	if(m_NumGroups > 0 && 
		m_NumGroups != m_aGroupAllocations.GetSize())
		m_SetControl.EnableWindow(true);
	else
		m_SetControl.EnableWindow(false);


}

void CIndividualGroups::OnDestroy() 
{
	CDialog::OnDestroy();
	
	for(long i=0;i<m_aGroupAllocations.GetSize();i++)
		delete m_aGroupAllocations[i];
	
}

void CIndividualGroups::OnChangeEdit1() 
{
	UpdateData(true);
	m_NumGroups++;
	RefreshButtons();
}

void CIndividualGroups::OnSet() 
{
	long num = 0,i;
	CString strnum;

	UpdateData(true);

	//validate as new numgroups, or ignore
	if(m_NumGroups+1 < m_aGroupAllocations.GetSize()){
		while(m_NumGroups+1 < m_aGroupAllocations.GetSize()){
			m_Groups.SetCurSel(m_Groups.GetCount()-1);
			OnDelete();						
		}
	}else{
		while(m_NumGroups >= m_aGroupAllocations.GetSize()){
			num++;
			//check for presence
			for(i=0;i<m_aGroups.GetSize();i++){
				if(m_aGroups[i] == num){
					num++;
					i = -1;
					continue;
				}
			}
			strnum.Format("%d",num);
			m_Number.SetWindowText(strnum);
			OnNew();
		}
	}

	RefreshButtons();
}
