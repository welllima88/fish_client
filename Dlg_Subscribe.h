#if !defined(AFX_DLG_SUBSCRIBE_H__17629EA7_F3C7_4319_92CE_4ADF8DFFD9C1__INCLUDED_)
#define AFX_DLG_SUBSCRIBE_H__17629EA7_F3C7_4319_92CE_4ADF8DFFD9C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Subscribe.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Subscribe dialog

#include "fish_struct.h"

#include <cassert>
#include "FeedControl.h"
#include "view_repository/RepositoryTreeCtrl.h"
#include "view_repository/repositoryitempropertydlg.h"
#include "ctrl/netbutton.h"				// image button

using namespace std;

class CDlg_Subscribe : public CDialog
{
public:
     enum {
        REPOSITORY_TREE_LEFT_PAD        =   8,
        REPOSITORY_TREE_TOP_PAD         =   56,
        REPOSITORY_TREE_RIGHT_PAD       =   3,
        REPOSITORY_TREE_BOTTOM_PAD      =   10,

        REPOSITORY_BTN_QUANTITY         =   5,
        REPOSITORY_SIZE_BTN_WIDTH       =   32,
        REPOSITORY_SIZE_BTN_HEIGHT      =   41,
        REPOSITORY_BTN_TOP_PADDING      =   5,
        REPOSITORY_BTN_LEFT_PADDING     =   8,
        REPOSITORY_BTN_RIGHT_PADDING    =   13,

        REPOSITORY_BTN_INTERVAL         =   3,
    };
   
// Construction
public:
	CDlg_Subscribe(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_Subscribe();
	BOOL Create(CWnd* pParentWnd = NULL);

	// For OPMLLoad Method
	enum
	{	
		LOCAL_OPML = 0,
		EXTERNAL_OPML
	};

// Dialog Data
	//{{AFX_DATA(CDlg_Subscribe)
	enum { IDD = IDD_DLG_SUBSCRIBE };
	CNetBitmapButton	m_btnAddScrap;
	CNetBitmapButton	m_btnRefresh;
	CNetBitmapButton	m_btnAddKeyword;
	CNetBitmapButton	m_btnAddGroup;
	CNetBitmapButton	m_btnAddChannel;

    CFishTreeCtrl	m_treeRepository;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Subscribe)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// draw functions
	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);

	static DWORD CheckURLThread(LPVOID lpParam);
	void MakeDefaultChannel(CString url, CString title);

	static DWORD WINAPI OPMLThread(LPVOID lpParam);
	static BOOL GetOPMLFromWeb(CString& XML, CString& sDate);

public:
    void OPMLRequest(CString XML = _T(""), CString sDate = _T(""));


public:
    // ITEM ADDITION
	int AddGroup(CString title, CString comment = _T(""), int groupid = 0, enum GROUP_TYPE type = GT_SUBSCRIBE);
	int AddSubscribeChannel(int gid, CString title, CString comment
		, int feedinterval, CString url, CString htmlurl
		, CString guid = _T(""), int keepcnt = 0, int keepdays = 0, BOOL bnotsearch = FALSE, int channelid = 0);
	int AddFilterChannel(int gid, CString title, CString comment
		, CString keyword, CHANNEL_TYPE filtertype
		, CString sguid = _T(""), BOOL bnotsearch = FALSE, int channelid = 0, BOOL bForceSave = FALSE);
	int AddScrapChannel(int gid, CString title
		, CString comment
		, CString sguid = _T(""), BOOL bnotsearch = FALSE, int channelid = 0);

    //////////////////////////////////////////////////////////////////////////
    // UTILITY METHOD
	BOOL FindItem(int id, auto_ptr<FISH_REPITEM>& item);
	BOOL FindChannel(FISH_CHANNEL& c);
	BOOL FindGroup(FISH_GROUP& g);

    // ITEM ATTRIB UPDATE
    BOOL MoveChannel(int id, int target_gid);
	BOOL ModifyChannel(int id, FISH_CHANNEL channel);
	BOOL RemoveChannel(int id);
							
	BOOL ModifyGroup(int id, FISH_GROUP group);
	BOOL RemoveGroup(int id);
	BOOL Remove(int id);

private:        // TREE MANAGEMENT METHOD
    HTREEITEM       Tree_ItemHandleFrom(int id);
    unsigned int    Tree_IconDefaultIndexOf(FISH_REPITEM* pItem);
    unsigned int    Tree_IconSelectedIndexOf(FISH_REPITEM* pItem);

