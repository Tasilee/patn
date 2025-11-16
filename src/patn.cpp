// patn.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "patn.h"
#include <io.h>

#include "MainFrame.h"
#include "TableFrame.h"
//#include "InPlaceFrame.h"
#include "PatnDoc.h"
#include "TableView.h"
#include "HistVw.h"
#include "scattplotVw.h"
#include "scattplotframe.h"
#include "fuseswnd.h"
#include "fuseview.h"
#include "dendview.h"
#include "assoswnd.h"
#include "assomxvw.h"
#include "assohist.h"
#include "Splash.h"
#include "Tway.h"
#include "histogramframe.h"
//#include "GstaView.h"
//#include "3DView.h"
//#include "scat.h"
#include "twaywnd.h"
#include "anosimview.h"
#include "boxwhiskerview.h"
#include "3dframe.h"
#include "anosimframe.h"
#include "boxwhiskerframe.h"
//#include "gstaframe.h"
//#include "scatframe.h"
#include "histogramframe.h"
#include "verify_patn.h"
//#include "randomnumbergeneration.h"
#include "getuseridentity.h"

#include "constfid.h"
#include "constfidview.h"

#include "pickamxdlg.h"
#include "pickfusedlg.h"
#include <afxadv.h>

#include "3dxview.h"
//#include "3dx9view.h"
#include ".\patn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define OLD_PATN_RANDOM 0

int m_randomgenerator = OLD_PATN_RANDOM;

PatnRandom * m_pre = NULL;

/////////////////////////////////////////////////////////////////////////////
// CPatnApp

BEGIN_MESSAGE_MAP(CPatnApp, CWinApp)
	//{{AFX_MSG_MAP(CPatnApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_FILE_ODBCCONNECTION, OnFileODBC)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)	
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_ODBCCONNECTION, OnUpdateFileODBC)
	ON_UPDATE_COMMAND_UI(ID_FILE_APPEND, OnUpdateFileAppend)
	ON_UPDATE_COMMAND_UI(ID_FILE_CHECKFORUPDATE, OnUpdateFileCheckForUpdate)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_CHECKFORUPDATE, OnFileCheckforupdateCloseFiles)
	ON_COMMAND(ID_HELP_PATNONLINE, OnHelpPatnonline)
	ON_COMMAND(ID_HELP_DISCUSSIONFORUM, OnHelpDiscussionforum)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatnApp construction

void CPatnApp::OnFileOpen()
{
	CWinApp::OnFileOpen();
}

void CPatnApp::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}

void CPatnApp::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}

void CPatnApp::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}

void CPatnApp::OnUpdateFileImport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}

void CPatnApp::OnUpdateFileODBC(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}

void CPatnApp::OnUpdateFileAppend(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true);
}
void CPatnApp::OnUpdateFileCheckForUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( false);
}

//external id string generation function
CString generateid(CString input)
{
	CString ret = input + "PATN 4 u";
	while(ret.GetLength() > 12)
	{
		CString end;
		end = ret.Right(12);
		ret = ret.Left(ret.GetLength()-12);
		while(ret.GetLength() < 12)
			ret.AppendChar(end.GetAt(ret.GetLength()-1));
		for(int i=0;i<12;i++)
		{
			char c = (ret.GetAt(i)+end.GetAt(i))%35;
			if(c > 25)
				c = c - 25 + '0';
			else
				c = c + 'a';
			ret.SetAt(i,c);
		}
	}
	return ret;
}


