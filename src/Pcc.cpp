// Pcc.cpp: implementation of the CPcc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "PatnDoc.h"
#include "Pcc.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "3dxview.h"

CPcc::CPcc(CPatnDoc *pDoc)
{
	m_pDoc = pDoc;	
	CView * pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL){
		pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(C3DXView))){
			pView->GetParentFrame()->PostMessage(WM_CLOSE);
			break;
		}		
	}
}

CPcc::~CPcc()
{

}


long CPcc::OnCreate(long extrinsic) //make use of it here
{
	
	long i, n;
	double tY_Y;
	double tb_tX_Y;
	double sse, ssto, rsquared;
	//double ssr; //--used for checking
	matrix X, *tX_X;
	vector Y, *tX_Y, *b;
	vector ones;
	dbllarray* inverse;

	//make sure association has not changed
	if(m_pDoc->m_OrdChange) return 0;

	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	//if(m_pDoc->m_Ord3d == 3) m_NumCoords = 3;
	//else m_NumCoords = 2;
	m_NumCoords = m_pDoc->m_Ord3d;

	//determine non-missing value index list.
	long * indexes = new long[n];
	long nn = 0;
	for (i=0;i<n;i++)
	{
		if (m_pDoc->GetValueAt(extrinsic,i) != m_pDoc->GetMissingValue() ) 
		{
			indexes[nn++] = i;
		}
	}
	n = nn;


	// set up dependent variable vector
	Y.SetSize(n);
	for (i = 0; i < n; i++) {
		// extrinsic is in place of column index for extrinsic variable
		Y[i] = m_pDoc->GetValueAt(extrinsic, indexes[i]);
	}

	// set up vector of ones
	ones.SetSize(n);
	for (i=0; i<n; i++) {
		ones[i] = 1;
	}

	// set up independent variables matrix
	X.SetSize(n);
	for (i = 0; i < n; i++) {
		b = new vector;
		b->SetSize(4);
		b->SetAt(0,1);
		b->SetAt(1,m_pDoc->m_aRowXCoords[indexes[i]]);
		b->SetAt(2,m_pDoc->m_aRowYCoords[indexes[i]]);
		b->SetAt(3,m_pDoc->m_aRowZCoords[indexes[i]]);
		X.SetAt(i,b);
	}

	tX_X = tmat_mat_mult(&X,&X);
	tX_Y = tmat_vect_mult(&X,&Y);
	
	inverse = Gauss(tX_X);
	if(inverse != NULL)
	{
		m_Vector = mat_vect_mult(inverse, tX_Y);


		// Calculate correlation coefficient (r squared)
 		tY_Y = tvect_vect_mult_n(&Y, &Y); 
		tb_tX_Y = tvect_vect_mult_p(m_Vector, tX_Y);
		sse = tY_Y - tb_tX_Y;

		//get mean for ssto
		double mean = 0;
		for(i=0;i<n;i++){
			mean += Y.GetAt(i);
		}
		mean /= n;

		/*HERE TO CHECK BY OTHER METHOD
		//get error sum
		double errsum = 0;
		for(i=0;i<n;i++){
			errsum += pow(Y.GetAt(i) - 
				(m_Vector->GetAt(0) + 
				m_Vector->GetAt(1)*X[i]->GetAt(1) + 
				m_Vector->GetAt(2)*X[i]->GetAt(2) + 
				m_Vector->GetAt(3)*X[i]->GetAt(3)),2);
		}
		sse = errsum;
		//get ssr 
		ssr = 0;
		for(i=0;i<n;i++){
			ssr += pow((m_Vector->GetAt(0) +
				m_Vector->GetAt(1)*X[i]->GetAt(1) + 
				m_Vector->GetAt(2)*X[i]->GetAt(2) + 
				m_Vector->GetAt(3)*X[i]->GetAt(3)) - 
				mean,2);
		}

		END OF CHECKING CODE*/


		ssto = tY_Y - n*(pow(mean,2));

		//ALSO USED FOR CHECKING
		//ssr = ssto - sse;

		rsquared = 1 - (sse / ssto);
	}
	else
	{
		rsquared = m_pDoc->GetMissingValue();
	}

	for(i=0;i<tX_X->GetSize();i++)
		delete tX_X->GetAt(i);
	for(i=0; i<inverse->GetSize(); i++)
		delete inverse->GetAt(i);
	delete inverse;
	delete tX_X;
	delete tX_Y;
	for(i=0;i<X.GetSize();i++)
		delete X[i];

	m_Vector->Add(rsquared);
	m_Vector->RemoveAt(0); //remove "B" value

	//scale vector coords for radius == 1
	double tmp;
	double theta,phi;
	long d1,d2,d3; //direction of initial vectors
	if(m_pDoc->m_Ord3d == 3){
		//tmp code to reduce magnitude of pcc vector
	//	mag = sqrt(pow(m_Vector->GetAt(0),2) + pow(m_Vector->GetAt(1),2) + pow(m_Vector->GetAt(2),2));
	//	while (mag > 0.5 || mag < -0.5) { 
	//		m_Vector->SetAt(0, m_Vector->GetAt(0)/2);
	//		m_Vector->SetAt(1, m_Vector->GetAt(1)/2);
	//		m_Vector->SetAt(2, m_Vector->GetAt(2)/2);
	//		mag = sqrt(pow(m_Vector->GetAt(0),2) + pow(m_Vector->GetAt(1),2) + pow(m_Vector->GetAt(2),2));
	//	}

		//get initial direction.
		if(m_Vector->GetAt(0) >= 0) d1 = 1;
		else d1 = 0;
		if(m_Vector->GetAt(1) >= 0) d2 = 1;
		else d2 = 0;
		if(m_Vector->GetAt(2) >= 0) d3 = 1;
		else d3 = 0;

		//do conversion to radius 0.5
		theta = atan(sqrt(pow(m_Vector->GetAt(0),2) + pow(m_Vector->GetAt(1),2))/m_Vector->GetAt(2));
		phi = atan(m_Vector->GetAt(1) / m_Vector->GetAt(0));
		m_Vector->SetAt(0,sin(theta)*cos(phi));
		m_Vector->SetAt(1,sin(theta)*sin(phi));
		m_Vector->SetAt(2,cos(theta)); 

		//reset directions (magnitudes will be correct)
		tmp = m_Vector->GetAt(0);
		if((d1 && tmp < 0) || (!d1 && tmp >= 0)) 
			m_Vector->SetAt(0,tmp * -1);
		tmp = m_Vector->GetAt(1);
		if((d2 && tmp < 0) || (!d2 && tmp >= 0)) 
			m_Vector->SetAt(1,tmp * -1);
		tmp = m_Vector->GetAt(2);
		if((d3 && tmp < 0) || (!d3 && tmp >= 0))
			m_Vector->SetAt(2,tmp * -1);

	}else if(m_pDoc->m_Ord3d == 2){
		//tmp code to reduce magnitude of pcc vector
//		mag = sqrt(pow(m_Vector->GetAt(0),2) + pow(m_Vector->GetAt(1),2));
//		while (mag > 0.5 || mag < -0.5) { 
//			m_Vector->SetAt(0, m_Vector->GetAt(0)/2);
//			m_Vector->SetAt(1, m_Vector->GetAt(1)/2);
//			mag = sqrt(pow(m_Vector->GetAt(0),2) + pow(m_Vector->GetAt(1),2));
//		}
		//get initial direction.
		if(m_Vector->GetAt(0) >= 0) d1 = 1;
		else d1 = 0;
		if(m_Vector->GetAt(1) >= 0) d2 = 1;
		else d2 = 0;
		
		//do conversion to radius 0.5
		theta = atan(m_Vector->GetAt(1) / m_Vector->GetAt(0));
		m_Vector->SetAt(0,cos(theta));
		m_Vector->SetAt(1,sin(theta));

		//reset directions (magnitudes will be correct)
		tmp = m_Vector->GetAt(0);
		if((d1 && tmp < 0) || (!d1 && tmp >= 0)) 
			m_Vector->SetAt(0,tmp * -1);
		tmp = m_Vector->GetAt(1);
		if((d2 && tmp < 0) || (!d2 && tmp >= 0)) 
			m_Vector->SetAt(1,tmp * -1);
	}else {
		//m_Vector->SetAt(0,1);		
	}


	delete m_pDoc->m_Vectors[extrinsic];
	m_pDoc->m_Vectors[extrinsic] = m_Vector; 

	delete indexes;
	return 1;
}


