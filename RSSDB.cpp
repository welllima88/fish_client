// RSSDB.cpp: implementation of the CRSSDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FISH.h"
#include "RSSDB.h"
#include "fish_common.h"
#include "LocalProperties.h"

//#include "XML/EzXMLParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define USE_LOCAL_READCNT

extern CFISHApp theApp;

//////////////////////////////////////////////////////////////////////
// static or global variable
//////////////////////////////////////////////////////////////////////
HANDLE CRSSDB::m_hThread = NULL;
BOOL CRSSDB::m_bThread = FALSE;
DWORD CRSSDB::m_dwThreadId = 0;
int CRSSDB::m_nInit = 0;
int CRSSDB::m_nSearchSeq = 0;

BOOL CRSSDB::m_bLastFlush = FALSE;

HANDLE CRSSDB::m_hSThread = NULL;
DWORD CRSSDB::m_dwSThreadId = 0;
BOOL CRSSDB::m_bSThread = FALSE;

HWND CRSSDB::m_hInformWnd = NULL;
HWND CRSSDB::m_hSubscribeWnd = NULL;

// XML Data list
XML_MAP	CRSSDB::m_mapData;
CRITICAL_SECTION m_csMap;

MAP_POSTMATCH m_mapPostMatch;

// search queue and critical section
SEARCH_LIST CRSSDB::m_queueSearch[MAX_SEARCHLISTCNT];
CRITICAL_SECTION m_csSearch[MAX_SEARCHLISTCNT];

// simple data transaction must be cleared as fast as possible
// so seperate work queue into two pieces
// in-queue - as array number is smaller, the priority is higher
WORK_LIST	CRSSDB::m_queueWork[MAX_WORKLISTCNT];
CRITICAL_SECTION m_csInQueue[MAX_WORKLISTCNT]; 

// out-queue
RESULT_LIST	CRSSDB::m_queueResult[MAX_WORKLISTCNT];
CRITICAL_SECTION m_csOutQueue[MAX_WORKLISTCNT]; 

// sequence
int CRSSDB::m_nseq = 0;					// work id sequence
int CRSSDB::m_npostseq = 0;				// post id sequence
int CRSSDB::m_PrevlastSeq = 0;

BOOL CRSSDB::m_bWorking = FALSE;
BOOL CRSSDB::m_bPostInit = FALSE;
BOOL CRSSDB::m_bFlush = FALSE;

CString m_err = _T("");

// keyword
KEYWORD_LIST CRSSDB::m_listKeyword;
CRITICAL_SECTION m_csKeywordList;

#define WTM_CLOSE							WM_APP + 1
#define WTM_NEXT							WM_APP + 2

#define	MAX_LOADXML							5				// 한번에 10개의 파일만 읽어온다.
#define MAX_FLUSHLIMIT						2				// 한번 flush에 채널 2개만 처리한다.

//////////////////////////////////////////////////////////////////////
// global fuctions pre-definition
//////////////////////////////////////////////////////////////////////


BOOL CheckItemByPostID(CEzXMLParser* pxml, int postid, IXMLDOMNodePtr& nodeptr);
BOOL CheckItemByGUID(CEzXMLParser* pxml, CString guid, IXMLDOMNodePtr& nodeptr);
int GetPostCnt(CEzXMLParser* pxml);
BOOL FindXML(XML_LIST& l, int postid, XML_LIST::iterator& it, IXMLDOMNodePtr& nodeptr);
XML_MAP::iterator MakeNewChannel(XML_MAP& m, int channelid, int chtype = CT_SUBSCRIBE);
CEzXMLParser* MakeDefaultChannelXML(TERM t, int channelid, int seq, int chtype, CString xmlurl);

BOOL AddItemtoXML(CEzXMLParser* pxml, POSTITEM& item, int ctype = CT_SUBSCRIBE);

BOOL CheckKeyword(POSTITEM& item, CString keyword);

BOOL GetXMLItem(POSTITEM& item, int chtype, int read, CEzXMLParser* pxml, DWORD flag);
BOOL GetXMLItem(POSTITEM& item, int chtype, int read, RSS_XMLDATA& xmldata, BOOL& bopen, DWORD flag);
BOOL GetXMLItemList(auto_ptr<POST_LIST>& aplistpost, int chid, int chtype, CEzXMLParser* pxml
					, BOOL flag, CString scond = _T(""));
BOOL GetXMLItemList(auto_ptr<POST_LIST>& aplistpost, int chid, int chtype, RSS_XMLDATA& xmldata
					, BOOL& bopen, BOOL flag, CString scond = _T(""));

BOOL GetXMLItemString(int postid, CEzXMLParser* pxml, CString& xml);
BOOL GetXMLItemString(int postid, RSS_XMLDATA& xmldata, BOOL& bopen, CString& xml);

BOOL UpdateXMLChannelData(RSS_XMLDATA& data, int flag = FISH_CHANNEL_ALL);

BOOL UpdateXMLItemListRead(CEzXMLParser* pxml, BOOL read);
BOOL UpdateXMLItemListRead(RSS_XMLDATA& xmldata, BOOL read);

BOOL SearchXML_Net(CEzXMLParser* pxml, WorkSearch *psearch, int ctype);
BOOL SearchXML_Local(CEzXMLParser* pxml, WorkSearch *psearch, int ctype);
BOOL SearchXML_Graph(CEzXMLParser* pxml, WorkSearch *psearch, int ctype);

BOOL SearchXML(RSS_XMLDATA* pxmldata, WorkSearch* psearch, BOOL *open);

void RemakeListterm(XML_LIST& l);

void FLUSH(RSS_XMLDATA& data, BOOL force = FALSE);

int GetSearchPriority(SEARCH_TYPE type);

//////////////////////////////////////////////////////////////////////
// global fuction
//////////////////////////////////////////////////////////////////////


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * RSS_XMLDATA의 XML을 조건에 맞추어 파일로 기록한다.
 * flush 조건은 다음과 같다.
 * 1. force 플래그가 TRUE인 경우
 * 2. data.flush 플래그가 TRUE이고 flush 시간조건을 만족한 경우
 * @Parameters
 * (in/out RSS_XMLDATA&) data - FLUSH를 수행할 대상 객체
 * (in COleDateTime) t - 시간 조건을 판단할 시간
 * (in COleDateTimeSpan) s - 시간 조건을 판한할 시간
 * (in BOOL) force - 강제 flush 플래그, TRUE면 강제 flush FALSE면 시간 조건을 판단한다.
 **************************************************************************/
