// FishHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "..\fish.h"
#include "FishHeaderCtrl.h"
#include "fishlistctrl.h"
#include "../Dlg_RSSList.h"
#include "../ctrl/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishHeaderCtrl

#define FISH_LISTHEADER_ASCENDING   _T("▲ ")
#define FISH_LISTHEADER_DESCENDING  _T("▼ ")

CFishHeaderCtrl::CFishHeaderCtrl()
{
    m_pList =   NULL;
    m_bOnDragging =   FALSE;
    _initialize();
}

void CFishHeaderCtrl::_initialize()
{
}

CFishHeaderCtrl::~CFishHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CFishHeaderCtrl, CSkinHeaderCtrl)
	//{{AFX_MSG_MAP(CFishHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
    ON_WM_NCMOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishHeaderCtrl message handlers

void CFishHeaderCtrl::OnPaint() 
{
    if (!m_pList)    
        m_pList    =   (CFishListCtrl*)GetParent();
    {
        CPaintDC dc(this); // device context for painting
        
        CRect clientRT, rectItem, clientRect;
        GetClientRect(&clientRT);
        clientRect = clientRT;
        
        CMemDC memDC(&dc, &clientRT);
        memDC.FillSolidRect(&clientRT, ((CFishListCtrl*)GetParent())->GetBkColor() );
        
        {
            // 반복 이미지 초기화 및 드로윙
            CDC bitmapDC;
            bitmapDC.CreateCompatibleDC(&memDC);
            CBitmap bitmapSpan;
            BITMAP bmSpan;
            bitmapSpan.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN);
            bitmapSpan.GetBitmap(&bmSpan);
            CBitmap* pOldBitmapSpan = bitmapDC.SelectObject(&bitmapSpan);
            
            memDC.StretchBlt(clientRT.left+FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH, 0, clientRT.Width(), clientRT.Height(), &bitmapDC, 0, 0, bmSpan.bmWidth, bmSpan.bmHeight, SRCCOPY);
            
            bitmapDC.SelectObject(pOldBitmapSpan);
            bitmapSpan.DeleteObject();
        }

        {
            // 개별 컬럼의 항목의 초기화 및 드로윙
            CBitmap left;
            CBitmap span;
            CBitmap right;
            
            left.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_START_N);
            span.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN_N);
            right.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_END_N);
            
            for(int i = 0; i <GetItemCount(); ++i)
            {
                drawitem_Bkgnd(&memDC, i, &left, &span, &right);
            }
        }
        
        {   // Stretched Blt 의 사용으로 인한 색 변경 현상때문에 추가된 부분임. 코드상으로선 불필요
            CPen topLinePen;
            topLinePen.CreatePen(PS_SOLID, 1, FISH_HEADER_CTRL_TOP_LINE_CLR);
            CPen* pOldPen = memDC.SelectObject(&topLinePen);
            memDC.MoveTo(clientRT.left, clientRT.top);
            memDC.LineTo(clientRT.right, clientRT.top);

			memDC.SelectObject(pOldPen);
        }
    }
}

