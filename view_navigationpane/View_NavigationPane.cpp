m_bBrowsing// View_InfoFunctionPane.cpp : implementation file
//

#include "stdafx.h"
#include "../fish.h"
#include "../fish_def.h"
#include "View_NavigationPane.h"
#include "../View_Information.h"
#include "../Dlg_RSSMain.h"
#include "../MainFrm.h"
#include "../LocalProperties.h"
#include "../Dlg_Style.h"
#include "../RSSCurrentList.h"
#include "../fish_common.h"
#include "../FocusManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CView_NavigationPane dialog

extern CFISHApp theApp;

CView_NavigationPane::CView_NavigationPane(CWnd* pParent /*=NULL*/)
	: CDialog(CView_NavigationPane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CView_NavigationPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_bSelfDraw     =   TRUE;
    m_nPostQuantity =   0;
    m_hStaticBkgnd  = CreateSolidBrush(VIEW_BKGND_COLOR);
    SetBrowseFlag();
    _initialize();
}

CView_NavigationPane::~CView_NavigationPane()
{
    _finalize();
	TRACE(_T("CView_NavigationPane Destroy\r\n"));
}

void CView_NavigationPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CView_NavigationPane)
	DDX_Control(pDX, IDC_ADDRESS_MSG, m_ctrlAddressMsg);
	DDX_Control(pDX, IDC_POST_BTN, m_btnPost);
	DDX_Control(pDX, IDC_BTN_LAST, m_btnLAST);
	DDX_Control(pDX, IDC_BTN_DBLNEXT, m_btnDBLNEXT);
	DDX_Control(pDX, IDC_BTN_NEXT, m_btnNEXT);
	DDX_Control(pDX, IDC_BTN_BACK, m_btnBACK);
	DDX_Control(pDX, IDC_BTN_DBLBACK, m_btnDBLBACK);
	DDX_Control(pDX, IDC_BTN_FIRST, m_btnFIRST);
	DDX_Control(pDX, IDC_VIEWLIST_FUNCTION_SUBSCRIBE, m_btnSubscribe);
	DDX_Control(pDX, IDC_VIEWLIST_FUNCTION_STYLE, m_btnStyle);
	DDX_Control(pDX, IDC_VIEWLIST_FUNCTION_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_VIEWLIST_FUNCTION_NEWTAB, m_btnNewTab);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CView_NavigationPane, CDialog)
	//{{AFX_MSG_MAP(CView_NavigationPane)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_FIRST, OnBtnViewFirst)
	ON_BN_CLICKED(IDC_BTN_LAST, OnBtnViewLast)
	ON_BN_CLICKED(IDC_BTN_NEXT, OnBtnViewNext)
	ON_BN_CLICKED(IDC_BTN_BACK, OnBtnViewPrev)
	ON_BN_CLICKED(IDC_BTN_DBLNEXT, OnBtnViewMultiNext)
	ON_BN_CLICKED(IDC_BTN_DBLBACK, OnBtnViewMultiPrev)
	ON_BN_CLICKED(IDC_VIEWLIST_FUNCTION_NEWTAB, OnBtnNewtab)
	ON_BN_CLICKED(IDC_VIEWLIST_FUNCTION_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_VIEWLIST_FUNCTION_STYLE, OnBtnStyle)
	ON_WM_SIZE()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_VIEWLIST_FUNCTION_SUBSCRIBE, OnBtnSubscribe)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_INFOPANE_UPDATE_POST_QUANTITY, OnUpdatePostQuantity)
	ON_MESSAGE(WM_INFORMCURRENTPOSTS, OnInformCurrentPosts)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CView_NavigationPane message handlers

