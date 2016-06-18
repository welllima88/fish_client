#if !defined(AFX_DLG_SEARCH_H__5BE1DBF4_72DB_4AAE_8FC5_09EA1ED29297__INCLUDED_)
#define AFX_DLG_SEARCH_H__5BE1DBF4_72DB_4AAE_8FC5_09EA1ED29297__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Search.h : header file
//
#include <vector>
#include <list>
using namespace std;

#include "./view_search/ACEdit.h"
#include "ctrl/netbutton.h"	

#pragma warning (disable:4786)

#define DLG_SRCH_RCMM_WND	13737
#define COLOR_SEARCH_VIEW_BG	RGB(0x5A, 0x96, 0xB5)
/////////////////////////////////////////////////////////////////////////////
// CDlg_Search dialog
class CRcmmWordWnd;

class CDlg_Search : public CDialog
{
	enum
	{
		TYPE_SRCH_RCMMWORD	= 0, //추천검색어
		TYPE_SRCH_RANGE_BTN	= 1, //검색범위 버튼 
		TYPE_SRCH_LOAD_DATA = 2  //로드 수 
	};
	enum
	{
		TYPE_SRCH_RANGE_ALL		= 0,//전체
		TYPE_SRCH_RANGE_LOCAL	= 1//로컬 
	};
	enum
	{
		TYPE_SRCH_NETWORK_STATUS_0	=	0,//전파버튼 그래프 갯수 0
		TYPE_SRCH_NETWORK_STATUS_1	=	1,//전파버튼 그래프 갯수 1
		TYPE_SRCH_NETWORK_STATUS_2	=	2,//전파버튼 그래프 갯수 2
		TYPE_SRCH_NETWORK_STATUS_3	=	3//전파버튼 그래프 갯수 3
	};

	enum
	{
		TYPE_SRCH_NETWORK_LOADNUM_STATUS_0	=	0,//전파버튼 그래프 갯수 0
		TYPE_SRCH_NETWORK_LOADNUM_STATUS_1	=	1,//전파버튼 그래프 갯수 1
		TYPE_SRCH_NETWORK_LOADNUM_STATUS_2	=	2,//전파버튼 그래프 갯수 2
		TYPE_SRCH_NETWORK_LOADNUM_STATUS_3	=	3//전파버튼 그래프 갯수 3
	};

	// Construction
public:
	BOOL isEnglishString(CString str);
	void EnableNetwork(BOOL bEnable = TRUE);
	void LoadNetworkLoadNumBtnBitmap(int nStatus = TYPE_SRCH_NETWORK_LOADNUM_STATUS_0);
	void LoadNetworkBtnBitmap(int nStatus = TYPE_SRCH_NETWORK_STATUS_0);
	void MakeInfoNSetRange();
	void LoadBtnBitmaps();
	void SetRcmmWord(CString strWord);
	void NumToString(int nCount, CString& strCount);
	void MakeLoadNumData();
	void ShowRcmmWord(BOOL bShow);
	void GetMsgRect(CString strWord, CRect &rect);

	// written by moonknit 2006-01-13
	void SetNetState(int nNodeCnt);

	void MakeRcmmWordList();

	void LoadBmpSrchRangeBtn();
	int GetSrchRange();
	void SetSrchRange(int nRange);
	void DeleteObjects();
	void InitObjects();
	void DrawRcmmWordTitle(CDC *pBufferDC);
	void DrawLoadNumData(CDC *pBufferDC);	
	void LoadBmpNetworkBtn(BOOL bFlag);
	void ClearEdit();
	void AddKeyword(CString strKeyword);
	void ShowControls();
	int GetCurrType();
	void SetCurrType(int nType);
	CDlg_Search(CWnd* pParent = NULL);   // standard constructor
	~CDlg_Search();
	BOOL Create(CWnd* pParentWnd = NULL);
	void MoveControls();
	
	// Dialog Data
	//{{AFX_DATA(CDlg_Search)
	enum { IDD = IDD_DLG_SEARCH };
	CNetBitmapButton	m_btnNetworkLoad;
	CACEdit				m_edtSearch;
//	CNetBitmapButton	m_btnSrchLocal;
//	CNetBitmapButton	m_btnSrchAll;
	CNetBitmapButton	m_btnSrchRangeN;
	CNetBitmapButton	m_btnSrchRangeAllLocal;
	CNetBitmapButton	m_btnSearch;
	CNetBitmapButton	m_btnTag;	
	CNetBitmapButton	m_btnNetwork;
	CString	m_sKeyword;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_Search)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:

	void MakeRecommWnd();
	// draw functions
	void DrawItems(CDC *pDC);
	void DrawBg(CDC *pBufferDC, CDC *pMemDC);
	// --
	void GetRecWord();

	CCriticalSection			m_RcmmWordCriticalSection;
	std::vector< SmartPtr<CRcmmWordWnd> >	m_vecRcmmWordList;//추천검색어를 지하는 Vector
	
	BOOL			m_bNetEnable;
	BOOL			m_bShowRecommend;

	BOOL			m_bSearchTime;			// Search 버튼의 연타를 막는다.
	
	BOOL			m_bInit;
	BOOL			m_bSelfDraw;

	BOOL			m_bMakeInfoNSetRange;//로드수, 추천검색어 등의 정보를 로드했는지..
	
	CFont			m_fontNormal;
	CFont			m_fontNormalB;
	CFont			m_fontNormalBUl;

	int				m_nValidRecomm;
	int				m_nSearchTimer;
	int				m_nRecommTimer;

	int				m_nCurrType;//TYPE : 추천검색어, 검색범위 버튼, 로드수

	// written by moonknit 2006-01-13
	int				m_nNodeState;

	CString			m_strLoadCount;	//로드수 
	int				m_nSrchRange;//검색범위(전체, 로컬)
	
	CRect			m_rectLoadNumData;//로드수 나타낼 영역
	CRect			m_rectSrchRcmmWord;//추천검색어 나타낼 영역 
	CRect			m_rectSrchRcmmWordTitle;//STR_RCMM_TITLE 영역 
	CRect			m_rectedtSRchBg;//Edit 뒷 배경 영역 

	CBitmap			m_bmpSrchBg;//배경 이미지 
	BITMAP			m_bitmapSrchBg;
	CPen			m_penedtRect;//입력창 외곽 Pen

	CBrush			m_BackBrush;

	CRgn			m_rgnClip;

	// 추천 검색어를 받아오기 위한 스레드
	list<CString>	m_listWords;
	HANDLE			m_hThread;
	int				m_nThreadTry;
	static DWORD WINAPI RecommandThread(LPVOID lpparam);
	
	// Generated message map functions
	//{{AFX_MSG(CDlg_Search)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButtonSearch();
	afx_msg void OnBtnSearchRangeNormal();
	afx_msg void OnBtnSearchRange();
	afx_msg void OnBtnNetwork();
	afx_msg void OnCompleteCharForEdit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnNetworkLoad();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
    afx_msg	void OnRedrawRcmmWnds(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNetCntChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnReceivedRWords(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_SEARCH_H__5BE1DBF4_72DB_4AAE_8FC5_09EA1ED29297__INCLUDED_)
