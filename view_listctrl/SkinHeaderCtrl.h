/********************************************************************
	CREATED:	2005/12/28
	CREATED:	28:12:2005   13:01
	FILENAME: 	SkinHeaderCtrl.h
	WRITER:		youngchang (eternalbleu@gmail.com)
	
	PURPOSE:    피쉬 프로젝트를 위한 최적화
    REFER:
    HISTORY:    
        2005/12/28 : CREATED BY youngchang (eternalbleu@gmail.com)
*********************************************************************/
#if !defined(AFX_SKINHEADERCTRL_H__8B0847B1_B4E6_4372_A62D_038582FFEA5C__INCLUDED_)
#define AFX_SKINHEADERCTRL_H__8B0847B1_B4E6_4372_A62D_038582FFEA5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinHeaderCtrl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl window

class CSkinHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSkinHeaderCtrl();

// Attributes
public:
    CFont m_ftDefault;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual ~CSkinHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinHeaderCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINHEADERCTRL_H__8B0847B1_B4E6_4372_A62D_038582FFEA5C__INCLUDED_)