//set/get registry stuff
CString GetKey2(CString pw="")
{
    HKEY hk; 
  //  DWORD dwData; 
    char szBuf[80]; 
 
    // Add your source name as a subkey under the Application 
    // key in the EventLog registry key. 
  
    // Add the name to the EventMessageFile subkey. 
	if(pw.GetLength())
	{
		if (RegCreateKey(HKEY_LOCAL_MACHINE, 
			"SOFTWARE\\PATN\\General", &hk)) {
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error updating the registry. Are you logged in as Administrator?"); 
			}
 
		// Set the name of the message file. 
	 
		strcpy(szBuf, pw); 
		if (RegSetValueEx(hk,             // subkey handle 
				"User Number2",       // value name 
				0,                        // must be zero 
				REG_SZ,            // value type 
				(LPBYTE) szBuf,           // pointer to value data 
				strlen(szBuf) + 1))      { // length of value data 
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not set the registry value."); 
				}
		RegCloseKey(hk); 
	}
	else
	{

		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PATN\\General",NULL, KEY_READ,&hk)){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error opening the registry."); 
		}
		
		DWORD sz = sizeof(char)*200;
		DWORD ty = REG_EXPAND_SZ;
		if(RegQueryValueEx(hk,"User Number2",NULL,&ty,(LPBYTE)szBuf,&sz)){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error reading from the registry.");
		}

		pw = szBuf;
		//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(pw);
		RegCloseKey(hk); 
	} 
    
return pw;

}


CString GetKey(CString pw="")
{
    HKEY hk; 
  //  DWORD dwData; 
    char szBuf[80]; 
 
    // Add your source name as a subkey under the Application 
    // key in the EventLog registry key. 
 
    
 
    // Add the name to the EventMessageFile subkey. 
	if(pw.GetLength())
	{
		RegCreateKey(HKEY_LOCAL_MACHINE, 
            "SOFTWARE\\PATN\\General", &hk);
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error updating the registry."); 
 
		// Set the name of the message file. 
	 
		strcpy(szBuf, pw); 
		RegSetValueEx(hk,             // subkey handle 
				"User Number",       // value name 
				0,                        // must be zero 
				REG_SZ,            // value type 
				(LPBYTE) szBuf,           // pointer to value data 
				strlen(szBuf) + 1);       // length of value data 
			//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not set the registry value."); 
		RegCloseKey(hk); 

		if (RegCreateKey(HKEY_CURRENT_USER, 
			"SOFTWARE\\PATN\\PATN\\General", &hk)) {
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error updating the registry."); 
			}
 
		// Set the name of the message file. 
	 
		strcpy(szBuf, pw); 
		if (RegSetValueEx(hk,             // subkey handle 
				"User Number",       // value name 
				0,                        // must be zero 
				REG_SZ,            // value type 
				(LPBYTE) szBuf,           // pointer to value data 
				strlen(szBuf) + 1))     {  // length of value data 
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Could not set the registry value."); 
				}
		RegCloseKey(hk); 
	}
	else
	{

		RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PATN\\General",NULL, KEY_READ ,&hk);
			//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error opening the registry."); 
		
		DWORD sz = sizeof(char)*200;
		DWORD ty = REG_EXPAND_SZ;
		RegQueryValueEx(hk,"User Number",NULL,&ty,(LPBYTE)szBuf,&sz);
			//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error reading from the registry.");

		pw = szBuf;
		RegCloseKey(hk); 

		if(pw == "")
		{
			RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\PATN\\PATN\\General",NULL, KEY_READ ,&hk);
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error opening the registry."); 
			
			sz = sizeof(char)*200;
			ty = REG_EXPAND_SZ;
			RegQueryValueEx(hk,"User Number",NULL,&ty,(LPBYTE)szBuf,&sz);
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Error reading from the registry.");

			pw = szBuf;
			RegCloseKey(hk); 
		}

	}
    
return pw;

}
//ie stuff
void OpenURL(CString url)
{
	//TRY init ptr to excel
	::CoInitialize(NULL);

	HRESULT hr;
	OLECHAR FAR* szFunction;
	DISPID functionID;
	VARIANT vResult;
	DISPPARAMS dpNoArgs = {NULL, NULL, 0, 0};

	IUnknown* pUnk;
	IDispatch* pIE;
	
	VARIANT vArgs[2];
	DISPPARAMS input;
	input.rgvarg = vArgs;

	CLSID clsid;
	hr = CLSIDFromProgID(L"InternetExplorer.Application", &clsid);  	
	HRESULT hr2 = hr;

	int pos = 0;
	while(hr == S_OK)
	{
		hr = ::CoCreateInstance( clsid, NULL, CLSCTX_SERVER,IID_IUnknown, (void**) &pUnk);		
		if(hr != S_OK)
			break;
		pos++;

		hr = pUnk->QueryInterface(IID_IDispatch, (void**)&pIE);
		if(hr != S_OK)
			break;
		pos++;

		//get NAVIGATE function
		szFunction = OLESTR("Navigate");
		hr = pIE->GetIDsOfNames(IID_NULL, &szFunction,1,LOCALE_USER_DEFAULT,&functionID);	
		if(hr != S_OK)
			break;
		pos++;
		
		BSTR bstrTemp = url.AllocSysString();
		vArgs[0].vt = VT_BSTR;
		vArgs[0].bstrVal = bstrTemp;
		input.rgvarg = vArgs;
		input.cArgs = 1;
		input.cNamedArgs = 0;		

		hr = pIE->Invoke(functionID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &input, &vResult, NULL, NULL);

		::SysFreeString(bstrTemp);

		szFunction = OLESTR("Visible");
		hr = pIE->GetIDsOfNames(IID_NULL, &szFunction,1,LOCALE_USER_DEFAULT,&functionID);	
		if(hr != S_OK)
			break;
		pos++;
		DISPID dispidNamed = DISPID_PROPERTYPUT;
		vArgs[0].vt = VT_BOOL;
		vArgs[0].boolVal = TRUE;
		input.cArgs = 1;
		input.cNamedArgs = 1;
		input.rgdispidNamedArgs = &dispidNamed;
		hr = pIE->Invoke(functionID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &input, &vResult, NULL, NULL);
		pos++;
		break;
	}

	//cleanup

//	VARIANT vFalse[2];
	//DISPPARAMS dpClose;

	if(hr2 == S_OK) pIE->Release();
	
	::CoUninitialize();

}