typedef CArray<double> doublex;
typedef CArray<doublex*> dbllarray;

dbllarray* CPcc::Gauss(dbllarray *A)
{
	dbllarray * inv_A = new dbllarray;
	dbllarray a,b;
	doublex *c;
	long i,j,k,n,z;
	double sum;
	CArray<long> order;

	//verify that A is square
	n = A->GetSize();
	if(A->GetAt(0)->GetSize() != n)
		return NULL;

	//init order
	for(i=0;i<n;i++){
		order.Add(i);
	}

	//check for zero in diagonal
	for(i=0;i<n;i++){

	if(A->GetAt(i)->GetAt(i) == 0 && i<n-1){
			//shift with another & record move
			for(j=i+1;j<n;j++){
				if(A->GetAt(i)->GetAt(j) != 0 &&
					A->GetAt(j)->GetAt(i) != 0){
					//swap them
					c = A->GetAt(i);
					A->SetAt(i,A->GetAt(j));
					A->SetAt(j,c);
					z = order[i];
					order[i] = order[j];
					order[j] = z;
					j = n;
				}
			}
		}
		if(A->GetAt(i)->GetAt(i) == 0 && i>0){
			//look for record in other direction
			for(j=i-1;j>=0;j--){
				if(A->GetAt(i)->GetAt(j) != 0 &&
					A->GetAt(j)->GetAt(i) != 0){
					//swap them
					c = A->GetAt(i);
					A->SetAt(i,A->GetAt(j));
					A->SetAt(j,c);
					z = order[i];
					order[i] = order[j];
					order[j] = z;
					j = -1;
				}
			}
		}
		if(A->GetAt(i)->GetAt(i) == 0)
			//if not fixed, error
			return NULL;
	}
	
	//setup
	b.SetSize(n);
	a.SetSize(n);
	inv_A->SetSize(n);
	for(i=0;i<n;i++){
		b[i] = new doublex;
		b[i]->SetSize(n);
		a[i] = new doublex;
		a[i]->SetSize(n);
		inv_A->SetAt(i, new doublex);
		inv_A->GetAt(i)->SetSize(n);
	}

	//init a 
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			a[i]->SetAt(j,A->GetAt(i)->GetAt(j));
		}
	}		
	
	//init b (identity row)
	for(z=0;z<n;z++){
		for(i=0;i<n;i++){
			if(z == i) b[z]->SetAt(i,1);
			else b[z]->SetAt(i,0);
		}
	}

	//forward elimination	
	for(k=1;k<n;k++){
		for(i=k;i<n;i++){
			for(j=k;j<n;j++){
				a[i]->SetAt(j,a[i]->GetAt(j) - a[i]->GetAt(k-1)/a[k-1]->GetAt(k-1)*a[k-1]->GetAt(j));
			}
			for(j=0;j<n;j++){
				b[j]->SetAt(i,b[j]->GetAt(i) - a[i]->GetAt(k-1)/a[k-1]->GetAt(k-1) * b[j]->GetAt(k-1));
			}
		}
	}
	
	//check for zero in diagonal
	for(i=0;i<n;i++){
		if(A->GetAt(i)->GetAt(i) == 0){
			//No handler yet, maybe later
			return NULL;
		}
	}

	for(z=0;z<n;z++){
		//back substitution
		inv_A->GetAt(n-1)->SetAt(z,b[z]->GetAt(n-1)/a[n-1]->GetAt(n-1));
		for(k=n-2;k>=0;k--){
			sum = 0;
			for(j=k+1;j<n;j++){
				sum += inv_A->GetAt(j)->GetAt(z) * a[k]->GetAt(j);
			}			
			inv_A->GetAt(k)->SetAt(z,(b[z]->GetAt(k)-sum)/a[k]->GetAt(k));
		}
	}
	
	for(i=0;i<n;i++){
		delete a[i];
		delete b[i];
	}


	//reorder rows in A, & cols in inv_A
	for(i=0;i<n;i++){
		if(order[i] != i){
			//find it
			for(j=i;j<n;j++){
				if(order[j] == i){
					//swap them
					z = order[i];
					order[i] = order[j];
					order[j] = z;
					c = A->GetAt(i);
					A->SetAt(i,A->GetAt(j));
					A->SetAt(j,c);
					j = n;
					for(k=0;k<n;k++){
						sum = inv_A->GetAt(k)->GetAt(i);
						inv_A->GetAt(k)->SetAt(i,inv_A->GetAt(k)->GetAt(j));
						inv_A->GetAt(k)->SetAt(j,sum);
					}
				}
			}
		}
	}

	return inv_A;
}		


