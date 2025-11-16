# Microsoft Developer Studio Project File - Name="patn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=patn - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "patn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "patn.mak" CFG="patn - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "patn - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "patn - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "patn - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /vd0 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib ddraw.lib dxguid.lib vfw32.lib /nologo /subsystem:windows /pdb:none /machine:I386
# SUBTRACT LINK32 /verbose /debug

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0xc09 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 opengl32.lib glu32.lib ddraw.lib dxguid.lib vfw32.lib  /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "patn - Win32 Release"
# Name "patn - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\3DFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\3DView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\3DXView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Analysis.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Anosim.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnosimFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnosimPage.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnosimResults.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnosimView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AppendOptiosn.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Association.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AssociationDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AssociationThread.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\assohist.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\assomxvw.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\assoswnd.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AttributeSelection.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AutoEvaluation.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Bitms.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BoxWhiskerFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BoxWhiskerView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CFont.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChooseAnosim.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Classification.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CntrItem.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Colr.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ColumnTotalsView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CompareRow.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\COPENGL.CPP

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DataTransform.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DataTransformDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dend.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dendview.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditText.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditText2.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Evaluation.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EvaluationRecipe.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Exponent.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Fuse.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fuseswnd.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fuseview.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FusionDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FusionThread.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GetUserIdentity.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Glf.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GroupsDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gsta.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HistogramFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\histvw.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IndividualGroups.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KruskalWalice.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KruskalWallis.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KWOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KWResults.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Logconst.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MCAO.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MCAOResults.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mst.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\newDialog.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Nonzeroc.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\numedit.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NumFrames.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectSize.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ODBCConnection.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ordination.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageColumnOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageNewTableOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PageRowOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\patn.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\patn.hpj

!IF  "$(CFG)" == "patn - Win32 Release"

# Begin Custom Build
OutDir=.\Release
ProjDir=.
TargetName=patn
InputPath=.\hlp\patn.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
ProjDir=.
TargetName=patn
InputPath=.\hlp\patn.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\patn.rc
# End Source File
# Begin Source File

SOURCE=.\PatnDoc.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Pcc.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PccMcaoPage.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PccVectorSelection.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Permut.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PickAMxDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PickAnosim.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PickFuseDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesBWObjects.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesFont.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesGeneral.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesOrdinationView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesRandom.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertiesVisibleStatistics.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RandomNumberGeneration.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RowTotalsView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scat.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScatFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectColumnsOrRowsDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Selection.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectWorksheet.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Splash.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ssh.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StartupDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TableFrame.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TableProperties.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TableView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tabs.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Texture.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TotalsView.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tway.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TwayOptions.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TwayWnd.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\verify_patn.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ViewExtension.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ViewHistoryDlg.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WriteAVI.cpp

!IF  "$(CFG)" == "patn - Win32 Release"

!ELSEIF  "$(CFG)" == "patn - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\3DFrame.h
# End Source File
# Begin Source File

SOURCE=.\3DView.h
# End Source File
# Begin Source File

SOURCE=.\3DXView.h
# End Source File
# Begin Source File

SOURCE=.\Analysis.h
# End Source File
# Begin Source File

SOURCE=.\Anosim.h
# End Source File
# Begin Source File

SOURCE=.\AnosimFrame.h
# End Source File
# Begin Source File

SOURCE=.\AnosimPage.h
# End Source File
# Begin Source File

SOURCE=.\AnosimResults.h
# End Source File
# Begin Source File

SOURCE=.\AnosimView.h
# End Source File
# Begin Source File

SOURCE=.\AppendOptiosn.h
# End Source File
# Begin Source File

SOURCE=.\Association.h
# End Source File
# Begin Source File

SOURCE=.\AssociationThread.h
# End Source File
# Begin Source File

SOURCE=.\Assohist.h
# End Source File
# Begin Source File

SOURCE=.\assomxvw.h
# End Source File
# Begin Source File

SOURCE=.\assoswnd.h
# End Source File
# Begin Source File

SOURCE=.\AttributeSelection.h
# End Source File
# Begin Source File

SOURCE=.\AutoEvaluation.h
# End Source File
# Begin Source File

SOURCE=.\Bitms.h
# End Source File
# Begin Source File

