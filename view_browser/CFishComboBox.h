/********************************************************************
	CREATED:	2006/02/23
	CREATED:	23:2:2006   9:31
	FILENAME: 	CFishComboBox.h
	WRITER:		eternalbleu@gmail.com
	
	PURPOSE:
        콘보박스를 구현함.
    REFER:
    HISTORY:    
        2006/02/23 : CREATED BY eternalbleu@gmail.com
*********************************************************************/
#if !defined(AFX_CFISHCOMBOBOX_H__9E5BAFCF_4520_400B_A1B4_26D2EC4773A0__INCLUDED_)
#define AFX_CFISHCOMBOBOX_H__9E5BAFCF_4520_400B_A1B4_26D2EC4773A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CFishComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFishComboBox window
#include "../ctrl/MemDC.h"
#include "../ctrl/CJFlatComboBox.h"
class CFishComboBox : public CCJFlatComboBox
{
public:
    enum {
        FISH_EXPLORER_BTN_WIDTH     =   13,
        FISH_EXPLORER_BTN_HEIGHT    =   12,
        FISH_EXPLORER_BTN_TOP_PAD   =   2,
        FISH_EXPLORER_BTN_WIDTH_RIGHTPAD    =   2,
    };
// Construction
public:
	CFishComboBox();
    BOOL m_bAutoComplete;

// Attributes
public:

// Operations
public:
    void DrawCombo(STATE eState, COLORREF clrTopLeft, COLORREF clrBottomRight, CDC *pPaintDC);
    int  InsertString(int nIndex, LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFishComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFishComboBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnEditUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFISHCOMBOBOX_H__9E5BAFCF_4520_400B_A1B4_26D2EC4773A0__INCLUDED_)
