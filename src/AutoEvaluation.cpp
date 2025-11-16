// AutoEvaluation.cpp: implementation of the CAutoEvaluation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "AutoEvaluation.h"

#include "mcao.h"
#include "newdialog.h"
#include "pcc.h"
#include "kruskalwalice.h"
#include "anosim.h"
                                           
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "progressdlgthread.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoEvaluation::CAutoEvaluation(CPatnDoc * pDoc)
{
	m_pDoc = pDoc;
}

CAutoEvaluation::~CAutoEvaluation()
{

}

void CAutoEvaluation::Run()
{	
	long i;
	CString show;

	CPcc cpcc(m_pDoc);
	CMCAO mcao;
	CnewDialog * newdlg = NULL;
	CKruskalWallace kw;
	CAnosim anosim(m_pDoc);
	CString ir;
	ir="";
	// set col group values
	CArray<long> anosim_cols_groups,anosim_rows_groups;

	anosim_cols_groups.SetSize(m_pDoc->GetNoOfColumns());
	if(m_pDoc->GetNoOfColumns() != m_pDoc->m_AnosimColGroups.GetSize()){
		for(i=0;i<m_pDoc->GetNoOfColumns();i++)
			anosim_cols_groups[i] = m_pDoc->GetColumnGroups(i);
	}else{
		for(i=0;i<m_pDoc->m_AnosimColGroups.GetSize();i++)
			anosim_cols_groups[i] = m_pDoc->m_AnosimColGroups[i];
	}

	// set row group values
	anosim_rows_groups.SetSize(m_pDoc->GetNoOfRows());
	if(m_pDoc->GetNoOfRows() != m_pDoc->m_AnosimRowGroups.GetSize()){
		for(i=0;i<m_pDoc->GetNoOfRows();i++)
			anosim_rows_groups[i] = m_pDoc->GetRowGroups(i);
	}else{
		for(i=0;i<m_pDoc->m_AnosimRowGroups.GetSize();i++)
			anosim_rows_groups[i] = m_pDoc->m_AnosimRowGroups[i];
	}

	//run analysis & close

	CString buffer;

	m_pDoc->m_log +=  CTime::GetCurrentTime().Format("%H:%M:%S, %B %d, %Y");
	m_pDoc->m_log += "\r\n";

	//if(m_params.anosim_rows){
	clock_t t2 = clock();
	CString sst;
	CString s;

	if(m_pDoc->m_AssocRowUpdate || m_pDoc->m_FuseRowUpdate){
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg = new CnewDialog;
		else newdlg = NULL;
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Evaluation Progress - Anosim on Rows...");
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
		m_pDoc->m_ANOSIMRowsExists = false;
		if(m_pDoc->m_RowAsim_pairs){
			//s = ir;	ir.Format("%s -EAR %d %d 0 ",s,m_pDoc->m_RowAsim_iterations,m_pDoc->m_RowAsim_seed);
			//do new anosim
			if(anosim.AnosimPairs(false,newdlg,
				&anosim_rows_groups,
				m_pDoc->m_RowAsim_iterations,
				m_pDoc->m_RowAsim_seed,true,m_pDoc->m_RowAsim_patn_groups)){
				m_pDoc->m_ANOSIMRowsExists = true;
				m_pDoc->m_RowAsim_pairs = true;
				for(i=0;i<anosim_rows_groups.GetSize();i++)
					m_pDoc->m_AnosimRowGroups.Add(anosim_rows_groups[i]);
				//buffer.Format("ANOSIM on row group pairs\r\n\tSeed: %d\r\n\tNumber of Iterations: %d\r\n",
				//	m_pDoc->m_RowAsim_seed,m_pDoc->m_RowAsim_iterations);
				//buffer += "\r\n";
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ Row ANOSIM succeeded " + sst + " \r\n";

				//show += "Row ANOSIM\r\n";
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- Row ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
				m_errlog += "- Row ANOSIM failed " + sst + " \r\n";
			}
		}else{
			m_pDoc->m_RowAsim_pairs = false;
			//s = ir;	ir.Format("%s -EAR %d %d 1 ",s,m_pDoc->m_RowAsim_iterations,m_pDoc->m_RowAsim_seed);
			if(anosim.AnosimRows(newdlg,m_pDoc->m_RowAsim_iterations,
				m_pDoc->m_RowAsim_seed,true,m_pDoc->m_RowAsim_patn_groups)){
				m_pDoc->m_ANOSIMRowsExists = true;
				//buffer.Format("ANOSIM on rows\r\n\tSeed: %d\r\n\tNumber of Iterations: %d\r\n",
				//	m_pDoc->m_RowAsim_seed,m_pDoc->m_RowAsim_iterations);
				//buffer += "\r\n";
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ Row ANOSIM succeeded " + sst + " \r\n";

				//show += "Row ANOSIM\r\n";
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
         		m_pDoc->m_log += "- Row ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
				m_errlog += "- Row ANOSIM failed " + sst + " \r\n";
			}
		}
		if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
	}else if(m_pDoc->m_ANOSIMRowsExists){
		m_pDoc->m_log += "= Row ANOSIM unchanged\r\n";
	}

	t2 = clock();
	//if(m_params.anosim_cols){
	if(m_pDoc->m_AssocColUpdate || m_pDoc->m_FuseColUpdate){
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg = new CnewDialog;
		else newdlg = NULL;
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - Anosim on Columns...");
		if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
		m_pDoc->m_ANOSIMColsExists = false;
	
		if(m_pDoc->m_ColumnAsim_pairs){
			//s = ir;	ir.Format("%s -EAC %d %d 1 ",s,m_pDoc->m_ColumnAsim_iterations,m_pDoc->m_ColumnAsim_seed);
			//do new anosim
			if(anosim.AnosimPairs(true,newdlg,
				&anosim_cols_groups,
				m_pDoc->m_ColumnAsim_iterations,
				m_pDoc->m_ColumnAsim_seed,true,true)){
				m_pDoc->m_ANOSIMColsExists = true;
				m_pDoc->m_ColumnAsim_pairs = true;
				for(i=0;i<anosim_cols_groups.GetSize();i++)
					m_pDoc->m_AnosimColGroups.Add(anosim_cols_groups[i]);
				//buffer.Format("ANOSIM on column group pairs\r\n\tSeed: %d\r\n\tNumber of Iterations: %d\r\n",
					//m_pDoc->m_ColumnAsim_seed,m_pDoc->m_RowAsim_iterations);
				//buffer += "\r\n";
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ Column ANOSIM succeeded " + sst + " \r\n";

				//show += "Column ANOSIM\r\n";
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
         	m_pDoc->m_log += "- Column ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
            m_errlog += "- Column ANOSIM failed " + sst + " \r\n";
         }
		}else{
			m_pDoc->m_ColumnAsim_pairs = false;
			//s = ir;	ir.Format("%s -EAC %d %d 0 ",s,m_pDoc->m_ColumnAsim_iterations,m_pDoc->m_ColumnAsim_seed);
			if(anosim.AnosimCols(newdlg,m_pDoc->m_ColumnAsim_iterations,
				m_pDoc->m_ColumnAsim_seed,true)){
				m_pDoc->m_ANOSIMColsExists = true;
				//buffer.Format("ANOSIM on columns\r\n\tSeed: %d\r\n\tNumber of Iterations: %d\r\n",
				//	m_pDoc->m_ColumnAsim_seed,m_pDoc->m_ColumnAsim_iterations);
				//buffer += "\r\n";
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ Column ANOSIM succeeded " + sst + " \r\n";
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
         	m_pDoc->m_log += "- Column ANOSIM failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
            m_errlog += "- Column ANOSIM failed " + sst + " \r\n";
         }

		}
		if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
	}else if(m_pDoc->m_ANOSIMColsExists){
		m_pDoc->m_log += "= Column ANOSIM unchanged\r\n";
	}

	//if(m_params.kw){
	t2 = clock();
	if((m_pDoc->m_AssocRowUpdate && m_pDoc->m_FuseRowUpdate) || m_pDoc->m_FuseRowUpdate){
		//if(!m_pDoc->m_KWAttributes.GetSize()){
		//	for(i=0;i<m_pDoc->GetNoOfColumns();i++)
		//		m_pDoc->m_KWAttributes.Add(i);
		//}
		//test if row groups exist
		if(m_pDoc->m_Rows.GetSize() <= 1){
			//report error
			//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Kruskal-Wallis cannot be performed if object groups are not defined");
			sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
			m_pDoc->m_log += "- Kruskal-Wallis Statistics failed " + sst + " \r\n    object groups are not defined\r\n";
			m_errlog += "- Kruskal-Wallis Statistics failed " + sst + " \r\n"; 
		}else{
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg = new CnewDialog;
			else newdlg = NULL;
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Evaluation Progress - Kruskal-Wallis...");
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
			kw.Create(newdlg,m_pDoc,true);
			m_pDoc->m_KWExists = true;
			delete newdlg;
			buffer.Format("Kruskal-Wallis across all row groups using selected (or all) Variables\r\n\r\n");		
			sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
			m_pDoc->m_log += "+ Kruskal-Wallis Statistics succeeded " + sst + " \r\n";
			//s = ir;	ir.Format("%s -EK ",s);
		}
	}else if(m_pDoc->m_KW.GetSize()){
		m_pDoc->m_log += "= Kruskal-Wallis Statistics unchanged\r\n";
	}
	
	//if(m_params.pcc){
	t2 = clock();
	long pccnew = 0;
	long pccfailed = 0;
	long step,progress;
	if((m_pDoc->m_AssocRowUpdate && m_pDoc->m_OrdUpdate) || m_pDoc->m_OrdUpdate){
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
			pccnew = 0;
		}else{
			//do pcc
			//for loop of selected items
			//for(i=0;i<m_params.pcc_indexes.GetSize();i++){
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg = new CnewDialog;
			else newdlg = NULL;
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->Create("Progress - PCC...");
			if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->ShowWindow(SW_SHOWNORMAL);
			progress = 0;
			for(i=0;i<m_pDoc->m_PccVectors.GetSize();i++){
				if(m_pDoc->m_PccVectors[i] >= m_pDoc->GetNoOfColumns()){
					m_pDoc->m_PccVectors.RemoveAll();
					break;
				}
			}

			if(m_pDoc->m_PccVectors.GetSize() == 0){
				for(i=m_pDoc->GetIdxLeftExtrinsicColumn();i!=-1 && i<m_pDoc->GetNoOfColumns();i++){
					m_pDoc->m_PccVectors.Add(i);
				}
			}
			if(m_pDoc->m_PccVectors.GetSize() == 0){
				for(i=0;i<m_pDoc->GetNoOfColumns();i++)
					m_pDoc->m_PccVectors.Add(i);
			}
			step = 100 / m_pDoc->m_PccVectors.GetSize();
			for(i=0;i<m_pDoc->m_PccVectors.GetSize();i++){
				progress ++;
				if(progress == step){
					progress = 0;
					if(!((CPatnApp*)AfxGetApp())->silent_mode)newdlg->inc();
				}
				if(!(cpcc.OnCreate(m_pDoc->m_PccVectors[i]))){
					pccfailed++;
				}
				m_pDoc->m_MCAOExists = false;	
			}
			if(!((CPatnApp*)AfxGetApp())->silent_mode)delete newdlg;
			if(pccfailed == m_pDoc->m_PccVectors.GetSize()){
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- PCC failed " + sst + " \r\n      Table has no columns\r\n";
				m_errlog += "- PCC failed " + sst + " \r\n";
			}else if(!pccfailed){
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ PCC succeeded " + sst + " \r\n";
				pccnew = 1;
				//s = ir;	ir.Format("%s -EP ",s);
			}else{
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				buffer.Format("() PCC: %d of %d columns success " + sst + " \r\n",m_pDoc->GetNoOfColumns()-pccfailed,m_pDoc->GetNoOfColumns());
				m_pDoc->m_log += buffer;
				pccnew = 1;
			}
		}
	}else{
		for(i=0;i<m_pDoc->m_Vectors.GetSize();i++){
			if(m_pDoc->m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
				break;
			}
		}
		if(i != m_pDoc->GetNoOfColumns()){
			m_pDoc->m_log += "= PCC unchanged\r\n";
			pccnew = 2;
		}else{
      		m_pDoc->m_log += "? PCC does not exist\r\n";
			pccnew = 2;
		}
	}

	//if(m_params.mcao){
	t2 = clock();
	if(pccnew == 1 && ((m_pDoc->m_AssocRowUpdate && m_pDoc->m_OrdUpdate) || m_pDoc->m_OrdUpdate)){
		//test for pcc data...
		for(i=0;i<m_pDoc->m_Vectors.GetSize();i++){
			if(m_pDoc->m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
				break;
			}
		}
		if(i == m_pDoc->GetNoOfColumns()){
			//report error
			//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("MCAO; PCC has not been run");
			sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
			m_pDoc->m_log += "- MCAO failed " + sst + " \r\n    no PCC\r\n     " + m_pDoc->m_logerror + "\r\n";
			m_errlog += "- MCAO failed " + sst + " \r\n";
		}else{
			//do mcao
			//newdlg = new CnewDialog(NULL);
			//newdlg->Create("Progress - (MCAO)");
			//newdlg->ShowWindow(SW_SHOWNORMAL);
			//mcao.m_dlg = newdlg;
			//progress dlg for mcao
			CProgressDlgThread * dlgthread;
			//dlgthread = new CProgressDlgThread;
			//dlgthread->m_bAutoDelete = true;
			//dlgthread->hnd = AfxGetMainWnd()->GetSafeHwnd();
			//dlgthread->CreateThread(0);
			//dlgthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
			//dlgthread->ResumeThread();	

			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->mhnd= m_parent; //AfxGetMainWnd()->GetSafeHwnd();
			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));

			if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready);

			long pp = 0;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd)) pp ++;

			//::WaitForSingleObject(dlgthread,20000);


			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Label.SetWindowText("Progress - (MCAO)");

			//dlgthread->dlg->m_Label.SetWindowText("Progress - (MCAO)");
            
			mcao.m_dlg = dlgthread->dlg;
			mcao.m_Seed = m_pDoc->m_MCAOSeed;
			mcao.m_Random = true;
			mcao.m_numPermutations = m_pDoc->m_MCAOIterations;
			if(mcao.m_numPermutations <= 0 || !(mcao.Create(m_pDoc))){
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "- MCAO Failed " + sst + " \r\n     " + m_pDoc->m_logerror + "\r\n";
				m_errlog += "- MCAO Failed " + sst + " \r\n";
			}else{
				//buffer.Format("MCAO on Variables run with PCC\r\n\tSeed: %d\r\n\tNumber of Iterations: %d",
				//	m_pDoc->m_MCAOSeed,m_pDoc->m_MCAOIterations);
				sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
				m_pDoc->m_log += "+ MCAO Succeeded " + sst + " \r\n";
			}
			m_pDoc->m_MCAOExists = true;
			//delete newdlg;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
			//AfxGetMainWnd()->BringWindowToTop();
		//	AfxGetMainWnd()->SetFocus();
			//s = ir;	ir.Format("%s -EM %d %d ",s,m_pDoc->m_MCAOIterations,m_pDoc->m_MCAOSeed);
		}
	}else if((m_pDoc->m_MCAOExists && pccnew == 2) || pccnew == 2){
		m_pDoc->m_log += "= MCAO unchanged\r\n";
	}else{
		sst.Format(" - Time taken %.2fs",(double)(clock()-t2)/CLOCKS_PER_SEC);
		m_pDoc->m_log += "- MCAO failed " + sst + " \r\n    no PCC\r\n";
		m_errlog += "- MCAO failed " + sst + " \r\n";
	}

	//COMMAND LINE TEXT
	//m_pDoc->m_log += ir + "\r\n\r\n";

	//if(show.GetLength()){
	//	show.Insert(0,"Evaluations successfully completed\r\n\r\n");
	//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(show);
	//}

	//refresh all
	POSITION pos = m_pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = m_pDoc->GetNextView(pos);
		pView->RedrawWindow();
	}
}

