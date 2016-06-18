// FishListBox.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "FishListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishListBox

#include "ctrl/MemDC.h"

CFishListBox::CFishListBox()
{
    m_ftDefault.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
}

CFishListBox::~CFishListBox()
{
}


BEGIN_MESSAGE_MAP(CFishListBox, CListBox)
	//{{AFX_MSG_MAP(CFishListBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishListBox message handlers
#define FISH_LISTBOX_TEXT_CLR       RGB(0x41, 0x41, 0x41)
#define FISH_LISTBOX_UNDERLINE_CLR  RGB(0xe1, 0xe1, 0xe1)
#define FISH_LISTBOX_LIST_OUTLINE_CLR   RGB(0xb9, 0xb9, 0xb9)

void CFishListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    /*
    // TODO: Add your code to draw the specified item
    enum {
        FISH_SCRAP_MGR_FG_CLR_SEL   =   RGB(0xFF, 0xFF, 0xFF),
        FISH_SCRAP_MGR_BG_CLR_SEL   =   RGB(0x00, 0x00, 0xFF),

        FISH_SCRAP_MGR_FG_CLR_DESEL =   RGB(0x00, 0x00, 0x00),
        FISH_SCRAP_MGR_BG_CLR_DESEL =   RGB(0xFF, 0xFF, 0xFF),    
    };
	CDC*			pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	BOOL			bIsSelected = FALSE;
	BOOL			bIsFocused = FALSE;
	BOOL			bIsDisabled = FALSE;
    CString         szText;

	bIsSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
	bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CRect rcItem = lpDrawItemStruct->rcItem;

//    pDC->SetBkMode(TRANSPARENT);

    COLORREF    bgClr;
    COLORREF    fgClr;

    if ( !bIsSelected )
    {
        bgClr   =   FISH_SCRAP_MGR_BG_CLR_DESEL;
        fgClr   =   FISH_SCRAP_MGR_FG_CLR_DESEL;
    } else {
        if ( bIsSelected && bIsFocused )
        {
            bgClr   =   FISH_SCRAP_MGR_BG_CLR_SEL;
            fgClr   =   FISH_SCRAP_MGR_FG_CLR_SEL;
        }

        if ( bIsSelected && !bIsFocused)
        {
            bgClr   =   FISH_SCRAP_MGR_BG_CLR_SEL;
            fgClr   =   FISH_SCRAP_MGR_FG_CLR_SEL;
        }
    }

    pDC->SetTextColor( fgClr );
    pDC->SetBkColor( bgClr );

	CListBox::GetText(lpDrawItemStruct->itemID, szText);
	pDC->DrawText(szText, -1, rcItem, DT_WORDBREAK | DT_EXPANDTABS | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    */
}

void CFishListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	
}

void CFishListBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CListBox::PreSubclassWindow();
}

void CFishListBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CListBox::OnPaint() for painting messages
}

BOOL CFishListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
    /*
    CRect clientRT;
    GetClientRect( &clientRT );

    CMemDC memDC(pDC, &clientRT);
    {
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, FISH_LISTBOX_LIST_OUTLINE_CLR);

        CPen* pOldPen   =   memDC.SelectObject( &pen );
        
        memDC.Rectangle( clientRT );

        memDC.SelectObject( pOldPen );
    }
    return TRUE;
    */
	return CListBox::OnEraseBkgnd(pDC);
}
