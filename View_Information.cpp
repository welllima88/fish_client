// View_Information.cpp : implementation file
//

#include "stdafx.h"
#include "FISH.h"
#include "View_Information.h"
#include "FISH_def.h"
#include "Dlg_ExplorerBar.h"
#include "Dlg_RSSMain.H"
#include "Dlg_Subscribe.h"
#include "RSSCurrentList.h"
#include "FocusManager.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CView_Information

extern CFISHApp theApp;

IMPLEMENT_DYNCREATE(CView_Information, CFormView)

DWORD    CView_Information::m_dwBrowserSeq   =   0;

CView_Information::CView_Information()
	: CFormView(CView_Information::IDD)
{
	//{{AFX_DATA_INIT(CView_Information)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInit			= FALSE;
	m_bSelfDraw		= TRUE;

	// make paint rgn & solid brush
	m_rgnPaint.CreateRectRgn(0, 0, 0, 0);
	m_brushBG.CreateSolidBrush(VIEW_BKGND_COLOR);
	m_brushTabBG.CreateSolidBrush(VIEW_TOP_BKGND_COLOR);
}

CView_Information::~CView_Information()
{
	if(m_rgnPaint.GetSafeHandle())
		m_rgnPaint.DeleteObject();
	if(m_brushBG.GetSafeHandle())
		m_brushBG.DeleteObject();
	if(m_brushTabBG.GetSafeHandle())
		m_brushTabBG.DeleteObject();
}

void CView_Information::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CView_Information)
	DDX_Control(pDX, IDC_INFORMATION_GOTO_MAINLIST_BTN, m_btnGotoMainList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CView_Information, CFormView)
	//{{AFX_MSG_MAP(CView_Information)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_INFORMATION_GOTO_MAINLIST_BTN, OnInformationGotoMainlistBtn)
	ON_WM_QUERYDRAGICON()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_NOTIFY(CTCN_CLICK, IDC_INFORMATION_TAB_CTRL, OnTabLButtonClick)
	ON_NOTIFY(CTCN_RCLICK, IDC_INFORMATION_TAB_CTRL, OnTabRButtonClick)
	ON_NOTIFY(CTCN_SELCHANGE, IDC_INFORMATION_TAB_CTRL, OnTabSelchange)
	ON_NOTIFY(CTCN_HIGHLIGHTCHANGE, IDC_INFORMATION_TAB_CTRL, OnTabHighlightchange)
	ON_NOTIFY(CTCN_ITEMMOVE, IDC_INFORMATION_TAB_CTRL, OnTabMoveitem)
	ON_NOTIFY(CTCN_ITEMCOPY, IDC_INFORMATION_TAB_CTRL, OnTabCopyitem)
	ON_NOTIFY(CTCN_LABELUPDATE, IDC_INFORMATION_TAB_CTRL, OnTabLabelupdate)
	ON_NOTIFY(CTCN_HITCLOSEREGION, IDC_INFORMATION_TAB_CTRL, OnTabCloseRegionClick)
    ON_MESSAGE(WM_REQUEST_POST_DBLCLK, OnRequestPostDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CView_Information diagnostics

#ifdef _DEBUG
void CView_Information::AssertValid() const
{
	CFormView::AssertValid();
}

void CView_Information::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CView_Information message handlers

/************************************************************************
OnInitialUpdate
@param  : 
@return : 
@remark : 
    익스플로러 주소바의 생성 시점을 변경해서는 안된다.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/03: 
************************************************************************/
void CView_Information::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
    // EXPLORER INIT
	m_spExplorerBar = SmartPtr<CDlg_ExplorerBar> (new CDlg_ExplorerBar);
	m_spExplorerBar->Create(this);
	m_spExplorerBar->ShowWindow(SW_NORMAL);

    // RSS MAIN INIT
    m_spRSSMain = SmartPtr<CDlg_RSSMain> (new CDlg_RSSMain);
	m_spRSSMain->Create(this);
	m_spRSSMain->ShowWindow(SW_NORMAL);

    // TAB CTRL INIT
    m_ctrlCustomTab.Create(WS_CHILD | WS_VISIBLE, &CRect(0, 0, 0, 0), this, IDC_INFORMATION_TAB_CTRL);
    m_ctrlCustomTab.SetDragCursors(AfxGetApp()->LoadCursor(IDC_INFORMATION_TAB_MOVE), AfxGetApp()->LoadCursor(IDC_INFORMATION_TAB_COPY));  // 아이콘을 설정하지 안으면 드래그가 보이지 않는다.

    // FUNCTION CTRL INIT
    m_dlgNavigationPane.Create(IDD_VIEW_NAVIGATIONPANE, this);

	//////////////////////////////////////////////////////////////////////////
	// 포커스 매니저 초기화
	CFocusManager::getInstance()->m_pRssList    =   m_spRSSMain->m_spRSSList.get();
	CFocusManager::getInstance()->m_pExplorerBar=   m_spExplorerBar.get();
    
    SmartPtr<CDlg_Subscribe> spSubscribe;
    CMainFrame* pMain = ((CMainFrame*)AfxGetMainWnd());
	if(pMain)
	{
		pMain->GetDlgSubscribe( spSubscribe );
		m_dlgNavigationPane.SetSubscribeHandle( spSubscribe->m_hWnd );

		m_dlgNavigationPane.ShowWindow(SW_NORMAL);

		// MAIN LIST BTN INIT
		m_btnGotoMainList.SetShowText(FALSE);
		m_btnGotoMainList.SetEnableBitmap(IDB_TABCTRL_GOTO_MAINLIST_BTN_N);
		m_btnGotoMainList.SetDisabledBitmap(IDB_TABCTRL_GOTO_MAINLIST_BTN_N);
		m_btnGotoMainList.SetFocussedBitmap(IDB_TABCTRL_GOTO_MAINLIST_BTN_N);
		m_btnGotoMainList.SetDownBitmap(IDB_TABCTRL_GOTO_MAINLIST_BTN_P);
		m_btnGotoMainList.SetActiveBgColor(VIEW_TOP_BKGND_COLOR, FALSE);
		m_btnGotoMainList.DrawBorder(FALSE);
		m_btnGotoMainList.SetInactiveBgColor(VIEW_TOP_BKGND_COLOR, FALSE);
		m_btnGotoMainList.SetSize(FISH_MAINLIST_BTN_WIDTH, FISH_MAINLIST_BTN_HEIGHT);
		m_btnGotoMainList.SetFlat(TRUE);
		m_bInit = TRUE;

		MoveControls();

	//    _testTabControlArticleAdd();            // FOR TAB TEST
		_testTabControlAttribute();

		pMain->SendMessage(WM_INITVIEW, (WPARAM) this, 0);

		theApp.m_spCL->SetPostCntHWnd(m_dlgNavigationPane.GetSafeHwnd());
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CView_Information::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CView_Information::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CView_Information::Create(CWnd* pParentWnd) 
{
	return CFormView::Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, CRect(0, 0, 0, 0), pParentWnd, CView_Information::IDD, NULL);
}

void CView_Information::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE(_T("CView_Information::OnSize\r\n"));
//	CFormView::OnSize(nType, cx, cy);
    if ( m_bInit )   m_dlgNavigationPane.Invalidate(FALSE);
	MoveControls();
}

