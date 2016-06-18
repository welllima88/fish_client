#if !defined(AFX_DLG_MESSAGEBOX_H__3B7A6A7F_474E_4E81_9A12_4143AD6F39CF__INCLUDED_)
#define AFX_DLG_MESSAGEBOX_H__3B7A6A7F_474E_4E81_9A12_4143AD6F39CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_MessageBox.h : header file
//

#include "CustomDlg.h"
#include "fish_def.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg_MessageBox dialog
class CDlg_MessageBox : public CCustomDlg
{
// Construction
public:
	virtual  ~CDlg_MessageBox();
	CDlg_MessageBox(LPCTSTR message = _T(""), UINT uType = FMB_OK, CWnd* pParent = NULL, CString title = _T(""));   // standard constructor
	BOOL			OnInitDialog();
	int				DoModal();

	enum
	{
		BTN_WIDTH = 73,
		BTN_HEIGHT = 25,
		MARGIN_LEFT	= 21,
		MARGIN_RIGHT = 21,
		MARGIN_TOP = 40,
		MARGIN_BOTTOM = 15,
		MAX_WIDTH = 340,
	};

// Dialog Data
	//{{AFX_DATA(CDlg_MessageBox)
	enum { IDD = IDD_DLG_MESSAGE_BOX };
		// NOTE: the ClassWizard will add data members here
	CStatic			m_staticMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_MessageBox)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CNetBitmapButton	m_btn1; // OK, Abort, Yes, Retry
	CNetBitmapButton	m_btn2; // Cancel, Retry, No, YesAll
	CNetBitmapButton	m_btn3; // Ignore, Cancel, No, NoAll
	CNetBitmapButton	m_btn4; // NoAll

	CString				m_strMessage;

	CFont				m_defaultFont;
	CRect				m_rectMessageBox;

	CBitmap				m_bmpMsgIcon;
	BITMAP				m_bitmapMsgIcon;

	UINT				m_uType;
	int					m_nClickedButton;
	CSize				m_sizeMsg;
	int					m_nLineHeight;
	int					m_nLineCnt;

    HWND                m_hPrevFocusWnd;

    HBRUSH              m_hBrushBkgnd;

	// Generated message map functions
	//{{AFX_MSG(CDlg_MessageBox)
		// NOTE: the ClassWizard will add member functions here
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtn1();
	afx_msg void OnBtn2();
	afx_msg void OnBtn3();
	afx_msg void OnBtn4();
	//}}AFX_MSG	
	void				CalcMsgBox();
	void				DeleteObjects();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_MESSAGEBOX_H__3B7A6A7F_474E_4E81_9A12_4143AD6F39CF__INCLUDED_)
