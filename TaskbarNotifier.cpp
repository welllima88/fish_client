// TaskbarNotifier.cpp : implementation file
// By John O'Byrne
// 11 August 2002: - Timer precision is now determined automatically
//                 Complete change in the way the popup is showing (thanks to this,now the popup can be always on top, it shows even while watching a movie)
//                 The popup doesn't steal the focus anymore (by replacing ShowWindow(SW_SHOW) by ShowWindow(SW_SHOWNOACTIVATE))
//                 Thanks to Daniel Lohmann, update in the way the taskbar pos is determined (more flexible now)
// 17 July 2002: - Another big Change in the method for determining the pos of the taskbar (using the SHAppBarMessage function)
// 16 July 2002: - Thanks to the help of Daniel Lohmann, the Show Function timings work perfectly now ;)
// 15 July 2002: - Change in the method for determining the pos of the taskbar
//               (now handles the presence of quick launch or any other bar).
//               Remove the Handlers for WM_CREATE and WM_DESTROY
//               SetSkin is now called SetBitmap
// 14 July 2002: - Changed the GenerateRegion func by a new one (to correct a win98 bug)

#include "stdafx.h"
#include "TaskbarNotifier.h"
#include "resource.h"

#define IDT_HIDDEN		0
#define IDT_APPEARING		1
#define IDT_WAITING		2
#define IDT_DISAPPEARING	3
#define TASKBAR_X_TOLERANCE	40
#define TASKBAR_Y_TOLERANCE 40
//#define ID_NOTIFIER_CLOSE_BTN	9090

#define NTMAXBUF			40


inline bool NearlyEqual( int a, int b, int epsilon )
{
	return abs( a - b ) < epsilon / 2;	
}

// CTaskbarNotifier
IMPLEMENT_DYNAMIC(CTaskbarNotifier, CWnd)
CTaskbarNotifier::CTaskbarNotifier()
{
	m_strCaption="";
	m_pWndParent=NULL;
	m_bMouseIsOver=FALSE;
	m_hBitmapRegion=NULL;
	m_hCursor=NULL;
	m_crNormalTextColor=RGB(133,146,181);
	m_crSelectedTextColor=RGB(10,36,106);
	m_nBitmapHeight=0;
	m_nBitmapWidth=0;
	
	m_dwTimeToStay=0;
	m_dwShowEvents=0;
	m_dwHideEvents=0;
	m_nCurrentPosX=0;
	m_nCurrentPosY=0;
	m_nCurrentWidth=0;
	m_nCurrentHeight=0;
	m_nIncrementShow=0;
	m_nIncrementHide=0;
	m_nTaskbarPlacement=ABE_BOTTOM;
	m_nAnimStatus=IDT_HIDDEN;
	m_rcText.SetRect(0,0,0,0);
// 	m_uTextFormat=DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS; // Default Text format (see DrawText in the win32 API for the different values)
	m_uTextFormat=DT_CENTER| DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS; // Default Text format (see DrawText in the win32 API for the different values)
	m_hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649)); // System Hand cursor
	
	// If running on NT, timer precision is 10 ms, if not timer precision is 50 ms
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
		m_dwTimerPrecision=10;
	else
		m_dwTimerPrecision=50;

	SetTextDefaultFont(); // We use default GUI Font

	//m_nType = EZ_NT_NOTE;
	//m_rcClose.SetRect(206, 5, 215,14);
	m_rcClose.SetRect(BG_WIDTH - BTN_WIDTH - CLOSE_MG_RIGHT, CLOSE_MG_TOP, BG_WIDTH - CLOSE_MG_RIGHT, CLOSE_MG_TOP + BTN_HEIGHT);
	
	m_bCursorHand = false;
}

CTaskbarNotifier::~CTaskbarNotifier()
{
	// No need to delete the HRGN,  SetWindowRgn() owns it after being called

	if( m_bmpCloseIcon.m_hObject )
		m_bmpCloseIcon.DeleteObject();	
}

