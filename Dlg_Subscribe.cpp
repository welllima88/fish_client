// Dlg_Subscribe.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include "FISH.h"
#include "Dlg_Subscribe.h"
#include "RSSDB.h"
#include "DateParser.h"
#include "fish_common.h"
#include "RSSCurrentList.H"
#include "GlobalIconManager.h"
#include "LocalProperties.h"
#include "MainFrm.h"
#include "TaskbarNotifier.h"
#include "./GnuModule/common.h"
#include "OPMLSock.h"
#include "./IrcModule/httpd.h"
#include "FocusManager.h"
#include "SearchManager.h"
#include "Dlg_MessageBox.h"
#include "Dlg_OpmlImport.h"
//#include "Dlg_RSSMain.h"

#define USE_OPMLMODIFYTIME
#define USE_OPMLSYNC

#define STR_CAUTION_DELETE				_T("������ �����Ͻðڽ��ϱ�?")
#define STR_DELETE_CONFIRM				_T("����Ȯ��")
#define STR_TOOLTIP_ADD_SCRAP			_T("��ũ�� ä�� �߰�")
#define STR_TOOLTIP_ADD_GROUP			_T("�׷� �߰�")
#define STR_TOOLTIP_ADD_CHANNEL			_T("���� ä�� �߰�")
#define STR_TOOLTIP_ADD_KEYWORD			_T("Ű���� �ڵ��˻� ä�� �߰�")
#define STR_TOOLTIP_REFRESH				_T("ä�� ����")
#define STR_GET_MESSAGE					_T("������ �޾ҽ��ϴ�.\n\n���ο�� : %d��")
#define STR_GROUP_OVERLAP_MSG_FORMAT	_T("\"%s\"��(��) �̹� ��ϵǾ� �ִ� �׷��Դϴ�. ���� ä�ε��� �����׷쿡 �߰� �Ͻðڽ��ϱ�?")
#define STR_GROUP_OVERLAP_TITLE			_T("�׷� �ߺ�")
#define STR_CHANNEL_OVERLAP_MSG_FORMAT	_T("\"%s\"��(��) �̹� ��ϵǾ� �ִ� ä���Դϴ�. �ߺ� ����Ͻðڽ��ϱ�?")
#define STR_CHANNEL_OVERLAP_TITLE		_T("ä�� �ߺ�")

#define STR_KEYWORD_OVERLAP_MSG			_T("�̹� ���� Ű���尡 ��ϵǾ� �ֽ��ϴ�. �ߺ���� �Ͻðڽ��ϱ�?")
#define STR_KEYWORD_OVERLAP_TITLE		_T("Ű���� �ߺ�")
#define STR_GROUP_OVERLAP_MSG			_T("�̹� ��ϵǾ� �ִ� �׷��Դϴ�. �ߺ���� �Ͻðڽ��ϱ�?")
//#define STR_KEYWORD_OVERLAP_TITLE		_T("Ű���� �ߺ�")
//#define STR_KEYWORD_OVERLAP_MSG			_T("�̹� ���� Ű���尡 ��ϵǾ� �ֽ��ϴ�. �ߺ���� �Ͻðڽ��ϱ�?")
//#define STR_KEYWORD_OVERLAP_TITLE		_T("Ű���� �ߺ�")

#define DEFAULT_INTERVAL				30

using namespace std;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFISHApp theApp;

struct URL_CHECK
{
	HWND hWnd;
	CString url;
	CString title;
};

// For OPMLLoad ....
struct	GROUP_POINTER
{
	int group_id;
	IXMLDOMNodePtr node_ptr;

	GROUP_POINTER(int gid, IXMLDOMNodePtr p_node)
		: group_id(gid), node_ptr(p_node)
	{}
};

/////////////////////////////////////////////////////////////////////////////
// CDlg_Subscribe dialog


CDlg_Subscribe::CDlg_Subscribe(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Subscribe::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Subscribe)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nsubseq		= 0;

	m_hOPMLThread	= NULL;

	m_bInit			= FALSE;
	m_bSelfDraw		= TRUE;
	m_bFlushOPML	= FALSE;
	m_bFeedInform	= TRUE;

	m_nDefaultGroup	= 0;

	m_nCurSelected	= 0;
	m_nManageFeedChannel	= 0;
	m_nManageFeedPost	= 0;
	m_nNotSaveSearchCnt	= 0;

	// make paint rgn & solid brush
   	m_rgnPaint.CreateRectRgn(0, 0, 0, 0);
	m_brushBG.CreateSolidBrush(CFishTreeCtrl::FISH_REPOSITORY_BG_COLOR);

    _initialize();
}

CDlg_Subscribe::~CDlg_Subscribe()
{
	if(m_rgnPaint.GetSafeHandle())
		m_rgnPaint.DeleteObject();
	if(m_brushBG.GetSafeHandle())
		m_brushBG.GetSafeHandle();
	if(m_uIDTimer) KillTimer(m_uIDTimer);

//	TRACE(_T("CDlg_Subscribe Destroy\r\n"));
}

void CDlg_Subscribe::_initialize()
{
}

void CDlg_Subscribe::_finalize()
{
}

void CDlg_Subscribe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Subscribe)
	DDX_Control(pDX, IDC_BTN_ADDSCRAP, m_btnAddScrap);
	DDX_Control(pDX, IDC_BTN_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_BTN_ADDKEYWORD, m_btnAddKeyword);
	DDX_Control(pDX, IDC_BTN_ADDGROUP, m_btnAddGroup);
	DDX_Control(pDX, IDC_BTN_ADDCHANNEL, m_btnAddChannel);
	DDX_Control(pDX, IDC_TREE_REPOSITORY, m_treeRepository);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Subscribe, CDialog)
	//{{AFX_MSG_MAP(CDlg_Subscribe)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_CHANNELPOST_ALL, OnViewChannelpostAll)
	ON_BN_CLICKED(IDC_BTN_ADDCHANNEL, OnBtnAddchannel)
	ON_BN_CLICKED(IDC_BTN_ADDGROUP, OnBtnAddgroup)
	ON_BN_CLICKED(IDC_BTN_ADDSCRAP, OnBtnAddscrap)
	ON_BN_CLICKED(IDC_BTN_ADDKEYWORD, OnBtnAddkeyword)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnBtnRefresh)
	ON_COMMAND(ID_REPOS_CM_ADD_GROUP, OnReposCmAddGroup)
	ON_COMMAND(ID_REPOS_CM_ADD_CHANNEL, OnReposCmAddChannel)
	ON_COMMAND(ID_REPOS_CM_ADD_KEYWORD, OnReposCmAddKeyword)
	ON_COMMAND(ID_REPOS_CM_ADD_SCRAP, OnReposCmAddScrap)
	ON_COMMAND(ID_REPOS_CM_DELETE, OnReposCmDelete)
	ON_COMMAND(ID_REPOS_CM_OPML_IMPORT, OnReposCmOpmlImport)
	ON_COMMAND(ID_REPOS_CM_OPML_EXPORT, OnReposCmOpmlExport)
	ON_COMMAND(ID_REPOS_CM_PROPERTY, OnReposCmProperty)
	ON_COMMAND(ID_REPOS_CM_REFRESH, OnReposCmRefresh)
	ON_COMMAND(ID_REPOS_CM_RENAME, OnReposCmRename)
	ON_COMMAND(ID_VIEW_CHANNELPOST_UNREAD, OnViewChannelpostUnread)
	ON_COMMAND(ID_VIEW_CHANNEL_MARK_AS_READ, OnViewChannelMarkAsRead)
	ON_COMMAND(ID_VIEW_CHANNEL_MARK_AS_UNREAD, OnViewChannelMarkAsUnread)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SUBSCRIBE_URL, OnSubscribeURL)
	ON_MESSAGE(WM_CHANNELUCNT_UPDATE, OnChannelUCntUpdate)
	ON_MESSAGE(WM_CHANNELPCNT_UPDATE, OnChannelPCntUpdate)
	ON_MESSAGE(WM_INITSUBSCRIBE, OnInitSubscribe)
	ON_MESSAGE(WM_AFTERCHECKURL, OnAfterCheckURL)
	ON_MESSAGE(WM_CHANNELSTATECHANGE, OnChannelStateChange)
	ON_MESSAGE(WM_FEEDCOMPLETE, OnFeedComplete)
	ON_MESSAGE(WM_UPDATEOPMLCOMPLETE, OnUpdateOPMLComplete)
	ON_MESSAGE(WM_SETSEARCHSAVE, OnSetSearchSave)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlg_Subscribe message handlers

BOOL CDlg_Subscribe::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CDlg_Subscribe::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CDlg_Subscribe::PostNcDestroy() 
{
	OPMLBackup();
	m_mapChannel.clear();
	m_mapGroup.clear();
	
	CDialog::PostNcDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// global method

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-17
 *
 * @Description
 * filename�� �̿��Ͽ� OPML ������ ����
 * ���� ���Žð��� dt�� ��� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in const CString&) filename - OPML ���� �̸�
 * (in int) loadtype - OPML�� �о�� ����� ���� (EzXMLParser.h ����)
 * (out COleDateTime&) dt - ��ȯ�� OPML�� ���� ���Žð�
 **************************************************************************/
BOOL GetOPMLDate(const CString& filename, int loadtype, COleDateTime& dt)
{
	CEzXMLParser parser;
	CString strDateTime;
	IXMLDOMNodeListPtr p_nodelist;
	
	// �ش� OPML ���� load
	if(!parser.LoadXML((LPTSTR)(LPCTSTR)filename, loadtype))
		return FALSE;

	// OPML ������ ���Žð� ����
	p_nodelist = parser.SearchNodes(XML_XPATH_HEAD);

	if(p_nodelist == NULL || p_nodelist->length == 0) return FALSE;

	strDateTime = GetNamedNodeText(XML_DATEMODIFIED, &parser, p_nodelist->item[0]);
	if(strDateTime == _T(""))
		return FALSE;

	// COleDateTime���� ��ȯ
	if(dt.ParseDateTime(strDateTime) == 0)
	{
		if(!CDateParser::getDate(strDateTime, dt))
			return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Subscribe Data Method

/*
 * written by moonknit

 * [history]
 * created 2005-11-17
 * modified 2005-12-23 : aquarelle :: ����� ���� ����Ʈ ���Ͽ��� �߰��Ҷ� id���� ���� �ֵ��� ����

 * [Description]
 * �׷��� �߰��Ѵ�. 

 * [Parameters]
 * (in CString) title - �׷��
 * (in CString) comment - �׷� ����
 * (in int) groupid - Group Id (����Ʈ�� 0, Feed List�� ���� �߰��Ҷ� ����)
 * (in enum GROUP_TYPE) type - Group�� Ÿ�� (����Ʈ�� GT_SUBCSRIBE)
 *
 * [Return Value]
 * (int) id - �߰��� �׷��� id�� (input param�� groupid���� 0�ϰ�쿡�� m_nsubseq, �׿ܴ� input param�� groupid)
 
 */
int CDlg_Subscribe::AddGroup(CString title, CString comment, int groupid, enum GROUP_TYPE type)
{
	pair<GROUP_MAP::iterator, bool> tmppair;

	if(groupid == 0)
	{
		tmppair = m_mapGroup.insert(GROUP_MAP::value_type(m_nsubseq+1, 
			FISH_GROUP(m_nsubseq+1, title, comment, type)) );

		if(!tmppair.second)
			return 0;

		++m_nsubseq;

		int seq = Tree_ItemAdd(m_nsubseq);
		(*(tmppair.first)).second.seq = seq;
	}
	else
	{
		tmppair = m_mapGroup.insert(GROUP_MAP::value_type(groupid, 
			FISH_GROUP(groupid, title, comment, type)) );

		if(!tmppair.second)
			return 0;

		if(groupid > m_nsubseq)
			m_nsubseq = groupid;
		//++m_nsubseq;

		int seq = Tree_ItemAdd(groupid);
		(*(tmppair.first)).second.seq = seq;
	}

	/*
#ifdef _DEBUG
    GROUP_MAP::iterator iter = m_mapGroup.begin();
    TRACE(_T("= ALL GROUP ITEM LIST =\n"));
    for (; iter != m_mapGroup.end(); iter++)
        TRACE(_T("id %d : %s\n"), iter->second.id, iter->second.title.GetBuffer(255));
#endif
		*/
	if(groupid == 0)
		return m_nsubseq;
	else
		return groupid;
}

BOOL CDlg_Subscribe::FindGroup(FISH_GROUP& g)
{
	GROUP_MAP::iterator it = m_mapGroup.find(g.id);

	if(it == m_mapGroup.end()) return FALSE;

	g = (*it).second;

	return TRUE;
}


/************************************************************************
RemoveGroup Ư�� �׷��� ID�� �޾Ƽ� �׷��� �����Ѵ�.
@param  : 
@return : 
@remark : 
    �������� ���Ŵ� ������ �ڷᱸ���󿡼� ��������,
    ���� Ʈ�������� ���ŵȴ�.
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/27:Ʈ������ �ֻ�� �׷��� ��� ������ ������ ä���� ��� ���ش�.
************************************************************************/
BOOL CDlg_Subscribe::RemoveGroup(int id)
{
	GROUP_MAP::iterator it = m_mapGroup.find(id);

	if(it == m_mapGroup.end()) return FALSE;

	// ���
    if ( m_treeRepository.GetItemData( m_treeRepository.GetRootItem() ) == id ) {
        vector<HTREEITEM> LIST;
        HTREEITEM hITEM = m_treeRepository.GetChildItem( m_treeRepository.GetRootItem() );
        for( ; hITEM; hITEM = m_treeRepository.GetNextSiblingItem( hITEM ) )    {
            LIST.push_back(hITEM);
        }
        
        vector<HTREEITEM>::iterator it  =   LIST.begin();
        for (; it!=LIST.end(); it++)    {
            RemoveChannel( m_treeRepository.GetItemData(*it) );
        }
        return FALSE;
    }

	CHANNEL_MAP::iterator it2, tit;
	for(it2 = m_mapChannel.begin(); it2 != m_mapChannel.end(); )
	{	
		tit = it2;
		++it2;

		if((*tit).second.groupid == id)
		{
			m_mapChannel.erase(tit);

			// RSS DB�� �ش� ä���� ������ ��û��
		}
	}

	m_mapGroup.erase(it);

	// Repository â�� ������ ��û��
	Tree_ItemRemove(id);

	m_bFlushOPML = TRUE;

	return TRUE;
}

/************************************************************************
ModifyGroup �׷��� ������ �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
    �׷� ID�� �׷��� �����͸� �޾ƿͼ� ������ �����ϰ� �ش� 
    ITEM�� �ٽ� �׸���.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::ModifyGroup(int id, FISH_GROUP group)
{
	// �׷� ������ ��ģ��.
	// Repository â�� ������ �˸�
	GROUP_MAP::iterator it = m_mapGroup.find(id);

	if(it == m_mapGroup.end())
		return FALSE;

	//(*it).second.id = group.id;
	(*it).second.unreadcnt = group.unreadcnt;
	(*it).second.postcnt = group.postcnt;
	//(*it).second.seq = group.seq;
	//(*it).second.type = group.type;
	(*it).second.title = group.title;
	(*it).second.comment = group.comment;

	Tree_ItemUpdate(&(*it).second);

	return TRUE;
}

/**************************************************************************
 * CDlg_Subscribe::AddFilterChannel
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 * modified 2006-02-23 :: guid �߰� by moonknit
 * modified 2006-03-04 :: parameter bForceSave �߰� by moonknit 
 *
 * @Description
 * �˻� �Ǵ� �ڵ� Ű���� ���͸� ä���� �߰��Ѵ�.
 *
 * @Parameters
 * (in int) gid - �׷� id
 * (in CString) title - ä���� ����
 * (in CString) comment - ä���� ����
 * (in CString) keyword - �˻� �Ǵ� ���͸��� Ű����
 * (in CHANNEL_TYPE) filtertype - �˻����� Ű���� �ڵ� ���������� �˷���
 * (in CString) sguid - guid
 * (in int) channelid - ä�� id, 0�̸� �������� �̿��ؼ� �����Ѵ�.
 * (in BOOL) bForceSave - 
 *
 * @Return
 * (int) ������ ä���� id
 **************************************************************************/
int CDlg_Subscribe::AddFilterChannel(int gid, CString title, CString comment
									 , CString keyword, CHANNEL_TYPE filtertype
									 , CString sguid, BOOL bnotsearch, int channelid
									 , BOOL bForceSave)
{
	FISH_GROUP g(gid);
	if(!FindGroup(g)) 
	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		gid = m_treeRepository.GetItemData(hITEM);
	}

	FILTER_CONDITION fc;
	fc.text = keyword;
	fc.type = 0;			// reserved

	if(channelid == 0)
	{
		channelid = m_nsubseq + 1;
//		sguid = fc.text;

		GUID guid;
		GnuCreateGuid(&guid);

		if(guid == NULL_GUID) return 0;

		sguid = GuidtoStr(guid);
	}

	pair<CHANNEL_MAP::iterator, bool> tmppair;
	tmppair = m_mapChannel.insert(CHANNEL_MAP::value_type(channelid, 
		FISH_CHANNEL(channelid, gid, title, comment, 0, filtertype, sguid, bnotsearch, fc, bForceSave)
		) );

	if(!tmppair.second)
		return 0;

	if(channelid > m_nsubseq)
		m_nsubseq = channelid;

	if(FALSE == bForceSave) ++m_nNotSaveSearchCnt;

	// RSS DB �� ä���� �߰���
	theApp.m_spRD->IChannelAdd(channelid, filtertype, bnotsearch, sguid, keyword, bForceSave);
		
	// Repository â�� ������ �˸�
	Tree_ItemAdd(channelid);

	return channelid;
}


/*
 * written by moonknit

 * [history]
 * created 2005-11-17
 * modified 2005-12-23 : aquarelle :: ����� ���� ����Ʈ ���Ͽ��� �߰��Ҷ� id���� ���� �ֵ��� ����
 * modified 2006-02-02 :: channel id ���� 0�� �� ��ƾ ����
 * modified 2006-02-23 :: guid �߰� by moonknit

 * [Description]
 * Subscribe Channel�� �߰��Ѵ�. 

 * [Parameters]
 * (in int) gid - �߰��� �׷� ���̵�
 * (in CString) title - ä�θ�
 * (in CString) comment - ä�� ����
 * (in int) feedinterval - �����ֱ�
 * (in CString) url - Feed Url
 * (in CString) guid - guid
 * (in int) keepcnt - ������ ����Ʈ�� ����
 * (in int) keepdays - �ִ� ���� �Ⱓ
 * (in BOOL) bnotsearch - �˻� ��� ä���� �ƴϸ� TRUE�� �����Ѵ�.
 * (in int) channelid - Channel Id (����Ʈ�� 0, Feed List�� ���� �߰��Ҷ� ����)
 *
 * [Return Value]
 * (int) id - �߰��� ä���� id�� (input param�� channelid���� 0�ϰ�쿡�� m_nsubseq, �׿ܴ� input param�� channelid)
 
 */
int CDlg_Subscribe::AddSubscribeChannel(int gid, CString title, CString comment, int feedinterval
									 , CString url, CString htmlurl, CString sguid
									 , int keepcnt, int keepdays
									 , BOOL bnotsearch, int channelid)
{
	FISH_GROUP g(gid);
	if(!FindGroup(g))
	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		gid = m_treeRepository.GetItemData(hITEM);
	}

	pair<CHANNEL_MAP::iterator, bool> tmppair;

	if(channelid == 0)
	{
		channelid = m_nsubseq + 1;
		sguid = url;
	}

	tmppair = m_mapChannel.insert(CHANNEL_MAP::value_type(channelid, 
		FISH_CHANNEL(channelid, gid, title, comment, feedinterval
		, CT_SUBSCRIBE, url, sguid, bnotsearch, htmlurl, keepcnt, keepdays) 
		) );



	if(!tmppair.second)
		return 0;

	if(channelid > m_nsubseq)
		m_nsubseq = channelid;

	// RSS DB �� ä���� �߰���
	theApp.m_spRD->IChannelAdd(channelid, CT_SUBSCRIBE, bnotsearch, sguid, _T(""), TRUE, keepcnt, keepdays);
	// Repository â�� ������ �˸�
	Tree_ItemAdd(channelid);
	return channelid;
}

