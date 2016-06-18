#if !defined(AFX_DLG_RSSMAIN_H__6289C3A7_0541_4D71_A036_A01572769739__INCLUDED_)
#define AFX_DLG_RSSMAIN_H__6289C3A7_0541_4D71_A036_A01572769739__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_RSSMain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSMain dialog
#include "./ctrl/SSplitter.h"
#include "Dlg_RSSList.h"
class CDlg_Browser;

class CDlg_RSSMain : public CDialog
{
// Construction
public:
	void SetSubscribeHwnd(HWND h);
	void ViewPosts(CString& html);
	void* GetBrowserPointer();
	virtual ~CDlg_RSSMain();
	CDlg_RSSMain(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd* pParentWnd = NULL);
	void MoveControls();

// Dialog Data
	//{{AFX_DATA(CDlg_RSSMain)
	enum { IDD = IDD_DLG_RSSMAIN };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_RSSMain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// draw functions
	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawOutline(CDC* pBufferDC);
	// --

	BOOL				m_bFirstView;
	BOOL				m_bInit;
	BOOL				m_bViewListRequested;
	UINT				m_nViewTimer;
	CSSplitter			m_splitter;
	BOOL				m_bSelfDraw;
#ifdef USE_WBTMPFILE
	int					m_nCachePageSeq;
	int					m_nCacheFileSeq;
#endif

public:
	SmartPtr<CDlg_RSSList>		m_spRSSList;
	SmartPtr<CDlg_Browser>		m_spBrowser;

	// Generated message map functions
	//{{AFX_MSG(CDlg_RSSMain)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnViewCurrentList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewFirstPost(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewLastPost(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewNextPosts(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewPrevPosts(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewHTML(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewRefresh(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_RSSMAIN_H__6289C3A7_0541_4D71_A036_A01572769739__INCLUDED_)
