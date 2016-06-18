// Dlg_Browser.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "MainFrm.h"
#include "View_Information.h"
#include "Dlg_ExplorerBar.h"
#include "Dlg_Browser.h"
#include "View_Information.h"
#include "FISH_def.h"
#include "strsafe.h"

//#include <Atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define USE_HTMLDOC

#define	TI_LOADHTML							1
#define TI_MOUSELEAVE						2
#define TI_LINKTIMER						3

#define OVERFLOW_SIZE						10485760			// 10 meg

#define NSTEP_DIV							7
#define	MIN_SCROLL							1

/////////////////////////////////////////////////////////////////////////////
// CDlg_Browser dialog


CDlg_Browser::CDlg_Browser(CWnd* pParent /*=NULL*/, DWORD dwSeq  /*=   0*/)
	: CDialog(CDlg_Browser::IDD, pParent), m_bLink2NewWindow(FALSE)
{
	//{{AFX_DATA_INIT(CDlg_Browser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInit			= FALSE;
	m_bSelfDraw		= TRUE;
    m_parent        =   NULL;
    m_dwBrowserSeq  = dwSeq;

	m_bLink2NewWindow	= FALSE;
	m_bTempBypass	= FALSE;
	m_bLinkClicked	= FALSE;
	m_bPopup		= FALSE;

	m_nProgress		= 0;
	m_nProgressMax	= 0;
	m_bClose		= FALSE;
	m_nMouseTimer	= NULL;
	m_nHtmlTimer	= NULL;
	m_nLinkTimer	= NULL;
}

CDlg_Browser::~CDlg_Browser()
{
	TRACE(_T("CDlg_Browser Destroy\r\n"));
	if(m_hMainWnd) ::SendMessage(m_hMainWnd, WM_SETFOCUS, 0, 0);

	m_bClose		= TRUE;
}


