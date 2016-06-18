#ifndef _FISH_STRUCT_H
#define _FISH_STRUCT_H

#include "fish_def.h"
#include <list>
#include <map>
#include "./GnuModule/GnuDefines.h"

using namespace std;

#include "XML/EzXMLParser.h"

// 채널 작업시 Lock 처리를 위한 정의 
#define USE_CHANNELLOCK

// Work Process 의 처리 시간을 TRACE 하기위한 정의
#define CHECK_WORKTIME
// RSS DB 작업 중 오래된 파일을 열어서 작업을 수행할 수 있도록 해주는 정의
//#define USE_OLDFILE_DOWORK
// 검색시 모든 XML을 뒤져서 찾을 수 있도록 해주는 정의
//#define USE_FIND_ALLXML

enum SEARCH_TYPE
{
	NO_SEARCH = -1,
	NET_SEARCH = 0,
	KEYWORD_SEARCH,
	LOCAL_SEARCH,
	LONGTERM_SEARCH,
	GRAPH_SEARCH,
	TAG_NET_SEARCH,
	TAG_LOCAL_SEARCH,
	GUID_SEARCH,			// NETWORK SEARCH ONLY
	SEARH_TYPECNT,
};

#define SIFLAG_READ					0x01
#define SIFLAG_READON				0x02
#define SIFLAG_SCRAP				0x04
#define SIFLAG_FILTER				0x08
#define SIFLAG_POST					0x10
#define SIFLAG_GRAPH				0x20
#define SIFLAG_FILE					0x40

enum CHANNEL_TYPE
{
	CT_SUBSCRIBE = 0,
	CT_SCRAP,
	CT_KEYWORD,
	CT_SEARCH,
	CT_LSEARCH, // LOCAL SEARCH
	CT_NONE
};

enum GROUP_TYPE
{
	GT_SUBSCRIBE = 0,
	GT_SCRAP,
	GT_KEYWORD,
	GT_SEARCH
};

//////////////////////////////////////////////////////////////////////
// post struct
//////////////////////////////////////////////////////////////////////
struct TAG
{
	CString	t;
};

struct POSTNODE
{
	ULONG	ip;
	UINT	port;
	int		postid;

	POSTNODE(ULONG i, UINT p, int pi)
		: ip(i), port(p), postid(pi)
	{
	};
};

typedef list<TAG> TAG_LIST;
typedef list<POSTNODE> NODE_LIST;

struct POSTITEM
{
	int postid;
	int channelid;
	int chtype;					// temporary data
	int stype;					// 검색 종류
								// NO_SEARCH(0) : not search
								// LOCAL_SEARCH: local search
								// NET_SEARCH: net search
	BOOL read;
	BOOL readon;
	BOOL xmlupdate;
	BOOL bns;					// boolean of not-search

	CString url;
	CString guid;
	CString author;
	CString category;

	CString subject;
	CString description;
	CString enclosure;
	COleDateTime pubdate;

	// added by moonknit 2005-12-06
	// 검색된 결과에 value를 측정하기 위한 데이터
	// 검색된 아이템의 경우 
	int	svread;		// 검색결과의 read 개수
	int svreadon;	// 검색결과의 readon 개수
	int svscrap;	// 검색결과의 scarp 개수
	int svfilter;	// 검색결과의 filter 개수
	int svhit;		// 검색결과의 query hit 개수
	// --

	CString encoded;			// 인용구등을 나타냄. 없을경우 Description내용과 같음
	CString comments;			// 덧글 링크
	CString commentsRss;		// 덧글 RSS 파일 링크