int CTaskbarNotifier::Create(CWnd *pWndParent)
{
	ASSERT(pWndParent!=NULL);
    m_pWndParent=pWndParent;
	
	WNDCLASSEX wcx; 

	wcx.cbSize = sizeof(wcx);
	wcx.lpfnWndProc = AfxWndProc;
	wcx.style = CS_DBLCLKS|CS_SAVEBITS;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = AfxGetInstanceHandle();
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground=::GetSysColorBrush(COLOR_WINDOW);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = _T("Fish Notifier");
	wcx.hIconSm = NULL;

	ATOM classAtom = RegisterClassEx(&wcx);
	      
	return CreateEx(WS_EX_TOPMOST, _T("Fish Notifier"),NULL,WS_POPUP,0,0,0,0,pWndParent->m_hWnd,NULL);
}

void CTaskbarNotifier::SetTextFont(LPCTSTR szFont,int nSize,int nNormalStyle,int nSelectedStyle)
{
	LOGFONT lf;
	m_myNormalFont.DeleteObject();
	m_myNormalFont.CreatePointFont(nSize,szFont);
	m_myNormalFont.GetLogFont(&lf);
	
	// We  set the Font of the unselected ITEM
	if (nNormalStyle & TN_TEXT_BOLD)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_NORMAL;
	
	if (nNormalStyle & TN_TEXT_ITALIC)
		lf.lfItalic=TRUE;
	else
		lf.lfItalic=FALSE;
	
	if (nNormalStyle & TN_TEXT_UNDERLINE)
		lf.lfUnderline=TRUE;
	else
		lf.lfUnderline=FALSE;

	m_myNormalFont.DeleteObject();
	m_myNormalFont.CreateFontIndirect(&lf);
	
	// We set the Font of the selected ITEM
	if (nSelectedStyle & TN_TEXT_BOLD)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_NORMAL;
	
	if (nSelectedStyle & TN_TEXT_ITALIC)
		lf.lfItalic=TRUE;
	else
		lf.lfItalic=FALSE;
	
	if (nSelectedStyle & TN_TEXT_UNDERLINE)
		lf.lfUnderline=TRUE;
	else
		lf.lfUnderline=FALSE;

	m_mySelectedFont.DeleteObject();
	m_mySelectedFont.CreateFontIndirect(&lf);
}

void CTaskbarNotifier::SetTextDefaultFont()
{
	LOGFONT lf;
	CFont *pFont=CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
    pFont->GetLogFont(&lf);
	m_myNormalFont.DeleteObject();
	m_mySelectedFont.DeleteObject();
	m_myNormalFont.CreateFontIndirect(&lf);
	lf.lfUnderline=TRUE;
	m_mySelectedFont.CreateFontIndirect(&lf);
}

void CTaskbarNotifier::SetTextColor(COLORREF crNormalTextColor,COLORREF crSelectedTextColor)
{
	m_crNormalTextColor=crNormalTextColor;
	m_crSelectedTextColor=crSelectedTextColor;
	RedrawWindow();
}

void CTaskbarNotifier::SetTextRect(RECT rcText)
{
	m_rcText=rcText;
}

void CTaskbarNotifier::SetTextFormat(UINT uTextFormat)
{
	m_uTextFormat=uTextFormat;
}

BOOL CTaskbarNotifier::SetBitmap(UINT nBitmapID,short red,short green,short blue)
{
	BITMAP bm;
	
	m_bitmapBackground.DeleteObject();

	if (!m_bitmapBackground.LoadBitmap(nBitmapID))
		return FALSE;
	GetObject(m_bitmapBackground.GetSafeHandle(), sizeof(bm), &bm);
	m_nBitmapWidth=bm.bmWidth;
	m_nBitmapHeight=bm.bmHeight;
	m_rcText.SetRect(0,0,bm.bmWidth,bm.bmHeight);

	if (red!=-1 && green!=-1 && blue!=-1)
	{
		// No need to delete the HRGN,  SetWindowRgn() owns it after being called
		m_hBitmapRegion=CreateRgnFromBitmap((HBITMAP)m_bitmapBackground.GetSafeHandle(),RGB(red,green,blue));
		SetWindowRgn(m_hBitmapRegion, true);
	}

	// Close Icon
	m_bmpCloseIcon.LoadBitmap(IDB_BTN_CLOSE_N);

	return TRUE;
}

