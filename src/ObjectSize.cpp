// ObjectSize.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "ObjectSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectSize dialog


CObjectSize::CObjectSize(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectSize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectSize)
	m_objectsize = 0.0;
	//}}AFX_DATA_INIT
}


void CObjectSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectSize)
	DDX_Control(pDX, IDC_SIZEOFOBJECTS, m_label);
	DDX_Text(pDX, IDC_OBJECTSIZE, m_objectsize);
	DDV_MinMaxDouble(pDX, m_objectsize, 0., 5.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectSize, CDialog)
	//{{AFX_MSG_MAP(CObjectSize)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectSize message handlers
