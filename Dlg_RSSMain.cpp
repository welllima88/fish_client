// Dlg_RSSMain.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "fish_common.h"
#include "Dlg_RSSMain.h"
#include "Dlg_Browser.h"
#include "Dlg_MessageBox.h"
#include "RSSCurrentList.h"
#include "LocalProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_CACHEPAGESEQ			5
#define MAX_CACHEFILESEQ			20

#define STR_ERR_DRM_HTMLTOOBIG		_T("생성된 HTML 페이지가 너무 큽니다.!!")

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSMain dialog

CDlg_RSSMain::CDlg_RSSMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_RSSMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_RSSMain)
	//}}AFX_DATA_INIT

	m_bInit			= FALSE;
	m_bViewListRequested	= FALSE;
	m_nViewTimer	= 0;
	m_bSelfDraw		= TRUE;
	m_bFirstView	= TRUE;

#ifdef USE_WBTMPFILE
	m_nCachePageSeq		= 0;
	m_nCacheFileSeq		= 0;
#endif
}

/************************************************************************

@param  : 
@return : 
@remark : 
@author : moonknit
@history:
    created at 2005/12/02: 
    modified 2005/12/02: 
        PostQuitMessage 가 FISHIEControl 에서 행해졌는데 이 것을 RSSMain 
        으로 이동함. 없애면 종료시 인스턴스사라지지 않음
************************************************************************/
CDlg_RSSMain::~CDlg_RSSMain()
{
	TRACE(_T("CDlg_RSSMain Destroy\r\n"));
	if(m_nViewTimer) KillTimer(m_nViewTimer);
	theApp.m_spCL->SetParentHandle(NULL);
}

void CDlg_RSSMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_RSSMain)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_RSSMain, CDialog)
	//{{AFX_MSG_MAP(CDlg_RSSMain)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_VIEW_CURRENTLIST, OnViewCurrentList)
	ON_MESSAGE(WM_VIEW_FIRSTPOST, OnViewFirstPost)
	ON_MESSAGE(WM_VIEW_LASTPOST, OnViewLastPost)
	ON_MESSAGE(WM_VIEW_NEXTPOSTS, OnViewNextPosts)
	ON_MESSAGE(WM_VIEW_PREVPOSTS, OnViewPrevPosts)
	ON_MESSAGE(WM_VIEW_HTML, OnViewHTML)
	ON_MESSAGE(WM_VIEW_REFRESH, OnViewRefresh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSMain message handlers
BOOL CDlg_RSSMain::Create(CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, pParentWnd);
}

void CDlg_RSSMain::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE(_T("CDlg_RSSMain::OnSize\r\n"));
	CDialog::OnSize(nType, cx, cy);

	if(m_splitter.GetSafeHwnd())
	{
		m_splitter.MoveWindow(0, 0, cx, cy);
	}

//	MoveControls();
}

#define HEIGHT_RSSLIST					200

static bool bInit = false;

void CDlg_RSSMain::MoveControls()
{
	if(!m_bInit) return;

	if(!m_spRSSList->m_hWnd || !m_spBrowser->m_hWnd)
		return;

	CRect clientRT, tmpClientRT, listRT;
	GetClientRect(&clientRT);

	m_spRSSList->GetClientRect(&listRT);
	tmpClientRT = clientRT;
	tmpClientRT.bottom = tmpClientRT.top + listRT.Height();
	m_spRSSList->MoveWindow(&tmpClientRT);

	{
		tmpClientRT.top = tmpClientRT.bottom;
		tmpClientRT.bottom = clientRT.bottom;
		m_spBrowser->MoveWindow(&tmpClientRT);

		bInit = true;
	}
}

BOOL CDlg_RSSMain::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_spRSSList = SmartPtr<CDlg_RSSList> (new CDlg_RSSList);
	m_spBrowser = SmartPtr<CDlg_Browser> (new CDlg_Browser);

	m_spRSSList->Create(this);
	m_spBrowser->Create(this);

	theApp.m_spCL->SetListDlg(m_spRSSList);
	theApp.m_spCL->SetParentHandle(this->GetSafeHwnd());

	m_spBrowser->SetLink2NewWindow(TRUE);			// link-click will make new window

	m_spBrowser->Navigate(_T("about:blank"));
