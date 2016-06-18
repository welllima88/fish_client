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

#define	MAX_LOADXML							5				// �ѹ��� 10���� ���ϸ� �о�´�.
#define MAX_FLUSHLIMIT						2				// �ѹ� flush�� ä�� 2���� ó���Ѵ�.

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
 * RSS_XMLDATA�� XML�� ���ǿ� ���߾� ���Ϸ� ����Ѵ�.
 * flush ������ ������ ����.
 * 1. force �÷��װ� TRUE�� ���
 * 2. data.flush �÷��װ� TRUE�̰� flush �ð������� ������ ���
 * @Parameters
 * (in/out RSS_XMLDATA&) data - FLUSH�� ������ ��� ��ü
 * (in COleDateTime) t - �ð� ������ �Ǵ��� �ð�
 * (in COleDateTimeSpan) s - �ð� ������ ������ �ð�
 * (in BOOL) force - ���� flush �÷���, TRUE�� ���� flush FALSE�� �ð� ������ �Ǵ��Ѵ�.
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
 * RSS_XMLDATA���� XML������ �Ҹ��Ų��.
 *
 * @Parameters
 * (in RSS_XMLDATA&) data - XML������ �Ҹ��ų ��� ��ü
 *
 * @Returns
 * (BOOL) XML���� �Ҹ� ��������
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
 * XML_LIST�� �� ��ü�� �Ⱓ�� to ������ �����Ѵ�.
 *
 * @Parameters
 * (in/out XML_LIST) l - �Ⱓ ������ ������ XML_LIST
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
 * XML �������� ���� Post ID���� ã�Ƴ���.
 *
 * @Parameters
 * (in CEzXMLParser*) parser - �˻��� ��� XML
 * (in int) channelid - ����� Post�� channel id
 * (in int&) unread - ���� �ʴ� post�� ������ ��ȯ�Ѵ�.
 * (in int&) postcnt - ��ü ����Ʈ�� ������ ��ȯ�Ѵ�.
 * (in/out RSS_CHANNELDATA&) cdata - GUID �ߺ� �˻縦 ���� ����
 *
 * @Returns
 * (int) ���� Post ID, ã�� ���ϸ� 0�� ��ȯ�Ѵ�.
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
		if(git != cdata.mapGuid.end())		// �̹� �����ϴ� ����Ʈ
		{
			// ����ó��
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
		// �̹� �ش��ϴ� ����Ʈ�� id�� ����ִٸ� ���� �� ���� sequence���� ���� ����Ʈ id�� ������.
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
 * post�� �����Ѵ�.
 *
 * @Parameters
 * (in int) postid - ������ ��� post id
 * (in/out CEzXMLParser*) pxml - ������ ����� XML ��ü ������
 * (in DWORD) flag - ���� ��� ����
 * FISH_ITEM_ALL : ������ ��ü ����
 * FISH_ITEM_TAG : �������� �±� ����
 * FISH_ITEM_NODE : �������� ��� ����
 * @Returns
 * (BOOL) ���� ��������
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
 * XML���ϸ��� �̿��Ͽ� XML ITEM ����
 *
 * @Parameters
 * (in int) postid - ���� ��� post id
 * (in/out RSS_XMLDATA&) xmldata - ������ xml ��� ������ ��� �ִ� ����ü
 * (in/out BOOL&) bopen - ������ ���� ������ ��ȯ�ϱ� ���� ����
 * (in/out BOOL&) decunread - �������� ����Ʈ�� �����Ͽ����� ������ ��ȯ�ϱ� ���� ����
 * (in DWORD) flag - ������ ����ϴ� flag
 *
 * @Returns
 * (BOOL) ���� ���� ����
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
 * XML������ ��� Item�� �����Ѵ�.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - Item�� ������ XML
 *
 * @Returns
 * (BOOL) ���� ���
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
 * �ش� ä���� XML�� �����.
 *
 * @Parameters
 * (in TERM) t - ���� ä���� �Ⱓ
 * (in int) channelid - ���� ä���� id
 * (in int) seq - ���� xml ������ �ߺ��� ���� �־��� sequence
 * (in int) chtype - ä���� ����
 * (in CString) xmlurl - ä���� ��Ȯ�ϰ� Ȯ���ϱ� ���� id�� ���ȴ�
 *
 * @Returns
 * (CEzXMLParser*) ������ XML ����ü
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

		// VERSION ������ term ���� �׸��� CHANNEL ������ �ִ´�.
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
 * XML�� �־��� post id�� ���� �������� �����ϴ��� ���θ� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - �˻� ��� XML
 * (in int) postid - �˻��� post id
 * (out IXMLDOMNodePtr&) nodeptr - �˻��� item�� node�� ��´�.
 *
 * @Returns
 * (BOOL) �־��� post id�� ���� item�� �߰� ����
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
 * XML�� �־��� GUID�� ���� �������� �����ϴ��� ���θ� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - �˻� ��� XML
 * (in CString) guid - �˻��� GUID
 * (out IXMLDOMNodePtr&) nodeptr - �˻��� item�� node�� ��´�.
 *
 * @Returns
 * (BOOL) �־��� post id�� ���� item�� �߰� ����
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
 * XML�� �����ϴ� post�� ������ ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - �˻翡 ����� XML
 *
 * @Returns
 * (int) post�� ����
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
 * updated 2006-03-10 :: �ð� ������ �̿��� XML�߰� ��Ŀ��� post id �� �̿��� ������� ����
 *						by moonknit
 *
 * @Description
 * XML_LIST���� �־��� �ð��� �ش�Ǵ� XML�� ã���ش�.
 *
 * @Parameters
 * (in/out XML_LIST&) l - �˻��� list
 * (in int) postid - �˻��� ����Ʈ ���̵�
 * (out XML_LIST::iterator&) it - ã�Ƴ� XML�� iterator
 * (oout IXMLDOMNodePtr&) nodeptr - �߰ߵ� ����Ʈ�� XML Node
 *
 * @Returns
 * (BOOL) �߰� ����
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
 * updated 2005-12-06 :: �˻� ��� ��ġ ���� �߰� �� �̿� ���� Parameter �߰� (int ctype) by moonknit
 * updated 2005-12-13 :: node�� tag ���� ����
 *						node�� memory ���������� ó���� �ϰ� tag�� ���� ����ü�� �����Ѵ�.
 *						by moonknit
 * updated 2006-01-24 :: CHECK_GUIDBYREGION ���� �� �߰� by moonknit
 * modified 2006-03-10 :: CHECK_GUIDBYREGION ���� �� ���� by moonknit
 * updated 2006-03-15 :: encoded ���� �߰�... by aquarelle
 *
 * @Description
 * Post�� XML�� �߰��Ѵ�.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - ����� XML
 * (in POSTITEM&) item - �߰��� post
 *
 * @Returns
 * (BOOL) �߰� ���� ����
 **************************************************************************/
BOOL AddItemtoXML(CEzXMLParser* pxml, POSTITEM& item, int ctype)
{
	// item �� �����Ͽ�
	// ������ �����
	IXMLDOMNodePtr nodeptr;

	IXMLDOMElementPtr elptr = NULL;
	elptr = pxml->CreateElement(XML_ITEM);

	if(elptr == NULL) return FALSE;

	// ���� attributes of item
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
 * POST�� �־��� Ű���尡 �ִ��� Ȯ���Ѵ�.
 * linear search
 * Author or description or subject will be a destinated object.
 *
 * @Parameters
 * (in POSTITEM&) item - �˻� ��� post
 * (in CString) keyword - Ű����
 *
 * @Returns
 * (BOOL) Ű���� �߰� ����
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
 * XML�� Channel ������ �����Ѵ�.
 *
 * @Parameters
 * (in/out RSS_XMLDATA&) data - ������ XML ����ü
 * (in int) flag - ���� ���
 *
 * @Returns
 * (BOOL) ���� ���� ����
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
 * updated 2005-12-06 ::  parameter chtype �߰�, chtype�� CT_SEARCH�� ���
 *						�߰� ������ XML_SV_READ, XML_SV_READON, XML_SV_SCRAP ������ ��û�Ѵ�.
 *						by moonknit
 * updated 2005-12-13 :: node�� tag ���� ����
 *						node�� memory ���������� ó���� �ϰ� tag�� ���� ����ü�� �����Ѵ�.
 *						by moonknit
 * updated 2006-03-15 :: encoded ���� �߰�... by aquarelle
 *
 * @Description
 * XML���� post id�� ���� post�� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in/out POSTITEM&) item - post id�� �־��ָ� �ش��ϴ� post�� ������ �޴´�
 * (in int) chtype - ä���� ����
 * (in int) read - ���� ó�� ���� �÷���
 * (in/out CEzXMLParser*) pxml - XML��ü
 * (in DWORD) flag - ������ ������ ����
 *
 * @Returns
 * (BOOL) ��������
 **************************************************************************/
BOOL GetXMLItem(POSTITEM& item, int chtype, int read, CEzXMLParser* pxml, DWORD flag)
{
	// XML���� POST ������ ã�Ƽ� item�� �����Ѵ�.
	// read���� ���� flag ó���� �����Ѵ�.
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

		// �˻� ����� ��� �߰� ������ �����Ѵ�.
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
 * modified 2006-03-20 :: parameter channelid �߰� by moonknit
 *
 * @Description
 * XML���� post id�� ���� post�� XML String �������� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in int) postid - ã�ƿ� post�� id
 * (out CString&) xml - ��ȯ�� xml string
 * (in int) channelid - DEFAULT 0, ä�� ���̵� ���� �־��ָ� m_mapPostMatch�� ����ϴ�
 *					�δ��� �����ش�.
 *
 * [Returns]
 * (BOOL) ��������
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

	// �޸𸮿��� ã�´�.
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
 * updated 2005-12-06 ::  parameter chtype �߰�, GetXMLItemȣ��� chtype�� �ѱ��.
 *						by moonknit
 *
 * @Description
 * XML���� post id�� ���� post�� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in/out POSTITEM&) item - post id�� �־��ָ� �ش��ϴ� post�� ������ �޴´�
 * (in int) chtype - ä���� ����
 * (in int) read - ���� ó�� ���� �÷���
 * (in/out RSS_XMLDATA&) xmldata - XML������ ���� ����ü
 * (in/out BOOL&) bopen - XML ������ ���� �����°��� ���� output
 * (in DWORD) flag - ������ ������ ����
 *
 * [Returns]
 * (BOOL) ��������
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
 * updated 2005-12-06 :: parameter chtype �߰�, chtype�� CT_SEARCH�� ���
 *						�߰� ������ XML_SV_READ, XML_SV_READON, XML_SV_SCRAP ������ ��û�Ѵ�.
 *						by moonknit
 * updated 2006-03-15 :: encoded ���� �߰�. by aquarelle
 *
 * @Description
 * XML�� ��� item�� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in/out auto_ptr<POST_LIST>&) aplistpost - item�� ���� list
 * (in int) chid - POST�� channelid ��
 * (in int) chtype - ä���� ����
 * (in CEzXMLParser*) pxml - XML��ü
 * (in BOOL) flag - �˻� ����
 * (in CString) scond - �˻� ����
 *
 * @Returns
 * (BOOL) Item ȹ�� ����
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

			// �˻� ����� ��� �߰� ������ �����Ѵ�.
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

		// channel data�� ������ ������ tag�� nodelist�� �������� �ʴ´�.

		aplistpost->push_back(item);

	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-xx
 * updated 2005-12-06 :: �Ķ���� int chtype �߰� �� GetXMLItemList ȣ��� chtype ���� by moonknit
 *
 * @Description
 * XML�� ��� item�� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in/out auto_ptr<POST_LIST>&) aplistpost - item�� ���� list
 * (in int) chid - POST�� channelid ��
 * (in int) chtype - ä���� ����
 * (in/out RSS_XMLDATA&) xmldata - XML��ü�� ���� ����ü
 * (in/out BOOL&) bopen - ���� ������ ��� �۾��� �ߴٸ� TRUE�� ��ȯ�Ѵ�.
 * (in BOOL) flag - �˻� ����
 * (in CString) scond - �˻� ����
 *
 * @Returns
 * (BOOL) Item ȹ�� ����
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
 * �־��� post id�� post�� XML string �������� �����س���.
 *
 * @Parameters
 * (in int) postid - ������ post�� id
 * (in/out CEzXMLParser*) pxml - XML parsing�� ������ Wrapper class
 * (out CString&) xml - ��ȯ�� string
 *
 * @Returns
 * (BOOL) Item ȹ�� ����
 **************************************************************************/
