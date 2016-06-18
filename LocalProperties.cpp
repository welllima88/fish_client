// LocalProperties.cpp: implementation of the CLocalProperties class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "LocalProperties.h"
#include "fish_common.h"
#include "fish_def.h"
#include "Dlg_MessageBox.h"
#include "./GnuModule/common.h"
#include "rssdb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define STR_LP_QUERY_COPYDATA		_T("최종 사용자의 구독 정보를 복사 하시겠습니까?")
#define STR_ERR_LP_COPYOPML			_T("복사할 OPML 파일을 발견하지 못했습니다.")

#define MAX_SRL						1000
#define MIN_SRL						60

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IDCHARDIV					62			// 숫자 10, 영문 52
static char idkey[] =
{
/*		 0    1    2    3    4    5    6    7    8    9   */
/* 0 */	'0', 'l', 'E', 'b', 'N', 'W', 'f', 'v', 'Y', 'Z', 
/* 1 */	'a', 'g', 'h', 'e', 'x', 'X', 'q', 'c', 'y', 'I', 
/* 2 */	'k', 'p', 'z', 'A', 'o', 't', 'H', 'M', 'n', 'w',
/* 3 */	'u', 'C', '3', 'T', 'U', 'V', 'P', '2', '4', 'D', 
/* 4 */	'K', 'G', '6', '1', '9', 'm', 'L', 'B', 'F', 'r',
/* 5 */	'O', '7', 'R', 'S', '5', 'J', 'i', 'j', 'd', 's',
/* 6 */	'8', 'Q', NULL
};

void GuidtoIRCID(GUID inGuid, char* pid, int len)
{
	DWORD dw[4];
	dw[0] = inGuid.Data1;
	dw[1] = MAKELONG(inGuid.Data2, inGuid.Data3);
	memcpy(&dw[2], &(inGuid.Data4[0]), 4);
	memcpy(&dw[3], &(inGuid.Data4[4]), 4); 

	int l = 0;
	DWORD data;
	DWORD pos;
	for(int i = 0; i < 4 && l < len; ++i)
	{
		data = dw[i];
		while(data)
		{
			pos = data % IDCHARDIV;
			pid[l] = idkey[pos];

			data /= IDCHARDIV;

			if(++l >= len) break;
		}
	}

	pid[l] = idkey[IDCHARDIV];
}

CLocalProperties::CLocalProperties()
{
	InitProperties();
}

CLocalProperties::~CLocalProperties()
{
	TRACE(_T("CLocalProperties Destroy\r\n"));
}

/**************************************************************************
 * method CLocalProperties::MakeFirstStart
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * 프로그램이 최초로 실행되었을 때 필요한 작업을 수행한다.
 *
 * @Parameters
 **************************************************************************/
void CLocalProperties::MakeFirstStart()
{

	// make first start flag
	WriteRegData(_T(""), REG_STR_FIRSTSTART, _T("1"));
}

/**************************************************************************
 * method CLocalProperties::InitProperties
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-??
 *
 * @Description
 * CLocalProperties의 멤버 변수들의 초기화 작업을 수행한다.
 *
 * @Parameters
 **************************************************************************/
