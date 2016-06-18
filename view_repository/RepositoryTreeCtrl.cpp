// RepositoryTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RepositoryTreeCtrl.h"
#include <wingdi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishTreeCtrl

#include "TreeSkinVerticalScrollbar.h"
#include "../ctrl/FishTransparentDragDlg.h"
#include "../MainFrm.h"
#include "../Dlg_Subscribe.h"
#include "../GlobalIconManager.h"
#include "../fish_common.h"
#include <algorithm>
using namespace std;

int GetHITEMImageID(int type, BOOL IsFocused, BOOL IsSelected, BOOL HasUnread);

map<int, CFishTreeCtrl::FISHCUSTOMICON>	CFishTreeCtrl::m_mapAniData;  // 애니메이션 하는 아이템을 잠시간 저장하기 위한 부분

#define _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_  if ( GetOSVersionType() > 4 ) {
#define _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_  }

CFishTreeCtrl::CFishTreeCtrl()
{
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    m_ctrlVerticalScrollBar =   NULL;
#endif

    m_pBMPManager       =   CFishBMPManager::getInstance();
	m_enumDragTYPE	    =   NONDRAG;
    m_pbmplistDragIMAGE	=   NULL;
    m_ctrlDragDlg   =   NULL;
    m_hPrevSelectedItem =   NULL;
	m_vecDragITEM.clear();
	_initialize();			// resource allocation
}