BOOL GetXMLItemString(int postid, CEzXMLParser* pxml, CString& xml)
{
	// XML���� POST ������ ã�Ƽ� item�� �����Ѵ�.
	// read���� ���� flag ó���� �����Ѵ�.
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
 * �־��� post id�� post�� XML string �������� �����س���.
 *
 * @Parameters
 * (in int) postid - ������ post�� id
 * (in/out RSS_XMLDATA&) xmldata - XML��ü�� ���� ����ü
 * (in/out BOOL&) bopen - ���� ������ ��� �۾��� �ߴٸ� TRUE�� ��ȯ�Ѵ�.
 * (out CString&) xml - ��ȯ�� string
 *
 * @Returns
 * (BOOL) Item ȹ�� ����
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
 * XML�� ��� Item�� ������ ���ÿ� �����Ѵ�. read�� readon �÷��׸� �����Ѵ�.
 *
 * @Parameters
 * (in/out CEzXMLParser*) pxml - XML ��ü
 * (in BOOL) read - ������ �� TRUE or FALSE
 *
 * @Returns
 * (BOOL) ���� ���� ����
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
 * XML�� ��� Item�� ������ ���ÿ� �����Ѵ�. read�� readon �÷��׸� �����Ѵ�.
 *
 * @Parameters
 * (in/out RSS_XMLDATA&) xmldata - XML ��ü�� ���� ������
 * (in DWORD) flag - ������ ����
 * (in BOOL) read - ������ �� TRUE or FALSE
 *
 * @Returns
 * (BOOL) ���� ���� ����
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
 * updated 2005-12-06 ::  parameter chtype �߰�, chtype�� CT_SEARCH�� ���
 *						XML_SV_READ, XML_SV_READON, XML_SV_SCRAP������ �����Ѵ�.
 *						by moonknit
 * updated 2005-12-13 :: node�� tag ���� ����
 *						node�� memory ���������� ó���� �ϰ� tag�� ���� ����ü�� �����Ѵ�.
 *						by moonknit
 * updated 2006-03-15 :: encoded ���� �߰�. by aquarelle
 *
 * @Description
 * XML�� Item�� ������ �����Ѵ�. 
 *
 * @Parameters
 * (in POSTITEM&) item - ������ ������ ������ �ִ� ��ü
 * (in/out CEzXMLParser*) pxml - XML ��ü
 * (out int&) unread - ���� ���� ���� ( 0 : ���� ����, 1 : �� ���� -> ����, -1 : ���� -> �� ����)
 * (in DWORD) flag - ������ ���
 *
 * @Returns
 * (BOOL) ���� ���� ����
 **************************************************************************/
BOOL CRSSDB::UpdateXMLItem(POSTITEM& item, int chtype, CEzXMLParser* pxml, int& unread, DWORD flag)
{
	// XML���� POST ������ ã�Ƽ� �־��� item������ ������ �����Ѵ�.
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
		// ��� ó���� ���� �˷����� ���� postid���� �����Ͽ��ش�.
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

	// ä���� ������ �˻��� ��쿡�� �˻����� ������ ����Ѵ�.
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

	// GUID�� ������� �ʴ´�.
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
		// �߰��ϴ� ��찡 �ƴϸ� ���� ����� �����Ѵ�.
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
 * updated 2005-12-06 ::  parameter chtype �߰�, UpdateXMLItem ȣ��� chtype ����
 *						by moonknit
 *
 * @Description
 * XML�� Item�� ������ �����Ѵ�. 
 *
 * @Parameters
 * (in POSTITEM&) item - ������ ������ ������ �ִ� ��ü
 * (in int) chtype - ä���� ���� 
 * (in/out RSS_XMLDATA&) xmldata - XML ��ü�� ������ �ִ� ����ü�� ��ü
 * (in DWORD) flag - ������ ���
 *
 * @Returns
 * (BOOL) ���� ���� ����
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
 * �ʿ� ���ο� ä���� ���� ��� �Ѵ�.
 *
 * @Parameters
 * (in/out XML_MAP&) m - ä�� ��
 * (in int) channelid - ������ ä���� id
 * (in int) chtype - ������ ä���� ����
 * 
 * @Returns
 * (XML_MAP::iterator) ������ ä���� iterator
 * ������ �����ϸ� m.end()�� ��ȯ�Ѵ�.
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
 * ���� CriticalSection�� �ʱ�ȭ�Ѵ�.
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
 * ���� CriticalSection�� �����Ѵ�.
 * XML �����͸� ���Ϸ� Flush �Ѵ�.
 * Thread�� �� �ִٸ� Thread�� �����Ѵ�.
 **************************************************************************/
void CRSSDB::DeinitStatic()
{
	if(!m_Init) return;

	--m_nInit;

	if(m_nInit == 0)
	{
		// ���� �����߿��� Flush�� ���� �ʴ´�.
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
 * �۾� ó������� queue�� ����ϰ� ��ϵ� �����쿡 ó������� �������� �޽����� ���� �˸���.
 *
 * @Parameters
 * (in int) i - ��� ó�� Priority
 * (in auto_ptr<RESULT>&) ptr - ����� ó�����
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
 * �˻� �۾��� �����Ѵ�.
 * �˻� ������� �۾��� �ѱ�
 *
 * @Parameters
 * (in/out auto_ptr<WORK>&) apwork - �˻� �۾� ������ ��� �ִ� auto_ptr
 **************************************************************************/
void CRSSDB::WP_Search(auto_ptr<WORK>& apwork)
{

	TRACE(_T("CRSSDB::WP_Search\r\n"));
	auto_ptr<WorkSearch> apsearch ( static_cast<WorkSearch*> (apwork.release()) );

	// �˻� ������ ť�� �ִ´�.
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
 * ����Ʈ�� ã�Ƽ� Uploader�� �ѱ⵵�� ó���Ѵ�.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - �۾��� ������ ������ �ִ� ����ü�� ������, ����� ����Ʈ����� ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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
 * �������� ���ο� post�� �ش� ä�ο� �߰��Ѵ�.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - �۾��� ������ ������ �ִ� ����ü�� ������, ����� ����Ʈ����� ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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

	// ���� ���ŵ� keepcnt, MIN_KEEPCNT �̻� ��ȿ�ϴ�.
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
 * ���� post�� ������ �����Ѵ�.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - �۾��� ������ ������ �ִ� ����ü�� ������, ����� ����Ʈ����� ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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
 * �־��� id�� ����Ʈ���� �����Ѵ�.
 *
 * @Parameters
 * (in/out WorkPostID*) ppostid - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ����Ʈ���̵��� ����� ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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
 * �־��� id�� ����Ʈ���� ��� ä�η� ��ũ���Ѵ�.
 *
 * @Parameters
 * (in/out WorkPostID*) ppostid - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ��ũ���� ä���� id�� ����ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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
 * �־��� ����Ʈ���� ������ �����´�.
 *
 * @Parameters
 * (in/out WorkPostData*) ppostdata - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ����Ʈ�� ����� ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
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
 * ä���� ��� ����Ʈ�� ���� ���¸� �����Ѵ�.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ä���� id�� ���� ó�� ������ ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelRead(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ä���� ���� ���¸� �ϰ������� �����Ѵ�.
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
 * ä���� ������ �����Ѵ�.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ä���� id�� ���� ó�� ������ ��� �ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelModify(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ä���� ���� ���¸� �ϰ������� �����Ѵ�.
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
 * ��� ä���� flush�� �����Ѵ�.
 *
 * @Parameters
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
int CRSSDB::WP_ChannelFlush(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ��� ä���� ����� �����͸� flush�Ѵ�.
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
 * ��� ä���� XML ���� �н��� �� �����Ѵ�.
 *
 * @Parameters
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelSetPath(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ��� ä���� ����� �����͸� flush�Ѵ�.
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
 * ������ ���丮�� xml���Ϸ� ���� channel�� post������ �о�´�.
 *
 * @Parameters
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 *
 * @Result
 * (int) - ä�� �ε� �۾��� ��� �ؾ��� ��쿡�� 1�� �׷��� ���� ��쿡�� 0�� ��ȯ�Ѵ�.
 **************************************************************************/
int CRSSDB::WP_ChannelLoad(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// �־��� ���丮���� fish �����͸� �о�´�.
	// fish �����͸� �о�������� channel�� ������ ����Ǿ�� �Ѵ�.
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
 * ���ο� ä���� �߰��Ѵ�.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, �߰��� ä���� ������ ����ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelAdd(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ä���� �����Ѵ�.
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
 * ä���� �����Ѵ�.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ä���� ������ ����ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelDelete(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	BOOL result;
	if( ((BOOL) pchannel->adinfo) == TRUE)
	{
		// ä�� ���̵� ����Ͽ� ä���� item�� �����͸� ��� �����Ѵ�.
		RemovePostByChannelID(pchannel->channelid);
	}
	else
	{
		// ä�� ���̵� �̿��Ͽ� ä����ü�� �����Ѵ�.
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
 * ä���� ��� post�� �����´�.
 *
 * @Parameters
 * (in/out WorkChannel*) pchannel - �۾��� ������ ������ �ִ� ����ü�� ������
 *			, ������ ä���� ������ ����ִ�.
 * (out int&) p - ó������� Priority
 * (out auto_ptr<RESULT>&) apr - ó������� ��� ��ȯ�� ������
 **************************************************************************/
void CRSSDB::WP_ChannelGet(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr)
{
	// ä�� ���̵� ä���� ����Ʈ�� �о�´�.
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
 * updated 2005-12-19 :: channel modify �߰� by moonknit
 *
 * @Description
 * WORK_LIST���� ó���� ���� ������ �� ó���� �ʿ��� �Լ��� ȣ�����ش�.
 *
 * @Parameters
 * (in WORK_LIST&) wlist - ó���� WORK_LIST
 * (in CRITICAL_SECTION&) cs - WORK_LIST�� ���� �̷�� critical section
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

	// ó�� ��� ����
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
 * ó�� ����� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in auto_ptr<RESULT>&) apresult - ó�����
 *
 * @Returns
 * (BOOL) ó����� ��ȯ ����
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
 * �۾� ó������� CRSSDB���� ó���� ��� �� �Լ��� ����Ѵ�.
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
 * �̰����� RSSDB �۾��� ����� ó���Ѵ�.
 * �Ǵ� GetResult�� �̿��ؼ� ó�� ����� ��������.
 *
 * @Parameters
 * (in RESULT_LIST&) rlist - ó���� ��� ���
 * (in CRITICAL_SECTION&) cs - ��� ��ϰ� ���� �̷�� critical section
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
 * RSS DB�� �����͸� ó���ϴ� ���� Thread
 * Work Queue�� �˻��Ͽ� ó���� �۾��� �ִ� ��� WorkProcess �Լ��� ȣ���Ѵ�.
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
 * Work Thread �� Search Thread�� �����Ѵ�.
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
 * WorkThread�� �����Ѵ�.
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
 * SearchThread�� �����Ѵ�.
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
 * WorkThread�� �Ҹ��Ѵ�.
 * WorkQueue�� ResultQueue�� ä�θ� �׸��� KeywordList�� Ŭ�����Ѵ�.
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

	// ������� ��ŭ ������ ���д�.
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
 * SearchThread�� �Ҹ��Ѵ�.
 * Search Queue�� Ŭ�����Ѵ�.
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
 * flush�� ���� path�� �����Ѵ�.
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
 * ��� xml�� flush �۾��� �����Ų��.
 *
 * @Parameters
 * (in BOOL) force - ������ flush �۾��� �����ų ��� ����ϴ� �÷���
 **************************************************************************/
int CRSSDB::FlushData(BOOL force, BOOL& keepon)
{
	keepon = FALSE;
	if(force)
	{
		m_bLastFlush = TRUE;
	}

	// ����Ʈ�� �ʱ�ȭ ���� �ʾҴٸ� FLUSH �� �������� �ʴ´�.
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
		// �ð��� ���� �ɸ��� �۾��� ���Ḧ ���ؼ� �߰��� ���� �� �ִ�.
		if(!force && !m_bThread) break;

		// ä���� ������ �˻��̰�
		// �˻� ��� �÷��װ� �����Ǿ� ���� �ʴٸ� ����� ������ �ʴ´�.
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

		// ��ũ���� �˻��� old data ó���� ���� �ʴ´�.
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
		// ���� flush (���� flush)�� �ƴ� ��� �ѹ� �۾��� flush �ϴ� ä���� ������ �Ѱ踦 �д�.
		if(cnt >= MAX_FLUSHLIMIT)
		{
			keepon = TRUE;
			(*it).second.ChannelUnlock();
			return cnt;
		}

		// ���� FLUSH�� ��쿡�� listData -> listOldData�� �ű��.
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
			// �ڵ� ����.... �׷��� ������ ������ ó���� xml�� load ������ �����Ѵ�.
			if(bAutoDelete)
				DELETE_XMLDATA((*it).second.listData.back());

/*
			// memory data ���� old data�� �ű�
			(*it).second.listOldData.push_front((*it).second.listData.back());

			if(DELETE_XMLDATA((*it).second.listOldData.front()))
			{
				(*it).second.listData.pop_front();
				(*it).second.unread -= unread;
				(*it).second.postcnt -= postcnt;

				if(unread)
				{
					// ���������� ����Ʈ ������ �˷��ش�.
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
				// �׻� ������ �����ʹ� �������� 0���� ��´�.
				(*it).second.listOldData.back().term.from.m_dt = 0;

				// XML ������ ��� from ������ �����Ѵ�.
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
 * m_listData �� �����Ϳ� 
 * m_listOldData �� �����͸� �ð������� ��迭 ��Ų��.
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
			// ä���� ������ ����Ѵ�.
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
 * xml������ �о�� ������ ����Ʈ�� �Է��Ѵ�.
 *
 * @Parameters
 * (in CString) filename - �о�� ���ϸ�
 *
 * @Returns
 * (BOOL) XML �ε� ��������
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

	// �� ���� ��
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

	// memory load ���θ� �Ǵ���
	termload = GetNamedNodeText(XML_LOAD, &tmpxml, node);

	int itermload = _ttoi((LPTSTR) (LPCTSTR) termload);

	// XML ������ ������ �ִ� ä���� id
	channelid = GetNamedNodeText(XML_CHANNELID, &tmpxml, node);

	int ichannelid = _ttoi((LPTSTR) (LPCTSTR) channelid);

	if(channelid.IsEmpty() || ichannelid < 0)
	{
		return FALSE;
	}

	// ä���� ����
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

	// Post MATCH�� ����ϸ� ���� Post ID�� ã�Ƴ���.
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

			// ä���� ���� ���� ����Ʈ�� ���� ����
			(*it).second.unread += unread;
			(*it).second.postcnt += postcnt;


			(*it).second.listData.push_front(RSS_XMLDATA(t, filename, pxml, ichanneltype));
			(*it).second.listData.front().load = itermload;

			// ���� ä�� ������ ���� ���� ����Ʈ�� ���� ����
			(*it).second.listData.front().unread = unread;
			(*it).second.listData.front().postcnt = postcnt;

			// ���������� ����Ʈ ������ �˷��ش�.

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
			// old data�� ����Ʈ�� ���� ���� ������ ó������ �ʴ´�.

			(*it).second.listOldData.push_front(RSS_XMLDATA(t, filename, NULL, ichanneltype));
		}

		// ���Ǵ� post sequence�� �����Ѵ�.
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
 * ������ ���丮�� ���� �����͸� �о�ͼ� XML list�� �ִ´�.
 * �����ϸ� ���������� �о�� xml ������ ��ȯ�Ѵ�.
 *
 * @Returns
 * (int) ���� ��������
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
 * modified 2006-03-20 :: parameter channelid �߰� by moonknit
 *
 * @Description
 * �޸� �������� �����Ϳ��� postid ���� �����ϴ� post ������ �����Ѵ�.
 *
 * @Parameters
 * (in int) postid - ������ ����� postid
 * (in DWORD) flag - ���� �÷���
 * (in int) channelid - DEFAULT 0, ä�� ���̵� ���� �־��ָ� m_mapPostMatch�� ����ϴ�
 *					�δ��� �����ش�.
 * @Returns
 * (BOOL) ���� ��������
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

	// �޸𸮿��� ã�´�.
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
		// ���������� ����Ʈ ������ �˷��ش�.
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
 * �޸𸮻��� ä���� ����Ʈ�� �����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - ������ ä�� id
 *
 * @Returns
 * (BOOL) ���� �������� 
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

	// �޸𸮿��� ã�´�.
	// ã�Ƽ� �����Ѵ�.
//	for(it2 = (*it).second.listData.begin(); it2 != (*it).second.listData.end(); ++it2)

	result = TRUE;
	do
	{
		// ���� �� �׸��� ������.
		rit = (*it).second.listData.rbegin();

		if((*rit).XML != NULL) RemoveXMLAllItem((*rit).XML.get(), (*it).second);

		// ������ XML�� ���ܵд�.
		if((*it).second.listData.size() == 1)
		{
			(*rit).flush = TRUE;
			(*rit).unread = 0;
			(*rit).postcnt = 0;
			break;
		}

		// XML ������ �����.
		DeleteFile((*rit).path);

		// ���� �� �׸��� �����Ѵ�.
		(*it).second.listData.pop_back();
	} while(TRUE);

	if(result)
	{
		(*it).second.unread = 0;
		(*it).second.postcnt = 0;

		(*it).second.flush = TRUE;
		m_bFlush = TRUE;

		// ���������� ����Ʈ ������ �˷��ش�.
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
 * ä���� �����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - ������ ��� ä�� ���̵�
 *
 * @Returns
 * (BOOL) ���� ���� ����
 **************************************************************************/
BOOL CRSSDB::RemoveChannel(int channelid)
{
	XML_MAP::iterator it = m_mapData.find(channelid);

	if(it == m_mapData.end())
	{
		return FALSE;
	}

	if(!(*it).second.ChannelLock()) return FALSE;

	// ���� �޸� �������� ������ ����

	XML_LIST::iterator it2 = (*it).second.listData.begin();

	for(;it2 != (*it).second.listData.end(); ++it2)
	{
		DeleteFile((*it2).path);
	}

	(*it).second.listData.clear();

	// ������ ������ ����
	it2 = (*it).second.listOldData.begin();

	for(;it2 != (*it).second.listOldData.end(); ++it2)
	{
		DeleteFile((*it2).path);
	}

	(*it).second.listOldData.clear();

	// ��ϵ� Ű���带 ���ش�.
	if((*it).second.type == CT_KEYWORD)
	{
		RemoveKeyword(channelid);
	}

	// POST MATCH ������ �����Ѵ�.
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
 * ä���� �߰��Ѵ�.
 *
 * @Parameters
 * (in int) postid - ��ũ���� ����Ʈ�� id
 * (in int) channelid - ��ũ���� ä���� id
 *
 * @Returns
 * (BOOL) ��ũ��
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
 * modifiedy 2006-03-10 :: keepcnt, keepdays�� �߰��� by moonknit
 *
 * @Description
 * ä���� �߰��Ѵ�.
 *
 * @Parameters
 * (in int) channelid - �߰��� channel id
 * (in int) chtype - �߰��� ä���� ����
 * (in CString) text - guid
 * (in CString) keyword - keyword (chtype == CT_KEYWORD �� ��쿡�� ��ȿ�ϴ�.)
 * (in int) keepcnt - ������ �ִ� ����Ʈ�� ����
 * (in int) keepdays - �ִ� ������ �Ⱓ
 *
 * @Returns
 * (BOOL) �߰� ���� ����
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

		// ���� ä���� ��쿡�� ����ڵ带 �ִ´�.
		// �� ���� ä���� UTF-8�� ����Ѵ�.
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
 * Ű���� ���͸� �۾��� �����Ѵ�.
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
 * updated 2005-12-06 :: AddItemtoXML ȣ��� chtype ���� by moonknit
 * updated 2006-01-24 :: CHECK_GUIDBYREGION ���� �� �߰� by moonknit
 * modified 2006-03-10 :: CHECK_GUIDBYREGION ���� �� ���� ���� by moonknit
 *
 * @Description
 * ���ο� Post�� �߰��Ѵ�.
 * ä���� subscribe�� ��츸 filtering �۾��� �����Ѵ�.
 *
 * @Parameters
 * (in POSTITEM&) item - �߰��� post ����
 *
 * @Returns
 * (BOOL) ����Ʈ �߰� ��������
 **************************************************************************/
BOOL CRSSDB::AddPost(POSTITEM& item)
{
	BOOL bnew = FALSE;			// �� XML ���� �÷���
	XML_MAP::iterator it = m_mapData.find(item.channelid);
	XML_LIST::iterator it2;
	BOOL result = FALSE;
	
	CEzXMLParser* pxml = NULL;

	if(it == m_mapData.end())
	{
		// ä���� �������� �����Ƿ� ���ο� Post�� �߰��� �� ����.
		return FALSE;
	}
	int chtype = (*it).second.type;

	if(!(*it).second.ChannelLock()) return FALSE;

	// ���� ä���� item�� bns ������ �߰��Ѵ�.
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
	// �������� ��ü�� �ľ��Ѵ�.
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
			// ã�ƿ� ����� ���� �ֽ� �޸� �������̰� item������ ���� �̻��̸� ���ο� XML�� �����Ѵ�.
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
			// listData�� XML ������ ��� �������� �ұ��ϰ�
			// item�� pubdate�� ���ϴ� XML�� ã�� �� ����

			// itemData�� ���ϴ� ��ü�� �ð� ������ �����ϵ����Ѵ�.

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


		// ���� �޸� �������� ���� �ֱ� ������ �Ⱓ ������ �����Ѵ�.
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
		// ����� ����Ʈ�� �ϳ����̰ų� 
		// ���� ����� ����Ʈ�� ���� ������ XML�������� �Ⱓ�� ���Եȴٸ� 
		// ����� ���� ������ XML�����Ϳ� �߰��Ѵ�.
		// �׷��� �ʴٸ� ������ �ֽ� XML�����Ϳ� �߰��Ѵ�.
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

		// ä�� ���� ������ ���� �ʴ� ����Ʈ�� ���� ����
		++((*it2).unread);
		++((*it2).postcnt);

		// ��� �ð� ���� ����
		COleDateTimeSpan s;
		s.SetDateTimeSpan(0, 0, 0, 1);

		(*it2).term.to = item.pubdate + s;

		(*it2).flush = TRUE;
		(*it).second.flush = TRUE;
		m_bFlush = TRUE;

		// ä���� ���� �ʴ� ����Ʈ�� ���� ����
//		if(!item.read)
		++((*it).second.unread);
		++((*it).second.postcnt);

		// ���������� ����Ʈ ������ �˷��ش�.
		SendMessage(m_hSubscribeWnd, WM_CHANNELUCNT_UPDATE, (*it).first, (*it).second.unread);
		SendMessage(m_hSubscribeWnd, WM_CHANNELPCNT_UPDATE, (*it).first, (*it).second.postcnt);

		m_mapPostMatch.insert(MAP_POSTMATCH::value_type(item.postid, item.channelid));
		(*it).second.mapGuid.insert(MAP_GUIDMATCH::value_type(item.guid, item.postid));

		result = TRUE;
	}

	(*it).second.ChannelUnlock();

	// AddPost�� ȣ���ϹǷ� Filtering ȣ�� ���� ChannelUnlock�� �����ؾ� �Ѵ�.
	if(chtype == CT_SUBSCRIBE && bAdd)
		Filtering(item);			

	return result;
}

/*
 * created 2005-10-xx
 * updated 2005-12-06 :: GetXMLItem ȣ��� ä�� Ÿ���� �̿��Ѵ�. by moonknit
 
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

	// �޸𸮿��� ã�´�.
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
 * ��û�� ����Ʈ�� ������ ��ȯ�Ѵ�.
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
 * updated 2005-12-06 :: GetXMLItemList ȣ��� ä�� ������ �ѱ� by moonknit 
 *
 * @Description
 * ä���� ��� ����Ʈ ������ ��ȯ�Ѵ�.
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
 * ä���� ��� ����Ʈ�� ���� ���θ� ó���Ѵ�.
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
			// ���������� ����Ʈ ������ �˷��ش�.
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
 * ä���� ������ ������ �� ����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - ������ ��� ä���� ���̵�
 * (in int) dest - ������ ��� ������
				CDM_SEARCHLOG : bsearchlog ����
 * (in int) flag - ������ ����
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
 * ����Ʈ�� ������ �����Ѵ�.
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

	// �޸𸮿��� ã�´�.
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

			// ���������� ����Ʈ ������ �˷��ش�.
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
 * ä���� ����Ѵ�.
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
			// channeld data ����
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
 * filtering keyword�� �߰��Ѵ�.
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
 * filtering keyword�� �����Ѵ�.
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
 * �˻� ������ ���� �켱 ������ ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in SEARCH_TYPE) type - �˻� ����
 *
 * @Return
 * (int) - �켱 ����
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
 * Search �۾��� �����ϵ��� ��û�Ѵ�.
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
 * XML���� �־��� �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) �˻��� ������ ������
 * (in/out WorkSearch*) psearch - (WorkSearch*) �˻� ����� �˻� ����� ���� ����ü ������
 * (in int) ctype - ä���� ���� (ä���� ������ ���� ����� ������ �ٸ��� ó���Ѵ�.)
 *
 * @Return
 * (BOOL) - �˻� ��������
 **************************************************************************/
BOOL SearchXML_Graph(CEzXMLParser* pxml, WorkSearch *psearch, int ctype)
{
	if(!pxml 
		|| !psearch 
		|| !(psearch->apresult->aplistpost.get())
		|| ctype == CT_SEARCH				// �˻� ����� ��˻��� ���´�.
		|| ctype == CT_LSEARCH				// �˻� ����� ��˻��� ���´�.
		)
		return FALSE;

	CString strxpath;
	CString strtmp;
	CString keylowered;
	keylowered = psearch->keyword;
	keylowered.MakeLower();

	strxpath = XML_XPATH_ITEM_ALL;
	strxpath += "[";
	// ���񿡼� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���ڿ��� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���뿡�� �˻�
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

		// ** graph item �� postid ��� �������� pubdate�� �����Ѵ�.
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
 * updated 2006-03-15 :: encoded ���� �߰�.. by aquarelle
 *
 * @Description
 * XML���� �־��� �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) �˻��� ������ ������
 * (in/out WorkSearch*) psearch - (WorkSearch*) �˻� ����� �˻� ����� ���� ����ü ������
 * (in int) ctype - ä���� ���� (ä���� ������ ���� ����� ������ �ٸ��� ó���Ѵ�.)
 *
 * @Return
 * (BOOL) - �˻� ��������
 **************************************************************************/
BOOL SearchXML_Local(CEzXMLParser* pxml, WorkSearch *psearch, int ctype)
{
	if(!pxml 
		|| !psearch 
		|| !(psearch->apresult->aplistpost.get())
		|| ctype == CT_SEARCH				// �˻� ����� ��˻��� ���´�.
		|| ctype == CT_LSEARCH				// �˻� ����� ��˻��� ���´�.
		)
		return FALSE;

	CString strxpath;
	CString strtmp;
	CString keylowered;
	keylowered = psearch->keyword;
	keylowered.MakeLower();

	strxpath = XML_XPATH_ITEM_ALL;
	strxpath += "[";
	// ���񿡼� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���ڿ��� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���뿡�� �˻�
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

		// ���� �˻� ����� ��� ���� ���� ó��
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
 * XML���� �־��� �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in CEzXMLParser*) pxml - (CEzXMLParser*) �˻��� ������ ������
 * (in/out WorkSearch*) psearch - (WorkSearch*) �˻� ����� �˻� ����� ���� ����ü ������
 * (in int) ctype - ä���� ���� (ä���� ������ ���� ����� ������ �ٸ��� ó���Ѵ�.)
 *
 * @Return
 * (BOOL) - �˻� ��������
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
	// ���񿡼� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_SUBJECT, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���ڿ��� �˻�
	strtmp.Format(STR_XML_CON_TRAN_XPATH_TEMPLATE, STR_XML_WHERE_AUTHOR, keylowered);
	strxpath += strtmp;
	strxpath += " or ";
	// ���뿡�� �˻�
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
 * XML ����ü���� �־��� �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in/out RSS_XMLDATA*) pxmldata - XML ����ü ������
 * (in/out WorkSearch*) psearch - �˻� ���� �� ����� ���� ������
 * (in int) ctype - ä���� ���� (ä���� ������ ���� ����� ������ �ٸ��� ó���Ѵ�.)
 * (out BOOL*) popen - XML ����ü���� XML ������ ���� �������� ����
 *
 * @Return
 * (BOOL) - �˻� ��������
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
 * Ư�� ä�ο� ���� ��Ʈ��ũ �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in RSS_CHANNELDATA&) channel - �˻��� ä��
 * (in/out auto_ptr<WorkSearch>&) apsearch - �˻� ����ü�� ������
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

		// �˻� ������ ���ԵǴ°�?
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
 * �� �˻��� �����Ѵ�.
 *
 * @Parameters
 * (in/out auto_ptr<WorkSearch>&) apsearch - �˻� ����ü�� ������
 *
 * @Return
 * (int) - ���� �˻��������� �˻��� ��������� �����Ѵ�.
 *
 *		not 1 or 2 : �˻��� �׸� ��
 *		1 : �˻��� ����� - ����Ʈ�� �ڿ� �߰�, �켱 ó��
 *		2 : �˻��� ����� - ����Ʈ�� �տ� �߰�, ���߿� ó��
 **************************************************************************/
int CRSSDB::Search_Map(auto_ptr<WorkSearch>& apsearch)
{
	XML_MAP::iterator it;
	int nchannelcnt = 0;

	int nmapsize = m_mapData.size();

	// ��Ʈ�� �˻��� ��쿡�� �˻� ��� ä���� ������ �����Ѵ�.
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

		// �ѹ��� �Ѱ��� ä�θ� ó���Ѵ�.
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

	// ��� �˻��� �����ؾߵǴ� ���� 
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
 * SEARCH_LIST���� ó���� �˻��� ������ �˻� �۾��� �����Ѵ�.
 *
 * @Parameters
 * (in SEARCH_LIST&) wlist - �˻��� SEARCH_LIST
 * (in CRITICAL_SECTION&) cs - SEARCH_LIST�� ���� �̷�� critical section
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

	if(!apsearch->valid) return;				// �Ⱓ�� ��ȿ���� ���� �˻� ������ ������� �ʴ´�.

#ifdef CHECK_WORKTIME
	DWORD start, interval;
	start = GetTickCount();
#endif

	int priority = 0;
	auto_ptr<RESULT> apr;

	// �̰����� �˻� ������ ���� �˻��� �����Ѵ�.
	// �˻��� ����ؾߵǸ� �ٽ� Search List�� �ִ´�.
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

	// ó�� ��� ����
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
 * RSS DB�� �����͸� ó���ϴ� ���� Thread
 * Work Queue�� �˻��Ͽ� ó���� �۾��� �ִ� ��� WorkProcess �Լ��� ȣ���Ѵ�.
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
				// CPU ������ Ȯ��.
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
 * RSSDB �۾��� �����ϵ��� ��û�Ѵ�.
 **************************************************************************/
#ifdef FISH_TEST
void CRSSDB::Trigger(int i)
#else
void CRSSDB::Trigger()
#endif
{
	// comment by moonknit 2005-11-02
	// �׽�Ʈ�� ���� ������ ó���� ����
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
 * ó�� ����� ��������.
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
 * updated 2005-12-07 :: parameter int chid �߰� by moonknit
 * updated 2005-12-13 :: gmt date deleted by moonknit
 *
 * @Description
 * �˻��� ��û�Ѵ�.
 *
 * @Parameters
 * (in SEARCH_TYPE) t - �˻��� ����, (0 : ���ð˻�, 1 : ��Ʈ�� �˻�, 2 : ��� �˻�)
 * (in CString) key - �˻���
 * (in CString) lcode - ��û���� ����ڵ�
 * (in CString) sterm - �˻� �Ⱓ
 * (in int) ncnt - �ִ� �˻� ���� (0 : ���� ���� ����)
 * (in int) chid - ���� �˻��� �˻� ����� ���� RSSDB�� ������ �ϱ� ���� �ʿ��� ���̴�.
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
 * �˻��� ��û�Ѵ�.
 *
 * @Parameters
 * (in SEARCH_TYPE) t - �˻��� ����, (LOCAL_SEARCH : ���� �˻� : ��Ʈ�� �˻�, 2 : ��� �˻�)
 * (in CString) key - �˻���
 * (in CString) lcode - ��û���� ����ڵ� - not using
 * (in CString) sterm - �˻� �Ⱓ
 * (in int) ncnt - �ִ� �˻� ���� (0 : ���� ���� ����)
 * (in SE_QUERY) q - ��Ʈ��ũ�� ���� ������ �����ϱ� ���� query data
 * (in int) chid - LOCAL_SEARCH���� �ʿ��� ä�� id
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
 * ��� ä���� flush �� �����͸� flush ó���ϵ��� ��û�Ѵ�.
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
 * ���ο� ����Ʈ�� �߰��ϵ��� ��û�Ѵ�.
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
 * ����Ʈ �ϳ��� �߰��� ��
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
 * ����Ʈ�� ���� ���·� �����ϵ��� ��û�Ѵ�.
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
 * ����Ʈ�� ��ũ���� ���·� �����ϵ��� ��û�Ѵ�.
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
 * ����Ʈ�� ������ ���� ���·� �����ϵ��� ��û�Ѵ�.
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
 * ����Ʈ�� �÷��� ���¸� TRUE�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� �˻� ������ ���� �����Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * ����Ʈ�� ������ ��û�Ѵ�.
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
 * �־��� ����Ʈ�� �ش� ä�η� ��ũ���� ��û�Ѵ�.
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
 * �־��� ����Ʈ�� �ش� ä�η� ��ũ���� ��û�Ѵ�.
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
 * �־��� ����Ʈ�� �ش� ä�η� ��ũ���� ��û�Ѵ�.
 *
 * @Parameters
 * (in int) id - ��ũ�� �� ����Ʈ�� ����Ʈ ���̵�
 * (in int) channelid - ��ũ�� ä���� ���̵�
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
 * ����Ʈ�� �о������ ��û�Ѵ�.
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
 * ����Ʈ�� �о������ ��û�Ѵ�.
 * 
 * @Parameters
 * (in LPVOID) lpuploader - UPLoad�� ������ uploadshell pointer
 * (in int) postid - ������ post�� id
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
 * ä���� ������ ��û�Ѵ�.
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
 * ä���� ��� ����Ʈ�� �о������ ��û�Ѵ�.
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
 * XML ���Ͽ��� ����Ʈ cache �����͸� �о������ ��û�Ѵ�.
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
 * ä�� �߰��� ��û�Ѵ�.
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
 * ä���� flag ���� �����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - ������ ��� ä���� ���̵�
 * (in int) dest - ������ ��� ������
				CDM_SEARCHLOG : bsearchlog ����
 * (in int) flag - ������ ����
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
 * ä���� ��� ����Ʈ�� ���� ó���� ��û�Ѵ�.
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