void CView_Information::MoveControls()
{
	if( !m_bInit || !m_spExplorerBar || !m_spRSSMain ) return;

    CRect clientRT, origRT;
    GetClientRect(&clientRT);
	origRT = clientRT;

	CRgn rgnControl, rgnPaint;
	rgnControl.CreateRectRgn(0, 0, 0, 0);
	rgnPaint.CreateRectRgn(0, 0, 0, 0);
	rgnPaint.SetRectRgn(&clientRT);

	int adressbarH = 0;

    clientRT.DeflateRect(VIEW_DLG_LEFT_PADDING, VIEW_DLG_TOP_PADDING, VIEW_DLG_RIGHT_PADDING, VIEW_DLG_BOTTOM_PADDING);

	if( m_ctrlCustomTab.GetItemCount() == 0 || m_mapSPBrowsers.size() == 0)
	{
        m_ctrlCustomTab.ShowWindow(SW_HIDE);
        m_btnGotoMainList.ShowWindow(SW_HIDE);

        m_spExplorerBar->ShowWindow(SW_HIDE);
		if(!m_spRSSMain->IsWindowVisible())
			m_spRSSMain->ShowWindow(SW_NORMAL);

		CRect rssmainRT     =   clientRT;
        rssmainRT.top       =   VIEW_DLG_TOP_PADDING;
	    m_spRSSMain->MoveWindow(&rssmainRT);

		rgnControl.SetRectRgn(&rssmainRT);
		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

		m_btnGotoMainList.FreezeBtnDownStateDraw(TRUE);    // 메인 리스트 버튼을 다운 상태로 바꾼다.
        m_dlgNavigationPane.SetBrowseFlag(FALSE);             // 하단 기능 버튼의 레이아웃을 정한다.
        m_dlgNavigationPane.Invalidate(FALSE);
	}
	else
    {   // 정보창 영역의 각 컨트롤들의 크기 지정

        // GOTO MAINLIST BTN LAYOUT
        CRect MainListBtnRT    =   clientRT;
        MainListBtnRT.left     =   FISH_MAINLIST_BTN_LEFT_PADDING;
        MainListBtnRT.top      =   FISH_TAB_TOP_PADDING;
        MainListBtnRT.right    =   VIEW_DLG_LEFT_PADDING + FISH_MAINLIST_BTN_WIDTH + FISH_MAINLIST_BTN_LEFT_PADDING - 2;
        MainListBtnRT.bottom   =   FISH_TAB_TOP_PADDING + FISH_MAINLIST_BTN_HEIGHT;
        m_btnGotoMainList.MoveWindow(&MainListBtnRT);

		rgnControl.SetRectRgn(&MainListBtnRT);
		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);
        
        // TAB LAYOUT
        CRect tabRT         =   clientRT;
        tabRT.top           =   FISH_TAB_TOP_PADDING;
        tabRT.left          =   FISH_TAB_LEFT_PADDING;
        tabRT.bottom        =   FISH_TAB_TOP_PADDING   +   FISH_TAB_BTN_HEIGHT;

		m_ctrlCustomTab.MoveWindow(&tabRT);

		rgnControl.SetRectRgn(&tabRT);
		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

        // ADDRESS BAR LAYOUT
        CRect addressRT     =   clientRT;
        addressRT.top       =   FISH_EXPLORERBAR_TOP_PADDING;
	    addressRT.bottom    =   FISH_EXPLORERBAR_TOP_PADDING + FISH_EXPLORERBAR_HEIGHT;
		adressbarH = addressRT.Height();
	    m_spExplorerBar->MoveWindow(&addressRT);

		rgnControl.SetRectRgn(&addressRT);
		rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);


		if(m_ctrlCustomTab.GetCurSel() == -1)
		{
			// RSSMAIN LAYOUT
			CRect rssmainRT     =   clientRT;
			rssmainRT.top       =   FISH_TAB_TOP_PADDING   +   FISH_TAB_BTN_HEIGHT  +   2;
			m_spRSSMain->MoveWindow(&rssmainRT);

			rgnControl.SetRectRgn(&rssmainRT);
			rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);
		}

		// IF TAB ITEM DOES NOT EXIST
		m_ctrlCustomTab.ShowWindow(SW_NORMAL);
		m_btnGotoMainList.ShowWindow(SW_NORMAL);

        //  메인 리스트 창 보여주기 시 주소 입력참 감추기
        if ( m_ctrlCustomTab.GetCurSel() == -1)
        {
            m_spExplorerBar->ShowWindow(SW_HIDE);
            m_spRSSMain->ShowWindow(SW_NORMAL);
        }
        else
        {
            m_spExplorerBar->ShowWindow(SW_NORMAL);
            m_spRSSMain->ShowWindow(SW_HIDE);
        }

		// 브라우저 선택 및 LAYOUT
        DWORD   SEQ =   -1;

        if ( m_ctrlCustomTab.GetCurSel() != -1 && m_ctrlCustomTab.GetItemCount() ) // 탭 선택 되었을 경우
        {
            m_btnGotoMainList.FreezeBtnDownStateDraw(FALSE);    // 메인 리스트 버튼을 일반 상태로 바꾼다.
            m_dlgNavigationPane.SetBrowseFlag(TRUE);
            m_dlgNavigationPane.Invalidate(FALSE);

            m_ctrlCustomTab.GetItemData( m_ctrlCustomTab.GetCurSel(), SEQ);

            assert (SEQ != -1);

            map<DWORD, SmartPtr<CDlg_Browser> >::iterator iter = m_mapSPBrowsers.begin();
            for (;iter != m_mapSPBrowsers.end(); iter++)
            {
				if((*iter).second != (void*) m_mapSPBrowsers[SEQ])
				{
					(*iter).second->ShowWindow(SW_HIDE);
				}
            }

            CRect browserRT   =   clientRT;
		    browserRT.top = FISH_BROWSER_TOP_PADDING;
            browserRT.left      += 1;       // for outline 
            browserRT.right     -= 1;
            browserRT.bottom    -= 1;
		    m_mapSPBrowsers[SEQ]->MoveWindow(&browserRT);
			if(!m_mapSPBrowsers[SEQ]->IsWindowVisible())
				m_mapSPBrowsers[SEQ]->ShowWindow(SW_NORMAL);

			rgnControl.SetRectRgn(&browserRT);
			rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);
        } else {
            m_btnGotoMainList.FreezeBtnDownStateDraw(TRUE);    // 메인 리스트 버튼을 다운 상태로 바꾼다.
            m_dlgNavigationPane.SetBrowseFlag(FALSE);
            m_dlgNavigationPane.Invalidate(FALSE);

            for (DWORD idx = 0; idx != m_dwBrowserSeq; idx++)
            {
                if (    SEQ == idx  )   continue;
                if ( m_mapSPBrowsers.find(idx)  !=  m_mapSPBrowsers.end()  )    m_mapSPBrowsers[idx]->ShowWindow(SW_HIDE);
            }
        }
    }

    // FUNCTION PANE LAYOUT
    CRect functionPaneRT    =   clientRT;
    functionPaneRT.top  =   functionPaneRT.bottom;
    functionPaneRT.bottom   =   functionPaneRT.top + VIEW_DLG_BOTTOM_PADDING;
    m_dlgNavigationPane.MoveWindow(&functionPaneRT);

	rgnControl.SetRectRgn(&functionPaneRT);
	rgnPaint.CombineRgn(&rgnPaint, &rgnControl, RGN_DIFF);

	m_rgnPaint.CopyRgn(&rgnPaint);

    m_btnGotoMainList.Invalidate(FALSE);        // 명시적으로 해줘야 탭 선택시에 버튼의 이미지가 변경됨. commented by eternalbleu@gmail.com
	Invalidate();
}

