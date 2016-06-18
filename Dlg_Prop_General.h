#if !defined(AFX_DLG_PROP_GENERAL_H__D677D4E9_BDA4_4D11_9731_BB3AB8F9D37C__INCLUDED_)
#define AFX_DLG_PROP_GENERAL_H__D677D4E9_BDA4_4D11_9731_BB3AB8F9D37C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Prop_General.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_General dialog
#include "Dlg_PropUnit.h"
#include "./ctrl/netbutton.h"

/**************************************************************************
 * class CDlg_Prop_General
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2006-01-17
 *
 * @Description
 * 일반 환경 설정 페이지
 **************************************************************************/

class CDlg_Prop_General : public CDlg_PropUnit
{
// Construction
public:
	void Save();
	CDlg_Prop_General(CWnd* pParent = NULL);   // standard constructor

	enum {
		BTN_WIDTH = 21,
		BTN_HEIGHT = 21,
		COMPONENT_HEIGHT = 18,
		COMPONENT_GAP_V = 2,
		EDIT_WIDTH = 250,
		DOTLINE_TOP = 23,
		CLR_BG = RGB(248, 248, 248),
		CLR_DOTLINE = RGB(68, 68, 68),
		CLR_NORMAL_FONT = RGB(130, 130, 130),
		CLR_BOLD_FONT = RGB(0, 0, 0),
		CLR_OUTLINE = RGB(185, 185, 185)
	};

// Dialog Data
	//{{AFX_DATA(CDlg_Prop_General)
	enum { IDD = IDD_DLG_PROP_GENERAL };
	BOOL	m_bAutoStart;
	BOOL	m_bUseInformWnd;
	BOOL	m_bCloseBtnExit;
	BOOL	m_bSearchAlwaysSave;
	CString	m_sUserPath;
	CNetBitmapButton m_btnUserpath;

	CStatic m_staticNormalSetting;
	CStatic m_staticRunSetting;
	CStatic m_staticInfoWndSetting;
	CStatic m_staticSearchSetting;
	CStatic m_staticUserPathSetting;

	CButton m_chkAutoStart;
	CButton m_chkCloseBtnExit;
	CButton m_chkUseInfoWnd;
	CButton m_chkSearchAlwaysSave;

	CEdit	m_editUserPath;
    
    HBRUSH  m_hBrushBkgnd;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Prop_General)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Init();

	// Generated message map functions
	//{{AFX_MSG(CDlg_Prop_General)
	afx_msg void OnChkAutologin();
	afx_msg void OnChkAutostart();
	afx_msg void OnChkSavepass();
	afx_msg void OnChkUseinformwnd();
	afx_msg void OnChkCloseBtnExit();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditUserpath();
	afx_msg void OnButtonUserpath();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnChkSearchalwayssave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PROP_GENERAL_H__D677D4E9_BDA4_4D11_9731_BB3AB8F9D37C__INCLUDED_)

