// FeedParser.cpp: implementation of the CFeedParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FISH.h"
#include "FeedParser.h"
#include "DateParser.h"

#include "XML/EzXMLParser.h"
#include "fish_common.h"
#include "fish_def.h"
#include "fish_struct.h"
#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

//#define LOOP_ALLPARSER

//////////////////////////////////////////////////////////////////////
// RSS 2.0 parsing 
//////////////////////////////////////////////////////////////////////
// xpath
#define XML_DPXPATH_RSS						_T("//rss")
#define XML_DPXPATH_CHANNEL					_T("//channel")
#define XML_DPXPATH_CHANNEL2				_T("//*[name()='channel']")
#define XML_DPXPATH_ITEM					_T("//item")
#define XML_DPXPATH_ITEM2					_T("//*[name()='item']")
#define XML_DPXPATH_RDF						_T("//rdf:RDF")
#define XML_DPXPATH_FEED_F					_T("//*[name()='feed']")

// item name
#define	XML_LASTBUILDDATE					_T("lastBuildDate")
#define XML_VERSION							_T("version")
#define XML_XMLNS							_T("xmlns")

#define RSS10_XPATH							XML_DPXPATH_RDF	
#define RSS10_TYPE							XML_XMLNS
#define RSS10_DATA							_T("http://purl.org/rss/1.0/")

#define RSS20_XPATH							XML_DPXPATH_RSS
#define RSS20_TYPE							XML_VERSION
#define RSS20_DATA							_T("2.0")

#define RSS091_XPATH						XML_DPXPATH_RSS
#define RSS091_TYPE							XML_VERSION
#define RSS091_DATA							_T("0.91")

#define RSS092_XPATH						XML_DPXPATH_RSS
#define RSS092_TYPE							XML_VERSION
#define RSS092_DATA							_T("0.92")

#define RSS093_XPATH						XML_DPXPATH_RSS
#define RSS093_TYPE							XML_VERSION
#define RSS093_DATA							_T("0.93")

#define RSS094_XPATH						XML_DPXPATH_RSS
#define RSS094_TYPE							XML_VERSION
#define RSS094_DATA							_T("0.94")

#define ATOM_XPATH							XML_DPXPATH_FEED_F

#define ATOM03_TYPE							XML_VERSION
#define ATOM03_DATA							_T("0.3")

#define ATOM10_TYPE							XML_XMLNS
#define ATOM10_DATA							_T("http://purl.org/atom/ns#")

#define RV_RSS093						_T("rss0.93")
#define RV_RSS094						_T("rss0.94")

struct PARSER
{
	RSSParser	rp;
	LPCTSTR		v;
	LPCTSTR		xpath;
	LPCTSTR		stype;
	LPCTSTR		data;
};

BOOL DefaultParser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime);

BOOL DefaultRSS20Parser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime);
BOOL DefaultRSS10Parser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime);
BOOL AtomParser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime);
RSSParser GetDefaultParser(CEzXMLParser* pparser, CString& v);
BOOL RemoveAllTag(CString& html); // htmlmaker에... 정의됨...


struct PARSER defaultparser[] = 
{
	{DefaultRSS20Parser, RV_RSS20, RSS20_XPATH, RSS20_TYPE, RSS20_DATA}
	, {DefaultRSS20Parser, RV_RSS091, RSS091_XPATH, RSS091_TYPE, RSS091_DATA}
	, {DefaultRSS20Parser, RV_RSS092, RSS092_XPATH, RSS092_TYPE, RSS092_DATA}
	, {DefaultRSS20Parser, RV_RSS093, RSS093_XPATH, RSS093_TYPE, RSS093_DATA}
	, {DefaultRSS20Parser, RV_RSS094, RSS094_XPATH, RSS094_TYPE, RSS094_DATA}
	, {DefaultRSS10Parser, RV_RSS10, RSS10_XPATH, RSS10_TYPE, NULL}
	, {AtomParser, RV_ATOM03, ATOM_XPATH, ATOM03_TYPE, ATOM03_DATA }
	, {AtomParser, RV_ATOM10, ATOM_XPATH, ATOM10_TYPE, ATOM10_DATA }
	, {0}
}; // 0.91버전... 2.0파서에서 지원 되도록 데이터 추가.... by aquarelle 2006/02/02

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedParser::CFeedParser()
{

}