/**************************************************************************
 * CView_Information::GetCurrentExplorerBrower
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 현재 선택된 CDlg_Browser가 있다면 그 포인터를
 * 선택된 CDlg_Browser가 없다면 새 창을 띄워서 반환한다.
 *
 * @Parameters
 *
 * @return
 * (void*) - (CDlg_Browser*) 획득한 Browser의 포인터
 **************************************************************************/
void* CView_Information::GetCurrentExplorerBrowser()
{
	void* pbrowser = GetSelectedBrowser();

    if(!pbrowser)
	{
		pbrowser = AddExplorerBrower();
	}

	return pbrowser;
}

int CView_Information::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CView_Information::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CFormView::PreCreateWindow(cs);
}

/**************************************************************************
 * deprecated
 *
 * CView_Information::ReadOn
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * theApp.m_spCL->ReadOnPost 참조
 **************************************************************************/
void CView_Information::ReadOn(int postid)
{
//	theApp.m_spCL->ReadOnPost(postid);
}

void CView_Information::RemoveExplorerBrowser(unsigned int seqNUM)
{
	map<DWORD, SmartPtr<CDlg_Browser> >::iterator it = m_mapSPBrowsers.find(seqNUM);
	if(it != m_mapSPBrowsers.end())  m_mapSPBrowsers.erase( it );
}

