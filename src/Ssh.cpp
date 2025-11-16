// Ssh.cpp: implementation of the CSsh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include <math.h>
#include "ssh.h"
#include "time.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

long sort_count;
long max_sorts;
long show_sort_box;

double g_stress;
long g_it;

// FORTRAN code present for easy algorithm checking

/*
C     Last change:  LB   23 Oct 97    5:26 pm
        PROGRAM SSH
*
* Multi-Dimensional-Scaling Program for HYBRID model with absolute CUT POINT
* defining INTERVAL SCALING < CUT < GUTTMAN-LINGOES STRONG MONOTONE 
REGRESSION
*
*<<Revision History>>
*<<Revision History>>
*
        PARAMETER (MAX99=100000,MAX96=210,MAXGG=1)
*
        EXTERNAL ININT
        INCLUDE 'DECLARD.INC'
        INCLUDE 'DECLARL.INC'
        INCLUDE 'COMMON.INC'
        INCLUDE 'DATINI.INC'
*       open (unit=11,file='t.tmp',status='unknown',access='append')
*----------------------------------------------PARAMETERS
*UNIX   jspoof=ispoof()
        CALL INPRAM
        CALL DATIM (0,'ssh ','    ',1)
        I92=N*(N-1)*0.5
        I92=MAX(I92,1)
        I96=N
*
        WRITE (IUTRMO,100)
100     FORMAT (5X,'Number of DIMENSIONS for solution (I,D:3)',
     1    7X,'? : ',$)
        CALL ININT (161,ND,1,M,3,'NUMBER OF DIMENSIONS')
*
        I1=I92+1
        I2=I1+I92
        I3=I2+I92
        I4=I3+I92
        I5=I4+N*ND
        I6=I5+N*ND
        I99=I6+N*ND
*============================
        INCLUDE 'ALLOCD.INC'
        INCLUDE 'ALLOCL.INC'
*============================
        CALL SSH1 (D(1),D(I1),D(I2),D(I3),D(I4),D(I5),D(I6),ND,LAB)
        END
*/
/*
        SUBROUTINE SSH1 (DIN,DOUT,DEST,INDX,CONF,GRAD,GRAL,ND,LAB)
*
* DIN  - INPUT DISSIMILARITY
* DOUT - EUCLIDEAN DISTANCES BASED ON CONFIGURATION IN CONF
* DEST - ESTIMATES OF DOUT BASED ON REGRESION
*
        EXTERNAL ININT
        DOUBLE PRECISION DRNS,SUM,DEV,U,T,SFGR,COSG
        REAL DIN(I92),DOUT(I92),DEST(I92),
     1       CONF(N,ND),GRAD(N,ND),GRAL(N,ND)
        INTEGER INDX(I92)
        CHARACTER LAB(M96)*8,TEXT*11,FMTT*80,ULT*1,OF1*43,OF2*43
        INCLUDE 'COMMON.INC'
*-------------------------------------------------SOME CONSTANTS
        SMALL=1.E-6
        STBEST=1.0
        ITNUM=0
        IRS=1
        FN=N
        SQRTN=SQRT(FN)
        SQRND=SQRT(1.0/FLOAT(ND))
        NBAK=0
        TEXT(11:)='*'
        FMTT=FMTI
*-------------------------------------------------SHEPHARD FILE
        WRITE (IUTRMO,5)
5       FORMAT (5x,'Output ultrametrics & estimates (Y/N,D:N)',
     1    7x,'? : ',$)
        CALL INYON (251,ULT,'N','OUTPUT SHEPHARD ULTRAMETRICS')
*-------------------------------------------------NUMBER OF ITERATIONS
        WRITE (IUTRMO,10)
10      FORMAT (5X,'Stopping rule 1: Maximum ITERATIONS (I,D:50)',
     1    4X,'? : ',$)
        CALL ININT (162,MAXIT,1,500,50,'MAXIMUM NUMBER OF ITERATIONS')
*-------------------------------------------------STRESS DIFFERENCE
        WRITE (IUTRMO,20)
20      FORMAT (5X,'Stopping rule 2: Min STRESS diff. (F,D:.005)',
     1   '    ? : ',$)
        CALL INREL (163,STRDIF,0.,1.,.005,'MINIMUM STRESS DIFF')
*-------------------------------------------------INTERVAL-RATIO ?
        WRITE (IUTRMO,21)
21      FORMAT (' -----REGRESSION FOR ASSOCIATIONS < CUT-POINT:'/
     1    5X,'1 = Interval'/
     2    5X,'2 = Ratio    (I,D:2)',28X,'? : ',$)
        CALL ININT (164,IREG,1,2,2,'1=INTERVAL__2=RATIO')
*---------------------------------------------------CUT POINT
        WRITE (IUTRMO,22)
22      FORMAT (5X,'Association ratio-ordinal CUT value (F,D:0.9)',
     1   '   ? : ',$)
        CALL INREL (165,CUT,-1.e10,1.e10,.9,'INTERVAL-ORDINAL CUT')
*--------------------------------------------------READ DIN
        CALL INLSYM (DIN,'N','N','aso')
        IER=1
        CALL INLAB (LAB,'N','N','rlb',1,N,IER)
		*=====================================================SORT DIN & LOCATE CUT
        WRITE (IUTRMO,2)
2       FORMAT ('+.....Sorting associations & locating cut point')
        INDX(1)=0
        CALL SRTSHL (DIN,INDX,I92)
        ICUT=I92+1
        DO 24 I=1,I92
          IF (DIN(I).GT.CUT) THEN
            ICUT=I
            GO TO 25
          END IF
24      CONTINUE
*-----------------STORE SORTED INPUT B4 ORDINALIZING
25      IF (ULT.EQ.'Y') THEN
          CALL REQOFL (IUINTR,'N','N','N','tmp',
     1      'SEQUENTIAL','UNFORMATTED','SCRATCH')
          WRITE (IUINTR) DIN
          CALL REQOFL (IUOUT2,'N','N','N','shp',
     1      'SEQUENTIAL','FORMATTED','UNKNOWN')
          OF1=OUTFIL
        END IF
*-----------------------------------------------------RANK ORDINALS
        IF (ICUT.LE.I92) THEN
          NORD=I92-ICUT+1
          CALL RANK (DIN(ICUT),NORD)
        END IF
*=====================================================INITIAL CONFIGURATION
        WRITE (IUTRMO,30)
30      FORMAT (' -----SOURCE OF INITIAL CONFIGURATION:'/
     1    5X,'1 = Generate UNIFORM RANDOM co-ordinates'/
     2    5X,'2 = Read from a PATN DATA file'/
     3    5X,'3 = Read from a PATN ORDINATION file (I,D:1)',4X,'? : ',$)
        CALL ININT (166,ICON,1,3,1,'1=UNIRAND_2=DATA_3=ORDN')
*=============================================================RANDOM LOOP
901     WRITE (IUTRMO,'(1X)')
        SFGR=SQRTN
        ACSAV=0.
        COSAV=0.
        SRATAV=0.8
        STLAST=1.0
        STEP=0.
        R=0.111111
*----------------------------------------INTERNAL UNIFORM RANDOM
        IF (ICON.EQ.1) THEN
          IF (ITNUM.EQ.0) THEN
            WRITE (IUTRMO,40)
40          FORMAT (5X,'Number of random starts to be used (I,D:10)',
     1        5X,'? : ',$)
            CALL ININT (167,NRS,1,999,10,'NUMBER OF RANDOM STARTS')
            WRITE (IUTRMO,100)
100         FORMAT (5X,'Random number SEED value (F,D:1235.)',
     1      12X,'? : ',$)
            CALL INREL (82,RNS,SMALL,1.E10,12345.,'RANDOM NUMBER SEED')
            DRNS=RNS
          ELSE
           IRS=IRS+1
           RNS=RNS+1.0
           DRNS=RNS
          END IF
          DO 120 I=1,N
            DO 110 J=1,ND
              CONF (I,J)=RANDU (DRNS)
110         CONTINUE
120       CONTINUE
*-----------------------------------------DATA READ
        ELSE IF (ICON.EQ.2) THEN
          IER=1
          RNS=-2.
          FMTI=FMTT
          CALL REQIFL ('Y','Y','dat',IER,'FORMATTED')
          DO 121 I=1,N
            READ (IUINPT,FMTI) (CONF(I,J),J=1,ND)
121       CONTINUE
          CLOSE (UNIT=IUINPT)
*-----------------------------------------ORD READ
        ELSE
          RNS=-3.
          CALL INORD (CONF,N,ND,ND,'Y','N','ssh',1,LAB)
        END IF
*-----------------------------------------------------INITIALIZE GRADIENT
        DO 125 I=1,N
          DO 124 L=1,ND
            GRAD(I,L)=SQRND
124       CONTINUE
125     CONTINUE
*=========================================================ITERATION
        DO 900 ITNUM=1,MAXIT
*1-----------------------------------NORMALIZE CONFIG + INI & TRANSFER 
GRADIENT
          DEV=0.D0
          DO 129 L=1,ND
            SUM=0.D0
*--GET MEAN
            DO 127 I=1,N
              SUM=SUM+CONF(I,L)
127         CONTINUE
            RMEAN=SUM/FN
*--SUB MEAN
            DO 128 I=1,N
              CONF(I,L)=CONF(I,L)-RMEAN
              DEV=DEV+CONF(I,L)**2
128         CONTINUE
129       CONTINUE
*---/ BY SD & READY GRAD-GRAL
          DEV=SQRT(DEV/FN)
          DO 131 I=1,N
            DO 130 L=1,ND
              CONF(I,L)=CONF(I,L)/DEV
              GRAL(I,L)=GRAD(I,L)/DEV
              GRAD(I,L)=0.
130         CONTINUE
131       CONTINUE
          SFGL=SFGR/DEV
*2-------------------------------------------------EUCLIDEAN ON CONFIG
          CALL ED (CONF,DOUT,INDX,I92,N,ND)
*3-------------------------------------------------REGRESSIONS
          IF (ICUT.GT.1) CALL LINREG (DIN,DOUT,I92,ICUT,IREG,R,X0,X1)
          IF (ICUT.LE.I92) CALL ORDREG (DOUT(ICUT),DEST(ICUT),NORD,R)
*4------------------------------------------ESTIMATES & DISPARITIES & STRESS
          CALL FIT (DIN,DOUT,DEST,I92,ICUT,IREG,X0,X1,U,T)
          STRESS=SQRT(U/T)
*S-IMP    STIMP=(STLAST-STRESS)/STLAST
          UT=SQRT(U)/(T*SQRT(T))
          UTS=SQRT(U*T)
*5--------------------------------------------CALCULATE GRADIENT GRAD
          DO 134 K=1,I92
            CALL LINLT (INDX(K),I,J)
            DK=DOUT(K)
            IF (DK.NE.0.0.AND.UTS.NE.0.0) THEN
              SCALE=STRESS*((UT*DK)-(DK-DEST(K))/UTS)/DK
              DO 133 L=1,ND
                SHIFT=SCALE*(CONF(I,L)-CONF(J,L))
                GRAD(I,L)=GRAD(I,L)+SHIFT
                GRAD(J,L)=GRAD(J,L)-SHIFT
133           CONTINUE
            ELSE
              SCALE=STRESS
            END IF
134       CONTINUE
*6------------------------------------------SCALE FACTOR & GRAD-GRAL ANGLE
          SFGR=0.
          COSG=0.
          DO 136 I=1,N
            DO 135 L=1,ND
              GIL=GRAD(I,L)
              SFGR=SFGR+GIL*GIL
              COSG=COSG+GIL*GRAL(I,L)
135         CONTINUE
136       CONTINUE
          SFGR=SQRT(SFGR/FN)
*---------------------->>-------------IF SFGR < SMALL: WRITE & STOP
          IF (SFGR.LT.SMALL) THEN
            TEXT='move < min'
            WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
            GO TO 200
          END IF
*---GO ON
          COSG=COSG/(SFGL*SFGR*FN)
          IF (ITNUM.EQ.1) THEN
            SRAT=0.8
          ELSE
            SRAT=STRESS/STLAST
          END IF
*---------------------------------------OVERSHOOT
          IF (COSG.LT.-0.95.OR.SRAT.GT.1.1) THEN
            NBAK=NBAK+1
            IF (NBAK.EQ.1) STB=1.0
            STB=STB/DEV
            STEPO=STEP
            STEP=STEP/20.0
            TEXT(1:10)='<<<<<<<<<<'
            WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
			            STEPO=STB*(STEPO-STEP)/SFGL
            DO 138 I=1,N
              DO 137 L=1,ND
                CONF(I,L)=CONF(I,L)-STEPO*GRAL(I,L)
                GRAD(I,L)=GRAL(I,L)
137           CONTINUE
138         CONTINUE
            SFGR=SFGL
            STRESS=STLAST
*7------------------------------------------------STEP SIZE ?
          ELSE
            STEPO=STEP
            NBAK=0
            SRATAV=SRAT**0.33334 * SRATAV**0.66666
            COSAV=COSG*0.66 + COSAV*0.34
            ACSAV=ABS(COSG)*0.66 + ACSAV*0.34
            IF (ITNUM.EQ.1) THEN
              STEP=25.0*STRESS*SFGR
            ELSE
              ANGLE=4.0**COSAV
              RELAX=1.6/
     1       ((1.0+MIN(1.0,SRATAV)**5)*(1.0+(ACSAV-ABS(COSAV))) )
              GOODL=SQRT(MIN(1.0,SRAT))
              STEP=STEP*ANGLE*RELAX*GOODL
            END IF
*-----DONT LET STEP BLOW-OUT!
            IF (STEP.GT.9.0) STEP=1.
*=====================================================WRITE STATUS
            TEXT(1:10)='>>>>>>>>>>'
            IP=MOD(ITNUM,10)+1
            TEXT(IP:IP)='!'
            WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
145         FORMAT ('+.....ITERATION=',I3,'  STRESS=',F7.4,
     1     '  STEP=',F7.4,'  R=',F6.3,1X,A)
*=========================================================STOPPING RULES
*--(1-STRDIF)>SRAT>1 AND (1-10*STRDIF)>SRATAV>1
            IF ((SRAT.GT.(1.0-STRDIF).AND.SRAT.LT.1.0).AND.
     1          (SRATAV.GT.(1.0-10.0*STRDIF).AND.SRATAV.LT.1.0) ) THEN
              TEXT='StrDif<Min'
              WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
              GO TO 200
*---MAXIT
            ELSE IF (ITNUM.GE.MAXIT) THEN
              TEXT='> Max Iter'
              WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
              GO TO 200
            END IF
*==================================================CONTINUE: MOVE POINTS
            SHIFT=STEP/SFGR
            DO 170 I=1,N
              DO 160 L=1,ND
                CONF(I,L)=CONF(I,L)+GRAD(I,L)*SHIFT
160           CONTINUE
170         CONTINUE
          END IF
          STLAST=STRESS
*DEBUG WRITES
*         write (11,777) itnum,stress,srat,sratav,cosg,
*     1   cosav,acsav,sfgr,step,R
*777      format (i3,6f7.3,3f8.4)
900     CONTINUE
*==================================================AGAIN ?
*200    write (11,777) itnum,stress,srat,sratav,cosg,
*     1     cosav,acsav,sfgr,step,R
200     IF (STRESS.LT.STBEST) THEN
          STBEST=STRESS
          CALL OUTORD (CONF,LAB,'SSH ','ssh',N,ND,-IUOUT1,ND)
          OF2=OUTFIL
          WRITE (IUOUT1,210) RNS,CUT,STRESS,ITNUM,IREG
210       FORMAT (/,' Random number seed    = ',G20.10,
     1            /,' Ratio-ordinal cut     = ',G20.10,
     2            /,' Stress (1)            = ',G20.10,
     3            /,' Number of iterations  = ',I16,
     4            /,' 1=Interval__2=Ratio   = ',I16)
          REWIND (UNIT=IUOUT1)
*--WRITE * --> TERM
          TEXT(11:)='*'
          WRITE (IUTRMO,145) ITNUM,STRESS,STEP,R,TEXT
          TEXT(11:)=' '
*-------------------------------SHEPHERD VALUES
          IF (ULT.EQ.'Y') THEN
            WRITE (IUTRMO,215)
215         format ('+.....Writing ultrametrics')
*--RE-READ DIN
            REWIND (UNIT=IUINTR)
            READ (IUINTR) DIN
*--WRITE IN COLUMN FORMAT
            WRITE (IUOUT2,220)
220         FORMAT ('   Index  Input   Output  Estimate')
            DO K=1,I92
              WRITE (IUOUT2,230) INDX(K),DIN(K),DOUT(K),DEST(K)
230           FORMAT (I8,3F8.4)
            END DO
            REWIND (UNIT=IUOUT2)
          END IF
        END IF

*--MORANDS ?
        IF (ICON.EQ.1.AND.IRS.LT.NRS) GO TO 901
*--------------------------------------------------NO MORE: WRITEOUT
        CALL OUTPUT (IUOUT1,OF2)
        IF (ULT.EQ.'Y') THEN
          CLOSE (UNIT=IUINTR)
          CALL OUTPUT(IUOUT2,OF1)
        END IF
*
        END
*/
bool CSsh::Ssh1(double * din,		//input assoc
		  double * dout,	//output assoc
		  double * dest,	//estimated assoc
		  long * indx,	//sorted order
		  ldblaray * conf,	//ordination data
		  ldblaray * grad,	//ordination data gradient
		  ldblaray * gral,	//same
		  long nd,			//number of dimensions.
		  long n,
		  CPatnDoc * pDoc,
		  double cut,	//cutoff value
		  long pMaxIterations, //number of random starts
		  double * pStress,
		  ldblaray * saveconf,
		  long pNumberOfRandomStarts,
		  long pRandomSeed,
		  long assocsize) //return stress value
{
	if(pDoc->m_AnalysisRecipe.rows.bCalculateStats){
		FILE * fp;
		fp = fopen("c:\\ordstats.csv","w");
		fprintf(fp,"Stress Pre,Post,Difference,%%,AverageMovement\n");
		fclose(fp);
	}

	sort_count = 0;	
	max_sorts = pMaxIterations*pNumberOfRandomStarts + 2;

	double _small = 0.000001;
	double stbest = 1.0;
	long itnum = -1;
	//STARTNEW
	itnum = -1;
	//ENDNEW
	long irs = 1;
	long fn = n;
	double sqrtn = sqrt(fn*1.0);
	double sqrnd = sqrt(1/(double)nd);
	long nbak = 0;
	bool ult = false;
	long maxit = pMaxIterations;
	double strdif = 0.005;
	long ireg = 2;

	//FILE *fs = fopen("c:\\out.txt","w+");

	//FOR PROGRESS
	long progress = 0;
	long progressStep = maxit * pNumberOfRandomStarts / 100;

	m_pDoc = pDoc;

	//randomize number generator
	//srand(pRandomSeed);
	((CPatnApp*)AfxGetApp())->RandomSeed(pRandomSeed);

	long i92 = (n*(n-1))/2;
	long i,j,k,l;
	long icut;
	bool done;
	long nord;
	long nrs;

	//sort din & locate cut
	indx[0] = -1;

	//make copy of din (To check it)
	//CArray<double> tmpdin;
	//for(i=0;i<din->GetSize();i++)
	//	tmpdin.Add(din->GetAt(i));
	g_it = 0;
	g_stress = 1;

	//SYSTEMTIME tme;
	//GetLocalTime(&tme);
	show_sort_box = 1;
	SrtShl(din,indx,i92);
	//SYSTEMTIME tme2;
	//GetLocalTime(&tme2);
	//long tme3 = (tme2.wMilliseconds - tme.wMilliseconds) + 60*(tme2.wSecond - tme.wSecond);
	if(n > 700){ //tme3 > 400){
		show_sort_box = 1;
	}else{
		show_sort_box = 0;
	}

	//do actual Check here;
	//double v1,v2;
	//long v3;
	//for(i=0;i<indx->GetSize();i++){
	//	v1 = din->GetAt(indx->GetAt(i));
	//	v2 = tmpdin.GetAt(i);
	//	v3 = indx->GetAt(i);
	//	if(din->GetAt(i) != tmpdin.GetAt(indx->GetAt(i)))
	//		AfxMessageBox("oops");
	//}
	//AfxMessageBox("check OK");

	icut = i92;
	done = false;
	for(i=0;i<i92 && !done;i++){
		if(din[i] > cut){
			icut = i;
			done = true;
		}
	}

	//store sorted input b4 ordinalizing
		//don't have to do this because ult == false

	//rank ordinals
	if(icut < i92){
		nord = i92 - icut + 1;
		Rank(din+icut,nord);
	}else{
		sort_count++;
	}

	long ntunnels = 0;

NineZeroOne:
	//initial configuration
	double sfgr = sqrtn;
	double acsav = 0;
	double cosav = 0;
	double sratav = 0.8;
	double stlast = 1;
	double step = 0;
	double r  = 0.111111;

	double x0,x1,u,t;
	long rns;
	double sum;
	
	//internal uniform random - default
	if(itnum == -1){
		nrs = pNumberOfRandomStarts;
		rns = pRandomSeed;		
	}else{
		irs++;
		rns++;
		CString str; str.Format("Progress - (SSH) - random start: %d - stress:%3.4f",irs,g_stress);
		m_dlg->m_Label.SetWindowText(str);
		m_dlg->RedrawWindow();
	}
	
	//TRYING SOMETHING
	//srand(rns);
	((CPatnApp*)AfxGetApp())->RandomSeed(rns);
		
	//force sort_count
	sort_count = (irs-1)*maxit+2;

	
	for(i=0;i<n;i++){
		for(j=0;j<nd;j++){
			conf[i][j] = (double)((CPatnApp*)AfxGetApp)->Rand()*2.0-1.0;
		}
	}

	//else data read
	//else ord read

	//initialize gradient
	for(i=0;i<n;i++){
		for(l=0;l<nd;l++){
			grad[i][l] = sqrnd;
		}
	}

	double angle;
	double rmean;
	double sfgl;
	double stress;
	double ut;
	double uts;
	double dk;
	double scale;
	double shift;
	double cosg;
	double gil;
	double srat;
	double stb;
	double step0;
	double relax;	
	double goodl;
	CString str;

	str.Format("Progress - (SSH) - iteration: %d - stress:%3.4f",g_it,g_stress);
	CString kkk;
	m_dlg->m_Label.SetWindowText(str);
	m_dlg->RedrawWindow();

	g_it = irs;
	g_stress = stbest;
	stress = 1;
	//iteration
	for(itnum=0;itnum<maxit;itnum++){
		g_it = irs;
		progress++;
		//FOR PROGRESSBAR
		if(progress == progressStep){
			m_dlg->inc();
			if(m_dlg->m_Cancel)
			{
				m_pDoc->m_logerror = "Rows: Ordination canceled by user.";
				return false;
			}
			progress = 0;
		}
		
		if(g_stress > stress){
			stbest = stress;
			g_stress = stress;
			str.Format("Progress - (SSH) - random start: %d - stress:%3.4f",g_it,g_stress);
			m_dlg->m_Label.SetWindowText(str);
			m_dlg->RedrawWindow();
		}
		
		//normalize config + ini & transfer
		double dev = 0;
		for(l=0;l<nd;l++){
			sum = 0;
			for(i=0;i<n;i++){
				sum += conf[i][l];
			}
			rmean = sum / fn;
			for(i=0;i<n;i++){
				conf[i][l] = conf[i][l] - rmean;
				dev += pow(conf[i][l],2);
			}
		}

		// / by sd & ready grad-gral
		dev = sqrt(dev/fn);
		for(i=0;i<n;i++){
			for(l=0;l<nd;l++){
				conf[i][l] = conf[i][l] / dev;
				gral[i][l] = grad[i][l] / dev;
				grad[i][l] = 0.0;
			}
		}
		sfgl = sfgr / dev;

		//euclidean on config
		Ed(conf,dout,indx,i92,n,nd);

		//regressions
		if(icut > 0)
			LinReg(din,dout,i92,icut,ireg,&r,&x0,&x1);
		if(icut < i92-1){
			OrdReg(dout+icut,dest+icut,nord);
		}else{
			sort_count++;
		}

		//estimates & disparities & stress
		Fit(din,dout,dest,i92,icut,ireg,x0,x1,&u,&t);
		stress = sqrt(u/t);
	//	stimp = (stlast-stress)/stlast;
		ut = sqrt(u) / (t * sqrt(t));
		uts = sqrt(u * t);

		//calculate gradient grad
		for(k=0;k<i92;k++){
			LinLt(indx[k],&i,&j);
			dk = dout[k];
			if(dk != 0 && uts != 0){
				scale = stress*((ut*dk)-(dk-dest[k])/uts)/dk;
				for(l=0;l<nd;l++){
					shift = scale*(conf[i][l] - conf[j][l]);
					double dist1 = grad[i][l];
					double dist2 = grad[j][l];
					grad[i][l] = dist1 + shift;
					grad[j][l] = dist2 - shift;
				}
			}else{
				scale = stress;
			}
		}

		//scale factor & grad-gral
		sfgr = 0;
		cosg = 0;
		for(i=0;i<n;i++){
			for(l=0;l<nd;l++){
				gil = grad[i][l];
				sfgr = sfgr + gil * gil;
				cosg = cosg + gil * gral[i][l];
			}
		}
		sfgr = sqrt(sfgr / fn);

		//if sfgr < small, write & stop
		if(sfgr < _small){
			//fprintf(fs,"\n%d\t%f\t%f\t%f\tmove < min",itnum,stress,step,r);
			goto TwoZeroZero;
		}
		//go on
		cosg = cosg / (sfgl*sfgr*fn);
		if(itnum == 0) 
			srat = 0.8;
		else 
			srat = stress / stlast;
	
		//overshoot
		if(cosg < -0.95 || srat > 1.1){
			nbak++;
			if(nbak == 1) stb = 1.0;
			stb = stb / dev;
			step0 = step;
			step = step / 20.0;
			step0 = stb*(step0-step)/sfgl;
			for(i=0;i<n;i++){
				for(l=0;l<nd;l++){
					conf[i][l] = conf[i][l] - 
						step0 * gral[i][l];
					grad[i][l] = gral[i][l];
				}
			}
			sfgr = sfgl;
			stress = stlast;
		}else{
			step0 = step;
			nbak = 0;
			sratav = pow(srat,0.33334) * pow(sratav,0.66666);
			cosav = cosg*0.66 + cosav*0.34;
			acsav = fabs(cosg)*0.66 + acsav*0.34;
			if(itnum == 0){
				step = 25.0*stress*sfgr;
			}else{
				angle = pow(4.0,cosav);
				relax = 1.6/((1+pow(min(1.0,sratav),5.0))*(1+(acsav - fabs(cosav))));
				goodl = sqrt(min(1.0,srat));
				step = step * angle * relax * goodl;
			}
			//don't let step blow out
			if(step > 9) step = 1;
			
			//stopping rules
			if((srat > (1-strdif) && srat < 1) &&
				(sratav > (1-10*strdif) && sratav < 1)){
				//fprintf(fs,"\n%d\t%f\t%f\t%f\tstrdif<min",itnum,stress,step,r);
				goto TwoZeroZero;
			}else if(itnum >= maxit){
				//fprintf(fs,"\n%d\t%f\t%f\t%f\t> max iter",itnum,stress,step,r);
				goto TwoZeroZero;
			}

			//continue: move points
			shift = step / sfgr;
			for(i=0;i<n;i++){
				for(l=0;l<nd;l++){
					conf[i][l] = conf[i][l] + grad[i][l] * shift;
				}
			}
		}
		stlast = stress;
	}

TwoZeroZero:
	//FOR PROGRESSBAR
	for(k=itnum;k<maxit;k++){
		progress++;
		if(progress == progressStep){
			m_dlg->inc();
			if(m_dlg->m_Cancel)
			{
				m_pDoc->m_logerror = "Rows: Ordination canceled by user.";
				return false;
			}
			progress = 0;
		}
	}

	if(stress < stbest){
		//save it all
		stbest = stress;
		*pStress = stress;
		for(i=0;i<n;i++){
			for(l=0;l<nd;l++){
				saveconf[i][l] = conf[i][l];
			}
		}
		//fprintf(fs,"*");

		//ult = false so shpherd values missing
	}
	
	//more rands?
	if(irs < nrs)
		goto NineZeroOne;

	//fprintf(fs,"\n\nrandom seed = %d\nratio-ordinal cut = %f\nStress = %f\nNumber of iterations = %d\n",rns,cut,stress,itnum);

	//fclose(fs);

	//no more, finish up
	return true;
}

