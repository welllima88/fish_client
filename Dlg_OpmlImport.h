#if !defined(AFX_DLG_OPMLIMPORT_H__A117939A_3BFF_4F87_A05F_30A0148772E5__INCLUDED_)
#define AFX_DLG_OPMLIMPORT_H__A117939A_3BFF_4F87_A05F_30A0148772E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_OpmlImport.h : header file
//

#include "CustomDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg_OpmlImport dialog

class CDlg_OpmlImport : public CCustomDlg
{
// Construction
public:
	CDlg_OpmlImport(CWnd* pParent = NULL);   // standard constructor

	enum {
		BTN_WIDTH = 73,
		BTN_HEIGHT = 25,
		DOT_BTN_WIDTH = 21,
		DOT_BTN_HEIGHT = 21,
		MARGIN_TOP = 40,
		MARGIN_LEFT = 18,
		MARGIN_RIGHT = 18,
		MARGIN_BOTTOM = 15,
		COMPONENT_GAP = 10,
		EDIT_MARGIN = 3,
		WIDTH = 350,
		HEIGHT = 150,
		CLR_BG = RGB(248, 248, 248),
		CLR_NORMAL_FONT = RGB(0, 0, 0),
		CLR_OUTLINE = RGB(185, 185, 185)
	};

// Dialog Data
	//{{AFX_DATA(CDlg_OpmlImport)
	enum { IDD = IDD_DLG_OPMLIMPORT };
	CStatic	m_staticComment;
	CNetBitmapButton	m_btnCancel;
	CNetBitmapButton	m_btnOk;
	CEdit				m_editFileLocation;
	CNetBitmapButton	m_btnFileDlg;
	CString				m_szFileLocation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_OpmlImport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CFont				m_fontDefault;
	CPen				m_penOutline;
	HBRUSH				m_hBrushBkgnd;

	// Generated message map functions
	//{{AFX_MSG(CDlg_OpmlImport)
	virtual BOOL OnInitDialog();	
	afx_msg void OnOK();
	afx_msg void OnBtnFiledlg();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_OPMLIMPORT_H__A117939A_3BFF_4F87_A05F_30A0148772E5__INCLUDED_)
