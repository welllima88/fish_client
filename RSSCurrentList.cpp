// RSSCurrentList.cpp: implementation of the CRSSCurrentList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "RSSDB.h"
#include "RSSCurrentList.h"
#include "Dlg_RSSList.h"
#include "fish_struct.h"
#include "MainFrm.H"
#include "HtmlMaker.h"
#include "LocalProperties.h"
#include "GnuModule/FileLock.h"		// for CAutoLock
#include "fish_common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_IPPCNT					5
#define MAX_POSTCNT					3000
#define STR_INFORM_POSTMAXPAGEHIT	_T("한번에 볼 수 있는 포스트의 개수는 %d개가 한계입니다.")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CFISHApp theApp;

const BOOL MakeStreamToFile(const CString& filename, CString& stream)
{
	CFile tmp;
	if(tmp.Open(filename, CFile::modeCreate | CFile::modeWrite))
	{
#ifdef _UNICODE
		int l = stream.GetLength() * 2;
		char* buf = (char*) malloc(l);
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, stream.GetBuffer(stream.GetLength() + 1), stream.GetLength() + 1
			, buf, l, NULL, NULL);
		tmp.Write(buf, strlen(buf));
		delete buf;
#else
		tmp.Write(stream.GetBuffer(stream.GetLength() + 1), stream.GetLength() + 1);
#endif
		tmp.Close();
		return TRUE;
	}

	return FALSE;
}

CRSSCurrentList::CRSSCurrentList()
{
	m_nCurrentListID	= 0;
	m_nCurrentReading	= 0;
	m_bUnreadOnly		= FALSE;
	m_hParent			= NULL;
	m_bViewReverse		= FALSE;
	m_sSearchWord		= _T("");
	m_bGroupLoad		= FALSE;
	m_splistpost		= SmartPtr<POST_LIST> (new POST_LIST);
	m_spviewposts		= SmartPtr<POST_LIST> (new POST_LIST);
}

CRSSCurrentList::~CRSSCurrentList()
{
	TRACE(_T("CRSSCurrentList Destroy\r\n"));
	if(m_splistpost != NULL)
		m_splistpost->clear();
	if(m_spviewposts != NULL)
		m_spviewposts->clear();
}

/**************************************************************************
 * method CRSSCurrentList::SetList
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * 기존 POSTITEM을 모두 제거하고 새로운 POST_LIST를 사용한다.
 * 
 * @Modification
 * 2005/02/08 eternalbleu 다수 아이템을 동시에 입력하는 방식으로 변경함.
 *
 * [Parameters]
 * (in/out SmartPtr<POST_LIST>&) l - 새로 사용할 POST_LIST
 **************************************************************************/
void CRSSCurrentList::SetList(SmartPtr<POST_LIST>& l)
{
	if(!m_splistdlg || m_splistpost.get == NULL) return;

	// GUI list clear 요청	
	m_splistdlg->List_Clear();

	// 기존 목록은 free & 새 목록 포인터 세팅
	m_splistpost = l;

	// 모든 아이템을 추가한다.
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		// GMT 시간을 더해 주어 Local 시간으로 만든다.
		(*it).pubdate += theApp.m_spLP->GetSPGMT();

		// 아이템을 추가한다.
		//m_splistdlg->List_ItemAdd((*it));     //commented by eternalbley
	}
    m_splistdlg->List_ItemAdd(m_splistpost);
}

/**************************************************************************
 * method CRSSCurrentList::AppendList
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 * updated 2005-12-19 :: bsearch parameter 추가 by moonknit
 *
 * @Description
 * 기존 POST_LIST에 새로운 POST_LIST를 병합한다.
 *
 * @Modification
 * 다수아이템을 동시에 입력하는 방식으로 변경함.
 *
 * @Parameters
 * (in/out SmartPtr<POST_LIST>&) l - 새로 추가할 POST_LIST
 * (in int) channelid - 추가할 포스트의 채널 아이디
 * (in BOOL) bsearch - 검색 결과인 경우 TRUE를 넣는다.
 **************************************************************************/
