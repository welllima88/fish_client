#if !defined(AFX_CHGPORTDLG_H__31E937FA_C7D7_4296_9E08_852F67B6E109__INCLUDED_)
#define AFX_CHGPORTDLG_H__31E937FA_C7D7_4296_9E08_852F67B6E109__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChgPortDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChgPortDlg dialog
#include "CustomDlg.h"

class CChgPortDlg : public CCustomDlg
{
// Construction
public:
	void LoadObjects();
	void DetachObjects();
	void Drawing2(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	void MoveControls();
	CChgPortDlg(CWnd* pParent = NULL);   // standard constructor
	~CChgPortDlg();

// Dialog Data
	//{{AFX_DATA(CChgPortDlg)
	enum { IDD = IDD_CHG_PORT_DLG };
	CString	m_strPort;
	CStatic	m_stcTitle;
	CEdit	m_edtPort;
	CNetBitmapButton	m_btnChg;
	CNetBitmapButton m_btnCancel;
	//}}AFX_DATA


	
	CBrush		m_DlgWhiteBackBrush;
	CFont		m_fontNormal;
	
	CRect		m_rectSept;

	CRect		m_rectTitle; //'변경할 포트를 입력하십시오'
	CRect		m_rectPortBack;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChgPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChgPortDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHGPORTDLG_H__31E937FA_C7D7_4296_9E08_852F67B6E109__INCLUDED_)
