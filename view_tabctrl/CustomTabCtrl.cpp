/**********************************************************************
**
**	CustomTabCtrl.cpp : implementation file of CFishTabCtrl class
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#include "stdafx.h"
#include "CustomTabCtrl.h"
#include "../GlobalIconManager.h"
#include "../resource.h"
#include "../fish_def.h"
#include "../FocusManager.h"

// CFishTabCtrlItem
CRect                       m_rectItemIconRegion;
CRect                       m_rectItemFunctRegin;

CFishTabCtrlItem::CFishTabCtrlItem(CString sText,LPARAM lParam) : 
			m_sText(sText), 
			m_lParam(lParam),	
			m_bShape(TAB_SHAPE1),
			m_fSelected(FALSE),
			m_fHighlighted(FALSE),
			m_fHighlightChanged(FALSE),
            m_bMouseOver(FALSE)
{
}

void CFishTabCtrlItem::operator=(const CFishTabCtrlItem &other)
{
	m_sText = other.m_sText;
	m_lParam = other.m_lParam;
}

void CFishTabCtrlItem::ComputeRgn()
{
	m_rgn.DeleteObject();

	CPoint pts[6];
	GetRegionPoints(m_rect, pts);
	m_rgn.CreatePolygonRgn(pts, 6, WINDING);
}

/************************************************************************
GetRegionPoints
@PARAM  : 
@RETURN : 
@REMARK : 
    실제로 그림을 그리는 역영을 얻고 클릭의 기준이 되는 영역을 얻는다.
    FISH 에 최적화된 탭의 모양을 직접 드로윙 ^^;;
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/10:CREATED
************************************************************************/
void CFishTabCtrlItem::GetRegionPoints(const CRect& rc, CPoint* pts) const
{
	switch(m_bShape)
	{
	case TAB_SHAPE2:
		{
			pts[0] = CPoint(rc.left, rc.top);
			pts[1] = CPoint(rc.left, rc.bottom);
            /*
			pts[2] = CPoint(rc.left + rc.Height()/2, rc.top);
			pts[3] = CPoint(rc.left + rc.Height()/2, rc.top);
			pts[4] = CPoint(rc.left + rc.Height()/2, rc.top);
			pts[5] = CPoint(rc.left + rc.Height()/2, rc.top);
            */
			pts[2] = CPoint(rc.left, rc.bottom);
			pts[3] = CPoint(rc.left, rc.top);
			pts[4] = CPoint(rc.left, rc.top);
			pts[5] = CPoint(rc.left, rc.top);
		}
		break;
	case TAB_SHAPE3:
		{
            /*
			pts[0] = rc.TopLeft();
			pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
			pts[2] = CPoint(rc.left, rc.bottom);
			pts[3] = CPoint(rc.left, rc.bottom);
			pts[4] = CPoint(rc.left, rc.bottom);
			pts[5] = CPoint(rc.left, rc.bottom);
            */
			pts[0] = CPoint(rc.left, rc.top);
			pts[1] = CPoint(rc.left, rc.top);
			pts[2] = CPoint(rc.left, rc.bottom);
			pts[3] = CPoint(rc.left, rc.bottom);
			pts[4] = CPoint(rc.left, rc.bottom);
			pts[5] = CPoint(rc.left, rc.bottom);
		}
		break;
	case TAB_SHAPE4:
		{
            /*
			pts[0] = rc.TopLeft();
			pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
			pts[2] = CPoint(rc.left + rc.Height()/2, rc.bottom);
			pts[3] = CPoint(rc.right - rc.Height()/2, rc.bottom);
			pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
			pts[5] = CPoint(rc.right, rc.top);
            */
			pts[0] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.top);
			pts[1] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
			pts[2] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
			pts[3] = CPoint(rc.right, rc.bottom);
			pts[4] = CPoint(rc.right, rc.top);
			pts[5] = CPoint(rc.right, rc.top);
		}
		break;
	case TAB_SHAPE5:
		{
            /*
			pts[0] = rc.TopLeft();
			pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
			pts[2] = CPoint(rc.left + rc.Height()/2 , rc.bottom);
			pts[3] = CPoint(rc.right - rc.Height()/2, rc.bottom);
			pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
			pts[5] = CPoint(rc.right - rc.Height()/2, rc.top);
            */
			pts[0] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.top);
			pts[1] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
			pts[2] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
			pts[3] = CPoint(rc.right, rc.bottom);
			pts[4] = CPoint(rc.right, rc.top);
			pts[5] = CPoint(rc.right, rc.top);
        }
		break;
	default:
		{
			pts[0] = CPoint(0,0);
			pts[1] = CPoint(0,0);
			pts[2] = CPoint(0,0);
			pts[3] = CPoint(0,0);
			pts[4] = CPoint(0,0);
			pts[5] = CPoint(0,0);
		}
		break;
	}
}

void CFishTabCtrlItem::GetDrawPoints(const CRect& rc, CPoint* pts) const
{
	switch(m_bShape)
	{
	case TAB_SHAPE2:
	case TAB_SHAPE3:
		{
			pts[0] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.top);
			pts[1] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
		}
		break;
	case TAB_SHAPE4:
	case TAB_SHAPE5:
		{
			pts[0] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.bottom);
			pts[1] = CPoint(rc.left + FISH_TAB_ITEM_INTERVAL, rc.top);
			pts[2] = CPoint(rc.right, rc.top);
			pts[3] = CPoint(rc.right, rc.bottom);
		}
		break;
	}

}