/**************************************************************************
 * CView_Information::AddExplorerBrower
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-28
 *
 * @Description
 * 새 브라우저 창을 생성하여 그 포인터를 반환한다.
 *
 * @Parameters
 *
 * @Return
 * (void*) - 생성한 Browser의 포인터
 **************************************************************************/
void* CView_Information::AddExplorerBrower()
{
    //////////////////////////////////////////////////////////////////////////
    // CREATION NEW BROWSER
    SmartPtr< CDlg_Browser > tmp = SmartPtr<CDlg_Browser> (new CDlg_Browser);
    tmp->m_parent   =   this;
    tmp->Create(this, m_dwBrowserSeq);
    tmp->WaitUntilInit();

    //////////////////////////////////////////////////////////////////////////
    // ADD BROWSER INTO VECTOR & TAB ITEM ADDITION
    UINT nINDEX = m_mapSPBrowsers.size();
    m_mapSPBrowsers[m_dwBrowserSeq] =   tmp;

    m_ctrlCustomTab.InsertItem(nINDEX, "새 창");
    m_ctrlCustomTab.SetItemData(nINDEX, m_dwBrowserSeq++);

    m_ctrlCustomTab.SetCurSel(nINDEX);

	int itemcnt = m_ctrlCustomTab.GetItemCount();
	int brscnt = m_mapSPBrowsers.size();
    
    MoveControls();
    
	return (void*) tmp.get();
}

/**************************************************************************
 * CView_Information::GetSelectedBrowser
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * 현재 선택된 브라우저의 포인터를 반환한다.
 *
 * @Parameters
 *
 * @return
 * (void*) - 선택된 Browser의 포인터
 **************************************************************************/
void* CView_Information::GetSelectedBrowser()
{
	if(!m_spRSSMain) return NULL;
    if ( m_ctrlCustomTab.GetItemCount == 0 || m_ctrlCustomTab.GetCurSel() == -1 )    return m_spRSSMain->GetBrowserPointer();

    DWORD   SEQ =   -1;
    m_ctrlCustomTab.GetItemData( m_ctrlCustomTab.GetCurSel(), SEQ );
    assert(SEQ != -1);
    return m_mapSPBrowsers[SEQ].get();
}

void CView_Information::DrawItems(CDC *pDC)
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
	DrawOutline(&bufferDC);

	pDC->BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), &bufferDC, 0, 0, SRCCOPY);

	bufferDC.SelectObject(pOldBmp);

	bmpBuffer.DeleteObject();
	bufferDC.DeleteDC();
	memDC.DeleteDC();
}

void CView_Information::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	CRect clientRT, browserRT;
	GetClientRect(&clientRT);

	//////////////////////////////////////////////////////
	// Fill Background color
	pBufferDC->FillSolidRect(0, 0, clientRT.Width(), clientRT.Height(), VIEW_BKGND_COLOR);
