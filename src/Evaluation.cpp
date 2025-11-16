// Evaluation.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "Evaluation.h"
#include "evaluationrecipe.h"
#include "anosim.h"
#include "pcc.h"
#include "kruskalWalice.h"
#include "mcao.h"
#include "anosimpage.h"
#include "kruskalwallis.h"
#include "pccmcaopage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvaluation dialog


CEvaluation::CEvaluation(CPatnDoc * doc,CWnd* pParent /*=NULL*/)
	: CDialog(CEvaluation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEvaluation)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = doc;
}


void CEvaluation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvaluation)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEvaluation, CDialog)
	//{{AFX_MSG_MAP(CEvaluation)
	ON_BN_CLICKED(IDC_SETPARMS, OnSetparms)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvaluation message handlers

void CEvaluation::OnSetparms() 
{
	//create property box...
	CAnosimPage pageAnosim(m_pDoc);
	CPccMcaoPage pagePccMcao(m_pDoc);
	CKruskalWallis pageKw(m_pDoc);

	long i;

	CPropertySheet		ps("Set evaluation parameters", this);

	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;	// remove 'Apply' button

	// anosim page
	ps.AddPage(&pageAnosim);
	pageAnosim.m_cols = m_params.anosim_cols;
	pageAnosim.m_rows = m_params.anosim_rows;
	pageAnosim.m_cols_seed = m_params.anosim_cols_seed;
	pageAnosim.m_rows_seed = m_params.anosim_rows_seed;
	pageAnosim.m_rows_iterations = m_params.anosim_rows_iterations;
	pageAnosim.m_cols_iterations = m_params.anosim_cols_iterations;
	pageAnosim.m_cols_groups.SetSize(m_params.anosim_cols_groups.GetSize());
	pageAnosim.m_patn_generated_groups = m_params.anosim_patn_generated_groups;
	for(i=0;i<m_params.anosim_cols_groups.GetSize();i++){
		pageAnosim.m_cols_groups[i] = m_params.anosim_cols_groups[i];
	}
	pageAnosim.m_rows_groups.SetSize(m_params.anosim_rows_groups.GetSize());
	for(i=0;i<m_params.anosim_rows_groups.GetSize();i++){
		pageAnosim.m_rows_groups[i] = m_params.anosim_rows_groups[i];
	}
	pageAnosim.m_cols_pairs = m_params.anosim_cols_pairs;
	pageAnosim.m_rows_pairs = m_params.anosim_rows_pairs;

	// kw page
	ps.AddPage(&pageKw);
	pageKw.m_patn_generated_groups = m_params.kw_patn_generated_groups;
	pageKw.m_kw = m_params.kw;
	pageKw.m_KWAttributes.SetSize(m_pDoc->m_KWAttributes.GetSize());
	for(i=0;i<m_pDoc->m_KWAttributes.GetSize();i++){
		pageKw.m_KWAttributes[i] = m_pDoc->m_KWAttributes[i];
	}

	// pcc / mcao page
	ps.AddPage(&pagePccMcao);
	pagePccMcao.m_pcc = m_params.pcc;
	pagePccMcao.m_pcc_indexes.SetSize(m_params.pcc_indexes.GetSize());
	for(i=0;i<m_params.pcc_indexes.GetSize();i++){
		pagePccMcao.m_pcc_indexes[i] = m_params.pcc_indexes[i];
	}
	pagePccMcao.m_mcao = m_params.mcao;
	pagePccMcao.m_mcao_seed = m_params.mcao_seed;
	pagePccMcao.m_mcao_iterations = m_params.mcao_iterations;

	if(ps.DoModal() != IDOK)
		return;

	//set parameters to new values
	// anosim page
	m_params.anosim_cols = (pageAnosim.m_cols)?true:false;
	m_params.anosim_rows = (pageAnosim.m_rows)?true:false;
	m_params.anosim_cols_seed = pageAnosim.m_cols_seed;
	m_params.anosim_rows_seed = pageAnosim.m_rows_seed;
	m_params.anosim_rows_iterations = pageAnosim.m_rows_iterations;
	m_params.anosim_cols_iterations = pageAnosim.m_cols_iterations;
	m_params.anosim_patn_generated_groups = pageAnosim.m_patn_generated_groups;
	for(i=0;i<pageAnosim.m_cols_groups.GetSize();i++)
		m_params.anosim_cols_groups[i] = pageAnosim.m_cols_groups[i];
	for(i=0;i<pageAnosim.m_rows_groups.GetSize();i++)
		m_params.anosim_rows_groups[i] = pageAnosim.m_rows_groups[i];
	m_params.anosim_cols_pairs = (pageAnosim.m_cols_pairs)?true:false;
	m_params.anosim_rows_pairs = (pageAnosim.m_rows_pairs)?true:false;

	// kw page
	m_params.kw = (pageKw.m_kw)?true:false;
	m_params.kw_patn_generated_groups = pageKw.m_patn_generated_groups;
	m_pDoc->m_KWAttributes.SetSize(pageKw.m_KWAttributes.GetSize());
	for(i=0;i<pageKw.m_KWAttributes.GetSize();i++){
		m_pDoc->m_KWAttributes[i] = pageKw.m_KWAttributes[i];
	}

	// pcc / mcao page
	m_params.pcc = (pagePccMcao.m_pcc)?true:false;
	m_params.pcc_indexes.SetSize(pagePccMcao.m_pcc_indexes.GetSize());
	for(i=0;i<pagePccMcao.m_pcc_indexes.GetSize();i++){
		m_params.pcc_indexes[i] = pagePccMcao.m_pcc_indexes[i];
	}
	m_params.mcao = (pagePccMcao.m_mcao)?true:false;
	m_params.mcao_seed = pagePccMcao.m_mcao_seed;
	m_params.mcao_iterations = pagePccMcao.m_mcao_iterations;
}

