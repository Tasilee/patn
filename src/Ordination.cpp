// Ordination.cpp: implementation of the COrdination class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "Ordination.h"
#include "Ssh.h"
#include "newdialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include <sys/timeb.h>
#include <time.h>

/////////////////////////////////////////////////////////////////

COrdination::COrdination(CPatnDoc *pDoc)
{
	m_pDoc = pDoc;                                        
}

COrdination::~COrdination()
{

}

BOOL COrdination::SSHColumns()
{
	return true;
}


#include "progressdlgthread.h"

BOOL COrdination::SSHRows(double cut,int ord3d)
{
	if(m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() <= 0){
		//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("SSH cannot be performed without an association matrix.",
		//	MB_OK | MB_ICONSTOP, 0);
		m_pDoc->m_logerror = "SSH cannot be performed without an association matrix";
		return false;
	}

	//CnewDialog * newdlg;
	//newdlg = new CnewDialog(NULL);
	//newdlg->Create("Progress - (SSH)");
	//newdlg->m_Progress.SetRange(0,105);
	//newdlg->ShowWindow(SW_SHOWNORMAL);

	CProgressDlgThread * dlgthread;
	//dlgthread = new CProgressDlgThread;
	//dlgthread->m_bAutoDelete = true;
	
	//dlgthread->hnd = AfxGetMainWnd()->GetSafeHwnd();
	//while(!dlgthread->CreateThread(0));
	//dlgthread->ResumeThread();
	//dlgthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);

//	struct __timeb64 timebuffer;
	//char *timeline;
	
	//FILE * fp = fopen("c:\\patn_debug_main.csv","w");
	
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"begin\n");

	if(!((CPatnApp*)AfxGetApp())->silent_mode) 
	{
		dlgthread->mhnd = m_parent;
		dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"AfxBeginThread - done\n");
	//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(" begun");
	}
	
	
	//long o = WaitForSingleObject(dlgthread,20000);
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"WaitForSingleObject %d - done\n",o);
    
	
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"dlgthread->m_ready == true, %d - done\n",ppp);
    
	CnewDialog * newdlg;
	long ppp = 0;	
	if(!((CPatnApp*)AfxGetApp())->silent_mode) 
	{
		while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd) || !dlgthread->m_ready) ppp++;
	}
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"::IsWindow(dlgthread->dlg->m_hWnd) == true, %d - done\n",ppp);

	//dlgthread->PostThreadMessage(-23,0,0);

	if(!((CPatnApp*)AfxGetApp())->silent_mode) newdlg = dlgthread->dlg;
	if(!((CPatnApp*)AfxGetApp())->silent_mode) newdlg->m_Cancel = false;

	//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("hoasidf");
	if(!((CPatnApp*)AfxGetApp())->silent_mode) newdlg->m_Label.SetWindowText("Progress - (SSH)");
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"newdlg->m_Label.SetWindowText - done\n");

	CString str;
	
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"processing\n");
	long i,k =m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize();
	long n;
	long p = (long) (k * (k-1) / 2.0);
	long j;

	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	//reset pcc
	for(i=0;i<m_pDoc->m_Vectors.GetSize();i++)
		for(j=0;j<m_pDoc->m_Vectors[i]->GetSize();j++)
			m_pDoc->m_Vectors[i]->SetAt(j,ANALYSIS_VALUE);

	//setup ssh inputs
	double * din,*dout,*dest;
	ldblaray * conf,*grad,*gral,*saveconf;
	long * indx;
	long size;

	if(ord3d == 3) size = 3;
	else if(ord3d == 2) size = 2;
	else size = 1 ;

	//create the above
	//conf.SetSize(n);
	//saveconf.SetSize(n);
	//grad.SetSize(n);
	//gral.SetSize(n);
	conf = (ldblaray*) malloc(sizeof(ldblaray)*n*4);
	grad = conf+n;
	gral = conf+2*n;
	saveconf = conf+3*n;
	double * pp = (double*) malloc(sizeof(double)*(5*n*size+4*p));
	if(pp == NULL){//insufficient memory
		free(conf);
		m_pDoc->m_logerror += "\r\nInsufficient system memory for SSH.\r\n";
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
		return false;
	}
	//free(pp);
	//double * pp = new double [4*n*size+4*p];//(double*) malloc(sizeof(double)*(4*n*size+3*p));
	for(i=0;i<n;i++){
		conf[i] = pp + size*i;
		grad[i] = pp + size*i+n*size;
		gral[i] = pp + size*i+2*n*size;
		saveconf[i] = pp + size*i+3*n*size;
	}

