// FocusManager.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"

#include "Dlg_Search.h"
#include "Dlg_ExplorerBar.h"
#include "Dlg_RSSList.h"
#include "Dlg_Subscribe.h"
#include "FocusManager.h"
#include "fish_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_ERR_FM_WRONGMEMBER				_T("멤버 인자중 초기화 되지 않은 인자가 존재합니다.")
#define	STR_ERR_FM_ERROR					_T("포커스 매니져 오류")

/////////////////////////////////////////////////////////////////////////////
// CFocusManager
CFocusManager*   CFocusManager::_instance  =   NULL;

CFocusManager::CFocusManager()
{
    m_pSearch       =   NULL;
    m_pExplorerBar  =   NULL;
    m_pSubScribe    =   NULL;
    m_pRssList      =   NULL;
}

CFocusManager::~CFocusManager()
{
}




BEGIN_MESSAGE_MAP(CFocusManager, CWnd)
	//{{AFX_MSG_MAP(CFocusManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFocusManager message handlers

CFocusManager* CFocusManager::getInstance(CWnd* pParent)
{
    if (_instance   ==  NULL)   {
        _instance   =   new CFocusManager();

        if (pParent !=  NULL)   _instance->Create(_T(""), _T(""), WS_CHILDWINDOW, CRect(0,0,0,0), pParent, 1234567);
    }

    return _instance;
}

CWnd* CFocusManager::SetNextFocus()
{
    if ( m_pSearch   !=  NULL && m_pExplorerBar  !=  NULL && m_pSubScribe    !=  NULL && m_pRssList  !=  NULL )
    {
        CWnd* pCurrentFocusWnd  =   GetFocus();  //CWnd::FromHandle( ::GetActiveWindow() );  //this->m_hWnd; 
        CWnd* pNextFocusWnd     =   NULL;


        if ( &(m_pSubScribe->m_treeRepository)  ==  GetFocus() )    {
            // Focus on TREECTRL
            pNextFocusWnd   =   &(m_pRssList->m_listRSS);
        }   else    if ( &(m_pRssList->m_listRSS)   ==  GetFocus() )    {
            // Focus on ADDGRP BTN or Focus on SRCH BTN
            if ( m_pSearch->m_edtSearch.GetStyle() & WS_DISABLED )
                pNextFocusWnd   =   &(m_pSubScribe->m_btnAddGroup);
            else
                pNextFocusWnd   =   &(m_pSearch->m_edtSearch);            
        }   else    if ( &(m_pSearch->m_edtSearch)   ==  GetFocus() )    {
            // Focus on ADDGRP BTN
            pNextFocusWnd   =   &(m_pSubScribe->m_btnAddGroup);
        }   else    if ( &(m_pSubScribe->m_btnAddGroup)  ==  GetFocus() )    {
            // Focus on ADD CHANNEL
            pNextFocusWnd   =   &(m_pSubScribe->m_btnAddChannel);
        }   else    if ( &(m_pSubScribe->m_btnAddChannel)  ==  GetFocus() )    {
            // Focus on ADD SCRAP
            pNextFocusWnd   =   &(m_pSubScribe->m_btnAddScrap);
        }   else    if ( &(m_pSubScribe->m_btnAddScrap)  ==  GetFocus() )    {
            // Focus on ADD KEYWORD
            pNextFocusWnd   =   &(m_pSubScribe->m_btnAddKeyword);
        }   else    if ( &(m_pSubScribe->m_btnAddKeyword)  ==  GetFocus() )    {
            // Focus on REFRESH
            pNextFocusWnd   =   &(m_pSubScribe->m_btnRefresh);
        }   else    if ( &(m_pSubScribe->m_btnRefresh)  ==  GetFocus() )    {
            // Focus on TREECTRL
            pNextFocusWnd   =   &(m_pSubScribe->m_treeRepository);
        }else    {
            //////////////////////////////////////////////////////////////////////////
            // 기본 포커스는 항상 트리임
            pNextFocusWnd   =   &(m_pSubScribe->m_treeRepository);
        }

        
		CWnd* dddd  =   pNextFocusWnd->SetFocus();
        //////////////////////////////////////////////////////////////////////////
        // The SetFocus function sets the keyboard focus to the specified window. 
/*        // The window must be attached to the calling thread's message queue.
        AttachThreadInput(
            GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(),NULL),
            GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(),NULL),
            TRUE);

        CWnd* dddd  =   pNextFocusWnd->SetFocus();

        AttachThreadInput(
            GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(), NULL),
            GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(), NULL),
            FALSE);
*/
    }   else    {
        FishMessageBox(STR_ERR_FM_WRONGMEMBER, STR_ERR_FM_ERROR, MB_OK);
    }
    return NULL;
}

BOOL CFocusManager::MoveToListCtrl()
{
    CWnd* pCurrentFocusWnd  =   GetFocus();  //CWnd::FromHandle( ::GetActiveWindow() );  //this->m_hWnd; 
    CWnd* pNextFocusWnd     =   &(m_pRssList->m_listRSS);

    if ( !pCurrentFocusWnd || !pNextFocusWnd || !(pCurrentFocusWnd->GetSafeHwnd()) || !(pNextFocusWnd->GetSafeHwnd()) ) return FALSE;

    CWnd* pWnd  =   pNextFocusWnd->SetFocus();
   
    //////////////////////////////////////////////////////////////////////////
    // SELECTION ROUTINE
//    if ( m_pRssList->m_listRSS.GetHotItem() == -1 && m_pRssList->m_listRSS.GetItemCount() != 0 )
//        m_pRssList->m_listRSS.SetHotItem( 0 );
//	if(m_pRssList->m_listRSS.GetItemCount() > 0 && m_pRssList->m_listRSS.GetSelectedCount() == 0)
//		m_pRssList->m_listRSS.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

    return TRUE;
}

BOOL CFocusManager::MoveToTreeCtrl()
{
    CWnd* pCurrentFocusWnd  =   GetFocus();  //CWnd::FromHandle( ::GetActiveWindow() );  //this->m_hWnd; 
    CWnd* pNextFocusWnd     =   &(m_pSubScribe->m_treeRepository);

    if ( !pCurrentFocusWnd || !pNextFocusWnd || !(pCurrentFocusWnd->GetSafeHwnd()) || !(pNextFocusWnd->GetSafeHwnd()) ) return FALSE;

    CWnd* pWnd  =   pNextFocusWnd->SetFocus();
/*    AttachThreadInput(
        GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(),NULL),
        GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(),NULL),
        TRUE);
    
    CWnd* pWnd  =   pNextFocusWnd->SetFocus();
    
    AttachThreadInput(
        GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(), NULL),
        GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(), NULL),
        FALSE);    
*/
    return TRUE;
}

BOOL CFocusManager::MoveToSearchDlg()
{
    CWnd* pCurrentFocusWnd  =   GetFocus();  //CWnd::FromHandle( ::GetActiveWindow() );  //this->m_hWnd; 
    CWnd* pNextFocusWnd     =   &(m_pSearch->m_edtSearch);

    if ( !pCurrentFocusWnd || !pNextFocusWnd || !(pCurrentFocusWnd->GetSafeHwnd()) || !(pNextFocusWnd->GetSafeHwnd()) ) return FALSE;

	CWnd* pWnd  =   pNextFocusWnd->SetFocus();
	/*
    AttachThreadInput(
        GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(),NULL),
        GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(),NULL),
        TRUE);
    
    CWnd* pWnd  =   pNextFocusWnd->SetFocus();
    
    AttachThreadInput(
        GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(), NULL),
        GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(), NULL),
        FALSE);    
		*/

    return TRUE;
}
