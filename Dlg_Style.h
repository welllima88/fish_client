#if !defined(AFX_DLG_STYLE_H__D776921B_E298_4464_98A4_62A57A4EA68E__INCLUDED_)
#define AFX_DLG_STYLE_H__D776921B_E298_4464_98A4_62A57A4EA68E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Style.h : header file
//

#include "CustomDlg.h"
#include "CustomListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg_Style dialog

class CDlg_Style : public CCustomDlg
{
// Construction
public:
	enum
	{
		MARGIN_LEFT		= 21,
		MARGIN_RIGHT	= 21,
		MARGIN_TOP		= 40,
		MARGIN_BOTTOM	= 15,
		BTN_WIDTH		= 73,
		BTN_HEIGHT		= 25,
		COMPONENT_GAP	= 10,
		OUTLINE_COLOR	= COLORREF(RGB(185, 185, 185))
	};

	CDlg_Style(CWnd* pParent = NULL, CString title = _T(""));   // standard constructor
	~CDlg_Style();
	BOOL OnInitDialog();
	void SearchStyle();
	CString GetSelectStyle();

// Dialog Data
	//{{AFX_DATA(CDlg_Style)
	enum { IDD = IDD_DLG_STYLE };
		// NOTE: the ClassWizard will add data members here
	//CListBox	m_listStyle;
	CCustomListBox	m_listStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Style)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void		OnBtnOk();
	void		OnBtnCancel();

// Implementation
protected:
	CNetBitmapButton	m_btnOk;
	CNetBitmapButton	m_btnCancel;
	CPen				m_penOutline;
	CFont				m_fontDefault;
	CString				m_strSelectionStyle;
	CString				m_strStylePath;

	HWND				m_hPrevFocusWnd;

	void				DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void				DrawItems(CDC *pDC);
	void				DeleteObjects();
	BOOL				OnEraseBkgnd(CDC* pDC);
	void				OnOK();

	// Generated message map functions
	//{{AFX_MSG(CDlg_Style)	
	afx_msg void OnDblclkListStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_STYLE_H__D776921B_E298_4464_98A4_62A57A4EA68E__INCLUDED_)
