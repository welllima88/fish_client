// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FISH.h"

#include "MainFrm.h"

#include "Dlg_RSSList.h"

#include "View_Information.h"
#include "Dlg_Subscribe.h"
#include "Dlg_Search.h"
#include "Dlg_Browser.h"
#include "Dlg_RSSMain.h"
#include "RSSDB.h"
#include "ResultManager.h"
#include "fish_common.h"
#include "LocalProperties.h"
#include "TaskbarNotifier.h"
#include "./GnuModule/GnuDoc.h"
#include "./GnuModule/GnuControl.h"
#include "SearchManager.h"
#include "Dlg_Properties.h"
#include "GlobalIconManager.h"
#include "Dlg_Login.h"
#include "view_repository/repositoryitempropertydlg.h"
#include "FocusManager.h"
#include "ScrapManager.h"
#include "OPMLSock.h"
#include "fish_def.h"
#include "URL.h"

#include "./IrcModule/httpd.h"
#include "./IrcModule/IrcdManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CFISHApp theApp;
#define ID_BUTTONCLOSE							3900
#define ID_BUTTONMAX							3901
#define ID_BUTTONMIN							3902

#define WINDOW_MINIMIZE_WIDTH					800
#define WINDOW_MINIMIZE_HEIGHT					600

#define MAX_FLUSHTIME							60			// 3 minutes

#define WM_ICON_NOTIFY							WM_USER + 1
#define DEFUALT_BORDERMARGIN					10

#define STR_ERR_GNUSOCK_FAILED					_T("�˻� ��� ���⸦ �����߽��ϴ�.")
#define STR_BTN_TEXT_CLOSE                      _T("�ݱ�")
#define STR_BTN_TEXT_MAXIMIZE                   _T("�ִ�ȭ")
#define STR_BTN_TEXT_MINIMIZE                   _T("�ּ�ȭ")

// Fish �̺�Ʈ �޽��� ���
const static UINT UWM_ARE_YOU_FISH=RegisterWindowMessage(FISH_GUID);

// Ʈ���� ������ ���� �ذ�
UINT g_uShellRestart = RegisterWindowMessage(_T("TaskbarCreated")); 

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_COPYDATA()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CREATE_NEW_SCRAP_MGR, OnCreateNewScrapMgr)
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_WM_SYSCOMMAND()
	ON_WM_ENDSESSION()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITVIEW, OnInitView)
	ON_MESSAGE(WM_RSSDBRESULT, OnRSSDBResult)
	ON_MESSAGE(WM_POSTINITALL, OnPostInitAll)
	ON_MESSAGE(WM_NAVIGATE_URL, OnNavigateURL)
	ON_MESSAGE(WM_NODEUPDATE, OnNodeUpdate)
	ON_MESSAGE(WM_REQUESTLOGIN, OnRequestLogin)
	ON_MESSAGE(WM_LOGINCOMPLETE, OnLoginComplete)
	ON_MESSAGE(WM_IRCDAUTHCOMPLETE, OnIrcdAuthComplete)
	ON_MESSAGE(WM_SCRAP, OnScrap)
	ON_MESSAGE(WM_READON, OnReadOn)
	ON_MESSAGE(WM_VIEW_IDPOST, OnReadPost)
	ON_MESSAGE(WM_GETNEWBROWSERPTR, OnGetNewBrowserPtr)
	ON_MESSAGE(WM_NAVIGATENEW, OnNavigateNew)
    ON_MESSAGE(WM_USERREQ_GOTO_ADDRBAR, OnUserreqGotoAddrbar)
	ON_REGISTERED_MESSAGE(g_uShellRestart, OnTrayShow)
	ON_REGISTERED_MESSAGE(UWM_ARE_YOU_FISH, OnAreYou)
	ON_BN_CLICKED(ID_BUTTONCLOSE, OnBtnClose)
	ON_BN_CLICKED(ID_BUTTONMAX, OnBtnMaximize)
	ON_BN_CLICKED(ID_BUTTONMIN, OnBtnMinimize)
	ON_COMMAND(ID_TRAY_OPEN, OnTrayOpen)
	ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
    ON_COMMAND_RANGE(ID_SCRAP_BASE_UID, ID_SCRAP_BASE_UID + MAX_SCRAPMENU_ITEM, OnCommandRange)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

#ifndef WH_MOUSE_LL
#define WH_MOUSE_LL        14

/*
 * Structure used by WH_MOUSE_LL
 */
typedef struct tagMSLLHOOKSTRUCT {
    POINT   pt;
    DWORD   mouseData;
    DWORD   flags;
    DWORD   time;
    DWORD   dwExtraInfo;
} MSLLHOOKSTRUCT, FAR *LPMSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT;
#endif

HINSTANCE hInstance = NULL;
HHOOK hook = NULL;
HWND hHookWnd = NULL;
HWND hOverWnd = NULL;
HWND hBrowserWnd = NULL;

void SetOverHwnd(HWND hWnd)
{
	hOverWnd = hWnd;
}
HWND GetOverHwnd()
{
	return hOverWnd;
}

void SetBrowserHwnd(HWND hWnd)
{
	hBrowserWnd = hWnd;
}

