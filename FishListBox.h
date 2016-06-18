#if !defined(AFX_FISHLISTBOX_H__1EF2F970_4138_4385_94C0_A174592E14A3__INCLUDED_)
#define AFX_FISHLISTBOX_H__1EF2F970_4138_4385_94C0_A174592E14A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FishListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFishListBox window

class CFishListBox : public CListBox
{
// Construction
public:
	CFishListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CFont m_ftDefault;
	virtual ~CFishListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFishListBox)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISHLISTBOX_H__1EF2F970_4138_4385_94C0_A174592E14A3__INCLUDED_)
