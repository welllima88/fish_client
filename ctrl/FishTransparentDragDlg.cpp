// FishTransparentDragDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\fish.h"
#include "../ctrl/MemDC.h"
#include "FishTransparentDragDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishTransparentDragDlg dialog
#define TRANSPARENT_DLG_CLASSNAME		        _T("FISHTransparentWnd")
#define FISH_REPOSITORY_BG_COLOR        RGB(0xF8, 0xF8, 0xF8)
#define WS_EX_LAYERED					0x00080000
#define ULW_COLORKEY					0x00000001
#define ULW_ALPHA						0x00000002
#define TRANSPARENT_RATIO               0

typedef BOOL (WINAPI *lpfnUpdateLayeredWindow)(	HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize,HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags );
typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)( HWND hwnd, COLORREF crKey, BYTE xAlpha, DWORD dwFlags );
static lpfnSetLayeredWindowAttributes g_lpfnSetLayeredWindowAttributes = NULL;
static lpfnUpdateLayeredWindow g_lpfnUpdateLayeredWindow = NULL;

BOOL InitLayeredWindows()
{
	if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
	{
		HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));

		g_lpfnUpdateLayeredWindow =	(lpfnUpdateLayeredWindow)GetProcAddress( hUser32, /*_T*/("UpdateLayeredWindow") );
		g_lpfnSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress( hUser32, /*_T*/("SetLayeredWindowAttributes") );

		if( g_lpfnUpdateLayeredWindow == NULL || g_lpfnSetLayeredWindowAttributes == NULL )
			return FALSE;
	}

	return TRUE;
}

CFishTransparentDragDlg::CFishTransparentDragDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFishTransparentDragDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFishTransparentDragDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if(!(::GetClassInfo(hInst, TRANSPARENT_DLG_CLASSNAME, &wndclass)))
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW ; //CS_SAVEBITS ;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor( hInst, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1); 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TRANSPARENT_DLG_CLASSNAME;
		if (!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}

	m_bLayeredWindows = InitLayeredWindows();

    m_pParent   =   pParent;
	// get layer window
}

void CFishTransparentDragDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFishTransparentDragDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFishTransparentDragDlg, CDialog)
	//{{AFX_MSG_MAP(CFishTransparentDragDlg)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFishTransparentDragDlg::Create(CWnd* pParent)
{
    CRect rcWindow  =   CRect(0, 0, 0, 0);
	DWORD dwStyle	= WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

    m_pParent   =   pParent;

    if( m_bLayeredWindows )
	{
		dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;

		if( !CreateEx( dwExStyle, TRANSPARENT_DLG_CLASSNAME, NULL, dwStyle, 
			rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), NULL, NULL, NULL ) )
			return FALSE;
	}
	else
	{
		CRect rcClient;
		GetParent()->GetClientRect( &rcClient );

		if( !CreateEx( dwExStyle, TRANSPARENT_DLG_CLASSNAME, NULL, dwStyle,
			rcWindow.left, rcWindow.top, rcClient.Width(), rcClient.Height(), NULL, NULL, NULL ) )
			return FALSE;
	}

    SetTransparentAlpha( (char)TRANSPARENT_RATIO );       // tranparent ratio setting

    return TRUE;
//    return CDialog::Create(IDD, pParent);
}

/////////////////////////////////////////////////////////////////////////////
// CFishTransparentDragDlg message handlers
BOOL CFishTransparentDragDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle	= WS_POPUP | WS_DISABLED;
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    CRect rcWindow(rect);

    m_pParent   =   pParentWnd;

    if( m_bLayeredWindows )
	{
		dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;

		if( !CreateEx( dwExStyle, TRANSPARENT_DLG_CLASSNAME, NULL, dwStyle, 
			rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), NULL, NULL, NULL ) )
			return FALSE;
	}
	else
	{
		CRect rcClient;
		GetParent()->GetClientRect( &rcClient );

		if( !CreateEx( dwExStyle, TRANSPARENT_DLG_CLASSNAME, NULL, dwStyle,
			rcWindow.left, rcWindow.top, rcClient.Width(), rcClient.Height(), NULL, NULL, NULL ) )
			return FALSE;
	}
    return TRUE;
