!include "MUI2.nsh"
!define VERSION '0.6'

Name "Half-Life VR"

OutFile ".\output\halflife-vr-${VERSION}.exe"
InstallDir $PROGRAMFILES\Steam\

;TODO: 
;!define MUI_ICON '.\images\icon.ico'
;!define MUI_HEADERIMAGE images\header.bmp ;150x57

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
  
;Welcome Page Settings
!define MUI_WELCOMEFINISHPAGE_BITMAP ".\images\welcome.bmp"; 164x314 
!define MUI_WELCOMEPAGE_TITLE 'Welcome to the installation for Half-Life 2 VR v${VERSION}'
!define MUI_WELCOMEPAGE_TEXT 'Thanks for installing Half-Life 2 Virtual Reality Mod, this should only take a few seconds.  \
Keep in mind this is a very early version so if you have any issues or ideas please send feedback so we can improve the mod'

!define MUI_DIRECTORYPAGE_TEXT 'Please select the location of your Steam installation.'

!define MUI_FINISHPAGE_TITLE 'Installation Completed Successfully.'
!define MUI_FINISHPAGE_TEXT 'Restart steam to see the mod in your games list on Steam.'


!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "" 
	SetOutPath $INSTDIR\steamapps\sourcemods\virtualhalf-life
	;TODO: add real files

	File /r .\package\*

	WriteUninstaller $INSTDIR\Uninstall.exe

	;TODO: REG ENTRIES FOR UNINSTALL

SectionEnd


Section "Uninstall"
	Delete $INSTDIR\Uninstall.exe
	Delete $INSTDIR\MyProg.exe
	RMDir $INSTDIR
SectionEnd