/************************************************************************
Remove  ���ڷ� ���� ������ �������� �ش� �������� �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::Remove(int id)
{
    if( m_mapChannel.find(id) != m_mapChannel.end() )
    {
        RemoveChannel(id);
        return TRUE;
    }   
    else if (m_mapGroup.find(id) != m_mapGroup.end() )
    {
        RemoveGroup(id);
        return TRUE;
    }
    return FALSE;
}

/************************************************************************
FindChannel ���ڷ� ���� �������� �������� ä���� �����ϴ��� Ȯ���Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::FindChannel(FISH_CHANNEL &c)
{
	CHANNEL_MAP::iterator it = m_mapChannel.find(c.id);

	if(it == m_mapChannel.end()) return FALSE;

	c = (*it).second;
	return TRUE;
}

/************************************************************************
RemoveChannel ���ڷ� ���� �������� ID�� �̿��ؼ� �ش� ä���� �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::RemoveChannel(int id)
{
	CHANNEL_MAP::iterator it = m_mapChannel.find(id);

	if(it == m_mapChannel.end()) return FALSE;

	// ���

	m_mapChannel.erase(it);

	// RSS DB�� �ش� ä���� ������ ��û��
	theApp.m_spRD->IChannelDelete(id, FALSE);
	theApp.m_spCL->RequestChannelDelete(id);

	// �˻� ����� ��� �˻� ������ �����Ѵ�.
	if((*it).second.type == CT_SEARCH || (*it).second.type == CT_LSEARCH)
	{
		theApp.m_spSM->RemoveChannel(id);
		if(FALSE == (*it).second.searchlog) --m_nNotSaveSearchCnt;
	}

	// Repository â�� ������ �˸�
	Tree_ItemRemove(id);
    return TRUE;
}

/************************************************************************
ModifyChannel ���ڷ� ���� ID�� ä�� �����͸� ������� �ش� ä���� �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::ModifyChannel(int id, FISH_CHANNEL channel)
{
	// ä���� �����͸� ������
	// keyword filtering�� keyword�� ���� �Ұ�
	CHANNEL_MAP::iterator it = m_mapChannel.find(id);

	if(it == m_mapChannel.end())
		return FALSE;
	
	(*it).second.unreadcnt = channel.unreadcnt;
	(*it).second.postcnt = channel.postcnt;
	(*it).second.type = channel.type;
	(*it).second.title = channel.title;
	(*it).second.comment = channel.comment;
	(*it).second.flush = channel.flush;
	(*it).second.feedinterval = channel.feedinterval;
	(*it).second.status = channel.status;
	(*it).second.dtlastfeed = channel.dtlastfeed;
	(*it).second.dtlastbuild = channel.dtlastbuild;		
	(*it).second.xmlurl = channel.xmlurl;
	(*it).second.churl = channel.churl;
	(*it).second.bns = channel.bns;

	Tree_ItemUpdate(&(*it).second);

    return TRUE;
}

/************************************************************************
MoveChannel ���ڷ� ���� ä�� ID�� ä���� ��� �׷��� ID�� �ű��.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::MoveChannel(int id, int target_gid)
{
	// ä���� ��ġ�� �����Ѵ�.
	assert( target_gid != 0 );

	CHANNEL_MAP::iterator it = m_mapChannel.find(id);
	
	if( it == m_mapChannel.end() )   return FALSE;

	// update group count
	GROUP_MAP::iterator git1 = m_mapGroup.find((*it).second.groupid);
	GROUP_MAP::iterator git2 = m_mapGroup.find(target_gid);

	(*git1).second.unreadcnt -= (*it).second.unreadcnt;
	(*git2).second.unreadcnt += (*it).second.unreadcnt;
	(*git1).second.postcnt -= (*it).second.postcnt;
	(*git2).second.postcnt += (*it).second.postcnt;

	Tree_ItemUpdate(&((*git1).second));
	Tree_ItemUpdate(&((*git2).second));
    
    (*it).second.groupid = target_gid;
	m_bFlushOPML = TRUE;

	// Repository â�� ������ �˸�
	// DO DO
	return FALSE;
}

/************************************************************************
FindItem �ش� �׷��� ID�� ���ڷ� ��� �׷��� ã�� �� ���纻�� 2��° ���ڿ� ���Ѵ�.
@PARAM  : 
    id      ã�� �������� ID
    item    ã�� �������� �����͸� ������ ������ auto_ptr ���·� �����Ǿ��־���.
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
BOOL CDlg_Subscribe::FindItem(int id, auto_ptr<FISH_REPITEM>& item)
{
	FISH_GROUP *pg = new FISH_GROUP(id);
	if(FindGroup(*pg))
	{
		item = auto_ptr<FISH_REPITEM> (pg);
		return TRUE;
	}
	delete pg;

	FISH_CHANNEL *pc = new FISH_CHANNEL(id, _T(""));
	if(FindChannel(*pc))
	{
		item = auto_ptr<FISH_REPITEM> (pc);
		return TRUE;
	}
	delete pc;

	return FALSE;
}

/************************************************************************
MakeDefaultChannel �⺻ ä���� �����.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::MakeDefaultChannel(CString url, CString title)
{
	// ������ Channel ������ �̿��Ͽ� ���ο� Channel�� �����.
	if(url == _T("") || title == _T(""))
		return;

	// �������� URL���� üũ
	if(!PathIsURL((LPCTSTR) url))
		return;

	int gid = 0, cid = 0;
	HTREEITEM hitem = NULL;
	GROUP_MAP::iterator	git;
	CHANNEL_MAP::iterator cit;
	
	// ���õ� �������� ã��
	// ������� ä���� ��� �׷쿡 ���ؾ� ���� ����...
	hitem = m_treeRepository.GetSelectedItem();
	if(hitem != NULL)
	{
		gid = m_treeRepository.GetItemData(hitem);
		git = m_mapGroup.find(gid);

		if(git == m_mapGroup.end())
		{
			cit = m_mapChannel.find(gid);

			if(cit != m_mapChannel.end())
				gid = (*cit).second.groupid;
			else
				gid = 1; // �� ó�� �׷�
		}
		
	}
	else
	{
		gid = 1; // �� ó�� �׷�
	}
	
	cid = AddSubscribeChannel(gid, title, _T(""), 0, url, _T(""));

	// Feed�� ��û�Ѵ�.
	if(cid != 0
//		&& CRSSDB::IsPostInit()
		) m_feedcontrol.AddFeed(cid, url, COleDateTime());
}

/************************************************************************
InitSubscribe ���� ������� �׷� ���� �� �ε�
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::OPMLRequest(CString XML, CString sDate)
{
	// ������ fish OPML�� ���ϸ��� �̿��Ͽ� ä�ΰ� �׷��� �����Ѵ�.
	CEzXMLParser		parser;
	IXMLDOMNodePtr		p_node;
	IXMLDOMNodeListPtr	p_nodelist;
	BOOL bUpdated = FALSE;

	CString local;
	local.Format(_T("%s%s"), theApp.m_spLP->GetUserPath(), S_DEFAULT_LOCALOPML);

	// ���� �޾ƿ� xml�� sdate�� �̿��Ͽ� ������ ������ ���Ͽ� ���� �ؾߵǴ����� Ȯ����
/*
	if(XML != _T("") && sDate != _T(""))
	{
		TRACE(_T("OPML Last Updat Time : %s\r\n"), sDate);
		if(sDate > theApp.m_spLP->GetLastOPMLTime())		// �����ؾߵǴ� ��� ��!!
		{

#ifdef USE_OPMLMODIFYTIME
			COleDateTime dtOld, dtNew;

			//TRACE(_T("\nOPML Right (50): %s"), XML.Right(50));

			if(GetOPMLDate(XML, CEzXMLParser::LOAD_STRING, dtNew))
			{
				GetOPMLDate(local, CEzXMLParser::LOAD_LOCAL, dtOld);

				if(dtNew > dtOld)
				{
					// ���� �����ؾ� ��
					bUpdated = TRUE;
				}
			}
#else
			bUpdated = TRUE;
#endif
		}
	}
*/
	// comment by moonknit
	// OPML Sync ����� �ϼ� �Ǹ� �Ʒ��ٰ� �Բ� �� �ּ��� �����Ѵ�.
	//bUpdated = FALSE;
	
	BOOL bLoad = FALSE;
/*	if(bUpdated)
	{
		bLoad = OPMLLoad(XML, LOCAL_OPML, CEzXMLParser::LOAD_STRING);
		if(bLoad)
		{
			theApp.m_spLP->SetLastOPMLTime(sDate);
			theApp.m_spLP->SaveProperties();

			//parser.SaveXML((LPTSTR) (LPCTSTR) local);
			OPMLSave();
		}
		else
			bLoad = OPMLLoad(local);
	}
	else
*/
		bLoad = OPMLLoad(local);

	if(!bLoad)
	{
		// TO DO
		// �� ������ ���� ����ڸ� ���� fish ���� OPML�� �ε��ϵ��� ó���Ѵ�.

		MakeDefaultOPML(parser);		
		p_nodelist = parser.SearchNodes(XML_XPATH_OPML);
		p_node = p_nodelist->item[0];
		
		parser.SaveXML((LPTSTR) (LPCTSTR) local);
	}

	// ��ü �����ڿ��� Subscribe �ʱ�ȭ�� �˸�
	// ChannelLoad�� �Ϸ� �Ǹ� �ʱ�ȭ �Ϸ� ó���� �����Ѵ�.
	int g = 0;
	if(m_mapGroup.size() == 0)
	{
		g = AddGroup(_T("�⺻ �׷�"), _T("���� �Ұ��� �ϵ��� ó��"));
	}

	// TO DO 
	// default OPML�� �����ϸ� �Ʒ� �ڵ�� �����Ѵ�.
	if(m_mapChannel.size() == 0 && g != 0)
	{
		// ���Ƿ� �׷� �� ä�� �߰�
		AddSubscribeChannel(g, _T("FISH PRJ - ���� ������ ��α�"), _T("FISHING IN THE NET"), 0, _T("http://211.239.119.176/blog/?feed=rss2"), _T(""));

	    Invalidate();
	}

	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		m_treeRepository.Expand( hITEM, TVE_EXPAND);

		m_nDefaultGroup = m_treeRepository.GetItemData(hITEM);
	}
    
	// CRSS DB�� Load �۾��� �����ϵ��� ��û�ؾ��Ѵ�.
	Sleep(100);
	theApp.m_spRD->IChannelLoad();
	theApp.m_spRD->Trigger();
}

/////////////////////////////////////////////////////////////////////////////
// CDlg_Subscribe Thread

DWORD CDlg_Subscribe::OPMLThread(LPVOID lpParam)
{
	CDlg_Subscribe* pview = (CDlg_Subscribe*) lpParam;
	if(!pview) return 0;
	// TO DO
	// OPML ������ �޾ƿ´�.
	CString OPML, sDate;
	BOOL bSuccess = GetOPMLFromWeb(OPML, sDate);

	// �޾ƿ� OPML ������ �̸��� �̿��Ͽ� InitSubscribe()�� �����Ѵ�.
	pview->m_hOPMLThread = NULL;

	if(bSuccess)
		pview->SendMessage(WM_INITSUBSCRIBE, (WPARAM) (LPCTSTR) OPML, (LPARAM) (LPCTSTR) sDate);
	else
		pview->SendMessage(WM_INITSUBSCRIBE, NULL, NULL);

	return 0;
}

