#include <afxtempl.h>
#include <memory>
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CTray window
using namespace std;

class CTray : public CWnd
{
// Construction
public:
	CTray();	

// Attributes
public:

	enum
	{
		DOWNLOADING = 0,
		CONNECTING		
	};

// Operations
public:
	BOOL Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, HICON icon, UINT uID);
	BOOL IsEnalbed() { return m_bEnabled; }
	BOOL IsVisible() { return !m_bHidden; }

	// Tray Icon Control
	void ShowIcon();
	void HideIcon();
	void RemoveIcon();

	// Icon Setting
	BOOL SetIcon(HICON icon);
	BOOL SetTrayIcon(UINT uResourceID, int flag = 0);
	BOOL SetAniIconList(auto_ptr<HICON> pIconList, int iNumIcons);
	BOOL SetAniIconList(UINT uBmpResourceID);

	// Menu Setting
	BOOL SetPopMenu(UINT menuid);

	// Animation Control
	BOOL Animate(UINT nDelayMilliSeconds, int nNumSeconds /*=-1*/);
	BOOL StopAnimation(int id = 0);

	// For Fish....
	void StartAnimaition(int AniType, int id = 0);

	virtual LRESULT OnTrayNotification(UINT wParam, LONG lParam);	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTray)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTray();

	// Generated message map functions
protected:
	protected:
	BOOL StepAnimate();	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	NOTIFYICONDATA			m_Nid;
	CMenu					m_Menu;
	UINT					m_MenuId;

	// Animation
	static UINT				m_nIDEvent;
	UINT					m_uIDTimer;
	HICON					m_hSavedIcon;
	HICON					m_hExBmpIcon;
	int						m_nCurrentIcon;
	int						m_nAnimationPeriod;
	CArray<HICON, HICON>	m_AniIconList;
	COleDateTime			m_StartTime;

	BOOL					m_bMenuEnable;
	BOOL					m_bEnabled;
	BOOL					m_bHidden;
	BOOL					m_bAnimating;

	list<int>				m_listFeedingChannel;
	int						m_nFeedingCnt;
	int						m_nFeedingResultCnt;
	int						m_nAniType;

	//{{AFX_MSG(CTray)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
