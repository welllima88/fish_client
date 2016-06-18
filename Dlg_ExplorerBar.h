#if !defined(AFX_DLG_EXPLORERBAR_H__A652E32B_2315_4541_9A19_EFFD90DC4ED3__INCLUDED_)
#define AFX_DLG_EXPLORERBAR_H__A652E32B_2315_4541_9A19_EFFD90DC4ED3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_ExplorerBar.h : header file
//

#include "GlobalIconManager.h"
#include "ctrl/NetButton.h"
#include "ctrl/HistoryCombo.h"
#include "ctrl/cjflatcombobox.h"
#include "view_browser/CFishComboBox.h"
#include <algorithm>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDlg_ExplorerBar dialog
class CView_Information;
class CDlg_ExplorerBar : public CDialog
{
public:
    enum {
        EXPLORERBAR_BTN_WIDTH       =   20,
        EXPLORERBAR_BTN_HEIGHT      =   18,
        FISH_COMBOBOX_FONT_SIZE     =   14,
        FISH_ADDRESSBAR_OUTLINE_COLOR   =   RGB(0xAC, 0xAC, 0xAC),
        FISH_EXPLORERDLG_OUTLINE_COLOR  =   RGB(0x43, 0x58, 0x75),
    };
private:
    CView_Information*  m_pParent;
	CFont   m_ftDefault;                    // 아이템의 폰트 그리기에 필요한 부분
    CList<CString, CString&>   m_listURL;

// Construction
public:
	CDlg_ExplorerBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_ExplorerBar();

// Attribute
protected:
	CString	m_sURL;
// Dialog Data
	//{{AFX_DATA(CDlg_ExplorerBar)
	enum { IDD = IDD_DLG_EXPLORERBAR };
	CNetBitmapButton	m_btnREFRESH;
	CNetBitmapButton	m_btnSTOP;
	CNetBitmapButton	m_btnFORWARD;
	CNetBitmapButton	m_btnBACK;
	CFishComboBox       m_cbGo;
	//}}AFX_DATA

// Operator
public:
	BOOL btnREFRESH_EnableWindow(BOOL bEnable = TRUE);
	BOOL btnSTOP_EnableWindow(BOOL bEnable = TRUE);
	BOOL btnFORWARD_EnableWindow(BOOL bEnable = TRUE);
	BOOL btnBACK_EnableWindow(BOOL bEnable = TRUE);
	BOOL Create(CWnd* pParentWnd = NULL);
	void FocusOnAddressCombo();
    void _initialize();
    void _finalize();
	void ExploreDest();
	void MoveControls();
    void AddURL();
	void LoadURL();
	void SaveURL();

    void UpdateState(CString url    =   _T(""));
	int  FindStringExact(int nIndexStart, LPCTSTR lpszFind);

// Operator
protected:
    BOOL UpdateData(BOOL bSaveAndValidate = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_ExplorerBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlg_ExplorerBar)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButtonBack();
	afx_msg void OnButtonForward();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonRefresh();
	afx_msg void OnPaint();
    afx_msg void OnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    //////////////////////////////////////////////////////////////////////////
    // UNIT TEST
    //////////////////////////////////////////////////////////////////////////
	BOOL	m_bInit;
	BOOL	m_bSelfDraw;

	// draw functions
	void    DrawItems(CDC *pDC);
	void    DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void    DrawOutline(CDC* pBufferDC);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_EXPLORERBAR_H__A652E32B_2315_4541_9A19_EFFD90DC4ED3__INCLUDED_)