SOURCE=.\BoxWhiskerFrame.h
# End Source File
# Begin Source File

SOURCE=.\BoxWhiskerView.h
# End Source File
# Begin Source File

SOURCE=.\CFont.h
# End Source File
# Begin Source File

SOURCE=.\ChooseAnosim.h
# End Source File
# Begin Source File

SOURCE=.\Classification.h
# End Source File
# Begin Source File

SOURCE=.\CntrItem.h
# End Source File
# Begin Source File

SOURCE=.\Colr.h
# End Source File
# Begin Source File

SOURCE=.\ColumnTotalsView.h
# End Source File
# Begin Source File

SOURCE=.\CompareRow.h
# End Source File
# Begin Source File

SOURCE=.\COPENGL.H
# End Source File
# Begin Source File

SOURCE=.\DataTransform.h
# End Source File
# Begin Source File

SOURCE=.\DataTransformDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dend.h
# End Source File
# Begin Source File

SOURCE=.\dendview.h
# End Source File
# Begin Source File

SOURCE=.\EditText.h
# End Source File
# Begin Source File

SOURCE=.\EditText2.h
# End Source File
# Begin Source File

SOURCE=.\Evaluation.h
# End Source File
# Begin Source File

SOURCE=.\EvaluationRecipe.h
# End Source File
# Begin Source File

SOURCE=.\Exponent.h
# End Source File
# Begin Source File

SOURCE=.\Fuse.h
# End Source File
# Begin Source File

SOURCE=.\fuseswnd.h
# End Source File
# Begin Source File

SOURCE=.\fuseview.h
# End Source File
# Begin Source File

SOURCE=.\FusionDlg.h
# End Source File
# Begin Source File

SOURCE=.\GetUserIdentity.h
# End Source File
# Begin Source File

SOURCE=.\Glf.h
# End Source File
# Begin Source File

SOURCE=.\GroupsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Gsta.h
# End Source File
# Begin Source File

SOURCE=.\HistogramFrame.h
# End Source File
# Begin Source File

SOURCE=.\histvw.h
# End Source File
# Begin Source File

SOURCE=.\IndividualGroups.h
# End Source File
# Begin Source File

SOURCE=.\KruskalWalice.h
# End Source File
# Begin Source File

SOURCE=.\KruskalWallis.h
# End Source File
# Begin Source File

SOURCE=.\KWOptions.h
# End Source File
# Begin Source File

SOURCE=.\KWResults.h
# End Source File
# Begin Source File

SOURCE=.\Logconst.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MCAO.h
# End Source File
# Begin Source File

SOURCE=.\MCAOResults.h
# End Source File
# Begin Source File

SOURCE=.\Mst.h
# End Source File
# Begin Source File

SOURCE=.\newDialog.h
# End Source File
# Begin Source File

SOURCE=.\Nonzeroc.h
# End Source File
# Begin Source File

SOURCE=.\numedit.h
# End Source File
# Begin Source File

SOURCE=.\NumFrames.h
# End Source File
# Begin Source File

SOURCE=.\ObjectSize.h
# End Source File
# Begin Source File

SOURCE=.\ODBCConnection.h
# End Source File
# Begin Source File

SOURCE=.\Ordination.h
# End Source File
# Begin Source File

SOURCE=.\PageColumnOptions.h
# End Source File
# Begin Source File

SOURCE=.\PageNewTableOptions.h
# End Source File
# Begin Source File

SOURCE=.\PageRowOptions.h
# End Source File
# Begin Source File

SOURCE=.\patn.h
# End Source File
# Begin Source File

SOURCE=.\PatnDoc.h
# End Source File
# Begin Source File

SOURCE=.\PatternSelection.h
# End Source File
# Begin Source File

SOURCE=.\Pcc.h
# End Source File
# Begin Source File

SOURCE=.\PccMcaoPage.h
# End Source File
# Begin Source File

SOURCE=.\PccVectorSelection.h
# End Source File
# Begin Source File

SOURCE=.\Permut.h
# End Source File
# Begin Source File

SOURCE=.\PickAMxDlg.h
# End Source File
# Begin Source File

SOURCE=.\PickAnosim.h
# End Source File
# Begin Source File

SOURCE=.\PickFuseDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesBWObjects.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesFont.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesGeneral.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesOrdinationView.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesRandom.h
# End Source File
# Begin Source File