void CFishTreeCtrl::_initialize()
{
	//////////////////////////////////////////////////////////////////////////
	// 기본적으로 사용하는 폰트의 스타일을 정의한다.
	m_ftGroupDefault.CreateFont(FISH_REPOSITORY_TREE_ITEM_GROUP_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
	m_ftGroupSelect.CreateFont(FISH_REPOSITORY_TREE_ITEM_GROUP_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

    m_ftPostQuantityDefault.CreateFont(FISH_REPOSITORY_TREE_ITEM_POST_QUANTITY_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    m_ftPostQuantitySelect.CreateFont(FISH_REPOSITORY_TREE_ITEM_POST_QUANTITY_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

    m_ftChannelDefault.CreateFont(FISH_REPOSITORY_TREE_ITEM_CHANNEL_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    m_ftChannelSelect.CreateFont(FISH_REPOSITORY_TREE_ITEM_CHANNEL_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    m_ctrlVerticalScrollBar =   new CTreeSkinVerticalScrollbar;
#endif
}

void CFishTreeCtrl::_initScrollBar()
{
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    //another way to hide scrollbars
	CWnd* pParent = GetParent();

	//Create scrollbars at runtime
	m_ctrlVerticalScrollBar->Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,CRect(0,0,0,0), pParent);
	m_ctrlVerticalScrollBar->pTree = this;

	PositionScrollBars();
#endif
    return;
}

void CFishTreeCtrl::PositionScrollBars()
{
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    
    enum { SCROLLBAR_WIDTH=12, SCROLLBAR_HEIGHT=12};

	CWnd* pParent = GetParent();
	
	CRect windowRT;
	GetWindowRect(&windowRT);

	int nTitleBarHeight = 0;
	
	if(pParent->GetStyle() & WS_CAPTION)
		nTitleBarHeight = GetSystemMetrics(SM_CYSIZE);

    // WINDOW SYSTEM VALUE
	int nDialogFrameHeight = 0;
	int nDialogFrameWidth = 0;
    int cxvs    =   0;
    int cyvs    =   0;
    int cxhs    =   0;
    int cyhs    =   0;

    if (pParent->GetStyle() & WS_VSCROLL)   {
        cxvs = GetSystemMetrics (SM_CXVSCROLL);
        cyvs = GetSystemMetrics (SM_CYVSCROLL);
    }

    if (pParent->GetStyle() & WS_HSCROLL)   {
        cxhs = GetSystemMetrics (SM_CXHSCROLL);
        cyhs = GetSystemMetrics (SM_CYHSCROLL);
    }
    
	if((pParent->GetStyle() & WS_BORDER))	{
		nDialogFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);
		nDialogFrameWidth = GetSystemMetrics(SM_CYDLGFRAME);
	}
	
	if(pParent->GetStyle() & WS_THICKFRAME)    {
		nDialogFrameHeight+=1;
		nDialogFrameWidth+=1;
	}
	

    windowRT.InflateRect(nDialogFrameWidth,
        nTitleBarHeight+nDialogFrameHeight,
        nDialogFrameWidth,
        nTitleBarHeight+nDialogFrameHeight);

    // SCROLLBAR SIZE SET
    CRect scrollbarRT   =   windowRT;
    pParent->ScreenToClient(scrollbarRT);
	CRect vBar(scrollbarRT.right-nDialogFrameWidth - SCROLLBAR_WIDTH, 
        scrollbarRT.top-nTitleBarHeight-nDialogFrameHeight - SCROLLBAR_HEIGHT + 1 + 12,
        scrollbarRT.right+0-nDialogFrameWidth, 
        scrollbarRT.bottom-nTitleBarHeight-nDialogFrameHeight);

    // SCROLLBAR WINDOWS RESIZING
	m_ctrlVerticalScrollBar->SetWindowPos(NULL,vBar.left,vBar.top,vBar.Width(),vBar.Height(),SWP_NOZORDER);
	m_ctrlVerticalScrollBar->UpdateThumbPosition();

    int nMin, nMax;
    GetScrollRange(SB_VERT, &nMin, &nMax);
    if ( nMax <= GetScrollLimit(SB_VERT) )
    {
        CRect treeRegionRT   =   windowRT;
	    ScreenToClient(&treeRegionRT);
        HRGN iehrgn = CreateRectRgn(treeRegionRT.left, treeRegionRT.top, treeRegionRT.right, treeRegionRT.bottom);
        m_ctrlVerticalScrollBar->ShowWindow(SW_HIDE);
        SetWindowRgn(iehrgn, false);
    }   else    {
        CRect treeRegionRT   =   windowRT;
	    ScreenToClient(&treeRegionRT);
        HRGN iehrgn = CreateRectRgn(treeRegionRT.left-nDialogFrameWidth, 
            treeRegionRT.top-nTitleBarHeight-nDialogFrameHeight, 
            treeRegionRT.right-nDialogFrameWidth - SCROLLBAR_WIDTH - 1,
            treeRegionRT.bottom-nTitleBarHeight-nDialogFrameHeight - SCROLLBAR_HEIGHT);
        m_ctrlVerticalScrollBar->ShowWindow(SW_NORMAL);
        SetWindowRgn(iehrgn, false);
    }
#endif
    return;
}

CFishTreeCtrl::~CFishTreeCtrl()
{
	_finalize();
}

void CFishTreeCtrl::_finalize()
{
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    if (m_ctrlVerticalScrollBar)    delete m_ctrlVerticalScrollBar;
#endif
	if (m_ftGroupDefault.m_hObject)		m_ftGroupDefault.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFishTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CFishTreeCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomDraw )	
    ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick )	
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDBClick )
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishTreeCtrl message handlers

/************************************************************************
OnNMCustomDraw	객별 아이템을 독자적으로 그리게 만든다. NW_CUSTOMDRAW 메시지를 처리한다.
				메시지 맵은 직접 추가해야함.
@param  : 
@return : 
@remark : 
@author : eternalbleu (youngchang)
@history:
    created at 2005/10/27
    refactoring at 2005/11/15
************************************************************************/
void CFishTreeCtrl::OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMTVCUSTOMDRAW* pLVCD = reinterpret_cast<NMTVCUSTOMDRAW*>( pNMHDR ); 
    *pResult = CDRF_DODEFAULT; 

    switch( pLVCD->nmcd.dwDrawStage )
	{
		case CDDS_PREPAINT:
            {
                *pResult = CDRF_NOTIFYITEMDRAW;
            }
			break;

		case CDDS_ITEMPREPAINT:
            {
                HTREEITEM hITEM = (HTREEITEM)pLVCD->nmcd.dwItemSpec;
                if (!hITEM) return;

                CDC* dc = CDC::FromHandle(pLVCD->nmcd.hdc);

                CRect rect;
                GetItemRect(hITEM, &rect, FALSE);
                CMemDC pDC(dc, &rect);
                
                DWORD dwData = GetItemData( (HTREEITEM)(pLVCD->nmcd.dwItemSpec));
                animate_UPDATE( dwData);               // 애니메이션 정보의 업데이트
                
                customdraw_RootBkgnd(hITEM, pDC);    //BEGIN: item background drawing
                customdraw_Selected(hITEM, pDC);     //BEGIN: 개별 아이템이 선택되었을 경우에 효과를 준다.
                customdraw_Text(hITEM, pDC);         //BEGIN: 개별 아이템의 텍스트를 그려준다.
                customdraw_Icon(hITEM, pDC);         //BEGIN: 지정된 배경 색을 이용해서 한장의 이미지로 투명한 아이콘을 그린다.
                customdraw_DropHightlight(hITEM, pDC);    //BEGIN: 아이템 이동시에 이동지역의 확실한 구분을 위해서 하단에 그림을 그린다.
                *pResult = CDRF_SKIPDEFAULT;		//Your application drew the item manually. The control will not draw the item. This occurs when dwDrawStage equals CDDS_ITEMPREPAINT.
            }
			break;
		default:
			break;
	}
}

/************************************************************************
customdraw_RootBkgnd 루트 아이콘의 배경 그림을 그린다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/14: 배경의 크기에 맞도록 이미지가 resize 하도록 변경함.
************************************************************************/
void CFishTreeCtrl::customdraw_RootBkgnd(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
    if ( !GetParentItem(hITEM) )    // COND : 루트 아이콘 이라면
    {
        //////////////////////////////////////////////////////////////////////////
        // 이미지 로드
        BITMAP bm;
        CBitmap*     pBMP =   NULL;

        if(GetItemState(hITEM, TVIS_SELECTED)  &   TVIS_SELECTED)   // COND : 선택된 상태라면 1번, 아니라면 0번을 로드
        {
            pBMP =   m_pBMPManager->getGroupBk(1);
        }   else    {
            pBMP =   m_pBMPManager->getGroupBk(0);
        }
        pBMP->GetBitmap(&bm);
        
        //////////////////////////////////////////////////////////////////////////
        // 이미지 드로윙
        CRect rectBg;
        GetItemRect(hITEM, rectBg, FALSE);

        CDC buffDC;
        buffDC.CreateCompatibleDC(pDC);
        CBitmap* pOldBitmap = buffDC.SelectObject( pBMP );

        pDC->StretchBlt(rectBg.left, rectBg.top, rectBg.Width(), rectBg.Height(), &buffDC, 2, 0, bm.bmWidth-4, bm.bmHeight, SRCCOPY);

        pDC->BitBlt(rectBg.left, rectBg.top, 2, bm.bmHeight, &buffDC, 0, 0, SRCCOPY);
        pDC->BitBlt(rectBg.right - 2, rectBg.top, 2, bm.bmHeight, &buffDC, bm.bmWidth-2, 0, SRCCOPY);

        //////////////////////////////////////////////////////////////////////////
        // 핸들 해제
        buffDC.SelectObject(pOldBitmap);
    }
}

/************************************************************************
customdraw_Selected		선택 아이템의 배경에 특수 효과를 주기 위해서 만듦
@param  : 
@return : 
@remark : 
	원래는 배경 혹은 기타 특수효과를 주기 위해서 만들었지만 현재에는 그다지
	쓸모가 있어 보이지 않는 녀석임
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18 : CREATE
    2005/12/14 : 다중 아이콘의 선택시 배경색을 지운다.
    2005/12/22 : 배경중 일부분만 지운다.
************************************************************************/
void CFishTreeCtrl::customdraw_Selected(HTREEITEM hITEM, CDC* pDC)
{
    //////////////////////////////////////////////////////////////////////////
    // 아이템이 선택된 상태의 효과를 그린다.
    CRect itemRT;
    GetItemRect(hITEM, &itemRT, FALSE);

    if ( GetItemState(hITEM, TVIS_SELECTED) && TVIS_SELECTED)   // COND : 선택된 아이템이라면
    {
        if ( GetDropHilightItem()   ==  hITEM && find(m_vecDragITEM.begin(), m_vecDragITEM.end(), hITEM) == m_vecDragITEM.end())
        {
            if ( GetParentItem(hITEM) )   {
                pDC->FillRect(&itemRT, &CBrush(FISH_REPOSITORY_BG_COLOR) );
            }
        }   else    {
            if ( GetParentItem(hITEM) )   {
                pDC->FillRect(&itemRT, &CBrush(FISH_REPOSITORY_BG_COLOR_S));
                if ( GetFocus() != this )
                    pDC->FillRect(&itemRT, &CBrush(FISH_REPOSITORY_BG_NONFOCUS_S));
            }
        }
    } else {
        if ( GetParentItem(hITEM) )   {
            pDC->FillRect(&itemRT, &CBrush(FISH_REPOSITORY_BG_COLOR) );
        }
    }
}

/************************************************************************
customdraw_Text		커스텀 드로우에서 텍스트를 출력하는 루틴
@param  : 
@return : 
@remark : 
	기본적으로 선택과 비선택시의 행위를 분리함.
	비선택시에는 아이템의 Drag 상태에 따라서 다른 모양으로 출력함
	(이용할 가능성이 높은 소스 -_-;; 너무나도 치기 귀찮다. -_-;;;)
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18  :   CREATED
    2005/12/13  :   dc setbkmode 설정을 opaque 로 변경하여 다중 선택시 이상한 드로윙 문제 해결함.
************************************************************************/
void CFishTreeCtrl::customdraw_Text(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
	BOOL expand = FALSE;

    //////////////////////////////////////////////////////////////////////////
    // 아이템의 텍스트를 그린다.
    BOOL bSelect    =   FALSE;
    if ( GetItemState(hITEM, TVIS_SELECTED) & TVIS_SELECTED )
        bSelect =   TRUE;

    COLORREF    fgClr, fgCnfClr;
    CFont       *pTextFont, *pCountFont, *pOldFont;
	CDlg_Subscribe* pParent = NULL;
    auto_ptr<FISH_REPITEM> it  =    auto_ptr<FISH_REPITEM>(new FISH_CHANNEL(GetItemData(hITEM), _T("")) );
    {
        pParent    =   (CDlg_Subscribe*)GetParent();
        if(!pParent || !pParent->FindItem(it->id, it)) return;
    
        if (GetParentItem(hITEM)) {
	        if ( bSelect ) {
                pTextFont   =   &m_ftChannelSelect;
                pCountFont  =   &m_ftPostQuantitySelect;
                fgClr   =   FISH_REPOSITORY_FG_COLOR_S;
                fgCnfClr    =   FISH_REPOSITORY_READCOUNT_FG_COLOR_S;
	        } else {
                pTextFont   =   &m_ftChannelDefault;
                pCountFont  =   &m_ftPostQuantityDefault;
                fgCnfClr    =   FISH_REPOSITORY_READCOUNT_FG_COLOR_N;
                if (it->unreadcnt == 0)
                    fgClr   =   FISH_REPOSITORY_CHANNEL_FG_COLOR1_N;
                else
                    fgClr   =   FISH_REPOSITORY_CHANNEL_FG_COLOR2_N;
	        }
        }   else    {
	        if ( bSelect ) {
                pTextFont   =   &m_ftGroupSelect;
                pCountFont  =   &m_ftPostQuantitySelect;
                fgClr   =   FISH_REPOSITORY_GRP_FG_COLOR2_N;
                fgCnfClr    =   FISH_REPOSITORY_READCOUNT_FG_COLOR_S;
	        } else {
                pTextFont   =   &m_ftGroupDefault;
                pCountFont  =   &m_ftPostQuantityDefault;
                fgCnfClr    =   FISH_REPOSITORY_READCOUNT_FG_COLOR_N;
                fgClr   =   FISH_REPOSITORY_GRP_FG_COLOR2_N;
	        }
        }
    }
    
//////////////////////////////////////////////////////////////////////////
// 현재 채널의 읽지 않은 포스트 개수와 기타 정보에 관한 사항을 기록한다.
    CString strItemText( _T("") ), strUnreadQ( _T("") );
    {
        if (
//			it->rep_type == REP_CHANNEL && 
			it->unreadcnt != 0)  
			strUnreadQ.Format(_T("(%d)"), it->unreadcnt);
        
        strItemText =   GetItemText(hITEM);
    }
    
    CRect itemRT(0, 0, 0, 0), textRT(0, 0, 0, 0), unreadQRT(0, 0, 0, 0), etcRT(0, 0, 0, 0);
    {
        pOldFont    =   pDC->SelectObject(pTextFont);
        if ( paramRT )    itemRT  =   paramRT;    else    GetItemRect(hITEM, &itemRT, FALSE);

		// right 상한을 줄인다.
        itemRT.DeflateRect(0, 0, 6, 0);
        pDC->DrawText(strItemText, &textRT, DT_CALCRECT|DT_EDITCONTROL);

        pDC->SelectObject(pCountFont);
        pDC->DrawText(strUnreadQ, &unreadQRT, DT_CALCRECT|DT_EDITCONTROL);
        pDC->SelectObject(pOldFont);

		// Tooltip Hit 처리에서 사용할 변수
		it->unreadstrwidth = unreadQRT.Width();
    }

    {
        enum {
            CHANNEL_NAME_UNREAD_Q_INTERVAL  =   3,
        };

        int nTextWidth  =   textRT.Width();
        int nUnreadWidth  =   unreadQRT.Width();

        if ( GetParentItem(hITEM) ) {
            textRT.left	= FISH_REPOSITORY_ITEM_TEXT_LEFT_MARGIN + FISH_CHANNEL_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT;
        }   else  {
            textRT.left	= FISH_REPOSITORY_ITEM_TEXT_LEFT_MARGIN + FISH_GROUP_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT;
        }


		if (GetParentItem(hITEM) == NULL)
        {
			int rem = nUnreadWidth;
//			if( !(GetItemState(hITEM, TVIS_EXPANDED) & TVIS_EXPANDED) 
//				&&GetChildItem(hITEM) != NULL )
			rem += GRP_EXPANDED_IMPACT_BMP_WIDTH+5;

			if ( textRT.left + nTextWidth + rem
				>= itemRT.Width() )  {
				textRT.right    =   itemRT.right - rem;
			}   else    {
				textRT.right    =   textRT.left +   nTextWidth; //textRT.left + nTextWidth;
			}
		}
		else
		{
			if ( textRT.left + nTextWidth + nUnreadWidth >= itemRT.Width() )  {
				textRT.right    =   itemRT.right - nUnreadWidth;
			}   else    {
				textRT.right    =   textRT.left +   nTextWidth; //textRT.left + nTextWidth;
			}
		}

        textRT.top      =   itemRT.top;
        textRT.bottom   =   itemRT.bottom;

        unreadQRT.left  =   textRT.right;
        unreadQRT.right =   textRT.right + nUnreadWidth;
        unreadQRT.top   =   itemRT.top;
        unreadQRT.bottom=   itemRT.bottom;

    }

    //////////////////////////////////////////////////////////////////////////
    pDC->SetBkMode(TRANSPARENT);
    pOldFont    =   pDC->SelectObject(pTextFont);
    pDC->SetTextColor(fgClr);
    pDC->DrawText(strItemText, &textRT, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
    pDC->SelectObject(pOldFont);

    pOldFont    =   pDC->SelectObject(pCountFont);
    pDC->SetTextColor(fgCnfClr);
    pDC->DrawText(strUnreadQ, &unreadQRT, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );

    pDC->SelectObject(pOldFont);


    //////////////////////////////////////////////////////////////////////////
    // GRP 아이콘의 EXPAND, UNEXPAND 표시
    {
        if (GetParentItem(hITEM) == NULL)
        {
            if( !(GetItemState(hITEM, TVIS_EXPANDED)   &   TVIS_EXPANDED) &&
                GetChildItem(hITEM) != NULL )
				expand = TRUE;

            {
                CRect expandRT(0, 0, 0, 0);
                expandRT    =   itemRT;
                expandRT.left   =   expandRT.right - GRP_EXPANDED_LEFT_PADDING;
                expandRT.right  =   expandRT.left + GRP_EXPANDED_IMPACT_BMP_WIDTH;

                CPoint pt;
                GetCursorPos(&pt);
                ScreenToClient(&pt);

                if ( GetItemState(hITEM, TVIS_SELECTED) & TVIS_SELECTED )
					drawExpandedMark(pDC, &expandRT, 1, expand);
                else
					drawExpandedMark(pDC, &expandRT, 0, expand);
			
            }
        }		

    }
}

/************************************************************************
GetHITEMImageID 각 아이템의 특성에 따른 아이콘을 선택한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
int GetHITEMImageID(int type, BOOL IsFocused, BOOL IsSelected, BOOL HasUnread)
{
	int nIndex = 0;
	switch(type)
	{
	case CT_SCRAP:
		if(IsSelected)
		{
			if(IsFocused) nIndex = 7;
			else nIndex = 11;
		}
		else nIndex = 2;
		break;
	case CT_KEYWORD:
		if(IsSelected)
		{
			if(IsFocused) nIndex = 8;
			else nIndex = 12;
		}
		else nIndex = 3;
		break;
	case CT_SEARCH:
	case CT_LSEARCH:
		nIndex = 4;
		break;
	default:
	case CT_SUBSCRIBE:
		if(HasUnread)
		{
			if(IsSelected)
			{
				if(IsFocused) nIndex = 5;
				else nIndex = 9;
			}
			else nIndex = 0;
		}
		else
		{
			if(IsSelected)
			{
				if(IsFocused) nIndex = 6;
				else nIndex = 10;
			}
			else nIndex = 1;
		}
		break;
	}
	return nIndex;
}

/************************************************************************
customdraw_Icon 아이콘을 그리는 부분
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/14: 아이템의 드로윙 과정을 singleton 을 이용하도록 변경
                그룹 아이템의 경우 채널의 유무에 따라서 다른 아이콘을 그림
************************************************************************/
void CFishTreeCtrl::customdraw_Icon(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
    //////////////////////////////////////////////////////////////////////////
    // 아이템의 종류 판별을 위해서 추가된 부분
    CDlg_Subscribe* pParent    =   (CDlg_Subscribe*)GetParent();
	if(!pParent) return;
    auto_ptr<FISH_REPITEM> it  =    auto_ptr<FISH_REPITEM>(new FISH_CHANNEL(GetItemData(hITEM), _T("")) );
	// 그런 아이템은 없습니다.
    if(!pParent->FindItem(it->id, it)) return;
    
    //////////////////////////////////////////////////////////////////////////
    // BEGIN : DRAWING
    CImageList* imgList =   NULL;
    int nIndex = 0, nSelIndex = 0;

	BOOL IsFocused = FALSE;
	BOOL IsSelected = FALSE;
	BOOL HasUnread = FALSE;

    CRect itemRT;

    if (paramRT)    
        itemRT  =   *paramRT;
    else    
        GetItemRect(hITEM, itemRT, FALSE);

    if ( GetParentItem(hITEM) ) {
        imgList = m_pBMPManager->getChannelIcon();
		// 이미지를 발견하지 못했으니 아이콘을 그리지 않는다.
		if(!imgList) return;

		if ( GetFocus() == this) IsFocused = TRUE;
		if ( GetItemState(hITEM, TVIS_SELECTED) & TVIS_SELECTED ) IsSelected = TRUE;
		if ( it->unreadcnt != 0 ) HasUnread = TRUE;

		nIndex = GetHITEMImageID(((FISH_CHANNEL*)it.get())->type, IsFocused, IsSelected, HasUnread);

		m_pBMPManager->drawTransparent( pDC, imgList, nIndex
			, CPoint(itemRT.left + FISH_REPOSITORY_ITEM_CHANNEL_ICON_LEFT_MARGIN + FISH_CHANNEL_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT, itemRT.top + FISH_REPOSITORY_ITEM_CHANNEL_ICON_TOP_MARGIN));

    }   else  {
		BOOL bSelected = (GetItemState(hITEM, TVIS_SELECTED) & TVIS_SELECTED);
        imgList = m_pBMPManager->getGroupIcon();
        if ( GetChildItem(hITEM) ) 
		{
			if(bSelected)
				nIndex = 2;
			else
				nIndex = 0;
		}
        else
		{
			if(bSelected)
				nIndex = 3;
			else
				nIndex = 1;
		}

        m_pBMPManager->drawTransparent(pDC, imgList, nIndex, 
            CPoint(itemRT.left + FISH_REPOSITORY_ITEM_GROUP_ICON_LEFT_MARGIN + FISH_GROUP_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT, itemRT.top + FISH_REPOSITORY_ITEM_GROUP_ICON_TOP_MARGIN), 
            TRUE, 
            CPoint(0, 0),
            FISH_REPOSITORY_MASK_COLOR);
    }



    // 기존의 아이콘을 그대로 두고서 다시 그림을 그린다.
    if ( hITEM != HTREEITEM_NONE && m_mapAniData.find( GetItemData(hITEM) ) != m_mapAniData.end() )   {
        animate_DrawItem( &(m_mapAniData.find( GetItemData(hITEM) )->second) );
        return;
    }
}

/************************************************************************
customdraw_Icon 아이콘만 새로그려야 할 필요가 있을 때 이용한다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/20:CREATED
    2005/12/23:아이콘 배경 그리기를 삭제함  (filckering 해결필요함)
************************************************************************/
void CFishTreeCtrl::animate_DrawItem(CFishTreeCtrl::FISHCUSTOMICON* data)
{
    //////////////////////////////////////////////////////////////////////////
    // 아이템의 종류 판별을 위해서 추가된 부분
    CDlg_Subscribe* pParent    =   (CDlg_Subscribe*)GetParent();
	if(!pParent) return;
    auto_ptr<FISH_REPITEM> it  =    auto_ptr<FISH_REPITEM>(new FISH_CHANNEL(GetItemData(data->hITEM), _T("")) );
	// 그런 아이템은 없습니다.
    if(!pParent->FindItem(it->id, it)) return;

    CRect rect;
    GetItemRect(data->hITEM, &rect, FALSE);
    
    CClientDC dc(this);
    CMemDC pDC(&dc, &rect);

	BOOL IsFocused = FALSE;
	BOOL IsSelected = FALSE;
	BOOL HasUnread = FALSE;

    customdraw_RootBkgnd(data->hITEM, pDC);    //BEGIN: item background drawing
    customdraw_Selected(data->hITEM, pDC);     //BEGIN: 개별 아이템이 선택되었을 경우에 효과를 준다.
    customdraw_Text(data->hITEM, pDC);         //BEGIN: 개별 아이템의 텍스트를 그려준다.
//    customdraw_Icon(data->hITEM, pDC);         //BEGIN: 지정된 배경 색을 이용해서 한장의 이미지로 투명한 아이콘을 그린다.

    // 원래 아이콘 그리기
    {
        CImageList* imgList =   NULL;
        int nIndex = 0, nSelIndex = 0;
        
        CRect itemRT;
        GetItemRect(data->hITEM, itemRT, FALSE);
        imgList = m_pBMPManager->getChannelIcon();

		if ( GetFocus() == this) IsFocused = TRUE;
		if ( GetItemState(data->hITEM, TVIS_SELECTED) & TVIS_SELECTED ) IsSelected = TRUE;
		if ( it->unreadcnt != 0 ) HasUnread = TRUE;

		nIndex = GetHITEMImageID(((FISH_CHANNEL*)it.get())->type, IsFocused, IsSelected, HasUnread);

//        GetItemImage(data->hITEM, nIndex, nSelIndex);
            
        m_pBMPManager->drawTransparent(pDC, imgList, nIndex, 
            CPoint(itemRT.left + FISH_REPOSITORY_ITEM_CHANNEL_ICON_LEFT_MARGIN + FISH_CHANNEL_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT, itemRT.top + FISH_REPOSITORY_ITEM_CHANNEL_ICON_TOP_MARGIN), 
            FALSE, 
            CPoint(0, 0),
            FISH_REPOSITORY_MASK_COLOR);
    }

    {
        CImageList* imgList = NULL;
        if (!data->pIMGLIST)    imgList   =   m_pBMPManager->getAnimateIcons();
        else    imgList =   data->pIMGLIST;
        
        CRect itemRT;
        GetItemRect(data->hITEM, itemRT, FALSE);
        m_pBMPManager->drawTransparent(pDC, imgList, data->nINDEX, 
            CPoint(itemRT.left + FISH_REPOSITORY_ITEM_CHANNEL_ICON_LEFT_MARGIN + FISH_CHANNEL_DEPTH * FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT, itemRT.top + FISH_REPOSITORY_ITEM_CHANNEL_ICON_TOP_MARGIN), 
            TRUE, 
            CPoint(1, 1),
            FISH_REPOSITORY_ANIMATE_MASK_COLOR); 
    }

    customdraw_DropHightlight(data->hITEM, pDC);    //BEGIN: 아이템 이동시에 이동지역의 확실한 구분을 위해서
}

/************************************************************************
customdraw_DropHightlight	지금 그리는 아이템이 drag 의 drop 아이템인 경우 하단에
						삽입 포인트를 알리는 선을 그려준다.
@param  : 
@return : 
@remark : 
	이녀석의 문제는 일반적이지 않다는 문제 ㅡ.ㅡ; 어떻게 개선할 것인가?
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18  :   CREATED
    2005/12/22  :   하단부의 빈 사각형 그리기 삭제
************************************************************************/
void CFishTreeCtrl::customdraw_DropHightlight(HTREEITEM hITEM, CDC* pDC)
{
    CRect itemRT;
    GetItemRect(hITEM, &itemRT, FALSE);

	if (hITEM == GetDropHilightItem() && m_enumDragTYPE != NONDRAG)
	{
        pDC->FillRect( CRect(itemRT.left, itemRT.bottom-2, itemRT.right, itemRT.bottom), &CBrush(FISH_REPOSITORY_DROPHIGHLIGHT_COLOR) );
    } else {
    }
	return;
}

/************************************************************************
OnBegindrag 마우스의 드래그 이벤트 처리 파트
@param  : 
@return : 
@remark : mouse dragging은 OnBegindrag, OnMouseMove, OnLButtonUp 3개의 메소드에서 이벤트 처리함
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/15  :   CREATE
    2005/12/15  :   첫번째 그룹 아이템을 이동하지 못하도록 변경함
    2005/01/30  :   이미지를 이용한 채널의 이동 표현이 아닌 Transparent Dlg 를 이용한 표현으로 변경
************************************************************************/
void CFishTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

    // TODO: Add your control notification handler code here
    ASSERT(m_enumDragTYPE == NONDRAG);

    //////////////////////////////////////////////////////////////////////////
    // PREPROCESSING
    CPoint	ptAction;
	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);

    if ( HitTest(ptAction) )
        SelectItem( HitTest(ptAction) );
    else
        return;

    //////////////////////////////////////////////////////////////////////////
    // PROCESSING BEGINDRAG
    HTREEITEM hITEM =   GetSelectedItem();

    if ( hITEM  ==   GetRootItem() ) return;

    if (GetCapture() != this)
        SetCapture();

    //////////////////////////////////////////////////////////////////////////
	// preprocessing : tree repository item transfering
	m_vecDragITEM.clear();
    m_vecDragITEM.push_back( GetSelectedItem() );
    
	//////////////////////////////////////////////////////////////////////////
	// 아이템의 이동을 3가지로 구분해서 처리한다.
    {
        if ( isGroup( m_vecDragITEM[0] ) )
            m_enumDragTYPE  =   GROUPDRAG;
        else 
            m_enumDragTYPE  =   CHANNELDRAG;
    }

	//////////////////////////////////////////////////////////////////////////
	// drag image creation & attach
	{
        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
        if (m_ctrlDragDlg == NULL)  m_ctrlDragDlg   =   new CFishTransparentDragDlg(this);
        m_ctrlDragDlg->Create(this);
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

        SmartPtr<CBitmap> bmpDragImg  =   SmartPtr<CBitmap>(new CBitmap);
        CClientDC dc(this);

        m_pbmplistDragIMAGE =   CreateDragImageEx( m_vecDragITEM[0] );
        CFishBMPManager::GetBMPfromList(&dc, m_pbmplistDragIMAGE, 0, bmpDragImg.get());

        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
        m_ctrlDragDlg->SetBkgndImage(bmpDragImg);
        m_ctrlDragDlg->DragShowNolock(TRUE);
        m_ctrlDragDlg->ShowWindow(SW_SHOW);
        m_ctrlDragDlg->DragMove( CPoint(0, ptAction.y) );
        m_ctrlDragDlg->DragMove( CPoint(0, ptAction.y) );
        m_ctrlDragDlg->DragEnter( this, CPoint(0, ptAction.y) );
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

	}
}