//	return CDialog::Create(IDD, pParentWnd);
}

BOOL CFishTransparentDragDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // TODO: Add extra initialization here
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFishTransparentDragDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
/*
    if (!m_spBkgndBitmap)   return;

    CRect clientRT;
    GetClientRect(&clientRT);
    
    CDC memDC;
    memDC.SelectObject(&m_spBkgndBitmap);
    pDC->BitBlt(0, 0, clientRT.Width(), clientRT.Height(), &memDC, 0, 0, SRCCOPY);
*/

    CRect rc;
    GetClientRect(&rc);
    CMemDC pDC(&dc, &rc);

    pDC->SetBkMode(TRANSPARENT);
    CBrush *pOldBrush   =   (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
    CPen pen, *pOldPen;
    pen.CreatePen(PS_DOT, 1, RGB(0xAD, 0xAD, 0xAD));
    pOldPen =   pDC.SelectObject(&pen);
    pDC->Rectangle(rc);

}

void CFishTransparentDragDlg::SetBkgndImage(SmartPtr<CBitmap> bitmap)
{
    ASSERT(m_hWnd);
    ShowWindow(SW_HIDE);
    BITMAP bm;
    m_spBkgndBitmap =   bitmap;
    m_spBkgndBitmap->GetBitmap(&bm);

    CRect wndRT;
    GetWindowRect(wndRT);
    wndRT.right     =   wndRT.left  +   bm.bmWidth;
    wndRT.bottom    =   wndRT.top   +   bm.bmHeight;
    MoveWindow(&wndRT);

    SetTransparentAlpha( (char)TRANSPARENT_RATIO );       // tranparent ratio setting
}

void CFishTransparentDragDlg::Relayout()
{

}

/************************************************************************
SetTransparentAlpha 투명도를 받아서 투명도를 설정한다.
@PARAM  : 
@RETURN : 
@REMARK : 
    http://kr.blog.yahoo.com/guyya_genesis/11.html
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/27:CREATED
************************************************************************/
BOOL CFishTransparentDragDlg::SetTransparentAlpha(char chAlpha)
{ 
    ASSERT(m_hWnd);
    // char chAlpha = 255; //투명도 설정 0 ~ 255
    CRect rt;
    GetWindowRect(&rt);
    SetWindowPos( &wndTop, rt.left, rt.top, 0, 0, SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE );
    if (m_bLayeredWindows)
        g_lpfnSetLayeredWindowAttributes(GetSafeHwnd(), RGB(0xFF, 0xFF, 0xFF), chAlpha, ULW_COLORKEY );
    
    return TRUE;
} 

void CFishTransparentDragDlg::MakeItOpaque()
{
    ASSERT(m_hWnd);
    SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE );
}

void CFishTransparentDragDlg::DragMove(CPoint pt)
{
    ASSERT(m_hWnd);
    m_pParent->ClientToScreen(&pt);
    CRect rt;
    GetWindowRect(&rt);
    int nWidth, nHeight;
    nWidth  =   rt.Width();
    nHeight =   rt.Height();
    rt  =   CRect(pt.x, pt.y, pt.x + nWidth, pt.y + nHeight);
    MoveWindow(rt);
}

BOOL CFishTransparentDragDlg::OnEraseBkgnd(CDC* dc) 
{
    CRect rc;
    GetClientRect(&rc);
    CMemDC pDC(dc, &rc);

    pDC.FillSolidRect(&rc, FISH_REPOSITORY_BG_COLOR);

    return TRUE;
}

BOOL CFishTransparentDragDlg::DragShowNolock(BOOL bShow)
{
    return CImageList::DragShowNolock(bShow);
}

BOOL CFishTransparentDragDlg::DragEnter(CWnd *pWndLock, CPoint point)
{
    return CImageList::DragEnter(pWndLock, point);
}

BOOL CFishTransparentDragDlg::DragLeave(CWnd *pWndLock)
{
    return CImageList::DragLeave(pWndLock);
}