void FLUSH(RSS_XMLDATA& data, COleDateTime t, COleDateTimeSpan s, BOOL force)
{
//	TRACE(_T("data : post[%d] \r\n"), data.postcnt);
	if( (force || (data.flush && data.flushtime + s > t) )
		&& data.XML != NULL
		)
	{
		data.XML->SaveXML();
		data.flush = FALSE;
		data.flushtime = t;
	}
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * RSS_XMLDATA에서 XML정보를 소멸시킨다.
 *
 * @Parameters
 * (in RSS_XMLDATA&) data - XML정보를 소멸시킬 대상 객체
 *
 * @Returns
 * (BOOL) XML정보 소멸 성공여부
 **************************************************************************/
BOOL DELETE_XMLDATA(RSS_XMLDATA& data)
{
	if(data.XML == NULL) return FALSE;

	IXMLDOMNodeListPtr	nodelist = NULL;
	IXMLDOMNodePtr		node = NULL;

	// check fish version
	nodelist = data.XML->SearchNodes(XML_XPATH_CHANNEL);

	if(nodelist == NULL || nodelist->length == 0)
		return FALSE;

	node = nodelist->item[0];

	SetNamedNodeText(XML_LOAD, data.XML.get(), _T("0"), 0, node);
	return TRUE;
}



/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML_LIST의 각 객체의 기간중 to 정보를 수정한다.
 *
 * @Parameters
 * (in/out XML_LIST) l - 기간 정보를 수정할 XML_LIST
 **************************************************************************/
void RemakeListterm(XML_LIST& l)
{
	XML_LIST::iterator	it;
	COleDateTime		tmptime;
	for(it = l.begin(); it != l.end(); ++it)
	{
		if(it == l.begin())
		{
//			(*it).term.to.m_dt = 0;
		}
		else
		{
			(*it).term.to = tmptime;
		}

		tmptime = (*it).term.from;
	}
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML 정보에서 최종 Post ID값을 찾아낸다.
 *
 * @Parameters
 * (in CEzXMLParser*) parser - 검색할 대상 XML
 * (in int) channelid - 등록할 Post의 channel id
 * (in int&) unread - 읽지 않는 post의 개수를 반환한다.
 * (in int&) postcnt - 전체 포스트의 개수를 반환한다.
 * (in/out RSS_CHANNELDATA&) cdata - GUID 중복 검사를 위한 정보
 *
 * @Returns
 * (int) 최종 Post ID, 찾지 못하면 0을 반환한다.
 **************************************************************************/
int CRSSDB::AddPostMatch(CEzXMLParser* parser, int channelid, int& unread, int& postcnt, RSS_CHANNELDATA& cdata)
{
	IXMLDOMNodeListPtr	nodelist = NULL;
	IXMLDOMNodePtr		node = NULL;
	int lastid = 0, id;
	CString sid;
	CString sguid;

	postcnt = unread = 0;

	if(!parser) return 0;

	nodelist = parser->SearchNodes(XML_XPATH_ITEM_ALL);

	if(nodelist == NULL || nodelist->length == 0)
		return 0;

	MAP_POSTMATCH::iterator it;
	MAP_GUIDMATCH::iterator git;
	BOOL bChanged = FALSE;

	BOOL binsert;

	for(int i = nodelist->length-1; i>=0; --i)
	{
		node = nodelist->item[i];
		sguid = GetNamedNodeText(XML_GUID, parser, node);
		binsert = FALSE;

		// check guid duplication
		git = cdata.mapGuid.find(sguid);
		if(git != cdata.mapGuid.end())		// 이미 존재하는 포스트
		{
			// 삭제처리
			parser->RemoveNodes(node);
			continue;
		}
				
		sid = GetNamedNodeText(XML_ID, parser, node);
		id = _ttoi((LPTSTR) (LPCTSTR) sid);

		it = m_mapPostMatch.find(id);

		if(it == m_mapPostMatch.end())
		{
			++postcnt;
			if(GetNamedNodeText(XML_READ, parser, node) != "1") ++unread;

			binsert = TRUE;
		}
		// 이미 해당하는 포스트의 id가 들어있다면 지난 번 최종 sequence에서 새로 포스트 id를 만들어낸다.
		else
		{
			if(CRSSDB::m_PrevlastSeq > lastid)
				lastid = CRSSDB::m_PrevlastSeq;
			
			{
				TCHAR tmp[16];
				++postcnt;
				if(GetNamedNodeText(XML_READ, parser, node) != "1") ++unread;

				id = (++lastid);

				SetNamedNodeText(XML_ID, parser, _itot(id, tmp, 10), 0, node);

				bChanged = TRUE;

				binsert = TRUE;
			}
		}

		if(binsert)
		{
			m_mapPostMatch.insert(MAP_POSTMATCH::value_type(id, channelid));
			cdata.mapGuid.insert(MAP_GUIDMATCH::value_type(sguid, id));
		}

		if(id > lastid)
			lastid = id;
	}

	if(bChanged)
		parser->SaveXML();

	return lastid;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2006-01-24 :: remove NODE & TAG
 *
 * @Description
 * post를 삭제한다.
 *
 * @Parameters
 * (in int) postid - 삭제할 대상 post id
 * (in/out CEzXMLParser*) pxml - 삭제에 사용할 XML 객체 포인터
 * (in DWORD) flag - 삭제 대상 설정
 * FISH_ITEM_ALL : 아이템 자체 삭제
 * FISH_ITEM_TAG : 아이템의 태그 삭제
 * FISH_ITEM_NODE : 아이템의 노드 삭제
 * @Returns
 * (BOOL) 삭제 성공여부
 **************************************************************************/
 BOOL CRSSDB::RemoveXMLItem(int postid, CEzXMLParser* pxml, BOOL& decunread, RSS_CHANNELDATA& cdata)
{
	if(!pxml) return FALSE;

	CString strxpath;
	strxpath.Format(XML_XPATH_ITEMSEARCH_ID, postid);
	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
		return FALSE;

	int i;
	CString sguid;
	for(i = listptr->length - 1; i >= 0; --i)
	{
		if(GetNamedNodeText(XML_READ, pxml, listptr->item[i]) != "1") decunread = TRUE;;

		sguid = GetNamedNodeText(XML_GUID, pxml, listptr->item[i]);

		pxml->RemoveNodes(listptr->item[i]);
	}

	m_mapPostMatch.erase(postid);
	cdata.mapGuid.erase(sguid);

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML파일명을 이용하여 XML ITEM 삭제
 *
 * @Parameters
 * (in int) postid - 삭제 대상 post id
 * (in/out RSS_XMLDATA&) xmldata - 삭제할 xml 대상 정보를 담고 있는 구조체
 * (in/out BOOL&) bopen - 파일이 열림 정보를 반환하기 위한 변수
 * (in/out BOOL&) decunread - 읽지않은 포스트를 삭제하였는지 정보를 반환하기 위한 변수
 * (in DWORD) flag - 삭제시 사용하는 flag
 *
 * @Returns
 * (BOOL) 삭제 성공 여부
 **************************************************************************/
BOOL CRSSDB::RemoveXMLItem(int postid, RSS_XMLDATA& xmldata, BOOL& bopen, BOOL& decunread, RSS_CHANNELDATA& cdata)
{
	bopen = FALSE;
	if(xmldata.XML != NULL)
		return RemoveXMLItem(postid, xmldata.XML.get(), decunread, cdata);

	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	xmldata.XML = apxml;
	bopen = TRUE;
	return RemoveXMLItem(postid, pxml, decunread, cdata);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * XML내부의 모든 Item을 삭제한다.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - Item을 삭제할 XML
 *
 * @Returns
 * (BOOL) 삭제 결과
 **************************************************************************/
BOOL CRSSDB::RemoveXMLAllItem(CEzXMLParser* pxml, RSS_CHANNELDATA& cdata)
{
	if(!pxml) return FALSE;

	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pxml->SearchNodes(XML_XPATH_ITEM_ALL);

	if(listptr == NULL || listptr->length == 0)
		return FALSE;

	int i, pid;
	CString sguid;
	for(i = listptr->length - 1; i >= 0; --i)
	{
		pid = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_ID, pxml, listptr->item[i]));
		sguid = GetNamedNodeText(XML_GUID, pxml, listptr->item[i]);

		if(pid != 0) m_mapPostMatch.erase(pid);
		cdata.mapGuid.erase(sguid);

		pxml->RemoveNodes(listptr->item[i]);
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * 해당 채널의 XML을 만든다.
 *
 * @Parameters
 * (in TERM) t - 생성 채널의 기간
 * (in int) channelid - 생성 채널의 id
 * (in int) seq - 생성 xml 파일의 중복을 위해 주어진 sequence
 * (in int) chtype - 채널의 종류
 * (in CString) xmlurl - 채널을 정확하게 확인하기 위한 id로 사용된다
 *
 * @Returns
 * (CEzXMLParser*) 생성된 XML 구조체
 **************************************************************************/
CEzXMLParser* MakeDefaultChannelXML(TERM t, int channelid, int seq, int chtype, CString xmlurl)
{
	CEzXMLParser* pxml = NULL;
	BOOL bsuccess = FALSE;
	IXMLDOMElementPtr elptr = NULL;

	pxml = new CEzXMLParser;

	CString str = _T("");
	str.Empty();
	str = XML_DECLARATION;
	str += XML_ROOT_DECLARATION;

	TCHAR	tmp[16];

	for(int i = 0; i < 1; ++i)
	{
		if(!pxml->LoadXML((LPTSTR)(LPCTSTR)str, CEzXMLParser::LOAD_STRING)) break;

		elptr = pxml->CreateElement(XML_CHANNEL);

		if(elptr == NULL) break;

		// VERSION 정보와 term 정보 그리고 CHANNEL 정보를 넣는다.
		if(!pxml->AddAttribute(elptr, XML_VERSION, FISHPOST_VERSION)) break;
		if(!pxml->AddAttribute(elptr, XML_LOAD, _T("1"))) break;
		if(!pxml->AddAttribute(elptr, XML_FROM, (LPTSTR) (LPCTSTR) ConvertTimetoString (t.from))) break;
		if(!pxml->AddAttribute(elptr, XML_TO, (LPTSTR) (LPCTSTR) ConvertTimetoString(t.to))) break;

		_stprintf(tmp, _T("%d"), channelid);
		if(!pxml->AddAttribute(elptr, XML_CHANNELID, tmp)) break;
		_stprintf(tmp, _T("%d"), chtype);
		if(!pxml->AddAttribute(elptr, XML_CHANNELTYPE, tmp)) break;

//		if(chtype == CT_SUBSCRIBE)
//			if(!pxml->AddAttribute(elptr, XML_CHANNELURL, (LPTSTR) (LPCTSTR) xmlurl)) break;

		if(!pxml->AddAttribute(elptr, XML_GUID, (LPTSTR) (LPCTSTR) xmlurl)) break;

		bsuccess = TRUE;
		break;
	}

	if(!bsuccess)
	{
		delete pxml;
		ASSERT(FALSE);
		return NULL;
	}


	pxml->AppendChild(elptr, NULL);

	TCHAR path[1024*2 +1];
	time_t cur = time(NULL);
//	_stprintf(path, _T("c:\\%03d_%d.xml"), channelid, seq);
	_stprintf(path, _T("%s%03d_%d.xml"), theApp.m_spLP->GetPostDataPath(), channelid, seq);

	pxml->SetSavePath(path);

	return pxml;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * XML에 주어진 post id를 가진 아이템이 존재하는지 여부를 반환한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - 검색 대상 XML
 * (in int) postid - 검색할 post id
 * (out IXMLDOMNodePtr&) nodeptr - 검색된 item의 node를 담는다.
 *
 * @Returns
 * (BOOL) 주어진 post id를 가진 item의 발견 여부
 **************************************************************************/
BOOL CheckItemByPostID(CEzXMLParser* pxml, int postid, IXMLDOMNodePtr& nodeptr)
{
	if(!pxml) return FALSE;

	CString strxpath = _T("");
	strxpath.Format(XML_XPATH_ITEMSEARCH_ID, postid);

	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	nodeptr = listptr->item[0];

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML에 주어진 GUID를 가진 아이템이 존재하는지 여부를 반환한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - 검색 대상 XML
 * (in CString) guid - 검색할 GUID
 * (out IXMLDOMNodePtr&) nodeptr - 검색된 item의 node를 담는다.
 *
 * @Returns
 * (BOOL) 주어진 post id를 가진 item의 발견 여부
 **************************************************************************/
BOOL CheckItemByGUID(CEzXMLParser* pxml, CString guid, IXMLDOMNodePtr& nodeptr)
{
	if(!pxml) return FALSE;

	CString strxpath = _T("");
	strxpath.Format(XML_XPATH_ITEMSEARCH_GUID, guid);

	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	nodeptr = listptr->item[0];

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * XML에 존재하는 post의 개수를 반환한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - 검사에 사용할 XML
 *
 * @Returns
 * (int) post의 개수
 **************************************************************************/
int GetPostCnt(CEzXMLParser* pxml)
{
	if(!pxml) return 0;

	IXMLDOMNodeListPtr plistptr = NULL;
	plistptr = pxml->SearchNodes(XML_XPATH_ITEM_ALL);

	if(plistptr == NULL)
		return 0;

	return plistptr->length;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2006-03-10 :: 시간 정보를 이용한 XML발견 방식에서 post id 를 이용한 방식으로 변경
 *						by moonknit
 *
 * @Description
 * XML_LIST에서 주어진 시간에 해당되는 XML을 찾아준다.
 *
 * @Parameters
 * (in/out XML_LIST&) l - 검사할 list
 * (in int) postid - 검사할 포스트 아이디
 * (out XML_LIST::iterator&) it - 찾아낸 XML의 iterator
 * (oout IXMLDOMNodePtr&) nodeptr - 발견된 포스트의 XML Node
 *
 * @Returns
 * (BOOL) 발견 여부
 **************************************************************************/
BOOL FindXML(XML_LIST& l, int postid, XML_LIST::iterator& it, IXMLDOMNodePtr& nodeptr)
{
	XML_LIST::iterator i;
	BOOL result = FALSE;

	for(i = l.begin(); i != l.end(); ++i)
	{
		if(!(*it).XML) continue;

		if(CheckItemByPostID((*it).XML.get(), postid, nodeptr))
		{
			result = TRUE;
			break;
		}
	}

	it = i;				// means list.end

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 :: 검색 결과 가치 정보 추가 및 이에 따른 Parameter 추가 (int ctype) by moonknit
 * updated 2005-12-13 :: node와 tag 정보 삭제
 *						node는 memory 정보에서만 처리를 하고 tag는 별도 구조체로 관리한다.
 *						by moonknit
 * updated 2006-01-24 :: CHECK_GUIDBYREGION 조건 절 추가 by moonknit
 * modified 2006-03-10 :: CHECK_GUIDBYREGION 조건 절 삭제 by moonknit
 * updated 2006-03-15 :: encoded 정보 추가... by aquarelle
 *
 * @Description
 * Post를 XML에 추가한다.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - 사용할 XML
 * (in POSTITEM&) item - 추가할 post
 *
 * @Returns
 * (BOOL) 추가 성공 여부
 **************************************************************************/
BOOL AddItemtoXML(CEzXMLParser* pxml, POSTITEM& item, int ctype)
{
	// item 을 생성하여
	// 정보를 기록함
	IXMLDOMNodePtr nodeptr;

	IXMLDOMElementPtr elptr = NULL;
	elptr = pxml->CreateElement(XML_ITEM);

	if(elptr == NULL) return FALSE;

	// 이하 attributes of item
	TCHAR tmp[16];

	_stprintf(tmp, _T("%d"), item.postid);

	if(!pxml->AddAttribute(elptr, XML_ID, tmp))
	{
		return FALSE;
	}

	_stprintf(tmp, _T("%d"), item.read);
	if(!pxml->AddAttribute(elptr, XML_READ, tmp))
	{
		return FALSE;
	}

	_stprintf(tmp, _T("%d"), item.readon);
	if(!pxml->AddAttribute(elptr, XML_READON, tmp))
	{
		return FALSE;
	}

#ifdef USE_XMLBNS
	if(item.bns) pxml->AddAttribute(elptr, XML_BNS, _T("1"));
#endif

//	_stprintf(tmp, _T("%d"), item.xmlupdate);
//	if(!pxml->AddAttribute(elptr, XML_XMLUPDATE, tmp))
//	{
//		return FALSE;
//	}

	if(!pxml->AddAttribute(elptr, XML_PUBDATE, (LPTSTR) (LPCTSTR) ConvertTimetoString(item.pubdate)))
	{
		return FALSE;
	}

	if(!pxml->AddAttribute(elptr, XML_SUBJECT, (LPTSTR) (LPCTSTR) item.subject))
	{
		return FALSE;
	}

	if(!pxml->AddAttribute(elptr, XML_GUID, (LPTSTR) (LPCTSTR) item.guid))
	{
		return FALSE;
	}

	if(!pxml->AddAttribute(elptr, XML_URL, (LPTSTR) (LPCTSTR) item.url))
	{
		return FALSE;
	}

	if(!pxml->AddAttribute(elptr, XML_AUTHOR, (LPTSTR) (LPCTSTR) item.author))
	{
		return FALSE;
	}

	if(!pxml->AddAttribute(elptr, XML_CATEGORY, (LPTSTR) (LPCTSTR) item.category))
	{
		return FALSE;
	}

	if(ctype == CT_SEARCH)
	{
		if(item.svread >= 0)
		{
			pxml->AddAttribute(elptr, XML_SV_READ, _itot(item.svread, tmp, 10));
		}

		if(item.svreadon >= 0)
		{
			pxml->AddAttribute(elptr, XML_SV_READON, _itot(item.svreadon, tmp, 10));
		}

		if(item.svscrap >= 0)
		{
			pxml->AddAttribute(elptr, XML_SV_SCRAP, _itot(item.svscrap, tmp, 10));
		}

		if(item.svfilter >= 0)
		{
			pxml->AddAttribute(elptr, XML_SV_FILTER, _itot(item.svfilter, tmp, 10));
		}

		if(item.svhit >= 0)
		{
			pxml->AddAttribute(elptr, XML_SV_HIT, _itot(item.svhit, tmp, 10));
		}
	}

	if(ctype == CT_SEARCH || ctype == CT_LSEARCH)
	{
		pxml->AddAttribute(elptr, XML_STYPE, _itot(item.stype, tmp, 10) );
	}

	pxml->AddAttribute(elptr, XML_DESC, (LPTSTR) (LPCTSTR) item.description);

	pxml->AddAttribute(elptr, XML_ENCLOSURE, (LPTSTR) (LPCTSTR) item.enclosure);

	pxml->AddAttribute(elptr, XML_ENCODED, (LPTSTR) (LPCTSTR) item.encoded);
	
	return pxml->AppendChild(elptr, NULL);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * POST에 주어진 키워드가 있는지 확인한다.
 * linear search
 * Author or description or subject will be a destinated object.
 *
 * @Parameters
 * (in POSTITEM&) item - 검색 대상 post
 * (in CString) keyword - 키워드
 *
 * @Returns
 * (BOOL) 키워드 발견 여부
 **************************************************************************/
BOOL CheckKeyword(POSTITEM& item, CString keyword)
{
	TRACE(_T("key = %s, subject = %s\r\n"), keyword, item.subject);
	if(item.author.Find(keyword) != -1) return TRUE;
	if(item.description.Find(keyword) != -1) return TRUE;
	if(item.subject.Find(keyword) != -1) return TRUE;

	return FALSE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML에 Channel 정보를 갱신한다.
 *
 * @Parameters
 * (in/out RSS_XMLDATA&) data - 갱신할 XML 구조체
 * (in int) flag - 갱신 대상
 *
 * @Returns
 * (BOOL) 갱신 성공 여부
 **************************************************************************/
BOOL UpdateXMLChannelData(RSS_XMLDATA& data, int flag)
{
	// check XML is open
	if(!data.XML)
	{
		CEzXMLParser* pxml = new CEzXMLParser;

		SmartPtr<CEzXMLParser> apxml(pxml);

		if(!apxml->LoadXML((LPTSTR) (LPCTSTR) data.path))
			return FALSE;

		data.XML = apxml;
	}

	CEzXMLParser* pxml = data.XML.get();
	if(!pxml) return FALSE;

	IXMLDOMNodeListPtr	nodelist = NULL;
	IXMLDOMNodePtr		node = NULL;

	// check fish version
	nodelist = data.XML->SearchNodes(XML_XPATH_CHANNEL);

	if(nodelist == NULL || nodelist->length == 0)
		return FALSE;

	node = nodelist->item[0];

	TCHAR tmp[16];

	if(flag & FISH_CHANNEL_TERMFROM)
	{
		SetNamedNodeText(XML_FROM, pxml, _bstr_t(ConvertTimetoString(data.term.from)), 0, node);
	}

	if(flag & FISH_CHANNEL_TERMTO)
	{
		SetNamedNodeText(XML_TO, pxml, _bstr_t(ConvertTimetoString(data.term.to)), 0, node);
	}

	if(flag & FISH_CHANNEL_LOAD)
	{
		_stprintf(tmp, _T("%d"), data.load);
		SetNamedNodeText(XML_LOAD, pxml, tmp, 0, node);
	}

	if(flag & FISH_CHANNEL_TYPE)
	{
		_stprintf(tmp, _T("%d"), data.type);
		SetNamedNodeText(XML_CHANNELTYPE, pxml, tmp, 0, node);
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 ::  parameter chtype 추가, chtype이 CT_SEARCH인 경우
 *						추가 정보로 XML_SV_READ, XML_SV_READON, XML_SV_SCRAP 정보를 요청한다.
 *						by moonknit
 * updated 2005-12-13 :: node와 tag 정보 삭제
 *						node는 memory 정보에서만 처리를 하고 tag는 별도 구조체로 관리한다.
 *						by moonknit
 * updated 2006-03-15 :: encoded 정보 추가... by aquarelle
 *
 * @Description
 * XML에서 post id를 가진 post를 반환한다.
 *
 * @Parameters
 * (in/out POSTITEM&) item - post id를 넣어주면 해당하는 post의 정보를 받는다
 * (in int) chtype - 채널의 종류
 * (in int) read - 읽음 처리 여부 플래그
 * (in/out CEzXMLParser*) pxml - XML객체
 * (in DWORD) flag - 가져올 데이터 종류
 *
 * @Returns
 * (BOOL) 성공여부
 **************************************************************************/
BOOL GetXMLItem(POSTITEM& item, int chtype, int read, CEzXMLParser* pxml, DWORD flag)
{
	// XML에서 POST 정보를 찾아서 item에 세팅한다.
	// read값에 따라 flag 처리를 수행한다.
	if(!pxml)
		return FALSE;

	CString strxpath;
	if(item.postid != 0)
	{
		strxpath.Format(XML_XPATH_ITEMSEARCH_ID, item.postid);
	}
	else
	{
		strxpath.Format(XML_XPATH_ITEMSEARCH_GUID, item.guid);
	}
	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	item.chtype = chtype;

	IXMLDOMNodePtr nodeptr = listptr->item[0];

	if(flag & FISH_GET_FLAG)
	{
		if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) item.read = TRUE;
		else item.read = FALSE;
		if(read == 1)
		{
			SetNamedNodeText(XML_READ, pxml, _T("1"), 0, nodeptr);
		}
		else if(read == 2)
		{
			SetNamedNodeText(XML_READ, pxml, _T("0"), 0, nodeptr);
		}

#ifdef USE_XMLBNS
		if(GetNamedNodeText(XML_BNS, pxml, nodeptr) == _T("1")) item.bns = TRUE;
		else item.bns = FALSE;
#endif

		if(GetNamedNodeText(XML_READON, pxml, nodeptr) == _T("1")) item.readon = TRUE;
		else item.readon = FALSE;

//		if(GetNamedNodeText(XML_XMLUPDATE, pxml, nodeptr) == _T("1")) item.xmlupdate = TRUE;
//		else item.xmlupdate = FALSE;

		// 검색 결과인 경우 추가 정보를 추출한다.
		if(chtype == CT_SEARCH)
		{
			item.svread = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READ, pxml, nodeptr));
			item.svreadon = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READON, pxml, nodeptr));
			item.svscrap = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_SCRAP, pxml, nodeptr));
			item.svfilter = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_FILTER, pxml, nodeptr));
			item.svhit = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_HIT, pxml, nodeptr));

			item.stype = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_STYPE, pxml, nodeptr));
		}

		if(chtype == CT_LSEARCH)
		{
			item.stype = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_STYPE, pxml, nodeptr));
		}
	}

	if(flag & FISH_GET_STRING)
	{
		item.author = GetNamedNodeText(XML_AUTHOR, pxml, nodeptr);

		item.category = GetNamedNodeText(XML_CATEGORY, pxml, nodeptr);

		item.description = GetNamedNodeText(XML_DESC, pxml, nodeptr);

		item.enclosure = GetNamedNodeText(XML_ENCLOSURE, pxml, nodeptr);

		item.guid = GetNamedNodeText(XML_GUID, pxml, nodeptr);

		item.url = GetNamedNodeText(XML_URL, pxml, nodeptr);

		item.subject = GetNamedNodeText(XML_SUBJECT, pxml, nodeptr);

		item.encoded = GetNamedNodeText(XML_ENCODED, pxml, nodeptr);

		ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, pxml, nodeptr), item.pubdate);
	}

	/*
	int i;

	if(flag & FISH_GET_NODE)
	{
		listptr = pxml->SearchNodes(nodeptr, XML_NODE);
		if(listptr != NULL && listptr->length > 0)
		{
			ULONG ip;
			UINT port;
			int postid;

			for(i = 0; i< listptr->length; ++i)
			{
				ip = static_cast<ULONG> (atol(GetNamedNodeText(XML_IP, pxml, listptr->item[i])));
				port = static_cast<UINT> (atoi(GetNamedNodeText(XML_PORT, pxml, listptr->item[i])));
				postid = atoi(GetNamedNodeText(XML_POSTID, pxml, listptr->item[i]));

				item.plistNode->push_back(POSTNODE(ip, port, postid));
			}
		}
	}

	if(flag & FISH_GET_TAG)
	{
		listptr = pxml->SearchNodes(nodeptr, XML_TAG);
		if(listptr != NULL && listptr->length > 0)
		{
			CString t;
			TAG tag;
			for(i = 0; i< listptr->length; ++i)
			{
				 if(listptr->item[i]->text.length() > 0)
				 {
					 t = (LPTSTR) listptr->item[i]->text;
					 tag.t = t;
					 item.plistTag->push_back(tag);
				 }
			}
		}
	}
	*/

	return TRUE;
}

/**************************************************************************
 * static method CRSSDB::GetPostByXMLString
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-02
 * modified 2006-03-20 :: parameter channelid 추가 by moonknit
 *
 * @Description
 * XML에서 post id를 가진 post를 XML String 형식으로 반환한다.
 *
 * @Parameters
 * (in int) postid - 찾아올 post의 id
 * (out CString&) xml - 반환할 xml string
 * (in int) channelid - DEFAULT 0, 채널 아이디를 직접 넣어주면 m_mapPostMatch를 사용하는
 *					부담을 덜어준다.
 *
 * [Returns]
 * (BOOL) 성공여부
 **************************************************************************/
BOOL CRSSDB::GetPostXMLSring(int postid, CString& xml, int channelid)
{
	MAP_POSTMATCH::iterator pit;
	int chid = channelid;

	if(chid == 0)
	{
		pit = m_mapPostMatch.find(postid);
		if(pit == m_mapPostMatch.end())
		{
			return FALSE;
		}
		chid = (*pit).second;
	}

	BOOL result = FALSE;
	BOOL xmlfind = FALSE;

	XML_MAP::iterator it = m_mapData.find(chid);
	XML_LIST::iterator it2;
	BOOL	bopen = FALSE;

	if(it == m_mapData.end()) return FALSE;

	if(!(*it).second.ChannelLock()) return FALSE;

	// 메모리에서 찾는다.
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if(GetXMLItemString(postid, (*it2), bopen, xml))
		{
			result = TRUE;
			break;
		}
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 ::  parameter chtype 추가, GetXMLItem호출시 chtype을 넘긴다.
 *						by moonknit
 *
 * @Description
 * XML에서 post id를 가진 post를 반환한다.
 *
 * @Parameters
 * (in/out POSTITEM&) item - post id를 넣어주면 해당하는 post의 정보를 받는다
 * (in int) chtype - 채널의 종류
 * (in int) read - 읽음 처리 여부 플래그
 * (in/out RSS_XMLDATA&) xmldata - XML정보를 가진 구조체
 * (in/out BOOL&) bopen - XML 파일을 새로 열었는가에 대한 output
 * (in DWORD) flag - 가져올 데이터 종류
 *
 * [Returns]
 * (BOOL) 성공여부
 **************************************************************************/
BOOL GetXMLItem(POSTITEM& item, int chtype, int read, RSS_XMLDATA& xmldata, BOOL& bopen, DWORD flag)
{
	bopen = FALSE;
	if(xmldata.XML != NULL)
	{
		return GetXMLItem(item, chtype, read, xmldata.XML.get(), flag);
	}
	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	xmldata.XML = apxml;
	bopen = TRUE;
	return GetXMLItem(item, chtype, read, pxml, flag);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 :: parameter chtype 추가, chtype이 CT_SEARCH인 경우
 *						추가 정보로 XML_SV_READ, XML_SV_READON, XML_SV_SCRAP 정보를 요청한다.
 *						by moonknit
 * updated 2006-03-15 :: encoded 정보 추가. by aquarelle
 *
 * @Description
 * XML의 모든 item을 반환한다.
 *
 * @Parameters
 * (in/out auto_ptr<POST_LIST>&) aplistpost - item을 담을 list
 * (in int) chid - POST의 channelid 값
 * (in int) chtype - 채널의 종류
 * (in CEzXMLParser*) pxml - XML객체
 * (in BOOL) flag - 검색 조건
 * (in CString) scond - 검색 조건
 *
 * @Returns
 * (BOOL) Item 획득 여부
 **************************************************************************/
BOOL GetXMLItemList(auto_ptr<POST_LIST>& aplistpost, int chid, int chtype, CEzXMLParser* pxml
					, BOOL flag, CString scond)
{
	if(pxml == NULL) return FALSE;

	CString strxpath;
	if(scond != _T("")) strxpath = scond;

//	if(chtype == CT_SEARCH || chtype == CT_LSEARCH)
//	{
//		strxpath.Format(XML_XPATH_ITEM_ALL);
//	}
//	else 
		if(flag & FISH_GET_UNREADONLY)
	{
		strxpath.Format(XML_XPATH_ITEMSEARCH_UNREAD);
	}
	else if(flag & FISH_GET_READONLY)
	{
		strxpath.Format(XML_XPATH_ITEMSEARCH_READ);
	}
	else
	{
		strxpath.Format(XML_XPATH_ITEM_ALL);
	}

	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
		return FALSE;

	IXMLDOMNodePtr nodeptr = NULL;

	POSTITEM item;

	int i;

	for(i = 0; i < listptr->length ; ++i)
	{
		nodeptr = listptr->item[i];

		item.postid = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_ID, pxml, nodeptr));

		item.chtype = chtype;

		if(item.postid == 0) continue;

		if(flag & FISH_GET_FLAG)
		{
			if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) item.read = TRUE;
			else item.read = FALSE;

			if(GetNamedNodeText(XML_READON, pxml, nodeptr) == _T("1")) item.readon = TRUE;
			else item.readon = FALSE;

#ifdef USE_XMLBNS
			if(GetNamedNodeText(XML_BNS, pxml, nodeptr) == _T("1")) item.bns = TRUE;
			else item.bns = FALSE;
#endif

//			if(GetNamedNodeText(XML_XMLUPDATE, pxml, nodeptr) == _T("1")) item.xmlupdate = TRUE;
//			else item.xmlupdate = FALSE;

			// 검색 결과인 경우 추가 정보를 추출한다.
			if(chtype == CT_SEARCH)
			{
				item.svread = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READ, pxml, nodeptr));
				item.svreadon = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READON, pxml, nodeptr));
				item.svscrap = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_SCRAP, pxml, nodeptr));
				item.svfilter = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_FILTER, pxml, nodeptr));
				item.svhit = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_HIT, pxml, nodeptr));
				
				item.stype = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_STYPE, pxml, nodeptr));
			}

			if(chtype == CT_LSEARCH)
			{
				item.stype = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_STYPE, pxml, nodeptr));
			}
		}

		if(flag & FISH_GET_STRING)
		{
			item.author = GetNamedNodeText(XML_AUTHOR, pxml, nodeptr);

			item.category = GetNamedNodeText(XML_CATEGORY, pxml, nodeptr);

			item.description = GetNamedNodeText(XML_DESC, pxml, nodeptr);

			item.enclosure = GetNamedNodeText(XML_ENCLOSURE, pxml, nodeptr);

			item.guid = GetNamedNodeText(XML_GUID, pxml, nodeptr);

			item.url = GetNamedNodeText(XML_URL, pxml, nodeptr);

			item.subject = GetNamedNodeText(XML_SUBJECT, pxml, nodeptr);

			item.encoded = GetNamedNodeText(XML_ENCODED, pxml, nodeptr);


			ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, pxml, nodeptr), item.pubdate);
		}

		// channel data를 가져갈 때에는 tag와 nodelist를 가져가지 않는다.

		aplistpost->push_back(item);

	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 :: 파라미터 int chtype 추가 및 GetXMLItemList 호출시 chtype 전달 by moonknit
 *
 * @Description
 * XML의 모든 item을 반환한다.
 *
 * @Parameters
 * (in/out auto_ptr<POST_LIST>&) aplistpost - item을 담을 list
 * (in int) chid - POST의 channelid 값
 * (in int) chtype - 채널의 종류
 * (in/out RSS_XMLDATA&) xmldata - XML객체를 담은 구조체
 * (in/out BOOL&) bopen - 닫힌 파일을 열어서 작업을 했다면 TRUE로 반환한다.
 * (in BOOL) flag - 검색 조건
 * (in CString) scond - 검색 조건
 *
 * @Returns
 * (BOOL) Item 획득 여부
 **************************************************************************/