/************************************************************************
OnMouseMove 마우스의 이동에 따른 행위 표현
@PARAM  : 
@RETURN : 
@REMARK : 
    마우스의 드래깅시 행해야할 행동을 보여준다.
    마우스의 포커스가 트리를 벗어날 경우를 표현하기 위해서 마우스 이벤트 트래커를
    활성화 시킨다.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    CRect clientRT;
    GetClientRect(&clientRT);

	SetOverHwnd(GetSafeHwnd());

    //////////////////////////////////////////////////////////////////////////
	// Redraw when cursor moves
    if ( m_enumDragTYPE != NONDRAG )
	{
        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_    // 2000 이하인 경우에는 창의 투명화가 불가하기 때문에 실행하지 않는다.
        m_ctrlDragDlg->DragLeave(this);
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

        //////////////////////////////////////////////////////////////////////////
        // 마지막 아이템의 높이 결정
        int nCount  =   0;
        HTREEITEM it    =   GetRootItem();
        do
        {
            nCount++;
            it  =   GetNextVisibleItem(it);
        }while (it);

        int nHeight =   0;
        if ( nCount >   GetVisibleCount() )
            nHeight =   GetVisibleCount() * GetItemHeight();
        else
            nHeight =   nCount * GetItemHeight();

        //////////////////////////////////////////////////////////////////////////
        // 결정된 높이를 기반으로 높이 조정
        CPoint sentinelPos (0, point.y);
        if ( nHeight - GetItemHeight() < point.y )    {
            sentinelPos.y   =   nHeight -   GetItemHeight();       //
        }   
        else if (clientRT.top    >   point.y) 
        {
            sentinelPos.y   =   0;
        }

        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
        m_ctrlDragDlg->DragMove( sentinelPos );
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
       
		if ( HitTest( sentinelPos ) != NULL )		{
			SelectDropTarget( HitTest( sentinelPos ) );						    //redraw target item image
		}

        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
        m_ctrlDragDlg->DragEnter(this, CPoint(0, point.y < sentinelPos.y ? point.y : sentinelPos.y ));
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

        //////////////////////////////////////////////////////////////////////////
        // 마우스 이동중 아이템의 선택
        if((point.y - clientRT.top) < 3)
        {
            SendMessage(WM_VSCROLL, SB_LINEUP);
        }
        if((clientRT.bottom - point.y) < 3)
        {
            SendMessage(WM_VSCROLL, SB_LINEDOWN);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // 마우스 이벤트 트래커
    TRACKMOUSEEVENT trackmouseevent;
    trackmouseevent.cbSize = sizeof(trackmouseevent);
    trackmouseevent.dwFlags = TME_LEAVE | TME_HOVER;
    trackmouseevent.hwndTrack = GetSafeHwnd();
    trackmouseevent.dwHoverTime = 0;
    ::_TrackMouseEvent(&trackmouseevent);

    CTreeCtrl::OnMouseMove(nFlags, point);
}

/************************************************************************
OnLButtonUp		마우스 업 이벤트 처리
@param  : 
@return : 
@remark : 
	채널, 그룹, 멀티 채널 이동에따라서 예외를 처리하는 부분
	절대로 리팩토링 필요함
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/18
************************************************************************/
void CFishTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    if ( GetCapture() == this ) ReleaseCapture();
    
    _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
    m_ctrlDragDlg->DragLeave(this);
    _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
    if (m_enumDragTYPE   !=  NONDRAG)
    {
        //////////////////////////////////////////////////////////////////////////
        // 드래그 완료후의 공통 처리부분
        if (m_pbmplistDragIMAGE)        {
            delete m_pbmplistDragIMAGE;
            m_pbmplistDragIMAGE = NULL;
        }

        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_
        if (m_ctrlDragDlg)        {
            m_ctrlDragDlg->ShowWindow(SW_HIDE);
            delete m_ctrlDragDlg;
            m_ctrlDragDlg   =   NULL;
        }
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

        //////////////////////////////////////////////////////////////////////////
        // 드래깅의 중간 처리 부분
        DWORD   dwData  =   GetItemData( m_vecDragITEM[0] );
        if (m_enumDragTYPE == GROUPDRAG)    {
            lbuttonup_GroupDrag();
        } 
        else if (m_enumDragTYPE == CHANNELDRAG || m_enumDragTYPE == MULTICHANNELDRAG) 
        {
            if (m_enumDragTYPE == CHANNELDRAG)  {
                lbuttonup_ChannelDrag();
            }
            EnsureVisible( GetDropHilightItem() );
        }

        //////////////////////////////////////////////////////////////////////////
        // 드래깅의 후처리 부분
        ReleaseCapture();

        HTREEITEM   hITEM   =   GetHandleFromData( dwData );
        ASSERT(hITEM);

        if ( GetParentItem(hITEM) != NULL )	{
            Expand( GetParentItem(hITEM), TVE_EXPAND);
        }
        Select(hITEM, TVGN_CARET);
        SelectItem( hITEM );

        SelectDropTarget( NULL );
        m_enumDragTYPE = NONDRAG;
        m_vecDragITEM.clear();

		Invalidate();
    }
    CTreeCtrl::OnLButtonUp(nFlags, point);
}

