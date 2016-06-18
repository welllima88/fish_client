#if !defined(AFX_VIEW_INFOFUNCTIONPANE_H__C0B1D5D9_6F9C_4F85_A894_B834C1F24D88__INCLUDED_)
#define AFX_VIEW_INFOFUNCTIONPANE_H__C0B1D5D9_6F9C_4F85_A894_B834C1F24D88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// View_InfoFunctionPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CView_NavigationPane dialog

#include "../ctrl/NetButton.h"
#include "../ctrl/MemDC.h"

class CView_NavigationPane : public CDialog
{
public:
    enum {
        FUNCTION_BTN_TOP_PAD    =   3,
        FUNCTION_BTN_INTERVAL   =   2,

        NEWTAB_BTN_WIDTH    =   47,
        NEWTAB_BTN_HEIGHT   =   15,

        SAVE_BTN_WIDTH      =   47,
        SAVE_BTN_HEIGHT     =   15,

        STYLE_BTN_WIDTH     =   47,
        STYLE_BTN_HEIGHT    =   15,

        SUBSCRIBE_BTN_WIDTH =   47,
        SUBSCRIBE_BTN_HEIGHT=   15,

        ADDRMSG_BTN_HEIGHT  =   15,			// modified by moonknit 2006-02-28 : 11 -> 13

        NAV_BTN_WIDTH    =   17,
        NAV_BTN_HEIGHT   =   14,

        NAV_CENTER_WIDTH    =   68,
        NAV_CENTER_HEIGHT   =   14,

        INTERVAL_BETWEEN_POSTBTN_N_NAVBTN   =   5,
        INTERVAL_BETWEEN_NAVBTNS    =   2,

        VIEW_BKGND_COLOR                =   RGB(0xF8, 0xF8, 0xF8),

        FISH_INFOPANE_DEFAULT_FONT_SIZE =   14,
    };
// Construction
public:
	CView_NavigationPane(CWnd* pParent = NULL);   // standard constructor
    ~CView_NavigationPane();

// Dialog Data
	//{{AFX_DATA(CView_NavigationPane)
	enum { IDD = IDD_VIEW_NAVIGATIONPANE };
	CStatic	m_ctrlAddressMsg;
	CNetBitmapButton	m_btnPost;
	CNetBitmapButton	m_btnLAST;
	CNetBitmapButton	m_btnDBLNEXT;
	CNetBitmapButton	m_btnNEXT;
	CNetBitmapButton	m_btnBACK;
	CNetBitmapButton	m_btnDBLBACK;
	CNetBitmapButton	m_btnFIRST;
	CNetBitmapButton	m_btnSubscribe;
	CNetBitmapButton	m_btnStyle;
	CNetBitmapButton	m_btnSave;
	CNetBitmapButton	m_btnNewTab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CView_NavigationPane)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CView_NavigationPane)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBtnViewFirst();
	afx_msg void OnBtnViewLast();
	afx_msg void OnBtnViewNext();
	afx_msg void OnBtnViewPrev();
	afx_msg void OnBtnViewMultiNext();
	afx_msg void OnBtnViewMultiPrev();
	afx_msg void OnBtnNewtab();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnStyle();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBtnSubscribe();
	//}}AFX_MSG
    afx_msg void OnUpdatePostQuantity(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInformCurrentPosts(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
    HWND    m_hSubscribe;
	BOOL    m_bSelfDraw;
    BOOL    m_bBrowsing;
    CFont   m_ftDefault;
	CFont   m_ftStatusFont;
    UINT    m_nPostQuantity;
    HBRUSH  m_hStaticBkgnd;
//    BOOL    m_bDrawStatus;

private:
    void _initialize();
    void _finalize();
    void DrawBg(CDC *pBufferDC);
    void DrawPostQuantity(CDC* pDC);

public:
    __inline SetSubscribeHandle(HWND h) { m_hSubscribe  =   h; }
    void MoveControls();
    void SetBrowseFlag(BOOL flag    =   FALSE)  
    {   
        m_bBrowsing =   flag;
        MoveControls();
    }

public:
	CString UpdateStatusBar(CString szStatus);
	void AlterationToArticleMode();
	void AlterationToBrowseMode();
    void _text_UpdatePostQuantity();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_INFOFUNCTIONPANE_H__C0B1D5D9_6F9C_4F85_A894_B834C1F24D88__INCLUDED_)

