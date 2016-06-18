#if !defined(AFX_DLG_LOGIN_H__417B2041_DE72_4023_B1B2_8DC79FF1C4FF__INCLUDED_)
#define AFX_DLG_LOGIN_H__417B2041_DE72_4023_B1B2_8DC79FF1C4FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Login.h : header file
//

#include "./ctrl/netbutton.h"

//CustomDlg
#define LOGIN_DLG_CLOSE_ID				8000
#define LOGIN_DLG_LOGIN_ID				8001
#define LOGIN_DLG_BTN_REG_ID			8002
#define LOGIN_DLG_BTN_FIND_ID			8003

#define COLOR_LOGIN_DLG_OUTLINE_OUT		COLORREF(RGB(0, 29, 46))
#define COLOR_LOGIN_DLG_OUTLINE_IN		COLORREF(RGB(101, 146, 172))
#define COLOR_LOGIN_DLG_BG				COLORREF(RGB(6, 89, 137))
#define COLOR_LOGIN_DLG_CONTENTS_BG		COLORREF(RGB(248, 248, 248))

#define COLOR_CUSTOM_DLG_SEPT			COLORREF(RGB(214, 214, 216))
#define SUB_TITLE_IMAGE_HEIGHT			27
#define SUB_SUBTITLE_IMAGE_HEIGHT		48

//SubDialog
#define COLOR_SUB_DLG_TEXT				COLORREF(RGB(87, 87, 87))
#define COLOR_SUB_DLG_BOLD_TEXT			COLORREF(RGB(48, 48, 48))
#define SUB_DLG_FUNC_BTN_WIDTH			65
#define SUB_DLG_FUNC_BTN_HEIGHT			24
#define COLOR_SUB_DLG_CTRL_RECT			COLORREF(RGB(213, 212, 217))

/////////////////////////////////////////////////////////////////////////////
// CDlg_Login dialog

class CDlg_Login : public CDialog
{
// Construction
public:
	CString GetLoginErrString(int err);
	BOOL Create(CWnd* pParentWnd);
	void DrawRect(CDC *pBufferDC, CRect rect, int nExcept = 0);
	void DrawItems2(CDC *pDC);
	void DrawOutline(CDC* pBufferDC, CDC *pMemDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawItems(CDC *pDC);
	void SetDrawBySelf(BOOL bDraw);
	void DeleteObjects();
	void InitObjects();
	CDlg_Login(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_Login)
	enum { IDD = IDD_DLG_LOGIN };
		// NOTE: the ClassWizard will add data members here
	CEdit		m_editPasswd;
	CEdit		m_editId;
	CButton		m_checkPasswdSave;
	CButton		m_checkAutoLogin;
	CString		m_sID;
	CString		m_sPasswd;
	BOOL		m_bAutoLogin;
	BOOL		m_bSavePassword;
	//}}AFX_DATA

	CNetBitmapButton	m_btnClose;
	CNetBitmapButton	m_btnLogin;
	CNetBitmapButton	m_btnReg;
	CNetBitmapButton	m_btnIdPasswdFind;

	CBitmap		m_bmpTitle;
	BITMAP		m_bitmapTitle;

	CBitmap		m_bmpBgTop;
	BITMAP		m_bitmapBgTop;
	CBitmap		m_bmpBgMid;
	BITMAP		m_bitmapBgMid;
	CBitmap		m_bmpBgBottom;
	BITMAP		m_bitmapBgBottom;

	CBitmap		m_bmpBgContents;
	BITMAP		m_bitmapBgContents;


	CBitmap		m_bmpHelp;
	BITMAP		m_bitmapHelp;
	
	CBitmap		m_bmpIdText;
	CBitmap		m_bmpPasswdText;
	BITMAP		m_bitmapIdText;

	CBitmap		m_bmpAutoLoginText;
	BITMAP		m_bitmapAutoLoginText;
	CBitmap		m_bmpPasswdSaveText;
	BITMAP		m_bitmapPasswdSaveText;

	//CBitmap		m_bmpBox;
	//BITMAP		m_bitmapBox;

	BOOL		m_bDrawBySelf;
	CRgn		m_rgnDlg;

	CRect		m_rectCaption;

	CFont		m_fontBold;
	CFont		m_fontNormal;
	CFont		m_fontUnderline;

	CPen		m_penDlgOutline_out;
	CPen		m_penDlgOutline_in;
	CPen		m_penDotLine;

	//CRect		m_rectReg;
	//CRect		m_rectFind;

	HWND		m_hMsgHwnd;
//	BOOL		m_bNoNeedPass;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Login)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CDlg_Login)
		// NOTE: the ClassWizard will add member functions here		
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnCancel();
	afx_msg void OnLogin();
	afx_msg void OnReg();
	afx_msg void OnFind();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 

	afx_msg void OnLoginResult(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_LOGIN_H__417B2041_DE72_4023_B1B2_8DC79FF1C4FF__INCLUDED_)