//	pBufferDC->FillRgn(&m_rgnPaint, &m_brushBG);

	// 탭 부분의 배경색을 칠한다.
	if(m_ctrlCustomTab.IsWindowVisible())
	{
		pBufferDC->FillRect(&CRect(VIEW_DLG_LEFT_PADDING, VIEW_DLG_TOP_PADDING, clientRT.right - VIEW_DLG_RIGHT_PADDING, FISH_TAB_TOP_PADDING + FISH_TAB_BTN_HEIGHT), &m_brushTabBG);
	}

	if(m_ctrlCustomTab.GetCurSel() == -1) return;

    CPen outlinePen, *pOldPen;
    outlinePen.CreatePen(PS_SOLID, 1, CFishTabCtrl::FISH_TAB_OUTLINE_COLOR);
    pOldPen =   pBufferDC->SelectObject(&outlinePen);
    pBufferDC->MoveTo(VIEW_DLG_LEFT_PADDING, FISH_TAB_TOP_PADDING + FISH_TAB_BTN_HEIGHT -1 );
    pBufferDC->LineTo(clientRT.right - VIEW_DLG_RIGHT_PADDING, FISH_TAB_TOP_PADDING + FISH_TAB_BTN_HEIGHT - 1);
    pBufferDC->SelectObject(&outlinePen);

    {
        CRect browserRT   =   clientRT;
        browserRT.DeflateRect(VIEW_DLG_LEFT_PADDING, VIEW_DLG_TOP_PADDING, VIEW_DLG_RIGHT_PADDING, VIEW_DLG_BOTTOM_PADDING);
        browserRT.top       = FISH_BROWSER_TOP_PADDING - 1;
        browserRT.right     -=  1;
        browserRT.bottom    -=  1;
        
        CPen outlinePen;
        outlinePen.CreatePen(PS_SOLID, 1, FISH_BROWSER_OUTLINE_COLOR);
        CPen* oldPen = pBufferDC->SelectObject(&outlinePen);
        pBufferDC->MoveTo(browserRT.TopLeft());
        pBufferDC->LineTo(browserRT.left, browserRT.bottom);
        pBufferDC->LineTo(browserRT.right, browserRT.bottom);
        pBufferDC->LineTo(browserRT.right, browserRT.top);
        pBufferDC->SelectObject(oldPen);
    }
}

void CView_Information::DrawOutline(CDC* pBufferDC)
{
}

void*   CView_Information::GetBrowserFrom(UINT seqID)
{
    return NULL;
}


BOOL CView_Information::OnEraseBkgnd(CDC* pDC) 
{
//	TRACE(_T("CView_Information::OnEraseBkgnd\r\n"));
	if(m_bSelfDraw)
	{
//        DrawItems(pDC);
		return FALSE;
	}	
	return CFormView::OnEraseBkgnd(pDC);
}

void CView_Information::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}

