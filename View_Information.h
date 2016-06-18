#if !defined(AFX_VIEW_INFORMATION_H__8552B21F_E8F1_4B73_A482_015B692B42B8__INCLUDED_)
#define AFX_VIEW_INFORMATION_H__8552B21F_E8F1_4B73_A482_015B692B42B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// View_Information.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CView_Information form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <map>
#include <cassert>
using namespace std;

#include "ctrl/NetButton.h"
#include "ctrl/MemDC.h"
#include "view_tabctrl/FishButton.h"
#include "view_tabctrl/CustomTabCtrl.h"
#include "view_navigationpane/View_NavigationPane.h"
#include "Dlg_Browser.h"

class CDlg_ExplorerBar;
class CDlg_RSSMain;

class CView_Information : public CFormView
{
public:
    enum {  // hiding magic number
        VIEW_DLG_TOP_PADDING            =   7,
        VIEW_DLG_LEFT_PADDING           =   2,
        VIEW_DLG_RIGHT_PADDING          =   8,
        VIEW_DLG_BOTTOM_PADDING         =   20,

        FISH_TAB_UPPER_PADDING          =   4,

        FISH_MAINLIST_BTN_TOP_PADDING   =   VIEW_DLG_TOP_PADDING + FISH_TAB_UPPER_PADDING,
        FISH_MAINLIST_BTN_LEFT_PADDING  =   5,
        FISH_MAINLIST_BTN_HEIGHT        =   21,
        FISH_MAINLIST_BTN_WIDTH         =   87,

        FISH_TAB_TOP_PADDING            =   VIEW_DLG_TOP_PADDING + FISH_TAB_UPPER_PADDING,
        FISH_TAB_LEFT_PADDING           =   VIEW_DLG_LEFT_PADDING + FISH_MAINLIST_BTN_WIDTH + FISH_MAINLIST_BTN_LEFT_PADDING - 1,
        FISH_TAB_RIGHT_PADDING          =   VIEW_DLG_RIGHT_PADDING,
        FISH_TAB_BTN_HEIGHT             =   FISH_MAINLIST_BTN_HEIGHT,

        FISH_EXPLORERBAR_TOP_PADDING    =   VIEW_DLG_TOP_PADDING + FISH_TAB_UPPER_PADDING + FISH_TAB_BTN_HEIGHT,
        FISH_EXPLORERBAR_HEIGHT         =   33,

        FISH_BROWSER_TOP_PADDING        =   FISH_EXPLORERBAR_TOP_PADDING + FISH_EXPLORERBAR_HEIGHT,
        
        VIEW_BKGND_COLOR                =   RGB(0xF8, 0xF8, 0xF8),
        VIEW_TOP_BKGND_COLOR            =   RGB(0x76, 0x8A, 0xAE),

        FISH_BROWSER_OUTLINE_COLOR      =   RGB(0x43, 0x58, 0x75),
    };

protected:
	DECLARE_DYNCREATE(CView_Information)
	CView_Information();           // protected constructor used by dynamic creation

// Form Data
public:
	virtual ~CView_Information();

	//{{AFX_DATA(CView_Information)
	enum { IDD = IDD_VIEW_INFORMATION };
	CNetBitmapButton   m_btnGotoMainList;
	//}}AFX_DATA

// Attributes
public:
    
    CFishTabCtrl                        m_ctrlCustomTab;
	SmartPtr<CDlg_ExplorerBar>		    m_spExplorerBar;
	SmartPtr<CDlg_RSSMain>			    m_spRSSMain;
    CView_NavigationPane                m_dlgNavigationPane;

protected:
    static DWORD                        m_dwBrowserSeq;

	CRgn								m_rgnPaint;
	CBrush								m_brushBG;
	CBrush								m_brushTabBG;

	BOOL				                m_bInit;
	BOOL				                m_bSelfDraw;
    map<DWORD, SmartPtr<CDlg_Browser> >       m_mapSPBrowsers;

	// for test

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CView_Information)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


	HICON m_hIcon;
    // Generated message map functions
	//{{AFX_MSG(CView_Information)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInformationGotoMainlistBtn();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg void OnTabLButtonClick(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTabRButtonClick(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTabSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabHighlightchange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTabMoveitem(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTabCopyitem(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTabLabelupdate(NMHDR* pNMHDR, LRESULT* pResult) ;
    afx_msg void OnTabCloseRegionClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRequestPostDblclk(LPARAM lParam, WPARAM wParam);
    DECLARE_MESSAGE_MAP()

// Operations
public:
	void GotoMainlist(BOOL bForce = FALSE);
	BOOL SetFocusOnAddrbar();
    void ReadOn(int postid);
	BOOL Create(CWnd* pParentWnd);
	void MoveControls();

public:
    //////////////////////////////////////////////////////////////////////////
    // BROWSER INTERFACE
    void*   GetSelectedBrowser();
	void*   GetCurrentExplorerBrowser();
    void*   AddExplorerBrower();
    void    RemoveExplorerBrowser(unsigned int seqNUM);
    void*   GetBrowserFrom(UINT seqID);
    UINT    GetSize()   { return m_mapSPBrowsers.size(); }
    void    Clear()     { m_mapSPBrowsers.clear();  }

    void*   NewExplorerBrowser() { return AddExplorerBrower(); }        // OLD INTERFACE


// Implementation
protected:
	// draw functions
	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawOutline(CDC* pBufferDC);
	// --

private:
    //////////////////////////////////////////////////////////////////////////
    // TEST CASE METHOD
    void _testTabControlArticleAdd();
    void _testTabControlAttribute();
    void _testTabControlFontChange();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_INFORMATION_H__8552B21F_E8F1_4B73_A482_015B692B42B8__INCLUDED_)