void CLocalProperties::InitProperties()
{
	// HWND initialize
	hSubscribeWnd = NULL;
	hMainWnd = NULL;

	szIRCID[0] = 0;
	bUserInit = FALSE;
	// 최초 실행 플래그
	// Read First Start Flag
	CString tmp;
	tmp = ReadRegData(_T(""), REG_STR_FIRSTSTART);
	if(tmp != _T("")) bFirst = FALSE;
	else bFirst = TRUE;

	// Read Auto Start
	tmp = ReadRegData(_T(""), REG_STR_AUTOSTART);
	if(tmp == _T("1")) bAutoStart = TRUE;
	else bAutoStart = FALSE;

	tmp = ReadRegData(_T(""), REG_STR_CLOSEBTNEXIT);
	if(tmp == _T("1")) bCloseBtnExit = TRUE;
	else bCloseBtnExit = FALSE;

	if(_T("1") == ReadRegData(_T(""), REG_STR_SEARCHSAVE)) bSearchSave = TRUE;
	else bSearchSave = FALSE;

	tmp = ReadRegData(_T(""), REG_STR_IRCID);
	if(!tmp.IsEmpty())
	{
		int len = tmp.GetLength();

#ifdef _UNICODE
		AnsiConvertWtoM((LPTSTR) (LPCTSTR) tmp, szIRCID, sizeof(szIRCID));
#else
		if(len < sizeof(szIRCID))
			strcpy(szIRCID, tmp);
#endif
	}

	TCHAR* stopstring;
	tmp = ReadRegData(_T(""), REG_STR_SWREAD);
	fSWRead = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWLINK);
	fSWLink = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWSCRAP);
	fSWScrap = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWFILTER);
	fSWFilter = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWHIT);
	fSWHit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWBASE);
	fSWBase = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	if(fSWBase == 0) fSWBase = 100;

	tmp = ReadRegData(_T(""), REG_STR_SWREADLIMIT);
	fSWReadLimit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWLINKLIMIT);
	fSWLinkLimit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWHITLIMIT);
	fSWHitLimit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWSCRAPLIMIT);
	fSWScrapLimit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	tmp = ReadRegData(_T(""), REG_STR_SWFILTERLIMIT);
	fSWFilterLimit = _tcstod ((LPTSTR) (LPCTSTR) tmp, &stopstring);

	UserPathTemp = ReadRegData(_T(""), REG_STR_USERPATH);

	// Read OPML Sync Information
	sUserID = ReadRegData(_T(""), REG_STR_LASTUSER);
	if(_T("") == sUserID) sUserID = DEFAULT_USERID;

	tmp = ReadRegData(_T(""), REG_STR_SAVEPASSWORD);
	if(tmp == _T("1")) bSavePassword = TRUE;
	else bSavePassword = FALSE;

	bAutoLogin = FALSE;
	sPassword = _T("");
	if(bSavePassword)
	{
		tmp = ReadRegData(_T(""), REG_STR_ENCODEDPASS);
		if(tmp.GetLength() < 8) sPassword = _T("");
		else sPassword = tmp;

		tmp = ReadRegData(_T(""), REG_STR_AUTOLOGIN);
		if(tmp == _T("1")) bAutoLogin = TRUE;
	}

	tmp = ReadRegData(_T(""), REG_STR_USEOPMLSYNC);
	if(tmp == _T("1")) bUseOPMLSync = TRUE;
	else bUseOPMLSync = FALSE;

	if(_T("1") == ReadRegData(_T(""), REG_STR_NOTUSEP2P)) bNotUseP2P = TRUE;
	else bNotUseP2P = FALSE;

	uSearchResultLimit = _ttoi(ReadRegData(_T(""), REG_STR_SRL));
	if(uSearchResultLimit > MAX_SRL) uSearchResultLimit = MAX_SRL;
	else if(uSearchResultLimit < MIN_SRL) uSearchResultLimit = MIN_SRL;

	if(_T("1") == ReadRegData(_T(""), REG_STR_POSTAUTODELETE)) bPostAutoDelete = TRUE;
	else bPostAutoDelete = FALSE;


	if(_T("1") == ReadRegData(_T(""), REG_STR_LISTDESCSUMMARY)) bListDescSummary = TRUE;
	else bListDescSummary = FALSE;

	PaperStyleFile = ReadRegData(_T(""), REG_STR_PAPERSTYLE);

	// 기본 페이지 당 포스트 수
	ppp		= DEFAULT_PPP;

	// get time zone
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);
	bias = tzi.Bias;
	spgmt.SetDateTimeSpan(0, 0, -bias, 0);

	// get favorites directory
	TCHAR szpath[_MAX_PATH];

	if(	SHGetSpecialFolderPath(
		NULL,
		szpath,
		CSIDL_FAVORITES,
		FALSE
		)
		)
	{
		sFavoritesDir = szpath;
	}

	// GET RUNNABLE PATH