BOOL GetXMLItemList(auto_ptr<POST_LIST>& aplistpost, int chid, int chtype
					, RSS_XMLDATA& xmldata, BOOL& bopen, BOOL flag, CString scond)
{
	bopen = FALSE;
	if(xmldata.XML != NULL)
		return GetXMLItemList(aplistpost, chid, chtype, xmldata.XML.get(), flag, scond);
	
	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	bopen = TRUE;
	xmldata.XML = apxml;
	return GetXMLItemList(aplistpost, chid, chtype, pxml, flag, scond);
}

/**************************************************************************
 * function GetXMLItemString
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-02
 *
 * @Description
 * 주어진 post id의 post를 XML string 형식으로 추출해낸다.
 *
 * @Parameters
 * (in int) postid - 추출할 post의 id
 * (in/out CEzXMLParser*) pxml - XML parsing을 수행할 Wrapper class
 * (out CString&) xml - 반환할 string
 *
 * @Returns
 * (BOOL) Item 획득 여부
 **************************************************************************/
BOOL GetXMLItemString(int postid, CEzXMLParser* pxml, CString& xml)
{
	// XML에서 POST 정보를 찾아서 item에 세팅한다.
	// read값에 따라 flag 처리를 수행한다.
	if(!pxml)
		return FALSE;

	CString strxpath;
	strxpath.Format(XML_XPATH_ITEMSEARCH_ID, postid);
	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	IXMLDOMNodePtr nodeptr = listptr->item[0];

	xml = (LPTSTR) nodeptr->xml;

	return TRUE;
}

/**************************************************************************
 * function GetXMLItemString
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-02
 *
 * @Description
 * 주어진 post id의 post를 XML string 형식으로 추출해낸다.
 *
 * @Parameters
 * (in int) postid - 추출할 post의 id
 * (in/out RSS_XMLDATA&) xmldata - XML객체를 담은 구조체
 * (in/out BOOL&) bopen - 닫힌 파일을 열어서 작업을 했다면 TRUE로 반환한다.
 * (out CString&) xml - 반환할 string
 *
 * @Returns
 * (BOOL) Item 획득 여부
 **************************************************************************/
