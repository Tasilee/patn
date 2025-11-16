// EvaluationRecipe.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "EvaluationRecipe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvaluationRecipe dialog


CEvaluationRecipe::CEvaluationRecipe(CPatnDoc * pDoc,EvaluationRecipe * recipe,CWnd* pParent /*=NULL*/)
	: CDialog(CEvaluationRecipe::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEvaluationRecipe)
	m_Recipe = _T("");
	//}}AFX_DATA_INIT

	m_created = false;

	m_pDoc = pDoc;

	//m_Recipe exists, setup HERE
	CString buffer;
CString ir,s;
	m_Recipe += "\r\nEvaluation - \r\n";
	if(recipe->anosim_rows){
		if(recipe->anosim_rows_pairs){
			//s = ir;	ir.Format("%s -EAR %d %d 1 ",s,recipe->anosim_rows_iterations,recipe->anosim_rows_seed);
			buffer.Format("ANOSIM on row group pairs\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n       Groups used: %s\r\n",
				recipe->anosim_rows_seed,recipe->anosim_rows_iterations,((!recipe->anosim_patn_generated_groups)?"PATN generated groups":"a-priori groups"));
		}else{
			//s = ir;	ir.Format("%s -EAR %d %d 0 ",s,recipe->anosim_rows_iterations,recipe->anosim_rows_seed);
			buffer.Format("ANOSIM on rows\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n       Groups used: %s\r\n",
				recipe->anosim_rows_seed,recipe->anosim_rows_iterations,((!recipe->anosim_patn_generated_groups)?"PATN generated groups":"a-priori groups"));
		}
			
		m_Recipe += "   " + buffer;
	}

	if(recipe->anosim_cols){
		if(recipe->anosim_cols_pairs){
			//s = ir;	ir.Format("%s -EAC %d %d 1 ",s,recipe->anosim_cols_iterations,recipe->anosim_cols_seed);
			buffer.Format("ANOSIM on column group pairs\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n",
				recipe->anosim_cols_seed,recipe->anosim_cols_iterations);
		}else{
			//s = ir;	ir.Format("%s -EAC %d %d 0 ",s,recipe->anosim_cols_iterations,recipe->anosim_cols_seed);
			buffer.Format("ANOSIM on columns\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n",
				recipe->anosim_cols_seed,recipe->anosim_cols_iterations);
		}
		m_Recipe += "   " + buffer;
	}

	if(recipe->kw){
		//s = ir;	ir.Format("%s -EK ",s);
		buffer.Format("Box & Whisker across all row groups using selected (or all) Variables\r\n       Groups used: %s\r\n",((!recipe->kw_patn_generated_groups)?"PATN generated groups":"a-priori groups"));		
		m_Recipe += "   " + buffer;
	}

	if(recipe->pcc){
		//s = ir;	ir.Format("%s -EP ",s);
		buffer.Format("PCC with selected variables\r\n");
		m_Recipe += "   " + buffer;
	}
	
	if(recipe->mcao){
	//s = ir;	ir.Format("%s -EM %d %d ",s,recipe->mcao_iterations,recipe->mcao_seed);
		buffer.Format("MCAO on variables run with PCC\r\n       Seed: %d\r\n       Number of Iterations: %d",
			recipe->mcao_seed,recipe->mcao_iterations);
		m_Recipe += "   " + buffer;
	}
	m_Recipe += ir;
}


void CEvaluationRecipe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvaluationRecipe)
	DDX_Control(pDX, IDOK, m_Box3);
	DDX_Control(pDX, IDCANCEL, m_Box2);
	DDX_Control(pDX, IDC_SAVERECIPE, m_Box1);
	DDX_Control(pDX, IDC_ANALYSIS_LAYOUT, m_Box);
	DDX_Text(pDX, IDC_ANALYSIS_LAYOUT, m_Recipe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEvaluationRecipe, CDialog)
	//{{AFX_MSG_MAP(CEvaluationRecipe)
	ON_BN_CLICKED(IDC_SAVERECIPE, OnSaverecipe)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvaluationRecipe message handlers

void CEvaluationRecipe::OnSaverecipe() 
{
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;

	strTitle = "Save Evaluation Recipe:";

	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"txt","*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"text (*.txt)|*.txt|All Files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();

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
		delete pFileDlg;
		return;
	}

	//now write string!
	CTime time = CTime::GetCurrentTime();

	f.WriteString("Recipe of evaluation to be performed on  ");
	f.WriteString(m_pDoc->m_TableName);
	f.WriteString("  at  ");
	f.WriteString(time.Format("%H:%M:%S, %B %d, %Y"));
	
	f.WriteString("\n\n\n");

	f.WriteString(m_Recipe);

	delete pFileDlg;

	f.Close();			
}


void CEvaluationRecipe::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(!m_created)
		return;

	CRect rect,rect2,rect3,rect4,rect5;
	GetWindowRect(rect);
	
	m_Box.GetWindowRect(rect2);
	m_Box1.GetWindowRect(rect3);
	m_Box2.GetWindowRect(rect4);
	m_Box3.GetWindowRect(rect5);

	rect2.bottom = rect.bottom - 40;
	rect3.top = rect.bottom - 32;
	rect3.bottom = rect.bottom - 10;
	rect4.top = rect.bottom - 32;
	rect4.bottom = rect.bottom - 10;
	rect5.top = rect.bottom - 32;
	rect5.bottom = rect.bottom - 10;

	rect2.right = rect.right - 15;
	
	m_dist = rect.bottom;
	m_dist2 = rect.right;

	ScreenToClient(rect2);
	m_Box.MoveWindow(rect2);

	ScreenToClient(rect3);
	m_Box1.MoveWindow(rect3);
	ScreenToClient(rect4);
	m_Box2.MoveWindow(rect4);
	ScreenToClient(rect5);
	m_Box3.MoveWindow(rect5);
	
}

BOOL CEvaluationRecipe::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_created = true;

	//get distance from bottom of box
	CRect rect;
	GetClientRect(rect);
	m_dist = rect.bottom;
	m_dist2 = rect.right;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