CPatnApp::CPatnApp()
{
	EnableHtmlHelp();
	//m_ordtype = 1;

	m_randomgenerator = OLD_PATN_RANDOM;

	// get font from registry
    HKEY hk; 
    char szBuf[80]; 
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\PATN\\General",NULL, KEY_READ,&hk)){
		// failed to open
		strcpy(m_globalFontName, "Calibri");
	} else {
		DWORD sz = sizeof(char)*200;
		DWORD ty = REG_EXPAND_SZ;
		if(RegQueryValueEx(hk,"fontName",NULL,&ty,(LPBYTE)szBuf,&sz)){
			// failed to read
			strcpy(m_globalFontName, "Calibri");
		} else {
			strcpy(m_globalFontName, szBuf);
		}
		RegCloseKey(hk); 	
	} 
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CPatnApp object

CPatnApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {048D9D80-71DB-11CF-9E71-444553540000}
static const CLSID clsid =
{ 0x48d9d80, 0x71db, 0x11cf, { 0x9e, 0x71, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

/////////////////////////////////////////////////////////////////////////////
// CPatnApp initialization

#include "getserialnumber.h"


BOOL CPatnApp::InitInstance()
{

	SetRegistryKey("PATN");


	m_randomgenerator = OLD_PATN_RANDOM;
//do recipe stuff
	CString cmdline;
	cmdline = m_lpCmdLine;
	//get the recipe file name
	//get the data file name
	long pos1 = cmdline.Find("-r ");
	long pos2 = cmdline.Find("-d ");
	silent_mode = false;
CCommandLineInfo cmdInfo;
		
	if(pos1 >= 0 && pos2 >= 0)
	{
		sm_recipe = cmdline.Mid(pos1 + 3,abs(pos1-pos2)-3);
		sm_data = cmdline.Right(cmdline.GetLength()-pos2-3);

		//if sm_recipe is a filename, open & get the contents
		FILE * fp = fopen(sm_recipe,"r");
		char c ;
		if(fp!=NULL)
		{
			sm_recipe = "";
			while((c = fgetc(fp)) != EOF)
			{
				sm_recipe = sm_recipe + c;
			}
			
			

			fclose(fp);
		}
		

	}
	if(sm_recipe.GetLength() > 0 && sm_data.GetLength() > 0)
	{
		silent_mode = true;
	}

	if(!silent_mode)
	{
	// CG: The following block was added by the Splash Screen component.
	//{
		ParseCommandLine(cmdInfo);

		// don't show startup box when directly opening a file
		if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileDDE){
		}else if( !cmdInfo.m_strFileName.GetLength())
		{			

			CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

		}
	//}
	}	//registry entry
//	m_pszRegistryKey=_tcsdup(_T("HKEY_CURRENT_USER\\Software\\PATN"));
	//CString version;
	//SetRegistryKey("PATN");
	//read	version = 3
	m_UserName = GetProfileString("General","User Name","");
	m_OrganisationName = GetProfileString("General","Organisation Name","");
	m_fileerrormsgs = (GetProfileInt("General","Error Messages",0))?true:false;
	m_mousewheelsensitivity = GetProfileInt("General","Mouse Wheel Sensitivity",40);
	m_ordobjectsize = atof(GetProfileString("General","Ordination Object Size","0.05"));
	//m_ordtype = GetProfileInt("General","Ordination Type",1);
	m_randomgenerator = GetProfileInt("General","Random Type",OLD_PATN_RANDOM);
	
	//window stuff
	int maximized = GetProfileInt("General","Maximixed",0);
	int win_top = GetProfileInt("General","Top Position",0);
	int win_left = GetProfileInt("General","Left Position",0);
	int win_width = GetProfileInt("General","Width",640);
	int win_height = GetProfileInt("General","Height",480);

	//write

	WritePatnProfile();

	
	m_copylabels = false;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(this->m_pszHelpFilePath,drive,dir,fname,ext);
	m_ProgramPath = drive;
	m_ProgramPath += dir;

	if (!AfxSocketInit())
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if(!AfxOleInit())
	{
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

//	if (!InitATL())
//		return FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_TABLETYPE,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CTableFrame), // custom MDI child frame
		RUNTIME_CLASS(CTableView));
	pDocTemplate->SetContainerInfo(IDR_MAINFRAME);
	//pDocTemplate->SetServerInfo(
	//	IDR_MAINFRAME, IDR_MAINFRAME,
	//	RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);
 
	pDocTemplate = new CMultiDocTemplate(
		IDR_FUSETYPE,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CFuseSplitterWnd),
		RUNTIME_CLASS(CFuseView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_ASSOCTYPE,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CAssocSplitterWnd),
		RUNTIME_CLASS(CAssocMatrixView));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_HISTOGRAM,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CHistogramFrame),
		RUNTIME_CLASS(CHistogram));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_SCATTERPLOT,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CScattplotFrame),
		RUNTIME_CLASS(CScattplot));
	AddDocTemplate(pDocTemplate);

	//**PROTO1**
	//Add pDocTemplate for CTway here. (same as CHistogram)
	pDocTemplate = new CMultiDocTemplate(
		IDR_TWAYTABLE,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CTwayWnd),
		RUNTIME_CLASS(CTway));
	AddDocTemplate(pDocTemplate);

	//Add pDocTemplate for CGstaView here. (same as CHistogram)
