// OrdinationLegend.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "OrdinationLegend.h"
#include ".\ordinationlegend.h"


// COrdinationLegend dialog

IMPLEMENT_DYNAMIC(COrdinationLegend, CDialog)
COrdinationLegend::COrdinationLegend(CWnd* pParent /*=NULL*/)
	: CDialog(COrdinationLegend::IDD, pParent)
{
	m_pDoc = NULL;
}

COrdinationLegend::~COrdinationLegend()
{
}

void COrdinationLegend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(COrdinationLegend, CDialog)
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnLvnItemchangedList)
END_MESSAGE_MAP()


// COrdinationLegend message handlers

void COrdinationLegend::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	
}

void COrdinationLegend::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect r; m_list.GetClientRect(&r);
	r.bottom = cy;
	r.right = cx;

	m_list.MoveWindow(r);
}

void COrdinationLegend::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

BOOL COrdinationLegend::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void COrdinationLegend::OnPopulate(int type)
{
	if(m_pDoc == NULL)
		return;

	//first, determine the number of intrinsic rows
	long rows = m_pDoc->GetNoOfRows();
	if(m_pDoc->GetIdxTopExtrinsicRow() > 0)
	{
		rows = m_pDoc->GetIdxTopExtrinsicRow();
	}

	//empty the list
	m_list.DeleteAllItems();

	//attach each object to the list
	// 1. Label Name
	// 2. Image (B&W or coloured circle)

	//setup bitmap & device to write on.
//	CClientDC dcbase(this);
//	CDC dc;
//	dc.CreateCompatibleDC(NULL);
//	CBitmap dcBMP;
///	dcBMP.CreateCompatibleBitmap(&dc,16,16);
//	CBitmap * old = dc.SelectObject(&dcBMP);

	CDC dcMem;
	CBitmap bmp;
	
	dcMem.CreateCompatibleDC(AfxGetMainWnd()->GetDC());
	dcMem.SetMapMode(MM_LOENGLISH);
	bmp.CreateCompatibleBitmap(AfxGetMainWnd()->GetDC(),16,16);	
	
	CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);

	CPoint pt;
	pt = dcMem.GetViewportOrg();
	CPoint pt2 = dcMem.GetWindowOrg();
	//dcMem.BitBlt(0,0,16,16,AfxGetMainWnd()->GetDC(),0,0,WHITENESS);
	//dcMem.SetBkMode(TRANSPARENT);
//			IMAGEINFO * ii;	

	//init image list
	CImageList * il = new CImageList();
	il->Create(16,16,ILC_COLOR32,rows,rows);

	for(long i=0;i<rows && i < 1;i++)
	{
		//add labels		
		CString label;
		m_pDoc->GetRowLabel(i,label);
		
		m_list.InsertItem(LVIF_TEXT | LVIF_IMAGE, i, label,0,0,i,0);

		//create the list image item
		//1. clear
		//2. draw
		if(type == 1) //coloured circle
		{			
			dcMem.FillSolidRect(0,0,16,16,RGB(255,0,0));
			dcMem.Ellipse(CRect(3,3,12,12));
			dcMem.SaveDC();
			il->Add(&bmp,RGB(0,0,1));

			//il->GetImageInfo(0,ii);
			
		}
		else //b&w
		{

		}
		
	}

	m_list.SetImageList(il,LVSIL_NORMAL);

//	dc.SelectObject(old);
	//dc.DeleteDC();
}
BOOL COrdinationLegend::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


