/********************************************************************
	CREATED:	2006/02/20
	CREATED:	20:2:2006   14:13
	FILENAME: 	FocusManager.h
	WRITER:		eternalbleu@gmail.com
	
	PURPOSE:
        이 클래스는 애플리케이션의 각 윈도우에 다른 윈도우로의 포커스 이동을 전담한다.
        CWnd 를 상속해서 만들었으며, 최초 작성은 함수호출에 의한 방식으로 구현되지만
        차후 구현은 메시지를 이용한 구현으로 변경되어야 할 것이다.
    REFER:
    HISTORY:    
        2006/02/20 : CREATED BY eternalbleu@gmail.com
*********************************************************************/
#if !defined(AFX_FOCUSMANAGER_H__B67C701E_E016_4AA6_B01C_CAE71A49D5C3__INCLUDED_)
#define AFX_FOCUSMANAGER_H__B67C701E_E016_4AA6_B01C_CAE71A49D5C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FocusManager.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFocusManager window

class CDlg_Search;
class CDlg_ExplorerBar;
class CDlg_Subscribe;
class CDlg_RSSList;
class CFocusManager : public CWnd
{
// Construction
protected:
	CFocusManager();
    
    static CFocusManager*   _instance;
public:
	static void Release() { if(_instance) {delete _instance; _instance = NULL;} }
    static CFocusManager* getInstance(CWnd* pParent =   NULL);

// Attributes
public:
    //////////////////////////////////////////////////////////////////////////
    // 간단한 구현을 위해서 public 으로 선언
    CDlg_Search*    m_pSearch;
    CDlg_ExplorerBar*   m_pExplorerBar;
    CDlg_Subscribe* m_pSubScribe;
    CDlg_RSSList*   m_pRssList;

// Operations
public:
    CWnd* SetNextFocus();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFocusManager)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL MoveToSearchDlg();
	BOOL MoveToTreeCtrl();
	BOOL MoveToListCtrl();
	virtual ~CFocusManager();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFocusManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOCUSMANAGER_H__B67C701E_E016_4AA6_B01C_CAE71A49D5C3__INCLUDED_)
