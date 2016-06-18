#if !defined(AFX_REPOSITORYITEMPROPERTYDLG_H__FC35777D_9E54_4878_BFED_2979710A9601__INCLUDED_)
#define AFX_REPOSITORYITEMPROPERTYDLG_H__FC35777D_9E54_4878_BFED_2979710A9601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// repositoryitempropertydlg.h : header file
//

#define	GROUP_DLG				100
#define	CHANNEL_DLG				101
#define SCRAP_DLG				102
#define KEYWORD_DLG				103
#define SEARCH_DLG				104

#define	GROUP_ADD_DLG			105
#define CHANNEL_ADD_DLG			106
#define SCRAP_ADD_DLG			107
#define KEYWORD_ADD_DLG			108
#define SEARCH_ADD_DLG			109

#define URL_INPUT_STEP		200
#define URL_SELECTION_STEP	201
#define URL_FINAL_STEP		202

#define ID_BTN_CANCEL			8887
#define ID_BTN_OK				8888
#define ID_BTN_URLCHECK			8889
#define ID_BTN_NEXT				8890
#define ID_BTN_PREV				8891

/////////////////////////////////////////////////////////////////////////////
// CFishRepItemPropertyDlg dialog

#include "../CustomDlg.h"
#include "../XML/EzXMLParser.h"
#include "../CustomListBox.h"

class CDlg_Subscribe;

class CFishRepItemPropertyDlg : public CCustomDlg
{
// Construction
public:
	CFishRepItemPropertyDlg(CWnd* pParent = NULL);   // standard constructor	
    ~CFishRepItemPropertyDlg();
    
// Dialog Data
	//{{AFX_DATA(CFishRepItemPropertyDlg)
	enum { IDD = IDD_REPOSITORY_ITEM_PROPERTY };
	CString	m_strTitle;
	CString	m_strComment;
//	UINT	m_nFeedInterval;
	CString m_strXmlUrl;
	CString m_strHtmlUrl;
	CString m_strKeyword;

	CEdit	m_editTitle;
	CEdit	m_editComment;	
	//CEdit	m_editFeedInterval;
	CComboBox m_comboFeedInterval;
	CEdit	m_editKeyword;
	CEdit	m_editXmlUrl;
	CEdit	m_editHtmlUrl;	

	CStatic	m_staticTitle;
	CStatic	m_staticComment;
	CStatic	m_staticFeedInterval;
	CStatic	m_staticKeyword;
	CStatic	m_staticXmlUrl;
	CStatic	m_staticHtmlUrl;
	CStatic m_staticTargetGroup;

	CStatic m_staticWizardComment;

	//CListBox m_listFeedUrl;
	CCustomListBox m_listFeedUrl;
	CComboBox m_comboTargetGroup;
	CButton m_checkAutoSearch;

	CButton m_checkIsSearch;
	BOOL	m_bIsSearch;
	CPen	m_penDotline;

	//}}AFX_DATA
	UINT			m_nFeedInterval;

	CNetBitmapButton	m_btnOk;
	CNetBitmapButton	m_btnCancel;
	//CNetBitmapButton	m_btnUrlCheck;
	CNetBitmapButton	m_btnNext;
	CNetBitmapButton	m_btnPrev;

    HWND                m_hPrevFocusWnd;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishRepItemPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFishRepItemPropertyDlg)
		// NOTE: the ClassWizard will add member functions here
//	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void			SetMode(int type);
	void			DeleteObjects();
	void			DrawItems(CDC *pDC);
	void			DrawOutline(CDC *pBufferDC, CDC *pMemDC);
	void			DrawBg(CDC *pBufferDC, CDC *pMemDC);	
	BOOL			OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void			ChangeWizardForm();
	afx_msg void	OnXmlUrlFound(WPARAM wParam, LPARAM lParam);
	static DWORD	UrlCheckThread(LPVOID param);


	afx_msg void	OnButtonOk();
	afx_msg void	OnOK();
	afx_msg void	OnCancel();
	afx_msg void	OnButtonCheck();
	afx_msg void	OnButtonPrev();
	afx_msg void	OnButtonNext();
	afx_msg void	OnCheckSearch();
	afx_msg BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	int				m_nDlgType;
	int				m_nWizardStep;
	UINT			m_nTargetGroup;
	BOOL			m_bUrlCheck;
	BOOL			m_bAutoSearch;
	BOOL			m_bChangeSearch;
	BOOL			m_bThreadRunning;

	HANDLE			m_hUrlChkThread;
	DWORD			m_dwUrlChkThreadId;
	

	CFont			m_DefaultFont;
	CFont			m_BoldFont;

	/*
	CBitmap			m_bmpSubTitleMid;
	BITMAP			m_bitmapSubTitleMid;
	CBitmap			m_bmpSubTitleR;
	BITMAP			m_bitmapSubTitleR;
	CBitmap			m_bmpSubTitleL;
	BITMAP			m_bitmapSubTitleL;
	CBitmap			m_bmpSubTitle;
	BITMAP			m_bitmapSubTitle;
	CBitmap			m_bmpMainTitle;
	BITMAP			m_bitmapMainTitle;
	*/
	CBitmap			m_bmpIcon;
	BITMAP			m_bitmapIcon;

	CBitmap			m_bmpBox;
	BITMAP			m_bitmapBox;
	
	CDlg_Subscribe*	m_pSubscribe;
	CEzXMLParser	m_xmlParser;

	CPen			m_penOutline;
	HBRUSH			m_hBrushBkgnd;
	HBRUSH			m_hBrushEditBg;
	CRect			m_rectSubTitle;

public:
	CString         TranslateString(CString str);
    const CString   getTitle()          const;
    const CString   getComment()        const;
    const UINT      getFeedInterval()   const;
	const UINT      getTargetGroup()	const;
    const CString   getXmlUrl()         const;
    const CString   getHtmlUrl()        const;	
	const CString   getKeyword()        const;
	const BOOL		getIsSearch()       const;
	const BOOL		getChangeSearch()   const;
	BOOL			OnInitDialog();
	void			setDlgType(int Type);
	INT_PTR         DoModal(int Type    =   CHANNEL_ADD_DLG, CString url    =   _T(""));
	void			SetSubscribeDlg(CDlg_Subscribe* psubscribe);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPOSITORYITEMPROPERTYDLG_H__FC35777D_9E54_4878_BFED_2979710A9601__INCLUDED_)