BOOL CTaskbarNotifier::SetBitmap(LPCTSTR szFileName,short red,short green,short blue)
{
	BITMAP bm;
	HBITMAP hBmp;
	
	hBmp=(HBITMAP) ::LoadImage(AfxGetInstanceHandle(),szFileName,IMAGE_BITMAP,0,0, LR_LOADFROMFILE);
	if (!hBmp)
		return FALSE;

	m_bitmapBackground.DeleteObject();
	m_bitmapBackground.Attach(hBmp);
	GetObject(m_bitmapBackground.GetSafeHandle(), sizeof(bm), &bm);
	m_nBitmapWidth=bm.bmWidth;
	m_nBitmapHeight=bm.bmHeight;
	m_rcText.SetRect(0,0,bm.bmWidth,bm.bmHeight);

	if (red!=-1 && green!=-1 && blue!=-1)
	{
		// No need to delete the HRGN,  SetWindowRgn() owns it after being called
		m_hBitmapRegion=CreateRgnFromBitmap((HBITMAP)m_bitmapBackground.GetSafeHandle(),RGB(red,green,blue));
		SetWindowRgn(m_hBitmapRegion, true);
	}

	// Close Icon
	m_bmpCloseIcon.LoadBitmap(IDB_BTN_CLOSE_N);

	return TRUE;
}