/*
        SUBROUTINE ED (CONF,ASS,INDX,I92,N,ND)
*
* CALCULATES EUCLIDEAN DISTANCE BETWEEN ALL ROW-PAIRS IN CONF IN SAME
* ORDER AS INDX SO AS TO MATCH DIN
*
        REAL CONF(N,ND),ASS(I92)
        INTEGER INDX(I92)
*
        DO 120 K=1,I92
          CALL LINLT (INDX(K),I,J)
          ASS(K)=0.
          DO 100 L=1,ND
            DIF=CONF(I,L)-CONF(J,L)
            ASS(K)=ASS(K)+DIF*DIF
100       CONTINUE
          ASS(K)=SQRT(ASS(K))
120     CONTINUE
        RETURN
        END
*/
void CSsh::Ed(ldblaray * conf,double*ass,
			  long*indx,long i92,long n,long nd)
{
	long i,j;
	double dif,dist1,dist2;

	for(long k=0;k<i92;k++){
		LinLt(indx[k],&i,&j);
		ass[k] = 0;
		for(long l=0;l<nd;l++){
			dist1 = conf[i][l];
			dist2 = conf[j][l];
			dif = dist1 - dist2;
			ass[k] = ass[k] + dif*dif;
		}
		ass[k] = sqrt(ass[k]);
	}
}

