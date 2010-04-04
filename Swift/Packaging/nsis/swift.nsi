# Ask to be an admin
RequestExecutionLevel admin # Note - this doesn't mean you get it!

# Use the newer, nicer installer style
!include MUI2.nsh

# set dependencies
!define msvccRedistributableExe "vcredist_x86.exe"

# define installer name
Name "Swift"
outFile "Swift-installer-win32-${buildVersion}.exe"
 
# set default install directory
installDir "$PROGRAMFILES\Swift"

# Declare variables needed later
Var StartMenuFolder

SetCompressor lzma
# How to do pages with the modern ui.
!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_DIRECTORY

# Remember the start menu folder for uninstall
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Swift" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES



# default section start
section "Main install"

# define output path
setOutPath $INSTDIR
 
# Specify files to go in output path.
# If you update this list, update the uninstall list too.
File "..\..\QtUI\Swift\Swift.exe"
File "..\..\QtUI\Swift\ssleay32.dll"
File "..\..\QtUI\Swift\libeay32.dll"
File "..\..\QtUI\Swift\phonon4.dll"
File "..\..\QtUI\Swift\QtCore4.dll"
File "..\..\QtUI\Swift\QtGui4.dll"
File "..\..\QtUI\Swift\QtWebKit4.dll"
File "..\..\QtUI\Swift\QtNetwork4.dll"
File "..\..\QtUI\Swift\QtXMLPatterns4.dll"

SetOutPath $INSTDIR\imageformats

File "..\..\QtUI\Swift\imageformats\qgif4.dll"
File "..\..\QtUI\Swift\imageformats\qico4.dll"
File "..\..\QtUI\Swift\imageformats\qjpeg4.dll"
File "..\..\QtUI\Swift\imageformats\qmng4.dll"
File "..\..\QtUI\Swift\imageformats\qsvg4.dll"
File "..\..\QtUI\Swift\imageformats\qtiff4.dll"

# create start menu item
CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Swift.lnk" "$INSTDIR\Swift.exe"
CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall Swift.lnk" "$INSTDIR\uninstaller.exe"
# remember where we put them
WriteRegStr HKCU "Software\Swift\" "Start Menu Folder" "$SMPROGRAMS\$StartMenuFolder"

# Add the information to Add/Remove
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "DisplayName" "Swift"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "UninstallString" "$\"$INSTDIR\uninstaller.exe$\""
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Swift" "QuietUninstallString" "$\"$INSTDIR\uninstaller.exe$\""


# define uninstaller name
writeUninstaller $INSTDIR\uninstaller.exe
 
# default section end
sectionEnd

Section -Prerequisites
# http://nsis.sourceforge.net/Embedding_other_installers
    SetOutPath $INSTDIR\Prerequisites
    MessageBox MB_YESNO "Install C++ Runtime?" /SD IDYES IDNO endRuntime
    File ${msvccRedistributableDir}\${msvccRedistributableExe}
    ExecWait "$INSTDIR\Prerequisites\${msvccRedistributableExe}"
    Delete $INSTDIR\Prerequisites\${msvccRedistributableExe}
    RmDir $INSTDIR\Prerequisites
    Goto endRuntime
  endRuntime:
SectionEnd

section "autostart"
  MessageBox MB_YESNO "Would you like Swift to run at startup?" /SD IDYES IDNO endAutostart
    WriteRegStr HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "Swift" "$INSTDIR\Swift.exe"
    Goto endAutostart
  endAutostart:
sectionEnd

# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
section "Uninstall"
    # Always delete uninstaller first
    delete $INSTDIR\uninstaller.exe
 
    # now delete installed files
    delete $INSTDIR\Swift.exe
    delete $INSTDIR\ssleay32.dll
    delete $INSTDIR\libeay32.dll
    delete $INSTDIR\phonon4.dll
    delete $INSTDIR\QtCore4.dll
    delete $INSTDIR\QtGui4.dll
    delete $INSTDIR\QtWebKit4.dll
    delete $INSTDIR\QtNetwork4.dll
    delete $INSTDIR\QtXMLPatterns4.dll
    delete $INSTDIR\imageformats\qgif4.dll
    delete $INSTDIR\imageformats\qico4.dll
    delete $INSTDIR\imageformats\qjpeg4.dll
    delete $INSTDIR\imageformats\qmng4.dll
    delete $INSTDIR\imageformats\qsvg4.dll
    delete $INSTDIR\imageformats\qtiff4.dll

    RmDir $INSTDIR\Prerequisites
    RmDir $INSTDIR\imageformats
    RmDir $INSTDIR

    ReadRegStr $0 HKCU "Software\Swift\" "Start Menu Folder"
    Delete "$0\Swift.lnk"
    Delete "$0\Uninstall Swift.lnk"
    RmDir "$0"

    DeleteRegKey HKEY_CURRENT_USER "Software\Swift\Start Menu Folder"
    DeleteRegKey /ifempty HKEY_CURRENT_USER "Software\Swift"
    DeleteRegKey HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run\Swift"

    Goto endUninstall
  endUninstall: 
sectionEnd


# TODO http://nsis.sourceforge.net/Check_whether_your_application_is_running_during_uninstallation
# http://nsis.sourceforge.net/Date_and_time_in_installer_or_application_name
# http://nsis.sourceforge.net/Removing_'Nullsoft_Install_System_vX.XX'_String_from_installer
