#if !defined(AFX_CUSTOMDLG_H__65CEF98D_AD53_4CC0_B44F_2384D4067746__INCLUDED_)
#define AFX_CUSTOMDLG_H__65CEF98D_AD53_4CC0_B44F_2384D4067746__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomDlg.h : header file
//
#include "./ctrl/netbutton.h"

//CustomDlg
#define CUSTOM_DLG_SYS_CLOSE_ID			7779
#define COLOR_CUSTOM_DLG_SEPT			COLORREF(RGB(214, 214, 216))
#define CUSTOM_DLG_BG_COLOR				COLORREF(RGB(248, 248, 248))
#define SUB_TITLE_IMAGE_HEIGHT			27
#define SUB_SUBTITLE_IMAGE_HEIGHT		48

//SubDialog
#define COLOR_SUB_DLG_TEXT				COLORREF(RGB(87, 87, 87))
#define COLOR_SUB_DLG_BOLD_TEXT			COLORREF(RGB(48, 48, 48))
#define SUB_DLG_FUNC_BTN_WIDTH			65
#define SUB_DLG_FUNC_BTN_HEIGHT			24
#define COLOR_SUB_DLG_CTRL_RECT			COLORREF(RGB(213, 212, 217))
#define COLOR_FOCUS_CAPTION				COLORREF(RGB(255, 255, 255))

/////////////////////////////////////////////////////////////////////////////
// CCustomDlg dialog

class CCustomDlg : public CDialog
{
// Construction
public:

	enum{		
		CAPTION_LEFT_MARGIN = 0,
		CAPTION_RIGHT_MARGIN = 10,
		CLOSE_BTN_TOP_MARGIN = 7,
		CLOSE_BTN_RIGHT_MARGIN = 8,
		CLOSE_BTN_WIDTH	= 14,
		CLOSE_BTN_HEIGHT = 14,
		FONT_HEIGHT = 14,
	};
	
	void SetSubTitleImage(UINT nIDResource);
	void DrawRect(CDC *pBufferDC, CRect rect, int nExcept = 0);
	void DrawItems2(CDC *pDC);
	void DrawOutline(CDC* pBufferDC, CDC *pMemDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void DrawItems(CDC *pDC);
	void SetDrawBySelf(BOOL bDraw);
	void DeleteObjects();
	void InitObjects();
	//CCustomDlg(CWnd* pParent = NULL);   // standard constructor
	CCustomDlg(UINT nIDTemplate, CWnd* pParent = NULL, CString title = _T(""));   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomDlg)
//	enum { IDD = IDD_CUSTOM_DLG };
//		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CNetBitmapButton	m_btnClose;

	CBitmap	m_bmpTitle;
	BITMAP	m_bitmapTitle;
	CBitmap	m_bmpLogo;
	BITMAP	m_bitmapLogo;

	CBitmap m_bmpTitleLeft;
	BITMAP	m_bitmapTitleLeft;
	CBitmap m_bmpTitleMid;
	BITMAP	m_bitmapTitleMid;
	CBitmap m_bmpTitleRight;
	BITMAP	m_bitmapTitleRight;


	CBitmap	m_bmpOutlineR;
	CBitmap	m_bmpOutlineL;
	BITMAP  m_bitmapOutLine;
	CBitmap	m_bmpBottom;	
	BITMAP	m_bitmapBottom;

	CBitmap	m_bmpSubTitleBg;
	BITMAP	m_bitmapSubTitleBg;
	CBitmap	m_bmpSubTitleR;
	BITMAP  m_bitmapSubTitleBgR;
	CBitmap	m_bmpSubTitle;
	BITMAP  m_bitmapSubTitle;
	UINT	m_nIDResource;

//	CBitmap	m_bmpSept;
//	BITMAP  m_bitmapSept;
	BOOL	m_bDrawBySelf;
	CRgn	m_rgnDlg;

	CRect	m_rectCaption;
	CRect	m_rectWindowText;

	CFont	m_fontBold;
	CPen	m_penCtrlRect;
	CPen	m_penSept;
	CString m_szCaption;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMDLG_H__65CEF98D_AD53_4CC0_B44F_2384D4067746__INCLUDED_)
