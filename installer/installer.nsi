!include "MUI2.nsh"
!define VERSION '0.9.3'

Name "Half-Life VR"

OutFile ".\output\halflife-vr-${VERSION}.exe"

Var SDK_INSTALLED
Var STEAM_EXE

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

!define MUI_DIRECTORYPAGE_TEXT 'Please verify the location of your Steam sourcemods folder.'

!define MUI_FINISHPAGE_TITLE 'Installation Completed Successfully.'
!define MUI_FINISHPAGE_TEXT 'Restart steam to see the mod in your games list on Steam.'


!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "" 

	

	SetOutPath $INSTDIR\halflife-vr
	RMDir /r $INSTDIR\virtualhalf-life

	File /r .\package\*

	WriteUninstaller $INSTDIR\Uninstall.exe

	;TODO: REG ENTRIES FOR UNINSTALL

SectionEnd

Function .onInit
	
	ReadRegDWORD $SDK_INSTALLED HKCU "Software\Valve\Steam\Apps\218" "Installed"
	; Check for Source SDK 2007
	ReadRegStr $R1 HKCU "Software\Valve\Steam" "SourceModInstallPath"
	ReadRegStr $STEAM_EXE HKCU "Software\Valve\Steam" "SteamExe"
	
	StrCmp $SDK_INSTALLED "1" SDK_INSTALLED
		

		MessageBox MB_YESNO|MB_ICONQUESTION \
		    "The Source SDK 2007 is required to play this mod but wasn't \ 
		    found on your computer. Do you want cancel this installation and install it now?" \
		    IDNO SKIP_SDK_INTALL

		    execshell open "steam://install/218"
		    
		    Abort

		SKIP_SDK_INTALL:
			

	SDK_INSTALLED:

	StrCpy $INSTDIR "$R1"

	;TODO: check key existense 
FunctionEnd

Section "Uninstall"
	Delete $INSTDIR\Uninstall.exe
	Delete $INSTDIR\MyProg.exe
	RMDir $INSTDIR
SectionEnd