/*matrix*  CPcc::tmat_mult(matrix *tA, matrix *A)
{
	long i, j, k;
	matrix* C = new matrix;
	vector* V;
	C->SetSize(m_pDoc->GetNoOfRows());

	for (i = 0; i < m_pDoc->GetNoOfRows(); i++) {
		for (j = 0; j < 4; j++) {
			V = new vector;
			V->SetSize(4);
			V->SetAt(j,0);
			for (k = 0; k < m_pDoc->GetNoOfRows(); k++) {
				//next line changed, check with old version
				// (the line you had here)
				// V->SetAt(j,V->GetAt(j) + A->GetAt(k)->GetAt(j) * tA->GetAt(i)->GetAt(k));
				// new line:
				V->SetAt(j,V->GetAt(j) + tA->GetAt(k)->GetAt(j) * A->GetAt(k)->GetAt(j));
			}
		}
		C->SetAt(i,V);
	}
	return C;
} */

matrix* CPcc::tmat_mat_mult(matrix *tA, matrix *A)
{
	long i, j, k;
	matrix* C = new matrix;
	vector* V;
	C->SetSize(m_NumCoords + 1);

	long nRows;
//	if ( m_pDoc->IsExtrinsicRows() )
//		nRows = m_pDoc->GetIdxTopExtrinsicRow();
//	else
//		nRows = m_pDoc->GetNoOfRows();

	nRows = tA->GetSize();
	for (i=0; i<(m_NumCoords + 1); i++) {
		V = new vector;
		V->SetSize(m_NumCoords + 1);
		for (j=0; j<(m_NumCoords + 1); j++) {
			V->SetAt(j,0);
			for (k=0; k <nRows; k++) {
				V->SetAt(j,V->GetAt(j) + tA->GetAt(k)->GetAt(i) * A->GetAt(k)->GetAt(j));
			}
		}
		C->SetAt(i,V);
	}
	return C;
}