void CFishTabCtrlItem::Draw(CDC& dc, CFont& font)
{
    enum {
        FISH_TAB_BKGND_LEFT_WIDTH   =   2,
        FISH_TAB_BKGND_LEFT_HEIGHT  =   21,
        FISH_TAB_BKGND_RIGHT_WIDTH  =   2,
        FISH_TAB_BKGND_RIGHT_HEIGHT =   21,
        FISH_TAB_BKGND_SPAN_WIDTH   =   2,
        FISH_TAB_BKGND_SPAN_HEIGHT  =   21,

    };

    //////////////////////////////////////////////////////////////////////////
    // 배경 그리기
    CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
    CBitmap* bmpLeft    =   NULL;
    CBitmap* bmpSpan    =   NULL;
    CBitmap* bmpRight   =   NULL;
    if (m_fSelected || m_fHighlighted)
    {
        bmpLeft    =   CFishBMPManager::getInstance()->getTabBkgnd(true, 0);
        bmpSpan    =   CFishBMPManager::getInstance()->getTabBkgnd(true, 1);
        bmpRight   =   CFishBMPManager::getInstance()->getTabBkgnd(true, 2);
    }
    else
    {
        bmpLeft    =   CFishBMPManager::getInstance()->getTabBkgnd(false, 0);
        bmpSpan    =   CFishBMPManager::getInstance()->getTabBkgnd(false, 1);
        bmpRight   =   CFishBMPManager::getInstance()->getTabBkgnd(false, 2);
    }

    CPoint pts[4];
    CRect rc            =   m_rect;
	GetDrawPoints(rc, pts);

    CRect itemRT    =   CRect(m_rect.left + FISH_TAB_ITEM_INTERVAL, m_rect.top, m_rect.right, m_rect.bottom );
    CRect leftRT    =   CRect(m_rect.left + FISH_TAB_ITEM_INTERVAL, m_rect.top, m_rect.left+FISH_TAB_ITEM_INTERVAL+ FISH_TAB_BKGND_LEFT_WIDTH, m_rect.top + FISH_TAB_BKGND_LEFT_HEIGHT);
    CRect rightRT   =   CRect(m_rect.right - FISH_TAB_BKGND_RIGHT_WIDTH, m_rect.top, m_rect.right, m_rect.top + FISH_TAB_BKGND_LEFT_HEIGHT);

    CPen outlinePen;
    outlinePen.CreatePen(PS_SOLID, 1, FISH_TAB_OUTLINE_COLOR);
    CPen* oldPen    =   dc.SelectObject(&outlinePen);
    dc.MoveTo(m_rect.left, m_rect.bottom);
    dc.LineTo(m_rect.right, m_rect.bottom);
    dc.SelectObject(oldPen);

//    BMPManager->drawTiledBlt(&dc, bmpSpan, itemRT);
    CDC memDC;
    BITMAP bm;
    memDC.CreateCompatibleDC(&dc);
    memDC.SelectObject(bmpSpan);
    bmpSpan->GetBitmap(&bm);
    dc.StretchBlt(itemRT.left, itemRT.top, itemRT.Width(), itemRT.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );

    BMPManager->drawTransparent(&dc, bmpLeft, leftRT.TopLeft(), FALSE);
    BMPManager->drawTransparent(&dc, bmpRight, rightRT.TopLeft(), FALSE);
    
    //////////////////////////////////////////////////////////////////////////
    // 아이템의 텍스트 부분 그리기, 텍스트의 영역을 이곳에서 처리한다.
    dc.SetBkMode(TRANSPARENT);
    rc.DeflateRect(FISH_TAB_INNER_PAD_LEFT, FISH_TAB_INNER_PAD_TOP+2, FISH_TAB_INNER_PAD_RIGHT, FISH_TAB_INNER_PAD_BOTTOM);
    CFont* pOldFont = dc.SelectObject(&font);
    dc.DrawText(m_sText, &m_rectText, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    dc.SelectObject(pOldFont);
    
    //////////////////////////////////////////////////////////////////////////
    // 닫기 버튼 그리기
    enum {
        CLOSE_BTN_N =   2,
        CLOSE_BTN_O =   1,
    };
    
    IMAGEINFO info;
    CFishBMPManager::getInstance()->getTabIcons()->GetImageInfo(1, &info);
    CPoint beg = pts[2];
    beg.x -= (info.rcImage.right - info.rcImage.left);
    beg.y += 2;
    if (m_bMouseOver)
        CFishBMPManager::drawTransparent(&dc, CFishBMPManager::getInstance()->getTabIcons(), CLOSE_BTN_O, beg, FALSE, CPoint(0, 0), RGB(0xC6, 0xFF, 0x00));
    else
        CFishBMPManager::drawTransparent(&dc, CFishBMPManager::getInstance()->getTabIcons(), CLOSE_BTN_N, beg, FALSE, CPoint(0, 0), RGB(0xC6, 0xFF, 0x00));
}

// CFishTabCtrl

LOGFONT CFishTabCtrl::lf_default = {14, 0, 0, 0, FW_NORMAL, 0, 0, 0,
			ANSI_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DEFAULT_APPLICATION_FONT };

BYTE CFishTabCtrl::m_bBitsGlyphs[] = {0xBD,0xFB,0xDF,0xBD,
										0xB9,0xF3,0xCF,0x9D,
										0xB1,0xE3,0xC7,0x8D,
										0xA3,0xC7,0xE3,0xC5,
										0xB1,0xE3,0xC7,0x8D,
										0xB9,0xF3,0xCF,0x9D,
										0xBD,0xFB,0xDF,0xBD};

CFishTabCtrl::CFishTabCtrl() :
			m_nButtonIDDown(CTCID_NOBUTTON),
			m_nPrevState(BNST_INVISIBLE),
			m_nNextState(BNST_INVISIBLE),
			m_nFirstState(BNST_INVISIBLE),
			m_nLastState(BNST_INVISIBLE),
			m_nItemSelected(-1),
			m_nItemNdxOffset(0),
			m_dwLastRepeatTime(0),
			m_hBmpBkLeftSpin(NULL),
			m_hBmpBkRightSpin(NULL),
			m_hCursorMove(NULL),
			m_hCursorCopy(NULL),
			m_nItemDragDest(0)
{
    // 차후 탭 아이콘이 들어간다면 이 부분에서 수정을 하면 된다.
    ::m_rectItemIconRegion.SetRect(0, 0, 0, 0);       // 아이템의 아이콘 영역
    ::m_rectItemFunctRegin.SetRect(0, 0, 16, 16);       // 아이템의 기능 버튼 영역 (현재는 CLOSE 버튼으로 이용한다.)


	RegisterWindowClass();
	SetControlFont(GetDefaultFont());
	m_bmpGlyphsMono.CreateBitmap(32,7,1,1,m_bBitsGlyphs);
}

// Register the window class if it has not already been registered.

BOOL CFishTabCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CustomTabCtrl_CLASSNAME, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CustomTabCtrl_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

CFishTabCtrl::~CFishTabCtrl()
{
	for(int i=0; i< m_aItems.GetSize(); i++)
		delete m_aItems[i];
	m_aItems.RemoveAll();

	::DeleteObject(m_hBmpBkLeftSpin);
	m_hBmpBkLeftSpin = NULL;
	::DeleteObject(m_hBmpBkRightSpin);
	m_hBmpBkRightSpin = NULL;
	::DestroyCursor(m_hCursorMove);
	m_hCursorMove = NULL;
	::DestroyCursor(m_hCursorCopy);
	m_hCursorCopy = NULL;
}

BEGIN_MESSAGE_MAP(CFishTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CFishTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(THM_WM_THEMECHANGED,OnThemeChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_EN_UPDATE(CTCID_EDITCTRL, OnUpdateEdit)
	//}}AFX_MSG_MAP
//	ON_WM_NCRBUTTONDOWN()
ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CFishTabCtrl message handlers

BOOL CFishTabCtrl::Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID)
{
	return CWnd::Create(CustomTabCtrl_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CFishTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CFishTabCtrl::OnPaint()
{
	CPaintDC dc(this);

	if(!m_hBmpBkLeftSpin)
	{
		m_rgbGlyph[0] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[1] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[2] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[3] = GetSysColor(COLOR_BTNTEXT);
	}

	CRect rCl;
	GetClientRect(&rCl);

	CPen blackPen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));

	CDC dcMem;
	CBitmap bmpMem;
	CBitmap* pOldBmp=NULL;

	if(!dcMem.CreateCompatibleDC(&dc)) return;

	if(!bmpMem.CreateCompatibleBitmap(&dc,rCl.Width(),rCl.Height())) return;

	pOldBmp = dcMem.SelectObject(&bmpMem);

	int nBtns = 2;
	if(GetStyle()&CTCS_FOURBUTTONS)
		nBtns = 4;

	{
		// clear background
		dcMem.FillSolidRect(&rCl, FISH_TAB_BKGND_COLOR);
	}

    {   // UNDERLINE CREATION
        if ( GetCurSel() != -1 )
        {
            CPen outlineClr, *pOldPen;
            outlineClr.CreatePen(PS_SOLID, 1, FISH_TAB_OUTLINE_COLOR);
            pOldPen =   dcMem.SelectObject(&outlineClr);
            dcMem.MoveTo(rCl.left, rCl.bottom-1);
            dcMem.LineTo(rCl.right, rCl.bottom-1);
            dcMem.SelectObject(&pOldPen);
        }
    }

	{
		// draw tab items visible and not selected
		
		for(int i= 0; i<m_aItems.GetSize(); i++)
		{
			if(m_aItems[i]->m_bShape && !m_aItems[i]->m_fSelected)
			{
				if(m_aItems[i]->m_fHighlighted)
					m_aItems[i]->Draw(dcMem,m_FontSelected);
				else
					m_aItems[i]->Draw(dcMem,m_Font);
			}
		}

		// draw selected tab item
		
		if(m_nItemSelected!=-1 && m_aItems[m_nItemSelected]->m_bShape)
			m_aItems[m_nItemSelected]->Draw(dcMem,m_FontSelected);

	}

	{
		// draw buttons
		if(m_nPrevState && m_nNextState)
		{
			int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;
			CRect rAll(rCl.Width() - nBtns*nA -2, 0, rCl.Width() ,rCl.Height());
			CRect rFirst,rPrev,rNext,rLast;

			if(nBtns==2)
			{
				rPrev.SetRect(rCl.Width() - rAll.Width() + 1, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
				rNext.SetRect(rCl.Width() - rAll.Width() + nA+2, 0, rCl.Width() - rAll.Width() + 2*nA+2, rCl.Height());
			}
			else
			{
				rFirst.SetRect(rCl.Width() - rAll.Width() + 1, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
				rPrev.SetRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() + 2*nA+1, rCl.Height());
				rNext.SetRect(rCl.Width() - rAll.Width() + 2*nA+2, 0, rCl.Width() - rAll.Width() + 3*nA+2, rCl.Height());
				rLast.SetRect(rCl.Width() - rAll.Width() + 3*nA+2, 0, rCl.Width() - rAll.Width() + 4*nA+2, rCl.Height());
			}

			if(nBtns==4)
			{
				CPoint ptFirst(rFirst.left+(rFirst.Width()-8)/2,rFirst.top+(rFirst.Height()-7)/2);
				if(m_hBmpBkLeftSpin)
					DrawBkLeftSpin(dcMem,rFirst,m_nFirstState-1);
				else
				{
					if(m_nFirstState==BNST_PRESSED)
						dcMem.DrawFrameControl(rFirst,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
					else
						dcMem.DrawFrameControl(rFirst,DFC_BUTTON,DFCS_BUTTONPUSH);
				}
				DrawGlyph(dcMem,ptFirst,0,m_nFirstState-1);
				
				CPoint ptLast(rLast.left+(rLast.Width()-8)/2,rLast.top+(rLast.Height()-7)/2);
				if(m_hBmpBkRightSpin)
					DrawBkRightSpin(dcMem,rLast,m_nLastState-1);
				else
				{
					if(m_nLastState==BNST_PRESSED)
						dcMem.DrawFrameControl(rLast,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
					else
						dcMem.DrawFrameControl(rLast,DFC_BUTTON,DFCS_BUTTONPUSH);
				}
				DrawGlyph(dcMem,ptLast,3,m_nLastState-1);
			}

            // 이전 으로 버튼과 다음으로 버튼 그리기
            //////////////////////////////////////////////////////////////////////////
            // PREPARATION
            dcMem.FillRect( CRect(rPrev.TopLeft(), rNext.BottomRight()), &CBrush(FISH_TAB_BKGND_COLOR) );

            CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
			CPoint ptPrev(rPrev.TopLeft());
            BMPManager->drawTransparent(&dcMem, BMPManager->GetTabBtnsBmp(), ptPrev, TRUE, CPoint(0, 0));
            /*
			CPoint ptPrev(rPrev.left+(rPrev.Width()-8)/2,rPrev.top+(rPrev.Height()-7)/2);
            if(m_hBmpBkLeftSpin)
				DrawBkLeftSpin(dcMem,rPrev,m_nPrevState-1);
			else
			{
				if(m_nPrevState==BNST_PRESSED)
					dcMem.DrawFrameControl(rPrev,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
				else
					dcMem.DrawFrameControl(rPrev,DFC_BUTTON,DFCS_BUTTONPUSH);
			}
			DrawGlyph(dcMem,ptPrev,1,m_nPrevState-1);
            */

			CPoint ptNext(rNext.TopLeft());
            BMPManager->drawTransparent(&dcMem, BMPManager->GetTabBtnsBmp(1), ptNext, TRUE, CPoint(0, 0));

            /*
			CPoint ptNext(rNext.left+(rNext.Width()-8)/2,rNext.top+(rNext.Height()-7)/2);
			if(m_hBmpBkRightSpin)
				DrawBkRightSpin(dcMem,rNext,m_nNextState-1);
			else
			{
				rNext.left -= 1;
				if(m_nNextState==BNST_PRESSED)
					dcMem.DrawFrameControl(rNext,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
				else
					dcMem.DrawFrameControl(rNext,DFC_BUTTON,DFCS_BUTTONPUSH);
			}
			DrawGlyph(dcMem,ptNext,2,m_nNextState-1);
            */
            CPen* pOldPen   =   dcMem.SelectObject(&blackPen);
            dcMem.MoveTo( CPoint(rPrev.left, rPrev.bottom) );
            dcMem.LineTo( CPoint(rNext.right, rNext.bottom) );
            dcMem.SelectObject(pOldPen);
            //////////////////////////////////////////////////////////////////////////
            // 이전 으로 버튼과 다음으로 버튼 그리기
        }
	}
/*
	{
		//////////////////////////////////////////////////////////////////////////
		// 탭 컨트롤과 CONTENTS 사이에 구분선을 그린다.
		int nOffsetX = 0;
		if(m_nPrevState && m_nNextState)
			nOffsetX = nBtns*FUNCTION_BTN_WIDTH+5;//nBtns*(rCl.Height()-3) + 5;

		CPoint pts[4];
		if(m_nItemSelected==-1)
		{
			pts[0] = CPoint(rCl.left, rCl.Height()-1); 
			pts[1] = CPoint(rCl.left, rCl.Height()-1); 
			pts[2] = CPoint(rCl.left, rCl.Height()-1); 
			pts[3] = CPoint(rCl.right - nOffsetX, rCl.Height()-1);
		}
		else
		{
			if(m_aItems[m_nItemSelected]->m_bShape)
			{
				pts[0] = CPoint(rCl.left, rCl.Height()-1); 
				pts[1] = CPoint(m_aItems[m_nItemSelected]->m_rect.left, rCl.Height()-1); 
				pts[2] = CPoint(m_aItems[m_nItemSelected]->m_rect.right, rCl.Height()-1); 
				pts[3] = CPoint(rCl.right - nOffsetX, rCl.Height()-1);
			}
			else
			{
				pts[0] = CPoint(rCl.left, rCl.Height()-1); 
				pts[1] = CPoint(rCl.left, rCl.Height()-1); 
				pts[2] = CPoint(rCl.left, rCl.Height()-1); 
				pts[3] = CPoint(rCl.right - nOffsetX, rCl.Height()-1);
			}
		}
		
		CPen* pOldPen = dcMem.SelectObject(&blackPen);
		dcMem.MoveTo(pts[0]);
		dcMem.LineTo(pts[1]);
		dcMem.MoveTo(pts[2]);
		dcMem.LineTo(pts[3]);
		dcMem.SelectObject(pOldPen);
	
	}
*/
    {
		if(m_nButtonIDDown>=0 && 
			(GetCursor()==m_hCursorMove || GetCursor()==m_hCursorCopy))
		{
			// Draw drag destination marker
			CPen* pOldPen = dcMem.SelectObject(&blackPen);
			int x;
			if(m_nItemDragDest==m_aItems.GetSize())
				x= m_aItems[m_nItemDragDest-1]->m_rectText.right + rCl.Height()/4-3;
			else
                if (m_aItems.GetSize() != 0)    x= m_aItems[m_nItemDragDest]->m_rectText.left - rCl.Height()/4-3;
			if(x>=rCl.right-7)
				x = rCl.right-7;
			dcMem.MoveTo(x,1);
			dcMem.LineTo(x+7,1);
			dcMem.MoveTo(x+1,2);
			dcMem.LineTo(x+6,2);
			dcMem.MoveTo(x+2,3);
			dcMem.LineTo(x+5,3);
			dcMem.MoveTo(x+3,4);
			dcMem.LineTo(x+4,4);
		}
	}
	dc.BitBlt(rCl.left,
                   rCl.top,
                   rCl.Width(),
                   rCl.Height(),
                   &dcMem,
                   rCl.left,
                   rCl.top,
                   SRCCOPY);

	dcMem.SelectObject(pOldBmp);
}

void CFishTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(cx && cy)
		RecalcLayout(RECALC_RESIZED, m_nItemSelected);
}

void CFishTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nExData;
	int nHitTest = HitTest(point, true, nExData);
	ProcessLButtonDown(nHitTest,nFlags,point,nExData);
//	NotifyParent(CTCN_CLICK,nHitTest,point);
	CWnd::OnLButtonDown(nFlags,point);
}

void CFishTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nExData;
	int nHTRet = ProcessLButtonDown(HitTest(point, false, nExData),nFlags,point);
	if(nHTRet>=0)
	{
		m_nButtonIDDown = CTCID_NOBUTTON;
		// ?? 아래 조건절은 무의미하다.
//		if(nHTRet==HitTest(point))
		EditLabel(nHTRet,TRUE);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

int CFishTabCtrl::ProcessLButtonUp(int nHitTest, UINT nFlags, CPoint point)
{
/*	switch(nHitTest)
	{
	case CTCHT_NOWHERE:
	default:
		{
			if((GetStyle()&CTCS_MULTIHIGHLIGHT) && (nFlags&MK_CONTROL))
				HighlightItem(nHitTest,TRUE,nFlags&MK_CONTROL);
			else
			{
				BOOL bNotify = nHitTest!=m_nItemSelected;
				SetCurSel(nHitTest,TRUE,nFlags&MK_CONTROL);
				if(bNotify)
					NotifyParent(CTCN_SELCHANGE,m_nItemSelected,point);
			}
			for(int i=0; i<m_aItems.GetSize();i++)
			{
				if(m_aItems[i]->m_fHighlightChanged)
					NotifyParent(CTCN_HIGHLIGHTCHANGE,i,point);
			}
		}
	}
*/
	return nHitTest;
}

int CFishTabCtrl::ProcessLButtonDown(int nHitTest, UINT nFlags, CPoint point, int nExData)
{
	SetCapture();
	switch(nHitTest)
	{
	case CTCHT_NOWHERE:
		m_nButtonIDDown = CTCID_NOBUTTON;
		break;
	case CTCHT_ONFIRSTBUTTON:
		{
			m_nButtonIDDown = CTCID_FIRSTBUTTON;
			m_nFirstState = BNST_PRESSED;
			RecalcLayout(RECALC_FIRST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONPREVBUTTON:
		{
			m_nButtonIDDown = CTCID_PREVBUTTON;
			m_nPrevState = BNST_PRESSED;
			RecalcLayout(RECALC_PREV_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONNEXTBUTTON:
		{
			m_nButtonIDDown = CTCID_NEXTBUTTON;
			m_nNextState = BNST_PRESSED;
			RecalcLayout(RECALC_NEXT_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONLASTBUTTON:
		{
			m_nButtonIDDown = CTCID_LASTBUTTON;
			m_nLastState = BNST_PRESSED;
			RecalcLayout(RECALC_LAST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	default:
		{
			DWORD dwStyle = GetStyle();
			if(nExData == CTCN_HITCLOSEREGION)
			{
				m_nButtonIDDown = nHitTest;
			}
			else
			{
				if(((dwStyle&CTCS_DRAGMOVE) && !(nFlags&MK_CONTROL) && m_hCursorMove) || 
					((dwStyle&CTCS_DRAGCOPY) && (nFlags&MK_CONTROL) && m_hCursorCopy))
				{
					m_nButtonIDDown = nHitTest;
					m_nItemDragDest = nHitTest;
					SetTimer(2,300,NULL);
				}
				else
					m_nButtonIDDown = CTCID_NOBUTTON;

				if((GetStyle()&CTCS_MULTIHIGHLIGHT) && (nFlags&MK_CONTROL))
					HighlightItem(nHitTest,TRUE,nFlags&MK_CONTROL);
				else
				{
					BOOL bNotify = nHitTest!=m_nItemSelected;
					SetCurSel(nHitTest,TRUE,nFlags&MK_CONTROL);
					if(bNotify)
						NotifyParent(CTCN_SELCHANGE,m_nItemSelected,point);
				}
				for(int i=0; i<m_aItems.GetSize();i++)
				{
					if(m_aItems[i]->m_fHighlightChanged)
						NotifyParent(CTCN_HIGHLIGHTCHANGE,i,point);
				}
			}

		}
		break;
	}
	return nHitTest;
}

BOOL CFishTabCtrl::NotifyParent(UINT code, int nItem, CPoint pt)
{
	CTC_NMHDR nmh;
	memset(&nmh,0,sizeof(CTC_NMHDR));
	nmh.hdr.hwndFrom = GetSafeHwnd();
	nmh.hdr.idFrom = GetDlgCtrlID();
	nmh.hdr.code = code;
	nmh.nItem = nItem;
	nmh.ptHitTest = pt;

	if(nItem>=0)
	{
        if (m_aItems.GetSize() == 0) return FALSE;
		_tcscpy(nmh.pszText, m_aItems[nItem]->m_sText.GetBuffer(MAX_LABEL_TEXT-1));
		nmh.lParam = m_aItems[nItem]->m_lParam;
		nmh.rItem = m_aItems[nItem]->m_rectText;
		nmh.fSelected = m_aItems[nItem]->m_fSelected;
		nmh.fHighlighted = m_aItems[nItem]->m_fHighlighted;
	}
	return (BOOL)GetParent()->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&nmh);
}

void CFishTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    //////////////////////////////////////////////////////////////////////////
    // 아이템의 삭제 메시지는 마우스 업시에 체크한다.
	int nExData;
	int nHitTest = HitTest(point, true, nExData);
	if(m_nButtonIDDown == nHitTest)
	{
		if(nExData == CTCN_HITCLOSEREGION)
		{
			NotifyParent(CTCN_HITCLOSEREGION, nHitTest, point);
		}
		else
		{
//			ProcessLButtonUp(nHitTest,nFlags,point);
			NotifyParent(CTCN_CLICK,nHitTest,point);
		}
	}
	

    //////////////////////////////////////////////////////////////////////////
    // 원래 코드 부분
	if(m_nPrevState || m_nNextState || m_nFirstState || m_nLastState)
	{
		m_nPrevState = BNST_NORMAL;
		m_nNextState = BNST_NORMAL;
		m_nFirstState = BNST_NORMAL;
		m_nLastState = BNST_NORMAL;
		Invalidate(FALSE);
		KillTimer(1);
	}
	if(m_nButtonIDDown>=0)
	{
		if((GetCursor()==m_hCursorCopy) && (GetKeyState(VK_CONTROL)&0x8000))
			CopyItem(m_nButtonIDDown,m_nItemDragDest, TRUE);
		else if((GetCursor()==m_hCursorMove) && !(GetKeyState(VK_CONTROL)&0x8000))
			MoveItem(m_nButtonIDDown,m_nItemDragDest, TRUE);
	}
	m_nButtonIDDown = CTCID_NOBUTTON;
	ReleaseCapture();
}

void CFishTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT trackmouseevent;
	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0;
	_TrackMouseEvent(&trackmouseevent);

	CRect rCl;
	GetClientRect(&rCl);

	int nBtns = 2;
	if(GetStyle()&CTCS_FOURBUTTONS)
		nBtns = 4;
	int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;
    CRect rAll(rCl.Width() - nBtns*nA -2 , 1, rCl.Width() ,rCl.Height());

	CRect rFirst,rPrev,rNext,rLast;

    {
        //////////////////////////////////////////////////////////////////////////
        // 마우스 오버시 처리해야하는 사항
        enum {
            CLOSE_BTN_N =   2,
            CLOSE_BTN_O =   1,
        };
        CClientDC dc(this);

        int idx =   -1;
        CRect rc;
        CPoint pts[4];
        CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
	    for(int i=0; i<m_aItems.GetSize(); i++)
	    {
            int sentinel = m_aItems[i]->HitTest(point);
            if (sentinel == HITTEST_RET_CLOSE)  {
                m_aItems[i]->m_bMouseOver   =   TRUE;
            }   else    {
                m_aItems[i]->m_bMouseOver   =   FALSE;
            }
	    }
    }

	if(nBtns==2)
	{
		rPrev.SetRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
		rNext.SetRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() +  2*nA+3, rCl.Height());
	}	else	{
		rFirst.SetRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
		rPrev.SetRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() + 2*nA+1, rCl.Height());
		rNext.SetRect(rCl.Width() - rAll.Width() + 2*nA+1, 0, rCl.Width() - rAll.Width() + 3*nA+2, rCl.Height());
		rLast.SetRect(rCl.Width() - rAll.Width() + 3*nA+2, 0, rCl.Width() - rAll.Width() + 4*nA+3, rCl.Height());
	}

	if(nBtns==4 && m_nFirstState)
	{
		if(rFirst.PtInRect(point))
		{
			if(m_nButtonIDDown==CTCID_FIRSTBUTTON)
				m_nFirstState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nFirstState = BNST_HOT;
			else
				m_nFirstState = BNST_NORMAL;
		}
		else
			m_nFirstState = BNST_NORMAL;
	}
	if(m_nPrevState)
	{
		if(rPrev.PtInRect(point))
		{
			if(m_nButtonIDDown==CTCID_PREVBUTTON)
				m_nPrevState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nPrevState = BNST_HOT;
			else
				m_nPrevState = BNST_NORMAL;
		}
		else
			m_nPrevState = BNST_NORMAL;
	}
	if(m_nNextState)
	{
		if(rNext.PtInRect(point))
		{
			if(m_nButtonIDDown==CTCID_NEXTBUTTON)
				m_nNextState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nNextState = BNST_HOT;
			else
				m_nNextState = BNST_NORMAL;
		}
		else
			m_nNextState = BNST_NORMAL;
	}
	if(nBtns==4 && m_nLastState)
	{
		if(rLast.PtInRect(point))
		{
			if(m_nButtonIDDown==CTCID_LASTBUTTON)
				m_nLastState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nLastState = BNST_HOT;
			else
				m_nLastState = BNST_NORMAL;
		}
		else
			m_nLastState = BNST_NORMAL;
	}
	if(m_nButtonIDDown>=0)
	{
		if(m_nItemDragDest>=m_aItems.GetSize())
			m_nItemDragDest = m_aItems.GetSize()-1;
        
        if ( m_nItemDragDest == -1 )    return;

		int x1 = m_aItems[m_nItemDragDest]->m_rectText.left - rCl.Height()/4;
		int x2 = m_aItems[m_nItemDragDest]->m_rectText.right + rCl.Height()/4;
		if(point.x>=rCl.right)
		{
			m_nItemDragDest++;
			if(m_nItemDragDest>=m_aItems.GetSize())
				RecalcLayout(RECALC_NEXT_PRESSED,m_aItems.GetSize()-1);
			else
				RecalcLayout(RECALC_NEXT_PRESSED,m_nItemDragDest);
		}
		else if(point.x>=x2)
		{
			m_nItemDragDest++;
			if(m_nItemDragDest>=m_aItems.GetSize())
				RecalcLayout(RECALC_ITEM_SELECTED,m_aItems.GetSize()-1);
			else
				RecalcLayout(RECALC_ITEM_SELECTED,m_nItemDragDest);
		}
		else if(point.x<x1)
		{
			if(m_nItemDragDest>0)
				m_nItemDragDest--;
			
			RecalcLayout(RECALC_ITEM_SELECTED,m_nItemDragDest);
		}
	}
    Invalidate(FALSE);
}

LONG CFishTabCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	if(m_nPrevState || m_nNextState)
	{
		m_nPrevState = BNST_NORMAL;
		m_nNextState = BNST_NORMAL;
		m_nFirstState = BNST_NORMAL;
		m_nLastState = BNST_NORMAL;
		Invalidate(FALSE);
		KillTimer(1);
	}

    {
        //////////////////////////////////////////////////////////////////////////
        // 마우스 오버에서 밖으로 나갈때 해야할 일
        enum {
            CLOSE_BTN_N =   2,
            CLOSE_BTN_O =   1,
        };
        CClientDC dc(this);

        int idx =   -1;
        IMAGEINFO info;
        CRect rc;
        CPoint pts[4];
        CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
	    for(int i=0; i<m_aItems.GetSize(); i++)
	    {
            rc    =   m_aItems[i]->m_rect;
            m_aItems[i]->GetDrawPoints(rc, pts);
            //////////////////////////////////////////////////////////////////////////
            // 닫기 버튼 그리기
            BMPManager->getTabIcons()->GetImageInfo(1, &info);
            CPoint beg = pts[2];
            beg.x -= (info.rcImage.right - info.rcImage.left);
            beg.y += 2;
            BMPManager->drawTransparent(&dc, BMPManager->getTabIcons(), CLOSE_BTN_N, beg, FALSE, CPoint(0, 0), RGB(0xC6, 0xFF, 0x00));
	    }
    }
	return 0;
}

void CFishTabCtrl::OnUpdateEdit() 
{
	if(m_ctrlEdit.m_hWnd)
	{
		m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
		RecalcLayout(RECALC_EDIT_RESIZED,m_nItemSelected);
		Invalidate(FALSE);
	}
}

LONG CFishTabCtrl::OnThemeChanged(WPARAM wParam, LPARAM lParam) 
{
	::DeleteObject(m_hBmpBkLeftSpin);
	m_hBmpBkLeftSpin = NULL;
	::DeleteObject(m_hBmpBkRightSpin);
	m_hBmpBkRightSpin = NULL;

	HBITMAP hBmpGlyph = NULL;
	CDC dcGlyph;
	dcGlyph.CreateCompatibleDC(NULL);
	CBitmap* pOldBmpGlyph = NULL;

	try
	{
		CThemeUtil tm;
		if(!tm.OpenThemeData(m_hWnd, L"SPIN"))
			AfxThrowUserException();

		{
			// left spin background
			int nBkType;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_BGTYPE,&nBkType))
				AfxThrowUserException();
			if(nBkType!=BT_IMAGEFILE)
				AfxThrowUserException();

			int nImageCount;
			if(!tm.GetThemeInt(SPNP_DOWNHORZ,0,TMT_IMAGECOUNT,&nImageCount))
				AfxThrowUserException();
			if(nImageCount!=4)
				AfxThrowUserException();

			WCHAR szSpinBkLeftBitmapFilename[MAX_PATH];
			if(!tm.GetThemeFilename(SPNP_DOWNHORZ,0,TMT_IMAGEFILE,szSpinBkLeftBitmapFilename,MAX_PATH))
				AfxThrowUserException();
			if(!(m_hBmpBkLeftSpin = tm.LoadBitmap(szSpinBkLeftBitmapFilename)))
				AfxThrowUserException();

			int nLeftImageLayout;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_IMAGELAYOUT,&nLeftImageLayout))
				AfxThrowUserException();
			if(nLeftImageLayout==IL_VERTICAL)
				m_fIsLeftImageHorLayout = FALSE;
			else
				m_fIsLeftImageHorLayout = TRUE;
			
			if(!tm.GetThemeMargins(SPNP_DOWNHORZ,0,TMT_SIZINGMARGINS,&m_mrgnLeft))
				AfxThrowUserException();
		}
		{
			// right spin background
			int nBkType;
			if(!tm.GetThemeEnumValue(SPNP_UPHORZ,0,TMT_BGTYPE,&nBkType))
				AfxThrowUserException();
			if(nBkType!=BT_IMAGEFILE)
				AfxThrowUserException();

			int nImageCount;
			if(!tm.GetThemeInt(SPNP_UPHORZ,0,TMT_IMAGECOUNT,&nImageCount))
				AfxThrowUserException();
			if(nImageCount!=4)
				AfxThrowUserException();

			WCHAR szSpinBkRightBitmapFilename[MAX_PATH];
			if(!tm.GetThemeFilename(SPNP_UPHORZ,0,TMT_IMAGEFILE,szSpinBkRightBitmapFilename,MAX_PATH))
				AfxThrowUserException();
			if(!(m_hBmpBkRightSpin = tm.LoadBitmap(szSpinBkRightBitmapFilename)))
				AfxThrowUserException();
	
			int nRightImageLayout;
			if(!tm.GetThemeEnumValue(SPNP_UPHORZ,0,TMT_IMAGELAYOUT,&nRightImageLayout))
				AfxThrowUserException();
			if(nRightImageLayout==IL_VERTICAL)
				m_fIsRightImageHorLayout = FALSE;
			else
				m_fIsRightImageHorLayout = TRUE;

			if(!tm.GetThemeMargins(SPNP_UPHORZ,0,TMT_SIZINGMARGINS,&m_mrgnRight))
				AfxThrowUserException();
		}
		{
			// glyph color
			int nGlyphType;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_GLYPHTYPE,&nGlyphType))
				AfxThrowUserException();
			
			if(nGlyphType==GT_IMAGEGLYPH)
			{
				COLORREF rgbTransGlyph = RGB(255,0,255);
				if(!tm.GetThemeColor(SPNP_DOWNHORZ,0,TMT_GLYPHTRANSPARENTCOLOR,&rgbTransGlyph))
					AfxThrowUserException();
				WCHAR szSpinGlyphIconFilename[MAX_PATH];
				if(!tm.GetThemeFilename(SPNP_DOWNHORZ,0,TMT_GLYPHIMAGEFILE,szSpinGlyphIconFilename,MAX_PATH))
					AfxThrowUserException();
				if(!(hBmpGlyph = tm.LoadBitmap(szSpinGlyphIconFilename)))
					AfxThrowUserException();

				CBitmap* pBmp = CBitmap::FromHandle(hBmpGlyph);
				if(pBmp==NULL)
					AfxThrowUserException();
				pOldBmpGlyph = dcGlyph.SelectObject(pBmp);
				BITMAP bm;
				pBmp->GetBitmap(&bm);
				m_rgbGlyph[0] = rgbTransGlyph;
				m_rgbGlyph[1] = rgbTransGlyph;
				m_rgbGlyph[2] = rgbTransGlyph;
				m_rgbGlyph[3] = rgbTransGlyph;
				if(m_fIsLeftImageHorLayout)
				{
					for(int i=0;i<bm.bmWidth;i++)
					{
						if(i<bm.bmWidth/4 && m_rgbGlyph[0]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[0]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth/4-1 && m_rgbGlyph[0]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmWidth/4 && i<bm.bmWidth/2 && m_rgbGlyph[1]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[1]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth/2-1 && m_rgbGlyph[1]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmWidth/2 && i<3*bm.bmWidth/4 && m_rgbGlyph[2]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[2]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==3*bm.bmWidth/4-1 && m_rgbGlyph[2]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=3*bm.bmWidth/4 && i<bm.bmWidth && m_rgbGlyph[3]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[3]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth-1 && m_rgbGlyph[3]==rgbTransGlyph)
								AfxThrowUserException();
						}
					}
				}
				else
				{
					for(int i=0;i<bm.bmHeight;i++)
					{
						if(i<bm.bmHeight/4 && m_rgbGlyph[0]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[0] = dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight/4-1 && m_rgbGlyph[0]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmHeight/4 && i<bm.bmHeight/2 && m_rgbGlyph[1]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[1]=dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight/2-1 && m_rgbGlyph[1]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmHeight/2 && i<3*bm.bmHeight/4 && m_rgbGlyph[2]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[2] = dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==3*bm.bmHeight/4-1 && m_rgbGlyph[2]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=3*bm.bmHeight/4 && i<bm.bmHeight && m_rgbGlyph[3]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[3]=dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight-1 && m_rgbGlyph[3]==rgbTransGlyph)
								AfxThrowUserException();
						}
					}
				}
				dcGlyph.SelectObject(pOldBmpGlyph);
				pOldBmpGlyph = NULL;
				::DeleteObject(hBmpGlyph);
				hBmpGlyph = NULL;
			}
			else if(nGlyphType==GT_FONTGLYPH)
			{
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_NORMAL,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[0]))
					AfxThrowUserException();
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_HOT,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[1]))
					AfxThrowUserException();
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_PRESSED,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[2]))
					AfxThrowUserException();	
			}
			else
				AfxThrowUserException();
		}
	}
	catch(CUserException* e)
	{
		e->Delete();
		::DeleteObject(m_hBmpBkLeftSpin);
		m_hBmpBkLeftSpin = NULL;
		::DeleteObject(m_hBmpBkRightSpin);
		m_hBmpBkRightSpin = NULL;
		if(pOldBmpGlyph)
			dcGlyph.SelectObject(pOldBmpGlyph);
		::DeleteObject(hBmpGlyph);
		hBmpGlyph = NULL;
	}
	return 0;
}

