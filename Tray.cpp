#include "stdafx.h"
#include "Tray.h"
#include "resource.h"

#define WM_ICON_NOTIFY			WM_USER + 1
#define ANIMATION_DELAY			400 //millisecond

UINT CTray::m_nIDEvent = WM_USER + 2;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTray::CTray()
{
	m_bEnabled = FALSE;
	m_bMenuEnable = FALSE;
	m_nCurrentIcon = 0;
	m_hSavedIcon = NULL;
	m_hExBmpIcon = NULL;
	m_nFeedingCnt = 0;
	m_nAniType = -1;
	m_bAnimating = FALSE;
	m_AniIconList.RemoveAll();	
	memset(&m_Nid, 0, sizeof(m_Nid));
}

CTray::~CTray()
{
	if(m_hExBmpIcon)
	{
		DestroyIcon(m_hExBmpIcon);
		m_hExBmpIcon = NULL;
	}

	if(m_AniIconList.GetSize() > 0)
	{
		for(int j = 0; j < m_AniIconList.GetSize(); j++)
		{
			DestroyIcon(m_AniIconList.GetAt(j));
		}
		m_AniIconList.RemoveAll();
	}

	DestroyWindow();
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, HICON icon, UINT uID)
{

	CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,10,10, NULL, 0);

	m_Nid.cbSize = sizeof(m_Nid);
	m_Nid.hWnd = pParent->GetSafeHwnd()? pParent->GetSafeHwnd() : m_hWnd;
	m_Nid.uID = uID;
	m_Nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_Nid.uCallbackMessage = uCallbackMessage;

	m_Nid.hIcon = icon;

	lstrcpyn(m_Nid.szTip, szToolTip, sizeof(m_Nid.szTip) / sizeof(m_Nid.szTip[0]));

	m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_Nid);

	return m_bEnabled;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
