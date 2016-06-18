// Dlg_ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "View_Information.h"
#include "Dlg_ExplorerBar.h"
#include "Dlg_Browser.h"
#include "View_Information.h"
#include "MainFrm.h"
#include "FishDoc.h"
#include "fish_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_ExplorerBar dialog

#define FISH_EXPLORER_DELIMITER     _T("@") // 레지스트리에 주소 저장시 개별요소의 구분자로 사용될 문자
#define MAX_BUFFER_SIZE             1024    // 주소저장의 버퍼 사이즈
#define MAX_URL_HISTORY_SIZE        30      // 주소의 총 개수

CDlg_ExplorerBar::CDlg_ExplorerBar(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_ExplorerBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_ExplorerBar)
	m_sURL = _T("");
	//}}AFX_DATA_INIT

	m_bInit			= FALSE;
	m_bSelfDraw		= TRUE;
    
    _initialize();
}

void CDlg_ExplorerBar::_initialize()
{
	m_ftDefault.CreateFont(FISH_COMBOBOX_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
}

CDlg_ExplorerBar::~CDlg_ExplorerBar()
{
    _finalize();
}

/************************************************************************
_finalize   현재 존재하는 주소들을 레지스트리에 저장한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/22:CREATED
************************************************************************/
void CDlg_ExplorerBar::_finalize()
{
    SaveURL();
}


void CDlg_ExplorerBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_ExplorerBar)
	DDX_Control(pDX, IDC_BUTTON4, m_btnREFRESH);
	DDX_Control(pDX, IDC_BUTTON3, m_btnSTOP);
	DDX_Control(pDX, IDC_BUTTON2, m_btnFORWARD);
	DDX_Control(pDX, IDC_BUTTON1, m_btnBACK);
	DDX_Control(pDX, IDC_COMBO_GO, m_cbGo);
	DDX_CBString(pDX, IDC_COMBO_GO, m_sURL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_ExplorerBar, CDialog)
	//{{AFX_MSG_MAP(CDlg_ExplorerBar)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonBack)
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonForward)
	ON_BN_CLICKED(IDC_BUTTON3, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON4, OnButtonRefresh)
	ON_CBN_SELCHANGE(IDC_COMBO_GO, OnSelchangeCombo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_ExplorerBar message handlers
BOOL CDlg_ExplorerBar::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pParent   =   (CView_Information*)pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

void CDlg_ExplorerBar::MoveControls()
{
    enum {
        EXPLORERBAR_BTN_PAD_LEFT        =   11,
        EXPLORERBAR_BTN_PAD_TOP         =   6,
        EXPLORERBAR_BTN_INTERVAL        =   3,
        EXPLORERBAR_BTN_HOFFSET         =   EXPLORERBAR_BTN_WIDTH + EXPLORERBAR_BTN_INTERVAL,
        EXPLORERBAR_COMBOBOX_PAD_LEFT   =   26 + 4 * (EXPLORERBAR_BTN_WIDTH + EXPLORERBAR_BTN_INTERVAL),
        EXPLORERBAR_COMBOBOX_PAD_RIGHT  =   13,
        EXPLORERBAR_COMBOBOX_PAD_TOP    =   6,
        EXPLORERBAR_COMBOBOX_HEIGHT     =   19,
    };

	if( !m_bInit ) return;

	CRect clientRT;
	GetClientRect(&clientRT);

    {
        //////////////////////////////////////////////////////////////////////////
        // 어드레스바 위치 및 크기 지정. 좌측에 4개의 버튼
        CRect cbrect;
	    m_cbGo.GetClientRect(&cbrect);
        cbrect.top      =   EXPLORERBAR_COMBOBOX_PAD_TOP;
	    cbrect.left     =   EXPLORERBAR_COMBOBOX_PAD_LEFT;
	    cbrect.right    =   clientRT.right - EXPLORERBAR_COMBOBOX_PAD_RIGHT;
	    cbrect.bottom   =   EXPLORERBAR_COMBOBOX_PAD_TOP + EXPLORERBAR_COMBOBOX_HEIGHT;
	    m_cbGo.MoveWindow(&cbrect);
    }

    {
        //////////////////////////////////////////////////////////////////////////
        // 버튼 위치 및 크기 지정
        CRect btnTemp(EXPLORERBAR_BTN_PAD_LEFT, EXPLORERBAR_BTN_PAD_TOP, EXPLORERBAR_BTN_PAD_LEFT + EXPLORERBAR_BTN_WIDTH, EXPLORERBAR_BTN_PAD_TOP + EXPLORERBAR_BTN_HEIGHT);
        m_btnBACK.MoveWindow(&btnTemp);

        btnTemp.OffsetRect(EXPLORERBAR_BTN_HOFFSET , 0);
        m_btnFORWARD.MoveWindow(&btnTemp);

        btnTemp.OffsetRect(EXPLORERBAR_BTN_HOFFSET, 0);
        m_btnSTOP.MoveWindow(&btnTemp);

        btnTemp.OffsetRect(EXPLORERBAR_BTN_HOFFSET, 0);
        m_btnREFRESH.MoveWindow(&btnTemp);
    }

	Invalidate();
}

BOOL CDlg_ExplorerBar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bInit = TRUE;

    {   // BTN SETTING
        m_btnREFRESH.SetEnableBitmap(IDB_EXPLORERBAR_REFRESH_N);
        m_btnREFRESH.SetFocussedBitmap(IDB_EXPLORERBAR_REFRESH_O);
        m_btnREFRESH.SetDownBitmap(IDB_EXPLORERBAR_REFRESH_P);
        m_btnREFRESH.SetDisabledBitmap(IDB_EXPLORERBAR_REFRESH_D);

        m_btnSTOP.SetEnableBitmap(IDB_EXPLORERBAR_STOP_N);
        m_btnSTOP.SetFocussedBitmap(IDB_EXPLORERBAR_STOP_O);
        m_btnSTOP.SetDownBitmap(IDB_EXPLORERBAR_STOP_P);
        m_btnSTOP.SetDisabledBitmap(IDB_EXPLORERBAR_STOP_D);

        m_btnFORWARD.SetEnableBitmap(IDB_EXPLORERBAR_FORWARD_N);
        m_btnFORWARD.SetFocussedBitmap(IDB_EXPLORERBAR_FORWARD_O);
        m_btnFORWARD.SetDownBitmap(IDB_EXPLORERBAR_FORWARD_P);
        m_btnFORWARD.SetDisabledBitmap(IDB_EXPLORERBAR_FORWARD_D);

        m_btnBACK.SetEnableBitmap(IDB_EXPLORERBAR_BACK_N);
        m_btnBACK.SetFocussedBitmap(IDB_EXPLORERBAR_BACK_O);
        m_btnBACK.SetDownBitmap(IDB_EXPLORERBAR_BACK_P);
        m_btnBACK.SetDisabledBitmap(IDB_EXPLORERBAR_BACK_D);

        m_btnREFRESH.SetDisabledBitmap(IDB_EXPLORERBAR_REFRESH_D);

        m_btnREFRESH.SetShowText(FALSE);
        m_btnSTOP.SetShowText(FALSE);
        m_btnFORWARD.SetShowText(FALSE);
        m_btnBACK.SetShowText(FALSE);

        m_btnREFRESH.SetSize(EXPLORERBAR_BTN_WIDTH, EXPLORERBAR_BTN_HEIGHT);
        m_btnSTOP.SetSize(EXPLORERBAR_BTN_WIDTH, EXPLORERBAR_BTN_HEIGHT);
        m_btnFORWARD.SetSize(EXPLORERBAR_BTN_WIDTH, EXPLORERBAR_BTN_HEIGHT);
        m_btnBACK.SetSize(EXPLORERBAR_BTN_WIDTH, EXPLORERBAR_BTN_HEIGHT);

        m_cbGo.SetFont(&m_ftDefault);
        m_cbGo.EnableAutoCompletion(FALSE);
    }

    //////////////////////////////////////////////////////////////////////////
    // 레지스트리에서 현재 저장된 주소들을 읽어들인다.
    LoadURL();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_ExplorerBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	MoveControls();
}

