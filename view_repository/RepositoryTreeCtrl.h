/********************************************************************
	created:	2005/11/15
	created:	15:11:2005   10:37
	filename: 	RepositoryTreeCtrl.h
	file path:	D:\my_document\#Onnet\SourceCode\SubModule\UnitTest\RepositorySDITest
	file base:	RepositoryTreeCtrl
	file ext:	h
	author:		youngchang (eternalbleu@gmail.com)
	purpose:    fish repository view tree 
    remark:
        multiple selection 지원하기 위해서 CMultiSelectableTreeCtrl 상속
        title : A multi-select tree control class
        refer : http://www.codeguru.com/Cpp/controls/treeview/misc-advanced/article.php/c629/
        author : The Chard 
    history:    
    2005 11 15  refactoring
    2005 11 17  draft version completed
    2005 11 18  visual source safe checkin
*********************************************************************/
#if !defined(AFX_FTREE_REPOSITORY_H__DB8A7200_D6CC_4762_84E3_1963769A159A__INCLUDED_)
#define AFX_FTREE_REPOSITORY_H__DB8A7200_D6CC_4762_84E3_1963769A159A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../FISH.h"
#include "../smartptr.h"
#include "../fish_def.h"
#include "../ctrl/MemDC.h"
#include "TreeSkinVerticalScrollbar.h"
#include <vector>
#include <map>
#include <cassert>
using namespace std;

#define _SHOW_UNREAD_POST_QUANTITY
#define SRCMASK     0x00220326 // mask raster op   
#define HTREEITEM_NONE	NULL
#define FISHANIITEMMAP  map<int, FISHCUSTOMICON>

//#define USE_CUSTOMDRAW_SCROLLBAR

class CFishBMPManager;
class CTreeSkinVerticalScrollbar;
class CFishTransparentDragDlg;

/////////////////////////////////////////////////////////////////////////////
// CFishTreeCtrl window
class CFishTreeCtrl : public CTreeCtrl
{
    enum {
        GRP_EXPANDED_LEFT_PADDING       =   12,
        GRP_EXPANDED_IMPACT_BMP_WIDTH   =   9,
        GRP_EXPANDED_IMPACT_BMP_HEIGHT  =   5,
    };

// Construction
public:
	CFishTreeCtrl();

// Attributes
public:
    enum {
        FISH_GROUP_DEPTH    =   0,
        FISH_CHANNEL_DEPTH  =   1,
        FISH_REPOSITORY_ANIMATION_INTERVAL      =   200,
        FISH_REPOSITORY_ANIMATION_RELEASE_TIME  =   400,
        FISH_REPOSITORY_LABELEDIT_CHK_INTERVAL  =   2000,
        FISH_REPOSITORY_LABELEDIT_UID           =   999999,
        FISH_REPOSITORY_LABELEDIT_MAX_LENGTH    =   255,

        FISH_REPOSITORY_BG_COLOR                =   RGB(0xF8, 0xF8, 0xF8),

		FISH_REPOSITORY_BG_NONFOCUS_S			=   RGB(0x9b, 0xb5, 0xd3),
        FISH_REPOSITORY_BG_COLOR_N				=   RGB(0xF8, 0xF8, 0xF8),
        FISH_REPOSITORY_CHANNEL_FG_COLOR1_N		=   RGB(0x86, 0x86, 0x86),      // 포스트 개수가 0개인 경우 글자 색 (채널)
        FISH_REPOSITORY_CHANNEL_FG_COLOR2_N		=   RGB(0x00, 0x00, 0x00),      // 포스트 개수가 0이 아닌 경우 글자색 (채널)
        FISH_REPOSITORY_GRP_FG_COLOR1_N			=   RGB(0x00, 0x00, 0x00),      // 포스트 개수가 0개인 경우 글자 색 (그룹)
        FISH_REPOSITORY_GRP_FG_COLOR2_N			=   RGB(0x00, 0x00, 0x00),      // 포스트 개수가 0이 아닌 경우 글자색 (그룹)
		FISH_REPOSITORY_BG_COLOR_S				=   RGB(0x51, 0x7d, 0xb2),
        FISH_REPOSITORY_FG_COLOR_S				=   RGB(0xFF, 0xFF, 0xFF),
        FISH_REPOSITORY_READCOUNT_FG_COLOR_S    =   RGB(0xFF, 0xFF, 0xFF),
        FISH_REPOSITORY_READCOUNT_FG_COLOR_N    =   RGB(0x00, 0x00, 0x00),
        FISH_REPOSITORY_DROPHIGHLIGHT_COLOR     =   RGB(0x10, 0x10, 0x10),