void CTray::ShowIcon()
{
	if (m_bEnabled && m_bHidden) {
		m_Nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		//m_Nid.uFlags = 0;
		Shell_NotifyIcon(NIM_ADD, &m_Nid);
		m_bHidden = FALSE;
	}
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
void CTray::HideIcon()
{
	if (m_bEnabled && !m_bHidden) {
		m_Nid.uFlags = NIF_ICON;
		Shell_NotifyIcon(NIM_DELETE, &m_Nid);
		m_bHidden = TRUE;
	}
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
void CTray::RemoveIcon()
{
	StopAnimation();

	if(m_AniIconList.GetSize() > 0)
	{
		for(int j = 0; j < m_AniIconList.GetSize(); j++)
		{
			DestroyIcon(m_AniIconList.GetAt(j));
		}
		m_AniIconList.RemoveAll();
	}

	m_Nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_DELETE, &m_Nid);	
	
	m_bEnabled = FALSE;
	m_bHidden = FALSE;	
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::SetPopMenu(UINT menuid)
{
	m_MenuId = menuid;
	m_bMenuEnable = m_Menu.LoadMenu(m_MenuId);

	return m_bMenuEnable;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::SetIcon(HICON icon)
{
	if(!m_bEnabled)
		return FALSE;

	m_Nid.uFlags = NIF_ICON;
			
	m_Nid.hIcon = icon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_Nid);	
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::SetTrayIcon(UINT uResourceId, int flag)
{
	HICON hIcon;

	if(flag == 0)
	{
		hIcon = AfxGetApp()->LoadIcon(uResourceId);
	}
	else
	{
		CBitmap bmpIcon;
		CImageList	trayicon;
		
		if(bmpIcon.LoadBitmap(uResourceId))
		{
			if(!trayicon.Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 1))
				return FALSE;

			trayicon.Add(&bmpIcon, RGB(88, 255, 0));
		}

		if(m_hExBmpIcon)
		{
			DestroyIcon(m_hExBmpIcon);
			m_hExBmpIcon = NULL;
		}

		m_hExBmpIcon = trayicon.ExtractIcon(0);

		hIcon = m_hExBmpIcon;
	}	

	return SetIcon(hIcon);
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::SetAniIconList(auto_ptr<HICON> pHIconList, int nNumIcons)
{

	if(m_AniIconList.GetSize() > 0)
	{
		for(int j = 0; j < m_AniIconList.GetSize(); j++)
		{
			DestroyIcon(m_AniIconList.GetAt(j));
		}
		m_AniIconList.RemoveAll();
	}    

    try 
	{
	    for (int i = 0; i < nNumIcons; i++)
		    m_AniIconList.Add((pHIconList.get())[i]);
    }
    catch (CMemoryException *e)
    {
        e->ReportError();
        e->Delete();
        m_AniIconList.RemoveAll();		

        return FALSE;
    }

    return TRUE;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::SetAniIconList(UINT uBmpResourceID)
{
    CBitmap bmpIconList;
	CImageList	animationIcon;

	if(m_AniIconList.GetSize() > 0)
	{
		for(int j = 0; j < m_AniIconList.GetSize(); j++)
		{
			DestroyIcon(m_AniIconList.GetAt(j));
		}
		m_AniIconList.RemoveAll();
	}

	m_AniIconList.RemoveAll();
	
	if(!bmpIconList.LoadBitmap(uBmpResourceID))
		return FALSE;
	
	if(!animationIcon.Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 1))
		return FALSE;
		
	animationIcon.Add(&bmpIconList, RGB(88, 255, 0));	
	
    try 
	{
	    for (int i = 0; i < animationIcon.GetImageCount(); i++)
		{
		    m_AniIconList.Add(animationIcon.ExtractIcon(i));
		}
    }
    catch (CMemoryException *e)
    {
        e->ReportError();
        e->Delete();
        m_AniIconList.RemoveAll();		

        return FALSE;
    }
	

    return TRUE;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::Animate(UINT nDelayMilliSeconds, int nNumSeconds /*=-1*/)
{
	//if(m_nAniType != DOWNLOADING)
	StopAnimation();

    m_nCurrentIcon = 0;
    m_nAnimationPeriod = nNumSeconds;
	m_StartTime = COleDateTime::GetCurrentTime();
	
	SetTrayIcon(IDB_TRAY_NORMAL, 1);

    m_hSavedIcon = (m_bEnabled)? m_Nid.hIcon : NULL;
	

	// Setup a timer for the animation
	m_uIDTimer = SetTimer(m_nIDEvent, nDelayMilliSeconds, NULL);
	if(m_uIDTimer != 0)
		m_bAnimating = TRUE;

    return (m_uIDTimer != 0);
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::StopAnimation(int id)
{
	BOOL bResult = FALSE;

	if(m_nAniType != DOWNLOADING)
	{
		if (m_uIDTimer)
			bResult = KillTimer(m_uIDTimer);

		m_uIDTimer = 0;

		if (m_hSavedIcon)
			SetIcon(m_hSavedIcon);

		m_hSavedIcon = NULL;
	}
	else if(m_nAniType == DOWNLOADING)
	{
		if(m_nFeedingCnt != 0)
			m_nFeedingCnt--;

		list<int>::iterator it;

		for(it = m_listFeedingChannel.begin(); it != m_listFeedingChannel.end(); it++)
		{
			if(*it == id)
			{
				m_listFeedingChannel.erase(it);
				break;
			}
		}

		bResult = TRUE;
	}

    return bResult;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
BOOL CTray::StepAnimate()
{
	if (!m_AniIconList.GetSize())
		return FALSE;

    m_nCurrentIcon++;
    if (m_nCurrentIcon >= m_AniIconList.GetSize())
        m_nCurrentIcon = 0;

    return SetIcon(m_AniIconList[m_nCurrentIcon]);
}





/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *
 *

 */
LRESULT CTray::OnTrayNotification(UINT wParam, LONG lParam) 
{
    //Return quickly if its not for this tray icon
    if (wParam != m_Nid.uID)
        return 0L;

    CMenu* pSubMenu;
    CWnd* pTarget = AfxGetMainWnd();
	if(!pTarget) return 0;

    // Clicking with right button brings up a context menu
    if (LOWORD(lParam) == WM_RBUTTONUP)
    {
		if (!m_bMenuEnable || !(pSubMenu = (CMenu*)m_Menu.GetSubMenu(0)))
		//if (!m_bMenuEnable || !(pSubMenu = menu.GetSubMenu(0)))
			return 0;
		
		::SetMenuDefaultItem(pSubMenu->m_hMenu, m_MenuId, m_bMenuEnable);
		
        // Display and track the popup menu
        CPoint pos;
        GetCursorPos(&pos);

        pTarget->SetForegroundWindow();

        ::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, pTarget->GetSafeHwnd(), NULL);

		//pSubMenu->TrackPopupMenu( TPM_RIGHTBUTTON | TPM_LEFTALIGN, pos.x, pos.y, pTarget, 0);

        
        pTarget->PostMessage(WM_NULL, 0, 0);
		
    } 
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
    {
		if(!pTarget->IsWindowVisible() || pTarget->IsIconic())
			pTarget->ShowWindow(SW_SHOWNORMAL);

        pTarget->SetForegroundWindow();  

/*        UINT uItem;
        if (m_bMenuEnable)
        {
            if (!(pSubMenu = (CMenu*)m_Menu.GetSubMenu(0)))
				return 0;

            uItem = pSubMenu->GetMenuItemID(m_MenuId);
        }
        else
            uItem = m_MenuId;
        
        pTarget->SendMessage(WM_COMMAND, uItem, 0);
*/        
    }

    return 1;
}


/*

 *	witten by aquarelle
 *	created		2005. 12. 9
 
 *	[Description]
 *	트레이
 *

 */
LRESULT CTray::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (message == m_Nid.uCallbackMessage)
        return OnTrayNotification(wParam, lParam);
	
	return CWnd::WindowProc(message, wParam, lParam);
}


BEGIN_MESSAGE_MAP(CTray, CWnd)
	//{{AFX_MSG_MAP(CTray)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTray message handlers

void CTray::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
    ASSERT(nIDEvent == m_nIDEvent);

	
    COleDateTime CurrentTime = COleDateTime::GetCurrentTime();
    COleDateTimeSpan period = CurrentTime - m_StartTime;
    if (m_nAnimationPeriod > 0 && m_nAnimationPeriod < period.GetTotalSeconds())
    {
		//if(m_nAniType == DOWNLOADING && m_nFeedingCnt == 0)
		if(m_nAniType == DOWNLOADING && m_listFeedingChannel.empty())
		{
			if (m_uIDTimer)
				KillTimer(m_uIDTimer);

			m_uIDTimer = 0;

			if (m_hSavedIcon)
				SetIcon(m_hSavedIcon);

			m_hSavedIcon = NULL;
			m_nAniType = -1;

			//SetTrayIcon(IDB_TRAY_NORMAL, 1);

			return;
		}
		else if(m_nAniType != DOWNLOADING)
		{
			StopAnimation();
			return;
		}
    }
	
    StepAnimate();
	
	//CWnd::OnTimer(nIDEvent);
}


void CTray::StartAnimaition(int anitype, int id)
{
	if(anitype == DOWNLOADING)
	{
		//if(m_nFeedingCnt == 0 && m_nAniType != anitype)
		if(m_listFeedingChannel.empty() && m_nAniType != anitype)
		{
			m_nAniType = anitype;
			//m_nFeedingCnt++;

			m_listFeedingChannel.push_back(id);
			
			SetAniIconList(IDB_TRAY_DOWNLOADING);
			Animate(ANIMATION_DELAY, 2);
		}
		else
		{
			//m_nFeedingCnt++;
			m_listFeedingChannel.push_back(id);
		}
	}
	else if(anitype == CONNECTING)
	{
		m_nAniType = anitype;
		m_nFeedingCnt = 0;
		SetAniIconList(IDB_TRAY_CONNECTING);
		Animate(ANIMATION_DELAY, 2);
	}
}