void CRSSCurrentList::AppendList(auto_ptr<POST_LIST>& l, int channelid, BOOL bsearch)
{
	if(l.get() == NULL) return;

	POSTITEM pi;

	// 현재 사용할 채널인 경우에 추가 한다.
	list<int>::iterator it2;
	BOOL bfind = FALSE;
	int si = m_listRemainChannel.size();
	for(it2 = m_listRemainChannel.begin() ; it2 != m_listRemainChannel.end(); ++it2)
	{
		if((*it2) == channelid)
		{
			// 검색 결과에 의한 목록 추가가 아니면 사용할 채널 목록에서 삭제한다.
			if(m_bGroupLoad) m_listRemainChannel.erase(it2);

			bfind = TRUE;
			break;
		}
	}

	if(!bfind) return;

	if(m_bFirstAppend)
	{
		m_bFirstAppend = FALSE;
		m_bHitMaxCnt = FALSE;
		m_splistpost->clear();

		m_npostcnt = 0;
		m_nunreadcnt = 0;
	}

	if(m_bHitMaxCnt) return;

	if(m_splistpost->size() == 0)
	{
		m_nCurrentReading = 0;
		m_splistdlg->List_Clear();

		m_splistdlg->Invalidate(FALSE);

		m_nCurrentListItemCnt += l->size();
	}
	else if(bsearch)
		m_nCurrentListItemCnt += l->size();

	TRACE(_T("list item cnt : %d\r\n"), m_nCurrentListItemCnt);

	DWORD dtstart = GetTickCount();
	SmartPtr<POST_LIST> sptemp = SmartPtr<POST_LIST> (new POST_LIST);

	if(CT_SEARCH == m_nchanneltype)
	{
		POST_LIST::iterator lit;
		for(lit = l->begin(); lit != l->end(); ++lit)
		{
			// 이미 존재하는 포스트는 새로 추가하지 않는다.
			if(FindPost((*lit).postid, pi)) continue;
			// GMT 시간을 더해 주어 Local 시간으로 만든다.
			(*lit).pubdate += theApp.m_spLP->GetSPGMT();

			++m_npostcnt;
			if(!(*lit).read)
				++m_nunreadcnt;

			m_splistpost->push_back((*lit));

			if(m_splistpost->size() > MAX_POSTCNT)
			{
				m_bHitMaxCnt = TRUE;
				break;
			}
			sptemp->push_back((*lit));
		}
	}
	else
	{
		POST_LIST::reverse_iterator rit;
		for(rit = l->rbegin(); rit != l->rend(); ++rit)
		{
			// 이미 존재하는 포스트는 새로 추가하지 않는다.
			if(FindPost((*rit).postid, pi)) continue;
			// GMT 시간을 더해 주어 Local 시간으로 만든다.
			(*rit).pubdate += theApp.m_spLP->GetSPGMT();

			++m_npostcnt;
			if(!(*rit).read)
				++m_nunreadcnt;

			m_splistpost->push_back((*rit));

			if(m_splistpost->size() > MAX_POSTCNT)
			{
				m_bHitMaxCnt = TRUE;
				break;
			}

			sptemp->push_back((*rit));
		}
	}

    m_splistdlg->List_ItemAdd(sptemp);

	if(m_nCurrentReading == 0 && m_bAutoSelect)
	{
		m_bAutoSelect = FALSE;
		m_splistdlg->m_listRSS.SetHotItem(0);
		m_splistdlg->m_listRSS.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	// 현재 목록의 포스트 개수가 바뀌었음을 알려주어야 함!!
	::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);

	TRACE(_T("appent time : %d\r\n"), GetTickCount() - dtstart);

//	if(m_listRemainChannel.size() == 0 || bsearch)
	{
		if(m_bHitMaxCnt)
		{
			SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
			CString msg;
			msg.Format(STR_INFORM_POSTMAXPAGEHIT, MAX_POSTCNT);
			FishMessageBox(msg);
		}
		else
		{
			if(m_bGroupLoad)
			{
				TRACE(_T("remain channel size : %d\r\n"), m_listRemainChannel.size());
				if(m_listRemainChannel.size() == 0)
					SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
			}
			else
				SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
		}

	}
}

/**************************************************************************
 * methos CRSSCurrentList::FindPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * 목록에서 postid를 이용하여 POSTITEM을 찾아내어 반환한다.
 * 읽기 처리를 해야할 경우에는 RSSDB에 읽기처리를 요청한다.
 *
 * @Parameters
 * (in int) id - 찾아야할 포스트의 id
 * (out POSTITEM&) post - 반환할 포스트 정보
 * (in BOOL) bread - 읽음 처리여부
 *
 * @Result
 * BOOL - 포스트의 발견여부
 **************************************************************************/