DWORD CDlg_Subscribe::CheckURLThread(LPVOID lpParam)
{
	URL_CHECK* purlcheck = (URL_CHECK*) lpParam;
	if(!purlcheck) return 0;
	// URL ������ �ǽ��� �� MakeDefaultChannel�� �����ϵ��� �Ѵ�.
	// check with purlcheck->url
	// TO DO

	BOOL succeed = FALSE;
	if(succeed)
	{
		::SendMessage(purlcheck->hWnd, WM_AFTERCHECKURL, (WPARAM) purlcheck, NULL);
		return 0;
	}

	delete purlcheck;
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Event Method


void CDlg_Subscribe::OnInitSubscribe(WPARAM wParam, LPARAM lParam)
{
	OPMLRequest();
}

/**************************************************************************
 * method CDlg_Subscribe::OnChannelUCntUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-17
 *
 * @Description
 * ä���� Post ������ ����Ǿ��� ��� �߻��ϴ� �̺�Ʈ �Լ�
 *
 * @Parameters
 * (in WPARAM) wParam - channel id
 * (in LPARAM) lParam - unread post cnt
 **************************************************************************/
void CDlg_Subscribe::OnChannelUCntUpdate(WPARAM wParam, LPARAM lParam)
{
	// ä���� Unread ���� ����
	CHANNEL_MAP::iterator it = m_mapChannel.find(wParam);
	if(it == m_mapChannel.end()) return;

	int oldcnt = (*it).second.unreadcnt;
	int newcnt = (int) lParam;
	if( oldcnt != newcnt)
	{
		(*it).second.unreadcnt = newcnt;

		// Tree â ����
		UpdateGroupUPostCnt((*it).second.groupid, newcnt - oldcnt);
		Tree_ItemUpdate(&((*it).second));
	}
}

/**************************************************************************
 * method CDlg_Subscribe::OnChannelPCntUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-19
 *
 * @Description
 * ä���� Post ������ ����Ǿ��� ��� �߻��ϴ� �̺�Ʈ �Լ�
 *
 * @Parameters
 * (in WPARAM) wParam - channel id
 * (in LPARAM) lParam - unread post cnt
 **************************************************************************/
void CDlg_Subscribe::OnChannelPCntUpdate(WPARAM wParam, LPARAM lParam)
{
	// ä���� Unread ���� ����
	CHANNEL_MAP::iterator it = m_mapChannel.find(wParam);
	if(it == m_mapChannel.end()) return;

	int oldcnt = (*it).second.postcnt;
	int newcnt = (int) lParam;
	if( oldcnt != newcnt)
	{
		(*it).second.postcnt = newcnt;

		UpdateGroupPostCnt((*it).second.groupid, newcnt - oldcnt);
	}
}

/************************************************************************
UpdateGroupUPostCnt �׷��� �������� ����Ʈ�� ���ڸ� ����
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::UpdateGroupUPostCnt(int id, int cnt)
{
	GROUP_MAP::iterator it = m_mapGroup.find(id);
	if(it == m_mapGroup.end()) return;

	(*it).second.unreadcnt += cnt;

	Tree_ItemUpdate(&((*it).second));
}

/************************************************************************
UpdateGroupPostCnt �׷��� ���� ����Ʈ�� ���ڸ� ������.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::UpdateGroupPostCnt(int id, int cnt)
{
	theApp.m_ntotalpostcnt += cnt;

	GROUP_MAP::iterator it = m_mapGroup.find(id);
	if(it == m_mapGroup.end()) return;

	(*it).second.postcnt += cnt;
}

/************************************************************************
OnAfterCheckURL ���ڷ� ���� URL�� üũ �´ٸ� ä�η� ����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::OnAfterCheckURL(WPARAM wParam, LPARAM lParam)
{
	URL_CHECK* purlcheck = (URL_CHECK*) wParam;
	if(!purlcheck) return;

	MakeDefaultChannel(purlcheck->url, purlcheck->title);

	delete purlcheck;
}

//////////////////////////////////////////////////////////////////////
// Call Method

// deprecated by moonknit 2006-01-24
void CDlg_Subscribe::RequestSubscribeChannelAdd(CString url)
{
	// URL ������ �ǽ��Ѵ�. using thread
	DWORD dwThreadID;

	URL_CHECK* purlcheck = new URL_CHECK;
	
	purlcheck->hWnd = m_hWnd;
	purlcheck->url = url;

	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) CheckURLThread, (LPVOID) purlcheck
		, CREATE_SUSPENDED, &dwThreadID);

	return;
}

#define DEFAULT_GID						1

/**************************************************************************
 * CDlg_Subscribe::RequestSearchChannelAdd
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * �˻� ä���� �߰��ϵ��� ��û�Ѵ�.
 * �⺻ �׷��� �����ϵ��� �Ѵ�.
 *
 * @Parameters
 * (in CString) searchword - �˻���
 * (in int) type - �˻� ����
 * (in BOOL) bsearch - ä�� �߰��� ���� �˻��� ���� ����
 *
 * @Returns
 * (int) ������ ä���� id
 **************************************************************************/
int CDlg_Subscribe::RequestSearchChannelAdd(CString searchword, int type, BOOL search)
{
    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();
    if ( m_treeRepository.GetParentItem( hITEM ) )
    {
        hITEM   =   m_treeRepository.GetParentItem( hITEM );
    }
    
    int chid    =   0;
    int gid     =   0;
    if (hITEM)
        gid = m_treeRepository.GetItemData(hITEM);
    else
        gid = m_nDefaultGroup;

	// �˻�� �̿��Ͽ� ä���� �����Ѵ�.

	CString comment;
	comment.Format( _T("�˻�") );
	BOOL bForceSave = theApp.m_spLP->GetForceSave();
	chid = AddFilterChannel(gid, searchword, comment, searchword, (CHANNEL_TYPE) type, _T(""), FALSE, 0, bForceSave);

    hITEM =   m_treeRepository.GetHandleFromData(chid);
	if(hITEM)
	{
		// added by moonknit 2006-02-27
		// for expand
		HTREEITEM hParent = m_treeRepository.GetParentItem(hITEM);
		if(NULL != hParent )	{
			m_treeRepository.Expand( hParent, TVE_EXPAND);
		}
	}

	if(chid)
	{
		// Ű�ֵ带 �̿��Ͽ� local �˻��� ��û�Ѵ�.
		theApp.m_spCL->RequestSearchChannel(chid);
		if(search)
		{
			theApp.m_spRD->IRequestSearch(LOCAL_SEARCH, searchword, _T(""), _T(""), 0, chid);
			theApp.m_spCL->SetSearchWord(searchword);
		}
	}

	return chid;

}

/**************************************************************************
 * method CDlg_Subscribe::AddScrapChannel
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-24
 * modified 2006-02-23 :: guid ���� ó�� �߰� by moonknit
 *
 * @Description
 * ��ũ�� ä���� �����Ѵ�.
 *
 * @Parameters
 * (in int) gid - �׷� id
 * (in CString) title - Scrap ����
 * (in CString) comment - ����
 * (in CString) sguid - ä���� guid
 * (in BOOL) bnotsearch - ä���� �˻� ����� �ƴϸ� TRUE�� �����Ѵ�.
 * (in int) channelid - ä�� id, 0�� ������ sequence���� �����ؼ� ����Ѵ�.
 *
 * @Returns
 * (int) ������ ä���� id
 **************************************************************************/
int CDlg_Subscribe::AddScrapChannel(int gid, CString title, CString comment, CString sguid, BOOL bnotsearch, int channelid)
{
	FISH_GROUP g(gid);
	if(!FindGroup(g))
	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		gid = m_treeRepository.GetItemData(hITEM);
	}

	if(channelid == 0)
	{
		channelid = m_nsubseq + 1;

		GUID guid;
		GnuCreateGuid(&guid);

		if(guid == NULL_GUID) return 0;

		sguid = GuidtoStr(guid);
	}

	pair<CHANNEL_MAP::iterator, bool> tmppair;
	tmppair = m_mapChannel.insert(CHANNEL_MAP::value_type(channelid, 
		FISH_CHANNEL(channelid, gid, title, comment, 0, CT_SCRAP, _T(""), sguid, bnotsearch)
		) );

	if(!tmppair.second)
		return 0;

	if(channelid > m_nsubseq)
		m_nsubseq = channelid;

	// RSS DB �� ä���� �߰���
	theApp.m_spRD->IChannelAdd(channelid, CT_SCRAP, bnotsearch, sguid);
		
	// Repository â�� ������ �˸�
	Tree_ItemAdd(channelid);

	return channelid;
}

/**************************************************************************
 * CDlg_Subscribe::RequestKeywordChannelAdd
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * Keyword �ڵ� ���͸��� ����Ѵ�.
 * ����� ���� �˻��� ��û�Ѵ�.
 * �⺻ �׷��� �����ϵ��� �Ѵ�.
 *
 * @Parameters
 * (in CString) title - ����
 * (in CString) comment - ����
 * (in CString) keyword - �˻���
 * (in int) gid - �߰��� �׷� id
 *
 * @Returns
 * (int) ������ ä���� id
 **************************************************************************/
int CDlg_Subscribe::RequestKeywordChannelAdd(CString title, CString comment, CString keyword, int gid)
{
	int chid = 0;

	// keyword�� �̿��Ͽ� RSS DB�� Keyword ���θ� Ȯ���Ѵ�.
	if(!theApp.m_spRD->IsExistKeyword(keyword))
	{
		// Ű���� ä���� �߰��Ѵ�.

		chid = AddFilterChannel(gid, title, comment, keyword, CT_KEYWORD);

		if(chid)
		{
			BOOL bsearch = TRUE;
			if(bsearch)
			{
				theApp.m_spCL->RequestSearchChannel(chid);
				// Ű���带 �̿��Ͽ� local �˻��� ��û�Ѵ�.
				theApp.m_spRD->IRequestSearch(LOCAL_SEARCH, keyword, _T(""), _T(""), 0, chid);
				theApp.m_spCL->SetSearchWord(keyword);
			}
		}
	}

	return chid;
}

void CDlg_Subscribe::Fetch(int channelid, CString url, COleDateTime dtlastbuild)
{
	// channelid == 0 �̰� type == FETCH_NORMAL �̸� ���� FETCH ���������� ���� ��� ä���� FETCH �۾��� �����Ѵ�.
    if ( m_feedcontrol.IsChannelFeeding(channelid) ) return;

	m_feedcontrol.AddFeed(channelid, url, dtlastbuild);
}

/**************************************************************************
 * CDlg_Subscribe::FetchByUser
 *
 * written by moonknit
 *
 * @history
 * created 2005-11
 * �ִϸ��̼� ����ȭ �߰��� -   2005/12/21 eternalbleu
 *
 * @Description
 * ����ڿ� ���� �������� fetch �۾��� �����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - fetch�� ������ ä�� id
 **************************************************************************/
void CDlg_Subscribe::FetchByUser(int channelid)
{
    CHANNEL_MAP::iterator it = m_mapChannel.find(channelid);
    GROUP_MAP::iterator it2 = m_mapGroup.find(channelid);
    
    {   // CHANNEL FETCH
        if(it != m_mapChannel.end())
        {
//            m_treeRepository.animate_ADD( channelid );  // added by eternalbleu
            Fetch(channelid, (*it).second.xmlurl, (*it).second.dtlastbuild);
        }
    }

    {   // GROUP FETCH
	    if(it2 != m_mapGroup.end())
	    {
		    list<int> idlist;
		    for(it = m_mapChannel.begin(); it != m_mapChannel.end(); ++it)
		    {	
			    if((*it).second.groupid == channelid)
			    {
				    Fetch((*it).second.id, (*it).second.xmlurl, (*it).second.dtlastbuild);
			    }
		    }
	    }
    }
}

/**************************************************************************
 * CDlg_Subscribe::FetchComplete
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * Fetch(Feeding) �۾��� ���� �� RSSDB�� ����Ʈ�� �߰��ϰ� ���� �� ó���� �����Ѵ�.
 *
 * @Parameters
 * (in int) channelid - fetch�� ������ ä�� id
 * (in COleDateTime) dt - fetch�� ä���� ���� ���� �ð�
 * (in int) postcnt - fetch�� ���� ���� ����Ʈ�� �߰��� ����Ʈ�� ��
 * (in int) keepcnt - ä�δ� ����Ʈ �ִ� ���� ���� ���� ����
 **************************************************************************/
void CDlg_Subscribe::FetchComplete(int channelid, COleDateTime dt, int postcnt, int keepcnt)
{
	// ä���� lastBuildDate�� �����Ѵ�.
	CHANNEL_MAP::iterator it = m_mapChannel.find(channelid);

	if(it != m_mapChannel.end())
	{
		if((*it).second.dtlastbuild > dt)
			(*it).second.dtlastbuild = dt;
	}

	if(keepcnt > MIN_KEEPCNT)
	{
		(*it).second.keepcnt = keepcnt;
	}

    TRACE( _T("(CDlg_Subscribe) Fetch Complete [%d]\r\n"), channelid );
	if(postcnt > 0) {
        OnChannelStateChange(channelid, CS_NEW);
    }   else    {
        OnChannelStateChange(channelid, CS_IDLE);
    }

	if(m_bFeedInform)
	{
		m_nManageFeedPost += postcnt;
		++m_nManageFeedChannel;
		if(!m_feedcontrol.IsFeedAny())
		{
			// TO DO
			m_bFeedInform = FALSE;

			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(!pFrame) return;
			SmartPtr<CTaskbarNotifier> spnotifier;
			pFrame->GetTaskbarNotifier(spnotifier);

			//!pFrame->IsWindowVisible() && 

			if(spnotifier != NULL && theApp.m_spLP->GetUseInformWnd() && postcnt > 0)
			{
				CString message;
						
				spnotifier->SetTextFont(DEFAULT_APPLICATION_FONT, 95,  TN_TEXT_NORMAL, TN_TEXT_UNDERLINE | TN_TEXT_BOLD );
				spnotifier->SetTextColor(RGB(255, 255, 255), RGB(255, 255, 255));
				spnotifier->SetTextRect(CRect(9, 45, spnotifier->m_nBitmapWidth-5, spnotifier->m_nBitmapHeight-10));

				message.Format(STR_GET_MESSAGE, postcnt);
				
				spnotifier->Show(message); // msg->string..
			}
		}
	}
}

/**************************************************************************
 * CDlg_Subscribe::OPMLLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-??
 * modified 2005-12-23 :aquarelle:: �ʱ�ȭ�� ����� ��������Ʈ�� �������� �ܺ� OPML���� ó��
 * modified 2006-02-92 :: channel type �߰� by moonknit
 * modified 2006-02-08 :: 2�� �̻��� �׷��� ������ OPML ���� ó�� �� �ε�Ÿ�� ���� �߰�. by aquarelle
 * modified 2006-02-16 :: �׷� �� ä���� �ߺ��� ó�� ���� ���� ���̾�α� �߰�  by aquarelle
 * modified 2006-02-20 :: loadfrom ���� �߰� �� �׿� ���� �ڵ� �߰� by moonknit
 * modified 2006-02-23 :: guid ���� ó�� �߰� by moonknit
 * modified 2006-03-02 :: not search flag �߰� by moonknit
 *
 * @Description
 * ������ OPML������ �ε��Ͽ� ��������Ʈ�� �߰��Ѵ�.
 *
 * @Parameters
 * (in const CString) filename - Load �� OPML ���� 
 * (in int) loadtype - OPML file�� �ε� ����.. (default : LOCAL_OPML - ��������Ʈ����)
 * (in int) loadfrom - Parser���� XML�� ���� �� ���Ǵ� �ɼ�, 0 means LOAD_LOCAL
 **************************************************************************/