void CEvaluation::OnOK() 
{
	//BeginWaitCursor();                        1

	CEvaluationRecipe dlg(m_pDoc,&m_params);
	
	if(dlg.DoModal() == IDOK){
		run(&dlg);

		CDialog::OnOK();
	}
	//EndWaitCursor();
}

BOOL CEvaluation::OnInitDialog()
{
	CDialog::OnInitDialog();

	init();
	
	return TRUE;
}

#include "progressdlgthread.h"

void CEvaluation::run(CEvaluationRecipe * dlg)
{
	long i;
	CString m_errlog,buffer;

	CPcc cpcc(m_pDoc);
	CMCAO mcao;
	CnewDialog * newdlg = NULL;
	CKruskalWallace kw;
	CAnosim anosim(m_pDoc);
CString ir,s;
	this->BeginWaitCursor();

	//this->ShowWindow(SW_HIDE);
	//EnableWindow(false);

	m_pDoc->m_kw_patn_generated_groups = m_params.kw_patn_generated_groups;

	if(true){
	//run analysis & close
		if(dlg != NULL){
			m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
			m_pDoc->m_log += "\r\n";
			m_pDoc->m_log += dlg->m_Recipe;
		}
		m_pDoc->m_log += "\r\n\r\n";

		clock_t t2 = clock();
		CString sst;

		if(m_params.anosim_rows){
if(!((CPatnApp*)AfxGetApp())->silent_mode)			newdlg = new CnewDialog;
else newdlg = NULL;
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - Anosim on Rows...");
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
			m_pDoc->m_ANOSIMRowsExists = false;
			m_pDoc->m_RowAsim_pairs = false;
			if(m_params.anosim_rows_pairs){
				//s = ir;	ir.Format("%s -EAR %d %d 1 ",s,m_params.anosim_rows_iterations,m_params.anosim_rows_seed);
				//do new anosim
				CArray<long> * grps;
				if(m_params.anosim_patn_generated_groups)
				{
					grps = &m_pDoc->m_AprioriGroups;
				}
				else
				{
					grps = &m_pDoc->m_Groups;
				}
				if(anosim.AnosimPairs(false,newdlg,
					grps,
					m_params.anosim_rows_iterations,
					m_params.anosim_rows_seed,false,
					m_params.anosim_patn_generated_groups)){
					m_pDoc->m_ANOSIMRowsExists = true;
					m_pDoc->m_RowAsim_pairs = true;
					for(i=0;i<m_pDoc->m_Groups.GetSize();i++)
						m_pDoc->m_AnosimRowGroups.Add(m_pDoc->m_Groups[i]);
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Row ANOSIM succeeded " + sst + " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- Row ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
					m_errlog += "- Row ANOSIM failed " + sst + " \r\n";
				}
			}else{
				//s = ir;	ir.Format("%s -EAR %d %d 0 ",s,m_params.anosim_rows_iterations,m_params.anosim_rows_seed);
				if(anosim.AnosimRows(newdlg,m_params.anosim_rows_iterations,
					m_params.anosim_rows_seed,false,
					m_params.anosim_patn_generated_groups)){
					m_pDoc->m_ANOSIMRowsExists = true;
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Row ANOSIM succeeded " + sst + " \r\n";

				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
         			m_pDoc->m_log += "- Row ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
					m_errlog += "- Row ANOSIM failed " + sst + " \r\n";
				}
			}
			if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
		}

		t2 = clock();
		if(m_params.anosim_cols){
if(!((CPatnApp*)AfxGetApp())->silent_mode)			newdlg = new CnewDialog;
else newdlg = NULL;
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - Anosim on Columns...");
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
			m_pDoc->m_ANOSIMColsExists = false;
			m_pDoc->m_ColumnAsim_pairs = false;
			if(m_params.anosim_cols_pairs){
				//s = ir;	ir.Format("%s -EAC %d %d 1 ",s,m_params.anosim_cols_iterations,m_params.anosim_cols_seed);
				//do new anosim
				if(anosim.AnosimPairs(true,newdlg,
					&m_pDoc->m_ColGroups,
					m_params.anosim_cols_iterations,
					m_params.anosim_cols_seed,false,false)){
					m_pDoc->m_ANOSIMColsExists = true;
					m_pDoc->m_ColumnAsim_pairs = true;
					for(i=0;i<m_pDoc->m_ColGroups.GetSize();i++)
						m_pDoc->m_AnosimColGroups.Add(m_pDoc->m_ColGroups[i]);
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Column ANOSIM succeeded " + sst + " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- Column ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
					m_errlog += "- Column ANOSIM failed " + sst + " \r\n";
				}
			}else{
//s = ir;	ir.Format("%s -EAC %d %d 0 ",s,m_params.anosim_cols_iterations,m_params.anosim_cols_seed);
if(anosim.AnosimCols(newdlg,m_params.anosim_cols_iterations,
					m_params.anosim_cols_seed,false)){
					m_pDoc->m_ANOSIMColsExists = true;
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ Column ANOSIM succeeded " + sst + " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
		     		m_pDoc->m_log += "- Column ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
					m_errlog += "- Column ANOSIM failed " + sst + " \r\n";
				}
			}
			if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
		}

		t2 = clock();
		
		if(m_params.kw){
			//s = ir;	ir.Format("%s -EK ",s);

			//test if row groups exist
			if(m_pDoc->m_Rows.GetSize() <= 1){
				//report error
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Kruskal-Wallis cannot be performed if object groups are not defined");
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- Box & Whisker failed " + sst + " \r\n    object groups are not defined\r\n";
				m_errlog += "- Box & Whisker Statistics failed " + sst + " \r\n"; 
			}else{
if(!((CPatnApp*)AfxGetApp())->silent_mode)				newdlg = new CnewDialog;
else newdlg = NULL;
				if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - Box & Whisker...");
				if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
				if(kw.Create(newdlg,m_pDoc,false))
				{
					m_pDoc->m_KWExists = true;
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					buffer.Format("Box & Whisker across all row groups using selected (or all) Variables\r\n       Groups used: %s\r\n\r\n",((!m_pDoc->m_kw_patn_generated_groups)?"PATN generated groups":"a-priori groups"));		
					//buffer.Format("Box & Whisker across all row groups using selected (or all) variables\r\n\r\n");		
					m_pDoc->m_log += "+ Box & Whisker succeeded " + sst + " \r\n";
				}
				else
				{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_errlog += "- Box & Whisker Statistics failed " + sst + " \r\n"; 
				}
				if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
			}
		}
		long step,progress;

		t2 = clock();
		if(m_params.pcc)
		{
		//				s = ir;	ir.Format("%s -EP ",s);
		}
		if(m_params.pcc && m_pDoc->m_aRowXCoords.GetSize() && 
			m_pDoc->m_aRowXCoords[0] != m_pDoc->GetMissingValue() && 
			(m_pDoc->m_Ord3d == 3 || m_pDoc->m_Ord3d == 2)){

			for(i=0;i<m_pDoc->m_Vectors.GetSize();i++){
				m_pDoc->m_Vectors[i]->SetAt(0,ANALYSIS_VALUE);
			}
			m_pDoc->m_MCAOExists = false;
			if(m_pDoc->m_OrdChange){
				//report error
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("PCC cannot be performed without all object ordination coordinates current");
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- PCC failed " + sst + " \r\n    no ordination coordinates\r\n";
				m_errlog += "- PCC failed " + sst + " \r\n";
			}else{				
				long pccfailed = 0;
				//do pcc
				//for loop of selected items
if(!((CPatnApp*)AfxGetApp())->silent_mode)				newdlg = new CnewDialog;
else newdlg = NULL;
				if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - PCC...");
				if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
				step = 100 / m_params.pcc_indexes.GetSize();
				progress = 0;
				for(i=0;i<m_params.pcc_indexes.GetSize();i++){
					progress ++;
					if(progress >= step){
						progress = 0;
if(!((CPatnApp*)AfxGetApp())->silent_mode)						newdlg->inc();
					}					
					if(!(cpcc.OnCreate(m_params.pcc_indexes[i]))){
						pccfailed++;
					}
					m_pDoc->m_MCAOExists = false;					
				}
				if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
				if(pccfailed == m_params.pcc_indexes.GetSize()){
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- PCC failed " + sst + " \r\n      Table has no columns\r\n";
					m_errlog += "- PCC failed " + sst + " \r\n";
				}else if(!pccfailed){
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ PCC succeeded " + sst + " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					buffer.Format("() PCC: %d of %d columns success " + sst + " \r\n",
						m_params.pcc_indexes.GetSize()-pccfailed,m_params.pcc_indexes.GetSize());
					m_pDoc->m_log += buffer;
				}
			}
		}

		t2 = clock();
		if(m_params.mcao){
			//s = ir;	ir.Format("%s -EM %d %d ",s,m_params.mcao_iterations,m_params.mcao_seed);
			//test for pcc data...
			for(i=0;i<m_pDoc->GetNoOfColumns();i++){
				if(m_pDoc->m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
					break;
				}
			}
			if(i == m_pDoc->GetNoOfColumns()){
				//report error
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- MCAO failed " + sst + " \r\n    no PCC\r\n     " + m_pDoc->m_logerror + "\r\n";
				m_errlog += "- MCAO failed " + sst + " \r\n";
			}else{
				//do mcao
			//	newdlg = new CnewDialog(NULL);
			//	newdlg->Create("Progress - (MCAO)");
			//	newdlg->ShowWindow(SW_SHOWNORMAL);

				//progress dlg for mcao
				CProgressDlgThread * dlgthread;
				//dlgthread = new CProgressDlgThread;
				//dlgthread->m_bAutoDelete = true;
				//dlgthread->hnd = this->GetSafeHwnd();
				//dlgthread->CreateThread();
				//dlgthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
				//dlgthread->ResumeThread();

				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->mhnd = this->m_parent;
				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));



				if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready);
				long pp = 0;
				//WaitMessage();
				if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd)) pp++;

				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Label.SetWindowText("Progress - (MCAO)");

				//WaitMessage();
				//long np1 = 0;
				//::WaitForSingleObject(dlgthread,20000);

				pp = 0;
				if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready) pp++;
				//while(!dlgthread->m_ready)
				//	np1 ++;

				

				//dlgthread->dlg->SendMessage(0);
                
				if(!((CPatnApp*)AfxGetApp())->silent_mode) mcao.m_dlg = dlgthread->dlg;
				mcao.m_Seed = m_params.mcao_seed;
				mcao.m_Random = true;
				mcao.m_numPermutations = m_params.mcao_iterations;
				if(mcao.m_numPermutations <= 0 || !(mcao.Create(m_pDoc))){
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "- MCAO Failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
					m_errlog += "- MCAO Failed " + sst + " \r\n";
				}else{
					sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
					m_pDoc->m_log += "+ MCAO Succeeded " + sst + " \r\n";
				}
				m_pDoc->m_MCAOExists = true;

				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->ShowWindow(SW_HIDE);
				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->PostMessage(WM_QUIT,0,0);
				//delete newdlg;
				if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
				//AfxGetMainWnd()->BringWindowToTop();
				//AfxGetMainWnd()->SetFocus();

			}
		}		
	}

	//COMMAND LINE TEXT
	//m_pDoc->m_log += ir + "\r\n\r\n";

	if(m_errlog.GetLength()){
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(m_errlog,IDOK,0);
	}

	//refresh all
	POSITION pos = m_pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = m_pDoc->GetNextView(pos);
		pView->RedrawWindow();
	}

	m_pDoc->m_createANOSIMCols = m_params.anosim_cols;
	m_pDoc->m_ColumnAsim_iterations = m_params.anosim_cols_iterations;
	m_pDoc->m_createANOSIMRows = m_params.anosim_rows;
	m_pDoc->m_RowAsim_iterations = m_params.anosim_rows_iterations;
	m_pDoc->m_ColumnAsim_seed = m_params.anosim_cols_seed;
	m_pDoc->m_RowAsim_seed = m_params.anosim_rows_seed;
	m_pDoc->m_ColumnAsim_pairs = m_params.anosim_cols_pairs;
	m_pDoc->m_RowAsim_pairs = m_params.anosim_rows_pairs;
	m_pDoc->m_RowAsim_patn_groups = m_params.anosim_patn_generated_groups;

	m_pDoc->m_createKW = m_params.kw;
	m_pDoc->m_kw_patn_generated_groups = m_params.kw_patn_generated_groups;
	m_pDoc->m_createMCAO = m_params.mcao;
	m_pDoc->m_MCAOIterations = m_params.mcao_iterations;
	m_pDoc->m_MCAOSeed = m_params.mcao_seed;
	m_pDoc->m_createPCC = m_params.pcc;	

	m_pDoc->m_PccVectors.SetSize(m_params.pcc_indexes.GetSize());
	for(i=0;i<m_params.pcc_indexes.GetSize();i++){
		m_pDoc->m_PccVectors[i] = m_params.pcc_indexes[i];
	}

	//this->ShowWindow(SW_SHOW);
	//EnableWindow(true);

	this->EndWaitCursor();
	if(!((CPatnApp*)AfxGetApp())->silent_mode) SetForegroundWindow();
}