void CFishHeaderCtrl::drawitem_Bkgnd(CDC* pDC, int idx, CBitmap* left, CBitmap* span, CBitmap* right)
{
    enum {
        STR_MAX_LENGTH  =   256,
    };

    CDC bitmapDC;
    bitmapDC.CreateCompatibleDC(pDC);

    CRect itemRT;
    GetItemRect(idx, &itemRT);

    BITMAP bmLeft, bmSpan, bmRight;
    left->GetBitmap(&bmLeft);
    span->GetBitmap(&bmSpan);
    right->GetBitmap(&bmRight);

    TCHAR buf[STR_MAX_LENGTH];
    HD_ITEM hditem;
    
    hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
    hditem.pszText = buf;
    hditem.cchTextMax = STR_MAX_LENGTH - 1;
    GetItem( idx, &hditem );

    //////////////////////////////////////////////////////////////////////////
    // IMAGE DRAW
    CBitmap* pOldBitmap = NULL;
    // SPAN
    pOldBitmap = bitmapDC.SelectObject(span);
    pDC->StretchBlt(itemRT.left, 0, itemRT.Width(), itemRT.Height(), &bitmapDC, 0,0, bmSpan.bmWidth, bmSpan.bmHeight, SRCCOPY);

    // LEFT
    if(hditem.iOrder==0)    {
        bitmapDC.SelectObject(left);
        pDC->BitBlt(itemRT.left, itemRT.top, FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,itemRT.Height(),&bitmapDC,0,0,/*bmLeft.bmWidth, bmLeft.bmHeight, */SRCCOPY);
    }    else    {
        bitmapDC.SelectObject(left);
        pDC->BitBlt(itemRT.left-1, itemRT.top, FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,itemRT.Height(),&bitmapDC,0,0,/*bmSpan.bmWidth, bmSpan.bmHeight, */SRCCOPY);
    }
    
    // RIGHT
    bitmapDC.SelectObject(right);
    pDC->BitBlt(itemRT.right-FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH, 0, FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH, itemRT.Height(), &bitmapDC,0,0,/*bmRight.bmWidth, bmRight.bmHeight,*/ SRCCOPY);
    bitmapDC.SelectObject(pOldBitmap);
    // IMAGE DRAW 2
    //////////////////////////////////////////////////////////////////////////
    
    DRAWITEMSTRUCT	DrawItemStruct;
    DrawItemStruct.CtlType		= 100;
    DrawItemStruct.hDC			= pDC->GetSafeHdc();
    DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
    DrawItemStruct.hwndItem 	= GetSafeHwnd(); 
    DrawItemStruct.rcItem	    = itemRT;
    DrawItemStruct.itemID	    = idx;
    DrawItem(&DrawItemStruct);

    CRect sortItemRT  =   itemRT;
    sortItemRT.DeflateRect(0, 0, TEXT_RIGHT_PADDING, 0);
   
    UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP | DT_END_ELLIPSIS | DT_VCENTER;
    
    CFont* def_font =   NULL;
    m_ftDefault.CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    def_font = pDC->SelectObject(&m_ftDefault);
    
    pDC->SetBkMode(TRANSPARENT);
    
    // 텍스트 영역의 설정
    if (idx != CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX)    {
        itemRT.DeflateRect(7,2,2,2);
    }    else     {
        uFormat |= DT_CENTER;
        itemRT.DeflateRect(2,2,2,2);
    }
    
    pDC->DrawText(buf, &itemRT, uFormat);
    pDC->SelectObject(def_font);
    m_ftDefault.DeleteObject();
    
    if (m_pList->m_sortBy != -1 && m_pList->m_sortBy != CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX && m_pList->m_sortBy == idx)
    {
        // 만약 소팅된 상태라면 현재 상태를 그려야한다.
        m_ftDefault.CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
        def_font = pDC->SelectObject(&m_ftDefault);
        
        TCHAR sortText[10];
        if (m_pList->m_bAscendingSort == true)
        {
            CString postfix =   FISH_LISTHEADER_ASCENDING;
            if ( m_pList->GetParent()->IsKindOf( RUNTIME_CLASS( CDlg_RSSList )) )
            {
                if ( ((CDlg_RSSList*)m_pList->GetParent())->GetSortType() == CDlg_RSSList::FISH_VIEWINFO_LISTCTRL_DATE_INDEX )
                    postfix =   FISH_LISTHEADER_DESCENDING;
            }
            _tcscpy( sortText, postfix );
        }
        else 
        {
            CString postfix =   FISH_LISTHEADER_DESCENDING;
            if ( m_pList->GetParent()->IsKindOf( RUNTIME_CLASS( CDlg_RSSList )) )
            {
                if ( ((CDlg_RSSList*)m_pList->GetParent())->GetSortType() == CDlg_RSSList::FISH_VIEWINFO_LISTCTRL_DATE_INDEX )
                    postfix =   FISH_LISTHEADER_ASCENDING;
            }
            _tcscpy( sortText, postfix );
        }

        pDC->DrawText(sortText, &sortItemRT, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
        
        pDC->SelectObject(def_font);
        m_ftDefault.DeleteObject();
    }
}

    /*
    //////////////////////////////////////////////////////////////////////////
    // 소팅 상태에서 헤더의 임팩트 이미지
    CBitmap bmpSelLeft;
    CBitmap bmpSelRight;
    CBitmap bmpSelSpan;
    
      bmpSelLeft.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_START_O);
      bmpSelRight.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_END_O);
      bmpSelSpan.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN_O);
      // 왼쪽 이미지의 그리기
      if(hditem1.iOrder==0)
      {
      pOldBitmap = bitmapDC.SelectObject(&bmpSelLeft);
      memDC.StretchBlt(clientRT.left,clientRT.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,clientRT.Height(),&bitmapDC,0,0,bm.bmWidth, bm.bmHeight, SRCCOPY);
      }
      else
      {
      memDC.StretchBlt(clientRT.left-1,clientRT.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH,clientRT.Height(),&bitmapDC,0,0,bm2.bmWidth, bm2.bmHeight, SRCCOPY);
      pOldBitmap = bitmapDC.SelectObject(&bmpSelLeft);
      memDC.StretchBlt(clientRT.left+1,clientRT.top,FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH-1,clientRT.Height(),&bitmapDC,0,0,bm2.bmWidth, bm2.bmHeight, SRCCOPY);
      }
      
        bitmapDC.SelectObject(pOldBitmap);
        
          
            // 중간 반복 이미지의 그리기
            int nWidth = clientRT.Width() - 4;
            CBitmap* pOldBitmap2 = bitmapDC.SelectObject(&bmpSelSpan);
            memDC.StretchBlt(clientRT.left+FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH, 0, nWidth, clientRT.Height(), &bitmapDC, 0,0, bm2.bmWidth, bm2.bmHeight, SRCCOPY);
            bitmapDC.SelectObject(pOldBitmap2);
            
              // 오른쪽 이미지의 그리기
              CBitmap* pOldBitmap3 = bitmapDC.SelectObject(&bmpSelRight);
              memDC.StretchBlt((clientRT.right-FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH), 0, FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH, clientRT.Height(), &bitmapDC,0,0,bm3.bmWidth, bm3.bmHeight, SRCCOPY);
              bitmapDC.SelectObject(pOldBitmap3);
              
                // 소팅 상태에서 헤더의 임팩트 정보
                //////////////////////////////////////////////////////////////////////////
        */

void CFishHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    m_bOnDragging   =   TRUE;	
    if (GetCapture() == this)   ReleaseCapture();

    //////////////////////////////////////////////////////////////////////////
    // COLUMN DRAGGING DISABLING
    CRect subjectRT, iconRT;
    GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, &subjectRT);
    GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX, &iconRT);

    if ( subjectRT.PtInRect(point) || iconRT.PtInRect(point) )
    {
        HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
        if (hCursor)
            ::SetCursor(hCursor);

        return;
    }

	CSkinHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CFishHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bOnDragging   =   FALSE;

    //////////////////////////////////////////////////////////////////////////
    // SORT ACTION EXPLICITLY
    CRect subjectRT, iconRT;
    GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, &subjectRT);
    GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX, &iconRT);

	BOOL isModifyCursor = FALSE;

    if ( subjectRT.PtInRect(point) )
    {
        if (m_pList->m_sortBy == CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX)      
            m_pList->m_bAscendingSort =   !m_pList->m_bAscendingSort;       // SORT TOGGLE
        else    
            m_pList->m_bAscendingSort =   true;

        m_pList->m_sortBy        =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX;
        m_pList->GetParent()->SendMessage(WM_LISTVIEW_SORT, CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, (LPARAM)this );

		isModifyCursor = TRUE;
    }

    if ( iconRT.PtInRect(point) )
    {
        if (m_pList->m_sortBy == CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX)      
            m_pList->m_bAscendingSort =   !m_pList->m_bAscendingSort;       // SORT TOGGLE
        else    
            m_pList->m_bAscendingSort =   true;

        m_pList->m_sortBy        =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX;
        m_pList->GetParent()->SendMessage(WM_LISTVIEW_SORT, CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX, (LPARAM)this );

		isModifyCursor = TRUE;
    }

	if (isModifyCursor)
	{
        HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
        if (hCursor)
            ::SetCursor(hCursor);
	}
    
	CSkinHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CFishHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT trackmouseevent;

        CRect rc1, rc2;
        GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, &rc1);
        GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX, &rc2);
        if ( rc1.PtInRect(point) || rc2.PtInRect(point))
        {
            HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
            if (hCursor)
                ::SetCursor(hCursor);
        }   

    //////////////////////////////////////////////////////////////////////////
    // PROCESS DRAGGING ACTION
    if (!m_bOnDragging)    {        // 헤더 컨트롤 마우스 오버 강조
        CClientDC dc(this); // device context for painting

        //////////////////////////////////////////////////////////////////////////
        // BLOCKING APP TO CHANGE MOUSE CURSOR
        CRect clientRT;
        GetClientRect(&clientRT);
        
        CMemDC memDC(&dc, &clientRT);
        
        CRect itemRT;
        CBitmap leftN, spanN, rightN;
        CBitmap leftO, spanO, rightO;
        leftN.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_START_N);
        spanN.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN_N);
        rightN.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_END_N);
        
        leftO.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_START_O);
        spanO.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_SPAN_O);
        rightO.LoadBitmap(IDB_LISTCTRL_COLUMNHEADER_END_O);
        
        BITMAP bm;
        CDC buffDC;
        buffDC.CreateCompatibleDC(&memDC);
        CBitmap *pOldBitmap = buffDC.SelectObject(&spanN);
        spanN.GetBitmap(&bm);
        memDC.StretchBlt(0, 0, clientRT.Width(), clientRT.Height(), &buffDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        int redrawidx = -1;
        for (int idx = 0; idx < GetItemCount(); idx++)
        {
            drawitem_Bkgnd(&memDC, idx, &leftN, &spanN, &rightN);     // 이전의 영역을 다시 그린다.
            GetItemRect(idx, &itemRT);
            itemRT.DeflateRect(2, 0, 2, 0);
            if ( itemRT.PtInRect(point) )   {
                redrawidx   =   idx;
            }
        }
        if (redrawidx != -1)
            drawitem_Bkgnd(&memDC, redrawidx, &leftO, &spanO, &rightO);     // 이전의 영역을 다시 그린다.

		memDC.SelectObject(pOldBitmap);
    }

	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0;
    ::_TrackMouseEvent(&trackmouseevent);
    CSkinHeaderCtrl::OnMouseMove(nFlags, point);
}

void CFishHeaderCtrl::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CSkinHeaderCtrl::OnCaptureChanged(pWnd);
}

afx_msg LONG CFishHeaderCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    Invalidate(FALSE);
 	return 0;
}

LRESULT CFishHeaderCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CSkinHeaderCtrl::WindowProc(message, wParam, lParam);
}

UINT CFishHeaderCtrl::OnNcHitTest(CPoint point)
{
    //////////////////////////////////////////////////////////////////////////
    // NonClient HitTest
        CRect rc1, rc2;
		CPoint pt = point;
		ScreenToClient(&pt);
        GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, &rc1);
        GetItemRect(CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX, &rc2);
        if ( rc1.PtInRect(pt) || rc2.PtInRect(pt))
        {
            HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
            if (hCursor)
                ::SetCursor(hCursor);
        }   
    
    return CHeaderCtrl::OnNcHitTest(point);
}

void CFishHeaderCtrl::OnNcMouseMove(UINT nHitTest, CPoint point)
{
    //////////////////////////////////////////////////////////////////////////
    // NonClient MouseMove Action
    CHeaderCtrl::OnNcMouseMove(nHitTest, point);
}