/*
        SUBROUTINE ORDREG (DOUT,DEST,NORD,R)
*
* GUTTMAN - RANK IMAGE REGRESSION
*
        REAL DOUT(NORD),DEST(NORD)
*---------------------------------------COPY DOUT-->DEST
        DO 100 I=1,NORD
          DEST(I)=DOUT(I)
100     CONTINUE
*---------------------------------------SORT DEST
        CALL SRTSH1 (DEST,NORD)
        RETURN
        END
*/
void CSsh::OrdReg(double*dout,double*dest,
				  long nord)
{
	//copy dout to dest
	//for(long i=size-1;i>=nord;i--){
	for(long i=0;i<nord;i++){
		dest[i] = dout[i];
	}

//	CArray<double> ndest;
//	ndest.SetSize(nord);
//	for(i=0;i<nord;i++){
//		ndest[i] = dest->GetAt(i+dest->GetSize()-nord);
//	}
	//sort dest
//	SrtSh1(&ndest,nord);
	
	//write back
//	for(i=0;i<nord;i++){
//		dest->SetAt(i+dest->GetSize()-nord,ndest[i]);
//	}

	//SrtSh1(dest,nord);
	SrtShl(dest,NULL,nord);
}

/*
        SUBROUTINE FIT (DIN,DOUT,DEST,I92,ICUT,IREG,X0,X1,U,T)
*
* INTERVAL REGRESSION TO ICUT-1 AND ORDINAL REGRESSION FROM ICUT->I92
*
        REAL DIN(I92),DOUT(I92),DEST(I92)
        double PRECISION U,T
*
        U=0.D0
        T=U
*--------------------------------------INTERVAL-RATIO
        IF (ICUT.GT.1) THEN
*---INTERVAL
          IF (IREG.EQ.1) THEN
            DO 100 I=1,ICUT-1
              DEST(I)=X0+X1*DIN(I)
              U=U+(DOUT(I)-DEST(I))**2
              T=T+DOUT(I)*DOUT(I)
100         CONTINUE
*---RATIO
          ELSE
            DO 110 I=1,ICUT-1
              DEST(I)=X1*DIN(I)
              U=U+(DOUT(I)-DEST(I))**2
              T=T+DOUT(I)*DOUT(I)
110         CONTINUE
          END IF
        END IF
*---------------------------------------ORDINAL
        IF (ICUT.LE.I92) THEN
          DO 120 I=ICUT,I92
            U=U+(DOUT(I)-DEST(I))**2
            T=T+DOUT(I)*DOUT(I)
120       CONTINUE
        END IF
*
        RETURN
        END
*/
void CSsh::Fit(double*din,double*dout,double*dest,
			   long i92,long icut,long ireg,double x0,double x1,double*u,double*t)
{
	long i;

	*u = 0;
	*t = 0;
	
	if(icut > 1){
		if(ireg == 0){
			for(i=0;i<icut;i++){
				dest[i] = x0+x1*din[i];
				*u = *u + pow(dout[i] - dest[i],2);
				*t = *t + dout[i]*dout[i];
			}
		}else{
			for(i=0;i<icut;i++){
				dest[i] = x1*din[i];
				*u = *u + pow(dout[i] - dest[i],2);
				*t = *t + dout[i]*dout[i];
			}
		}
		if(icut < i92){
			for(i=icut;i<i92;i++){
				*u = *u + pow(dout[i] - dest[i],2);
				*t = *t + dout[i]*dout[i];
			}
		}
	}
}