BOOL CDlg_ExplorerBar::PreTranslateMessage(MSG* pMsg) 
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
			ExploreDest();
            //////////////////////////////////////////////////////////////////////////
            // 포커스를 브라우저로 옮긴다.
            CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(pFrame) ((CDlg_Browser*)((CView_Information*)pFrame->GetFirstView())->GetSelectedBrowser())->SetFocus();

			return TRUE;
		}

		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/************************************************************************
ExploreDest 콤보 박스에 있는 주소로 브라우저의 요청을 넘긴다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_ExplorerBar::ExploreDest()
{
	UpdateData();
	// m_sURL
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if(!pFrame) return;
	
    CView_Information* ptrView  =   (CView_Information*)pFrame->GetFirstView();
    CDlg_Browser* pBrowser  =   (CDlg_Browser*)ptrView->GetSelectedBrowser();

    CWnd* pCurrentFocusWnd  =   GetFocus();
    CWnd* pNextFocusWnd     =   pBrowser;

    pNextFocusWnd->SetFocus();
    pFrame->RequestExplorerURL(m_sURL);
}

void CDlg_ExplorerBar::DrawItems(CDC *pDC)
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

void CDlg_ExplorerBar::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
    enum {
        SPLITTER_LEFT_PADDING   =   110,
    };

	CRect clientRT, bkgndRT, comboRT;
	GetClientRect(&clientRT);

	//////////////////////////////////////////////////////
	// Fill Background color
	//pBufferDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255,255, 255));
    CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
    CBitmap*    bkgnd   =   CFishBMPManager::getInstance()->getExplorerBkgnd();
    CBitmap*    splitter = CFishBMPManager::getInstance()->getExplorerBkgnd(1);

    bkgndRT =   clientRT;
    bkgndRT.DeflateRect(0, 0, 0, 0);

    pBufferDC->FillSolidRect(clientRT, RGB(0x43, 0x58, 0x75));
    BMPManager->drawTiledBlt(pBufferDC, bkgnd, bkgndRT);
    BMPManager->drawTransparent(pBufferDC, splitter, CPoint(SPLITTER_LEFT_PADDING, 0));

    // COMBOBOX OUTLINE IMPACT
    m_cbGo.GetWindowRect(comboRT);
    ScreenToClient(comboRT);
    comboRT.InflateRect(1, 1, 1, 1);

    {
        CPen outlinePen;
        outlinePen.CreatePen(PS_SOLID, 1, FISH_ADDRESSBAR_OUTLINE_COLOR);
        CPen* oldPen = pBufferDC->SelectObject(&outlinePen);
        pBufferDC->Rectangle(&comboRT);
        pBufferDC->SelectObject(oldPen);    
    }


    {
        CPen outlinePen;
        outlinePen.CreatePen(PS_SOLID, 1, FISH_EXPLORERDLG_OUTLINE_COLOR);
        CPen* oldPen = pBufferDC->SelectObject(&outlinePen);
        pBufferDC->MoveTo(clientRT.TopLeft());
        pBufferDC->LineTo(clientRT.left, clientRT.bottom);
        pBufferDC->LineTo(clientRT.right, clientRT.bottom);
        pBufferDC->LineTo(clientRT.right, clientRT.top);
        pBufferDC->SelectObject(oldPen);
    }
}

void CDlg_ExplorerBar::DrawOutline(CDC* pBufferDC)
{
}


BOOL CDlg_ExplorerBar::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bSelfDraw)
	{
		return FALSE;
	}	

	return CDialog::OnEraseBkgnd(pDC);
}

void CDlg_ExplorerBar::OnButtonBack() 
{
	//////////////////////////////////////////////////////////////////////////
	// BACK BTN CLICKED
    ((CDlg_Browser*)((CView_Information*)m_pParent)->GetSelectedBrowser())->WBGoBack();
}

void CDlg_ExplorerBar::OnButtonForward() 
{
	//////////////////////////////////////////////////////////////////////////
	// FORWARD BTN CLICKED
    ((CDlg_Browser*)((CView_Information*)m_pParent)->GetSelectedBrowser())->WBGoForward();
}

void CDlg_ExplorerBar::OnButtonStop() 
{
	//////////////////////////////////////////////////////////////////////////
	// STOP BTN CLICKED
    ((CDlg_Browser*)((CView_Information*)m_pParent)->GetSelectedBrowser())->WBStop();
}

void CDlg_ExplorerBar::OnButtonRefresh() 
{
	//////////////////////////////////////////////////////////////////////////
	// REFFESH BTN CLICKED
    ((CDlg_Browser*)((CView_Information*)m_pParent)->GetSelectedBrowser())->WBRefresh();
}

void CDlg_ExplorerBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawItems(&dc);	
	// Do not call CDialog::OnPaint() for painting messages
}

/************************************************************************
OnSelchangeCombo    콤보박스에서 아이템 선택시 액션 처리
@PARAM  : 
@RETURN : 
@REMARK : 
    http://www.devpia.com/Forum/BoardView.aspx?no=340096&ref=340096&page=2&forumname=VC_QA&stype=&KeyW=%c4%de%ba%b8%b9%da%bd%ba+%bc%b1%c5%c3&KeyR=title
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/06:CREATED
************************************************************************/
void CDlg_ExplorerBar::OnSelchangeCombo()
{
    CString szPrevUrl   =   m_sURL;
    m_cbGo.SetCurSel( m_cbGo.GetCurSel() );
    UpdateData();
    
    if (szPrevUrl   !=  m_sURL)
        ExploreDest();
}