    // added by eternalbleu 2006-01-03
    // stl container 에서 사용하기 위해서 복사 생성자 추가.
    // 기본 생성자에서 각 멤버변수를 초기화해서 DEBUG 모드에서 값이 NULL로 초기화 되도록함.
    POSTITEM(const POSTITEM& pi)
		: postid(pi.postid), channelid(pi.channelid), read(pi.read), readon(pi.readon), xmlupdate(pi.xmlupdate)
		, url(pi.url), guid(pi.guid), author(pi.author), category(pi.category)
		, subject(pi.subject), description(pi.description), enclosure(pi.enclosure), pubdate(pi.pubdate)
        , svread(pi.svread), svreadon(pi.svreadon), svscrap(pi.svscrap), svfilter(pi.svfilter), svhit(pi.svhit)
        , encoded(pi.encoded), comments(pi.comments), commentsRss(pi.commentsRss)
		, chtype(pi.chtype), stype(pi.stype)
    {
    }
        
    enum { MAX_VALUE = 0, };
	POSTITEM(int pid = 0, int cid = 0, BOOL r = FALSE, BOOL ron = FALSE, BOOL xu = FALSE
		, CString u = "", CString g = "", CString a = "", CString c = ""
		, CString s = "", CString d = "", CString e = "", COleDateTime pub = COleDateTime()
		, CString en = "", CString co = "", CString cr = "")
		: postid(pid), channelid(cid), read(r), readon(ron), xmlupdate(xu)
		, url(u), guid(g), author(a), category(c)
		, subject(s), description(d), enclosure(e), pubdate(pub)
        , svread(MAX_VALUE), svreadon(MAX_VALUE), svscrap(MAX_VALUE), svfilter(MAX_VALUE), svhit(MAX_VALUE)
        , encoded(en), comments(co), commentsRss(cr)
		, chtype(CT_NONE), stype(NO_SEARCH)
	{
	}

	~POSTITEM()
	{
	}
	
};
// -- post struct

typedef list<POSTITEM> POST_LIST;

//////////////////////////////////////////////////////////////////////
// RSS DATA STRUCT
//////////////////////////////////////////////////////////////////////

struct TERM
{
	COleDateTime	from;
	COleDateTime	to;

	TERM()
	{
	};

	BOOL operator == (const TERM t)
	{
		return (from == t.from && to == t.to);
	};

	const BOOL isNULL()
	{
		if(from.m_dt != 0)
		{
			if(to.m_dt != 0)
				return from >= to;
			else
				return FALSE;
		}

		return TRUE;
	}

	const BOOL haveAll(const COleDateTime& t)
	{
		if(from.m_dt == 0) 
		{
			if(to.m_dt != 0)
				return t < to;

			return TRUE;
		}

		if(to.m_dt != 0)
		{
			return (from <= t) & (t < to);
		}

		return (from <= t);
	}

	const BOOL haveAll(const TERM& t)
	{
		return haveAll(t.from) && haveAll(t.to);
	}

	const BOOL haveSome(const TERM& t)
	{
		return haveAll(t.from) || haveAll(t.to);
	}

	static const BOOL parsetermunit(CString stu, COleDateTimeSpan& ts)
	{
		int l = stu.GetLength();
		int n = _ttoi((LPCTSTR) stu.Mid(0, l - 1));

		if(n == 0) return FALSE;

		char c = stu.GetAt(l - 1);
		COleDateTimeSpan s;
		switch (c)
		{
		case 'M':
		case 'm':
			s.SetDateTimeSpan(n*30, 0, 0, 0);
			break;
		case 'D':
		case 'd':
			s.SetDateTimeSpan(n, 0, 0, 0);
			break;
		case 'H':
		case 'h':
			s.SetDateTimeSpan(0, n, 0, 0);
			break;
		}

		ts = s;

		return TRUE;
	}

	static const BOOL parseterm(CString st, COleDateTime& from, COleDateTimeSpan& s)
	{
		int i = st.Find(_T("_"));
		if(i != 8) return FALSE;

		CString f = st.Left(i);
		CString t = st.Mid(i+1);

		if(_ttoi((LPCTSTR) f) == 0) return FALSE;

		int YY, mm, dd, HH;
		YY = _ttoi((LPCTSTR) f.Mid(0, 2));			// YY
		mm = _ttoi((LPCTSTR) f.Mid(2, 2));			// mm
		dd = _ttoi((LPCTSTR) f.Mid(4, 2));			// dd
		HH = _ttoi((LPCTSTR) f.Mid(6, 2));			// HH

		if(mm == 0 || dd == 0) return FALSE;

		if(!parsetermunit(t, s)) return FALSE;

		from.SetDateTime(2000 + YY, mm, dd, HH, 0, 0);

		return TRUE;

	}