/*
        SUBROUTINE RANK (DATA,N)
*
        REAL DATA(N)
*
        CALL SRTSH1 (DATA,N)
        I=0
10        I=I+1
          IF (I.EQ.N) THEN
            DATA(I)=FLOAT(I)
            RETURN
          END IF
*
          IF (DATA(I).EQ.DATA(I+1)) THEN
            SUM=2.0
            SUMRNK=FLOAT(I+I+1)
            DO 110 J=I+2,N
              IF (DATA(I).EQ.DATA(J)) THEN
                SUM=SUM+1.0
                SUMRNK=SUMRNK+FLOAT(J)
              ELSE
                CALL RANK2 (DATA,N,I,J-1,SUMRNK,SUM)
                GO TO 10
              END IF
110         CONTINUE
            CALL RANK2 (DATA,N,I,N,SUMRNK,SUM)
            RETURN
          ELSE
            DATA(I)=FLOAT(I)
          END IF
120     GO TO 10
*
        ENDx
*/
void CSsh::Rank(double* data,long n)
{
	double sumrnk;
	double sum;
	bool done;
	long i;
	
	//double * ndata;
	//ndata = (double*) malloc(sizeof(double)*n);
	//for(i=0;i<n;i++){
	//	ndata[i] = data[i];
	//}

	//SrtSh1(ndata,n);
	SrtShl(data,NULL,n);
		
	for(i=0;i<n-1;i++){
		if(data[i] == data[i+1]){
			sum = 2.0;
			sumrnk = i+i+1+2;
			done = false;
			for(long j=i+2;j<n && !done;j++){
				if(data[i] == data[j]){
					sum++;
					sumrnk += j+1;
				}else{
					Rank2(data,&i,j-1,sumrnk,sum);
					done = true;
				}
			}
			if(!done){
				Rank2(data,&i,n-1,sumrnk,sum);
				//before return, write ndata back to data
				//for(i=0;i<n;i++){
				//	data[i] = ndata[i];
				//}
				return;
			}
		}else{
			data[i] = i+1;
		}
	}
	data[i] = i+1;
	//before return, write ndata back to data
	//for(i=0;i<n;i++){
	//	data[i] = ndata[i];
	//}
	//free(ndata);

}