CString CAutoEvaluation::GetRecipeString()
{
	CString show;
	CString buffer;

	show += "\r\nAttempted Evaluations - \r\n";
	
	//if(m_params.anosim_rows){
	if(m_pDoc->m_RowAsim_pairs){
		//do new anosim
		buffer.Format("ANOSIM on row group pairs\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n       Groups used: %s\r\n",
			m_pDoc->m_RowAsim_seed,m_pDoc->m_RowAsim_iterations,((!m_pDoc->m_RowAsim_patn_groups)?"PATN generated groups":"a-priori groups"));
		//buffer += "\r\n";
		show += "   " + buffer;
	}else{
		buffer.Format("ANOSIM on rows\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n       Groups used: %s\r\n",
			m_pDoc->m_RowAsim_seed,m_pDoc->m_RowAsim_iterations,((!m_pDoc->m_RowAsim_patn_groups)?"PATN generated groups":"a-priori groups"));
		//buffer += "\r\n";
		show += "   " + buffer;
	}
	
	//if(m_params.anosim_cols){
	if(m_pDoc->m_ColumnAsim_pairs){
		//do new anosim
		buffer.Format("ANOSIM on column group pairs\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n",
			m_pDoc->m_ColumnAsim_seed,m_pDoc->m_ColumnAsim_iterations);
		//buffer += "\r\n";
		show += "   " + buffer;
	}else{
		buffer.Format("ANOSIM on columns\r\n       Seed: %d\r\n       Number of Iterations: %d\r\n",
			m_pDoc->m_ColumnAsim_seed,m_pDoc->m_ColumnAsim_iterations);
		//buffer += "\r\n";
		show += "   " + buffer;
	}

	buffer.Format("Box & Whisker across all row groups using selected (or all) Variables\r\n       Groups used: %s\r\n",((!m_pDoc->m_kw_patn_generated_groups)?"PATN generated groups":"a-priori groups"));		
	show += "   " + buffer;
	
	buffer.Format("PCC with selected Variables\r\n");
	show += "   " + buffer;

	buffer.Format("MCAO on Variables run with PCC\r\n       Seed: %d\r\n       Number of Iterations: %d",
		m_pDoc->m_MCAOSeed,m_pDoc->m_MCAOIterations);
	show += "   " + buffer;
			
	return show;
}