void CDlg_Browser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Browser)
	DDX_Control(pDX, IDC_EXPLORER, m_Browser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Browser, CDialog)
	//{{AFX_MSG_MAP(CDlg_Browser)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMMANDSTATECHANGE, OnCommandStateChange)
    ON_MESSAGE(WM_STOPBUTTONCHANGE, OnStopExchange)
	ON_MESSAGE(WM_URLCHANGED, OnURLChanged)
	ON_MESSAGE(WM_TITLECHANGED, OnTitleChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Browser message handlers

BOOL CDlg_Browser::Create(CWnd* pParentWnd, DWORD dwSeq  /*=   0*/) 
{
	// TODO: Add your specialized code here and/or call the base class
    m_dwBrowserSeq  =   dwSeq;
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CDlg_Browser::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_hInformWnd = NULL;

	ModifyStyle(0, WS_CLIPCHILDREN);

	CWnd* pWnd = AfxGetMainWnd();
	if(!pWnd) return FALSE;
	m_hMainWnd = pWnd->GetSafeHwnd();

	Navigate(_T("about:blank"));
	m_Browser.SetSilent(TRUE);
	m_Browser.SetRegisterAsBrowser(TRUE);
	m_Browser.SetRegisterAsDropTarget(TRUE);

	CRect rect;
	GetClientRect(&rect);
	m_Browser.MoveWindow(&rect);

	ShowWindow(SW_HIDE);

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Browser::OnSize(UINT nType, int cx, int cy) 
{
	if(m_Browser)
	{
		CRect rect;
		GetClientRect(&rect);
//		m_Browser.MoveWindow(&rect);
		m_Browser.SetWindowPos(NULL,rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOSENDCHANGING);

		GetWindowRect(&m_rcMain);

//		Invalidate();
	}
}

/**************************************************************************
 * method CDlg_Browser::Navigate
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-28
 *
 * @Description
 * 내장 IWebBrowser 를 이용한 Navigate
 *
 * @Parameters
 * (in CString) url - Navigate할 대상 URL
 *
 * @return
 **************************************************************************/
void CDlg_Browser::Navigate(CString url)
{
	if(m_Browser)
	{
		COleVariant vtEmpty;
		COleVariant comurl(url);
		m_Browser.Navigate2(comurl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
	}
}

/*************************************************************************
 * method CDlg_Browser::GetWBIDispatch
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-28
 *
 * @Description
 * 내장 IWebBrowser 객체의 DISPATCH 포인터를 반환한다.
 *
 * @Parameters
 *
 * @return
 * (LPDISPATCH) - IWebBrowser의 DISPATCH 포인터
 *				획득 실패시 NULL이 반환된다.
 **************************************************************************/
LPDISPATCH  CDlg_Browser::GetWBIDispatch()
{
	return m_Browser.GetApplication();
}

void CDlg_Browser::DrawItems(CDC *pDC)
{
}

void CDlg_Browser::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
}

void CDlg_Browser::DrawOutline(CDC* pBufferDC)
{
}


BOOL CDlg_Browser::OnEraseBkgnd(CDC* pDC) 
{
	CRect clip;
	GetClientRect(&clip);
//	m_Browser.GetClientRect(&clip);
//	ScreenToClient(&clip);
//	pDC->ExcludeClipRect(&clip);
	pDC->FillSolidRect(&clip, RGB(255, 255, 255));
	if(m_bSelfDraw)
	{
//		DrawItems(pDC);
		return FALSE;
	}

	return CDialog::OnEraseBkgnd(pDC);
}

/**************************************************************************
 * method CMainFrame::LoadHTML
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-29
 *
 * @Description
 * IE Control 에 HTML 내용올려줄 것을 요청한다.
 *
 * @Parameters
 * (int CString) html - IE Control에 올릴 HTML
 *
 * @return
 **************************************************************************/
void CDlg_Browser::LoadHTML(CString html)
{
#ifdef USE_HTMLDOC
//	KillTimer(TI_LOADHTML);
	int len = html.GetLength();
	if(len > 0 && len < OVERFLOW_SIZE)
	{
		m_strHTML = html;
		//m_nHtmlTimer = SetTimer(TI_LOADHTML, 100, NULL);
		LoadHTMLDoc();
	}
#endif
	m_hPrevFocus = ::GetFocus();
}

void CDlg_Browser::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TI_LOADHTML)
	{
		KillTimer(TI_LOADHTML);
		m_nHtmlTimer = NULL;

		if(m_Browser.GetBusy())
			m_nHtmlTimer = SetTimer(TI_LOADHTML, 100, NULL);
		else
			LoadHTMLDoc();
	}
	else if(nIDEvent == TI_LINKTIMER)
	{
		KillTimer(nIDEvent);
		m_nLinkTimer = NULL;
		m_bLinkClicked = FALSE;
	}
	else if(nIDEvent == TI_MOUSELEAVE)
	{
		POINT pt;
		BOOL toCheck = !GetCursorPos(&pt);

		if(m_rcMain.PtInRect(pt) && (GetBrowserHwnd() == GetSafeHwnd()) )
		{
		}
		else
		{
			::SetFocus(m_hPrevFocus);
			SetBrowserHwnd(NULL);
			KillTimer(TI_MOUSELEAVE);
			m_nMouseTimer = NULL;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

/**************************************************************************
 * method CMainFrame::LoadHTMLDoc
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-29
 *
 * @Description
 * IE Control 에 HTML 내용을 업로드한다.
 *
 * @Parameters
 * (int CString) html - IE Control에 올릴 HTML
 **************************************************************************/
void CDlg_Browser::LoadHTMLDoc()
{
/*
    HRESULT hr;
    IUnknown* pUnkBrowser = NULL;
    IUnknown* pUnkDisp = NULL;
    IStream* pStream = NULL;
    HGLOBAL hHTMLText;
	IHTMLDocument2* pHtmlDoc = NULL;
	IPersistStreamInit* pPersistStreamInit = NULL;

	if(!m_Browser) return;

	LPDISPATCH lpdoc;
	lpdoc = m_Browser.GetDocument();

	if(!lpdoc) return;

	IWebBrowser* m_pBrowser = (IWebBrowser*) m_Browser.GetApplication();

	// This is the DocumentComplete event for the top 
	//   frame - page is loaded!
	// Create a stream containing the HTML.
	// Alternatively, this stream may have been passed to us.

	unsigned int cchLength = (m_strHTML.GetLength() + 1) * sizeof(TCHAR);
	//  TODO: Safely determine the length of szHTMLText in TCHAR.
	hHTMLText = GlobalAlloc( GPTR, cchLength);
	
	if ( hHTMLText )
	{
		StringCchCopy((TCHAR*)hHTMLText, cchLength, m_strHTML);
		//  TODO: Add error handling code here.

		hr = CreateStreamOnHGlobal( hHTMLText, TRUE, &pStream );
		if ( SUCCEEDED(hr) )
		{
			IHTMLDocument2Ptr spDoc(lpdoc);
			// Query for IPersistStreamInit.
			hr = spDoc->QueryInterface( IID_IPersistStreamInit,  (void**)&pPersistStreamInit );
			if ( SUCCEEDED(hr) )
			{
				// Initialize the document.
				hr = pPersistStreamInit->InitNew();
				if ( SUCCEEDED(hr) )
				{
					// Load the contents of the stream.
					hr = pPersistStreamInit->Load( pStream );
				}
				pPersistStreamInit->Release();
			}
			spDoc->close();

			pStream->Release();
		}
		GlobalFree( hHTMLText );
	}
	*/
	

	LPDISPATCH lpdoc;
	lpdoc = m_Browser.GetDocument();

	if(!lpdoc)
	{
		return;
	}

	try
	{
		SetDeactivate();

		IHTMLDocument2Ptr spDoc(lpdoc);

		SAFEARRAY *sfArray = NULL;
		VARIANT *param = NULL;
		HRESULT hr = NULL;

		_bstr_t tmp(m_strHTML);
		BSTR bstr = tmp.copy(); 

		sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);

//		TRACE(_T("Safe Array Create\r\n"));

		if (sfArray != NULL) 
		{
			hr = SafeArrayAccessData(sfArray,(LPVOID*) & param);
			param->vt = VT_BSTR;
			param->bstrVal = bstr;
			hr = SafeArrayUnaccessData(sfArray);

			// 이해할 수 없음... designMode를 on/off 해줌으로써 hash link가 정상 작동함
			spDoc->put_designMode(CComBSTR("on"));	
			spDoc->writeln(sfArray);
			spDoc->put_designMode(CComBSTR("off")); 
			spDoc->close();

//			spDoc->put_title(_bstr_t(_T("fishes")));
//			spDoc->put_charset(_bstr_t(_T("UTF-16")));

		}

		SysFreeString(bstr);
		if (sfArray != NULL) {
			SafeArrayAccessData(sfArray,(LPVOID*) & param);
			SafeArrayDestroy(sfArray);
			SafeArrayUnaccessData(sfArray);
		}

		m_strHTML = _T("");

		if(m_hInformWnd) ::SendMessage(m_hInformWnd, WM_HTMLLOADCOMPLETE, NULL, NULL);

	}
	catch(CException&)
	{
	}
}

BOOL CDlg_Browser::WaitUntilInit()
{
	while(!m_bInit)
	{
		Sleep(100);
	}

	return TRUE;
}

/*
typedef enum CommandStateChangeConstants {
    CSC_UPDATECOMMANDS = 0xFFFFFFFF,
    CSC_NAVIGATEFORWARD = 0x00000001,
    CSC_NAVIGATEBACK = 0x00000002
} CommandStateChangeConstants;

Constants

CSC_UPDATECOMMANDS
Enabled state of a toolbar button might have changed.

CSC_NAVIGATEFORWARD
Enabled state of the Forward button has changed.

CSC_NAVIGATEBACK
Enabled state of the Back button has changed.

  [in] VARIANT_BOOL value that specifies the enabled state.
VARIANT_FALSE
Command is disabled.
VARIANT_TRUE
Command is enabled.

void CommandStateChange(
    long Command,
    VARIANT_BOOL Enable
);


*/

/************************************************************************
OnCommandStateChange    IE CONTROL 에서 발생하는 BACK, FORWARD 정보를 처리한다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/02: 
	updated at 2006/01/20 :: ExplorerBar와 연동 by moonknit
************************************************************************/
void CDlg_Browser::OnCommandStateChange(WPARAM wParam, LPARAM lParam)
{
    // VALIDATION CHECK
    if (m_parent == NULL || !m_parent->m_spExplorerBar) 
	{
		return;
	}

	BOOL bLinked = FALSE;
	if(m_parent->GetSelectedBrowser() == this)
		bLinked = TRUE;

    switch ( (CommandStateChangeConstants) wParam  )
    {
    case CSC_NAVIGATEBACK:
        m_vbBackAvailable    =   (VARIANT_BOOL)lParam;
		if(bLinked)
			m_parent->m_spExplorerBar->btnBACK_EnableWindow((BOOL) lParam);
        break;
    case CSC_NAVIGATEFORWARD:
        m_vbForwardAvailable =   (VARIANT_BOOL)lParam;
		if(bLinked)
			m_parent->m_spExplorerBar->btnFORWARD_EnableWindow((BOOL) lParam);
        break;
    case CSC_UPDATECOMMANDS:
        // NOT USED
        break;
    default:
        break;
    }
    return;
}

/*
StopButtonState
wParam = STOP BUTTON ENABLE STATE (BOOLEAN)
*/
void CDlg_Browser::OnStopExchange(WPARAM wParam, LPARAM lParam)
{
    // VALIDATION CHECK
    if (m_parent == NULL || !m_parent->m_spExplorerBar) 
	{
		return;
	}

    m_parent->m_spExplorerBar->btnSTOP_EnableWindow( (BOOL) wParam );
//    if ( (BOOL) wParam == FALSE )   m_parent->m_spExplorerBar->AddURL();

    return;
}

void CDlg_Browser::OnURLChanged(WPARAM wParam, LPARAM lParam)
{
	CString sOldURL = m_strURL;

    m_strURL	=	m_Browser.GetLocationURL();

	// if current url is not the one received, change to it
//	TRACE(_T("cur url : %s :: dest url : %s\r\n"), sOldURL, m_strURL);

	// url 의 변경이 없다면 쓸데없는 고생은 하지 않는다.
	if(m_strURL == sOldURL) return;

    // VALIDATION CHECK
    if (m_parent == NULL || !m_parent->m_spExplorerBar) 
	{
		return;
	}

	if( m_parent->GetSelectedBrowser() == this )
	{
        int idx = m_parent->m_spExplorerBar->FindStringExact(-1, m_strURL);
        if ( idx == CB_ERR )    m_parent->m_spExplorerBar->AddURL();

		m_parent->m_spExplorerBar->UpdateState(m_strURL);
	}
}

void CDlg_Browser::OnTitleChanged(WPARAM wParam, LPARAM lParam)
{
    enum {  MAX_TITLE_LENGTH    =   24, };
	TCHAR* lpszTitle = (TCHAR*) wParam;

    if(!lpszTitle || m_dwBrowserSeq == MAX_NUM) {
        return;
    }
    
    CString szTitle(lpszTitle);
    if (szTitle.GetLength() >= MAX_TITLE_LENGTH)
    {
        szTitle =   CString( szTitle.GetBufferSetLength(MAX_TITLE_LENGTH-3) );
        szTitle += _T("...");
    }

    int nCount = m_parent->m_ctrlCustomTab.GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        DWORD dwData;
        m_parent->m_ctrlCustomTab.GetItemData(i, dwData);
        if (dwData == m_dwBrowserSeq)
        {
            m_parent->m_ctrlCustomTab.SetItemText( i, szTitle );
            break;
        }
    }
}


const CString   CDlg_Browser::GetURL()    const
{
    return m_strURL;
}

/**************************************************************************
 * method CDlg_Browser::WBStop
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-14
 *
 * @Description
 * 내장 IWebBrowser 를 이용해 Stop 
 **************************************************************************/
void CDlg_Browser::WBStop()
{
	if(m_Browser) m_Browser.Stop();
}

/**************************************************************************
 * method CDlg_Browser::WBGoForward
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-14
 *
 * @Description
 * 내장 IWebBrowser 를 이용해 Go Forward 
 **************************************************************************/
void CDlg_Browser::WBGoForward()
{
	if(m_Browser) m_Browser.GoForward();
}

/**************************************************************************
 * method CDlg_Browser::WBGoBack
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-14
 *
 * @Description
 * 내장 IWebBrowser 를 이용해 Go Back
 **************************************************************************/
void CDlg_Browser::WBGoBack()
{
	if(m_Browser) m_Browser.GoBack();
}

/**************************************************************************
 * method CDlg_Browser::WBRefresh
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-14
 *
 * @Description
 * 내장 IWebBrowser 를 이용해 Refresh
 **************************************************************************/
void CDlg_Browser::WBRefresh()
{
	if(m_Browser) m_Browser.Refresh();
}

void CDlg_Browser::SetLink2NewWindow(BOOL bnew)
{
	m_bLink2NewWindow = bnew;
}

void CDlg_Browser::OnOK()
{
}

void CDlg_Browser::OnCancel()
{
}

void CDlg_Browser::SetTempBypass(BOOL bpass)
{
	m_bTempBypass = bpass;
}

void CDlg_Browser::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}