void CFishTabCtrl::OnTimer(UINT nIDEvent) 
{
	CWnd::OnTimer(nIDEvent);
	if(nIDEvent==1)
	{
		if(m_nFirstState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nFirstState = BNST_PRESSED;
			RecalcLayout(RECALC_FIRST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;

		}
		if(m_nPrevState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nPrevState = BNST_PRESSED;
			RecalcLayout(RECALC_PREV_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;

		}
		if(m_nNextState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nNextState = BNST_PRESSED;
			RecalcLayout(RECALC_NEXT_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;
		}
		if(m_nLastState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nLastState = BNST_PRESSED;
			RecalcLayout(RECALC_LAST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;

		}
	}
	else if(nIDEvent==2)
	{
		KillTimer(2);
		if(m_nButtonIDDown>=0)
		{
			SetTimer(2,10,NULL);
			DWORD dwStyle = GetStyle();
			if((dwStyle&CTCS_DRAGCOPY) && (GetKeyState(VK_CONTROL)&0x8000))
			{
				if(m_hCursorCopy) SetCursor(m_hCursorCopy);
			}
			else if((dwStyle&CTCS_DRAGMOVE) && !(GetKeyState(VK_CONTROL)&0x8000))
			{
				if(m_hCursorMove) SetCursor(m_hCursorMove);
			}
			else
			{
				m_nButtonIDDown = CTCID_NOBUTTON;
				ReleaseCapture();
			}
			Invalidate(FALSE);
		}
	}
}

void CFishTabCtrl::SetControlFont(const LOGFONT& lf, BOOL fRedraw)
{
	if(m_Font.m_hObject)
	{
		DeleteObject(m_Font);
		m_Font.m_hObject = NULL;
	}

	if(m_FontSelected.m_hObject)
	{
		DeleteObject(m_FontSelected);
		m_FontSelected.m_hObject = NULL;
	}

	if(!m_Font.CreateFontIndirect(&lf))
		m_Font.CreateFontIndirect(&lf_default);


	LOGFONT lfSel;
	m_Font.GetLogFont(&lfSel);
//	lfSel.lfWeight = FW_BOLD;
	m_FontSelected.CreateFontIndirect(&lfSel);

	if(fRedraw)
	{
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
		Invalidate();
	}
}

int CFishTabCtrl::InsertItem(int nItem, CString sText, LPARAM lParam)
{
	if(nItem<0 || nItem>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(sText.GetLength()>MAX_LABEL_TEXT-1)
		return CTCERR_TEXTTOOLONG;

	CFishTabCtrlItem* pItem = new CFishTabCtrlItem(sText,lParam);
	if(pItem==NULL)
		return CTCERR_OUTOFMEMORY;

	try
	{
		m_aItems.InsertAt(nItem,pItem);
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		delete pItem;
		return CTCERR_OUTOFMEMORY;;
	}

	if(m_nItemSelected>=nItem)
		m_nItemSelected++;

	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=m_aItems.GetSize()-1; i>nItem; i--)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i);
			m_ctrlToolTip.DelTool(this,i);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i+1);
		}
		m_ctrlToolTip.DelTool(this,nItem+1);
	}
	
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);

	return nItem;
}

