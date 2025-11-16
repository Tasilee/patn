// AutoEvaluation.h: interface for the CAutoEvaluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOEVALUATION_H__FC061EAD_3637_4914_A0C7_747E04FA41EE__INCLUDED_)
#define AFX_AUTOEVALUATION_H__FC061EAD_3637_4914_A0C7_747E04FA41EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "patndoc.h"

class CAutoEvaluation  
{
public:
	CAutoEvaluation(CPatnDoc * pDoc);
	virtual ~CAutoEvaluation();

	CPatnDoc * m_pDoc;

   CString m_errlog;

	CString GetRecipeString();
	void Run();

	HWND m_parent;
};

#endif // !defined(AFX_AUTOEVALUATION_H__FC061EAD_3637_4914_A0C7_747E04FA41EE__INCLUDED_)