BOOL CRSSCurrentList::FindPost(int id, POSTITEM& post, BOOL bread)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			post = (*it);

			if(bread)
			{
				theApp.m_spRD->IPostRead(id, (*it).channelid);
			}
			return TRUE;
		}
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::GetPostXMLStream
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * postid를 이용하여 post를 읽어서 화면으로 보여주기 위한 HMTL을 생성하여 반환한다.
 *
 * @Parameters
 * (in int) id - 찾아야할 포스트의 id
 * (out CString&) xmlstream - 반환할 텍스트
 * (in BOOL) bread - 읽음 처리여부
 *
 * @Result
 * BOOL - 포스트의 발견여부
 **************************************************************************/
BOOL CRSSCurrentList::GetPostXMLStream(int id, CString& xmlstream, BOOL bread)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			// HTML Maker를 이용한다.
			if(!RequestHTMLMake((*it), xmlstream))
				return FALSE;

			if(bread && !(*it).read)
			{
				theApp.m_spRD->IPostRead(id, (*it).channelid);
			}

			m_nCurrentReading = id;

//			m_splistdlg->List_Select(m_nCurrentReading);

			m_bListEnd = FALSE;

			return TRUE;
		}
	}

	return FALSE;
}

std::list<int>::iterator FindIntItem(list<int>& src, int i)
{
	std::list<int>::iterator it;
	for(it = src.begin(); it != src.end(); ++it)
	{
		if(i == (*it)) return it;
	}

	return it;
}

/**************************************************************************
 * method CRSSCurrentList::GetPostXMLStream
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-29
 *
 * @Description
 * 복수의 postid를 이용하여 html page를 생성한다.
 *
 * @Parameters
 * (in list<int>) ids - 찾아야할 포스트의 id 목록
 * (out CString&) xmlstream - 반환할 텍스트
 *
 * @Result
 * BOOL - 포스트의 발견여부
 **************************************************************************/
BOOL CRSSCurrentList::GetPostXMLStream(list<int> ids, CString& xmlstream)
{
	if(ids.size() == 0) return FALSE;

	POST_LIST::iterator it;
	list<int>::iterator it2;
	SmartPtr<POST_LIST> aplist (new POST_LIST);

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		it2 = FindIntItem(ids, (*it).postid);
		if(it2 != ids.end())
		{
			aplist->push_back(*it);

			ids.erase(it2);
		}
	}

	m_bListEnd = TRUE;

	// HTML Maker를 이용한다.
	if(aplist->size() == 0 || !RequestHTMLMake(aplist, xmlstream, FALSE))
		return FALSE;

	return TRUE;
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * 목록상의 모든 POST를 이용하여 화면으로 보여줄 HTML을 생성한다.
 *
 * @Parameters
 * (out CString&) xmlstream - 반환할 텍스트
 **************************************************************************/
BOOL CRSSCurrentList::GetAllXMLStream(CString& xmlstream)
{
	return RequestHTMLMake(xmlstream, FALSE);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * LIST로 처리를 알려야 할 경우 사용되는 LIST DIALOG의 스마트 포인터를 설정한다.
 *
 * @Parameters
 * (in/out SmartPtr<CDlg_RSSList>&) splistdlg - 사용할 LIST DIALOG의 스마트 포인터 (reference count)
 **************************************************************************/
void CRSSCurrentList::SetListDlg(SmartPtr<CDlg_RSSList>& splistdlg)
{
	m_splistdlg = splistdlg;
}

/**************************************************************************
 * method CRSSCurrentList::ReadOnPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 * updated 2005-11-28 :: readon 처리
 * modified 2006-03-20 :: parameter bNav 추가 by moonknit
 *
 * @Description
 * 해당 id의 포스트의 link를 연다.
 *
 * @Parameters
 * (in int) id - link를 열 post
 * (in BOOL) bNav - url 을 이용한 navigation 여부
 **************************************************************************/
BOOL CRSSCurrentList::ReadOnPost(int id, BOOL bNav)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			if(!(*it).readon)
			{
				(*it).readon = TRUE;
				theApp.m_spRD->IPostReadOn(id, (*it).channelid);
			}
			
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(pFrame)
			{
				pFrame->NavigateNewWindow((*it).url);
			}

			return TRUE;
		}
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::ReadPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * 읽음/안 읽음 처리 
 *
 * @Parameters
 * (in int) id - 읽음 상태를 변경할 포스트의 id
 * (in BOOL) bread - 읽음 처리여부
 *
 * @Result
 * BOOL - 포스트의 발견여부
 **************************************************************************/
