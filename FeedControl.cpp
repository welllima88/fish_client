// FeedControl.cpp: implementation of the CFeedControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FISH.h"
#include "FeedControl.h"
#include "fish_def.h"
#include <memory>
#include <list>
#include "RSSDB.h"
#include "fish_common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

// ������ channel id�� ������ �������� �ߺ��ؼ� ������ �� ����.
// FEED�� ������̰ų� �������� �������� ������ ������.
struct FEED_ITEM
{
	int channelid;
	CString url;
	COleDateTime lasttime;
	auto_ptr<CFeeder> apfeeder;

	FEED_ITEM(int chid, CString u, COleDateTime l)
		: channelid(chid), url(u), lasttime(l)
	{
	}

	~FEED_ITEM()
	{
	}
};

typedef list<auto_ptr<FEED_ITEM> > LIST_FEEDITEM;
CRITICAL_SECTION g_csfeedlist;

LIST_FEEDITEM g_listFeedReady;
LIST_FEEDITEM g_listFeeding;

#define MAX_FEEDCNT					5

BOOL IsFeeding(LIST_FEEDITEM& l, int channelid)
{
	LIST_FEEDITEM::iterator it;

	for(it = l.begin(); it != l.end(); ++it)
	{
		if((*it)->channelid == channelid)
			return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedControl::CFeedControl()
{
	InitializeCriticalSection(&g_csfeedlist);
	m_hCallerWnd = NULL;
}

CFeedControl::~CFeedControl()
{
	TRACE(_T("CFeedControl Destroy\r\n"));
	g_listFeedReady.clear();

//	int cnt = 0;
//	while(g_listFeeding.size() > 0)
//	{
//		Sleep(1000);
//
//		if(++cnt == 10)
//		{
//			g_listFeeding.clear();
//		}
//	}
	g_listFeeding.clear();

	DeleteCriticalSection(&g_csfeedlist);
}

#define MAX_FEEDTIME					60 // seconds

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-10
 *
 * @Description
 * Feed�� �����ϰ� �����Ѵ�.
 **************************************************************************/
void CFeedControl::Feed()
{
	if(g_listFeedReady.size() == 0 && g_listFeeding.size() == 0) return;

	static BOOL Feed;

	if(Feed) return;

//	TRACE(_T("feed report : ready [%d], feeding [%d]\r\n"), g_listFeedReady.size(), g_listFeeding.size());

	Feed = TRUE;

	EnterCriticalSection(&g_csfeedlist);
	LIST_FEEDITEM::iterator it, tit;

	int i = g_listFeeding.size();

	COleDateTime cur = COleDateTime::GetCurrentTime();
	COleDateTimeSpan s;
	s.SetDateTimeSpan(0, 0, 0, MAX_FEEDTIME);
	cur -= s;

	BOOL IsFeeding;

	for(it = g_listFeeding.begin(); it != g_listFeeding.end(); )
	{
		tit = it;
		// ������ ���� �ʰų�
		// ���� �ð��� ���� �ð��� ������
		++it;
		IsFeeding = ((*tit)->apfeeder.get() != NULL) && ((*tit)->apfeeder->IsFeeding());
		if(
			!IsFeeding 
			|| ((*tit)->apfeeder->m_dtstarttime < cur)		// feeding time over
			)
		{
			TRACE(_T("erase feeding [channel %d]\r\n"), (*tit)->channelid);

			// ���� ���н� ���������� �˸�
			if(IsFeeding && (*tit)->apfeeder->m_bToInform)
			{
				(*tit)->apfeeder->m_bToInform = FALSE;
				SendMessage(m_hCallerWnd, WM_CHANNELSTATECHANGE, (*tit)->channelid, CS_FAILED);
				PostMessage(m_hCallerWnd, WM_FEEDCOMPLETE, (*tit)->channelid, (LPARAM) FALSE);
			}

			g_listFeeding.erase(tit);
		}
	}
	
	CFeeder* pfeeder = NULL;

	while(g_listFeeding.size() < MAX_FEEDCNT && g_listFeedReady.size() != 0)
	{
		g_listFeeding.push_front(g_listFeedReady.back());
		g_listFeedReady.pop_back();

		it = g_listFeeding.begin();

		if(it == g_listFeeding.end())
			break;

		if((*it).get() != NULL)
		{
			TRACE(_T("feeding : ["));
			TRACE((*it)->url);
			TRACE(_T("]\r\n"));

			pfeeder = new CFeeder((*it)->channelid, (*it)->url, (*it)->lasttime);
			g_listFeeding.front()->apfeeder = auto_ptr<CFeeder> (pfeeder);
			pfeeder->StartFeed();
		}
	}

	Feed = FALSE;

	LeaveCriticalSection(&g_csfeedlist);
}

/**************************************************************************
 * method CFeedControl::AddFeed
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-10
 *
 * @Description
 * ���ο� Feed�� �߰��Ѵ�.
 *
 * @Comment
 * ���� ä���� Pubdate Pasrsing ������ RSS ������ ������ �߰������� ������ �� �ֵ��� �Ѵ�. 
 * (Parsing ���� ���ϸ� ���� �� �ִ�.)
 * by moonknit
 *
 * @Parameters
 * (in int) channelid - Feed�� ������ ä���� id
 * (in CString) url - Feed�� ������ ä���� url
 * (in COleDateTime) lasttime - Feed�� ������ ä���� ���� Post ���� �ð�
 **************************************************************************/
BOOL CFeedControl::AddFeed(int channelid, CString url, COleDateTime lasttime)
{
	EnterCriticalSection(&g_csfeedlist);
	if(g_listFeeding.size() == 0 && g_listFeedReady.size() == 0)
	{
		// �ֱ� ����... ���� �� �ִ°�?
		g_listFeeding.push_front(auto_ptr<FEED_ITEM> (new FEED_ITEM(channelid, url, lasttime)) );
		CFeeder* pfeeder = new CFeeder(channelid, url, lasttime);
		g_listFeeding.front()->apfeeder = auto_ptr<CFeeder> (pfeeder);
		pfeeder->StartFeed();

		LeaveCriticalSection(&g_csfeedlist);
		return TRUE;
	}

	if(IsFeeding(g_listFeeding, channelid) || IsFeeding(g_listFeedReady, channelid))
	{
		LeaveCriticalSection(&g_csfeedlist);
		return FALSE;
	}

	g_listFeedReady.push_front(auto_ptr<FEED_ITEM> (new FEED_ITEM(channelid, url, lasttime)) );

	LeaveCriticalSection(&g_csfeedlist);

	Feed();

	return TRUE;
}

/**************************************************************************
 * method CFeedControl::ClearFeed
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-10
 *
 * @Description
 * ��� ��� Feed�� �����Ѵ�.
 **************************************************************************/
void CFeedControl::ClearFeed()
{
	EnterCriticalSection(&g_csfeedlist);
	g_listFeedReady.clear();
	LeaveCriticalSection(&g_csfeedlist);
}

/**************************************************************************
 * method CFeedControl::RemoveFeed
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-10
 *
 * @Description
 * ������� Feed ��Ͽ��� �ش� ä���� Feed�� �����Ѵ�.
 **************************************************************************/
void CFeedControl::RemoveFeed(int channelid)
{
	EnterCriticalSection(&g_csfeedlist);
	LIST_FEEDITEM::iterator it;
	for(it = g_listFeedReady.begin(); it != g_listFeedReady.end(); ++it)
	{
		if((*it)->channelid == channelid)
		{
			g_listFeedReady.erase(it);
			break;
		}
	}
	LeaveCriticalSection(&g_csfeedlist);
}

/**************************************************************************
 * method CFeedControl::IsChannelFeeding
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-17
 *
 * @Description
 * ���� �ش� ä���� Feeding ������ Ȯ���Ѵ�.
 **************************************************************************/
BOOL CFeedControl::IsChannelFeeding(int channelid)
{
	BOOL result = FALSE;
	EnterCriticalSection(&g_csfeedlist);
	if(IsFeeding(g_listFeeding, channelid) || IsFeeding(g_listFeedReady, channelid))
	{
		result = TRUE;
	}

	LeaveCriticalSection(&g_csfeedlist);

	return result;
}

/**************************************************************************
 * method CFeedControl::IsFeedAny
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-17
 *
 * @Description
 * Feed ó������ ä���� �ϳ��� ������ TRUE�� ��ȯ�Ѵ�.
 **************************************************************************/
BOOL CFeedControl::IsFeedAny()
{
	BOOL bReturn = FALSE;
	if(g_listFeeding.size() > 0) 
	{
		EnterCriticalSection(&g_csfeedlist);
		LIST_FEEDITEM::iterator it;
		for(it = g_listFeedReady.begin(); it != g_listFeedReady.end(); ++it)
		{
			if((*it)->apfeeder.get() == NULL || (*it)->apfeeder->IsFeeding())
			{
				bReturn = TRUE;
				break;
			}
		}
		LeaveCriticalSection(&g_csfeedlist);
	}
	if(g_listFeedReady.size() > 0) bReturn = TRUE;

	return bReturn;
}