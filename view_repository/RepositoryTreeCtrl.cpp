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

map<int, CFishTreeCtrl::FISHCUSTOMICON>	CFishTreeCtrl::m_mapAniData;  // �ִϸ��̼� �ϴ� �������� ��ð� �����ϱ� ���� �κ�

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
	// �⺻������ ����ϴ� ��Ʈ�� ��Ÿ���� �����Ѵ�.
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
OnNMCustomDraw	���� �������� ���������� �׸��� �����. NW_CUSTOMDRAW �޽����� ó���Ѵ�.
				�޽��� ���� ���� �߰��ؾ���.
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
                animate_UPDATE( dwData);               // �ִϸ��̼� ������ ������Ʈ
                
                customdraw_RootBkgnd(hITEM, pDC);    //BEGIN: item background drawing
                customdraw_Selected(hITEM, pDC);     //BEGIN: ���� �������� ���õǾ��� ��쿡 ȿ���� �ش�.
                customdraw_Text(hITEM, pDC);         //BEGIN: ���� �������� �ؽ�Ʈ�� �׷��ش�.
                customdraw_Icon(hITEM, pDC);         //BEGIN: ������ ��� ���� �̿��ؼ� ������ �̹����� ������ �������� �׸���.
                customdraw_DropHightlight(hITEM, pDC);    //BEGIN: ������ �̵��ÿ� �̵������� Ȯ���� ������ ���ؼ� �ϴܿ� �׸��� �׸���.
                *pResult = CDRF_SKIPDEFAULT;		//Your application drew the item manually. The control will not draw the item. This occurs when dwDrawStage equals CDDS_ITEMPREPAINT.
            }
			break;
		default:
			break;
	}
}

/************************************************************************
customdraw_RootBkgnd ��Ʈ �������� ��� �׸��� �׸���.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/14: ����� ũ�⿡ �µ��� �̹����� resize �ϵ��� ������.
************************************************************************/
void CFishTreeCtrl::customdraw_RootBkgnd(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
    if ( !GetParentItem(hITEM) )    // COND : ��Ʈ ������ �̶��
    {
        //////////////////////////////////////////////////////////////////////////
        // �̹��� �ε�
        BITMAP bm;
        CBitmap*     pBMP =   NULL;

        if(GetItemState(hITEM, TVIS_SELECTED)  &   TVIS_SELECTED)   // COND : ���õ� ���¶�� 1��, �ƴ϶�� 0���� �ε�
        {
            pBMP =   m_pBMPManager->getGroupBk(1);
        }   else    {
            pBMP =   m_pBMPManager->getGroupBk(0);
        }
        pBMP->GetBitmap(&bm);
        
        //////////////////////////////////////////////////////////////////////////
        // �̹��� �����
        CRect rectBg;
        GetItemRect(hITEM, rectBg, FALSE);

        CDC buffDC;
        buffDC.CreateCompatibleDC(pDC);
        CBitmap* pOldBitmap = buffDC.SelectObject( pBMP );

        pDC->StretchBlt(rectBg.left, rectBg.top, rectBg.Width(), rectBg.Height(), &buffDC, 2, 0, bm.bmWidth-4, bm.bmHeight, SRCCOPY);

        pDC->BitBlt(rectBg.left, rectBg.top, 2, bm.bmHeight, &buffDC, 0, 0, SRCCOPY);
        pDC->BitBlt(rectBg.right - 2, rectBg.top, 2, bm.bmHeight, &buffDC, bm.bmWidth-2, 0, SRCCOPY);

        //////////////////////////////////////////////////////////////////////////
        // �ڵ� ����
        buffDC.SelectObject(pOldBitmap);
    }
}