/*vector* CPcc::tmatvect_mult(matrix *tA, vector *B)
{
	long i, j, k;
	vector *C = new vector;
	C->SetSize(m_pDoc->GetNoOfRows());

	for (i = 0; i < m_pDoc->GetNoOfRows(); i++) {
		C->SetAt(i,0);
		for (j = 0; j < 4; j++) {
			for (k = 0; k < m_pDoc->GetNoOfRows(); k++) {
				C->SetAt(i,C->GetAt(i) + tA->GetAt(k)->GetAt(j) * B->GetAt(k));
			}
		}
	}
	return C;
} */

vector* CPcc::tmat_vect_mult(matrix *tA, vector *B)
{
	long i, j;
	vector *C = new vector;
	C->SetSize(m_NumCoords + 1);

	long nRows;
//	if ( m_pDoc->IsExtrinsicRows() )
//		nRows = m_pDoc->GetIdxTopExtrinsicRow();
//	else
//		nRows = m_pDoc->GetNoOfRows();

	nRows = tA->GetSize();

	for (i=0; i<(m_NumCoords + 1); i++) {
		C->SetAt(i,0);
		for (j=0; j<nRows; j++) {
			C->SetAt(i,C->GetAt(i) + tA->GetAt(j)->GetAt(i) * B->GetAt(j));
		}
	}
	return C;
}