int CFishTabCtrl::MoveItem(int nItemSrc, int nItemDst)
{
	return MoveItem(nItemSrc, nItemDst, FALSE);
}

int CFishTabCtrl::MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
	if(nItemSrc<0||nItemSrc>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(nItemDst<0||nItemDst>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(nItemSrc==nItemDst || nItemSrc==nItemDst-1)
		return nItemSrc;

	CFishTabCtrlItem *pItem = m_aItems[nItemSrc];
	
	// remove item from old place
	CString sOldTooltip;
	if(m_ctrlToolTip.m_hWnd)
	{
		m_ctrlToolTip.GetText(sOldTooltip,this,nItemSrc+1);
		for(int i=nItemSrc+1; i< m_aItems.GetSize(); i++)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i+1);
			m_ctrlToolTip.DelTool(this,i);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i);
		}
	}

	m_aItems.RemoveAt(nItemSrc);

	// insert item in new place
	if(nItemDst>nItemSrc)
		nItemDst--;

	try
	{
		m_aItems.InsertAt(nItemDst,pItem);
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		delete pItem;
		if(fMouseSel)
			NotifyParent(CTCN_ITEMMOVE,nItemSrc,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}

	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=m_aItems.GetSize()-1; i>nItemDst; i--)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i);
			m_ctrlToolTip.DelTool(this,i+1);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i+1);
		}
		m_ctrlToolTip.DelTool(this,nItemDst+1);
		m_ctrlToolTip.AddTool(this,sOldTooltip,CRect(0,0,0,0),nItemDst+1);
	}
	
	m_nItemSelected = nItemDst;

	RecalcLayout(RECALC_ITEM_SELECTED,m_nItemSelected);
	Invalidate(FALSE);
	if(fMouseSel)
		NotifyParent(CTCN_ITEMMOVE,m_nItemSelected,CPoint(0,0));

	return nItemDst;
}

