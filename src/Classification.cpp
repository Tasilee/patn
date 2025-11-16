// Classification.cpp
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "Classification.h"
#include "Dend.h"
#include <math.h>

#include "Association.h"
#include "associationdlg.h"
#include "fusiondlg.h"
#include "newdialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "progressdlgthread.h"

CClassification::CClassification(CPatnDoc* pDoc)
{
	ASSERT( pDoc->IsKindOf(RUNTIME_CLASS(CPatnDoc)) );
	m_pDoc = pDoc;
}

//for intermediate results
int file;
void WriteToSeedGroups(int count, long * seeds,double radius)
{
	FILE * fp = fopen("c:\\seedgroups.csv","a");
	if(fp)
	{
		fprintf(fp,"no.groups %d,radius %g,",count,radius);
		for(int i=0;i<count;i++)
		{
			fprintf(fp,"%d,",seeds[i]);
		}
		fprintf(fp,"\n");
		fclose(fp);
	}
}

long iteration_count = 0;
BOOL CClassification::Alloc(long nAssocMeasure, long nNoOfGroups)
{
	file = 0;

	double power = m_pDoc->m_AnalysisRecipe.rows.dPower;
	if(nAssocMeasure == 2)
	{
		CProgressDlgThread * dlgthread;

		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->mhnd = m_parent;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready);

		long pp = 0;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd) || !dlgthread->m_ready) pp++;

		//::WaitForSingleObject(dlgthread,20000);
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Progress.SetRange(0,122);
		
		long nCols,nRows;
		double missingvalue = m_pDoc->GetMissingValue();

		if ( m_pDoc->IsExtrinsicColumns() )
			nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
		else
			nCols = m_pDoc->GetNoOfColumns();
		if ( m_pDoc->IsExtrinsicRows() )
			nRows = m_pDoc->GetIdxTopExtrinsicRow();
		else
			nRows = m_pDoc->GetNoOfRows();

		double *col_min = new double[nCols];
		double *col_max = new double[nCols];
		double *col_range = new double[nCols];
		long * seedidx = new long[nNoOfGroups+1];
		long seedidxsize = 0;
		for(long i=0;i<nCols;i++)
		{
			col_min[i] = m_pDoc->GetColumnMinimum(i);
			col_max[i] = m_pDoc->GetColumnMaximum(i);
			col_range[i] = col_max[i] - col_min[i];
		}
		
		//CORRECT ALGORITHM - GOWERMETRIC ASSOCIATIONS
		//1. determine correct # of groups by varying radius
		double start_radius = 1;
		double radius = start_radius;
		double step = radius/2.0;

		long count = 0;
		seedidx[0] = 0;
		while(seedidxsize != nNoOfGroups && count < 30)
		{
			seedidxsize = 1;
			
			for(long i=1;i<nRows;i++)
			{
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seedidx(j))
					double dist = 0;
					long missing = 0;				
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
						if(v1 == missingvalue ||
							v2 == missingvalue)
							missing++;
						else
							dist += fabs(v1 - v2)/(double)col_range[k];
					}

					//add to seedidx if distance > radius
					if(nCols == missing)
					{
						//error
						missing--;
					}
					dist = dist / (double)(nCols-missing);
					if(dist < radius)
					{					
						break;
					}					
				}
				if(j == seedidxsize)				
				{
					//seedidx.Add(i);					
					seedidx[seedidxsize] = i;
					seedidxsize++;
				}

				if(seedidxsize > nNoOfGroups)
					break;
			}
			count++; //force a break
			
			//intermediate results write
			//WriteToSeedGroups(seedidxsize,seedidx,radius);

			if(seedidxsize == nNoOfGroups)
				continue;

			//PERFORM RECONCILIATION OF NUMBER OF GROUPS IF count >= 20
			if(count < 20)
			{
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else if(seedidxsize > nNoOfGroups)
				{
					radius += step;
				}
				step /= 2.0;
			}
			else
			{
				//loop while number of groups is < nNoOfGroups
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else
				{
					break;
				}			
			}

			CString str;
			str.Format("Allocation (Gower metric): seed generation iteration: %d",count);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) void * v = dlgthread->dlg;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread != NULL && dlgthread->dlg != NULL && dlgthread->dlg->m_Label.m_hWnd != NULL)
				{
					dlgthread->dlg->m_Label.SetWindowText(str);
					dlgthread->dlg->inc();
					dlgthread->dlg->RedrawWindow();
					if(dlgthread->dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "Rows: Allocation canceled by user.";
						dlgthread->PostThreadMessage(WM_QUIT,0,0);
						AfxGetMainWnd()->SetFocus();
						AfxGetMainWnd()->BringWindowToTop();
						//delete dlgthread;

						delete [] col_min;
						delete [] col_max;
						delete [] col_range;
						delete [] seedidx;

						return false;
					}
				}
			}
		}	

		double * seeds = new double[seedidxsize*nCols];
		double * seedgroup_nonmissingvalues = new double[seedidxsize*nCols];

		//2. allocate all objects to a group
		long * groups = new long[nRows];
		double * groups_dist = new double[nRows];
		long * groupsize = new long[seedidxsize];
		for(long i=0;i<seedidxsize;i++)
		{
			groupsize[i] = 0;
			for(long j=0;j<nCols;j++)
			{
				seeds[i*nCols + j] = missingvalue;
				seedgroup_nonmissingvalues[i*nCols+j] = 0;
			}
		}
		for(long i=0;i<nRows;i++)
			groups[i] = -1;

		long iteration = 0;
		long movement = 1;
		long min_movement = -1;
		long * min_groups = new long[nRows];
		double * min_dists = new double[nRows];
		while(movement && iteration < 100)
		{
			//write centroids
			/*if(iteration)
			{
				CString str;
				str.Format("c:\\centroids%d.csv",iteration);
				FILE * fp = fopen(str,"w");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						for(long jj=0;jj<nCols;jj++)
						{
							fprintf(fp,"%g,",seeds[ii*nCols+jj]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
				}

                str.Format("c:\\centroidgroups.csv");
				fp = fopen(str,"a");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						fprintf(fp,"%i,",groupsize[ii]);					
					}				
					fprintf(fp,"\n");
					fclose(fp);
				}
			}*/


			movement = 0;

			for(long i=0;i<nRows;i++)
			{
				//step 4. pop from current group if current group has > 1 member
				if(iteration)
				{
					if(groupsize[groups[i]] == 1)
						continue;

					groupsize[groups[i]]--;
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = seeds[groups[i]*nCols+k];
						if(v1 != missingvalue &&
							v2 != missingvalue)
						{
							seeds[groups[i]*nCols+k] = v2 - v1;
							
						}
						if(v1 != missingvalue) seedgroup_nonmissingvalues[groups[i]*nCols+k] --;
					}
				}
				
				double min_dist;
				long min_idx;
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seeds(j))
					double dist = 0;
					long missing = 0;
					if(iteration)
					{
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = seeds[j*nCols+k];
							if(v1 == missingvalue ||
								v2 == missingvalue)
							{
								missing++;
							}
							else
							{
								if(seedgroup_nonmissingvalues[j*nCols+k] > 0)
									v2 = v2 / seedgroup_nonmissingvalues[j*nCols+k];
								dist += fabs(v1 - v2)/(double)col_range[k];
							}
						}
						dist = dist/(double)(nCols-missing);
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
					else
					{
						//seeds present in indexed file					
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
							if(v1 == missingvalue ||
								v2 == missingvalue)
								missing++;
							else
								dist += fabs(v1 - v2)/(double)col_range[k];
						}
						dist = dist/(double)(nCols-missing);
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
				}		
				//add this group to group min_idx;
				if(groups[i] != min_idx)
				{
					movement++;				
				}
				groups[i] = min_idx;	
				groups_dist[i] = min_dist;
				groupsize[min_idx]++;
								
				for(long k=0;k<nCols;k++)
				{
					long idx = groups[i]*nCols+k;
					double v1 = seeds[idx];	
					if(m_pDoc->GetValueAt(k,i) != missingvalue)
					{
						if(v1 == missingvalue)
							seeds[idx] = m_pDoc->GetValueAt(k,i);
						else 
							seeds[idx] = seeds[idx] + m_pDoc->GetValueAt(k,i);
						seedgroup_nonmissingvalues[idx]++;
					}
				}
			}

			//3. calc group centroid	
			//already done!

			//4. pop each object from group (recalc centroid) and allocate again
			iteration++;

			CString str;
			str.Format("Allocation (Gower metric): iteration: %d, movement: %d",iteration,movement);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread != NULL && dlgthread->dlg != NULL && dlgthread->dlg->m_Label.m_hWnd != NULL)
				{
					dlgthread->dlg->m_Label.SetWindowText(str);
					dlgthread->dlg->inc();
					dlgthread->dlg->RedrawWindow();
					if(dlgthread->dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "Rows: Allocation canceled by user.";
						dlgthread->PostThreadMessage(WM_QUIT,0,0);
						AfxGetMainWnd()->BringWindowToTop();
						AfxGetMainWnd()->SetFocus();					

						/*delete [] col_min;
						delete [] col_max;
						delete [] col_range;
						delete [] seedidx;
						delete [] seeds;
						delete [] groups;
						delete [] groupsize;
						return false;*/
						break;
					}
				}

			}
			//backup min_movement
			if(min_movement == -1 || min_movement > movement)
			{	
				min_movement = movement;
				//copy groups to min_groups
				for(long p=0;p<nRows;p++)
				{
					min_groups[p] = groups[p];
					min_dists[p] = groups_dist[p];
				}
			}
		}

		//write-back row groups
		m_pDoc->m_loading = true;
		for(long i=0;i<nRows;i++)
		{		
			//m_pDoc->SetRowGroups(i,groups[i]);		
			m_pDoc->SetRowGroups(i,min_groups[i]+1,min_dists[i]);		
		}
		m_pDoc->m_loading = false;

		CTime time = CTime::GetCurrentTime();
		m_pDoc->m_FuseTypeRows = "Non-hierarchical Clustering";
		m_pDoc->m_FuseAssoRows = "Gower metric";	
		m_pDoc->m_FuseTimeRows = time.Format("%H:%M:%S, %B %d, %Y");

		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->BringWindowToTop();		

		delete [] col_min;
		delete [] col_max;
		delete [] col_range;
		delete [] seedidx;
		delete [] seeds;
		delete [] groups;
		delete [] groupsize;
		delete [] min_groups;
		delete [] min_dists;
		delete [] groups_dist;
		delete [] seedgroup_nonmissingvalues;
		return TRUE;
	}
	else if(nAssocMeasure == 1)
	{
		CProgressDlgThread * dlgthread;
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->mhnd = m_parent;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));		

		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready);

		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd));
		
		//::WaitForSingleObject(dlgthread,20000);

		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Progress.SetRange(0,122);

		long nCols,nRows;
		double missingvalue = m_pDoc->GetMissingValue();

		if ( m_pDoc->IsExtrinsicColumns() )
			nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
		else
			nCols = m_pDoc->GetNoOfColumns();
		if ( m_pDoc->IsExtrinsicRows() )
			nRows = m_pDoc->GetIdxTopExtrinsicRow();
		else
			nRows = m_pDoc->GetNoOfRows();

		double *col_min = new double[nCols];
		double *col_max = new double[nCols];
		double *col_range = new double[nCols];
		long * seedidx = new long[nNoOfGroups+1];
		long seedidxsize = 0;
		for(long i=0;i<nCols;i++)
		{
			col_min[i] = m_pDoc->GetColumnMinimum(i);
			col_max[i] = m_pDoc->GetColumnMaximum(i);
			col_range[i] = col_max[i] - col_min[i];
		}
		
		//CORRECT ALGORITHM - GOWERMETRIC ASSOCIATIONS
		//1. determine correct # of groups by varying radius
		double start_radius = 1;
		double radius = start_radius;
		double step = radius/2.0;