	static const BOOL parseterm(CString st, TERM& dest)
	{
		TERM t;

		if(st.GetLength() == 0)
		{
//			t.to = COleDateTime::GetCurrentTime();
			dest = t;
			return TRUE;
		}

		COleDateTimeSpan s;

		if(!parseterm(st, t.from, s)) return FALSE;

		t.to = t.from + s;

		dest = t;
		return TRUE;
	}

	// gmt must be as 1, 1.5, 2, 2.5, ... 0.5, -0.5, -1.0, -1.5 ...
	static const BOOL parsegmt(CString sg, double* pf)
	{
		double f = 0.0;
		char c;
		BOOL bminus = FALSE;

		int l = sg.GetLength();

		if(l == 0 || sg == _T("0")) f = 0;
		else
		{
			c = sg.GetAt(l-1);
			if(c == '-') bminus = TRUE;

			TCHAR* stopstring;

			f = _tcstod((LPCTSTR) sg.Mid(0, l-1), &stopstring);
			if(f == 0.0) return FALSE;
		}

		*pf = (bminus) ? 0 - f : f;
		return TRUE;
	}

	static const BOOL convertgmt(const double gmt, COleDateTimeSpan& s)
	{
		if(gmt <= 12 && gmt >= -12)
		{
			int h = (int) gmt;
			double hm = gmt - h;
			int m = (int) (hm * 60);
			s.SetDateTimeSpan(0, h, m, 0); 
			return TRUE;
		}

		return FALSE;
	}

	const TERM& operator+=(const COleDateTimeSpan s)
	{
		if(from.m_dt != 0) from = from + s;
		if(to.m_dt != 0) to = to + s;
		
		return (*this);
	}

	const TERM& operator-=(const COleDateTimeSpan s)
	{
		if(from.m_dt != 0) from = from - s;
		if(to.m_dt != 0) to = to - s;
		
		return (*this);
	}

	const TERM& operator+=(const double gmt)
	{
		COleDateTimeSpan s;
		if(convertgmt(gmt, s))
		{
			*this += s;
		}
		return (*this);
	}

};

// 채널의 특정 기간의 post 정보를 관리
struct RSS_XMLDATA
{
	BOOL				flush;
	int					load;
	int					unread;
	int					postcnt;
	int					type;					// channel type
	TERM				term;					// 포함된 포스트pubdate가 속하는 기간
	CString				path;
	COleDateTime		flushtime;
	SmartPtr<CEzXMLParser> XML;

	RSS_XMLDATA(TERM t, CEzXMLParser* x, int ty = CT_SUBSCRIBE)
		: flush(TRUE), term(t), path(""), XML(x), type(ty), load(1), unread(0), postcnt(0)
	{
		flushtime = COleDateTime::GetCurrentTime();
	};

	RSS_XMLDATA(TERM t, CString p, CEzXMLParser* x, int ty = CT_SUBSCRIBE)
		: flush(TRUE), term(t), path(p), XML(x), type(ty), load(1), unread(0), postcnt(0)
	{
		flushtime = COleDateTime::GetCurrentTime();
	};

	~RSS_XMLDATA()
	{
	};

	bool operator > (const RSS_XMLDATA& d) const
	{
		return (term.from > d.term.from ? true : false) ;
	};
};

typedef list<RSS_XMLDATA> XML_LIST;
typedef map<CString, int > MAP_GUIDMATCH;		// post guid to post id