BEGIN_EVENTSINK_MAP(CDlg_Browser, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlg_Browser)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 105 /* CommandStateChange */, OnCommandStateChangeExplorer, VTS_I4 VTS_BOOL)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 251 /* NewWindow2 */, OnNewWindow2Explorer, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 259 /* DocumentComplete */, OnDocumentCompleteExplorer, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 106 /* DownloadBegin */, OnDownloadBeginExplorer, VTS_NONE)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 104 /* DownloadComplete */, OnDownloadCompleteExplorer, VTS_NONE)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 113 /* TitleChange */, OnTitleChangeExplorer, VTS_BSTR)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 252 /* NavigateComplete2 */, OnNavigateComplete2Explorer, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 250 /* BeforeNavigate2 */, OnBeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 108 /* ProgressChange */, OnProgressChangeExplorer, VTS_I4 VTS_I4)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 102 /* StatusTextChange */, OnStatusTextChangeExplorer, VTS_BSTR)
	ON_EVENT(CDlg_Browser, IDC_EXPLORER, 262 /* WindowSetResizable */, OnWindowSetResizableExplorer, VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlg_Browser::OnCommandStateChangeExplorer(long Command, BOOL Enable) 
{
	OnCommandStateChange(Command, Enable);
	
}

void CDlg_Browser::OnNewWindow2Explorer(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel) 
{
	CString selectedURL = GetSelectedLink();

	if((!m_bLinkClicked && selectedURL.IsEmpty()) || m_bClose)
	{
		*Cancel = TRUE;
	}
	else
	{
		BOOL bPopup = m_bPopup;

		m_bPopup = FALSE;
		m_bLinkClicked = FALSE;

		if(bPopup)
		{
			return;
		}

		if(m_hMainWnd)
		{
			LPDISPATCH lpdispatch = (LPDISPATCH) ::SendMessage(m_hMainWnd, WM_GETNEWBROWSERPTR, 0, 0);

			if(lpdispatch)
			{
				*ppDisp = lpdispatch;
			}
		}
	}
}

void CDlg_Browser::OnDocumentCompleteExplorer(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
//	m_bLinkClicked = TRUE;	
}

void CDlg_Browser::OnDownloadBeginExplorer() 
{
    OnStopExchange(TRUE, NULL);
//	m_bLinkClicked = FALSE;	
}

void CDlg_Browser::OnDownloadCompleteExplorer() 
{
	OnStopExchange(FALSE, NULL);
//	m_bLinkClicked = TRUE;

}

void CDlg_Browser::OnTitleChangeExplorer(LPCTSTR Text) 
{
	OnTitleChanged((WPARAM) Text, NULL);	
}

void CDlg_Browser::OnNavigateComplete2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	OnURLChanged(NULL, NULL);
}

