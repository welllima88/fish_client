// RSSURL.cpp: implementation of the CRSSURL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "RSSURL.h"
#include "LocalProperties.h"
#include "fish_def.h"
#include "./GnuModule/GnuDefines.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

list<CString> CRSSURL::urllist;
CString CRSSURL::travelpath = _T("");
HANDLE CRSSURL::hTravelThread = NULL;
HWND CRSSURL::replyhwnd = NULL;
CCriticalSection csurllist;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRSSURL::CRSSURL()
{

}

CRSSURL::~CRSSURL()
{
	TRACE(_T("CRSSURL Destroy\r\n"));
}

void CRSSURL::SetReplyHWND(HWND hreply)
{ 
	replyhwnd = hreply; 
}

/**************************************************************************
 * STATIC CRSSURL::GetFeedURL
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-11
 *
 * @Description
 * 해당 HTTP URL 을 이용하여 RSS XML URL을 획득한다.
 *
 * @Parameters
 * url을 이용하여 webpage를 열어 rssurl을 찾아낸다.
 **************************************************************************/
BOOL CRSSURL::GetFeedURL(CString httpurl, list<CString>& resulturl)
{
	resulturl.clear();

	// TO DO
	return FALSE;
}

/**************************************************************************
 * STATIC CRSSURL::StopTravelThread
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Travel Thread를 강제로 닫는다.
 **************************************************************************/
void CRSSURL::StopTravelThread()
{
	if(hTravelThread)
		CloseHandle(hTravelThread);
}

/**************************************************************************
 * STATIC CRSSURL::IsTraveling
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Travel Thread 수행 여부를 확인한다.
 *
 * @Return
 * (BOOL)
 **************************************************************************/
BOOL CRSSURL::IsTraveling()
{
	return (hTravelThread != NULL) ? TRUE : FALSE;
}

/**************************************************************************
 * STATIC CRSSURL::TravelForXMLURL
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * 외부의 요청에 의해 Path 내부의 URL 파일로 부터 RSS URL 획득작업을 시작한다.
 *
 * @Parameters
 * (in CString) path - URL 파일을 검색할 디렉토리 또는 파일명
 *
 * @Return
 * (BOOL)
 **************************************************************************/
BOOL CRSSURL::TravelForXMLURL(CString path)
{
	if(path.GetLength() == 0) return FALSE;

#ifdef FISH_TEST
	travelpath = path;
	TravelThread();
	return TRUE;
#else
	if(IsTraveling()) return FALSE;

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TravelThread, NULL, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	travelpath = path;
	hTravelThread = hThread;
	ResumeThread(hThread);
#endif

	return TRUE;
}

/**************************************************************************
 * STATIC CRSSURL::GetURLFromURLFile
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * url 파일을 이용하여 htmlurl을 얻어낸다.
 *
 * @Return
 * (int) - 찾아낸 rss url의 개수
 **************************************************************************/
BOOL CRSSURL::GetURLFromURLFile(CString filename, CString& htmlurl)
{
	TCHAR szurl[MAX_URL];
	if(GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), _T(""), szurl, MAX_URL, filename) == 0)
	{
		return FALSE;
	}

	htmlurl = szurl;

	TRACE(_T("aquired url = [%s]\r\n"), htmlurl);

	return TRUE;
}

/**************************************************************************
 * STATIC CRSSURL::ParseWebPage
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-14
 *
 * @Description
 * web page 를 파싱하여 RSS url을 획득한다.
 *
 * @Return
 * (int) - 한 페이지에서 얻은 RSS url의 개수
 **************************************************************************/
int CRSSURL::ParseWebPage(CString url)
{
	list<CString> rsslist;
	// URL로 부터 rss url을 획득
	if(!GetFeedURL(url, rsslist)) return 0;

	// 결과를 목록에 넣는다.
	if(!csurllist.Lock(500)) return 0;
	urllist.insert(urllist.end(), rsslist.begin(), rsslist.end());
	csurllist.Unlock();

	return rsslist.size();
}

