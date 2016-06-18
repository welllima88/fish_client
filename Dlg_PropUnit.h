// Dlg_PropUnit.h: interface for the CDlg_PropUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLG_PROPUNIT_H__AF6A9536_775D_421D_B961_A30ADB423833__INCLUDED_)
#define AFX_DLG_PROPUNIT_H__AF6A9536_775D_421D_B961_A30ADB423833__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_PropUnit.h : header file
//
#include "./ctrl/netbutton.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg_PropUnit dialog

/**************************************************************************
 * class CDlg_PropUnit
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
 * CDlg_Properties의 설정 페이지의 Wrapper 클래스
 **************************************************************************/


#define WM_CHILD_INIT					WM_APP + 376
#define WM_CHILD_MODIFIED				WM_APP + 377

class CDlg_PropUnit : public CDialog
{
// Construction
public:
	
	BOOL		bModified;

	int			PropID;
	UINT		nResourceID;
	CString		MenuName;
	CString		Title;
	CFont		m_fontBold;
	CFont		m_fontNormal;
	CPen		m_penOutline;
	CPen		m_penTitleUnder;

	virtual void Save() {}
	CDlg_PropUnit(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_PropUnit)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_PropUnit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void Init() {}
	void SendModified();

	// Generated message map functions
	//{{AFX_MSG(CDlg_PropUnit)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DLG_PROPUNIT_H__AF6A9536_775D_421D_B961_A30ADB423833__INCLUDED_)
