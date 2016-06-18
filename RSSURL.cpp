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
 * �ش� HTTP URL �� �̿��Ͽ� RSS XML URL�� ȹ���Ѵ�.
 *
 * @Parameters
 * url�� �̿��Ͽ� webpage�� ���� rssurl�� ã�Ƴ���.
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
 * Travel Thread�� ������ �ݴ´�.
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
 * Travel Thread ���� ���θ� Ȯ���Ѵ�.
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
 * �ܺ��� ��û�� ���� Path ������ URL ���Ϸ� ���� RSS URL ȹ���۾��� �����Ѵ�.
 *
 * @Parameters
 * (in CString) path - URL ������ �˻��� ���丮 �Ǵ� ���ϸ�
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
 * url ������ �̿��Ͽ� htmlurl�� ����.
 *
 * @Return
 * (int) - ã�Ƴ� rss url�� ����
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
 * web page �� �Ľ��Ͽ� RSS url�� ȹ���Ѵ�.
 *
 * @Return
 * (int) - �� ���������� ���� RSS url�� ����
 **************************************************************************/
int CRSSURL::ParseWebPage(CString url)
{
	list<CString> rsslist;
	// URL�� ���� rss url�� ȹ��
	if(!GetFeedURL(url, rsslist)) return 0;

	// ����� ��Ͽ� �ִ´�.
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
 * ���丮 ������ url ������ �̿��Ͽ� rss url�� �����س���.
 * recursive function
 *
 * @Parameters
 * (in CString) path - �˻��� ���丮
 * (in BOOL) bonlycnt - ���Ϸ� ���� RSS URL ������ ���� �ʰ� url ������ ������ ����Ѵ�.
 *
 * @Return
 * (int) - ã�Ƴ� rss url�� ����
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

			// ��ũ ���Ϸ� ���� URL ȹ��
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
		// URL�� ���� rss url�� ȹ��
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
 * Travel ����� ����� URL�� �޾ƿ´�.
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
 * Travel ����� ����� URL�� �޾ƿ´�.
 *
 * @Parameters
 * (out CString&) url - �޾ư� rss url�� ��� ��
 * (out BOOL&) bnext - �� �޾ư� rss url�� �ִ��� ����
 * (in BOOL) bpop - �޾ư� ���� ��������� ���� (TRUE)�� ����Ѵ�.
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