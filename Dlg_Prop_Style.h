#if !defined(AFX_DLG_PROP_STYLE_H__1C2AB8DC_0915_44CF_A7F7_BC8672F066DE__INCLUDED_)
#define AFX_DLG_PROP_STYLE_H__1C2AB8DC_0915_44CF_A7F7_BC8672F066DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Prop_Style.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Style dialog

#include "Dlg_PropUnit.h"
#include "./ctrl/netbutton.h"

/**************************************************************************
 * class CDlg_Prop_Network
 *
 * written by aquarelle
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2006-03-20
 *
 * @Description
 * 스타일 관련 환경 설정 페이지
 **************************************************************************/

class CDlg_Prop_Style : public CDlg_PropUnit
{
// Construction
public:
	void Save();
	CDlg_Prop_Style(CWnd* pParent = NULL);   // standard constructor

	enum {
		BTN_WIDTH = 21,
		BTN_HEIGHT = 21,
		COMPONENT_HEIGHT = 18,
		COMPONENT_GAP_V = 10,
		EDIT_WIDTH = 250,
		DOTLINE_TOP = 23,
		CLR_BG = RGB(248, 248, 248),
		CLR_DOTLINE = RGB(68, 68, 68),
		CLR_NORMAL_FONT = RGB(130, 130, 130),
		CLR_BOLD_FONT = RGB(0, 0, 0),
		CLR_OUTLINE = RGB(185, 185, 185)
	};

// Dialog Data
	//{{AFX_DATA(CDlg_Prop_Style)
	enum { IDD = IDD_DLG_PROP_STYLE };
	CStatic				m_staticSummary;
	CStatic				m_staticPaperStyle;
	CStatic				m_staticStyle;
	CEdit				m_editPaperStyle;
	CButton				m_chkSummary;
	CNetBitmapButton	m_btnFileDlg;
	BOOL				m_bSummary;
	CString				m_szPaperStyle;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Prop_Style)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HBRUSH m_hBrushBkgnd;

	void Init();

	// Generated message map functions
	//{{AFX_MSG(CDlg_Prop_Style)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnBtnFiledlg();
	afx_msg void OnCheckSummary();
	afx_msg void OnChangeEditPaperstyle();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PROP_STYLE_H__1C2AB8DC_0915_44CF_A7F7_BC8672F066DE__INCLUDED_)