BOOL CDlg_Subscribe::OPMLLoad(const CString filename,  int loadtype, int loadfrom)
{
	// TO DO
	// �Ϲ� ������ OPML�̳�
	// FISH���� ����ϴ� ������ OPML�� �о�� ������ �׷� ä�� ������ �����Ѵ�.
	CEzXMLParser		parser;
	IXMLDOMNodePtr		p_node;
	IXMLDOMNodeListPtr	p_nodelist;
	BOOL				bFishOPML = FALSE;	
	BOOL				bhasGroup = FALSE;
	BOOL				bns = FALSE;
	int					gid;
	int					addcnt = 0;
	int					nctype;
	int					cid;
	int					nkeepd;
	int					nkeepc;
	CString				title,
						comment,
						feedinterval,
						url,
						htmlurl,
						type,
						strTemp,
						channeltype,
						guid;

	list<GROUP_POINTER> list_newgroupid;
	list<GROUP_POINTER>::iterator it;
	CHANNEL_MAP::iterator cit;
	GROUP_MAP::iterator git;
	int	nogroupid = 0;

	int nChannelOverlapedSel = 0;
	int	nGroupOverlapedSel = 0;
	
	if(parser.LoadXML((LPTSTR)(LPCTSTR)filename, loadfrom))
	{
		if(loadfrom == CEzXMLParser::LOAD_STRING)
		{
			parser.SaveXML(S_DEFAULT_LOCALOPML);
		}
		p_nodelist = parser.SearchNodes(XML_XPATH_HEAD);
		p_node = p_nodelist->item[0];

		 //����� fish ��������Ʈ�� ������찡 �ƴѰ�� Fish �������� Ȯ��...
		if(loadtype != LOCAL_OPML)
		{
			strTemp = GetNamedNodeText(XML_EXPANSIONSTATE, &parser, p_node, 0);
			if(strTemp == _T("Fish"))
			{
				bFishOPML = TRUE;
				bhasGroup = TRUE;
			}
		}
		else
		{
			bFishOPML = TRUE;
			bhasGroup = TRUE;
		}
				
		p_nodelist = parser.SearchNodes(XML_XPATH_OUTLINE);			
		if(p_nodelist->Getlength() == 0)
			return FALSE;

		// Fish ������ �ƴ� ���..
		if(!bFishOPML)
		{
			// Group Outline�� �ִ��� �˻�....
			for(int i=0; p_nodelist->item[i] != NULL; i++)
			{
				p_node = p_nodelist->item[i];
				
				if(p_node->childNodes->length > 0)
				{
					bhasGroup = TRUE;
					break;
				}
			}
			// Groupȭ ������ ���� OPML �̶��....
			// OPML Head ��������.... Group ���� ������... ���ο� �׷��߰�...
			if(!bhasGroup)
			{
				IXMLDOMNodeListPtr templist;
				templist = parser.SearchNodes(XML_XPATH_HEAD);
				title = GetNamedNodeText(XML_TITLE, &parser, templist->item[0], 1);
				comment = GetNamedNodeText(XML_COMMENT, &parser, templist->item[0], 1);
				if(title.IsEmpty())
					title =  STR_NEWGROUP;

				gid = AddGroup(title, comment);
				if(gid != 0)
				{
					list_newgroupid.push_front(GROUP_POINTER(gid, p_nodelist->item[0]->parentNode));
					addcnt++;
				}
				else
					return FALSE;
			}
		}
	
		// OPML ���Ͽ��� ���������� �׷� ä�� ���� �߰�.
		for(int i=0; p_nodelist->item[i] != NULL; i++)
		{
			p_node = p_nodelist->item[i];

			title = GetNamedNodeText(XML_TITLE, &parser, p_node, 0);
			if(title.IsEmpty())
				title = GetNamedNodeText(XML_TEXT, &parser, p_node, 0);

			comment = GetNamedNodeText(XML_DESCRIPTION, &parser, p_node, 0);
			if(comment.IsEmpty())
				comment = GetNamedNodeText(XML_COMMENT, &parser, p_node, 0);

			type = GetNamedNodeText(XML_TYPE, &parser, p_node, 0);
			url = GetNamedNodeText(XML_XMLURL, &parser, p_node, 0);
			guid = GetNamedNodeText(XML_GUID, &parser, p_node, 0);

			// NOT SEARCH FLAG
			if(GetNamedNodeText(XML_BNS, &parser, p_node, 0) == "1") bns = TRUE;
			else bns = FALSE;

			if(guid.IsEmpty())
				guid = url;

			// URL�� guid�� ������ �׷����� �Ǵ�
			if(((url.IsEmpty() && guid.IsEmpty()) || p_node->hasChildNodes() ) && bhasGroup == TRUE)
			{
				if(loadtype != LOCAL_OPML)
				{	
					for(git = m_mapGroup.begin(); git != m_mapGroup.end(); git++)
					{
						if((*git).second.title == title)
						{
							if(nGroupOverlapedSel != IDYESALL && nGroupOverlapedSel != IDNOALL)
							{
								CString message;
								message.Format(STR_GROUP_OVERLAP_MSG_FORMAT, title);
								nGroupOverlapedSel = FishMessageBox(message, STR_GROUP_OVERLAP_TITLE, FMB_ICONWARNING | FMB_YESYESALLNONOALL);
							}
							
							if(nGroupOverlapedSel == IDYES || nGroupOverlapedSel == IDYESALL)
							{
								gid = (*git).first;
							}
							else
							{
								gid = 0;
							}
							
							break;
						}
					}

					if(git == m_mapGroup.end())
						gid = 0;
					
				}
				
				if(loadtype == LOCAL_OPML)
				{
					int groupid = _ttoi((LPTSTR)(LPCTSTR) GetNamedNodeText(XML_ID, &parser, p_node, 0));
					int gtype = _ttoi((LPTSTR)(LPCTSTR) GetNamedNodeText(XML_GROUPTYPE, &parser, p_node, 0));
					gid = AddGroup(title, comment, groupid, (GROUP_TYPE) gtype);
				}
				else
				{
					if(gid == 0)
						gid = AddGroup(title, comment);
				}				
				
				if(gid != 0)
				{
					list_newgroupid.push_front(GROUP_POINTER(gid, p_node));
					addcnt++;
				}
				else
					return FALSE;

				continue;
			}
			else
			{				
				htmlurl = GetNamedNodeText(XML_HTMLURL, &parser, p_node, 0);
				feedinterval = GetNamedNodeText(XML_FEEDINTERVAL, &parser, p_node, 0);
				channeltype = GetNamedNodeText(XML_CHANNELTYPE, &parser, p_node, 0);

				nctype = CT_SUBSCRIBE;
				if(!channeltype.IsEmpty())
					nctype = _ttoi((LPTSTR) (LPCTSTR) channeltype);

				// added by moonknit 2006-03-09
				if(nctype == CT_SUBSCRIBE && loadtype == LOCAL_OPML)
				{
					nkeepc = _ttoi(GetNamedNodeText(XML_KEEPCNT, &parser, p_node, 0));
					nkeepd = _ttoi(GetNamedNodeText(XML_KEEPDAYS, &parser, p_node, 0));
				}

				// ä�� �߰�...
				cid = 0;
				if(loadtype == LOCAL_OPML)
				{					
//					TRACE(_T("%s : %s\n"), title, GetNamedNodeText(XML_ID, &parser, p_node, 0));
					cid = _ttoi((LPTSTR)(LPCTSTR) GetNamedNodeText(XML_ID, &parser, p_node, 0));
				}
				else
				{
					//
					for(it = list_newgroupid.begin(); it != list_newgroupid.end(); it++)
					{
						if((*it).node_ptr == p_node->parentNode)
						{						
							break;
						}						
					}
					if(it != list_newgroupid.end())
						gid = (*it).group_id;
					else
					{
						if(nogroupid == 0)
						{
							IXMLDOMNodeListPtr templist;
							templist = parser.SearchNodes(XML_XPATH_HEAD);
							CString grouptitle;
							grouptitle = GetNamedNodeText(XML_TITLE, &parser, templist->item[0], 1);
							if(!grouptitle.IsEmpty())
								nogroupid = AddGroup(grouptitle);
							else
								nogroupid = AddGroup(STR_NEWGROUP);

							templist = parser.SearchNodes(XML_XPATH_BODY);

							list_newgroupid.push_front(GROUP_POINTER(nogroupid, templist->item[0]));
							addcnt++;
						}						
						
						gid = nogroupid;
					}
				}

				if(bFishOPML)
				{
					switch(nctype)
					{
					case CT_KEYWORD:
					case CT_SEARCH:
					case CT_LSEARCH:
						if(AddFilterChannel(gid, title, comment, url, (CHANNEL_TYPE) nctype, guid, bns, cid, TRUE))
							addcnt++;
						break;
					case CT_SCRAP:
						if(AddScrapChannel(gid, title, comment, guid, bns, cid))
							addcnt++;
						break;
					case CT_SUBSCRIBE:
					default:
						if(AddSubscribeChannel(gid, title, comment, _ttoi((LPTSTR)(LPCTSTR)feedinterval),url, htmlurl, guid, nkeepc, nkeepd, bns, cid))
							addcnt++;
					}
				}
				else
				{
					int seq = 0;
					for(cit = m_mapChannel.begin(); cit != m_mapChannel.end(); cit++)
					{
						if((*cit).second.xmlurl == url)
						{
							if(nChannelOverlapedSel != IDYESALL && nChannelOverlapedSel != IDNOALL)
							{
								CString message;
								message.Format(STR_CHANNEL_OVERLAP_MSG_FORMAT, title);
								nChannelOverlapedSel = FishMessageBox(message, STR_CHANNEL_OVERLAP_TITLE, FMB_ICONWARNING | FMB_YESYESALLNONOALL);
							}
							
							if(nChannelOverlapedSel == IDYES || nChannelOverlapedSel == IDYESALL)
							{
								if(seq = AddSubscribeChannel(gid, title, comment, DEFAULT_INTERVAL,url, htmlurl, guid, 0, 0, FALSE, cid))
									addcnt++;									
							}
								
							break;
						}
					}

					if(cit == m_mapChannel.end())
					{
						seq = AddSubscribeChannel(gid, title, comment, DEFAULT_INTERVAL,url, htmlurl, guid, 0, 0, FALSE, cid);
					}

					if(seq)
					{
						FetchByUser(seq);
						addcnt++;
					}
				}
			}
		}

		if(addcnt > 0)
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		TRACE(_T("\nOPML load Failed -> %s"), filename);
	return FALSE;
}

/**************************************************************************
 * CDlg_Subscribe::OPMLSave
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-?? 
 * modified 2006-02-02 :: channel type �߰� by moonknit
 * modified 2006-02-23 :: guid ���� �߰� by moonknit
 *
 * @Description
 * ���� ��������Ʈ�� �����Ѵ�.
 *
 * @Parameters
 *
 **************************************************************************/
BOOL CDlg_Subscribe::OPMLSave()
{
	// ������ ���Ͽ� fish OPML������ ����Ѵ�.
	// OPML ������ ���� ���θ� Ȯ���Ͽ��� �Ѵ�.
	// TO DO
	
	CEzXMLParser parser;
	CString	strTemp;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	CHANNEL_MAP::iterator cit;
	GROUP_MAP::iterator git;
	COleDateTime dt;
	TCHAR	buffer[256];

	CString local;
	local.Format(_T("%s%s"), theApp.m_spLP->GetUserPath(), S_DEFAULT_LOCALOPML);

	// ������ XML�� ���� ���....
	if(!parser.LoadXML((LPTSTR) (LPCTSTR) local, CEzXMLParser::LOAD_LOCAL))
	{
		if(!MakeDefaultOPML(parser))
			return FALSE;
	}
	else
	{
		// ������ XML�� ���� ��� 
		// ���� ���� �ð� node�� ������ ���� �ð����� ����
		p_nodelist = parser.SearchNodes(XML_XPATH_HEAD);
		
		p_node = p_nodelist->item[0]->attributes->getNamedItem(_bstr_t(XML_DATEMODIFIED));
		
		if(p_node != NULL)
		{			
			dt = COleDateTime::GetCurrentTime();			
			p_node->text = (LPTSTR)(LPCTSTR) dt.Format(FISH_DT_TEMPLETE);
			
			// Body ��� ������ ��� ��� ����...
			parser.RemoveAllTagNodes(XML_XPATH_OUTLINE);
		}
		else // ������ �߸��Ǿ� �ִٸ�... OPML �ٽ� ����
		{
			parser.Clear();
			if(!MakeDefaultOPML(parser))
				return FALSE;	
		}		
	}

	// Body ��� ����
	p_nodelist = parser.SearchNodes(XML_XPATH_BODY);
	p_node = p_nodelist->item[0];

	// Tree�� ��ó�� �׷� item ����...
	HTREEITEM gtreeitem;
	HTREEITEM ctreeitem;
	
	gtreeitem = m_treeRepository.GetRootItem();		

	while(gtreeitem != NULL)
	{
		// �׷� outline ���.. ������.. body �ؿ�... ����..

		git = m_mapGroup.find(m_treeRepository.GetItemData(gtreeitem));

		p_element = parser.CreateElement(XML_OUTLINE);

		if(p_element == NULL)
			return FALSE;


		if(!parser.AddAttribute(p_element, XML_TITLE, (LPTSTR)(LPCTSTR) (*git).second.title))
			return FALSE;		
		
		if(!parser.AddAttribute(p_element, XML_ID, _itot((*git).second.id, buffer, 10)))
			return FALSE;
		if(!parser.AddAttribute(p_element, XML_DESCRIPTION, (LPTSTR)(LPCTSTR) (*git).second.comment))
			return FALSE;
		if(!parser.AddAttribute(p_element, XML_GROUPTYPE, _itot((*git).second.type, buffer, 10)))
			return FALSE;

		if(!parser.AppendChild(p_element, p_node))
			return FALSE;

		p_node = p_node->lastChild;

		ctreeitem = m_treeRepository.GetChildItem(gtreeitem);

		// group ���� channel item ����...
		while(ctreeitem != NULL)
		{
			cit = m_mapChannel.find(m_treeRepository.GetItemData(ctreeitem));

			if(cit == m_mapChannel.end() || !(*cit).second.flush )
			{
				ctreeitem = m_treeRepository.GetNextSiblingItem(ctreeitem);
				continue;
			}

			p_element = parser.CreateElement(XML_OUTLINE);
			
			if(p_element == NULL)
				return FALSE;
			
			if(!parser.AddAttribute(p_element, XML_TYPE, XML_RSS))
				return FALSE;

			if((*cit).second.type != CT_SUBSCRIBE)
			{
				if(!parser.AddAttribute(p_element, XML_CHANNELTYPE, _itot((*cit).second.type, buffer, 10)))
					return FALSE;
			}
			
			if(!parser.AddAttribute(p_element, XML_ID, _itot((*cit).second.id, buffer, 10)))
				return FALSE;			
			if(!parser.AddAttribute(p_element, XML_TITLE, (LPTSTR)(LPCTSTR) (*cit).second.title))
				return FALSE;
			if((*cit).second.type != CT_SCRAP)
			{
				if(!parser.AddAttribute(p_element, XML_XMLURL, (LPTSTR)(LPCTSTR) (*cit).second.xmlurl))
					return FALSE;
			}
			if(!parser.AddAttribute(p_element, XML_GUID, (LPTSTR)(LPCTSTR) (*cit).second.guid))
				return FALSE;
			if((*cit).second.type == CT_SUBSCRIBE)
			{
				if(!parser.AddAttribute(p_element, XML_HTMLURL, (LPTSTR)(LPCTSTR) (*cit).second.churl))
					return FALSE;
			}
			if((*cit).second.keepcnt > 0) parser.AddAttribute(p_element, XML_KEEPCNT, _itot((*cit).second.keepcnt, buffer, 10));
			if((*cit).second.keepdays > 0) parser.AddAttribute(p_element, XML_KEEPDAYS, _itot((*cit).second.keepdays, buffer, 10));
			if(!parser.AddAttribute(p_element, XML_FEEDINTERVAL, _itot((*cit).second.feedinterval, buffer, 10)))
				return FALSE;
			if(!parser.AddAttribute(p_element, XML_DESCRIPTION, (LPTSTR)(LPCTSTR) (*cit).second.comment))
				return FALSE;
			if(!parser.AddAttribute(p_element, XML_BNS, ((*cit).second.bns) ? _T("1") : _T("0")))
				return FALSE;
			
			if(!parser.AppendChild(p_element, p_node))
				return FALSE;

			ctreeitem = m_treeRepository.GetNextSiblingItem(ctreeitem);
		}

		p_node = p_node->parentNode;
		gtreeitem = m_treeRepository.GetNextSiblingItem(gtreeitem);
	}

	// ������ ���ϸ����� ����
	if(parser.SaveXML((LPTSTR)(LPCTSTR) local))
		return TRUE;
	
	return FALSE;
}


/**************************************************************************
 * CDlg_Subscribe::OPMLExport
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-?? 
 *
 * @Description
 * ���õ� ��������Ʈ�� OPML ���Ϸ� ��������.
 *
 * @Parameters
 * (in const CString) filename - ����� ���� �̸�
 **************************************************************************/
BOOL CDlg_Subscribe::OPMLExport(const CString filename)
{
	// �Ϲ������� ����ϴ� ������ OPML�� ������ �����Ѵ�.
	// TO DO
	CEzXMLParser parser;
	CString	strTemp;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	CHANNEL_MAP::iterator cit;
	GROUP_MAP::iterator git;
	
	if(MakeDefaultOPML(parser, 1))// XML Document ����
	{
		// Body ��� ����
		p_nodelist = parser.SearchNodes(XML_XPATH_BODY);
		p_node = p_nodelist->item[0]; 

		// Tree�� ��ó�� �׷� item ����...
#ifdef _USE_MULTISELECTION_TREE_
		HTREEITEM treeitem;
		
		// Ʈ������ ���õ� ��ó�� �������� ������...
		treeitem = m_treeRepository.GetFirstSelectedItem();		

		// Tree Item �� ���������� ������� �о�.. OPML ����
		// Group Item �� ����...
		for(;treeitem != NULL; treeitem = m_treeRepository.GetNextSelectedItem(treeitem))
		{
            if(m_treeRepository.GetParentItem(treeitem) != NULL)
            {
                cit = m_mapChannel.find(m_treeRepository.GetItemData(treeitem));
                
                if(cit != m_mapChannel.end())
                {
                    p_element = parser.CreateElement(XML_OUTLINE);
                    if(p_element == NULL)
                        return FALSE;
                    
                    if(!parser.AddAttribute(p_element, XML_TYPE, XML_RSS))
                        return FALSE;	
                    if(!parser.AddAttribute(p_element, XML_TITLE, (LPTSTR)(LPCTSTR) (*cit).second.title))
                        return FALSE;				
                    if(!parser.AddAttribute(p_element, XML_XMLURL, (LPTSTR)(LPCTSTR) (*cit).second.xmlurl))
                        return FALSE;
                    if(!parser.AddAttribute(p_element, XML_HTMLURL, (LPTSTR)(LPCTSTR) (*cit).second.churl))
                        return FALSE;
                    if(!parser.AddAttribute(p_element, XML_DESCRIPTION, (LPTSTR)(LPCTSTR) (*cit).second.comment))
                        return FALSE;
                    
                    if(!parser.AppendChild(p_element, p_node))
                        return FALSE;			
                }
            }
		}
#else
        HTREEITEM treeitem  =   m_treeRepository.GetRootItem();
		HTREEITEM childtreeitem;
		
        for(; treeitem != NULL; treeitem = m_treeRepository.GetNextSiblingItem(treeitem))
		{
			if(childtreeitem = m_treeRepository.GetChildItem(treeitem))
			{
				git = m_mapGroup.find(m_treeRepository.GetItemData(treeitem));
				p_element = parser.CreateElement(XML_OUTLINE);

				if(!parser.AddAttribute(p_element, XML_TITLE, (LPTSTR)(LPCTSTR) (*git).second.title))
							return FALSE;
				if(!parser.AppendChild(p_element, p_node))
							return FALSE;

				p_node = p_element;

				if(p_node == NULL)
					return FALSE;
				
				for(; childtreeitem != NULL; childtreeitem = m_treeRepository.GetNextSiblingItem(childtreeitem))
				{
					cit = m_mapChannel.find(m_treeRepository.GetItemData(childtreeitem));
            
					if(cit != m_mapChannel.end() && (*cit).second.type == CT_SUBSCRIBE)
					{
						p_element = parser.CreateElement(XML_OUTLINE);
						if(p_element == NULL)
							return FALSE;
                
						if(!parser.AddAttribute(p_element, XML_TYPE, XML_RSS))
							return FALSE;	
						if(!parser.AddAttribute(p_element, XML_TITLE, (LPTSTR)(LPCTSTR) (*cit).second.title))
							return FALSE;				
						if(!parser.AddAttribute(p_element, XML_XMLURL, (LPTSTR)(LPCTSTR) (*cit).second.xmlurl))
							return FALSE;
						if(!parser.AddAttribute(p_element, XML_HTMLURL, (LPTSTR)(LPCTSTR) (*cit).second.churl))
							return FALSE;
						if(!parser.AddAttribute(p_element, XML_DESCRIPTION, (LPTSTR)(LPCTSTR) (*cit).second.comment))
							return FALSE;
                
						if(!parser.AppendChild(p_element, p_node))
							return FALSE;
					}
				}

				p_node = p_node->parentNode;
			}
		}
#endif

		if(parser.SaveXML((LPTSTR)(LPCTSTR) filename))
			return TRUE;
	}
	return FALSE;
}

BOOL CDlg_Subscribe::OPMLBackup()
{
	// ���� �ֱ⸦ �Ǵ��Ͽ� �����Ѵ�.
	// OMPL�� ���� ������ ������ ���Ϸ� ����ϰ�
	// TO DO

	// ftp�� �����ϸ�
	// ������ ���� �������� �˸���.
	// ���߿�
	return FALSE;
}


/************************************************************************** 
 * written by aquarelle
 *
 * @history
 * created 2005-11-28
 * modified 2005-12-19 FISH�� �����ʹ� ��� ��Ʈ���� �����ͷ� ó��.
 *
 * @Description
 * OPML ������ �⺻ Type�� ����
 *
 * @Parameters]
 * (in/out CEzXMLParser&) parser 
 * (in int) type : 0 - Fish�� OPML, 1 - �Ϲ� OPML, defualt �� 0
 *
 * @Returns
 * (BOOL)
 * FALSE : failed
 * TRUE : succeeded
 **************************************************************************/
BOOL CDlg_Subscribe::MakeDefaultOPML(CEzXMLParser& parser, int type)
{
	CString	strTemp;
	BOOL	result;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;	
	COleDateTime dt;

	strTemp = XML_DECLARATION;
	strTemp += OPML_ROOT_DECLARATION;

	parser.Clear();

	if(!parser.LoadXML((LPTSTR)(LPCTSTR) strTemp, CEzXMLParser::LOAD_STRING))
		return FALSE;

	// <head/><body/> ����
	p_nodelist = parser.SearchNodes(XML_XPATH_OPML);
	p_node = p_nodelist->item[0];
	p_element = parser.CreateElement(XML_HEAD);		
	result = parser.AppendChild(p_element, p_node);
	
	if(type == 0) // Fish �� OPML �ΰ��
	{
		// Ÿ��Ʋ ǥ��...
		result = parser.AddAttribute(p_element, XML_TITLE, _T("Fish Feed List"));
		dt = COleDateTime::GetCurrentTime();

		// ���� �ð� ǥ��...
		result = parser.AddAttribute(p_element, XML_DATECREATED, (LPTSTR)(LPCTSTR) dt.Format(FISH_DT_TEMPLETE));

		// ���� �ð� ǥ��...
		result = parser.AddAttribute(p_element, XML_DATEMODIFIED, (LPTSTR)(LPCTSTR) dt.Format(FISH_DT_TEMPLETE));

		// ������ ǥ��...
		result = parser.AddAttribute(p_element, XML_OWNERNAME, _T(""));
		result = parser.AddAttribute(p_element, XML_EXPANSIONSTATE, _T("Fish"));
	}
	else
	{
		// head �ȿ� title �±� ����..	
		p_element = parser.CreateElement(XML_TITLE);
		p_element->text = "OPML generated by Fish";	
		result = parser.AppendChild(p_element, p_node->firstChild);

		// ���� �ð� ǥ��...
		p_element = parser.CreateElement(XML_DATECREATED);
		dt = COleDateTime::GetCurrentTime();
		p_element->text = (LPTSTR)(LPCTSTR) dt.Format(FISH_DT_TEMPLETE);
		result = parser.AppendChild(p_element, p_node->firstChild);
		// ���� �ð� ǥ��...
		p_element = parser.CreateElement(XML_DATEMODIFIED);
		dt = COleDateTime::GetCurrentTime();
		p_element->text = (LPTSTR)(LPCTSTR) dt.Format(FISH_DT_TEMPLETE);
		result = parser.AppendChild(p_element, p_node->firstChild);

		// ������ ǥ��...
		p_element = parser.CreateElement(XML_OWNERNAME);
		//p_element->text = _T("...");
		result = parser.AppendChild(p_element, p_node->firstChild);
		
		/*
		// Fish ǥ��....
		p_element = parser.CreateElement(XML_EXPANSIONSTATE);		
		if(type == 0)
		{
			p_element->text = _T("Fish");
		}
		result = parser.AppendChild(p_element, p_node->firstChild);
		*/
	}

	p_element = parser.CreateElement(XML_BODY);
	result = parser.AppendChild(p_element, p_node);

	return result;	
}

/************************************************************************** 
 * written by moonknit
 *
 * @history
 * created 2005-12
 *
 * @Description
 * �ֱ����� Fetch �۾��� �����Ѵ�.
 **************************************************************************/
void CDlg_Subscribe::Manage()
{
	// �ֱ����� Fetch�� �̰����� �����Ѵ�.
	CHANNEL_MAP::iterator it;
	COleDateTimeSpan s;
	COleDateTime cur = COleDateTime::GetCurrentTime();
	for(it = m_mapChannel.begin(); it != m_mapChannel.end(); ++it)
	{
		if((*it).second.feedinterval <= 0) continue;

		s.SetDateTimeSpan(0, 0, (*it).second.feedinterval, 0);
		if((*it).second.dtlastfeed + s < cur)
		{
			// ������ �ڵ� Fetch �����ۿ� ���ؼ��� Post ������ �����Ѵ�.
			if(!m_feedcontrol.IsFeedAny())
			{
				m_bFeedInform = TRUE;
				m_nManageFeedPost = 0;
				m_nManageFeedChannel = 0;
			}

			(*it).second.dtlastfeed = cur;
			Fetch((*it).first, (*it).second.xmlurl, (*it).second.dtlastbuild);
		}
	}

	m_feedcontrol.Feed();

	OPMLBackup();
}


//////////////////////////////////////////////////////////////////////
// Tree control method


// �׷��� ��� Tree���� �׷������ ������ ��ȯ�ϰ�
// ä���� ��� �׷� ���� Tree���� ������ ��ȯ�Ѵ�.
int CDlg_Subscribe::Tree_ItemAdd(int id, HTREEITEM* param  /*=   NULL*/)
{
	// TO DO
	auto_ptr<FISH_REPITEM> apitem;
	FindItem(id, apitem);

	return Tree_ItemAdd(apitem.get(), param);
}

/************************************************************************
Tree_ItemAdd 
@param  : 
@return : 
	���� �߰��� �������� ��ȣ�� �����Ѵ�.
	0 : ������ ���
	GROUP ���̿����� ��ȣ, CHANNEL ���̿����� ��ȣ�� �����ϸ�,
	index 1 �� base-index �̴�.
@remark : 
	���� ���� Ʈ���󿡼� ������ �������� �����Ѵٸ� �� �������� �ϴܿ�
	���ο� �������� �߰��Ѵ�. 
    
    CHANNEL:
    ���� �������� �ʴ»��¿��� �߰��� ���̶��
    �ֻ���� �׷� �����ۿ� ä���� �߰��ϰ� �ȴ�. �׷��� ���ٸ� 0�� �����ϰ�
    ä���� �߰����� �ʴ´�.

    ������ Ʈ���� ��Ҹ����� �� �˻縦 �ϱ� ������ ���� ������ �ڷ��� ���¿�
    �ٸ� ���� ����

    �������� �ε����� ��� ��ƾ�� ���� private �޼ҵ�� ������.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
    imple. complete 2005/11/24
************************************************************************/
int CDlg_Subscribe::Tree_ItemAdd(FISH_REPITEM* pitem, HTREEITEM* param  /*=   NULL*/)
{
	// TODO: �Է� �׷� �������� �̸��� �Ҵ��ϴ� ������ ������. SetData �� ID���� ���õ� �߰�
    assert (pitem != NULL);
    int ret_value =   0;
	switch (pitem->rep_type)
	{
	case REP_CHANNEL:
		{
            //////////////////////////////////////////////////////////////////////////
            // ä�� �߰�
			HTREEITEM       hCurrentItem	=   Tree_ItemHandleFrom( ((FISH_CHANNEL*)pitem)->groupid );
			if (hCurrentItem == NULL)   break;
            
			HTREEITEM ret = m_treeRepository.InsertItem(pitem->title, Tree_IconDefaultIndexOf(pitem), Tree_IconSelectedIndexOf(pitem), hCurrentItem);
	        m_treeRepository.SetItemData(ret, (DWORD) pitem->id);
            if (param)  *param  =   ret;
            m_treeRepository.ShowScrollBar(SB_HORZ, TRUE);

            //////////////////////////////////////////////////////////////////////////
            // ���� ���� ����. 
            HTREEITEM iter = m_treeRepository.GetChildItem( m_treeRepository.GetParentItem(ret) );
            int nIndex = 0;
            do
            {
                if (iter == ret)    {
                    ++nIndex;
                    ret_value =   nIndex;
                }
                ++nIndex;
            } while( iter = m_treeRepository.GetNextSiblingItem(iter) );
    		break;
		}
	case REP_GROUP:
		{
            //////////////////////////////////////////////////////////////////////////
            // �׷� �߰�
	        HTREEITEM ret = m_treeRepository.InsertItem(pitem->title, Tree_IconDefaultIndexOf(pitem), Tree_IconSelectedIndexOf(pitem));
	        m_treeRepository.SetItemData(ret, (DWORD) pitem->id);
            if (param)  *param  =   ret;
            m_treeRepository.ShowScrollBar(SB_HORZ, TRUE);

            //////////////////////////////////////////////////////////////////////////
            // ���� ���� ����. 
            HTREEITEM iter = m_treeRepository.GetRootItem();
            int nIndex = 0;
            do
            {
                if (iter == ret)    {
                    ++nIndex;
                    ret_value =   nIndex;
                }
                ++nIndex;
            } while( iter = m_treeRepository.GetNextSiblingItem(iter) );
            m_treeRepository.ShowScrollBar(SB_HORZ, TRUE);
    		break;
        }
    default:
        {
//            AfxMessageBox( _T("�˼����� Ÿ�� �������� �߰��Ϸ��� �Ͽ����ϴ�.") );
            FishMessageBox( _T("�˼����� Ÿ�� �������� �߰��Ϸ��� �Ͽ����ϴ�.") );
        }
	}

	//call this to position the scrollbars properly
    m_treeRepository.ShowScrollBar(SB_HORZ, TRUE);
	return ret_value;
}

/************************************************************************
Tree_ItemUpdate ���ڷ� ���޹��� �������� ������ �̿� Ʈ������ ������ ������Ʈ
@param  : 
@return : 
@remark : 
    ���޹��� ������ ID�� �̿��ؼ� ã�´�. ���� ID�� �ش��ϴ� Ʈ�� ��������
    ��ȿ���� assertion �Ѵ�.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
    imple. complete 2005/11/24
************************************************************************/
void CDlg_Subscribe::Tree_ItemUpdate(FISH_REPITEM* pitem)
{
    assert( pitem != NULL );
//    TRACE(pitem->title);
    HTREEITEM hItem = Tree_ItemHandleFrom(pitem->id);
    if (!hItem) return;     // �Ⱥ��̴� �������� ��쿡�� NULL ���� ����.

    int nINDEX  =   0;

    m_treeRepository.GetItemImage(hItem, nINDEX, nINDEX);
    if(hItem != NULL)   m_treeRepository.updateItem(hItem, (TCHAR*)pitem->title.GetBuffer(255), nINDEX, nINDEX, pitem->id);
}

/************************************************************************
Tree_ItemRemove     ���ڷ� ���޹��� ID�� ���� �������� �����Ѵ�.
@param  : 
@return : 
@remark : 
    �׷� �������� ��쿡�� ������ ���� �����۵鵵 ������.
    ä�� �������� �ܿ����� ä�� ������ �ܵ����θ� ������.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
    imple. complete 2005/11/24
************************************************************************/
void CDlg_Subscribe::Tree_ItemRemove(int id)
{
    assert ( m_treeRepository.GetCount() != 0 );
    HTREEITEM hItem;
    if ( hItem = Tree_ItemHandleFrom(id) ) 
        m_treeRepository.DeleteItem(hItem);


	//call this to position the scrollbars properly
    m_treeRepository.ShowScrollBar(SB_HORZ, TRUE);
}

/************************************************************************
Tree_ItemMove 
@param  : 
@return : 
    TRUE : �̵��� �������� ���
    FALSE : �̵��� ������ ���
@remark : 
    �������� �̵��� ó���ϴ� �κ���. 
    SRCID, DESTID�� ������ �����ϰ� �����ϵ��� �������.
    �׷� -> �׷� : ��� �׷��� �ڷ� ������ �̵�
    �׷� -> ä�� : ä���� �θ� �������� �ڷ� �̵�
    ä�� -> ä�� : ��� ä���� �ڷ� �̵� (�ٸ� �׷��� ä�ε� �̵� ������)
    ä�� -> �׷� : ��� �׷��� ���� �ڿ� ����
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
    imple. complete 2005/11/24
************************************************************************/
BOOL CDlg_Subscribe::Tree_ItemMove(int src, int dest)
{
    assert ( m_treeRepository.GetCount() != 0 );
    //////////////////////////////////////////////////////////////////////////
    // �ش� ID �� ������ �ڵ��� ���Ѵ�.
    HTREEITEM hSRC, hDEST;
    assert ( (hSRC    =   Tree_ItemHandleFrom(src)) && (hDEST   =  Tree_ItemHandleFrom(dest)) );

    //////////////////////////////////////////////////////////////////////////
    // �ش� �ڵ��� ������ ���� Ʈ���� �����ϴ� �޼ҵ� INVOKE
    if (m_treeRepository.GetParentItem(hSRC) == NULL)
    {
        if (m_treeRepository.GetParentItem( hDEST ) == NULL)
        {
            m_treeRepository.moveGroupPos(hSRC, hDEST);
			m_bFlushOPML = TRUE;
            return TRUE;
        } else 
        {
            hDEST = m_treeRepository.GetParentItem(hDEST);
            m_treeRepository.moveGroupPos(hSRC, hDEST);
			m_bFlushOPML = TRUE;
            return TRUE;
        }
    } else
    {
        if ( m_treeRepository.GetParentItem( hDEST ) == NULL)
        {
            m_treeRepository.moveChannelToGroup(hSRC, hDEST);
			m_bFlushOPML = TRUE;
            return TRUE;
        } else 
        {
            m_treeRepository.moveChannelPos(hSRC, hDEST);
			m_bFlushOPML = TRUE;
            return TRUE;
        }
    }
	return FALSE;
}

/************************************************************************
Tree_ItemHandleFrom ���ڷ� ���� ���� ID�� DATA�� ������ �ִ� �������� 
                    �ڵ��� �����Ѵ�.
@param  : 
@return : 
    0           :   id �� ������ ���� ���� ���
    HTREEITEM   :   id �� ������ ���� �ִ� ���
@remark : 
    �ߺ��� ����� �ߺ��� ���ϱ� ���� Extract Method.
    ������ ������ ���� ������ ��Ұ� ���ٸ� 0�� �����ϵ��� �����Ǿ���.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/24
    imple. complete 2005/11/24
************************************************************************/
HTREEITEM   CDlg_Subscribe::Tree_ItemHandleFrom(int id)
{
//	TRACE(_T("Tree_ItemHandlFrom\r\n"));
	return m_treeRepository.GetHandleFromData(id);

/*
    HTREEITEM ret = 0;
    for (HTREEITEM iter = m_treeRepository.GetFirstVisibleItem(); iter != 0; iter = m_treeRepository.GetNextVisibleItem(iter) )
    {
        if (id == m_treeRepository.GetItemData(iter) )            return (ret = iter);
    }

    return ret;
	*/
}

/************************************************************************
Tree_IconIndexOf    ���ڷ� ���� �������� ������ �ε����� ��ȯ�Ѵ�.
@param  : 
@return : 
@remark : 
    ����� �׷�� Ʈ�� �������� �������� 0�� 1�� ������ ��������.
    ���� ������ ������ ������.
    (�߰����ڸ� �ڷᱸ�� ���ο� �������� �ε��� ���� ���� ���� �־���� ������ ����.)
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/24
************************************************************************/
unsigned int    CDlg_Subscribe::Tree_IconDefaultIndexOf(FISH_REPITEM* pItem)
{
    enum {
        CI_SUBSCRIBE    =   0,
        CI_SCRAP        =   2,
        CI_SEARCH       =   4,
        CI_KEYWORD      =   3,
        CI_NONE         =   0,
    };

    if (pItem->rep_type == REP_CHANNEL){
        switch( ((FISH_CHANNEL*)pItem)->type )
        {
        case CT_SUBSCRIBE:
            return CI_SUBSCRIBE;
            break;
        case CT_SCRAP:
            return CI_SCRAP;
            break;
		case CT_LSEARCH:
        case CT_SEARCH:
            return CI_SEARCH;
            break;
        case CT_KEYWORD:
            return CI_KEYWORD;
            break;
        default:
            return CI_NONE;
            break;
        }
    }
     
	return NULL;
}

/************************************************************************
Tree_IconSelectedIndexOf    ���ڷ� ���� �������� ���ý��� ������ �ε����� ��ȯ�Ѵ�.
@param  : 
@return : 
@remark : 
    ����� �⺻ �����ܰ� ������ �ε����� ��ȯ�Ѵ�.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/24
************************************************************************/
unsigned int    CDlg_Subscribe::Tree_IconSelectedIndexOf(FISH_REPITEM* pItem)
{
    enum {
        CI_SUBSCRIBE    =   5,
        CI_SCRAP        =   7,
        CI_SEARCH       =   4,
        CI_KEYWORD      =   8,
        CI_NONE         =   5,
    };


    if (pItem->rep_type == REP_CHANNEL){
        switch( ((FISH_CHANNEL*)pItem)->type )
        {
        case CT_SUBSCRIBE:
            return CI_SUBSCRIBE;
            break;
        case CT_SCRAP:
            return CI_SCRAP;
            break;
		case CT_LSEARCH:
        case CT_SEARCH:
            return CI_SEARCH;
            break;
        case CT_KEYWORD:
            return CI_KEYWORD;
            break;
        default:
            return CI_NONE;
            break;
        }
    }
     
	return NULL;
}

void CDlg_Subscribe::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE(_T("CDlg_Subscribe::OnSize\r\n"));
//	CDialog::OnSize(nType, cx, cy);

	// ������ ��Ʈ�ѵ��� ��ġ�� �����Ѵ�.
	MoveControls();    
}

