// RSSDB.h: interface for the CRSSDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSSDB_H__226D62EB_8F81_4E42_B9D7_B64B058AFDFD__INCLUDED_)
#define AFX_RSSDB_H__226D62EB_8F81_4E42_B9D7_B64B058AFDFD__INCLUDED_

/**************************************************************************
 * class CRSSDB
 * 
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-10-xx
 *
 * @Description
 * Post 정보를 관리하는 클래스.
 * Post 정보를 입출력하는 Thread와
 * Post 검색을 처리하는 Thread, 
 * 두 개의 Thread가 Main Thread에 영향을 주지 않기 위해 생성되어 동작한다.
 **************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "fish_struct.h"
#include "./GnuModule/GnuDefines.h"			// SE_QUERY

#define USE_FIND_ALLXML

class CRSSDB  
{
public:
	static void SetWorking(BOOL b) { m_bWorking = b; }
	static BOOL IsWorking() { return m_bWorking; }

	static void SetPostInit(BOOL b) { m_bPostInit = b; }
	static BOOL IsPostInit() { return m_bPostInit; }

	BOOL IsExistKeyword(const CString& keyword);

	// INTERFACE
	void ProcessRSSResult();
	BOOL GetResult(auto_ptr<RESULT>& apresult);

	int IRequestUpload(LPVOID lpuploader, int postid);

	int IRequestSearch(SEARCH_TYPE t, CString key, CString lcode, CString sterm, int ncnt, int chid = 0);
	int IRequestNetSearch(SEARCH_TYPE t, CString key, CString lcode, CString sterm, int ncnt, SE_QUERY q, int chid = 0);

	int IFlush();

	int IChannelSetPath(CString path);

	int IChannelLoad();
	int IChannelDelete(int channelid, BOOL dataonly = TRUE);
	int IChannelGet(int channelid, int gettype = FISH_GET_DEFAULT);
	int IChannelAdd(int channelid, int chtype = CT_SUBSCRIBE
		, BOOL bnotsearch = FALSE, CString text = _T("")
		, CString keyword = _T(""), BOOL bLog = TRUE
		, int keepcnt = 0, int keepdays = 0);
	int IChannelRead(int channelid, BOOL bread = TRUE);

	int IChannelNotSearch(int channelid, BOOL flag);
	int IChannelSearchLog(int channelid, BOOL flag);
	int IChannelModify(int channelid, int dest, BOOL flag);

	int IPostRead(int pid, int cid = 0, BOOL bset = TRUE, BOOL binc = FALSE);
	int IPostReadOn(int pid, int cid = 0, BOOL bset = TRUE, BOOL binc = FALSE, BOOL bnavigate = FALSE);
	int IPostXMLUpdate(int pid, int cid = 0, BOOL bset = TRUE);

	int IPostDelete(auto_ptr<ID_LIST>& idlist, int flag = FISH_ITEM_ALL, int channelid = 0);
	int IPostDelete(ID_LIST& idlist, int flag = FISH_ITEM_ALL, int channelid = 0);
	int IPostDelete(int id, int flag = FISH_ITEM_ALL, int channelid = 0);

	int IPostUpdate(auto_ptr<POST_LIST>& items, int flag = FISH_ITEM_ALL);
	int IPostUpdate(POST_LIST& items, int flag = FISH_ITEM_ALL);
	int IPostUpdate(auto_ptr<POSTITEM>& apitem, int flag = FISH_ITEM_ALL);

	int IPostSVUpdate(auto_ptr<POSTITEM>& apitem);

	int IPostScrap(auto_ptr<ID_LIST>& apidlist, int channelid);
	int IPostScrap(ID_LIST& idlist, int channelid);
	int IPostScrap(int id, int channelid);


	// 채널별로 추가할 Post를 넣어준다.
	int IPostAdd(auto_ptr<POST_LIST>& items, const COleDateTime& lasttime, int addtype = AT_NORMAL, int channelid = 0);
	int IPostAdd(auto_ptr<POSTITEM>& apitem, GUID guid, int addtype = AT_NORMAL);

	int IPostGet(auto_ptr<POST_LIST>& items, int gettype = FISH_GET_DEFAULT);
	// -- INTERFACE

	// work routine
#ifdef FISH_TEST
	static void Trigger(int i = 0);
#else
	static void Trigger();
#endif

	static void SetInformWnd(HWND h);
	static void SetSubscriveWnd(HWND h);

	CRSSDB();
	virtual ~CRSSDB();

	BOOL Start();

	void Release() { DeinitStatic(); }

	static int m_PrevlastSeq;

protected:
	BOOL m_Init;
	void StopThread();
	BOOL StartThread();

	void StopSThread();
	BOOL StartSThread();

	void InitStatic();
	void DeinitStatic();

	int IPostFlagUpdate(int pid, int cid, BOOL bset = TRUE, int flag = FISH_ITEM_READ);

	static int AddPostMatch(CEzXMLParser* parser, int channelid, int& unread, int& postcnt, RSS_CHANNELDATA& cdata);

	static BOOL RemoveXMLItem(int postid, CEzXMLParser* pxml, BOOL& decunread, RSS_CHANNELDATA& cdata);
	static BOOL RemoveXMLItem(int postid, RSS_XMLDATA& xmldata, BOOL& bopen, BOOL& decunread, RSS_CHANNELDATA& cdata);
	static BOOL RemoveXMLAllItem(CEzXMLParser* pxml, RSS_CHANNELDATA& cdata);


	// thread
	static DWORD WINAPI WorkThread();
	static HANDLE m_hThread;
	static DWORD m_dwThreadId;
	static BOOL m_bThread;

	// search thread
	static DWORD WINAPI SearchThread();
	static HANDLE m_hSThread;
	static HANDLE m_hSThreadEvent;
	static DWORD m_dwSThreadId;
	static BOOL m_bSThread;

	static void PUSH_RESULT(int i, auto_ptr<RESULT>& ptr);
	static int PUSH_WORK(int i, auto_ptr<WORK>& ptr);

	static void TriggerSearch();


	static int Search_Channel(RSS_CHANNELDATA& channel, auto_ptr<WorkSearch>& apsearch);
	static int Search_Map(auto_ptr<WorkSearch>& apsearch);

	static void SearchProcess(SEARCH_LIST& wlist, CRITICAL_SECTION& cs);

	static void WorkProcess(WORK_LIST& wlist, CRITICAL_SECTION& cs);
	static void ResultProcess(RESULT_LIST& rlist, CRITICAL_SECTION& cs);

	static void WP_PostAdd(WorkAddPost* ppostadd, int& p, auto_ptr<RESULT>& apr);
	static void WP_PostUpdate(WorkPostData* ppostdata, int& p, auto_ptr<RESULT>& apr);
	static void WP_PostDelete(WorkPostID* ppostid, int& p, auto_ptr<RESULT>& apr);
	static void WP_PostGet(WorkPostData* ppostdata, int& p, auto_ptr<RESULT>& apr);
	static void WP_PostScrap(WorkPostID* ppostid, int& p, auto_ptr<RESULT>& apr);

	static void WP_ChannelAdd(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static void WP_ChannelDelete(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static void WP_ChannelGet(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static int	WP_ChannelLoad(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static int	WP_ChannelFlush(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static void WP_ChannelSetPath(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static void WP_ChannelRead(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);
	static void WP_ChannelModify(WorkChannel* pchannel, int& p, auto_ptr<RESULT>& apr);

	static void WP_Search(auto_ptr<WORK>& apwork);

	static void WP_Upload(WorkUpload* pupload, int& p, auto_ptr<RESULT>& apr);

	static BOOL SaveChannel(int channelid);

	static void Filtering(POSTITEM& item);

	// list and queue
	static int m_nInit;

	static BOOL m_bLastFlush;
	static BOOL m_bFlush;
	// working check
	static BOOL m_bWorking;

	static BOOL m_bPostInit;

	static int m_nSearchSeq;

	// sequence
	static int m_nseq;
	static int m_npostseq;

	static XML_MAP	m_mapData;

	static WORK_LIST m_queueWork[MAX_WORKLISTCNT];
	static RESULT_LIST m_queueResult[MAX_WORKLISTCNT];

	static SEARCH_LIST m_queueSearch[MAX_SEARCHLISTCNT];

	static KEYWORD_LIST m_listKeyword;

	// window handler for result information
	static HWND m_hInformWnd;
	static HWND m_hSubscribeWnd;

	// sequence of ID

	// XML Data manipulate
	static BOOL UpdateXMLItem(POSTITEM& item, int chtype, CEzXMLParser* pxml, int& unread, DWORD flag);
	static BOOL UpdateXMLItem(POSTITEM& item, int chtype, RSS_XMLDATA& xmldata, BOOL& bopen, int& unread, DWORD flag);
	static BOOL AddChannel(int channelid, int chtype, int exData, CString text
		, CString keyword = _T(""), int keepcnt = 0, int keepdays = 0);
	static BOOL AddPost(POSTITEM& item);
	static BOOL RemoveChannel(int channelid);
	static BOOL RemovePostByPostID(int postid, DWORD flag = FISH_ITEM_ALL, int channelid = 0);
	static BOOL RemovePostByChannelID(int channelid);
	static BOOL ScrapPostByPostID(int postid, int channelid);
	static BOOL GetPostData(POSTITEM& item, DWORD flag = FISH_GET_ALL);
	static BOOL GetPostListData(POST_LIST* plist, DWORD flag);
	static BOOL GetPostXMLSring(int postid, CString& xml, int channelid = 0);
	static BOOL GetChannelPostData(int channelid, DWORD flag, auto_ptr<POST_LIST>& aplistpost);
	static BOOL UpdatePostData(POSTITEM& item, DWORD flag = FISH_ITEM_ALL);
	static BOOL UpdateChannelDataRead(int channelid, BOOL read);
	static BOOL ModifyChannel(int channelid, int dest, int flag);
	static int FlushData(BOOL force, BOOL& keepon);
	static int SetPath(CString path);

	static void ReArrangeData();
	static BOOL LoadXMLFile(CString filename, int& posts);
	static int LoadXMLData(int start, int& nread, BOOL& keepon);

	// XML Data Search
	static BOOL SearchPost();

	static BOOL AddKeyword(int channelid, CString keyword);
	static void RemoveKeyword(int channelid);
};

#endif // !defined(AFX_RSSDB_H__226D62EB_8F81_4E42_B9D7_B64B058AFDFD__INCLUDED_)