void CTaskbarNotifier::Show(LPCTSTR szCaption,DWORD dwTimeToShow,DWORD dwTimeToStay,DWORD dwTimeToHide)
{
	UINT nScreenWidth;
	UINT nScreenHeight;
	UINT nEvents;
	UINT nBitmapSize;
	CRect rcTaskbar;

	m_strCaption=szCaption;
	m_dwTimeToStay=dwTimeToStay;
	
	nScreenWidth=::GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight=::GetSystemMetrics(SM_CYSCREEN);
	HWND hWndTaskbar = ::FindWindow(_T("Shell_TrayWnd"),0);
	::GetWindowRect(hWndTaskbar,&rcTaskbar);

	// Daniel Lohmann: Calculate taskbar position from its window rect. However, on XP
	// it may be that the taskbar is slightly larger or smaller than the
	// screen size. Therefore we allow some tolerance here.

	if( NearlyEqual( rcTaskbar.left, 0, TASKBAR_X_TOLERANCE ) && NearlyEqual( rcTaskbar.right, nScreenWidth, TASKBAR_X_TOLERANCE ) )
	{
		// Taskbar is on top or on bottom
		m_nTaskbarPlacement = NearlyEqual( rcTaskbar.top, 0, TASKBAR_Y_TOLERANCE ) ? ABE_TOP : ABE_BOTTOM;
		nBitmapSize=m_nBitmapHeight;
	}
	else 
	{
		// Taskbar is on left or on right
		m_nTaskbarPlacement = NearlyEqual( rcTaskbar.left, 0, TASKBAR_X_TOLERANCE ) ? ABE_LEFT : ABE_RIGHT;
		nBitmapSize=m_nBitmapWidth;
	}

	// We calculate the pixel increment and the timer value for the showing animation
	if (dwTimeToShow > m_dwTimerPrecision)
	{
		nEvents=min((dwTimeToShow / m_dwTimerPrecision), nBitmapSize);
		m_dwShowEvents = dwTimeToShow / nEvents;
		m_nIncrementShow = nBitmapSize / nEvents;
	}
	else
	{
		m_dwShowEvents = m_dwTimerPrecision;
		m_nIncrementShow = nBitmapSize;
	}

	// We calculate the pixel increment and the timer value for the hiding animation
	if( dwTimeToHide > m_dwTimerPrecision )
	{
		nEvents = min((dwTimeToHide / m_dwTimerPrecision), nBitmapSize);
		m_dwHideEvents = dwTimeToHide / nEvents;
		m_nIncrementHide = nBitmapSize / nEvents;
	}
	else
	{
		m_dwShowEvents = m_dwTimerPrecision;
		m_nIncrementHide = nBitmapSize;
	}
	
	// Compute init values for the animation
	switch (m_nAnimStatus)
	{
		case IDT_HIDDEN:
			if (m_nTaskbarPlacement==ABE_RIGHT)
			{
				m_nCurrentPosX=rcTaskbar.left;
				m_nCurrentPosY=rcTaskbar.bottom-m_nBitmapHeight;
				m_nCurrentWidth=0;
				m_nCurrentHeight=m_nBitmapHeight;
			}
			else if (m_nTaskbarPlacement==ABE_LEFT)
			{
				m_nCurrentPosX=rcTaskbar.right;
				m_nCurrentPosY=rcTaskbar.bottom-m_nBitmapHeight;
				m_nCurrentWidth=0;
				m_nCurrentHeight=m_nBitmapHeight;
			}
			else if (m_nTaskbarPlacement==ABE_TOP)
			{
				m_nCurrentPosX=rcTaskbar.right-m_nBitmapWidth;
				m_nCurrentPosY=rcTaskbar.bottom;
				m_nCurrentWidth=m_nBitmapWidth;
				m_nCurrentHeight=0;
			}
			else //if (m_nTaskbarPlacement==ABE_BOTTOM)
			{
				// Taskbar is on the bottom or Invisible
				m_nCurrentPosX=rcTaskbar.right-m_nBitmapWidth;
				m_nCurrentPosY=rcTaskbar.top;
				m_nCurrentWidth=m_nBitmapWidth;
				m_nCurrentHeight=0;
			}

			ShowWindow(SW_SHOWNOACTIVATE);	
			SetTimer(IDT_APPEARING,m_dwShowEvents,NULL);
			break;

		case IDT_APPEARING:
			RedrawWindow();
			break;

		case IDT_WAITING:
			RedrawWindow();
			KillTimer(IDT_WAITING);
			SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
			break;

		case IDT_DISAPPEARING:
			KillTimer(IDT_DISAPPEARING);
			SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
			if (m_nTaskbarPlacement==ABE_RIGHT)
			{
				m_nCurrentPosX=rcTaskbar.left-m_nBitmapWidth;
				m_nCurrentWidth=m_nBitmapWidth;
			}
			else if (m_nTaskbarPlacement==ABE_LEFT)
			{
				m_nCurrentPosX=rcTaskbar.right;
				m_nCurrentWidth=m_nBitmapWidth;
			}
			else if (m_nTaskbarPlacement==ABE_TOP)
			{
				m_nCurrentPosY=rcTaskbar.bottom;
				m_nCurrentHeight=m_nBitmapHeight;
			}
			else //if (m_nTaskbarPlacement==ABE_BOTTOM)
			{
				m_nCurrentPosY=rcTaskbar.top-m_nBitmapHeight;
				m_nCurrentHeight=m_nBitmapHeight;
			}
			
			SetWindowPos(&wndTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);
			RedrawWindow();
			break;
	}
}

void CTaskbarNotifier::Hide()
{
	switch (m_nAnimStatus)
	{
		case IDT_APPEARING:
			KillTimer(IDT_APPEARING);
			break;
		case IDT_WAITING:
			KillTimer(IDT_WAITING);
			break;
		case IDT_DISAPPEARING:
			KillTimer(IDT_DISAPPEARING);
			break;
	}
	MoveWindow(0,0,0,0);
	ShowWindow(SW_HIDE);
	m_nAnimStatus=IDT_HIDDEN;
}