void CDlg_Browser::OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
    /// 현재 포커스를 가져온다.
	CComBSTR bstrURL(URL->bstrVal);

	// 자바스크립트 링크 체크
	if(!m_bLink2NewWindow || m_bTempBypass)
	{
		if(m_bTempBypass)
			m_bTempBypass = FALSE;
	}
	// post view page 이거나 ??
	else
	{
		// navigate by new window
		if(bstrURL == _T("about:blank")
			// post view page에서 javascript를 사용하면 new link 처리를 하지 않는다.
			|| (_tcsncicmp(bstrURL, _T("javascript:"), 11) == 0 
			&& m_bLink2NewWindow
			)
			)
		{
		}
		else
		{
			// cancel current navigation
			*Cancel = TRUE;

			if(m_hMainWnd)
			{
				_bstr_t tmp( URL->bstrVal, FALSE ); //should use FALSE to fix memory problem
				::SendMessage(m_hMainWnd, WM_NAVIGATENEW, (WPARAM) (LPCTSTR) (LPTSTR) tmp, 0);
			}
		}
	}
}

LRESULT CDlg_Browser::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_NCPAINT)
	{
		return 0;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlg_Browser::OnProgressChangeExplorer(long Progress, long ProgressMax) 
{
	if((m_nProgress == Progress && m_nProgressMax == ProgressMax)
		|| Progress == -1 || ProgressMax <= 0 
		) return;

	m_nProgress = Progress;
	m_nProgressMax = ProgressMax;
}

BOOL CDlg_Browser::PreTranslateMessage(MSG* pMsg) 
{
    //////////////////////////////////////////////////////////////////////////
    // ALT + D EVENT PROCESS
    if (GetKeyState(VK_MENU) & KF_UP)
    {
        if(pMsg->message == WM_SYSKEYDOWN && GetAsyncKeyState(0x44) < 0)
        {
            //MessageBox(_T("ALT+D Button Down"), _T("GOTO ADDR"), /*dwType*/MB_ICONERROR);
            ::SendMessage(m_hMainWnd, WM_USERREQ_GOTO_ADDRBAR, 0, 0);
        }
    }

#ifdef USE_SENSITIVEWHEEL
	switch (pMsg->message)
	{
	case WM_LBUTTONUP:
		m_bLinkClicked = TRUE;
		m_nLinkTimer = SetTimer(TI_LINKTIMER, 100, NULL);
		break;
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_RETURN)
		{
			m_bLinkClicked = TRUE;
			m_nLinkTimer = SetTimer(TI_LINKTIMER, 100, NULL);
		}
		break;
	case WM_MOUSEMOVE:
		if(m_bInit)
		{
			HWND hWnd = GetSafeHwnd();
			if(
				(GetBrowserHwnd() != hWnd)
				&& !m_nMouseTimer
				&& GetForegroundWindow() == AfxGetMainWnd()
				)
			{
				SetBrowserFocus();
				SetBrowserHwnd(hWnd);
				m_nMouseTimer = SetTimer(TI_MOUSELEAVE, 100, NULL);
			}
		}
		break;
	}
#endif

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlg_Browser::OnStatusTextChangeExplorer(LPCTSTR Text) 
{
	// TODO: Add your control notification handler code here
//	TRACE(_T("STATUS TEXT CHANGED : %s\r\n"), Text);
	CMainFrame* pMain = (CMainFrame*) AfxGetMainWnd();
	if(!pMain) return;
    CView_Information* pView    =   ((CView_Information*)(pMain)->GetFirstView());
    if ( pView && pView->m_dlgNavigationPane.m_hWnd )
        pView->m_dlgNavigationPane.UpdateStatusBar(Text);
    
}

void CDlg_Browser::SetDeactivate(BOOL bDeactive)
{
	LPDISPATCH pApplication = m_Browser.GetApplication();
	if(!pApplication) return;

	IOleObject* pIOO;
	HRESULT hr = pApplication->QueryInterface(IID_IOleObject, (void**)&pIOO);
	if (SUCCEEDED(hr))
	{
		IOleClientSite* pIOCS;
		hr = pIOO->GetClientSite((IOleClientSite**) &pIOCS);
		if(SUCCEEDED(hr))
		{
			CRect rect;
			GetClientRect(&rect);
			LONG iVerb = 0;
			if(bDeactive)
				iVerb = OLEIVERB_INPLACEACTIVATE;
			else
				iVerb = OLEIVERB_UIACTIVATE;

			pIOO->DoVerb(iVerb, NULL, pIOCS, 0, m_hWnd, &rect);

			pIOCS->Release();
		}

		pIOO->Release();
	}
}

void CDlg_Browser::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if(bShow)
	{
		CRect rect, rcBrowser;
		GetClientRect(&rect);

		m_Browser.GetClientRect(&rcBrowser);
		
		if(rect != rcBrowser)
			m_Browser.SetWindowPos(NULL,rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOSENDCHANGING);

	}

	CDialog::OnShowWindow(bShow, nStatus);
}