/************************************************************************
OnTabLButtonClick   왼쪽 버튼 클릭시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabLButtonClick(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	switch(((CTC_NMHDR*)pNMHDR)->nItem)
	{
	case CTCHT_ONFIRSTBUTTON:
		TRACE(_T("OnLButtonClicked: First\n"));
		break;
	case CTCHT_ONPREVBUTTON:
		TRACE(_T("OnLButtonClicked: Prev\n"));
		break;
	case CTCHT_ONNEXTBUTTON:
		TRACE(_T("OnLButtonClicked: Next\n"));
		break;
	case CTCHT_ONLASTBUTTON:
		TRACE(_T("OnLButtonClicked: Last\n"));
		break;
	case CTCHT_NOWHERE:
		TRACE(_T("OnLButtonClicked: Nowhere\n"));
		break;
	default:
		{
/*
			TRACE(_T("Notify(CTCN_CLICK): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
				((CTC_NMHDR*)pNMHDR)->nItem,
				((CTC_NMHDR*)pNMHDR)->pszText,
				((CTC_NMHDR*)pNMHDR)->lParam,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
				((CTC_NMHDR*)pNMHDR)->rItem.left,
				((CTC_NMHDR*)pNMHDR)->rItem.top,
				((CTC_NMHDR*)pNMHDR)->rItem.right,
				((CTC_NMHDR*)pNMHDR)->rItem.bottom,
				((CTC_NMHDR*)pNMHDR)->fSelected,
				((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
		}
		break;
	}
}

/************************************************************************
OnTabRButtonClick   아이템 오른쪽 버튼 클릭시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabRButtonClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	switch(((CTC_NMHDR*)pNMHDR)->nItem)
	{
	case CTCHT_ONFIRSTBUTTON:
		TRACE(_T("OnLButtonClicked: First\n"));
		break;
	case CTCHT_ONPREVBUTTON:
		TRACE(_T("OnLButtonClicked: Prev\n"));
		break;
	case CTCHT_ONNEXTBUTTON:
		TRACE(_T("OnLButtonClicked: Next\n"));
		break;
	case CTCHT_ONLASTBUTTON:
		TRACE(_T("OnLButtonClicked: Last\n"));
		break;
	default:
		{
			int nInsNdx;
			CMenu menu;
			menu.CreatePopupMenu();
			if(((CTC_NMHDR*)pNMHDR)->nItem==CTCHT_NOWHERE)
			{
                /*
				TRACE(_T("OnLButtonClicked: Nowhere\n"));
				nInsNdx = m_ctrlCustomTab.GetItemCount();
				menu.AppendMenu(MF_STRING,1,_T("Insert Item"));
				menu.AppendMenu(MF_STRING|MF_GRAYED,2,_T("Delete Item"));
				menu.AppendMenu(MF_STRING|MF_GRAYED,3,_T("Rename"));
                */
			}
			else
			{
/*				TRACE(_T("Notify(CTCN_RCLICK): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
						((CTC_NMHDR*)pNMHDR)->nItem,
						((CTC_NMHDR*)pNMHDR)->pszText,
						((CTC_NMHDR*)pNMHDR)->lParam,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
						((CTC_NMHDR*)pNMHDR)->rItem.left,
						((CTC_NMHDR*)pNMHDR)->rItem.top,
						((CTC_NMHDR*)pNMHDR)->rItem.right,
						((CTC_NMHDR*)pNMHDR)->rItem.bottom,
						((CTC_NMHDR*)pNMHDR)->fSelected,
						((CTC_NMHDR*)pNMHDR)->fHighlighted);
						*/
				m_ctrlCustomTab.SetCurSel(((CTC_NMHDR*)pNMHDR)->nItem);
				nInsNdx = ((CTC_NMHDR*)pNMHDR)->nItem;
                /*
				menu.AppendMenu(MF_STRING,1,_T("Insert Item"));
				menu.AppendMenu(MF_STRING,2,_T("Delete Item"));
				menu.AppendMenu(MF_STRING,3,_T("Rename"));
                */
			}
	
			CPoint pt(((CTC_NMHDR*)pNMHDR)->ptHitTest);
			m_ctrlCustomTab.ClientToScreen(&pt);

			int nRet = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, 
								pt.x, pt.y, this);
			switch(nRet)
			{
			case 1:
				{
					if(m_ctrlCustomTab.InsertItem(nInsNdx,_T("New SS_WHITERECT Item"),SS_WHITERECT)<0)
					{
					//	AfxMessageBox(_T("InsertItem(...) failed.\nPossible errors:\n1. Item index out of range."));
					}
				}
				break;
			case 2:
				{
					if(m_ctrlCustomTab.DeleteItem(((CTC_NMHDR*)pNMHDR)->nItem)!=CTCERR_NOERROR)
					{
					//	AfxMessageBox(_T("DeleteItem(...) failed.\nPossible errors:\n1. Item index out of range."));
					}
				}
				break;
			case 3:
				{
					if(m_ctrlCustomTab.EditLabel(((CTC_NMHDR*)pNMHDR)->nItem)!=CTCERR_NOERROR)
					{
					//	AfxMessageBox(_T("EditLabel(...) failed.\nPossible errors:\n1. Item index out of range.\n2. Item not selected.\n3. CTCS_EDITLABELS style not specified."));
					}
				}
				break;
			}
		}
		break;
	}
}

