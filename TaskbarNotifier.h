// CTaskbarNotifier Header file
// By John O'Byrne - 15 July 2002

#pragma once

#include <ShellApi.h>

#define WM_TASKBARNOTIFIERCLICKED	WM_USER+123
#define TN_TEXT_NORMAL			0x0000
#define TN_TEXT_BOLD			0x0001
#define TN_TEXT_ITALIC			0x0002
#define TN_TEXT_UNDERLINE		0x0004


/*
enum EZ_NOTIFY_TYPE
{
	EZ_NT_NOTE = 1, // 쪽지
	EZ_NT_FILE = 2 // 파일 다운로드..
};
*/

// CTaskbarNotifier
//============== USAGE =====================
/* 
	Step 1. Include Header file
	#include "./ctrl/TaskbarNotifier.h"

	Step 2. Declear member variable..
	CTaskbarNotifier	m_ezNotifier;

	Step 3. Initialize control 
	m_ezNotifier.Create(this);
	m_ezNotifier.SetBitmap(IDB_ENPPYBOX2, 0, 255, 0);  //bmp, transparent color.
	m_ezNotifier.SetTextFont(STR_DEFAULT_FONT,95,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE | TN_TEXT_BOLD );
	m_ezNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,0));
	m_ezNotifier.SetTextRect(CRect(5,5,m_ezNotifier.m_nBitmapWidth-10,m_ezNotifier.m_nBitmapHeight-10));

	Step 4. Show Message..
	m_ezNotifier.Show(msg); // msg->string..

*/
#include "./ctrl/NetButton.h"

class CTaskbarNotifier : public CWnd
{
	DECLARE_DYNAMIC(CTaskbarNotifier)

public:
	CTaskbarNotifier();
	virtual ~CTaskbarNotifier();

	int Create(CWnd *pWndParent);
	void Show(LPCTSTR szCaption,DWORD dwTimeToShow=500,DWORD dwTimeToStay=3000,DWORD dwTimeToHide=200);
	void Hide();
	BOOL SetBitmap(UINT nBitmapID,short red=-1,short green=-1,short blue=-1);
	BOOL SetBitmap(LPCTSTR szFileName,short red=-1,short green=-1,short blue=-1);
	void SetTextFont(LPCTSTR szFont,int nSize,int nNormalStyle,int nSelectedStyle);
	void SetTextDefaultFont();
	void SetTextColor(COLORREF crNormalTextColor,COLORREF crSelectedTextColor);
	void SetTextRect(RECT rcText);
	void SetTextFormat(UINT uTextFormat);

	enum
	{
		BTN_HEIGHT = 14,
		BTN_WIDTH = 14,
		BG_WIDTH = 172,
		BG_HEIGHT = 120,
		CLOSE_MG_RIGHT = 8,
		CLOSE_MG_TOP = 8,
	};
	
	CWnd *m_pWndParent;
	
	CFont m_myNormalFont;
	CFont m_mySelectedFont;
	COLORREF m_crNormalTextColor;
	COLORREF m_crSelectedTextColor;
	HCURSOR m_hCursor;
	
	CBitmap m_bitmapBackground;
	HRGN m_hBitmapRegion;
	int m_nBitmapWidth;
	int m_nBitmapHeight;

	CString m_strCaption;
	CRect m_rcText;
	UINT m_uTextFormat;
	BOOL m_bMouseIsOver;

	CNetBitmapButton	m_btnClose;
	

	int m_nAnimStatus;
	int m_nTaskbarPlacement;
	DWORD m_dwTimerPrecision;
 	DWORD m_dwTimeToStay;
	DWORD m_dwShowEvents;
	DWORD m_dwHideEvents;
	int m_nCurrentPosX;
	int m_nCurrentPosY;
	int m_nCurrentWidth;
	int m_nCurrentHeight;
	int m_nIncrementShow;
	int m_nIncrementHide;
	
	// msg type by bemlove
	DWORD m_nType;
	__inline void SetType( DWORD nType )	{ m_nType = nType; }
	__inline DWORD GetType(void)			{ return m_nType; }
	

	CBitmap m_bmpCloseIcon;
	CRect	m_rcClose;
	bool	m_bCursorHand;
protected:
	HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
//	afx_msg void OnBtnClose();
	void	ParseMessage( void );

};


