// Dlg_Search.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_Search.h"
#include "SearchManager.h"
#include "fish_common.h"
#include "FocusManager.h"
#include <locale>

#include "./view_search/RcmmWordWnd.h"
#include "Dlg_MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_RECOMMWND			20
#define BTN_DLG_SRCH_WIDTH	500

#define BTN_SEARCH_TARGET_WIDTH     54      // 검색 범위 버튼
#define BTN_SEARCH_TARGET_HEIGHT    22

#define BTN_CONNECTION_WIDTH        27      // 커넥션 상태 버튼
#define BTN_CONNECTION_HEIGHT       22

#define BTN_GO_WIDTH		33              // GO 버튼
#define BTN_GO_HEIGHT		22              // GO 버튼
#define BTN_TAG_WIDTH		24              // TAG 버튼
#define BTN_TAG_HEIGHT		22              // TAG 버튼

#define BTN_INEDIT_HEIGHT	13
#define BTN_RANGE_INTERVAL  14

#define BTN_LOCAL_WIDTH     45
#define BTN_LOCAL_HEIGHT    14

#define BTN_GLOBAL_WIDTH    45
#define BTN_GLOBAL_HEIGHT   14

#define INTERVAL_OUTLINE_BT_BTN         17
#define INTERVAL_BTN_BT_INFO            4
#define INTERVAL_TARGET_BTN_CONNECT     4

#define MAX_THREADTRY						3

#define TID_RECWORDVALID					1
#define TID_SEARCHINTERVAL					2

#define ELPS_RECWORDVALID					1 * 60 * 1000		// 5 mins

#define STR_RCMM_TITLE						_T("추천")
#define STR_DS_SEARCHPOST					_T("Search %s Users & %s Posts")
#define STR_SRCHRANGE_IMG_LOAD_FAILED       _T("[LIZZY]CDlg_Search::LoadBmpSrchRangeBtn 버튼 이미지 로드 실패  \n")
#define STR_NETWORKBTN_IMG_LOAD_FAILED      _T("[LIZZY]CDlg_Search::LoadNetworkBtnBitmap 버튼 이미지 로드 실패  \n")
#define STR_NETWORKLOADBTN_IMG_LOAD_FAILED  _T("[LIZZY]CDlg_Search::LoadNetworkLoadNumBtnBitmap 버튼 이미지 로드 실패  \n")

#define URL_RECOMMANDWORD			 	_T("http://3fishes.co.kr/cfish/rank.xml")
#define XML_XPATH_KEYWORD				_T("//keyword")
#define XML_TITLE						_T("title")

/////////////////////////////////////////////////////////////////////////////
// CDlg_Search dialog


CDlg_Search::CDlg_Search(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Search::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Search)
	m_sKeyword = _T("");
	//}}AFX_DATA_INIT
	m_bInit			= FALSE;
	m_bSelfDraw		= TRUE;

	m_BackBrush.CreateSolidBrush( (COLORREF) RGB(249, 250, 253) );

	InitObjects();

	m_nCurrType = TYPE_SRCH_RCMMWORD;
	m_strLoadCount = _T("");
	m_nSrchRange = TYPE_SRCH_RANGE_ALL;
	m_bMakeInfoNSetRange = FALSE;
	m_nNodeState = TYPE_SRCH_NETWORK_STATUS_0;

	m_hThread		= NULL;
	m_nThreadTry	= 0;

	m_bSearchTime	= TRUE;

	m_nSearchTimer	= NULL;
	m_nRecommTimer	= NULL;

	m_nValidRecomm	= NULL;

	m_rgnClip.CreateRectRgn(0, 0, 0, 0);

	m_vecRcmmWordList.clear();
}

CDlg_Search::~CDlg_Search()
{
	TRACE(_T("CDlg_Search Destroy\r\n"));
	m_vecRcmmWordList.clear();

	if(m_nSearchTimer) KillTimer(m_nSearchTimer);
	if(m_nRecommTimer) KillTimer(m_nRecommTimer);

	if( m_BackBrush.m_hObject )
		m_BackBrush.DeleteObject();

	DeleteObjects();
}

void CDlg_Search::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Search)
	DDX_Control(pDX, IDC_BTN_NETWORK_LOAD, m_btnNetworkLoad);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_edtSearch);
//	DDX_Control(pDX, IDC_BTN_SRCH_LOCAL, m_btnSrchLocal);
//	DDX_Control(pDX, IDC_BTN_SRCH_ALL, m_btnSrchAll);
	DDX_Control(pDX, IDC_BTN_SRCH_R_N, m_btnSrchRangeN);
	DDX_Control(pDX, IDC_BTN_SRCH_R, m_btnSrchRangeAllLocal);
	DDX_Control(pDX, IDC_BUTTON_SRCH, m_btnSearch);
	DDX_Control(pDX, IDC_BUTTON_TAG, m_btnTag);
	DDX_Control(pDX, IDC_BTN_NETWORK, m_btnNetwork);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_sKeyword);
	DDV_MaxChars(pDX, m_sKeyword, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Search, CDialog)
	//{{AFX_MSG_MAP(CDlg_Search)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)	
	ON_BN_CLICKED(IDC_BTN_SRCH_R_N, OnBtnSearchRangeNormal)
	ON_BN_CLICKED(IDC_BTN_SRCH_R, OnBtnSearchRange)
	ON_BN_CLICKED(IDC_BTN_NETWORK, OnBtnNetwork)
	ON_MESSAGE(UM_COMPLETECHAR, OnCompleteCharForEdit)
	ON_BN_CLICKED(IDC_BTN_NETWORK_LOAD, OnBtnNetworkLoad)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NETCNTCHANGED, OnNetCntChanged)
	ON_MESSAGE(WM_RECEIVEDRWORDS, OnReceivedRWords)
    ON_MESSAGE(WM_REDRAW_RCMMDWNDS, OnRedrawRcmmWnds)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Search message handlers
BOOL CDlg_Search::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CDlg_Search::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		if(pMsg->wParam == VK_RETURN)
		{
			 
			if(m_edtSearch.GetSafeHwnd())
			{
				if( m_edtSearch.m_hWnd == pMsg->hwnd )
				{
					OnButtonSearch();
					return TRUE;
				}
			} 

			// exploring
			return TRUE;			
		}
        if(pMsg->wParam ==  VK_TAB)
        {
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
        }
        break;
	default:
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
void CDlg_Search::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE(_T("CDlg_Search::OnSize\r\n"));
//	CDialog::OnSize(nType, cx, cy);

	MoveControls();

	if( m_bMakeInfoNSetRange == FALSE )
	{
		if( cx == BTN_DLG_SRCH_WIDTH )
		{
			MakeInfoNSetRange();
			m_bMakeInfoNSetRange = TRUE;
		}			
	}
	
}

BOOL CDlg_Search::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LoadBtnBitmaps();
	MoveControls();	 
	
	if( m_edtSearch.GetSafeHwnd())
	{
		m_edtSearch.SetParent((CWnd*)this);
		m_edtSearch.Init();
	}

	GetRecWord();
	
	SetCurrType(TYPE_SRCH_RCMMWORD);
	EnableNetwork(FALSE);