BOOL CRSSCurrentList::ReadPost(list<int>& idlist, BOOL bread)
{
	POST_LIST::iterator it;
	BOOL result = FALSE;
	BOOL single = FALSE;
	int pid, cid;

	list<int>::iterator idit;
	POST_LIST	pl;

	if(idlist.size() == 1) single = TRUE;

	for(idit = idlist.begin(); idit != idlist.end(); ++idit)
	{
		for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
		{
			if((*it).postid == (*idit))
			{
				if(single)
				{
					pid = (*it).postid;
					cid = (*it).channelid;
				}
				else
					pl.push_back( POSTITEM((*it).postid, (*it).channelid, bread, bread, bread) );

				(*it).read = bread;

				result = TRUE;
				break;
			}
		}
	}

	if(single)
		theApp.m_spRD->IPostRead(pid, cid, bread);
	else
		theApp.m_spRD->IPostUpdate(pl, FISH_ITEM_READ);

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * 포스트를 삭제함
 *
 * @Parameters
 * (in list<int>) idlist - 삭제할 포스트의 idlist
 *
 * @Result
 * BOOL - 포스트의 발견여부
 **************************************************************************/
BOOL CRSSCurrentList::DeletePost(list<int>& idlist)
{
	POST_LIST::iterator it, tit;
	BOOL result = FALSE;

	list<int>::iterator idit;

	for(idit = idlist.begin(); idit != idlist.end(); ++idit)
	{
		for(it = m_splistpost->begin(); it != m_splistpost->end();)
		{
			tit = it;
			++it;

			if((*tit).postid == (*idit))
			{
				// count post
				if(!(*tit).read)
					--m_nunreadcnt;
				--m_npostcnt;

				m_splistpost->erase(tit);
				result = TRUE;
				break;
			}
		}
	}

	// 현재 목록의 포스트 개수가 바뀌었음을 알려주어야 함!!
	::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);

	theApp.m_spRD->IPostDelete(idlist);

	return result;
}

void CRSSCurrentList::SetCurrentListID(int nID)
{
	m_nCurrentListID = nID;
}

int CRSSCurrentList::GetCurrentListID()
{
	return m_nCurrentListID;
}

/**************************************************************************
 * method CRSSCurrentList::RequestSearchChannel
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-19
 *
 * @Description
 * 검색 채널을 등록
 **************************************************************************/