HWND GetBrowserHwnd()
{
	return hBrowserWnd;
}
// �Ʒ� �ڵ�� ��ŷ�� ���� ����
/*
static LRESULT CALLBACK MouseWheelProc(
  int nCode,     // hook code
  WPARAM wParam, // message identifier
  LPARAM lParam  // message data
)
{
	if(nCode < 0)
	{
		CallNextHookEx(hook, nCode, wParam, lParam);
		return 0;
	} 

	HWND hWnd = ::GetForegroundWindow();
	if(hWnd == hHookWnd)
	{
		if(wParam == WM_MOUSEMOVE)
		{
			LPMSLLHOOKSTRUCT lpHS = (LPMSLLHOOKSTRUCT) lParam;
			::PostMessage(hWnd, WM_MOUSEMOVE, (WPARAM) lpHS->mouseData,(LPARAM) MAKELPARAM (lpHS->pt.x, lpHS->pt.y));
		}
	}


	return CallNextHookEx(hook, nCode, wParam, lParam);
}

BOOL CMainFrame::CreateMouseHook()
{
	if(hook) return FALSE;
	hInstance = AfxGetInstanceHandle();
	hook = SetWindowsHookEx(WH_MOUSE_LL, MouseWheelProc, hInstance, NULL);

	if(hook)
	{
		hHookWnd = GetSafeHwnd();
		return TRUE;
	}
	return FALSE;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_bInit				= FALSE;
	m_uBorderMargin		= DEFUALT_BORDERMARGIN;
	m_bSelfDraw			= TRUE;
	m_bHide				= FALSE;
	m_pTmpMenu			= NULL;
	m_bMaxmize			= FALSE;
	m_hLoginWnd			= NULL;
	m_bPostInit			= FALSE;

	m_nFlushTime		= 0;

	m_rgnResize.CreateRectRgn(-1, -1, -1, -1);

	m_rgnClip.CreateRectRgn(0, 0, 0, 0);

    m_dwScrapPostID =   -1;

    m_bInTimer          = FALSE;
	m_pVI				= NULL;

	m_hBrowserWnd		= 0;

	m_nMainTimer		= NULL;
	m_nGnuOpenTimer		= NULL;

	SetWindowMinMax();
}

CMainFrame::~CMainFrame()
{
	SetOverHwnd(NULL);
	if(hook)
	{
		UnhookWindowsHookEx(hook);
		hook = NULL;
	}
	if(m_nMainTimer) KillTimer(m_nMainTimer);
	if(m_nGnuOpenTimer) KillTimer(m_nGnuOpenTimer);
	m_pVI = NULL;
	theApp.m_spGD->ClearAll();
	theApp.m_spGD->SaveProperties();
	theApp.m_spLP->SaveProperties();

	::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, m_bSetDragFullWindows, NULL, NULL);
	
	if(m_spSubscribe != NULL) delete m_spSubscribe.release();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD cs = GetClassLong(GetSafeHwnd(), GCL_STYLE);
	cs ^= (CS_VREDRAW | CS_HREDRAW);
	SetClassLong(GetSafeHwnd(), GCL_STYLE, cs);

	int cx, cy;
	CString tmp = ReadRegData(_T(""), REG_STR_MFCX);
	cx = _ttoi(tmp);
	tmp = ReadRegData(_T(""), REG_STR_MFCY);
	cy = _ttoi(tmp);

	CRect rectWnd;		
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWnd, 0);
//	rectWnd.InflateRect(3, 3, 3, 3);

	if(cx < WINDOW_MINIMIZE_WIDTH) cx = WINDOW_MINIMIZE_WIDTH;
	else if(cx > rectWnd.Width()) cx = rectWnd.Width();

	if(cy < WINDOW_MINIMIZE_HEIGHT) cy = WINDOW_MINIMIZE_HEIGHT;
	else if(cy > rectWnd.Height()) cy = rectWnd.Height();

	MoveWindow(lpCreateStruct->x, lpCreateStruct->y, cx, cy);
	
	if(
		(lpCreateStruct->x + cx > rectWnd.Width()) 
		|| (lpCreateStruct->y + cy > rectWnd.Height())
		)
	{
		CenterWindow();
	}

	//Coded by lizzy 050528
	//���콺�� ���� ���� â ���� ǥ�� ���� ����
	::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS,0, &m_bSetDragFullWindows, 0);
	//���콺�� ���� ���� â ���� ǥ��..Ǯ��
	::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, NULL);

	m_spSubscribe		= SmartPtr<CDlg_Subscribe> (new CDlg_Subscribe());
	m_spSearch			= SmartPtr<CDlg_Search> (new CDlg_Search());

    //////////////////////////////////////////////////////////////////////////
    // ��Ŀ�� �Ŵ��� �ʱ�ȭ �۾�
    CFocusManager::getInstance()->m_pSearch     =   m_spSearch.get();
    CFocusManager::getInstance()->m_pSubScribe  =   m_spSubscribe.get();

	m_spSubscribe->Create(this);
	m_spSearch->Create(this);

	m_spSubscribe->ShowWindow(SW_NORMAL);
	m_spSearch->ShowWindow(SW_NORMAL);

    // SYS BTN INIT
    {
        enum { 
            TRANSPARENT_COLOR   =   RGB(0xDE, 0xFF, 0x00),
        };

        CClientDC dc(this);
        CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
        CImageList* sysIMGLIST  =   BMPManager->getSysmenuBtn();

        CRect rect;
        CBitmap btnCls_BMP;
        CBitmap btnMin_BMP;
        CBitmap btnMax_BMP;

        m_btnClose.SetTransParentColor(TRANSPARENT_COLOR);
        m_btnMaximize.SetTransParentColor(TRANSPARENT_COLOR);
        m_btnMinimize.SetTransParentColor(TRANSPARENT_COLOR);

        m_btnClose.Create(STR_BTN_TEXT_CLOSE, WS_VISIBLE, rect, this, ID_BUTTONCLOSE);
        m_btnMaximize.Create(STR_BTN_TEXT_MAXIMIZE, WS_VISIBLE, rect, this, ID_BUTTONMAX);
        m_btnMinimize.Create(STR_BTN_TEXT_MINIMIZE, WS_VISIBLE, rect, this, ID_BUTTONMIN);
        
        m_btnClose.ShowWindow(SW_NORMAL);
        m_btnMaximize.ShowWindow(SW_NORMAL);
        m_btnMinimize.ShowWindow(SW_NORMAL);
        
        m_btnClose.SetShowText(FALSE);
        m_btnMaximize.SetShowText(FALSE);
        m_btnMinimize.SetShowText(FALSE);


        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 3, &btnCls_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 0, &btnMin_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 1, &btnMax_BMP);

        m_btnClose.SetEnableBitmap(&btnCls_BMP);
        m_btnMinimize.SetEnableBitmap(&btnMin_BMP);
        m_btnMaximize.SetEnableBitmap(&btnMax_BMP);

        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 7, &btnCls_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 4, &btnMin_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 5, &btnMax_BMP);

        m_btnClose.SetFocussedBitmap(&btnCls_BMP);
        m_btnMinimize.SetFocussedBitmap(&btnMin_BMP);
        m_btnMaximize.SetFocussedBitmap(&btnMax_BMP);

        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 11, &btnCls_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 8, &btnMin_BMP);
        BMPManager->GetBMPfromList(&dc, sysIMGLIST, 9, &btnMax_BMP);

        m_btnClose.SetDownBitmap(&btnCls_BMP);
        m_btnMinimize.SetDownBitmap(&btnMin_BMP);
        m_btnMaximize.SetDownBitmap(&btnMax_BMP);
    }

	// Tray Icon ����..
    {
        m_TrayIcon = SmartPtr<CTray> (new CTray());	
        CBitmap bmpIcon;
        CImageList	trayicon;
        bmpIcon.LoadBitmap(IDB_TRAY_NORMAL);
        trayicon.Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 1);
		trayicon.Add(&bmpIcon, RGB(88, 255, 0));
        m_TrayIcon->Create(NULL, WM_ICON_NOTIFY, _T("Fish"), trayicon.ExtractIcon(0), IDB_TRAY_NORMAL);		
		m_TrayIcon->SetPopMenu(IDR_MENU_TRAY);
        trayicon.DeleteImageList();        
		bmpIcon.DeleteObject();
    }

	// �˸�â ����...
	{
		m_spNotifier = SmartPtr<CTaskbarNotifier> (new CTaskbarNotifier());
		m_spNotifier->Create(this);
		m_spNotifier->SetBitmap(IDB_NOTIFIER_BG, 0, 255, 0);  //bmp, transparent color.
	}

	theApp.m_spLP->SetMainHwnd(GetSafeHwnd());

	MoveButtons();

	m_pTmpMenu = GetMenu();
	
	// Logo ���� ����
	CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
	CBitmap*    bmpTopLeft  = BMPManager->getMainframeBk();
	BITMAP bm;
	bmpTopLeft->GetBitmap(&bm);
	m_rcLogo.SetRect(0, 0, bm.bmWidth, bm.bmHeight);
	m_rcLogo.DeflateRect(15, 15, 0, 5);

	FrameHide();
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style &= ~FWS_ADDTOTITLE; //Untitled ���ֱ� 
	cs.dwExStyle ^= WS_EX_CLIENTEDGE;
	cs.style |= WS_CLIPCHILDREN;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

/************************************************************************
OnSize
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/16:�ִ�, �ּ�ȭ�� ���� �������� �ý��� �������� ��ȭ �߰�
	2006/02/01:�ִ�, �ּ�ȭ�� ���� �ý��� ������ ��ȭ ���� by moonknit
************************************************************************/
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE(_T("CMainFrame::OnSize\r\n"));
	CFrameWnd::OnSize(nType, cx, cy);

	if(IsWindowVisible())
	{
		MakeNCRgn();

		CRect clientRT;
		GetClientRect(&clientRT);

		m_rcScreenWnd = clientRT;
		ClientToScreen(&m_rcScreenWnd);

		MoveControls();

		m_splitter.m_nMaxBottom =   clientRT.right - MAINFRAME_SPLITTER_RIGHT_LIMIT;
		m_splitter.m_nMaxRight  =   clientRT.right - MAINFRAME_SPLITTER_RIGHT_LIMIT;
	}
}


void CMainFrame::MoveControls()
{
    if(!m_bInit) return;

    enum {
        SEARCH_DLG_TOP_PADDING      =   0,          // unit is pixel
        SEARCH_DLG_BODY_SPAN        =   2,
        SEARCH_DLG_WIDTH            =   379,
        SEARCH_DLG_HEIGHT           =   59,
        SUBSCRIBE_DLG_WIDTH         =   200,
        HORIZONTAL_SPLITTER_TOP_PADDING     =   SEARCH_DLG_TOP_PADDING + SEARCH_DLG_HEIGHT,
        HORIZONTAL_SPLITTER_LEFT_PADDING    =   8,
        HORIZONTAL_SPLITTER_RIGHT_PADDING   =   8,
        HORIZONTAL_SPLITTER_BOTTOM_PADDING  =   8,
    };

	if(!m_spSubscribe  || !m_spSearch) return;

    // �ý��� ��ư ��ġ ����
	MoveButtons();

    // �˻�â ��ġ ����
	CRgn rgnTmp;
	rgnTmp.CreateRectRgn(0, 0, 0, 0);

	CRect clientRT, searchRT;
	GetClientRect(&clientRT);

	m_rgnClip.SetRectRgn(&clientRT);

    searchRT         =   clientRT;
	searchRT.top     =   SEARCH_DLG_TOP_PADDING;
	searchRT.bottom  =   searchRT.top + SEARCH_DLG_HEIGHT;
	searchRT.left    =   searchRT.Width() / 2 - SEARCH_DLG_WIDTH / 2;
	searchRT.right   =   searchRT.left + SEARCH_DLG_WIDTH;

	m_spSearch->MoveWindow(&searchRT);

	rgnTmp.SetRectRgn(&searchRT);
	m_rgnClip.CombineRgn(&m_rgnClip, &rgnTmp, RGN_DIFF);

    // ���� ����â ũ��� ��ġ ����
	if(m_bInit)
	{
		if(m_splitter.GetSafeHwnd())
		{
			CRect splitRT;
			splitRT.SetRect(clientRT.left   +   HORIZONTAL_SPLITTER_LEFT_PADDING, 
                clientRT.top    +   HORIZONTAL_SPLITTER_TOP_PADDING, 
                clientRT.right  -   HORIZONTAL_SPLITTER_RIGHT_PADDING,
                clientRT.bottom -   HORIZONTAL_SPLITTER_BOTTOM_PADDING);

			m_splitter.MoveWindow( &splitRT );

			rgnTmp.SetRectRgn(&splitRT);
			m_rgnClip.CombineRgn(&m_rgnClip, &rgnTmp, RGN_DIFF);
		}
	}

	Invalidate();
}