//////////////////////////////////////////////////////////////////////
// Other UI methods
void CDlg_Subscribe::MoveControls()
{
    if(!m_bInit) return;

	// ���� ��Ʈ���� ��ġ�� �����Ѵ�.
	// TO DO
	CRect clientRT;
    GetClientRect(&clientRT);

	CRgn rgnControl, rgnPaint;
	rgnPaint.CreateRectRgn(0, 0, 0, 0);
	rgnControl.CreateRectRgn(0, 0, 0, 0);
	rgnPaint.SetRectRgn(&clientRT);

    //////////////////////////////////////////////////////////////////////////
    // BUTTON RELAYOUT
    {
        CRect btnRT = clientRT;
        btnRT.left  +=  REPOSITORY_BTN_LEFT_PADDING;
        btnRT.right = btnRT.left + REPOSITORY_SIZE_BTN_WIDTH;
		btnRT.top	= REPOSITORY_BTN_TOP_PADDING;
		btnRT.bottom = btnRT.top + REPOSITORY_SIZE_BTN_HEIGHT;

        //UINT hInterval = (btnRT.right - btnRT.left - 5 * REPOSITORY_SIZE_BTN_WIDTH) / (REPOSITORY_BTN_QUANTITY - 1);
        UINT hInterval = REPOSITORY_BTN_INTERVAL;
        
        m_btnAddGroup.MoveWindow( &btnRT );
//		rgnControl.SetRectRgn(&btnRT);
//		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

		btnRT.left = btnRT.right + hInterval;
		btnRT.right = btnRT.left + REPOSITORY_SIZE_BTN_WIDTH;
        m_btnAddChannel.MoveWindow( &btnRT );
//		rgnControl.SetRectRgn(&btnRT);
//		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

		btnRT.left = btnRT.right + hInterval;
		btnRT.right = btnRT.left + REPOSITORY_SIZE_BTN_WIDTH;
        m_btnAddScrap.MoveWindow( &btnRT);
//		rgnControl.SetRectRgn(&btnRT);
//		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

		btnRT.left = btnRT.right + hInterval;
		btnRT.right = btnRT.left + REPOSITORY_SIZE_BTN_WIDTH;
        m_btnAddKeyword.MoveWindow( &btnRT);
//		rgnControl.SetRectRgn(&btnRT);
//		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

		btnRT.left = btnRT.right + hInterval;
		btnRT.right = btnRT.left + REPOSITORY_SIZE_BTN_WIDTH;
        m_btnRefresh.MoveWindow( &btnRT);
//		rgnControl.SetRectRgn(&btnRT);
//		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

    }

    //////////////////////////////////////////////////////////////////////////
    // TREE RELAYOUT
    {
        CRect treeRT(
            clientRT.left + REPOSITORY_TREE_LEFT_PAD, 
            clientRT.top + REPOSITORY_TREE_TOP_PAD, 
            clientRT.right - REPOSITORY_TREE_RIGHT_PAD, 
            clientRT.bottom - REPOSITORY_TREE_BOTTOM_PAD
            );
        m_treeRepository.MoveWindow(&treeRT);

        //////////////////////////////////////////////////////////////////////////
        // hiding the horizontal scrollbar
//        m_treeRepository.HideScrollbar();
		rgnControl.SetRectRgn(&treeRT);
		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);
    }

	m_rgnPaint.CopyRgn(&rgnPaint);
	Invalidate();
    m_treeRepository.Invalidate();
}

