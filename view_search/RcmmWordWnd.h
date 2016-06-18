#if !defined(AFX_RCMMWORDWND_H__D19E867C_B538_4A5E_B7AF_6235CCC056C4__INCLUDED_)
#define AFX_RCMMWORDWND_H__D19E867C_B538_4A5E_B7AF_6235CCC056C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RcmmWordWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRcmmWordWnd view

class CRcmmWordWnd : public CWnd
{
protected:	

// Attributes
public:
	CRcmmWordWnd();           // protected constructor used by dynamic creation
	~CRcmmWordWnd();

// Operations
public:
	void SetRealRect(CRect rect);
	CRect GetRealRect();
	void DrawMorePoint(CDC *pBufferDC, CRect rectOrg, CString strText
		, UINT nFormat = DT_VCENTER|DT_SINGLELINE|DT_LEFT | DT_NOPREFIX);
	CString GetRcmmWord();
	void SetRcmmWord(CString strWord);
	CString strRcmmWord; //추천 검색어 
	void InitView();
	void DrawMsg(CDC *pBufferDC);
	void DeleteObjects();
	void InitObjects();
	void Drawing();

	CRect		m_rectWnd;
	CRect		m_rectReal;//Drawing 되는 위치...

	CFont		m_fontNormal;	
	CFont		m_fontNormalBUl;

    CWnd*       m_pParent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRcmmWordWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	BOOL		m_bInit;

// Implementation
protected:

	int			m_nViewTimer;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRcmmWordWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
    afx_msg void OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RCMMWORDWND_H__D19E867C_B538_4A5E_B7AF_6235CCC056C4__INCLUDED_)