//	ShowControls();

	if( m_btnSearch.GetSafeHwnd() )
		m_btnSearch.ShowWindow(FALSE);  	

	m_bInit = TRUE;

	m_nNodeState = TYPE_SRCH_NETWORK_STATUS_0;
	m_edtSearch.EnableWindow(FALSE);
	m_btnSearch.EnableWindow(FALSE);

	MakeRecommWnd();

	m_nRecommTimer = SetTimer(TID_RECWORDVALID, ELPS_RECWORDVALID, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Search::MoveControls()
{
	CRect rectView;
	GetClientRect(&rectView);

	//Edit 외곽영역(라인부분 포함한 영역)
	m_rectedtSRchBg = rectView;
    m_rectedtSRchBg.left   = m_rectedtSRchBg.left + BTN_SEARCH_TARGET_WIDTH + INTERVAL_TARGET_BTN_CONNECT;
	m_rectedtSRchBg.top += 15; //6 + 9
	m_rectedtSRchBg.bottom = m_rectedtSRchBg.top + 22;

	int nLeft = m_rectedtSRchBg.left;
	int nTop = m_rectedtSRchBg.top;

	//검색 버튼 (일반)
    {
        CRect rt ( CPoint( 0, m_rectedtSRchBg.top ),
            CSize( BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT ) );

	    if( m_btnSrchRangeN.GetSafeHwnd() )
	    {
		    m_btnSrchRangeN.MoveWindow( rt );
//            m_btnSrchRangeN.ShowWindow( SW_HIDE );
	    }
    }
		
	//검색 버튼(범위 - 전체)
    {
        CRect rt ( CPoint( 0, m_rectedtSRchBg.top ),
            CSize( BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT ) );

	    if( m_btnSrchRangeAllLocal.GetSafeHwnd() )
	    {
		    m_btnSrchRangeAllLocal.MoveWindow( rt );
//            m_btnSrchRangeAllLocal.ShowWindow( SW_HIDE );
	    }
    }

	//전파버튼	
    {
        CRect rt ( CPoint( m_rectedtSRchBg.left, m_rectedtSRchBg.top ),
            CSize( BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT ) );

	    if( m_btnNetwork.GetSafeHwnd() )
	    {
		    m_btnNetwork.MoveWindow( rt );
//            m_btnNetwork.ShowWindow( SW_HIDE );
	    }
    }
		
	//로드수 버튼
    {
        CRect rt ( CPoint( m_rectedtSRchBg.left, m_rectedtSRchBg.top ),
            CSize( BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT ) );

        if( m_btnNetworkLoad.GetSafeHwnd() )
	    {
		    m_btnNetworkLoad.MoveWindow( rt );
//            m_btnNetworkLoad.ShowWindow( SW_HIDE );
	    }
    }

    //Go (Search) Btn
    {
        CRect rt ( CPoint( m_rectedtSRchBg.right - BTN_GO_WIDTH, m_rectedtSRchBg.top ),
            CSize( BTN_GO_WIDTH, BTN_GO_HEIGHT ) );

        if( m_btnSearch.GetSafeHwnd() )
	    {
		    m_btnSearch.MoveWindow( rt );
//            m_btnSearch.ShowWindow( SW_HIDE );
	    }
    }

	//Tag Btn
    {
        CRect rt ( CPoint( m_rectedtSRchBg.right - BTN_TAG_WIDTH, m_rectedtSRchBg.top ),
            CSize( BTN_TAG_WIDTH, BTN_TAG_HEIGHT ) );

        if( m_btnTag.GetSafeHwnd() )
	    {
		    m_btnTag.MoveWindow( rt );
//            m_btnTag.ShowWindow( SW_HIDE );
	    }
    }

	//Edit control
    {
        CRect rt ( CPoint( m_rectedtSRchBg.left + BTN_CONNECTION_WIDTH, m_rectedtSRchBg.top + 4 ),
            CSize( m_rectedtSRchBg.Width() - ( BTN_CONNECTION_WIDTH + BTN_GO_WIDTH ), BTN_INEDIT_HEIGHT ) );
	    if( m_edtSearch.GetSafeHwnd())
	    {
		    m_edtSearch.MoveWindow( rt );
//            m_edtSearch.ShowWindow( SW_HIDE );
	    }
    }

/*
	//'전체' 버튼
    {
        CRect rt ( CPoint( 0, m_rectedtSRchBg.bottom + 4 ),
            CSize( BTN_GLOBAL_WIDTH, BTN_GLOBAL_HEIGHT ) );

        if( m_btnSrchAll.GetSafeHwnd() )
	    {
		    m_btnSrchAll.MoveWindow( rt );
//            m_btnSrchAll.ShowWindow( SW_HIDE );
	    }
    }

    //'로컬' 버튼 
    {
        CRect rt ( CPoint( 50, m_rectedtSRchBg.bottom + 4 ),
            CSize( BTN_LOCAL_WIDTH, BTN_LOCAL_HEIGHT ) );

        if( m_btnSrchLocal.GetSafeHwnd() )
	    {
		    m_btnSrchLocal.MoveWindow( rt );
//            m_btnSrchLocal.ShowWindow( SW_HIDE );
	    }
    }
*/

    MakeInfoNSetRange();

	Invalidate();
}

void CDlg_Search::DrawItems(CDC *pDC)
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
//	bufferDC.BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), pDC, 0, 0, SRCCOPY);

	DrawBg(&bufferDC, &memDC);
	if( m_nCurrType == TYPE_SRCH_LOAD_DATA ) //로드수 
		DrawLoadNumData(&bufferDC);
	else if (m_nCurrType == TYPE_SRCH_RCMMWORD && m_bShowRecommend && m_nValidRecomm) //추천 검색어
		DrawRcmmWordTitle(&bufferDC);

	pDC->BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), &bufferDC, 0, 0, SRCCOPY);

	bufferDC.SelectObject(pOldBmp);

	bmpBuffer.DeleteObject();
	bufferDC.DeleteDC();
	memDC.DeleteDC();
}

void CDlg_Search::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	CRect rect, rectImage;
	GetClientRect(&rect);

#ifdef USE_CLIPRECOMMEND
	if(m_bShowRecommend)
	{
		CRgn rgnTmp;
		rgnTmp.CreateRectRgn(0,0,0,0);
		pBufferDC->SelectClipRgn(&m_rgnClip);
	}
