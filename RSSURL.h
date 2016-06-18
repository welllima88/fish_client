// RSSURL.h: interface for the CRSSURL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSSURL_H__C2481C8B_6B18_46C0_883E_2153127EA0E8__INCLUDED_)
#define AFX_RSSURL_H__C2481C8B_6B18_46C0_883E_2153127EA0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

using namespace std;

class CRSSURL  
{
protected:
	static DWORD WINAPI TravelThread();
	static void StopTravelThread();
	static int TravelPath(CString path, BOOL bonlycnt = FALSE);

	static list<CString> urllist;
	static CString travelpath;
	static HANDLE hTravelThread;
	static HWND replyhwnd;

public:
	static void SetReplyHWND(HWND hreply);
	static int ParseWebPage(CString url);
	static BOOL GetFeedURL(CString httpurl, list<CString>& resulturl);
	static BOOL GetURLFromURLFile(CString filename, CString& htmlurl);

	static BOOL IsTraveling();
	static BOOL GetTraveledURLs(list<CString>& l, BOOL bclear = TRUE);
	static BOOL GetTraveledURL(CString& url, BOOL& bnext, BOOL bpop = TRUE);
	static BOOL TravelForXMLURL(CString path);

	CRSSURL();
	virtual ~CRSSURL();

};

#endif // !defined(AFX_RSSURL_H__C2481C8B_6B18_46C0_883E_2153127EA0E8__INCLUDED_)
