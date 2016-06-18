// RSSCurrentList.h: interface for the CRSSCurrentList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSSCURRENTLIST_H__39C2F2E0_1F8F_4DDA_98DF_C7D187A214B1__INCLUDED_)
#define AFX_RSSCURRENTLIST_H__39C2F2E0_1F8F_4DDA_98DF_C7D187A214B1__INCLUDED_

/**************************************************************************
 * class CRSSCurrentList
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
 * RSS DB에서 읽어와 View로 보여주기 전에 임시로 정보를 저장하는 공간.
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory>
#include <list>

#include "fish_struct.h"

class CDlg_RSSList;
class CDlg_Browser;

using namespace std;

class CRSSCurrentList
{
public:
	enum
	{
		HMT_NONE = 0,
		HMT_ALL,
		HMT_LIST,
		HMT_SINGLE
	};

	void SetSearchWord(CString sWord) { m_sSearchWord = sWord; }

	int GetCurrentPage(int npagepost);

	BOOL Refresh(CString& xmlstream);

	void SetCurrentChannelType(int ntype) { m_nchanneltype = ntype; };
    
    VOID SetListMode(BOOL bShowSrchStyle);

	BOOL RequestSearchChannel(int channelid);

	BOOL GetNextPost(CString& xmlstream, int npos = 1, int ncnt = 1, BOOL bread = TRUE);
	BOOL GetPrevPost(CString& xmlstream, int npos = 1, int ncnt = 1, BOOL bread = TRUE);
	BOOL GetFirstPost(CString& xmlstream, BOOL bread = TRUE);
	BOOL GetLastPost(CString& xmlstream, BOOL bread = TRUE);

	BOOL RequestChannelDelete(int channelid);

	BOOL RequestGroupOnLoad(int groupid, list<int>& idlist, BOOL bunreadonly, BOOL bautoselect = FALSE);
	BOOL RequestChannelOnLoad(int channelid, int channeltype, BOOL bunreadonly, BOOL bautoselect = FALSE);
	int GetCurrentListID();
	void SetCurrentListID(int nID);
	BOOL ReadPost(list<int>& id, BOOL bread);
	BOOL DeletePost(list<int>& idlist);

	BOOL UpdatePostList(auto_ptr<POST_LIST>& aplist, int updateflag, BOOL bredraw = TRUE);
	BOOL UpdatePost(POSTITEM& item, int updateflag, BOOL bredraw = FALSE);
	BOOL IsCurrentChannel(int chid);
	BOOL ReadOnPost(int id, BOOL bNav = FALSE);
	BOOL GetPostXMLStream(int id, CString& xmlstream, BOOL bread = FALSE);
	BOOL GetPostXMLStream(list<int> ids, CString& xmlstream);
	BOOL GetAllXMLStream(CString& xmlstream);
	BOOL FindPost(int id, POSTITEM& appost, BOOL bread = FALSE);
	BOOL SavePost(list<int> ids, CString filename);
	BOOL SavePostCurrent(CString filename);
	void AppendList(auto_ptr<POST_LIST>& l, int channelid = 0, BOOL bsearch = FALSE);
	void SetList(SmartPtr<POST_LIST>& l);
	void SetListDlg(SmartPtr<CDlg_RSSList>& splistdlg);
	void SetParentHandle(HWND hParent) { m_hParent = hParent; }
	void SetPostCntHWnd(HWND hWnd) { m_hPostCntHwnd = hWnd; }
	
	CRSSCurrentList();
	virtual ~CRSSCurrentList();
protected:
	SmartPtr<CDlg_RSSList>		m_splistdlg;

	// 현재 읽고 있는 채널 또는 그룹의 ID
	int							m_nCurrentListID;
	// 그룹을 읽을 경우 최초의 채널
	int							m_nFirstChannel;
	// 현재 읽고 있는 포스트 목록은 Unread조건으로 가져온 것인가?
	BOOL						m_bUnreadOnly;
	BOOL						m_bFirstAppend;
	BOOL						m_bListEnd;
	BOOL						m_bAutoSelect;
	BOOL						m_bGroupLoad;
	BOOL						m_bHitMaxCnt;
	BOOL						m_bUseChannelPage;

	list<int>					m_listRemainChannel;
	list<int>					m_listCurrentChannel;
	// 현재 읽고 있는 포스트의 번호 
	int							m_nCurrentReading;
	int							m_nCurrentListItemCnt;
	HWND						m_hParent;
	HWND						m_hPostCntHwnd;

	int							m_npostcnt;
	int							m_nunreadcnt;

	int							m_nchanneltype;

	BOOL						m_bViewReverse;
	BOOL						m_bSinglePost;
	SmartPtr<POST_LIST>			m_spviewposts;;
	SmartPtr<POST_LIST>			m_splistpost;

	CString						m_sSearchWord;

	BOOL RequestHTMLMake(POSTITEM& post, CString& xmlstream);
	BOOL RequestHTMLMake(SmartPtr<POST_LIST> &aplist, CString& xmlstream, BOOL breverse);
	BOOL RequestHTMLMake(CString& xmlstream, BOOL breverse);		// m_splistpost를 이용해 HTML을 생성한다. 
};

const BOOL MakeStreamToFile(const CString& filename, CString& stream);

#endif // !defined(AFX_RSSCURRENTLIST_H__39C2F2E0_1F8F_4DDA_98DF_C7D187A214B1__INCLUDED_)