/**************************************************************************
 * method CMainFrame::RequestExplorerURL
 *
 * written by moonknit
 *
 * @history
 * created 2005-11
 *
 * @Description
 * URLŽ���� â�� ��û�Ͽ� �� ����¡ �۾��� �����Ѵ�.
 *
 * @Parameters
 * (int CString) url - �� ����¡�� ��� URL
 **************************************************************************/
void CMainFrame::RequestExplorerURL(CString url)
{
	if (!m_pVI && !m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information ))) return;

	CDlg_Browser* pBrowser = (CDlg_Browser*) m_pVI->GetCurrentExplorerBrowser();

	if(!pBrowser) return;
	pBrowser->Navigate(url);
}

void CMainFrame::OnClose() 
{
	if(!theApp.m_spLP->GetCloseBtnExit())
		ShowWindow(SW_HIDE);
	else
	{
		if(m_bPostInit)
			CloseFrame();
		else
			SetTimer(TID_EXIT, 500, NULL);
	}
}

void CMainFrame::CloseFrame()
{
	// �ϴ� ���߰� ����.
	ShowWindow(SW_HIDE);

	// clear to remove closing popup from Web Browser
	if (m_pVI && m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information )))
	{
		m_pVI->Clear();
		m_pVI = NULL;
	}

	int cx, cy;

	if(m_bMaxmize)
	{
		cx = m_rectBeforeWnd.Width();
		cy = m_rectBeforeWnd.Height();
	}
	else
	{
		CRect rcWnd;
		GetClientRect(&rcWnd);
		cx = rcWnd.Width();
		cy = rcWnd.Height();
	}

	TCHAR tmp[16];
	WriteRegData(_T(""), REG_STR_MFCX, _itot(cx, tmp, 10));
	WriteRegData(_T(""), REG_STR_MFCY, _itot(cy, tmp, 10));

	// Save Test ��...
//	if(m_spSubscribe->GetFlushOPMLFlag())
//	{
	m_spSubscribe->OPMLSave();
//		if(m_spSubscribe->OPMLSave() && theApp.m_spLP->GetUseOPMLSync() && theApp.m_spLP->IsIRCConnected() )
//		{
//			theApp.m_spOS->StartTransfer(theApp.m_spLP->GetUserPath() + _T('\\') + S_DEFAULT_LOCALOPML);
//		}
///	}
	theApp.SetClosing(TRUE);

	theApp.m_spRD->Release();
	theApp.m_spGD->m_Control.StopListening();
	m_TrayIcon->RemoveIcon();

	CFrameWnd::OnClose();
//	PostQuitMessage(0);
}

/**************************************************************************
 * method CMainFrame::OnInitView
 *
 * written by moonknit
 *
 * @history
 * created 2005-11
 *
 * @Description
 * Doc/View�� View�� �ʱ�ȭ �Ϸ� �� ó���� �۾��� �����Ѵ�.
 * Splitter�� �ʱ�ȭ �Ѵ�.
 *
 * @Parameters
 * (in WAPRAM) wParam - (CView_Information*)
 **************************************************************************/
void CMainFrame::OnInitView(WPARAM wParam, LPARAM lParam)
{
	m_pVI = (CView_Information*) wParam;
	if (!m_pVI || !m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information ))) return;

	if(!m_bInit)
	{
		CRect rect;
		GetClientRect(&rect);

		rect.DeflateRect(m_uBorderMargin, m_uBorderMargin, m_uBorderMargin, m_uBorderMargin);

		m_splitter.Create(
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | SS_VERT,
			this,
			m_spSubscribe.get(),
			m_pVI,
			IDC_SPLITTER_VERT_1,
			rect,
			MAINFRAME_SPLITTER_LEFT_LIMIT,
			rect.right - MAINFRAME_SPLITTER_RIGHT_LIMIT
		);

		m_bInit = TRUE;

		m_pVI->m_spRSSMain->SetSubscribeHwnd(m_spSubscribe->GetSafeHwnd());

		// commented by moonknit 2006-01-18
		// ����� �α����� InitView �������� �Ϸ� �ȴٸ� �̰����� ����� �ʱ�ȭ�� �����Ѵ�.
        CFocusManager::getInstance(this);

		m_spSubscribe->PostMessage(WM_SETFOCUS, NULL, NULL);

//		CreateMouseHook();

		InitUser();
//		TryLogin();
	}
}

/**************************************************************************
 * method CMainFrame::InitUser
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-18
 *
 * @Description
 * ����� �ʱ�ȭ�� �����Ѵ�.
 * �� �Լ� ȣ�� ������ ��Ų��.
 **************************************************************************/
void CMainFrame::InitUser()
{

	// Initializing Objects

	// Properties Load
	theApp.m_spLP->LoadProperties();
	theApp.m_spGD->LoadProperties();

	theApp.m_spRD->SetInformWnd(this->GetSafeHwnd());
	theApp.m_spRD->SetSubscriveWnd(m_spSubscribe->GetSafeHwnd());
	

//    CString msg;
//    msg.Format(_T("handler to set : %d"), this->GetSafeHwnd());
//    AfxMessageBox(msg);

	// RSSDB Thread ����
	theApp.m_spRD->Start();

	// Channel Load
	m_spSubscribe->OPMLRequest();
	// -- Initializing User Parameters
}

/**************************************************************************
 * method CMainFrame::OnPostInitAll
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-14
 *
 * @Description
 * ��� �ʱ�ȭ�� �Ϸ��� ó���� �۾��� �̰����� �����Ѵ�.
 * 1. ���� ��ɾ� ó��
 **************************************************************************/
void CMainFrame::OnPostInitAll(WPARAM wParam, LPARAM lParam)
{
	if(m_bPostInit) return;

	m_bPostInit = TRUE;
	// ������ 1ȸ Refresh
	Sleep(100);
	m_spSubscribe->OnBtnRefresh();

	m_nMainTimer = SetTimer(TID_MAINTIMER, 1000, NULL);			// per 1 seconds

	// �̰����� IRC ������ �õ��Ѵ�.
	if(!theApp.m_spLP->GetNotUseP2P()) IrcdAuth();

	// ���� ���� ó��
	if(theApp.pendinglink)
	{
		ProcessArguments((LPCTSTR)  *(theApp.pendinglink));
		delete theApp.pendinglink;
		theApp.pendinglink = NULL;
	}
}

void CMainFrame::OnNavigateURL(WPARAM wParam, LPARAM lParam)
{
	TCHAR* lpszurl = (TCHAR*) wParam;

	NavigateNewWindow(lpszurl);

	delete lpszurl;
}

/**************************************************************************
 * method CMainFrame::GetDlgRSSMain
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * CDlg_RSSMain�� ����Ʈ �����͸� ȹ���Ѵ�.
 *
 * @Parameters
 * (out SmartPtr<CDlg_RSSMain>&) spmain - ��ȯ�� CDlg_RSSMain�� ����Ʈ ������
 **************************************************************************/
BOOL CMainFrame::GetDlgRSSMain(SmartPtr<CDlg_RSSMain>& spmain)
{
	if (!m_pVI || !m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information ))) return FALSE;


	if(!m_bInit) return FALSE;
	
	spmain = m_pVI->m_spRSSMain;
	return TRUE;
}


/**************************************************************************
 * method CMainFrame::GetDlgSubscribe
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * CDlg_Subscribe�� ����Ʈ �����͸� ȹ���Ѵ�.
 *
 * @Parameters
 * (out SmartPtr<CDlg_Subscribe>&) spsubscribe - ��ȯ�� CDlg_Subscribe�� ����Ʈ ������
 **************************************************************************/
BOOL CMainFrame::GetDlgSubscribe(SmartPtr<CDlg_Subscribe>& spsubscribe)
{
	spsubscribe = m_spSubscribe;
	return TRUE;
}

/**************************************************************************
 * method CMainFrame::GetScrapChannel
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * ����Ʈ�� ��ũ���� ��� ��ũ�� ä���� ��ȯ�Ѵ�.
 **************************************************************************/
int CMainFrame::GetScrapChannel()
{
	// ��ũ�� ä�� ����â
	// TO DO
	return -1;
}

/**************************************************************************
 * method CMainFrame::Scrap
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-26
 *
 * @Description
 * �־��� post ��ũ���Ѵ�.
 *
 * @Parameters
 * (in int) postid - ��ũ���� post�� ID
  **************************************************************************/
void CMainFrame::Scrap(int postid)
{
	int channelid = GetScrapChannel();

	// ��ũ�� �Է� ��û
	if(channelid != -1)
		theApp.m_spRD->IPostScrap(postid, channelid);
}

void CMainFrame::Scrap(int channelid, int postid)
{
	if(channelid != -1)
		theApp.m_spRD->IPostScrap(postid, channelid);
}