HRGN CTaskbarNotifier::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	// this code is written by Davide Pizzolato

	if (!hBmp) return NULL;

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

	CDC dcBmp;
	CDC* pDC = GetDC();
	if(!pDC) return NULL;
	dcBmp.CreateCompatibleDC(pDC);	//Creates a memory device context for the bitmap
	dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
//	const DWORD NTMAXBUF = 40;		// size of one block in RECTs
	// (i.e. NTMAXBUF*sizeof(RECT) in bytes)
	LPRECT	pRects;								
	DWORD	cBlocks = 0;			// number of allocated blocks

	INT		i, j;					// current position in mask image
	INT		first = 0;				// left position of current scan line
	// where mask was found
	bool	wasfirst = false;		// set when if mask was found in current scan line
	bool	ismask;					// set when current color is mask color

	// allocate memory for region data
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * NTMAXBUF * sizeof(RECT) ];
	memset( pRgnData, 0, RDHDR + cBlocks * NTMAXBUF * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RDHDR;
	pRgnData->iType		= RDH_RECTANGLES;
	pRgnData->nCount	= 0;
	for ( i = 0; i < bm.bmHeight; i++ )
		for ( j = 0; j < bm.bmWidth; j++ ){
			// get color
			ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth)))){
				// get offset to RECT array if RGNDATA buffer
				pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
				// save current RECT
				pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+(j==(bm.bmWidth-1)), bm.bmHeight - i );
				// if buffer full reallocate it
				if ( pRgnData->nCount >= cBlocks * NTMAXBUF ){
					LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * NTMAXBUF * sizeof(RECT) ];
					memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * NTMAXBUF * sizeof(RECT) );
					delete pRgnData;
					pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				}
				wasfirst = false;
			} else if ( !wasfirst && ismask ){		// set wasfirst when mask is found
				first = j;
				wasfirst = true;
			}
		}
		dcBmp.DeleteDC();	//release the bitmap
		// create region
		/*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
		//	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
		/* ExtCreateRegion replacement { */
		HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
		ASSERT( hRgn!=NULL );
		pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
		for(i=0;i<(int)pRgnData->nCount;i++)
		{
			HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
			VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
			if (hr) DeleteObject(hr);
		}
		ASSERT( hRgn!=NULL );
		/* } ExtCreateRegion replacement */

		delete pRgnData;

		return hRgn;
}

