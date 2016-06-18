/********************************************************************
	CREATED:	2006/02/20
	CREATED:	20:2:2006   14:13
	FILENAME: 	FocusManager.h
	WRITER:		eternalbleu@gmail.com
	
	PURPOSE:
        �� Ŭ������ ���ø����̼��� �� �����쿡 �ٸ� ��������� ��Ŀ�� �̵��� �����Ѵ�.
        CWnd �� ����ؼ� ���������, ���� �ۼ��� �Լ�ȣ�⿡ ���� ������� ����������
        ���� ������ �޽����� �̿��� �������� ����Ǿ�� �� ���̴�.
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
    // ������ ������ ���ؼ� public ���� ����
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
