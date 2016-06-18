// SearchManager.h: interface for the CSearchManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHMANAGER_H__AF7044FF_84EE_454D_9C1C_C019A581C23E__INCLUDED_)
#define AFX_SEARCHMANAGER_H__AF7044FF_84EE_454D_9C1C_C019A581C23E__INCLUDED_

/**************************************************************************
 * class CSearchManager
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-12-xx
 *
 * @Description
 * 검색 요청 및 결과를 관리하는 클래스
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "fish_struct.h"
#include "./GnuModule/GnuDefines.h"

#include <list>

using namespace std;

class CSearchManager  
{
public:
	typedef SE_ITEM SNM_ITEM;

	enum { DEFAULT_CNT = 0, };

	struct SPM_ITEM			// Search Post Manage Item
	{
		UINT		uread;
		UINT		ureadon;
		UINT		uscrap;
		UINT		ufilter;
		UINT		uhit;
		BOOL		bmodified;
		int			postid;
		int			channelid;
		CString		sguid;		// post guid
		CString		stitle;
		COleDateTime	dtlastupdate;			// spm item data updated
		COleDateTime	dtlastvalidate;			// DB DATA Valid
		list<SNM_ITEM>	listnode;
		LPVOID		lpdownloader;

		SPM_ITEM()
			: uread(DEFAULT_CNT), ureadon(DEFAULT_CNT), uscrap(DEFAULT_CNT), ufilter(DEFAULT_CNT), uhit(DEFAULT_CNT)
			, bmodified(FALSE), lpdownloader(NULL), postid(0)
		{
		}

		~SPM_ITEM()
		{
			listnode.clear();
		}
	};

	struct SCM_ITEM			// Search Channel Manage Item
	{
		GUID		guid;
		CString		keyword;
		int			channelid;
		int			resultcnt;
		bool		tomanage;
		COleDateTime dtlastupdate;
		list<SPM_ITEM> listreadyitem;
		list<SPM_ITEM> listdoneitem;
		SCM_ITEM()
			:channelid(0), resultcnt(0), guid(NULL_GUID), tomanage(FALSE)
		{
		}

		~SCM_ITEM();

		void MoveToDone(CString sguid);
		SPM_ITEM* AddItem(CString sguid, SE_ITEM& item, CString keyword);
		SPM_ITEM* AddCompleteItem(POSTITEM& item);
		SPM_ITEM* finditem(CString sguid, BOOL& updated);
		void Update_PostID(CString sguid, int postid);
		int Get_PostID(CString sguid);
		void Manage();
	};

public:
	void RemoveChannel(int channelid);
	int FindChannelID(GUID guid);
	SCM_ITEM* FindChannelItem(GUID guid);
	void Manage();

	// 검색 내용 등록
	GUID Search(CString key, int type, TERM t);
	// 검색 결과 등록
	BOOL InsertSearchResult(GUID guid, CString sguid, SE_ITEM& item);
	BOOL InsertSearchResult(GUID guid, POSTITEM& item);
	// 전송 완료 알림
	BOOL PostComplete(GUID guid, CString sguid, auto_ptr<POSTITEM>& apitem);
	// 포스트 id를 갱신함
	BOOL PostUpdate(GUID guid, CString sguid, int postid);

	static UINT uSearchResultLimit;
	static void ResetMaxSearchLimit();

	CSearchManager();
	virtual ~CSearchManager();

protected:
	friend SCM_ITEM;
	static BOOL KeywordSendToWeb(CString keyword);

	list<SCM_ITEM>		listSearch;

};

#endif // !defined(AFX_SEARCHMANAGER_H__AF7044FF_84EE_454D_9C1C_C019A581C23E__INCLUDED_)
