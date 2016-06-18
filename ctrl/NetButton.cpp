//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!
#include "stdafx.h"
//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!

#include "NetButton.h"
#include "MemDC.h"

#include "../GlobalIconManager.h"

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.10.18	**************************************** //
// *******			**************************************** //
// ********************************************************************* //

// NetButton.cpp : implementation file
//

/////////////////////////////////////////////////////////////////////////////
// CNetDibButton

CNetDibButton::CNetDibButton()
{
	// Initialize the Button Images
	m_bmpButton         = NULL;
	m_bmpButtonDown     = NULL;
	m_bmpButtonFocussed = NULL;
	m_bmpButtonDisabled = NULL;

	m_MouseOnButton = FALSE;

	m_cxIcon = 0;
	m_cyIcon = 0;
	m_hCursor = NULL;

	// Default type is "flat" button
	m_bIsFlat = TRUE; 

	// By default draw border in "flat" button 
	m_bDrawBorder = TRUE; 

	// By default icon is aligned horizontally
	m_nAlign = ST_ALIGN_HORIZ; 

	// By default show the text button
	m_bShowText = FALSE; 

	// By default, for "flat" button, don't draw the focus rect
	m_bDrawFlatFocus = FALSE;

	// By default, for "flat" & "Image" button, don't draw the thin frame rect
	m_bThin = FALSE;

	SetDefaultInactiveBgColor();
	SetDefaultInactiveFgColor();
	SetDefaultActiveBgColor();
	SetDefaultActiveFgColor();
	SetDefaultTransParentColor();
} // End of CNetDibButton

CNetDibButton::~CNetDibButton()
{
	// Destroy the cursor (if any)
	if (m_hCursor != NULL) ::DestroyCursor(m_hCursor);

	// Destroy the Button Images
	if( m_bmpButton != NULL ) delete m_bmpButton;
	if( m_bmpButtonDown != NULL ) delete m_bmpButtonDown;
	if( m_bmpButtonFocussed != NULL ) delete m_bmpButtonFocussed;
	if( m_bmpButtonDisabled != NULL ) delete m_bmpButtonDisabled;

} // End of ~CNetDibButton

BEGIN_MESSAGE_MAP(CNetDibButton, CButton)
    //{{AFX_MSG_MAP(CNetDibButton)
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CNetDibButton::LoadBitmaps(LPTSTR lpszBitmap, LPTSTR lpszBitmapDown, 
							   LPTSTR lpszBitmapFocus, LPTSTR lpszBitmapDisabled,
							   BYTE cx, BYTE cy)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	delete m_bmpButton;
	delete m_bmpButtonDown;
	delete m_bmpButtonFocussed;
	delete m_bmpButtonDisabled;

	if(lpszBitmap==NULL) return FALSE;
	m_bmpButton         = new CNetDib(lpszBitmap);

	if(lpszBitmapDown==NULL)
		m_bmpButtonDown     = new CNetDib(lpszBitmap);
	else
		m_bmpButtonDown     = new CNetDib(lpszBitmapDown);

	if(lpszBitmapFocus==NULL)
		m_bmpButtonFocussed = new CNetDib(lpszBitmapDown);
	else
		m_bmpButtonFocussed = new CNetDib(lpszBitmapFocus);

	if(lpszBitmapDisabled==NULL)
		m_bmpButtonDisabled = new CNetDib(lpszBitmapFocus);
	else
		m_bmpButtonDisabled = new CNetDib(lpszBitmapDisabled);

	m_cxIcon = cx;
	m_cyIcon = cy;

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetDibButton::SetEnableBitmaps(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	delete m_bmpButton;

	if(lpszBitmap==NULL) return FALSE;
	m_bmpButton         = new CNetDib(lpszBitmap);

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetDibButton::SetDownBitmaps(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	delete m_bmpButtonDown;

	if(lpszBitmap==NULL) return FALSE;
	m_bmpButtonDown         = new CNetDib(lpszBitmap);

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetDibButton::SetFocussedBitmaps(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	delete m_bmpButtonFocussed;

	if(lpszBitmap==NULL) return FALSE;
	m_bmpButtonFocussed         = new CNetDib(lpszBitmap);

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetDibButton::SetDisabledBitmaps(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	delete m_bmpButtonDisabled;

	if(lpszBitmap==NULL) return FALSE;
	m_bmpButtonDisabled         = new CNetDib(lpszBitmap);

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetDibButton::SetBtnCursor(int nCursorId)
{
	HINSTANCE hInstResource;
	// Destroy any previous cursor
	if (m_hCursor != NULL) ::DestroyCursor(m_hCursor);
	m_hCursor = NULL;

	// If we want a cursor
	if (nCursorId != -1)
	{
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nCursorId),
											RT_GROUP_CURSOR);
		// Load icon resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource/*AfxGetApp()->m_hInstance*/, MAKEINTRESOURCE(nCursorId), IMAGE_CURSOR, 0, 0, 0);
		// If something wrong then return FALSE
		if (m_hCursor == NULL) return FALSE;
	}

	return TRUE;
} // End of SetBtnCursor

void CNetDibButton::SetFlat(BOOL bState)
{
	m_bIsFlat = bState;
	Invalidate(FALSE); 
} // End of SetFlat

BOOL CNetDibButton::GetFlat()
{
	return m_bIsFlat;
} // End of GetFlat

void CNetDibButton::SetAlign(int nAlign)
{
	switch (nAlign)
	{    
	case ST_ALIGN_HORIZ:
		m_nAlign = ST_ALIGN_HORIZ;   
		break;
	case ST_ALIGN_VERT:
		m_nAlign = ST_ALIGN_VERT;
		break;
	}
	Invalidate(FALSE); 
} // End of SetAlign

int CNetDibButton::GetAlign()
{
	return m_nAlign;
} // End of GetAlign

void CNetDibButton::DrawBorder(BOOL bEnable)
{
	m_bDrawBorder = bEnable;
} // End of DrawBorder

const char* CNetDibButton::GetVersionC()
{
	return "2.3";
} // End of GetVersionC

const short CNetDibButton::GetVersionI()
{
	return 23; // Divide by 10 to get actual version
} // End of GetVersionI

void CNetDibButton::SetShowText(BOOL bShow)
{
	m_bShowText = bShow;
	Invalidate(FALSE); 
} // End of SetShowText

BOOL CNetDibButton::GetShowText()
{
	return m_bShowText;
} // End of GetShowText

void CNetDibButton::SetThin(BOOL bThin)
{
	m_bThin = bThin;
	Invalidate(FALSE); 
} // End of SetThin

BOOL CNetDibButton::GetThin()
{
	return m_bThin;
} // End of GetThin

void CNetDibButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd* pWnd;  // Finestra attiva
	CWnd* pParent; // Finestra che contiene il bottone

	CButton::OnMouseMove(nFlags, point);

	// If the mouse enter the button with the left button pressed
	// then do nothing
	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	pWnd = GetActiveWindow();
	pParent = GetOwner();

	if ((GetCapture() != this) && 
		(
	#ifndef ST_LIKEIE
		pWnd != NULL && 
	#endif
		pParent != NULL)) 
	{
		m_MouseOnButton = TRUE;
		//SetFocus();	// Thanks Ralph!
		SetCapture();
		Invalidate(FALSE); 
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		if (!rc.PtInRect(point))
		{
			// Redraw only if mouse goes out
			if (m_MouseOnButton == TRUE)
			{
			m_MouseOnButton = FALSE;
			Invalidate(FALSE); 
			}
			// If user is NOT pressing left button then release capture!
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}
} // End of OnMouseMove

void CNetDibButton::OnKillFocus(CWnd * pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	if (m_MouseOnButton == TRUE)
	{
		m_MouseOnButton = FALSE;
		Invalidate(FALSE); 
	}
} // End of OnKillFocus

// written by moonknit
// date : created on 2005-01-05
// desc : Kill Focus Forcely
//		If button didn't release focus and so the Image of the button will not be changed,
//		you can use this function for release focused image.
void CNetBitmapButton::KillFocus()
{
	if (m_bIsFlat == FALSE) return;

	if (m_MouseOnButton == TRUE)
	{		
		m_MouseOnButton = FALSE;		
		Invalidate(FALSE);
	}
} // End of KillFocus

void CNetDibButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_MouseOnButton == TRUE)
	{
		ReleaseCapture();
		Invalidate(FALSE); 
	}

	// Call base message handler
	CButton::OnCaptureChanged(pWnd);
} // End of OnCaptureChanged

void CNetDibButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
#ifdef ST_USE_MEMDC
	CDC  *pdrawDC = CDC::FromHandle(lpDIS->hDC);
	CMemDC memDC(pdrawDC);
	CDC  *pDC = &memDC;
#else	
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
#endif

	CPen *pOldPen;
	BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	BOOL bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

	CRect itemRect = lpDIS->rcItem;

	if (m_bIsFlat == FALSE)
	{
		if (bIsFocused)
		{
			CBrush br(RGB(0,0,0));  
			pDC->FrameRect(&itemRect, &br);
			itemRect.DeflateRect(1, 1);
		}
	}

    //////////////////////////////////////////////////////////////////////////
    // 재 그리기시 배경을 지우지 않도록 변경함. - delete by eternalbleu
//	// Prepare draw... paint button's area with background color
//	COLORREF bgColor;
//	if ((m_MouseOnButton == TRUE) || (bIsPressed))
//		bgColor = GetActiveBgColor();
//	else
//		bgColor = GetInactiveBgColor();
//
//	CBrush br(bgColor);
//	pDC->FillRect(&itemRect, &br);

	// Draw pressed button
	if (bIsPressed)
	{
		if (m_bIsFlat == TRUE)
		{
			if (m_bDrawBorder == TRUE)
			{
				CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // Bianco
				CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Grigio scuro

				// Disegno i bordi a sinistra e in alto
				// Dark gray line
				pOldPen = pDC->SelectObject(&penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// White line
				pDC->SelectObject(penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
		}
		else    
		{
			CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(&itemRect, &brBtnShadow);
		}
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat == TRUE)
		{
			if (m_MouseOnButton == TRUE && m_bDrawBorder == TRUE)
			{
				// Disegno i bordi a sinistra e in alto
				// White line
				pOldPen = pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// Dark gray line
				pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
			else if(m_bThin)
			{
				// Disegno i bordi a sinistra e in alto
				// White line
				pOldPen = pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// Dark gray line
				pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
		}
		else
		{
			// Disegno i bordi a sinistra e in alto
			// White line
			pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.left, itemRect.top);
			pDC->LineTo(itemRect.right, itemRect.top);
			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(itemRect.left+1, itemRect.bottom-1);
			pDC->LineTo(itemRect.left+1, itemRect.top+1);
			pDC->LineTo(itemRect.right, itemRect.top+1);
			// Disegno i bordi a destra e in basso
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.top-1);
			// Dark gray line
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(itemRect.left+1, itemRect.bottom-2);
			pDC->LineTo(itemRect.right-2, itemRect.bottom-2);
			pDC->LineTo(itemRect.right-2, itemRect.top);
			//
			pDC->SelectObject(pOldPen);
		}
	}

	// Read the button title
	CString sTitle;
	GetWindowText(sTitle);

	// If we don't want the title displayed
	if (m_bShowText == FALSE) sTitle.Empty();

	CRect captionRect = lpDIS->rcItem;

	// Draw the icon
	DrawTheIcon(pDC, &sTitle, &lpDIS->rcItem, &captionRect, bIsPressed, bIsFocused, bIsDisabled);

	// Write the button title (if any)
	if (sTitle.IsEmpty() == FALSE)
	{
		// Disegno la caption del bottone
		// Se il bottone e' premuto muovo la captionRect di conseguenza
		if (bIsPressed)
		captionRect.OffsetRect(1, 1);

		// ONLY FOR DEBUG 
		// Evidenzia il rettangolo in cui verra' centrata la caption 
		//CBrush brBtnShadow(RGB(255, 0, 0));
		//pDC->FrameRect(&captionRect, &brBtnShadow);

#ifdef ST_USE_MEMDC
		// Get dialog's font
		CFont *pCurrentFont = GetFont(); 
		CFont *pOldFont = pDC->SelectObject(pCurrentFont);
#endif
		if ((m_MouseOnButton == TRUE) || (bIsPressed)) 
		{
			pDC->SetTextColor(GetActiveFgColor());
			pDC->SetBkColor(GetActiveBgColor());
		} 
		else 
		{
			pDC->SetTextColor(GetInactiveFgColor());
			pDC->SetBkColor(GetInactiveBgColor());
		}

		// Center text
		CRect centerRect = captionRect;
		pDC->DrawText(sTitle, -1, captionRect, DT_SINGLELINE|DT_CALCRECT);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);
		/* RFU
		captionRect.OffsetRect(0, (centerRect.Height() - captionRect.Height())/2);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())-4, (centerRect.Height() - captionRect.Height())/2);
		*/

		pDC->DrawState(captionRect.TopLeft(), captionRect.Size(), (LPCTSTR)sTitle, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), 
		TRUE, 0, (CBrush*)NULL);
#ifdef ST_USE_MEMDC
		pDC->SelectObject(pOldFont);
#endif
	}

	// mod by kahi 2000/10/18