/* vector* CPcc::matvect_mult(matrix *A, vector *B)
{
	long i, j, k;
	vector *C = new vector;
	C->SetSize(m_pDoc->GetNoOfRows());

	for (i = 0; i < m_pDoc->GetNoOfRows(); i++) {
		C->SetAt(i,0);
		for (j = 0; j < 4; j++) {
			for (k = 0; k < m_pDoc-> GetNoOfRows(); k++) {
				//k swapped with j 
				C->SetAt(i,C->GetAt(i) + A->GetAt(k)->GetAt(j) * B->GetAt(k));
			}
		}
	}
	return C;
} */

vector* CPcc::mat_vect_mult(matrix *A, vector *B)
{
	long i, j;
	vector *C = new vector;
	C->SetSize(m_NumCoords + 1);

	for (i=0; i<(m_NumCoords + 1); i++) {
		C->SetAt(i,0);
		for (j=0; j<(m_NumCoords + 1); j++) {
			C->SetAt(i,C->GetAt(i) + A->GetAt(i)->GetAt(j) * B->GetAt(j));
		}
	}
	return C;
}


double CPcc::tvect_vect_mult_p(vector *tA, vector *B)
{
	long i;
	double C = 0;

	for (i=0; i<(m_NumCoords + 1); i++) {
		C += tA->GetAt(i) * B->GetAt(i);
	}

	return C;
}

double CPcc::tvect_vect_mult_n(vector *tA, vector *B)
{
	long i;
	double C = 0;

	long nRows;
//	if ( m_pDoc->IsExtrinsicRows() )
//		nRows = m_pDoc->GetIdxTopExtrinsicRow();
//	else
//		nRows = m_pDoc->GetNoOfRows();

	nRows = tA->GetSize();
	for (i = 0; i < nRows; i++) {
		C += tA->GetAt(i) * B->GetAt(i);
	}
	return C;
}


void CPcc::ExportPcc(void)
{
	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;
	CString strLabel,strBuffer;
	long n;

	strTitle = "Save PCC Data:";

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
		strPathName = ((CPatnApp*)AfxGetApp())->sm_data + " Evaluation PCC Results.csv";
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

	strBuffer.Format("Project Comment: %s\n",m_pDoc->m_TableName);strBuffer.Replace(","," ");
	f.WriteString(strBuffer);
	strBuffer.Format("Association Option: %s\n",m_pDoc->m_OrdAsso);
	f.WriteString(strBuffer);
	strBuffer.Format("Cut Threshold: %0.2f,Number of Random Starts: %ld,",m_pDoc->m_OrdValue,m_pDoc->m_AnalysisRecipe.rows.nOrdRandomStarts);
	f.WriteString(strBuffer);
	strBuffer.Format("Random Number Seed: %ld,Maximum Iterations: %ld\n",m_pDoc->m_AnalysisRecipe.rows.nOrdSeed,m_pDoc->m_AnalysisRecipe.rows.nOrdIterations);
	f.WriteString(strBuffer);
	strBuffer.Format("Creation Date/Time: %s\n",m_pDoc->m_OrdTime);
	f.WriteString(strBuffer);

	if(m_pDoc->m_Ord3d == 3)
		f.WriteString(" Variable,ID,X,Y,Z,r-squared\n");
	else if(m_pDoc->m_Ord3d == 2)
		f.WriteString(" Variable,ID,X,Y,r-squared\n");
	else
		f.WriteString(" Variable,ID,X,r-squared\n");
	
	for(n=0;n<m_pDoc->GetNoOfColumns();n++){
		if(m_pDoc->m_Vectors[n]->GetAt(0) != ANALYSIS_VALUE){
			m_pDoc->GetColumnLabel(n,strBuffer);
			//strLabel.Format("(%d) ",n);
			strLabel = "";
			strLabel += strBuffer;
			f.WriteString(strLabel);
			strLabel.Format(",%d",n+1);
			f.WriteString(strLabel);
			if(m_pDoc->m_Ord3d == 3){
				strLabel.Format(",%.3f,%.3f,%.3f,%.3f\n",
					m_pDoc->m_Vectors[n]->GetAt(0),m_pDoc->m_Vectors[n]->GetAt(1),
					m_pDoc->m_Vectors[n]->GetAt(2),m_pDoc->m_Vectors[n]->GetAt(3));
			}else if(m_pDoc->m_Ord3d == 2){
				strLabel.Format(",%.3f,%.3f,%.3f\n",
					m_pDoc->m_Vectors[n]->GetAt(0),m_pDoc->m_Vectors[n]->GetAt(1),
					m_pDoc->m_Vectors[n]->GetAt(2));
			}else
			{
				strLabel.Format(",%.3f,%.3f\n",
					m_pDoc->m_Vectors[n]->GetAt(0),m_pDoc->m_Vectors[n]->GetAt(1));
			}
			f.WriteString(strLabel);
		}
	}

	
		
	f.Close();
}