// 하나의 채널의 post와 관련된 모든 정보를 관리한다 
struct RSS_CHANNELDATA
{
	BOOL				flush;
	BOOL				oldused;
	BOOL				bsearchlog;			// 검색 채널의 경우 임의로 파일 기록 설정을 처리하지 않으면
											// 파일로 기록을 남기지 않는다.
	BOOL				bns;				// boolean of not-search

	DWORD				exFlag;				// 채널의 추가적인 데이터 값 처리를 위한 데이터 (BOOLEAN FLAG array)

	int					type;				// channel type
	int					unread;				// 현재 채널의 읽지 않은 포스트 수
	int					postcnt;			// 현재 채널의 포스트 수
	int					keepcnt;			// 채널의 최대 포스트 수
	int					keepdays;			// 포스트가 유지되는 기간

	CString				guid;
	COleDateTime		dtDivid;
	COleDateTime		dtFlush;
	COleDateTime		dtOldUseTime;
	COleDateTime		dtLastPost;			// 채널의 oldest post의 시간
	XML_LIST			listData;
	XML_LIST			listOldData;
	MAP_GUIDMATCH		mapGuid;			// 현재 채널에 포함된 포스트의 guid 목록

	SmartPtr<CCriticalSection>	spcs;

	RSS_CHANNELDATA(int t = CT_SUBSCRIBE)
		: flush(FALSE), type(t), unread(0), oldused(FALSE)
		, bsearchlog(FALSE), postcnt(0), guid(_T("")), bns(FALSE), exFlag(0)
		, islock(FALSE), keepcnt(0), keepdays(0)
	{
		spcs = SmartPtr<CCriticalSection> (new CCriticalSection);
	}

	~RSS_CHANNELDATA()
	{
	}

	BOOL IsLock()
	{
		return islock;
	}

	BOOL ChannelLock(int ntime = 1000)
	{
#ifdef USE_CHANNELLOCK
		if(!spcs) return FALSE;

		if(islock)
		{
			Sleep(10);
			return FALSE;
		}

		if(spcs->Lock(ntime))
		{
			islock = TRUE;
			return TRUE;
		}
		return FALSE;
#else
		return TRUE;
#endif
	}

	void ChannelUnlock()
	{
#ifdef USE_CHANNELLOCK
		if(!spcs) return;

		spcs->Unlock();
		islock = FALSE;
#endif
	}
private:
	BOOL				islock;
};

// key는 channel id
// data는 RSS_CHANNELDATA
typedef map<int, RSS_CHANNELDATA> XML_MAP;

/*
struct POST_MATCH
{
	int					channelid;
	COleDateTime		pubdate;

	POST_MATCH(int cid, COleDateTime pd)
		: channelid(cid), pubdate(pd)
	{
	};
};

typedef map<int, POST_MATCH> MAP_POSTMATCH;
*/
typedef map<int, int> MAP_POSTMATCH;			// post id to channel id
//////////////////////////////////////////////////////////////////////
// Subscribe structure
//////////////////////////////////////////////////////////////////////

struct FILTER_CONDITION
{
	int					type;					// 0 : keyword 자동 검색, 1 : search
	CString				text;

    FILTER_CONDITION(int t = 0, CString txt = _T("") ) : type(t), text(txt) {}
};

enum REP_TYPE
{
	REP_CHANNEL = 0,
	REP_GROUP
};

struct FISH_REPITEM
{
	int					id;						// 기록
	int					unreadcnt;
	int					postcnt;
	int					seq;
	int					unreadstrwidth;
	REP_TYPE            rep_type;				// 기록
	CString				title;					// 기록
	CString				comment;				// 기록

	virtual ~FISH_REPITEM()
	{
	}

protected:
	FISH_REPITEM(int id, CString t, CString c, REP_TYPE ty)
		: id(id), title(t), comment(c), rep_type(ty), unreadcnt(0), postcnt(0), unreadstrwidth(0)
	{
	}
};

