// SearchManager.cpp: implementation of the CSearchManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "SearchManager.h"
#include "RSSDB.H"
#include "MainFrm.h"
#include "Dlg_Subscribe.h"
#include "LocalProperties.h"
#include "./GnuModule/GnuDoc.h"
#include "./GnuModule/GnuControl.h"
#include "./IrcModule/httpd.h"
#include "fish_common.h"		// GetLine();
#include "RSSCurrentList.h"
#include "URLEncode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_SEARCHCHANNELTIME					600			// secs
#define MAX_SEARCHPOSTTIME						60			// secs
#define INTERVAL_DATAVALID						3			// secs

#define MAX_RESULT								60			// 최대 검색 한계

extern CFISHApp theApp;

UINT CSearchManager::uSearchResultLimit	= MAX_RESULT;

//////////////////////////////////////////////////////////////////////
// SCM_ITEM
//////////////////////////////////////////////////////////////////////
CSearchManager::SCM_ITEM::~SCM_ITEM()
{
	listreadyitem.clear();
	listdoneitem.clear();
}

/**************************************************************************
 * CSearchManager::SCM_ITEM::finditem
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * 포스트의 GUID를 이용하여 포스트를 찾아낸다.
 *
 * @Parameters
 * (in CString) sguid - 포스트의 guid
 * (out BOOL&) updated - 이미 정보 갱신이 완료된 포스트인 경우 TRUE로 반환한다.
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) 검색 포스트 객체의 포인터
 **************************************************************************/
CSearchManager::SPM_ITEM* CSearchManager::SCM_ITEM::finditem(CString sguid, BOOL& updated)
{
	updated = FALSE;
	list<SPM_ITEM>::iterator it;
	for(it = listreadyitem.begin(); it != listreadyitem.end(); ++it)
	{
//		TRACE(_T("items sguid %s - %s\r\n"), (*it).sguid, sguid);
		if((*it).sguid == sguid)
		{
			return &(*it);
		}
	}

	for(it = listdoneitem.begin(); it != listdoneitem.end(); ++it)
	{
		if((*it).sguid == sguid)
		{
			updated = TRUE;
			return &(*it);
		}
	}

	return NULL;
}

/**************************************************************************
 * method CSearchManager::SCM_ITEM::Update_PostID
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-09
 *
 * @Description
 * 포스트의 GUID를 이용하여 포스트의 postid값을 갱신한다.
 *
 * @Parameters
 * (in CString) sguid - 포스트의 guid
 * (in int) postid - 설정하려고 하는 postid
 *
 * @Returns
 * (void)
 **************************************************************************/
void CSearchManager::SCM_ITEM::Update_PostID(CString sguid, int postid)
{
	BOOL updated;
	SPM_ITEM* pitem = finditem(sguid, updated);

	if(!pitem) return;

	pitem->postid = postid;
}

/**************************************************************************
 * method CSearchManager::SCM_ITEM::Get_PostID
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-09
 *
 * @Description
 * 포스트의 guid를 이용하여 포스트의 postid값을 반환한다.
 *
 * @Parameters
 * (in CString) sguid - 찾으려고 하는 포스트의 guid
 *
 * @Returns
 * (int) 반환되는 post id
 **************************************************************************/
int CSearchManager::SCM_ITEM::Get_PostID(CString sguid)
{
	BOOL updated;
	SPM_ITEM* pitem = finditem(sguid, updated);

	if(!pitem) return 0;

	return pitem->postid;
}

/**************************************************************************
 * CSearchManager::SCM_ITEM::AddItem
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 * modified 2006-03-15 :: parameter keyword 추가 by moonknit
 *
 * @Description
 * 포스트의 GUID와 검색 결과 정보를 이용하여 포스트의 노드 정보등을 갱신한다.
 *
 * @Parameters
 * (in CString) sguid - 검색된 포스트의 고유 아이디
 * (in SE_ITEM&) item - 검색 결과 정보
 * (in CString) keyword - 검색어
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) 검색 포스트 객체의 포인터
 **************************************************************************/