void CDlg_Browser::OnWindowSetResizableExplorer(BOOL Resizable) 
{
}

LRESULT CDlg_Browser::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
/*
#ifdef USE_SENSITIVEWHEEL
	switch (message)
	{
	case WM_MOUSEWHEEL:
		if(m_Browser && (GetBrowserHwnd() == GetSafeHwnd()))
		{
//			::SendMessage(GetBrowserHwnd(), message, wParam, lParam);
//			ScrollBrowser((short) HIWORD(wParam));
			return TRUE;
		}
		break;
	}
#endif
*/	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

/*
	from http://www.codeproject.com/miscctrl/scrollbrowser.asp

    //
    // All this code does is what
    // "m_browser.Document.Body.ScrollTop = 100;"
    // does in VB. Gotta love COM in C++.
    //

    // let's say m_browser is the WebBrowser's member variable.

    HRESULT hr;

    // get the document dispatch from browser
    IDispatch *pDisp = m_browser.GetDocument();
    ASSERT( pDisp ); //if NULL, we failed
    
    // get document interface
    IHTMLDocument2 *pDocument = NULL;
    hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
    ASSERT( SUCCEEDED( hr ) );
    ASSERT( pDocument );

    //
    // this is the trick! 
    // take the body element from document...
    //
    IHTMLElement *pBody = NULL;
    hr = pDocument->get_body( &pBody );
    ASSERT( SUCCEEDED( hr ) );
    ASSERT( pBody );

    // from body we can get element2 interface,
    // which allows us to do scrolling
    IHTMLElement2 *pElement = NULL;
    hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement);
    ASSERT(SUCCEEDED(hr));
    ASSERT( pElement );

    // now we are ready to scroll
    // scroll down to 100th pixel from top
    pElement->put_scrollTop( 100 ); 
    
    // try to get the whole page size - but the returned number
    // is not allways correct. especially with pages that use dynamic html
    // tricks...
    long scroll_height; 
    pElement->get_scrollHeight( &s );

    // we can use this workaround!
    long real_scroll_height;
    pElement->put_scrollTop( 20000000 ); // ask to scroll really far down...
    pElement->get_scrollTop( &real_scroll_height );
    real_scroll_height += window_height; // will return the scroll height
    // for the first visible pixel, to get whole html page size must
    // add the window's height... (to obtain window_height is
    // left as an exercise for the reader)


    // print to debug output
    TRACE( "real scroll height: %ld, get_scrollHeight: %ld\n", 
                     real_scroll_height, scroll_height );
*/