BOOL CRSSCurrentList::RequestSearchChannel(int channelid)
{
	if(m_nCurrentListID == channelid) return FALSE;

	m_sSearchWord = _T("");

	m_nCurrentListID = channelid;
	m_bUnreadOnly = FALSE;			// Search Channel View에선 읽은 채널과 읽지 않은 채널 모두를 보여준다.

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_bFirstAppend = TRUE;
	m_listRemainChannel.push_back(channelid);
	m_listCurrentChannel.push_back(channelid);

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::RequestChannelOnLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * 해당 채널의 포스트를 읽어와서 CurrentList에 추가한다.
 *
 * @Parameters
 * (in int) channelid - 읽어올 채널의 id
 * (in int) channeltype - 채널 목록 보기의 style 설정
 * (in BOOL) bunreadonly - 읽지 않은 포스트만 가져오기를 원하면 TRUE, 모든 포스트를 가져오기를 원하면 FLASE
 **************************************************************************/
BOOL CRSSCurrentList::RequestChannelOnLoad(int channelid
										   , int channeltype
										   , BOOL bunreadonly
										   , BOOL bautoselect)
{
//	if( m_nCurrentListID == channelid && m_bUnreadOnly == bunreadonly) 
//        return FALSE;

    //////////////////////////////////////////////////////////////////////////
    // 리스트 채널의 모드 변경
    if ( channeltype  == CT_SEARCH || channeltype == CT_LSEARCH )
        theApp.m_spCL->SetListMode(TRUE);
    else
        theApp.m_spCL->SetListMode(FALSE);
    //////////////////////////////////////////////////////////////////////////

	m_nchanneltype = channeltype;

	m_sSearchWord = _T("");

	m_nCurrentListID	= channelid;
	m_bUnreadOnly		= bunreadonly;
	m_bAutoSelect		= bautoselect;
	m_bGroupLoad		= FALSE;

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_bFirstAppend = TRUE;
	m_listRemainChannel.push_back(channelid);
	m_listCurrentChannel.push_back(channelid);

	if(bunreadonly)
		theApp.m_spRD->IChannelGet(channelid, FISH_GET_DEFAULT | FISH_GET_UNREADONLY);
	else
		theApp.m_spRD->IChannelGet(channelid);

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::RequestGroupOnLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * 해당 그룹의 포스트를 읽어와서 CurrentList에 추가한다.
 *
 * @Parameters
 * (in int) groupid - 읽어올 그룹의 id
 * (in list<int>&) idlist - 읽어올 채널의 id 목록
 * (in BOOL) bunreadonly - 읽지 않은 포스트만 가져오기를 원하면 TRUE, 모든 포스트를 가져오기를 원하면 FLASE
 **************************************************************************/
BOOL CRSSCurrentList::RequestGroupOnLoad(int groupid, list<int>& idlist, BOOL bunreadonly, BOOL bautoselect)
{
//	if(m_nCurrentListID == groupid  && m_bUnreadOnly == bunreadonly ) 
//        return FALSE;
	m_nchanneltype = CT_NONE;

	m_sSearchWord = _T("");

	m_nCurrentListID	= groupid;
	m_bUnreadOnly		= bunreadonly;
	m_bAutoSelect		= bautoselect;
	m_bGroupLoad		= TRUE;

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_listRemainChannel.insert(m_listRemainChannel.end(), idlist.begin(), idlist.end());
	m_listCurrentChannel.insert(m_listCurrentChannel.end(), idlist.begin(), idlist.end());

	m_bFirstAppend = TRUE;

	list<int>::iterator it;
	for(it = idlist.begin(); it != idlist.end(); ++it)
	{
		if(it == idlist.begin())
			m_nFirstChannel = (*it);

		if(bunreadonly)
			theApp.m_spRD->IChannelGet((*it), FISH_GET_DEFAULT | FISH_GET_UNREADONLY);
		else
			theApp.m_spRD->IChannelGet((*it));
	}

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetCurrentPage
 *
 * written by moonknit
 *
 * @history
 * created 2006-03-03
 *
 * @Description
 * 현재가 몇번째 페이지인지 알려준다.
 *
 * @Parameters
 * (in int) npagepost = 페이지당 포스트 수
 *
 * @Return
 * (int) page 번호 -1은 페이지를 발견하지 못한 것임
 **************************************************************************/
int CRSSCurrentList::GetCurrentPage(int npagepost)
{
	if(m_bListEnd) return -1;

	int postcnt = 0;

	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == m_nCurrentReading)
		{
			int npage = (postcnt) / npagepost;
//			if(0 == (postcnt % npagepost)) --npage;
			return npage;
		}

		++postcnt;
	}

	return -1;
}

/**************************************************************************
 * method CRSSCurrentList::GetNextPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 * updated 2006-01-13 :: npos, ncnt 추가 및 그에 따른 수정 by moonknit
 *
 * @Description
 * 현재 선택된 포스트의 다음 포스트를 읽어온다.
 * ncnt > 1 인경우에는 read처리를 하지 않는다?
 *
 * @Parameters
 * (out CString&) xmlstream - 선택된 포스트로 생성된 html
 * (in int) npos - 몇번째 뒤?
 * (in int) ncnt - 몇개의 아이템
 * (in BOOL) bread - 읽음 처리 여부
 **************************************************************************/
BOOL CRSSCurrentList::GetNextPost(CString& xmlstream, int npos, int ncnt, BOOL bread)
{
	if(npos < 1 
		|| ncnt < 1
		|| ncnt > MAX_IPPCNT			// MAXIMUM ITEMS PER PAGE COUNT
		|| m_splistpost->size() == 0) return FALSE;

	// page is 0-device
	int nPage = GetCurrentPage(ncnt);
	int nLastPage = (m_splistpost->size() - 1) / ncnt;
//	if(0 == (m_splistpost->size() % ncnt)) --nLastPage;

	if(nPage == nLastPage) return FALSE;

#ifdef USE_POSTJUMP
	int nDestPage = nPage + npos;
	if( nDestPage > nLastPage ) nDestPage = nLastPage;
#else
	int nDestPage = nPage + 1;
#endif

	int nDestPost = nDestPage * ncnt;

	// 다음 페이지의 첫 포스트 발견
	POST_LIST::iterator it, tit;
	int n = 0;
	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if(n++ == nDestPost) break;
	}

	if(ncnt == 1)
	{
		if(!RequestHTMLMake((*it), xmlstream))
			return FALSE;

		if(bread && !(*it).read)
		{
			theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
		}

		m_nCurrentReading = (*it).postid;
	}
	else
	{
		int tmpcnt = ncnt;

		tit = it;
		SmartPtr<POST_LIST> aptmplist(new POST_LIST);
		for(; it != m_splistpost->end(); ++it)
		{
			if(tmpcnt-- == 0) break;

			aptmplist->push_back((*it));
			m_nCurrentReading = (*it).postid;
		}

		if(!RequestHTMLMake(aptmplist, xmlstream, FALSE))
			return FALSE;

		if(bread)
		{
			it = tit;
			tmpcnt = ncnt;
			for(; it != m_splistpost->end(); ++it)
			{
				if(tmpcnt-- == 0) break;
				if(!(*it).read)
				{
					theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
				}
			}
		}
	}

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = FALSE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetPrevPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 * updated 2006-01-13 :: npos, ncnt 추가 및 그에 따른 수정 by moonknit
 *
 * @Description
 * 현재 선택된 포스트의 이전 포스트를 읽어온다.
 *
 * @Parameters
 * (out CString&) xmlstream - 선택된 포스트로 생성된 html
 * (in int) npos - 몇번째 뒤?
 * (in int) ncnt - 몇개의 아이템
 * (in BOOL) bread - 읽음 처리 여부
 **************************************************************************/
BOOL CRSSCurrentList::GetPrevPost(CString& xmlstream, int npos, int ncnt, BOOL bread)
{
	if(npos < 1 
		|| ncnt < 1
		|| ncnt > MAX_IPPCNT			// MAXIMUM ITEMS PER PAGE COUNT
		|| m_splistpost->size() == 0) return FALSE;

	// page is 0-device
	int nPage = GetCurrentPage(ncnt);
	int nLastPage = ((m_splistpost->size() - 1) / ncnt);

	// 첫 페이지에선 더 이상 뒤로 갈 수 없다.
	if(nPage == 0) return FALSE;

#ifdef USE_POSTJUMP
	int nDestPage = nPage + npos;
	if( nDestPage > nLastPage ) nDestPage = nLastPage;
#else
	int nDestPage;
	if(nPage == -1) nDestPage = nLastPage;
	else nDestPage = nPage - 1;
#endif

	int nDestPost = nDestPage * ncnt;

	// 다음 페이지의 첫 포스트 발견
	POST_LIST::iterator it, tit;
	int n = 0;
	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if(n++ == nDestPost) break;
	}

	if(ncnt == 1)
	{
		if(!RequestHTMLMake((*it), xmlstream))
			return FALSE;

		if(bread && !(*it).read)
		{
			theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
		}

		m_nCurrentReading = (*it).postid;
	}
	else
	{
		int tmpcnt = ncnt;
		BOOL bCR = FALSE;

		tit = it;
		SmartPtr<POST_LIST> aptmplist(new POST_LIST);
		for(; it != m_splistpost->end(); ++it)
		{
			if(tmpcnt-- == 0) break;

			aptmplist->push_back((*it));
			if(!bCR)
			{
				m_nCurrentReading = (*it).postid;
				bCR = TRUE;
			}
		}

		if(!RequestHTMLMake(aptmplist, xmlstream, FALSE))
			return FALSE;

		if(bread)
		{
			it = tit;
			tmpcnt = ncnt;
			for(; it != m_splistpost->end(); ++it)
			{
				if(tmpcnt-- == 0) break;
				if(!(*it).read)
				{
					theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
				}
			}
		}
	}

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = FALSE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetFirstPost
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-13
 *
 * @Description
 * 현재 목록중 최초의 포스트를 읽어온다.
 *
 * @Parameters
 * (in int) chid - 검색할 채널
 **************************************************************************/
BOOL CRSSCurrentList::GetFirstPost(CString& xmlstream, BOOL bread)
{
	if(m_splistpost->size() == 0) return FALSE;

	POST_LIST::iterator it = m_splistpost->begin();

	if(!RequestHTMLMake((*it), xmlstream))
		return FALSE;

	if(bread && !(*it).read)
	{
		theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
	}

	m_nCurrentReading = (*it).postid;

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = TRUE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetLastPost
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-13
 *
 * @Description
 * 현재 목록중 최후의 포스트를 읽어온다.
 *
 * @Parameters
 * (in int) chid - 검색할 채널
 **************************************************************************/
BOOL CRSSCurrentList::GetLastPost(CString& xmlstream, BOOL bread)
{
	if(m_splistpost->size() == 0) return FALSE;

	POST_LIST::reverse_iterator it = m_splistpost->rbegin();

	if(!RequestHTMLMake((*it), xmlstream))
		return FALSE;

	if(bread && !(*it).read)
	{
		theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
	}

	m_nCurrentReading = (*it).postid;

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = TRUE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::IsCurrentChannel
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * 현재 등록된 채널 목록 중 주어진 채널 id를 가진 채널이 있으면 갱신한다.
 *
 * @Parameters
 * (in int) chid - 검색할 채널
 **************************************************************************/
BOOL CRSSCurrentList::IsCurrentChannel(int chid)
{
	list<int>::iterator it;

	for(it = m_listCurrentChannel.begin(); it != m_listCurrentChannel.end(); ++it)
	{
		if((*it) == chid) return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::UpdatePostList
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * 현재 목록과 비교하여 갱신된 포스트의 내용을 변경해준다.
 *
 * @Parameters
 * (in POSTITEM&) item - 갱신된 포스트의 목록
 **************************************************************************/
BOOL CRSSCurrentList::UpdatePost(POSTITEM& item, int updateflag, BOOL bredraw)
{
	ASSERT(m_splistdlg.get());

	POST_LIST::iterator it;
	BOOL updated = FALSE;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == item.postid)
		{
			// 읽음 여부 변경시
			if(updateflag & FISH_ITEM_READ)
			{
				if((*it).read != item.read)
				{
					if(item.read)	// 안읽은 것 -> 읽은 것
						--m_nunreadcnt;
					else
						++m_nunreadcnt;
				}
			}

			if(updateflag == FISH_ITEM_ALL)
			{
				(*it) = item;
				(*it).pubdate += theApp.m_spLP->GetSPGMT();
				updated = TRUE;
			}
			else
			{
				if(updateflag & FISH_ITEM_READ)
				{
					(*it).read = item.read;
//					(*it).svread = item.svread;
				}

				if(updateflag & FISH_ITEM_READON)
				{
					(*it).readon = item.readon;
//					(*it).svreadon = item.svreadon;
				}

				if(updateflag & FISH_ITEM_SEARCHVALUE)
				{
					(*it).svread = item.svread;
					(*it).svreadon = item.svreadon;
					(*it).svscrap = item.svscrap;
					(*it).svfilter = item.svfilter;
					(*it).svhit = item.svhit;
					TRACE(_T("Search Value Changed : r[%d], ro[%d], s[%d], f[%d], h[%d]\r\n")
						, item.svread
						, item.svreadon
						, item.svscrap
						, item.svfilter
						, item.svhit
						);
				}
			}

			m_splistdlg->List_ItemUpdate((*it));
			break;
		}
	}

	return updated;
}

/**************************************************************************
 * method CRSSCurrentList::UpdatePostList
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * 현재 목록과 비교하여 갱신된 포스트의 내용을 변경해준다.
 *
 * @Parameters
 * (in/out SmartPtr<POST_LIST>&) aplist - 갱신할 포스트 목록
 **************************************************************************/
BOOL CRSSCurrentList::UpdatePostList(auto_ptr<POST_LIST>& aplist, int updateflag, BOOL bredraw)
{
	ASSERT(aplist.get());
	BOOL updated = FALSE;

	int oldunreadcnt = m_nunreadcnt;

	POST_LIST::iterator it;
	for(it = aplist->begin(); it != aplist->end(); ++it)
	{
		if(UpdatePost((*it), updateflag))
			updated = TRUE;
		
	}

	if(oldunreadcnt != m_nunreadcnt)
	{
		::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);
	}

	if(updated && bredraw && m_nCurrentReading == 0)
	{
		SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
	}

	return updated;
}

/************************************************************************
SetListMode 리스트의 표현모드를 정의한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/16:CREATED
************************************************************************/
VOID CRSSCurrentList::SetListMode(BOOL bShowSrchStyle)
{
    m_splistdlg->List_Mode(bShowSrchStyle);
    return;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 *
 * @Description
 * Html 만들기를 요청함과 동시에 임시 목록에 저장해 둔다.
 *
 * @Parameters
 * (in POSTITEM&) post - Html을 만들때 사용할 포스트 구조체
 * (out CString&) xmlstream - Html로 생성된 결과물
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(POSTITEM& post, CString& xmlstream)
{
	m_spviewposts->clear();
	BOOL bresult;
	m_spviewposts->push_back(post);
	m_bSinglePost = TRUE;
	bresult = CHtmlMaker::MakeHtml_Xslt(post, xmlstream, theApp.m_spLP->GetStyleFile());
	m_bViewReverse = FALSE;
	m_bUseChannelPage = FALSE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 * updated 2006-03-21 :: MakeHtml_Xslt 함수 변경으로 인한 수정 by aquarelle
 *
 * @Description
 * Html 만들기를 요청함과 동시에 임시 목록에 저장해 둔다.
 *
 * @Parameters
 * (in SmartPtr<POST_LIST>&) aplist - Html을 만들때 사용할 포스트 목록
 * (out CString&) xmlstream - Html로 생성된 결과물
 * (in BOOL) breserse - Html을 생성할 때 목록의 역으로 html을 생성할 때 필요한 변수
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(SmartPtr<POST_LIST> &aplist, CString& xmlstream, BOOL breverse)
{
	m_spviewposts->clear();
	BOOL bresult;
	m_bSinglePost = FALSE;
	m_spviewposts->insert(m_spviewposts->end(), aplist->begin(), aplist->end());

	DWORD flag = HM_LOADLOCAL | (!breverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);
	
	bresult = CHtmlMaker::MakeHtml_Xslt(aplist, xmlstream, theApp.m_spLP->GetStyleFile(), flag);

	m_bViewReverse = breverse;
	m_bUseChannelPage = FALSE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-03-20
 *
 * @Description
 * m_splistpost를이용해 Html 만든다.
 *
 * @Parameters
 * (out CString&) xmlstream - Html로 생성된 결과물
 * (in BOOL) breserse - Html을 생성할 때 목록의 역으로 html을 생성할 때 필요한 변수
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(CString& xmlstream, BOOL breverse)
{
	BOOL bresult;
	m_spviewposts->clear();
	m_bSinglePost = FALSE;

	DWORD flag = HM_LOADLOCAL | (!breverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);
	bresult = CHtmlMaker::MakeHtml_Xslt(m_splistpost, xmlstream, theApp.m_spLP->GetStyleFile(), flag);
	m_bViewReverse = breverse;
	m_bUseChannelPage = TRUE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::Refresh
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 * updated 2006-03-21 :: MakeHtml_Xslt 함수 변경으로 인한 수정 by aquarelle
 *
 * @Description
 * 현재 보고 있는 임시 Post 목록을 이용해 Html을 다시 생성한다.
 *
 * @Parameters
 * (out CString&) xmlstream - Html로 생성된 결과물
 **************************************************************************/
BOOL CRSSCurrentList::Refresh(CString& xmlstream)
{
	BOOL bresult;

	DWORD flag = HM_LOADLOCAL | (!m_bViewReverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);

	if(m_spviewposts->size() > 0)
	{
		if(m_bSinglePost)
			bresult = CHtmlMaker::MakeHtml_Xslt(m_spviewposts->front(), xmlstream, theApp.m_spLP->GetStyleFile(), flag);
		else
			bresult = CHtmlMaker::MakeHtml_Xslt(m_spviewposts, xmlstream, theApp.m_spLP->GetStyleFile(), flag);
	}
	else
		bresult = CHtmlMaker::MakeHtml_Xslt(m_splistpost, xmlstream, theApp.m_spLP->GetStyleFile(), flag);

	return bresult;
}





BOOL CRSSCurrentList::SavePostCurrent(CString filename)
{

	if(!m_spviewposts->empty())
	{
		//CString buffer;
		/*
		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_POSTSAVE_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);
		*/

		CHtmlMaker htmlmaker;
		//if(!htmlmaker.MakeHtml(m_spviewposts, buffer, HM_LOADSTRING | HM_PAPERSTYLE))
		if(!htmlmaker.MakeHtml(m_spviewposts, theApp.m_spLP->GetPaperStyle(), HM_LOADLOCAL | HM_PAPERSTYLE))
			return FALSE;
		if(!htmlmaker.Save(filename))
			return FALSE;

		return TRUE;
	}
	else
		return FALSE;	
}

BOOL CRSSCurrentList::SavePost(list<int> ids, CString filename)
{
	if(ids.size() == 0) return FALSE;

	POST_LIST::iterator it;
	list<int>::iterator it2;
	SmartPtr<POST_LIST> aplist (new POST_LIST);

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		it2 = FindIntItem(ids, (*it).postid);
		if(it2 != ids.end())
		{
			aplist->push_back(*it);

			ids.erase(it2);
		}
	}

	if(!aplist->empty())
	{
		/*
		CString buffer;
		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_POSTSAVE_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		*/

		CHtmlMaker htmlmaker;
		
		DWORD flag = HM_LOADLOCAL | HM_PAPERSTYLE;
		if(!htmlmaker.MakeHtml(aplist, theApp.m_spLP->GetPaperStyle(), flag))
			return FALSE;
		if(!htmlmaker.Save(filename))
			return FALSE;

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CRSSCurrentList::RequestChannelDelete(int channelid)
{
	if(IsCurrentChannel(channelid))
	{
		m_listRemainChannel.clear();
		m_listCurrentChannel.clear();

		m_splistpost->clear();
		m_spviewposts->clear();

		m_nCurrentReading = 0;
		m_splistdlg->List_Clear();

		m_splistdlg->Invalidate(FALSE);

		::SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);

		return TRUE;
	}

	return FALSE;
}