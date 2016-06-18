#if !defined(AFX_SCRAPMANAGER_H__5DC6C401_F9B2_4212_A8F3_515055627C27__INCLUDED_)
#define AFX_SCRAPMANAGER_H__5DC6C401_F9B2_4212_A8F3_515055627C27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrapManager.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrapManager window

class CDlg_Subscribe;
class CScrapManager : public CWnd
{
private:

// Construction
public:
	CScrapManager(CWnd* pWnd);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrapManager)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init(CWnd* pParentWnd);
	BOOL HideScrapMgr();
	BOOL ShowScrapMgr(CPoint ptContext =   CPoint(0, 0));
	virtual ~CScrapManager();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScrapManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	SmartPtr<CDlg_Subscribe> m_spSubscribe;
    CMenu  m_ctrlPopMenu;
    CWnd*  m_pParent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRAPMANAGER_H__5DC6C401_F9B2_4212_A8F3_515055627C27__INCLUDED_)