/************************************************************************
isChildOf 2개의 항목의 종속성(부모, 자식)관계를 판단한다.
@param  : 
    hitemChild 자식 핸들
    hitemSuspectedParent 의심되는 부모의 핸들
@return : 
    TRUE 자식 핸들이 맞다면
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/15
************************************************************************/
BOOL CFishTreeCtrl::isChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)	break;
	}
	while ((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

/************************************************************************
moveGroupPos    그룹 아이템의 현재 위치 변경
@param  : 
    dest 핸들의 뒤에 아이템을 이동
@return : 
    변경된 아이템의 위치
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/17
************************************************************************/
HTREEITEM CFishTreeCtrl::moveGroupPos(HTREEITEM srcITEM, HTREEITEM dstITEM)
{
    assert ( GetParentItem(srcITEM) == NULL && GetParentItem(dstITEM) == NULL );
    return moveBetweenSibling(srcITEM, dstITEM);
}

/************************************************************************
moveChannelPos  채널 아이템의 현재 위치 변경
@param  : 
@return : 
    새로 바뀐 위치의 핸들을 리턴
@remark : 
    moveBetweenSibling 으로 동시에 묵어야 하는데 현재 그냥 묵으면 버그가 발생하기 때문에
    이 상태로 진행하겠음 (05. 11. 17)
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/17
************************************************************************/
HTREEITEM CFishTreeCtrl::moveChannelPos(HTREEITEM srcITEM, HTREEITEM dstITEM)
{
    assert (GetParentItem(srcITEM) != NULL && GetParentItem(dstITEM) != NULL);
    return moveBetweenSibling(srcITEM, dstITEM);
}

/************************************************************************
updateItem  인자로 받은 값을 조합해서 트리 뷰에서 아이템의 정보를 갱신한다.
@param  : 
@return : 
@remark : 
    hItem 의 유효성을 assertion 한다.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/24
************************************************************************/
HTREEITEM CFishTreeCtrl::updateItem(HTREEITEM hItem, tstring text, unsigned int iImage, unsigned int iSelectedImage, DWORD data)
{
    assert ( hItem != NULL );

    TCHAR sztBuffer[FISH_REPOSITORY_LABELEDIT_MAX_LENGTH + 1];               // text buffer
    TVITEM tvstruct;
    tvstruct.hItem = hItem;
    tvstruct.cchTextMax = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
    tvstruct.pszText = sztBuffer;
    tvstruct.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
    GetItem(&tvstruct);

    _tcscpy( tvstruct.pszText, (TCHAR*) text.c_str() );

    tvstruct.iImage            =   iImage;
    tvstruct.iSelectedImage    =   iSelectedImage;

    SetItem( &tvstruct );
    SetItemData( tvstruct.hItem, data );

    return tvstruct.hItem;
}

/************************************************************************
moveBetweenSibling 동일한 레벨의 아이템의 이동관련 작업 메소드
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
HTREEITEM	CFishTreeCtrl::moveBetweenSibling(HTREEITEM srcITEM, HTREEITEM dstITEM)
{
    TV_INSERTSTRUCT tvstruct;
    TCHAR sztBuffer[FISH_REPOSITORY_LABELEDIT_MAX_LENGTH+1];

    {
        // avoid an infinite recursion situation
        tvstruct.item.hItem = srcITEM;
        tvstruct.item.cchTextMax = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
        tvstruct.item.pszText = sztBuffer;
        tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;
        GetItem(&tvstruct.item); // get information of the dragged element
    
        tvstruct.hParent    =   GetParentItem(dstITEM);
        tvstruct.hInsertAfter = dstITEM;
        tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;
        tvstruct.item.cChildren =   GetChildItem(srcITEM) ? 1 : 0;
    }

    HTREEITEM RET = InsertItem(&tvstruct);
    SetItemData( RET, (DWORD)GetItemData(srcITEM) );

    // BETWEEN CHANNEL ITEMS
    if (GetParentItem(srcITEM) != NULL && GetParentItem(dstITEM) != NULL)   
    {
        ((CDlg_Subscribe*)GetParent())->MoveChannel( (DWORD)GetItemData(srcITEM), (DWORD)GetItemData(GetParentItem(dstITEM)) );
        DeleteItem(srcITEM);
    } 
    else
    // BETWEEN GROUP ITEMS
    {
        //////////////////////////////////////////////////////////////////////////
        // 기존의 그룹 내에 잇던 채널을 이동시킨다.
	    vector<HTREEITEM> childList;
	    if ( GetChildItem( srcITEM ) )
	    {
    	    HTREEITEM item = GetChildItem( srcITEM );
		    do 
		    {
		        childList.push_back( item );
            } while ( item = GetNextSiblingItem(item) );
        }

        vector<HTREEITEM>::reverse_iterator it;
        for (it = childList.rbegin(); it != childList.rend(); it++)
        {
            moveChannelToGroup(*it, RET);			// item transfering
        }

        //////////////////////////////////////////////////////////////////////////
        // 그룹 이동후에 기존에 펼쳐진 상태라면 펼쳐진 상태로 만든다.
        if ( GetItemState(srcITEM, TVIS_EXPANDED) )
        {
            Expand(RET, TVE_EXPAND);
        }
    }
    DeleteItem( srcITEM );			//MSDN: If hITEM has the TVI_ROOT value, all items are deleted from the tree view control. 

    animate_UPDATE( GetItemData(RET) );
    return RET;
}


/************************************************************************
moveChannelToGroup 아이템 이동 메소드
@param  : 
    srcITEM 소스 아이템 핸들
    dstITEM 목적지 아이템 핸들
@return : 
    HTREEITEM	새롭게 삽입된 아이템의 핸들
@remark : 
	소스 아이템이 루트라면 이는 무시하고 NULL을 리턴한다.
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/15
************************************************************************/
HTREEITEM CFishTreeCtrl::moveChannelToGroup(HTREEITEM srcITEM, HTREEITEM dstITEM)
{
    assert ( GetParentItem(srcITEM) != NULL && GetParentItem(dstITEM) == NULL );
    //////////////////////////////////////////////////////////////////////////
    // SUBSCRIBE MANAGER INTEGRATION PART
    ((CDlg_Subscribe*)GetParent())->MoveChannel( GetItemData(srcITEM), GetItemData(dstITEM) );

    TV_INSERTSTRUCT tvstruct;
    TCHAR sztBuffer[FISH_REPOSITORY_LABELEDIT_MAX_LENGTH+1];
    HTREEITEM hPrevItem, RET;

    // avoid an infinite recursion situation
    {
        tvstruct.item.hItem = srcITEM;
        tvstruct.item.mask  = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE;
        tvstruct.item.cchTextMax= FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
        tvstruct.item.pszText   = sztBuffer;
        GetItem(&tvstruct.item); // get information of the dragged element
        hPrevItem           =   tvstruct.hParent;
        tvstruct.hParent    =   dstITEM;
        tvstruct.item.mask  =   TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_STATE | TVIF_SELECTEDIMAGE;
        tvstruct.hInsertAfter   = TVI_FIRST;
    }

    RET = InsertItem(&tvstruct);
    SetItemData(RET, GetItemData(srcITEM));

    DeleteItem(srcITEM);

    {// 채널이 속해 있던 기존의 그룹, 이동 후의 그룹에 대해서 후처리를 한다. (자식의 개수변경의 문제로 Expand가 동작하지 않는 것을 방지 하기 위한것)
        TVITEM structItem;
        structItem.cchTextMax    = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
        structItem.pszText       = sztBuffer;
        structItem.mask          = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;

        /// 이전 그룹
//        structItem.hItem         = hPrevItem;
//        GetItem(&structItem); // get information of the dragged element
//        structItem.cChildren     = GetChildItem(hPrevItem) ? 1 : 0;
//        if (!structItem.cChildren)   
//            structItem.state    ^=  TVIS_EXPANDED;
//         SetItem(&structItem);

        /// 이동 후 그룹
        structItem.hItem         = dstITEM;
        GetItem(&structItem); // get information of the dragged element
        structItem.cChildren     = GetChildItem(dstITEM) ? 1 : 0;
        SetItem(&structItem);
    }
    animate_UPDATE( GetItemData(RET) );

    return RET;
}

/************************************************************************
InvalidateitemRect    아이템의 핸들을 얻어와서 그 아이템의 영역을 다시 그린다.
@param  :
    hITEM   다시 그려야할 아이템
@return : 
@remark : 
    2005 11 17 proposed by moonknit
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/17   by youngchang
    2005/01/19  :   MODIFIED    미동작 하던 문제 해결. OnCustomDraw 로 해결했기 때문에 OnCustomDraw에 영향을 받음
************************************************************************/
void CFishTreeCtrl::InvalidateItem(HTREEITEM hITEM)
{
    CClientDC dc(this);
    LRESULT   garbage;
    NMTVCUSTOMDRAW LVCD;
    LVCD.nmcd.dwItemSpec    =   (DWORD)hITEM;
    LVCD.nmcd.hdc           =   dc.m_hDC;
    LVCD.nmcd.dwDrawStage   =   CDDS_ITEMPREPAINT;

    OnNMCustomDraw( reinterpret_cast<NMHDR*>(&LVCD), &garbage );
}

/************************************************************************
InvalidateAll   모든 아이템을 새로 그린다. (explicitly)
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/20:CREATED
************************************************************************/
void CFishTreeCtrl::InvalidateAll()
{
    HTREEITEM it    =   GetRootItem();

    // GRP REDRAW
    for (; it!= NULL; it=GetNextSiblingItem(it) )
    {
        InvalidateItem(it);

        // CHANNEL REDRAW
        if (GetChildItem(it) != NULL)
        {
            HTREEITEM it2   =   GetChildItem(it);
            for(; it2!=NULL; it2=GetNextSiblingItem(it2))
            {
                InvalidateItem(it2);
            }
        }
    }

    return;
}

/************************************************************************
lbuttonup_ChannelDrag 채널의 드래깅이 끝난후의 처리
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishTreeCtrl::lbuttonup_ChannelDrag()
{
    if (!GetDropHilightItem())  return;
    
    if ( GetDropHilightItem() != m_vecDragITEM[0] && GetParentItem(GetDropHilightItem()) != NULL )  
    {
        moveChannelPos(m_vecDragITEM[0], GetDropHilightItem());
    }
    else if ( GetDropHilightItem() != m_vecDragITEM[0] && GetParentItem(GetDropHilightItem()) == NULL )
    {
        moveChannelToGroup(m_vecDragITEM[0], GetDropHilightItem());
    }
}

/************************************************************************
lbuttonup_GroupDrag 그룹의 드래깅이 끝난후의 처리
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishTreeCtrl::lbuttonup_GroupDrag()
{
    //////////////////////////////////////////////////////////////////////////
	// 그룹 아이템의 이동 후 처리
	// 그룹 아이템이 채널 아이템을 가지고 있는 경우 처리
	if ( GetParentItem(GetDropHilightItem()) != NULL )  
        SelectDropTarget( GetParentItem(GetDropHilightItem()) );

    // switch position
    SelectDropTarget( moveGroupPos(m_vecDragITEM[0], GetDropHilightItem()) );
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * 현재 클릭한 위치의 아이템의 데이터를 반환한다.
 *
 * @Parameters
 * (in CPoint) pt - 검색할 위치
 * (out BOOL&) bfind - 발견여부
 *
 * @Return
 * (DWORD) 발견된 아이템의 데이터
 **************************************************************************/
DWORD CFishTreeCtrl::GetItemData2(CPoint pt, BOOL& bfind)
{
	CRect rect;
	ScreenToClient(&pt);
	GetClientRect(&rect);
	bfind = FALSE;
	HTREEITEM hITEM = NULL, htmpitem;
	if(!rect.PtInRect(pt)) return (-1);

	hITEM = GetRootItem();
	while(hITEM)
	{
		if(GetItemRect(hITEM, &rect, FALSE) && rect.PtInRect(pt))
		{
			bfind = TRUE;
			return GetItemData(hITEM);
		}

		// children
		htmpitem = GetNextItem(hITEM, TVGN_CHILD);

		// siblings
		if(htmpitem == NULL)
			htmpitem = GetNextItem(hITEM, TVGN_NEXT);

		// parent siblings
		if(htmpitem == NULL)
		{
			htmpitem = GetNextItem(hITEM, TVGN_PARENT);
			if(htmpitem)
				htmpitem = GetNextItem(htmpitem, TVGN_NEXT);
		}

		hITEM = htmpitem;
	}

	return (-1);
}

/************************************************************************
animate_ADD 인자로 얻은 ID를 갖는 아이템을 애니메이션 리스트에 추가한다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/23:ID로 추가할 수 있도록 변경함
************************************************************************/
void CFishTreeCtrl::animate_ADD(int ITEMID)
{
	HTREEITEM hITEM = GetHandleFromData(ITEMID);
	if(HTREEITEM_NONE == hITEM) return;

    m_mapAniData[ITEMID] = FISHCUSTOMICON();
    m_mapAniData[ITEMID].hITEM   =   hITEM;
    m_mapAniData[ITEMID].nINDEX  =   0;
    m_mapAniData[ITEMID].pIMGLIST    =   m_pBMPManager->getAnimateIcons();
    GetItemRect( m_mapAniData[ITEMID].hITEM, &(m_mapAniData[ITEMID].rtOUTLINE), FALSE );

    if ( GetParentItem(m_mapAniData[ITEMID].hITEM) ) 
	{
		m_mapAniData[ITEMID].leftPADDING =   FISH_REPOSITORY_ITEM_CHANNEL_ICON_LEFT_MARGIN;
	}
}

/************************************************************************
animate_REMOVE
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:지연 시간 루틴 삭제. SmartPtr 관련 버그 수정. 지우기 끝난후 잘못그리는 버그 수정
    2005/12/23:ID로 추가할 수 있도록 변경함
	2006/03/02:삭제할 대상의 Validation Check by moonknit
************************************************************************/
void CFishTreeCtrl::animate_REMOVE(int ITEMID)
{
	HTREEITEM hITEM = GetHandleFromData(ITEMID);
	if(HTREEITEM_NONE == hITEM) return;

	map<int, FISHCUSTOMICON>::iterator it = m_mapAniData.find( ITEMID );
	if(it == m_mapAniData.end()) return;

    InvalidateItem( hITEM );
    m_mapAniData.erase( it );
}

/************************************************************************
animate_UPDATE  단일 아이템의 애니메이션 정보를 업데이트 한다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/23:CREATED
    2005/12/23:ID로 추가할 수 있도록 변경함
************************************************************************/
void CFishTreeCtrl::animate_UPDATE(DWORD ITEMID)
{
	HTREEITEM hITEM = GetHandleFromData(ITEMID);
	if(HTREEITEM_NONE == hITEM) return;

	map<int, FISHCUSTOMICON>::iterator it = m_mapAniData.find( ITEMID );
	if(it == m_mapAniData.end()) return;

    (*it).second.hITEM  =   hITEM;
    GetItemRect( hITEM, &(*it).second.rtOUTLINE, FALSE);
}

/************************************************************************
animate_ALLUPDATE   전체 아이템의 애니메이션 정보를 업데이트 한다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/23:CREATED
************************************************************************/
void CFishTreeCtrl::animate_ALLUPDATE()
{
    map<int, FISHCUSTOMICON>::iterator it = m_mapAniData.begin();
    for (; it != m_mapAniData.end(); it++)
    {
        animate_UPDATE(it->first);
    }
}

/************************************************************************
GetHandleFromData   인자로 받은 데이터를 이용해서 아이템을 찾는다.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:보이는 아이템만 검색하던 버그 수정
    2005/12/22:1BASE Counting 추가함.
    2005/12/22:핸들을 찾으면서 동시에 외곽사각형을 얻는다.
************************************************************************/
HTREEITEM   CFishTreeCtrl::GetHandleFromData(WORD data)
{
//    BOOL bRetCount  =   TRUE;
//    CRect rootRT;
//    GetItemRect(GetFirstVisibleItem(), &rootRT, FALSE);

    for(HTREEITEM hGROUPITEM    =   GetRootItem(); hGROUPITEM; hGROUPITEM = GetNextItem(hGROUPITEM, TVGN_NEXT))
    {
		if(GetItemData(hGROUPITEM) == data) return hGROUPITEM;

        for (HTREEITEM hCHANNELITEM = GetChildItem(hGROUPITEM); hCHANNELITEM; hCHANNELITEM = GetNextItem(hCHANNELITEM, TVGN_NEXT))
        {
            if ( GetItemData(hCHANNELITEM) == data )    {
                return hCHANNELITEM;
            }
        }
    }
    return HTREEITEM_NONE;
}

/************************************************************************
OnTimer
@param  : 
@return : 
@remark : 
    라벨 에디트 시의 시간 초과 처리
    아이콘 에니메이션 타이머
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/20:CREATED
    2005/12/21:고속 컴퓨터에서는 피드가 너무 빠른 관계로 애니메이션을 타이머에서 처리함. (deleted)
    2005/12/22:아이템의 애니메이션이 끝나고 다시 그려주기 추가함.
************************************************************************/
void CFishTreeCtrl::OnTimer(UINT nIDEvent) 
{
    // TODO: Add your message handler code here and/or call defaultz
    CPoint ptCursorPos;
    GetCursorPos(&ptCursorPos);
    ScreenToClient(&ptCursorPos);
    
    switch (nIDEvent)
    {
    case TID_REPOSITORY_ANIMATION:  // ANIMATE
        {
            if ( !m_mapAniData.size() ) return;
            
            UINT    nCount  = m_pBMPManager->getAnimateIcons()->GetImageCount();
            map<int, FISHCUSTOMICON>::iterator   iter =   m_mapAniData.begin();
            for (; iter != m_mapAniData.end(); iter++)
            {
                if (GetParentItem(iter->second.hITEM))
                {
                    if ( GetItemState(GetParentItem(iter->second.hITEM), TVIS_EXPANDED) & TVIS_EXPANDED )  animate_DrawItem( &(iter->second) );
                }
                iter->second.nINDEX =   (iter->second.nINDEX+1) % nCount;
            }
        }
        break;

    case TID_REPOSITORY_LABELEDIT:
        {
            CPoint pt;
            GetCursorPos(&pt);
            HTREEITEM hCurItem  =   HitTest(pt);
            HTREEITEM hPrevItem =   HitTest(m_ptLabelEdit);
            
            if (hPrevItem   ==  hCurItem)
            {
                TRACE(_T("(REPOS TREE) BEGIN LABEL EDIT\n"));
                KillTimer(TID_REPOSITORY_LABELEDIT);
                EditLabel( HitTest(pt) );
                m_ptLabelEdit   =   CPoint(0, 0);
            }   else    {
                KillTimer(TID_REPOSITORY_LABELEDIT);
                TRACE(_T("(REPOS TREE) KILL LABEL EDIT TIMER\n"));
                m_ptLabelEdit   =   CPoint(0, 0);
            }
        }
        break;

    default:
        break;
    }
    
    CTreeCtrl::OnTimer(nIDEvent);
}


/************************************************************************
EditLabel   아이템 핸들을 인자로 얻어서 라벨 에디트 프로시져를 실행
@param  : 
@return : 
@remark : 
    메시지 기반으로 작성하지 않았기 때문에 차후에 변경이 필요할 듯. (구현 시간 부족함 ㅡㅡ;;)
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:CREATED
    2005/12/23:프로시져 동작 완성함. (메시지 기반으로 변경할 필요 있음)
************************************************************************/
void CFishTreeCtrl::EditLabel(HTREEITEM hITEM, BOOL fMouseSel)
{
    CRect   outlineRT;
    GetItemRect(hITEM, &outlineRT, FALSE);
    HTREEITEM   m_hLabelEditItem    =   hITEM;

    {   // BACKGROUND & ICON REDRAW
//        UINT    nIndex  =   0;
        FISHCUSTOMICON tmpSTRUCT;
        GetItemImage(hITEM, tmpSTRUCT.nINDEX, tmpSTRUCT.nINDEX);
        tmpSTRUCT.rtOUTLINE     =   outlineRT;
        tmpSTRUCT.hITEM         =   hITEM;
        tmpSTRUCT.pIMGLIST      =   GetParentItem(hITEM) ? m_pBMPManager->getChannelIcon() : m_pBMPManager->getGroupIcon();

        if ( GetParentItem(hITEM) ) 
		{
            CClientDC dc(this);
            dc.FillRect( &tmpSTRUCT.rtOUTLINE, &CBrush(FISH_REPOSITORY_BG_COLOR_S) );
            tmpSTRUCT.leftPADDING   =   FISH_CHANNEL_DEPTH * GetIndent();
        }
        InvalidateItem( hITEM );
    }

    {   // EDIT BOX MESSAGE PROC
        
        try
        {
            CDlg_Subscribe* pParentWnd =   (CDlg_Subscribe*)GetParent();
            CRect editLabelRT;
            auto_ptr<FISH_REPITEM> tmpREPITEM;
            pParentWnd->FindItem( GetItemData(hITEM), tmpREPITEM);

            // GET EDIT CTRL HEIGHT
            CDC* pDC = GetDC();
			if(!pDC) return;
            CFont* pOldFont = pDC->SelectObject( &m_ftGroupDefault );
            int h = pDC->DrawText( _T("TEST"), editLabelRT, DT_CALCRECT );
            pDC->SelectObject(pOldFont);
            ReleaseDC(pDC);

            // GET ITEM INFO
            TV_INSERTSTRUCT tvstruct;
            TCHAR sztBuffer[FISH_REPOSITORY_LABELEDIT_MAX_LENGTH+1];
            tvstruct.item.hItem = hITEM;
            tvstruct.item.cchTextMax = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
            tvstruct.item.pszText = sztBuffer;
            tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;
            GetItem(&tvstruct.item); // get information of the dragged element

            // SET EDIT CTRL SIZE
            GetItemRect(hITEM, &editLabelRT, TRUE);
            editLabelRT.right   =   outlineRT.right;

            if( editLabelRT.Height() > h )  {
                editLabelRT.top += (editLabelRT.Height()-h)/2;
                editLabelRT.bottom -= (editLabelRT.Height()-h)/2;
            }
            
            editLabelRT.left  =   FISH_REPOSITORY_ITEM_LABELEDIT_LEFT_MARGIN;
            editLabelRT.right -=  FISH_REPOSITORY_ITEM_LABELEDIT_RIGHT_MARGIN;
            
            // CREATE & PROC EDIT CTRL
            if(m_ctrlEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,editLabelRT,this,FISH_REPOSITORY_LABELEDIT_UID))
            {
                m_ctrlEdit.SetFont(&m_ftGroupDefault, FALSE);
                CString sOld = tvstruct.item.pszText;
                m_ctrlEdit.SetLimitText(FISH_REPOSITORY_LABELEDIT_MAX_LENGTH);
                m_ctrlEdit.SetWindowText(tvstruct.item.pszText);
                m_ctrlEdit.SetFocus();
                m_ctrlEdit.SetSel(0,-1);
                if(fMouseSel)
                    ReleaseCapture();
                for (;;) 
                {
                    MSG msg;
                    ::GetMessage(&msg, NULL, 0, 0);
                    
                    switch (msg.message) 
                    {
				case WM_KEYDOWN:
					{
						if (msg.wParam == VK_ESCAPE)    {       // UPDATE CANCEL
                            _tcscpy(tvstruct.item.pszText, tmpREPITEM->title);
                            updateItem(hITEM, tvstruct.item.pszText, tvstruct.item.iImage, tvstruct.item.iSelectedImage, GetItemData(hITEM) );

                            m_ctrlEdit.DestroyWindow();
                            m_hLabelEditItem  =   HTREEITEM_NONE;
							Invalidate(FALSE);
                            return;
						}
						if(msg.wParam == VK_RETURN)     {       // UPDATE TITLE
                            m_ctrlEdit.GetWindowText(tmpREPITEM->title);

                            switch(tmpREPITEM->rep_type) {
                            case REP_CHANNEL:
                                pParentWnd->ModifyChannel(tmpREPITEM->id, *(FISH_CHANNEL*)tmpREPITEM.get());
                            	break;
                            case REP_GROUP:
                                pParentWnd->ModifyGroup(tmpREPITEM->id, *(FISH_GROUP*)tmpREPITEM.get());
                                break;
                            default:
                                break;
                            }
                            
							m_ctrlEdit.DestroyWindow();
                            m_hLabelEditItem  =   HTREEITEM_NONE;
							Invalidate(FALSE);
                            return;
						}
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					break;
				case WM_LBUTTONDOWN:
					{
						if(msg.hwnd==m_hWnd)
						{
							POINTS pt = MAKEPOINTS(msg.lParam);
                            if( HitTest(CPoint(pt.x,pt.y)) != m_hLabelEditItem )
							{   // UPDATE CANCEL
                                _tcscpy(tvstruct.item.pszText, tmpREPITEM->title);
                                updateItem(hITEM, tvstruct.item.pszText, tvstruct.item.iImage, tvstruct.item.iSelectedImage, GetItemData(hITEM) );

    							m_ctrlEdit.DestroyWindow();
								TranslateMessage(&msg);
								DispatchMessage(&msg);
                                m_hLabelEditItem  =   HTREEITEM_NONE;
                                return;
							}
						}
						else if(msg.hwnd == m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						else
						{   // UPDATE CANCEL
                            _tcscpy(tvstruct.item.pszText, tmpREPITEM->title);
                            updateItem(hITEM, tvstruct.item.pszText, tvstruct.item.iImage, tvstruct.item.iSelectedImage, GetItemData(hITEM) );

                            m_ctrlEdit.DestroyWindow();
                            m_hLabelEditItem  =   HTREEITEM_NONE;
                            return;
						}
					}
					break;
				case WM_LBUTTONUP:
					{
						if(msg.hwnd==m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
					break;
				case WM_NCLBUTTONDOWN:
					{   // UPDATE CANCEL
                        _tcscpy(tvstruct.item.pszText, tmpREPITEM->title);
                        updateItem(hITEM, tvstruct.item.pszText, tvstruct.item.iImage, tvstruct.item.iSelectedImage, GetItemData(hITEM) );
                        
                        m_ctrlEdit.DestroyWindow();
						TranslateMessage(&msg);
						DispatchMessage(&msg);
                        m_hLabelEditItem  =   HTREEITEM_NONE;
                        return;
					}
					break;
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					break;
				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
                    }
                }
            }
        }
        catch(CMemoryException* e)
        {
            e->Delete();
            if(fMouseSel)   m_hLabelEditItem  =   HTREEITEM_NONE;
            return;
        }
    }

    m_hLabelEditItem  =   HTREEITEM_NONE;
    return;
}

void CFishTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    CPoint ptCursorPos(0, 0);
    GetCursorPos( &ptCursorPos );
    ScreenToClient( &ptCursorPos );
    
    HTREEITEM hItem =   HitTest(ptCursorPos);
    if (!hItem) return;

    Expand(hItem, TVE_TOGGLE);
    SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
    SelectItem(hItem);
    animate_ALLUPDATE();    // added by eternalbleu 2005/12/22
    return;        
}

BOOL CFishTreeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return false;
	// TODO: Add your message handler code here and/or call default
#ifdef USE_CUSTOMDRAW_SCROLLBAR
	m_ctrlVerticalScrollBar->UpdateThumbPosition();
#endif

	return CTreeCtrl::OnEraseBkgnd(pDC);
}

void CFishTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_CUSTOMDRAW_SCROLLBAR
	m_ctrlVerticalScrollBar->UpdateThumbPosition();
#endif
	
	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CFishTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_CUSTOMDRAW_SCROLLBAR
    m_ctrlVerticalScrollBar->UpdateThumbPosition();
#endif

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CFishTreeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    // TODO: Add your message handler code here and/or call defaultthis]
    ShowScrollBar(SB_HORZ, TRUE);
#ifdef USE_CUSTOMDRAW_SCROLLBAR
	m_ctrlVerticalScrollBar->UpdateThumbPosition();
#endif

    if (GetCount() < GetVisibleCount())
        return TRUE;
    else
	{
		if(GetOverHwnd() != GetSafeHwnd()) return TRUE;
    	return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
	}
}

void CFishTreeCtrl::OnPaint() 
{
    // 스크롤바 감추기
//    ShowScrollBar(SB_HORZ, TRUE);

    // 스크롤바 스키닝
    CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);

    //////////////////////////////////////////////////////////////////////////
    // 아무런 아이템의 없는 경우에 배경이 표시되어야함.
    if ( !GetCount() )
    {
        CRect clientRT;
        GetClientRect(&clientRT);
        memDC.FillSolidRect(clientRT, FISH_REPOSITORY_BG_COLOR);

        CBitmap* pBMP   =   m_pBMPManager->getSubscribeBkgnd(3);
        m_pBMPManager->drawTransparent(&memDC, pBMP, CPoint(0, 0), FALSE);
        return;
    }

#ifdef USE_CUSTOMDRAW_SCROLLBAR
	m_ctrlVerticalScrollBar->UpdateThumbPosition();
#endif

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
    //Default();
}

/************************************************************************
CreateDragImageEx
@PARAM  : 
@RETURN : 
@REMARK : 
    http://www.devpia.com/Forum/BoardView.aspx?no=324885&ref=324885&page=2&forumname=VC_QA&stype=&KeyW=%b5%e5%b7%a1%b1%d7+%c0%cc%b9%cc%c1%f6&KeyR=title
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/26:CREATED
************************************************************************/
CImageList* CFishTreeCtrl::CreateDragImageEx(HTREEITEM hitem)
{
    CDC *pDC = GetDC();

    if( pDC )
    {
        CSize size;
        CRect rect;
        GetItemRect(hitem, rect, FALSE);

        CDC memDC;
        memDC.CreateCompatibleDC(pDC);
        CBitmap memBmp;
        memBmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height() );
        CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

        //draw text
        memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect.left, rect.top, SRCCOPY);
        memDC.FillSolidRect( rect, RGB(0xFF, 0xFF, 0xFF) );

        CRect tmpRT =   rect;
        customdraw_RootBkgnd( hitem, &memDC, CRect(0, 0, tmpRT.Width(), tmpRT.Height()) );
        customdraw_Icon( hitem, &memDC, CRect(0, 0, tmpRT.Width(), tmpRT.Height()) );
        customdraw_Text( hitem, &memDC, CRect(0, 0, tmpRT.Width(), tmpRT.Height()) );

        memDC.SelectObject( pOldBmp );

        CImageList* pImageList = new CImageList();
        pImageList->Create( rect.Width(), rect.Height(), ILC_COLOR32, 1, 1 );
        pImageList->Add( &memBmp, RGB(0xFF, 0x00, 0xFF));

		ReleaseDC(pDC);
        return pImageList;
    }
    else
    {
        return NULL;
    }
}

