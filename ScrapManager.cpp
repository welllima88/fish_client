// ScrapManager.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "MainFrm.h"
#include "ScrapManager.h"
#include "Dlg_Subscribe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_MENUTEXT_CREATE_NEW_SCRAP_ITEM _T("새 스크랩 채널 만들기 (&C)")
/////////////////////////////////////////////////////////////////////////////
// CScrapManager
CScrapManager::CScrapManager(CWnd* pWnd)
{
    CMainFrame* pMain = ((CMainFrame*)pWnd);
	pMain->GetDlgSubscribe( m_spSubscribe );
    m_ctrlPopMenu.LoadMenu(IDR_SCRAP_MGR);
}

CScrapManager::~CScrapManager()
{

}


BEGIN_MESSAGE_MAP(CScrapManager, CWnd)
	//{{AFX_MSG_MAP(CScrapManager)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScrapManager message handlers

void CScrapManager::Init(CWnd* pParentWnd)
{
    m_pParent   =   pParentWnd;
    Create(_T(""), _T(""), WS_CHILD, CRect(0,0,0,0), pParentWnd, 0);
}

BOOL CScrapManager::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

/************************************************************************
ShowScrapMgr
@PARAM  : 
@RETURN : 
@REMARK : 
    메뉴의 uID부분은 ID_FISH_SCRAPMENU_BASE_VALUE(32800) 를 베이스 값으로해서
    생성된다. 따라서 차후 이 보다 큰값이 메뉴에 의해서 선점된 경우에는 정상적인
    동작이 안되는 경우가 발생할 가능성이 존재한다. 최대 300개의 공간을 가지고 있으며 
    가장 큰 값은 33100 을 갖는다.

@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/03:CREATED
************************************************************************/
BOOL CScrapManager::ShowScrapMgr(CPoint ptContext)
{
    //////////////////////////////////////////////////////////////////////////
    // SCRAP CHANNEL LOADING
    list<FISH_CHANNEL> m_listScrap;
    m_spSubscribe->GetScrapList( m_listScrap );

    //////////////////////////////////////////////////////////////////////////
    // ITEM INSERTION
    CMenu menu;
    if ( !menu.CreatePopupMenu() )  return FALSE;

    UINT uID = 0;
    list<FISH_CHANNEL>::iterator it = m_listScrap.begin();
	for(; it != m_listScrap.end(); it++)
    {
        uID =   ID_SCRAP_BASE_UID + it->id;
        menu.AppendMenu( MF_STRING , uID, it->title );
    }

    //////////////////////////////////////////////////////////////////////////
    // SEPARATOR ADDITION
    if ( m_listScrap.size() != 0 )
    {
        menu.AppendMenu( MF_SEPARATOR, 0, _T("") );
    }

    menu.AppendMenu( MF_STRING, ID_CREATE_NEW_SCRAP_MGR, STR_MENUTEXT_CREATE_NEW_SCRAP_ITEM);
    menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptContext.x , ptContext.y, m_pParent);
    return TRUE;
}

BOOL CScrapManager::HideScrapMgr()
{
    return TRUE;    
}