//	pDocTemplate = new CMultiDocTemplate(
//		IDR_GSTAVIEW,
//		RUNTIME_CLASS(CPatnDoc),
//		RUNTIME_CLASS(CGstaFrame),
//		RUNTIME_CLASS(CGstaView));
//	AddDocTemplate(pDocTemplate);

	//Add pDocTemplate for C3DXView here. (same as CHistogram)
	pDocTemplate = new CMultiDocTemplate(
		IDR_3DXVIEW,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(C3DFrame),
		RUNTIME_CLASS(C3DXView));
	AddDocTemplate(pDocTemplate);

//	pDocTemplate = new CMultiDocTemplate(
//		IDR_3DX9VIEW,
//		RUNTIME_CLASS(CPatnDoc),
//		RUNTIME_CLASS(C3DFrame),
//		RUNTIME_CLASS(C3DX9View));
//	AddDocTemplate(pDocTemplate);

	//Add pDocTemplate for C3DView here. (same as CHistogram)
/*	pDocTemplate = new CMultiDocTemplate(
		IDR_3DVIEW,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(C3DFrame),
		RUNTIME_CLASS(C3DView));
	AddDocTemplate(pDocTemplate);

	//Add pDocTemplate for CScat here. (same as CHistogram)
	pDocTemplate = new CMultiDocTemplate(
		IDR_SCAT,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CScatFrame),
		RUNTIME_CLASS(CScat));
	AddDocTemplate(pDocTemplate);
*/
	//New doc template for CAnosimView
	pDocTemplate = new CMultiDocTemplate(
		IDR_ANOSIM,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CAnosimFrame),
		RUNTIME_CLASS(CAnosimView));
	AddDocTemplate(pDocTemplate);

	//New doc template for CBoxWhiskerView
	pDocTemplate = new CMultiDocTemplate(
		IDR_BOXWHISKER,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CBoxWhiskerFrame),
		RUNTIME_CLASS(CBoxWhiskerView));
	AddDocTemplate(pDocTemplate);

	//New doc template for CConstFid
	pDocTemplate = new CMultiDocTemplate(
		IDR_CONSTFID,
		RUNTIME_CLASS(CPatnDoc),
		RUNTIME_CLASS(CConstFid),
		RUNTIME_CLASS(CConstFidView));
	AddDocTemplate(pDocTemplate);


	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();