SOURCE=.\PropertiesVisibleStatistics.h
# End Source File
# Begin Source File

SOURCE=.\RandomNumberGeneration.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RowTotalsView.h
# End Source File
# Begin Source File

SOURCE=.\Scat.h
# End Source File
# Begin Source File

SOURCE=.\ScatFrame.h
# End Source File
# Begin Source File

SOURCE=.\SelectColumnsOrRowsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Selection.h
# End Source File
# Begin Source File

SOURCE=.\SelectWorksheet.h
# End Source File
# Begin Source File

SOURCE=.\SelOptions.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Ssh.h
# End Source File
# Begin Source File

SOURCE=.\StartupDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TableFrame.h
# End Source File
# Begin Source File

SOURCE=.\TableProperties.h
# End Source File
# Begin Source File

SOURCE=.\TableView.h
# End Source File
# Begin Source File

SOURCE=.\tabs.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TotalsView.h
# End Source File
# Begin Source File

SOURCE=.\Tway.h
# End Source File
# Begin Source File

SOURCE=.\TwayOptions.h
# End Source File
# Begin Source File

SOURCE=.\TwayWnd.h
# End Source File
# Begin Source File

SOURCE=.\verify_patn.h
# End Source File
# Begin Source File

SOURCE=.\ViewExtension.h
# End Source File
# Begin Source File

SOURCE=.\ViewHistoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\WriteAVI.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe;dat"
# Begin Source File

SOURCE=.\res\append10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\append9.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ARROW_RI.ICO
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Windows\Desktop\excel\res\Arrow_ri.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn_impo.ico
# End Source File
# Begin Source File

SOURCE=.\res\btn_new.ico
# End Source File
# Begin Source File

SOURCE=.\res\btn_open.ico
# End Source File
# Begin Source File

SOURCE=.\res\btn_wiza.ico
# End Source File
# Begin Source File

SOURCE=.\fonts\courier1.glf
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\DragEW.cur
# End Source File
# Begin Source File

SOURCE=.\res\DraggingEW.cur
# End Source File
# Begin Source File

SOURCE=.\res\DraggingNS.cur
# End Source File
# Begin Source File

SOURCE=.\res\DragNS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LeftOfRow.cur
# End Source File
# Begin Source File

SOURCE=.\res\patn.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Windows\Desktop\excel\res\patn.ico
# End Source File
# Begin Source File

SOURCE=.\res\patn.rc2
# End Source File
# Begin Source File

SOURCE=.\patterns.dat
# End Source File
# Begin Source File

SOURCE=.\res\patterns.dat
# End Source File
# Begin Source File

SOURCE=.\res\SizeColumn.cur
# End Source File
# Begin Source File

SOURCE=.\res\SizeHiddenColumn.cur
# End Source File
# Begin Source File

SOURCE=.\res\SizeHiddenRow.cur
# End Source File
# Begin Source File

SOURCE=.\res\SizeRow.cur
# End Source File
# Begin Source File

SOURCE=.\res\splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\startup1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\STOP.ICO
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Windows\Desktop\excel\res\Stop.ico
# End Source File
# Begin Source File

SOURCE=.\res\Table.ico
# End Source File
# Begin Source File

SOURCE=.\res\text2.bin
# End Source File
# Begin Source File

SOURCE=.\fonts\times_new1.glf
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TopOfColumn.cur
# End Source File
# End Group
# End Target
# End Project
# Section patn : {560A11D0-4CF6-11CF-9E71-444553540000}
# 	0:8:Splash.h:D:\Patn32\Programs\PatnTable\v1\Splash.h
# 	0:10:Splash.cpp:D:\Patn32\Programs\PatnTable\v1\Splash.cpp
# 	1:11:IDB_SPLASH1:105
# 	2:10:ResHdrName:resource.h
# 	2:11:ProjHdrName:stdafx.h
# 	2:10:WrapperDef:_SPLASH_SCRN_
# 	2:12:SplClassName:CSplashWnd
# 	2:21:SplashScreenInsertKey:4.0
# 	2:10:HeaderName:Splash.h
# 	2:10:ImplemName:Splash.cpp
# 	2:7:BmpID16:IDB_SPLASH
# End Section