/**************************************************************************
 * method CMainFrame::Scrap
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * �־��� post�� ��� ��ũ���Ѵ�.
 *
 * @Parameters
 * (in list<int>&) idlist - ��ũ���� ����Ʈ�� id ���
 **************************************************************************/
void CMainFrame::Scrap(list<int>& idlist)
{
	int channelid = GetScrapChannel();

	// ��ũ�� �Է� ��û
	if(channelid != 0)
		theApp.m_spRD->IPostScrap(idlist, channelid);
}

/**************************************************************************
 * method CMainFrame::ReadOn
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * Post�� ReadOn ���·� �����. (undo �Ұ�!!)
 *
 * @Parameters
 * (in int) postid - ������� (Read On) ó�� ��� ����Ʈ�� ���̵�
 **************************************************************************/
void CMainFrame::ReadOn(int postid)
{
	theApp.m_spRD->IPostReadOn(postid);
}

/**************************************************************************
 * method CMainFrame::GetNewWBIDispatch
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * CView_Information�� ��û�Ͽ� ��â�� ���� �� �� �� â�� IWebBrowser�� DISPACH �����͸�
 * ��ȯ�ϵ����Ѵ�.
 *
 * @Parameters
 *
 * @return
 * (LPDISPATCH) - ���� ������ IWebBrowser�� DISPATCH ������
 *				���н� NULL�� ��ȯ�ȴ�.
 **************************************************************************/
LPDISPATCH CMainFrame::GetNewWBIDispatch()
{
	if (!m_pVI || !m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information ))) return NULL;

	if(!m_bInit) return NULL;

	CDlg_Browser* pbrowser = (CDlg_Browser*) m_pVI->NewExplorerBrowser();
	if(!pbrowser) return NULL;

	return pbrowser->GetWBIDispatch();
}

/**************************************************************************
 * method CMainFrame::NavigateNewWindow
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-23
 *
 * @Description
 * url�� �̿��Ͽ� �� â�� ��� Ž���� �����Ѵ�.
 *
 * @Parameters
 * (in LPCTSTR) lpszurl - Ž���� url
 **************************************************************************/
BOOL CMainFrame::NavigateNewWindow(LPCTSTR lpszurl)
{
	if(!lpszurl) return FALSE;
	CString surl = lpszurl;

	if (!m_pVI || !m_pVI->IsKindOf( RUNTIME_CLASS( CView_Information ))) return FALSE;

	if(!m_bInit) return FALSE;

	CDlg_Browser* pbrowser = (CDlg_Browser*) m_pVI->NewExplorerBrowser();
	if(!pbrowser) return FALSE;

	pbrowser->Navigate(surl);

	return TRUE;
}

void CMainFrame::DrawItems(CDC *pDC)
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
DrawBg
@PARAM  : 
@RETURN : 
@REMARK : 
    ��Ʈ�� �޴����� ���� �ѹ��� �� �Լ����ο� enumeration ���� ���ǵǾ�����.
    ã�ƺ��� �� �� �ִٴ� ��333; 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/09:MODIFIED
************************************************************************/
void CMainFrame::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	if(!m_spSearch) return;

    enum {
        TOPLEFT_WIDTH   =   200,
        TOPLEFT_HEIGHT  =   59,
        TOPRIGHT_WIDTH  =   200,
        TOPRIGHT_HEIGHT =   59,
        BOTTOMLEFT_WIDTH    =   15,
        BOTTOMLEFT_HEIGHT   =   11,
        BOTTOMRIGHT_WIDTH    =   15,
        BOTTOMRIGHT_HEIGHT   =   11,
        LEFT_WIDTH      =   8,
        RIGHT_WIDTH     =   8,
        BOTTOMCENTER_WIDTH    =     8,
        BOTTOMCENTER_HEIGHT   =   11,

        ORIGINAL_LEFT_IMG_SIZE      =   634,
        ORIGINAL_RIGHT_IMG_SIZE     =   634,
        ORIGINAL_TOPSPAN_WIDTH      =   2,
        ORIGINAL_TOPSPAN_HEIGHT     =   59,
    };

	CRect clientRT, tmpRT, controlRT;
	GetClientRect(&clientRT);
    CFishBMPManager* BMPManager = CFishBMPManager::getInstance();

//	pBufferDC->SelectClipRgn(&m_rgnClip);

	// ��� �κ��� ��� ��ĥ�ϱ�
    tmpRT   =   clientRT;
    tmpRT.DeflateRect(10, 0, 10, 0);
    tmpRT.bottom    =   TOPLEFT_HEIGHT;

	m_spSearch->GetWindowRect(&controlRT);
	ScreenToClient(&controlRT);
	tmpRT.right = controlRT.left;

    CDC memDC;
    memDC.CreateCompatibleDC(pBufferDC);

	// ����� �ݺ� �̹��� �׸��� - �˻�â�� ����
    CBitmap* pOldBitmap = memDC.SelectObject(BMPManager->getMainframeBk(7));      // LEFT = 5, RIGHT = 6, BOTTOMCENTER = 4, TOPSPAN = 7
    pBufferDC->StretchBlt(tmpRT.left, tmpRT.top, tmpRT.Width(), ORIGINAL_TOPSPAN_HEIGHT, &memDC, 0, 0, ORIGINAL_TOPSPAN_WIDTH, ORIGINAL_TOPSPAN_HEIGHT, SRCCOPY );

	// ����� �ݺ� �̹��� �׸��� - �˻�â�� ����
	tmpRT.left = controlRT.right;
	tmpRT.right = clientRT.right - 10;
	pBufferDC->StretchBlt(tmpRT.left, tmpRT.top, tmpRT.Width(), ORIGINAL_TOPSPAN_HEIGHT, &memDC, 0, 0, ORIGINAL_TOPSPAN_WIDTH, ORIGINAL_TOPSPAN_HEIGHT, SRCCOPY );

    // ��� ����, ���� �׸� �׸���
    CBitmap*    bmpTopLeft  = BMPManager->getMainframeBk();	
    CBitmap*    bmpTopRight = BMPManager->getMainframeBk(1);
    BMPManager->drawTransparent(pBufferDC, bmpTopLeft, CPoint(0,0), FALSE, CPoint(0, 0), RGB(0xFF, 0x00, 0xFF));
    BMPManager->drawTransparent(pBufferDC, bmpTopRight, CPoint(clientRT.right - TOPRIGHT_WIDTH, clientRT.top), FALSE, CPoint(0, 0), RGB(0xFF, 0x00, 0xFF));

    // �ϴ� ���� , ���� �׸� �׸���
    tmpRT   =   clientRT;
    CBitmap*    bmpBottomLeft  = BMPManager->getMainframeBk(2);
    CBitmap*    bmpBottomRight = BMPManager->getMainframeBk(3);
    BMPManager->drawTransparent(pBufferDC, bmpBottomLeft, CPoint(0, tmpRT.bottom - BOTTOMLEFT_HEIGHT ), FALSE, CPoint(0, 0), RGB(0xFF, 0x00, 0xFF));
    BMPManager->drawTransparent(pBufferDC, bmpBottomRight, CPoint(clientRT.right - BOTTOMRIGHT_WIDTH , tmpRT.bottom - BOTTOMLEFT_HEIGHT), FALSE, CPoint(0, 0), RGB(0xFF, 0x00, 0xFF));

    // ���� �׵θ��� �׸���
    memDC.SelectObject(BMPManager->getMainframeBk(5));      // LEFT = 5, RIGHT = 6, BOTTOMCENTER = 4
    pBufferDC->StretchBlt(0, TOPLEFT_HEIGHT, LEFT_WIDTH, clientRT.Height() - TOPLEFT_HEIGHT - BOTTOMLEFT_HEIGHT, &memDC, 0, 0, 8, ORIGINAL_LEFT_IMG_SIZE, SRCCOPY );

    // ���� �׵θ��� �׸���
    memDC.SelectObject(BMPManager->getMainframeBk(6));      // LEFT = 5, RIGHT = 6, BOTTOMCENTER = 4
    pBufferDC->StretchBlt(clientRT.right - RIGHT_WIDTH , TOPLEFT_HEIGHT, LEFT_WIDTH, clientRT.Height() - TOPLEFT_HEIGHT - BOTTOMLEFT_HEIGHT, &memDC, 0, 0, 8, ORIGINAL_RIGHT_IMG_SIZE, SRCCOPY );

    // �ϴ� �߾Ӽ� �׸���
    BITMAP bm;
    BMPManager->getMainframeBk(4)->GetBitmap(&bm);
    memDC.SelectObject(BMPManager->getMainframeBk(4));
    CRect outlineRT = CRect(clientRT.left + BOTTOMLEFT_WIDTH, clientRT.bottom - BOTTOMCENTER_HEIGHT - 1, clientRT.right - BOTTOMRIGHT_WIDTH, clientRT.bottom);
    pBufferDC->StretchBlt(outlineRT.left, outlineRT.top, outlineRT.Width(), outlineRT.Height(), &memDC, 0, 0, bm.bmWidth, BOTTOMCENTER_HEIGHT, SRCCOPY );
    //BMPManager->drawTiledBlt(pBufferDC, BMPManager->getMainframeBk(4), );

	memDC.SelectObject(pOldBitmap);
}