BOOL CDlg_Subscribe::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bInit = TRUE;

	m_feedcontrol.SetCallerWnd(this->GetSafeHwnd());

	// TREE INIT
    {
        m_treeRepository.SetItemHeight(CFishTreeCtrl::FISH_REPOSITORY_TREE_ITEM_HEIGHT);
        m_treeRepository.SetBkColor(CFishTreeCtrl::FISH_REPOSITORY_BG_COLOR);
        m_treeRepository.SetIndent(CFishTreeCtrl::FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT);
        m_treeRepository.SetFont( &m_treeRepository.m_ftGroupDefault );
        m_treeRepository.SetTimer(TID_REPOSITORY_ANIMATION, CFishTreeCtrl::FISH_REPOSITORY_ANIMATION_INTERVAL, NULL);
        m_treeRepository._initScrollBar();
    }

    // BTN BITMAP SET
    {
	    m_btnAddScrap.DrawBorder(FALSE);
	    m_btnRefresh.DrawBorder(FALSE);
	    m_btnAddKeyword.DrawBorder(FALSE);
	    m_btnAddGroup.DrawBorder(FALSE);
	    m_btnAddChannel.DrawBorder(FALSE);

	    m_btnAddScrap.SetShowText(FALSE);
	    m_btnRefresh.SetShowText(FALSE);
	    m_btnAddKeyword.SetShowText(FALSE);
	    m_btnAddGroup.SetShowText(FALSE);
	    m_btnAddChannel.SetShowText(FALSE);

        m_btnAddScrap.LoadBitmaps(IDB_REPOSITORY_ADDSCRAP_N, IDB_REPOSITORY_ADDSCRAP_P, IDB_REPOSITORY_ADDSCRAP_O, NULL
			, REPOSITORY_SIZE_BTN_WIDTH, REPOSITORY_SIZE_BTN_HEIGHT);
        m_btnRefresh.LoadBitmaps(IDB_REPOSITORY_REFRESH_N, IDB_REPOSITORY_REFRESH_P, IDB_REPOSITORY_REFRESH_O, NULL
			, REPOSITORY_SIZE_BTN_WIDTH, REPOSITORY_SIZE_BTN_HEIGHT);
        m_btnAddKeyword.LoadBitmaps(IDB_REPOSITORY_ADDKEYWORD_N, IDB_REPOSITORY_ADDKEYWORD_P, IDB_REPOSITORY_ADDKEYWORD_O, NULL
			, REPOSITORY_SIZE_BTN_WIDTH, REPOSITORY_SIZE_BTN_HEIGHT);
        m_btnAddGroup.LoadBitmaps(IDB_REPOSITORY_ADDGROUP_N, IDB_REPOSITORY_ADDGROUP_P, IDB_REPOSITORY_ADDGROUP_O, NULL
			, REPOSITORY_SIZE_BTN_WIDTH, REPOSITORY_SIZE_BTN_HEIGHT);
        m_btnAddChannel.LoadBitmaps(IDB_REPOSITORY_ADDCHANNEL_N, IDB_REPOSITORY_ADDCHANNEL_P, IDB_REPOSITORY_ADDCHANNEL_O, NULL
			, REPOSITORY_SIZE_BTN_WIDTH, REPOSITORY_SIZE_BTN_HEIGHT);		

//		m_btnAddScrap.SetFlatFocus(TRUE);
//		m_btnRefresh.SetFlatFocus(TRUE);
//		m_btnAddKeyword.SetFlatFocus(TRUE);
//		m_btnAddGroup.SetFlatFocus(TRUE);
//		m_btnAddChannel.SetFlatFocus(TRUE);
    }

    //////////////////////////////////////////////////////////////////////////
    // TOOLTIP INIT
    
    {
        m_Tooltip.Create(this, TTS_ALWAYSTIP);

        //////////////////////////////////////////////////////////////////////////
        //m_btnAddScrap.GetWindowText(szTooltip);

        m_ToolInfo.cbSize   =   sizeof(TOOLINFO);
        m_ToolInfo.uFlags   =   TTF_SUBCLASS | TTF_IDISHWND | TTF_TRANSPARENT | TTF_CENTERTIP;
        m_ToolInfo.hwnd     =   GetSafeHwnd();
		
        //ti.uId      =   (WPARAM)m_btnAddScrap.GetSafeHwnd();
        //ti.lpszText =   szTooltip.GetBuffer(MAX_BUFF);
        //m_Tooltip.SetToolInfo(&ti);
		m_Tooltip.SetDelayTime(200);
		
		m_Tooltip.AddTool(&m_btnAddScrap, STR_TOOLTIP_ADD_SCRAP);
		m_Tooltip.AddTool(&m_btnAddGroup, STR_TOOLTIP_ADD_GROUP);
		m_Tooltip.AddTool(&m_btnAddChannel, STR_TOOLTIP_ADD_CHANNEL);
		m_Tooltip.AddTool(&m_btnAddKeyword, STR_TOOLTIP_ADD_KEYWORD);
		m_Tooltip.AddTool(&m_btnRefresh, STR_TOOLTIP_REFRESH);

    }
    

	m_uIDTimer = SetTimer(TID_SUBSCRIBE_OPMLSAVE, 10 * MINUTE, NULL);

	theApp.m_spLP->SetSubscribeHwnd(GetSafeHwnd());

	if(m_uIDTimer == 0)
		return FALSE;
	else
		return TRUE;  // return TRUE unless you set the focus to a control
			          // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlg_Subscribe::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			if(m_Tooltip.HitTest(&m_btnAddScrap, pMsg->pt, &m_ToolInfo))
			{				
				m_Tooltip.RelayEvent(pMsg);
			}
			else if(m_Tooltip.HitTest(&m_btnAddChannel, pMsg->pt, &m_ToolInfo))
			{
				m_Tooltip.RelayEvent(pMsg);
			}
			else if(m_Tooltip.HitTest(&m_btnAddGroup, pMsg->pt, &m_ToolInfo))
			{
				m_Tooltip.RelayEvent(pMsg);
			}
			else if(m_Tooltip.HitTest(&m_btnAddKeyword, pMsg->pt, &m_ToolInfo))
			{
				m_Tooltip.RelayEvent(pMsg);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{	
		}
		break;
	case WM_KEYDOWN:
        switch (pMsg->wParam)
        {
        case VK_ESCAPE:
            return TRUE;
            break;
        case VK_RETURN:
            RequestPost();
            return TRUE;
            break;

        case VK_DELETE:
            OnReposCmDelete();
            return TRUE;
            break;

        case 'd':
        case 'D':
            break;

        case VK_TAB:
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
            break;

        case VK_F2:
            m_treeRepository.EditLabel( m_treeRepository.GetSelectedItem() );
            return TRUE;
            break;

        case VK_RIGHT:

            if (m_treeRepository.GetItemData( m_treeRepository.GetSelectedItem() ) == theApp.m_spCL->GetCurrentListID() )
            {
                CFocusManager::getInstance()->MoveToListCtrl();
            }
            else
            {
                if ( m_treeRepository.GetParentItem( m_treeRepository.GetSelectedItem() ) )
                {
                    RequestPost(FALSE, TRUE);
                    CFocusManager::getInstance()->MoveToListCtrl();
                }
                else
                    CDialog::PreTranslateMessage(pMsg);
            }

            return TRUE;

        case VK_LEFT:
            CDialog::PreTranslateMessage(pMsg);
            return TRUE;

        default:
            break;
        }

		break;
	default:
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/************************************************************************
OnBtnAddscrap ��ũ�� �߰� ��ư
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::OnBtnAddscrap() 
{
	// TODO: Add your control notification handler code here
	OnReposCmAddScrap();
}

/************************************************************************
OnBtnAddkeyword Ű���� �߰� ��ư
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::OnBtnAddkeyword() 
{
	// TODO: Add your control notification handler code here
	OnReposCmAddKeyword();	
}

/************************************************************************
OnBtnAddchannel		ä�� �߰� ��ư
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
		���� �׽�Ʈ�� �׷� �߰��� ���� �ϰ� ����.
************************************************************************/
void CDlg_Subscribe::OnBtnAddchannel() 
{
	// TODO: Add your control notification handler code here
    //////////////////////////////////////////////////////////////////////////
    // ä�� �߰�
	OnReposCmAddChannel();
}

/************************************************************************
OnBtnAddgroup		�׷� �߰� ��ư
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/22
		���� �׽�Ʈ�� ä�� �߰��� �۾��� �ϰ� ����.
************************************************************************/
void CDlg_Subscribe::OnBtnAddgroup() 
{
	// TODO: Add your control notification handler code here
    //////////////////////////////////////////////////////////////////////////
    // �׷� �߰�
	OnReposCmAddGroup();

}

/************************************************************************
OnBtnRefresh    ��ü �������� FETCH
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:CREATED
************************************************************************/
void CDlg_Subscribe::OnBtnRefresh() 
{
//	if(!CRSSDB::IsPostInit())
//	{
//		FishMessageBox(	STR_INFRO_WAITPOSTINIT );
//		return;
//	}

    GROUP_MAP::iterator it = m_mapGroup.begin();
    for (; it!= m_mapGroup.end(); it++)
    {
        FetchByUser(it->first);
    }
//    m_btnRefresh.SetFocus();
}

/************************************************************************
GetScrapList ������ ������ ��ũ���� ����Ʈ�� �Ҵ��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
int CDlg_Subscribe::GetScrapList(list<FISH_CHANNEL>& listchannel)
{
	listchannel.clear();

	CHANNEL_MAP::iterator it;

	for(it = m_mapChannel.begin(); it != m_mapChannel.end(); ++it)
	{
		if((*it).second.type == CT_SCRAP)
		{
			listchannel.push_back((*it).second);
		}
	}

	return listchannel.size();
}

/************************************************************************
DrawItems ����� �پ��� ��Ҹ� �׸���.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::DrawItems(CDC *pDC)
{
	CRect rectDlg;	
	GetClientRect(&rectDlg);

	if(rectDlg.Width() == 0 || rectDlg.Height() == 0) return;

	if(!pDC) return;

	CDC memDC, bufferDC;
	CBitmap bmpBuffer;
	CBitmap* pOldBmp;
	bmpBuffer.CreateCompatibleBitmap(pDC, rectDlg.Width(), rectDlg.Height());

	memDC.CreateCompatibleDC(pDC);
	bufferDC.CreateCompatibleDC(pDC);
	bufferDC.SetBkMode(TRANSPARENT);
	pOldBmp = (CBitmap*) bufferDC.SelectObject(&bmpBuffer);

	// copy back ground
	bufferDC.BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), pDC, 0, 0, SRCCOPY);

	DrawBg(&bufferDC, &memDC);

	pDC->BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), &bufferDC, 0, 0, SRCCOPY);

	bufferDC.SelectObject(pOldBmp);

	bmpBuffer.DeleteObject();
	bufferDC.DeleteDC();
	memDC.DeleteDC();
}

/************************************************************************
DrawBg ���� ����� �׸���.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
    enum {
        FISH_REPOSITORY_BG_COLOR =   CFishTreeCtrl::FISH_REPOSITORY_BG_COLOR,
        BKGND_TOP_PADDING   =   REPOSITORY_TREE_TOP_PAD - 6,
    };
	CRect clientRT, tempRT;
	GetClientRect(&clientRT);WS_EX_TOPMOST;
	//////////////////////////////////////////////////////
	// Fill Background color
	pBufferDC->FillSolidRect(0, 0, clientRT.Width(), clientRT.Height(), FISH_REPOSITORY_BG_COLOR);

//	pBufferDC->FillRgn(&m_rgnPaint, &m_brushBG);

    CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
    CBitmap* bmpSubscribeBkgndTop   =   CFishBMPManager::getInstance()->getSubscribeBkgnd(0);
    CBitmap* bmpSubscribeBkgndLeft  =   CFishBMPManager::getInstance()->getSubscribeBkgnd(1);
    CBitmap* bmpSubscribeBkgndRight =   CFishBMPManager::getInstance()->getSubscribeBkgnd(2);
    CBitmap* bmpSubscribeBkgndBottom=   CFishBMPManager::getInstance()->getSubscribeBkgnd(3);
    BMPManager->drawTransparent(pBufferDC, bmpSubscribeBkgndTop, CPoint(0, BKGND_TOP_PADDING), TRUE, RGB(0xFF, 0x00, 0xFF));
    BMPManager->drawTransparent(pBufferDC, bmpSubscribeBkgndLeft, CPoint(0, BKGND_TOP_PADDING+6), TRUE, RGB(0xFF, 0x00, 0xFF));
    BMPManager->drawTransparent(pBufferDC, bmpSubscribeBkgndRight, CPoint(188, BKGND_TOP_PADDING+6), TRUE, RGB(0xFF, 0x00, 0xFF));
//    BMPManager->drawTransparent(pBufferDC, bmpSubscribeBkgndBottom, CPoint(0, BKGND_TOP_PADDING), TRUE, RGB(0xFF, 0x00, 0xFF));
}

BOOL CDlg_Subscribe::OnEraseBkgnd(CDC* pDC) 
{
//	TRACE(_T("CDlg_Subscribe::OnEraseBkgnd\r\n"));
	if(m_bSelfDraw)
	{
//		DrawItems(pDC);
		return FALSE;
	}

	return CDialog::OnEraseBkgnd(pDC);
}

/**************************************************************************
 * CDlg_Subscribe::RequestPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 * updated 2006-02-16 :: ä�� ������ ��쿡�� �˻� ä�� ����� �˻� ������ ������ �� �ֵ��� ������ �߰� �Ѵ�.
 * updated 2006-03-20 :: parameter bautoselect �߰� by moonknit
 *
 * @Description
 * �ش� �׷� �Ǵ� ä���� ����Ʈ�� Current ������� �о���̵��� ��û�Ѵ�.
 *
 * @Parameters
 * (in BOOL) bunreadonly - ���� ���� ����Ʈ���� �о������ ó���Ѵ�.
 * (in BOOL) bautoselect - ä��/�׷� ���� ����� list ���� ù��° �����ۿ� selection ó�� ���� 
 **************************************************************************/
void CDlg_Subscribe::RequestPost(BOOL bunreadonly, BOOL bautoselect)
{
	BOOL  bfind =   FALSE;
    DWORD dwData=   -1;
    HTREEITEM   hITEM   =   NULL;
    if ( hITEM  =   m_treeRepository.GetSelectedItem() )
    {
        bfind   =   TRUE;
        dwData  =   m_treeRepository.GetItemData( hITEM );
    }
    
	if( !bfind ) return;

	CHANNEL_MAP::iterator it = m_mapChannel.find(dwData);

	if(it != m_mapChannel.end())
	{
        // ������ �ϴ� ä���� ��ũ��, �˻��� ��쿡�� bunreadonly �� false�� ó���Ѵ�.
		theApp.m_spCL->RequestChannelOnLoad(dwData, it->second.type, bunreadonly, bautoselect);

		if(it->second.type == CT_SEARCH 
			|| it->second.type == CT_LSEARCH
			|| it->second.type == CT_KEYWORD)
		{
			theApp.m_spCL->SetSearchWord(it->second.xmlurl);
		}

		return;
	}

	GROUP_MAP::iterator it2 = m_mapGroup.find(dwData);

	if(it2 != m_mapGroup.end())
	{
		list<int> idlist;
		for(it = m_mapChannel.begin(); it != m_mapChannel.end(); ++it)
		{	
			if((*it).second.groupid == dwData)
			{
				idlist.push_back((*it).second.id);
			}
		}

		if(idlist.size() > 0)
			theApp.m_spCL->RequestGroupOnLoad(dwData, idlist, bunreadonly, bautoselect);
		return;
	}
}

/**************************************************************************
 * CDlg_Subscribe::OnChannelStateChange
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 * modified 2006-02-10 :: TrayIcon Animaition �ڵ� �߰� by aquarelle
 * modified 2006-03-02 :: m_treeRepository Validation Check by moonknit
 *
 * @Description
 * ä���� ���°� ���� ���
 *
 * @Parameters
 * (in WPARAM) wParam - ä���� ID
 * (in LPARAM) lParam - ä���� ����
 **************************************************************************/
void CDlg_Subscribe::OnChannelStateChange(WPARAM wParam, LPARAM lParam)
{
	if(!m_treeRepository.GetSafeHwnd()) return;

	// added by aquarelle
	CMainFrame* mainfrm = (CMainFrame*)AfxGetMainWnd();
	if(!mainfrm) return;

	SmartPtr<CTray> sptrayicon;
	mainfrm->GetTrayIcon(sptrayicon);
	//--

    switch(lParam)
    {
    case CS_NEW:        // ä�ο� ���ο� �������� �ִ� ���
        {
            TRACE(_T("(REPOS CHANNEL STATE) NEW ITEM EXIST\n"));
            m_treeRepository.animate_REMOVE( wParam );
			
			if(sptrayicon != NULL) // added by aquarelle
				sptrayicon->StopAnimation(wParam);
        }
        break;
    case CS_IDLE:       // ä�ο� ���ο� �������� ���� ���
        {
            TRACE(_T("(REPOS CHANNEL STATE) IDLE STATE\n"));
            m_treeRepository.animate_REMOVE( wParam );
			
			if(sptrayicon != NULL) // added by aquarelle
				sptrayicon->StopAnimation(wParam);
        }
        break;

    case CS_FEEDING:
        {
            TRACE(_T("(REPOS CHANNEL STATE) FEEDING\n"));
			m_treeRepository.animate_ADD( wParam );    // added by eternalbleu
			
			if(sptrayicon != NULL)		// added by aquarelle
				sptrayicon->StartAnimaition(CTray::DOWNLOADING, wParam);
        }
        break;

    case CS_FAILED:
        {
            TRACE(_T("(REPOS CHANNEL STATE) FAILED\n"));
            m_treeRepository.animate_REMOVE( wParam );
			if(sptrayicon != NULL)
				sptrayicon->StopAnimation(wParam);
        }
        break;

    default:
        break;
    }
}

/**************************************************************************
 * CDlg_Subscribe::OnChannelStateChange
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * �Ѱ� ä���� FEED�� ������ ó���� �����Ѵ�.
 *
 * @Parameters
 * (in WPARAM) wParam - ���� ���� ����Ʈ�� ����
 * (in LPARAM) lParam - Feed ��� ����
 **************************************************************************/
void CDlg_Subscribe::OnFeedComplete(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("Feed complete channel[%d]\r\n"), wParam);
	if((BOOL) lParam == TRUE)
		m_feedcontrol.Feed();
	// not defined
}