/************************************************************************
customdraw_Selected		���� �������� ��濡 Ư�� ȿ���� �ֱ� ���ؼ� ����
@param  : 
@return : 
@remark : 
	������ ��� Ȥ�� ��Ÿ Ư��ȿ���� �ֱ� ���ؼ� ��������� ���翡�� �״���
	���� �־� ������ �ʴ� �༮��
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18 : CREATE
    2005/12/14 : ���� �������� ���ý� ������ �����.
    2005/12/22 : ����� �Ϻκи� �����.
************************************************************************/
void CFishTreeCtrl::customdraw_Selected(HTREEITEM hITEM, CDC* pDC)
{
    //////////////////////////////////////////////////////////////////////////
    // �������� ���õ� ������ ȿ���� �׸���.
    CRect itemRT;
    GetItemRect(hITEM, &itemRT, FALSE);

    if ( GetItemState(hITEM, TVIS_SELECTED) && TVIS_SELECTED)   // COND : ���õ� �������̶��
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
customdraw_Text		Ŀ���� ��ο쿡�� �ؽ�Ʈ�� ����ϴ� ��ƾ
@param  : 
@return : 
@remark : 
	�⺻������ ���ð� ���ý��� ������ �и���.
	���ýÿ��� �������� Drag ���¿� ���� �ٸ� ������� �����
	(�̿��� ���ɼ��� ���� �ҽ� -_-;; �ʹ����� ġ�� ������. -_-;;;)
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18  :   CREATED
    2005/12/13  :   dc setbkmode ������ opaque �� �����Ͽ� ���� ���ý� �̻��� ����� ���� �ذ���.
************************************************************************/
void CFishTreeCtrl::customdraw_Text(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
	BOOL expand = FALSE;

    //////////////////////////////////////////////////////////////////////////
    // �������� �ؽ�Ʈ�� �׸���.
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
// ���� ä���� ���� ���� ����Ʈ ������ ��Ÿ ������ ���� ������ ����Ѵ�.
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

		// right ������ ���δ�.
        itemRT.DeflateRect(0, 0, 6, 0);
        pDC->DrawText(strItemText, &textRT, DT_CALCRECT|DT_EDITCONTROL);

        pDC->SelectObject(pCountFont);
        pDC->DrawText(strUnreadQ, &unreadQRT, DT_CALCRECT|DT_EDITCONTROL);
        pDC->SelectObject(pOldFont);

		// Tooltip Hit ó������ ����� ����
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
    // GRP �������� EXPAND, UNEXPAND ǥ��
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
GetHITEMImageID �� �������� Ư���� ���� �������� �����Ѵ�.
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
customdraw_Icon �������� �׸��� �κ�
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/14: �������� ����� ������ singleton �� �̿��ϵ��� ����
                �׷� �������� ��� ä���� ������ ���� �ٸ� �������� �׸�
************************************************************************/
void CFishTreeCtrl::customdraw_Icon(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  /*=   NULL*/)
{
    //////////////////////////////////////////////////////////////////////////
    // �������� ���� �Ǻ��� ���ؼ� �߰��� �κ�
    CDlg_Subscribe* pParent    =   (CDlg_Subscribe*)GetParent();
	if(!pParent) return;
    auto_ptr<FISH_REPITEM> it  =    auto_ptr<FISH_REPITEM>(new FISH_CHANNEL(GetItemData(hITEM), _T("")) );
	// �׷� �������� �����ϴ�.
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
		// �̹����� �߰����� �������� �������� �׸��� �ʴ´�.
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



    // ������ �������� �״�� �ΰ� �ٽ� �׸��� �׸���.
    if ( hITEM != HTREEITEM_NONE && m_mapAniData.find( GetItemData(hITEM) ) != m_mapAniData.end() )   {
        animate_DrawItem( &(m_mapAniData.find( GetItemData(hITEM) )->second) );
        return;
    }
}

/************************************************************************
customdraw_Icon �����ܸ� ���α׷��� �� �ʿ䰡 ���� �� �̿��Ѵ�.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/20:CREATED
    2005/12/23:������ ��� �׸��⸦ ������  (filckering �ذ��ʿ���)
************************************************************************/
void CFishTreeCtrl::animate_DrawItem(CFishTreeCtrl::FISHCUSTOMICON* data)
{
    //////////////////////////////////////////////////////////////////////////
    // �������� ���� �Ǻ��� ���ؼ� �߰��� �κ�
    CDlg_Subscribe* pParent    =   (CDlg_Subscribe*)GetParent();
	if(!pParent) return;
    auto_ptr<FISH_REPITEM> it  =    auto_ptr<FISH_REPITEM>(new FISH_CHANNEL(GetItemData(data->hITEM), _T("")) );
	// �׷� �������� �����ϴ�.
    if(!pParent->FindItem(it->id, it)) return;

    CRect rect;
    GetItemRect(data->hITEM, &rect, FALSE);
    
    CClientDC dc(this);
    CMemDC pDC(&dc, &rect);

	BOOL IsFocused = FALSE;
	BOOL IsSelected = FALSE;
	BOOL HasUnread = FALSE;

    customdraw_RootBkgnd(data->hITEM, pDC);    //BEGIN: item background drawing
    customdraw_Selected(data->hITEM, pDC);     //BEGIN: ���� �������� ���õǾ��� ��쿡 ȿ���� �ش�.
    customdraw_Text(data->hITEM, pDC);         //BEGIN: ���� �������� �ؽ�Ʈ�� �׷��ش�.
//    customdraw_Icon(data->hITEM, pDC);         //BEGIN: ������ ��� ���� �̿��ؼ� ������ �̹����� ������ �������� �׸���.

    // ���� ������ �׸���
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

    customdraw_DropHightlight(data->hITEM, pDC);    //BEGIN: ������ �̵��ÿ� �̵������� Ȯ���� ������ ���ؼ�
}

/************************************************************************
customdraw_DropHightlight	���� �׸��� �������� drag �� drop �������� ��� �ϴܿ�
						���� ����Ʈ�� �˸��� ���� �׷��ش�.
@param  : 
@return : 
@remark : 
	�̳༮�� ������ �Ϲ������� �ʴٴ� ���� ��.��; ��� ������ ���ΰ�?
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/18  :   CREATED
    2005/12/22  :   �ϴܺ��� �� �簢�� �׸��� ����
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
OnBegindrag ���콺�� �巡�� �̺�Ʈ ó�� ��Ʈ
@param  : 
@return : 
@remark : mouse dragging�� OnBegindrag, OnMouseMove, OnLButtonUp 3���� �޼ҵ忡�� �̺�Ʈ ó����
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/11/15  :   CREATE
    2005/12/15  :   ù��° �׷� �������� �̵����� ���ϵ��� ������
    2005/01/30  :   �̹����� �̿��� ä���� �̵� ǥ���� �ƴ� Transparent Dlg �� �̿��� ǥ������ ����
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
	// �������� �̵��� 3������ �����ؼ� ó���Ѵ�.
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
OnMouseMove ���콺�� �̵��� ���� ���� ǥ��
@PARAM  : 
@RETURN : 
@REMARK : 
    ���콺�� �巡��� ���ؾ��� �ൿ�� �����ش�.
    ���콺�� ��Ŀ���� Ʈ���� ��� ��츦 ǥ���ϱ� ���ؼ� ���콺 �̺�Ʈ Ʈ��Ŀ��
    Ȱ��ȭ ��Ų��.
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
        _BEG_IF_WINDOWS_VERSION_IS_OVER_WIN2000_    // 2000 ������ ��쿡�� â�� ����ȭ�� �Ұ��ϱ� ������ �������� �ʴ´�.
        m_ctrlDragDlg->DragLeave(this);
        _END_IF_WINDOWS_VERSION_IS_OVER_WIN2000_

        //////////////////////////////////////////////////////////////////////////
        // ������ �������� ���� ����
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
        // ������ ���̸� ������� ���� ����
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
        // ���콺 �̵��� �������� ����
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
    // ���콺 �̺�Ʈ Ʈ��Ŀ
    TRACKMOUSEEVENT trackmouseevent;
    trackmouseevent.cbSize = sizeof(trackmouseevent);
    trackmouseevent.dwFlags = TME_LEAVE | TME_HOVER;
    trackmouseevent.hwndTrack = GetSafeHwnd();
    trackmouseevent.dwHoverTime = 0;
    ::_TrackMouseEvent(&trackmouseevent);

    CTreeCtrl::OnMouseMove(nFlags, point);
}

/************************************************************************
OnLButtonUp		���콺 �� �̺�Ʈ ó��
@param  : 
@return : 
@remark : 
	ä��, �׷�, ��Ƽ ä�� �̵������� ���ܸ� ó���ϴ� �κ�
	����� �����丵 �ʿ���
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
        // �巡�� �Ϸ����� ���� ó���κ�
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
        // �巡���� �߰� ó�� �κ�
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
        // �巡���� ��ó�� �κ�
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
isChildOf 2���� �׸��� ���Ӽ�(�θ�, �ڽ�)���踦 �Ǵ��Ѵ�.
@param  : 
    hitemChild �ڽ� �ڵ�
    hitemSuspectedParent �ǽɵǴ� �θ��� �ڵ�
@return : 
    TRUE �ڽ� �ڵ��� �´ٸ�
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
moveGroupPos    �׷� �������� ���� ��ġ ����
@param  : 
    dest �ڵ��� �ڿ� �������� �̵�
@return : 
    ����� �������� ��ġ
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
moveChannelPos  ä�� �������� ���� ��ġ ����
@param  : 
@return : 
    ���� �ٲ� ��ġ�� �ڵ��� ����
@remark : 
    moveBetweenSibling ���� ���ÿ� ����� �ϴµ� ���� �׳� ������ ���װ� �߻��ϱ� ������
    �� ���·� �����ϰ��� (05. 11. 17)
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
updateItem  ���ڷ� ���� ���� �����ؼ� Ʈ�� �信�� �������� ������ �����Ѵ�.
@param  : 
@return : 
@remark : 
    hItem �� ��ȿ���� assertion �Ѵ�.
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
moveBetweenSibling ������ ������ �������� �̵����� �۾� �޼ҵ�
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
        // ������ �׷� ���� �մ� ä���� �̵���Ų��.
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
        // �׷� �̵��Ŀ� ������ ������ ���¶�� ������ ���·� �����.
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
moveChannelToGroup ������ �̵� �޼ҵ�
@param  : 
    srcITEM �ҽ� ������ �ڵ�
    dstITEM ������ ������ �ڵ�
@return : 
    HTREEITEM	���Ӱ� ���Ե� �������� �ڵ�
@remark : 
	�ҽ� �������� ��Ʈ��� �̴� �����ϰ� NULL�� �����Ѵ�.
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

    {// ä���� ���� �ִ� ������ �׷�, �̵� ���� �׷쿡 ���ؼ� ��ó���� �Ѵ�. (�ڽ��� ���������� ������ Expand�� �������� �ʴ� ���� ���� �ϱ� ���Ѱ�)
        TVITEM structItem;
        structItem.cchTextMax    = FISH_REPOSITORY_LABELEDIT_MAX_LENGTH;
        structItem.pszText       = sztBuffer;
        structItem.mask          = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;

        /// ���� �׷�
//        structItem.hItem         = hPrevItem;
//        GetItem(&structItem); // get information of the dragged element
//        structItem.cChildren     = GetChildItem(hPrevItem) ? 1 : 0;
//        if (!structItem.cChildren)   
//            structItem.state    ^=  TVIS_EXPANDED;
//         SetItem(&structItem);

        /// �̵� �� �׷�
        structItem.hItem         = dstITEM;
        GetItem(&structItem); // get information of the dragged element
        structItem.cChildren     = GetChildItem(dstITEM) ? 1 : 0;
        SetItem(&structItem);
    }
    animate_UPDATE( GetItemData(RET) );

    return RET;
}

/************************************************************************
InvalidateitemRect    �������� �ڵ��� ���ͼ� �� �������� ������ �ٽ� �׸���.
@param  :
    hITEM   �ٽ� �׷����� ������
@return : 
@remark : 
    2005 11 17 proposed by moonknit
@author : youngchang (eternalbleu@gmail.com)
@history:
    created at 2005/11/17   by youngchang
    2005/01/19  :   MODIFIED    �̵��� �ϴ� ���� �ذ�. OnCustomDraw �� �ذ��߱� ������ OnCustomDraw�� ������ ����
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
InvalidateAll   ��� �������� ���� �׸���. (explicitly)
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
lbuttonup_ChannelDrag ä���� �巡���� �������� ó��
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
lbuttonup_GroupDrag �׷��� �巡���� �������� ó��
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
	// �׷� �������� �̵� �� ó��
	// �׷� �������� ä�� �������� ������ �ִ� ��� ó��
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
 * ���� Ŭ���� ��ġ�� �������� �����͸� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in CPoint) pt - �˻��� ��ġ
 * (out BOOL&) bfind - �߰߿���
 *
 * @Return
 * (DWORD) �߰ߵ� �������� ������
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
animate_ADD ���ڷ� ���� ID�� ���� �������� �ִϸ��̼� ����Ʈ�� �߰��Ѵ�.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/23:ID�� �߰��� �� �ֵ��� ������
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
    2005/12/22:���� �ð� ��ƾ ����. SmartPtr ���� ���� ����. ����� ������ �߸��׸��� ���� ����
    2005/12/23:ID�� �߰��� �� �ֵ��� ������
	2006/03/02:������ ����� Validation Check by moonknit
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
animate_UPDATE  ���� �������� �ִϸ��̼� ������ ������Ʈ �Ѵ�.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/23:CREATED
    2005/12/23:ID�� �߰��� �� �ֵ��� ������
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
animate_ALLUPDATE   ��ü �������� �ִϸ��̼� ������ ������Ʈ �Ѵ�.
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
GetHandleFromData   ���ڷ� ���� �����͸� �̿��ؼ� �������� ã�´�.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:���̴� �����۸� �˻��ϴ� ���� ����
    2005/12/22:1BASE Counting �߰���.
    2005/12/22:�ڵ��� ã���鼭 ���ÿ� �ܰ��簢���� ��´�.
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
    �� ����Ʈ ���� �ð� �ʰ� ó��
    ������ ���ϸ��̼� Ÿ�̸�
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/20:CREATED
    2005/12/21:��� ��ǻ�Ϳ����� �ǵ尡 �ʹ� ���� ����� �ִϸ��̼��� Ÿ�̸ӿ��� ó����. (deleted)
    2005/12/22:�������� �ִϸ��̼��� ������ �ٽ� �׷��ֱ� �߰���.
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
EditLabel   ������ �ڵ��� ���ڷ� �� �� ����Ʈ ���ν����� ����
@param  : 
@return : 
@remark : 
    �޽��� ������� �ۼ����� �ʾұ� ������ ���Ŀ� ������ �ʿ��� ��. (���� �ð� ������ �Ѥ�;;)
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/22:CREATED
    2005/12/23:���ν��� ���� �ϼ���. (�޽��� ������� ������ �ʿ� ����)
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
    // ��ũ�ѹ� ���߱�
//    ShowScrollBar(SB_HORZ, TRUE);

    // ��ũ�ѹ� ��Ű��
    CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, &rect);

    //////////////////////////////////////////////////////////////////////////
    // �ƹ��� �������� ���� ��쿡 ����� ǥ�õǾ����.
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

            if ( !SelectDropTarget(hITEM) ) FishMessageBox( _T("�߸��� ����� ���õǾ����ϴ�.") );
        }
    }
 	return 0;
}

/************************************************************************
OnMouseHover    ���콺�� �������� �����̴� ���ߴ� ������ ȣ��ȴ�.
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

    GetClientRect(&ierect);     // client �� ������ ���Ұ�� ��ũ�ѹٰ� �����Ǿ��� ��� �� ũ�Ⱑ ����ȴ�.

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