//		CArray<long> seedidx;

		long count = 0;
		seedidx[0] = 0;
		while(seedidxsize != nNoOfGroups && count < 30)
		{
			//seedidx.RemoveAll();
			//seedidx.Add(0); //first object is always seed of first group.
			seedidxsize = 1;
			
			for(long i=1;i<nRows;i++)
			{
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seedidx(j))
					double num = 0;
					double den = 0;
					double dist = 0;
					long missing = 0;				
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
						if(v1 == missingvalue || v2 == missingvalue)
						{
							missing++;
						}
						else
						{
							if(v1 < 0 || v2 < 0)
							{
								//err
								;
							}
							else
							{
								num += fabs(v1 - v2);
								den += (v1+v2);
							}
						}
					}

					//add to seedidx if distance > radius
					if(den == 0)
					{
						//err
						den = 1;
					}
					dist = num/(double)den;
					if(dist < radius)
					{					
						break;
					}
					//else if(dist < min_dist || min_dist == -1)
					//{
					//	min_dist = dist;
					//}
				}
				//if(j == seedidx.GetSize())				
				///{
				//	seedidx.Add(i);
				//	seed_min_span.Add(min_dist);
				//}

				if(j == seedidxsize)				
				{
					//seedidx.Add(i);					
					seedidx[seedidxsize] = i;
					seedidxsize++;
				}

				if(seedidxsize > nNoOfGroups)
					break;
			}
			count++; //force a break

			//intermediate results write