BOOL GetXMLItemString(int postid, RSS_XMLDATA& xmldata, BOOL& bopen, CString& xml)
{
	bopen = FALSE;
	if(xmldata.XML != NULL)
		return GetXMLItemString(postid, xmldata.XML.get(), xml);
	
	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	bopen = TRUE;
	xmldata.XML = apxml;
	return GetXMLItemString(postid, pxml, xml);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * XML의 모든 Item의 내용을 동시에 갱신한다. read와 readon 플래그만 갱신한다.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - XML 객체
 * (in BOOL) read - 수정될 값 TRUE or FALSE
 *
 * @Returns
 * (BOOL) 수정 성공 여부
 **************************************************************************/
BOOL UpdateXMLItemListRead(CEzXMLParser* pxml, BOOL read)
{
	if(pxml == NULL) return FALSE;

	IXMLDOMNodeListPtr listptr = NULL;

	if(read)
	{
		listptr = pxml->SearchNodes(XML_XPATH_ITEMSEARCH_UNREAD);
	}
	else
	{
		listptr = pxml->SearchNodes(XML_XPATH_ITEMSEARCH_READ);
	}

	if(listptr == NULL || listptr->length == 0)
		return FALSE;

	IXMLDOMNodePtr nodeptr = NULL;

	int i;

	_bstr_t text;
	if(read) text = _T("1");
	else text = _T("0");

	BOOL postread = FALSE;

	for(i = 0; i < listptr->length ; ++i)
	{
		nodeptr = listptr->item[i];

		SetNamedNodeText(XML_READ, pxml, text, 0, nodeptr);
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * XML의 모든 Item의 내용을 동시에 갱신한다. read와 readon 플래그만 갱신한다.
 *
 * @Parameters
 * (in/out RSS_XMLDATA&) xmldata - XML 객체를 담은 포인터
 * (in DWORD) flag - 수정할 종류
 * (in BOOL) read - 수정될 값 TRUE or FALSE
 *
 * @Returns
 * (BOOL) 수정 성공 여부
 **************************************************************************/
BOOL UpdateXMLItemListRead(RSS_XMLDATA& xmldata, BOOL read)
{
	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	xmldata.XML = apxml;
	return UpdateXMLItemListRead(pxml, read);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 ::  parameter chtype 추가, chtype이 CT_SEARCH인 경우
 *						XML_SV_READ, XML_SV_READON, XML_SV_SCRAP갱신을 수행한다.
 *						by moonknit
 * updated 2005-12-13 :: node와 tag 정보 삭제
 *						node는 memory 정보에서만 처리를 하고 tag는 별도 구조체로 관리한다.
 *						by moonknit
 * updated 2006-03-15 :: encoded 정보 추가. by aquarelle
 *
 * @Description
 * XML의 Item의 내용을 갱신한다. 
 *
 * @Parameters
 * (in POSTITEM&) item - 갱신할 내용을 가지고 있는 객체
 * (in/out CEzXMLParser*) pxml - XML 객체
 * (out int&) unread - 읽음 변경 여부 ( 0 : 변경 없음, 1 : 안 읽음 -> 읽음, -1 : 읽음 -> 안 읽음)
 * (in DWORD) flag - 수정될 대상
 *
 * @Returns
 * (BOOL) 수정 성공 여부
 **************************************************************************/
BOOL CRSSDB::UpdateXMLItem(POSTITEM& item, int chtype, CEzXMLParser* pxml, int& unread, DWORD flag)
{
	// XML에서 POST 정보를 찾아서 주어진 item값으로 정보를 변경한다.
	IXMLDOMNodePtr nodeptr;
	if(item.postid != 0)
	{
		if(!CheckItemByPostID(pxml, item.postid, nodeptr))
			return FALSE;
	}
	else if(!CheckItemByGUID(pxml, item.guid, nodeptr)) return FALSE;

	TCHAR tmp[16];

	unread = 0;
	if(item.postid == 0)
	{
		// 결과 처리를 위해 알려지지 않은 postid값을 세팅하여준다.
		item.postid = _ttoi(GetNamedNodeText(XML_ID, pxml, nodeptr));
	}

	if(flag & FISH_ITEM_READ)
	{
		BOOL postread;
		if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) postread = TRUE;
		else postread = FALSE;

		if(item.read != postread)
		{
			_stprintf(tmp, _T("%d"), (item.read) ? 1 : 0);
			SetNamedNodeText(XML_READ, pxml, tmp, 0, nodeptr);

			if(postread) 
			{
				unread = 1;
			}
			else unread = -1;
		}

#ifdef USE_LOCAL_READCNT
		if(item.read)
		{
			// increase read count of the local post
			if(chtype != CT_SEARCH && (flag & FISH_ITEM_INC_COUNT))
			{
				item.svread = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READ, pxml, nodeptr));
				++(item.svread);
				_stprintf(tmp, _T("%d"), item.svread);
				SetNamedNodeText(XML_SV_READ, pxml, tmp, 0, nodeptr);
			}
		}
#endif
	}

	// 채널의 종류가 검색인 경우에만 검색값의 수정을 허용한다.
	if((chtype == CT_SEARCH) && (flag & FISH_ITEM_SEARCHVALUE))
	{
		if(item.svread >= 0)
		{
			SetNamedNodeText(XML_SV_READ, pxml, _itot(item.svread, tmp, 10), 0, nodeptr);
		}

		if(item.svreadon >= 0)
		{
			SetNamedNodeText(XML_SV_READON, pxml, _itot(item.svreadon, tmp, 10), 0, nodeptr);
		}

		if(item.svscrap >= 0)
		{
			SetNamedNodeText(XML_SV_SCRAP, pxml, _itot(item.svscrap, tmp, 10), 0, nodeptr);
		}

		if(item.svfilter >= 0)
		{
			SetNamedNodeText(XML_SV_FILTER, pxml, _itot(item.svfilter, tmp, 10), 0, nodeptr);
		}

		if(item.svhit >= 0)
		{
			SetNamedNodeText(XML_SV_HIT, pxml, _itot(item.svhit, tmp, 10), 0, nodeptr);
		}
	}

	if(item.stype != NO_SEARCH && (flag & FISH_ITEM_SEARCHTYPE))
	{
		SetNamedNodeText(XML_STYPE, pxml, _itot(item.stype, tmp, 10), 0, nodeptr);
	}

	if(flag & FISH_ITEM_READON)
	{
		SetNamedNodeText(XML_READON, pxml, item.readon ? _T("1") : _T("0"), 0, nodeptr);

		if(item.readon)
		{
#ifdef USE_LOCAL_READCNT
			if(chtype != CT_SEARCH && (flag & FISH_ITEM_INC_COUNT))
			{
				item.svreadon = _ttoi((LPTSTR) (LPCTSTR) GetNamedNodeText(XML_SV_READON, pxml, nodeptr));
				++(item.svreadon);
				_stprintf(tmp, _T("%d"), item.svreadon);
				SetNamedNodeText(XML_SV_READON, pxml, tmp, 0, nodeptr);
			}
#endif

			if(flag & FISH_ITEM_URL_NAVIGATE)
			{
				CString tmpurl = GetNamedNodeText(XML_URL, pxml, nodeptr);
				if(tmpurl.GetLength() > 0 && m_hInformWnd)
				{
					TCHAR* lpszurl = (TCHAR*) malloc((tmpurl.GetLength() + 1)* sizeof(TCHAR));
					_tcscpy(lpszurl, (LPTSTR) (LPCTSTR) tmpurl);

					SendMessage(m_hInformWnd, WM_NAVIGATE_URL, (WPARAM) lpszurl, 0);
				}
			}
		}
	}

//	if(flag & FISH_ITEM_XMLUPDATE)
//	{
//		_stprintf(tmp, _T("%d"), (item.xmlupdate) ? 1 : 0);
//		SetNamedNodeText(XML_XMLUPDATE, pxml, tmp, 0, nodeptr);
//	}

	if(flag & FISH_ITEM_URL)
	{
		SetNamedNodeText(XML_URL, pxml, _bstr_t(item.url), 0, nodeptr);
	}

	// GUID는 변경되지 않는다.
/*	if(flag & FISH_ITEM_GUID)
	{
		SetNamedNodeText(XML_READ, *pxml, _bstr_t(item.guid), 0, nodeptr);
	}
*/
	if(flag & FISH_ITEM_AUTHOR)
	{
		SetNamedNodeText(XML_AUTHOR, pxml, _bstr_t(item.author), 0, nodeptr);
	}

	if(flag & FISH_ITEM_CATEGORY)
	{
		SetNamedNodeText(XML_CATEGORY, pxml, _bstr_t(item.category), 0, nodeptr);
	}

	if(flag & FISH_ITEM_SUBJECT)
	{
		SetNamedNodeText(XML_SUBJECT, pxml, _bstr_t(item.subject), 0, nodeptr);
	}

	if(flag & FISH_ITEM_DESCRIPTION)
	{
		SetNamedNodeText(XML_DESC, pxml, _bstr_t(item.description), 0, nodeptr);
	}

	if(flag & FISH_ITEM_PUBDATE)
	{
		SetNamedNodeText(XML_PUBDATE, pxml, _bstr_t(ConvertTimetoString(item.pubdate)), 0, nodeptr);
	}

	if(flag & FISH_ITEM_ENCLOSURE)
	{
		SetNamedNodeText(XML_ENCLOSURE, pxml, _bstr_t(item.enclosure), 0, nodeptr);
	}

	if(flag & FISH_ITEM_ENCODED)
	{
		SetNamedNodeText(XML_ENCODED, pxml, _bstr_t(item.encoded), 0, nodeptr);
	}

/*
	IXMLDOMNodeListPtr listptr = NULL;
	IXMLDOMElementPtr elptr = NULL;
	int i;

	if(flag & FISH_ITEM_NODE)
	{
		// 추가하는 경우가 아니면 기존 목록을 삭제한다.
		if(!(flag & FISH_ADD_LIST))
		{
			listptr = pxml->SearchNodes(nodeptr, XML_NODE);
			if(listptr != NULL && listptr->length > 0)
			{
				for(i = listptr->length - 1; i >= 0; --i)
				{
					nodeptr->removeChild(listptr->item[i]);
				}
			}
		}

		if(item.plistNode.get() != NULL)
		{
			NODE_LIST::iterator nit;
			for(nit = item.plistNode->begin(); nit != item.plistNode->end(); ++nit)
			{
				elptr = pxml->CreateElement(XML_NODE);
				if(elptr != NULL)
				{
					_stprintf(tmp, "%ul", (*nit).ip);
					if(pxml->AddAttribute(elptr, XML_IP, tmp))
						break;
					_stprintf(tmp, "%u", (*nit).port);
					if(pxml->AddAttribute(elptr, XML_PORT, tmp))
						break;
					_stprintf(tmp, "%d", (*nit).postid);
					if(pxml->AddAttribute(elptr, XML_POSTID, tmp))
						break;

					pxml->AppendChild(elptr, nodeptr);
				}
			}

			elptr = NULL;
		}
	}

	if(flag & FISH_ITEM_TAG)
	{
		if(!(flag & FISH_ADD_LIST))
		{
			listptr = pxml->SearchNodes(nodeptr, XML_TAG);
			if(listptr != NULL && listptr->length > 0)
			{
				for(i = listptr->length - 1; i >= 0; --i)
				{
					nodeptr->removeChild(listptr->item[i]);
				}
			}
		}

		if(item.plistTag.get() != NULL)
		{
			TAG_LIST::iterator tit;

			for(tit = item.plistTag->begin(); tit != item.plistTag->end(); ++tit)
			{
				elptr = pxml->CreateElement(XML_TAG);
				if(elptr != NULL)
				{
					elptr->put_text(_bstr_t((*tit).t));
					pxml->AppendChild(elptr, nodeptr);
				}
			}
		}

		elptr = NULL;

	}
*/

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 ::  parameter chtype 추가, UpdateXMLItem 호출시 chtype 전달
 *						by moonknit
 *
 * @Description
 * XML의 Item의 내용을 갱신한다. 
 *
 * @Parameters
 * (in POSTITEM&) item - 갱신할 내용을 가지고 있는 객체
 * (in int) chtype - 채널의 종류 
 * (in/out RSS_XMLDATA&) xmldata - XML 객체를 가지고 있는 구조체의 객체
 * (in DWORD) flag - 수정될 대상
 *
 * @Returns
 * (BOOL) 수정 성공 여부
 **************************************************************************/
BOOL CRSSDB::UpdateXMLItem(POSTITEM& item, int chtype, RSS_XMLDATA& xmldata, BOOL& bopen, int& unread, DWORD flag)
{
	bopen = FALSE;
	if(xmldata.XML != NULL)
	{
		return UpdateXMLItem(item, chtype, xmldata.XML.get(), unread, flag);
	}

	CEzXMLParser* pxml = new CEzXMLParser;

	SmartPtr<CEzXMLParser> apxml(pxml);

	if(!apxml->LoadXML((LPTSTR) (LPCTSTR) xmldata.path))
		return FALSE;

	xmldata.XML = apxml;
	bopen = TRUE;
	return UpdateXMLItem(item, chtype, xmldata.XML.get(), unread, flag);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 맵에 새로운 채널을 생성 등록 한다.
 *
 * @Parameters
 * (in/out XML_MAP&) m - 채널 맵
 * (in int) channelid - 생성할 채널의 id
 * (in int) chtype - 생성할 채널의 종류
 * 
 * @Returns
 * (XML_MAP::iterator) 생성된 채널의 iterator
 * 생성에 실패하면 m.end()를 반환한다.
 **************************************************************************/
XML_MAP::iterator MakeNewChannel(XML_MAP& m, int channelid, int chtype)
{
	pair<XML_MAP::iterator, bool> tmppair;

	
	tmppair = m.insert(XML_MAP::value_type(channelid, RSS_CHANNELDATA(chtype)));
	if(!tmppair.second)
	{
		return m.end();
	}

	return tmppair.first;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRSSDB::CRSSDB()
{
	m_Init = FALSE;
	InitStatic();
}

CRSSDB::~CRSSDB()
{
	TRACE(_T("CRSSDB Destroy\r\n"));
	DeinitStatic();
}

void CRSSDB::SetInformWnd(HWND h)
{
	m_hInformWnd = h;
}

void CRSSDB::SetSubscriveWnd(HWND h)
{
	m_hSubscribeWnd = h;
}

//////////////////////////////////////////////////////////////////////
// Initialize
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 각종 CriticalSection을 초기화한다.
 **************************************************************************/
void CRSSDB::InitStatic()
{
	if(m_nInit == 0)
	{
		CString sPrevSeq = ReadRegData(_T(""), REG_STR_LASTSEQ);
		m_PrevlastSeq = _ttoi((LPTSTR) (LPCTSTR) sPrevSeq);

		InitializeCriticalSection(&m_csMap);

		for(int i = 0; i <MAX_WORKLISTCNT ; ++i)
		{
			InitializeCriticalSection(&m_csInQueue[i]);
			InitializeCriticalSection(&m_csOutQueue[i]);
		}

		for(i = 0; i < MAX_SEARCHLISTCNT ; ++i)
		{
			InitializeCriticalSection(&m_csSearch[i]);
		}

		InitializeCriticalSection(&m_csKeywordList);

		CoInitialize(NULL);
	}

	++m_nInit;

	m_Init = TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 각종 CriticalSection을 해제한다.
 * XML 데이터를 파일로 Flush 한다.
 * Thread가 떠 있다면 Thread를 정리한다.
 **************************************************************************/
void CRSSDB::DeinitStatic()
{
	if(!m_Init) return;

	--m_nInit;

	if(m_nInit == 0)
	{
		// 강제 종료중에는 Flush를 하지 않는다.
		if(!theApp.IsExiting())
		{
			BOOL keepon;
			if(!m_bLastFlush)
				FlushData(TRUE, keepon);

			StopSThread();
			StopThread();
		}

		DeleteCriticalSection(&m_csMap);

		for(int i = 0; i < MAX_WORKLISTCNT; ++i)
		{
			DeleteCriticalSection(&m_csInQueue[i]);
			DeleteCriticalSection(&m_csOutQueue[i]);
		}

		for(i = 0; i < MAX_SEARCHLISTCNT; ++i)
		{
			DeleteCriticalSection(&m_csSearch[i]);
		}

		DeleteCriticalSection(&m_csKeywordList);

		CoUninitialize();

		if(IsPostInit())
		{
			TCHAR szbuf[16];
			WriteRegData(_T(""), REG_STR_LASTSEQ, _itot(m_npostseq, szbuf, 10));
		}
	}

	m_Init = FALSE;
}

//////////////////////////////////////////////////////////////////////
// Work Process
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 작업 처리결과를 queue에 기록하고 등록된 윈도우에 처리결과가 들어왔음을 메시지를 보내 알린다.
 *
 * @Parameters
 * (in int) i - 결과 처리 Priority
 * (in auto_ptr<RESULT>&) ptr - 등록할 처리결과
 **************************************************************************/
void CRSSDB::PUSH_RESULT(int i, auto_ptr<RESULT>& ptr)
{
	m_err = _T("");

	if(i < 0 || i >= MAX_WORKLISTCNT) return;

	EnterCriticalSection(&m_csOutQueue[i]);
	m_queueResult[i].push_front(ptr);
	LeaveCriticalSection(&m_csOutQueue[i]);

	if(m_hInformWnd)
	{
		SendMessage(m_hInformWnd, WM_RSSDBRESULT, NULL, NULL);
	}
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-30
 *
 * @Description
 * 검색 작업을 실행한다.
 * 검색 스레드로 작업을 넘김
 *
 * @Parameters
 * (in/out auto_ptr<WORK>&) apwork - 검색 작업 내용을 담고 있는 auto_ptr
 **************************************************************************/
void CRSSDB::WP_Search(auto_ptr<WORK>& apwork)
{

	TRACE(_T("CRSSDB::WP_Search\r\n"));
	auto_ptr<WorkSearch> apsearch ( static_cast<WorkSearch*> (apwork.release()) );

	// 검색 스레드 큐에 넣는다.
	int i = GetSearchPriority(apsearch->type);
	if(i < 0 || i >= MAX_SEARCHLISTCNT) return;

	if(m_queueSearch[i].size() > MAX_SEARCHQUEUE) return;

	EnterCriticalSection(&m_csSearch[i]);
	m_queueSearch[i].push_front(apsearch);
	LeaveCriticalSection(&m_csSearch[i]);
	
	TriggerSearch();
}

/**************************************************************************
 * static method CRSSDB::WP_Upload
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-02
 *
 * @Description
 * 포스트를 찾아서 Uploader로 넘기도록 처리한다.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - 작업할 내용을 가지고 있는 구조체의 포인터, 등록할 포스트목록을 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_Upload(WorkUpload* pupload, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = TRUE;
	CString xml;

	result = GetPostXMLSring(pupload->postid, xml);

	apr = auto_ptr<RESULT> (new ResultUpload(pupload->workid, result, xml, pupload->lpuploader));
	p = PTW_POST_UPLOAD;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 여러개의 새로운 post를 해당 채널에 추가한다.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - 작업할 내용을 가지고 있는 구조체의 포인터, 등록할 포스트목록을 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_PostAdd(WorkAddPost* ppostadd, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = FALSE;
	int postcnt = 0;
	int channelid = 0;
	int Ex = 0;

	if(ppostadd->aplistpost.get() == NULL) result = FALSE;
	else
	{
		POST_LIST::iterator it, tit;
		int keepcnt = ppostadd->aplistpost->size();
		for(it = ppostadd->aplistpost->begin(); it != ppostadd->aplistpost->end() && m_bThread; )
		{
			tit = it;
			++it;
			if(AddPost((*tit)))
			{
				++postcnt;

				if(!channelid)	channelid = (*tit).channelid;

				result = TRUE;
			}
			else
			{
				ppostadd->aplistpost->erase(tit);
			}
		}

		if(keepcnt > MIN_KEEPCNT)
		{
			XML_MAP::iterator cit = m_mapData.find(channelid);

			if(cit != m_mapData.end())
			{
				if((*cit).second.keepcnt < keepcnt)
				{
					Ex = keepcnt;
				}
			}
		}
	}

	ResultAddPost* prap = new ResultAddPost(ppostadd->workid, ppostadd->subtype, result, ppostadd->t
		, ppostadd->aplistpost, ppostadd->guid, m_err, ppostadd->addtype, ppostadd->channelid, postcnt);

	// 새로 갱신될 keepcnt, MIN_KEEPCNT 이상만 유효하다.
	prap->Ex = Ex;

	apr = auto_ptr<RESULT> (prap);
	p = PTW_POST_ADD;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 여러 post의 내용을 갱신한다.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - 작업할 내용을 가지고 있는 구조체의 포인터, 등록할 포스트목록을 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_PostUpdate(WorkPostData* ppostdata, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = TRUE;

	if(ppostdata->aplistpost.get() == NULL) result = FALSE;
	else
	{
		POST_LIST::iterator it;
		for(it = ppostdata->aplistpost->begin(); it != ppostdata->aplistpost->end(); ++it)
		{
			TRACE(_T("POST UPDATED start : %d\r\n"), (*it).postid);
			if(!UpdatePostData((*it), ppostdata->adinfo))
				result = FALSE;
			TRACE(_T("POST UPDATED end : %d\r\n"), (*it).postid);
		}
	}

	apr = auto_ptr<RESULT> (new ResultPost(ppostdata->workid, ppostdata->subtype, result, ppostdata->aplistpost, _T(""), ppostdata->adinfo));
	p = PTW_POST_UPDATE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 주어진 id의 포스트들을 삭제한다.
 *
 * @Parameters
 * (in/out WorkPostID*) ppostid - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 삭제할 포스트아이디의 목록을 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_PostDelete(WorkPostID* ppostid, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = TRUE;

	if(ppostid->aplistid.get() == NULL)
	{
		result = FALSE;
	}
	else
	{
		ID_LIST::iterator it;
		for(it = ppostid->aplistid->begin(); it != ppostid->aplistid->end(); ++it)
		{
			RemovePostByPostID((*it), ppostid->adinfo, ppostid->channelid);
		}
	}

	apr = auto_ptr<RESULT> (new ResultNormal(ppostid->workid, ppostid->subtype, result));
	p = PTW_POST_DELETE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 주어진 id의 포스트들을 대상 채널로 스크랩한다.
 *
 * @Parameters
 * (in/out WorkPostID*) ppostid - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 스크랩할 채널의 id를 담고있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_PostScrap(WorkPostID* ppostid, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = TRUE;

	if(ppostid->aplistid.get() == NULL)
	{
		result = FALSE;
	}
	else
	{
		ID_LIST::iterator it;
		for(it = ppostid->aplistid->begin(); it != ppostid->aplistid->end(); ++it)
		{
			ScrapPostByPostID((*it), ppostid->adinfo);
		}
	}

	apr = auto_ptr<RESULT> (new ResultNormal(ppostid->workid, ppostid->subtype, result));
	p = PTW_POST_DELETE;
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 주어진 포스트들의 내용을 가져온다.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 삭제할 포스트의 목록을 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_PostGet(WorkPostData* ppostdata, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result = GetPostListData(ppostdata->aplistpost.get(), ppostdata->adinfo);

	apr = auto_ptr<RESULT> (new ResultPost(ppostdata->workid, ppostdata->subtype, result, ppostdata->aplistpost));
	p = PTW_POST_GET;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 채널의 모든 포스트의 읽음 상태를 변경한다.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 삭제할 채널의 id와 읽음 처리 정보를 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelRead(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 채널의 읽음 상태를 일괄적으로 변경한다.
	BOOL result = UpdateChannelDataRead(pchannel->channelid, pchannel->adinfo);

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, result));
	p = PTW_CHANNEL_FLUSH;
}

/**************************************************************************
 * written by WP_ChannelModify
 *
 * @history
 * created 2005-12-19
 *
 * @Description
 * 채널의 정보를 변경한다.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 삭제할 채널의 id와 읽음 처리 정보를 담고 있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelModify(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 채널의 읽음 상태를 일괄적으로 변경한다.
	BOOL result = ModifyChannel(pchannel->channelid, pchannel->adinfo, pchannel->addata);

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, result));
	p = PTW_CHANNEL_FLUSH;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 모든 채널의 flush를 수행한다.
 *
 * @Parameters
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
int CRSSDB::WP_ChannelFlush(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 모든 채널의 변경된 데이터를 flush한다.
	BOOL keepon;
	pchannel->addata += FlushData(FALSE, keepon);

	if(keepon) 
	{
		return 2;
	}

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, pchannel->addata ));
	p = PTW_CHANNEL_FLUSH;
	return 0;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2006-02-27
 *
 * @Description
 * 모든 채널의 XML 저장 패스를 재 설정한다.
 *
 * @Parameters
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelSetPath(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 모든 채널의 변경된 데이터를 flush한다.
	BOOL result = SetPath(pchannel->text);

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, result ));
	p = PTW_CHANNEL_FLUSH;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 지정된 디렉토리의 xml파일로 부터 channel의 post정보를 읽어온다.
 *
 * @Parameters
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 *
 * @Result
 * (int) - 채널 로딩 작업을 계속 해야할 경우에는 1을 그렇지 않을 경우에는 0을 반환한다.
 **************************************************************************/
int CRSSDB::WP_ChannelLoad(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 주어진 디렉토리에서 fish 데이터를 읽어온다.
	// fish 데이터를 읽어오기전에 channel의 생성이 선행되어야 한다.
	BOOL keepon = FALSE;
	int start = pchannel->adinfo;
	int nread = 0;
	int next = LoadXMLData(start, nread, keepon);

	pchannel->adinfo = next;
	pchannel->addata += nread;

	if(keepon) 
	{
		return 2;
	}

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, pchannel->addata));
	p = PTW_CHANNEL_LOAD;
	return 0;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-10
 *
 * @Description
 * 새로운 채널을 추가한다.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 추가할 채널의 정보를 담고있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelAdd(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 채널을 생성한다.
	BOOL result = AddChannel(pchannel->channelid, pchannel->adinfo
		, pchannel->addata, pchannel->text, pchannel->textEx
		, pchannel->addataEx[0], pchannel->addataEx[1]);

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, result));
	p = PTW_CHANNEL_ADD;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 채널을 삭제한다.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 삭제할 채널의 정보를 담고있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelDelete(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result;
	if( ((BOOL) pchannel->adinfo) == TRUE)
	{
		// 채널 아이디를 사용하여 채널의 item의 데이터를 모두 삭제한다.
		RemovePostByChannelID(pchannel->channelid);
	}
	else
	{
		// 채널 아이디를 이용하여 채널자체를 삭제한다.
		result = RemoveChannel(pchannel->channelid);
	}

	apr = auto_ptr<RESULT> (new ResultNormal(pchannel->workid, pchannel->subtype, result));
	p = PTW_CHANNEL_DELETE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 채널의 모든 post를 가져온다.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - 작업할 내용을 가지고 있는 구조체의 포인터
 *			, 가져올 채널의 정보를 담고있다.
 * (out int&) p - 처리결과의 Priority
 * (out auto_ptr<RESULT>&) apr - 처리결과를 담아 반환할 포인터
 **************************************************************************/
void CRSSDB::WP_ChannelGet(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// 채널 아이디를 채널의 포스트를 읽어온다.
	auto_ptr<POST_LIST> aplistpost(new POST_LIST);
	BOOL result = GetChannelPostData(pchannel->channelid, pchannel->adinfo, aplistpost);

	apr = auto_ptr<RESULT> (new ResultPost(pchannel->workid, pchannel->subtype, result, aplistpost, m_err, pchannel->channelid));
	p = PTW_CHANNEL_GET;

	POST_LIST::iterator it;

	ResultPost* ppost = 
		static_cast<ResultPost*> (apr.get());

}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-19 :: channel modify 추가 by moonknit
 *
 * @Description
 * WORK_LIST에서 처리할 일을 가져와 그 처리에 필요한 함수를 호출해준다.
 *
 * @Parameters
 * (in WORK_LIST&) wlist - 처리할 WORK_LIST
 * (in CRITICAL_SECTION&) cs - WORK_LIST와 쌍을 이루는 critical section
 **************************************************************************/
void CRSSDB::WorkProcess(WORK_LIST& wlist, CRITICAL_SECTION& cs)
{
	int goon = 0;
	auto_ptr<WORK> apwork;
	EnterCriticalSection(&cs);
//	TRACE(_T("work item process LOCK\r\n"));
	if(wlist.size() > 0)
		apwork = wlist.back();
	wlist.pop_back();
	LeaveCriticalSection(&cs);
//	TRACE(_T("work item process UNLOCK\r\n"));

	if(apwork.get() == NULL) return;

	SetWorking(TRUE);

#ifdef _DEBUG
	DWORD start, interval;
	start = GetTickCount();
#endif

	int priority = 0;
	auto_ptr<RESULT> apr;

	switch(apwork->worktype)
	{
	case WT_POSTADD:
		{
			WorkAddPost* ppostadd = 
				static_cast<WorkAddPost*> (apwork.get());

//			TRACE(_T("WS_POST_ADD\r\n"));
			WP_PostAdd(ppostadd, priority, apr);
		}
		break;
	case WT_POSTDATA:
		{
			WorkPostData* ppostdata = 
				static_cast<WorkPostData*> (apwork.get());

			switch(ppostdata->subtype)
			{
			case WS_POST_UPDATE:
//				TRACE(_T("WS_POST_UPDATE\r\n"));
				WP_PostUpdate(ppostdata, priority, apr);
				break;
			case WS_POST_GET:
//				TRACE(_T("WS_POST_GET\r\n"));
				WP_PostGet(ppostdata, priority, apr);
				break;
			}
		}
		break;
	case WT_POSTID:
		{
			WorkPostID* ppostid = 
				static_cast<WorkPostID*> (apwork.get());

			switch(ppostid->subtype)
			{
			case WS_POST_DELETE:
//				TRACE(_T("WS_POST_DELETE\r\n"));
				WP_PostDelete(ppostid, priority, apr);
				break;
			case WS_POST_SCRAP:
//				TRACE(_T("WS_POST_SCRAP\r\n"));
				WP_PostScrap(ppostid, priority, apr);
				break;
			}
		}
		break;
	case WT_CHANNEL:
		{
			WorkChannel* pchannel =
				static_cast<WorkChannel*> (apwork.get());
			switch(pchannel->subtype)
			{
			case WS_CHANNEL_ADD:
//				TRACE(_T("WS_CHANNEL_ADD\r\n"));
				WP_ChannelAdd(pchannel, priority, apr);
				break;
			case WS_CHANNEL_DELETE:
//				TRACE(_T("WS_CHANNEL_DELETE\r\n"));
				WP_ChannelDelete(pchannel, priority, apr);
				break;
			case WS_CHANNEL_GET:
//				TRACE(_T("WS_CHANNEL_GET\r\n"));
				WP_ChannelGet(pchannel, priority, apr);
				break;
			case WS_CHANNEL_LOAD:
//				TRACE(_T("WS_CHANNEL_LOAD\r\n"));
				goon = WP_ChannelLoad(pchannel, priority, apr);
				break;
			case WS_CHANNEL_FLUSH:
//				TRACE(_T("WS_CHANNEL_FLUSH\r\n"));
				goon = WP_ChannelFlush(pchannel, priority, apr);
				break;
			case WS_CHANNEL_SETPATH:
//				TRACE(_T("WS_CHANNEL_SETPATH\r\n"));
				WP_ChannelSetPath(pchannel, priority, apr);
				break;
			case WS_CHANNEL_READ:
//				TRACE(_T("WS_CHANNEL_READ\r\n"));
				WP_ChannelRead(pchannel, priority, apr);
				break;
			case WS_CHANNEL_MODIFY:
//				TRACE(_T("WS_CHANNEL_MODIFY\r\n"));
				WP_ChannelModify(pchannel, priority, apr);
				break;
			}
		}
		break;
	case WT_SEARCH:
		{
			WP_Search(apwork);

		}
	case WT_UPLOAD:
		{
			WorkUpload* pupload = static_cast<WorkUpload*> (apwork.get());
			WP_Upload(pupload, priority, apr);
		}
		break;
	}

#ifdef _DEBUG
	interval = GetTickCount() - start;
	TRACE(_T("rss db work time = %d\r\n"), interval);
#endif

	if(goon != 0)
	{
		EnterCriticalSection(&cs);
		if(goon == 2) wlist.push_front(apwork);
		else wlist.push_back(apwork);			// default push_back
		LeaveCriticalSection(&cs);
	}

	// 처리 결과 전송
	if(apr.get() != NULL && goon == 0)
		PUSH_RESULT(priority, apr);

	SetWorking(FALSE);
}

//////////////////////////////////////////////////////////////////////
// Result Process
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 *
 * @Description
 * 처리 결과를 반환한다.
 *
 * @Parameters
 * (in auto_ptr<RESULT>&) apresult - 처리결과
 *
 * @Returns
 * (BOOL) 처리결과 반환 여부
 **************************************************************************/
BOOL CRSSDB::GetResult(auto_ptr<RESULT>& apresult)
{
	BOOL result = FALSE;
	for(int i = 0; i < MAX_WORKLISTCNT; ++i)
	{
		if(m_queueResult[i].size() > 0)
		{
			EnterCriticalSection(&m_csOutQueue[i]);
			apresult = m_queueResult[i].back();
			m_queueResult[i].pop_back();
			result = TRUE;
			LeaveCriticalSection(&m_csOutQueue[i]);
			break;;
		}
	}

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 작업 처리결과를 CRSSDB에서 처리할 경우 이 함수를 사용한다.
 **************************************************************************/
void CRSSDB::ProcessRSSResult()
{
	for(int i = 0; i < MAX_WORKLISTCNT; ++i)
	{
		while(m_queueResult[i].size() > 0)
		{
			ResultProcess(m_queueResult[i], m_csOutQueue[i]);
		}
	}
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 이곳에서 RSSDB 작업의 결과를 처리한다.
 * 또는 GetResult를 이용해서 처리 결과를 가져간다.
 *
 * @Parameters
 * (in RESULT_LIST&) rlist - 처리할 결과 목록
 * (in CRITICAL_SECTION&) cs - 결과 목록과 쌍을 이루는 critical section
 **************************************************************************/
void CRSSDB::ResultProcess(RESULT_LIST& rlist, CRITICAL_SECTION& cs)
{
	EnterCriticalSection(&cs);
	switch(rlist.back()->resulttype)
	{
	case RT_NORMAL:
		{
		}
		break;
	case RT_POST:
		{
		}
		break;
	case RT_SEARCH:
		{
		}
		break;
	}

	rlist.pop_back();

	LeaveCriticalSection(&cs);
}

//////////////////////////////////////////////////////////////////////
// Work Thread
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * RSS DB의 데이터를 처리하는 메인 Thread
 * Work Queue를 검사하여 처리할 작업이 있는 경우 WorkProcess 함수를 호출한다.
 **************************************************************************/
DWORD CRSSDB::WorkThread()
{
	TRACE(_T("Work thread Start\r\n"));
	
	DWORD dwWait = 0;
	m_bThread = TRUE;
	CoInitialize(NULL);
	int i;
//	while(m_bThread)
	MSG msg;
	while(::GetMessage(&msg, NULL, 0, 0))
	{
		if(msg.message == WTM_CLOSE)
		{
			break;
		}

		if(!m_bThread) break;

		for(i = 0; i < MAX_WORKLISTCNT && m_bThread ; ++i)
		{
			if(m_queueWork[i].size() > 0)
			{
				WorkProcess(m_queueWork[i], m_csInQueue[i]);
				break;
			}
		}

		for(i = 0; i < MAX_WORKLISTCNT && m_bThread; ++i)
		{
			if(m_queueWork[i].size() > 0)
			{
				Trigger();
				break;
			}
		}
	}

	CoUninitialize();

	TRACE(_T("Work thread End\r\n"));

	return 0;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-29
 *
 * @Description
 * Work Thread 와 Search Thread를 가동한다.
 **************************************************************************/
BOOL CRSSDB::Start()
{
	if(!StartThread())
		return FALSE;
	if(!StartSThread())
	{
		StopThread();
		return FALSE;
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * WorkThread를 실행한다.
 **************************************************************************/
BOOL CRSSDB::StartThread()
{
	if(m_hThread)
	{
		return FALSE;
	}

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) WorkThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	m_hThread = hThread;
	m_dwThreadId = dwThreadID;

	ResumeThread(hThread);

	PostThreadMessage(m_dwThreadId, WTM_NEXT, NULL, NULL);

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-29
 *
 * @Description
 * SearchThread를 실행한다.
 **************************************************************************/
BOOL CRSSDB::StartSThread()
{
	if(m_hSThread)
	{
		return FALSE;
	}

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SearchThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	m_hSThread = hThread;
	m_dwSThreadId = dwThreadID;

	ResumeThread(hThread);

	PostThreadMessage(m_dwSThreadId, WTM_NEXT, NULL, NULL);

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * WorkThread를 소멸한다.
 * WorkQueue와 ResultQueue와 채널맵 그리고 KeywordList를 클리어한다.
 **************************************************************************/
void CRSSDB::StopThread()
{
	// clear queue & list
	for(int i = 0; i < MAX_WORKLISTCNT; ++i)
	{
		EnterCriticalSection(&m_csInQueue[i]);
		m_queueWork[i].clear();
		LeaveCriticalSection(&m_csInQueue[i]);

		EnterCriticalSection(&m_csOutQueue[i]);
		m_queueResult[i].clear();
		LeaveCriticalSection(&m_csOutQueue[i]);
	}

	EnterCriticalSection(&m_csKeywordList);
	m_listKeyword.clear();
	LeaveCriticalSection(&m_csKeywordList);

	// signal stop
	m_bThread = FALSE;
	PostThreadMessage(m_dwThreadId, WTM_CLOSE, NULL, NULL);

	// close thread handle
	if(m_hThread)
	{
		DWORD dwResult = WaitForSingleObject(m_hThread, 5000);

		if(dwResult != WAIT_OBJECT_0)
		{
			CloseHandle(m_hThread);
			Sleep(5000);
		}

		m_hThread = NULL;
	}

	XML_MAP::iterator it;

	for(it = m_mapData.begin(); it != m_mapData.end(); ++it)
	{ 
		if(!(*it).second.ChannelLock()) continue;

		(*it).second.listData.clear();
		(*it).second.listOldData.clear();
		(*it).second.ChannelUnlock();
	}

	// 쓰고싶은 만큼 쓰도록 놔둔다.
//	EnterCriticalSection(&m_csMap);
//	m_mapData.clear();
//	LeaveCriticalSection(&m_csMap);

	m_mapPostMatch.clear();
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-29
 * 
 * @Description
 * SearchThread를 소멸한다.
 * Search Queue를 클리어한다.
 **************************************************************************/
void CRSSDB::StopSThread()
{
	// clear queue & list
	for(int i = 0; i < MAX_SEARCHLISTCNT; ++i)
	{
		EnterCriticalSection(&m_csSearch[i]);
		m_queueSearch[i].clear();
		LeaveCriticalSection(&m_csSearch[i]);
	}

	// signal stop
	m_bSThread = FALSE;

	PostThreadMessage(m_dwSThreadId, WTM_CLOSE, NULL, NULL);

	// close thread handle
	if(m_hSThread)
	{
		DWORD dwResult = WaitForSingleObject(m_hSThread, 1000);

		if(dwResult != WAIT_OBJECT_0)
		{
			CloseHandle(m_hSThread);
		}

		m_hSThread = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// XML Manipulate
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2006-02-27
 *
 * @Description
 * flush를 위한 path를 변경한다.
 *
 * @Parameters
 **************************************************************************/
int CRSSDB::SetPath(CString path)
{
	XML_MAP::iterator it;
	XML_LIST::iterator it2;
	CString filename, fullpath;
	int r;

	if(_T("\\") != path.Right(1)) path = path + _T("\\");

	for(it = m_mapData.begin(); it != m_mapData.end(); ++it)
	{
		for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
		{
			if(!(*it2).XML) continue;
			fullpath = (*it2).XML->GetSavePath();
			r = fullpath.ReverseFind(_T('\\'))+1;
			filename = fullpath.Mid(r);
			fullpath.Format(_T("%s%s"), path, filename);
			(*it2).XML->SetSavePath((LPTSTR) (LPCTSTR) fullpath);

			(*it2).path = fullpath;

			TRACE(_T("new path : %s\r\n"), fullpath);
		}

		for(it2 = (*it).second.listOldData.begin(); it2 != (*it).second.listOldData.end(); ++it2)
		{
			fullpath = (*it2).path;
			r = fullpath.ReverseFind(_T('\\'))+1;
			filename = fullpath.Mid(r);
			fullpath.Format(_T("%s%s"), path, filename);

			(*it2).path = fullpath;
			if((*it2).XML != NULL) (*it2).XML->SetSavePath((LPTSTR) (LPCTSTR) fullpath);

			TRACE(_T("new path : %s\r\n"), fullpath);
		}
	}

	return 1;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 모든 xml의 flush 작업을 실행시킨다.
 *
 * @Parameters
 * (in BOOL) force - 강제로 flush 작업을 실행시킬 경우 사용하는 플래그
 **************************************************************************/
int CRSSDB::FlushData(BOOL force, BOOL& keepon)
{
	keepon = FALSE;
	if(force)
	{
		m_bLastFlush = TRUE;
	}

	// 포스트가 초기화 되지 않았다면 FLUSH 를 수행하지 않는다.
	if(!IsPostInit()) return 0;

	if(!m_bFlush) return 0;

	XML_MAP::iterator it;
	XML_LIST::iterator it2;
	COleDateTime t = COleDateTime::GetCurrentTime();
	COleDateTimeSpan s1, s2;
	BOOL bAutoDelete = theApp.m_spLP->GetAutoDelete();
	int cnt = 0;

	s1.SetDateTimeSpan(0, 0, 0, INTERVAL_FLUSH);
	s2.SetDateTimeSpan(0, 0, 0, INTERVAL_OLDUSE);
//	s1.SetDateTimeSpan(0, 0, 0, 60);
//	s2.SetDateTimeSpan(0, 0, 0, 60);

	for(it = m_mapData.begin(); it != m_mapData.end(); ++it)
	{
		// 시간이 많이 걸리는 작업은 종료를 위해서 중간에 끊을 수 있다.
		if(!force && !m_bThread) break;

		// 채널의 종류가 검색이고
		// 검색 기록 플래그가 성립되어 있지 않다면 기록을 남기지 않는다.
		if((*it).second.type == CT_SEARCH || (*it).second.type == CT_LSEARCH)
		{
			if(!(*it).second.bsearchlog) continue;
		}

		if(!(*it).second.ChannelLock()) continue;

		if((*it).second.flush)
		{
			for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
			{
				UpdateXMLChannelData((*it2));
				FLUSH((*it2), t, s1, force);
			}
			++cnt;

/*			for(it2 = (*it).second.listOldData.begin(); it2 != (*it).second.listOldData.end(); ++it2)
			{
				FLUSH((*it2), t, s1, force);
				++cnt;
			}
*/
			(*it).second.flush = FALSE;
		}

		// 스크랩과 검색은 old data 처리를 하지 않는다.
		if((*it).second.type == CT_SCRAP 
			|| (*it).second.type == CT_SEARCH
			|| (*it).second.type == CT_LSEARCH
			) 
		{
			(*it).second.ChannelUnlock();
			continue;
		}

/*		if((*it).second.oldused && (*it).second.dtOldUseTime + s2 > t)
		{
			for(it2 = (*it).second.listOldData.begin(); it2 != (*it).second.listOldData.end(); ++it2)
			{
				if(DELETE_XMLDATA((*it2))) ++cnt;
			}

			(*it).second.oldused = FALSE;
		}
*/
		// 강제 flush (최종 flush)가 아닌 경우 한번 작업에 flush 하는 채널의 개수의 한계를 둔다.
		if(cnt >= MAX_FLUSHLIMIT)
		{
			keepon = TRUE;
			(*it).second.ChannelUnlock();
			return cnt;
		}

		// 최종 FLUSH할 경우에만 listData -> listOldData로 옮긴다.
		if(!force)
		{
			(*it).second.ChannelUnlock();
			continue;
		}

		BOOL backup = FALSE;
		if((*it).second.keepcnt != 0)
			backup = ( (*it).second.keepcnt < (*it).second.postcnt );
		else
			backup = ( MAX_CHANNELITEMCNT > (*it).second.postcnt );

		if(backup)
		{
			// 자동 삭제.... 그러나 실제로 데이터 처리는 xml의 load 정보를 변경한다.
			if(bAutoDelete)
				DELETE_XMLDATA((*it).second.listData.back());

/*
			// memory data 에서 old data로 옮김
			(*it).second.listOldData.push_front((*it).second.listData.back());

			if(DELETE_XMLDATA((*it).second.listOldData.front()))
			{
				(*it).second.listData.pop_front();
				(*it).second.unread -= unread;
				(*it).second.postcnt -= postcnt;

				if(unread)
				{
					// 구독관리에 포스트 개수를 알려준다.
					SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
				}

				if(postcnt)
				{
					SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, (*it).first, (*it).second.postcnt);
				}

			}

//			(*it).second.dtDivid = (*it).second.listData.back().term.from;
			BOOL bRemoved = FALSE;

			while((*it).second.listOldData.size() > MAX_OLDLISTCNT)
			{
				DELETE_XMLDATA((*it).second.listOldData.back());
				(*it).second.listOldData.pop_back();

				bRemoved = TRUE;
			}

			if(bRemoved && (*it).second.listOldData.size() > 0)
			{
				// 항상 최초의 데이터는 시작점을 0으로 잡는다.
				(*it).second.listOldData.back().term.from.m_dt = 0;

				// XML 파일을 열어서 from 시점을 갱신한다.
				if(UpdateXMLChannelData((*it).second.listOldData.back()))
				{
					(*it).second.dtOldUseTime = COleDateTime::GetCurrentTime();
					(*it).second.oldused = TRUE;
				}

			}
			*/
		}

		(*it).second.ChannelUnlock();
	}

	m_bFlush = FALSE;

	return cnt;
}



/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * m_listData 의 데이터와 
 * m_listOldData 의 데이터를 시간순으로 재배열 시킨다.
 **************************************************************************/
void CRSSDB::ReArrangeData()
{
	greater<RSS_XMLDATA> g;

	XML_MAP::iterator it = m_mapData.begin();

	for(; it != m_mapData.end(); ++it)
	{
		if(!(*it).second.ChannelLock()) continue;

		(*it).second.listData.sort(g);
		(*it).second.listOldData.sort(g);

		RemakeListterm((*it).second.listData);
		RemakeListterm((*it).second.listOldData);
		
		if((*it).second.listData.size() > 0)
		{
			(*it).second.dtDivid = (*it).second.listData.back().term.from;
			// 채널의 종류를 기록한다.
			(*it).second.type = (*it).second.listData.back().type;
		}
		else if((*it).second.listOldData.size() > 0)
		{
			(*it).second.type = (*it).second.listOldData.back().type;
		}

		(*it).second.ChannelUnlock();
	}
}



/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * xml파일을 읽어와 데이터 리스트에 입력한다.
 *
 * @Parameters
 * (in CString) filename - 읽어올 파일명
 *
 * @Returns
 * (BOOL) XML 로드 성공여부
 **************************************************************************/
BOOL CRSSDB::LoadXMLFile(CString filename, int& posts)
{
	CEzXMLParser tmpxml;

	if(!tmpxml.LoadXML((LPTSTR) (LPCTSTR) filename))
		return FALSE;

	IXMLDOMNodeListPtr	nodelist = NULL;
	IXMLDOMNodePtr		node = NULL;
	CString version = _T("")
		, termfrom = _T("")
		, termto = _T("")
		, termload = _T("")
		, channelid = _T("")
		, channeltype = _T("")
		, guid = _T("")
		;

	// check fish version
	nodelist = tmpxml.SearchNodes(XML_XPATH_CHANNEL);

	if(nodelist == NULL || nodelist->length == 0)
		return FALSE;

	node = nodelist->item[0];

	version = GetNamedNodeText(XML_VERSION, &tmpxml, node);

	TCHAR* stopstring;
	float fversion = _tcstod ((LPTSTR) (LPCTSTR) version, &stopstring);
	float fv2  = _tcstod (FISHPOST_VERSION, &stopstring);

	// 신 버전 비교
//	if(fversion > fv2)
//	{
//		return FALSE;
//	}

	// get from time
	termfrom = GetNamedNodeText(XML_FROM, &tmpxml, node);
	termto = GetNamedNodeText(XML_TO, &tmpxml, node);
	COleDateTime timefrom, timeto;

	ConvertStringtoTime(termfrom, timefrom);
	ConvertStringtoTime(termto, timeto);

	if(timefrom.m_dt == 0 && timeto.m_dt == 0)
	{
		return FALSE;
	}

	// memory load 여부를 판단함
	termload = GetNamedNodeText(XML_LOAD, &tmpxml, node);

	int itermload = _ttoi((LPTSTR) (LPCTSTR) termload);

	// XML 파일이 가지고 있는 채널의 id
	channelid = GetNamedNodeText(XML_CHANNELID, &tmpxml, node);

	int ichannelid = _ttoi((LPTSTR) (LPCTSTR) channelid);

	if(channelid.IsEmpty() || ichannelid < 0)
	{
		return FALSE;
	}

	// 채널의 종류
	channeltype = GetNamedNodeText(XML_CHANNELTYPE, &tmpxml, node);

	int ichanneltype = _ttoi((LPTSTR) (LPCTSTR) channeltype);

	TERM t;
	t.from = timefrom;
	t.to = timeto;

	CEzXMLParser* pxml = NULL;

	XML_MAP::iterator it = m_mapData.find(ichannelid);

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(ichanneltype != (*it).second.type)
	{
		return FALSE;
	}

	guid = GetNamedNodeText(XML_GUID, &tmpxml, node);
//	TRACE(_T("guid check : read [%s], memory[%s]\r\n"), guid, (*it).second.guid);
	if(guid != (*it).second.guid)
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	// Post MATCH를 등록하며 최종 Post ID를 찾아낸다.
	int unread, postcnt;
	int ilastpostid = AddPostMatch(&tmpxml, ichannelid, unread, postcnt, (*it).second);

	(*it).second.dtFlush = COleDateTime::GetCurrentTime();

	if(postcnt > 0)
	{
		if(itermload != 0)
		{
			pxml = new CEzXMLParser;

			// data detach and attach
			pxml->SetSavePath((LPTSTR) (LPCTSTR) filename);
			pxml->m_pXMLDoc = tmpxml.m_pXMLDoc;
			tmpxml.m_pXMLDoc = NULL;

			// 채널의 읽지 않은 포스트의 개수 설정
			(*it).second.unread += unread;
			(*it).second.postcnt += postcnt;


			(*it).second.listData.push_front(RSS_XMLDATA(t, filename, pxml, ichanneltype));
			(*it).second.listData.front().load = itermload;

			// 구간 채널 정보의 읽지 않은 포스트의 개수 설정
			(*it).second.listData.front().unread = unread;
			(*it).second.listData.front().postcnt = postcnt;

			// 구독관리에 포스트 개수를 알려준다.

			if(m_bThread)
			{
				int id = (*it).first;
				int unread = (*it).second.unread;
				int postcnt = (*it).second.postcnt;

				if(unread)
					::SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, id, unread);

				if(postcnt)
					::SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, id, postcnt);
			}
		}
		else
		{
			// old data의 포스트의 읽지 않음 개수는 처리하지 않는다.

			(*it).second.listOldData.push_front(RSS_XMLDATA(t, filename, NULL, ichanneltype));
		}

		// 사용되는 post sequence를 갱신한다.
		if(m_npostseq < ilastpostid)
			m_npostseq = ilastpostid;

	}

	(*it).second.ChannelUnlock();

	if(postcnt == 0) return FALSE;
	posts = postcnt;

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * Description
 * 지정된 디렉토리로 부터 데이터를 읽어와서 XML list에 넣는다.
 * 성공하면 정상적으로 읽어온 xml 개수를 반환한다.
 *
 * @Returns
 * (int) 다음 시작지점
 **************************************************************************/
int CRSSDB::LoadXMLData(int start, int& nread, BOOL& keepon)
{
	CFileFind f;

	keepon = FALSE;

	CString	findname;
	CString	xmlpath;

	TRACE(_T("Start Load XML Data\r\n"));

//	findname.Format(_T("%s*.xml"), PATH_RSSDB);
	findname.Format(_T("%s*.xml"), theApp.m_spLP->GetPostDataPath());

	BOOL bNext = f.FindFile(findname);
	if(!bNext) 
	{
		SetPostInit(TRUE);
		return 0;
	}

	keepon = TRUE;

	int cnt = 0;
	int ntry = 0;
	int step = 0;
	int xmlpostcnts = 0;

	while(bNext && m_bThread)
	{
		bNext = f.FindNextFile();
		if(f.IsDirectory() || f.IsDots()) continue;

		if(step++ < start) continue;

		xmlpath = f.GetFilePath();
//			TRACE(_T("XML File Name : %s \r\n"), xmlpath);
		if(LoadXMLFile(xmlpath, xmlpostcnts)) ++cnt;
		else DeleteFile(xmlpath);

		if(step - start >= MAX_LOADXML) break;
	}

	nread = cnt;

	if(!bNext)
	{
		ReArrangeData();

		TRACE(_T("End Load XML Data\r\n"));

		SetPostInit(TRUE);

		keepon = FALSE;
	}

//    AfxMessageBox(_T("Load XML End"));

	return step;
}

//////////////////////////////////////////////////////////////////////
// XML Data manipulate
//////////////////////////////////////////////////////////////////////


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 * modified 2006-03-20 :: parameter channelid 추가 by moonknit
 *
 * @Description
 * 메모리 관리중인 데이터에서 postid 값을 만족하는 post 정보를 제거한다.
 *
 * @Parameters
 * (in int) postid - 삭제할 대상의 postid
 * (in DWORD) flag - 삭제 플래그
 * (in int) channelid - DEFAULT 0, 채널 아이디를 직접 넣어주면 m_mapPostMatch를 사용하는
 *					부담을 덜어준다.
 * @Returns
 * (BOOL) 삭제 성공여부
 **************************************************************************/
BOOL CRSSDB::RemovePostByPostID(int postid, DWORD flag, int channelid)
{
	MAP_POSTMATCH::iterator pit;

	int chid = channelid;
	if(chid == 0)
	{
		pit = m_mapPostMatch.find(postid);
		if(pit == m_mapPostMatch.end())
		{
			return FALSE;
		}
		chid = (*pit).second;
	}

	XML_MAP::iterator it = m_mapData.find(chid);
	XML_LIST::iterator it2;
	BOOL result = FALSE;
	BOOL xmlfind = FALSE;
	BOOL decunread = FALSE;

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	// 메모리에서 찾는다.
	BOOL bopen = FALSE;
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if(RemoveXMLItem(postid, (*it2), bopen, decunread, (*it).second))
		{
			result = TRUE;
			(*it2).flush = TRUE;
			break;
		}
	}

	if(result)
	{
		// 구독관리에 포스트 개수를 알려준다.
		if(decunread)
		{
			--((*it).second.unread);
			--((*it2).unread);
			SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		}

		--((*it).second.postcnt);
		--((*it2).postcnt);
		SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, (*it).first, (*it).second.postcnt);

		(*it).second.flush = TRUE;
		m_bFlush = TRUE;
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 메모리상의 채널의 포스트만 삭제한다.
 *
 * @Parameters
 * (in int) channelid - 삭제할 채널 id
 *
 * @Returns
 * (BOOL) 삭제 성공여부 
 **************************************************************************/
BOOL CRSSDB::RemovePostByChannelID(int channelid)
{
	BOOL result = FALSE;
	XML_MAP::iterator it = m_mapData.find(channelid);
	XML_LIST::reverse_iterator rit;

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	if((*it).second.listData.size() == 0) 
	{
		(*it).second.ChannelUnlock();
		return FALSE;
	}

	// 메모리에서 찾는다.
	// 찾아서 삭제한다.
//	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)

	result = TRUE;
	do
	{
		// 제일 뒷 항목을 꺼낸다.
		rit = (*it).second.listData.rbegin();

		if((*rit).XML != NULL) RemoveXMLAllItem((*rit).XML.get(), (*it).second);

		// 마지막 XML은 남겨둔다.
		if((*it).second.listData.size() == 1)
		{
			(*rit).flush = TRUE;
			(*rit).unread = 0;
			(*rit).postcnt = 0;
			break;
		}

		// XML 파일을 지운다.
		DeleteFile((*rit).path);

		// 제일 뒷 항목을 제거한다.
		(*it).second.listData.pop_back();
	} while(TRUE);

	if(result)
	{
		(*it).second.unread = 0;
		(*it).second.postcnt = 0;

		(*it).second.flush = TRUE;
		m_bFlush = TRUE;

		// 구독관리에 포스트 개수를 알려준다.
		SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, (*it).first, (*it).second.postcnt);
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * 채널을 삭제한다.
 *
 * @Parameters
 * (in int) channelid - 삭제할 대상 채널 아이디
 *
 * @Returns
 * (BOOL) 삭제 성공 여부
 **************************************************************************/
BOOL CRSSDB::RemoveChannel(int channelid)
{
	XML_MAP::iterator it = m_mapData.find(channelid);

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	// 현재 메모리 관리중인 데이터 해제

	XML_LIST::iterator it2 = (*it).second.listData.begin();

	for(;it2 != (*it).second.listData.end(); ++it2)
	{
		DeleteFile((*it2).path);
	}

	(*it).second.listData.clear();

	// 오래된 데이터 해제
	it2 = (*it).second.listOldData.begin();

	for(;it2 != (*it).second.listOldData.end(); ++it2)
	{
		DeleteFile((*it2).path);
	}

	(*it).second.listOldData.clear();

	// 등록된 키워드를 없앤다.
	if((*it).second.type == CT_KEYWORD)
	{
		RemoveKeyword(channelid);
	}

	// POST MATCH 정보를 해제한다.
	MAP_POSTMATCH::iterator it3, tit;
	for(it3 = m_mapPostMatch.begin(); it3 != m_mapPostMatch.end(); )
	{
		tit = it3;
		++it3;
		if((*tit).second == channelid)
		{
			m_mapPostMatch.erase(tit);
		}
	}

	(*it).second.mapGuid.clear();

	(*it).second.ChannelUnlock();

	EnterCriticalSection(&m_csMap);
	m_mapData.erase(channelid);
	LeaveCriticalSection(&m_csMap);

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 채널을 추가한다.
 *
 * @Parameters
 * (in int) postid - 스크랩될 포스트의 id
 * (in int) channelid - 스크랩할 채널의 id
 *
 * @Returns
 * (BOOL) 스크랩
 **************************************************************************/
BOOL CRSSDB::ScrapPostByPostID(int postid, int channelid)
{
	POSTITEM item;
	item.postid = postid;
	if(GetPostData(item))
	{
		item.channelid = channelid;
		item.read = FALSE;
		if(AddPost(item))
		{
			return TRUE;
		}
	}

	return FALSE;
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 * modifiedy 2006-03-10 :: keepcnt, keepdays를 추가함 by moonknit
 *
 * @Description
 * 채널을 추가한다.
 *
 * @Parameters
 * (in int) channelid - 추가할 channel id
 * (in int) chtype - 추가할 채널의 종류
 * (in CString) text - guid
 * (in CString) keyword - keyword (chtype == CT_KEYWORD 인 경우에만 유효하다.)
 * (in int) keepcnt - 보관할 최대 포스트의 개수
 * (in int) keepdays - 최대 보관할 기간
 *
 * @Returns
 * (BOOL) 추가 성공 여부
 **************************************************************************/
BOOL CRSSDB::AddChannel(int channelid, int chtype, int exData, CString text
						, CString keyword, int keepcnt, int keepdays)
{
	XML_MAP::iterator it = m_mapData.find(channelid);

	if(it == m_mapData.end())
	{
		if(chtype == CT_NONE)
			return FALSE;

		EnterCriticalSection(&m_csMap);
		it = MakeNewChannel(m_mapData, channelid, chtype);
		LeaveCriticalSection(&m_csMap);

		if(it == m_mapData.end())
			return FALSE;

		// 구독 채널의 경우에만 언어코드를 넣는다.
		// 그 외의 채널은 UTF-8을 사용한다.
		(*it).second.guid = text;
		(*it).second.bns = (exData & CHANNEL_CREATE_BNS) ? TRUE : FALSE;
		(*it).second.bsearchlog = (exData & CHANNEL_CREATE_SEARCHLOG) ? TRUE : FALSE;
		(*it).second.exFlag = (DWORD) exData;
		(*it).second.keepcnt = keepcnt;
		(*it).second.keepdays = keepdays;
	}
	else
		return FALSE;

	if(chtype == CT_KEYWORD)
	{
		AddKeyword(channelid, keyword);
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 키워드 필터링 작업을 수행한다.
 **************************************************************************/
void CRSSDB::Filtering(POSTITEM& item)
{
	KEYWORD_LIST::iterator it;

	EnterCriticalSection(&m_csKeywordList);
	for(it = m_listKeyword.begin(); it != m_listKeyword.end(); ++it)
	{
		if(CheckKeyword(item, (*it).keyword))
		{
			item.channelid = (*it).channelid;
			AddPost(item);
		}
	}
	LeaveCriticalSection(&m_csKeywordList);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 * updated 2005-12-06 :: AddItemtoXML 호출시 chtype 전달 by moonknit
 * updated 2006-01-24 :: CHECK_GUIDBYREGION 조건 절 추가 by moonknit
 * modified 2006-03-10 :: CHECK_GUIDBYREGION 조건 절 조건 제거 by moonknit
 *
 * @Description
 * 새로운 Post를 추가한다.
 * 채널이 subscribe인 경우만 filtering 작업을 수행한다.
 *
 * @Parameters
 * (in POSTITEM&) item - 추가할 post 정보
 *
 * @Returns
 * (BOOL) 포스트 추가 성공여부
 **************************************************************************/
BOOL CRSSDB::AddPost(POSTITEM& item)
{
	BOOL bnew = FALSE;			// 새 XML 생성 플래그
	XML_MAP::iterator it = m_mapData.find(item.channelid);
	XML_LIST::iterator it2;
	BOOL result = FALSE;
	
	CEzXMLParser* pxml = NULL;

	if(it == m_mapData.end())
	{
		// 채널이 존재하지 않으므로 새로운 Post를 추가할 수 없다.
		return FALSE;
	}
	int chtype = (*it).second.type;

	if(!(*it).second.ChannelLock()) return FALSE;

	// 구독 채널은 item에 bns 정보를 추가한다.
	if(CT_SUBSCRIBE == chtype)
		item.bns = (*it).second.bns;

	IXMLDOMNodePtr nodeptr;
	
	MAP_GUIDMATCH::iterator git;

	git = (*it).second.mapGuid.find(item.guid);
	if(git != (*it).second.mapGuid.end())
	{
		(*it).second.ChannelUnlock();
		return FALSE;
	}

/*
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if(!(*it2).XML) continue;

		if(CheckItemByGUID((*it2).XML.get(), item.guid, nodeptr))
		{
			(*it).second.ChannelUnlock();
			return FALSE;
		}
	}
*/
	// 데이터의 정체를 파악한다.
	if((*it).second.listData.size() == 0)
		bnew = TRUE;
	else if((*it).second.listData.front().postcnt > MAX_UNITITEMCNT)
		bnew = TRUE;
	else if(! ( (*it).second.listData.front().XML ) )
		bnew = TRUE;
	else
	{
		it2 = (*it).second.listData.begin();
		pxml = (*it2).XML.get();
	}
/*
	if((*it).second.dtDivid < item.pubdate || chtype != CT_SUBSCRIBE)
	{
		BOOL bfind = FALSE;
		if(chtype == CT_SUBSCRIBE)
			bfind = FindXML((*it).second.listData, item.pubdate, it2);
		else
		{
			it2 = (*it).second.listData.begin();
			bfind = (it2 != (*it).second.listData.end()) ? TRUE : FALSE;
		}

		if(bfind)
		{
			pxml = (*it2).XML.get();
			// 찾아온 목록이 가장 최신 메모리 데이터이고 item개수가 일정 이상이면 새로운 XML을 생성한다.
			if(it2 == (*it).second.listData.begin())
			{
				int itemcnt;
				itemcnt = GetPostCnt(pxml);

				if(
//					chtype == CT_SUBSCRIBE && 
					itemcnt > MAX_UNITITEMCNT)
				{
					bnew = TRUE;
				}
			}
		}
		else if((*it).second.listData.size() == 0)
		{
			bnew = TRUE;
		}
		else
		{
			// commented by moonknit 2005-10-27
			// listData에 XML 정보가 들어 있음에도 불구하고
			// item의 pubdate가 속하는 XML을 찾을 수 없음

			// itemData에 속하는 객체의 시간 정보를 수정하도록한다.

			(*it).second.ChannelUnlock();
			return FALSE;
		}
	}
	*/

	if(bnew)
	{
		TERM t;
		BOOL isfirst = TRUE;

		pxml = MakeDefaultChannelXML(t, item.channelid, m_npostseq, chtype, (*it).second.guid);
		if(!pxml) 
		{
			(*it).second.ChannelUnlock();
			return FALSE;
		}


		// 기존 메모리 데이터의 가장 최근 정보중 기간 정보를 갱신한다.
		if((*it).second.listData.size() > 0)
		{

			if((*it).second.listData.front().term.to.m_dt == 0)
			{
				(*it).second.listData.front().term.to = COleDateTime::GetCurrentTime();
			}

			t.from = (*it).second.listData.front().term.to;
			
			isfirst = FALSE;

			CString stime = ConvertTimetoString(t.from);
		}

		(*it).second.listData.push_front(RSS_XMLDATA(t, pxml, chtype));

		(it2) = (*it).second.listData.begin();
/*
		// 목록의 포스트가 하나뿐이거나 
		// 새로 등록할 포스트가 새로 생성된 XML데이터의 기간에 포함된다면 
		// 등록을 새로 생성된 XML데이터에 추가한다.
		// 그렇지 않다면 기존의 최신 XML데이터에 추가한다.
		if(isfirst || item.pubdate > t.from)
		{
			it2 = (*it).second.listData.begin();
		}
		else
		{
			FindXML((*it).second.listData, item.pubdate, it2);
			pxml = (*it2).XML.get();
		}
		*/

	}

	item.postid = m_npostseq + 1;
	BOOL bAdd = FALSE;

	if(AddItemtoXML(pxml, item, chtype))
	{
		bAdd = TRUE;
		m_npostseq = item.postid;

		// 채널 구간 정보의 읽지 않는 포스트의 개수 증가
		++((*it2).unread);
		++((*it2).postcnt);

		// 기록 시간 정보 갱신
		COleDateTimeSpan s;
		s.SetDateTimeSpan(0, 0, 0, 1);

		(*it2).term.to = item.pubdate + s;

		(*it2).flush = TRUE;
		(*it).second.flush = TRUE;
		m_bFlush = TRUE;

		// 채널의 읽지 않는 포스트의 개수 증가
//		if(!item.read)
		++((*it).second.unread);
		++((*it).second.postcnt);

		// 구독관리에 포스트 개수를 알려준다.
		SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, (*it).first, (*it).second.postcnt);

		m_mapPostMatch.insert(MAP_POSTMATCH::value_type(item.postid, item.channelid));
		(*it).second.mapGuid.insert(MAP_GUIDMATCH::value_type(item.guid, item.postid));

		result = TRUE;
	}

	(*it).second.ChannelUnlock();

	// AddPost를 호출하므로 Filtering 호출 전에 ChannelUnlock을 수행해야 한다.
	if(chtype == CT_SUBSCRIBE && bAdd)
		Filtering(item);			

	return result;
}

/*
 * created 2005-10-xx
 * updated 2005-12-06 :: GetXMLItem 호출시 채널 타입을 이용한다. by moonknit
 
 * [Parameters]
 * item -	(in) item.postid
			(out) except upper two data
 
 */
BOOL CRSSDB::GetPostData(POSTITEM& item, DWORD flag)
{
	MAP_POSTMATCH::iterator pit;
	int chid = item.channelid;
	if(chid == 0)
	{
		pit = m_mapPostMatch.find(item.postid);
		if(pit == m_mapPostMatch.end())
		{
			return FALSE;
		}

		chid = (*pit).second;
	}

	BOOL result = FALSE;
	BOOL xmlfind = FALSE;

	XML_MAP::iterator it = m_mapData.find(chid);
	XML_LIST::iterator it2;

	if(it == m_mapData.end()) return FALSE;

	if(!(*it).second.ChannelLock()) return FALSE;

	BOOL bopen;

	// 메모리에서 찾는다.
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if(GetXMLItem(item, (*it).second.type, 0, (*it2), bopen, flag))
		{
			result = TRUE;
			break;
		}
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 요청한 포스트의 내용을 반환한다.
 **************************************************************************/
BOOL CRSSDB::GetPostListData(POST_LIST* plist, DWORD flag)
{
	BOOL result = TRUE;

	POST_LIST::iterator it;

	for(it = plist->begin(); it != plist->end(); ++it)
	{
		if(!GetPostData((*it), flag))
			result = FALSE;
	}

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 * updated 2005-12-06 :: GetXMLItemList 호출시 채널 종류를 넘김 by moonknit 
 *
 * @Description
 * 채널의 모든 포스트 정보를 반환한다.
 **************************************************************************/
BOOL CRSSDB::GetChannelPostData(int channelid, DWORD flag, auto_ptr<POST_LIST>& aplistpost)
{
	XML_MAP::iterator it = m_mapData.find(channelid);
	XML_LIST::reverse_iterator it2;
	BOOL result = FALSE;

	if(it == m_mapData.end()) return FALSE;

	if(!(*it).second.ChannelLock()) return FALSE;

	aplistpost->clear();

	BOOL bopen;

	for(it2 = (*it).second.listData.rbegin(); it2 != (*it).second.listData.rend(); ++ it2)
	{
		if(GetXMLItemList(aplistpost, channelid, (*it).second.type, (*it2), bopen, flag))
		{
			result = TRUE;
		}
	}

#ifdef USE_OLDFILE_DOWORK
	for(it2 = (*it).second.listOldData.rbegin(); it2 != (*it).second.listOldData.rend(); ++ it2)
	{
		if(flag & FISH_USEOLDDATA)
		{
			result = GetXMLItemList(aplistpost, channelid, (*it).second.type, (*it2), bopen, flag);

			if(bopen)
			{
				(*it).second.dtOldUseTime = COleDateTime::GetCurrentTime();
				(*it).second.oldused = TRUE;
			}
		}
	}
#endif

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널의 모든 포스트의 읽음 여부를 처리한다.
 **************************************************************************/
BOOL CRSSDB::UpdateChannelDataRead(int channelid, BOOL read)
{
	XML_MAP::iterator it = m_mapData.find(channelid);
	XML_LIST::iterator it2;
	BOOL result = FALSE;
	int totalunread = 0;

	if(it == m_mapData.end()) return FALSE;

	if(!(*it).second.ChannelLock()) return FALSE;

	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++ it2)
	{
		if(!(*it2).XML)
		{
#ifdef FISH_USEOLDDATA
			if(UpdateXMLItemListRead((*it2), read))
			{
				(*it).second.dtOldUseTime = COleDateTime::GetCurrentTime();
				(*it).second.oldused = TRUE;
				result = TRUE;
				(*it2).flush = TRUE;
			}
#endif
		}
		else
		{
			if(UpdateXMLItemListRead((*it2).XML.get(), read))
			{
				result = TRUE;
				(*it2).flush = TRUE;
			}
		}

		if(read)
		{
			(*it2).unread = 0;
		}
		else if((*it2).XML != NULL)
		{
			totalunread += (*it2).unread = GetPostCnt((*it2).XML.get());
		}
	}

	if(result)
	{
		int oldunread = (*it).second.unread;
		if(read)
		{
			(*it).second.unread = 0;
		}
		else
		{
			(*it).second.unread = totalunread;
		}

		if(oldunread != (*it).second.unread)
		{
			// 구독관리에 포스트 개수를 알려준다.
			SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		}

		(*it).second.flush = TRUE;
		m_bFlush = TRUE;
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-19
 *
 * @Description
 * 채널의 정보를 갱신할 때 사용한다.
 *
 * @Parameters
 * (in int) channelid - 갱신할 대상 채널의 아이디
 * (in int) dest - 갱신할 대상 아이템
				CDM_SEARCHLOG : bsearchlog 갱신
 * (in int) flag - 갱신할 내용
 **************************************************************************/
BOOL CRSSDB::ModifyChannel(int channelid, int dest, int flag)
{
	XML_MAP::iterator it = m_mapData.find(channelid);
	BOOL result = FALSE;

	if(it == m_mapData.end()) return FALSE;

	if(!(*it).second.ChannelLock()) return FALSE;

	switch(dest)
	{
	case CMD_SEARCHLOG:
		(*it).second.bsearchlog = static_cast<BOOL> (flag);
		break;
	case CMD_BNSFLAG:
		(*it).second.bns = static_cast<BOOL> (flag);
		break;
	}


	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 포스트의 정보를 갱신한다.
 **************************************************************************/
BOOL CRSSDB::UpdatePostData(POSTITEM& item, DWORD flag)
{
	MAP_POSTMATCH::iterator pit;
	int chid = item.channelid;
	if(chid == 0)
	{
		pit = m_mapPostMatch.find(item.postid);
		if(pit == m_mapPostMatch.end())
		{
			return FALSE;
		}
		else
			chid = (*pit).second;
	}

	BOOL result = FALSE;
	BOOL xmlfind = FALSE;
	XML_MAP::iterator it = m_mapData.find(chid);
	XML_LIST::iterator it2;
	int unread = 0;

	if(it == m_mapData.end()) 
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;
	
	BOOL bopen;

	// 메모리에서 찾는다.
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if(UpdateXMLItem(item, (*it).second.type, (*it2), bopen, unread, flag))
		{
			result = TRUE;
			break;
		}
	}

	if(result)
	{
		if(unread)
		{
			(*it).second.unread += unread;
			(*it2).unread += unread;

			// 구독관리에 포스트 개수를 알려준다.
			SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		}

		(*it2).flush = TRUE;
		(*it).second.flush = TRUE;
		m_bFlush = TRUE;
	}

	(*it).second.ChannelUnlock();

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널을 기록한다.
 **************************************************************************/
BOOL CRSSDB::SaveChannel(int channelid)
{
	TRACE(_T("SaveChannel\r\n"));
	XML_MAP::iterator it = m_mapData.find(channelid);
	XML_LIST::iterator it2;
	BOOL result = TRUE;

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	// save all xml of memory data
	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)
	{
		if((*it2).XML != NULL)
		{
			// channeld data 갱신
			UpdateXMLChannelData((*it2));

			if(!(*it2).XML->SaveXML())
			{
				result = FALSE;
			}
		}
		else
			result = FALSE;
	}

	(*it).second.ChannelUnlock();

	return result;
}

//////////////////////////////////////////////////////////////////////
// key word filtering
//////////////////////////////////////////////////////////////////////
/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * filtering keyword를 추가한다.
 **************************************************************************/
BOOL CRSSDB::AddKeyword(int channelid, CString keyword)
{
	KEYWORD_LIST::iterator it;
	BOOL bskip = FALSE;
	BOOL result = FALSE;

	EnterCriticalSection(&m_csKeywordList);
	if(channelid > 0 && !keyword.IsEmpty())
	{
		for(it = m_listKeyword.begin(); it != m_listKeyword.end(); ++it)
		{
			if((*it).channelid == channelid)
			{
				bskip = TRUE;
				break;
			}
		}

		if(!bskip)
		{
			m_listKeyword.push_back(KEYWORD_FILTER(channelid, keyword));
			result = TRUE;
		}
	}
	LeaveCriticalSection(&m_csKeywordList);

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * filtering keyword를 제거한다.
 **************************************************************************/
void CRSSDB::RemoveKeyword(int channelid)
{
	KEYWORD_LIST::iterator it;

	if(channelid < 0) return;

	for(it = m_listKeyword.begin(); it != m_listKeyword.end(); ++it)
	{
		if((*it).channelid == channelid)
		{
			m_listKeyword.erase(it);
			return;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////
// XML POST ITEM search
//////////////////////////////////////////////////////////////////////

struct SEARCH_TAB
{
	int					t;
	int					p;		// priority
};

struct SEARCH_TAB shtab[] = 
{
	{NET_SEARCH, PTS_NET_SEARCH},
	{LOCAL_SEARCH, PTS_LOCAL_SEARCH},
	{LONGTERM_SEARCH, PTS_LONGTERM_SEARCH},
	{GRAPH_SEARCH, PTS_GRAPH_SEARCH}
};

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-01
 *
 * @Description
 * 검색 종류에 따른 우선 순위를 반환한다.
 *
 * @Parameters
 * (in SEARCH_TYPE) type - 검색 종류
 *
 * @Return
 * (int) - 우선 순위
 **************************************************************************/
int GetSearchPriority(SEARCH_TYPE type)
{
	for(int i = 0; ; ++i)
	{
		if(shtab[i].t == SEARH_TYPECNT) break;

		if(shtab[i].t == type) return shtab[i].p;

	}
	return 0;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-01
 *
 * @Description
 * Search 작업을 수행하도록 요청한다.
 **************************************************************************/
void CRSSDB::TriggerSearch()
{
#ifdef FISH_TEST
	for(int i = 0; i < MAX_SEARCHLISTCNT; ++i)
	{
		while(m_queueSearch[i].size() > 0)
		{
			SearchProcess(m_queueSearch[i], m_csSearch[i]);
		}
	}
#else
	TRACE(_T("Trigger Search\r\n"));
//	SetEvent(m_hSThreadEvent);
	Sleep(1);
	PostThreadMessage(m_dwSThreadId, WTM_NEXT, NULL, NULL);
#endif
}

/**************************************************************************
 * function SearchXML_Graph
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * XML에서 주어진 검색을 수행한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) 검색을 수행할 포인터
 * (in/out WorkSearch*) psearch - (WorkSearch*) 검색 내용과 검색 결과를 담은 구조체 포인터
 * (in int) ctype - 채널의 종류 (채널의 종류에 따라 결과의 내용을 다르게 처리한다.)
 *
 * @Return
 * (BOOL) - 검색 성공여부
 **************************************************************************/
BOOL SearchXML_Graph(CEzXMLParser* pxml, WorkSearch *psearch, int ctype)
{
	if(!pxml 
		|| !psearch 
		|| !(psearch->apresult->aplistpost.get())
		|| ctype == CT_SEARCH				// 검색 결과의 재검색을 막는다.
		|| ctype == CT_LSEARCH				// 검색 결과의 재검색을 막는다.
		)
		return FALSE;

	CString strxpath;
	CString strtmp;
	CString keylowered;
	keylowered = psearch->keyword;
	keylowered.MakeLower();

	strxpath = XML_XPATH_ITEM_ALL;
	strxpath += "[";
	// 제목에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 작자에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 내용에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_DESC, keylowered);
	strxpath += strtmp;
	strxpath += "]";

	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	SearchedItem item;
	time_t pubdate;
	COleDateTime date;
	COleDateTimeSpan gmtspan;

	IXMLDOMNodePtr nodeptr;

	for(int i = 0; i < listptr->length; ++i)
	{
		nodeptr = listptr->item[i];
		ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, pxml, nodeptr), date);
		if(!psearch->t.haveAll(date)) continue;

		if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) item.bread = TRUE;
		else item.bread = FALSE;
		if(GetNamedNodeText(XML_READON, pxml, nodeptr) == _T("1")) item.breadon = TRUE;
		else item.breadon = FALSE;

//		guid = GetNamedNodeText(XML_GUID, pxml, nodeptr);
//		title = GetNamedNodeText(XML_SUBJECT, pxml, nodeptr);

//		date -= gmtspan;
		pubdate = ConvertDateTimeToTimeT(date);
		if(pubdate == -1) continue;

		// ** graph item 은 postid 대신 아이템의 pubdate를 전송한다.
		item.pubdate = pubdate;
		
		psearch->apresult->apitems->push_back(item);
	}

	return FALSE;
}

/**************************************************************************
 * function SearchXML_Local
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 * updated 2006-03-15 :: encoded 정보 추가.. by aquarelle
 *
 * @Description
 * XML에서 주어진 검색을 수행한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) 검색을 수행할 포인터
 * (in/out WorkSearch*) psearch - (WorkSearch*) 검색 내용과 검색 결과를 담은 구조체 포인터
 * (in int) ctype - 채널의 종류 (채널의 종류에 따라 결과의 내용을 다르게 처리한다.)
 *
 * @Return
 * (BOOL) - 검색 성공여부
 **************************************************************************/
BOOL SearchXML_Local(CEzXMLParser* pxml, WorkSearch *psearch, int ctype)
{
	if(!pxml 
		|| !psearch 
		|| !(psearch->apresult->aplistpost.get())
		|| ctype == CT_SEARCH				// 검색 결과의 재검색을 막는다.
		|| ctype == CT_LSEARCH				// 검색 결과의 재검색을 막는다.
		)
		return FALSE;

	CString strxpath;
	CString strtmp;
	CString keylowered;
	keylowered = psearch->keyword;
	keylowered.MakeLower();

	strxpath = XML_XPATH_ITEM_ALL;
	strxpath += "[";
	// 제목에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 작자에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 내용에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_DESC, keylowered);
	strxpath += strtmp;
	strxpath += "]";

	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	int j = 0;
	TAG tag;
	POSTITEM item;

	IXMLDOMNodePtr nodeptr;

	for(int i = 0; i < listptr->length; ++i)
	{
		nodeptr = listptr->item[i];

		// 로컬 검색 결과는 모두 읽지 않음 처리
		item.read = FALSE;
//		if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) item.read = TRUE;
//		else item.read = FALSE;

		if(GetNamedNodeText(XML_READON, pxml, nodeptr) == _T("1")) item.readon = TRUE;
		else item.readon = FALSE;

//		if(GetNamedNodeText(XML_BNS, pxml, nodeptr) == _T("1")) continue;

#ifdef USE_XMLBNS
		if(GetNamedNodeText(XML_BNS, pxml, nodeptr) == _T("1")) item.bns = TRUE;
		else item.bns = FALSE;
#endif

//		if(GetNamedNodeText(XML_XMLUPDATE, pxml, nodeptr) == _T("1")) item.xmlupdate = TRUE;
//		else item.xmlupdate = FALSE;
		item.channelid = psearch->chid;

		item.author = GetNamedNodeText(XML_AUTHOR, pxml, nodeptr);

		item.category = GetNamedNodeText(XML_CATEGORY, pxml, nodeptr);

		item.description = GetNamedNodeText(XML_DESC, pxml, nodeptr);

		item.enclosure = GetNamedNodeText(XML_ENCLOSURE, pxml, nodeptr);

		item.guid = GetNamedNodeText(XML_GUID, pxml, nodeptr);

		item.url = GetNamedNodeText(XML_URL, pxml, nodeptr);

		item.subject = GetNamedNodeText(XML_SUBJECT, pxml, nodeptr);

		item.encoded = GetNamedNodeText(XML_ENCODED, pxml, nodeptr);

		item.stype = LOCAL_SEARCH;

		ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, pxml, nodeptr), item.pubdate);

/*		listptr = pxml->SearchNodes(nodeptr, XML_TAG);
		if(listptr != NULL && listptr->length > 0)
		{
			CString t;
			for(j = 0; j< listptr->length; ++j)
			{
				 if(listptr->item[j]->text.length() > 0)
				 {
					 t = (LPTSTR) listptr->item[j]->text;
					 tag.t = t;
					 item.plistTag->push_back(tag);
				 }
			}
		}
*/
		psearch->apresult->aplistpost->push_back(item);
	}

	return TRUE;
}


/**************************************************************************
 * fuction SearchXML_Net
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * XML에서 주어진 검색을 수행한다.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) 검색을 수행할 포인터
 * (in/out WorkSearch*) psearch - (WorkSearch*) 검색 내용과 검색 결과를 담은 구조체 포인터
 * (in int) ctype - 채널의 종류 (채널의 종류에 따라 결과의 내용을 다르게 처리한다.)
 *
 * @Return
 * (BOOL) - 검색 성공여부
 **************************************************************************/
BOOL SearchXML_Net(CEzXMLParser* pxml, WorkSearch *psearch, int ctype)
{
	if(!pxml 
		|| !psearch 
		|| !(psearch->apresult->apitems.get())
		)
		return FALSE;

	CString strxpath;
	CString strtmp;
	CString keylowered;
	keylowered = psearch->keyword;
	keylowered.MakeLower();

	strxpath = XML_XPATH_ITEM_ALL;
	strxpath += "[";
	// 제목에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 작자에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// 내용에서 검색
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_DESC, keylowered);
	strxpath += strtmp;
	strxpath += "]";

	IXMLDOMNodeListPtr listptr = NULL;

	listptr = pxml->SearchNodes((LPTSTR) (LPCTSTR) strxpath);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	SearchedItem item;
	CString guid;
	CString link;
	CString title;
	CString postid;
	COleDateTime date;
	COleDateTimeSpan gmtspan;

	item.bscrap = (ctype == CT_SCRAP) ? TRUE : FALSE;
	item.bfilter = (ctype == CT_KEYWORD) ? TRUE : FALSE;

	IXMLDOMNodePtr nodeptr;
	
	for(int i = 0; i < listptr->length; ++i)
	{
		nodeptr = listptr->item[i];

		ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, pxml, nodeptr), date);
		if(!psearch->t.haveAll(date)) continue;

#ifdef USE_XMLBNS
		if(GetNamedNodeText(XML_BNS, pxml, nodeptr) == _T("1")) continue;
#endif

		postid = GetNamedNodeText(XML_ID, pxml, nodeptr);
		item.id = _ttoi((LPCTSTR) postid);
		if(item.id == 0) continue;

		if(GetNamedNodeText(XML_READ, pxml, nodeptr) == _T("1")) item.bread = TRUE;
		else item.bread = FALSE;
		if(GetNamedNodeText(XML_READON, pxml, nodeptr) == _T("1")) item.breadon = TRUE;
		else item.breadon = FALSE;

		guid = GetNamedNodeText(XML_GUID, pxml, nodeptr);
		title = GetNamedNodeText(XML_SUBJECT, pxml, nodeptr);
		link = GetNamedNodeText(XML_URL, pxml, nodeptr);

		item.sguid = guid;
		item.t = title;
		
		if(link == guid)
			item.link = GNU_DEFAULT_URL;

		item.pubdate = ConvertDateTimeToTimeT(date);
		psearch->apresult->apitems->push_back(item);
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * XML 구조체에서 주어진 검색을 수행한다.
 *
 * @Parameters
 * (in/out RSS_XMLDATA*) pxmldata - XML 구조체 포인터
 * (in/out WorkSearch*) psearch - 검색 내용 및 결과를 담은 포인터
 * (in int) ctype - 채널의 종류 (채널의 종류에 따라 결과의 내용을 다르게 처리한다.)
 * (out BOOL*) popen - XML 구조체에서 XML 파일을 새로 열었는지 여부
 *
 * @Return
 * (BOOL) - 검색 성공여부
 **************************************************************************/
BOOL SearchXML(RSS_XMLDATA* pxmldata, WorkSearch* psearch, int ctype, BOOL *open)
{
	if(!pxmldata || !psearch || !open) return FALSE;

	*open = FALSE;

	CEzXMLParser* pxml = pxmldata->XML.get();
//	TRACE(_T("xml parse point : %d\r\n"), pxml);
	if(!pxml)
	{
		pxml = new CEzXMLParser;
		TRACE(_T("create new parser : %d\r\n"), pxml);
		SmartPtr<CEzXMLParser> apxml(pxml);

		if(!apxml->LoadXML((LPTSTR) (LPCTSTR) pxmldata->path))
			return FALSE;

		pxmldata->XML = apxml;
		*open = TRUE;
	}

	BOOL bresult = FALSE;
	switch(psearch->type)
	{
	case NET_SEARCH:
	case LONGTERM_SEARCH:
		bresult = SearchXML_Net(pxml, psearch, ctype);
		break;
	case LOCAL_SEARCH:
		bresult = SearchXML_Local(pxml, psearch, ctype);
		break;
	case GRAPH_SEARCH:
		bresult = SearchXML_Graph(pxml, psearch, ctype);
		break;
	}

	return bresult;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * 특정 채널에 대한 네트워크 검색을 수행한다.
 *
 * @Parameters
 * (in RSS_CHANNELDATA&) channel - 검색할 채널
 * (in/out auto_ptr<WorkSearch>&) apsearch - 검색 구조체의 포인터
 **************************************************************************/
int CRSSDB::Search_Channel(RSS_CHANNELDATA& channel, auto_ptr<WorkSearch>& apsearch)
{
	// do not channellock!!

	XML_LIST::iterator it;
	BOOL btravelold = FALSE;
	int chcnt = 0;

	if(!channel.ChannelLock()) return 0;

	if(apsearch->type == LONGTERM_SEARCH)
	{
		btravelold = TRUE;
	}

	BOOL bopen;

	for(it = channel.listData.begin(); it != channel.listData.end() && m_bSThread; ++it)
	{
		if(apsearch->type == NET_SEARCH
			&& apsearch->apresult->apitems->size() > MAX_SEARCHCOUNT)
		{
			break;
		}

		// 검색 범위에 포함되는가?
		bopen = FALSE;

		if( (*it).term.haveSome (apsearch->t) || btravelold)
		{
			SearchXML(&(*it), apsearch.get(), (*it).type, &bopen);
		}

		if(bopen)
		{
			channel.dtOldUseTime = COleDateTime::GetCurrentTime();
			channel.oldused = TRUE;
		}
	}

/*
	if(btravelold)
	{
		for(it = channel.listOldData.begin(); it != channel.listOldData.end() && m_bSThread; ++it)
		{
			bopen = FALSE;

			SearchXML(&(*it), apsearch.get(), (*it).type, &bopen);

			if(bopen)
			{
				channel.dtOldUseTime = COleDateTime::GetCurrentTime();
				channel.oldused = TRUE;
			}
		}
	}
*/	
	channel.ChannelUnlock();

	return chcnt;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-01
 *
 * @Description
 * 맵 검색을 수행한다.
 *
 * @Parameters
 * (in/out auto_ptr<WorkSearch>&) apsearch - 검색 구조체의 포인터
 *
 * @Return
 * (int) - 현재 검색조건으로 검색을 계속할지를 결정한다.
 *
 *		not 1 or 2 : 검색을 그만 함
 *		1 : 검색을 계속함 - 리스트의 뒤에 추가, 우선 처리
 *		2 : 검색을 계속함 - 리스트의 앞에 추가, 나중에 처리
 **************************************************************************/
int CRSSDB::Search_Map(auto_ptr<WorkSearch>& apsearch)
{
	XML_MAP::iterator it;
	int nchannelcnt = 0;

	int nmapsize = m_mapData.size();

	// 네트웍 검색인 경우에는 검색 대상 채널의 개수를 제한한다.
#ifdef USE_NETSEARCHMAXCHANNEL
	int nremaincnt = 0;
	int nremaincntorig = 0;
	BOOL bNetSearch = ( apsearch->type == CT_SEARCH );
	if(bNetSearch)
	{
		if(m_nSearchSeq < nmapsize) m_nSearchSeq = 0;
		nremaincnt = MAX_NETSEARCHCHANNEL;
		if(nremaincnt > nmapsize) nremaincnt = nmapsize;
		nremaincntorig = nremaincnt;
	}
#endif

	for(it = m_mapData.begin(); it != m_mapData.end() && m_bSThread; ++it)
	{
		++nchannelcnt;
#ifdef USE_NETSEARCHMAXCHANNEL
		if(m_nSearchSeq > nchannelcnt) continue;
#endif
		if(apsearch->nchannelcnt > nchannelcnt) continue;

		if((*it).second.type != CT_SEARCH
			&& (*it).second.type != CT_LSEARCH
			)
		{
			Search_Channel((*it).second, apsearch);
		}

#ifdef USE_NETSEARCHMAXCHANNEL
		--(nremaincnt);
#endif

		// 한번에 한개의 채널만 처리한다.
		if(apsearch->type == LONGTERM_SEARCH || apsearch->type == GRAPH_SEARCH) break;
	}

#ifdef USE_NETSEARCHMAXCHANNEL
	if(bNetSearch)
	{
		if(nremaincnt > 0)
		{
			int cnt = 0;
			for(it = m_mapData.begin(); it != m_mapData.end() && m_bSThread; ++it)
			{
				if(nremaincnt <= 0) break;

				if((*it).second.type != CT_SEARCH
					&& (*it).second.type != CT_LSEARCH
					)
				{
					Search_Channel((*it).second, apsearch);
				}

				--(nremaincnt);
			}

			m_nSearchSeq += ;
		}
		else
			m_nSearchSeq += nremaincntorig;
	}
#endif

	// 계속 검색을 수행해야되는 것인 
	apsearch->nchannelcnt = nchannelcnt;
	if(apsearch->nchannelcnt >= m_mapData.size() 
		|| apsearch->IsComplete()
		) return 0;

	return 1;
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-01
 *
 * @Description
 * SEARCH_LIST에서 처리할 검색을 가져와 검색 작업을 수행한다.
 *
 * @Parameters
 * (in SEARCH_LIST&) wlist - 검색할 SEARCH_LIST
 * (in CRITICAL_SECTION&) cs - SEARCH_LIST와 쌍을 이루는 critical section
 **************************************************************************/
void CRSSDB::SearchProcess(SEARCH_LIST& wlist, CRITICAL_SECTION& cs)
{
	auto_ptr<WorkSearch> apsearch;
	EnterCriticalSection(&cs);
	TRACE(_T("search item process LOCK\r\n"));
	if(wlist.size() > 0)
		apsearch = wlist.back();
	wlist.pop_back();
	LeaveCriticalSection(&cs);
	TRACE(_T("search item process UNLOCK\r\n"));

	if(apsearch.get() == NULL) return;

	if(!apsearch->valid) return;				// 기간이 유효하지 않은 검사 정보는 사용하지 않는다.

#ifdef CHECK_WORKTIME
	DWORD start, interval;
	start = GetTickCount();
#endif

	int priority = 0;
	auto_ptr<RESULT> apr;

	// 이곳에서 검색 종류에 따른 검색을 수행한다.
	// 검색을 계속해야되면 다시 Search List에 넣는다.
	int goon = Search_Map(apsearch);

	if(goon != 0)
	{
		EnterCriticalSection(&cs);
		if(goon == 2) wlist.push_front(apsearch);
		else wlist.push_back(apsearch);
		LeaveCriticalSection(&cs);
	}
	else
	{
		apr = auto_ptr<RESULT> (apsearch->apresult.release());
		apsearch->apresult = auto_ptr<ResultSearch> (NULL);
	}

#ifdef CHECK_WORKTIME
	interval = GetTickCount() - start;
	TRACE(_T("search time = %d\r\n"), interval);
#endif

	// 처리 결과 전송
	if(apr.get() != NULL && goon == 0)
		PUSH_RESULT(priority, apr);

	if(goon != 0) TriggerSearch();
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * RSS DB의 데이터를 처리하는 메인 Thread
 * Work Queue를 검사하여 처리할 작업이 있는 경우 WorkProcess 함수를 호출한다.
 **************************************************************************/
DWORD CRSSDB::SearchThread()
{
	TRACE(_T("Start Search Thread\r\n"));

	CoInitialize(NULL);
	DWORD dwWait = 0;
	m_bSThread = TRUE;
	int i;

	MSG msg;
//	using posthreadmessage
	while(::GetMessage(&msg, NULL, 0, 0))
	{
		if(msg.message == WTM_CLOSE)
		{
			break;
		}

		if(!m_bSThread) break;

		for(i = 0; i < MAX_SEARCHLISTCNT && m_bSThread; ++i)
		{
			if(m_queueSearch[i].size() > 0)
			{
				// Searching...
				// TO DO
				// CPU 점유율 확인.
				SearchProcess(m_queueSearch[i], m_csSearch[i]);
				break;
			}
		}

		for(i = 0; i < MAX_SEARCHLISTCNT && m_bSThread; ++i)
		{
			if(m_queueSearch[i].size() > 0)
			{
//				SetEvent(m_hSThreadEvent);
				TriggerSearch();
				break;
			}
		}
	}

	CoUninitialize();
	TRACE(_T("Search thread End\r\n"));

	return 0;
}

BOOL CRSSDB::SearchPost()
{
	// DEPRECATED
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Interface function
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * RSSDB 작업을 수행하도록 요청한다.
 **************************************************************************/
#ifdef FISH_TEST
void CRSSDB::Trigger(int i)
#else
void CRSSDB::Trigger()
#endif
{
	// comment by moonknit 2005-11-02
	// 테스트를 위해 스레드 처리를 피함
#ifdef FISH_TEST
	if(i < 0 || i >= MAX_WORKLISTCNT) return;

	WorkProcess(m_queueWork[i], m_csInQueue[i]);
#else
	Sleep(1);
	PostThreadMessage(m_dwThreadId, WTM_NEXT, NULL, NULL);
#endif
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 처리 결과를 가져간다.
 **************************************************************************/
int CRSSDB::PUSH_WORK(int i, auto_ptr<WORK>& ptr)
{
	if(i < 0 || i >= MAX_WORKLISTCNT) return -1;

	if(m_queueWork[i].size() > MAX_QUEUE) return -1;

	if(m_nseq == MAX_WORKSEQ) m_nseq = 0;

	EnterCriticalSection(&m_csInQueue[i]);
	m_queueWork[i].push_front(ptr);
	LeaveCriticalSection(&m_csInQueue[i]);

#ifdef FISH_TEST
	Trigger(i);
#else 
	Trigger();
#endif

	return m_nseq;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-30
 * updated 2005-12-07 :: parameter int chid 추가 by moonknit
 * updated 2005-12-13 :: gmt date deleted by moonknit
 *
 * @Description
 * 검색을 요청한다.
 *
 * @Parameters
 * (in SEARCH_TYPE) t - 검색의 종류, (0 : 로컬검색, 1 : 네트웍 검색, 2 : 장기 검색)
 * (in CString) key - 검색어
 * (in CString) lcode - 요청자의 언어코드
 * (in CString) sterm - 검색 기간
 * (in int) ncnt - 최대 검색 개수 (0 : 개수 제한 없음)
 * (in int) chid - 로컬 검색시 검색 결과를 직접 RSSDB로 재전달 하기 위해 필요한 값이다.
 **************************************************************************/
int CRSSDB::IRequestSearch(SEARCH_TYPE t, CString key, CString lcode, CString sterm, int ncnt, int chid)
{
	return PUSH_WORK(PTW_SEARCH, auto_ptr<WORK> (new WorkSearch(++m_nseq, t, key, lcode, sterm, ncnt, chid)) );
}

/**************************************************************************
 * method CRSSDB::IRequestNetSearch
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-29
 *
 * @Description
 * 검색을 요청한다.
 *
 * @Parameters
 * (in SEARCH_TYPE) t - 검색의 종류, (LOCAL_SEARCH : 로컬 검색 : 네트웍 검색, 2 : 장기 검색)
 * (in CString) key - 검색어
 * (in CString) lcode - 요청자의 언어코드 - not using
 * (in CString) sterm - 검색 기간
 * (in int) ncnt - 최대 검색 개수 (0 : 개수 제한 없음)
 * (in SE_QUERY) q - 네트워크를 통해 응답을 전달하기 위한 query data
 * (in int) chid - LOCAL_SEARCH에서 필요한 채널 id
 **************************************************************************/
int CRSSDB::IRequestNetSearch(SEARCH_TYPE t, CString key, CString lcode, CString sterm, int ncnt, SE_QUERY q, int chid)
{
	return PUSH_WORK(PTW_SEARCH, auto_ptr<WORK> (new WorkSearch(++m_nseq, t, key, lcode, sterm, ncnt, q)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 모든 채널의 flush 할 데이터를 flush 처리하도록 요청한다.
 **************************************************************************/
int CRSSDB::IFlush()
{
	if(m_bLastFlush) return 0;

	return PUSH_WORK(PTW_CHANNEL_FLUSH, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_FLUSH, 0, FALSE)) );
}

int CRSSDB::IChannelSetPath(CString path)
{
	return PUSH_WORK(PTW_CHANNEL_SETPATH, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_SETPATH, 0, 0, 0, path)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 새로운 포스트를 추가하도록 요청한다.
 **************************************************************************/
int CRSSDB::IPostAdd(auto_ptr<POST_LIST>& items, const COleDateTime& lasttime, int addtype, int channelid)
{
	TRACE(_T("PostAdd\r\n"));
	return PUSH_WORK(PTW_POST_ADD, auto_ptr<WORK> (new WorkAddPost(++m_nseq, WS_POST_ADD, items, lasttime
		, addtype, channelid)));
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 하나를 추가할 때
 **************************************************************************/
int CRSSDB::IPostAdd(auto_ptr<POSTITEM>& apitem, GUID guid, int addtype)
{
	TRACE(_T("PostAdd\r\n"));
	auto_ptr<POST_LIST> apitems(new POST_LIST);
	apitems->push_back(*apitem);
	COleDateTime lasttime = COleDateTime::GetCurrentTime();
	return PUSH_WORK(PTW_POST_ADD, auto_ptr<WORK> (new WorkAddPost(++m_nseq, WS_POST_ADD, apitems, lasttime
		, guid, addtype, apitem->channelid)));
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-22
 *
 * @Description
 * 포스트를 읽음 상태로 변경하도록 요청한다.
 **************************************************************************/
int CRSSDB::IPostRead(int pid, int cid, BOOL bset, BOOL binc)
{
	int flag = FISH_ITEM_READ;
	if(bset)
	{
		if(binc) flag |= FISH_ITEM_INC_COUNT;
	}
	return IPostFlagUpdate(pid, cid, bset, flag);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-22
 *
 * @Description
 * 포스트를 링크읽음 상태로 변경하도록 요청한다.
 **************************************************************************/
int CRSSDB::IPostReadOn(int pid, int cid, BOOL bset, BOOL binc, BOOL bnavigate)
{
	int flag = FISH_ITEM_READON;
	if(bset)
	{
		if(bnavigate) flag |= FISH_ITEM_URL_NAVIGATE;
		if(binc) flag |= FISH_ITEM_INC_COUNT;
	}
	return IPostFlagUpdate(pid, cid, bset, flag);
}

/***************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-22
 *
 * @Description
 * 포스트를 데이터 갱신 상태로 변경하도록 요청한다.
 ***************************************************************************/
int CRSSDB::IPostXMLUpdate(int pid, int cid, BOOL bset)
{
	return IPostFlagUpdate(pid, cid, bset, FISH_ITEM_XMLUPDATE);
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-22
 *
 * @Description
 * 포스트의 플래그 상태를 TRUE로 변경을 요청한다.
 **************************************************************************/
int CRSSDB::IPostFlagUpdate(int pid, int cid, BOOL bset, int flag)
{
	if(cid == 0)
	{
		MAP_POSTMATCH::iterator it;
		it = m_mapPostMatch.find(pid);

		if(it == m_mapPostMatch.end())
			return 0;

		cid = (*it).second;
	}

	auto_ptr<POST_LIST> aplist(new POST_LIST);
	aplist->clear();
	aplist->push_back(POSTITEM(pid, cid, bset, bset, bset));

	return PUSH_WORK(PTW_POST_UPDATE, auto_ptr<WORK> (new WorkPostData(++m_nseq, WS_POST_UPDATE, aplist, flag)));
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 포스트의 갱신을 요청한다.
 **************************************************************************/
int CRSSDB::IPostUpdate(auto_ptr<POST_LIST>& items, int flag)
{
	TRACE(_T("PostUpdate\r\n"));
	return PUSH_WORK(PTW_POST_UPDATE, auto_ptr<WORK> (new WorkPostData(++m_nseq, WS_POST_UPDATE, items, flag)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * 포스트의 갱신을 요청한다.
 **************************************************************************/
int CRSSDB::IPostUpdate(POST_LIST& items, int flag)
{
	TRACE(_T("PostUpdate\r\n"));
	auto_ptr<POST_LIST> apitems(new POST_LIST);
	(*apitems) = items;

	return PUSH_WORK(PTW_POST_UPDATE, auto_ptr<WORK> (new WorkPostData(++m_nseq, WS_POST_UPDATE, apitems, flag)) );
}

/**************************************************************************
 * static method CRSSDB::IPostUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-09
 *
 * @Description
 * 포스트의 갱신을 요청한다.
 **************************************************************************/
int CRSSDB::IPostUpdate(auto_ptr<POSTITEM>& apitem, int flag)
{
//	TRACE(_T("PostSVUpdate\r\n"));
	auto_ptr<POST_LIST> apitems(new POST_LIST);

	apitems->push_back(*apitem);

	return PUSH_WORK(PTW_POST_UPDATE, auto_ptr<WORK> (new WorkPostData(++m_nseq, WS_POST_UPDATE, apitems, flag)) );
}

/**************************************************************************
 * static method CRSSDB::IPostSVUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-09
 *
 * @Description
 * 포스트의 검색 정보의 값을 갱신한다.
 **************************************************************************/
int CRSSDB::IPostSVUpdate(auto_ptr<POSTITEM>& apitem)
{
	return IPostUpdate(apitem, FISH_ITEM_SEARCHVALUE);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 포스트의 삭제를 요청한다.
 **************************************************************************/
int CRSSDB::IPostDelete(auto_ptr<ID_LIST>& idlist, int flag, int channelid)
{
	TRACE(_T("PostDelete\r\n"));
	return PUSH_WORK(PTW_POST_DELETE, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_DELETE, idlist, flag, channelid)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * 포스트의 삭제를 요청한다.
 **************************************************************************/
int CRSSDB::IPostDelete(ID_LIST& idlist, int flag, int channelid)
{
	TRACE(_T("PostDelete\r\n"));
	auto_ptr<ID_LIST> apidlist(new ID_LIST);
	(*apidlist) = idlist;
	return PUSH_WORK(PTW_POST_DELETE, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_DELETE, apidlist, flag, channelid)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 포스트의 삭제를 요청한다.
 **************************************************************************/
int CRSSDB::IPostDelete(int id, int flag, int channelid)
{
	TRACE(_T("PostDelete\r\n"));
	auto_ptr<ID_LIST> apidlist(new ID_LIST);
	apidlist->push_back(id);
	return PUSH_WORK(PTW_POST_DELETE, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_DELETE, apidlist, flag, channelid)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 주어진 포스트를 해당 채널로 스크랩을 요청한다.
 **************************************************************************/
int CRSSDB::IPostScrap(auto_ptr<ID_LIST>& apidlist, int channelid)
{
	TRACE(_T("PostScrap"));
	return PUSH_WORK(PTW_POST_SCRAP, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_SCRAP, apidlist, channelid)) );

}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 주어진 포스트를 해당 채널로 스크랩을 요청한다.
 **************************************************************************/
int CRSSDB::IPostScrap(ID_LIST& idlist, int channelid)
{
	TRACE(_T("PostScrap\r\n"));
	auto_ptr<ID_LIST> apidlist(new ID_LIST);
	(*apidlist) = idlist;
	return PUSH_WORK(PTW_POST_SCRAP, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_SCRAP, apidlist, channelid)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 주어진 포스트를 해당 채널로 스크랩을 요청한다.
 *
 * @Parameters
 * (in int) id - 스크랩 할 포스트의 포스트 아이디
 * (in int) channelid - 스크랩 채널의 아이디
 **************************************************************************/
int CRSSDB::IPostScrap(int id, int channelid)
{
	TRACE(_T("PostScrap\r\n"));
	auto_ptr<ID_LIST> apidlist(new ID_LIST);
	apidlist->push_back(id);
	return PUSH_WORK(PTW_POST_SCRAP, auto_ptr<WORK> (new WorkPostID(++m_nseq, WS_POST_SCRAP, apidlist, channelid)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 포스트를 읽어오도록 요청한다.
 **************************************************************************/
int CRSSDB::IPostGet(auto_ptr<POST_LIST>& items, int gettype)
{
	TRACE(_T("PostGet\r\n"));
	return PUSH_WORK(PTW_POST_GET, auto_ptr<WORK> (new WorkPostData(++m_nseq, WS_POST_GET, items, gettype)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 포스트를 읽어오도록 요청한다.
 * 
 * @Parameters
 * (in LPVOID) lpuploader - UPLoad를 수행할 uploadshell pointer
 * (in int) postid - 전송할 post의 id
 **************************************************************************/
int CRSSDB::IRequestUpload(LPVOID lpuploader, int postid)
{
	TRACE(_T("PostSend\r\n"));
	return PUSH_WORK(PTW_POST_UPLOAD, auto_ptr<WORK> (new WorkUpload(++m_nseq, postid, lpuploader)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널의 삭제를 요청한다.
 **************************************************************************/
int CRSSDB::IChannelDelete(int channelid, BOOL dataonly)
{
	TRACE(_T("ChannelDelete\r\n"));
	return PUSH_WORK(PTW_CHANNEL_DELETE, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_DELETE, channelid, (int) dataonly)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널의 모든 포스트를 읽어오도록 요청한다.
 **************************************************************************/
int CRSSDB::IChannelGet(int channelid, int gettype)
{
	TRACE(_T("ChannelGet\r\n"));
	return PUSH_WORK(PTW_CHANNEL_GET, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_GET, channelid, gettype)) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * XML 파일에서 포스트 cache 데이터를 읽어오도록 요청한다.
 **************************************************************************/
int CRSSDB::IChannelLoad()
{
	TRACE(_T("ChannelLoad\r\n"));
	int ret = PUSH_WORK(PTW_CHANNEL_LOAD, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_LOAD, 0)) );
	PostThreadMessage(m_dwThreadId, WTM_NEXT, NULL, NULL);
	return ret;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널 추가를 요청한다.
 **************************************************************************/
int CRSSDB::IChannelAdd(int channelid, int chtype, BOOL bnotsearch, CString text
						, CString keyword, BOOL bLog, int keepcnt, int keepdays)
{
	TRACE(_T("ChannelAdd\r\n"));
	int data = 0;
	if(bnotsearch) data |= CHANNEL_CREATE_BNS;
	if(bLog) data |= CHANNEL_CREATE_SEARCHLOG;
	WorkChannel* pChannel = new WorkChannel(++m_nseq, WS_CHANNEL_ADD, channelid, chtype, data, text, keyword);
	pChannel->addataEx[0] = keepcnt;
	pChannel->addataEx[1] = keepdays;
	return PUSH_WORK(PTW_CHANNEL_ADD, auto_ptr<WORK> (pChannel));
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-19
 *
 * @Description
 * 채널의 flag 값을 변경한다.
 *
 * @Parameters
 * (in int) channelid - 갱신할 대상 채널의 아이디
 * (in int) dest - 갱신할 대상 아이템
				CDM_SEARCHLOG : bsearchlog 갱신
 * (in int) flag - 갱신할 내용
 **************************************************************************/
int CRSSDB::IChannelModify(int channelid, int dest, int flag)
{
	TRACE(_T("ChannelSearchLog\r\n"));
	return PUSH_WORK(PTW_CHANNEL_MODIFY, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_MODIFY, channelid, dest, flag)));
}

int CRSSDB::IChannelNotSearch(int channelid, BOOL flag)
{
	return IChannelModify(channelid, CMD_BNSFLAG, flag);
}

int CRSSDB::IChannelSearchLog(int channelid, BOOL flag)
{
	return IChannelModify(channelid, CMD_SEARCHLOG, flag);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-07
 *
 * @Description
 * 채널의 모든 포스트를 읽음 처리를 요청한다.
 **************************************************************************/
int CRSSDB::IChannelRead(int channelid, BOOL bread)
{
	TRACE(_T("ChannelRead\r\n"));
	return PUSH_WORK(PTW_CHANNEL_READ, auto_ptr<WORK> (new WorkChannel(++m_nseq, WS_CHANNEL_READ, channelid, (int) bread)) );
}

BOOL CRSSDB::IsExistKeyword(const CString &keyword)
{
	BOOL result = FALSE;
	EnterCriticalSection(&m_csKeywordList);

	KEYWORD_LIST::iterator it;
	for(it = m_listKeyword.begin(); it != m_listKeyword.end(); ++it)
	{
		if(keyword == (*it).keyword)
		{
			result = TRUE;
			break;
		}
	}
	LeaveCriticalSection(&m_csKeywordList);
	return result;
}

