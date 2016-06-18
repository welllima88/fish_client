#if !defined(AFX_DLG_PROP_NETWORK_H__B2163912_1E4E_42D9_B68C_A5DDD1E2BB0F__INCLUDED_)
#define AFX_DLG_PROP_NETWORK_H__B2163912_1E4E_42D9_B68C_A5DDD1E2BB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Prop_Network.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Network dialog
#include "Dlg_PropUnit.h"

/**************************************************************************
 * class CDlg_Prop_Network
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
 * 네트워크 환경 설정 페이지
 **************************************************************************/

class CDlg_Prop_Network : public CDlg_PropUnit
{
// Construction
public:
	void Save();
	CDlg_Prop_Network(CWnd* pParent = NULL);   // standard constructor

	enum {		
		COMPONENT_HEIGHT = 18,
		COMPONENT_GAP_V = 10,
		EDIT_WIDTH = 60,
		STATIC_MAX_WIDTH = 135,
		DOTLINE_TOP = 23,
		CLR_BG = RGB(248, 248, 248),
		CLR_DOTLINE = RGB(68, 68, 68),
		CLR_OUTLINE = RGB(185, 185, 185),
		CLR_NORMAL_FONT = RGB(130, 130, 130),
		CLR_BOLD_FONT = RGB(0, 0, 0),
		EDIT_MARGIN = 4,
	};

// Dialog Data
	//{{AFX_DATA(CDlg_Prop_Network)
	enum { IDD = IDD_DLG_PROP_NETWORK };
	UINT	m_nMaxUpload;
	UINT	m_nPort;
	UINT	m_nMaxDownload;

	CStatic m_staticNetwork;
	CStatic m_staticPort;
	CStatic m_staticConPort;
	CStatic m_staticTransLimit;
	CStatic m_staticUpload;
	CStatic m_staticDownload;
	CStatic m_staticMax;
	CStatic m_staticMax2;

	CEdit	m_editPort;
	CEdit	m_editUpload;
	CEdit	m_editDownload;
	
	//}}AFX_DATA
    HBRUSH m_hBrushBkgnd;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Prop_Network)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Init();

	// Generated message map functions
	//{{AFX_MSG(CDlg_Prop_Network)
	afx_msg void OnChangeEditMaxdownload();
	afx_msg void OnChangeEditMaxupload();
	afx_msg void OnChangeEditPort();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PROP_NETWORK_H__B2163912_1E4E_42D9_B68C_A5DDD1E2BB0F__INCLUDED_)
