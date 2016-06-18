#pragma once

/**********************************************************************
**
**	CustomTabCtrl.h : include file
**
**	by Andrzej Markowski June 2004
**
**  modified by youngchang park (eternalbleu@gmail.com), November 2005
**
**********************************************************************/

#include <Afxtempl.h>
#include <afxcmn.h>

#include "themeutil.h"

// CustomTabCtrlItem

#define TAB_SHAPE1		0		//  Invisible

#define TAB_SHAPE2		1		//	 __
								//	| /
								//	|/

#define TAB_SHAPE3		2		//	|\
								//	|/

#define TAB_SHAPE4		3		//	____________
								//	\          /
								//   \________/

#define TAB_SHAPE5		4		//	___________
								//	\          \
								//	  \________/

#define HITTEST_RET_TRUE            1
#define HITTEST_RET_CLOSE           2
#define HITTEST_RET_FALSE           0

#define RECALC_PREV_PRESSED			0
#define RECALC_NEXT_PRESSED			1
#define RECALC_ITEM_SELECTED		2
#define RECALC_RESIZED				3
#define RECALC_FIRST_PRESSED		4
#define RECALC_LAST_PRESSED			5
#define RECALC_EDIT_RESIZED			6

#define MAX_LABEL_TEXT				255

typedef struct _CTC_NMHDR 
{
    NMHDR hdr;
	int	nItem;
	TCHAR pszText[MAX_LABEL_TEXT];
	LPARAM lParam;
	RECT rItem;
	POINT ptHitTest;
	BOOL fSelected;
	BOOL fHighlighted;
} CTC_NMHDR;


//////////////////////////////////////////////////////////////////////////
// 좌측과 우측의 아이콘 첨부를 위해서 추가한 부분
extern CRect                       m_rectItemIconRegion;
extern CRect                       m_rectItemFunctRegin;

class CFishTabCtrlItem
{
	friend class CFishTabCtrl;
public:
    enum {
        FISH_TAB_INNER_PAD_LEFT =   3,
        FISH_TAB_INNER_PAD_TOP  =   2,
        FISH_TAB_INNER_PAD_RIGHT    =   0,
        FISH_TAB_INNER_PAD_BOTTOM   =   2,

        FISH_TAB_ITEM_INTERVAL  =   1,
        FISH_TAB_FIXEDWIDTH     =   80, //px


        FISH_TAB_DEFLATE_WIDTH_RIGHT    =   0,
        FISH_TAB_OUTLINE_COLOR  =   RGB(0x42, 0x59, 0x73),
        FISH_TAB_BACKGROUND_COLOR_SEL    =   RGB(0xFE, 0xFA, 0xFE),
        FISH_TAB_BACKGROUND_COLOR_DESEL  =   RGB(0x69, 0x8B, 0xAB),
        FISH_TAB_FOREGROUND_COLOR_SEL   =   RGB(0x02, 0x42, 0x9C),
        FISH_TAB_FOREGROUND_COLOR_DESEL =   RGB(0x06, 0x02, 0x06),
    };

private:
								CFishTabCtrlItem(CString sText, LPARAM lParam);
	void						ComputeRgn();
	void						Draw(CDC& dc, CFont& font);
	UINT						HitTest(CPoint pt);
	void						GetRegionPoints(const CRect& rc, CPoint* pts) const;
	void						GetDrawPoints(const CRect& rc, CPoint* pts) const;
	void						operator=(const CFishTabCtrlItem &other);

private:
	CString						m_sText;                // 탭 아이템의 스트링
	LPARAM						m_lParam;               // 탭 아이템의 데이터
	CRect						m_rect;                 // 탭 아이템의 그림 전체 영역 사각형
	CRect						m_rectText;             // 탭 아이템의 스트링 영역. 에디트 박스를 띄울때 사용된다. 그림을 그릴때에는 사용되지 않는다.
	CRgn						m_rgn;			        //
	BYTE						m_bShape;               // 탭 아이템의 모양. 위치에 따라 변하는 것으로 생각됨
	BOOL						m_fSelected;            // 탭 아이템의 선택 여부
	BOOL						m_fHighlighted;         // 탭 아이템의 하이라이트
	BOOL						m_fHighlightChanged;
    BOOL                        m_bMouseOver;
};