CSearchManager::SPM_ITEM* CSearchManager::SCM_ITEM::AddItem(CString sguid, SE_ITEM& item, CString keyword)
{
	BOOL updated;
	SPM_ITEM* pitem = finditem(sguid, updated);

	// COleDateTime에서 ASSERT 발생을 막기 위해서
	tm *ptm = localtime(&item.di.pubdate);
	if(!ptm) return NULL;

	if(!pitem)
	{
		// 새로 검색 결과를 추가하지는 않는다.
		if(resultcnt >= CSearchManager::uSearchResultLimit) return FALSE;

		if(resultcnt == 0)
			CSearchManager::KeywordSendToWeb(keyword);

		++resultcnt;
		SPM_ITEM sitem;
		sitem.sguid = sguid;
		sitem.channelid = channelid;
		sitem.stitle = item.di.name;

//		TRACE(_T("guid : %s, title : %s\r\n"), sguid, item.di.name);

		listreadyitem.push_back(sitem);
		pitem = &(listreadyitem.back());
		pitem->dtlastvalidate = COleDateTime::GetCurrentTime();

		// RSSDB에 포스트를 추가한다.
		auto_ptr<POSTITEM> apitem(new POSTITEM);
		apitem->channelid = channelid;
		apitem->guid = sguid;
		apitem->subject = item.di.name;
		apitem->stype = NET_SEARCH;
	
		apitem->pubdate = COleDateTime::COleDateTime(item.di.pubdate);

		// added by moonknit 2006-02-22
		// url data can be transfered
		if(item.di.link == GNU_DEFAULT_URL)
			apitem->url = sguid;
		else if(item.di.link.GetLength() > 0)
			apitem->url = item.di.link;
			
		theApp.m_spRD->IPostAdd(apitem, guid, AT_SEARCH_NET);
	}

	if(pitem)
	{
		TRACE(_T("query hit flag [%d]: %d, %d, %d, %d\r\n"), pitem->postid, item.di.bread, item.di.breadon, item.di.bscrap, item.di.bfilter);
		++(pitem->uhit);
		if(item.di.bread) ++(pitem->uread);
		if(item.di.breadon) ++(pitem->ureadon);
		if(item.di.bscrap) ++(pitem->uscrap);
		if(item.di.bfilter) ++(pitem->ufilter);
//		TRACE(_T("query flag result : %d, %d, %d, %d, %d\r\n"), pitem->uread, pitem->ureadon, pitem->uscrap, pitem->ufilter, pitem->uhit);
		
		pitem->bmodified = TRUE;

		COleDateTime current = COleDateTime::GetCurrentTime();
		pitem->dtlastupdate = current;
		dtlastupdate = pitem->dtlastupdate;
		if(!updated)
			pitem->listnode.push_back(item);
	}

	return pitem;
}

/**************************************************************************
 * CSearchManager::SCM_ITEM::AddCompleteItem
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * 포스트의 GUID와 검색 결과 정보를 이용하여 포스트의 노드 정보등을 갱신한다.
 *
 * @Parameters
 * (in CString) sguid - 검색된 포스트의 고유 아이디
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) 검색 포스트 객체의 포인터
 **************************************************************************/