//	newdlg->inc();

//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("aosidf333");

	//fill din
	p = 0;
	for(i=0;i<m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize();i++){
		for(k=0;k<m_pDoc->m_AssocMatrixRows.m_aMatrix[i]->GetSize();k++){
			p++;
		}
	}
	din = pp+4*n*size;
	dout = pp+4*n*size + p;
	dest = pp+4*n*size + 2*p;
	indx = (long*) malloc(sizeof(long)*p);
	
	for(i=0;i<p;i++){
		dout[i] = 0;
		dest[i] = 0;
		indx[i] = 0;
	}
	p = 0;
	for(i=0;i<m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize();i++){
		for(k=0;k<m_pDoc->m_AssocMatrixRows.m_aMatrix[i]->GetSize();k++){
			din[p] = m_pDoc->m_AssocMatrixRows.m_aMatrix[i]->GetAt(k);
			p++;
		}
	}
	if(!((CPatnApp*)AfxGetApp())->silent_mode) newdlg->inc();
	if(!((CPatnApp*)AfxGetApp())->silent_mode) newdlg->m_Label.SetWindowText("Progress - SSH");
	bool ret = true;
	if(!((CPatnApp*)AfxGetApp())->silent_mode && dlgthread->dlg->m_Cancel)
	{
		m_pDoc->m_logerror = "SSH cancelled by user.";
		ret = false;
	}
	else
	{
		double stress;
		//run ssh
		CSsh ssh;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) ssh.m_dlg = newdlg;

		//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"Initiation complete\n");

		//ssh.Ssh1(&din,&dout,&dest,&indx,&conf,&grad,&gral,size,n,m_pDoc,cut,m_pDoc->m_OrdIterations,&stress,&saveconf,m_pDoc->m_OrdRandomStarts,m_pDoc->m_OrdSeed);
		if(ssh.Ssh1(din,dout,dest,indx,conf,grad,gral,size,n,
			m_pDoc,cut,m_pDoc->m_OrdIterations,&stress,saveconf,
			m_pDoc->m_OrdRandomStarts,m_pDoc->m_OrdSeed,p))
		{
			//rescale for radius 1
			RescaleRadius(conf, n, ord3d);

			//process ssh outputs
			for(i=0;i<n;i++){
				m_pDoc->m_aRowXCoords.SetAt(i,conf[i][0]);
				if(ord3d >= 2) m_pDoc->m_aRowYCoords.SetAt(i,conf[i][1]);
				else m_pDoc->m_aRowYCoords.SetAt(i,0);
				if(ord3d >= 3) m_pDoc->m_aRowZCoords.SetAt(i,conf[i][2]);
				else m_pDoc->m_aRowZCoords.SetAt(i,0);
			}		
			
			CTime time = CTime::GetCurrentTime();
			m_pDoc->m_OrdTime = time.Format("%H:%M:%S, %B %d, %Y");
			m_pDoc->m_OrdValue = cut;
			m_pDoc->m_OrdAsso = m_pDoc->m_AssocMatrixRows.m_strMeasure;
			m_pDoc->m_OrdStress = stress;
		}
		else
		{
			m_pDoc->m_logerror = "SSH cancelled by user.";
			ret = false;
		}
	}

	i=0;
	free(indx);	
	i = sizeof(pp);
	free((double *)pp);
	free(conf);

//	newdlg->PostMessage(WM_CLOSE,0,0L);
//	delete newdlg;
	if(!((CPatnApp*)AfxGetApp())->silent_mode) i = dlgthread->PostThreadMessage(WM_QUIT,0,0);
	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"dlgthread->PostThreadMessage(WM_QUIT), %d\n",i);

	//_ftime64( &timebuffer ); timeline = _ctime64( & ( timebuffer.time ) );fprintf(fp, "%.19s.%hu %s,", timeline, timebuffer.millitm, &timeline[20] );fprintf(fp,"End %d\n\n",ret);

//	fclose(fp);

	return ret;
}