//	_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
	//	REGCLS_MULTIPLEUSE);

		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line.

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open

	EnableShellOpen();
	RegisterShellFileTypes();

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	//if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
//	{
		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
//		return TRUE;
//	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_INPLACE_SERVER);

    //_Module.UpdateRegistryFromResource(IDR_PATN, TRUE);
//	_Module.RegisterServer(TRUE);


	// don't create a new patn file automatically when starting patn
	if ( cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew )
	{
		// Dispatch commands specified on the command line
		CString s;
		s.Format("parsing %d",cmdInfo.m_nShellCommand == CCommandLineInfo::FileDDE);
		//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(s);
		// AfxMessageBox(s);
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

if(!silent_mode)
	{	
	WINDOWPLACEMENT windowplacement;
	windowplacement.length = sizeof(WINDOWPLACEMENT);
	windowplacement.flags = 0;
	if(maximized) {
		windowplacement.showCmd = SW_SHOWMAXIMIZED;
		windowplacement.flags = WPF_RESTORETOMAXIMIZED;
	}else{ 
		windowplacement.showCmd = SW_SHOW;
	}
	windowplacement.ptMaxPosition.x = windowplacement.ptMaxPosition.y = -1;
	windowplacement.ptMinPosition.x = windowplacement.ptMinPosition.y = 0;
	windowplacement.rcNormalPosition.top = win_top;
	windowplacement.rcNormalPosition.bottom = win_top + win_height;
	windowplacement.rcNormalPosition.left = win_left;
	windowplacement.rcNormalPosition.right = win_left + win_width;

	// The main window has been initialized, so show and update it.
	pMainFrame->CenterWindow(pMainFrame->GetDesktopWindow());
	
	m_pMainWnd->SetWindowPlacement(&windowplacement);
	
//	((CMainFrame*) m_pMainWnd)->m_wndViewToolBar.GetDockingFrame()->LoadBarState("View Toolbar");
//	((CMainFrame*) m_pMainWnd)->m_wndToolBar.GetDockingFrame()->LoadBarState("Standard Toolbar");

	pMainFrame->UpdateWindow();	
	
	
	}
else
{
		//open file?
		m_bGetExternalData = TRUE;
		m_bGetOdbc = FALSE;

		CWinApp::OnFileNew();
		
		

		return FALSE;
}
	
	
	return TRUE;

	
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
//	afx_msg void OnStnClickedLongdate();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd * item;

	item = this->GetDlgItem(IDC_DISTRIBUTION);
	//item->SetWindowText("Registered to: " + PATN_DISTRIBUTION_USER);	
	//item = this->GetDlgItem(IDC_DISTRIBUTION_ORG);
	//item->SetWindowText("             " + PATN_DISTRIBUTION_ORG);
	item = this->GetDlgItem(IDC_LONGDATE);
	item->SetWindowText(PATN_DATELONG);
	item = this->GetDlgItem(IDC_VERSION);
	CString string;
	string.Format("PATN %s",PATN_VERSION);
	item->SetWindowText(string);

	CEdit * edit;
	edit = (CEdit*)this->GetDlgItem(IDC_ABOUT_TEXT);
	HRSRC rsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_ABOUT_TEXT),"TEXT");
	HGLOBAL h = LoadResource(NULL,rsrc);
	char * c = (char*)h;
	edit->SetWindowText(c);

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_HELPINFO()
//	ON_STN_CLICKED(IDC_LONGDATE, OnStnClickedLongdate)
END_MESSAGE_MAP()