BOOL CView_NavigationPane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
    {   // FUNCTION BTN INIT
        m_btnNewTab.SetShowText(FALSE);
        m_btnSave.SetShowText(FALSE);
        m_btnStyle.SetShowText(FALSE);
        m_btnSubscribe.SetShowText(FALSE);

        m_btnNewTab.LoadBitmaps(IDB_VIEWINFO_BTN_NEWTAB_N, IDB_VIEWINFO_BTN_NEWTAB_P, IDB_VIEWINFO_BTN_NEWTAB_O);
        m_btnSave.LoadBitmaps(IDB_VIEWINFO_BTN_SAVE_N, IDB_VIEWINFO_BTN_SAVE_P, IDB_VIEWINFO_BTN_SAVE_O);
        m_btnStyle.LoadBitmaps(IDB_VIEWINFO_BTN_STYLE_N, IDB_VIEWINFO_BTN_STYLE_P, IDB_VIEWINFO_BTN_STYLE_O);
        m_btnSubscribe.LoadBitmaps(IDB_VIEWINFO_BTN_SUBSCRIBE_N, IDB_VIEWINFO_BTN_SUBSCRIBE_P, IDB_VIEWINFO_BTN_SUBSCRIBE_O);

        m_btnNewTab.SetSize(NEWTAB_BTN_WIDTH, NEWTAB_BTN_HEIGHT);
        m_btnSave.SetSize(SAVE_BTN_WIDTH, SAVE_BTN_HEIGHT);
        m_btnStyle.SetSize(STYLE_BTN_WIDTH, STYLE_BTN_HEIGHT);
        m_btnSubscribe.SetSize(SUBSCRIBE_BTN_WIDTH, SUBSCRIBE_BTN_HEIGHT);
    }

    {   // NAVIGATION BTN INIT
	    m_btnFIRST.SetShowText(FALSE);
	    m_btnDBLBACK.SetShowText(FALSE);
	    m_btnBACK.SetShowText(FALSE);
	    m_btnNEXT.SetShowText(FALSE);
	    m_btnDBLNEXT.SetShowText(FALSE);
	    m_btnLAST.SetShowText(FALSE);
	    m_btnPost.SetShowText(TRUE);
        m_btnPost.SetTextBkMode(TRANSPARENT);
        m_btnPost.SetFont(&m_ftDefault);
        m_btnPost.DrawBorder(FALSE);

	    m_btnFIRST.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_BACK2_N, IDB_VIEWINFO_NAV_BTN_BACK2_P, IDB_VIEWINFO_NAV_BTN_BACK2_O);
	    m_btnDBLBACK.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_BACK3_N, IDB_VIEWINFO_NAV_BTN_BACK3_P, IDB_VIEWINFO_NAV_BTN_BACK3_O);
	    m_btnBACK.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_BACK1_N, IDB_VIEWINFO_NAV_BTN_BACK1_P, IDB_VIEWINFO_NAV_BTN_BACK1_O);
	    m_btnNEXT.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_NEXT1_N, IDB_VIEWINFO_NAV_BTN_NEXT1_P, IDB_VIEWINFO_NAV_BTN_NEXT1_O);
	    m_btnDBLNEXT.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_NEXT3_N, IDB_VIEWINFO_NAV_BTN_NEXT3_P, IDB_VIEWINFO_NAV_BTN_NEXT3_O);
	    m_btnLAST.LoadBitmaps(IDB_VIEWINFO_NAV_BTN_NEXT2_N, IDB_VIEWINFO_NAV_BTN_NEXT2_P, IDB_VIEWINFO_NAV_BTN_NEXT2_O);
	    m_btnPost.LoadBitmaps(IDB_VIEWINFO_NAV_CENTER_N, IDB_VIEWINFO_NAV_CENTER_P, IDB_VIEWINFO_NAV_CENTER_N);

	    m_btnFIRST.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnDBLBACK.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnBACK.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnNEXT.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnDBLNEXT.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnLAST.SetSize(NAV_BTN_WIDTH, NAV_BTN_HEIGHT);
	    m_btnPost.SetSize(NAV_CENTER_WIDTH, NAV_CENTER_HEIGHT);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CView_NavigationPane::_initialize()
{
    m_ftDefault.CreateFont(FISH_INFOPANE_DEFAULT_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
}


void CView_NavigationPane::_finalize()
{

}

afx_msg BOOL CView_NavigationPane::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//    BOOL RET = CDialog::OnEraseBkgnd(pDC);

//    CRect clientRT;
//    GetClientRect(&clientRT);
//    CMemDC smartDC(pDC, &clientRT);
//	
//    if(m_bSelfDraw)
//	{
//		DrawBg(&smartDC);
//        return TRUE;
//	}
//	MoveControls();

//	return RET;
}

