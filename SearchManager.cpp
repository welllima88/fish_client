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

#define MAX_RESULT								60			// �ִ� �˻� �Ѱ�

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
 * ����Ʈ�� GUID�� �̿��Ͽ� ����Ʈ�� ã�Ƴ���.
 *
 * @Parameters
 * (in CString) sguid - ����Ʈ�� guid
 * (out BOOL&) updated - �̹� ���� ������ �Ϸ�� ����Ʈ�� ��� TRUE�� ��ȯ�Ѵ�.
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) �˻� ����Ʈ ��ü�� ������
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
 * ����Ʈ�� GUID�� �̿��Ͽ� ����Ʈ�� postid���� �����Ѵ�.
 *
 * @Parameters
 * (in CString) sguid - ����Ʈ�� guid
 * (in int) postid - �����Ϸ��� �ϴ� postid
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
 * ����Ʈ�� guid�� �̿��Ͽ� ����Ʈ�� postid���� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in CString) sguid - ã������ �ϴ� ����Ʈ�� guid
 *
 * @Returns
 * (int) ��ȯ�Ǵ� post id
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
 * modified 2006-03-15 :: parameter keyword �߰� by moonknit
 *
 * @Description
 * ����Ʈ�� GUID�� �˻� ��� ������ �̿��Ͽ� ����Ʈ�� ��� �������� �����Ѵ�.
 *
 * @Parameters
 * (in CString) sguid - �˻��� ����Ʈ�� ���� ���̵�
 * (in SE_ITEM&) item - �˻� ��� ����
 * (in CString) keyword - �˻���
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) �˻� ����Ʈ ��ü�� ������
 **************************************************************************/
CSearchManager::SPM_ITEM* CSearchManager::SCM_ITEM::AddItem(CString sguid, SE_ITEM& item, CString keyword)
{
	BOOL updated;
	SPM_ITEM* pitem = finditem(sguid, updated);

	// COleDateTime���� ASSERT �߻��� ���� ���ؼ�
	tm *ptm = localtime(&item.di.pubdate);
	if(!ptm) return NULL;

	if(!pitem)
	{
		// ���� �˻� ����� �߰������� �ʴ´�.
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

		// RSSDB�� ����Ʈ�� �߰��Ѵ�.
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
 * ����Ʈ�� GUID�� �˻� ��� ������ �̿��Ͽ� ����Ʈ�� ��� �������� �����Ѵ�.
 *
 * @Parameters
 * (in CString) sguid - �˻��� ����Ʈ�� ���� ���̵�
 *
 * @Returns
 * (CSearchManager::SPM_ITEM*) �˻� ����Ʈ ��ü�� ������
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

		// RSSDB�� ����Ʈ�� �߰��Ѵ�.
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
 * ����Ʈ�� GUID�� �̿��Ͽ� ����Ʈ�� ���� ���� ��� ��Ͽ��� ���� ���� �Ϸ� ������� �ű��.
 *
 * @Parameters
 * (in CString) sguid - ����Ʈ�� guid
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
 * ������ ����Ʈ ���� �� �ٿ�ε� ����
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
			// Post id�� �޾ƿ� �����ۿ� ���ؼ��� ó���Ѵ�.
			// �ٿ�ε� ���� ������ �����ִ��� Ȯ���Ѵ�.
			// �ٿ�δ��� �����Ͽ� (*tit).listnode �� �Ѱ��ش�. 
			if(!theApp.m_spGD->m_Control.GetDownloadingSlot())
				bNeedCount = FALSE;

			if((*tit).postid != 0)
			{
//				++TotalDownload;
//				TRACE(_T("Total Download Count : %d\r\n"), TotalDownload);
				// ���� �ʱ�ȭ 
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
			// list doneitem ���� ������ ������ �˻� �������� ���� ������ �����Ѵ�.
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
 * guid(�˻�id)�� �̿��Ͽ� ä�� ���̵� ã�´�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 *
 * @Returns
 * (int) �˻��� ä���� id, 0�� �߰� ����
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
 * guid(�˻�id)�� �̿��Ͽ� �˻� ä�� ��ü�� �����͸� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 *
 * @Returns
 * (SCM_ITEM*) �˻��� ä�� ��ü�� �����ͷ� NULL�� �߰��� ���������� �ǹ��Ѵ�.
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
 * guid(�˻�id)�� �̿��Ͽ� ä���� ã�� �˻� ����� ����Ѵ�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 * (in CString) sguid - �˻��� ����Ʈ�� ���� ���̵�
 * (in SE_ITEM&) item - �˻� ��� ����
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
 * guid(�˻�id)�� �̿��Ͽ� ä���� ã�� �˻� ����� ����Ѵ�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 * (in POSTITEM&) item - �˻� ��� ����
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
 * guid(�˻�id)�� �̿��Ͽ� ����Ʈ�� �Ϸ��۾��� �����Ѵ�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 * (in CString) sguid - ����Ʈ�� id
 * (in/out auto_ptr<POSTITEM>&) apitem - �߰��� ����Ʈ
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
 * PostCompleted�� ���ؼ� ����� Post Add�� ��� �߻��Ǵ� postid���� �����Ѵ�.
 *
 * @Parameters
 * (in GUID) guid - �˻� id
 * (in CString) sguid - ����Ʈ�� id
 * (in int) postid - ������ postid��
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
 * 1. �ٿ�ε� ����
 * 2. ������ ����Ʈ ����
 **************************************************************************/
void CSearchManager::Manage()
{
	list<SCM_ITEM>::iterator it, tit;

	// ������ ����Ʈ ���� & �ٿ�ε� ����
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
 * �˻� ��û
 *
 * @Parameters
 * (in CString) key - �˻���
 * (in int) type - �˻��� ����
 **************************************************************************/
GUID CSearchManager::Search(CString key, int type, TERM t)
{
//	TRACE(_T("CSearchManager::Search"));
	// �˻� �Ⱓ�� �����Ѵ�.
	// �˻� ��û���� �Ⱓ�� local �ð� �����̹Ƿ� gmt +0 �������� �����Ѵ�.
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
		// Gnucontrol�� ���� �˻��� �����ϰ� guid�� �޾ƿ´�.
		COleDateTimeSpan span = t.to - t.from;
		int nspanhour = (int) span.GetTotalHours();
		// ä�� ���� 
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
		// local search�� �Բ� �����Ѵ�.

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
 * �� �������� �˻�� ����Ѵ�.
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
	//unicode ��ȯ
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

	//utf-8 ��ȯ
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
		// ERROR : URL, PORT, filename�� �ϳ��� ������
		SNList.Close();
		return FALSE;
	}
	
	// ��û ���
	if( GetLine( &SNList, line) < 0)
	{
		// ERROR��
		SNList.Close();
		return FALSE;
	}

	int ret = 0;
	// ��� ���� �˻�
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
 * �ش� ä�� id�� ���� �˻� ������ �����Ѵ�.
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