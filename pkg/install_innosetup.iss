; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; Customize the Qt dir installation and version
[Code]
#define QT_DIR 	      "C:\Qt\Qt5.1.0\5.1.0\mingw48_32\bin"
#define VERSION       "2.1.0"
#define RELEASE_DIR   "build-TarotClub-Desktop_Qt_5_1_0_MinGW_32bit-Release"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{66E297AA-D2C9-4060-851F-3B8B9383ACF2}
AppName=TarotClub {#VERSION}
AppVerName=TarotClub-{#VERSION}
AppPublisher=TarotClub Corp.
AppPublisherURL=http://www.tarotclub.fr
AppSupportURL=http://www.tarotclub.fr
AppUpdatesURL=http://www.tarotclub.fr
DefaultDirName={pf}\TarotClub
DefaultGroupName=TarotClub
LicenseFile=..\COPYING-FR
OutputDir=.
OutputBaseFilename=TarotClub-{#VERSION}-Setup
Compression=lzma
SolidCompression=true
WizardImageFile=compiler:wizmodernimage-IS.bmp
WizardSmallImageFile=compiler:wizmodernsmallimage-IS.bmp
SetupIconFile=install.ico

[Languages]
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: ..\{#RELEASE_DIR}\bin\TarotClub.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\src\data\cards\default\*; DestDir: {app}\default; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\doc\tarotclub.qch; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\doc\tarotclub.qhc; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\ai\beginner.js; DestDir: {app}\ai; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\ai\tarotlib\*; DestDir: {app}\ai\tarotlib; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\prj\tarotclub_en.qm; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\prj\tarotclub_fr.qm; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\lib\tarotclub.xsl; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

; Miscellaneous information text files
Source: ..\COPYING; DestDir: {app}; Flags: ignoreversion
Source: ..\COPYING-FR; DestDir: {app}; Flags: ignoreversion
Source: ..\HISTORY; DestDir: {app}; Flags: ignoreversion
Source: ..\README; DestDir: {app}; Flags: ignoreversion

; Fonts used in SVG files
Source: ..\src\data\fonts\kanzlei.ttf; DestDir: {app}; Flags: ignoreversion
Source: ..\src\data\fonts\kanzlei.ttf; DestDir: {fonts}; FontInstall: Kanzlei; Flags: onlyifdoesntexist uninsneveruninstall

; Qt DLL files
Source: {#QT_DIR}\icudt51.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\icuin51.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\icuuc51.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\libgcc_s_dw2-1.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\libstdc++-6.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\libwinpthread-1.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5CLucene.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Core.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Gui.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Help.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Network.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Script.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Sql.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Svg.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Widgets.dll; DestDir: {app}; Flags: ignoreversion
Source: {#QT_DIR}\Qt5Xml.dll; DestDir: {app}; Flags: ignoreversion

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: {group}\TarotClub; Filename: {app}\TarotClub.exe; WorkingDir: {app}
Name: {group}\{cm:ProgramOnTheWeb,TarotClub}; Filename: http://www.tarotclub.fr
Name: {group}\{cm:UninstallProgram,TarotClub}; Filename: {uninstallexe}
Name: {commondesktop}\TarotClub; Filename: {app}\TarotClub.exe; Tasks: desktopicon; WorkingDir: {app}
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\TarotClub; Filename: {app}\TarotClub.exe; Tasks: quicklaunchicon; WorkingDir: {app}

[Run]
Filename: {app}\TarotClub.exe; Description: {cm:LaunchProgram,TarotClub}; Flags: nowait postinstall skipifsilent

