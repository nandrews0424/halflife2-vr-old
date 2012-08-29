!include "MUI2.nsh"

Name "Half-Life VR"

OutFile ".\output\halflife-vr-0.1.exe"
InstallDir $PROGRAMFILES\Steam\
 
!insertmacro MUI_PAGE_DIRECTORY

Section "" 
	SetOutPath $INSTDIR
	;TODO: add real files

	File .\package\* 

	WriteUninstaller $INSTDIR\Uninstall.exe

	;TODO: REG ENTRIES FOR UNINSTALL

SectionEnd


Section "Uninstall"
	Delete $INSTDIR\Uninstall.exe
	Delete $INSTDIR\MyProg.exe
	RMDir $INSTDIR
SectionEnd