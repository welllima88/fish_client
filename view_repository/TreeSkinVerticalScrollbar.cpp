// SkinVerticleScrollbar.cpp : implementation file
//

#include "stdafx.h"
#include "../FISH.h"
#include "TreeSkinVerticalScrollbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeSkinVerticalScrollbar

CTreeSkinVerticalScrollbar::CTreeSkinVerticalScrollbar()
{
	bMouseDown = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;
	bDragging = false;

	nThumbTop = 36;
	dbThumbInterval = 0.000000;
	pTree = NULL;

}

CTreeSkinVerticalScrollbar::~CTreeSkinVerticalScrollbar()
{
}


BEGIN_MESSAGE_MAP(CTreeSkinVerticalScrollbar, CStatic)
	//{{AFX_MSG_MAP(CTreeSkinVerticalScrollbar)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeSkinVerticalScrollbar message handlers

BOOL CTreeSkinVerticalScrollbar::OnEraseBkgnd(CDC* pDC) 
{
	return CStatic::OnEraseBkgnd(pDC);
}

void CTreeSkinVerticalScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    pTree->Invalidate(FALSE);

	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	int nHeight = clientRect.Height() - 37;
	

	CRect rectUpArrow(0,11,12,37);
	CRect rectDownArrow(0,nHeight,12,nHeight+26);
	CRect rectThumb(0,nThumbTop,12,nThumbTop+26);

	if(rectThumb.PtInRect(point))
	{
		bMouseDown = true;
	}

	if(rectDownArrow.PtInRect(point))
	{
		bMouseDownArrowDown = true;
		SetTimer(2,250,NULL);
	}

	if(rectUpArrow.PtInRect(point))
	{
		bMouseDownArrowUp = true;
		SetTimer(2,250,NULL);
	}

    if ( GetCapture() != this )   SetCapture();

	CStatic::OnLButtonDown(nFlags, point);
}

void CTreeSkinVerticalScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();
	
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	int nHeight = clientRect.Height() - 37;
	CRect rectUpArrow(0,11,12,37);
	CRect rectDownArrow(0,nHeight,12,nHeight+26);
	CRect rectThumb(0,nThumbTop,12,nThumbTop+26);



	if(rectUpArrow.PtInRect(point) && bMouseDownArrowUp)
	{
		ScrollUp();	
		bInChannel = false;
	}

	if(rectDownArrow.PtInRect(point) && bMouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if(rectThumb.PtInRect(point))
	{
		bInChannel = false;
	}

	if(bInChannel == true && !bMouseDown)
	{
		if(point.y > nThumbTop)
		{
	    	ScrollDown();
		}
		else
		{
            ScrollUp();
		}
	}

	bMouseDown = false;
	bDragging = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;

    if ( GetCapture() == this ) ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}

void CTreeSkinVerticalScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
    static int prevThumbTop =   0;
	CRect clientRect;
	GetClientRect(&clientRect);

	if(bMouseDown)
	{
		nThumbTop = point.y; //-13 so mouse is in middle of thumb
        LimitThumbPosition();

		double nMax = pTree->GetScrollLimit(SB_VERT);
		int nPos = pTree->GetScrollPos(SB_VERT);

		double nHeight = clientRect.Height()-98;
		double nVar = nMax;
		dbThumbInterval = nHeight/nVar;

		//figure out how many times to scroll total from top
		//then minus the current position from it
		int nScrollTimes = (int)((nThumbTop - 36)/dbThumbInterval)-nPos;

		//grab the row height dynamically
		//so if the font size or type changes
		//our scroll will still work properly
/*        if (prevThumbTop < nThumbTop)
        {*/
            for (int it = 0; it < nMax; it++)
            {
                if ( point.y > 49+dbThumbInterval*it && point.y <= 49+dbThumbInterval*(it+1) )
                {
                    if (it < pTree->GetScrollPos(SB_VERT) )   {
                        pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0), NULL);
                    }   else    if ( it == pTree->GetScrollPos(SB_VERT) )    {
                    }   else    {
                        pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0), NULL);
                    }
                    break;
                }
            }
/*        }   else    {
            for (int it = 0; it <= nMax; it++)
            {
                if ( point.y > 49+dbThumbInterval*it-2 && point.y < 49+dbThumbInterval*it+2 )
                {
                    if (dbThumbInterval*nPos > point.y )   pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0), NULL);
                    else
                        pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0), NULL);

                    break;
                }
            }
        }*/
    	Draw();
    }
	CStatic::OnMouseMove(nFlags, point);
}

void CTreeSkinVerticalScrollbar::OnPaint() 
{
	CPaintDC dc(this); 
	
	Draw();
}

void CTreeSkinVerticalScrollbar::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		if(bMouseDownArrowDown)
		{
			ScrollDown();
		}
		
		if(bMouseDownArrowUp)
		{
			ScrollUp();
		}
	}
	else if(nIDEvent == 2)
	{
		if(bMouseDownArrowDown)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
		
		if(bMouseDownArrowUp)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
	}
	CStatic::OnTimer(nIDEvent);
}

void CTreeSkinVerticalScrollbar::PageDown()
{
	pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CTreeSkinVerticalScrollbar::PageUp()
{
	pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CTreeSkinVerticalScrollbar::ScrollUp()
{
	pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CTreeSkinVerticalScrollbar::ScrollDown()
{
	pTree->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CTreeSkinVerticalScrollbar::UpdateThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	double nPos = pTree->GetScrollPos(SB_VERT);
	double nMax = pTree->GetScrollLimit(SB_VERT);
	double nHeight = (clientRect.Height()-98);
	double nVar = nMax;

	dbThumbInterval = nHeight/nVar;

	double nNewdbValue = (dbThumbInterval * nPos);
	int nNewValue = (int)nNewdbValue;


	nThumbTop = 36+nNewValue;

	LimitThumbPosition();

	Draw();
}


void CTreeSkinVerticalScrollbar::Draw()
{

	CClientDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CMemDC memDC(&dc, &clientRect);
	memDC.FillSolidRect(&clientRect,  RGB(74,82,107));
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
    CBitmap* pOldBitmap;
	bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_TOP);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,clientRect.top,12,11,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;

	bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_UPARROW);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,clientRect.top+11,12,26,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
	
	//draw the background (span)
	bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_SPAN);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	int nHeight = clientRect.Height() - 37;

	memDC.StretchBlt(clientRect.left, clientRect.top+37, 12,nHeight,&bitmapDC, 0,0, 12, 1, SRCCOPY);

	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
	
	//draw the down arrow of the scrollbar
	bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_DOWNARROW);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,nHeight,12,26,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;

		//draw the down arrow of the scrollbar
	bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_BOTTOM);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left+1,nHeight+26,11,11,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;

	//If there is nothing to scroll then don't
	//show the thumb control otherwise show it
	if(pTree->GetScrollLimit(SB_VERT) != 0)
	{
		//draw the thumb control
		bitmap.LoadBitmap(IDB_LISTCTRL_VERTICLE_SCROLLBAR_THUMB);
		pOldBitmap = bitmapDC.SelectObject(&bitmap);
		memDC.BitBlt(clientRect.left,clientRect.top+nThumbTop,12,26,&bitmapDC,0,0,SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		pOldBitmap = NULL;
	}


}

void CTreeSkinVerticalScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if(nThumbTop+26 > (clientRect.Height()-37))
	{
		nThumbTop = clientRect.Height()-62;
	}

	if(nThumbTop < (clientRect.top+36))
	{
		nThumbTop = clientRect.top+36;
	}
}