struct FISH_CHANNEL : public FISH_REPITEM
{
	BOOL				flush;					// OPML로 기록할지 여부... comment by aquarelle
	BOOL				searchlog;				// 검색한 결과를 기록에 남길 것인지 여부 
												// type == CT_SEARCH 또는 CT_LSEARCH 인 경우에만 유효하다.

	BOOL				bns;					// boolean of not-search

	int					groupid;
	int					feedinterval;			// 기록
	int					status;
	CHANNEL_TYPE        type;					// CHANNEL_TYPE, 기록
	int					dateparsetype;			// date parsing 시 사용하는 type

	int					keepcnt;				// 채널의 최대 보관 개수
	int					keepdays;				// 포스트가 유지되는 기간

	COleDateTime		dtlastfeed;
	COleDateTime		dtlastbuild;			// rss last build datetime, 기록

	CString				xmlurl;					// XML 데이터를 얻어올 때 사용하는 URL, 기록
	CString				churl;					// channel 에 등록된 url, rss 2.0 의 link, 기록
	CString				rsstype;				// rss의 종류
	CString				guid;

	FILTER_CONDITION	fc;						// 

	FISH_CHANNEL(int id, CString title)
		: FISH_REPITEM(id, title, _T(""), REP_CHANNEL), dtlastfeed(COleDateTime::GetCurrentTime())
		, bns(FALSE), keepcnt(0), keepdays(0), groupid(0), feedinterval(0)
		, status(0), dateparsetype(0)
	{
	}

	FISH_CHANNEL(int id, int gid, CString t, CString c, int fi
		, CHANNEL_TYPE chtype, CString url, CString guid, BOOL bns
		, CString htmlurl = _T(""), int keepcnt = 0, int keepdays = 0)
		: FISH_REPITEM(id, t, c, REP_CHANNEL), dtlastfeed(COleDateTime::GetCurrentTime())
		, groupid(gid), feedinterval(fi), type(chtype), churl(htmlurl)
		, searchlog(FALSE), xmlurl(url), guid(guid), bns(bns)
		, keepcnt(keepcnt), keepdays(keepdays)
	{
		if(chtype == CT_SEARCH || chtype == CT_LSEARCH)
			flush = FALSE;
		else
			flush = TRUE;
	}

	FISH_CHANNEL(int id, int gid, CString t, CString c, int fi
		, CHANNEL_TYPE chtype, CString guid, BOOL bns, FILTER_CONDITION fc = FILTER_CONDITION(0, _T(""))
		, BOOL bForceSave = FALSE
		)
		: FISH_REPITEM(id, t, c, REP_CHANNEL), dtlastfeed(COleDateTime::GetCurrentTime())
		, groupid(gid), feedinterval(fi), type(chtype), fc(fc)
		, searchlog(FALSE), xmlurl(fc.text), guid(guid), bns(bns)
		, keepcnt(0), keepdays(0)
	{
		if(chtype == CT_SEARCH || chtype == CT_LSEARCH)
			flush = bForceSave;
		else
			flush = TRUE;
	}

	~FISH_CHANNEL()
	{
	}
};

typedef list<int> GCHANNEL_LIST;

struct FISH_GROUP : public FISH_REPITEM
{
	BOOL				expand;
	GROUP_TYPE			type; // added by aquarelle  - 2006/02/27

	FISH_GROUP(int id, GROUP_TYPE gtype = GT_SUBSCRIBE)
		: FISH_REPITEM(id, _T(""), _T(""), REP_GROUP), type(gtype)
	{		
	}

	FISH_GROUP(int id, CString t, CString c, GROUP_TYPE gtype = GT_SUBSCRIBE)
		: FISH_REPITEM(id, t, c, REP_GROUP), expand(TRUE), type(gtype)
	{
	}

	~FISH_GROUP()
	{
	}
};

enum REPITEM_TYPE
{
	RIT_CHANNEL = 0,
	RIT_GROUP
};

