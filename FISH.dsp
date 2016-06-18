# Microsoft Developer Studio Project File - Name="FISH" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FISH - Win32 Debug_NOTUNICODE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FISH.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FISH.mak" CFG="FISH - Win32 Debug_NOTUNICODE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FISH - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Release_NOTUNICODE" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Debug_NOTUNICODE" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Release_BuildInstaller" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Release_WIN98" (based on "Win32 (x86) Application")
!MESSAGE "FISH - Win32 Debug_WIN98" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/FISH", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FISH - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Winmm.lib Wininet.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /FORCE:MULTIPLE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Winmm.lib Wininet.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_NOTUNICODE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FISH___Win32_Release_NOTUNICODE"
# PROP BASE Intermediate_Dir "FISH___Win32_Release_NOTUNICODE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "FISH___Win32_Release_NOTUNICODE"
# PROP Intermediate_Dir "FISH___Win32_Release_NOTUNICODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Wininet.lib Version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_NOTUNICODE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FISH___Win32_Debug_NOTUNICODE"
# PROP BASE Intermediate_Dir "FISH___Win32_Debug_NOTUNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "FISH___Win32_Debug_NOTUNICODE"
# PROP Intermediate_Dir "FISH___Win32_Debug_NOTUNICODE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Wininet.lib Version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_BuildInstaller"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FISH___Win32_Release_BuildInstaller"
# PROP BASE Intermediate_Dir "FISH___Win32_Release_BuildInstaller"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_BuildInstaller"
# PROP Intermediate_Dir "Release_BuildInstaller"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /Gm /Gi /GX /Zi /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FAcs /Fa"cod/" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Winmm.lib Wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# ADD LINK32 Winmm.lib Wininet.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /map /machine:I386
# SUBTRACT LINK32 /pdb:none /incremental:yes
# Begin Custom Build
OutDir=.\Release_BuildInstaller
TargetPath=.\Release_BuildInstaller\FISH.exe
InputPath=.\Release_BuildInstaller\FISH.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del "C:\Work\FISH\NSIS_Script\FISH.exe" 
	copy "$(TargetPath)" "../../NSIS_Script" 
	del "C:\Work\FISH\NSIS_Script\FISHInstall.exe" 
	"C:\Program Files\NSIS\makensisw.exe" "../../NSIS_Script/fishInstall.nsi" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_WIN98"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FISH___Win32_Release_WIN98"
# PROP BASE Intermediate_Dir "FISH___Win32_Release_WIN98"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_WIN98"
# PROP Intermediate_Dir "Release_WIN98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Winmm.lib Wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# ADD LINK32 UnicoWS.lib Kernel32.lib Advapi32.lib User32.lib Gdi32.lib Shell32.lib Comdlg32.lib Version.lib Mpr.lib Rasapi32.lib Winmm.lib Winspool.lib Vfw32.lib Oleacc.lib Oledlg.lib Wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib"
# Begin Custom Build
OutDir=.\Release_WIN98
TargetPath=.\Release_WIN98\FISH.exe
InputPath=.\Release_WIN98\FISH.exe
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del "C:\Work\FISH\NSIS_Script\FISH.exe" 
	copy "$(TargetPath)" "../../NSIS_Script" 
	del "C:\Work\FISH\NSIS_Script\FISHInstall.exe" 
	"C:\Program Files\NSIS\makensisw.exe" "../../NSIS_Script/fishInstall.nsi" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_WIN98"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FISH___Win32_Debug_WIN98"
# PROP BASE Intermediate_Dir "FISH___Win32_Debug_WIN98"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_WIN98"
# PROP Intermediate_Dir "Debug_WIN98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 UnicoWS.lib Kernel32.lib Advapi32.lib User32.lib Gdi32.lib Shell32.lib Comdlg32.lib Version.lib Mpr.lib Rasapi32.lib Winmm.lib Winspool.lib Vfw32.lib Oleacc.lib Oledlg.lib Winmm.lib Wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib"
# ADD LINK32 UnicoWS.lib Kernel32.lib Advapi32.lib User32.lib Gdi32.lib Shell32.lib Comdlg32.lib Version.lib Mpr.lib Rasapi32.lib Winmm.lib Winspool.lib Vfw32.lib Oleacc.lib Oledlg.lib Wininet.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib"

