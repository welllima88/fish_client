#if !defined(AFX_DLG_PROPERTIES_H__CC348FB2_87CF_47C9_918E_17684167959F__INCLUDED_)
#define AFX_DLG_PROPERTIES_H__CC348FB2_87CF_47C9_918E_17684167959F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Properties.h : header file
//

#include <map>
#include "Dlg_PropUnit.h"
#include "CustomDlg.h"
#include "CustomListBox.h"

/**************************************************************************
 * class CDlg_Properties
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
 * 환경 설정 다이얼로그
 **************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CDlg_Properties dialog
typedef std::map<int, SmartPtr<CDlg_PropUnit> > PROPMAP;
typedef PROPMAP::iterator						PROPMAPITERATOR;

class CDlg_Properties : public CCustomDlg
{
// Construction
public:
	void AddPropPage(SmartPtr<CDlg_PropUnit>& page, CString menuname = _T(""), CString title = _T(""));
	CDlg_Properties(CWnd* pParent = NULL);   // standard constructor

	enum{
		MARGIN_TOP = 40,
		MARGIN_LEFT = 18,
		MARGIN_RIGHT = 18,
		MARGIN_BOTTOM = 15,
		LIST_WIDTH = 120,
		LIST_HEIGHT = 240,
		LIST_MARGIN_RIGHT = 22,
		BTN_WIDTH = 73,
		BTN_HEIGHT = 25,
		WIDTH = 470,
		HEIGHT = 310
	};

// Dialog Data
	//{{AFX_DATA(CDlg_Properties)
	enum { IDD = IDD_DLG_PROPERTIES };
	//CListBox	m_listMenu;
	CCustomListBox		m_listMenu;
	CNetBitmapButton	m_btnApply;
	CNetBitmapButton	m_btnCancel;
	CNetBitmapButton	m_btnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Properties)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	PROPMAP		m_Propmap;
	SmartPtr<CDlg_PropUnit>		currentpage;
	int			menuseq;	
	BOOL		bInit;
	BOOL		bModified;
	CPen		m_penOutline;
	CFont		m_fontNormal;

	void MoveControls();
	void CalcPageRect(CRect& rect);

	// Generated message map functions
	//{{AFX_MSG(CDlg_Properties)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnBtnApply();
	afx_msg void OnSelchangeListMenu();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnChildInit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChildModified(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PROPERTIES_H__CC348FB2_87CF_47C9_918E_17684167959F__INCLUDED_)