//			WriteToSeedGroups(seedidxsize,seedidx,radius);

			if(seedidxsize == nNoOfGroups)
				continue;

			//PERFORM RECONCILIATION OF NUMBER OF GROUPS IF count >= 20
			if(count < 20)
			{
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else if(seedidxsize > nNoOfGroups)
				{
					radius += step;
				}
				step /= 2.0;
			}
			else
			{
				//loop while number of groups is < nNoOfGroups
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else
				{
					break;
				}			
			}

			CString str;
			str.Format("Allocation (Bray-curtis): seed generation iteration: %d",count);		
			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Label.SetWindowText(str);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->inc();
			if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->RedrawWindow();
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread->dlg->m_Cancel)
				{
					m_pDoc->m_logerror = "Rows: Allocation canceled by user.";
					dlgthread->PostThreadMessage(WM_QUIT,0,0);
					AfxGetMainWnd()->BringWindowToTop();
					AfxGetMainWnd()->SetFocus();
					//delete dlgthread;

					delete [] col_min;
					delete [] col_max;
					delete [] col_range;
					delete [] seedidx;

					return false;
				}
			}
			
		}	

		//fuse groups if necessary by dissregarding the minimum spanning group (or one of them)
		//while(seedidx.GetSize() > nNoOfGroups)
		//{
			//long minidx = 1;
			//for(long i=2;i<seedidx.GetSize();i++)
			//{
			//	if(seed_min_span[i] < seed_min_span[minidx])
			//		minidx = i;
			//}
			//seed_min_span.RemoveAt(minidx);
			//seedidx.RemoveAt(minidx);
		//}	
				
		//CArray<dblarray*,dblarray*> seeds;
		//seeds.SetSize(seedidx.GetSize());
		//for(long i=0;i<seeds.GetSize();i++)
		//{
		//	seeds[i] = new dblarray;
		//	seeds[i]->SetSize(nCols);
		//	//for(long j=0;j<nCols;j++)
			//{
			//	seeds[i]->SetAt(j,m_pDoc->GetValueAt(j,seedidx[i]));
			//}
		//}/

		double * seeds = new double[seedidxsize*nCols];
		double * seedgroup_nonmissingvalues = new double[seedidxsize*nCols];

		//2. allocate all objects to a group
		//CArray<long> groups,groupsize;
		//groups.SetSize(nRows);
		//groupsize.SetSize(seeds.GetSize());
		long * groups = new long[nRows];
		double * groups_dist = new double[nRows];
		long * groupsize = new long[seedidxsize];
		for(long i=0;i<seedidxsize;i++)
		{
			groupsize[i] = 0;
			for(long j=0;j<nCols;j++)
			{
				seeds[i*nCols + j] = missingvalue;
				seedgroup_nonmissingvalues[i*nCols + j] = 0;
			}
		}
		for(long i=0;i<nRows;i++)
			groups[i] = -1;

		long iteration = 0;
		long movement = 1;
		long min_movement = -1;
		long * min_groups = new long[nRows];
		double * min_dists = new double[nRows];
		while(movement && iteration < 100)
		{
			//write centroids
			/*if(iteration)
			{
				CString str;
				str.Format("c:\\centroids%d.csv",iteration);
				FILE * fp = fopen(str,"w");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						for(long jj=0;jj<nCols;jj++)
						{
							fprintf(fp,"%g,",seeds[ii*nCols+jj]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
				}

                str.Format("c:\\centroidgroups.csv");
				fp = fopen(str,"a");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						fprintf(fp,"%i,",groupsize[ii]);					
					}				
					fprintf(fp,"\n");
					fclose(fp);
				}
			}*/

			movement = 0;

			for(long i=0;i<nRows;i++)
			{
				//step 4. pop from current group if current group has > 1 member
				if(iteration)
				{
					if(groupsize[groups[i]] == 1)
						continue;

					groupsize[groups[i]]--;
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = seeds[groups[i]*nCols+k];
						if(v1 != missingvalue &&
							v2 != missingvalue)
						{
							seeds[groups[i]*nCols+k] = v2 - v1;
						}
						if(v1 != missingvalue)
							seedgroup_nonmissingvalues[groups[i]*nCols+k]--;
					}
				}
				
				double min_dist;
				long min_idx;
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seeds(j))
					double dist = 0;
					double num = 0,den = 0;
					long missing = 0;
					if(iteration)
					{
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = seeds[j*nCols+k];
							if(v1 == missingvalue ||
								v2 == missingvalue)
							{							
								missing++;
							}
							else
							{
								if(seedgroup_nonmissingvalues[j*nCols+k] > 0)
									v2 = v2 / seedgroup_nonmissingvalues[j*nCols+k];

								if(v1 >= 0 && v2 >= 0)
								{
									num += fabs(v1-v2);
									den += (v1 + v2);
								}
								else
								{
									//err
								}
							}
						}
						if(den == 0)
						{
							//err
							den = 1;
						}
						dist = num/(double)den;
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
					else
					{
						//seeds present in indexed file					
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
							if(v1 == missingvalue ||
								v2 == missingvalue)
							{
								missing++;
							}
							else
							{
								if(v1 >= 0 && v2 >= 0)
								{
									num += fabs(v1-v2);
									den += (v1+v2);
								}
							}
						}
						if(den == 0)
						{
							//err
							den = 1;
						}
						dist = num/(double)den;
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
				}		
				//add this group to group min_idx;
				if(groups[i] != min_idx)
				{
					movement++;				
				}
				groups[i] = min_idx;	
				groups_dist[i] = min_dist;
				groupsize[min_idx]++;
								
				for(long k=0;k<nCols;k++)
				{
					long idx = groups[i]*nCols+k;
					double v1 = seeds[idx];	
					if(m_pDoc->GetValueAt(k,i) != missingvalue)
					{
						if(v1 == missingvalue)
							seeds[idx] = m_pDoc->GetValueAt(k,i);
						else 
							seeds[idx] = seeds[idx] + m_pDoc->GetValueAt(k,i);
						seedgroup_nonmissingvalues[idx]++;
					}
				}
			}

			//3. calc group centroid	
			//already done!

			//4. pop each object from group (recalc centroid) and allocate again
			iteration++;

			CString str;
			str.Format("Allocation (Bray-curtis): iteration: %d, movement: %d",iteration,movement);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread != NULL && dlgthread->dlg != NULL && dlgthread->dlg->m_Label.m_hWnd != NULL)
				{
					dlgthread->dlg->m_Label.SetWindowText(str);				
					dlgthread->dlg->inc();
					dlgthread->dlg->RedrawWindow();
					if(dlgthread->dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "Rows: Allocation (Bray-Curtis) canceled by user.";
						break;					
						
						//dlgthread->PostThreadMessage(WM_QUIT,0,0);
						//AfxGetMainWnd()->SetFocus();
						//AfxGetMainWnd()->BringWindowToTop();
						//delete [] col_min;
						//delete [] col_max;
						//delete [] col_range;
						//delete [] seedidx;
						//delete [] seeds;
						//delete [] groups;
						//delete [] groupsize;
						//return false;
					}
				}
			}

			//do min_movement;
			if(min_movement == -1 || min_movement >= movement)
			{
				min_movement = movement;
				for(long p=0;p<nRows;p++)
				{
					min_groups[p] = groups[p];
					min_dists[p] = groups_dist[p];
				}
					
			}
		}

		//write-back row groups
		m_pDoc->m_loading = true;
		for(long i=0;i<nRows;i++)
		{		
			m_pDoc->SetRowGroups(i,min_groups[i]+1,min_dists[i]);					
		}
		m_pDoc->m_loading = false;

		CTime time = CTime::GetCurrentTime();
		m_pDoc->m_FuseTypeRows = "Non-hierarchical Clustering";
		m_pDoc->m_FuseAssoRows = "Bray & Curtis";	
		m_pDoc->m_FuseTimeRows = time.Format("%H:%M:%S, %B %d, %Y");


		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
		AfxGetMainWnd()->BringWindowToTop();
		AfxGetMainWnd()->SetFocus();
		
		delete [] col_min;
		delete [] col_max;
		delete [] col_range;
		delete [] seedidx;
		delete [] seeds;
		delete [] groups;
		delete [] groupsize;
		delete [] seedgroup_nonmissingvalues;
		return TRUE;
	}
	else //minkowski nAssocMeasure == 3
	{
		CProgressDlgThread * dlgthread;
	
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->mhnd = m_parent;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread = (CProgressDlgThread *)AfxBeginThread((RUNTIME_CLASS(CProgressDlgThread)));
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->m_ready);

		long pp = 0;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) while(!dlgthread->dlg || !::IsWindow(dlgthread->dlg->m_hWnd)) pp++;

		//::WaitForSingleObject(dlgthread,20000);
		
		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->dlg->m_Progress.SetRange(0,122);
		
		long nCols,nRows;
		double missingvalue = m_pDoc->GetMissingValue();

		if ( m_pDoc->IsExtrinsicColumns() )
			nCols = m_pDoc->GetIdxLeftExtrinsicColumn();
		else
			nCols = m_pDoc->GetNoOfColumns();
		if ( m_pDoc->IsExtrinsicRows() )
			nRows = m_pDoc->GetIdxTopExtrinsicRow();
		else
			nRows = m_pDoc->GetNoOfRows();

	//	double *col_min = new double[nCols];
	//	double *col_max = new double[nCols];
	//	double *col_range = new double[nCols];
		long * seedidx = new long[nNoOfGroups+1];
		long seedidxsize = 0;
	//	for(long i=0;i<nCols;i++)
	//	{
	//		col_min[i] = m_pDoc->GetColumnMinimum(i);
	//		col_max[i] = m_pDoc->GetColumnMaximum(i);
	//		col_range[i] = col_max[i] - col_min[i];
	//	}
		
		//CORRECT ALGORITHM - GOWERMETRIC ASSOCIATIONS

		//pre (1) - determine start_radius
		double start_radius = 0;
		for(long i=2;i<nRows;i++)
		{
			//calc dist between obj(i) & obj(0)
			double dist = 0;
			long missing = 0;				
			for(long k=0;k<nCols;k++)
			{
				double v1 = m_pDoc->GetValueAt(k,i);
				double v2 = m_pDoc->GetValueAt(k,0);
				if(v1 == missingvalue ||
					v2 == missingvalue)
					missing++;
				else
					dist += pow(fabs(v1 - v2),power);
			}

			//add to seedidx if distance > radius
			if(nCols == missing)
			{
				//error
				missing--;
			}
			dist = pow(dist,1/(double)power) / (double)(nCols-missing);
			if(dist > start_radius)
			{					
				start_radius = dist;
			}					
		}
		start_radius /= 2.0;

		//1. determine correct # of groups by varying radius
		double radius = start_radius;
		double step = radius/2.0;