        FISH_REPOSITORY_MASK_COLOR              =   RGB(0xEF, 0xEF, 0xEF),
        FISH_REPOSITORY_ANIMATE_MASK_COLOR      =   RGB(0x00, 0xB6, 0xFF),

        FISH_REPOSITORY_ITEM_TEXT_LEFT_MARGIN   =   29,
        FISH_REPOSITORY_TREE_ITEM_DEPTH_INDENT  =   0,

        FISH_REPOSITORY_ITEM_GROUP_ICON_LEFT_MARGIN	=   8,
        FISH_REPOSITORY_ITEM_GROUP_ICON_TOP_MARGIN  =   4,
        FISH_REPOSITORY_ITEM_GROUP_ICON_WIDTH       =   16,
        FISH_REPOSITORY_ITEM_CHANNEL_ICON_LEFT_MARGIN    =   8,
        FISH_REPOSITORY_ITEM_CHANNEL_ICON_TOP_MARGIN    =   4,
        FISH_REPOSITORY_ITEM_CHANNEL_ICON_WIDTH     =   16,

        FISH_REPOSITORY_ITEM_CHANNEL_TEXT_POSTQ_INTERVAL    =   3,

        FISH_REPOSITORY_ITEM_LABELEDIT_LEFT_MARGIN =   29,
        FISH_REPOSITORY_ITEM_LABELEDIT_RIGHT_MARGIN =   5,
        FISH_REPOSITORY_TREE_ITEM_HEIGHT        =   24,

        FISH_REPOSITORY_TREE_ITEM_GROUP_FONT_SIZE     =   14,
        FISH_REPOSITORY_TREE_ITEM_CHANNEL_FONT_SIZE   =   14,
        FISH_REPOSITORY_TREE_ITEM_POST_QUANTITY_FONT_SIZE     =   12,
    };

    enum DRAGTYPE
    {
        NONDRAG = 0,
        GROUPDRAG,
        CHANNELDRAG,
        MULTICHANNELDRAG,
    };

    vector<HTREEITEM>   m_vecDragITEM;      // 드래깅과 관련 된 처리에 필요한 부분
    DRAGTYPE            m_enumDragTYPE;
	CImageList*         m_pbmplistDragIMAGE;

    CFishBMPManager*     m_pBMPManager;    // 아이콘 전체를 관리하는 아이콘 매니저
    CFishTransparentDragDlg*    m_ctrlDragDlg;

	CFont   m_ftGroupDefault;                    // 아이템의 폰트 그리기에 필요한 부분
	CFont	m_ftGroupSelect;
    CFont   m_ftChannelDefault;
    CFont   m_ftChannelSelect;
    CFont   m_ftPostQuantitySelect;                  // 기타 상황에서 사용하는 폰트를 정의
    CFont   m_ftPostQuantityDefault;                  // 기타 상황에서 사용하는 폰트를 정의

	CPoint	m_ptContext;                    // 컨텍스트 메뉴 관련
    CPoint  m_ptLabelEdit;                  // 라벨 에디트 관련

    CEdit   m_ctrlEdit;

    HTREEITEM   m_hPrevSelectedItem;

#ifdef USE_CUSTOMDRAW_SCROLLBAR
    CTreeSkinVerticalScrollbar*  m_ctrlVerticalScrollBar;
#endif

public:                     // about ANIMATE
    struct FISHCUSTOMICON {
        HTREEITEM       hITEM;
        CRect           rtOUTLINE; 
        int             leftPADDING;
        int             nINDEX;
        CImageList*     pIMGLIST;
        FISHCUSTOMICON()   :   hITEM(0), rtOUTLINE(0, 0, 0, 0), leftPADDING(0), nINDEX(0), pIMGLIST(NULL) {}
    };