void CDlg_Browser::ScrollBrowser(short nDelta)
{
	
#ifdef USE_HTML_EXTENDED

    // let's say m_browser is the WebBrowser's member variable.
    HRESULT hr;

    // get the document dispatch from browser
    IDispatch *pDisp = m_Browser.GetDocument();
	if(!pDisp) return;
    
    // get document interface
    IHTMLDocument2* pDocument = NULL;
    hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
	if(!SUCCEEDED( hr ) || !pDocument) return;

    //
    // this is the trick! 
    // take the body element from document...
    //
    IHTMLElement *pBody = NULL;
    hr = pDocument->get_body( &pBody );
    if(!SUCCEEDED( hr ) || !pBody ) return;

    // from body we can get element2 interface,
    // which allows us to do scrolling
    IHTMLElement2 *pElement = NULL;
    hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement);
    if(!SUCCEEDED(hr) || !pElement ) return;

	int nUnitDelta = (nDelta / WHEEL_DELTA) ;

    // now we are ready to scroll
    // scroll down to 100th pixel from top
	long destpixel = 0, scrollHeight = 0, nStep = 0, clientHeight = 0;
	pElement->get_clientHeight( &clientHeight );
	pElement->get_scrollHeight( &scrollHeight );
	if(clientHeight < scrollHeight)
	{
		nStep = clientHeight / NSTEP_DIV;
		if(nStep < MIN_SCROLL) nStep = MIN_SCROLL;
		pElement->get_scrollTop( &destpixel );
		destpixel -= nUnitDelta * nStep;
		pElement->put_scrollTop( destpixel ); 
		return;
	}

	long clientWidth = 0, scrollWidth = 0;
	pElement->get_clientWidth( &clientWidth );
	pElement->get_scrollWidth( &scrollWidth );

	if(clientWidth < scrollWidth)
	{
		nStep = clientWidth / NSTEP_DIV;
		if(nStep < MIN_SCROLL) nStep = MIN_SCROLL;

		pElement->get_scrollLeft( &destpixel );
		destpixel -= nUnitDelta * nStep;
		pElement->put_scrollLeft( destpixel ); 
	}