void CMainFrame::DrawOutline(CDC* pBufferDC)
{
	if(!pBufferDC) return;
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{
//	TRACE(_T("CMainFrame::OnEraseBkgnd\r\n"));
	if(m_bSelfDraw)
	{
//		DrawItems(pDC);
		return FALSE;
	}

	return CFrameWnd::OnEraseBkgnd(pDC);
}

/**************************************************************************
 * method CMainFrame::FrameHide
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * Frame�� ����ų� �巯�� �� �ִ�.
 *
 * @Parameters
 * (in BOOL) bhide - TRUE�� ��� Frame�� ����� FALSE�� �巯����.
 **************************************************************************/
void CMainFrame::FrameHide(BOOL bhide)
{
	DWORD hidestyle = WS_CAPTION  | WS_THICKFRAME | WS_OVERLAPPED;
	if(bhide)
	{
		ModifyStyle( hidestyle, NULL, SWP_FRAMECHANGED);
		SetMenu(NULL);
	}
	else
	{
		ModifyStyle( NULL, hidestyle, SWP_FRAMECHANGED);
		SetMenu(m_pTmpMenu);
	}

	m_bHide = bhide;
}

/**************************************************************************
 * method CMainFrame::OnNcHitTest
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-02
 *
 * @Description
 * Frame�� ����ų� �巯�� �� �ִ�.
 *
 * @Parameters
 * (in BOOL) bhide - TRUE�� ��� Frame�� ����� FALSE�� �巯����.
 **************************************************************************/
UINT CMainFrame::OnNcHitTest(CPoint point) 
{
	if(m_bHide)
	{
/*		if(m_bMaxmize == FALSE && m_rccaption.PtInRect(point))
			return HTCAPTION;
		else*/
		return HTCLIENT;
	}
	else
	{
		return CFrameWnd::OnNcHitTest(point);
	}
}

void CMainFrame::MoveButtons()
{
    enum {
        POS_SYSBUTTONTOP	    =   5,
        POS_SYSBUTTONRIGHT	    =   9,
        SPAN_SYSBUTTON		    =   3,
        SIZE_SYSBUTTONHEIGHT    =   13,
        SIZE_SYSBUTTONHWIDTH    =   13,
    };

	CRect rect, tmprect;
	GetClientRect(rect);

	CRgn rgnTmp;
	rgnTmp.CreateRectRgn(0, 0, 0, 0);
	
	tmprect = rect;
	tmprect.top += POS_SYSBUTTONTOP;
	tmprect.bottom = tmprect.top + SIZE_SYSBUTTONHEIGHT;
	tmprect.right -= POS_SYSBUTTONRIGHT;
	tmprect.left = tmprect.right - SIZE_SYSBUTTONHWIDTH;

	if(m_btnClose.GetSafeHwnd())
	{
		m_btnClose.MoveWindow(&tmprect);
		m_btnClose.Invalidate();
	}

	tmprect.right = tmprect.left - SPAN_SYSBUTTON;
	tmprect.left = tmprect.right - SIZE_SYSBUTTONHWIDTH;

	if(m_btnMaximize.GetSafeHwnd())
	{
		m_btnMaximize.MoveWindow(&tmprect);
		m_btnMaximize.Invalidate();
	}

	tmprect.right = tmprect.left - SPAN_SYSBUTTON;
	tmprect.left = tmprect.right - SIZE_SYSBUTTONHWIDTH;

	if(m_btnMinimize.GetSafeHwnd())
	{
		m_btnMinimize.MoveWindow(&tmprect);
		m_btnMinimize.Invalidate();
	}
}

void CMainFrame::OnBtnClose()
{
	if(!theApp.m_spLP->GetCloseBtnExit())
		ShowWindow(SW_HIDE);
	else
	{
		CloseFrame();
	}
}

void CMainFrame::OnBtnMaximize()
{
	CRect rectWnd;		
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWnd, 0);
	rectWnd.DeflateRect(1, 0, 0, 0);
//	rectWnd.InflateRect(3, 3, 3, 3);

	WINDOWPLACEMENT lpwndpl;
	if( GetWindowPlacement(&lpwndpl) ) 
	{
		if( lpwndpl.showCmd == SW_MAXIMIZE )
		{
			// ��ư �̹��� ���� : Normal ���·�...
    		m_bMaxmize = FALSE;

            CClientDC dc(this);
            CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
            CImageList* sysIMGLIST  =   BMPManager->getSysmenuBtn();
            CBitmap normal_BMP;
            CBitmap rollover_BMP;
            CBitmap down_BMP;
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 1, &normal_BMP);
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 5, &rollover_BMP);
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 9, &down_BMP);
            m_btnMaximize.SetEnableBitmap(&normal_BMP);
            m_btnMaximize.SetFocussedBitmap(&rollover_BMP);
            m_btnMaximize.SetDownBitmap(&down_BMP); 

			lpwndpl.showCmd = SW_RESTORE;
			SetWindowPlacement(&lpwndpl);

			MoveWindow(&m_rectBeforeWnd);
        }
		else
		{
			// ��ư �̹��� ���� : Max ���·�....
			m_bMaxmize = TRUE;

			if(lpwndpl.showCmd != SW_SHOWMINIMIZED)
				GetWindowRect(&m_rectBeforeWnd); //���� ���� ���..	

			CClientDC dc(this);
            CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
            CImageList* sysIMGLIST  =   BMPManager->getSysmenuBtn();
            CBitmap normal_BMP;
            CBitmap rollover_BMP;
            CBitmap down_BMP;
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 2, &normal_BMP);
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 6, &rollover_BMP);
            BMPManager->GetBMPfromList(&dc, sysIMGLIST, 10, &down_BMP);
            m_btnMaximize.SetEnableBitmap(&normal_BMP);
            m_btnMaximize.SetFocussedBitmap(&rollover_BMP);
            m_btnMaximize.SetDownBitmap(&down_BMP);

			lpwndpl.showCmd = SW_MAXIMIZE;
			SetWindowPlacement(&lpwndpl);

			MoveWindow(&rectWnd);
		} 
	}
}

void CMainFrame::OnBtnMinimize()
{
	ShowWindow(SW_MINIMIZE);
}

void CMainFrame::OnRSSDBResult(WPARAM wParam, LPARAM lParam)
{
	if(!theApp.IsClosing())	CResultManager::ProcessResult();
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = m_nMinCX;
	lpMMI->ptMinTrackSize.y = m_nMinCY;
	
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::SetWindowMinMax(int nMinCX, int nMinCY)
{
    if(nMinCX < WINDOW_MINIMIZE_WIDTH)
		m_nMinCX = WINDOW_MINIMIZE_WIDTH;
	else
		m_nMinCX = nMinCX;
	if(nMinCY < WINDOW_MINIMIZE_HEIGHT)
		m_nMinCY = WINDOW_MINIMIZE_HEIGHT;
	else
		m_nMinCY = nMinCY;
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bMaxmize == FALSE && m_rgnResize.PtInRegion(point))
	{
//		ReleaseCapture(); 
		HCURSOR curWait = NULL;

		if(m_rcResizeTop.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENS);
		}
		else if(m_rcResizeTopLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENWSE);
		}
		else if(m_rcResizeTopRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENESW);
		}
		else if(m_rcResizeLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZEWE);
		}
		else if(m_rcResizeRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZEWE);
		}
		else if(m_rcResizeBottom.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENS);
		}
		else if(m_rcResizeBottomLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENESW);
		}
		else if(m_rcResizeBottomRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENWSE);
		}
		

		if(curWait != NULL && curWait != GetCursor())
			::SetCursor(curWait);
	}
	else if(m_rcLogo.PtInRect(point))
	{
		HCURSOR curWait = NULL;

		curWait = ::LoadCursor(NULL, IDC_HAND);

		if(curWait != NULL && curWait != GetCursor())
			::SetCursor(curWait);
	}

	CFrameWnd::OnMouseMove(nFlags, point);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bMaxmize == FALSE && m_rgnResize.PtInRegion(point))
	{
		ReleaseCapture(); 
		HCURSOR curWait = NULL;

		if(m_rcResizeTop.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENS);
			SendMessage(WM_SYSCOMMAND, SC_SZTOP, 0);
		}
		else if(m_rcResizeTopLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENWSE);
			SendMessage(WM_SYSCOMMAND, SC_SZTOPLEFT, 0);
		}
		else if(m_rcResizeTopRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENESW);
			SendMessage(WM_SYSCOMMAND, SC_SZTOPRIGHT, 0);
		}
		else if(m_rcResizeLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZEWE);
			SendMessage(WM_SYSCOMMAND, SC_SZLEFT, 0);
		}
		else if(m_rcResizeRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZEWE);
			SendMessage(WM_SYSCOMMAND, SC_SZRIGHT, 0);
		}
		else if(m_rcResizeBottom.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENS);
			SendMessage(WM_SYSCOMMAND, SC_SZBOTTOM, 0);
		}
		else if(m_rcResizeBottomLeft.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENESW);
			SendMessage(WM_SYSCOMMAND, SC_SZBOTTOMLEFT, 0);
		}
		else if(m_rcResizeBottomRight.PtInRect(point))
		{
			curWait = ::LoadCursor(NULL,IDC_SIZENWSE);
			SendMessage(WM_SYSCOMMAND, SC_SZBOTTOMRIGHT, 0);
		}

		if(curWait != NULL && curWait != GetCursor())
			::SetCursor(curWait);	 

	}
	else if(m_bMaxmize == FALSE && m_rccaption.PtInRect(point))
	{
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); 
	}


	if(m_rcLogo.PtInRect(point))
	{
		CURL url;
		url.Open(_T("http://www.3fishes.co.kr/"));
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}

