#ifndef _FISH_STRUCT_H
#define _FISH_STRUCT_H

#include "fish_def.h"
#include <list>
#include <map>
#include "./GnuModule/GnuDefines.h"

using namespace std;

#include "XML/EzXMLParser.h"

// ä�� �۾��� Lock ó���� ���� ���� 
#define USE_CHANNELLOCK

// Work Process �� ó�� �ð��� TRACE �ϱ����� ����
#define CHECK_WORKTIME
// RSS DB �۾� �� ������ ������ ��� �۾��� ������ �� �ֵ��� ���ִ� ����
//#define USE_OLDFILE_DOWORK
// �˻��� ��� XML�� ������ ã�� �� �ֵ��� ���ִ� ����
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
	int stype;					// �˻� ����
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
	// �˻��� ����� value�� �����ϱ� ���� ������
	// �˻��� �������� ��� 
	int	svread;		// �˻������ read ����
	int svreadon;	// �˻������ readon ����
	int svscrap;	// �˻������ scarp ����
	int svfilter;	// �˻������ filter ����
	int svhit;		// �˻������ query hit ����
	// --

	CString encoded;			// �ο뱸���� ��Ÿ��. ������� Description����� ����
	CString comments;			// ���� ��ũ
	CString commentsRss;		// ���� RSS ���� ��ũ

    // added by eternalbleu 2006-01-03
    // stl container ���� ����ϱ� ���ؼ� ���� ������ �߰�.
    // �⺻ �����ڿ��� �� ��������� �ʱ�ȭ�ؼ� DEBUG ��忡�� ���� NULL�� �ʱ�ȭ �ǵ�����.
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

// ä���� Ư�� �Ⱓ�� post ������ ����
struct RSS_XMLDATA
{
	BOOL				flush;
	int					load;
	int					unread;
	int					postcnt;
	int					type;					// channel type
	TERM				term;					// ���Ե� ����Ʈpubdate�� ���ϴ� �Ⱓ
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

// �ϳ��� ä���� post�� ���õ� ��� ������ �����Ѵ� 
struct RSS_CHANNELDATA
{
	BOOL				flush;
	BOOL				oldused;
	BOOL				bsearchlog;			// �˻� ä���� ��� ���Ƿ� ���� ��� ������ ó������ ������
											// ���Ϸ� ����� ������ �ʴ´�.
	BOOL				bns;				// boolean of not-search

	DWORD				exFlag;				// ä���� �߰����� ������ �� ó���� ���� ������ (BOOLEAN FLAG array)

	int					type;				// channel type
	int					unread;				// ���� ä���� ���� ���� ����Ʈ ��
	int					postcnt;			// ���� ä���� ����Ʈ ��
	int					keepcnt;			// ä���� �ִ� ����Ʈ ��
	int					keepdays;			// ����Ʈ�� �����Ǵ� �Ⱓ

	CString				guid;
	COleDateTime		dtDivid;
	COleDateTime		dtFlush;
	COleDateTime		dtOldUseTime;
	COleDateTime		dtLastPost;			// ä���� oldest post�� �ð�
	XML_LIST			listData;
	XML_LIST			listOldData;
	MAP_GUIDMATCH		mapGuid;			// ���� ä�ο� ���Ե� ����Ʈ�� guid ���

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

// key�� channel id
// data�� RSS_CHANNELDATA
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
	int					type;					// 0 : keyword �ڵ� �˻�, 1 : search
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
	int					id;						// ���
	int					unreadcnt;
	int					postcnt;
	int					seq;
	int					unreadstrwidth;
	REP_TYPE            rep_type;				// ���
	CString				title;					// ���
	CString				comment;				// ���

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
	BOOL				flush;					// OPML�� ������� ����... comment by aquarelle
	BOOL				searchlog;				// �˻��� ����� ��Ͽ� ���� ������ ���� 
												// type == CT_SEARCH �Ǵ� CT_LSEARCH �� ��쿡�� ��ȿ�ϴ�.

	BOOL				bns;					// boolean of not-search

	int					groupid;
	int					feedinterval;			// ���
	int					status;
	CHANNEL_TYPE        type;					// CHANNEL_TYPE, ���
	int					dateparsetype;			// date parsing �� ����ϴ� type

	int					keepcnt;				// ä���� �ִ� ���� ����
	int					keepdays;				// ����Ʈ�� �����Ǵ� �Ⱓ

	COleDateTime		dtlastfeed;
	COleDateTime		dtlastbuild;			// rss last build datetime, ���

	CString				xmlurl;					// XML �����͸� ���� �� ����ϴ� URL, ���
	CString				churl;					// channel �� ��ϵ� url, rss 2.0 �� link, ���
	CString				rsstype;				// rss�� ����
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
	SEARCH_TYPE type;		// �˻� ����� ����
	int			chid;		// local �˻� ����� �˸������� ä�� id
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


// ** ResultSearch�� ����� �����Ƿ� ResultSearch�� ���� �ڿ� ���;� �Ѵ�.
struct WorkSearch : public WORK
{
	SEARCH_TYPE		type;
	BOOL			valid;
	int				nmaxcnt;				// �ִ� ��� ����
	int				nchannelcnt;			// �۾� ó���� ä���� ����
	int				chid;					// LOCAL SEARCH�� ��� ������ ������ �ٷ� ������Ʈ �ϱ� ���� ä�� id�� ������.
	TERM			t;

	CString			lcode;					// ����ڵ�
	CString			keyword;
	auto_ptr<ResultSearch>	apresult;
	
	WorkSearch(int id, SEARCH_TYPE type, CString key
		, CString lcode, CString sterm, int ncnt, int chid)
		: WORK(id, WT_SEARCH, 0), type(type), keyword(key)
		, lcode(lcode), nmaxcnt(ncnt), nchannelcnt(0), chid(chid)
	{
		// ��� ���� ����
		apresult = auto_ptr<ResultSearch> (new ResultSearch(id, type));
		apresult->chid = chid;

		valid = TERM::parseterm(sterm, t);
	}

	WorkSearch(int id, SEARCH_TYPE type, CString key
		, CString lcode, CString sterm, int ncnt, SE_QUERY q, int chid = 0)
		: WORK(id, WT_SEARCH, 0), type(type), keyword(key)
		, lcode(lcode), nmaxcnt(ncnt), nchannelcnt(0), chid(chid)
	{
		// ��� ���� ����
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