#endif

	//////////////////////////////////////////////////////
	// Fill Background color
	
	//배경 이미지
	CBitmap* pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpSrchBg);
	pBufferDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), pMemDC,
		0, 0, m_bitmapSrchBg.bmWidth, m_bitmapSrchBg.bmHeight, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	//입력창 외곽 배경색
	pBufferDC->FillSolidRect(&m_rectedtSRchBg, CLR_WHITE);

    /*
	//입력창 외곽 Rect
	CPen* pOldPen = (CPen*)pBufferDC->SelectObject(&m_penedtRect);
	int nTop = m_rectedtSRchBg.top;
	int nBottom = m_rectedtSRchBg.bottom - 1;
	int nLeft = m_rectedtSRchBg.left;
	int nRight = m_rectedtSRchBg.right - 1;
	//LT - LB
	pBufferDC->MoveTo(nLeft, nTop);
	pBufferDC->LineTo(nLeft, nBottom);
	//LB - RB
	pBufferDC->MoveTo(nLeft, nBottom);
	pBufferDC->LineTo(nRight, nBottom);
	//RB - RT
	pBufferDC->MoveTo(nRight, nBottom);
	pBufferDC->LineTo(nRight, nTop);
	//RT - LT
	pBufferDC->MoveTo(nRight, nTop);
	pBufferDC->LineTo(nLeft, nTop);
	
	pBufferDC->SelectObject(pOldPen);
    */
}

BOOL CDlg_Search::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bSelfDraw)
	{
//		DrawItems(pDC);
		return FALSE;
	}

	return CDialog::OnEraseBkgnd(pDC);
}

//Go 버튼 클릭..(검색 )
void CDlg_Search::OnButtonSearch() 
{
	UpdateData(TRUE);

    m_sKeyword.TrimLeft(_T(" "));
    m_sKeyword.TrimRight(_T(" "));

    if ( isEnglishString(m_sKeyword) && m_sKeyword.GetLength() < MIN_SEARCHLENGTH_ENG)
    {
		CString sMsg;
		sMsg.Format(STR_SEARCH_LENGTHLIMIT, MIN_SEARCHLENGTH_ENG, MIN_SEARCHLENGTH_ETC);
		//AfxMessageBox( sMsg );
        FishMessageBox( (LPCTSTR)sMsg );
		return;
    }   
    else    
    if ( !isEnglishString(m_sKeyword) && m_sKeyword.GetLength() < MIN_SEARCHLENGTH_ETC )    
    {
		CString sMsg;
		sMsg.Format(STR_SEARCH_LENGTHLIMIT, MIN_SEARCHLENGTH_ENG, MIN_SEARCHLENGTH_ETC);
//		AfxMessageBox( sMsg );
        FishMessageBox( (LPCTSTR)sMsg );
		return;
	}

	TERM t;

	if(!m_bSearchTime) return;

	if(m_nSrchRange == TYPE_SRCH_RANGE_ALL)
	{
		theApp.m_spSM->Search(m_sKeyword, NET_SEARCH, t);
		//검색어 History 에 추가 
		AddKeyword(m_sKeyword);
	}
	else
	{
		theApp.m_spSM->Search(m_sKeyword, LOCAL_SEARCH, t);
	}

	m_bSearchTime = FALSE;
	m_nSearchTimer = SetTimer(TID_SEARCHINTERVAL, 500, NULL);

	ClearEdit();
}

//////////////////////////////////////////////////////////////////////////
// NAME : SetCurrType
// PARM : void
// RETN : void
// DESC : 현재 Type 을 Setting 
// DATE : 2005-12-28 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::SetCurrType(int nType)
{
	ASSERT( nType >= 0 );
	m_nCurrType = nType;
}

//////////////////////////////////////////////////////////////////////////
// NAME : GetCurrType
// PARM : void
// RETN : void
// DESC : 현재 Type 을 반환 
// DATE : 2005-12-28 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
int CDlg_Search::GetCurrType()
{
	return m_nCurrType;
}



