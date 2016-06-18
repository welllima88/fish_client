#if !defined(AFX_FISHLINKMANAGER_H__7A6C776C_EEDE_4AE2_A24F_8EF2F33A9349__INCLUDED_)
#define AFX_FISHLINKMANAGER_H__7A6C776C_EEDE_4AE2_A24F_8EF2F33A9349__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FishLinkManager.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFishLinkManager window
#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))
#define OP_FISHLINK				12000
#define OP_CLCOMMAND			12001

class CFishLinkManager : public CWnd
{
// Construction
private:
    static CFishLinkManager* _instance;

public:
    static CFishLinkManager* getInstance(){
        if (    _instance   ==  NULL    )   {
            _instance   =   new CFishLinkManager();
        }
        return _instance;
    }

// Attributes
public:

// Operations
public:
    bool    HaveFishRegAccess();
    bool    Ask4RegFix(bool checkOnly, bool dontAsk);
    void    BackupReg(void);
    void    RevertReg(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishLinkManager)
	//}}AFX_VIRTUAL

// Implementation
public:
	CFishLinkManager();
	virtual ~CFishLinkManager();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFishLinkManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISHLINKMANAGER_H__7A6C776C_EEDE_4AE2_A24F_8EF2F33A9349__INCLUDED_)