int CFishTabCtrl::CopyItem(int nItemSrc, int nItemDst)
{
	return CopyItem(nItemSrc, nItemDst, FALSE);
}

int CFishTabCtrl::CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
	if(nItemSrc<0||nItemSrc>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(nItemDst<0||nItemDst>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	CString sDst;
	try
	{
		BOOL bAppendFlag=TRUE;
		int n = m_aItems[nItemSrc]->m_sText.GetLength();
		if(n>=4)
		{
			if(m_aItems[nItemSrc]->m_sText[n-1]==_T(')') && 
				m_aItems[nItemSrc]->m_sText[n-2]>_T('1') &&
				m_aItems[nItemSrc]->m_sText[n-2]<=_T('9') &&
				m_aItems[nItemSrc]->m_sText[n-3]==_T('('))
			{
				n = m_aItems[nItemSrc]->m_sText.GetLength()-3;
				bAppendFlag = FALSE;
			}
			else if(m_aItems[nItemSrc]->m_sText[n-1]==_T(')') && 
					m_aItems[nItemSrc]->m_sText[n-2]>=_T('0') &&
					m_aItems[nItemSrc]->m_sText[n-2]<=_T('9') &&
					m_aItems[nItemSrc]->m_sText[n-3]>=_T('1') &&
					m_aItems[nItemSrc]->m_sText[n-3]<=_T('9') &&
					m_aItems[nItemSrc]->m_sText[n-4]==_T('('))
			{
				n = m_aItems[nItemSrc]->m_sText.GetLength()-4;
				bAppendFlag = FALSE;
			}
		}
		int ndx = 1;
		while(1)
		{
			ndx++;
			if(bAppendFlag)
				sDst.Format(_T("%s (%d)"),(LPCTSTR)m_aItems[nItemSrc]->m_sText,ndx);
			else
				sDst.Format(_T("%s(%d)"),(LPCTSTR)m_aItems[nItemSrc]->m_sText.Left(n),ndx);
			for(int i=0;i<m_aItems.GetSize();i++)
			{
				if(m_aItems[i]->m_sText==sDst)
					break;
			}
			if(i==m_aItems.GetSize())
				break;
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		if(fMouseSel)
			NotifyParent(CTCN_OUTOFMEMORY,nItemSrc,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}

	int nRetItem = InsertItem(nItemDst,sDst,m_aItems[nItemSrc]->m_lParam);
	if(nRetItem>=0)
	{
		SetCurSel(nRetItem);
		if(fMouseSel)
			NotifyParent(CTCN_ITEMCOPY,nRetItem,CPoint(0,0));
	}
	else if(fMouseSel && nRetItem==CTCERR_OUTOFMEMORY)
		NotifyParent(CTCN_OUTOFMEMORY,nRetItem,CPoint(0,0));

	return nRetItem;
}

int CFishTabCtrl::DeleteItem(int nItem)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	try
	{
		if(m_ctrlToolTip.m_hWnd)
		{
			for(int i=nItem+1; i< m_aItems.GetSize(); i++)
			{
				CString s;
				m_ctrlToolTip.GetText(s,this,i+1);
				m_ctrlToolTip.DelTool(this,i);
				m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i);
			}
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		return CTCERR_OUTOFMEMORY;
	}

	if(m_aItems.GetSize()==1)
	{
		m_nItemSelected = -1;
	}
	else if(m_nItemSelected==nItem)
	{
		if(m_nItemSelected==m_aItems.GetSize()-1) // last item
		{
			m_nItemSelected--;
			m_aItems[m_nItemSelected]->m_fSelected = TRUE;	
		}
		else
			m_aItems[m_nItemSelected+1]->m_fSelected = TRUE;	
	}
	else if(m_nItemSelected>nItem)
		m_nItemSelected--;

	delete m_aItems[nItem];
	m_aItems.RemoveAt(nItem);

	RecalcLayout(RECALC_RESIZED,m_nItemSelected);

	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

void CFishTabCtrl::DeleteAllItems()
{
	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=0; i< m_aItems.GetSize(); i++)
		{
			delete m_aItems[i];
			m_ctrlToolTip.DelTool(this,i+1);
		}
	}
	else
	{
		for(int i=0; i< m_aItems.GetSize(); i++)
			delete m_aItems[i];
	}

	m_aItems.RemoveAll();

	m_nItemSelected = -1;
		
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
}

int CFishTabCtrl::SetCurSel(int nItem)
{
    if (nItem == -1 && m_nItemSelected != -1){
        m_aItems[m_nItemSelected]->m_fSelected   =   FALSE;
        m_nItemSelected = -1;
    }
	return SetCurSel(nItem,FALSE,FALSE);
}

int CFishTabCtrl::HighlightItem(int nItem, BOOL fHighlight)
{
	if(!(GetStyle()&CTCS_MULTIHIGHLIGHT))
		return CTCERR_NOMULTIHIGHLIGHTSTYLE;
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(m_nItemSelected==-1 && !fHighlight)
		return CTCERR_NOERROR;
	if(m_nItemSelected==-1)
	{
		SetCurSel(nItem);
		return CTCERR_NOERROR;
	}
	if(fHighlight==m_aItems[nItem]->m_fHighlighted || nItem==m_nItemSelected)
		return CTCERR_NOERROR;
	
	m_aItems[nItem]->m_fHighlighted = fHighlight;
	return CTCERR_NOERROR;
}

int CFishTabCtrl::GetItemText(int nItem, CString& sText)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	sText = m_aItems[nItem]->m_sText;
	return CTCERR_NOERROR;
}

int CFishTabCtrl::SetItemText(int nItem, CString sText)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	m_aItems[nItem]->m_sText = sText;
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

int CFishTabCtrl::SetItemTooltipText(int nItem, CString sText)
{
	if(!(GetStyle()&CTCS_TOOLTIPS))
		return CTCERR_NOTOOLTIPSSTYLE;
	if(nItem>=CTCID_LASTBUTTON && nItem<m_aItems.GetSize())
	{
		if(m_ctrlToolTip.m_hWnd==NULL)
		{
			if(!m_ctrlToolTip.Create(this))
				return CTCERR_CREATETOOLTIPFAILED;
			m_ctrlToolTip.Activate(TRUE);
		}
		if(nItem>=0)
			nItem++;
		m_ctrlToolTip.DelTool(this,nItem);
		m_ctrlToolTip.AddTool(this,sText,CRect(0,0,0,0),nItem);
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
		Invalidate(FALSE);
		return CTCERR_NOERROR;
	}
	return CTCERR_INDEXOUTOFRANGE;
}

int CFishTabCtrl::GetItemData(int nItem, DWORD& dwData)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	dwData = m_aItems[nItem]->m_lParam;
	return CTCERR_NOERROR;
}

int CFishTabCtrl::GetItemRect(int nItem, CRect& rect) const
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	rect = m_aItems[nItem]->m_rectText;
	return CTCERR_NOERROR;
}

int CFishTabCtrl::SetItemData(int nItem, DWORD dwData)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	m_aItems[nItem]->m_lParam = dwData;
	return CTCERR_NOERROR;
}

int CFishTabCtrl::IsItemHighlighted(int nItem)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	return (m_aItems[nItem]->m_fHighlighted)?1:0;
}

