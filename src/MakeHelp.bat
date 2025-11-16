@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by PATN.HPJ. >"hlp\patn.hm"
echo. >>"hlp\patn.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\patn.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\patn.hm"
echo. >>"hlp\patn.hm"
echo // Prompts (IDP_*) >>"hlp\patn.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\patn.hm"
echo. >>"hlp\patn.hm"
echo // Resources (IDR_*) >>"hlp\patn.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\patn.hm"
echo. >>"hlp\patn.hm"
echo // Dialogs (IDD_*) >>"hlp\patn.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\patn.hm"
echo. >>"hlp\patn.hm"
echo // Frame Controls (IDW_*) >>"hlp\patn.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\patn.hm"
REM -- Make help for Project PATN


echo Building Win32 Help files
start /wait hcrtf -x "hlp\patn.hpj"
echo.
if exist Debug\nul copy "hlp\patn.hlp" Debug
if exist Debug\nul copy "hlp\patn.cnt" Debug
if exist Release\nul copy "hlp\patn.hlp" Release
if exist Release\nul copy "hlp\patn.cnt" Release
echo.