/**************************************************************************
 * STATIC CRSSURL::TravelPath
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * 디렉토리 내부의 url 파일을 이용하여 rss url을 추출해낸다.
 * recursive function
 *
 * @Parameters
 * (in CString) path - 검색할 디렉토리
 * (in BOOL) bonlycnt - 파일로 부터 RSS URL 추출을 하지 않고 url 파일의 개수만 계산한다.
 *
 * @Return
 * (int) - 찾아낸 rss url의 개수
 **************************************************************************/
#define EXT_URL						_T(".url")
int CRSSURL::TravelPath(CString path, BOOL bonlycnt)
{
	CFileFind f;

	CString search;
	if(path.Right(4).CompareNoCase(EXT_URL) == 0)
		search = path;
	else
		search.Format(_T("%s\\*.*"), path);

	int ncnt = 0;
	if(f.FindFile(search))
	{
		BOOL bnext = TRUE;
		while(bnext)
		{
			bnext = f.FindNextFile();

			if(f.IsDots()) continue;

			if(f.IsDirectory())
			{
				ncnt += TravelPath(f.GetFilePath(), bonlycnt);
				continue;
			}

			if(f.GetFileName().Right(4).CompareNoCase(EXT_URL) != 0)
				continue;

			if(bonlycnt)
			{
				++ncnt;
				continue;
			}

			// 링크 파일로 부터 URL 획득
			CString url = _T("");
			if(!GetURLFromURLFile(f.GetFilePath(), url)) continue;

			int nget = ParseWebPage(url);

			if(nget > 0)
				SendMessage(replyhwnd, WM_INFROM_TRAVELGET, nget, 0);

			ncnt += nget;
		}
	}

	return ncnt;
}

/**************************************************************************
 * STATIC CRSSURL::TravelThread
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Travel Thread
 * @Return
 **************************************************************************/
DWORD CRSSURL::TravelThread()
{
	// list clear
	if(!csurllist.Lock(500)) return 0;
	TRACE(_T("Travel thread Start\r\n"));
	urllist.clear();
	csurllist.Unlock();

	if(travelpath.Find(_T("/")) != -1)			// URL
	{
		// URL로 부터 rss url을 획득
		ParseWebPage(travelpath);
	}
	else
	{
		int totalcnt = TravelPath(travelpath, TRUE);

		SendMessage(replyhwnd, WM_INFORM_TRAVELCNT, totalcnt, 0);

		TravelPath(travelpath);
	}

	SendMessage(replyhwnd, WM_INFORM_TRAVELEND, 0, 0);

	hTravelThread = NULL;

	TRACE(_T("Travel thread End\r\n"));
	return 0;
}

/**************************************************************************
 * STATIC CRSSURL::GetTraveledURLs
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Travel 결과로 얻오온 URL을 받아온다.
 *
 * @Return
 **************************************************************************/
BOOL CRSSURL::GetTraveledURLs(list<CString>& l, BOOL bclear)
{
	if(urllist.size() == 0) return FALSE;

	if(!csurllist.Lock(500)) return FALSE;

	l.clear();
	l.insert(l.end(), urllist.begin(), urllist.end());

	if(bclear)
		urllist.clear();

	csurllist.Unlock();

	return TRUE;
}

/**************************************************************************
 * STATIC CRSSURL::GetTraveledURL
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Travel 결과로 얻오온 URL을 받아온다.
 *
 * @Parameters
 * (out CString&) url - 받아갈 rss url을 담는 곳
 * (out BOOL&) bnext - 더 받아갈 rss url이 있는지 여부
 * (in BOOL) bpop - 받아간 것을 지울것인지 여부 (TRUE)로 사용한다.
 *
 * @Return
 **************************************************************************/
BOOL CRSSURL::GetTraveledURL(CString& url, BOOL& bnext, BOOL bpop)
{
	if(urllist.size() == 0) return FALSE;

	if(!csurllist.Lock(500)) return FALSE;

	url = *(urllist.front());

	if(bpop)
		urllist.pop_front();

	bnext = (urllist.size() > 0);

	csurllist.Unlock();

	return TRUE;
}