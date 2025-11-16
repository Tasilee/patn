// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes

//#define DIRECT3D_VERSION 0x0700
//#include "D3d8.h"
//#include "D3dx8.h"
//#include "d3d8types.h"




#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <afxtempl.h>		// MFC template classes
#include <afxcview.h>
#include <afxdhtml.h>

// Patn global constants
const double g_nStandardColumnWidth = 8.5;
const int	g_nDefaultDecimalPrecision = 2;

const int	g_clm = 3; // cell left margin
const int	g_ctm = 3; // cell top margin
const int	g_crm = 2; // cell right margin
const int	g_cbm = 2; // cell bottom margin

#ifndef PATN_VERSION
	#define PATN_VERSION "Version 4.00"
	#define PATN_VERSION_NUMBER "4.00"
#endif

#ifndef PATN_DATESHORT
	#define PATN_DATESHORT "28 / 1 / 2024"
#endif

#ifndef PATN_DATELONG
	#define PATN_DATELONG "28th January 2024"
#endif

#ifndef PATN_DISTRIBUTION
#define PATN_DISTRIBUTION_USER (((CPatnApp*)AfxGetApp())->m_UserName)
#define PATN_DISTRIBUTION_ORG (((CPatnApp*)AfxGetApp())->m_OrganisationName)
#define PATN_DISTRIBUTION
#endif

// user-defined messages are all defined here to avoid using same id
#define NO_USER_ACTION_ON_STARTUP	(WM_USER + 0)
#define WM_RESIZE_EDIT				(WM_USER + 1)
	/*#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CPatnModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	DWORD dwThreadID;
};
extern CPatnModule _Module;
#include <atlcom.h>

*/