void CDlg_Subscribe::OnReposCmAddGroup() 
{
    TRACE( _T("(REPOS CONTEXT) READ ADD GROUP\n") );

    CFishRepItemPropertyDlg dlg;
    if (IDCANCEL    ==  dlg.DoModal(GROUP_ADD_DLG))      return;
//    m_treeRepository.SetFocus();

	// TODO: �Է� �׷� �������� �̸��� �Ҵ��ϴ� ������ ������. SetData �� ID���� ���õ� �߰�
    {
		GROUP_MAP::iterator it;
		BOOL bOverlaped = FALSE;
		//CString szOverlapMsg;
		for(it = m_mapGroup.begin(); it != m_mapGroup.end(); it++)
		{
			if((*it).second.title == dlg.getTitle())
			{
				bOverlaped = TRUE;
				//szOverlapMsg.Format(STR_GROUP_OVERLAP_MSG_FORMAT, dlg.getTitle());
				break;
			}
		}

		if(!bOverlaped || (bOverlaped && FishMessageBox(STR_GROUP_OVERLAP_MSG, STR_GROUP_OVERLAP_TITLE, FMB_YESNO | FMB_ICONWARNING) == IDYES))
		{
			int seq =   AddGroup( dlg.getTitle(), dlg.getComment() );
			HTREEITEM hITEM =   m_treeRepository.GetHandleFromData( seq );
			m_treeRepository.Select(hITEM, TVGN_CARET);
		}
    }

	Invalidate();		
}