void CView_NavigationPane::DrawBg(CDC *pBufferDC)
{
    CRect clientRT;
    GetClientRect(&clientRT);
    pBufferDC->FillRect(clientRT, &CBrush(VIEW_BKGND_COLOR));
}

/************************************************************************
OnUpdatePostQuantity    전달인자로 받은 포스트의 개수를 중앙 버튼 위에 그리는 역할
@PARAM  : 
    WPARAM 포스트의 개수 (UNSIGNED INTEGER)
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/13:CREATED
************************************************************************/
void CView_NavigationPane::OnUpdatePostQuantity(WPARAM wParam, LPARAM lParam)
{
    CString text;
    text.Format(_T("%d"), wParam);
    m_btnPost.SetWindowText(text);
}

/************************************************************************
_text_UpdatePostQuantity    포스트 개수 업데이트 함수의 테스트 케이스
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/13:CREATED
************************************************************************/
void CView_NavigationPane::_text_UpdatePostQuantity()
{
    ::SendMessage(GetSafeHwnd(), WM_INFOPANE_UPDATE_POST_QUANTITY, 0, NULL);
}

void CView_NavigationPane::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
    CRect clientRT;
    GetClientRect(&clientRT);
    CMemDC smartDC(&dc, &clientRT);
	
	DrawBg(&smartDC);
	
	// Do not call CDialog::OnPaint() for painting messages
#ifdef _EXECUTE_UNIT_TEST_MODE_
//    _text_UpdatePostQuantity();
#endif
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewFirst
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 최상단의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewFirst()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	spmain->SendMessage(WM_VIEW_FIRSTPOST, 0, 0);
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewLast
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 최하단의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewLast()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	spmain->SendMessage(WM_VIEW_LASTPOST, 0, 0);
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewNext
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 선택된 포스트의 다음 포스트를 본다.
 * 선택된 포스트가 없으면 최상단의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewNext()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	spmain->SendMessage(WM_VIEW_NEXTPOSTS, 1, 1);
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewPrev
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 선택된 포스트의 위의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewPrev()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	spmain->SendMessage(WM_VIEW_PREVPOSTS, 1, 1);
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewFirst
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 선택된 포스트의 다음 포스트를 설정한 페이지당 포스트의 개수만큼 본다.
 * 선택된 포스트가 없으면 최상단의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewMultiNext()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	int ppp = theApp.m_spLP->GetPPP();		// posts per page

	spmain->SendMessage(WM_VIEW_NEXTPOSTS, 1, ppp);
}

/**************************************************************************
 * method CView_NavigationPane::OnBtnViewFirst
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-16
 *
 * @Description
 * 선택된 포스트의 이전 포스트를 설정한 페이지당 포스트의 개수만큼 본다.
 * 선택된 포스트가 없으면 최상단의 포스트를 본다.
 *
 * @Parameters
 **************************************************************************/
void CView_NavigationPane::OnBtnViewMultiPrev()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

	SmartPtr<CDlg_RSSMain> spmain;
	pFrame->GetDlgRSSMain(spmain);

	if(!spmain) return;

	int ppp = theApp.m_spLP->GetPPP();		// posts per page

	spmain->SendMessage(WM_VIEW_PREVPOSTS, 1, ppp);
}