typedef map<int, FISH_GROUP > GROUP_MAP;
typedef map<int, FISH_CHANNEL > CHANNEL_MAP;



// -- RSS DATA STRUCT

//////////////////////////////////////////////////////////////////////
// FEED ITEM STRUCT
//////////////////////////////////////////////////////////////////////
struct FETCH_ITEM
{
	int channelid;
	COleDateTime dtlastbuild;

	FETCH_ITEM (int c, COleDateTime dt)
		: channelid(c), dtlastbuild(dt)
	{
	}
};

typedef list<FETCH_ITEM> FETCH_LIST;

//////////////////////////////////////////////////////////////////////
// work struct used by in-queue
//////////////////////////////////////////////////////////////////////
enum WORK_TYPE
{
	WT_UNDEFINED = 0,
	WT_POSTDATA,
	WT_POSTADD,
	WT_POSTID,
	WT_UPLOAD,
	WT_CHANNEL,
	WT_SEARCH
};

enum RESULT_TYPE
{
	RT_UNDEFINED = 0,
	RT_NORMAL,
	RT_POST,
	RT_POSTADD, 
	RT_UPLOAD,
	RT_SEARCH
};

enum WORK_SUBTYPE
{
	WS_UNDEFINED = 0,
	WS_POST_ADD,
	WS_POST_UPDATE,
	WS_POST_GET,
	WS_POST_DELETE,
	WS_POST_SCRAP,
	WS_CHANNEL_ADD,
	WS_CHANNEL_DELETE,
	WS_CHANNEL_GET,
	WS_CHANNEL_LOAD,
	WS_CHANNEL_FLUSH,
	WS_CHANNEL_MODIFY,
	WS_CHANNEL_READ,
	WS_CHANNEL_SETPATH
};

enum CHANNELMODIFY_DEST
{
	CMD_SEARCHLOG = 0,
	CMD_BNSFLAG
};

enum ADD_TYPE
{
	AT_NORMAL = 0,
	AT_FETCH,
	AT_SEARCH_LOCAL,
	AT_SEARCH_NET
};

typedef list<int> ID_LIST;

struct WORK
{
	const int		workid;
	const int		worktype;
	const int		subtype;

	virtual ~WORK()
	{
	};


protected:
	WORK(int id, int type, int stype)
		: workid(id), worktype(type), subtype(stype)
	{
	}
};

struct WorkAddPost : public WORK
{
	const			COleDateTime t;
	int				addtype;
	int				channelid;
	GUID			guid;
	auto_ptr<POST_LIST> aplistpost;

	WorkAddPost(int id, int stype, auto_ptr<POST_LIST>& items, const COleDateTime& t
		, int at = AT_NORMAL, int channelid = 0)
		: WORK(id, WT_POSTADD, stype), t(t), addtype(at), channelid(channelid)
	{
		aplistpost = items;
	}

	WorkAddPost(int id, int stype, auto_ptr<POST_LIST>& items, const COleDateTime& t
		, GUID guid, int at = AT_NORMAL, int channelid = 0)
		: WORK(id, WT_POSTADD, stype), t(t), addtype(at), guid(guid), channelid(channelid)
	{
		aplistpost = items;
	}

	~WorkAddPost()
	{
	}
};

struct WorkPostData : public WORK
{
	int				adinfo;
	auto_ptr<POST_LIST> aplistpost;

	WorkPostData(int id, int stype, auto_ptr<POST_LIST>& items, int ai = 0)
		: WORK(id, WT_POSTDATA, stype), adinfo(ai)
	{
		aplistpost = items;
	};

	~WorkPostData()
	{
	}
};

struct WorkChannel : public WORK
{
	int				adinfo;
	int				addata;
	int				addataEx[10];
	int				channelid;
	CString			text;
	CString			textEx;

	WorkChannel(int id, int stype, int chid, int ai = 0
		, int ad = 0, CString t = _T(""), CString tE = _T(""))
		: WORK(id, WT_CHANNEL, stype), channelid(chid), text(t), adinfo(ai)
		, addata(ad), textEx(tE)
	{
	}