int	CFishTabCtrl::HitTest(CPoint pt, bool chkClose, int& nExt)
{
/*
	CRect rCl;
			int nA = rCl.Height()-3;

*/
	nExt = NULL;

    CRect rCl;
	GetClientRect(&rCl);

	int nBtns = 2;
	if(GetStyle()&CTCS_FOURBUTTONS)
		nBtns = 4;
	int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;

    //////////////////////////////////////////////////////////////////////////
    // 기능 버튼의 히드 검사
    CRect rAll(rCl.Width() - nBtns*nA -2 , 1, rCl.Width() ,rCl.Height());
	CRect rFirst,rPrev,rNext,rLast;
    if(nBtns==2)
	{
		rPrev.SetRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
		rNext.SetRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() +  2*nA+3, rCl.Height());
	}
	else
	{
		rFirst.SetRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height());
		rPrev.SetRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() + 2*nA+1, rCl.Height());
		rNext.SetRect(rCl.Width() - rAll.Width() + 2*nA+1, 0, rCl.Width() - rAll.Width() + 3*nA+2, rCl.Height());
		rLast.SetRect(rCl.Width() - rAll.Width() + 3*nA+2, 0, rCl.Width() - rAll.Width() + 4*nA+3, rCl.Height());
	}

	if(nBtns==4 && m_nFirstState && rFirst.PtInRect(pt))
		return CTCHT_ONFIRSTBUTTON;

	if(m_nPrevState && rPrev.PtInRect(pt))
		return CTCHT_ONPREVBUTTON;
	
	if(m_nNextState && rNext.PtInRect(pt))
		return CTCHT_ONNEXTBUTTON;
	
	if(nBtns==4 && m_nLastState && rLast.PtInRect(pt))
		return CTCHT_ONLASTBUTTON;

	for(int i=0; i<m_aItems.GetSize(); i++)
	{
        {
            int sentinel = m_aItems[i]->HitTest(pt);
		    if(sentinel)
            {
                if (chkClose && GetStyle() & CTCS_CLOSEREGION_HITTEST && sentinel == HITTEST_RET_CLOSE)
                {
					nExt = CTCN_HITCLOSEREGION;
//                    NotifyParent(CTCN_HITCLOSEREGION, i, pt);
                    return i;
                } else {
                    return i;
                }
            }
			    
        }
	}
	return CTCHT_NOWHERE;
}

int CFishTabCtrl::HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
	if(!(GetStyle()&CTCS_MULTIHIGHLIGHT))
		return CTCERR_NOMULTIHIGHLIGHTSTYLE;

	for(int i=0; i<m_aItems.GetSize();i++)
		m_aItems[i]->m_fHighlightChanged = FALSE;

	if(fCtrlPressed)
	{
		if(nItem!=m_nItemSelected)
		{
			m_aItems[nItem]->m_fHighlighted = !m_aItems[nItem]->m_fHighlighted;
			if(fMouseSel)
				m_aItems[nItem]->m_fHighlightChanged = TRUE;
		}
	}
	else if(!m_aItems[nItem]->m_fHighlighted)
	{
		m_aItems[nItem]->m_fHighlighted = TRUE;
		m_aItems[nItem]->m_fHighlightChanged = TRUE;
		for(int i=0;i<m_aItems.GetSize();i++)
		{
			if(i!=m_nItemSelected)
			{
				if(m_aItems[i]->m_fHighlighted)
				{
					m_aItems[i]->m_fHighlighted = FALSE;
					if(fMouseSel)
						m_aItems[i]->m_fHighlightChanged = TRUE;
				}
			}
		}
	}
	if(fMouseSel)
		RecalcLayout(RECALC_ITEM_SELECTED,nItem);
	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

int CFishTabCtrl::SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(m_nItemSelected!=-1)
		m_aItems[m_nItemSelected]->m_fSelected = FALSE;

	m_nItemSelected = nItem;
	
	if(m_nItemSelected!=-1)
		m_aItems[m_nItemSelected]->m_fSelected = TRUE;

	if(fMouseSel)
		RecalcLayout(RECALC_ITEM_SELECTED,m_nItemSelected);
	else
	{
		m_nItemNdxOffset = nItem;
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	}
	Invalidate(FALSE);
	HighlightItem(nItem, fMouseSel, fCtrlPressed);
	return CTCERR_NOERROR;
}

/************************************************************************
RecalcLayout 탭 아이템의 레이아웃을 재 설정한다.
@param  : 
@return : 
@remark : 
    기본적으로 버튼의 레이아웃을 결정하고 그뒤에 아이템 개별에 대해서 
    레이아웃을 결정한다.
@author : youngchang (eternalbleu@gmail.com)
@history:
    commented at 2005/11/28
************************************************************************/
void CFishTabCtrl::RecalcLayout(int nRecalcType, int nItem)
{
	CRect rCl;
	GetClientRect(&rCl);


	{
		int nBtns = 2;
		if(GetStyle()&CTCS_FOURBUTTONS)
			nBtns = 4;

	    int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;
        rCl.right   -=  nA  *   nBtns;

		int nWidth = RecalcRectangles();
	
		if((GetStyle()&CTCS_AUTOHIDEBUTTONS) && (m_aItems.GetSize()<2 || nWidth <= rCl.Width()))
		{
			m_nFirstState = BNST_INVISIBLE;
			m_nPrevState = BNST_INVISIBLE;
			m_nNextState = BNST_INVISIBLE;
			m_nLastState = BNST_INVISIBLE;
			m_nItemNdxOffset = 0;
			RecalcOffset(0);
			if(nRecalcType==RECALC_EDIT_RESIZED)
				RecalcEditResized(0,nItem);
			
			if(m_ctrlToolTip.m_hWnd)
			{
				m_ctrlToolTip.SetToolRect(this,CTCID_FIRSTBUTTON,CRect(0,0,0,0));
				m_ctrlToolTip.SetToolRect(this,CTCID_PREVBUTTON,CRect(0,0,0,0));
				m_ctrlToolTip.SetToolRect(this,CTCID_NEXTBUTTON,CRect(0,0,0,0));
				m_ctrlToolTip.SetToolRect(this,CTCID_LASTBUTTON,CRect(0,0,0,0));
			}
			return;
		}
		
        //////////////////////////////////////////////////////////////////////////
        // 툴팁 영역 지정
		if(m_ctrlToolTip.m_hWnd)
		{
			int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;
            CRect rAll(rCl.Width() - nBtns*nA -2, 1, rCl.Width(), rCl.Height());       //        CRect rAll(rCl.Width() - nBtns*nA -2 , 1, rCl.Width() ,rCl.Height());
			if(nBtns == 2)
			{
				m_ctrlToolTip.SetToolRect(this,CTCID_FIRSTBUTTON,CRect(0,0,0,0));
				m_ctrlToolTip.SetToolRect(this,CTCID_PREVBUTTON,CRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height()));
				m_ctrlToolTip.SetToolRect(this,CTCID_NEXTBUTTON,CRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() + 2*nA+3, rCl.Height()));
				m_ctrlToolTip.SetToolRect(this,CTCID_LASTBUTTON,CRect(0,0,0,0));
			}
			else
			{
				m_ctrlToolTip.SetToolRect(this,CTCID_FIRSTBUTTON,CRect(rCl.Width() - rAll.Width() + 0, 0, rCl.Width() - rAll.Width() + nA+1, rCl.Height()));
				m_ctrlToolTip.SetToolRect(this,CTCID_PREVBUTTON,CRect(rCl.Width() - rAll.Width() + nA+1, 0, rCl.Width() - rAll.Width() + 2*nA+3, rCl.Height()));
				m_ctrlToolTip.SetToolRect(this,CTCID_NEXTBUTTON,CRect(rCl.Width() - rAll.Width() + 2*nA+1, 0, rCl.Width() - rAll.Width() + 3*nA+2, rCl.Height()));
				m_ctrlToolTip.SetToolRect(this,CTCID_LASTBUTTON,CRect(rCl.Width() - rAll.Width() + 3*nA+2, 0, rCl.Width() - rAll.Width() + 4*nA+3, rCl.Height()));
			}
		}

		int nBnWidth = nBtns*FUNCTION_BTN_WIDTH+3;//nBtns*(rCl.Height()-3)+3;

		if(m_nFirstState==BNST_INVISIBLE)
			m_nFirstState = BNST_NORMAL;
		if(m_nPrevState==BNST_INVISIBLE)
			m_nPrevState = BNST_NORMAL;
		if(m_nNextState == BNST_INVISIBLE)
			m_nNextState = BNST_NORMAL;
		if(m_nLastState == BNST_INVISIBLE)
			m_nLastState = BNST_NORMAL;
		
		if(m_aItems.GetSize()==0)
			return;

		switch(nRecalcType)
		{
		case RECALC_FIRST_PRESSED:
			{
				m_nItemNdxOffset=0;
				RecalcRectangles();
				RecalcOffset(nBnWidth);
			}
			break;
		case RECALC_PREV_PRESSED:
			{	
				RecalcOffset(nBnWidth);
				if(m_nItemNdxOffset>0)
				{
					m_nItemNdxOffset--;
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
			}
			break;
		case RECALC_NEXT_PRESSED:
			{	
				RecalcOffset(nBnWidth);
				if(m_aItems[m_aItems.GetSize()-1]->m_rect.right>rCl.Width() && m_nItemNdxOffset!=m_aItems.GetSize()-1)
				{
					m_nItemNdxOffset++;
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
			}
			break;
		case RECALC_ITEM_SELECTED:
			{
				RecalcOffset(nBnWidth);
				if(m_aItems[nItem]->m_bShape==TAB_SHAPE2 || m_aItems[nItem]->m_bShape==TAB_SHAPE3)
				{
					m_nItemNdxOffset--;
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
				else
				{
					while(m_nItemNdxOffset<nItem && 
							m_aItems[nItem]->m_bShape==TAB_SHAPE4 && 
							m_aItems[nItem]->m_rect.right>rCl.Width() && 
							m_aItems[nItem]->m_rect.left>nBnWidth)
					{
						m_nItemNdxOffset++;
						RecalcRectangles();
						RecalcOffset(nBnWidth);
					}
				}
			}
			break;
		case RECALC_EDIT_RESIZED:
			{
				RecalcOffset(nBnWidth);
				RecalcEditResized(nBnWidth,nItem);
			}
			break;
		case RECALC_LAST_PRESSED:
			{
				m_nItemNdxOffset=m_aItems.GetSize()-1;
			}
		default:	// window resized
			{
				BOOL bNdxOffsetChanged = FALSE;
				RecalcOffset(nBnWidth);
				while(m_nItemNdxOffset>=0 && m_aItems[m_aItems.GetSize()-1]->m_rect.right<rCl.Width())
				{
					m_nItemNdxOffset--;
					if(m_nItemNdxOffset>=0)
					{
						RecalcRectangles();
						RecalcOffset(nBnWidth);
					}
					bNdxOffsetChanged = TRUE;
				}
				if(bNdxOffsetChanged)
				{
					m_nItemNdxOffset++;
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
			}
			break;
		}
	}
}

void CFishTabCtrl::RecalcEditResized(int nOffset, int nItem)
{
	CRect rCl;
	GetClientRect(rCl);
	CDC* pDC = GetDC();
	if(!pDC) return;
	do
	{
		CRect r;
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);
		int h = pDC->DrawText(m_aItems[nItem]->m_sText+"X", r, DT_CALCRECT);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		r = m_aItems[nItem]->m_rectText;
        r.left += m_rectItemIconRegion.Width();
        r.right += m_rectItemIconRegion.Width();
		if(r.Height()>h)
		{
			r.top += (r.Height()-h)/2;
			r.bottom -= (r.Height()-h)/2;
		}
		r.left += 2;
		if(r.right>rCl.right && m_nItemSelected>m_nItemNdxOffset)
		{
			m_nItemNdxOffset++;
			RecalcRectangles();
			RecalcOffset(nOffset);
		}
		else
		{
			if(r.right>rCl.right)
				r.right = rCl.right;
			m_ctrlEdit.MoveWindow(r);
			int n = m_aItems[nItem]->m_sText.GetLength();
			int nStart, nEnd;
			m_ctrlEdit.GetSel(nStart,nEnd);
			if(nStart==nEnd && nStart==n)
			{
				m_ctrlEdit.SetSel(0,0);
				m_ctrlEdit.SetSel(n,n);
			}
			return;
		}
	} 
	while(1);

	ReleaseDC(pDC);
}

void CFishTabCtrl::RecalcOffset(int nOffset)
{
	CRect rCl;
	GetClientRect(&rCl);

	int nBtns = 2;
	if(GetStyle()&CTCS_FOURBUTTONS)
		nBtns = 4;
	int nBnWidth = nBtns * FUNCTION_BTN_WIDTH + 3;//nBtns*(rCl.Height()-3)+3;

    rCl.right   -=  nBnWidth;       // 오른쪽의 버튼의 영역을 예약함.
    nOffset    =   -nOffset - m_rectItemFunctRegin.Width();
    if (m_nItemNdxOffset    ==  0)  {
        nOffset =   0;
    }

    // 탭의 모양을 정하는 부분
	for(int i = 0; i<m_aItems.GetSize(); i++)
	{
		if(i <= m_nItemNdxOffset-1)
		{
			m_aItems[i]->m_bShape = TAB_SHAPE1;
			nOffset -= m_aItems[i]->m_rect.Width() ;//- rCl.Height()/2;
			m_aItems[i]->m_rectText.SetRectEmpty();
		}
        /*
		else if(i==m_nItemNdxOffset-1)
		{
			if(i==m_nItemSelected)
				m_aItems[i]->m_bShape = TAB_SHAPE2;
			else
				m_aItems[i]->m_bShape = TAB_SHAPE3;
			nOffset -= m_aItems[i]->m_rect.Width() - rCl.Height()/2;
			m_aItems[i]->m_rect.SetRect(0,0,rCl.Height()/2,rCl.Height()-1);
			m_aItems[i]->m_rect.OffsetRect(nBnWidth,0);
			m_aItems[i]->m_rectText.SetRectEmpty();
		}
        */
		else
		{
			if(i==m_nItemSelected)
				m_aItems[i]->m_bShape = TAB_SHAPE4;
			else if(i==m_aItems.GetSize()-1)	// last item
				m_aItems[i]->m_bShape = TAB_SHAPE4;
			else
				m_aItems[i]->m_bShape = TAB_SHAPE5;
			m_aItems[i]->m_rect.OffsetRect(nOffset,0);
			m_aItems[i]->m_rectText.OffsetRect(nOffset,0);
		}
		m_aItems[i]->ComputeRgn();
		if(m_ctrlToolTip.m_hWnd)
			m_ctrlToolTip.SetToolRect(this,i+1,m_aItems[i]->m_rectText);
	}
}

//////////////////////////////////////////////////////////////////////////
// 탭 아이템의 사각형 영영을 구한다. 
// 외곽현 텍스트의 영역을 구함
int CFishTabCtrl::RecalcRectangles()
{
	CRect rCl;
	GetClientRect(&rCl);

    int nBtns = 2;
    if(GetStyle()&CTCS_FOURBUTTONS)
        nBtns = 4;
    int nA = FUNCTION_BTN_WIDTH;//rCl.Height()-3;
    rCl.right   -=  nA  *   nBtns;

	int nWidth = 0;
	
	{
		// calculate width
		int nOffset = 0;

		CRect rcText;
		CDC* pDC = GetDC();
		if(!pDC) return 0;
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);

        if(GetStyle()&CTCS_FIXEDWIDTH)                  // 고정 간격처리
		{
			int nMaxWidth=0;
			for(int i=0; i<m_aItems.GetSize(); i++)
			{
				int w=0;
				int h = pDC->DrawText(m_aItems[i]->m_sText, rcText, DT_CALCRECT);
				if(h>0)
					w = rcText.Width();
				if(w>nMaxWidth)
					nMaxWidth = w;
			}
			for(i=0; i<m_aItems.GetSize(); i++)
			{
				m_aItems[i]->m_rect = CRect(0, 0, nMaxWidth+FISH_TAB_INNER_PAD_RIGHT, rCl.Height());
				m_aItems[i]->m_rect += CPoint(nOffset, 0);
				m_aItems[i]->m_rectText = CRect(FISH_TAB_INNER_PAD_LEFT, FISH_TAB_INNER_PAD_TOP, nMaxWidth, rCl.Height() - FISH_TAB_INNER_PAD_BOTTOM);
				m_aItems[i]->m_rectText += CPoint(nOffset, 0);

				nOffset += m_aItems[i]->m_rect.Width();
				nWidth = m_aItems[i]->m_rect.right;
			}
		}
		else                                            // 비고정 간격처리
		{
			for(int i= 0; i<m_aItems.GetSize(); i++)
			{
				int w=0;
				int h = pDC->DrawText(m_aItems[i]->m_sText, rcText, DT_CALCRECT);
				if(h>0)
					w = FISH_TAB_FIXEDWIDTH + m_rectItemFunctRegin.Width() + m_rectItemIconRegion.Width();
				m_aItems[i]->m_rect = CRect(0, 0, w+FISH_TAB_DEFLATE_WIDTH_RIGHT+m_rectItemIconRegion.Width()+m_rectItemFunctRegin.Width(), rCl.Height());
				m_aItems[i]->m_rect += CPoint(nOffset, 0);
				m_aItems[i]->m_rectText = CRect(FISH_TAB_INNER_PAD_LEFT+2, FISH_TAB_INNER_PAD_TOP, w, rCl.Height() - FISH_TAB_INNER_PAD_BOTTOM);
				m_aItems[i]->m_rectText += CPoint(nOffset, 0);

				nOffset += m_aItems[i]->m_rect.Width();
				nWidth = m_aItems[i]->m_rect.right;

			}
		}
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
	}

	return nWidth;
}