/************************************************************************
OnBtnNewtab 새 브라우저를 띄우는 버튼
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::OnBtnNewtab() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(pFrame) pFrame->NavigateNewWindow( _T("about:blank") );
}

/************************************************************************
OnBtnStyle 스타일 선택 버튼
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::OnBtnStyle() 
{
	// TODO: Add your control notification handler code here
	CDlg_Style dlg(NULL, _T("스타일 선택"));	
	CString OldStyle = theApp.m_spLP->GetStyleFile();
	if(dlg.DoModal() == IDOK)
	{
		CString Style;
		Style = dlg.GetSelectStyle();

		if(OldStyle != Style)
		{
			theApp.m_spLP->SetStyleFile(Style);
			theApp.m_spLP->SaveProperties();

			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(!pFrame) return;

			SmartPtr<CDlg_RSSMain> spmain;
			pFrame->GetDlgRSSMain(spmain);

			if(!spmain) return;

			spmain->SendMessage(WM_VIEW_REFRESH, 0, 0);
		}
	}
	
	return;
}

/************************************************************************
OnBtnSave 저장 버튼
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(FALSE, STR_XML_FORMAT, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, STR_HTML_FILE_FORMAT, this);	

    if(dlg.DoModal() == IDOK)
    {
        CString fpath = dlg.GetPathName();
		if(theApp.m_spCL->SavePostCurrent(fpath))
		{
			TRACE(_T("Save Post File : %s\n"), fpath);
		}
		else
			FishMessageBox(_T("Post Save Faile!"));
    }

	return;
}

/**************************************************************************
 * method CView_NavigationPane::OnInformCurrentPosts
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-08
 *
 * @Description
 * 현재 목록의 전체 포스트 개수와 읽지 않은 포스트 개수가 갱신되었음을 알려준다.
 *
 * @Parameters
 * (in WPARAM) wParam - 전체 포스트 개수
 * (in LPARAM) lParam - 읽지 않은 포스트 개수
 **************************************************************************/
void CView_NavigationPane::OnInformCurrentPosts(WPARAM wParam, LPARAM lParam)
{
    CString text;
	if(FALSE)		// 현재 보여주는 항목이 전체 포스트 개수인 경우
	{
		text.Format(_T("%d"), wParam);
	}
	else
	{
		text.Format(_T("%d"), lParam);
	}
    m_btnPost.SetWindowText(text);
}

void CView_NavigationPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	MoveControls();
}

void CView_NavigationPane::MoveControls()
{
	if(!m_btnNewTab.GetSafeHwnd()) return;
    CRect clientRT;
    GetClientRect(&clientRT);

    if (m_bBrowsing)
        AlterationToBrowseMode();
    else
        AlterationToArticleMode();
}