void CEvaluation::init()
{
	long i;

	//setup m_params defaults
	m_params.anosim_cols = (m_pDoc->m_createANOSIMCols)?true:false;
	m_params.anosim_cols_iterations = m_pDoc->m_ColumnAsim_iterations;
	m_params.anosim_rows = (m_pDoc->m_createANOSIMRows)?true:false;
	m_params.anosim_rows_iterations = m_pDoc->m_RowAsim_iterations;
	m_params.anosim_cols_seed = m_pDoc->m_ColumnAsim_seed;
	m_params.anosim_rows_seed = m_pDoc->m_RowAsim_seed;
	m_params.anosim_cols_pairs = (m_pDoc->m_ColumnAsim_pairs)?true:false;
	m_params.anosim_rows_pairs = (m_pDoc->m_RowAsim_pairs)?true:false;
	m_params.anosim_rows_pairs = (m_pDoc->m_RowAsim_pairs)?true:false;
	m_params.anosim_patn_generated_groups = (m_pDoc->m_RowAsim_patn_groups)?true:false;

	/*
	// set col group values
	m_params.anosim_cols_groups.SetSize(m_pDoc->GetNoOfColumns());
	if(m_pDoc->GetNoOfColumns() != m_pDoc->m_AnosimColGroups.GetSize()){
		for(i=0;i<m_pDoc->GetNoOfColumns();i++)
			m_params.anosim_cols_groups[i] = m_pDoc->GetColumnGroups(i);
	}else{
		for(i=0;i<m_pDoc->m_AnosimColGroups.GetSize();i++)
			m_params.anosim_cols_groups[i] = m_pDoc->m_AnosimColGroups[i];
	}

	// set row group values
	m_params.anosim_rows_groups.SetSize(m_pDoc->GetNoOfRows());
	if(m_pDoc->GetNoOfRows() != m_pDoc->m_AnosimRowGroups.GetSize()){
		for(i=0;i<m_pDoc->GetNoOfRows();i++)
			m_params.anosim_rows_groups[i] = m_pDoc->GetRowGroups(i);
	}else{
		for(i=0;i<m_pDoc->m_AnosimRowGroups.GetSize();i++)
			m_params.anosim_rows_groups[i] = m_pDoc->m_AnosimRowGroups[i];
	}

	*/

	//init by user columns selection OTHERWISE intrinsics
	//if(m_pDoc->m_ColSelection != NULL && m_pDoc->m_ColSelection->GetCount() > 0){
	//	m_pDoc->m_KWAttributes.RemoveAll();
	//	m_pDoc->m_PccVectors.RemoveAll();
	//	i = m_pDoc->m_ColSelection->GetFirst();
	//	while(i >= 0){
	//		m_pDoc->m_KWAttributes.Add(i);
	//		m_pDoc->m_PccVectors.Add(i);
	//		i = m_pDoc->m_ColSelection->GetNext();
	//	}
	//}else 
	{
		//add extrinsics to pcc && kw
		//if(m_pDoc->m_PccVectors.GetSize() == 0)
		if(!(m_pDoc->GetIdxLeftExtrinsicColumn() < 0 || 
			m_pDoc->GetIdxLeftExtrinsicColumn() >=m_pDoc->GetNoOfColumns())){
		
			m_pDoc->m_PccVectors.RemoveAll();
			m_pDoc->m_KWAttributes.RemoveAll();
			for(i=m_pDoc->GetIdxLeftExtrinsicColumn();i<m_pDoc->GetNoOfColumns();i++){
				m_pDoc->m_PccVectors.Add(i);
				m_pDoc->m_KWAttributes.Add(i);
			}
		}
		else if (m_pDoc->m_PccVectors.GetSize()==0)
		{
			m_pDoc->m_PccVectors.RemoveAll();
			m_pDoc->m_KWAttributes.RemoveAll();
			for(i=0;i<m_pDoc->GetNoOfColumns();i++){
				m_pDoc->m_PccVectors.Add(i);
				m_pDoc->m_KWAttributes.Add(i);
			}
		}

		
	}	

	m_params.pcc_indexes.SetSize(m_pDoc->m_PccVectors.GetSize());
	for(i=0;i<m_pDoc->m_PccVectors.GetSize();i++){
		m_params.pcc_indexes[i] = m_pDoc->m_PccVectors[i];
	}

	//m_params.kw = false;
	//m_params.mcao = false;
	//m_params.mcao_iterations = m_pDoc->m_MCAOIterations;
	//m_params.mcao_seed = m_pDoc->m_MCAOSeed;
	//m_params.pcc = false;
	m_params.kw = (m_pDoc->m_createKW)?true:false;
	m_params.kw_patn_generated_groups = (m_pDoc->m_kw_patn_generated_groups)?true:false;
	m_params.mcao = (m_pDoc->m_createMCAO)?true:false;
	m_params.mcao_iterations = m_pDoc->m_MCAOIterations;
	m_params.mcao_seed = m_pDoc->m_MCAOSeed;
	m_params.pcc = (m_pDoc->m_createPCC)?true:false;

}