BOOL CFishTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(GetStyle()&CTCS_TOOLTIPS && m_ctrlToolTip.m_hWnd && 
		(pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_LBUTTONUP || pMsg->message==WM_MOUSEMOVE))
			m_ctrlToolTip.RelayEvent(pMsg);

    switch(pMsg->message)
	{
	case WM_KEYDOWN:
        if(pMsg->wParam ==  VK_TAB)
        {
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
        }

		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CFishTabCtrl::DrawBkLeftSpin(CDC& dc, CRect& r, int nImageNdx)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap* pBmp = CBitmap::FromHandle(m_hBmpBkLeftSpin);
	BITMAP bm;
	pBmp->GetBitmap(&bm);
	CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
	if(m_fIsLeftImageHorLayout)
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m_mrgnLeft.cxLeftWidth,
				m_mrgnLeft.cyTopHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				0,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m_mrgnLeft.cxRightWidth,
				r.top,
				m_mrgnLeft.cxRightWidth,
				m_mrgnLeft.cyTopHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-m_mrgnLeft.cxRightWidth,
				0,
				SRCCOPY);

		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m_mrgnLeft.cyBottomHeight,
				m_mrgnLeft.cxLeftWidth,
				m_mrgnLeft.cyBottomHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				bm.bmHeight-m_mrgnLeft.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m_mrgnLeft.cxRightWidth,
				r.bottom-m_mrgnLeft.cyBottomHeight,
				m_mrgnLeft.cxRightWidth,
				m_mrgnLeft.cyBottomHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-m_mrgnLeft.cxRightWidth,
				bm.bmHeight-m_mrgnLeft.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m_mrgnLeft.cxLeftWidth,
			r.top,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyTopHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+m_mrgnLeft.cxLeftWidth,
			0,
			bm.bmWidth/4-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m_mrgnLeft.cxLeftWidth,
			r.bottom-m_mrgnLeft.cyBottomHeight,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+m_mrgnLeft.cxLeftWidth,
			bm.bmHeight-m_mrgnLeft.cyBottomHeight,
			bm.bmWidth/4-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxLeftWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4,
			m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxLeftWidth,
			bm.bmHeight-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m_mrgnLeft.cxRightWidth,
			r.top+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxRightWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			(nImageNdx+1)*bm.bmWidth/4-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxRightWidth,
			bm.bmHeight-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m_mrgnLeft.cxLeftWidth,
			r.top+m_mrgnLeft.cyTopHeight,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4 + m_mrgnLeft.cxLeftWidth,
			m_mrgnLeft.cyTopHeight,
			bm.bmWidth/4-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			bm.bmHeight-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);
	}
	else
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m_mrgnLeft.cxLeftWidth,
				m_mrgnLeft.cyTopHeight,
				&dcMem,
				0,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m_mrgnLeft.cxRightWidth,
				r.top,
				m_mrgnLeft.cxRightWidth,
				m_mrgnLeft.cyTopHeight,
				&dcMem,
				bm.bmWidth-m_mrgnLeft.cxRightWidth,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m_mrgnLeft.cyBottomHeight,
				m_mrgnLeft.cxLeftWidth,
				m_mrgnLeft.cyBottomHeight,
				&dcMem,
				0,
				(nImageNdx+1)*bm.bmHeight/4-m_mrgnLeft.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m_mrgnLeft.cxRightWidth,
				r.bottom-m_mrgnLeft.cyBottomHeight,
				m_mrgnLeft.cxRightWidth,
				m_mrgnLeft.cyBottomHeight,
				&dcMem,
				bm.bmWidth-m_mrgnLeft.cxRightWidth,
				(nImageNdx+1)*bm.bmHeight/4-m_mrgnLeft.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m_mrgnLeft.cxLeftWidth,
			r.top,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyTopHeight,
			&dcMem,
			m_mrgnLeft.cxLeftWidth,
			nImageNdx*bm.bmHeight/4,
			bm.bmWidth-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m_mrgnLeft.cxLeftWidth,
			r.bottom-m_mrgnLeft.cyBottomHeight,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyBottomHeight,
			&dcMem,
			m_mrgnLeft.cxLeftWidth,
			(nImageNdx+1)*bm.bmHeight/4-m_mrgnLeft.cyBottomHeight,
			bm.bmWidth-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxLeftWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			0,
			nImageNdx*bm.bmHeight/4+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxLeftWidth,
			bm.bmHeight/4-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m_mrgnLeft.cxRightWidth,
			r.top+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxRightWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			bm.bmWidth-m_mrgnLeft.cxRightWidth,
			nImageNdx*bm.bmHeight/4+m_mrgnLeft.cyTopHeight,
			m_mrgnLeft.cxRightWidth,
			bm.bmHeight/4-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m_mrgnLeft.cxLeftWidth,
			r.top+m_mrgnLeft.cyTopHeight,
			r.Width()-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			r.Height()-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			&dcMem,
			m_mrgnLeft.cxLeftWidth,
			nImageNdx*bm.bmHeight/4+m_mrgnLeft.cyTopHeight,
			bm.bmWidth-m_mrgnLeft.cxLeftWidth-m_mrgnLeft.cxRightWidth,
			bm.bmHeight/4-m_mrgnLeft.cyTopHeight-m_mrgnLeft.cyBottomHeight,
			SRCCOPY);
	}
	dcMem.SelectObject(pOldBmp);
}