HTREEITEM CFishTreeCtrl::HitTest(CPoint pt, UINT* pFlags /*= NULL*/)
{
    HTREEITEM ret   =   NULL;
    CRect rt;
    HTREEITEM it    =   GetFirstVisibleItem();
    while (it)
    {
        GetItemRect(it, rt, FALSE);
        if (rt.PtInRect(pt))
        {
            ret =   it;
            break;
        }
        it  =   GetNextVisibleItem(it);
    }

    return ret;
}

void CFishTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    SetFocus();
    
    HTREEITEM hITEM =   HitTest(point);
    if (hITEM)
    {
        SetItemState(hITEM, TVIS_SELECTED, TVIS_SELECTED);
        SelectItem( hITEM );
    }


    CTreeCtrl::OnLButtonDown(nFlags, point);
}

afx_msg LONG CFishTreeCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    CRect clientRT;
    GetClientRect( &clientRT );

	SetOverHwnd(NULL);

    CPoint point;
    GetCursorPos( &point );
    ScreenToClient( &point );

    if ( m_enumDragTYPE   !=  NONDRAG )
    {
        if ( clientRT.bottom < point.y)
        {
            CRect   fVisibleItemRT, eVisibleItemRT;
            int     nCount  =   GetVisibleCount();

            HTREEITEM hITEM =   GetFirstVisibleItem();
            for (int it =   0; it < nCount; hITEM   =   GetNextVisibleItem(hITEM), it++);

            if ( !SelectDropTarget(hITEM) ) FishMessageBox( _T("잘못된 대상이 선택되었습니다.") );
        }
    }
 	return 0;
}