//		CArray<long> seedidx;

		long count = 0;
		seedidx[0] = 0;
		while(seedidxsize != nNoOfGroups && count < 40)
		{
			//seedidx.RemoveAll();
			//seedidx.Add(0); //first object is always seed of first group.
			seedidxsize = 1;
			
			for(long i=1;i<nRows;i++)
			{
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seedidx(j))
					double dist = 0;
					long missing = 0;				
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
						if(v1 == missingvalue ||
							v2 == missingvalue)
							missing++;
						else
							dist += pow(fabs(v1 - v2),power);
					}

					//add to seedidx if distance > radius
					if(nCols == missing)
					{
						//error
						missing--;
					}
					dist = pow(dist,1/(double)power) / (double)(nCols-missing);
					if(dist < radius)
					{					
						break;
					}					
				}
				if(j == seedidxsize)				
				{
					//seedidx.Add(i);					
					seedidx[seedidxsize] = i;
					seedidxsize++;
				}

				if(seedidxsize > nNoOfGroups)
					break;
			}
			count++; //force a break

			//intermediate results write
//			WriteToSeedGroups(seedidxsize,seedidx,radius);

			if(seedidxsize == nNoOfGroups)
				continue;

			//PERFORM RECONCILIATION OF NUMBER OF GROUPS IF count >= 20
			if(count < 35)
			{
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else if(seedidxsize > nNoOfGroups)
				{
					radius += step;
				}
				step /= 2.0;
			}
			else
			{
				//loop while number of groups is < nNoOfGroups
				if(seedidxsize < nNoOfGroups)
				{
					radius -= step;
				}
				else
				{
					break;
				}			
			}

			CString str;
			str.Format("Allocation (Minkowski): seed generation iteration: %d",count);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) void * v = dlgthread->dlg;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread != NULL && dlgthread->dlg != NULL && dlgthread->dlg->m_Label.m_hWnd != NULL)
				{
					dlgthread->dlg->m_Label.SetWindowText(str);
					dlgthread->dlg->inc();
					dlgthread->dlg->RedrawWindow();
					if(dlgthread->dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "Rows: Allocation canceled by user.";
						dlgthread->PostThreadMessage(WM_QUIT,0,0);
						AfxGetMainWnd()->SetFocus();
						AfxGetMainWnd()->BringWindowToTop();
						//delete dlgthread;

					//	delete [] col_min;
					//	delete [] col_max;
					//	delete [] col_range;
					//	delete [] seedidx;

						return false;
					}
				}
			}
		}	

		
		double * seeds = new double[seedidxsize*nCols];
		double * seedgroup_nonmissingvalues = new double[seedidxsize*nCols];

		//2. allocate all objects to a group
		//CArray<long> groups,groupsize;
		//groups.SetSize(nRows);
		//groupsize.SetSize(seeds.GetSize());
		long * groups = new long[nRows];
		double * groups_dist = new double[nRows];
		long * groupsize = new long[seedidxsize];
		for(long i=0;i<seedidxsize;i++)
		{
			groupsize[i] = 0;
			for(long j=0;j<nCols;j++)
			{
				seeds[i*nCols + j] = missingvalue;
				seedgroup_nonmissingvalues[i*nCols + j] = 0;
			}
		}
		for(long i=0;i<nRows;i++)
			groups[i] = -1;

		long iteration = 0;
		long movement = 1;
		long min_movement = -1;
		long * min_groups = new long[nRows];
		double * min_dists = new double[nRows];
		while(movement && iteration < 100)
		{
			//write centroids
			if(iteration)
			{
				/*CString str;
				str.Format("c:\\centroids%d.csv",iteration);
				FILE * fp = fopen(str,"w");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						for(long jj=0;jj<nCols;jj++)
						{
							fprintf(fp,"%g,",seeds[ii*nCols+jj]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
				}

                str.Format("c:\\centroidgroups.csv");
				fp = fopen(str,"a");
				if(fp)
				{
					for(long ii=0;ii<seedidxsize;ii++)
					{			
						fprintf(fp,"%i,",groupsize[ii]);					
					}				
					fprintf(fp,"\n");
					fclose(fp);
				}*/
			}

			movement = 0;

			for(long i=0;i<nRows;i++)
			{
				//step 4. pop from current group if current group has > 1 member
				if(iteration)
				{
					if(groupsize[groups[i]] == 1)
						continue;

					groupsize[groups[i]]--;
					for(long k=0;k<nCols;k++)
					{
						double v1 = m_pDoc->GetValueAt(k,i);
						double v2 = seeds[groups[i]*nCols+k];						
						
						if(v1 != missingvalue &&
							v2 != missingvalue)
						{							
							seeds[groups[i]*nCols+k] = v2 - v1;
							
						}
						if(v1 != missingvalue) seedgroup_nonmissingvalues[groups[i]*nCols+k]--;
					}
				}
				
				double min_dist;
				long min_idx;
				for(long j=0;j<seedidxsize;j++)
				{
					//calc dist between obj(i) & obj(seeds(j))
					double dist = 0;
					long missing = 0;
					if(iteration)
					{
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = seeds[j*nCols+k];
							if(v1 == missingvalue ||
								v2 == missingvalue)
							{
								missing++;
							}
							else
							{
								if(seedgroup_nonmissingvalues[j*nCols+k]>0)
									v2 = v2 / seedgroup_nonmissingvalues[j*nCols+k];
								dist += pow(fabs(v1 - v2),power);
							}
						}
						dist = pow(dist,1.0/(double)power)/(double)(nCols-missing);
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
					else
					{
						//seeds present in indexed file					
						for(long k=0;k<nCols;k++)
						{
							double v1 = m_pDoc->GetValueAt(k,i);
							double v2 = m_pDoc->GetValueAt(k,seedidx[j]);
							if(v1 == missingvalue ||
								v2 == missingvalue)
							{
								missing++;
							}
							else
							{
								dist += pow(fabs(v1 - v2),power);
							}
						}
						dist = pow(dist,1.0/(double)power)/(double)(nCols-missing);
						if(j==0 || min_dist > dist)
						{
							min_dist = dist;
							min_idx = j;
						}
					}
				}		
				//add this group to group min_idx;
				if(groups[i] != min_idx)
				{
					movement++;				
				}
				groups[i] = min_idx;	
								
				groupsize[min_idx]++;
				for(long k=0;k<nCols;k++)
				{
					long idx = groups[i]*nCols+k;					
				double v1 = seeds[idx];					
					if(m_pDoc->GetValueAt(k,i) != missingvalue)
					{
						if(v1 == missingvalue)
							seeds[idx] = m_pDoc->GetValueAt(k,i);
						else
							seeds[idx] = v1 + m_pDoc->GetValueAt(k,i);
						seedgroup_nonmissingvalues[idx]++;
					}
				}
			}

			//3. calc group centroid	
			//already done!

			//4. pop each object from group (recalc centroid) and allocate again
			iteration++;

			CString str;
			str.Format("Allocation (Minkowski): iteration: %d, movement: %d",iteration,movement);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) 
			{
				if(dlgthread != NULL && dlgthread->dlg != NULL && dlgthread->dlg->m_Label.m_hWnd != NULL)
				{
					dlgthread->dlg->m_Label.SetWindowText(str);
					dlgthread->dlg->inc();
					dlgthread->dlg->RedrawWindow();
					if(dlgthread->dlg->m_Cancel)
					{
						m_pDoc->m_logerror = "Rows: Allocation canceled by user.";
						dlgthread->PostThreadMessage(WM_QUIT,0,0);
						AfxGetMainWnd()->BringWindowToTop();
						AfxGetMainWnd()->SetFocus();					

						/*delete [] col_min;
						delete [] col_max;
						delete [] col_range;
						delete [] seedidx;
						delete [] seeds;
						delete [] groups;
						delete [] groupsize;
						return false;*/
						break;
					}
				}
			}

			//backup min_movement
			if(min_movement == -1 || min_movement > movement)
			{	
				min_movement = movement;
				//copy groups to min_groups
				for(long p=0;p<nRows;p++)
				{
					min_groups[p] = groups[p];
					min_dists[p] = groups_dist[p];
				}
			}
		}

		//write-back row groups
		m_pDoc->m_loading = true;
		for(long i=0;i<nRows;i++)
		{		
			//m_pDoc->SetRowGroups(i,groups[i]);		
			m_pDoc->SetRowGroups(i,min_groups[i]+1,min_dists[i]);		

		}
		m_pDoc->m_loading = false;

		CTime time = CTime::GetCurrentTime();
		m_pDoc->m_FuseTypeRows = "Non-hierarchical Clustering";
		m_pDoc->m_FuseAssoRows = "Minkowski";	
		m_pDoc->m_FuseTimeRows = time.Format("%H:%M:%S, %B %d, %Y");

		if(!((CPatnApp*)AfxGetApp())->silent_mode) dlgthread->PostThreadMessage(WM_QUIT,0,0);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->BringWindowToTop();		

	//	delete [] col_min;
	//	delete [] col_max;
	//	delete [] col_range;
		delete [] seedidx;
		delete [] seeds;
		delete [] groups;
		delete [] groupsize;
		delete [] min_groups;
		delete [] groups_dist;
		delete [] min_dists;
		delete [] seedgroup_nonmissingvalues;
		return TRUE;
	}
}