//pcc for MCAO, returns rsquared value
double CPcc::OnCreate2(Row* data) 
{
	long i, n;
	double tY_Y;
	double tb_tX_Y;
	double sse, ssto, rsquared;
	//double ssr; //--used for checking
	matrix X, *tX_X;
	vector Y, *tX_Y, *b;
	vector ones;
	dbllarray* inverse;

	//make sure association has not changed
	if(m_pDoc->m_OrdChange) return 0;

	if ( m_pDoc->IsExtrinsicRows() )
		n = m_pDoc->GetIdxTopExtrinsicRow();
	else
		n = m_pDoc->GetNoOfRows();

	//determine non-missing value index list.
	long * indexes = new long[n];
	long nn = 0;
	for (i=0;i<n;i++)
	{
		if (data->GetAt(i) != m_pDoc->GetMissingValue() ) 
		{
			indexes[nn++] = i;
		}
	}
	n = nn;


	//if(m_pDoc->m_Ord3d) m_NumCoords = 3;
	//else m_NumCoords = 2;
	m_NumCoords = m_pDoc->m_Ord3d;

	// set up dependent variable vector
	Y.SetSize(n);
	for (i = 0; i < n; i++) {
		// extrinsic is in place of column index for extrinsic variable
		Y[i] = data->GetAt(indexes[i]);
	}

	// set up vector of ones
	ones.SetSize(n);
	for (i=0; i<n; i++) {
		ones[i] = 1;
	}

	// set up independent variables matrix
	X.SetSize(n);
	for (i = 0; i < n; i++) {
		b = new vector;
		b->SetSize(4);
		b->SetAt(0,1);
		b->SetAt(1,m_pDoc->m_aRowXCoords[indexes[i]]);
		b->SetAt(2,m_pDoc->m_aRowYCoords[indexes[i]]);
		b->SetAt(3,m_pDoc->m_aRowZCoords[indexes[i]]);
		X.SetAt(i,b);
	}

	tX_X = tmat_mat_mult(&X,&X);
	tX_Y = tmat_vect_mult(&X,&Y);
	
	inverse = Gauss(tX_X);
	m_Vector = mat_vect_mult(inverse, tX_Y);

	// Calculate correlation coefficient (r squared)
 	tY_Y = tvect_vect_mult_n(&Y, &Y); 
	tb_tX_Y = tvect_vect_mult_p(m_Vector, tX_Y);
	sse = tY_Y - tb_tX_Y;

	//get mean for ssto
	double mean = 0;
	for(i=0;i<n;i++){
		mean += Y.GetAt(i);
	}
	mean /= n;

	ssto = tY_Y - n*(pow(mean,2));

	rsquared = 1 - (sse / ssto);

	for(i=0;i<tX_X->GetSize();i++)
		delete tX_X->GetAt(i);
	for(i=0; i<inverse->GetSize(); i++)
		delete inverse->GetAt(i);
	delete inverse;
	delete tX_X;
	delete tX_Y;
	for(i=0;i<X.GetSize();i++)
		delete X[i];

	delete m_Vector;
	delete indexes;	
	return rsquared;
}