!ENDIF 

# Begin Target

# Name "FISH - Win32 Release"
# Name "FISH - Win32 Debug"
# Name "FISH - Win32 Release_NOTUNICODE"
# Name "FISH - Win32 Debug_NOTUNICODE"
# Name "FISH - Win32 Release_BuildInstaller"
# Name "FISH - Win32 Release_WIN98"
# Name "FISH - Win32 Debug_WIN98"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\view_search\ACEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\view_search\ACListWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChgPortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\CJFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Custsite.cpp
# End Source File
# Begin Source File

SOURCE=.\DateParser.cpp
# End Source File
# Begin Source File

SOURCE=.\DirDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Browser.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_ExplorerBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_OpmlImport.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_General.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_Network.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_Style.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Properties.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_PropUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_RSSList.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_RSSMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Search.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Style.cpp
# End Source File
# Begin Source File

SOURCE=.\util\ExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\ezparser_util.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\XML\EzXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\FeedControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Feeder.cpp
# End Source File
# Begin Source File

SOURCE=.\FeedParser.cpp
# End Source File
# Begin Source File

SOURCE=.\FISH.cpp
# End Source File
# Begin Source File

SOURCE=.\fish_common.cpp
# End Source File
# Begin Source File

SOURCE=.\FISHDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FishLinkManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FishListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\fishlistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\FishTransparentDragDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FocusManager.cpp
# End Source File
# Begin Source File

SOURCE=.\util\GlobalFunctions.cpp

!IF  "$(CFG)" == "FISH - Win32 Release"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_NOTUNICODE"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_NOTUNICODE"

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_BuildInstaller"

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_WIN98"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_WIN98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ctrl\HistoryCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\Idispimp.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetDib.cpp
# End Source File
# Begin Source File

SOURCE=.\OPMLSock.cpp
# End Source File
# Begin Source File

SOURCE=.\view_search\RcmmWordWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ResultManager.cpp
# End Source File
# Begin Source File

SOURCE=.\RSSCurrentList.cpp
# End Source File
# Begin Source File

SOURCE=.\RSSDB.cpp
# End Source File
# Begin Source File

SOURCE=.\RSSURL.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl\SSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TagManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskbarNotifier.cpp
# End Source File
# Begin Source File

SOURCE=.\timeconvert.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Tray.cpp
# End Source File
# Begin Source File

SOURCE=.\view_repository\TreeSkinVerticalScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\view_repository\TreeSkinVerticalScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\URLEncode.cpp
# End Source File
# Begin Source File

SOURCE=.\View_Information.cpp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\View_NavigationPane.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\WebText.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\view_search\ACEdit.h
# End Source File
# Begin Source File

SOURCE=.\view_search\ACListWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChgPortDlg.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\CJFlatComboBox.h
# End Source File
# Begin Source File

SOURCE=.\CustomDlg.h
# End Source File
# Begin Source File

SOURCE=.\CustomListBox.h
# End Source File
# Begin Source File

SOURCE=.\Custsite.h
# End Source File
# Begin Source File

SOURCE=.\DateParser.h
# End Source File
# Begin Source File

SOURCE=.\DirDialog.h
# End Source File
# Begin Source File

SOURCE=.\DispIds.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Browser.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_ExplorerBar.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Login.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_MessageBox.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_OpmlImport.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_General.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_Network.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Prop_Style.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Properties.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_PropUnit.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_RSSList.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_RSSMain.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Search.h
# End Source File
# Begin Source File

SOURCE=.\Dlg_Style.h
# End Source File
# Begin Source File

SOURCE=.\util\ExceptionHandler.h
# End Source File
# Begin Source File

SOURCE=.\XML\EzXMLParser.h
# End Source File
# Begin Source File

SOURCE=.\FeedControl.h
# End Source File
# Begin Source File