/************************************************************************
OnMouseHover    마우스가 영역에서 움직이다 멈추는 순간에 호출된다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/31:CREATED
************************************************************************/
afx_msg LONG CFishTreeCtrl::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
//    MessageBox(_T("HOVER TEST"));
 	return 0;
}

void CFishTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    SetFocus();

    CPoint ptCursorPos(0, 0);
    GetCursorPos( &ptCursorPos );
    ScreenToClient( &ptCursorPos );
	
    HTREEITEM hItem =   HitTest(ptCursorPos);
    if (!hItem) return;

    SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
    SelectItem(hItem);
    animate_ALLUPDATE();    // added by eternalbleu 2005/12/22
    CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CFishTreeCtrl::OnNMDBClick(NMHDR *pNMHDR, LRESULT *pResult)
{
}

void CFishTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMTVCUSTOMDRAW* pLVCD = reinterpret_cast<NMTVCUSTOMDRAW*>( pNMHDR ); 

    {
        //////////////////////////////////////////////////////////////////////////
        // Locking the Control Window
        CImageList::DragLeave(this);

        BOOL bFirstSelect   = TRUE;
		BOOL bExpandAction  = FALSE;
        UINT nFlag;
        CPoint ptCursorPos(0, 0);
        GetCursorPos( &ptCursorPos );
        ScreenToClient( &ptCursorPos );
        
        HTREEITEM hItem = HitTest(ptCursorPos, &nFlag);

        {
            TV_INSERTSTRUCT tvstruct;
            TCHAR sztBuffer[FISH_REPOSITORY_LABELEDIT_MAX_LENGTH+1];

            // avoid an infinite recursion situation
            tvstruct.item.hItem = hItem;
            tvstruct.item.cchTextMax = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
            tvstruct.item.pszText = sztBuffer;
            tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;
            GetItem(&tvstruct.item); // get information of the dragged element
        }

        if (hItem != NULL)
        {
            if(m_hPrevSelectedItem !=  hItem)  {
                m_hPrevSelectedItem =   hItem;
            }   else    {
                bFirstSelect    =   FALSE;
            }

            if( !GetParentItem(hItem) )
            {
				CRect rc, expandRT;
				GetItemRect(hItem, rc, FALSE);
                expandRT    =   rc;
                expandRT.left   =   expandRT.right - GRP_EXPANDED_LEFT_PADDING;
                expandRT.right  =   expandRT.left + GRP_EXPANDED_IMPACT_BMP_WIDTH;


				if(expandRT.PtInRect(ptCursorPos))
				{
					Expand(hItem, TVE_TOGGLE);
					bExpandAction = TRUE;
					animate_ALLUPDATE();    // added by eternalbleu 2005/12/22
//					SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
//					SelectItem(hItem);
				}
            }

            
			if ( !bExpandAction )
            {
                if ( bFirstSelect )
                {
                    SelectItem(hItem);
                    //////////////////////////////////////////////////////////////////////////
                    // REQUEST POST FROM CHANNEL
                    if (GetSelectedItem())
                    {
                        ((CDlg_Subscribe*)GetParent())->RequestPost();
                        SetFocus();
                    }
                    ::SendMessage( ((CMainFrame*)(::AfxGetApp()->m_pMainWnd))->GetFirstView()->GetSafeHwnd(), WM_REQUEST_POST_DBLCLK, NULL, NULL );
                }   
                else    
                {
                    ::SendMessage( ((CMainFrame*)(::AfxGetApp()->m_pMainWnd))->GetFirstView()->GetSafeHwnd(), WM_REQUEST_POST_DBLCLK, NULL, NULL );
                }
            }

/*
            if ( GetParentItem(hItem) )
            {
                if ( bFirstSelect )
                {
                    SelectItem(hItem);
                    //////////////////////////////////////////////////////////////////////////
                    // REQUEST POST FROM CHANNEL
                    if (GetSelectedItem())
                    {
                        ((CDlg_Subscribe*)GetParent())->RequestPost();
                        SetFocus();
                    }
                    ::SendMessage( ((CMainFrame*)(::AfxGetApp()->m_pMainWnd))->GetFirstView()->GetSafeHwnd(), WM_REQUEST_POST_DBLCLK, NULL, NULL );
                }   
                else    
                {
                    ::SendMessage( ((CMainFrame*)(::AfxGetApp()->m_pMainWnd))->GetFirstView()->GetSafeHwnd(), WM_REQUEST_POST_DBLCLK, NULL, NULL );
                }
            }

            else 
            {
                if (bFirstSelect)
                {
                    Expand(hItem, TVE_TOGGLE);
                    SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
                    SelectItem(hItem);
                    animate_ALLUPDATE();    // added by eternalbleu 2005/12/22
                }   
                else
                {
                    Expand(hItem, TVE_TOGGLE);
                    SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
                    SelectItem(hItem);
                    animate_ALLUPDATE();    // added by eternalbleu 2005/12/22
                }
            }
*/
        }

        //////////////////////////////////////////////////////////////////////////
        // Unlocking the Control Window
        CImageList::DragEnter(this, CPoint(0, 0));
        ShowScrollBar(SB_HORZ, TRUE);
        InvalidateAll();
    }
}

void CFishTreeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CTreeCtrl::OnSize(nType, cx, cy);

	ShowScrollBar(SB_HORZ, FALSE);

	Invalidate();

//	HideScrollbar();
}

void CFishTreeCtrl::drawExpandedMark(CDC *pDC, CRect rt, int type /*=0*/, BOOL expand /*= TRUE*/)
{
    enum { 
        NORMAL  =   0,
        OVER    =   1,
        PRESS   =   2,
    };
    
    CPoint ptOrigin(rt.left, rt.CenterPoint().y - GRP_EXPANDED_IMPACT_BMP_HEIGHT / 2);
    
    m_pBMPManager->drawTransparent(pDC, m_pBMPManager->GetTreeGrpExpand(type, expand), ptOrigin, TRUE, CPoint(0, 2));           // normal 0, over 1, press 2
    return;
}

/************************************************************************
HideScrollBars
@PARAM  : 
@RETURN : 
@REMARK : 
    http://www.devpia.com/Forum/BoardView.aspx?no=453210&ref=453202&forumname=VC_QA&stype=VCF&KeyW=%c6%ae%b8%ae+%bd%ba%c5%a9%b7%d1%b9%d9&KeyR=title
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/13:CREATED
************************************************************************/
void CFishTreeCtrl::HideScrollbar()
{
    CRect ierect(0,0,0,0);
    int cxvs    =   0;
    int cyvs    =   GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ

    GetClientRect(&ierect);     // client 로 영역을 구할경우 스크롤바가 생성되었을 경우 그 크기가 변경된다.

    int tmp     =   GetScrollLimit(SB_VERT);
    
    if ( GetScrollLimit(SB_VERT) != 0 && GetScrollLimit(SB_VERT) != 1 && GetCount() >= GetVisibleCount() )
    {
        cxvs = GetSystemMetrics (SM_CYHSCROLL); //Get the system metrics - HORZ
    }

    ierect.right  +=  cxvs;

    //Here we set the position of the window to the clientrect + the size of the scrollbars
    SetWindowPos(NULL, ierect.left, ierect.top, ierect.right, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);
    
    //Just to be safe that the left/top corner is 0...
    CRect clientRT(ierect);
    ierect.top      =   0;
    ierect.left     =   0;
    ierect.right    =   clientRT.Width();
    ierect.bottom   =   clientRT.Height();
    
    HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!
    
    //The -2 is probably a border of some kind that we also need to remove. I could not find any good
    //metrics that gave me an 2 as an answer. So insted we makes it static with -2.
    iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom);
    
    //After the range has been made we add it...
    SetWindowRgn(iehrgn, TRUE);
}

