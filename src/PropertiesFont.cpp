// PropertiesFont.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "tableview.h"

#include "PropertiesFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertiesFont property page

IMPLEMENT_DYNCREATE(CPropertiesFont, CPropertyPage)

CFont * newfont;

CPropertiesFont::CPropertiesFont() : CPropertyPage(CPropertiesFont::IDD)
{
	//{{AFX_DATA_INIT(CPropertiesFont)
	//}}AFX_DATA_INIT
	newfont = new CFont;
}

CPropertiesFont::~CPropertiesFont()
{
	delete newfont;
}

void CPropertiesFont::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesFont)
	DDX_Control(pDX, IDC_EXAMPLE_TEXT, m_text_example);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesFont, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertiesFont)
	ON_BN_CLICKED(ID_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesFont message handlers

void CPropertiesFont::OnFont() 
{
	CFontDialog dlg(&(pDoc->m_lfCells));
	dlg.m_cf.rgbColors = pDoc->m_rgbFontCells;
	
	if ( dlg.DoModal() != IDOK )
		return;

	pDoc->m_nPointSizeCells = dlg.GetSize();
	pDoc->m_rgbFontCells = dlg.GetColor();

	pDoc->SetModifiedFlag();

	POSITION pos = pDoc->GetFirstViewPosition();
	CView * pView;
	while(pos != NULL){
		pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView))){
			((CTableView*)pView)->SetTableFont();
			break;
		}
		pView = NULL;
	}

	CFont * newfont = new CFont;
	newfont->CreateFontIndirect(&pDoc->m_lfCells);
	m_text_example.SetFont(newfont);

	// save new font to the registry
	HKEY hk;
    char szBuf[80];
	if (!RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\PATN\\General", &hk)) {
 
		// Set the name of the message file. 
	 
		strcpy(szBuf, pDoc->m_lfCells.lfFaceName); 
		RegSetValueEx(hk,             // subkey handle 
				"fontName",       // value name 
				0,                        // must be zero 
				REG_SZ,            // value type 
				(LPBYTE) szBuf,           // pointer to value data 
				strlen(szBuf) + 1);       // length of value data 

		RegCloseKey(hk); 
	}

	pDoc->UpdateAllViews(NULL);	
}

BOOL CPropertiesFont::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_text_example.SetWindowText("Some text in the selected font.");
	
	newfont->CreateFontIndirect(&pDoc->m_lfCells);
	m_text_example.SetFont(newfont);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
