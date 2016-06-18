// FeedParser.h: interface for the CFeedParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDPARSER_H__B11E50C3_38C7_49F6_BE36_C1E0B9C98637__INCLUDED_)
#define AFX_FEEDPARSER_H__B11E50C3_38C7_49F6_BE36_C1E0B9C98637__INCLUDED_

/**************************************************************************
 * class CFeedParser
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-11-xx
 *
 * @Description
 * RSS XML 정보를 Parsing하여 내부에서 사용하는 구조체 데이터로 변환한다.
 *
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>

using namespace std;

class CEzXMLParser;

struct POSTITEM;
typedef list<POSTITEM> POST_LIST;

/*
 * RSSParser function
 * [Params]
 * struct Client* cptr   - connection message originated from
 * struct Client* sptr   - source of message, may be different from cptr
 * int            parc   - parameter count
 * char*          parv[] - parameter vector
 */
typedef BOOL (*RSSParser) (CEzXMLParser*, POST_LIST*, int, COleDateTime&);

class CFeedParser : public CObject 
{
	typedef map<int, RSSParser> MAP_PARSER;
	MAP_PARSER m_mapParser;

public:
	static BOOL Parsing(CEzXMLParser* pparser, POST_LIST* plist, int channelid, COleDateTime& lasttime, CString& v);
	CFeedParser();
	virtual ~CFeedParser();

protected:
	int LoadParser();
};

#endif // !defined(AFX_FEEDPARSER_H__B11E50C3_38C7_49F6_BE36_C1E0B9C98637__INCLUDED_)