	~WorkChannel()
	{
	}
};

struct WorkPostID : public WORK
{
	int				adinfo;
	int				channelid;
	auto_ptr<ID_LIST>	aplistid;

	WorkPostID(int id, int stype, auto_ptr<ID_LIST>& ids, int ai = 0, int chid = 0)
		: WORK(id, WT_POSTID, stype), adinfo(ai), channelid(chid)
	{
		aplistid = ids;
	}

	~WorkPostID()
	{
	}
};

struct WorkUpload : public WORK
{
	LPVOID lpuploader;
	int postid;

	WorkUpload(int id, int postid, LPVOID lpuploader)
		: WORK(id, WT_UPLOAD, 0), postid(postid), lpuploader(lpuploader)
	{
	}

	~WorkUpload()
	{
	}
};

// -- work struct used by in-queue

//////////////////////////////////////////////////////////////////////
// result struct used by out-queue
//////////////////////////////////////////////////////////////////////
struct RESULT
{
	const int		workid;
	const int		resulttype;
	const int		subtype;

	virtual ~RESULT()
	{
	};

protected:
	RESULT(int id, int type, int stype)
		: workid(id), resulttype(type), subtype(stype)
	{
	}
};

struct ResultNormal : public RESULT
{
	int				addinfo;
	BOOL			result;
	CString			err;

	ResultNormal(int id, int stype, BOOL r, CString e = _T(""), int ai = 0)
		: RESULT(id, RT_NORMAL, stype), result(r), addinfo(ai), err(e)
	{
	}

	~ResultNormal()
	{
	}
};

struct ResultAddPost : public RESULT
{
	BOOL			result;
	int				addtype;
	int				channelid;
	int				postcnt;
	int				Ex;
	GUID			guid;
	CString			err;
	const COleDateTime t;
	auto_ptr<POST_LIST> aplistpost;

	ResultAddPost(int id, int stype, BOOL r, const COleDateTime& t
		, auto_ptr<POST_LIST>& aplist
		, GUID guid
		, CString e = _T("")
		, int at = AT_NORMAL, int channelid = 0, int postcnt = 0)
		: RESULT(id, RT_POSTADD, stype), result(r), err(e), t(t)
		, addtype(at), channelid(channelid), aplistpost(aplist), postcnt(postcnt)
		, guid(guid)
	{
		aplist = auto_ptr<POST_LIST> (NULL);
	}

	~ResultAddPost()
	{
		if(aplistpost.get != 0)
			aplistpost->clear();
	}
};

struct ResultPost : public RESULT
{
	int					addinfo;
	BOOL				result;
	auto_ptr<POST_LIST> aplistpost;
	CString				err;

	ResultPost(int id, int stype, BOOL r, auto_ptr<POST_LIST>& plist, CString e = _T(""), int ai = 0)
		: RESULT(id, RT_POST, stype), addinfo(ai), result(r), err(e)
	{
		aplistpost = plist;
	}

	~ResultPost()
	{
		if(aplistpost.get() != 0)
			aplistpost->clear();
	}
};

struct ResultUpload : public RESULT
{
	BOOL result;
	LPVOID lpuploader;
	CString xml;

	ResultUpload(int id, BOOL r, CString xml, LPVOID lpuploader)
		: RESULT(id, RT_UPLOAD, 0), result(r), xml(xml), lpuploader(lpuploader)
	{
	}

	~ResultUpload()
	{
	}
};

struct SearchedItem
{
	int			id;			// postid or postdatetime
	BOOL		bread;
	BOOL		breadon;
	BOOL		bscrap;
	BOOL		bfilter;
	time_t		pubdate;
	CString		sguid;
	CString		t;			// text data
	CString		link;
};

typedef list<SearchedItem> SITEM_LIST;

struct ResultSearch : public RESULT
{
	SEARCH_TYPE type;		// 검색 결과의 종류
	int			chid;		// local 검색 결과를 알리기위한 채널 id
	SE_QUERY	querydata;

