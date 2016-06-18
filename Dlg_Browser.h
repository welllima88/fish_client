//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLG_BROWSER_H__CA761EBB_5A21_4EA9_B556_0599FC86A0AC__INCLUDED_)
#define AFX_DLG_BROWSER_H__CA761EBB_5A21_4EA9_B556_0599FC86A0AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Browser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Browser dialog
class CFISHIEControl;
class CView_Information;

class CDlg_Browser : public CDialog
{
public:
    enum {
        MAX_NUM =   -1,
    };
// Construction
public:
	CDlg_Browser(CWnd* pParent = NULL, DWORD dwSeq  =   MAX_NUM);   // standard constructor
    CView_Information* m_parent;
	virtual ~CDlg_Browser();

// Dialog Data
	//{{AFX_DATA(CDlg_Browser)
	enum { IDD = IDD_DLG_BROWSER };
	CWebBrowser2	m_Browser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Browser)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// draw functions
	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawOutline(CDC* pBufferDC);
	// --


	void OnOK();
	void OnCancel();

	BOOL			m_bInit;
	BOOL			m_bSelfDraw;
	int				m_nMouseTimer;
	int				m_nHtmlTimer;
    DWORD           m_dwBrowserSeq;
	CString			m_strHTML;
    CString         m_strURL;

    VARIANT_BOOL    m_vbBackAvailable;
    VARIANT_BOOL    m_vbForwardAvailable;

	BOOL			m_bClose;
	BOOL			m_bLink2NewWindow;
	BOOL			m_bTempBypass;
	BOOL			m_bLinkClicked;
	BOOL			m_bPopup;

	int				m_nProgress, m_nProgressMax;

	HWND			m_hMainWnd;
	HWND			m_hPrevFocus;
	HWND			m_hInformWnd;

	int				m_nLinkTimer;

	CRect			m_rcMain;
public:
	void SetInformWnd(HWND h)			{ m_hInformWnd = h; }
    //////////////////////////////////////////////////////////////////////////
    // INTERFACE FOR STATUS
    VARIANT_BOOL isBackEnable() { if(m_bLink2NewWindow) return FALSE; return m_vbBackAvailable;  }
    VARIANT_BOOL isForwardEnable()  { if(m_bLink2NewWindow) return FALSE; return m_vbForwardAvailable; }

public:
	HWND GetBHwnd();
	CString GetSelectedLink();
	void SetBrowserFocus();
	void ScrollBrowser(short nDelta);
	void SetDeactivate(BOOL bDeactive = TRUE);
	void SetTempBypass(BOOL bpass);
	void SetLink2NewWindow(BOOL bnew);
    const CString   GetURL()    const;
	void WBRefresh();
	void WBGoBack();
	void WBGoForward();
	void WBStop();
	BOOL WaitUntilInit();
	void LoadHTMLDoc();
	void LoadHTML(CString html);
	LPDISPATCH GetWBIDispatch();
	void Navigate(CString url);
	BOOL Create(CWnd* pParentWnd = NULL, DWORD dwSeq  =   MAX_NUM);

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlg_Browser)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnCommandStateChangeExplorer(long Command, BOOL Enable);
	afx_msg void OnNewWindow2Explorer(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel);
	afx_msg void OnDocumentCompleteExplorer(LPDISPATCH pDisp, VARIANT FAR* URL);
	afx_msg void OnDownloadBeginExplorer();
	afx_msg void OnDownloadCompleteExplorer();
	afx_msg void OnTitleChangeExplorer(LPCTSTR Text);
	afx_msg void OnNavigateComplete2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL);
	afx_msg void OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnProgressChangeExplorer(long Progress, long ProgressMax);
	afx_msg void OnStatusTextChangeExplorer(LPCTSTR Text);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnWindowSetResizableExplorer(BOOL Resizable);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnCommandStateChange(WPARAM wParam, LPARAM lParam);
    afx_msg void OnStopExchange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnURLChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTitleChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_BROWSER_H__CA761EBB_5A21_4EA9_B556_0599FC86A0AC__INCLUDED_)
