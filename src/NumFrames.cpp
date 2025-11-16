// NumFrames.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "NumFrames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumFrames dialog


CNumFrames::CNumFrames(CWnd* pParent /*=NULL*/)
	: CDialog(CNumFrames::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNumFrames)
	m_numframes = 0;
	//}}AFX_DATA_INIT
}


void CNumFrames::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNumFrames)
	DDX_Text(pDX, IDC_NUMFRAMES, m_numframes);
	DDV_MinMaxInt(pDX, m_numframes, 1, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNumFrames, CDialog)
	//{{AFX_MSG_MAP(CNumFrames)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumFrames message handlers