//	TCHAR szModule[_MAX_PATH];
//	GetModuleFileName(NULL, szModule, _MAX_PATH);
//	RunPath = szModule;
#ifdef _UNICODE
	RunPath = __wargv[0];
#else
	RunPath = __argv[0];
#endif

//	TRACE(_T("RUN PAHT : %s\r\n"), RunPath);
	RunPath = RunPath.Mid(0, RunPath.ReverseFind('\\') + 1);

	// SET PARIAL PATH ENABLE
	PartialsInDir		= true;

	// Set IRC ID NULL
	szIRCID[0] = '\0';

	bIRCConnected = FALSE;
	bOPMLConnected = FALSE;

	if(bFirst) MakeFirstStart();
}

void CLocalProperties::MakeAutoStart()
{
	// 시스템이 자동 실행 등록/해제 /////////////////////////////////////////////
	HKEY	hKeyExt;
	DWORD	dwDisposition=0;
	
	if(	bAutoStart )
	{
		RegCreateKeyEx(HKEY_CURRENT_USER, (LPCTSTR)REG_STR_WINAUTORUN, 
			0, _T(""),	REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hKeyExt, &dwDisposition);
		
		// 실행 파일 위치 얻기
		TCHAR szModule[_MAX_PATH];
//		GetModuleFileName(NULL, szModule, _MAX_PATH);
//		RegSetValueEx(hKeyExt, FISH_TITLE, 0, REG_EXPAND_SZ, (LPBYTE)szModule, _tcslen(szModule));
#ifdef _UNICODE
		_tcscpy(szModule, __wargv[0]);
#else
		strcpy(szModule, __argv[0]);
#endif
		RegSetValueEx(hKeyExt, FISH_TITLE, 0, REG_EXPAND_SZ, (LPBYTE)szModule, _tcslen(szModule) * sizeof(TCHAR) );
	}
	else
	{
		RegOpenKey(HKEY_CURRENT_USER,(LPCTSTR)REG_STR_WINAUTORUN,&hKeyExt);
		RegDeleteValue((HKEY)hKeyExt,(LPCTSTR)FISH_TITLE);
	}
	
	RegCloseKey(hKeyExt);
	// end 시스템이 자동 실행 등록/해제 //////////////////////////////////////////
}

/**************************************************************************
 * method CLocalProperties::SaveProperties
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-??
 *
 * @Description
 * 설정값을 레지스트리에 저장한다.
 * 설정 값은 CGnuDoc 과 CLocalProperties 두 곳에서 관리된다.
 *
 * @Parameters
 **************************************************************************/