#define RESIZE_MARGIN					5
#define CAPTION_HEIGHT					59
#define DLG_ARC_SIZE					2

/**************************************************************************
 * method CMainFrame::MakeNCRgn
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 *
 * @Description
 * ���� �������� �ܰ��� �����Ѵ�.
 **************************************************************************/
void CMainFrame::MakeNCRgn()
{
	CRect	rc, rccaption;
	GetClientRect(&rc);	

	rccaption = rc;
//	ClientToScreen(&rccaption);

	// make caption region
	rccaption.top		+= RESIZE_MARGIN;
	rccaption.bottom	= rccaption.top + CAPTION_HEIGHT - RESIZE_MARGIN;
	m_rccaption			= rccaption;

	// make resize region
//	SetRoundedRgn(); 

	CRect rc2;
	CRgn rgn1, rgn2;

	rc2 = rc;
	rc2.DeflateRect(RESIZE_MARGIN, RESIZE_MARGIN, RESIZE_MARGIN, RESIZE_MARGIN);

	GetRoundedWindow(rc, DLG_ARC_SIZE / 2, rgn1);
	GetRoundedWindow(rc2, DLG_ARC_SIZE / 2, rgn2);
	
	rgn1.CombineRgn(&rgn1, &rgn2, RGN_DIFF);

	SetResizeRgn(&rgn1);
}

//�ܰ� Border �ڸ���..
#define SIZE_FRAMEDEPTH						3
/**************************************************************************
 * method CMainFrame::SetRoundedRgn
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 *
 * @Description
 * �ܰ��� �ڸ���.
 **************************************************************************/
void CMainFrame::SetRoundedRgn()
{	
	if( this->GetSafeHwnd() && m_bHide)
	{
		CRect   rcMain;
		CRgn    rgnMain;
		GetClientRect(&rcMain);	
		rgnMain.CreateRoundRectRgn(
			rcMain.left// + SIZE_FRAMEDEPTH
			, rcMain.top// + SIZE_FRAMEDEPTH
			, rcMain.right// + SIZE_FRAMEDEPTH + 1
			, rcMain.bottom// + SIZE_FRAMEDEPTH + 1
			, 0, 0);			
		
		SetWindowRgn((HRGN)rgnMain.m_hObject, true);	
		rgnMain.DeleteObject();		
	}	
}

/**************************************************************************
 * method CMainFrame::SetResizeRgn
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 *
 * @Description
 * �������� ������ �����Ѵ�.
 **************************************************************************/
void CMainFrame::SetResizeRgn(CRgn *prgn)
{
	if(!prgn) return;
	if(m_rgnResize.EqualRgn(prgn)) return;
	
	m_rgnResize.CopyRgn(prgn);
	
	CRect rc, rcTmp;
	m_rgnResize.GetRgnBox(&rc);

	// left
	rcTmp.top		= rc.top + RESIZE_MARGIN;
	rcTmp.left		= rc.left;
	rcTmp.bottom	= rc.bottom - RESIZE_MARGIN;
	rcTmp.right		= rc.left + RESIZE_MARGIN;
	
	m_rcResizeLeft	= rcTmp;

	// right
	rcTmp.top		= rc.top + RESIZE_MARGIN;
	rcTmp.left		= rc.right - RESIZE_MARGIN;
	rcTmp.bottom	= rc.bottom - RESIZE_MARGIN;
	rcTmp.right		= rc.right;
	
	m_rcResizeRight	= rcTmp; 

	// top
	rcTmp.top		= rc.top;
	rcTmp.left		= rc.left + RESIZE_MARGIN;
	rcTmp.bottom	= rc.top + RESIZE_MARGIN;
	rcTmp.right		= rc.right - RESIZE_MARGIN;

	m_rcResizeTop	= rcTmp;

	// top left
	rcTmp.top		= rc.top;
	rcTmp.left		= rc.left;
	rcTmp.bottom	= rc.top + RESIZE_MARGIN;
	rcTmp.right		= rc.left + RESIZE_MARGIN;

	m_rcResizeTopLeft	= rcTmp;

	// top right
	rcTmp.top		= rc.top;
	rcTmp.left		= rc.right - RESIZE_MARGIN;
	rcTmp.bottom	= rc.top + RESIZE_MARGIN;
	rcTmp.right		= rc.right;

	m_rcResizeTopRight	= rcTmp;

	// bottom
	rcTmp.top		= rc.bottom - RESIZE_MARGIN;
	rcTmp.left		= rc.left + RESIZE_MARGIN;
	rcTmp.bottom	= rc.bottom;
	rcTmp.right		= rc.right - RESIZE_MARGIN;

	m_rcResizeBottom	= rcTmp;

	// bottom left
	rcTmp.top		= rc.bottom - RESIZE_MARGIN;
	rcTmp.left		= rc.left;
	rcTmp.bottom	= rc.bottom;
	rcTmp.right		= rc.left + RESIZE_MARGIN;

	m_rcResizeBottomLeft	= rcTmp;

	// bottom right
	rcTmp.top		= rc.bottom - RESIZE_MARGIN;
	rcTmp.left		= rc.right - RESIZE_MARGIN;
	rcTmp.bottom	= rc.bottom;
	rcTmp.right		= rc.right;

	m_rcResizeBottomRight	= rcTmp;
}

/**************************************************************************
 * method CMainFrame::OnTrayShow
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-14
 *
 * @Description
 * Explorer ������ ���� Task bar ���� Tray�� ������� ������ �ذ��ϱ� ���� �̺�Ʈ �Լ�
 **************************************************************************/
void CMainFrame::OnTrayShow(WPARAM wParam, LPARAM lParam) 
{ 
    // TrayIcon�� �ٽ� �����ݴϴ�. ShowTray�� Tray�� �����ִ� �Լ��Դϴ�. 
	m_TrayIcon->ShowIcon();
} 

/**************************************************************************
 * method CMainFrame::OnAreYou
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-14
 *
 * @Description
 * �ܺο��� Fish���� Ȯ���� ���� �޽����� ������ ��� ���� �ϱ� ���� �̺�Ʈ �Լ�
 **************************************************************************/
LRESULT CMainFrame::OnAreYou(WPARAM, LPARAM)
{
	return UWM_ARE_YOU_FISH;
} 

/**************************************************************************
 * method CMainFrame::OnCopyData
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-14
 *
 * @Description
 * �ܺο��� �Ѿ�� �����͸� ó���Ѵ�.
 * �������� ������ OP_FISHDATA�� ��쿡�� Argument�� �Ǵ��Ͽ� ó���Ѵ�.
 **************************************************************************/
BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	if (pCopyDataStruct->dwData == OP_FISHDATA)
	{
		ShowWindow(SW_NORMAL);
		FlashWindow(true);
		SetForegroundWindow();

		// PROCESS arguments
		ProcessArguments((LPCTSTR) pCopyDataStruct->lpData);
	}
	
	return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

/**************************************************************************
 * method CMainFrame::OnTrayOpen
 *
 * written by aquarelle
 *
 * @history
 * created 2005-12-15
 *
 * @Description
 * Tray Menu �ڵ鷯 (Open)
 **************************************************************************/
void CMainFrame::OnTrayOpen()
{
	ActivateFrame(SW_SHOWNORMAL);
}

/**************************************************************************
 * method CMainFrame::OnTrayExit
 *
 * written by aquarelle
 *
 * @history
 * created 2005-12-15
 *
 * @Description
 * Tray Menu �ڵ鷯 (Exit)
 **************************************************************************/
void CMainFrame::OnTrayExit()
{
	CloseFrame();	
}


/**************************************************************************
 * method CMainFrame::GetTaskbarNotifier
 *
 * written by aquarelle
 *
 * @history
 * created 2005-12-23
 *
 * @Description
 * �˸�â ����Ʈ ������ ��ȯ
 **************************************************************************/