BOOL CClassification::Fuse(long nType, long nFuseTechnique, long nNoOfGroups, double nBeta)
{
	CWaitCursor waitCursor;
	CString temp;
	CFusionDlg fuse;
	CDend dend;
	CTime time = CTime::GetCurrentTime();
	// TODO: Make CFuse a friend and change the following to passing 'this' pointer

	switch (nType )
	{
		case columns:
			// TODO: improve handling of following error
			// - perhaps check at analysis dialog box stage
			if ( m_pDoc->m_AssocMatrixCols.m_aMatrix.GetSize() == 0 )
			{
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Columns: An association measure must be generated\nbefore attempting fusion.",MB_OK | MB_ICONSTOP);
				m_pDoc->m_logerror = "Columns: An association measure must be generated\nbefore attempting fusion.";
				return FALSE;
			}

			fuse.Setup(m_pDoc,false,
			            &m_pDoc->m_AssocMatrixCols.m_aMatrix,
						&m_pDoc->m_FuseTableCols,
			            nFuseTechnique,
						&m_pDoc->m_nCutOffCols,
			            nNoOfGroups,
			            nBeta,
						&m_pDoc->m_aColumnGroups);
			

			m_pDoc->m_FuseAssoCols = m_pDoc->m_AssocMatrixCols.m_strMeasure;
			m_pDoc->m_FuseBetaCols = nBeta;
			m_pDoc->m_FuseTimeCols = time.Format("%H:%M:%S, %B %d, %Y");
			switch (nFuseTechnique)
			{
				case CFusionDlg::NearestNeighbour:
					temp.LoadString(FUSE_NEAREST_NEIGHBOUR);
					break;
				case CFusionDlg::FurthestNeighbour:
					temp.LoadString(FUSE_FURTHEST_NEIGHBOUR);
					break;
				case CFusionDlg::FlexibleWPGMA:
					temp.LoadString(FUSE_FLEXIBLE_WPGMA);
					break;
				case CFusionDlg::FlexibleUPGMA:
					temp.LoadString(FUSE_FLEXIBLE_UPGMA);
					break;
			}
			m_pDoc->m_FuseTypeCols = temp;
			break;

		case rows:
			// TODO: improve handling of following error
			// - perhaps check at analysis dialog box stage
			if ( m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() == 0 )
			{
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Rows: An association measure must be generated\nbefore attempting fusion.",MB_OK | MB_ICONSTOP);
				m_pDoc->m_logerror = "Rows: An association measure must be generated\nbefore attempting fusion.";
				return FALSE;
			}
			fuse.Setup(m_pDoc,true,
						&m_pDoc->m_AssocMatrixRows.m_aMatrix,
			            &m_pDoc->m_FuseTableRows,
			            nFuseTechnique,
						&m_pDoc->m_nCutOffRows,
			            nNoOfGroups,
			            nBeta,
						&m_pDoc->m_aRowGroups);
			m_pDoc->m_FuseAssoRows = m_pDoc->m_AssocMatrixRows.m_strMeasure;
			m_pDoc->m_FuseBetaRows = nBeta;
			m_pDoc->m_FuseTimeRows = time.Format("%H:%M:%S, %B %d, %Y");
			switch (nFuseTechnique)
			{
				case CFusionDlg::NearestNeighbour:
					temp.LoadString(FUSE_NEAREST_NEIGHBOUR);
					break;
				case CFusionDlg::FurthestNeighbour:
					temp.LoadString(FUSE_FURTHEST_NEIGHBOUR);
					break;
				case CFusionDlg::FlexibleWPGMA:
					temp.LoadString(FUSE_FLEXIBLE_WPGMA);
					break;
				case CFusionDlg::FlexibleUPGMA:
				default:
					temp.LoadString(FUSE_FLEXIBLE_UPGMA);
					break;
			}
			m_pDoc->m_FuseTypeRows = temp;

			break;
	}
	//if ( !fuse.Fuse() )
	//	return FALSE;
	fuse.DoModal();
	if(!fuse.m_value){
   	//error handled by CFuseDlg
	   return false;
   }

	BOOL bSuccess;
	switch (nType )
	{
		case columns:
			bSuccess = dend.SetDendOrder(&m_pDoc->m_FuseTableCols,
										 &m_pDoc->m_DendOrderCols,
										 &m_pDoc->m_DendTableCols,
										 &m_pDoc->m_aColumnGroups,
										 &m_pDoc->m_GroupDendOrderCols,
										 &m_pDoc->m_GroupDendTableCols);
			break;

		case rows:
			bSuccess = dend.SetDendOrder(&m_pDoc->m_FuseTableRows,
										 &m_pDoc->m_DendOrderRows,
										 &m_pDoc->m_DendTableRows,
										 &m_pDoc->m_aRowGroups,
										 &m_pDoc->m_GroupDendOrderRows,
										 &m_pDoc->m_GroupDendTableRows);
			break;
	}
	if ( !bSuccess ){
   	m_pDoc->m_logerror = "Creation of Dendrogram failed";
		return FALSE;
   }

	return TRUE;
}



