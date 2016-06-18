// CFishComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "..\fish.h"
#include "CFishComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishComboBox

CFishComboBox::CFishComboBox()
{
    m_btnComboN.DeleteObject();
    m_btnComboO.DeleteObject();
    m_btnComboP.DeleteObject();

    m_btnComboN.LoadBitmap(IDB_EXPLORERBAR_COMBO_BTN);
    m_btnComboO.LoadBitmap(IDB_EXPLORERBAR_COMBO_BTN);
    m_btnComboP.LoadBitmap(IDB_EXPLORERBAR_COMBO_BTN);
    m_btnComboO.GetBitmap(&m_bitmapDlg);
    
    m_bAutoComplete =    TRUE;
}

CFishComboBox::~CFishComboBox()
{
}


BEGIN_MESSAGE_MAP(CFishComboBox, CCJFlatComboBox)
	//{{AFX_MSG_MAP(CFishComboBox)
	ON_WM_ERASEBKGND()
//    ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishComboBox message handlers
BOOL CFishComboBox::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        m_bAutoComplete = TRUE;

        int nVirtKey = (int) pMsg->wParam;
        if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
            m_bAutoComplete = FALSE;
    }
	return CComboBox::PreTranslateMessage(pMsg);
}

BOOL CFishComboBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return CCJFlatComboBox::OnEraseBkgnd(pDC);
}

void CFishComboBox::DrawCombo(STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight, CDC *pPaintDC)
{
	CRect clientRT;
	GetClientRect(&clientRT);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pPaintDC);

	if (!IsWindowEnabled()) {
		return;
	}

	//Test by lizzy
	CRect rectTest;
	rectTest = clientRT;
	rectTest.top += 2;

    CBitmap	*pOldBmp = NULL;
	switch (eState)
	{
	case normal:
        {
		    pOldBmp = (CBitmap *)MemDC.SelectObject(m_btnComboN);
		    pPaintDC->BitBlt(clientRT.right - m_bitmapDlg.bmWidth - 2 , rectTest.top + FISH_EXPLORER_BTN_TOP_PAD, m_bitmapDlg.bmWidth, m_bitmapDlg.bmHeight, &MemDC, 0, 0, SRCCOPY);		
        }
		break;

	case raised:
        {
		    pOldBmp = (CBitmap *)MemDC.SelectObject(m_btnComboO);
		    pPaintDC->BitBlt(clientRT.right - m_bitmapDlg.bmWidth - 2 , rectTest.top + FISH_EXPLORER_BTN_TOP_PAD, m_bitmapDlg.bmWidth, m_bitmapDlg.bmHeight, &MemDC, 0, 0, SRCCOPY);
        }
		break;

	case pressed:
        {
		    pOldBmp = (CBitmap *)MemDC.SelectObject(m_btnComboP);
		    pPaintDC->BitBlt(clientRT.right - m_bitmapDlg.bmWidth - 2, rectTest.top + FISH_EXPLORER_BTN_TOP_PAD, m_bitmapDlg.bmWidth, m_bitmapDlg.bmHeight, &MemDC, 0, 0, SRCCOPY);
        }
		break;
	}
//    if (pOldBmp->m_hObject) MemDC.SelectObject(pOldBmp);

	CRect rectStr;
	rectStr = clientRT;
	rectStr.left += 3;
	rectStr.top += 3;
	rectStr.bottom -= 3;
	CRect rectBtn;
	rectBtn = clientRT;
	rectBtn.left = rectBtn.right - m_bitmapDlg.bmWidth + 1;

	 
	CRgn rgnClipping[2];
	rgnClipping[0].CreateRectRgnIndirect(&rectStr);
	rgnClipping[1].CreateRectRgnIndirect(&rectBtn);
	pPaintDC->SelectClipRgn(&rgnClipping[0], RGN_DIFF);
	pPaintDC->SelectClipRgn(&rgnClipping[1], RGN_DIFF);

	MemDC.DeleteDC();
} 

void CFishComboBox::OnEditUpdate() 
{
    // if we are not to auto update the text, get outta here
    if (!m_bAutoComplete) 
        return;
    
    // Get the text in the edit box
    CString str;
    GetWindowText(str);
    int nLength = str.GetLength();
    
    // Currently selected range
    DWORD dwCurSel = GetEditSel();
    WORD dStart = LOWORD(dwCurSel);
    WORD dEnd   = HIWORD(dwCurSel);
    
    // Search for, and select in, and string in the combo box that is prefixed
    // by the text in the edit box
    if (SelectString(-1, str) == CB_ERR)
    {
        SetWindowText(str);            // No text selected, so restore what 
        // was there before
        if (dwCurSel != CB_ERR)
            SetEditSel(dStart, dEnd);    //restore cursor postion
    }
    
    // Set the text selection as the additional text that we have added
    if (dEnd < nLength && dwCurSel != CB_ERR)
        SetEditSel(dStart, dEnd);
    else
        SetEditSel(nLength, -1);
    
    CFishComboBox::OnEditUpdate();
}

int  CFishComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
    return CComboBox::InsertString(nIndex, lpszString);
}