#ifndef _DEBUG
//#using <mscorlib.dll>
//#using <System.dll>

//using namespace System;
//using namespace System::Diagnostics;
//using namespace System::ComponentModel;
#endif


// App command to run the dialog
void CPatnApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();	
}

/////////////////////////////////////////////////////////////////////////////
// CPatnApp commands


void CPatnApp::OpenFuseView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition( );
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Fusion Details" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for fusion details.\nMay be already open.");
				bool row = false,col = false;
				CPatnDoc * doc = (CPatnDoc * ) pDoc;

				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for Fuseiation matrix. \nMay be already open.");
				POSITION pos = pDoc->GetFirstViewPosition();
				CView * view;
				while(pos){
					view = pDoc->GetNextView(pos);
					if(view->IsKindOf(RUNTIME_CLASS(CFuseView))){
						CFuseSplitterWnd * v = (CFuseSplitterWnd*) view->GetParentFrame();
						if(v->m_IsRow)
							row = true;
						else
							col = true;						
					}
				}
				if(row && col){
					//choose which window
					CPickFuseTableDlg dlg;
					if ( dlg.DoModal() == IDOK ){
						int isrow = 0;
						if(CPickFuseTableDlg::ROWS == dlg.m_nDataType){
							isrow = 1;
						}
						pos = pDoc->GetFirstViewPosition();
						while(pos){
							view = pDoc->GetNextView(pos);
							if(view->IsKindOf(RUNTIME_CLASS(CFuseView))){
								CFuseSplitterWnd * v = (CFuseSplitterWnd*) view->GetParentFrame();
								if((v->m_IsRow && isrow) || 
									(!v->m_IsRow && !isrow)){
									v->BringWindowToTop();
									//v->ShowWindow(SW_RESTORE );
									doc->m_FuseColVisible = 1;
									doc->m_FuseRowVisible = 1;
									break;
								}
							}
						}						
					}
				}else if(row || col){
					view->GetParentFrame()->BringWindowToTop();
					//view->GetParentFrame()->ShowWindow(SW_RESTORE );
					if(col)
						doc->m_FuseColVisible = 1;
					else
						doc->m_FuseRowVisible = 1;

				}
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
			// TODO: follow through the above sequence to see how default window title is set
			// by framework, then implement code to set title to something which includes
			// "Fusion Details", doc name and # if more than one fuse framewnd is open
		}
	}
}

void CPatnApp::OpenAssocView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Association Matrix" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				bool row = false,col = false;
				CPatnDoc * doc = (CPatnDoc * ) pDoc;

				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for association matrix. \nMay be already open.");
				POSITION pos = pDoc->GetFirstViewPosition();
				CView * view;
				while(pos){
					view = pDoc->GetNextView(pos);
					if(view->IsKindOf(RUNTIME_CLASS(CAssocMatrixView))){
						CAssocSplitterWnd * v = (CAssocSplitterWnd*) view->GetParentFrame();
						if(v->m_IsRow)
							row = true;
						else
							col = true;						
					}
				}
				if(row && col){
					//choose which window
					CPickAssocMatrixDlg dlg;
					if ( dlg.DoModal() == IDOK ){
						int isrow = 0;
						if(CPickAssocMatrixDlg::ROWS == dlg.m_nDataType){
							isrow = 1;
						}
						pos = pDoc->GetFirstViewPosition();
						while(pos){
							view = pDoc->GetNextView(pos);
							if(view->IsKindOf(RUNTIME_CLASS(CAssocMatrixView))){
								CAssocSplitterWnd * v = (CAssocSplitterWnd*) view->GetParentFrame();
								if((v->m_IsRow && isrow) || 
									(!v->m_IsRow && !isrow)){
									v->BringWindowToTop();
									//v->ShowWindow(SW_RESTORE );
									doc->m_AssocColVisible = 1;
									doc->m_AssocRowVisible = 1;
									break;
								}
							}
						}						
					}
				}else if(row || col){
					view->GetParentFrame()->BringWindowToTop();
					//view->GetParentFrame()->ShowWindow(SW_RESTORE );
					if(col)
						doc->m_AssocColVisible = 1;
					else
						doc->m_AssocRowVisible = 1;

				}
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