bool CClassification::Allocate(long nNoOfGroups)
{
    CAssocMatrixRow* pAMR;
	long irow;
	long jcol;
	long iseeds;
	long rowsize;
	long seedssize;
	double mincutoff;
	double maxcutoff;
	double asovalue;
	bool ingroup;

	//CnewDialog * dlg;
	
//	CProgressDlgThread * dlgthread;
//	dlgthread = new CProgressDlgThread;
//	dlgthread->hnd = AfxGetMainWnd()->GetSafeHwnd();
//	dlgthread->CreateThread(CREATE_SUSPENDED);
//	dlgthread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
//	dlgthread->ResumeThread();

	//dlg = new CnewDialog;
	CString progress_title;
	long pos=0;
	progress_title.Format("Analysis Progress - Allocation on Rows... Iteration: %d",iteration_count++);
	//dlg->Create(progress_title);
	//dlg->ShowWindow(SW_SHOWNORMAL);

    //initialize seeds: the first association value in the matrix
	double initseed;
	seeds.RemoveAll();
	pAMR = m_pDoc->m_AssocMatrixRows.m_aMatrix[0];
	initseed = pAMR->GetAt(0);
	seeds.Add(initseed);
	// Allocate the association values to their groups	
	for ( irow = 0; irow < m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize(); irow++ ){ 
		if(irow/20 > pos){
			pos++;
			progress_title.Format("Analysis Progress - Allocation on Rows... Iteration: %d",iteration_count++);
			//dlg->m_str = progress_title;
			//dlg->m_Label.SetWindowText(progress_title);
			//dlg->inc();
			//dlg->RedrawWindow();
//			if(dlgthread->dlg->m_Cancel)
//			{
//		//		delete dlg;
//				return false;
//			}
		}
		pAMR = m_pDoc->m_AssocMatrixRows.m_aMatrix[irow];
		if(pAMR != NULL){
			rowsize = pAMR->GetSize();
			for ( jcol = 0; jcol<rowsize; jcol++ ){
				asovalue = pAMR->GetAt(jcol);
				ingroup = false;
				seedssize = seeds.GetSize();
				for ( iseeds = 0; iseeds<seedssize; iseeds++ ){
					mincutoff = (seeds.GetAt(iseeds) - radius);
					maxcutoff = (seeds.GetAt(iseeds) + radius);
					if ((asovalue > mincutoff) && (asovalue < maxcutoff)) { 
						if (irow == 0){
							m_pDoc->SetRowGroups(irow, (iseeds+1));
						}
						m_pDoc->SetRowGroups(irow+1, (iseeds+1));
						ingroup = true;
						break;
					}
				}
				if (ingroup == true){
					break;
				}
			}			
			if ((ingroup == false) && (seeds.GetSize() < nNoOfGroups)){
				seeds.Add(asovalue);
				m_pDoc->SetRowGroups(irow+1, seeds.GetSize());
				ingroup = true;
			}
			if (ingroup == false){
				notInGroup++;
			}
		}
	}

	//if(dlg->m_Cancel)
//	{
//		delete dlg;
//		return false;
//	}
//	else
//	{
//		delete dlg;
		return true;
//	}
}
