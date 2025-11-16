// PropertiesRandom.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "PropertiesRandom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesRandom dialog


CPropertiesRandom::CPropertiesRandom(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPropertiesRandom::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesRandom)
	m_randomtype = -1;
	m_randomseed = 12345;
	//}}AFX_DATA_INIT
}


void CPropertiesRandom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesRandom)
	DDX_Control(pDX, IDC_ABOUT_METHOD, m_about_method);
	DDX_Radio(pDX, IDC_UNIFORMRANDOM, m_randomtype);
	DDX_Text(pDX, IDC_SEED, m_randomseed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesRandom, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesRandom)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_UNIFORMRANDOM, OnUniformrandom)
	ON_BN_CLICKED(IDC_LUXURY, OnLuxury)
	ON_BN_CLICKED(IDC_MAR, OnMar)
	ON_BN_CLICKED(IDC_ENU, OnEnu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesRandom message handlers

void CPropertiesRandom::OnExport() 
{
	UpdateData(true);

	CFileDialog dlg(false);

	if(dlg.DoModal() != IDOK){
		return;
	}

	CStdioFile file;

	if(!file.Open(dlg.GetPathName(),CFile::modeCreate | CFile::modeWrite)){
		return;
	}

	((CPatnApp*)AfxGetApp())->SetRandomGenerator(m_randomtype);
	((CPatnApp*)AfxGetApp())->RandomSeed(m_randomseed);

	for(long i=0;i<10000;i++){
		CString str;
		str.Format("%f\n",((CPatnApp*)AfxGetApp())->Rand());
		file.WriteString(str);
	}

	file.Close();

}

BOOL CPropertiesRandom::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if(m_randomtype == 0){//uniform
		OnUniformrandom();
	}else if(m_randomtype == 1){//luxury
		OnLuxury();
	}else if(m_randomtype == 2){//mar
		OnMar();
	}else if(m_randomtype == 3){//ecu
		OnEnu();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertiesRandom::OnUniformrandom() 
{
	m_about_method.SetWindowText("generates uniform random numbers \
between 0 and 1");	
}

void CPropertiesRandom::OnLuxury() 
{
	m_about_method.SetWindowText("an advanced pseudo-random number generator based on the \
RCARRY algorithm proposed in 1991 by Marsaglia and Zaman.  \
RCARRY used a subtract-and-borrow algorithm with a period on the order of \
10^171 but still had detectable correlations between \
numbers.  Martin Luescher proposed the RANLUX \
algorithm in 1993;  RANLUX generates pseudo-random numbers using \
RCARRY but throws away numbers to destroy correlations.  \
Thus RANLUX trades execution speed for quality;  by choosing a \
larger luxury setting one gets better random numbers slower. \
By the tests availible at the time it was proposed, \
RANLUX at the default luxury setting appears to be a \
significant advance quality over previous \
generators.  Luxury is set at 4.  Generates numbers between 0 and 1. \r\n\r\n \
http://www.mpa-garching.mpg.de/~tomek/htmls/refs/ranlux.about.html \r\n \
RANLUX random number generator:  Excerpts from discussion in the Usenet news groups \n\n \
Possible errors are ignored.");
	
}

void CPropertiesRandom::OnMar() 
{
	m_about_method.SetWindowText("a lagged Fibonacci generator proposed by Marsaglia and \
Zaman and is a good research grade generator.  This version of \
is based on the paper by James,  which is a good \
reference for the properties of RANMARand several other \
generators.  Generates numbers between 0 and 1.");
}

void CPropertiesRandom::OnEnu() 
{
	m_about_method.SetWindowText("Ranecu is an advanced multiplicative linear congruential random number \
generator with a period of aproximately 10^18. \
Ranecu is a direct translation from Fortran of the RANECU \
subroutine published in the paper \r\n \
F. James, Comp. Phys. Comm. 60 (1990) p 329-344 \r\n\r\n \
The algorithm was originally described in \r\n \
P. L'Ecuyer, Commun. ACM. 1988 (1988) p 742 \r\n\r\n \
Generates numbers between 0 and 1.");
	
}