/************************************************************************
AlterationToArticleMode 브라우저 모드에서 기사 모드로 전환
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::AlterationToArticleMode()
{
    CRect clientRT;
    GetClientRect(&clientRT);

    {
        // NEW TAB
        CRect newtabRT  =   clientRT;
        newtabRT.left   =   clientRT.right - NEWTAB_BTN_WIDTH;
        newtabRT.top    =   FUNCTION_BTN_TOP_PAD;
        newtabRT.bottom =   FUNCTION_BTN_TOP_PAD + NEWTAB_BTN_HEIGHT;
        m_btnNewTab.MoveWindow(newtabRT);
        m_btnNewTab.ShowWindow(SW_NORMAL);
        
        // STYLE BTN
        CRect styleRT   =   clientRT;
        styleRT.left    =   newtabRT.left - FUNCTION_BTN_INTERVAL - STYLE_BTN_WIDTH;
        styleRT.right   =   styleRT.left + STYLE_BTN_WIDTH;
        styleRT.top     =   FUNCTION_BTN_TOP_PAD;
        styleRT.bottom  =   FUNCTION_BTN_TOP_PAD + STYLE_BTN_HEIGHT;
        m_btnStyle.MoveWindow(styleRT);
        m_btnStyle.ShowWindow(SW_NORMAL);
        
        // SAVE BTN
        CRect saveRT   =   clientRT;
        saveRT.left    =   styleRT.left - FUNCTION_BTN_INTERVAL - SAVE_BTN_WIDTH;
        saveRT.right   =   saveRT.left + SAVE_BTN_WIDTH;
        saveRT.top     =   FUNCTION_BTN_TOP_PAD;
        saveRT.bottom  =   FUNCTION_BTN_TOP_PAD + SAVE_BTN_HEIGHT;
        m_btnSave.MoveWindow(saveRT);
        m_btnSave.ShowWindow(SW_HIDE);


        CRect addrRT        =   clientRT;
        addrRT.right        =   clientRT.right  -   NEWTAB_BTN_WIDTH - FUNCTION_BTN_INTERVAL - SUBSCRIBE_BTN_WIDTH - FUNCTION_BTN_INTERVAL;
        addrRT.top          =   FUNCTION_BTN_TOP_PAD;
        addrRT.bottom       =   FUNCTION_BTN_TOP_PAD + ADDRMSG_BTN_HEIGHT;
        m_ctrlAddressMsg.MoveWindow(addrRT);
        m_ctrlAddressMsg.ShowWindow(SW_NORMAL);
//        m_bDrawStatus   =   TRUE;

        m_btnSubscribe.ShowWindow(SW_HIDE);
    }

    {   // NAVIGATION BTN RELAYOUT
        // POST BTN RELAYOUT
        CRect postRT;
        postRT.left   =   clientRT.CenterPoint().x - (NAV_CENTER_WIDTH / 2);
        postRT.right  =   clientRT.CenterPoint().x + (NAV_CENTER_WIDTH / 2);       // be prevented from floor or ceil effect 
        postRT.top    =   clientRT.CenterPoint().y - (NAV_CENTER_HEIGHT / 2);
        postRT.bottom =   clientRT.CenterPoint().y + (NAV_CENTER_HEIGHT / 2);
        m_btnPost.MoveWindow(postRT);
        m_btnPost.ShowWindow(SW_HIDE);

        // BACK 1 RELAYOUT
        CRect backsRT   =   postRT;
        backsRT.right   -=  INTERVAL_BETWEEN_POSTBTN_N_NAVBTN + NAV_CENTER_WIDTH;
        backsRT.left    =  backsRT.right - NAV_BTN_WIDTH;
        backsRT.top     =   clientRT.CenterPoint().y - (NAV_BTN_HEIGHT /2);
        backsRT.bottom  =   clientRT.CenterPoint().y + (NAV_BTN_HEIGHT /2);
        m_btnBACK.MoveWindow(backsRT);
        m_btnBACK.ShowWindow(SW_HIDE);

        // BACK 2 RELAYOUT
        backsRT.right   -=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        backsRT.left    -=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        m_btnDBLBACK.MoveWindow(backsRT);
        m_btnDBLBACK.ShowWindow(SW_HIDE);

        // BACK 3 RELAYOUT
        backsRT.right   -=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        backsRT.left    -=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        m_btnFIRST.MoveWindow(backsRT);
        m_btnFIRST.ShowWindow(SW_HIDE);

        // NEXT 1 RELAYOUT
        CRect nextsRT   =   postRT;
        nextsRT.left    +=  INTERVAL_BETWEEN_POSTBTN_N_NAVBTN + NAV_CENTER_WIDTH;
        nextsRT.right   =  nextsRT.left + NAV_BTN_WIDTH;
        nextsRT.top     =   clientRT.CenterPoint().y - (NAV_BTN_HEIGHT /2);
        nextsRT.bottom  =   clientRT.CenterPoint().y + (NAV_BTN_HEIGHT /2);
        m_btnNEXT.MoveWindow(nextsRT);
        m_btnNEXT.ShowWindow(SW_HIDE);

        // NEXT 2 RELAYOUT
        nextsRT.right   +=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        nextsRT.left    +=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        m_btnDBLNEXT.MoveWindow(nextsRT);
        m_btnDBLNEXT.ShowWindow(SW_HIDE);

        // NEXT 3 RELAYOUT
        nextsRT.right   +=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        nextsRT.left    +=  INTERVAL_BETWEEN_NAVBTNS + NAV_BTN_WIDTH;
        m_btnLAST.MoveWindow(nextsRT);
        m_btnLAST.ShowWindow(SW_HIDE);
    }
}

/************************************************************************
AlterationToBrowseMode 기사 모드에서 브라우저 모드로 전환
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::AlterationToBrowseMode()
{
    CRect clientRT;
    GetClientRect(&clientRT);

    {
        // NEW TAB
        CRect newtabRT  =   clientRT;
        newtabRT.left   =   clientRT.right - NEWTAB_BTN_WIDTH;
        newtabRT.top    =   FUNCTION_BTN_TOP_PAD;
        newtabRT.bottom =   FUNCTION_BTN_TOP_PAD + NEWTAB_BTN_HEIGHT;
        m_btnNewTab.MoveWindow(newtabRT);
        m_btnNewTab.ShowWindow(SW_NORMAL);
        
        // SUBSCRIBE BTN
        CRect subscribeRT   =   clientRT;
        subscribeRT.left    =   newtabRT.left - FUNCTION_BTN_INTERVAL - SUBSCRIBE_BTN_WIDTH;
        subscribeRT.right   =   subscribeRT.left + SUBSCRIBE_BTN_WIDTH;
        subscribeRT.top     =   FUNCTION_BTN_TOP_PAD;
        subscribeRT.bottom  =   FUNCTION_BTN_TOP_PAD + SUBSCRIBE_BTN_HEIGHT;
        m_btnSubscribe.MoveWindow(subscribeRT);
        m_btnSubscribe.ShowWindow(SW_NORMAL);

        CRect addrRT        =   clientRT;
        addrRT.right        =   clientRT.right  -   NEWTAB_BTN_WIDTH - FUNCTION_BTN_INTERVAL - SUBSCRIBE_BTN_WIDTH - FUNCTION_BTN_INTERVAL;
        addrRT.top          =   FUNCTION_BTN_TOP_PAD;
        addrRT.bottom       =   FUNCTION_BTN_TOP_PAD + ADDRMSG_BTN_HEIGHT;
        m_ctrlAddressMsg.MoveWindow(addrRT);
        m_ctrlAddressMsg.ShowWindow(SW_NORMAL);
//        m_bDrawStatus       =   TRUE;

        m_btnStyle.ShowWindow(SW_HIDE);
        m_btnSave.ShowWindow(SW_HIDE);
    }

    {   // NAVIGATION BTN RELAYOUT
        // POST BTN RELAYOUT
        m_btnPost.ShowWindow(SW_HIDE);

        // BACK 1 RELAYOUT
        m_btnBACK.ShowWindow(SW_HIDE);

        // BACK 2 RELAYOUT
        m_btnDBLBACK.ShowWindow(SW_HIDE);

        // BACK 3 RELAYOUT
        m_btnFIRST.ShowWindow(SW_HIDE);

        // NEXT 1 RELAYOUT
        m_btnNEXT.ShowWindow(SW_HIDE);

        // NEXT 2 RELAYOUT
        m_btnDBLNEXT.ShowWindow(SW_HIDE);

        // NEXT 3 RELAYOUT
        m_btnLAST.ShowWindow(SW_HIDE);
    }
}

/************************************************************************
OnCtlColor 컨트롤의 컬러는 재 지정하는 메소드
@PARAM  : 
@RETURN : 
@REMARK : 
    ON_WM_CTLCOLOR 이벤트 핸들러
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
HBRUSH	CView_NavigationPane::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(VIEW_BKGND_COLOR);
		pDC->SetTextColor(RGB(0, 0, 0));
	}
	return m_hStaticBkgnd;
}

/************************************************************************
UpdateStatusBar 상태바 업데이트
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CString CView_NavigationPane::UpdateStatusBar(CString szStatus)
{
    if (m_ctrlAddressMsg.m_hWnd)
    {
        m_ctrlAddressMsg.SetWindowText( szStatus );
    }
    return _T("");
}

/************************************************************************
OnBtnSubscribe SubScribe 버튼
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CView_NavigationPane::OnBtnSubscribe() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;

    CView_Information* pView =   (CView_Information*)(pFrame->GetFirstView());
    CString szURL   =   ((CDlg_Browser*)pView->GetSelectedBrowser())->GetURL();

    
    ::SendMessage(m_hSubscribe, WM_SUBSCRIBE_URL, (WPARAM)szURL.GetBuffer(MAX_BUFF), NULL );
}

void CView_NavigationPane::OnOK()
{
}

BOOL CView_NavigationPane::PreTranslateMessage(MSG* pMsg) 
{

    switch(pMsg->message)
	{
	case WM_KEYDOWN:
        if(pMsg->wParam ==  VK_TAB)
        {
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
        }

        break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