void CLocalProperties::SaveProperties()
{
	if(!bUserInit) return;

	TCHAR tmp[32];
	// REGISTRY RECORD
	// MAIN PROPERTIES
	if(dwFlagMain1 & FLAG_REG_AUTOSTART)
	{
		WriteRegData(_T(""), REG_STR_AUTOSTART, bAutoStart ? _T("1") : _T("0"));

		MakeAutoStart();
	}

	if(dwFlagMain1 & FLAG_REG_USERPATH)
		WriteRegData(_T(""), REG_STR_USERPATH, UserPathParent);

	if(dwFlagMain1 & FLAG_REG_CLOSEBTNEXIT)
		WriteRegData(_T(""), REG_STR_CLOSEBTNEXIT, bCloseBtnExit ? _T("1") : _T("0"));

	if(dwFlagMain1 & FLAG_REG_IRCID)
		WriteRegData(_T(""), REG_STR_IRCID, szIRCID);

	if(dwFlagMain1 & FLAG_REG_SEARCHSAVE)
		WriteRegData(_T(""), REG_STR_SEARCHSAVE, bSearchSave ? _T("1") : _T("0"));

	// Search Weight
	if(dwFlagSearch & FLAG_REG_SWREAD)
	{
		_stprintf(tmp, _T("%f"), fSWRead);
		WriteRegData(_T(""), REG_STR_SWREAD, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWLINK)
	{
		_stprintf(tmp, _T("%f"), fSWLink);
		WriteRegData(_T(""), REG_STR_SWLINK, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWSCRAP)
	{
		_stprintf(tmp, _T("%f"), fSWScrap);
		WriteRegData(_T(""), REG_STR_SWSCRAP, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWFILTER)
	{
		_stprintf(tmp, _T("%f"), fSWFilter);
		WriteRegData(_T(""), REG_STR_SWFILTER, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWHIT)
	{
		_stprintf(tmp, _T("%f"), fSWHit);
		WriteRegData(_T(""), REG_STR_SWHIT, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWBASE)
	{
		_stprintf(tmp, _T("%f"), fSWBase);
		WriteRegData(_T(""), REG_STR_SWBASE, tmp);
	}

	// search quantity limitation
	if(dwFlagSearch & FLAG_REG_SWREADLIMIT)
	{
		_stprintf(tmp, _T("%f"), fSWReadLimit);
		WriteRegData(_T(""), REG_STR_SWREADLIMIT, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWLINKLIMIT)
	{
		_stprintf(tmp, _T("%f"), fSWLinkLimit);
		WriteRegData(_T(""), REG_STR_SWLINKLIMIT, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWHITLIMIT)
	{
		_stprintf(tmp, _T("%f"), fSWHitLimit);
		WriteRegData(_T(""), REG_STR_SWHITLIMIT, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWSCRAPLIMIT)
	{
		_stprintf(tmp, _T("%f"), fSWScrapLimit);
		WriteRegData(_T(""), REG_STR_SWSCRAPLIMIT, tmp);
	}

	if(dwFlagSearch & FLAG_REG_SWFILTERLIMIT)
	{
		_stprintf(tmp, _T("%f"), fSWFilterLimit);
		WriteRegData(_T(""), REG_STR_SWFILTERLIMIT, tmp);
	}


	// USER PROPERTIES
	if(dwFlagMain2 & FLAG_REG_DOWNLOADPATH)
		WriteRegData(_T(""), REG_STR_DOWNLOADPATH, DownloadPath);

	if(dwFlagMain2 & FLAG_REG_PPP)
		WriteRegData(_T(""), REG_STR_PPP, _itot(ppp, tmp, 10));

	if(dwFlagMain2 & FLAG_REG_USERINFORMWND)
		WriteRegData(_T(""), REG_STR_USEINFORMWND, bUseInformWnd ? _T("1") : _T("0"));

	if(dwFlagMain2 & FLAG_REG_STYLE)
		WriteRegData(_T(""), REG_STR_STYLE, StyleFile);

	// OPML SYNC
	if(dwFlagOPML & FLAG_REG_USERID)
		WriteRegData(_T(""), REG_STR_LASTUSER, sUserID);

	if(bSavePassword)
	{
		if(dwFlagOPML & FLAG_REG_PASSWORD)
			WriteRegData(_T(""), REG_STR_ENCODEDPASS, sPassword);

		if(dwFlagOPML & FLAG_REG_AUTOLOGIN)
			WriteRegData(_T(""), REG_STR_AUTOLOGIN, bAutoLogin ? _T("1") : _T("0"));
	}

	if(dwFlagOPML & FLAG_REG_SAVEPASSWORD)
		WriteRegData(_T(""), REG_STR_SAVEPASSWORD, bSavePassword ? _T("1") : _T("0"));

	if(dwFlagOPML & FLAG_REG_LASTOPMLTIME)
		WriteRegData(sUserID, REG_STR_LASTOPMLTIME, sLastOPMLTime);

	if(dwFlagOPML & FLAG_REG_USEOPMLSYNC)
		WriteRegData(sUserID, REG_STR_USEOPMLSYNC, bUseOPMLSync ? _T("1") : _T("0"));

	// USE P2P
	if(dwFlagMain2 & FLAG_REG_NOTUSEP2P)
		WriteRegData(_T(""), REG_STR_NOTUSEP2P, bNotUseP2P ? _T("1") : _T("0"));

	// SEARCH RESULT LIMITATION
	if(dwFlagMain2 & FLAG_REG_SRL)
		WriteRegData(_T(""), REG_STR_SRL, _itot(uSearchResultLimit, tmp, 10));

	// USE AUTO POST DELETION
	if(dwFlagMain1 & FLAG_REG_POSTAUTODELETE)
		WriteRegData(_T(""), REG_STR_POSTAUTODELETE, bPostAutoDelete ? _T("1") : _T("0"));

	if(dwFlagMain3 & FLAG_REG_LISTDESCSUMMARY)
		WriteRegData(_T(""), REG_STR_LISTDESCSUMMARY, bListDescSummary ? _T("1") : _T("0"));

	if(dwFlagMain3 & FLAG_REG_PAPERSTYLE)
		WriteRegData(_T(""), REG_STR_PAPERSTYLE, PaperStyleFile);

	dwFlagMain1 = 0;
	dwFlagMain2 = 0;
	dwFlagMain3 = 0;
	dwFlagSearch = 0;
	dwFlagOPML = 0;
}


/**************************************************************************
 * function CopyFiles
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-22
  *
 * @Description
 * 복수의 파일의 복사를 수행한다.
 *
 * @Parameters
 * (in CString) a - 소스가 되는 파일, *.* 등 허용
 * (in CString) b - 대상이 되는 디렉토리 또는 파일명 (한개의 파일을 복사할 때는 파일명을 넣는다.)
 * (in BOOL) bmove - 파일을 옮긴다.
 **************************************************************************/
BOOL CopyFiles(CString a, CString b, BOOL bmove)
{
	CFileFind f;
	BOOL bloop;
	BOOL bdestdir = TRUE;
	BOOL bsourcesingle = FALSE;

	TRACE(_T("copy [ %s ] to [ %s ]\r\n"), a, b);

	if(a.GetLength() == 0 || b.GetLength() == 0) return FALSE;

	bloop = f.FindFile(b);
	if(bloop)
	{
		bdestdir = FALSE;
	}

	if(bdestdir && b.Right(1) != _T("\\")) b+= _T("\\");

	f.Close();

	if(a.Find(_T("*")) == -1)
		bsourcesingle = TRUE;

	if((bsourcesingle && bdestdir)
		|| (!bsourcesingle && !bdestdir)
		) return FALSE;

	bloop = f.FindFile(a);

	if(!bloop) return FALSE;

	if(bsourcesingle)
	{
		while(bloop)
		{
			bloop = f.FindNextFile();

			if(f.IsNormal())
			{
				if(bmove)
					return MoveFile(f.GetFilePath(), b);

				return CopyFile(f.GetFilePath(), b, TRUE);
			}
		}
	}
	else
	{
		CString dest;
		while(bloop)
		{
			bloop = f.FindNextFile();

			if(!f.IsDots() && !f.IsDirectory())
			{
				dest.Format(_T("%s%s"), b, f.GetFileName());
				
//				TRACE(_T("copy file from : %s - to : %s\r\n"), f.GetFilePath(), dest);

				if(bmove)
				{
					if(!MoveFile(f.GetFilePath(), dest))
						return FALSE;
				}
				else
				{
					if(!CopyFile(f.GetFilePath(), dest, FALSE))
						return FALSE;
				}
			}
		}
	}

	return TRUE;
}

/**************************************************************************
 * function CopyOPMLFromTo
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-22
  *
 * @Description
 * OPML 파일과 XML 파일을 복사한다.
 *
 * @Parameters
 * (in CString) from - 소스가 되는 디렉토리
 * (in CString) to - 대상이 되는 디렉토리
 * (in BOOL) bmove - 파일을 옮긴다.
 **************************************************************************/
BOOL CopyOPMLFromTo(CString from, CString to, BOOL bmove)
{
	// create directory
	CString xmlfrom, xmlto;
	xmlto = to + DEFAULT_POSTPATH;
	makedir(xmlto);

	CFileFind f;
	CString opml, opdest;
	opml.Format(_T("%s%s"), from, S_DEFAULT_LOCALOPML);
	opdest.Format(_T("%s%s"), to, S_DEFAULT_LOCALOPML);
	if(!f.FindFile(opml))
	{
//		AfxMessageBox(STR_ERR_LP_COPYOPML);
        FishMessageBox(STR_ERR_LP_COPYOPML);
		return FALSE;
	}

	if(!bmove)
	{
		if(!CopyFile(opml, opdest, FALSE)) return FALSE;
	}
	else
	{
		if(!MoveFile(opml, opdest)) return FALSE;
	}

//	TRACE(_T("OPML from : %s, to : %s\r\n"), opml, opdest);

	f.Close();

	xmlfrom.Format(_T("%s%s*.xml"), from, DEFAULT_POSTPATH);
	CopyFiles(xmlfrom, xmlto, bmove);

	TRACE(_T("xml from : %s, to : %s\r\n"), xmlfrom, xmlto);

	// XML 파일에 적용한다.
	theApp.m_spRD->IChannelSetPath(xmlto);

	return TRUE;
}

/**************************************************************************
 * method CLocalProperties::LoadProperties
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-??
 * modified 2006-02-22 :: 사용자 폴더 관리 변경 by moonknit
 *
 * @Description
 * 설정값을 레지스트리에서 읽어온다.
 * 설정 값은 CGnuDoc 과 CLocalProperties 두 곳에서 관리된다.
 *
 * @Parameters
 * (in CString) userid - 로그인 한 사용자의 id
 **************************************************************************/
void CLocalProperties::LoadProperties()
{
	CString tmp;

	// Read Use Inform Window
	tmp = ReadRegData(_T(""), REG_STR_USEINFORMWND);
	if(tmp == _T("1")) bUseInformWnd = TRUE;
	else bUseInformWnd = FALSE;

	// Read PPP
	tmp = ReadRegData(_T(""), REG_STR_PPP);
	if(tmp != _T(""))
	{
		ppp = _ttoi((LPCTSTR) tmp);
	}

	if(ppp < 2 || ppp > 10) ppp = 2;

	// Read Download Path
	DownloadPath = ReadRegData(_T(""), REG_STR_DOWNLOADPATH);
	if(DownloadPath == _T(""))
	{
		tmp.Format(_T("%s%s"), RunPath, DEFAULT_DOWNLOADPATH);
		SetDownloadPath(tmp);
	}

	if(UserPathParent == _T(""))
	{
		if(UserPathTemp == _T(""))
		{
			// 최초 사용자의 경우 실행경로로 부터 사용자 폴더를 설정한다.
			UserPathParent.Format(_T("%s%s"), RunPath, DEFAULT_USERPATH);
//			UserPath.Format(_T("%s%s\\"), UserPathParent, sUserID);
		}
		else
		{
			UserPathParent = UserPathTemp;
		}
	}

	if(UserPathParent != UserPathTemp)
		WriteRegData(_T(""), REG_STR_USERPATH, UserPathParent);				// 최종 사용자의 폴더 기록

	StyleFile = ReadRegData(_T(""), REG_STR_STYLE);

//	SaveDataPath = UserPathParent + DEFAULT_SAVEPATH;;
	TempDataPath = UserPathParent + DEFAULT_TEMPPATH;
	PostDataPath = UserPathParent + DEFAULT_POSTPATH;

//	makedir(SaveDataPath);
	makedir(TempDataPath);
	makedir(PostDataPath);

	bUserInit = TRUE;
}

void CLocalProperties::LoadOPMLInform(CString userid)
{
	if(userid == _T("")) return;

	sUserID = userid;

	// Read Last OPML Update Time
	sLastOPMLTime = ReadRegData(sUserID, REG_STR_LASTOPMLTIME);

}

CString CLocalProperties::GetFavoritesDir()
{
	return sFavoritesDir;
}

// written by		: moonknit
// name				: CLocalProperties::SetDownloadPath
// created			: 2005-05-18
// last updated		: 2005-05-18
// desc				:
// 다운로드 폴더를 설정한다.
bool CLocalProperties::SetDownloadPath(CString sPath)
{
#ifndef USE_DOWNLOAD
	DownloadPath = sPath;
#else
	CString sNewPath = sPath;
	// Load partial files
	if(makedir(sNewPath) == 0)
	{
		if(UserPath.ReverseFind(_T('\\')) != UserPath.GetLength() - 1)
			UserPath += _T("\\");
		sNewPath = UserPath + _T("share");

		if(makedir(sNewPath) == 0)
		{
			return false;
		}
	}

	DownloadPath = sNewPath;

	CString PartialPath = PartialsInDir ? DownloadPath + _T("\\Partials") : RunPath + _T("Partials");
	PartialPath.Replace(_T("\\\\"), _T("\\"));
	CreateDirectory(PartialPath, NULL);
	
	//다운로드 경로의 남응 용량..
	ULONGLONG remain = ReCalcDownPath(DownloadPath);

	// 레지스트리 기록
	if(remain != 0)
		WriteRegData(_T(""), REG_STR_DOWNLOADPATH, sNewPath);
#endif
	dwFlagMain2 |= FLAG_REG_DOWNLOADPATH;

	return true;
}

//////////////////////////////////////////////////
// NAME : ReCalcDownPath
// PARM : void
// RETN : void
// DESC : 다운로드 경로의 남은 용량 계산하기 
// Date : 2005-09-26 coded by happyune, origin
//		2005-12-22 :: 용량 구하는 부분만 남기고 제거 by moonknit
//////////////////////////////////////////////////
ULONGLONG CLocalProperties::ReCalcDownPath(CString path)
{
	int nIndex = 0;
	nIndex = path.Find(_T("\\"));
	path = path.Left(nIndex + 1); //다운로드 경로의 Drive 명 구하기 

	ULONGLONG  FreeSize = 0;

	//다운로드 경로의 남은 용량 구하기
	int nReturn = avldspace(path, 0, FreeSize);
	if( nReturn == 2 )
	{
		TRACE(_T("CLocalProperties::ReCalcDownPath 용량을 check 하려는 경로가 잘못되었습니다."));
		return 0;
	}
	
	return FreeSize;
}

CString CLocalProperties::GetPartialDir()
{
	return PartialsInDir ? DownloadPath + _T("\\Partials") : RunPath + _T("Partials");
}

void CLocalProperties::SetIRCID(char* pid)
{ 
	if(!pid || strlen(pid) >= sizeof(szIRCID)) return; 
	dwFlagMain1 |= FLAG_REG_IRCID;
	strcpy(szIRCID, pid); 
}

void CLocalProperties::SetUserSeq(int seq)
{
	nUserSeq = seq;
}

void CLocalProperties::SetUserPath(CString sFlag)
{
	if(sFlag == UserPathParent) return;

	CString from, to;
	from = UserPathParent;
	to = sFlag;
	CopyOPMLFromTo(from, to, TRUE);

	UserPathParent = sFlag; 
	dwFlagMain1 |= FLAG_REG_USERPATH;
}

void CLocalProperties::MakeRandomIRCID()
{
	GUID guid;
	GnuCreateGuid(&guid);

	if(guid == NULL_GUID) return;

	dwFlagMain1 |= FLAG_REG_IRCID;

	GuidtoIRCID(guid, szIRCID, sizeof(szIRCID) - 1);
}

void CLocalProperties::SetSearchSave(BOOL bFlag)
{
	if(bFlag == bSearchSave) return;
	bSearchSave = bFlag; 
	dwFlagMain1 |= FLAG_REG_SEARCHSAVE;

	if(TRUE == bFlag)
	{
		::SendMessage(hSubscribeWnd, WM_SETSEARCHSAVE, NULL, NULL);
	}
}