//	m_spBrowser->Navigate( _T("http://www.3fishes.co.kr/") );
    
	m_bInit = TRUE;

	CRect rect;
	GetClientRect(&rect);

	m_splitter.Create(
		WS_CHILD|WS_VISIBLE|SS_HORIZ,
		this,
		m_spRSSList.get(),
		m_spBrowser.get(),
		IDC_SPLITTER_HORIZ_1,
		rect,
		100,
		300
	);

	m_spRSSList->ShowWindow(SW_NORMAL);
	m_spBrowser->ShowWindow(SW_NORMAL);
    MoveControls();
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlg_RSSMain::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		else if(pMsg->wParam == VK_RETURN)
		{
			// exploring
			return TRUE;
		}

/*		else if(::GetKeyState(VK_LCONTROL) < 0)
		{
			switch(pMsg->wParam)
			{
			case 'd':
			case 'D':
				CancelList();
				break;
			case 'a':
			case 'A':
				SelectAll();
				break;
			}
		}
		*/
		break;

	default:
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void* CDlg_RSSMain::GetBrowserPointer()
{
	return m_spBrowser.get();
}


void CDlg_RSSMain::DrawItems(CDC *pDC)
{
}

void CDlg_RSSMain::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
}

void CDlg_RSSMain::DrawOutline(CDC* pBufferDC)
{
}


BOOL CDlg_RSSMain::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
//    return CDialog::OnEraseBkgnd(pDC);
}

#define MAX_HTMLLENGTH						1048576  // 1Meg bytes

void CDlg_RSSMain::OnViewCurrentList(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_VIEWTIMER
	if(m_nViewTimer != 0)
	{
		m_bViewListRequested = TRUE;
		return;
	}
#endif

	CString html;
	if(!theApp.m_spCL->GetAllXMLStream(html))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);

#ifdef USE_VIEWTIMER
	m_bViewListRequested = FALSE;

	m_nViewTimer = SetTimer(TID_VIEWLIST, 200, NULL);
#endif
}

void CDlg_RSSMain::OnViewFirstPost(WPARAM wParam, LPARAM lParam)
{
	CString html;
	if(!theApp.m_spCL->GetFirstPost(html))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);
}

void CDlg_RSSMain::OnViewLastPost(WPARAM wParam, LPARAM lParam)
{
	CString html;
	if(!theApp.m_spCL->GetLastPost(html))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);
}

void CDlg_RSSMain::OnViewNextPosts(WPARAM wParam, LPARAM lParam)
{
	CString html;
	if(!theApp.m_spCL->GetNextPost(html, (int) wParam, (int) lParam))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);
}

void CDlg_RSSMain::OnViewPrevPosts(WPARAM wParam, LPARAM lParam)
{
	CString html;
	if(!theApp.m_spCL->GetPrevPost(html, (int) wParam, (int) lParam))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);
}

void CDlg_RSSMain::OnViewHTML(WPARAM wParam, LPARAM lParam)
{
}

void CDlg_RSSMain::OnViewRefresh(WPARAM wParam, LPARAM lParam)
{
	CString html;
	if(!theApp.m_spCL->Refresh(html))
	{
		// html 가져오기 실패
		return;
	}

	ViewPosts(html);
}

/**************************************************************************
 * method CDlg_RSSMain::ViewPosts
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * post를 이용해 만들어진 html을 보여준다.
 *
 * @Parameters
 * (in CString&) html - IE로 보여줄 html 스트링
 **************************************************************************/
void CDlg_RSSMain::ViewPosts(CString& html)
{
	if(!m_spBrowser) return;

	if(html.GetLength() > MAX_HTMLLENGTH)
	{
//		AfxMessageBox(STR_ERR_DRM_HTMLTOOBIG);
        FishMessageBox(STR_ERR_DRM_HTMLTOOBIG);
	}

#ifdef USE_WBTMPFILE
	// using temporary file
	CString filename;
	filename.Format(_T("%sposttemp_%02d.html"), theApp.m_spLP->GetTempDataPath(), m_nCacheFileSeq++);

	if(m_nCacheFileSeq > MAX_CACHEFILESEQ) m_nCacheFileSeq = 0;

	if(MakeStreamToFile(filename, html))
	{
		m_spBrowser->SetTempBypass(TRUE);
		m_spBrowser->Navigate(filename);
	}
#else
	m_spBrowser->LoadHTML(html);
#endif
}

void CDlg_RSSMain::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CDialog::OnPaint() for painting messages
}


void CDlg_RSSMain::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TID_VIEWLIST)
	{
		KillTimer(nIDEvent);
		m_nViewTimer = 0;
		if(m_bViewListRequested)
		{
			OnViewCurrentList(NULL, NULL);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlg_RSSMain::SetSubscribeHwnd(HWND h)
{
	m_spBrowser->SetInformWnd(h);
}