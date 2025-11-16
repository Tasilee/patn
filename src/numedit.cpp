// numedit.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "numedit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit()
{
}

CNumEdit::~CNumEdit()
{
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	int nStart, nEnd,i;
	CString strBuffer;
	GetWindowText(strBuffer);
	long nDecPos = strBuffer.Find('.');
	long nLineCount = GetLineCount();
	GetSel(nStart, nEnd);

	//* limit size of long part to 30 digits - otherwise causes run-time error
	if ( nDecPos > 30 || (nDecPos == -1 && strBuffer.GetLength() > 30) )
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	switch(nChar)
	{
		case '.':
			if ( nDecPos != -1 ) // '.' already exists in the string
			{
				if ( nStart == nEnd )
				{
					if ( nStart != nDecPos )
					{
						MessageBeep(MB_ICONEXCLAMATION);
						return;
					}
					// otherwise entered '.' in same pos so just move one char to right
					SendMessage(WM_KEYDOWN,VK_RIGHT,1);
					return;
				}
				if ( (nStart > nDecPos && nEnd > nDecPos) ||
				     (nStart < nDecPos && nEnd < nDecPos) )
				{
					MessageBeep(MB_ICONEXCLAMATION);
					return;
				}
			}
			// otherwise just fall through
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if ( !strBuffer.IsEmpty() &&
		          strBuffer[0] == '-' &&
		          nStart == 0 &&
		          ::GetKeyState(VK_INSERT) & 1 /* insert mode */ )
			// trying to insert char before '-'
			{
				MessageBeep(MB_ICONEXCLAMATION);
				return;
			}
			break;

		case '-':
		case VK_SUBTRACT:
			if ( strBuffer.GetLength() > 0 )
			{
				if ( nStart == nEnd && nStart == 0 )
				{
					if ( strBuffer[0] == '-' )
					{
						SendMessage(WM_KEYDOWN,VK_RIGHT,1);
						return;
					}
					break;
				}

				if ( nStart == 0 || nEnd == 0 )
					break;

				// otherwise trying to enter '-' somewhere other than at beginning of string
				MessageBeep(MB_ICONEXCLAMATION);
				return;
			}
			break;

		case VK_BACK:
			break;
			
		case VK_ESCAPE:
			Undo();
			return ;

		case VK_RETURN:
			GetParent()->PostMessage(WM_KEYDOWN,VK_RETURN,1);
			return;

		default:
			MessageBeep(MB_ICONEXCLAMATION);	// Unhandled key
			return;
	}

	//check if change valid


	CEdit::OnChar(nChar, nRepCnt, nFlags);
	CString tmp;
	GetWindowText(tmp);

	//check maximum character ranges
	nDecPos = tmp.Find('.');

	strLast = tmp;
	
	if(nDecPos != -1 && nDecPos+8 <= tmp.GetLength())
	{// sets max character length (decimal)
		CEdit::SetSel(0,tmp.GetLength());
		CEdit::ReplaceSel(strBuffer);
		i = tmp.GetLength();
		CEdit::SetSel(nEnd,nEnd);
		strLast = strBuffer;
	}

	if(nDecPos > 12 || 
		(nDecPos == -1 && tmp.GetLength() > 6))
	{// sets max character length (non decimal)
		CEdit::SetSel(0,tmp.GetLength());
		CEdit::ReplaceSel(strBuffer);
		i = tmp.GetLength();
		CEdit::SetSel(nEnd,nEnd);
		strLast = strBuffer;
	}
	
	// check whether the new char has changed the line count - if so, notify parent
	if ( nLineCount != GetLineCount() )
	{
		GetParent()->SendMessage(WM_RESIZE_EDIT,0,0);
	}
}

void CNumEdit::OnChange() 
{
	// TODO: Add your control notification handler code here
	CString tmp;
	long nDecPos;
	//	long i;
	GetWindowText(tmp);

	//check maximum character ranges
	nDecPos = tmp.Find('.');

	if(nDecPos != -1 && nDecPos+8 <= tmp.GetLength())
	{// sets max character length (decimal)
		CEdit::SetSel(0,tmp.GetLength());
		CEdit::ReplaceSel(strLast);
		CEdit::SetSel(nDecPos,nDecPos);
	}else if(nDecPos > 12 || 
		(nDecPos == -1 && tmp.GetLength() > 6))
	{// sets max character length (non decimal)
		CEdit::SetSel(0,tmp.GetLength());
		CEdit::ReplaceSel(strLast);
		CEdit::SetSel(nDecPos,nDecPos);
	}else if(tmp.GetLength() > 0){
		strLast = tmp;
	}	
}

void CNumEdit::OnSetfocus() 
{
	GetWindowText(strLast);	
}