CSearchManager::SPM_ITEM* CSearchManager::SCM_ITEM::AddCompleteItem(POSTITEM& item)
{
	BOOL updated;
	BOOL created = FALSE;
	SPM_ITEM* pitem = finditem(item.guid, updated);

	if(!pitem)
	{
		++resultcnt;
		SPM_ITEM sitem;
		sitem.sguid = item.guid;
		sitem.channelid = channelid;
		sitem.stitle = item.subject;

//		TRACE(_T("guid : %s, title : %s\r\n"), item.guid, item.subject);

		listdoneitem.push_back(sitem);
		pitem = &(listdoneitem.back());

		pitem->dtlastvalidate = COleDateTime::GetCurrentTime();

		// RSSDB에 포스트를 추가한다.
		auto_ptr<POSTITEM> apitem(new POSTITEM);
		*apitem = item;
		apitem->channelid	= channelid;
		apitem->svread		= 0;
		apitem->svreadon	= 0;
		apitem->svscrap		= 0;
		apitem->svfilter	= 0;

		theApp.m_spRD->IPostAdd(apitem, guid, AT_SEARCH_NET);

		created = TRUE;
	}

	if(pitem)
	{
		pitem->bmodified = TRUE;

		pitem->dtlastupdate = COleDateTime::GetCurrentTime();
		dtlastupdate = pitem->dtlastupdate;
		if(!updated && !created)
		{
			MoveToDone(item.guid);
			int postid = Get_PostID(item.guid);

			auto_ptr<POSTITEM> apitem(new POSTITEM);
			*apitem = item;
			apitem->postid = postid;
			apitem->channelid = channelid;
			apitem->svread = pitem->uread;
			apitem->svreadon = pitem->ureadon;
			apitem->svscrap = pitem->uscrap;
			apitem->svfilter = pitem->ufilter;

			theApp.m_spRD->IPostUpdate(apitem);
		}
	}

	return pitem;
}

/**************************************************************************
 * CSearchManager::SCM_ITEM::MoveToDone
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * 포스트의 GUID를 이용하여 포스트를 정보 갱신 대기 목록에서 정보 갱신 완료 목록으로 옮긴다.
 *
 * @Parameters
 * (in CString) sguid - 포스트의 guid
 **************************************************************************/
void CSearchManager::SCM_ITEM::MoveToDone(CString sguid)
{
	list<SPM_ITEM>::iterator it;
	for(it = listreadyitem.begin(); it != listreadyitem.end(); ++it)
	{
		if((*it).sguid == sguid)
		{
			break;
		}
	}

	if(it != listreadyitem.end())
	{
		listdoneitem.push_back((*it));
		listreadyitem.erase(it);
	}
}

//static int TotalDownload = 0;

/**************************************************************************
 * CSearchManager::SCM_ITEM::Manage
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * 오래된 포스트 삭제 및 다운로드 수행
 **************************************************************************/