/*
        SUBROUTINE RANK2 (DATA,N,I1,I2,SUMRNK,SUM)
        REAL DATA(N)
*
        RNKAV=SUMRNK/SUM
        DO 100 I=I1,I2
          DATA (I)=RNKAV
100     CONTINUE
        I1=I2
        RETURN
        END
*/
void CSsh::Rank2(double*data,long *i1,long i2,double sumrnk,double sum)
{
	double rnkav = sumrnk/sum;
	for(long i=*i1;i<=i2;i++) data[i] = rnkav;
	*i1 = i2;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSsh::CSsh()
{

}

CSsh::~CSsh()
{

}

void CSsh::SrtShl(
				  double*  data,
				  long* indx,
				  long n)
{
	sort_count++;

	long i,j,fm,ii;
	//setup index
	if(n == 0){
		if(indx != NULL) indx[0] = 1;
		return;
	}else if(indx != NULL && indx[0] == -1){
		for(i=0;i<n;i++){
			indx[i] = i;
		}
	}

	long fn = n;
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;
	long tempi;
	CnewDialog * newdlg;
	CString number;

	show_sort_box = false;
	if(show_sort_box){		
		//number.Format("No: %d of a maximum %d",sort_count,max_sorts);		
		CString str;
		str.Format("Progress - (SSH) - iteration: %d - stress:%3.4f - Sort Progress.. %d/%d",g_it,g_stress,sort_count,max_sorts);
		newdlg = new CnewDialog(NULL);
		newdlg->Create(str);
		newdlg->m_Progress.SetRange(0,100);
		newdlg->ShowWindow(SW_SHOWNORMAL);
	}
	long progress = 0;
	long step = max(1,n-m) * max(1,nloops) / 100*2;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,n-m);i++){
			if(progress == step){
				progress = 0;
				if(show_sort_box) {
					newdlg->inc();
					CString str;
					str.Format("Progress - (SSH) - random start: %d - stress:%3.4f -Sort Progress %d/%d",g_it,g_stress,sort_count,max_sorts);
					m_dlg->m_Label.SetWindowText(str);
					m_dlg->RedrawWindow();
				}
			}else{
				progress++;
			}

			if(data[i-1] > data[i+m-1]){
				tempd = data[i+m-1];
				if(indx != NULL) tempi = indx[i+m-1];
				data[i+m-1] = data[i-1];
				if(indx != NULL) indx[i+m-1] = indx[i-1];
				if(i <= m){
					data[i-1] = tempd;
					if(indx != NULL) indx[i-1] = tempi;
				}else{
					for(j=i-m;j>=1;j-=m){
						if(tempd >= data[j-1]) 
							break;
						data[j+m-1] = data[j-1];
						if(indx != NULL) indx[j+m-1] = indx[j-1];
					}
					data[j+m-1] = tempd;
					if(indx != NULL) indx[j+m-1] = tempi;
				}
			}
		}
		m = fm/2;
	}

	if(show_sort_box){
		newdlg->PostMessage(WM_CLOSE,0,0L);
		delete newdlg;
	}
}