void COrdination::RescaleRadius(ldblaray * conf, long n, long dim) {
//scale vector coords for radius == 1
	double tmp;
	double theta,phi;
	long d1,d2,d3; //direction of initial vectors
	long i;
	

	//get mean x, y, z
		double mx = 0, my = 0, mz = 0;
		for(i=0;i<n;i++) {
			mx += conf[n][0];
			my += conf[n][1];
			mz += conf[n][2];
		}
		mx /= (double)n;
		my /= (double)n;
		mz /= (double)n;

		//get max radius
		double r, maxr = 0;
		for(i=0;i<n;i++){
			r = sqrt((mx - conf[n][0]) * (mx - conf[n][0]) + (my - conf[n][1]) * (my - conf[n][1]) + (mz - conf[n][2]) * (mz - conf[n][2]));
			if(r > maxr) maxr = r;
		}

		for(i=0;i<n;i++) {
			double x = conf[n][0];
			double y = conf[n][1];
			double z = conf[n][2];
		
			//get initial direction.
			if(x >= 0) d1 = 1;
			else d1 = 0;
			if(y >= 0) d2 = 1;
			else d2 = 0;
			if(z >= 0) d3 = 1;
			else d3 = 0;

			//do conversion to radius 0.5
			r = sqrt((mx - conf[n][0]) * (mx - conf[n][0]) + (my - conf[n][1]) * (my - conf[n][1]) + (mz - conf[n][2]) * (mz - conf[n][2]));
			theta = atan(sqrt(pow(x,2) + pow(y,2))/z);
			phi = atan(y / x);
			x = sin(theta)*cos(phi) * r / maxr;
			y = sin(theta)*sin(phi) * r / maxr;
			z = cos(theta) * r / maxr; 

			//reset directions (magnitudes will be correct)
			tmp = x;
			if((d1 && tmp < 0) || (!d1 && tmp >= 0)) 
				mx = tmp * -1;
			tmp = y;
			if((d2 && tmp < 0) || (!d2 && tmp >= 0)) 
				y = tmp * -1;
			tmp = z;
			if((d3 && tmp < 0) || (!d3 && tmp >= 0))
				z = tmp * -1;

			conf[n][0] = x;
			conf[n][1] = y;
			conf[n][2] = z;
		}
}


void COrdination::ExportRowOrdination(void)
{
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	int n;

	strTitle = "Save Ordination Coordinates";
if(!((CPatnApp*)AfxGetApp())->silent_mode)
	{
	
	// Get path for the new file
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
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Analysis Ordination Coordinates.csv";
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

	strBuffer.Format("Project Comment: %s\n",m_pDoc->m_TableName);
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",m_pDoc->m_OrdAsso);
	f.WriteString(strBuffer);
	strBuffer.Format("Cut Threshold: %0.2f,Number of Random Starts: %ld,",m_pDoc->m_OrdValue,m_pDoc->m_AnalysisRecipe.rows.nOrdRandomStarts);
	f.WriteString(strBuffer);
	strBuffer.Format("Random Number Seed: %ld,Maximum Iterations: %ld\n",m_pDoc->m_AnalysisRecipe.rows.nOrdSeed,m_pDoc->m_AnalysisRecipe.rows.nOrdIterations);
	f.WriteString(strBuffer);
	strBuffer.Format("Creation Date/Time: %s\n",m_pDoc->m_OrdTime);
	f.WriteString(strBuffer);

	if(m_pDoc->m_Ord3d == 3) f.WriteString("Label,ID,X,Y,Z\n");
	else if(m_pDoc->m_Ord3d == 2) f.WriteString("Label,ID,X,Y\n");
	else if(m_pDoc->m_Ord3d == 1) f.WriteString("Label,ID,X\n");

	int nRows;
	if ( m_pDoc->IsExtrinsicRows() )
		nRows = m_pDoc->GetIdxTopExtrinsicRow();
	else
		nRows = m_pDoc->GetNoOfRows();

	for(n=0;n<nRows;n++){
		m_pDoc->GetRowLabel(n,strBuffer);strBuffer.Replace(","," ");
		strLabel.Format("%s,%d",strBuffer,n+1);
		f.WriteString(strLabel);
		if(m_pDoc->m_Ord3d == 3){
			strLabel.Format(",%g,%g,%g",
				m_pDoc->m_aRowXCoords[n],m_pDoc->m_aRowYCoords[n],
				m_pDoc->m_aRowZCoords[n]);
		}else if(m_pDoc->m_Ord3d == 2){
			strLabel.Format(",%g,%g",
				m_pDoc->m_aRowXCoords[n],m_pDoc->m_aRowYCoords[n]);
		}else if(m_pDoc->m_Ord3d == 1){
			strLabel.Format(",%g",
				m_pDoc->m_aRowXCoords[n]);
		}
		strLabel += "\n";
		f.WriteString(strLabel);
	}

	
		
	f.Close();
}