public:   
	void    Tree_ItemUpdate(FISH_REPITEM* pitem);
    void    Tree_ItemRemove(int id);
	int     Tree_ItemAdd(FISH_REPITEM* pitem, HTREEITEM* param  =   NULL);
    int     Tree_ItemAdd(int id, HTREEITEM* param  =   NULL);
    BOOL    Tree_ItemMove(int src, int dest);

protected:
	void MoveControls();


	// attributes
	HANDLE			m_hOPMLThread;

	CHANNEL_MAP		m_mapChannel;
	GROUP_MAP		m_mapGroup;

	BOOL			m_bFeedInform;

	BOOL			m_bInit;
	BOOL			m_bSelfDraw;
	CPoint			m_ptContext;

	int				m_nsubseq;          // 채널과 그룹이 공유하지 않는 Unique ID

	int				m_nManageFeedChannel;		// feed 완료한 채널의 수
	int				m_nManageFeedPost;			// feed 된 새 post의 수

	int				m_nCurSelected;

	int				m_nDefaultGroup;

	CFeedControl	m_feedcontrol;

	CRgn			m_rgnPaint;
	CBrush			m_brushBG;

	BOOL			m_bFlushOPML;		// OPML을 갱신할지 여부
	UINT			m_uIDTimer;

	int				m_nNotSaveSearchCnt;

    CToolTipCtrl    m_Tooltip;
	TOOLINFO		m_ToolInfo;

public:
    void _initialize();
    void _finalize();

	BOOL IsRemainSearchSave() { return (m_nNotSaveSearchCnt > 0) ? TRUE : FALSE; }

	void SetFlushOPML() { m_bFlushOPML = TRUE; }
    
	void SelectChannel(int chid);
	void MarkAs(BOOL bread);
	void RequestPost(BOOL bunreadonly = FALSE, BOOL bautoselect = FALSE);
	int GetScrapList(list<FISH_CHANNEL>& listchannel);
	int RequestKeywordChannelAdd(CString title, CString comment, CString keyword, int gid);
	int RequestSearchChannelAdd(CString searchword, int type, BOOL bsearch = TRUE);
	void RequestSubscribeChannelAdd(CString url);
	void UpdateGroupUPostCnt(int id, int cnt);
	void UpdateGroupPostCnt(int id, int cnt);

	void FetchComplete(int channelid, COleDateTime dt, int postcnt = 0, int keepcnt = 0);
	void FetchByUser(int channelid);

	void Manage();

	BOOL OPMLSave();
    BOOL OPMLLoad(const CString filename,  int loadtype = LOCAL_OPML, int loadfrom = 0);
	BOOL OPMLBackup();
	BOOL OPMLExport(const CString filename);

private:
	BOOL MakeDefaultOPML(CEzXMLParser& parser, int type=0);
	BOOL GetFlushOPMLFlag() { return m_bFlushOPML; }

	void Fetch(int channelid, CString url, COleDateTime dtlastbuild);

public:
	// Generated message map functions
	//{{AFX_MSG(CDlg_Subscribe)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewChannelpostAll();
	afx_msg void OnBtnAddchannel();
	afx_msg void OnBtnAddgroup();
	afx_msg void OnBtnAddscrap();
	afx_msg void OnBtnAddkeyword();
	afx_msg void OnBtnRefresh();
	afx_msg void OnReposCmAddGroup();
	afx_msg void OnReposCmAddChannel();
	afx_msg void OnReposCmAddKeyword();
	afx_msg void OnReposCmAddScrap();
	afx_msg void OnReposCmDelete();
	afx_msg void OnReposCmOpmlImport();
	afx_msg void OnReposCmOpmlExport();
	afx_msg void OnReposCmProperty();
	afx_msg void OnReposCmRefresh();
	afx_msg void OnReposCmRename();
	afx_msg void OnViewChannelpostUnread();
	afx_msg void OnViewChannelMarkAsRead();
	afx_msg void OnViewChannelMarkAsUnread();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	afx_msg void OnSubscribeURL(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChannelUCntUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChannelPCntUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitSubscribe(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAfterCheckURL(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChannelStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFeedComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateOPMLComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetSearchSave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_SUBSCRIBE_H__17629EA7_F3C7_4319_92CE_4ADF8DFFD9C1__INCLUDED_)
