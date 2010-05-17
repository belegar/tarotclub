; Script generated with the Venis Install Wizard

!include WinMessages.nsh

; Define your application name
!define APPNAME "TarotClub"
!define APPNAMEANDVERSION "TarotClub 2.0.0 Alpha 1"
!define QT_DIR	"C:\Qt\2010.02.1\qt\bin"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\TarotClub"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "TarotClub2.0.0-Alpha1-Setup.exe"

; Use compression
SetCompressor LZMA

; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\TarotClub.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYING-FR"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "TarotClub" Section1

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\default\"
	File "..\src\data\cards\default\atout-01.svg"
	File "..\src\data\cards\default\atout-02.svg"
	File "..\src\data\cards\default\atout-03.svg"
	File "..\src\data\cards\default\atout-04.svg"
	File "..\src\data\cards\default\atout-05.svg"
	File "..\src\data\cards\default\atout-06.svg"
	File "..\src\data\cards\default\atout-07.svg"
	File "..\src\data\cards\default\atout-08.svg"
	File "..\src\data\cards\default\atout-09.svg"
	File "..\src\data\cards\default\atout-10.svg"
	File "..\src\data\cards\default\atout-11.svg"
	File "..\src\data\cards\default\atout-12.svg"
	File "..\src\data\cards\default\atout-13.svg"
	File "..\src\data\cards\default\atout-14.svg"
	File "..\src\data\cards\default\atout-15.svg"
	File "..\src\data\cards\default\atout-16.svg"
	File "..\src\data\cards\default\atout-17.svg"
	File "..\src\data\cards\default\atout-18.svg"
	File "..\src\data\cards\default\atout-19.svg"
	File "..\src\data\cards\default\atout-20.svg"
	File "..\src\data\cards\default\atout-21.svg"
	File "..\src\data\cards\default\carreau-01.svg"
	File "..\src\data\cards\default\carreau-02.svg"
	File "..\src\data\cards\default\carreau-03.svg"
	File "..\src\data\cards\default\carreau-04.svg"
	File "..\src\data\cards\default\carreau-05.svg"
	File "..\src\data\cards\default\carreau-06.svg"
	File "..\src\data\cards\default\carreau-07.svg"
	File "..\src\data\cards\default\carreau-08.svg"
	File "..\src\data\cards\default\carreau-09.svg"
	File "..\src\data\cards\default\carreau-10.svg"
	File "..\src\data\cards\default\carreau-11.svg"
	File "..\src\data\cards\default\carreau-12.svg"
	File "..\src\data\cards\default\carreau-13.svg"
	File "..\src\data\cards\default\carreau-14.svg"
	File "..\src\data\cards\default\coeur-01.svg"
	File "..\src\data\cards\default\coeur-02.svg"
	File "..\src\data\cards\default\coeur-03.svg"
	File "..\src\data\cards\default\coeur-04.svg"
	File "..\src\data\cards\default\coeur-05.svg"
	File "..\src\data\cards\default\coeur-06.svg"
	File "..\src\data\cards\default\coeur-07.svg"
	File "..\src\data\cards\default\coeur-08.svg"
	File "..\src\data\cards\default\coeur-09.svg"
	File "..\src\data\cards\default\coeur-10.svg"
	File "..\src\data\cards\default\coeur-11.svg"
	File "..\src\data\cards\default\coeur-12.svg"
	File "..\src\data\cards\default\coeur-13.svg"
	File "..\src\data\cards\default\coeur-14.svg"
	File "..\src\data\cards\default\excuse.svg"
	File "..\src\data\cards\default\pique-01.svg"
	File "..\src\data\cards\default\pique-02.svg"
	File "..\src\data\cards\default\pique-03.svg"
	File "..\src\data\cards\default\pique-04.svg"
	File "..\src\data\cards\default\pique-05.svg"
	File "..\src\data\cards\default\pique-06.svg"
	File "..\src\data\cards\default\pique-07.svg"
	File "..\src\data\cards\default\pique-08.svg"
	File "..\src\data\cards\default\pique-09.svg"
	File "..\src\data\cards\default\pique-10.svg"
	File "..\src\data\cards\default\pique-11.svg"
	File "..\src\data\cards\default\pique-12.svg"
	File "..\src\data\cards\default\pique-13.svg"
	File "..\src\data\cards\default\pique-14.svg"
	File "..\src\data\cards\default\trefle-01.svg"
	File "..\src\data\cards\default\trefle-02.svg"
	File "..\src\data\cards\default\trefle-03.svg"
	File "..\src\data\cards\default\trefle-04.svg"
	File "..\src\data\cards\default\trefle-05.svg"
	File "..\src\data\cards\default\trefle-06.svg"
	File "..\src\data\cards\default\trefle-07.svg"
	File "..\src\data\cards\default\trefle-08.svg"
	File "..\src\data\cards\default\trefle-09.svg"
	File "..\src\data\cards\default\trefle-10.svg"
	File "..\src\data\cards\default\trefle-11.svg"
	File "..\src\data\cards\default\trefle-12.svg"
	File "..\src\data\cards\default\trefle-13.svg"
	File "..\src\data\cards\default\trefle-14.svg"

	SetOutPath "$INSTDIR\"
	File "..\prj\bin\TarotClub.exe"
	File "TarotClub.url"
	File "..\src\data\fonts\kanzlei.ttf"

	; Qt Library
	File "${QT_DIR}\QtCore4.dll"
	File "${QT_DIR}\QtSvg4.dll"
	File "${QT_DIR}\mingwm10.dll"
	File "${QT_DIR}\QtGui4.dll"
	File "${QT_DIR}\QtXml4.dll"
	File "${QT_DIR}\QtNetwork4.dll"
	File "${QT_DIR}\libgcc_s_dw2-1.dll"
	
	; Fonts
	System::Call "GDI32::AddFontResourceA(t) i ('$INSTDIR\kanzlei.ttf') .s"
	Pop $0
	# $0 is zero if the function failed
	SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0
	
	CreateShortCut "$DESKTOP\TarotClub.lnk" "$INSTDIR\TarotClub.exe"
	CreateDirectory "$SMPROGRAMS\TarotClub"
	CreateShortCut "$SMPROGRAMS\TarotClub\TarotClub.lnk" "$INSTDIR\TarotClub.exe"
	CreateShortCut "$SMPROGRAMS\TarotClub\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$SMPROGRAMS\TarotClub\Page Web du projet TarotClub.lnk" "$INSTDIR\TarotClub.url"

SectionEnd


Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete Shortcuts
	Delete "$DESKTOP\TarotClub.lnk"
	RMDir /r $SMPROGRAMS
	
	; Clean up TarotClub
	RMDir /r $INSTDIR

SectionEnd

; On initialization
Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

; eof