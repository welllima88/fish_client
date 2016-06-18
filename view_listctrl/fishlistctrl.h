/********************************************************************
	CREATED:	2005/12/28
	CREATED:	28:12:2005   10:26
	FILENAME: 	fishlistctrl.h
	WRITER:		CFishListCtrl
	
	PURPOSE:
    REFER:  http://www.codeproject.com/listctrl/skinlist.asp
    HISTORY:    
        2005/12/28 : CREATED BY CFishListCtrl
*********************************************************************/
#if !defined(AFX_FISHLISTCTRL_H__6DFBBE09_CEE7_4AFD_99AE_00A751A51D1B__INCLUDED_)
#define AFX_FISHLISTCTRL_H__6DFBBE09_CEE7_4AFD_99AE_00A751A51D1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// fishlistctrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFishListCtrl window
#include "../smartptr.h"
#include "../fish_def.h"
#include "FishHeaderCtrl.h"
#include "SkinListCtrl.h"
#include <map>
using std::map;

class CFishListCtrl : public CSkinListCtrl
{
public:
    enum {
        // 리스트 드로윙에 필요한 매직넘버들
        FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR   =   RGB(0xEC, 0xEC, 0xEC),
        FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR  =   RGB(0xF8, 0xF8, 0xF8),
        FISH_VIEWINFO_LISTCTRL_FGCOLOR_N        =   RGB(0x82, 0x82, 0x82),
        FISH_VIEWINFO_LISTCTRL_SUBJECT_FGCOLOR_N=   RGB(0x00, 0x00, 0x00),
        FISH_VIEWINFO_LISTCTRL_SUBJECT_FGCOLOR_S=   RGB(0xFF, 0xFF, 0xFF),
        FISH_VIEWINFO_LISTCTRL_BGCOLOR_S        =   RGB(0x51, 0x7D, 0xB2)/*RGB(0xC1, 0xCA, 0xE3)*/,
        FISH_VIEWINFO_LISTCTRL_FGCOLOR_S        =   RGB(0xFF, 0xFF, 0xFF),
        FISH_VIEWINFO_LISTCTRL_BGCOLOR_NF       =   RGB(0x9B, 0xB5, 0xD3),
        FISH_VIEWINFO_LISTCTRL_FGCOLOR_UNREAD   =   RGB(0x00, 0x00, 0x00),


        FISH_VIEWINFO_LISTCTRL_PROGRESS_TEXT_N    =   RGB(0x83, 0x83, 0x83),
        FISH_VIEWINFO_LISTCTRL_PROGRESS_TEXT_S    =   RGB(0xFF, 0xFF, 0xFF),
        FISH_VIEWINFO_LISTCTRL_TOP_AND_BOTTOM_PADDING      =   5,
        MAX_STRING       =   255,

        // 인덱스
        FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX    =   0,      // 제목
        FISH_VIEWINFO_LISTCTRL_ICON_INDEX       =   1,      // 아이콘
        FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX   =   2,      // 출처
        FISH_VIEWINFO_LISTCTRL_DATE_INDEX       =   3,      // 날짜
        FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX     =   4,      // 작성자
        FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX    =   5,      // 관심도
        FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX =   6,      // 관심도
        FISH_VIEWINFO_LISTCTRL_COLUMN_QUANTITY  =   7,      // 컬럼의 개수

        // 폰트 크기관련
        FISH_VIEWINFO_LISTCTRL_FONT_SIZE        =   19,
        FISH_VIEWINFO_HEADERCTRL_FONT_SIZE      =   12,
        FISH_VIEWINFO_LISTCTRL_HEADER_HEIGHT     =   16, //  18,
        FISH_VIEWINFO_LISTCTRL_HEADERUN_DERLINE_HEIGHT =   2, //  15,
        FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT       =   20, //  15,
    };   
// Construction
public:
	CFishListCtrl();
	virtual ~CFishListCtrl();
    void    _initialize();
    void    _finalize();

// Attributes
public:
    // TEST
    CRect   m_rectClient;
	CRect	m_rectScreenWnd;

    // SETTING VALUES
    CFont   m_fontHeaderHeightCoordinator;
    CFont   m_fontContentsHeightCoordinator;
    CFont   m_fontDefaultHeader;
    
    CFont   m_fontDefaultContents;
    CFont   m_fontDefaultContentsBold;
    CFont   m_fontDefaultContentsSelected;

    CFont   m_fontProgressBar;

	// Memory Rect
  
    //CFont   m_fontShowingDefault;
    int     m_sortCriterionColumn;
    bool    m_bAscendingSort;
    int     m_sortBy;
    CFishHeaderCtrl m_ctrlHeader;

    // BKGND REDRAW
    int     m_nVertPos;                                 // 스크롤바를 이용한 위치 찾기
    DWORD   m_dwPrevItem;

    CWnd*   m_pParent;

// Operations
public:
	void	SelectByData(DWORD dwData);
    int     GetItemIndexFromData(DWORD dwData);
    void    ToggleIconSort();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishListCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
    //int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	int FindItemByPosition(CPoint pt);

	int iFindItem;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFishListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHDNItemclickListRss(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHDNBegindragListRss(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHDNBegintrackListRss(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

public:
    void customdraw_DrawBkgnd(CDC* pDC, NMLVCUSTOMDRAW* pLVCD);
    void customdraw_DrawText(CDC* pDC, NMLVCUSTOMDRAW* pLVCD);
    void customdraw_DrawProgressBar(CDC* pDC, NMLVCUSTOMDRAW* pLVCD);
    void customdraw_DrawIcon(CDC* pDC, NMLVCUSTOMDRAW* pLVCD);

public:
#ifdef _DONT_USE_CUSTOMSCROLLBAR_
    void PositionScrollBars();
	void Init();
#endif

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISHLISTCTRL_H__6DFBBE09_CEE7_4AFD_99AE_00A751A51D1B__INCLUDED_)