//	if (m_bIsFlat == FALSE || (m_bIsFlat == TRUE && m_bDrawFlatFocus == TRUE))
	if (m_bDrawFlatFocus == TRUE)
	{
		// Draw the focus rect
		if (bIsFocused)
		{
			CRect focusRect = itemRect;
			focusRect.DeflateRect(3, 3);
			pDC->DrawFocusRect(&focusRect);
		}
	}
} // End of DrawItem


void CNetDibButton::DrawTheIcon(CDC* pDC, CString* title, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL IsFocused, BOOL IsDisabled)
{
	CRect iconRect = rcItem;

	switch (m_nAlign)
	{
	case ST_ALIGN_HORIZ:
		if (title->IsEmpty())
		{
			// Center the icon horizontally
			iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
		}
		else
		{
			// L'icona deve vedersi subito dentro il focus rect
			iconRect.left += 3;  
			captionRect->left += m_cxIcon + 3;
		}
		// Center the icon vertically
		iconRect.top += ((iconRect.Height() - m_cyIcon)/2);
		break;
	case ST_ALIGN_VERT:
		// Center the icon horizontally
		iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
		if (title->IsEmpty())
		{
			// Center the icon vertically
			iconRect.top += ((iconRect.Height() - m_cyIcon)/2);           
		}
		else
		{
			captionRect->top += m_cyIcon;
		}
		break;
	}
    
	CBrush br(GetActiveBgColor());  

	// If button is pressed then press the icon also
	if (IsPressed) iconRect.OffsetRect(1, 1);
	// Ole'!
	if(IsDisabled)
	{
		m_bmpButtonDisabled->PaintImage(iconRect.left, iconRect.top, pDC->m_hDC, GetTransParentColor());
	}
	else
	{
		if(m_MouseOnButton == TRUE)
		{
			if(IsPressed)
			{
				m_bmpButtonDown->PaintImage(iconRect.left, iconRect.top, pDC->m_hDC, GetTransParentColor());
			}
			else
				m_bmpButtonFocussed->PaintImage(iconRect.left, iconRect.top, pDC->m_hDC, GetTransParentColor());
		}
		else
			m_bmpButton->PaintImage(iconRect.left, iconRect.top, pDC->m_hDC, GetTransParentColor());
	}
} // End of DrawTheIcon

void CNetDibButton::PreSubclassWindow() 
{
	// Add BS_OWNERDRAW style
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
	CButton::PreSubclassWindow();
} // End of PreSubclassWindow