/************************************************************************
OnTabMoveitem   아이템 이동시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabMoveitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	TRACE(_T("Notify(CTCN_ITEMMOVE): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
						((CTC_NMHDR*)pNMHDR)->nItem,
						((CTC_NMHDR*)pNMHDR)->pszText,
						((CTC_NMHDR*)pNMHDR)->lParam,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
						((CTC_NMHDR*)pNMHDR)->rItem.left,
						((CTC_NMHDR*)pNMHDR)->rItem.top,
						((CTC_NMHDR*)pNMHDR)->rItem.right,
						((CTC_NMHDR*)pNMHDR)->rItem.bottom,
						((CTC_NMHDR*)pNMHDR)->fSelected,
						((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
}

/************************************************************************
OnTabCopyitem   아이템 복사시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabCopyitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	TRACE(_T("Notify(CTCN_ITEMCOPY): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
						((CTC_NMHDR*)pNMHDR)->nItem,
						((CTC_NMHDR*)pNMHDR)->pszText,
						((CTC_NMHDR*)pNMHDR)->lParam,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
						((CTC_NMHDR*)pNMHDR)->rItem.left,
						((CTC_NMHDR*)pNMHDR)->rItem.top,
						((CTC_NMHDR*)pNMHDR)->rItem.right,
						((CTC_NMHDR*)pNMHDR)->rItem.bottom,
						((CTC_NMHDR*)pNMHDR)->fSelected,
						((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
}

/************************************************************************
OnTabLabelupdate    탭 라벨 텍스트 변경시 행해야 할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabLabelupdate(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	TRACE(_T("Notify(CTCN_LABELUPDATE): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
						((CTC_NMHDR*)pNMHDR)->nItem,
						((CTC_NMHDR*)pNMHDR)->pszText,
						((CTC_NMHDR*)pNMHDR)->lParam,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
						((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
						((CTC_NMHDR*)pNMHDR)->rItem.left,
						((CTC_NMHDR*)pNMHDR)->rItem.top,
						((CTC_NMHDR*)pNMHDR)->rItem.right,
						((CTC_NMHDR*)pNMHDR)->rItem.bottom,
						((CTC_NMHDR*)pNMHDR)->fSelected,
						((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
	if(CString(((CTC_NMHDR*)pNMHDR)->pszText).IsEmpty())
	{
		*pResult = 1; // Invalid label name
//		AfxMessageBox(_T("Invalid label name"));
	}
	else
		*pResult = 0; // Label name OK
}

/************************************************************************
OnTabSelchange  탭 아이템 변경시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
	updated at 2006/01/20 :: 선택된 Browser에 따라 ExplorerBar의 Back, Forward Button 연동
						by moonknit
************************************************************************/
void CView_Information::OnTabSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	TRACE(_T("Notify(CTCN_SELCHANGE): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
				((CTC_NMHDR*)pNMHDR)->nItem,
				((CTC_NMHDR*)pNMHDR)->pszText,
				((CTC_NMHDR*)pNMHDR)->lParam,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
				((CTC_NMHDR*)pNMHDR)->rItem.left,
				((CTC_NMHDR*)pNMHDR)->rItem.top,
				((CTC_NMHDR*)pNMHDR)->rItem.right,
				((CTC_NMHDR*)pNMHDR)->rItem.bottom,
				((CTC_NMHDR*)pNMHDR)->fSelected,
				((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
    //////////////////////////////////////////////////////////////////////////
    // Explorer bar status update
    m_spExplorerBar->MoveControls();
    m_spExplorerBar->UpdateState();
    MoveControls();
	*pResult = 0;
}

/************************************************************************
OnTabHighlightchange    탭 하이라이트 아이템 변경시 해야할 일
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
************************************************************************/
void CView_Information::OnTabHighlightchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	TRACE(_T("Notify(CTCN_HIGHLIGHTCHANGE): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
				((CTC_NMHDR*)pNMHDR)->nItem,
				((CTC_NMHDR*)pNMHDR)->pszText,
				((CTC_NMHDR*)pNMHDR)->lParam,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
				((CTC_NMHDR*)pNMHDR)->rItem.left,
				((CTC_NMHDR*)pNMHDR)->rItem.top,
				((CTC_NMHDR*)pNMHDR)->rItem.right,
				((CTC_NMHDR*)pNMHDR)->rItem.bottom,
				((CTC_NMHDR*)pNMHDR)->fSelected,
				((CTC_NMHDR*)pNMHDR)->fHighlighted);
*/
//    MoveControls();
    *pResult = 0;
}

/************************************************************************
OnTabCloseReginClick 탭 클로그 영역 클리시 해야할 일
@param  : 
@return : 
@remark : 
    현재 익스플로러가 죽으면서 애플리케이션 전체가 죽는 문제가 발생한다.
    내 생각으로는 익스플로러가 삭제되면서 종료 메시지를 보는 듯 하다.
    (by eternableu)
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/12/01: 
	updated at 2006/01/20 :: OnInformationGotoMainlistBtn() 호출 후 MoveControls가 다시 호출 되는 루틴 수정
						by moonknit
************************************************************************/
void CView_Information::OnTabCloseRegionClick(NMHDR* pNMHDR, LRESULT* pResult)
{
/*	TRACE(_T("Notify(CTCN_ITEMCLOSE): nItem(%d) pszText(\'%s\') lParam(%d) point(x=%d,y=%d) rect(l=%d,t=%d,r=%d,b=%d) bSelected(%d) bHighlighted(%d)\n"),
				((CTC_NMHDR*)pNMHDR)->nItem,
				((CTC_NMHDR*)pNMHDR)->pszText,
				((CTC_NMHDR*)pNMHDR)->lParam,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.x,
				((CTC_NMHDR*)pNMHDR)->ptHitTest.y,
				((CTC_NMHDR*)pNMHDR)->rItem.left,
				((CTC_NMHDR*)pNMHDR)->rItem.top,
				((CTC_NMHDR*)pNMHDR)->rItem.right,
				((CTC_NMHDR*)pNMHDR)->rItem.bottom,
				((CTC_NMHDR*)pNMHDR)->fSelected,
				((CTC_NMHDR*)pNMHDR)->fHighlighted);
				*/
    DWORD SEQ   =   -1;
    m_ctrlCustomTab.GetItemData( ((CTC_NMHDR*)pNMHDR)->nItem, SEQ);
    m_ctrlCustomTab.DeleteItem( ((CTC_NMHDR*)pNMHDR)->nItem );

    map<DWORD, SmartPtr<CDlg_Browser> >::iterator it =  m_mapSPBrowsers.find(SEQ);
    m_mapSPBrowsers.erase( it );

    //////////////////////////////////////////////////////////////////////////
    // Explorer bar status update
    m_spExplorerBar->MoveControls();
    m_spExplorerBar->UpdateState();

    if (m_ctrlCustomTab.GetItemCount() == 0)  {
        GotoMainlist(TRUE);
    }   else    {
        MoveControls();
    }
    *pResult    =   0;
}

void CView_Information::GotoMainlist(BOOL bForce)
{
	if(-1 == m_ctrlCustomTab.GetCurSel() && !bForce) return;		// 이미 list가 선택되어있음

    m_btnGotoMainList.FreezeBtnDownStateDraw(TRUE);
    m_ctrlCustomTab.SetCurSel(-1);
    m_ctrlCustomTab.Invalidate();
    MoveControls();
}

void CView_Information::OnInformationGotoMainlistBtn() 
{
	// TODO: Add your control notification handler code here
	GotoMainlist();
    return;	
}

void CView_Information::_testTabControlArticleAdd()
{
	m_ctrlCustomTab.InsertItem(0,"SS_BLACKRECT");
	m_ctrlCustomTab.SetItemData(0,SS_BLACKRECT);
	m_ctrlCustomTab.InsertItem(1,"SS_GRAY");
	m_ctrlCustomTab.SetItemData(1,SS_GRAYRECT);
	m_ctrlCustomTab.InsertItem(2,"SS_WHITERECT");
	m_ctrlCustomTab.SetItemData(2,SS_WHITERECT);
}

void CView_Information::_testTabControlAttribute()
{
    m_ctrlCustomTab.ModifyStyle(0,CTCS_TOOLTIPS,0);
    
    m_ctrlCustomTab.SetItemTooltipText(CTCID_FIRSTBUTTON,"처음으로");
    m_ctrlCustomTab.SetItemTooltipText(CTCID_PREVBUTTON,"앞으로");
    m_ctrlCustomTab.SetItemTooltipText(CTCID_NEXTBUTTON,"뒤로");
    m_ctrlCustomTab.SetItemTooltipText(CTCID_LASTBUTTON,"마지막으로");
    m_ctrlCustomTab.SetItemTooltipText(0,"Press to fill the static window background with the color used to draw window frames.\0");
    m_ctrlCustomTab.SetItemTooltipText(1,"Press to fill the static window background with the color used to fill the screen background.\0");
    m_ctrlCustomTab.SetItemTooltipText(2,"Press to fill the static window background with the color used to fill the the window background.\0");
    
    m_ctrlCustomTab.ModifyStyle(0, CTCS_AUTOHIDEBUTTONS, 0);
    m_ctrlCustomTab.ModifyStyle(0, CTCS_DRAGCOPY, 0);
    m_ctrlCustomTab.ModifyStyle(0, CTCS_DRAGMOVE, 0);
    m_ctrlCustomTab.ModifyStyle(0, CTCS_CLOSEREGION_HITTEST, 0);
    return;
}

void CView_Information::_testTabControlFontChange()
{
    // Default font
	m_ctrlCustomTab.SetControlFont(m_ctrlCustomTab.GetDefaultFont(), TRUE);

    // 변경될 폰트
	LOGFONT lf = {12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DEFAULT_APPLICATION_FONT};
	m_ctrlCustomTab.SetControlFont(lf, TRUE);
    
    return;
}

void CView_Information::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

afx_msg void CView_Information::OnRequestPostDblclk(LPARAM lParam, WPARAM wParam)
{
    if (m_ctrlCustomTab.GetCurSel() != -1 )
        OnInformationGotoMainlistBtn();
}

void CView_Information::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawItems(&dc);		
	// Do not call CFormView::OnPaint() for painting messages
}

LRESULT CView_Information::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_NCPAINT)
	{
		return 0;
	}
	
	return CFormView::DefWindowProc(message, wParam, lParam);
}

BOOL CView_Information::SetFocusOnAddrbar()
{
//    MessageBox(_T("TEST"), _T("TEST"));
    m_spExplorerBar->FocusOnAddressCombo();
    return TRUE;
}

BOOL CView_Information::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
        if(pMsg->wParam ==  VK_TAB)
        {
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
        }
//		else if(GetAsyncKeyState(0x4E) < 0)
//		{
//			NewExplorerBrowser();
//			return TRUE;
//		}

		break;
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}