BOOL CMainFrame::GetTaskbarNotifier(SmartPtr<CTaskbarNotifier>& spnotifier)
{
	spnotifier = m_spNotifier;
	return TRUE;
}


/**************************************************************************
 * method CMainFrame::GetTaskbarNotifier
 *
 * written by aquarelle
 *
 * @history
 * created 2006-02-10
 *
 * @Description
 * Ʈ���̾����� ����Ʈ ������ ��ȯ
 **************************************************************************/
BOOL CMainFrame::GetTrayIcon(SmartPtr<CTray>& sptrayicon)
{
	sptrayicon = m_TrayIcon;
	return TRUE;
}


/**************************************************************************
 * method CMainFrame::OnNodeUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-10
 *
 * @Description
 * ���� ����� ������ ����Ǿ��� �� ȣ��ȴ�.
 **************************************************************************/
void CMainFrame::OnNodeUpdate(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("OnNodeUpdate node cnt[%d]\r\n"), wParam);
	if(!m_spSearch) return;

	m_spSearch->SetNetState((int) wParam);
    m_spSearch->ShowControls();
}

/**************************************************************************
 * method CMainFrame::OnProperties
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * ȯ�� ����
 **************************************************************************/
void CMainFrame::OnProperties() 
{
	CDlg_Properties prop;
	prop.DoModal();
}

/**************************************************************************
 * method CMainFrame::TryLogin
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-27
 *
 * @Description
 * �α��� â�� ���� �α��� �۾��� �����Ѵ�.
 **************************************************************************/
void CMainFrame::TryLogin()
{
	m_spLogin = SmartPtr<CDlg_Login> (new CDlg_Login);
	m_spLogin->Create(this);
	m_spLogin->m_hMsgHwnd = this->GetSafeHwnd();
	m_hLoginWnd = m_spLogin->GetSafeHwnd();
	m_spLogin->ShowWindow(SW_NORMAL);

}

/**************************************************************************
 * method CMainFrame::OnRequestLogin
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-27
 *
 * @Description
 * ���� IRC�� ���� �α��� �۾��� �����Ѵ�.
 **************************************************************************/
void CMainFrame::OnRequestLogin(WPARAM wParam, LPARAM lParam)
{
	// �ϴ� �� �ʸ� ó���Ѵ�.
	int err;

	theApp.m_spLP->SetOPMLConnected(FALSE);

	if(WebAuth(err))
	{
		theApp.m_spLP->SetOPMLConnected(TRUE);
		theApp.m_spLP->CopyTempIDTo();
		::SendMessage(m_hLoginWnd, WM_LOGINRESULT, (WPARAM) err, 0);
		return;
	}
	::SendMessage(m_hLoginWnd, WM_LOGINRESULT, (WPARAM) err, 0);
}

void CMainFrame::IrcdAuth()
{
	if(!m_spIrcd)
	{
		m_spIrcd = SmartPtr<CIrcdManager> (new CIrcdManager(GetSafeHwnd()));
		m_spIrcd->SetIP(IRCD_HOSTNAME);
		m_spIrcd->SetPort(IRCD_PORT);
		m_spIrcd->SetSearchViewHandle(m_spSearch->GetSafeHwnd());
		m_spIrcd->SetRandomID(TRUE);
	}

	m_spIrcd->Login();	
}

/**************************************************************************
 * method CMainFrame::WebAuth
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-27
 *
 * @Description
 * �� �������� ���� �α��� �۾��� �����Ѵ�.
 **************************************************************************/
BOOL CMainFrame::WebAuth(int& err)
{
	CMemFile SNList;
	TCHAR line[MAX_BUFF+1]={0};
	CString str = _T(""), rstr;
	BOOL breturn = FALSE;

	str.Format(_T("%s?cc=0001&ii=%s&pp=%s")
		, URL_WEBDB
		, theApp.m_spLP->GetTempUserID()
		, theApp.m_spLP->GetEncodedPassword());

	TRACE(_T("web auth : %s\r\n"), str);

	if( !GetHttpFileAsc( HOST_NAME, 
		80, (LPTSTR)((LPCTSTR )str), &SNList, NULL, NULL) )
	{
		// ERROR : URL, PORT, filename�� �ϳ��� ������
		err = ERR_WB_HOSTNOTFOUND;
		SNList.Close();
		return FALSE;
	}
	
	// ��û ���
	if( GetLine( &SNList, line) < 0)
	{
		// ERROR��
		err = ERR_WB_WRONGRESULT;
		SNList.Close();
		return FALSE;
	}

	int ret = 0;
	// ��� ���� �˻�
	BOOL ischeck = CheckResultCode( line , ret );
	if(ret == 1)
	{
		int userseq = 0;

		if( GetLine( &SNList, line) > 0)
		{
			userseq = _ttoi((LPTSTR) (LPCTSTR) line);
		}

		if(userseq > 0)
		{
			theApp.m_spLP->SetUserSeq(userseq);
			breturn = TRUE;
		}
	}
	else
	{
		switch(ret)
		{
		case 0:
			err = ERR_WB_WRONGPASS;
			break;
		case -1:
			err = ERR_WB_LOGINPARAMETER;
			break;
		case -2:
			err = ERR_WB_IDNOTEXIST;
			break;
		case -9:
			err = ERR_WB_DBCONN;
			break;
		default:
			err = ERR_WB_UNKNOWN;
		}
	}

	SNList.Close();
	return breturn;
}

void CMainFrame::OnLoginComplete(WPARAM wParam, LPARAM lParam)
{
	InitUser();
}

void CMainFrame::OnIrcdAuthComplete(WPARAM wParam, LPARAM lParam)
{
	if(wParam == ERR_NONE)
	{
		m_spSearch->EnableNetwork(TRUE);
		// ������ Seed Node ��û
		theApp.m_spGD->ReqSeedNodeList();

		// ���� ������ ����
		if(theApp.m_spGD->m_Control.StartListening() == false)
		{
			FishMessageBox(STR_ERR_GNUSOCK_FAILED);
			m_nGnuOpenTimer = SetTimer(TID_GNUOPEN, 1000, NULL);
		}
	}

	// TO DO
//	AfxMessageBox(IRC ������ ���´�.)
}

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawItems(&dc);

//	UpdateWindow();
	
	// Do not call CFrameWnd::OnPaint() for painting messages
}

CView* CMainFrame::GetFirstView()
{
	return m_pVI;
}

/**************************************************************************
 * method CMainFrame::OnGetNewBrowserPtr
 *
 * written by moonknit
 *
 * @History
 * created 2006-02-02
 *
 * @Description
 * ��â�� �����͸� ��ȯ�Ѵ�.
 *
 * @Return
 * (LRESULT) ��â�� ������, NULL �̸� ��â�� ���� �������� �ǹ��Ѵ�.
 **************************************************************************/
LRESULT CMainFrame::OnGetNewBrowserPtr(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT) GetNewWBIDispatch();
}

/**************************************************************************
 * method CMainFrame::OnNavigateNew
 *
 * written by moonknit
 *
 * @History
 * created 2006-02-02
 *
 * @Description
 * ��â�� ����� Ž���Ѵ�.
 *
 * @Parameters
 * (in WPARAM) wParam - URL ������ (LPCTSTR)
 *
 * @Return
 * (LRESULT) - ��â Ž�� ���� ����
 **************************************************************************/
LRESULT CMainFrame::OnNavigateNew(WPARAM wParam, LPARAM lParam)
{
	if(!wParam) return FALSE;

	NavigateNewWindow((LPCTSTR) wParam);

	return TRUE;
}

/**************************************************************************
 * method CMainFrame::OnScrap
 *
 * written by moonknit
 *
 * @History
 * created 2006-12-XX
 *
 * @Description
 * ����Ʈ�� ��ũ���Ѵ�.
 *
 * @Parameters
 * (in WPARAM) wParam - ����Ʈ ���̵�
 *
 * @Return
 **************************************************************************/
void CMainFrame::OnScrap(WPARAM wParam, LPARAM lParam)
{
//	int channelid = GetScrapChannel();
	if(!m_spSubscribe)  return;
    m_dwScrapPostID   =   wParam;

    CPoint ptContext;
    ::GetCursorPos(&ptContext);

    CScrapManager mgr(this);
    mgr.Init(this);
    mgr.ShowScrapMgr(ptContext);
}

/**************************************************************************
 * method CMainFrame::OnReadOn
 *
 * written by moonknit
 *
 * @History
 * created 2006-12-XX
 *
 * @Description
 * ����Ʈ�� ��ũ Ŭ���� ó���Ѵ�.
 *
 * @Parameters
 * (in WPARAM) wParam - ����Ʈ ���̵�
 *
 * @Return
 **************************************************************************/
void CMainFrame::OnReadOn(WPARAM wParam, LPARAM lParam)
{
	ReadOn((int) wParam);
}

