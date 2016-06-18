// Feeder.cpp: implementation of the CFeeder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FISH.h"
#include "Feeder.h"
#include "FeedParser.h"
#include "fish_common.h"

#include "RSSDB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HWND CFeeder::m_hCallerWnd = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//extern CFISHApp theApp;

CFeeder::CFeeder(int channelid, CString url, COleDateTime lasttime)
	: m_ichannelid(channelid), m_surl(url), m_dtlasttime(lasttime)
{
	m_hThread = NULL;
	m_bThread = FALSE;

	m_dtstarttime = COleDateTime::GetCurrentTime();
}

CFeeder::~CFeeder()
{
	// Thread가 닫혀있지 않다면 강제로 닫는다.
	m_bStop = TRUE;

	int ncnt = 0;
//	while(m_hThread)
//	{
//		Sleep(10);
//
//		if(++ncnt == 10)	// wait 1 second
//			break;
//	}

	if(m_hThread)
		CloseHandle(m_hThread);
}

void CFeeder::SetCallerWnd(HWND h)
{
	m_hCallerWnd = h;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-10
 *
 * @Description
 * FEED 작업을 하는 메인 Thread
 **************************************************************************/
DWORD CFeeder::FeedThread(LPVOID lpparam)
{
	if(!lpparam)
		return -1;

	CFeeder* pfeeder = (CFeeder*) lpparam;
	pfeeder->m_bStop = FALSE;

	int channelid = pfeeder->m_ichannelid;

	TRACE(_T("Feed thread Start [channel %d]\r\n") , channelid);

	SendMessage(m_hCallerWnd, WM_CHANNELSTATECHANGE, channelid, CS_FEEDING);

	pfeeder->m_bToInform = TRUE;

	CoInitialize(NULL);	

	auto_ptr<POST_LIST> aplist;
	// 여기서 feed 업무 처리
	BOOL result = FALSE;

	CEzXMLParser parser;
	COleDateTime lasttime;

	try
	{
		result = parser.LoadXML((LPTSTR) (LPCTSTR) pfeeder->m_surl, CEzXMLParser::LOAD_WEB);
	}
	catch (CException*)
	{
	}

	if(result && pfeeder->m_bStop != TRUE)
	{
		// 파싱 요청~
		// TO DO
		// parser, aplist, pfeeder->m_dtlasttime, pfeeder->m_ichannelid
		// aplist에는 결과 저장
		// pfeeder->m_dtlasttime은 최종 channel갱신 시간을 비교하여 새로 업데이트 할 내용이 있는지 판단하기 위한 정보
		// post중 pubdate가 m_dtlasttime 이후의 것만 가져온다.
		// pfeeder->m_ichannelid는 post의 channelid정보
		
		CFeedParser feedparser;

		IXMLDOMNodeListPtr pnodelist;
		IXMLDOMNodePtr pnode;
		CString version;

		aplist = auto_ptr<POST_LIST> (new POST_LIST);

		// version check
		try
		{
			pnodelist = parser.SearchNodes(_T("*"));

			if(pnodelist != NULL && pnodelist->length != 0)
			{
				pnode = pnodelist->item[0];

				version = (LPTSTR) (pnode->nodeName);

				if(version == _T("rss"))
				{
					version += GetNamedNodeText(XML_VERSION, &parser, pnode, 0);
				//	TRACE(version + "\n");
				}
				else if (version == _T("rdf:RDF"))
				{
					version = RV_RSS10;
				//	TRACE(version + "\n");
				}
				else if (version == _T("feed"))
				{
					if(GetNamedNodeText(XML_VERSION, &parser, pnode, 0).IsEmpty())
					{
						version = RV_ATOM10;
					}
					else
					{
						version = RV_ATOM03;
					}
				//	TRACE(version + "\n");
				}

				// parsing
				result = feedparser.Parsing(&parser, aplist.get(), channelid, pfeeder->m_dtlasttime, version);
			}

		}catch(_com_error& e)
		{
			PrintComError(e);
			result = FALSE;
		}

	}

	pfeeder->m_bThread = FALSE;

	CoUninitialize();

	if(!pfeeder->m_bToInform) return -1;

	if(result)
	{
		// Feed 결과를 CRSSDB로 넘김
		if(!theApp.IsClosing())
			theApp.m_spRD->IPostAdd(aplist, lasttime, AT_FETCH, channelid);
		// 구독 관리자에게 구독 성공을 알림
	}
	else
	{
		// 구독 실패시 구독관리에 알림
		SendMessage(m_hCallerWnd, WM_CHANNELSTATECHANGE, channelid, CS_FAILED);
	}

	PostMessage(m_hCallerWnd, WM_FEEDCOMPLETE, channelid, (LPARAM) TRUE);

	TRACE(_T("FEED Thread End [channel %d]\r\n"), channelid);

	return 0;
}

/**************************************************************************
 * method CFeeder::StartFeed
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-10
 *
 * @Description
 * FeedThread를 실행한다.
 **************************************************************************/
BOOL CFeeder::StartFeed()
{
#ifdef FISH_TEST
	FeedThread((LPVOID) this);
#else
	if(m_hThread)
	{
		return FALSE;
	}

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) FeedThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	m_hThread = hThread;
	m_bThread = TRUE;
	ResumeThread(hThread);
#endif

	return TRUE;
}

BOOL CFeeder::IsFeeding()
{
	return m_bThread;
}

int CFeeder::GetChannelID()
{
	return m_ichannelid;
}