SOURCE=.\Feeder.h
# End Source File
# Begin Source File

SOURCE=.\FeedParser.h
# End Source File
# Begin Source File

SOURCE=.\FISH.h
# End Source File
# Begin Source File

SOURCE=.\fish_common.h
# End Source File
# Begin Source File

SOURCE=.\fish_def.h
# End Source File
# Begin Source File

SOURCE=.\fish_struct.h
# End Source File
# Begin Source File

SOURCE=.\fish_unicode.h
# End Source File
# Begin Source File

SOURCE=.\FISHDoc.h
# End Source File
# Begin Source File

SOURCE=.\FishLinkManager.h
# End Source File
# Begin Source File

SOURCE=.\FishListBox.h
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\fishlistctrl.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\FishTransparentDragDlg.h
# End Source File
# Begin Source File

SOURCE=.\FocusManager.h
# End Source File
# Begin Source File

SOURCE=.\util\GlobalFunctions.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\HistoryCombo.h
# End Source File
# Begin Source File

SOURCE=.\HtmlMaker.h
# End Source File
# Begin Source File

SOURCE=.\Idispimp.h
# End Source File
# Begin Source File

SOURCE=.\LocalProperties.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetBitmap.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetButton.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\NetDib.h
# End Source File
# Begin Source File

SOURCE=.\OPMLSock.h
# End Source File
# Begin Source File

SOURCE=.\view_search\RcmmWordWnd.h
# End Source File
# Begin Source File

SOURCE=.\ResultManager.h
# End Source File
# Begin Source File

SOURCE=.\RSSCurrentList.h
# End Source File
# Begin Source File

SOURCE=.\RSSDB.h
# End Source File
# Begin Source File

SOURCE=.\RSSURL.h
# End Source File
# Begin Source File

SOURCE=.\ScrapManager.h
# End Source File
# Begin Source File

SOURCE=.\SearchManager.h
# End Source File
# Begin Source File

SOURCE=.\smartptr.h
# End Source File
# Begin Source File

SOURCE=.\ctrl\SSplitter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\Struct.h
# End Source File
# Begin Source File

SOURCE=.\struct_def.h
# End Source File
# Begin Source File

SOURCE=.\TagManager.h
# End Source File
# Begin Source File

SOURCE=.\TaskbarNotifier.h
# End Source File
# Begin Source File

SOURCE=.\Tray.h
# End Source File
# Begin Source File

SOURCE=.\url.h
# End Source File
# Begin Source File

SOURCE=.\URLEncode.h
# End Source File
# Begin Source File

SOURCE=.\View_Information.h
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\View_NavigationPane.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WebText.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\view_repository\bitmap\animateNew.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\animateTimer_01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MessageBox\asterisk.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\AttentionProgressBar.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back1_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back1_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back1_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back2_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back2_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back2_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back3_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back3_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\back3_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\back_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\back_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\back_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\back_press.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Notifier\bg.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ETC\bg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\bg_bottom.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\bg_contents.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\bg_mid.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\bg_top.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\bkgnd.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\bkgnd_middle.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\box.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_apply_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_apply_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_apply_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_apply_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_cancel_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_cancel_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_cancel_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_cancel_o.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\btn_cancel_set.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\btn_cancel_set1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_close_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_close_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_combo_N.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_combo_O.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_combo_P.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_dot_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_dot_n.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\SummitBtn\btn_Go.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_idpasswd_find_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_idpasswd_find_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_login_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_login_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_next_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_next_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_next_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_next_o.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\btn_next_set.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_all_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_all_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_all_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_all_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_no_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_ok_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_ok_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_ok_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_ok_o.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\btn_ok_set.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_prev_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_prev_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_prev_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_prev_o.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\btn_prev_set.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_reg_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\btn_reg_n.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\BtnCenter\btn_SrchRange_All_D.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\BtnCenter\btn_SrchRange_All_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\BtnCenter\btn_SrchRange_All_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\BtnCenter\btn_SrchRange_All_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\BtnCenter\btn_SrchRange_Local_D.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\SummitBtn\Btn_Tag_N.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_all_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_all_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_all_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_all_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_d.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_n.bmp
# End Source File
# Begin Source File