void CSsh::LinReg(
				  double  *x,
				  double  *y,
				  long n,
				  long icut,
				  long ireg,
				  double *r,
				  double *x0,
				  double *x1)
{
	double _small = 0.00001;
	double sx = 0.0;
	double sxx = sx;
	double sy = sx;
	double syy = sx;
	double sxy = sx;
	long fn = icut;
	double y0,y1;
	long i;
	
	double valmis = m_pDoc->GetMissingValue();

	//sums & sumosquares
	for(i=0;i<icut;i++){
		if(x[i] != valmis && y[i] != valmis){
			sx += x[i];
			sxx += x[i]*x[i];
			sy += y[i];
			syy += y[i]*y[i];
			sxy += x[i]*y[i];
		}else{
			fn--;
		}
	}
     
	double sxn = fn*sxx-sx*sx;
	double syn = fn*syy-sy*sy;
	double sxyn = fn*sxy-sx*sy;

	double sxnsyn = sqrt(sxn*syn);
	if(sxnsyn > _small){
		*r = sxyn/sxnsyn;
	}else if(sxyn > _small){
		*r = 0.0;
	}else{
		*r = 1.0;
	}

	*x0 = 0.0;
	y0 = 0.0;
	*x1 = sxy/sxx;
	y1 = sxy/syy;
}


void CSsh::SrtSh1(
			double * data,
			long n)
{
	long ii,fm,i;
//	bool skip;
	long j;
	long fn = n;
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,n-m);i++){
			if(data[i-1] > data[i+m-1]){
				tempd = data[i+m-1];
				data[i+m-1] = data[i-1];
				if(i <= m){
					data[i-1] = tempd;
				}else{
					//skip = false;
					for(j=i-m;/*!skip &&*/ j>=1;j-=m){
						if(tempd >= data[j-1]) //skip = true;
							break;
						//if(!skip){
							data[j+m-1] = data[j-1];
						//}
					}
					data[j+m-1] = tempd;
				}
			}
		}
		m = fm/2;
	}
}

void CSsh::LinLt(
		   double positn,
		   long *irow,
		   long *icol)
{
	positn++;
	*irow = (long) (sqrt(2*positn+0.25)+0.499);
	*icol = (long) (positn - (*irow*(*irow-1)/(2.0)));
	*icol = *icol - 1;
}

