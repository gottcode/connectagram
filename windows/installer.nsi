;--------------------------------
;Definitions

!define APPNAME "Connectagram"
!define VERSIONMAJOR 1
!define VERSIONMINOR 2
!define VERSIONPATCH 4
!define APPVERSION "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONPATCH}"
!define ABOUTURL "https://gottcode.org/connectagram/"

;--------------------------------
;Includes

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "TextFunc.nsh"

;--------------------------------
;General

;Use highest compression
SetCompressor /SOLID /FINAL lzma

;Name and file
Name "${APPNAME}"
OutFile "${APPNAME}_${APPVERSION}.exe"

;Default installation folder
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Variables

Var StartMenuFolder

;--------------------------------
;Interface Settings

!define MUI_ICON "..\icons\connectagram.ico"
!define MUI_UNICON "..\icons\connectagram.ico"
!define MUI_ABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Language Selection Dialog Settings

;Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKLM"
!define MUI_LANGDLL_REGISTRY_KEY "Software\${APPNAME}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
;Start Menu Folder Page Settings

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${APPNAME}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

;--------------------------------
;Finish Page Settings

!define MUI_FINISHPAGE_RUN "$INSTDIR\Connectagram.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ReadMe.txt"

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English" ;first language is the default language
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "SpanishInternational"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Korean"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "NorwegianNynorsk"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Ukrainian"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "Slovak"
!insertmacro MUI_LANGUAGE "Croatian"
!insertmacro MUI_LANGUAGE "Bulgarian"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Thai"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Latvian"
!insertmacro MUI_LANGUAGE "Macedonian"
!insertmacro MUI_LANGUAGE "Estonian"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Lithuanian"
!insertmacro MUI_LANGUAGE "Slovenian"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "SerbianLatin"
!insertmacro MUI_LANGUAGE "Arabic"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Hebrew"
!insertmacro MUI_LANGUAGE "Indonesian"
!insertmacro MUI_LANGUAGE "Mongolian"
!insertmacro MUI_LANGUAGE "Luxembourgish"
!insertmacro MUI_LANGUAGE "Albanian"
!insertmacro MUI_LANGUAGE "Breton"
!insertmacro MUI_LANGUAGE "Belarusian"
!insertmacro MUI_LANGUAGE "Icelandic"
!insertmacro MUI_LANGUAGE "Malay"
!insertmacro MUI_LANGUAGE "Bosnian"
!insertmacro MUI_LANGUAGE "Kurdish"
!insertmacro MUI_LANGUAGE "Irish"
!insertmacro MUI_LANGUAGE "Uzbek"
!insertmacro MUI_LANGUAGE "Galician"
!insertmacro MUI_LANGUAGE "Afrikaans"
!insertmacro MUI_LANGUAGE "Catalan"
!insertmacro MUI_LANGUAGE "Esperanto"
!insertmacro MUI_LANGUAGE "Asturian"

;--------------------------------
;Reserve Files

!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Functions

Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Installer Section

Section "install"

	;Copy files
	SetOutPath $INSTDIR
	File ..\release\Connectagram.exe
	File $%QTDIR%\bin\libgcc_s_dw2-1.dll
	File $%QTDIR%\bin\libstdc++-6.dll
	File $%QTDIR%\bin\libwinpthread-1.dll
	File $%QTDIR%\bin\Qt5Core.dll
	File $%QTDIR%\bin\Qt5Gui.dll
	File $%QTDIR%\bin\Qt5Network.dll
	File $%QTDIR%\bin\Qt5Widgets.dll

	SetOutPath $INSTDIR\bearer
	File $%QTDIR%\plugins\bearer\qgenericbearer.dll
	File $%QTDIR%\plugins\bearer\qnativewifibearer.dll

	SetOutPath $INSTDIR\platforms
	File $%QTDIR%\plugins\platforms\qwindows.dll

	SetOutPath $INSTDIR\translations
	File ..\translations\*.qm
	File $%QTDIR%\translations\qtbase_*.qm

	SetOutPath $INSTDIR\data\en
	File ..\data\en\*
	SetOutPath $INSTDIR\data\fr
	File ..\data\fr\*
	SetOutPath $INSTDIR\data\hu
	File ..\data\hu\*
	SetOutPath $INSTDIR\data\ru
	File ..\data\ru\*

	;Create ReadMe file
	SetOutPath $INSTDIR
	File /oname=ReadMe.txt ..\README
	FileOpen $4 "ReadMe.txt" a
	FileSeek $4 0 END
	FileWrite $4 "$\r$\n$\r$\nCredits$\r$\n=======$\r$\n$\r$\n"
	FileClose $4
	File ..\CREDITS
	${FileJoin} "ReadMe.txt" "CREDITS" "ReadMe.txt"
	Delete $INSTDIR\CREDITS
	FileOpen $4 "ReadMe.txt" a
	FileSeek $4 0 END
	FileWrite $4 "$\r$\n$\r$\nNews$\r$\n====$\r$\n$\r$\n"
	FileClose $4
	File ..\NEWS
	${FileJoin} "ReadMe.txt" "NEWS" "ReadMe.txt"
	Delete $INSTDIR\NEWS

	;Registry information for add/remove programs
	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "Graeme Gott"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\Connectagram.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${APPVERSION}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSIONMINOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" "$0"

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	;Create shortcut
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk" "$INSTDIR\Connectagram.exe"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

	!insertmacro MUI_UNGETLANGUAGE

FunctionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

	; Remove from registry
	DeleteRegKey HKLM "Software\${APPNAME}"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"

	;Remove files
	Delete $INSTDIR\Connectagram.exe
	Delete $INSTDIR\ReadMe.txt
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\bearer\*.dll
	Delete $INSTDIR\platforms\*.dll
	Delete $INSTDIR\data\*\*
	Delete $INSTDIR\translations\*.qm
	Delete $INSTDIR\Uninstall.exe

	;Remove directories
	RMDir /r $INSTDIR\data
	RMDir $INSTDIR\bearer
	RMDir $INSTDIR\platforms
	RMDir $INSTDIR\translations
	RMDir $INSTDIR

	;Remove shortcut
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	Delete "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"

SectionEnd