//////////////////////////////////////////////////////////////////////////
// NAME : OnBtnSearchRangeNormal
// PARM : void
// RETN : void
// DESC : 검색버튼((Normal 상태)클릭 => 검색버튼(전체/로컬로 바꿔준다.)
// DATE : 2005-12-28 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::OnBtnSearchRangeNormal() 
{
	SetSrchRange(TYPE_SRCH_RANGE_LOCAL);
	LoadBmpSrchRangeBtn();	

//	SetCurrType(TYPE_SRCH_RANGE_BTN);	
    SetCurrType(TYPE_SRCH_RCMMWORD);
	ShowControls(); 
/*
    if( GetSrchRange() == TYPE_SRCH_RANGE_ALL )
		OnBtnSrchAll();
	else
		OnBtnSrchLocal();
*/
    Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// NAME : OnBtnSearchRange
// PARM : void
// RETN : void
// DESC : 검색버튼(전체/로컬 상태)클릭 => 검색버튼(Normal)로 바꿔준다.즉 추천검색어 상태로  
// DATE : 2005-12-28 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::OnBtnSearchRange() 
{
	SetSrchRange(TYPE_SRCH_RANGE_ALL);
	LoadBmpSrchRangeBtn();

    SetCurrType(TYPE_SRCH_RCMMWORD);	
	ShowControls(); 

    Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// NAME : OnBtnNetwork
// PARM : void
// RETN : void
// DESC : 전파버튼(|||) 클릭 => 로드수 나타내기 
// DATE : 2005-12-29 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::OnBtnNetwork() 
{
	//로드 수 가져오기..
	MakeLoadNumData();
	SetCurrType(TYPE_SRCH_LOAD_DATA);	
	ShowControls(); 
}

//////////////////////////////////////////////////////////////////////////
// NAME : OnBtnNetworkLoad
// PARM : void
// RETN : void
// DESC : 전파버튼(Load수) 클릭 => 추천검색어 상태로 바꾸기. 
// DATE : 2005-12-29 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::OnBtnNetworkLoad() 
{
    SetCurrType(TYPE_SRCH_RCMMWORD);
    ShowControls();
}



//////////////////////////////////////////////////////////////////////////
// NAME : ShowControls
// PARM : int nCurrType 
// RETN : void
// DESC : Type에 따라 Control Show
// DATE : 2005-12-28 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::ShowControls()
{
	if(m_bNetEnable)
	{
		switch(m_nCurrType)
		{	
		case TYPE_SRCH_RANGE_BTN :
			ShowRcmmWord(FALSE);
            if ( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
            {
                m_btnSrchRangeN.ShowWindow(TRUE);
                m_btnSrchRangeAllLocal.ShowWindow(FALSE);
            }
            else
            {
                m_btnSrchRangeN.ShowWindow(FALSE);
                m_btnSrchRangeAllLocal.ShowWindow(TRUE);
            }
			m_btnNetworkLoad.ShowWindow(FALSE);

//			m_btnSrchAll.ShowWindow(TRUE);
//			m_btnSrchLocal.ShowWindow(TRUE);
			m_btnNetwork.ShowWindow(TRUE);
			break;
		case TYPE_SRCH_LOAD_DATA :
			ShowRcmmWord(FALSE);
            if ( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
            {
                m_btnSrchRangeN.ShowWindow(TRUE);
                m_btnSrchRangeAllLocal.ShowWindow(FALSE);
            }
            else
            {
                m_btnSrchRangeN.ShowWindow(FALSE);
                m_btnSrchRangeAllLocal.ShowWindow(TRUE);
            }

            m_btnNetwork.ShowWindow(FALSE);
//			m_btnSrchAll.ShowWindow(FALSE);
//			m_btnSrchLocal.ShowWindow(FALSE);

			m_btnNetworkLoad.ShowWindow(TRUE);
			break;
		case TYPE_SRCH_RCMMWORD :
		default :
//			m_btnSrchAll.ShowWindow(FALSE);
// 			m_btnSrchLocal.ShowWindow(FALSE);
			m_btnNetworkLoad.ShowWindow(FALSE);

            if ( m_nNodeState == TYPE_SRCH_NETWORK_STATUS_0)
    			ShowRcmmWord(FALSE);
            else
                ShowRcmmWord(TRUE);

            if ( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
            {
                m_btnSrchRangeN.ShowWindow(TRUE);
                m_btnSrchRangeAllLocal.ShowWindow(FALSE);
            }
            else
            {
                m_btnSrchRangeN.ShowWindow(FALSE);
                m_btnSrchRangeAllLocal.ShowWindow(TRUE);
            }
			m_btnNetwork.ShowWindow(TRUE);
			break;
		}
	}
	else
	{
		switch(m_nCurrType)
		{
		case TYPE_SRCH_RANGE_BTN:
            if ( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
            {
                m_btnSrchRangeN.ShowWindow(TRUE);
                m_btnSrchRangeAllLocal.ShowWindow(FALSE);
            }
            else
            {
                m_btnSrchRangeN.ShowWindow(FALSE);
                m_btnSrchRangeAllLocal.ShowWindow(TRUE);
            }
// 			m_btnSrchLocal.ShowWindow(TRUE);
			break;

        default:
            if ( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
            {
                m_btnSrchRangeN.ShowWindow(TRUE);
                m_btnSrchRangeAllLocal.ShowWindow(FALSE);
            }
            else
            {
                m_btnSrchRangeN.ShowWindow(FALSE);
                m_btnSrchRangeAllLocal.ShowWindow(TRUE);
            }
// 			m_btnSrchLocal.ShowWindow(FALSE);
		}
// 		m_btnSrchAll.ShowWindow(FALSE);
		m_btnNetwork.ShowWindow(TRUE);
		m_btnNetworkLoad.ShowWindow(FALSE);
		ShowRcmmWord(FALSE);
	}

	if( m_edtSearch.GetSafeHwnd())
	{
		m_edtSearch.SetSel(0, 0);
	}
	UpdateWindow();

	MoveControls();
}


//////////////////////////////////////////////////////////////////////////
// NAME : AddKeyword
// PARM : CString strKeyword : 저장하려는 Keyword
// RETN : void
// DESC : 입력한 Keyword의 History를 저장하는 함수 
// DATE : 2006-01-01 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::AddKeyword(CString strKeyword)
{
	if( strKeyword.GetLength() == 0 )
	{
		return ;
	}
	m_edtSearch.AddSearchString(strKeyword);
}

//////////////////////////////////////////////////////////////////////////
// NAME : ClearEdit
// PARM : void
// RETN : void
// DESC : 검색어 입력창을 초기화 
// DATE : 2006-01-01 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CDlg_Search::ClearEdit()
{
	m_sKeyword = _T("");
	UpdateData(TRUE);
}


//////////////////////////////////////////////////
// NAME : OnCompleteCharForEdit
// PARM : WPARAM wParam, LPARAM lParam
// RETN : void
// DESC : edtSearch로 부터 온 한문자 입력 완료 메시지 핸들러 함수
// Date : 2005-08-02 coded by happyune, origin
//////////////////////////////////////////////////
void CDlg_Search::OnCompleteCharForEdit(WPARAM wParam, LPARAM lParam)
{ 	
	CString strWord;
	m_edtSearch.GetWindowText(strWord);
	if( strWord.IsEmpty() == TRUE )
	{
		if( m_btnSearch.GetSafeHwnd())
			m_btnSearch.ShowWindow(SW_HIDE);
		if(m_btnTag.GetSafeHwnd())
			m_btnTag.ShowWindow(SW_SHOW);
	}
	else
	{
		if( m_btnSearch.GetSafeHwnd())
			m_btnSearch.ShowWindow(SW_SHOW);		
		if( m_btnTag.GetSafeHwnd() )
			m_btnTag.ShowWindow(SW_HIDE);
	} 
	UpdateData(TRUE); 
}




//////////////////////////////////////////////////
// NAME : DrawLoadNumData
// PARM : CDC *pBufferDC
// RETN : void
// DESC : 로드 수 나타내기 
// Date : 2005-08-02 coded by lizzy337, origin
//////////////////////////////////////////////////
void CDlg_Search::DrawLoadNumData(CDC *pBufferDC)
{
	pBufferDC->FillSolidRect(&m_rectLoadNumData, COLOR_SEARCH_VIEW_BG);

	CFont* pOldFont = NULL;
	pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontNormal);
	pBufferDC->SetTextColor(CLR_WHITE);
	pBufferDC->DrawText(m_strLoadCount, m_rectLoadNumData, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
	pBufferDC->SelectObject(pOldFont);
}

//////////////////////////////////////////////////
// NAME : DrawRcmmWordTitle
// PARM : CDC *pBufferDC
// RETN : voidi
// DESC : 추천 검색어 나타내기 
// Date : 2006-01 coded by lizzy337, origin
//////////////////////////////////////////////////
void CDlg_Search::DrawRcmmWordTitle(CDC *pBufferDC)
{
	pBufferDC->FillSolidRect(&m_rectSrchRcmmWord, COLOR_SEARCH_VIEW_BG);
	//pBufferDC->FillSolidRect(&m_rectSrchRcmmWord, COLORREF(RGB(255, 0, 0)));
	
	CFont* pOldFont = NULL;
	//"추천 검색어"
	pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontNormalB);
	pBufferDC->SetTextColor(CLR_WHITE);
	pBufferDC->DrawText(STR_RCMM_TITLE, m_rectSrchRcmmWordTitle, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
	pBufferDC->SelectObject(pOldFont); 
}

void CDlg_Search::InitObjects()
{
	m_fontNormal.CreateFont(12, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	m_fontNormalB.CreateFont(14, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	m_fontNormalBUl.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, TRUE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	m_bmpSrchBg.LoadBitmap(IDB_SRCH_DLG_BG);
	m_bmpSrchBg.GetBitmap(&m_bitmapSrchBg);
	m_penedtRect.CreatePen(PS_SOLID, 1, COLORREF(RGB(0x97, 0xb8, 0xc9)));
}

void CDlg_Search::DeleteObjects()
{
	if( m_fontNormal.m_hObject ) m_fontNormal.DeleteObject();
	if( m_fontNormalB.m_hObject ) m_fontNormalB.DeleteObject();
	if( m_fontNormalBUl.m_hObject ) m_fontNormalBUl.DeleteObject();
	if( m_bmpSrchBg.m_hObject ) m_bmpSrchBg.DeleteObject();
	if( m_penedtRect.m_hObject ) m_penedtRect.DeleteObject();
}


//////////////////////////////////////////////////
// NAME : OnBtnSrchAll
// PARM : void
// RETN : void
// DESC : '전체'버튼 클릭하기
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
/*
void CDlg_Search::OnBtnSrchAll() 
{
	SetSrchRange(TYPE_SRCH_RANGE_ALL);
	LoadBmpSrchRangeBtn();
}
*/
//////////////////////////////////////////////////
// NAME : OnBtnSrchLocal
// PARM : void
// RETN : void
// DESC : '로컬'버튼 클릭하기
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
/*
void CDlg_Search::OnBtnSrchLocal() 
{
	SetSrchRange(TYPE_SRCH_RANGE_LOCAL);
	LoadBmpSrchRangeBtn();	
}
*/
//////////////////////////////////////////////////
// NAME : SetSrchRange
// PARM : int nRange
// RETN : void
// DESC : 검색 Range Setting 하기 
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
void CDlg_Search::SetSrchRange(int nRange)
{
	if( nRange == m_nSrchRange) return;

	if( nRange == TYPE_SRCH_RANGE_ALL || nRange == TYPE_SRCH_RANGE_LOCAL)
		m_nSrchRange = nRange;
	else
		ASSERT(FALSE);

/*
			if( m_btnSrchAll.GetSafeHwnd())
				m_btnSrchAll.EnableWindow(m_bNetEnable && (nRange != TYPE_SRCH_RANGE_ALL));
		
		    if( m_btnSrchLocal.GetSafeHwnd() )
				m_btnSrchLocal.EnableWindow(m_bNetEnable && (nRange != TYPE_SRCH_RANGE_LOCAL));
*/
		
	if(m_nNodeState == TYPE_SRCH_NETWORK_STATUS_0)
	{
		if(nRange == TYPE_SRCH_RANGE_ALL)
		{
			m_edtSearch.EnableWindow(FALSE);
			m_btnSearch.EnableWindow(FALSE);
		}
		else
		{
			m_edtSearch.EnableWindow(TRUE);
			m_btnSearch.EnableWindow(TRUE);
		}
	}
}

//////////////////////////////////////////////////
// NAME : GetSrchRange
// PARM : void
// RETN : int
// DESC : 검색 Range 반환 
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
int CDlg_Search::GetSrchRange()
{
	return m_nSrchRange;
}

//////////////////////////////////////////////////
// NAME : LoadBmpSrchRangeBtn
// PARM : void
// RETN : void
// DESC : 검색-Range의 버튼의 이미지 Loading 하기 ( 전체/로컬 )
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
void CDlg_Search::LoadBmpSrchRangeBtn()
{
	if( m_btnSrchRangeAllLocal.GetSafeHwnd() == INVALID_HANDLE_VALUE )
	{
        TRACE(STR_SRCHRANGE_IMG_LOAD_FAILED);
		return ;
	}
	if( m_nSrchRange == TYPE_SRCH_RANGE_ALL )
	{			
		m_btnSrchRangeAllLocal.LoadBitmaps(IDB_SRCHR_BTN_N, IDB_SRCHR_BTN_P, IDB_SRCHR_BTN_O, IDB_SRCHR_BTN_N,
			BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT);	
	}
	else if ( m_nSrchRange == TYPE_SRCH_RANGE_LOCAL )
	{
		m_btnSrchRangeAllLocal.LoadBitmaps(IDB_SRCHL_BTN_N, IDB_SRCHL_BTN_P, IDB_SRCHL_BTN_O, IDB_SRCHL_BTN_N,
			BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT);
	}
}

//////////////////////////////////////////////////
// NAME : MakeRcmmWordList
// PARM : void
// RETN : void
// DESC : 추천 검색어 목록 만들기...
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
/*
 *	추천검색어를 유지하는 리스트가 있다는 조건하에 코딩한 것입니다.
    추천검색어의 갯수를 구하고 그 갯수만큼 루틴을 돌아서서
	추천검색어를 나타낼 윈도우(CRcmmWordWnd)를 생성합니다.
	생성한 윈도우는 vector(m_vecRcmmWordList)에서 유지합니다. 
 */
void CDlg_Search::MakeRcmmWordList()
{
	// Lock을 사용하는 대신 m_hThread가 떠 있으면 m_listWords를 사용하지 않는다.
	if( m_hThread || m_listWords.size() == 0) return;

	CRect rcClient;
	GetClientRect(&rcClient);

#ifdef USE_CLIPRECOMMEND
	m_rgnClip.SetRectRgn(&rcClient);
#endif

	CString strRcmmWord;
	CRect rectWord;
#ifdef USE_CLIPRECOMMEND
	CRgn rgnTmp;
	rgnTmp.CreateRectRgn(0, 0, 0, 0);
#endif
	int nBeforeLeft = m_rectSrchRcmmWordTitle.left;//"추천검색어 : " 길이
	nBeforeLeft += m_rectSrchRcmmWordTitle.Width();
	nBeforeLeft += 5;
	rectWord = m_rectSrchRcmmWordTitle;
	rectWord.left = nBeforeLeft;

	list<CString>::iterator it;
	std::vector< SmartPtr<CRcmmWordWnd> >::iterator wit;

	int nIndex = 0;

	it = m_listWords.begin();
	wit = m_vecRcmmWordList.begin();
	for( ;
		wit != m_vecRcmmWordList.end() && it != m_listWords.end(); ++wit, ++it)
	{

		for( ; it != m_listWords.end(); ++it )
		{
			strRcmmWord = (*it);
//			TRACE(_T("kEYWORD : %s\r\n"), strRcmmWord);
			if(strRcmmWord == _T("")) continue;
					
			//추천 검색어를 나타낼 영역의 크기를 구한다. 
			GetMsgRect(strRcmmWord, rectWord);		
			int nWidth = rectWord.Width();
			int nHeight = rectWord.Height();
			rectWord.left = nBeforeLeft;
			rectWord.top = m_rectSrchRcmmWordTitle.top;
			rectWord.bottom = rectWord.top + nHeight; ///+ 12;
			rectWord.right = rectWord.left + nWidth;

			// 너무 긴 추천 검색어는 제거한다.
			if(rectWord.Width() > m_rectSrchRcmmWord.Width() / 2)
				continue;

			++nIndex;

			//추천검색어 중에서 마지막으로 나타내는 검색어의 right를 제한
	//		if( rectWord.left <= m_rectSrchRcmmWord.right && 
	//			rectWord.right > m_rectSrchRcmmWord.right )
	//			rectWord.right = m_rectSrchRcmmWord.right;

			(*wit)->MoveWindow(rectWord);
			(*wit)->SetRcmmWord(strRcmmWord);
			(*wit)->SetRealRect(rectWord);
			//영역 밖의 것은 Hide 한다. 
			if( rectWord.right > m_rectSrchRcmmWord.right || !m_bShowRecommend)
			{
				(*wit)->ShowWindow(SW_HIDE);

#ifdef USE_CLIPRECOMMEND
				rgnTmp.SetRectRgn(&rectWord);
				m_rgnClip.CombineRgn(&m_rgnClip, &rgnTmp, RGN_DIFF);
#endif
			}
			else if(m_bShowRecommend)
			{
				(*wit)->ShowWindow(SW_NORMAL);
				(*wit)->Invalidate();
	//			CRect rc;
	//			pWnd->GetWindowRect(&rc);
	//			ScreenToClient(&rc);
			}
			
		
			nBeforeLeft += nWidth;
			nBeforeLeft += 3;//간격 

			break;
		}

		if(nBeforeLeft >= m_rectSrchRcmmWord.right )
			break;
	}

	while(wit != m_vecRcmmWordList.end())
	{
		(*wit)->ShowWindow(SW_HIDE);
		++wit;
	}

	m_nValidRecomm = nIndex;

	Invalidate();

}

//////////////////////////////////////////////////
// NAME : MakeLoadNumData
// PARM : void
// RETN : void
// DESC : 로드 수 정보 구성하기 //형식 : Search in 갯수 Urls & 갯수 Posts
// Date : 2006-01-03 coded by lizzy337, origin
//////////////////////////////////////////////////
void CDlg_Search::MakeLoadNumData()
{	
	//Url 수 가져오기
	//int nUrlCount = Url 수 가져오기()
	CString strUserCount;
	strUserCount = _T("");
	NumToString(theApp.m_nnetusercnt, strUserCount);
//	NumToString(100000, strUserCount);


	//Posts 수 가져오기
	//int nPostCount = Posts 수 가져오기();
	CString strPostCount;
	strPostCount = _T("");
	NumToString(theApp.m_nnetpostcnt, strPostCount);
//	NumToString(1000000000, strPostCount);

	m_strLoadCount = _T("");
	m_strLoadCount.Format(STR_DS_SEARCHPOST, strUserCount, strPostCount);
}


//============================================================================
// NAME : GetMsgRect()
// PARM : CString strWord : Drawing 하려는 String, CRect rect : 반환될 Rect
// RETN : void
// DESC : 문자열이 Drawing 될 영역의 넓이를 계산
// Date : 2005-11-28 coded by lizzy, origin
//		updated 2006-01-23 :: 영역 계산시 Bold Font를 이용한다. by moonknit
//============================================================================
void CDlg_Search::GetMsgRect(CString strWord, CRect &rect)
{
	CClientDC dc(NULL);
  
	// modified by moonknit
//	CFont* pOldFont = (CFont*)dc.SelectObject(&m_fontNormal);
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_fontNormalBUl);
	
	CRect rectTemp;
	rectTemp.left = 0;
	rectTemp.top = 0;
	rectTemp.bottom = 12;
	rectTemp.right = 1000;	
	
	UINT nFormat = DT_CALCRECT | DT_SINGLELINE | DT_LEFT | DT_NOPREFIX;
	
	dc.DrawText(strWord,  &rectTemp,  nFormat); // Drawing 될 영역 구하기	
	dc.SelectObject(pOldFont);
	
	rect = rectTemp;
}



 
//============================================================================
// NAME : ShowRcmmWord()
// PARM : BOOL bShow : TRUE => Show, FALSE => Hide
// RETN : void
// DESC : 추천검색어 목록 Show/Hide
// Date : 2006-01-04 coded by lizzy, origin
//============================================================================
void CDlg_Search::ShowRcmmWord(BOOL bShow)
{
	m_bShowRecommend = bShow;

	m_RcmmWordCriticalSection.Lock();
	int nSize = m_vecRcmmWordList.size();
	if( nSize <= 0 )
		return;
	std::vector< SmartPtr<CRcmmWordWnd> >::iterator itr;
	CRect rectTemp;
	if(bShow == TRUE)
	{
		for( itr = m_vecRcmmWordList.begin() ; itr != m_vecRcmmWordList.end() ; itr++ )
		{
			rectTemp = (*itr)->GetRealRect();
			if( rectTemp.right > m_rectSrchRcmmWord.right )
				(*itr)->ShowWindow(FALSE);
			else
				(*itr)->ShowWindow(TRUE);
		}
	}
	else
	{
		for( itr = m_vecRcmmWordList.begin() ; itr != m_vecRcmmWordList.end() ; itr++ )
		{
			(*itr)->ShowWindow(FALSE);			
		}
	}
	m_RcmmWordCriticalSection.Unlock();
}

void CDlg_Search::NumToString(int nCount, CString& strCount)
{
	NUMBERFMT nFmt = { 0, 0, 3, _T("."), _T(","), 1 };
	TCHAR szSize[20]={0}, tmp[20]={0};
	_stprintf(tmp, _T("%d"), nCount);
	::GetNumberFormat( NULL, NULL, tmp, &nFmt, szSize, 20 );
	strCount.Format(_T("%s"), szSize);	
}

void CDlg_Search::SetRcmmWord(CString strWord)
{
	if(m_edtSearch.IsWindowEnabled())
	{
		m_sKeyword = strWord;
		UpdateData(FALSE);

		SendMessage(UM_COMPLETECHAR, NULL, NULL);

		OnButtonSearch();
	}
}

void CDlg_Search::LoadBtnBitmaps()
{	
	if( m_btnSrchRangeN.GetSafeHwnd())
		m_btnSrchRangeN.LoadBitmaps(IDB_SRCHN_BTN_N, IDB_SRCHN_BTN_P, IDB_SRCHN_BTN_O, IDB_SRCHN_BTN_N,
		BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT);
	if( m_btnSrchRangeAllLocal.GetSafeHwnd())
		m_btnSrchRangeAllLocal.LoadBitmaps(IDB_SRCHR_BTN_N, IDB_SRCHR_BTN_P, IDB_SRCHR_BTN_O, IDB_SRCHR_BTN_N,
		BTN_SEARCH_TARGET_WIDTH, BTN_SEARCH_TARGET_HEIGHT);
	if( m_btnNetwork.GetSafeHwnd())
		m_btnNetwork.LoadBitmaps(IDB_SRCH_NETWORK_0_N, IDB_SRCH_NETWORK_0_P, IDB_SRCH_NETWORK_0_O, IDB_SRCH_NETWORK_0_N,
		BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
	if( m_btnNetworkLoad.GetSafeHwnd())
		m_btnNetworkLoad.LoadBitmaps(IDB_SRCH_NETWORK_LOADNUM_0_N, IDB_SRCH_NETWORK_LOADNUM_0_P,
		IDB_SRCH_NETWORK_LOADNUM_0_O, IDB_SRCH_NETWORK_LOADNUM_0_N,
		BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
	if( m_btnSearch.GetSafeHwnd() )
		m_btnSearch.LoadBitmaps(IDB_SRCH_GO, IDB_SRCH_GO, IDB_SRCH_GO, IDB_SRCH_GO,
		BTN_GO_WIDTH, BTN_GO_HEIGHT);
	if( m_btnTag.GetSafeHwnd() )
		m_btnTag.LoadBitmaps(IDB_SRCH_TAG, IDB_SRCH_TAG, IDB_SRCH_TAG, IDB_SRCH_TAG,
		BTN_TAG_WIDTH, BTN_TAG_HEIGHT);

/*
    if( m_btnSrchAll.GetSafeHwnd() )
		m_btnSrchAll.LoadBitmaps(IDB_SRCHR_ALL_BTN_N, IDB_SRCHR_ALL_BTN_P, IDB_SRCHR_ALL_BTN_O, 
		IDB_SRCHR_ALL_BTN_D, BTN_GLOBAL_WIDTH, BTN_GLOBAL_HEIGHT);
	if( m_btnSrchLocal.GetSafeHwnd())
		m_btnSrchLocal.LoadBitmaps(IDB_SRCHR_LOCAL_BTN_N, IDB_SRCHR_LOCAL_BTN_P, IDB_SRCHR_LOCAL_BTN_O, 
		IDB_SRCHR_LOCAL_BTN_D, BTN_LOCAL_WIDTH, BTN_LOCAL_HEIGHT);
*/

}




//============================================================================
// NAME : MakeInfoNSetRange()
// PARM : void
// RETN : void
// DESC : 추천검색어, 로드수 등을 로드하고, 영역을 설정한다. 
// Date : 2006-01-09 coded by lizzy, origin
//============================================================================
void CDlg_Search::MakeInfoNSetRange()
{
	//""추천 검색어" title 영역 구하기"	
	m_rectSrchRcmmWordTitle = m_rectedtSRchBg;
	m_rectSrchRcmmWordTitle.top = m_rectSrchRcmmWordTitle.bottom + 3;
	m_rectSrchRcmmWordTitle.bottom = m_rectSrchRcmmWordTitle.top + 14;
	CRect rectTemp;
	GetMsgRect(STR_RCMM_TITLE, rectTemp);
	m_rectSrchRcmmWordTitle.right = m_rectSrchRcmmWordTitle.left + rectTemp.Width();
	m_rectSrchRcmmWordTitle.right += 8;
	
	//추천검색어영역, 로드수 영역 구하기 
	m_rectLoadNumData = m_rectedtSRchBg;	
	m_rectLoadNumData.top = m_rectLoadNumData.bottom + 5;
	m_rectLoadNumData.bottom = m_rectLoadNumData.top + 11;
	m_rectSrchRcmmWord = m_rectLoadNumData;
	
	
	MakeRcmmWordList();
	MakeLoadNumData();
	
}

//============================================================================
// NAME : LoadNetworkBtnBitmap()
// PARM : int nStatus
// RETN : void
// DESC : 전파버튼-로드수의 이미지를 각 상황에 따라 Load 한다. 
// Date : 2006-01-09 coded by lizzy, origin
//============================================================================
void CDlg_Search::LoadNetworkBtnBitmap(int nStatus)
{
	if( m_btnNetwork.GetSafeHwnd() == INVALID_HANDLE_VALUE )
	{
		TRACE(STR_NETWORKBTN_IMG_LOAD_FAILED);
		return ;
	}	
		
	switch(nStatus)
	{		
		case TYPE_SRCH_NETWORK_STATUS_1 :
			m_btnNetwork.LoadBitmaps(IDB_SRCH_NETWORK_1_N, IDB_SRCH_NETWORK_1_P, IDB_SRCH_NETWORK_1_O, IDB_SRCH_NETWORK_1_N,
				BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
			break;
		case TYPE_SRCH_NETWORK_STATUS_2 :
			m_btnNetwork.LoadBitmaps(IDB_SRCH_NETWORK_2_N, IDB_SRCH_NETWORK_2_P, IDB_SRCH_NETWORK_2_O, IDB_SRCH_NETWORK_2_N,
				BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
			break;
		case TYPE_SRCH_NETWORK_STATUS_3 :
			m_btnNetwork.LoadBitmaps(IDB_SRCH_NETWORK_3_N, IDB_SRCH_NETWORK_3_P, IDB_SRCH_NETWORK_3_O, IDB_SRCH_NETWORK_3_N,
				BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
			break;
		case TYPE_SRCH_NETWORK_STATUS_0 :
			m_btnNetwork.LoadBitmaps(IDB_SRCH_NETWORK_0_N, IDB_SRCH_NETWORK_0_P, IDB_SRCH_NETWORK_0_O, IDB_SRCH_NETWORK_0_N,
				BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
		default :
			break;
	}
}

//============================================================================
// NAME : LoadNetworkLoadNumBtnBitmap()
// PARM : int nStatus
// RETN : void
// DESC : 전파버튼의 이미지를 각 상황에 따라 Load 한다. 
// Date : 2006-01-09 coded by lizzy, origin
//============================================================================
void CDlg_Search::LoadNetworkLoadNumBtnBitmap(int nStatus)
{
	if( m_btnNetworkLoad.GetSafeHwnd() == INVALID_HANDLE_VALUE )
	{
		TRACE(STR_NETWORKLOADBTN_IMG_LOAD_FAILED);
		return ;
	}

	switch(nStatus)
	{		
	case TYPE_SRCH_NETWORK_LOADNUM_STATUS_1 :
		m_btnNetworkLoad.LoadBitmaps(IDB_SRCH_NETWORK_LOADNUM_1_N, IDB_SRCH_NETWORK_LOADNUM_1_P, IDB_SRCH_NETWORK_LOADNUM_1_O, IDB_SRCH_NETWORK_LOADNUM_1_N,
			BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
		break;
	case TYPE_SRCH_NETWORK_LOADNUM_STATUS_2 :
		m_btnNetworkLoad.LoadBitmaps(IDB_SRCH_NETWORK_LOADNUM_2_N, IDB_SRCH_NETWORK_LOADNUM_2_P, IDB_SRCH_NETWORK_LOADNUM_2_O, IDB_SRCH_NETWORK_LOADNUM_2_N,
			BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
		break;
	case TYPE_SRCH_NETWORK_LOADNUM_STATUS_3 :
		m_btnNetworkLoad.LoadBitmaps(IDB_SRCH_NETWORK_LOADNUM_3_N, IDB_SRCH_NETWORK_LOADNUM_3_P, IDB_SRCH_NETWORK_LOADNUM_3_O, IDB_SRCH_NETWORK_LOADNUM_3_N,
			BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
		break;
	case TYPE_SRCH_NETWORK_LOADNUM_STATUS_0 :
		m_btnNetworkLoad.LoadBitmaps(IDB_SRCH_NETWORK_LOADNUM_0_N, IDB_SRCH_NETWORK_LOADNUM_0_P, IDB_SRCH_NETWORK_LOADNUM_0_O, IDB_SRCH_NETWORK_LOADNUM_0_N,
			BTN_CONNECTION_WIDTH, BTN_CONNECTION_HEIGHT);
	default :
		break;
	}
}
 
/**************************************************************************
 * method CDlg_Search::SetNetState
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-13
 *
 * @Description
 * 연결 노드의 개수가 변경되었을 때 호출된다.
 **************************************************************************/
void CDlg_Search::SetNetState(int nNodeCnt)
{
	int nNodeState = TYPE_SRCH_NETWORK_STATUS_0;
	if(nNodeCnt == 1)
		nNodeState = TYPE_SRCH_NETWORK_STATUS_1;
	else if(nNodeCnt == 2)
		nNodeState = TYPE_SRCH_NETWORK_STATUS_2;
	else if(nNodeCnt > 2)
		nNodeState = TYPE_SRCH_NETWORK_STATUS_3;

	if(m_nNodeState == nNodeState) return;

	if(m_nNodeState == TYPE_SRCH_NETWORK_STATUS_0)
	{
		m_edtSearch.EnableWindow(TRUE);
		m_btnSearch.EnableWindow(TRUE);
	}
	else if(nNodeState == TYPE_SRCH_NETWORK_STATUS_0)
	{
		if(m_nSrchRange == TYPE_SRCH_RANGE_ALL)
		{
			m_edtSearch.EnableWindow(FALSE);
			m_btnSearch.EnableWindow(FALSE);
		}
	}

	m_nNodeState = nNodeState;

	LoadNetworkBtnBitmap(nNodeState);
	LoadNetworkLoadNumBtnBitmap(nNodeState);
}

void CDlg_Search::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    DrawItems(&dc);	

    // Do not call CDialog::OnPaint() for painting messages
}

void CDlg_Search::OnNetCntChanged(WPARAM wParam, LPARAM lParam)
{
	MakeLoadNumData();
	Invalidate();
}

void CDlg_Search::EnableNetwork(BOOL bEnable)
{
	m_bNetEnable = bEnable;

	m_btnNetwork.EnableWindow(bEnable);
	m_btnNetworkLoad.EnableWindow(bEnable);

//	m_btnSrchRangeN.EnableWindow(bEnable);
//	m_btnSrchRangeAllLocal.EnableWindow(bEnable);

	if(!bEnable)
		SetSrchRange(TYPE_SRCH_RANGE_LOCAL);
	else
		SetSrchRange(TYPE_SRCH_RANGE_ALL);

	ShowControls();
}

HBRUSH CDlg_Search::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

//	if(!IsWindowEnabled())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor( (COLORREF) CLR_WHITE );
		return m_BackBrush;
	}

	return hbr;
}

void CDlg_Search::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TID_RECWORDVALID)
	{
		GetRecWord();
	}
	else if(nIDEvent == TID_SEARCHINTERVAL)
	{
		KillTimer(nIDEvent);
		m_nSearchTimer = NULL;
		m_bSearchTime = TRUE;
	}
	
	CDialog::OnTimer(nIDEvent);
}

/**************************************************************************
 * method CDlg_Search::GetRecWord
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-16
 *
 * @Description
 * 스레드를 이용하여 추천 검색어를 받아온다.
 **************************************************************************/
void CDlg_Search::GetRecWord()
{
	if(m_hThread)
	{
		if(++m_nThreadTry > MAX_THREADTRY)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
			m_nThreadTry = 0;
		}
		else
			return;
	}

//	RecommandThread((LPVOID) this);
//	return;

	m_nThreadTry = 0;

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) RecommandThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return;

	m_hThread = hThread;
	ResumeThread(hThread);
}

DWORD CDlg_Search::RecommandThread(LPVOID lpparam)
{
	CDlg_Search* p = (CDlg_Search*) lpparam;
	if(!p) return -1;

	CoInitialize(NULL);

	p->m_listWords.clear();

	BOOL result = FALSE;
	CEzXMLParser parser;
	HWND hWnd = p->GetSafeHwnd();

	TRACE(_T("Start Load Recommend Words\r\n"));
	try
	{
		result = parser.LoadXML(URL_RECOMMANDWORD, CEzXMLParser::LOAD_WEB);
	}
	catch (CException*)
	{
	}

	if(result)
	{
		IXMLDOMNodeListPtr pnodelist;
		IXMLDOMNodePtr pnode;

		try
		{
			pnodelist = parser.SearchNodes(XML_XPATH_KEYWORD);
			if(pnodelist != NULL && pnodelist->length > 0)
			{
				CString key;
				for(int i = 0; i < pnodelist->length; ++i)
				{
					pnode = pnodelist->item[i];
					key = GetNamedNodeText(XML_TITLE, &parser, pnode, 1);

					if(key != _T(""))
					{
						p->m_listWords.push_back(key);
					}
				}
			}
		}
		catch (CException&)
		{
		}
	}

	TRACE(_T("End Load Recommend Words\r\n"));

	CoUninitialize();
	p->m_hThread = NULL;

	// p->m_hThread = NULL 이후에 호출한다.
	if(p->m_listWords.size() > 0)
		::PostMessage(hWnd, WM_RECEIVEDRWORDS, NULL, NULL);

	return 0;
}

void CDlg_Search::OnReceivedRWords(WPARAM wParam, LPARAM lParam)
{
	MakeRcmmWordList();
}

BOOL CDlg_Search::isEnglishString(CString str)
{
    enum {
        MAX_STRING  =   1024,
    };

    BOOL RET    =   FALSE;

    TCHAR*  tsz =   str.GetBuffer(MAX_STRING);

    for (int idx = 0; idx < str.GetLength(); idx++)
    {
        BYTE* ch;
        ch  =   (BYTE*)&tsz[idx];

        if( ch[1]   ==  0 && _istalpha(ch[0]) )
        {
            RET =   TRUE;
            break;
        }
    }
    
    return RET;
}

void CDlg_Search::OnRedrawRcmmWnds(WPARAM wParam, LPARAM lParam)
{
    vector< SmartPtr<CRcmmWordWnd> >::iterator it  =   m_vecRcmmWordList.begin();
    for (; it != m_vecRcmmWordList.end(); it++)
    {
        (*it)->Invalidate();
    }
}

void CDlg_Search::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	OnRedrawRcmmWnds(NULL, NULL);
}

void CDlg_Search::MakeRecommWnd()
{
	std::vector< SmartPtr<CRcmmWordWnd> >::iterator wit;
	for(wit = m_vecRcmmWordList.begin(); wit != m_vecRcmmWordList.end(); ++wit)
	{
		(*wit)->ShowWindow(SW_HIDE);
	}
	m_vecRcmmWordList.clear();

	for(int i = 0 ; i < MAX_RECOMMWND; ++i)
	{
		SmartPtr<CRcmmWordWnd> pWnd = SmartPtr<CRcmmWordWnd> (new CRcmmWordWnd);
		pWnd->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, DLG_SRCH_RCMM_WND + i );		
		pWnd->InitView();
		pWnd->SetRcmmWord(_T(""));
		pWnd->SetRealRect(CRect(0, 0, 0, 0));
		pWnd->ShowWindow(SW_HIDE);
		
		
		//추천 검색어 목록을 유지한다. 
		m_vecRcmmWordList.push_back(pWnd);
	}
}