CFeedParser::~CFeedParser()
{
	m_mapParser.clear();
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-11
 *
 * @Description
 * XML을 파싱을 위한 dll을 읽어온다.
 **************************************************************************/
int CFeedParser::LoadParser()
{
	int cnt = 0;

	m_mapParser.clear();

	return cnt;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-11
 *
 * @Description
 * XML을 파싱하여 POST_LIST를 채운다.
 **************************************************************************/
BOOL CFeedParser::Parsing(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime, CString& v)
{
	if(pparser == NULL || plist == NULL) return FALSE;

	BOOL result = FALSE;

	// 이곳에선 dll의 parsing 함수를 사용한다.
	// TO DO
	try
	{
//		result = DLLParser(pparser, plist, lasttime);
		if(result) return TRUE;
	}
	catch (...)
	{
	}

#ifdef LOOP_ALLPARSER
	RSSParser parser;
	for(int i = 0; defaultparser[i].rp != 0; ++i)
	{
		parser = defaultparser[i].rp;
		if((*parser) (pparser, plist, lasttime))
		{
			return TRUE;
		}
	}

	return FALSE;

#else
	RSSParser parser = GetDefaultParser(pparser, v);

	if(parser)
	{
		return (*parser) (pparser, plist, channelid, lasttime);
	}
	return result;
#endif
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-11
 *
 * @Description
 * 어떤 파서를 써야할지 검사한다.
 **************************************************************************/
RSSParser GetDefaultParser(CEzXMLParser* pparser, CString& v)
{
	IXMLDOMNodeListPtr listptr = NULL;
	CString rssinfodata, rsstmp;

	TRACE(_T("version : %s\n"), v);

	if(!v.IsEmpty())
	{
		for(int i = 0; defaultparser[i].rp != 0; ++i)
		{
			rsstmp = defaultparser[i].v;

			if(rsstmp == v)
			{
				TRACE(_T("\nParser Version : %s\n"), defaultparser[i].v);
				return defaultparser[i].rp;
			}
		}

		return NULL;
	}

	for(int i = 0; defaultparser[i].rp != 0; ++i)
	{
		pparser->SearchNodes((LPTSTR) defaultparser[i].xpath);

		if(listptr != NULL && listptr->length > 0)
		{
			rssinfodata = GetNamedNodeText(defaultparser[i].stype, pparser, listptr->item[0]);

			if(!defaultparser[i].data)
			{
				TRACE(_T("Parser Version : %s"), defaultparser[i].v);
				return defaultparser[i].rp;
			}

			rsstmp = defaultparser[i].data;
			if(rssinfodata == rsstmp)
			{
				TRACE(_T("Parser Version : %s"), defaultparser[i].v);
				return defaultparser[i].rp;
			}
			
			break;
		}
	}

	return NULL;
}

/**************************************************************************
 * written by moonknit, aquarelle
 *
 * @history
 * created 2005-11-11
 * updated 2005-12-15 :: last update 정보가 없는 경우의 처리 by moonknit
 * updated 2006-01-27 :: 제목이 없는 경우 본분 내용에서 일부 발췌 by aquarelle
 * updated 2006-02-02 :: pubdate가 없는 Feed 처리 추가 by aquarelle
 * updated 2006-02-06 :: Date Parsing 실패시 처리 추가 by aquarelle
 *
 * @Description
 * RSS 2.0 파서 
 * RSS 0.91 버전파싱 호환(?) by aquarelle 2006/02/02
 **************************************************************************/
BOOL DefaultRSS20Parser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime)
{
	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pparser->SearchNodes(XML_DPXPATH_CHANNEL);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	CString lastBuildDate = GetNamedNodeText(XML_LASTBUILDDATE, pparser, listptr->item[0], 1);
	if(lastBuildDate.GetLength() == 0)
		lastBuildDate = GetNamedNodeText(XML_PUBDATE_2, pparser, listptr->item[0], 1);

	COleDateTime lbd;
	BOOL bHaveLastbuild = FALSE;
	if(lastBuildDate.GetLength() > 0)
	{
		if(CDateParser::getDate(lastBuildDate, lbd) && lbd.m_dt != 0)
		{

			bHaveLastbuild = TRUE;

			// 파싱할 필요가 없다.
			if(lbd <= lasttime)
				return TRUE;
		}
		else
			lbd.SetTime(0, 0, 0);
	}

	listptr = pparser->SearchNodes(XML_DPXPATH_ITEM);
	
	IXMLDOMNodePtr nodeptr;

	BOOL read = FALSE
		, readon = FALSE
		, xmlupdate = FALSE
		, bDateError = FALSE;

	CString author
		, category
		, description
		, enclosure
		, guid
		, url
		, subject
		, strPubDate
		, encoded
		, comments
		, commentRss;

	COleDateTime pubdate;
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	
	for(int i=0; listptr->item[i] != NULL ; i++)
	{
		nodeptr = listptr->item[i];

		//	pubDate
		strPubDate = GetNamedNodeText(XML_PUBDATE_2, pparser, nodeptr, 1);
		if(strPubDate.IsEmpty())
		{
			strPubDate = GetNamedNodeText(XML_DC_DATE, pparser, nodeptr, 1);
		}
		//TRACE(_T("String Date : %s\n"), strPubDate);
		if(!strPubDate.IsEmpty())
		{
			if(!bDateError)
				bDateError = !(CDateParser::getDate(strPubDate, pubdate));

			if(bDateError) // Date Parsing 실패시.. 현재시기간 기준으로 시간 설정..
			{
				COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
				pubdate = currentTime + span;
				//return FALSE;
			}
		}
		else// pubdate 미지원 Feed 처리...
		{	// 파싱시간 기준으로 pubdate 입력. 신규post 추가시 guid 비교하므로 문제 안됨(?)
			COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
			pubdate = currentTime + span;
		}

		
		if(pubdate <= lasttime)
			break;

		if(!bHaveLastbuild && pubdate > lbd) lbd = pubdate;


		//	Subject
		subject		= GetNamedNodeText(XML_TITLE, pparser, nodeptr, 1);
				
		//	Author
		author		= GetNamedNodeText(XML_AUTHOR, pparser, nodeptr, 1);
		if(author.IsEmpty())
		{
			author	= GetNamedNodeText(XML_DC_CREATOR, pparser, nodeptr, 1);
		}

		//	Category
		category	= GetNamedNodeText(XML_CATEGORY, pparser, nodeptr, 1);
		if(category.IsEmpty())
		{
			category = GetNamedNodeText(XML_DC_SUBJECT, pparser, nodeptr, 1);
		}

		//	Decription
		description	= GetNamedNodeText(XML_DESCRIPTION, pparser, nodeptr, 1);

		if(subject.IsEmpty() && !description.IsEmpty())	// 제목이 비었을경우 본문내용에서.. 발췌
		{
			subject = description;
			RemoveAllTag(subject);
			subject = subject.Left(20);
			subject += _T("...");
		}

		//	URL
		url	= GetNamedNodeText(XML_LINK, pparser, nodeptr, 1);
		if(url.IsEmpty())
		{
			url = GetNamedNodeText(XML_URL, pparser, nodeptr, 1);
		}	

		// guid
		guid = GetNamedNodeText(XML_GUID, pparser, nodeptr, 1);

		if(url.IsEmpty() && guid.IsEmpty())
		{
			return FALSE;
		}

		if(url.IsEmpty())
			url = guid;

		if(guid.IsEmpty())
			guid = url;

		// enclosure
		enclosure = GetNamedNodeText(XML_ENCLOSURE, pparser, nodeptr, 1);

		// content : 아직 item에 포함 안됨
		encoded = GetNamedNodeText(XML_CONTENT_ENCODED, pparser, nodeptr, 1);
		if(description.IsEmpty())
			description = encoded;

		// comments : 아직 item에 포함 안됨
		comments = GetNamedNodeText(XML_COMMENTS, pparser, nodeptr, 1);

		// commentsRss : 아직 item에 포함 안됨
		commentRss = GetNamedNodeText(XML_WFW_COMMENTRSS, pparser, nodeptr, 1);

		//if(guid.GetLength() == 0) guid = url;

		// POSTITEM Insert
		plist->insert(plist->begin(),
						POSTITEM(1, channelid, read, readon, xmlupdate,	url, guid, author,
									category, subject, description, enclosure, pubdate,
									encoded, comments, commentRss));
	}

	lasttime = lbd;

	return TRUE;
}

/**************************************************************************
 * written by moonknit, aquarelle
 *
 * @history
 * created 2005-11-11
 * updated 2005-12-15 :: last update 정보가 없는 경우의 처리 by moonknit
 * updated 2006-01-27 :: 제목이 없는 경우 본분 내용에서 일부 발췌 by aquarelle
 * updated 2006-02-02 :: pubdate가 없는 Feed 처리 추가 by aquarelle
 * updated 2006-02-06 :: Date Parsing 실패시 처리 추가 by aquarelle
 *
 * @Description
 * RSS 1.0 파서
 **************************************************************************/
BOOL DefaultRSS10Parser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime)
{
	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pparser->SearchNodes(XML_DPXPATH_CHANNEL2);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	CString lastBuildDate = GetNamedNodeText(XML_DC_DATE, pparser, listptr->item[0], 1);
	COleDateTime lbd;

	BOOL bHaveLastbuild = FALSE;
	if(lastBuildDate.GetLength() > 0)
	{
		if(CDateParser::getDate(lastBuildDate, lbd))
		{
			bHaveLastbuild = TRUE;

			// 파싱할 필요가 없다.
			if(lbd <= lasttime)
				return TRUE;
		}
		else
			lbd.SetTime(0, 0, 0);
	}

	listptr = pparser->SearchNodes(XML_DPXPATH_ITEM2);
	
	IXMLDOMNodePtr nodeptr;

	BOOL read = FALSE
		, readon = FALSE
		, xmlupdate = FALSE
		, bDateError = FALSE;

	CString author
		, category
		, description
		, enclosure
		, guid
		, url
		, subject
		, strPubDate
		, encoded
		, commentRss;
	
	COleDateTime pubdate;
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	
	for(int i=0; listptr->item[i] != NULL ; i++)
	{
		nodeptr = listptr->item[i];
		
		//	pubDate
		strPubDate = GetNamedNodeText(XML_DC_DATE, pparser, nodeptr, 1);
		if(!strPubDate.IsEmpty())
		{
			if(!bDateError)
				bDateError = !(CDateParser::getDate(strPubDate, pubdate));

			if(bDateError) // Date Parsing 실패시.. 현재시기간 기준으로 시간 설정..
			{
				COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
				pubdate = currentTime + span;
				//return FALSE;
			}
		}
		else // pubdate 미지원 Feed 처리...
		{	// 파싱시간 기준으로 pubdate 입력. 신규post 추가시 guid 비교하므로 문제 안됨(?)
			COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
			pubdate = currentTime + span;
		}
		
		if(pubdate <= lasttime)
			break;

		if(!bHaveLastbuild && pubdate > lbd) lbd = pubdate;

		//	Subject
		subject		= GetNamedNodeText(XML_TITLE_F, pparser, nodeptr, 1);
				
		//	Author
		author	= GetNamedNodeText(XML_DC_CREATOR, pparser, nodeptr, 1);		

		//	Category
		category = GetNamedNodeText(XML_DC_SUBJECT, pparser, nodeptr, 1);		

		//	Decription
		description	= GetNamedNodeText(XML_DESCRIPTION_F, pparser, nodeptr, 1);		
		if(description.IsEmpty())
		{
			description = GetNamedNodeText(XML_DC_DESCRIPTION, pparser, nodeptr, 1);
		}

		if(subject.IsEmpty() && !description.IsEmpty())	// 제목이 비었을경우 본문내용에서.. 발췌
		{
			subject = description;
			RemoveAllTag(subject);
			subject = subject.Left(20);
			subject += _T("...");
		}

		//	URL
		url	= GetNamedNodeText(XML_LINK_F, pparser, nodeptr, 1);
		
		// guid
		guid = GetNamedNodeText(XML_GUID_F, pparser, nodeptr, 1);

		if(url.IsEmpty() && guid.IsEmpty())
		{
			return FALSE;
		}
		
		if(guid.IsEmpty())
			guid = url;		

		if(url.IsEmpty())
			url = guid;
		

		// enclosure
		enclosure = GetNamedNodeText(XML_ENCLOSURE_F, pparser, nodeptr, 1);

		// encoded : 추가 안함.
		encoded = GetNamedNodeText(XML_CONTENT_ENCODED, pparser, nodeptr, 1);
		
		if(description.IsEmpty())
			description = encoded;


		// commentsRss : 추가 안함.
		commentRss = GetNamedNodeText(XML_WFW_COMMENTRSS, pparser, nodeptr, 1);
		

		// POSTITEM Insert
		plist->insert(plist->begin(),
						POSTITEM(1, channelid, read, readon, xmlupdate,	url, guid, author,
									category, subject, description, enclosure, pubdate,
									encoded, _T(""), commentRss));
	}

	lasttime = lbd;

	return TRUE;
}


/**************************************************************************
 * written by aquarelle
 *
 * @history
 * created 2005-12-22 
 * updated 2006-01-27 :: 제목이 없는 경우 본분 내용에서 일부 발췌 by aquarelle
 * updated 2006-02-02 :: pubdate가 없는 Feed 처리 추가 by aquarelle
 * updated 2006-02-06 :: Date Parsing 실패시 처리 추가 by aquarelle
 *
 * @Description
 * Atom 파서 : 0.3, 1.0 공용
 **************************************************************************/
BOOL AtomParser(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime)
{
	IXMLDOMNodeListPtr listptr = NULL;
	listptr = pparser->SearchNodes(XML_DPXPATH_FEED_F);

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}
	
	CString lastBuildDate = GetNamedNodeText(XML_MODIFIED_F, pparser, listptr->item[0], 1);
	if(lastBuildDate.GetLength() == 0)
	{
		lastBuildDate = GetNamedNodeText(XML_UPDATED_F, pparser, listptr->item[0], 1);
	}
	
	COleDateTime lbd;

	BOOL bHaveLastbuild = FALSE;
	if(lastBuildDate.GetLength() > 0)
	{
		if(CDateParser::getDate(lastBuildDate, lbd))
		{
			bHaveLastbuild = TRUE;

			// 파싱할 필요가 없다.
			if(lbd <= lasttime)
				return TRUE;
		}
		else
			lbd.SetTime(0, 0, 0);
	}

	listptr = pparser->SearchNodes(XML_XPATH_ENTRY_F);

	int cnt = listptr->length;
	
	IXMLDOMNodePtr nodeptr, in_nodeptr;
	IXMLDOMNodeListPtr in_nodelistptr;

	BOOL read = FALSE
		, readon = FALSE
		, xmlupdate = FALSE
		, bDateError = FALSE;

	CString author
		, category
		, description
		, enclosure
		, guid
		, url
		, subject
		, strPubDate
		, encoded
		, commentRss;
	
	COleDateTime pubdate;
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	
	for(int i=0; listptr->item[i] != NULL ; i++)
	{
		nodeptr = listptr->item[i];
		
		//	pubDate
		strPubDate = GetNamedNodeText(XML_MODIFIED_F, pparser, nodeptr, 1);
		if(strPubDate.IsEmpty())
		{
			strPubDate = GetNamedNodeText(XML_UPDATED_F, pparser, nodeptr, 1);
		}
		if(strPubDate.IsEmpty())
		{
			strPubDate = GetNamedNodeText(XML_ISSUED_F, pparser, nodeptr, 1);
		}
		if(strPubDate.IsEmpty())
		{
			strPubDate = GetNamedNodeText(XML_CREATED_F, pparser, nodeptr, 1);
		}

		if(!strPubDate.IsEmpty())
		{
			if(!bDateError)
				bDateError = !(CDateParser::getDate(strPubDate, pubdate));

			if(bDateError) // Date Parsing 실패시.. 현재시기간 기준으로 시간 설정..
			{
				COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
				pubdate = currentTime + span;
				//return FALSE;
			}
		}
		else // pubdate 미지원 Feed 처리...
		{	// 파싱시간 기준으로 pubdate 입력. 신규post 추가시 guid 비교하므로 문제 안됨(?)
			COleDateTimeSpan span(0, 0, 0, listptr->Getlength() - i);
			pubdate = currentTime + span;
		}
		
		if(pubdate <= lasttime)
			break;

		if(!bHaveLastbuild && pubdate > lbd) lbd = pubdate;

		//	Subject
		subject		= GetNamedNodeText(XML_TITLE_F, pparser, nodeptr, 1);
				
		//	Author 
		author	= GetNamedNodeText(XML_XPATH_AUTHOR_NAME_F, pparser, nodeptr, 1);		

		//	Category
		category = GetNamedNodeText(XML_CATEGORY_F, pparser, nodeptr, 1);
		if(category.GetLength() == 0)
		{
			category = GetNamedNodeText(XML_DC_SUBJECT_F, pparser, nodeptr, 1);
		}

		//	Decription
		in_nodelistptr = pparser->SearchNodes(nodeptr, XML_CONTENT_F);
		if(in_nodelistptr->length == 0)
			in_nodelistptr = pparser->SearchNodes(nodeptr, XML_SUMMARY_F);

		in_nodeptr = in_nodelistptr->item[0];

		if(in_nodeptr != NULL)
		{
			if(GetNamedNodeText(XML_TYPE, pparser, in_nodeptr, 0) == _T("application/xhtml+xml"))
			{
				in_nodeptr = in_nodeptr->firstChild;
				description = _T("");
				while(in_nodeptr != NULL)
				{
					description	+= (LPTSTR) in_nodeptr->xml;
					description.TrimLeft();
					description += _T("\r\n");
					in_nodeptr = in_nodeptr->nextSibling;
				}
				description.TrimRight();
			}
			else
			{
				description	= (LPTSTR) in_nodeptr->text;
			}
		}

		if(subject.IsEmpty() && !description.IsEmpty())	// 제목이 비었을경우 본문내용에서.. 발췌
		{
			subject = description;
			RemoveAllTag(subject);
			subject = subject.Left(20);
			subject += _T("...");
		}

		//	URL, enclosure
		
		in_nodelistptr = pparser->SearchNodes(nodeptr, XML_LINK_F);
		
		for(int i = 0; in_nodelistptr->item[i] != NULL; i++)
		{
			in_nodeptr = in_nodelistptr->item[i];

			//	URL
			if(GetNamedNodeText(XML_REL, pparser, in_nodeptr, 0) == XML_TYPE_ALTERNATE)
			{			
				url	= GetNamedNodeText(_T("href"), pparser, in_nodeptr, 0);
			}
			// enclosure
			else if(GetNamedNodeText(XML_REL, pparser, in_nodeptr, 0) == XML_TYPE_ENCLOSURE)
			{
				enclosure = GetNamedNodeText(XML_HREF,pparser, in_nodeptr, 0);
			}
		}			
		
		// guid
		guid = GetNamedNodeText(XML_ID_F, pparser, nodeptr, 1);

		if(url.IsEmpty() && guid.IsEmpty())
		{
			return FALSE;
		}

		if(url.IsEmpty())
			url = guid;

		if(guid.IsEmpty())
			guid = url;

		// encoded : 추가 안함.
		//encoded = GetNamedNodeText(XML_CONTENT_ENCODED, pparser, nodeptr, 1);		

		// commentsRss : 추가 안함.
		//commentRss = GetNamedNodeText(XML_WFW_COMMENTRSS, pparser, nodeptr, 1);

		// POSTITEM Insert
		plist->insert(plist->begin(),
						POSTITEM(1, channelid, read, readon, xmlupdate,	url, guid, author,
									category, subject, description, enclosure, pubdate));
	}

	lasttime = lbd;

	return TRUE;
}