#if !defined(AFX_FISHHEADERCTRL_H__2D1D18FD_CCAE_46D8_BFB6_2A56B26568F9__INCLUDED_)
#define AFX_FISHHEADERCTRL_H__2D1D18FD_CCAE_46D8_BFB6_2A56B26568F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FishHeaderCtrl.h : header file
//

#include "SkinHeaderCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CFishHeaderCtrl window

class CFishListCtrl;
class CFishHeaderCtrl : public CSkinHeaderCtrl
{
public:
    enum {  FISH_VIEWINFO_LISTCTRL_HEADER_STARTIMG_WIDTH    =   2,
            FISH_VIEWINFO_LISTCTRL_HEADER_END_WIDTH         =   2,
            FISH_VIEWINFO_LISTCTRL_HEADER_IMG_HEIGHT        =   12, 
            TEXT_RIGHT_PADDING                              =   15,
    };
    
    enum {
        FISH_HEADER_CTRL_TOP_LINE_CLR   =   RGB(0xBC, 0xBC, 0xBC),
    };
// Construction
public:
	CFishHeaderCtrl();

// Attributes
public:
    BOOL m_bOnDragging;

protected:
	CRect m_rcFirst;

    // Operations
public:
    CFishListCtrl* m_pList;
    void _initialize();
    void drawitem_Bkgnd(CDC* pDC, int idx, CBitmap* left, CBitmap* span, CBitmap* right);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishHeaderCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFishHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFishHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg LONG OnMouseLeave(WPARAM wParam, LPARAM lParam) ;
	afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISHHEADERCTRL_H__2D1D18FD_CCAE_46D8_BFB6_2A56B26568F9__INCLUDED_)