void CPatnApp::OpenHistogram(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{	
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Histogram" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for histogram");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}

	}
}


void CPatnApp::OpenScatterplot(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{	
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Scatterplot" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for scatterplot");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}


//**PROTO1**
//Create function same as OpenHistogram (above), but for OpenTway.
void CPatnApp::OpenTway(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Tway Table" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for two-way table");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

/*void CPatnApp::OpenGstaView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Gsta View" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for Gsta table");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}*/

void CPatnApp::Open3DXView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "3DX View" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for 3DX View");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

void CPatnApp::Open3DX9View(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "3DX9 View" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for 3DX View");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

/*void CPatnApp::Open3DView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "3D View" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for 3D View");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}*/

/*void CPatnApp::OpenScat(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Scatter Plot" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for Scatterplot");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}*/

void CPatnApp::OpenAnosimView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "Anosim View" )
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for anosim view. \nMay be already open.");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

void CPatnApp::OpenBoxWhiskerView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "BoxWhisker View")
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for Box & Whisker view. \nMay be already open.");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

void CPatnApp::OpenConstFidView(COleDocument* pDoc)
{
	CFrameWnd* pFrame;
	CString strTypeName = "";
	POSITION pos;
	 
	pos = GetFirstDocTemplatePosition();
	while ( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate(pos);
		if ( pTemplate->GetDocString(strTypeName, CDocTemplate::docName) &&
		   strTypeName == "ConstFid View")
		{
			pFrame = pTemplate->CreateNewFrame(pDoc,NULL);
			if ( pFrame	== NULL )
			{
				if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Failed to create window for Cosntancy & Fidelity view. \nMay be already open.");
				return;
			}
			pTemplate->InitialUpdateFrame(pFrame,pDoc,TRUE);
		}
	}
}

void CPatnApp::OnFileNew() 
{
	// m_bGetExternalData will be checked by CPatnDoc::OnNewDocument() to determine
	// whether to create a new table or to import data from an external source
	m_bGetExternalData = FALSE;

	CWinApp::OnFileNew();
}


void CPatnApp::OnFileImport() 
{
	// m_bGetExternalData will be checked by CPatnDoc::OnNewDocument() to determine
	// whether to create a new table or to import data from an external source
	m_bGetExternalData = TRUE;
	m_bGetOdbc = FALSE;

	CWinApp::OnFileNew();
}

BOOL CPatnApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following line was added by the Splash Screen component.
	return CWinApp::PreTranslateMessage(pMsg);
}

void CPatnApp::OnFileODBC()
{
	m_bGetExternalData = TRUE;
	m_bGetOdbc = TRUE;

	CWinApp::OnFileNew();
}

void CPatnApp::SetOrdType(int value)
{
	//m_ordtype = value;
	//WritePatnProfile();
}
	
void CPatnApp::WritePatnProfile()
{
	WriteProfileString("General","Version",PATN_VERSION);
	WriteProfileInt("General","Error Messages",m_fileerrormsgs);
	WriteProfileInt("General","Mouse Wheel Sensitivity",m_mousewheelsensitivity);
	CString size; size.Format("%f",m_ordobjectsize);
	WriteProfileString("General","Ordination Object Size",size);
	//WriteProfileInt("General","Ordination Type",m_ordtype);
	WriteProfileInt("General","Random Type",m_randomgenerator);
}

void CPatnApp::RandomSeed(long seed)
{
	m_pre = new PatnRandom(seed);	
}

double CPatnApp::Rand()
{
	return m_pre->next();
}

int CPatnApp::GetRandomGenerator()
{
	int	i = OLD_PATN_RANDOM;
	return i;
}

