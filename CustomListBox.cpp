// CustomListBox.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "CustomListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomListBox

CCustomListBox::CCustomListBox()
{
	m_clrSelectedItemBg = RGB(0, 0, 244);
	m_clrBackground = RGB(255, 255, 255);
	m_clrBorder = RGB(0, 0, 0);
	m_clrNormalItemBg = RGB(255, 255, 255);
	m_clrNormalFont = RGB(0, 0, 0);
	m_clrSelectedFont = RGB(255, 255, 255);

	m_pFontNormal = NULL;
	m_pFontSelected = NULL;
	m_nItemGap = 0;
	m_nMarginLeft = 0;
	m_nMarginRight = 0;
	m_bListFill = FALSE;
}

CCustomListBox::~CCustomListBox()
{
}


BEGIN_MESSAGE_MAP(CCustomListBox, CListBox)
	//{{AFX_MSG_MAP(CCustomListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
//		ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomListBox message handlers


void CCustomListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item	
	CDC				dc;
	
	BOOL			bIsSelected = FALSE;
	BOOL			bIsFocused = FALSE;
	BOOL			bIsDisabled = FALSE;
    CString			szText;
	

	bIsSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
	bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

	CRect rcItem = lpDrawItemStruct->rcItem;


    COLORREF    bgClr;
    COLORREF    fgClr;

    if ( !bIsSelected )
    {
        bgClr   =   m_clrNormalItemBg;
        fgClr   =   m_clrNormalFont;
    }
	else
	{
        if(bIsSelected && bIsFocused)
        {
            bgClr   =   m_clrSelectedItemBg;
            fgClr   =   m_clrSelectedFont;
        }

        if(bIsSelected && !bIsFocused)
        {
            bgClr   =   m_clrSelectedItemBg;
            fgClr   =   m_clrSelectedFont;
        }
    }
    

	GetText(lpDrawItemStruct->itemID, szText);
	
	dc.Attach(lpDrawItemStruct->hDC);
	//if(m_nItemGap != 0)
	//{
		CPen pen_h, pen_v, *pOldPen;
		pen_h.CreatePen(PS_SOLID, m_nItemGap, m_clrBorder);
		pen_v.CreatePen(PS_SOLID, m_nSideGap, m_clrBorder);		
		
		if(m_nSideGap != 0)
		{
			pOldPen = dc.SelectObject(&pen_v);
			dc.MoveTo(rcItem.left, rcItem.bottom);
			dc.LineTo(rcItem.left, rcItem.top);
			dc.SelectObject(pOldPen);
		}


		if(lpDrawItemStruct->itemID != 0 && m_nItemGap != 0)
		{
			pOldPen = dc.SelectObject(&pen_h);
			dc.MoveTo(rcItem.left, rcItem.top);
			dc.LineTo(rcItem.right, rcItem.top);
			dc.SelectObject(pOldPen);
		}

		if(m_nSideGap != 0)
		{
			pOldPen = dc.SelectObject(&pen_v);
			dc.MoveTo(rcItem.right - m_nItemGap, rcItem.top);
			dc.LineTo(rcItem.right - m_nItemGap, rcItem.bottom);			
			dc.SelectObject(pOldPen);
		}		
		
		int size = GetCount();
		
		if(lpDrawItemStruct->itemID == size - 1)
		{
			if(m_nItemGap != 0)
			{
				pOldPen = dc.SelectObject(&pen_h);
				dc.MoveTo(rcItem.right, rcItem.bottom);
				dc.LineTo(rcItem.left, rcItem.bottom);
				dc.SelectObject(pOldPen);
			}

			if(m_bListFill)
			{
				CRect clientRect;
				GetClientRect(clientRect);
				clientRect.top = rcItem.bottom;

				if(m_nSideGap != 0)
				{
					pOldPen = dc.SelectObject(&pen_v);
					dc.MoveTo(clientRect.left, clientRect.top);
					dc.LineTo(clientRect.left, clientRect.bottom);
					dc.MoveTo(clientRect.right - m_nSideGap, clientRect.top);
					dc.LineTo(clientRect.right - m_nSideGap, clientRect.bottom);
					dc.SelectObject(pOldPen);
				}

				if(m_nItemGap != 0)
				{
					pOldPen = dc.SelectObject(&pen_h);
					dc.MoveTo(clientRect.left, clientRect.top);
					dc.LineTo(clientRect.right, clientRect.top);
					dc.MoveTo(clientRect.left, clientRect.bottom);
					dc.LineTo(clientRect.left, clientRect.bottom);
					dc.SelectObject(pOldPen);
				}
				//dc.Rectangle(clientRect);
			}
		}
		
		//dc.SelectObject(pOldPen);

		//if(lpDrawItemStruct->itemID == 0)
		//	rcItem.top += m_nItemGap + 1;
		//else
		//if(lpDrawItemStruct->itemID != 0)
			rcItem.top += m_nItemGap;

		rcItem.right -= m_nSideGap;
		rcItem.left += m_nSideGap;
		
		//rcItem.DeflateRect(m_nItemGap, m_nItemGap, m_nItemGap, m_nItemGap);
	//}

	dc.FillSolidRect(rcItem, bgClr);

	rcItem.left += m_nMarginLeft;
	rcItem.right -= m_nMarginRight;

	CFont* pOldFont = NULL;
	if(!bIsSelected && m_pFontNormal != NULL)
		pOldFont = (CFont*)dc.SelectObject(m_pFontNormal);
	else if(bIsSelected && m_pFontSelected != NULL)
		pOldFont = (CFont*)dc.SelectObject(m_pFontSelected);

	dc.SetTextColor( fgClr );
    dc.SetBkColor( bgClr );
	dc.DrawText(szText, -1, rcItem, DT_WORDBREAK | DT_EXPANDTABS | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);	
	
	if(pOldFont != NULL)
		dc.SelectObject(pOldFont);

	dc.Detach();
}

void CCustomListBox::SetColors(COLORREF selItemBg, COLORREF selItemFont, COLORREF itemBg, COLORREF itemFont, COLORREF bg, COLORREF border)
{
	m_clrSelectedItemBg = selItemBg;
	m_clrSelectedFont = selItemFont;	
	m_clrNormalItemBg = itemBg;
	m_clrNormalFont = itemFont;
	m_clrBackground = bg;
	m_clrBorder = border;	
}

/*
void CCustomListBox::OnPaint()
{
	CPaintDC dc(this);
	

}
*/