void CDlg_ExplorerBar::AddURL()
{
    m_sURL.TrimLeft( _T(" ") );
    m_sURL.TrimRight( _T(" ") );
    m_sURL.TrimRight( _T("/") );

    // 무시하는 형태의 주소
    if ( m_sURL.GetLength() == 0 || m_sURL == _T("") || 
         m_sURL == _T("http://") || m_sURL == _T("http:///") ||
         m_sURL == _T("about:blank"))    
         return;

    //////////////////////////////////////////////////////////////////////////
    // 최대 갯수 30을 유지한다.
    if ( m_listURL.GetCount() >  MAX_URL_HISTORY_SIZE )
    {
        CString str =   m_listURL.RemoveTail();
        m_cbGo.DeleteString( m_cbGo.FindStringExact( -1,  str ) );
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 주소에 http:// 가 존재하는 주소만 입력한다.
    if ( m_sURL.Find( _T("http://") ) != -1 )
    {
        int nIndex  =   m_cbGo.FindStringExact(-1, m_sURL);
        POSITION pos    =   m_listURL.Find( m_sURL );

        /// COMBO INSERTION
        if ( nIndex == CB_ERR && pos == NULL)
        {
            m_cbGo.InsertString(0, m_sURL);
            m_listURL.InsertBefore( m_listURL.GetHeadPosition(), m_sURL );
        } else {
            m_cbGo.DeleteString( nIndex );
            m_cbGo.InsertString(0, m_sURL);
            m_cbGo.SetCurSel( 0 );      // 삭제한 뒤에 재설정이 필요하다.
            m_listURL.RemoveAt(pos);
            m_listURL.InsertBefore( m_listURL.GetHeadPosition(), m_sURL);
        }
    }
}

void CDlg_ExplorerBar::UpdateState(CString url)
{
    //////////////////////////////////////////////////////////////////////////
    // BACK, FORWARD AVAILABILITY UPDATE
    if ( url != _T("") )    m_sURL  =   url;

    CDlg_Browser* pBrowser = (CDlg_Browser*) m_pParent->GetSelectedBrowser();
	if(pBrowser)
	{
		m_btnBACK.EnableWindow(pBrowser->isBackEnable());
		m_btnFORWARD.EnableWindow(pBrowser->isForwardEnable());
	}


    //////////////////////////////////////////////////////////////////////////
    // 익스플로러 주소창의 주소값을 Invalidate
    {
        m_sURL  =   ((CDlg_Browser*) m_pParent->GetSelectedBrowser()) ->GetURL();
        UpdateData(FALSE);
        AddURL();
    }
}

/************************************************************************
SaveURL 콤보 박스에 저장된 주소를 레지스트리 저장한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_ExplorerBar::SaveURL()
{
    CString history;
    int nCount  =   m_listURL.GetCount();

    POSITION pos    =   m_listURL.GetHeadPosition();
    for(; pos != NULL; )
    {
        CString& tsz    =   m_listURL.GetAt( pos );

        history += tsz;
        history += FISH_EXPLORER_DELIMITER;

        m_listURL.GetNext(pos);
    }

    WriteRegData( _T(""), _T("HistoryAddress"), history);
}

/************************************************************************
LoadURL 콤보 박스로 레지스트리 저장된 주소를 로드한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_ExplorerBar::LoadURL()
{
    CString history =   ReadRegData( _T(""), _T("HistoryAddress"));

    TCHAR*  token   =   _tcstok((LPTSTR)(LPCTSTR)history, FISH_EXPLORER_DELIMITER );
    CStringArray szHistoryURL;
    while (token != NULL) {
        szHistoryURL.Add(token);
        token = _tcstok(NULL, FISH_EXPLORER_DELIMITER);
    }

    for (int it = 0; it < szHistoryURL.GetSize(); it++)
    {
        m_cbGo.AddString( szHistoryURL[it] );
        m_listURL.AddTail( szHistoryURL[it] );
    }

}

BOOL CDlg_ExplorerBar::UpdateData(BOOL bSaveAndValidate /*= TRUE*/)
{
    return CDialog::UpdateData( bSaveAndValidate );
}

