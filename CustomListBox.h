#if !defined(AFX_CUSTOMLISTBOX_H__83887B9F_8592_4DC5_8FF9_067DC6D668A4__INCLUDED_)
#define AFX_CUSTOMLISTBOX_H__83887B9F_8592_4DC5_8FF9_067DC6D668A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomListBox window

class CCustomListBox : public CListBox
{
// Construction
public:
	CCustomListBox();

// Attributes
public:

// Operations
public:
	void SetColors(COLORREF selItemBg, COLORREF selItemFont, COLORREF itemBg, COLORREF itemFont, COLORREF bg, COLORREF border);
	void SetClrBorder(COLORREF color) { m_clrBorder = color; }
	void SetClrBackground(COLORREF color) { m_clrBackground = color; }
	void SetClrNormalItemBg(COLORREF color) { m_clrNormalItemBg = color; }
	void SetClrNormalFont(COLORREF color) { m_clrNormalFont = color; }
	void SetClrSelectedFont(COLORREF color) { m_clrSelectedFont = color; }
	void SetClrSelectedItemBg(COLORREF color) { m_clrSelectedItemBg = color; }
	void SetFontNormal(CFont* font) { m_pFontNormal = font;}
	void SetFontSelected(CFont* font) { m_pFontSelected = font;}
	void SetItemGap(int gap) { m_nItemGap = gap; m_nSideGap = gap; }
	void SetItemGap(int h_gap, int v_gap) { m_nItemGap = h_gap; m_nSideGap = v_gap; }
	void SetMargins(int left, int right) { m_nMarginLeft = left; m_nMarginRight = right;}
	void SetListFill(BOOL fill) { m_bListFill = fill; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomListBox)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
//	afx_msg void OnPaint();
	//}}AFX_MSG

	COLORREF	m_clrSelectedItemBg;
	COLORREF	m_clrBackground;
	COLORREF	m_clrBorder;
	COLORREF	m_clrNormalItemBg;
	COLORREF	m_clrNormalFont;
	COLORREF	m_clrSelectedFont;

	CFont*		m_pFontNormal;
	CFont*		m_pFontSelected;
	int			m_nItemGap;
	int			m_nSideGap;
	int			m_nMarginLeft;
	int			m_nMarginRight;
	BOOL		m_bListFill;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMLISTBOX_H__83887B9F_8592_4DC5_8FF9_067DC6D668A4__INCLUDED_)