#endif
	
}

void CDlg_Browser::SetBrowserFocus()
{
#ifdef USE_HTML_EXTENDED
    HRESULT hr;

    // get the document dispatch from browser
    IDispatch *pDisp = m_Browser.GetDocument();
	if(!pDisp) return;
    
    // get document interface
    IHTMLDocument2 *pDocument = NULL;
    hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
	if(!SUCCEEDED( hr )  || !pDocument) return;

    //
    // this is the trick! 
    // take the body element from document...
    //
    IHTMLElement *pBody = NULL;
    hr = pDocument->get_body( &pBody );
    if(!SUCCEEDED( hr ) || !pBody ) return;

    // from body we can get element2 interface,
    // which allows us to do scrolling
    IHTMLElement2 *pElement = NULL;
    hr = pBody->QueryInterface(IID_IHTMLElement2,(void**)&pElement);
    if(!SUCCEEDED(hr) || !pElement ) return;

	HWND hWnd = ::GetFocus();
	m_hPrevFocus = hWnd;

	pElement->focus();
	
#endif
	  
}


CString CDlg_Browser::GetSelectedLink()
{
    // let's say m_browser is the WebBrowser's member variable.
    HRESULT hr;

    // get the document dispatch from browser
    IDispatch *pDisp = m_Browser.GetDocument();
	if(!pDisp) return _T("");
    
    // get document interface
    IHTMLDocument2* pDocument = NULL;
    hr = pDisp->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
	if(!SUCCEEDED( hr ) || !pDocument) return _T("");

    //
    // this is the trick! 
    // take the body element from document...
    //
    IHTMLElement *pElement = NULL;
    hr = pDocument->get_activeElement( &pElement );
    if(SUCCEEDED( hr ) && pElement )
	{
		BSTR bstr;
		pElement->get_tagName(&bstr);
		CString strTag;
		strTag = bstr;
		SysFreeString(bstr);

		if(SUCCEEDED( hr ) && (strTag == _T ("a") || strTag == _T ("A")) )
		{

			IHTMLAnchorElement * pAnchor = NULL;
			hr = pElement->QueryInterface
				(IID_IHTMLAnchorElement, (void**)&pAnchor);
                                
			if (SUCCEEDED (hr) && (pAnchor != NULL))
			{
				BSTR bstrURL;
				if (!FAILED (pAnchor->get_href(&bstrURL)))
				{
					CString url = bstrURL;
					SysFreeString (bstrURL);
					return url;
				}
				pAnchor->Release();
			}
		}

		pElement->Release();
	}

	return _T("");
}

HWND CDlg_Browser::GetBHwnd()
{
	HWND hWnd = NULL;
    HRESULT hr;

    // get the document dispatch from browser
 	IWebBrowser2* m_pBrowser = (IWebBrowser2*) m_Browser.GetApplication();
	if(!m_pBrowser) return NULL;
	hr = m_pBrowser->get_HWND((long*)&hWnd);
	if(SUCCEEDED(hr)) return hWnd;

	return NULL;
}