// CFishTabCtrl

// styles
#define CTCS_FIXEDWIDTH			1		// Makes all tabs the same width. 
#define CTCS_FOURBUTTONS		2		// Four buttons (First, Prev, Next, Last) 
#define CTCS_AUTOHIDEBUTTONS	4		// Auto hide buttons
#define CTCS_TOOLTIPS			8		// Tooltips
#define CTCS_MULTIHIGHLIGHT		16		// Multi highlighted items
#define CTCS_EDITLABELS			32		// Allows item text to be edited in place
#define CTCS_DRAGMOVE			64		// Allows move items
#define CTCS_DRAGCOPY			128		// Allows copy items

#define CTCS_CLOSEREGION_HITTEST    256     // CLOST BUTTON ENABLE. by eternalbleu

// hit test
#define CTCHT_ONFIRSTBUTTON		-1
#define CTCHT_ONPREVBUTTON		-2
#define CTCHT_ONNEXTBUTTON		-3
#define CTCHT_ONLASTBUTTON		-4
#define CTCHT_NOWHERE			-5

// notification messages
#define CTCN_CLICK				1
#define CTCN_RCLICK				2
#define CTCN_SELCHANGE			3
#define CTCN_HIGHLIGHTCHANGE	4
#define CTCN_ITEMMOVE			5
#define CTCN_ITEMCOPY			6
#define CTCN_LABELUPDATE		7
#define CTCN_OUTOFMEMORY		8

#define CTCN_HITCLOSEREGION     9       // 아이템 클로시 부모에게 발신되는 메시지

#define CTCID_FIRSTBUTTON		-1
#define CTCID_PREVBUTTON		-2
#define CTCID_NEXTBUTTON		-3	
#define CTCID_LASTBUTTON		-4
#define CTCID_NOBUTTON			-5

#define CTCID_EDITCTRL			1

#define REPEAT_TIMEOUT			250

// error codes
#define CTCERR_NOERROR					0
#define CTCERR_OUTOFMEMORY				-1
#define CTCERR_INDEXOUTOFRANGE			-2
#define CTCERR_NOEDITLABELSTYLE			-3
#define CTCERR_NOMULTIHIGHLIGHTSTYLE	-4
#define CTCERR_ITEMNOTSELECTED			-5
#define CTCERR_ALREADYINEDITMODE		-6
#define CTCERR_TEXTTOOLONG				-7
#define CTCERR_NOTOOLTIPSSTYLE			-8
#define CTCERR_CREATETOOLTIPFAILED		-9

// button states
#define BNST_INVISIBLE			0
#define BNST_NORMAL				DNHZS_NORMAL
#define BNST_HOT				DNHZS_HOT
#define BNST_PRESSED			DNHZS_PRESSED

#define CustomTabCtrl_CLASSNAME    _T("CFishTabCtrl")  // Window class name

class CFishTabCtrl : public CWnd
{
public:
    enum {
        FISH_TAB_INNER_PAD_LEFT     =   CFishTabCtrlItem::FISH_TAB_INNER_PAD_LEFT,
        FISH_TAB_INNER_PAD_RIGHT    =   CFishTabCtrlItem::FISH_TAB_INNER_PAD_RIGHT,
        FISH_TAB_INNER_PAD_TOP      =   CFishTabCtrlItem::FISH_TAB_INNER_PAD_TOP,
        FISH_TAB_INNER_PAD_BOTTOM   =   CFishTabCtrlItem::FISH_TAB_INNER_PAD_BOTTOM,

        FISH_TAB_OUTLINE_COLOR  =   CFishTabCtrlItem::FISH_TAB_OUTLINE_COLOR,
        FISH_TAB_BKGND_COLOR    =   CFishTabCtrlItem::FISH_TAB_BACKGROUND_COLOR_DESEL,
        FISH_TAB_ITEM_INTERVAL  =   CFishTabCtrlItem::FISH_TAB_ITEM_INTERVAL,
        FISH_TAB_DEFLATE_WIDTH_RIGHT    =   CFishTabCtrlItem::FISH_TAB_DEFLATE_WIDTH_RIGHT,