void CPatnApp::SetRandomGenerator(int type)
{
	
	m_randomgenerator = OLD_PATN_RANDOM;
	WritePatnProfile();
}

int CPatnApp::ExitInstance() 
{
	if(m_pre != NULL){
		delete m_pre;
		m_pre = NULL;
	}

	return CWinApp::ExitInstance();
}

int CPatnApp::IsRegistryIdentityValid()
{
	return 1;
}
int CPatnApp::IsLocalIdentityValid()
{
	return 1;
}

int CPatnApp::IsUserIdenityValid()
{
	return 1;
}

void CPatnApp::WindowChange()
{
	WINDOWPLACEMENT wp;
	m_pMainWnd->GetWindowPlacement(&wp);

	if(m_pMainWnd->IsZoomed())
		WriteProfileInt("General","Maximixed",1);
	else
		WriteProfileInt("General","Maximixed",0);

	WriteProfileInt("General","Top Position",wp.rcNormalPosition.top);
	WriteProfileInt("General","Left Position",wp.rcNormalPosition.left);
	WriteProfileInt("General","Height",wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
	WriteProfileInt("General","Width",wp.rcNormalPosition.right - wp.rcNormalPosition.left);

//	((CMainFrame*) m_pMainWnd)->m_wndViewToolBar.GetDockingFrame()->SaveBarState("View Toolbar");
//	((CMainFrame*) m_pMainWnd)->m_wndToolBar.GetDockingFrame()->SaveBarState("Standard Toolbar");

//	CFrameWnd * d1 = ((CMainFrame*) m_pMainWnd)->m_wndViewToolBar.GetDockingFrame();
//	CFrameWnd * d2 = ((CMainFrame*) m_pMainWnd)->m_wndToolBar.GetDockingFrame();
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

//BOOL CPatnApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//}

BOOL CAboutDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	//// TODO: Add your message handler code here and/or call default

	//return CDialog::OnHelpInfo(pHelpInfo);
	return false;
}

#include "installhelpdlg.h"
#include "howtocheckforupdate.h"
void CPatnApp::OnFileCheckforupdate()
{
	// removed
	


}

void CPatnApp::OnFileCheckforupdateCloseFiles()
{
	// removed

}

#include <htmlhelp.h>
void CPatnApp::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: Add your specialized code here and/or call the base class

	CWinApp::WinHelp(dwData, nCmd);
		//CWinApp::WinHelp(dwData, nCmd);
	/*switch (nCmd)
	{
		case HELP_CONTEXT: 
			::HtmlHelpA(NULL, ".\\patn.chm",HH_HELP_CONTEXT, dwData );
			break;
		case HELP_FINDER: 
			::HtmlHelpA(NULL, ".\\patn.chm",HH_DISPLAY_TOPIC, 0);
		         break;	
	}*/

}

void CPatnApp::OnHelpPatnonline()
{
	
	OpenURL("https://patn.org");

#ifndef _DEBUG

#endif
}

void CPatnApp::OnHelpDiscussionforum()
{

	OpenURL("https://patn.org/forums/forum/patn-and-pattern-analysis");
#ifndef _DEBUG

#endif
}

BOOL CPatnApp::OnDDECommand(LPTSTR lpszCommand)
{
	// TODO: Add your specialized code here and/or call the base class



	return CWinApp::OnDDECommand(lpszCommand);
}


void CPatnApp::SetLandscape(bool value)
{
	PRINTDLG pd;
	pd.lStructSize=(DWORD)sizeof(PRINTDLG);
	BOOL bRet=GetPrinterDeviceDefaults(&pd);
	if(bRet)
	{
		// protect memory handle with ::GlobalLock and ::GlobalUnlock
		DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(m_hDevMode);
		// set orientation to landscape
		if(value)
			pDevMode->dmOrientation=DMORIENT_LANDSCAPE;      
		else
			pDevMode->dmOrientation=DMORIENT_PORTRAIT;
		::GlobalUnlock(m_hDevMode);
	}	
}
//void CAboutDlg::OnStnClickedLongdate()
//{
//	// TODO: Add your control notification handler code here
//}