/************************************************************************
FocusOnAddressCombo
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CDlg_ExplorerBar::FocusOnAddressCombo()
{
    CWnd* pCurrentFocusWnd  =   GetFocus();
    CWnd* pNextFocusWnd =   &m_cbGo;

    pNextFocusWnd->SetFocus();
}

/************************************************************************
FindStringExact 입력 받은 주소가 콤보 박스에 이미 존재하는지 확인하는 메소드
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
int CDlg_ExplorerBar::FindStringExact(int nIndexStart, LPCTSTR lpszFind)
{
    return m_cbGo.FindStringExact(nIndexStart, lpszFind);
}

//////////////////////////////////////////////////////////////////////////
// 각 버튼을 사용가능 불가하도록 조정하는 메소드임.
BOOL CDlg_ExplorerBar::btnBACK_EnableWindow(BOOL bEnable)
{
    return m_btnBACK.EnableWindow(bEnable);
}

BOOL CDlg_ExplorerBar::btnFORWARD_EnableWindow(BOOL bEnable)
{
    return m_btnFORWARD.EnableWindow(bEnable);
}

BOOL CDlg_ExplorerBar::btnSTOP_EnableWindow(BOOL bEnable)
{
    return m_btnSTOP.EnableWindow(bEnable);
}

BOOL CDlg_ExplorerBar::btnREFRESH_EnableWindow(BOOL bEnable)
{
    return m_btnREFRESH.EnableWindow(bEnable);
}