/**************************************************************************
 * method CMainFrame::OnReadPost
 *
 * written by aquarelle
 *
 * @History
 * created 2006-01-31
 *
 * @Description
 * ���� IWebBrowser ���� �߻��� ReadPost ó�� : �ش� ID�� ����Ʈ �б� ��û
 **************************************************************************/
void CMainFrame::OnReadPost(WPARAM wParam, LPARAM lParam)
{
	SmartPtr<CDlg_RSSMain> spmain;
	GetDlgRSSMain(spmain);

	if(!spmain)
		return;

	spmain->m_spRSSList->RequestPostOpen((int) wParam);
}

void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(m_rccaption.PtInRect(point))
	{
		OnBtnMaximize();
		return;
	}

	CFrameWnd::OnLButtonDblClk(nFlags, point);
}


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(theApp.IsClosing()) return;

	if(nIDEvent == TID_MAINTIMER)
	{
		if(!m_spSubscribe)
		{}
		else
		{
			m_spSubscribe->Manage();
		}

		if(theApp.m_spLP->IsIRCConnected())
		{
			theApp.m_spGD->m_Control.Timer();
		}

		// local / remote search manager
		theApp.m_spSM->Manage();

		if(m_spIrcd != NULL)
			m_spIrcd->Timer();

		if(++m_nFlushTime > MAX_FLUSHTIME)
		{
			m_nFlushTime = 0;
			theApp.m_spRD->IFlush();
		}
	}
	else if(nIDEvent = TID_GNUOPEN)
	{
		KillTimer(nIDEvent);
		m_nGnuOpenTimer = NULL;

		if(theApp.m_spGD->m_Control.StartListening(TRUE) == false)
		{
			m_nGnuOpenTimer = SetTimer(TID_GNUOPEN, 1000, NULL);
		}
	}
    //////////////////////////////////////////////////////////////////////////
    // ä�� ��ũ�� �ν��ؼ� �׿� �´� ������ ���ϴ� �κ�
    else if(nIDEvent ==  TID_SUBSCRIPTION)
    {
        //////////////////////////////////////////////////////////////////////////
        // manual lock
        if ( m_bInTimer )   {
            KillTimer(TID_SUBSCRIPTION);
            return;
        }
        m_bInTimer  =   TRUE;

        //////////////////////////////////////////////////////////////////////////
        // prevent this function from calling iteratively
        KillTimer(TID_SUBSCRIPTION);

        while ( m_vecChannelLink.size() )       // don't use size() comparison function. in loop, the value is changed.
        {
            CFishRepItemPropertyDlg dlg;
            dlg.SetSubscribeDlg( m_spSubscribe.get() );
            
            if ( IDCANCEL    ==  dlg.DoModal( CHANNEL_ADD_DLG, m_vecChannelLink[0] ) )      {
                m_vecChannelLink.erase( m_vecChannelLink.begin() );
                continue;
            }
        
            DWORD iDATA = dlg.getTargetGroup();
        
            FISH_GROUP g(iDATA);
        
            if(! m_spSubscribe->FindGroup(g) )	// ���õ� �׷��� ���� ��� �߰���� �׷��� �ֻ��� �׷�����.. ���� by aquarelle
            {
                HTREEITEM hITEM = m_spSubscribe->m_treeRepository.GetRootItem();
                iDATA = m_spSubscribe->m_treeRepository.GetItemData(hITEM);
            }
        
            {
				BOOL bns = FALSE;
				int keepc = 0, keepd = 0;
                int seq = m_spSubscribe->AddSubscribeChannel( iDATA, dlg.getTitle(), dlg.getComment(), dlg.getFeedInterval()
					, dlg.getXmlUrl(), dlg.getHtmlUrl()
					, _T(""), keepc, keepd, bns);
                if( !seq )    m_spSubscribe->FetchByUser(seq);
                HTREEITEM hITEM =   m_spSubscribe->m_treeRepository.GetHandleFromData(seq);
                m_spSubscribe->m_treeRepository.Select(hITEM, TVGN_CARET);
            }

            m_vecChannelLink.erase( m_vecChannelLink.begin() );
        }

        m_spSubscribe->Invalidate();
        //////////////////////////////////////////////////////////////////////////
        // manual lock
        m_bInTimer  =   FALSE;
    }
	else if(nIDEvent == TID_EXIT)
	{
		KillTimer(nIDEvent);
		OnClose();
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}

/**************************************************************************
 * method CMainFrame::ProcessArguments
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-14
 * modified 2005-02-15 by eternalbleu
 *
 * @Description
 * Argument�� ó���Ͽ� ���� ������ �����Ѵ�.
 * ���� �����ڵ��� ���԰� ó�� ����
 **************************************************************************/
void CMainFrame::ProcessArguments(const LPCTSTR args)
{
	CString szMsg = args;
    szMsg.Replace(_T("\r"), _T("\0"));
    szMsg.Replace(_T("\n"), _T("\0"));
    szMsg.TrimLeft(_T(" "));
    szMsg.TrimRight(_T(" "));

    if ( szMsg.Find( _T("://"), 0 ) == -1 || szMsg   ==  _T("") ) 
        return;
    {
        //////////////////////////////////////////////////////////////////////////
        // FISH LINK PROCESS
        if ( szMsg.Find( _T("fish://"), 0 ) != -1 || szMsg.Find( _T("feed://"), 0) != -1 ) 
        {
            szMsg.Replace( _T("fish://"), _T("") );
            szMsg.Replace( _T("feed://"), _T("") );

            //////////////////////////////////////////////////////////////////////////
            // LOCKING
            m_vecChannelLink.push_back( szMsg );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // �������� �ʱ�ȭ �� ����
    SetTimer(TID_SUBSCRIPTION, 100, NULL);
}

/************************************************************************
OnUserreqGotoAddrbar    ������� ��û���� ��Ŀ���� �ּ�â���� �̵��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/17:CREATED
************************************************************************/
void CMainFrame::OnUserreqGotoAddrbar(WPARAM wParam, LPARAM lParam)
{
    ((CView_Information*)GetFirstView())->SetFocusOnAddrbar();
}

/**************************************************************************
 * CMainFrame::ChangeToListView
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-24
 *
 * @Description
 * View�� ������ List View�� �����Ѵ�.
 *
 * @Parameters
 * (in int) chid - List View�� ��Ÿ�� ä��
 **************************************************************************/
void CMainFrame::ChangeToListView(int chid)
{
    CView_Information* ptrView = (CView_Information*) GetFirstView();

	if (!ptrView) return;
	if (!ptrView->IsKindOf( RUNTIME_CLASS( CView_Information ))) return;

	ptrView->GotoMainlist();

	if(m_spSubscribe != NULL)
		m_spSubscribe->SelectChannel(chid);
}


void CMainFrame::OnCreateNewScrapMgr() 
{
	// TODO: Add your command handler code here
    m_spSubscribe->OnReposCmAddScrap();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
    switch(pMsg->message)
	{
	case WM_MOUSEWHEEL:
		{
#ifdef USE_SENSITIVEWHEEL
			HWND hDest;
/*			hDest = GetBrowserHwnd();
			if(hDest)
			{
				::SendMessage(hDest, pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
			*/
			hDest = GetOverHwnd();
			if(hDest)
			{
				::SendMessage(hDest, pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
#endif
		}
		break;
	case WM_MOUSEMOVE:
		{
/*			POINT pt;
			pt.x = LOWORD(pMsg->lParam);
			pt.y = HIWORD(pMsg->lParam);
			if(m_rcScreenWnd.PtInRect(pt))
			if(m_rcBrowser.PtInRect(pt))
			{
				if(GetOverHwnd() != m_hBrowserWnd) 
				{
					::SendMessage(m_hBrowserWnd, WM_WHEELENABLE, (WPARAM) TRUE, NULL);
					SetOverHwnd(m_hBrowserWnd);
				}
			}
			else if(GetOverHwnd() == m_hBrowserWnd)
			{
				SetOverHwnd(NULL);
				::SendMessage(m_hBrowserWnd, WM_WHEELENABLE, (WPARAM) FALSE, NULL);
			}
			*/
		}
		break;
	}


	return CFrameWnd::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{

	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}

void CMainFrame::OnCommandRange(UINT nID)
{
    //MessageBox(_T("TEST"));
    if (m_dwScrapPostID != -1)
    {
        Scrap(nID - ID_SCRAP_BASE_UID, (int) m_dwScrapPostID);
        m_dwScrapPostID   =   -1;
    }
}

void CMainFrame::StopNetwork()
{
	delete m_spIrcd.release();
	theApp.m_spGD->m_Control.StopListening();
}

void CMainFrame::StartNetwork()
{
	IrcdAuth();
}

void CMainFrame::OnEndSession(BOOL bEnding)
{
	if(bEnding)
		CloseFrame();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID == SC_MAXIMIZE || nID == SC_RESTORE)
	{
		OnBtnMaximize();
		return;
	}
	
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CFrameWnd::OnWindowPosChanged(lpwndpos);
}