BEGIN_MESSAGE_MAP(CTaskbarNotifier, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
//	ON_BN_CLICKED(ID_NOTIFIER_CLOSE_BTN, OnBtnClose)
END_MESSAGE_MAP()


// CTaskbarNotifier message handlers

void CTaskbarNotifier::OnMouseMove(UINT nFlags, CPoint point)
{

	if( m_rcText.PtInRect( point ) ) 
	{
		m_bCursorHand = true;
		if( m_bMouseIsOver == FALSE )
		{
			m_bMouseIsOver = TRUE;
			RedrawWindow();
		}
	}
	else if( m_rcClose.PtInRect( point ))
	{
		m_bCursorHand = true;
		if( m_bMouseIsOver == TRUE )
		{
			m_bMouseIsOver = FALSE;
			RedrawWindow();
		}
		m_bMouseIsOver = FALSE;
	}
	else
	{
		m_bCursorHand = false;
		if( m_bMouseIsOver == TRUE )
		{
			m_bMouseIsOver = FALSE;
			RedrawWindow();
		}
	}

	TRACKMOUSEEVENT t_MouseEvent;
	t_MouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
	t_MouseEvent.dwFlags     = TME_LEAVE | TME_HOVER;
	t_MouseEvent.hwndTrack   = m_hWnd;
	t_MouseEvent.dwHoverTime = 1;

	// We Tell Windows we want to receive WM_MOUSEHOVER and WM_MOUSELEAVE
	::_TrackMouseEvent(&t_MouseEvent);

	CWnd::OnMouseMove(nFlags, point);
}

void CTaskbarNotifier::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Notify the parent window that the Notifier popup was clicked
	m_pWndParent->PostMessage(WM_TASKBARNOTIFIERCLICKED,0,0);
	// 마우스 클릭시 이벤트 처리..
	if( m_rcText.PtInRect( point ) )
	{
		Hide();
		ParseMessage();
	}	
	else if( m_rcClose.PtInRect( point ) )
	{
		Hide();
	}	
	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CTaskbarNotifier::OnMouseHover(WPARAM w, LPARAM l)
{
//	if (m_bMouseIsOver==FALSE)
//	{
//		m_bMouseIsOver=TRUE;
//		RedrawWindow();
//	}
	return 0;
}

LRESULT CTaskbarNotifier::OnMouseLeave(WPARAM w, LPARAM l)
{
//	if (m_bMouseIsOver==TRUE)
//	{
//		m_bMouseIsOver=FALSE;
//		RedrawWindow();
//	}
	return 0;
}

BOOL CTaskbarNotifier::OnEraseBkgnd(CDC* pDC)
{
	CDC memDC;
	CBitmap *pOldBitmap;
	
	memDC.CreateCompatibleDC(pDC);
	if( memDC != NULL )
	{
		pOldBitmap=memDC.SelectObject(&m_bitmapBackground);
		// draw bg..
		pDC->BitBlt(0,0,m_nCurrentWidth,m_nCurrentHeight,&memDC,0,0,SRCCOPY);
		memDC.SelectObject(pOldBitmap);
			

//		pDC->BitBlt(0,0,m_nCurrentWidth,m_nCurrentHeight,&bufferDC,0,0,SRCCOPY);
//		bufferDC.SelectObject(pOldBitmap);
		
		memDC.DeleteDC();
// 		bufferDC.DeleteDC();
			
	}

	return TRUE;
}

void CTaskbarNotifier::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	CFont *pOldFont;
	
	// draw icon
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	if( memDC != NULL )
	{

		CBitmap* pOldBmpNote=NULL;
		pOldBmpNote = (CBitmap*)memDC.SelectObject( &m_bmpCloseIcon );
		if( pOldBmpNote )
		{
			dc.BitBlt( m_rcClose.left, m_rcClose.top, m_rcClose.Width(), m_rcClose.Height(), &memDC, 0, 0, SRCCOPY );			
			memDC.SelectObject(pOldBmpNote );
			pOldBmpNote->DeleteObject();
		}


		dc.SetTextColor(m_crNormalTextColor);
		pOldFont = dc.SelectObject( & m_myNormalFont );
		dc.SetBkMode(TRANSPARENT); 
		//rcClient.DeflateRect(10, 50, 10, 50);
		//CRect rect = CRect(m_rcText.left, , m_rcText.right, m_rcText.bottom );
		CRect rect = m_rcText;
		dc.DrawText(m_strCaption, -1, rect, m_uTextFormat);
		dc.SelectObject( pOldFont );
		
		memDC.DeleteDC();
	}

	if (m_bMouseIsOver)
	{
		dc.SetTextColor(m_crSelectedTextColor);
		pOldFont=dc.SelectObject(&m_mySelectedFont);
	}
	else
	{
		dc.SetTextColor(m_crNormalTextColor);
		pOldFont=dc.SelectObject(&m_myNormalFont);
	}

	dc.SelectObject(pOldFont);
}