	auto_ptr<SITEM_LIST> apitems;
	auto_ptr<POST_LIST> aplistpost;

	ResultSearch(int id, SEARCH_TYPE t)
		: RESULT(id, RT_SEARCH, 0), type(t)
	{
		querydata.QueryGuid = NULL_GUID;
		if(t == LOCAL_SEARCH || t == KEYWORD_SEARCH)
		{
			aplistpost = auto_ptr<POST_LIST> (new POST_LIST);
		}
	}


	ResultSearch(int id, SEARCH_TYPE t, SE_QUERY q)
		: RESULT(id, RT_SEARCH, 0), type(t), querydata(q)
	{
		switch(t)
		{
		case LOCAL_SEARCH:
			aplistpost = auto_ptr<POST_LIST> (new POST_LIST);
			break;
		case NET_SEARCH:
		case LONGTERM_SEARCH:
		case GRAPH_SEARCH:
			apitems = auto_ptr<SITEM_LIST> (new SITEM_LIST);
			break;
		}
	}

	~ResultSearch()
	{
	}
};


// ** ResultSearch를 멤버로 가지므로 ResultSearch의 선언 뒤에 나와야 한다.
struct WorkSearch : public WORK
{
	SEARCH_TYPE		type;
	BOOL			valid;
	int				nmaxcnt;				// 최대 결과 개수
	int				nchannelcnt;			// 작업 처리한 채널의 개수
	int				chid;					// LOCAL SEARCH인 경우 포스터 정보를 바로 업데이트 하기 위해 채널 id를 가진다.
	TERM			t;

	CString			lcode;					// 언어코드
	CString			keyword;
	auto_ptr<ResultSearch>	apresult;
	
	WorkSearch(int id, SEARCH_TYPE type, CString key
		, CString lcode, CString sterm, int ncnt, int chid)
		: WORK(id, WT_SEARCH, 0), type(type), keyword(key)
		, lcode(lcode), nmaxcnt(ncnt), nchannelcnt(0), chid(chid)
	{
		// 결과 정보 설정
		apresult = auto_ptr<ResultSearch> (new ResultSearch(id, type));
		apresult->chid = chid;

		valid = TERM::parseterm(sterm, t);
	}

	WorkSearch(int id, SEARCH_TYPE type, CString key
		, CString lcode, CString sterm, int ncnt, SE_QUERY q, int chid = 0)
		: WORK(id, WT_SEARCH, 0), type(type), keyword(key)
		, lcode(lcode), nmaxcnt(ncnt), nchannelcnt(0), chid(chid)
	{
		// 결과 정보 설정
		apresult = auto_ptr<ResultSearch> (new ResultSearch(id, type, q));

		valid = TERM::parseterm(sterm, t);
	}

	const BOOL IsComplete()
	{
		if(nmaxcnt > 0)
		{
			if(apresult->apitems.get()) 
			{
				if(type == CT_SEARCH && apresult->apitems->size() >= MAX_SEARCHCOUNT) return TRUE;
			}
			if(apresult->aplistpost.get()) return nmaxcnt <= apresult->aplistpost->size();
		}

		return FALSE;
	}

	~WorkSearch()
	{
	}
};

typedef list< auto_ptr<WorkSearch> > SEARCH_LIST;
// -- result struct used by out-queue

typedef list< auto_ptr<RESULT> > RESULT_LIST;
typedef list< auto_ptr<WORK> > WORK_LIST;

//////////////////////////////////////////////////////////////////////
// key word filtering
//////////////////////////////////////////////////////////////////////
struct KEYWORD_FILTER
{
	int			channelid;
	CString		keyword;

	KEYWORD_FILTER(int id, CString k)
		: channelid(id), keyword(k)
	{
	};
};

typedef list<KEYWORD_FILTER> KEYWORD_LIST;

// -- key word filtering



#endif