void CFishTreeCtrl::ShowScrollbar()
{
    RECT ierect;
    GetClientRect(&ierect); 

    SetWindowPos(NULL, ierect.left, ierect.top, ierect.right, ierect.bottom, SWP_NOMOVE | SWP_NOZORDER);
    
    HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!
    
    //The -2 is probably a border of some kind that we also need to remove. I could not find any good
    //metrics that gave me an 2 as an answer. So insted we makes it static with -2.
    iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom);
    
    //After the range has been made we add it...
    SetWindowRgn(iehrgn, TRUE);
}

void CFishTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	ModifyStyle(WS_HSCROLL ,0,0);

	ShowScrollBar(SB_HORZ, FALSE);

	Invalidate();

	*pResult = 0;
}

void CFishTreeCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	ModifyStyle(WS_HSCROLL ,0,0);

	ShowScrollBar(SB_HORZ, FALSE);

	CTreeCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

int CFishTreeCtrl::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
    RECT rect;

    UINT nFlags;

    HTREEITEM hitem = CTreeCtrl::HitTest( point, &nFlags );
    if(nFlags & TVHT_ONITEM )
    {
        GetItemRect( hitem, &rect, TRUE );

        pTI->hwnd = m_hWnd;
        pTI->uId = (UINT)hitem;
        pTI->lpszText = LPSTR_TEXTCALLBACK;
        pTI->rect = rect;

        return pTI->uId;
    }

    return -1;
}

void CFishTreeCtrl::PreSubclassWindow() 
{
	CTreeCtrl::PreSubclassWindow();

//	EnableToolTips(TRUE);
}

BOOL CFishTreeCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	// Do not process the message from built in tooltip 
	if( nID == (UINT)m_hWnd &&
		(( pNMHDR->code == TTN_NEEDTEXTA && pTTTA->uFlags & TTF_IDISHWND ) ||
		( pNMHDR->code == TTN_NEEDTEXTW && pTTTW->uFlags & TTF_IDISHWND ) ) )
		return FALSE;

	// Get the mouse position
	const MSG* pMessage;
	CPoint pt;
	pMessage = GetCurrentMessage();
	ASSERT ( pMessage );
	pt = pMessage->pt;
	ScreenToClient( &pt );

	UINT nFlags;
	HTREEITEM hitem = HitTest( pt, &nFlags );
	strTipText = GetItemText( (HTREEITEM ) nID);

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif
	*pResult = 0;

	return TRUE;    // message was handled
}