SOURCE=.\res\btn\btn_yes_o.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\center_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\center_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderEnd.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderEnd_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderSpan.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderSpan_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderStart.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\ColumnHeaderStart_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\combobox_btn.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\icons\copyitem.cur
# End Source File
# Begin Source File

SOURCE=.\res\default.xslt
# End Source File
# Begin Source File

SOURCE=.\res\MessageBox\exclamation.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark2_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark2_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark2_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\expanded_mark_P.bmp
# End Source File
# Begin Source File

SOURCE=.\res\FISH.ico
# End Source File
# Begin Source File

SOURCE=.\res\FISH.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FISHDoc.ico
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\forward_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\forward_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\forward_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\forward_press.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\GlobalSearch_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\GlobalSearch_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\GlobalSearch_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\goto_mainlist_btn_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\goto_mainlist_btn_press.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\groupBG_01.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\groupBG_02.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MessageBox\hand.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\HorizontalScrollBarLeftArrow.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\HorizontalScrollBarRightArrow.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\HorizontalScrollBarSpan.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\HorizontalScrollBarThumb.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\ico_channel.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\ico_group.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\ico_key.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\ico_scrap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Notifier\icon.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_subtitle_left.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_subtitle_mid.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_subtitle_right.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_c.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_c_m.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_g.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_g_m.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_k.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_k_m.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_s.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_s_m.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_se.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\img_t_se_m.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\info1.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\listctrl_new_item.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\LocalSearch_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\LocalSearch_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\RangeBtn\LocalSearch_P.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_auto_login.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_check_box.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_help_tap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_id.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_Login.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_Login_over.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_passwd_save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_password.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\login_title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_bottomcenter.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_bottomleft.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_bottomright.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_left.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_right.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_topleft.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_topright.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\mainframe_topspan.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\icons\moveitem.cur
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_0_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_0_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_0_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_1_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_1_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_1_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_2_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_2_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_2_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_3_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_3_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetConnection_3_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_0_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_0_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_0_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_1_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_1_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_1_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_2_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_2_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_2_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_3_N.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_3_O.bmp
# End Source File
# Begin Source File

SOURCE=.\view_search\bitmap\ITuneStyle\ConnectionRateBtn\NetLoad_3_P.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\newtab_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\newtab_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\newtab_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next1_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next1_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next1_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next2_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next2_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next2_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next3_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next3_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\navigation_btn\next3_over.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\outline_bottom.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\outline_left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\outline_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\postsavestyle.xslt
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\propertyDlg\Property_Title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MessageBox\question.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addgroup\r_ico1.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addgroup\r_ico1a.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addgroup\r_ico1b.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addchannel\r_ico2.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addchannel\r_ico2a.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addchannel\r_ico2b.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addscrap\r_ico3.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addscrap\r_ico3a.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addscrap\r_ico3b.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addkeyword\r_ico4.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addkeyword\r_ico4a.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\addkeyword\r_ico4b.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\feedall\r_ico5.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\feedall\r_ico5a.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\feedall\r_ico5b.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\refresh_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\refresh_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\refresh_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\refresh_press.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\save_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\save_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\save_over.bmp
# End Source File
# Begin Source File

SOURCE=.\ctrl\bmp\spliter_horizontal.bmp
# End Source File
# Begin Source File

SOURCE=.\ctrl\bmp\spliter_vertical.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\stop_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\stop_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\stop_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_browser\bitmap\stop_press.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\style_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\style_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\style_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\subscribe_bk_bmp.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\subscribe_bk_bmp_bottom.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\subscribe_bk_bmp_left.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\subscribe_bk_bmp_right.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\subscribe_bk_bmp_top.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\subscribe_down.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\subscribe_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\view_navigationpane\bitmap\function_btn\subscribe_over.bmp
# End Source File
# Begin Source File