        FISH_TAB_FIXEDWIDTH =   CFishTabCtrlItem::FISH_TAB_FIXEDWIDTH,

        FUNCTION_BTN_WIDTH  =   24,
        FUNCTION_BTN_HEIGHT =   21,
    };
    
public:

	// Construction

	CFishTabCtrl();
	virtual						~CFishTabCtrl();
	BOOL						Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID);

	// Attributes

	int							GetItemCount() {return m_aItems.GetSize();}
	int							GetCurSel() { return m_nItemSelected; }
	int							SetCurSel(int nItem);
	int							IsItemHighlighted(int nItem);
	int							HighlightItem(int nItem, BOOL fHighlight);
	int							GetItemData(int nItem, DWORD& dwData);
	int							SetItemData(int nItem, DWORD dwData);
	int							GetItemText(int nItem, CString& sText);
	int							SetItemText(int nItem, CString sText);
	int							GetItemRect(int nItem, CRect& rect) const;
	int							SetItemTooltipText(int nItem, CString sText);
	void						SetDragCursors(HCURSOR hCursorMove, HCURSOR hCursorCopy);
	BOOL						ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);
	void						SetControlFont(const LOGFONT& lf, BOOL fRedraw=FALSE);
	static const LOGFONT&		GetDefaultFont() {return lf_default;}

	// Operations

	int							InsertItem(int nItem, CString sText, LPARAM lParam=0);
	int							DeleteItem(int nItem);
	void						DeleteAllItems();
	int							MoveItem(int nItemSrc, int nItemDst);
	int							CopyItem(int nItemSrc, int nItemDst);
	int							HitTest(CPoint pt, bool chkClose, int& nExt);
	int							EditLabel(int nItem);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFishTabCtrl)
	afx_msg BOOL				OnEraseBkgnd(CDC* pDC);
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LONG				OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LONG				OnThemeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void				OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void				OnPaint();
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void				OnTimer(UINT nIDEvent);
	afx_msg void				OnUpdateEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	

private:
	void						RecalcLayout(int nRecalcType,int nItem);
	void						RecalcEditResized(int nOffset, int nItem);
	void						RecalcOffset(int nOffset);
	int							RecalcRectangles();
	BOOL						RegisterWindowClass();
	int							ProcessLButtonDown(int nHitTest, UINT nFlags, CPoint point, int nExData = NULL);
	int							ProcessLButtonUp(int nHitTest, UINT nFlags, CPoint point);
	int							MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
	int							CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
	int							SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
	int							HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
	void						DrawGlyph(CDC& dc, CPoint& pt, int nImageNdx, int nColorNdx);
	void						DrawBkLeftSpin(CDC& dc, CRect& r, int nImageNdx);
	void						DrawBkRightSpin(CDC& dc, CRect& r, int nImageNdx);
	BOOL						NotifyParent(UINT code, int nItem, CPoint pt);
	int							EditLabel(int nItem, BOOL fMouseSel);
private:
	static LOGFONT				lf_default;
	static BYTE					m_bBitsGlyphs[];
	HCURSOR						m_hCursorMove;
	HCURSOR						m_hCursorCopy;
	int							m_nItemSelected;
	int							m_nItemNdxOffset;
	int							m_nItemDragDest;
	int							m_nPrevState;
	int							m_nNextState;
	int							m_nFirstState;
	int							m_nLastState;
	int							m_nButtonIDDown;
	DWORD						m_dwLastRepeatTime;
	COLORREF					m_rgbGlyph[4];
	CBitmap						m_bmpGlyphsMono;
	HBITMAP						m_hBmpBkLeftSpin;
	BOOL						m_fIsLeftImageHorLayout;
	MY_MARGINS					m_mrgnLeft;
	MY_MARGINS					m_mrgnRight;
	HBITMAP						m_hBmpBkRightSpin;
	BOOL						m_fIsRightImageHorLayout;
	CToolTipCtrl				m_ctrlToolTip;
	CEdit						m_ctrlEdit;
	CFont						m_Font;
	CFont						m_FontSelected;
	CArray <CFishTabCtrlItem*,CFishTabCtrlItem*>	m_aItems;
};
