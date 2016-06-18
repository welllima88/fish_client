#if !defined( __NETBUTTON_H )
#define __NETBUTTON_H

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.09.19	**************************************** //
// *******			**************************************** //
// ********************************************************************* //

// CNetButtonh : header file
//

// Comment this if you don't want that CNetButton hilights itself
// also when the window is inactive (like happens in Internet Explorer)
#define ST_LIKEIE

// Comment this if you don't want to use CMemDC class
#define ST_USE_MEMDC

/////////////////////////////////////////////////////////////////////////////
// CNetButton window

#include "NetDib.h"
#include "NetBitmap.h"

class CNetDibButton : public CButton
{
// Construction
public:
    CNetDibButton();
	~CNetDibButton();
    enum {ST_ALIGN_HORIZ, ST_ALIGN_VERT};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNetDibButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetBtnCursor(int nCursorId = -1);

	void SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint = FALSE);
	BOOL GetFlatFocus();

	void SetDefaultActiveFgColor(BOOL bRepaint = FALSE);
	void SetActiveFgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetActiveFgColor();
	
	void SetDefaultActiveBgColor(BOOL bRepaint = FALSE);
	void SetActiveBgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetActiveBgColor();
	
	void SetDefaultInactiveFgColor(BOOL bRepaint = FALSE);
	void SetInactiveFgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetInactiveFgColor();

	void SetDefaultInactiveBgColor(BOOL bRepaint = FALSE);
	void SetInactiveBgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetInactiveBgColor();

	void SetDefaultTransParentColor(BOOL bRepaint = FALSE);
	void SetTransParentColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetTransParentColor();

	void SetShowText(BOOL bShow = TRUE);
	BOOL GetShowText();

	void SetAlign(int nAlign);
	int GetAlign();

	void SetFlat(BOOL bState = TRUE);
	BOOL GetFlat();

	void SetThin(BOOL bThin = TRUE);
	BOOL GetThin();

	void DrawBorder(BOOL bEnable = TRUE);
	BOOL LoadBitmaps(LPTSTR lpszBitmap, LPTSTR lpszBitmapDown = NULL, LPTSTR lpszBitmapFocus = NULL, LPTSTR lpszBitmapDisabled = NULL, BYTE cx = 32, BYTE cy = 32);
	BOOL SetEnableBitmaps(LPTSTR lpszBitmap);
	BOOL SetDownBitmaps(LPTSTR lpszBitmap);
	BOOL SetFocussedBitmaps(LPTSTR lpszBitmap);
	BOOL SetDisabledBitmaps(LPTSTR lpszBitmap);
	static const short GetVersionI();
	static const char* GetVersionC();

protected:
    //{{AFX_MSG(CNetDibButton)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void DrawTheIcon(CDC* pDC, CString* title, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL bIsFocused, BOOL IsDisabled);
	int m_nAlign;
	BOOL m_bShowText;
	BOOL m_bDrawBorder;
	BOOL m_bIsFlat;
	BOOL m_MouseOnButton;
	BOOL m_bDrawFlatFocus;
	BOOL m_bThin;

	HCURSOR m_hCursor;

	CNetDib   *m_bmpButton;
	CNetDib   *m_bmpButtonDown;
	CNetDib   *m_bmpButtonFocussed;
	CNetDib   *m_bmpButtonDisabled;
	BYTE m_cyIcon;
	BYTE m_cxIcon;

    	COLORREF  m_crInactiveBg;
    	COLORREF  m_crInactiveFg;
    	COLORREF  m_crActiveBg;
    	COLORREF  m_crActiveFg;
    	COLORREF  m_crTransParent;
};


class CNetBitmapButton : public CButton
{
    enum {
        FISH_BUTTON_BG_COLORREF = RGB(0xEC, 0xEC, 0xEC),
    };
    BOOL m_bFreezeDrawDownBitmap;

// Construction
public:
    CNetBitmapButton();
	~CNetBitmapButton();
    enum {ST_ALIGN_HORIZ, ST_ALIGN_VERT};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNetBitmapButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectButton();
	BOOL IsSelected()	{ return m_ButtonSelected; }
	void KillFocus();

	BOOL SetBtnCursor(int nCursorId = -1);