BOOL CTaskbarNotifier::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
// 	if (nHitTest == HTCLIENT)
	if (m_bCursorHand)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CTaskbarNotifier::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case IDT_APPEARING:
			m_nAnimStatus=IDT_APPEARING;
			switch(m_nTaskbarPlacement)
			{
				case ABE_BOTTOM:
					if (m_nCurrentHeight<m_nBitmapHeight)
					{
						m_nCurrentPosY-=m_nIncrementShow;
						m_nCurrentHeight+=m_nIncrementShow;
					}
					else
					{
						KillTimer(IDT_APPEARING);
						SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
						m_nAnimStatus=IDT_WAITING;
					}
					break;
				case ABE_TOP:
					if (m_nCurrentHeight<m_nBitmapHeight)
						m_nCurrentHeight+=m_nIncrementShow;
					else
					{
						KillTimer(IDT_APPEARING);
						SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
						m_nAnimStatus=IDT_WAITING;
					}
					break;
				case ABE_LEFT:
					if (m_nCurrentWidth<m_nBitmapWidth)
						m_nCurrentWidth+=m_nIncrementShow;
					else
					{
						KillTimer(IDT_APPEARING);
						SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
						m_nAnimStatus=IDT_WAITING;
					}
					break;
				case ABE_RIGHT:
					if (m_nCurrentWidth<m_nBitmapWidth)
					{
						m_nCurrentPosX-=m_nIncrementShow;
						m_nCurrentWidth+=m_nIncrementShow;
					}
					else
					{
						KillTimer(IDT_APPEARING);
						SetTimer(IDT_WAITING,m_dwTimeToStay,NULL);
						m_nAnimStatus=IDT_WAITING;
					}
					break;
			}
			SetWindowPos(&wndTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);
			break;

		case IDT_WAITING:
			KillTimer(IDT_WAITING);
			SetTimer(IDT_DISAPPEARING,m_dwHideEvents,NULL);
			break;

		case IDT_DISAPPEARING:
			m_nAnimStatus=IDT_DISAPPEARING;
			switch(m_nTaskbarPlacement)
			{
				case ABE_BOTTOM:
					if (m_nCurrentHeight>0)
					{
						m_nCurrentPosY+=m_nIncrementHide;
						m_nCurrentHeight-=m_nIncrementHide;
					}
					else
					{
						KillTimer(IDT_DISAPPEARING);
						Hide();
					}
					break;
				case ABE_TOP:
					if (m_nCurrentHeight>0)
						m_nCurrentHeight-=m_nIncrementHide;
					else
					{
						KillTimer(IDT_DISAPPEARING);
						Hide();
					}
					break;
				case ABE_LEFT:
					if (m_nCurrentWidth>0)
						m_nCurrentWidth-=m_nIncrementHide;
					else
					{
						KillTimer(IDT_DISAPPEARING);
						Hide();
					}
					break;
				case ABE_RIGHT:
					if (m_nCurrentWidth>0)
					{
						m_nCurrentPosX+=m_nIncrementHide;
						m_nCurrentWidth-=m_nIncrementHide;
					}
					else
					{
						KillTimer(IDT_DISAPPEARING);
						Hide();
					}
					break;
			}
			SetWindowPos(&wndTopMost,m_nCurrentPosX,m_nCurrentPosY,m_nCurrentWidth,m_nCurrentHeight,SWP_NOACTIVATE);
			break;
	}

	CWnd::OnTimer(nIDEvent);
}


// Add[Modify] at 2003-03-24 오후 8:56:18 by bemlove 
void CTaskbarNotifier::ParseMessage()
{/*
	// 클릭시 실행..
	CMainFrame* pMainFrame = NULL;
	pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if( pMainFrame==NULL || pMainFrame->GetSafeHwnd() == INVALID_HANDLE_VALUE ) 
		return;
	CInfoView* pIFView = (CInfoView*)pMainFrame->GetViewPointer(INDEX_INFO_VIEW);
	if( pIFView->GetSafeHwnd())
	{
		if( m_nType == EZ_NT_NOTE )
		{
// 			pIFView->NavigateADBanner(TYPE_SHOWMESSAGE);
			pIFView->ViewNote();
		}
		else if( m_nType == EZ_NT_FILE )
			pIFView->OpenDownloadPath();
	}
	*/
}//end of ParseMessage()
/*
void CTaskbarNotifier::OnBtnClose()
{
	Hide();
}
*/