/************************************************************************
OnContextMenu ���ؽ�Ʈ �޴��� �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_Subscribe::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	HTREEITEM hItem = m_treeRepository.GetSelectedItem();

    if (hItem)  {
        m_treeRepository.SetItemState( hItem, TVIS_SELECTED, TVIS_SELECTED );
        m_treeRepository.SelectItem( hItem );
    
        // Context Menu �����
	    CMenu Menu;
	    if(!Menu.LoadMenu(IDR_MENU_SUBSCRIBE)) return;

	    CMenu* pContext = (CMenu*)Menu.GetSubMenu(0);
	    if(!pContext) return;

        pContext->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
    }   else    {
        // Context Menu �����
	    CMenu Menu;
	    if(!Menu.LoadMenu(IDR_MENU_SUBSCRIBE)) return;

	    CMenu* pContext = (CMenu*)Menu.GetSubMenu(0);
	    if(!pContext) return;
        pContext->EnableMenuItem(ID_REPOS_CM_DELETE, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_REPOS_CM_RENAME, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_REPOS_CM_PROPERTY, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_VIEW_CHANNELPOST_ALL, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_VIEW_CHANNELPOST_UNREAD, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_VIEW_CHANNEL_MARK_AS_READ, MF_DISABLED | MF_GRAYED);
        pContext->EnableMenuItem(ID_VIEW_CHANNEL_MARK_AS_UNREAD, MF_DISABLED | MF_GRAYED);

        pContext->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}

void CDlg_Subscribe::OnReposCmAddChannel() 
{
	// modified by aquarelle : 2006/01/24
	
    TRACE( _T("(REPOS CONTEXT) READ ADD CHANNEL\n") );
    CFishRepItemPropertyDlg dlg;
	dlg.SetSubscribeDlg(this);

    if (IDCANCEL    ==  dlg.DoModal(CHANNEL_ADD_DLG))      return;
//    m_treeRepository.SetFocus();

    // TODO: �Է� �׷� �������� �̸��� �Ҵ��ϴ� ������ ������. SetData �� ID���� ���õ� �߰�
	DWORD iDATA = dlg.getTargetGroup();

	FISH_GROUP g(iDATA);
	
	if(!FindGroup(g))	// ���õ� �׷��� ���� ��� �߰���� �׷��� �ֻ��� �׷�����.. ���� by aquarelle
	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		iDATA = m_treeRepository.GetItemData(hITEM);
	}


    {
		//BOOL bns = FALSE;
		BOOL bOverlaped = FALSE;
		CHANNEL_MAP::iterator it;
		CString szOverlapMsg;
		for(it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
		{
			if((*it).second.xmlurl == dlg.getXmlUrl())
			{
				bOverlaped = TRUE;
				szOverlapMsg.Format(STR_CHANNEL_OVERLAP_MSG_FORMAT, dlg.getTitle());
				break;
			}
		}
		
		if(!bOverlaped || (bOverlaped && FishMessageBox(szOverlapMsg, STR_CHANNEL_OVERLAP_TITLE, FMB_YESNO | FMB_ICONWARNING) == IDYES))
		{
			int keepc = 0, keepd = 0;
			int seq = AddSubscribeChannel( iDATA, dlg.getTitle(), dlg.getComment(), dlg.getFeedInterval()
				, dlg.getXmlUrl(), dlg.getHtmlUrl(), _T("")
				, keepc, keepd, dlg.getIsSearch());
    		if( seq )
			{
				FetchByUser(seq);
				m_bFlushOPML = TRUE;

				HTREEITEM hITEM =   m_treeRepository.GetHandleFromData(seq);
				m_treeRepository.Select(hITEM, TVGN_CARET);

				RequestPost();

				CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
				if(pFrame) pFrame->ChangeToListView(seq);
			}
		}		
    }

	Invalidate();
}

void CDlg_Subscribe::OnReposCmAddKeyword() 
{
    TRACE( _T("(REPOS CONTEXT) ADD KEYWORD\n") );

    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();

    CFishRepItemPropertyDlg dlg;
	dlg.SetSubscribeDlg(this);
    if (IDCANCEL    ==  dlg.DoModal(KEYWORD_ADD_DLG))      return;
//    m_treeRepository.SetFocus();

    //if ( hITEM == NULL )//  return;
	//{
	//	hITEM = m_treeRepository.GetRootItem();
	//}

	//DWORD iDATA = m_treeRepository.GetItemData( hITEM );

	DWORD iDATA = dlg.getTargetGroup();
	
	FISH_GROUP g(iDATA);
	if(!FindGroup(g)) // ���õ� �������� �׷��� �ƴҰ��..�߰���� �׷��� �θ�׷�����.. ����
	{
		hITEM = m_treeRepository.GetParentItem(hITEM);
		iDATA = m_treeRepository.GetItemData(hITEM);
	}

    {
		CHANNEL_MAP::iterator it;
		BOOL bOverlaped = FALSE;

		for(it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
		{
			if((*it).second.type == CT_KEYWORD && (*it).second.fc.text == dlg.getKeyword())
			{
				bOverlaped = TRUE;
				break;
			}
		}
		
		if(!bOverlaped || (bOverlaped && FishMessageBox(STR_KEYWORD_OVERLAP_MSG, STR_KEYWORD_OVERLAP_TITLE, FMB_YESNO | FMB_ICONWARNING) == IDYES))
		{
			int seq =   RequestKeywordChannelAdd(dlg.getTitle(), dlg.getComment(), dlg.getKeyword(), iDATA);
			m_bFlushOPML = TRUE;
			HTREEITEM hITEM =   m_treeRepository.GetHandleFromData(seq);
			if (hITEM) 
			{
				// added by moonknit 2006-02-27
				// for expand
				HTREEITEM hParent = m_treeRepository.GetParentItem(hITEM);
				if(NULL != hParent )	{
					m_treeRepository.Expand( hParent, TVE_EXPAND);
				}
				m_treeRepository.Select(hITEM, TVGN_CARET);

				CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
				if(pFrame) pFrame->ChangeToListView(seq);
			}
		}
    }
	Invalidate();
}

void CDlg_Subscribe::OnReposCmAddScrap() 
{
    TRACE( _T("(REPOS CONTEXT) ADD SCRAP\n") );

    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();

    CFishRepItemPropertyDlg dlg;
	dlg.SetSubscribeDlg(this);
    if (IDCANCEL    ==  dlg.DoModal(SCRAP_ADD_DLG))      return;
//    m_treeRepository.SetFocus();

    //if ( hITEM == NULL )//  return;
	//{
	//	hITEM = m_treeRepository.GetRootItem();
	//}

	//DWORD iDATA = m_treeRepository.GetItemData( hITEM );

	DWORD iDATA = dlg.getTargetGroup();
	
	FISH_GROUP g(iDATA);
	if( !FindGroup(g) ) // ���õ� �������� �׷��� �ƴҰ��..�߰���� �׷��� �θ�׷�����.. ����
	{
		hITEM = m_treeRepository.GetParentItem(hITEM);
		iDATA = m_treeRepository.GetItemData(hITEM);
	}
	
	
    {
        int seq = AddScrapChannel(iDATA, dlg.getTitle(), dlg.getComment());
		m_bFlushOPML = TRUE;
        HTREEITEM hITEM =   m_treeRepository.GetHandleFromData(seq);

		// added by moonknit 2006-02-27
		if(hITEM)
		{
			// for expand
			HTREEITEM hParent = m_treeRepository.GetParentItem(hITEM);
			if(NULL != hParent )	{
				m_treeRepository.Expand( hParent, TVE_EXPAND);
			}

	        m_treeRepository.Select(hITEM, TVGN_CARET);
		}
    }
   
    Invalidate();
}

void CDlg_Subscribe::OnReposCmDelete() 
{
    TRACE( _T("(REPOS CONTEXT) DELETE ITEM\n") );

    if (IDYES == FishMessageBox(STR_CAUTION_DELETE , STR_DELETE_CONFIRM, MB_YESNO | MB_ICONWARNING ) )
    {
        vector<HTREEITEM> LIST;
        LIST.push_back(m_treeRepository.GetSelectedItem());

		HTREEITEM afterSelect = *LIST.begin();
		if(m_treeRepository.GetParentItem(afterSelect) != NULL &&
			m_treeRepository.GetNextSiblingItem(afterSelect) == NULL &&
			m_treeRepository.GetPrevSiblingItem(afterSelect) != NULL)
		{
			afterSelect = m_treeRepository.GetPrevSiblingItem(afterSelect);
		}
		

        vector<HTREEITEM>::iterator it = LIST.begin();
        for (it; it != LIST.end(); it++)
        {
            Remove( m_treeRepository.GetItemData(*it) );
        }

		if(afterSelect)
			m_treeRepository.Select(afterSelect, TVGN_CARET);

		m_bFlushOPML = TRUE;
    }
}

void CDlg_Subscribe::OnReposCmOpmlImport() 
{
    TRACE( _T("(REPOS CONTEXT) IMPORT OPML FILE\n") );
	

	CDlg_OpmlImport dlg;

	if(dlg.DoModal() == IDOK)
	{
		if(PathFileExists(dlg.m_szFileLocation))
		{
			if(!OPMLLoad(dlg.m_szFileLocation, EXTERNAL_OPML))
                FishMessageBox(STR_ERR_OPML_IMPORT);
			else
				m_bFlushOPML = TRUE;
		}
		else
		{
			CEzXMLParser parser;
			if(parser.LoadXML((LPTSTR)(LPCTSTR) dlg.m_szFileLocation, CEzXMLParser::LOAD_WEB))
			{
				IXMLDOMDocumentPtr pDoc = parser.GetRootNode();
				if(pDoc != NULL && OPMLLoad((LPCTSTR) pDoc->xml, EXTERNAL_OPML, CEzXMLParser::LOAD_STRING))
				{
					m_bFlushOPML = TRUE;
				}
				else
					FishMessageBox(STR_ERR_OPML_IMPORT);
			}
		}
	}
}

void CDlg_Subscribe::OnReposCmOpmlExport() 
{
    TRACE( _T("(REPOS CONTEXT) EXPORT OPML FILE\n") );

    CFileDialog dlg(FALSE, STR_XML_FORMAT, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, STR_OPML_FILE_FORMAT, this);	

    if(dlg.DoModal() == IDOK)
    {
        CString fpath = dlg.GetPathName();
		if(OPMLExport(fpath))
		{
			TRACE(_T("Export Opml File : %s\n"), fpath);
		}
		else
//			AfxMessageBox(STR_ERR_OPML_EXPORT);
            FishMessageBox(STR_ERR_OPML_EXPORT);
    }
}

/************************************************************************
OnReposCmProperty   ���� ���� ����
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:CREATED
	2006/01/25: ä�� ������ ���� ������ ����ǵ��� ����... by aquarelle
************************************************************************/
void CDlg_Subscribe::OnReposCmProperty() 
{
    TRACE( _T("(REPOS CONTEXT) CHANGE PROPERTY\n") );

    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();
    auto_ptr<FISH_REPITEM>  apITEM;

	DWORD dwData = m_treeRepository.GetItemData(hITEM);

    FindItem( dwData, apITEM );

    if ( apITEM->rep_type == REP_GROUP )    {
        CFishRepItemPropertyDlg dlg;
        dlg.m_strTitle      =   apITEM->title;
        dlg.m_strComment    =   apITEM->comment;

		int feedInterval = 0;
		CHANNEL_MAP::iterator it, it2;

		for(it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
		{
			if((apITEM.get())->id == (*it).second.groupid)
			{
				feedInterval = (*it).second.feedinterval;
				it2 = it;
				it2++;
				for(; it2 != m_mapChannel.end(); it2 ++)
				{
					if((apITEM.get())->id == (*it2).second.groupid)
						break;
				}

				if(it2 != m_mapChannel.end()  && (*it2).second.feedinterval != feedInterval)
				{
					int test = (*it2).second.feedinterval;
					feedInterval = -1;
					break;
				}
			}
		}
		dlg.m_nFeedInterval = feedInterval;

        if (IDCANCEL    ==  dlg.DoModal(GROUP_DLG))      return;
//        m_treeRepository.SetFocus();

        ((FISH_GROUP*)apITEM.get())->title       =   dlg.getTitle();
        ((FISH_GROUP*)apITEM.get())->comment     =   dlg.getComment();

        ModifyGroup( dwData,*((FISH_GROUP*)apITEM.get()) );

		if( dlg.getFeedInterval() != -1 || dlg.getChangeSearch())
		{
			for(it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
			{
				if((apITEM.get())->id == (*it).second.groupid)
				{

					if(dlg.getFeedInterval() != -1)
						(*it).second.feedinterval = dlg.getFeedInterval();
					if(dlg.getChangeSearch())
						(*it).second.bns = dlg.getIsSearch();
				}
			}
		}

        Invalidate();
		m_bFlushOPML = TRUE;
    }   else    {
        CFishRepItemPropertyDlg dlg;
        dlg.m_strTitle      =   apITEM->title;
        dlg.m_strComment    =   apITEM->comment;

		if( ((FISH_CHANNEL*)apITEM.get())->type == CT_SUBSCRIBE)
		{
			dlg.m_nFeedInterval =   ((FISH_CHANNEL*)apITEM.get())->feedinterval;
			dlg.m_strHtmlUrl    =   ((FISH_CHANNEL*)apITEM.get())->churl;
			dlg.m_strXmlUrl     =   ((FISH_CHANNEL*)apITEM.get())->xmlurl;
			dlg.m_bIsSearch		=	((FISH_CHANNEL*)apITEM.get())->bns;

			if (IDCANCEL    ==  dlg.DoModal(CHANNEL_DLG))      return;
//            m_treeRepository.SetFocus();
//			if ( hITEM == NULL )  return;

            ((FISH_CHANNEL*)apITEM.get())->title    =   dlg.getTitle();
			((FISH_CHANNEL*)apITEM.get())->comment  =   dlg.getComment();
			((FISH_CHANNEL*)apITEM.get())->feedinterval =   dlg.getFeedInterval();
			((FISH_CHANNEL*)apITEM.get())->churl =   dlg.getHtmlUrl();
			((FISH_CHANNEL*)apITEM.get())->xmlurl   =   dlg.getXmlUrl();
			((FISH_CHANNEL*)apITEM.get())->bns		=	dlg.getIsSearch();
		}
		else if(((FISH_CHANNEL*)apITEM.get())->type == CT_SCRAP)
		{
			if (IDCANCEL    ==  dlg.DoModal(SCRAP_DLG))      return;
//            m_treeRepository.SetFocus();
//			if ( m_treeRepository.GetSelectedItem() == NULL )  return;

            ((FISH_CHANNEL*)apITEM.get())->title    =   dlg.getTitle();
			((FISH_CHANNEL*)apITEM.get())->comment  =   dlg.getComment();
		}
		else if(((FISH_CHANNEL*)apITEM.get())->type == CT_SEARCH 
			|| ((FISH_CHANNEL*)apITEM.get())->type == CT_KEYWORD
			|| ((FISH_CHANNEL*)apITEM.get())->type == CT_LSEARCH)
		{
			dlg.m_strKeyword =  ((FISH_CHANNEL*)apITEM.get())->fc.text;
			
			if(((FISH_CHANNEL*)apITEM.get())->type == CT_SEARCH
				|| ((FISH_CHANNEL*)apITEM.get())->type == CT_LSEARCH
				)
				if (IDCANCEL    ==  dlg.DoModal(SEARCH_DLG))      return;
//                m_treeRepository.SetFocus();
			
			if(((FISH_CHANNEL*)apITEM.get())->type == CT_KEYWORD)
				if (IDCANCEL    ==  dlg.DoModal(KEYWORD_DLG))      return;
//                m_treeRepository.SetFocus();

//			if ( m_treeRepository.GetSelectedItem() == NULL )  return;

			((FISH_CHANNEL*)apITEM.get())->title    =   dlg.getTitle();
			((FISH_CHANNEL*)apITEM.get())->comment  =   dlg.getComment();
		}

		ModifyChannel( dwData, *((FISH_CHANNEL*)apITEM.get()) );
		Invalidate();
		m_bFlushOPML = TRUE;
    }	
}

/************************************************************************
OnReposCmRefresh    ���ΰ�ħ ���ؽ�Ʈ �޴� ����
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/21:CREATED
************************************************************************/
void CDlg_Subscribe::OnReposCmRefresh() 
{
    TRACE( _T("(REPOS CONTEXT) REFRESH ITEM\n") );
   
    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();

    if(hITEM)   {
        DWORD dwData = m_treeRepository.GetItemData(hITEM);;
        FetchByUser(dwData);
    }   else    {
        GROUP_MAP::iterator it = m_mapGroup.begin();
        for (; it!= m_mapGroup.end(); it++)
        {
            FetchByUser(it->first);
        }
    }

    CoInitialize(NULL);
    RequestPost();
    CoUninitialize();
}

/************************************************************************
OnReposCmRename �̸� ���� �޽��� ó��
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:CREATED
************************************************************************/
void CDlg_Subscribe::OnReposCmRename() 
{
    TRACE( _T("(REPOS CONTEXT) RENAME ITEM\n") );
    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();
	
    m_treeRepository.EditLabel(hITEM, FALSE);
}

/************************************************************************
OnViewChannelpostAll
@PARAM  : ��� ��� ä���� ����Ʈ ���
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/17:CREATED
************************************************************************/
void CDlg_Subscribe::OnViewChannelpostAll() 
{
	RequestPost(FALSE);	
}

/************************************************************************
OnViewChannelpostUnread
@PARAM  : ���� ���� ����Ʈ�� ���
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/17:CREATED
************************************************************************/
void CDlg_Subscribe::OnViewChannelpostUnread() 
{
	// TODO: Add your command handler code here
    RequestPost(TRUE);
}

/************************************************************************
OnViewChannelMarkAsRead
@PARAM  : ���� ���·� ä�� ���� ����Ʈ ��ŷ
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/17:CREATED
************************************************************************/
void CDlg_Subscribe::OnViewChannelMarkAsRead() 
{
	MarkAs(TRUE);
    RequestPost(FALSE);
	
    
}

/************************************************************************
OnViewChannelMarkAsUnread
@PARAM  : ���� ���� ���·� ä�� ���� ����Ʈ ��ŷ
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/17:CREATED
************************************************************************/
void CDlg_Subscribe::OnViewChannelMarkAsUnread() 
{
	MarkAs(FALSE);
    RequestPost(FALSE);
}

/**************************************************************************
 * method CDlg_Subscribe::MarkAs
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-18
 *
 * @Description
 * ���õ� �׷�/ä���� ����Ʈ�� Read/Unread �� ó���Ѵ�.
 *
 * @Parameters
 * (in BOOL) bread - TRUE�̸� Read ó��, FALSE�̸� Unreadó��
 **************************************************************************/
void CDlg_Subscribe::MarkAs(BOOL bread)
{
    HTREEITEM hITEM =   m_treeRepository.GetSelectedItem();

    DWORD dwData    =   m_treeRepository.GetItemData(hITEM);

    if( m_treeRepository.GetParentItem(hITEM)    ==  NULL ) {
        GROUP_MAP::iterator it = m_mapGroup.begin();
        for (; it != m_mapGroup.end(); it++)
        {
            if (it->second.id   ==   dwData)    break;
        }

	    if(it != m_mapGroup.end())
	    {
            CHANNEL_MAP::iterator it2    =   NULL;
		    for( it2 = m_mapChannel.begin(); it2 != m_mapChannel.end(); ++it2)
		    {	
			    if((*it2).second.groupid == dwData)
			    {
				    theApp.m_spRD->IChannelRead((*it2).second.id, bread);
			    }
		    }

		    return;
	    }
    }   else    {
        CHANNEL_MAP::iterator it = m_mapChannel.begin();
        for (; it != m_mapChannel.end(); it++)
        {
            if (it->second.id   ==   dwData)    break;
        }

	    if(it != m_mapChannel.end())
	    {
		    // ������ �ϴ� ä���� ��ũ��, �˻��� ��쿡�� bunreadonly �� false�� ó���Ѵ�.
		    theApp.m_spRD->IChannelRead(dwData, bread);
		    return;
	    }
    }
}

void CDlg_Subscribe::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawItems(&dc);	
	// Do not call CDialog::OnPaint() for painting messages
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2006-02-20
 *
 * @Description
 * Web Page ���� OPML ������ �о�´�. 
 *
 * @Parameters
 * (out CString&) XML - �о�� OPML�� String ��
 * (out CString&) sDate - �о�� OPML�� ���� ���� �ð� (���� ����)
 **************************************************************************/
BOOL CDlg_Subscribe::GetOPMLFromWeb(CString& XML, CString& sDate)
{
	CMemFile SNList;
	TCHAR line[MAX_BUFF+1]={0};
	CString str = _T(""), rstr;
	BOOL breturn = FALSE;

	str.Format(_T("%s?cc=0005&ii=%s&pp=%s&tt=%s")
		, URL_WEBDB
		, theApp.m_spLP->GetTempUserID()
		, theApp.m_spLP->GetEncodedPassword()
		, theApp.m_spLP->GetLastOPMLTime()
		);

	TRACE(_T("OPML Get URL : %s\r\n"), str);

	if( !GetHttpFileAsc( HOST_NAME, 
		80, (LPTSTR)((LPCTSTR )str), &SNList, NULL, NULL) )
	{
		// ERROR : URL, PORT, filename�� �ϳ��� ������
		SNList.Close();
		return FALSE;
	}
	
	// ��û ���
	if( GetLine( &SNList, line) < 0)
	{
		// ERROR��
		SNList.Close();
		return FALSE;
	}

	int ret = 0;
	// ��� ���� �˻�
	BOOL ischeck = CheckResultCode( line , ret );
	if(ret == 1)
	{
		sDate.Empty();
		if(GetLine(&SNList, line))
		{
			sDate = line;
			XML.Empty();
			if(GetRemains(&SNList, XML))
			{
				breturn = TRUE;
			}
		}
	}
	else
	{
	}

	SNList.Close();
	return breturn;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2006-02-20
 *
 * @Description
 * OPML ������ �Ϸ� �� �� ����� �˷��ش�.
 *
 * @Parameters
 * (in WPARAM) wParam - (int) OPML ������Ʈ ��� ��

		code means as below
		1	OPML����
		2	���� ����
		3	�����������
		4	DB �������
		5	DB Update ����
		6	�������� �ʴ� ȸ��
		7	Ʋ�� ������ ��Ŷ
		8	�������
		9	��������

 * (in LPARAM) lParam - (LPCTSTR) ���� �ð� // format : yyyymmddHHMMSS
 **************************************************************************/
void CDlg_Subscribe::OnUpdateOPMLComplete(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 2)
	{
		theApp.m_spLP->SetLastOPMLTime((LPCSTR) lParam);
	}
}



void CDlg_Subscribe::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    
	if(nIDEvent == TID_SUBSCRIBE_OPMLSAVE && m_bFlushOPML)
	{		
		if(OPMLSave())
		{
			TRACE(_T("\nSaveOPML : %s"), (COleDateTime::GetCurrentTime()).Format(FISH_DT_TEMPLETE));
			if(theApp.m_spLP->CanOPMLUse())
			{
			//	theApp.m_spOS->StartTransfer(theApp.m_spLP->GetUserPath() + _T('\\') + S_DEFAULT_LOCALOPML);
			}
			m_bFlushOPML = FALSE;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlg_Subscribe::SelectChannel(int chid)
{
	HTREEITEM hITEM = Tree_ItemHandleFrom(chid);
	if(hITEM) m_treeRepository.SelectItem( hITEM );
}

void CDlg_Subscribe::OnSubscribeURL(WPARAM wParam, LPARAM lParam)
{
    CFishRepItemPropertyDlg dlg;
	dlg.SetSubscribeDlg(this);
    dlg.m_strXmlUrl =   (TCHAR*)wParam;
	CString szOverlapMsg;

    if (IDCANCEL    ==  dlg.DoModal(CHANNEL_ADD_DLG))      return;
//    m_treeRepository.SetFocus();
    {
		CHANNEL_MAP::iterator it;
		BOOL bOverlaped = FALSE;
		for(it = m_mapChannel.begin(); it != m_mapChannel.end(); it++)
		{
			if((*it).second.xmlurl == dlg.getXmlUrl())
			{
				bOverlaped = TRUE;
				szOverlapMsg.Format(STR_CHANNEL_OVERLAP_MSG_FORMAT, dlg.getTitle());
				break;
			}
		}

		
		if(bOverlaped && FishMessageBox(szOverlapMsg, STR_CHANNEL_OVERLAP_TITLE, FMB_YESNO | FMB_ICONWARNING) == IDNO)
		{
			/*
			int seq =   AddGroup( dlg.getTitle(), dlg.getComment() );
			HTREEITEM hITEM =   m_treeRepository.GetHandleFromData( seq );
			m_treeRepository.Select(hITEM, TVGN_CARET);
			*/
			return;
		}
    }

    // TODO: �Է� �׷� �������� �̸��� �Ҵ��ϴ� ������ ������. SetData �� ID���� ���õ� �߰�
	DWORD iDATA = dlg.getTargetGroup();

	FISH_GROUP g(iDATA);
	
	if(!FindGroup(g))	// ���õ� �׷��� ���� ��� �߰���� �׷��� �ֻ��� �׷�����.. ���� by aquarelle
	{
		HTREEITEM hITEM = m_treeRepository.GetRootItem();
		iDATA = m_treeRepository.GetItemData(hITEM);
	}


    {
		BOOL bns = FALSE;
		int keepc = 0, keepd = 0;
        int seq = AddSubscribeChannel( iDATA, dlg.getTitle(), dlg.getComment()
			, dlg.getFeedInterval(), dlg.getXmlUrl()
			, dlg.getHtmlUrl(), _T(""), keepc, keepd, bns);
    	if( seq )
		{
			FetchByUser(seq);
			m_bFlushOPML = TRUE;

			HTREEITEM hITEM =   m_treeRepository.GetHandleFromData(seq);
			m_treeRepository.Select(hITEM, TVGN_CARET);

			RequestPost();

			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(pFrame) pFrame->ChangeToListView(seq);
		}
    }

	Invalidate();

    return;
}

void CDlg_Subscribe::OnSetFocus(CWnd* pOldWnd) 
{
	CDialog::OnSetFocus(pOldWnd);

}

void CDlg_Subscribe::OnSetSearchSave(WPARAM wParam, LPARAM lParam)
{
	CHANNEL_MAP::iterator it;

	for(it = m_mapChannel.begin(); it != m_mapChannel.end(); ++it)
	{
		if((*it).second.type == CT_SEARCH || (*it).second.type == CT_LSEARCH)
		{
			if(FALSE == (*it).second.searchlog)
			{
				theApp.m_spRD->IChannelSearchLog((*it).first, TRUE);
				--m_nNotSaveSearchCnt;
			}
		}
	}
}
