// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__D7BF8FE0_6221_4C41_BC75_0B8CA272B1B9__INCLUDED_)
#define AFX_MAINFRM_H__D7BF8FE0_6221_4C41_BC75_0B8CA272B1B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>

#include "FISH_DEF.H"
#include "./ctrl/SSplitter.h"
#include "./ctrl/netbutton.h"				// image button
#include "taskbarnotifier.h"
#include "Tray.h"

using namespace std;

class CView_Information;
class CDlg_Subscribe;
class CDlg_RSSMain;
class CDlg_Search;
class CDlg_Login;
class CIrcdManager;
//class CTaskbarNotifier;

class CMainFrame : public CFrameWnd
{
public:
    enum {
        MAINFRAME_SPLITTER_LEFT_LIMIT   =   194,
        MAINFRAME_SPLITTER_RIGHT_LIMIT  =   348,    // 왼쪽으로 부터 몇 픽셀인지를 나타낸다.
    };

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

protected:
    vector<CString>             m_vecChannelLink;
	SmartPtr<CDlg_Subscribe>	m_spSubscribe;
	SmartPtr<CDlg_Search>		m_spSearch;
	SmartPtr<CTray>				m_TrayIcon;
	SmartPtr<CTaskbarNotifier>	m_spNotifier;
	SmartPtr<CIrcdManager>		m_spIrcd;
	CView_Information*			m_pVI;
	CSSplitter					m_splitter;
	CMenu*						m_pTmpMenu;

	SmartPtr<CDlg_Login>		m_spLogin;
	HWND						m_hLoginWnd;

	BOOL						m_bInit;
	BOOL						m_bSetDragFullWindows;
	BOOL						m_bSelfDraw;
	BOOL						m_bHide;

	BOOL						m_bMaxmize; // TRUE : Maximize 상태 , FALSE : Maxmize 상태 아님..Normal 상태 

	BOOL						m_bPostInit;

	int							m_nMinCX;
	int							m_nMinCY;

	int							m_nFlushTime;

	int							m_nGnuOpenTimer;
	int							m_nMainTimer;

	CRect						m_rccaption;

	CRgn						m_rgnResize;
	CRect						m_rcResizeLeft, m_rcResizeRight;
	CRect						m_rcResizeBottom, m_rcResizeTop;
	CRect						m_rcResizeTopLeft, m_rcResizeTopRight;
	CRect						m_rcResizeBottomLeft, m_rcResizeBottomRight;

	CRect						m_rcBrowser;
	HWND						m_hBrowserWnd;

	CRect						m_rectBeforeWnd;
	CRect						m_rcScreenWnd;

	CNetBitmapButton			m_btnClose;
	CNetBitmapButton			m_btnMaximize;
	CNetBitmapButton			m_btnMinimize;

	UINT						m_uBorderMargin;

	CRgn						m_rgnClip;

    BOOL                        m_bInTimer;
    DWORD                       m_dwScrapPostID;

	CRect						m_rcLogo;
    // Operations
public:
	void SetBrowserMoveInfo(CRect rc, HWND h)	{ m_rcBrowser = rc; m_hBrowserWnd = h; }
	void RequestExplorerURL(CString url);
protected:
	BOOL CreateMouseHook();
	void CloseFrame();
	void IrcdAuth();
	BOOL WebAuth(int& err);
	void InitUser();
	void MoveControls();

	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawOutline(CDC* pBufferDC);

	void SetWindowMinMax(int nMinCX = 0, int nMinCY = 0);
	void SetResizeRgn(CRgn *prgn);
	void MakeNCRgn();
	void SetRoundedRgn();

	void ProcessArguments(const LPCTSTR args);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ChangeToListView(int chid);
	CView* GetFirstView();
	void TryLogin();
	void MoveButtons();
	void FrameHide(BOOL bhide = TRUE);
	LPDISPATCH GetNewWBIDispatch();
	BOOL NavigateNewWindow(LPCTSTR lpszurl);
	void ReadOn(int postid);
	void Scrap(list<int>& idlist);
	void Scrap(int postid);
	void Scrap(int channelid, int postid);
	int GetScrapChannel();
	BOOL GetDlgRSSMain(SmartPtr<CDlg_RSSMain>& spmain);
	BOOL GetDlgSubscribe(SmartPtr<CDlg_Subscribe>& spsubscribe);
	BOOL GetTaskbarNotifier(SmartPtr<CTaskbarNotifier>& spnotifier);
	BOOL GetTrayIcon(SmartPtr<CTray>& sptrayicon);
	void StopNetwork();
	void StartNetwork();
    
    static DWORD WINAPI ParameterSubscriptionThread(LPVOID lpparam);

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnProperties();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCreateNewScrapMgr();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
    afx_msg void OnCommandRange(UINT nID);
	afx_msg void OnBtnClose();
	afx_msg void OnBtnMaximize();
	afx_msg void OnBtnMinimize();
	afx_msg void OnInitView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNavigateURL(WPARAM wPram, LPARAM lParam);
	afx_msg void OnPostInitAll(WPARAM wPram, LPARAM lParam);
	afx_msg void OnRSSDBResult(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNodeUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTrayShow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRequestLogin(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLoginComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnIrcdAuthComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAreYou(WPARAM, LPARAM);
	afx_msg void OnTrayOpen();
	afx_msg void OnTrayExit();
	afx_msg void OnScrap(WPARAM wParam, LPARAM lParam);
	afx_msg void OnReadOn(WPARAM wParam, LPARAM lParam);
	afx_msg void OnReadPost(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUserreqGotoAddrbar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetNewBrowserPtr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNavigateNew(WPARAM wParam, LPARAM lParam);
    afx_msg void OnFocusOnAddrCombo();
	afx_msg void OnEndSession(BOOL bEnding);
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

extern HWND GetOverHwnd();
extern void SetOverHwnd(HWND hWnd);
extern HWND GetBrowserHwnd();
extern void SetBrowserHwnd(HWND hWnd);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D7BF8FE0_6221_4C41_BC75_0B8CA272B1B9__INCLUDED_)
