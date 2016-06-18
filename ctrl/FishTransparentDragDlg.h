/********************************************************************
	CREATED:	2006/01/27
	CREATED:	27:1:2006   13:59
	FILENAME: 	FishTransparentDragDlg.h
	WRITER:		eternalbleu
	
	PURPOSE:    
        마우스 드래깅시에 그려지는 투명 윈도우를 생성한다.
        컬러키, 알파 블렌딩 이용
    REFER:
        http://www.codeproject.com/treectrl/ctreelistctrl.asp
    HISTORY:    
        2006/01/27 : CREATED BY eternalbleu
*********************************************************************/
#if !defined(AFX_FISHTRANSPARENTDRAGDLG_H__C8AB3E2F_BE41_46B9_8EDD_BBA9620219F8__INCLUDED_)
#define AFX_FISHTRANSPARENTDRAGDLG_H__C8AB3E2F_BE41_46B9_8EDD_BBA9620219F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FishTransparentDragDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFishTransparentDragDlg dialog
#include "../smartptr.h"
class CFishTransparentDragDlg : public CDialog
{
// Construction
public:
	static BOOL DragLeave(CWnd* pWndLock);
	static BOOL DragEnter(CWnd* pWndLock, CPoint point);
	static BOOL DragShowNolock(BOOL bShow);
	void DragMove(CPoint pt);
	virtual BOOL Create(CWnd* pParent);
	void SetBkgndImage(SmartPtr<CBitmap> bitmap);
    BOOL SetTransparentAlpha(char chAlpha);
    void MakeItOpaque();
	CFishTransparentDragDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFishTransparentDragDlg)
	enum { IDD = IDD_DRAG_DLG_TRANSPARENT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishTransparentDragDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFishTransparentDragDlg)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	SmartPtr<CBitmap> m_spBkgndBitmap;
    void Relayout();

private:
    BOOL m_bLayeredWindows;
    CWnd*   m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISHTRANSPARENTDRAGDLG_H__C8AB3E2F_BE41_46B9_8EDD_BBA9620219F8__INCLUDED_)