SOURCE=.\view_mainframe\bitmap\sysmenu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\t_auto_login.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\t_id.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\t_passwd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\t_passwd_save.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_desel_left.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_desel_right.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_desel_span.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_next_btn_n.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_prev_btn_n.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_sel_left.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_sel_right.bmp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\bitmap\tab_sel_span.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\Title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LoginDlg\title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\title_left.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\title_mid.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CustomDlg\title_right.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray\tray_connecting.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray\tray_downloading.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray\tray_normal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray\tray_off.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray\tray_unread.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\variousChannel_01.bmp
# End Source File
# Begin Source File

SOURCE=.\view_repository\bitmap\variousGroup_01.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollbarBottom.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollBarDownArrow.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollBarSpan.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollBarThumb.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollbarTop.bmp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\bitmap\VerticleScrollBarUpArrow.bmp
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Group "tabctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\view_tabctrl\CustomTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\CustomTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\DotNetTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\Schemadef.h
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\ThemeUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\ThemeUtil.h
# End Source File
# Begin Source File

SOURCE=.\view_tabctrl\Tmschema.h
# End Source File
# End Group
# Begin Group "treectrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\view_repository\MultipleSelectionTree.cpp
# End Source File
# Begin Source File

SOURCE=.\view_repository\MultipleSelectionTree.h
# End Source File
# Begin Source File

SOURCE=.\view_repository\RepositoryTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\view_repository\RepositoryTreeCtrl.h
# End Source File
# End Group
# Begin Group "listctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\view_listctrl\FishHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\FishHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinHorizontalScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinHorizontalScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinVerticleScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\view_listctrl\SkinVerticleScrollbar.h
# End Source File
# End Group
# Begin Group "dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dlg_Subscribe.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_Subscribe.h
# End Source File
# Begin Source File

SOURCE=.\view_repository\repositoryitempropertydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\view_repository\repositoryitempropertydlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\view_browser\CFishComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\view_browser\CFishComboBox.h
# End Source File
# Begin Source File

SOURCE=.\fab
# End Source File
# Begin Source File

SOURCE=.\FISH.rc
# End Source File
# Begin Source File

SOURCE=.\GlobalIconManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalIconManager.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\util\MaskBlt.cpp
# End Source File
# Begin Source File

SOURCE=.\util\MaskBlt.h
# End Source File
# Begin Source File

SOURCE=.\util\md5e.cpp
# End Source File
# Begin Source File

SOURCE=.\util\md5e.h
# End Source File
# End Group
# Begin Group "Gnucleus"

# PROP Default_Filter ""
# Begin Group "Header Files - Gnucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GnuModule\Common.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Conversions.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\FileLock.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\FileTypes.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuControl.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDefines.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDoc.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDownload.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDownloadShell.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuLocal.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuNode.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuRouting.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuSock.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuUpload.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuUploadShell.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuWordHash.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Packet.h
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Sha1.h
# End Source File
# End Group
# Begin Group "Source Files - Gnucleus"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GnuModule\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Conversions.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\FileLock.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\FileTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuControl.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDownload.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuDownloadShell.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuLocal.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuNode.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuRouting.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuSock.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuUpload.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\GnuUploadShell.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Packet.cpp
# End Source File
# Begin Source File

SOURCE=.\GnuModule\Sha1.cpp
# End Source File
# End Group
# End Group
# Begin Group "Irc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IRCModule\GetHttp.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCModule\GetHttp.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\httpd.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCModule\httpd.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IRC_Command.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\irc_defines.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IRC_Error.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\irc_utils.cpp

!IF  "$(CFG)" == "FISH - Win32 Release"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_NOTUNICODE"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_NOTUNICODE"

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_BuildInstaller"

!ELSEIF  "$(CFG)" == "FISH - Win32 Release_WIN98"

!ELSEIF  "$(CFG)" == "FISH - Win32 Debug_WIN98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IRCModule\irc_utils.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IrcdManager.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IrcdManager.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IrcSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCModule\IrcSocket.h
# End Source File
# Begin Source File

SOURCE=.\IRCModule\Server2ServerSock.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section FISH : {01480057-0023-00D4-2400-D4002500D400}
# 	1:8:IDR_FISH:102
# End Section