void CFishTabCtrl::DrawBkRightSpin(CDC& dc, CRect& r, int nImageNdx)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap* pBmp = CBitmap::FromHandle(m_hBmpBkRightSpin);
	BITMAP bm;
	pBmp->GetBitmap(&bm);
	CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
	if(m_fIsRightImageHorLayout)
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m_mrgnRight.cxLeftWidth,
				m_mrgnRight.cyTopHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				0,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m_mrgnRight.cxRightWidth,
				r.top,
				m_mrgnRight.cxRightWidth,
				m_mrgnRight.cyTopHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-m_mrgnRight.cxRightWidth,
				0,
				SRCCOPY);

		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m_mrgnRight.cyBottomHeight,
				m_mrgnRight.cxLeftWidth,
				m_mrgnRight.cyBottomHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				bm.bmHeight-m_mrgnRight.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m_mrgnRight.cxRightWidth,
				r.bottom-m_mrgnRight.cyBottomHeight,
				m_mrgnRight.cxRightWidth,
				m_mrgnRight.cyBottomHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-m_mrgnRight.cxRightWidth,
				bm.bmHeight-m_mrgnRight.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m_mrgnRight.cxLeftWidth,
			r.top,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyTopHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+m_mrgnRight.cxLeftWidth,
			0,
			bm.bmWidth/4-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m_mrgnRight.cxLeftWidth,
			r.bottom-m_mrgnRight.cyBottomHeight,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+m_mrgnRight.cxLeftWidth,
			bm.bmHeight-m_mrgnRight.cyBottomHeight,
			bm.bmWidth/4-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxLeftWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4,
			m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxLeftWidth,
			bm.bmHeight-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m_mrgnRight.cxRightWidth,
			r.top+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxRightWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			(nImageNdx+1)*bm.bmWidth/4-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxRightWidth,
			bm.bmHeight-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m_mrgnRight.cxLeftWidth,
			r.top+m_mrgnRight.cyTopHeight,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4 + m_mrgnRight.cxLeftWidth,
			m_mrgnRight.cyTopHeight,
			bm.bmWidth/4-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			bm.bmHeight-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);
	}
	else
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				m_mrgnRight.cxLeftWidth,
				m_mrgnRight.cyTopHeight,
				&dcMem,
				0,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-m_mrgnRight.cxRightWidth,
				r.top,
				m_mrgnRight.cxRightWidth,
				m_mrgnRight.cyTopHeight,
				&dcMem,
				bm.bmWidth-m_mrgnRight.cxRightWidth,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-m_mrgnRight.cyBottomHeight,
				m_mrgnRight.cxLeftWidth,
				m_mrgnRight.cyBottomHeight,
				&dcMem,
				0,
				(nImageNdx+1)*bm.bmHeight/4-m_mrgnRight.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-m_mrgnRight.cxRightWidth,
				r.bottom-m_mrgnRight.cyBottomHeight,
				m_mrgnRight.cxRightWidth,
				m_mrgnRight.cyBottomHeight,
				&dcMem,
				bm.bmWidth-m_mrgnRight.cxRightWidth,
				(nImageNdx+1)*bm.bmHeight/4-m_mrgnRight.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+m_mrgnRight.cxLeftWidth,
			r.top,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyTopHeight,
			&dcMem,
			m_mrgnRight.cxLeftWidth,
			nImageNdx*bm.bmHeight/4,
			bm.bmWidth-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+m_mrgnRight.cxLeftWidth,
			r.bottom-m_mrgnRight.cyBottomHeight,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyBottomHeight,
			&dcMem,
			m_mrgnRight.cxLeftWidth,
			(nImageNdx+1)*bm.bmHeight/4-m_mrgnRight.cyBottomHeight,
			bm.bmWidth-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxLeftWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			0,
			nImageNdx*bm.bmHeight/4+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxLeftWidth,
			bm.bmHeight/4-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-m_mrgnRight.cxRightWidth,
			r.top+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxRightWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			bm.bmWidth-m_mrgnRight.cxRightWidth,
			nImageNdx*bm.bmHeight/4+m_mrgnRight.cyTopHeight,
			m_mrgnRight.cxRightWidth,
			bm.bmHeight/4-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+m_mrgnRight.cxLeftWidth,
			r.top+m_mrgnRight.cyTopHeight,
			r.Width()-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			r.Height()-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			&dcMem,
			m_mrgnRight.cxLeftWidth,
			nImageNdx*bm.bmHeight/4+m_mrgnRight.cyTopHeight,
			bm.bmWidth-m_mrgnRight.cxLeftWidth-m_mrgnRight.cxRightWidth,
			bm.bmHeight/4-m_mrgnRight.cyTopHeight-m_mrgnRight.cyBottomHeight,
			SRCCOPY);
	}
	dcMem.SelectObject(pOldBmp);
}

void CFishTabCtrl::DrawGlyph(CDC& dc, CPoint& pt, int nImageNdx, int nColorNdx)
{
	CDC dcMem, dcMemMono;
	dcMem.CreateCompatibleDC(&dc);
	dcMemMono.CreateCompatibleDC(&dc);

	CBitmap* pOldBmpGlyphMono = dcMemMono.SelectObject(&m_bmpGlyphsMono);

	CBitmap bmpGlyphColor;
	bmpGlyphColor.CreateCompatibleBitmap(&dc,8,7);
	
	CBitmap* pOldBmpGlyphColor = dcMem.SelectObject(&bmpGlyphColor);

	COLORREF rgbOldTextGlyph =  dcMem.SetTextColor(m_rgbGlyph[nColorNdx]);
	dcMem.BitBlt(0, 0, 8, 7, &dcMemMono, nImageNdx*8, 0, SRCCOPY);
	dcMem.SetTextColor(rgbOldTextGlyph);

	COLORREF rgbOldBk = dc.SetBkColor(RGB(255,255,255));
	COLORREF rgbOldText = dc.SetTextColor(RGB(0,0,0));
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMem, 0, 0, SRCINVERT);
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMemMono, nImageNdx*8, 0, SRCAND);
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMem, 0, 0, SRCINVERT);

	dcMem.SelectObject(pOldBmpGlyphColor);
	dcMemMono.SelectObject(pOldBmpGlyphMono);
	dc.SetBkColor(rgbOldBk);
	dc.SetTextColor(rgbOldText);
}

BOOL CFishTabCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if(dwRemove&CTCS_TOOLTIPS)
		m_ctrlToolTip.DestroyWindow();
	if(dwRemove&CTCS_MULTIHIGHLIGHT)
	{
		for(int i=0;i<m_aItems.GetSize();i++)
			m_aItems[i]->m_fHighlighted = FALSE;
	}
	if(dwAdd&CTCS_MULTIHIGHLIGHT)
	{
		for(int i=0;i<m_aItems.GetSize();i++)
		{
			if(i==m_nItemSelected)
				m_aItems[i]->m_fHighlighted = TRUE;
		}
	}
	CWnd::ModifyStyle(dwRemove,dwAdd,nFlags);
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return TRUE;
}

void CFishTabCtrl::PreSubclassWindow() 
{
	OnThemeChanged(0,0);
	CWnd::ModifyStyle(0,WS_CLIPCHILDREN);
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	CWnd::PreSubclassWindow();
}

void CFishTabCtrl::SetDragCursors(HCURSOR hCursorMove, HCURSOR hCursorCopy)
{
	::DestroyCursor(m_hCursorMove);
	m_hCursorMove = NULL;
	::DestroyCursor(m_hCursorCopy);
	m_hCursorCopy = NULL;
	m_hCursorMove = CopyCursor(hCursorMove);
	m_hCursorCopy = CopyCursor(hCursorCopy);
}

void CFishTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	int nExData;
	int nHitTest = HitTest(point, false, nExData);
	NotifyParent(CTCN_RCLICK,nHitTest,point);
	CWnd::OnRButtonDown(nFlags, point);
}

int CFishTabCtrl::EditLabel(int nItem)
{
	return EditLabel(nItem, FALSE);
}

int CFishTabCtrl::EditLabel(int nItem, BOOL fMouseSel)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(!(GetStyle()&CTCS_EDITLABELS))
		return CTCERR_NOEDITLABELSTYLE;
	if(nItem!=m_nItemSelected)
		return CTCERR_ITEMNOTSELECTED;
	if(m_ctrlEdit.m_hWnd)
		return CTCERR_ALREADYINEDITMODE;
	
	try
	{
		CRect r;
		CDC* pDC = GetDC();
		if(!pDC) return 0;
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);
		int h = pDC->DrawText(m_aItems[nItem]->m_sText, r, DT_CALCRECT);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		r = m_aItems[nItem]->m_rectText;

		if(r.Height()>h)
		{
			r.top += (r.Height()-h)/2;
			r.bottom -= (r.Height()-h)/2;
		}
		r.left += 2;

        if(m_ctrlEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,r,this,CTCID_EDITCTRL))
		{
			CString sOld = m_aItems[nItem]->m_sText;
			m_ctrlEdit.SetFont(&m_FontSelected,FALSE);
			m_ctrlEdit.SetLimitText(MAX_LABEL_TEXT);
			m_ctrlEdit.SetWindowText(m_aItems[nItem]->m_sText);
			m_ctrlEdit.SetFocus();
			m_ctrlEdit.SetSel(0,-1);
			if(fMouseSel)
				ReleaseCapture();
			for (;;) 
			{
				MSG msg;
				::GetMessage(&msg, NULL, 0, 0);

				switch (msg.message) 
				{
				case WM_KEYDOWN:
					{
						if (msg.wParam == VK_ESCAPE)
						{
							m_aItems[nItem]->m_sText = sOld;
							m_ctrlEdit.DestroyWindow();
							RecalcLayout(RECALC_RESIZED,m_nItemSelected);
							Invalidate(FALSE);
							return CTCERR_NOERROR;
						}
						if(msg.wParam == VK_RETURN)
						{
							if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
								break;
							m_ctrlEdit.GetWindowText(m_aItems[nItem]->m_sText);
							m_ctrlEdit.DestroyWindow();
							RecalcLayout(RECALC_RESIZED,nItem);
							Invalidate(FALSE);
							return CTCERR_NOERROR;
						}
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					break;
				case WM_LBUTTONDOWN:
					{
						if(msg.hwnd==m_hWnd)
						{
							POINTS pt = MAKEPOINTS(msg.lParam);
							int nExData;
							if(HitTest(CPoint(pt.x,pt.y), false, nExData)!=m_nItemSelected)
							{
								if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
									break;
								m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
								m_ctrlEdit.DestroyWindow();
								TranslateMessage(&msg);
								DispatchMessage(&msg);
								return CTCERR_NOERROR;
							}
						}
						else if(msg.hwnd==m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						else
						{
							if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
								break;
							m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
							m_ctrlEdit.DestroyWindow();
							return CTCERR_NOERROR;
						}
					}
					break;
				case WM_LBUTTONUP:
					{
						if(msg.hwnd==m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
					break;
				case WM_NCLBUTTONDOWN:
					{
						if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
							break;
						m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
						m_ctrlEdit.DestroyWindow();
						TranslateMessage(&msg);
						DispatchMessage(&msg);
						return CTCERR_NOERROR;
					}
					break;
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					break;
				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
				}
			}
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		if(fMouseSel)
			NotifyParent(CTCN_OUTOFMEMORY,nItem,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}
	return CTCERR_NOERROR;
}

UINT CFishTabCtrlItem::HitTest(CPoint pt)			
{ 
    UINT ret = HITTEST_RET_FALSE;
    CRect rt = m_rect;
    rt.left = rt.right - m_rectItemFunctRegin.Width();
    if (m_bShape && m_rgn.PtInRegion(pt))   ret = HITTEST_RET_TRUE;         // TRUE is 1
    if (m_bShape && rt.PtInRect(pt))        ret = HITTEST_RET_CLOSE;

//    TRACE(_T("PT: %d %d\n"), pt.x, pt.y);
//    TRACE(_T("PT: %d %d %d %d\n"), rt.left, rt.top, rt.right, rt.bottom);

//    if (ret)    TRACE (_T("CUSTOM ITEM HIT TEST RESULT : %d\n"), ret);

    return ret;
}