    static map<int, FISHCUSTOMICON>	m_mapAniData;  // 애니메이션 하는 아이템을 잠시간 저장하기 위한 부분

    void    animate_DrawItem(CFishTreeCtrl::FISHCUSTOMICON* data);
    void    animate_ADD(int ITEMID);    // 아이템의 애니메이션 시작 인터페이스
    void    animate_REMOVE(int ITEMID);
    void    animate_UPDATE(DWORD ITEMID);
    void    animate_ALLUPDATE();
    void    animate_FORCE_TIMER()   {   OnTimer(TID_REPOSITORY_ANIMATION);  };

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishTreeCtrl)
	public:
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFishTreeCtrl();

	// Generated message map functions
	protected:
protected:
	//{{AFX_MSG(CFishTreeCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    afx_msg LONG OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LONG OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()

    private:
    // 객체의 생성과 소멸 중 실패가능성이 존재하는 파트
    void _initialize();
    void _finalize();

public:
    // 스크롤바 관련
    void _initScrollBar();
    void PositionScrollBars();

private:
    // OnCustomDraw 세부 파트 메소드 추출
    void customdraw_RootBkgnd(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  =   NULL);
    void customdraw_Selected(HTREEITEM hITEM, CDC* pDC);
    void customdraw_Text(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  =   NULL);
    void customdraw_Icon(HTREEITEM hITEM, CDC* pDC, LPRECT paramRT  =   NULL);
   
	void customdraw_DropHightlight(HTREEITEM hITEM, CDC* pDC);

    // DRAG END ACTION : METHOD EXTRACTION
    void lbuttonup_ChannelDrag();
    void lbuttonup_GroupDrag();

public:
    // LABEL EDIT
    void    EditLabel(HTREEITEM hITEM, BOOL fMouseSel  =   FALSE);
    CImageList* CreateDragImageEx(HTREEITEM hitem);

public:
	void InvalidateAll();
	void ShowScrollbar();
	void        HideScrollbar();
	void        drawExpandedMark(CDC* pDC, CRect rt, int type = 0, BOOL expand = TRUE);
	DWORD		GetItemData2(CPoint pt, BOOL& bfind);   // written by moonknit
    HTREEITEM   GetHandleFromData(WORD data);

    // 아이템 종류 판별 인터페이스
    BOOL        isChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
    BOOL        isGroup(HTREEITEM paramHandle) { return GetParentItem(paramHandle) == NULL; }

    // 트리의 아이템의 정보를 변경시키는 메소드
    HTREEITEM   updateItem(HTREEITEM hItem, tstring text, unsigned int iImage, unsigned int iSelectedImage, DWORD data);

    // 아이템 위치 변경 인터페이스
    HTREEITEM   moveGroupPos(HTREEITEM srcItem, HTREEITEM destItem);		                // 그룹 아이템의 위치와 상태 정보 변경
    HTREEITEM   moveChannelToGroup(HTREEITEM hitemDrag, HTREEITEM hitemDrop);				// 채널 아이템의 위치와 상태 정보 변경
    HTREEITEM   moveChannelPos(HTREEITEM srcItem, HTREEITEM destItem);
    HTREEITEM	moveBetweenSibling(HTREEITEM srcItem, HTREEITEM destItem);

    // 아이템의 특정 영역 다시 그리기 인터페이스
    void        InvalidateItem(HTREEITEM hitem);
    BOOL        IsSelected(HTREEITEM hItem) const {return !!(TVIS_SELECTED & CTreeCtrl::GetItemState(hItem, TVIS_SELECTED));};

    // OVERRIDE
    HTREEITEM   HitTest(CPoint pt, UINT* pFlags = NULL);
    BOOL        DeleteItem (HTREEITEM hItem) {
        return CTreeCtrl::DeleteItem(hItem);
    }

	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTREE_REPOSITORY_H__DB8A7200_D6CC_4762_84E3_1963769A159A__INCLUDED_)