void CNetDibButton::SetDefaultInactiveBgColor(BOOL bRepaint)
{
	m_crInactiveBg = ::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultInactiveBgColor

void CNetDibButton::SetInactiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveBg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetInactiveBgColor

const COLORREF CNetDibButton::GetInactiveBgColor()
{
	return m_crInactiveBg;
} // End of GetInactiveBgColor

void CNetDibButton::SetDefaultInactiveFgColor(BOOL bRepaint)
{
	m_crInactiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultInactiveFgColor

void CNetDibButton::SetInactiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveFg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetInactiveFgColor

const COLORREF CNetDibButton::GetInactiveFgColor()
{
	return m_crInactiveFg;
} // End of GetInactiveFgColor

void CNetDibButton::SetDefaultActiveBgColor(BOOL bRepaint)
{
	m_crActiveBg = ::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultActiveBgColor

void CNetDibButton::SetActiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveBg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetActiveBgColor

const COLORREF CNetDibButton::GetActiveBgColor()
{
	return m_crActiveBg;
} // End of GetActiveBgColor

void CNetDibButton::SetDefaultActiveFgColor(BOOL bRepaint)
{
	m_crActiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultActiveFgColor

void CNetDibButton::SetActiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveFg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetActiveFgColor

const COLORREF CNetDibButton::GetActiveFgColor()
{
	return m_crActiveFg;
} // End of GetActiveFgColor

void CNetDibButton::SetDefaultTransParentColor(BOOL bRepaint)
{
	m_crTransParent = RGB(0,255,0); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultTransParentColor

void CNetDibButton::SetTransParentColor(COLORREF crNew, BOOL bRepaint)
{
	m_crTransParent = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetTransParentColor

const COLORREF CNetDibButton::GetTransParentColor()
{
	return m_crTransParent;
} // End of GetTransParentColor

void CNetDibButton::SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	
	// Repaint the button
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetFlatFocus

BOOL CNetDibButton::GetFlatFocus()
{
	return m_bDrawFlatFocus;
} // End of GetFlatFocus

BOOL CNetDibButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// If a cursor was specified then use it!
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
} // End of OnSetCursor

/////////////////////////////////////////////////////////////////////////////
// CNetBitmapButton

CNetBitmapButton::CNetBitmapButton()
{
	// Initialize the Button Images
	m_MouseOnButton = FALSE;
    m_bFreezeDrawDownBitmap =   FALSE;

	m_cxIcon = 0;
	m_cyIcon = 0;
	m_hCursor = NULL;

	// Default type is "flat" button
	m_bIsFlat = TRUE; 

	// By default draw border in "flat" button 
	m_bDrawBorder = FALSE; 

	// By default icon is aligned horizontally
	m_nAlign = ST_ALIGN_HORIZ; 

	// By default show the text button
	m_bShowText = TRUE; 

	// By default, for "flat" button, don't draw the focus rect
	m_bDrawFlatFocus = FALSE;

	// By default, for "flat" & "Image" button, don't draw the thin frame rect
	m_bThin = FALSE;

	m_ButtonSelected = FALSE;

	SetDefaultInactiveBgColor();
	SetDefaultInactiveFgColor();
	SetDefaultActiveBgColor();
	SetDefaultActiveFgColor();
	SetDefaultTransParentColor();
	SetDefaulTextFont();  
} // End of CNetBitmapButton

CNetBitmapButton::~CNetBitmapButton()
{
	// Destroy the cursor (if any)
	if (m_hCursor != NULL) ::DestroyCursor(m_hCursor);

} // End of ~CNetBitmapButton

BEGIN_MESSAGE_MAP(CNetBitmapButton, CButton)
    //{{AFX_MSG_MAP(CNetBitmapButton)
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CNetBitmapButton::LoadBitmaps(LPTSTR lpszBitmap, LPTSTR lpszBitmapDown, 
							   LPTSTR lpszBitmapFocus, LPTSTR lpszBitmapDisabled,
							   BYTE cx, BYTE cy)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButton.DeleteObject();
	m_bmpButtonDown.DeleteObject();
	m_bmpButtonFocussed.DeleteObject();
	m_bmpButtonDisabled.DeleteObject();

	if (!m_bmpButton.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	if (lpszBitmapDown!=NULL)
	{
		if (!m_bmpButtonDown.LoadBitmap(lpszBitmapDown))
		{
			TRACE0("Failed to load down bitmap of bitmap button\n");
			return bAllLoaded=FALSE;
		}
	}
	
	if (lpszBitmapFocus!=NULL)
	{
		if (!m_bmpButtonFocussed.LoadBitmap(lpszBitmapFocus))
		{
			TRACE0("Failed to load focussed bitmap of bitmap button\n");
			return bAllLoaded=FALSE;
		}
	}
	
	if (lpszBitmapDisabled!=NULL)
	{
		if (!m_bmpButtonDisabled.LoadBitmap(lpszBitmapDisabled))
		{
			TRACE0("Failed to load disabled bitmap of bitmap button\n");
			return bAllLoaded=FALSE;
		}
	}

    SetSize(cx, cy);

	RedrawWindow();
	return bAllLoaded;
}

BOOL CNetBitmapButton::LoadBitmaps(UINT nBitmap, UINT nBitmapDown, 
							   UINT nBitmapFocus, UINT nBitmapDisabled, 
							   BYTE cx, BYTE cy)
{
	return LoadBitmaps(MAKEINTRESOURCE(nBitmap),
						MAKEINTRESOURCE(nBitmapDown),
						MAKEINTRESOURCE(nBitmapFocus),
						MAKEINTRESOURCE(nBitmapDisabled), cx, cy);
}

BOOL CNetBitmapButton::SetEnableBitmap(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButton.DeleteObject();
	
	if (!m_bmpButton.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetEnableBitmap(CBitmap* BMP)
{
    if (!BMP)   return FALSE;
    
    BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButton.DeleteObject();
	
    //m_bmpButton.m_hObject   =   CopyImage(BMP, IMAGE_BITMAP, bm.bmWidth, bm.bmHeight, LR_COPYFROMRESOURCE);
    CFishBMPManager::copyBMPtoBMP(BMP, &m_bmpButton);

    BITMAP bm;
    m_bmpButton.GetBitmap(&bm);
    SetSize(bm.bmWidth, bm.bmHeight);
    
	if (!m_bmpButton.m_hObject)
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetEnableBitmap(UINT nBitmap)
{
	return SetEnableBitmap(MAKEINTRESOURCE(nBitmap));
}

BOOL CNetBitmapButton::SetDownBitmap(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonDown.DeleteObject();
	
	if (!m_bmpButtonDown.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetDownBitmap(CBitmap* BMP)
{
    if (!BMP)   return FALSE;

    BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonDown.DeleteObject();
	
    CFishBMPManager::copyBMPtoBMP(BMP, &m_bmpButtonDown);

    if (!m_bmpButtonDown.m_hObject)
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetDownBitmap(UINT nBitmap)
{
	return SetDownBitmap(MAKEINTRESOURCE(nBitmap));
}

BOOL CNetBitmapButton::SetFocussedBitmap(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonFocussed.DeleteObject();
	
	if (!m_bmpButtonFocussed.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetFocussedBitmap(CBitmap* BMP)
{
    if (!BMP)   return FALSE;

	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonFocussed.DeleteObject();
	
    CFishBMPManager::copyBMPtoBMP(BMP, &m_bmpButtonFocussed);

	if (!m_bmpButtonFocussed.m_hObject)
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetFocussedBitmap(UINT nBitmap)
{
	return SetFocussedBitmap(MAKEINTRESOURCE(nBitmap));
}

BOOL CNetBitmapButton::SetDisabledBitmap(LPTSTR lpszBitmap)
{
	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonDisabled.DeleteObject();
	
	if (!m_bmpButtonDisabled.LoadBitmap(lpszBitmap))
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetDisabledBitmap(CBitmap* BMP)
{
    if (!BMP)   return FALSE;

	BOOL bAllLoaded=TRUE;

	//Delete old ones
	m_bmpButtonDisabled.DeleteObject();
	
    CFishBMPManager::copyBMPtoBMP(BMP, &m_bmpButtonDisabled);

    if (!m_bmpButtonDisabled.m_hObject)
	{
		TRACE0("Failed to load up bitmap of bitmap button\n");
		return FALSE;
	}

	return bAllLoaded;
}

BOOL CNetBitmapButton::SetDisabledBitmap(UINT nBitmap)
{
	return SetDisabledBitmap(MAKEINTRESOURCE(nBitmap));
}

BOOL CNetBitmapButton::SetBtnCursor(int nCursorId)
{
	HINSTANCE hInstResource;
	// Destroy any previous cursor
	if (m_hCursor != NULL) ::DestroyCursor(m_hCursor);
	m_hCursor = NULL;

	// If we want a cursor
	if (nCursorId != -1)
	{
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nCursorId),
											RT_GROUP_CURSOR);
		// Load icon resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource/*AfxGetApp()->m_hInstance*/, MAKEINTRESOURCE(nCursorId), IMAGE_CURSOR, 0, 0, 0);
		// If something wrong then return FALSE
		if (m_hCursor == NULL) return FALSE;
	}

	return TRUE;
} // End of SetBtnCursor

void CNetBitmapButton::SetFlat(BOOL bState)
{
	m_bIsFlat = bState;
	Invalidate(FALSE); 
} // End of SetFlat


BOOL CNetBitmapButton::GetFlat()
{
	return m_bIsFlat;
} // End of GetFlat

void CNetBitmapButton::SetAlign(int nAlign)
{
	switch (nAlign)
	{    
	case ST_ALIGN_HORIZ:
		m_nAlign = ST_ALIGN_HORIZ;
		break;
	case ST_ALIGN_VERT:
		m_nAlign = ST_ALIGN_VERT;
		break;
	}
	Invalidate(FALSE); 
} // End of SetAlign

int CNetBitmapButton::GetAlign()
{
	return m_nAlign;
} // End of GetAlign

BOOL CNetBitmapButton::SetTextFont(LPCTSTR lpFontName)
{
	BOOL bRet;
	m_font.DeleteObject();
	
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

	_tcscpy(metrics.lfStatusFont.lfFaceName, lpFontName);
    m_nFontHeight = metrics.lfStatusFont.lfHeight;
	bRet = m_font.CreateFontIndirect(&metrics.lfStatusFont);
	Invalidate(FALSE); 
	return bRet;
}

void CNetBitmapButton::SetDefaulTextFont()
{
	// Create a font twice as tall as the system status bar font
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

        m_nFontHeight = metrics.lfStatusFont.lfHeight ;
	VERIFY(m_font.CreateFontIndirect(&metrics.lfStatusFont));
}

void CNetBitmapButton::DrawBorder(BOOL bEnable)
{
	m_bDrawBorder = bEnable;
} // End of DrawBorder

const char* CNetBitmapButton::GetVersionC()
{
	return "2.3";
} // End of GetVersionC

const short CNetBitmapButton::GetVersionI()
{
	return 23; // Divide by 10 to get actual version
} // End of GetVersionI

void CNetBitmapButton::SetShowText(BOOL bShow)
{
	m_bShowText = bShow;
	Invalidate(FALSE); 
} // End of SetShowText

BOOL CNetBitmapButton::GetShowText()
{
	return m_bShowText;
} // End of GetShowText

void CNetBitmapButton::SetThin(BOOL bThin)
{
	m_bThin = bThin;
	Invalidate(FALSE); 
} // End of SetThin

BOOL CNetBitmapButton::GetThin()
{
	return m_bThin;
} // End of GetThin

void CNetBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
//	TRACE("@@@@  OnLButtonMove\n");
	CWnd* pWnd;  // Finestra attiva
	CWnd* pParent; // Finestra che contiene il bottone

	CButton::OnMouseMove(nFlags, point);

	// If the mouse enter the button with the left button pressed
	// then do nothing
//	TRACE(" nFlags = %d   nMouseOnBtn = %d \n", nFlags, (m_MouseOnButton)? 1: 0 );
	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

	// If our button is not flat then do nothing
	// add by kahi 2000/04/04
	//return;	// 아무것도 안한다.

	
	if (m_bIsFlat == FALSE) return;


	pWnd = GetActiveWindow();
	pParent = GetOwner();

	if ((GetCapture() != this ) && 
		(	
#ifndef ST_LIKEIE
		pWnd != NULL && 
#endif
		pParent != NULL)) 
	{
		m_MouseOnButton = TRUE;
		//SetFocus();	// Thanks Ralph!

//		TRACE(" nMouseOnBtn = %d \n",  (m_MouseOnButton)? 1: 0 );
		 
		SetCapture();
		Invalidate(FALSE);
//		TRACE(_T("mouse newly in\r\n"));
 
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);
		if (!rc.PtInRect(point))
		{
			// Redraw only if mouse goes out
			if (m_MouseOnButton == TRUE)
			{
				
				m_MouseOnButton = FALSE;
				Invalidate(FALSE); 
//				TRACE(_T("mouse out\r\n"));
			}
			// If user is NOT pressing left button then release capture!
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}	

 
} // End of OnMouseMove

void CNetBitmapButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_MouseOnButton == TRUE)
	{
		ReleaseCapture();
		Invalidate(FALSE); 
	}

	// Call base message handler
	CButton::OnCaptureChanged(pWnd);
} // End of OnCaptureChanged

void CNetBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
#ifdef ST_USE_MEMDC
	CDC  *pdrawDC = CDC::FromHandle(lpDIS->hDC);
	CMemDC memDC(pdrawDC);
	CDC  *pDC = &memDC;
#else	
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
#endif

	CPen *pOldPen;
	BOOL bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
	// modified by moonknit 2006-03-08
//	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS) || m_ButtonSelected;
	BOOL bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

	CRect itemRect = lpDIS->rcItem;

	if (m_bIsFlat == FALSE)
	{
		if (bIsFocused)
		{
			CBrush br(RGB(0,0,0));  
			pDC->FrameRect(&itemRect, &br);
			itemRect.DeflateRect(1, 1);
		}
	}

    //////////////////////////////////////////////////////////////////////////
    // 재 그리기시 배경을 지우지 않도록 변경함. - delete by eternalbleu
	// Prepare draw... paint button's area with background color
    /*
		COLORREF bgColor;
		if ((m_MouseOnButton == TRUE) || (bIsPressed))
			bgColor = GetActiveBgColor();
		else
			bgColor = GetInactiveBgColor();
	
		CBrush br(bgColor);
		pDC->FillRect(&itemRect, &br);
        */

	// Draw pressed button
	if (bIsPressed)
	{
		if (m_bIsFlat == TRUE)
		{
			if (m_bDrawBorder == TRUE)
			{
				CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // Bianco
				CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Grigio scuro

				// Disegno i bordi a sinistra e in alto
				// Dark gray line
				pOldPen = pDC->SelectObject(&penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// White line
				pDC->SelectObject(penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
		}
		else    
		{
			CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(&itemRect, &brBtnShadow);
		}
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat == TRUE)
		{
			if (m_MouseOnButton == TRUE && m_bDrawBorder == TRUE)
			{
				// Disegno i bordi a sinistra e in alto
				// White line
				pOldPen = pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// Dark gray line
				pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
			else if(m_bThin)
			{
				// Disegno i bordi a sinistra e in alto
				// White line
				pOldPen = pDC->SelectObject(&penBtnHiLight);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.left, itemRect.top);
				pDC->LineTo(itemRect.right, itemRect.top);
				// Disegno i bordi a destra e in basso
				// Dark gray line
				pDC->SelectObject(penBtnShadow);
				pDC->MoveTo(itemRect.left, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
				pDC->LineTo(itemRect.right-1, itemRect.top-1);
				//
				pDC->SelectObject(pOldPen);
			}
		}
		else
		{
			// Disegno i bordi a sinistra e in alto
			// White line
			pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.left, itemRect.top);
			pDC->LineTo(itemRect.right, itemRect.top);
			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(itemRect.left+1, itemRect.bottom-1);
			pDC->LineTo(itemRect.left+1, itemRect.top+1);
			pDC->LineTo(itemRect.right, itemRect.top+1);
			// Disegno i bordi a destra e in basso
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(itemRect.left, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.bottom-1);
			pDC->LineTo(itemRect.right-1, itemRect.top-1);
			// Dark gray line
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(itemRect.left+1, itemRect.bottom-2);
			pDC->LineTo(itemRect.right-2, itemRect.bottom-2);
			pDC->LineTo(itemRect.right-2, itemRect.top);
			//
			pDC->SelectObject(pOldPen);
		}
	}

	// Read the button title
	CString sTitle;
	GetWindowText(sTitle);

	// If we don't want the title displayed
	if (m_bShowText == FALSE) sTitle.Empty();

	//CRect captionRect = lpDIS->rcItem;

    // added by eternalbleu 2005/01/13
    CRect captionRect;
    GetClientRect(&captionRect);

	// Draw the icon
	DrawTheIcon(pDC, &CString(""), &lpDIS->rcItem, &captionRect, bIsPressed, bIsFocused, bIsDisabled);

	// Write the button title (if any)
	if (sTitle.IsEmpty() == FALSE)
	{
		// Disegno la caption del bottone
		// Se il bottone e' premuto muovo la captionRect di conseguenza
		if (bIsPressed)
		captionRect.OffsetRect(1, 1);

		// ONLY FOR DEBUG 
		// Evidenzia il rettangolo in cui verra' centrata la caption 
		//CBrush brBtnShadow(RGB(255, 0, 0));
		//pDC->FrameRect(&captionRect, &brBtnShadow);

		CFont* pOldFont = NULL;
#ifdef ST_USE_MEMDC
		// Get dialog's font
		CFont *pCurrentFont = GetFont(); 
		pOldFont = pDC->SelectObject(pCurrentFont);
#endif
        if (m_nBkMode   ==  TRANSPARENT)
        {
            pDC->SetBkMode(m_nBkMode);
        } 
        else if ((m_MouseOnButton == TRUE) || (bIsPressed)) 
		{
			pDC->SetTextColor(GetActiveFgColor());
			pDC->SetBkColor(GetActiveBgColor());
		} 
		else 
		{
			pDC->SetTextColor(GetInactiveFgColor());
			pDC->SetBkColor(GetInactiveBgColor());
		}

		// Center text
		CRect centerRect = captionRect;
		pDC->DrawText(sTitle, -1, captionRect, DT_SINGLELINE|DT_CALCRECT);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())/2, (centerRect.Height() - captionRect.Height())/2);
		/*
		captionRect.OffsetRect(0, (centerRect.Height() - captionRect.Height())/2);
		captionRect.OffsetRect((centerRect.Width() - captionRect.Width())-4, (centerRect.Height() - captionRect.Height())/2);
		*/

		pDC->DrawState(captionRect.TopLeft(), captionRect.Size(), (LPCTSTR)sTitle, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL), 
			   TRUE, 0, (CBrush*)NULL);

		// add by kahi 2000/04/04
		pDC->SelectObject(pOldFont);

#ifdef ST_USE_MEMDC
		pDC->SelectObject(pOldFont);
#endif
	}

	// mod by kahi 2000/10/18
//	if (m_bIsFlat == FALSE || (m_bIsFlat == TRUE && m_bDrawFlatFocus == TRUE))
	if (m_bDrawFlatFocus == TRUE)
	{
		// Draw the focus rect
		if (bIsFocused)
		{
			CRect focusRect = itemRect;
//			focusRect.DeflateRect(3, 3);
			pDC->DrawFocusRect(&focusRect);
		}
	}
} // End of DrawItem

void CNetBitmapButton::DrawTheIcon(CDC* pDC, CString* title, RECT* rcItem, CRect* captionRect, BOOL IsPressed, BOOL IsFocused, BOOL IsDisabled)
{
	CRect iconRect = rcItem;

	switch (m_nAlign)
	{
	case ST_ALIGN_HORIZ:
		if (title->IsEmpty())
		{
			// Center the icon horizontally
			iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
		}
		else
		{
			// L'icona deve vedersi subito dentro il focus rect
			iconRect.left += 3;  
			captionRect->left += m_cxIcon + 3;
		}
		// Center the icon vertically
		iconRect.top += ((iconRect.Height() - m_cyIcon)/2);
		break;
	case ST_ALIGN_VERT:
		// Center the icon horizontally
		iconRect.left += ((iconRect.Width() - m_cxIcon)/2);
		if (title->IsEmpty())
		{
			// Center the icon vertically
			iconRect.top += ((iconRect.Height() - m_cyIcon)/2);           
		}
		else
		{
			captionRect->top += m_cyIcon;
		}
		break;
	}
    
	CBrush br(GetActiveBgColor());  

	// If button is pressed then press the icon also
	// if (m_bIsFlat == FALSE || (m_bIsFlat == TRUE && m_bDrawFlatFocus == TRUE))
    //    if (IsPressed) iconRect.OffsetRect(1, 1); //주석처리 이유 : 버튼을 누를때 생기는 외곽선을 없애기 위해 
	// Ole'!
	if(IsDisabled)
	{
	  if(!m_bmpButtonDisabled.GetSafeHandle()) return;
	  m_bmpButtonDisabled.DrawTransparent(pDC,iconRect.left, iconRect.top, m_cxIcon, m_cyIcon, GetTransParentColor());
	}
	else
	{
		if(m_MouseOnButton == TRUE || m_bFreezeDrawDownBitmap == TRUE)
		{
			if(IsPressed || m_bFreezeDrawDownBitmap == TRUE)
			{
				if(!m_bmpButtonDown.GetSafeHandle()) return;
				m_bmpButtonDown.DrawTransparent(pDC,iconRect.left, iconRect.top, m_cxIcon, m_cyIcon,GetTransParentColor());
			}
			else
			{
				if(!m_bmpButtonFocussed.GetSafeHandle()) return;
				m_bmpButtonFocussed.DrawTransparent(pDC,iconRect.left, iconRect.top, m_cxIcon, m_cyIcon, GetTransParentColor());
			}
		}
		else
		{
			if(!m_bmpButton.GetSafeHandle()) return;
			m_bmpButton.DrawTransparent(pDC,iconRect.left, iconRect.top, m_cxIcon, m_cyIcon, GetTransParentColor());
		}
	}
} // End of DrawTheIcon

void CNetBitmapButton::PreSubclassWindow() 
{
	// Add BS_OWNERDRAW style
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
	CButton::PreSubclassWindow();
} // End of PreSubclassWindow

void CNetBitmapButton::SetDefaultInactiveBgColor(BOOL bRepaint)
{
	m_crInactiveBg = FISH_BUTTON_BG_COLORREF;//::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultInactiveBgColor

void CNetBitmapButton::SetInactiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveBg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetInactiveBgColor

const COLORREF CNetBitmapButton::GetInactiveBgColor()
{
	return m_crInactiveBg;
} // End of GetInactiveBgColor

void CNetBitmapButton::SetDefaultInactiveFgColor(BOOL bRepaint)
{
	m_crInactiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultInactiveFgColor

void CNetBitmapButton::SetInactiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crInactiveFg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetInactiveFgColor

const COLORREF CNetBitmapButton::GetInactiveFgColor()
{
	return m_crInactiveFg;
} // End of GetInactiveFgColor

void CNetBitmapButton::SetDefaultActiveBgColor(BOOL bRepaint)
{
	m_crActiveBg = FISH_BUTTON_BG_COLORREF;//::GetSysColor(COLOR_BTNFACE); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultActiveBgColor

void CNetBitmapButton::SetActiveBgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveBg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetActiveBgColor

const COLORREF CNetBitmapButton::GetActiveBgColor()
{
	return m_crActiveBg;
} // End of GetActiveBgColor

void CNetBitmapButton::SetDefaultActiveFgColor(BOOL bRepaint)
{
	m_crActiveFg = ::GetSysColor(COLOR_BTNTEXT); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultActiveFgColor

void CNetBitmapButton::SetActiveFgColor(COLORREF crNew, BOOL bRepaint)
{
	m_crActiveFg = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetActiveFgColor

const COLORREF CNetBitmapButton::GetActiveFgColor()
{
	return m_crActiveFg;
} // End of GetActiveFgColor

void CNetBitmapButton::SetDefaultTransParentColor(BOOL bRepaint)
{
	m_crTransParent = RGB(0,255,0); 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetDefaultTransParentColor

void CNetBitmapButton::SetTransParentColor(COLORREF crNew, BOOL bRepaint)
{
	m_crTransParent = crNew; 
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE); 
	}
} // End of SetTransParentColor

const COLORREF CNetBitmapButton::GetTransParentColor()
{
	return m_crTransParent;
} // End of GetTransParentColor

void CNetBitmapButton::SetFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	
	// Repaint the button
	if (bRepaint == TRUE) 
	{
		Invalidate(FALSE);
	}
} // End of SetFlatFocus

BOOL CNetBitmapButton::GetFlatFocus()
{
	return m_bDrawFlatFocus;
} // End of GetFlatFocus

BOOL CNetBitmapButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// If a cursor was specified then use it!
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);
} // End of OnSetCursor

#undef ST_USE_MEMDC
#undef ST_LIKE


void CNetBitmapButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	TRACE("@@@@  OnLButtonUp\n");
	CButton::OnLButtonUp(nFlags, point);
 
//	KillFocus();//lizzy
}


void CNetBitmapButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	TRACE("@@@@  OnLButtonDown\n"); 
	SetFocus();
	CButton::OnLButtonDown(nFlags, point);
}

//Added by lizzy 2005-08-26
void CNetBitmapButton::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	//bShow == 1 : Show, 0 == hide
	if( bShow == 1 )
	{			
		CButton::OnShowWindow(bShow, nStatus);
//		Invalidate(FALSE);
		UpdateWindow();	
	}
	else
	{
		CButton::OnShowWindow(bShow, nStatus);
	}
	
}

BOOL CNetBitmapButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}

void CNetBitmapButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here

    if (m_MouseOnButton == FALSE)
    {
		// modified by moonknit 2006-03-08
		// want to make selected button state

//      m_MouseOnButton =   TRUE;
//		Invalidate(TRUE;
		if(!m_ButtonSelected)
			SelectButton();
	}
    
}

// written by moonknit 2006-03-08
// add this code to make selected button state, not fucused button state
void CNetBitmapButton::SelectButton()
{
	if(!m_ButtonSelected)
	{
		m_ButtonSelected = TRUE; 
		Invalidate(TRUE);
	}
}

void CNetBitmapButton::OnKillFocus(CWnd * pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	if(m_ButtonSelected) m_ButtonSelected = FALSE;

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	if (m_MouseOnButton == TRUE)
	{
		m_MouseOnButton = FALSE;
		Invalidate(FALSE); 
	}
	
} // End of OnKillFocus