// RcmmWordWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../fish.h"
#include "../fish_def.h"
#include "RcmmWordWnd.h"

#include "../Dlg_Search.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRcmmWordWnd

//IMPLEMENT_DYNCREATE(CRcmmWordWnd, CWnd)

CRcmmWordWnd::CRcmmWordWnd()
{
    m_pParent   =   NULL;
	m_nViewTimer	= NULL;
	m_bInit		= FALSE;

	InitObjects();
}

CRcmmWordWnd::~CRcmmWordWnd()
{
	DeleteObjects();
	if(m_nViewTimer) KillTimer(m_nViewTimer);
}


BEGIN_MESSAGE_MAP(CRcmmWordWnd, CWnd)
	//{{AFX_MSG_MAP(CRcmmWordWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRcmmWordWnd drawing

void CRcmmWordWnd::OnDraw(CDC* pDC)
{
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CRcmmWordWnd message handlers

void CRcmmWordWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	Drawing();
}

void CRcmmWordWnd::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 1 :
		Drawing();
		KillTimer(nIDEvent);
		m_nViewTimer = NULL;
		break;
	}
	
	CWnd::OnTimer(nIDEvent);
}



void CRcmmWordWnd::Drawing()
{	
	CClientDC dc(this);
	
	CDC bufferDC, memDC;
	CBitmap bmpBuffer;
	CBitmap* pOldBmp;
	
	bufferDC.CreateCompatibleDC(&dc);
	if( bufferDC != NULL )
	{
		bmpBuffer.CreateCompatibleBitmap(&dc, m_rectWnd.Width(), m_rectWnd.Height());
		pOldBmp = (CBitmap*)bufferDC.SelectObject(&bmpBuffer);
		bufferDC.SetBkMode(TRANSPARENT);
		memDC.CreateCompatibleDC(&dc);
		if( memDC != NULL )
		{		
			DrawMsg(&bufferDC);  
			
			dc.BitBlt(m_rectWnd.left, m_rectWnd.top, m_rectWnd.Width(), m_rectWnd.Height(), 
				&bufferDC, 0, 0, SRCCOPY);
			
			bufferDC.SelectObject(pOldBmp);
			bmpBuffer.DeleteObject();
			bufferDC.DeleteDC();
			memDC.DeleteDC();
		}
	}
//	UpdateWindow();
}

void CRcmmWordWnd::InitObjects()
{
	m_fontNormal.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	m_fontNormalBUl.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, TRUE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	
}

void CRcmmWordWnd::DeleteObjects()
{
	if( m_fontNormal.m_hObject ) m_fontNormal.DeleteObject();
	if( m_fontNormalBUl.m_hObject ) m_fontNormalBUl.DeleteObject();	
}

#define FISH_RCMM_BKGND_CLR RGB(0x5A, 0x96, 0xB5)
#define FISH_RCMM_FG_CLR RGB(0xFF, 0xFF, 0xFF)

void CRcmmWordWnd::DrawMsg(CDC *pBufferDC)
{
	pBufferDC->FillSolidRect(&m_rectWnd, FISH_RCMM_BKGND_CLR);
//	pBufferDC->FillSolidRect(&m_rectWnd, RGB(255, 0, 0));
	
	CFont* pOldFont = NULL;
	
	pBufferDC->SetTextColor( FISH_RCMM_FG_CLR );
	CRect rectTemp;

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if( m_rectWnd.PtInRect(pt))
		pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontNormalBUl);
	else
		pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontNormal);
	
	
//	DrawMorePoint(pBufferDC, m_rectWnd, strRcmmWord );

//	TRACE(_T("WORD : %s\r\n"), strRcmmWord);
	pBufferDC->DrawText(strRcmmWord, &m_rectWnd
		, DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS );
	
	pBufferDC->SelectObject(pOldFont);
}
 

void CRcmmWordWnd::InitView()
{
	m_nViewTimer = SetTimer(1, 100, NULL);
	
	GetClientRect(&m_rectWnd);
	m_bInit = TRUE;
}

void CRcmmWordWnd::SetRcmmWord(CString strWord)
{
	strRcmmWord = _T("");
	if( strWord.IsEmpty() == TRUE )
		return;
	else
		strRcmmWord.Format(_T("%s"), strWord);	
}

CString CRcmmWordWnd::GetRcmmWord()
{
	return strRcmmWord;
}

//////////////////////////////////////////////////////////////////////////
// NAME : DrawMorePoint
// PARM : CDC *pBufferDC : dc, CRect rectOrg : Text를 나타낼 기본 영역,
//		 CString strText : 나타낼 Text, UINT nFormat =/*DT_VCENTER|DT_SINGLELINE|DT_LEFT*/
// RETN : void
// DESC : '...'STR_TEXT_POINT 나타내기
// DATE : 2005-08-22 coded by lizzy, origin
//////////////////////////////////////////////////////////////////////////
void CRcmmWordWnd::DrawMorePoint(CDC *pBufferDC, CRect rectOrg, CString strText, UINT nFormat)
{

/*
	CRect rectTemp2;
	rectTemp2 = rectOrg;
	pBufferDC->DrawText(strText, &rectTemp2, nFormat | DT_CALCRECT);
	if( rectOrg.Width() >= rectTemp2.Width() ) //설정한 Rect가 Text크기보다 크므로 그냥 Draw
	{
		pBufferDC->DrawText(strText, rectOrg,
			nFormat);
		//DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );
	}
	else
	{

		pBufferDC->DrawText(strText, rectOrg,
			nFormat);

		//영역 약간 줄이기
		rectTemp2 = rectOrg;
		rectTemp2.right -= 12;
		
		//DrawText
		pBufferDC->DrawText(strText, &rectTemp2, nFormat);
		
		
		CRect rectTempPoint;
		rectTempPoint = rectTemp2;
		rectTempPoint.left = rectTemp2.right;
		rectTempPoint.right = rectOrg.right;
		
		//'...' Draw
		pBufferDC->DrawText(_T("..."), &rectTempPoint, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );		

	}
*/	
}

CRect CRcmmWordWnd::GetRealRect()
{	
	return m_rectReal;
}

void CRcmmWordWnd::SetRealRect(CRect rect)
{
	m_rectReal = rect;
}



void CRcmmWordWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDlg_Search* pWnd = (CDlg_Search*)GetParent();
	if(!pWnd) return;

	pWnd->SetRcmmWord(strRcmmWord);
	
	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CRcmmWordWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CRcmmWordWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_rectWnd.PtInRect(point))
	{
        if (m_pParent)  m_pParent->SendMessage(WM_REDRAW_RCMMDWNDS, NULL, NULL);

		HCURSOR hCursor = ::LoadCursor (NULL, IDC_HAND);
		if (NULL != hCursor) ::SetCursor (hCursor);

        Invalidate(TRUE);
	}

    TRACKMOUSEEVENT trackmouseevent;
	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0;
    ::_TrackMouseEvent(&trackmouseevent);

    CWnd::OnMouseMove(nFlags, point);
}

void CRcmmWordWnd::OnSize(UINT nType, int cx, int cy) 
{
	GetClientRect(&m_rectWnd);
}

void CRcmmWordWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
    if (m_pParent)  m_pParent->SendMessage(WM_REDRAW_RCMMDWNDS, NULL, NULL);
}

void CRcmmWordWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    if (m_pParent)  m_pParent->SendMessage(WM_REDRAW_RCMMDWNDS, NULL, NULL);
}

BOOL CRcmmWordWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	m_pParent   =   pParentWnd;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