	void SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint = FALSE);
	BOOL GetFlatFocus();

	void SetDefaultActiveFgColor(BOOL bRepaint = FALSE);
	void SetActiveFgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetActiveFgColor();
	
	void SetDefaultActiveBgColor(BOOL bRepaint = FALSE);
	void SetActiveBgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetActiveBgColor();
	
	void SetDefaultInactiveFgColor(BOOL bRepaint = FALSE);
	void SetInactiveFgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetInactiveFgColor();

	void SetDefaultInactiveBgColor(BOOL bRepaint = FALSE);
	void SetInactiveBgColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetInactiveBgColor();

	void SetDefaultTransParentColor(BOOL bRepaint = FALSE);
	void SetTransParentColor(COLORREF crNew = RGB(0x00, 0x00, 0x00), BOOL bRepaint = FALSE);
	const COLORREF GetTransParentColor();

	void SetShowText(BOOL bShow = TRUE);
	BOOL GetShowText();

	void SetAlign(int nAlign);
	int GetAlign();

	void SetFlat(BOOL bState = TRUE);
	BOOL GetFlat();

	void SetThin(BOOL bThin = TRUE);
	BOOL GetThin();

	void DrawBorder(BOOL bEnable = TRUE);
	
	void SetDefaulTextFont();
	BOOL SetTextFont(LPCTSTR lpFontName);

	BOOL LoadBitmaps(UINT nBitmap, UINT nBitmapDown = NULL, UINT nBitmapFocus = NULL, UINT nBitmapDisabled = NULL, BYTE cx = 32, BYTE cy = 32);
	BOOL LoadBitmaps(LPTSTR lpszBitmap, LPTSTR lpszBitmapDown = NULL, LPTSTR lpszBitmapFocus = NULL, LPTSTR lpszBitmapDisabled = NULL, BYTE cx = 32, BYTE cy = 32);

	BOOL SetEnableBitmap(UINT nBitmap);
	BOOL SetEnableBitmap(LPTSTR lpszBitmap);
    BOOL SetEnableBitmap(CBitmap* BMP);

	BOOL SetDownBitmap(UINT nBitmap);
	BOOL SetDownBitmap(LPTSTR lpszBitmap);
    BOOL SetDownBitmap(CBitmap* BMP);

	BOOL SetFocussedBitmap(UINT nBitmap);
	BOOL SetFocussedBitmap(LPTSTR lpszBitmap);
    BOOL SetFocussedBitmap(CBitmap* BMP);

	BOOL SetDisabledBitmap(UINT nBitmap);
	BOOL SetDisabledBitmap(LPTSTR lpszBitmap);
    BOOL SetDisabledBitmap(CBitmap* BMP);

    VOID FreezeBtnDownStateDraw(BOOL nFreeze  =   FALSE)    { m_bFreezeDrawDownBitmap = nFreeze; }
    VOID SetTextBkMode(int mode =   TRANSPARENT)    { m_nBkMode =   mode;}

    void SetSize(BYTE cx, BYTE cy) { m_cxIcon   =   cx;     m_cyIcon    =   cy;}

	static const short GetVersionI();
	static const char* GetVersionC();

protected:
    //{{AFX_MSG(CNetBitmapButton)
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void DrawTheIcon(CDC* pDC, CString* title, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL bIsFocused, BOOL IsDisabled);
	int m_nAlign;
	BOOL m_bShowText;
	BOOL m_bDrawBorder;
	BOOL m_bIsFlat;
	BOOL m_MouseOnButton;
	BOOL m_ButtonSelected;
	BOOL m_bDrawFlatFocus;
	BOOL m_bThin;

	CFont	m_font;
        int	m_nFontHeight;
        int m_nBkMode;
	HCURSOR m_hCursor;

	CNetBitmap   m_bmpButton;
	CNetBitmap   m_bmpButtonDown;
	CNetBitmap   m_bmpButtonFocussed;
	CNetBitmap   m_bmpButtonDisabled;
	BYTE m_cyIcon;
	BYTE m_cxIcon;

    COLORREF  m_crInactiveBg;
    COLORREF  m_crInactiveFg;
    COLORREF  m_crActiveBg;
    COLORREF  m_crActiveFg;
    COLORREF  m_crTransParent;

	TOOLINFO	m_tooltip;
};

#endif	// __NETBUTTON_H
