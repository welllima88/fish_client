// SkinHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "../FISH.h"
#include "../ctrl/MemDC.h"
#include "SkinHeaderCtrl.h"
#include "fishlistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl

CSkinHeaderCtrl::CSkinHeaderCtrl()
{
    
}

CSkinHeaderCtrl::~CSkinHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkinHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSkinHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MEASUREITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl message handlers

void CSkinHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
}

void CSkinHeaderCtrl::OnPaint() 
{
    enum {
        FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH    =   2,
        FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH         =   2,
        FISH_VIEWINFO_LISTCTRL_HEADER_IMG_HEIGHT        =   12,
    };

	CPaintDC dc(this); // device context for painting
	
	CRect rect, rectItem, clientRect;
	GetClientRect(&rect);
    clientRect = rect;

    CMemDC memDC(&dc, &rect);
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);
	
	memDC.FillSolidRect(&rect, ((CFishListCtrl*)GetParent())->GetBkColor() );

	CBitmap bitmapSpan;
    BITMAP bmSpan;
	bitmapSpan.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN);
    bitmapSpan.GetBitmap(&bmSpan);
	CBitmap* pOldBitmapSpan = bitmapDC.SelectObject(&bitmapSpan);

	memDC.StretchBlt(rect.left+FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH, 0, rect.Width(), rect.Height(), &bitmapDC, 0, 0, bmSpan.bmWidth, bmSpan.bmHeight, SRCCOPY);

	bitmapDC.SelectObject(pOldBitmapSpan);
	bitmapSpan.DeleteObject();
	
	int nItems = GetItemCount();
    
	CBitmap bitmap;
	CBitmap bitmap2;
	CBitmap bitmap3;
    BITMAP bm;
    BITMAP bm2;
    BITMAP bm3;
	
	bitmap.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_START);
	bitmap2.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN);
	bitmap3.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_END);

    bitmap.GetBitmap(&bm);
    bitmap.GetBitmap(&bm2);
    bitmap.GetBitmap(&bm3);

	for(int i = 0; i <nItems; i++)
	{
		
		TCHAR buf1[256];
		HD_ITEM hditem1;
		
		hditem1.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem1.pszText = buf1;
		hditem1.cchTextMax = 255;
		GetItem( i, &hditem1 );
		
		GetItemRect(i, &rect);
		
		CBitmap* pOldBitmap = NULL;
		
		//make sure we draw the start piece
		//on the first item so it has a left border

		//For the following items we will just use the
		//right border of the previous items as the left
		//border
		if(hditem1.iOrder==0)
		{
			pOldBitmap = bitmapDC.SelectObject(&bitmap);
			memDC.StretchBlt(rect.left,rect.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,rect.Height(),&bitmapDC,0,0,bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else
		{
			memDC.StretchBlt(rect.left-1,rect.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,rect.Height(),&bitmapDC,0,0,bmSpan.bmWidth, bmSpan.bmHeight, SRCCOPY);
			pOldBitmap = bitmapDC.SelectObject(&bitmap2);
			memDC.StretchBlt(rect.left+1,rect.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH-1,rect.Height(),&bitmapDC,0,0,bm2.bmWidth, bm2.bmHeight, SRCCOPY);
		}

		bitmapDC.SelectObject(pOldBitmap);
		
		//span the bitmap for the width of the column header item
		int nWidth = rect.Width() - 4;
		
		CBitmap* pOldBitmap2 = bitmapDC.SelectObject(&bitmap2);
		
		memDC.StretchBlt(rect.left+FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH, 0, nWidth, 1, &bitmapDC, 0,0, 1, bm2.bmHeight, SRCCOPY);

		bitmapDC.SelectObject(pOldBitmap2);
		
		
		//draw the end piece of the column header
		CBitmap* pOldBitmap3 = bitmapDC.SelectObject(&bitmap3);
		memDC.StretchBlt((rect.right-FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH), 0, FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH, rect.Height(), &bitmapDC,0,0,bm3.bmWidth, bm3.bmHeight, SRCCOPY);
		bitmapDC.SelectObject(pOldBitmap3);
		
		//Get all the info for the current
		//item so we can draw the text to it
		//in the desired font and style
		DRAWITEMSTRUCT	DrawItemStruct;
		GetItemRect(i, &rectItem);
		
		
		DrawItemStruct.CtlType		= 100;
		DrawItemStruct.hDC			= dc.GetSafeHdc();
		DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
		DrawItemStruct.hwndItem 	= GetSafeHwnd(); 
		DrawItemStruct.rcItem	= rectItem;
		DrawItemStruct.itemID	= i;
		DrawItem(&DrawItemStruct);
		
		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP |DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;
		
        m_ftDefault.CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		    OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

		CFont* def_font = memDC.SelectObject(&m_ftDefault);
		
		memDC.SetBkMode(TRANSPARENT);
		rectItem.DeflateRect(2,2,2,2);
		
		TCHAR buf[256];
		HD_ITEM hditem;
		
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( DrawItemStruct.itemID, &hditem );

		memDC.DrawText(buf, &rectItem, uFormat);
		memDC.SelectObject(def_font);
		m_ftDefault.DeleteObject();
	}		
}

BOOL CSkinHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return false;	
}

void CSkinHeaderCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	CHeaderCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CSkinHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CSkinHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    GetParent()->Invalidate(FALSE);
	CHeaderCtrl::OnLButtonUp(nFlags, point);
}