void CSearchManager::SCM_ITEM::Manage()
{
	if(!tomanage) return;

	list<SPM_ITEM>::iterator it, tit;

	COleDateTime cur, removetime, validtime;
	COleDateTimeSpan s;
	int nMaxTime, nValidTime;
	nMaxTime = MAX_SEARCHPOSTTIME;
	nValidTime = INTERVAL_DATAVALID;
	s.SetDateTimeSpan(0, 0, 0, nMaxTime);

	cur = COleDateTime::GetCurrentTime();

	removetime = cur - s;
	s.SetDateTimeSpan(0, 0, 0, nValidTime);
	validtime = cur - s;
	BOOL bNeedCount = TRUE;

	int nremain = listreadyitem.size();
	int cnt_loader = 0;

	for(it = listreadyitem.begin(); it != listreadyitem.end(); )
	{
		tit = it;
		++it;

		if((*tit).lpdownloader != NULL)
		{
			++cnt_loader;
			if((*tit).bmodified && (*tit).dtlastvalidate < validtime)
			{
				(*tit).dtlastvalidate = cur;

				(*tit).bmodified = FALSE;

				auto_ptr<POSTITEM> apitem(new POSTITEM);

				apitem->postid = (*tit).postid;
				apitem->channelid = (*tit).channelid;

				apitem->svread = (*tit).uread;
				apitem->svreadon = (*tit).ureadon;
				apitem->svscrap = (*tit).uscrap;
				apitem->svfilter = (*tit).ufilter;
				apitem->svhit = (*tit).uhit;

				theApp.m_spRD->IPostSVUpdate(apitem);
			}
		}
		else if(bNeedCount) // if((*tit).lpdownloader != NULL)
		{
			// Post id를 받아온 아이템에 대해서만 처리한다.
			// 다운로드 가능 개수가 남아있는지 확인한다.
			// 다운로더를 생성하여 (*tit).listnode 를 넘겨준다. 
			if(!theApp.m_spGD->m_Control.GetDownloadingSlot())
				bNeedCount = FALSE;

			if((*tit).postid != 0)
			{
//				++TotalDownload;
//				TRACE(_T("Total Download Count : %d\r\n"), TotalDownload);
				// 변수 초기화 
				TRACE(_T("Download Item : %s\r\n"), (*tit).sguid);

				ResultGroup ItemGroup;
				Result		Item;

				ItemGroup.Name			=	(*tit).stitle;

				CString FileNameLower (ItemGroup.Name) ;
				FileNameLower.MakeLower();

				ItemGroup.Type			=	GDT_POST;
				ItemGroup.NameLower		=	FileNameLower;
				ItemGroup.AvgSpeed		=	0;
				ItemGroup.Size			=	0;
				ItemGroup.bDownloading	=   false;
				ItemGroup.bCompleted	=   false;
				ItemGroup.bNoHosts		=	false;
				ItemGroup.SearchGUID	=	guid;
				ItemGroup.Sha1Hash		=	(*tit).sguid;

				list<SNM_ITEM>::iterator it2;

				for(it2 = (*tit).listnode.begin(); it2 != (*tit).listnode.end(); ++it2)
				{
					Item.FileIndex			=   (*it2).di.nid;
					Item.Name				=	ItemGroup.Name;
					Item.NameLower			=	ItemGroup.NameLower;
					Item.UserID				=	(*it2).ui.suid;
					Item.Size				=	0;
					Item.Host				=	StrtoIP( (*it2).ui.shostip );
					Item.Port				=	(WORD) (*it2).ui.ngnuport;
					Item.Busy				=	false;
					Item.Firewall			=	false;
					Item.wVersion			=	(*it2).ui.wVersion;

					ItemGroup.ResultList.push_back(Item);
				}

				(*tit).lpdownloader = (LPVOID) theApp.m_spGD->m_Control.Download(ItemGroup);
			} // if((*tit).postid != 0)
		} // if((*tit).lpdownloader != NULL)
	}

	for(it = listdoneitem.begin(); it != listdoneitem.end(); )
	{
		tit = it;
		++it;

		if((*tit).bmodified && (*tit).dtlastvalidate < validtime)
		{
			// list doneitem 에서 제거할 때에는 검색 데이터의 최종 정보를 갱신한다.
			(*tit).dtlastvalidate = cur;
			if((*tit).postid)
			{
				(*tit).bmodified = FALSE;

				auto_ptr<POSTITEM> apitem(new POSTITEM);
				apitem->postid = (*tit).postid;
				apitem->channelid = (*tit).channelid;
				apitem->svread = (*tit).uread;
				apitem->svreadon = (*tit).ureadon;
				apitem->svscrap = (*tit).uscrap;
				apitem->svfilter = (*tit).ufilter;
				apitem->svhit = (*tit).uhit;

				theApp.m_spRD->IPostSVUpdate(apitem);
			}
		}
		else if((*tit).dtlastupdate < removetime)
		{

			listdoneitem.erase(tit);
		}
	}

//	TRACE(_T("Ready Item : %d - Downloading : %d, Completed : %d\r\n")
//		, listreadyitem.size(), cnt_loader, listdoneitem.size());

	if(listdoneitem.size() == 0 && listreadyitem.size() == 0)
		tomanage = FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchManager::CSearchManager()
{
	ResetMaxSearchLimit();
}

CSearchManager::~CSearchManager()
{
	TRACE(_T("CSearchManager Destroy\r\n"));
	listSearch.clear();
}

/**************************************************************************
 * CSearchManager::FindChannelID
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * guid(검색id)를 이용하여 채널 아이디를 찾는다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 *
 * @Returns
 * (int) 검색된 채널의 id, 0은 발견 실패
 **************************************************************************/
int CSearchManager::FindChannelID(GUID guid)
{
	SCM_ITEM* pitem = FindChannelItem(guid);

	if(pitem)
		return pitem->channelid;

	return 0;
}

/**************************************************************************
 * CSearchManager::FindChannelItem
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * guid(검색id)를 이용하여 검색 채널 객체의 포인터를 반환한다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 *
 * @Returns
 * (SCM_ITEM*) 검색된 채널 객체의 포인터로 NULL은 발견을 실패했음을 의미한다.
 **************************************************************************/
CSearchManager::SCM_ITEM* CSearchManager::FindChannelItem(GUID guid)
{
	list<SCM_ITEM>::iterator it;
	for(it = listSearch.begin(); it != listSearch.end(); ++it)
	{
		if((*it).guid == guid)
		{
			return &(*it);
		}
	}

	return NULL;
}


/**************************************************************************
 * CSearchManager::InsertSearchResult
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * guid(검색id)를 이용하여 채널을 찾아 검색 결과를 등록한다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 * (in CString) sguid - 검색된 포스트의 고유 아이디
 * (in SE_ITEM&) item - 검색 결과 정보
 **************************************************************************/
BOOL CSearchManager::InsertSearchResult(GUID guid, CString sguid, SE_ITEM& item)
{
//	TRACE(_T("Search Result with SGUID : %s\r\n"), sguid);
	SCM_ITEM* pitem = FindChannelItem(guid);
	if(pitem)
	{
		if(pitem->AddItem(sguid, item, pitem->keyword) != NULL)
		{
			pitem->tomanage = TRUE;
			Manage();
			return TRUE;
		}
	}
	return FALSE;
}

/**************************************************************************
 * CSearchManager::InsertSearchResult
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-11
 *
 * @Description
 * guid(검색id)를 이용하여 채널을 찾아 검색 결과를 등록한다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 * (in POSTITEM&) item - 검색 결과 정보
 **************************************************************************/
BOOL CSearchManager::InsertSearchResult(GUID guid, POSTITEM& item)
{
//	TRACE(_T("Search Result FROM LOCAL\r\n"));
	SCM_ITEM* pitem = FindChannelItem(guid);
	if(pitem)
	{
		if(pitem->AddCompleteItem(item) != NULL)
		{
			pitem->tomanage = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

/**************************************************************************
 * CSearchManager::PostComplete
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * guid(검색id)를 이용하여 포스트의 완료작업을 수행한다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 * (in CString) sguid - 포스트의 id
 * (in/out auto_ptr<POSTITEM>&) apitem - 추가할 포스트
 **************************************************************************/
BOOL CSearchManager::PostComplete(GUID guid, CString sguid, auto_ptr<POSTITEM>& apitem)
{
	TRACE(_T("Post Complete : %s\r\n"), sguid);
	SCM_ITEM* pitem = FindChannelItem(guid);
	if(pitem)
	{
		BOOL bupdated;
		SPM_ITEM* pitem2 = pitem->finditem(sguid, bupdated);
		if(!pitem2) return FALSE;

		// set read & readon & scrap & filter count
		apitem->svread = pitem2->uread;
		apitem->svreadon = pitem2->ureadon;
		apitem->svscrap = pitem2->uscrap;
		apitem->svfilter = pitem2->ufilter;
		apitem->svhit = pitem2->uhit;

		pitem->MoveToDone(sguid);
		int postid = pitem->Get_PostID(sguid);
		TRACE(_T("Completed Item postid : %d\r\n"), postid);
		apitem->postid = postid;
		apitem->channelid = pitem->channelid;
		theApp.m_spRD->IPostUpdate(apitem);

		Manage();

		return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 * method CSearchManager::PostUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-09
 *
 * @Description
 * PostCompleted에 의해서 수행된 Post Add의 결과 발생되는 postid값을 갱신한다.
 *
 * @Parameters
 * (in GUID) guid - 검색 id
 * (in CString) sguid - 포스트의 id
 * (in int) postid - 설정할 postid값
 **************************************************************************/
BOOL CSearchManager::PostUpdate(GUID guid, CString sguid, int postid)
{
	SCM_ITEM* pitem = FindChannelItem(guid);
	if(pitem)
	{
		pitem->Update_PostID(sguid, postid);

		return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 * CSearchManager::Manage
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-07
 *
 * @Description
 * 1. 다운로드 수행
 * 2. 오래된 포스트 삭제
 **************************************************************************/
void CSearchManager::Manage()
{
	list<SCM_ITEM>::iterator it, tit;

	// 오래된 포스트 삭제 & 다운로드 수행
//	COleDateTime cur;
//	COleDateTimeSpan s;
//	s.SetDateTimeSpan(0, 0, 0, nMaxTime);

//	cur = COleDateTime::GetCurrentTime() - s;

	int size = listSearch.size();

	for(it = listSearch.begin(); it != listSearch.end();)
	{
		tit = it;
		++it;

//		if((*tit).dtlastupdate < cur)
//			listSearch.erase(tit);
//		else
		{
			(*tit).Manage();
		}

	}
}

#define DEFAULT_SEARCHMAX						0

/**************************************************************************
 * CSearchManager::Search
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 검색 요청
 *
 * @Parameters
 * (in CString) key - 검색어
 * (in int) type - 검색의 종류
 **************************************************************************/
GUID CSearchManager::Search(CString key, int type, TERM t)
{
//	TRACE(_T("CSearchManager::Search"));
	// 검색 기간을 수정한다.
	// 검색 요청자의 기간은 local 시간 기준이므로 gmt +0 기준으로 변경한다.
	GUID guid;
	int chid = 0;
	guid = NULL_GUID;

	t -= theApp.m_spLP->GetSPGMT();

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return guid;

	if(type == LOCAL_SEARCH)
	{
		
		SmartPtr<CDlg_Subscribe> spsubscribe;
		pFrame->GetDlgSubscribe(spsubscribe);
		if(!spsubscribe) return guid;

		chid = spsubscribe->RequestSearchChannelAdd(key, CT_LSEARCH);

		theApp.m_spCL->SetCurrentChannelType(CT_LSEARCH);
	}
	else
	{
		// Gnucontrol을 통해 검색을 시작하고 guid를 받아온다.
		COleDateTimeSpan span = t.to - t.from;
		int nspanhour = (int) span.GetTotalHours();
		// 채널 생성 
		chid = 0;

		SmartPtr<CDlg_Subscribe> spsubscribe;
		pFrame->GetDlgSubscribe(spsubscribe);
		if(!spsubscribe) return guid;

		chid = spsubscribe->RequestSearchChannelAdd(key, CT_SEARCH, FALSE);

		theApp.m_spCL->SetCurrentChannelType(CT_SEARCH);

		guid = theApp.m_spGD->m_Control.SendQuery(key, type, t.from, nspanhour, DEFAULT_SEARCHMAX);

		// add search
		SCM_ITEM item;
		item.guid = guid;
		item.channelid = chid;
		item.keyword = key;

		listSearch.push_back(item);

		// search
		// PROPERTY
		// local search가 함께 수행한다.

#ifdef USE_ALLSEARCHWITHLOCAL
		{
			SE_QUERY querydata;
			querydata.Origin = NULL;
			querydata.QueryGuid = guid;

			theApp.m_spRD->IRequestNetSearch(LOCAL_SEARCH, key, _T(""), _T(""), 0, querydata, chid);
		}
#endif
	}

	if(chid != 0)
	{
        theApp.m_spCL->SetListMode(TRUE);
		pFrame->ChangeToListView(chid);
	}
	
	return guid;
}

/**************************************************************************
 * method CSearchManager::KeywordSendToWeb
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-15
 *
 * @Description
 * 웹 페이지에 검색어를 등록한다.
 **************************************************************************/
BOOL CSearchManager::KeywordSendToWeb(CString keyword)
{
	CMemFile SNList;
	TCHAR line[MAX_BUFF+1]={0};
	CString str = _T(""), rstr;


	int		iUnicodeLen=0, iUtfLen=0;
	unsigned short* sUnicode = NULL;
	char*	sUTF = NULL;

//	TRACE(_T("keyword : %s\r\n"), keyword);

#ifdef _UNICODE
	iUtfLen = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR) keyword, -1, sUTF, NULL, NULL, NULL);
	if(iUtfLen)
	{
		sUTF = (char*) malloc(iUtfLen);
		WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR) keyword, -1, sUTF, iUtfLen, NULL, NULL);
	}
#else
	//unicode 전환
	iUnicodeLen = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)keyword, 
		m_sFileNm.GetLength(), 
		NULL, NULL);
	sUnicode = (unsigned short*)malloc(iUnicodeLen);
	memset((void*)sUnicode, 0x00, iUnicodeLen);
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)keyword,
		m_sFileNm.GetLength(), 
		sUnicode, iUnicodeLen);	
	sUnicode[iUnicodeLen] = 0;
//	TRACE(_T("unicode keyword[%d] : %s\r\n"), iUnicodeLen, sUnicode);

	//utf-8 전환
	iUtfLen = WideCharToMultiByte(CP_UTF8, 0, sUnicode, iUnicodeLen,
		NULL, NULL, NULL, NULL);
	sUTF = (char*)malloc(iUtfLen);
	memset((void*)sUTF, 0x00, iUtfLen);

	WideCharToMultiByte(CP_UTF8, 0, sUnicode, iUnicodeLen,
		sUTF, iUtfLen, NULL, NULL);

	sUTF[iUtfLen] = 0;
//	TRACE(_T("utf8 keyword : %s\r\n"), sUTF, iUtfLen);
#endif

	CURLEncode clsEncode;
	CString sKeyword = clsEncode.URLEncode(sUTF, iUtfLen);

	str.Format(_T("%s?cc=0006&kk=%s")
		, URL_WEBDB
		, sKeyword);

//	TRACE(_T("keyword send : %s\r\n"), str);

	if( !GetHttpFileAsc( HOST_NAME, 
		80, (LPTSTR)((LPCTSTR )str), &SNList, NULL, NULL) )
	{
		// ERROR : URL, PORT, filename중 하나가 비정상
		SNList.Close();
		return FALSE;
	}
	
	// 요청 결과
	if( GetLine( &SNList, line) < 0)
	{
		// ERROR임
		SNList.Close();
		return FALSE;
	}

	int ret = 0;
	// 결과 오류 검색
	BOOL ischeck = CheckResultCode( line , ret );
	SNList.Close();

	return (ret == 1);
}

/**************************************************************************
 * method CSearchManager::RemoveChannel
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-21
 *
 * @Description
 * 해당 채널 id를 가진 검색 단위를 삭제한다.
 **************************************************************************/
void CSearchManager::RemoveChannel(int channelid)
{
	list<SCM_ITEM>::iterator it;
	for(it = listSearch.begin(); it != listSearch.end(); ++it)
	{
		if((*it).channelid == channelid)
		{
			listSearch.erase(it);
			break;
		}
	}
}

void CSearchManager::ResetMaxSearchLimit()
{
	if(theApp.m_spCL != NULL) uSearchResultLimit = theApp.m_spLP->GetSRL();
}