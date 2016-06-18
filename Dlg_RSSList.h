#if !defined(AFX_DLG_RSSLIST_H__6DC0CB25_30ED_47F2_A863_818113A078A2__INCLUDED_)
#define AFX_DLG_RSSLIST_H__6DC0CB25_30ED_47F2_A863_818113A078A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_RSSList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSList dialog
#include <deque>
#include <cmath>
#include <algorithm>
#include <vector>
#include "fish_struct.h"
#include "view_listctrl/fishlistctrl.h"
#include "resource.h"

using namespace std;

class CRSSCurrentList;
struct POSTITEM;

#define POST_UNREAD     _T("U")
#define POST_READ       _T("R")
#define POST_IMPACT     _T("I")

//#define MARKETING_VERSION

class CDlg_RSSList : public CDialog
{
protected:
    DECLARE_DYNCREATE(CDlg_RSSList)

public:
//////////////////////////////////////////////////////////////////////////
    enum {
        FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX    =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX,
        FISH_VIEWINFO_LISTCTRL_ICON_INDEX       =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_ICON_INDEX,
        FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX  =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX,
        FISH_VIEWINFO_LISTCTRL_DATE_INDEX       =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_DATE_INDEX,
        FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX     =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX,
        FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX    =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX,
        FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX,      // 관심도
        FISH_VIEWINFO_LISTCTRL_COLUMN_QUANTITY  =   CFishListCtrl::FISH_VIEWINFO_LISTCTRL_COLUMN_QUANTITY,
        MAX_STRING       =   CFishListCtrl::MAX_STRING,
    };   

    enum DATETYPE   {
        FISH_VIEWINFO_LISTCTRL_TWENTYFOURRULE    =   0,
        FISH_VIEWINFO_LISTCTRL_TWELVERULE   =   1,
    };
    enum LISTACTION { ADD, REMOVE, UPDATE, CLEAR, DEFAULT, };

private:
    DWORD           m_dwSortType;
    BOOL                m_bShowSrchStyle;
    deque<POSTITEM>  m_listListItems;
    vector<LVCOLUMN>    m_vecColumnInfo;
    CPoint          m_ptContext;
    VOID INSERT_COLUMN();
    VOID UPDATE_COLUMNSTATE(int sentinel);
    VOID ContextMenu_UpdateHeaderMenuStatus(CMenu* pMenu);

	static double	m_freadq;
	static double	m_flinkq;
	static double	m_fscrapq;
	static double	m_ffilterq;
	static double	m_fhitq;
	static double	m_fatbase;
    
// Construction
public:
	static void SetReadWeight(double f = -1);
	static void SetLinkWeight(double f = -1);
	static void SetScrapWeight(double f = -1);
	static void SetFilterWeight(double f = -1);
	static void SetHitWeight(double f = -1);
	static void SetAtBaseWeight(double f = 100);
	static double GetReadWeight()					{ return m_freadq; }
	static double GetLinkWeight()					{ return m_flinkq; }
	static double GetScrapWeight()					{ return m_fscrapq; }
	static double GetFilterWeight()					{ return m_ffilterq; }
	static double GetHitWeight()					{ return m_fhitq; }
	static double GetAtBaseWeight()					{ return m_fatbase; }

	CDlg_RSSList(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd* pParentWnd = NULL);
    VOID _initialize();
    VOID _finalize();
    VOID LOAD_STATE();
    VOID SAVE_STATE();

	VOID MoveControls();

	VOID RequestPostOpen(int id);
	VOID RequestPostScrap(list<int>& idlist);

    CString TranslateString(CString& str);

public:
    // 소트 관련 함수 클래스
    class SortCriterion : public std::binary_function<POSTITEM, POSTITEM, bool> 
    {
        int     m_type;
        bool    m_ascend;

    public:
        SortCriterion(int type = FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, bool ascend = true) {
            m_ascend    =   ascend;
            m_type      =   type;
        }

        bool operator() (POSTITEM p1, POSTITEM p2) const {
            switch(m_type) {
            case FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX:
                {
                    if (m_ascend) return p1.subject.Compare(p2.subject) < 0;
                    else return p1.subject.Compare(p2.subject) > 0;
                }
            	break;
            case FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX:    
                {
                    if (m_ascend) return p1.category.Compare(p2.category) < 0;
                    else return p1.category.Compare(p2.category) > 0;
                }
            	break;
            case FISH_VIEWINFO_LISTCTRL_DATE_INDEX:
                {
                    if (m_ascend) return (p1.pubdate > p2.pubdate) > 0;
                    else return (p1.pubdate < p2.pubdate) > 0;
                }
            	break;
            case FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX:
                {
                    if (m_ascend) return p1.author.Compare(p2.author) < 0;
                    else return p1.author.Compare(p2.author) > 0;
                }
            	break;
            case FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX:
                {
                    // 차후에 변경되어야 할 것으로 보임
                    if (m_ascend) return CDlg_RSSList::GetAttentionRate(&p1) < CDlg_RSSList::GetAttentionRate(&p2);
                    else return CDlg_RSSList::GetAttentionRate(&p1) > CDlg_RSSList::GetAttentionRate(&p2);
                }
            	break;
            case FISH_VIEWINFO_LISTCTRL_ICON_INDEX:
                {
                    if (m_ascend)   return (p1.read)?true:false;
                    else return (!p1.read)?true:false;
                }
                break;
            case FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX:
                {
                    if (m_ascend)   return (p1.stype) > (p2.stype);
                    else return (p1.stype) < (p2.stype);
                }
                break;
            default:
                assert(false);
                break;
            }
            return true;
        }
    };

public:
	VOID	List_Select(int id);
    VOID    List_Init();
	BOOL    List_Clear();
    BOOL    List_ItemAdd(POSTITEM pi);
    BOOL    List_ItemAdd(SmartPtr<POST_LIST>& pitems);
    BOOL    List_ItemUpdate(POSTITEM pi);
    BOOL    List_ItemRemove(int id);
	BOOL    List_Mode(BOOL bShowSrchStyle);
    BOOL    List_Sync(LISTACTION action = DEFAULT, POSTITEM* pi =   NULL);
    INT     List_FindByItemData(int data);

    CString GetIcon(POSTITEM* pi);
    CString GetDate(POSTITEM* pi);

    static UINT GetAttentionRate(POSTITEM* pi);
    static CString GetStringFromNumber(UINT num);
    


public:
	void SetSortType(DWORD SortType);
	DWORD GetSortType();
// Dialog Data
	//{{AFX_DATA(CDlg_RSSList)
	enum { IDD = IDD_DLG_RSSLIST };
	//}}AFX_DATA
    CFishListCtrl   m_listRSS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_RSSList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual VOID DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	// Generated message map functions
	//{{AFX_MSG(CDlg_RSSList)
	virtual BOOL OnInitDialog();
	afx_msg VOID OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg VOID OnDestroy();
	afx_msg VOID OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg VOID OnListHdAuthor();
	afx_msg VOID OnListHdDate();
	afx_msg VOID OnListHdGotoNext();
	afx_msg VOID OnListHdGotoPrev();
	afx_msg VOID OnListHdPublisher();
	afx_msg VOID OnListHdSubject();
	afx_msg VOID OnListItemGenerateHtml();
	afx_msg VOID OnListItemDelete();
	afx_msg VOID OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg VOID OnListSort(WPARAM wParam, LPARAM lParam);
	afx_msg VOID OnPaint();
	afx_msg void OnListItemGolink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	BOOL				m_bInit;
	BOOL				m_bSelfDraw;
	CWnd*				m_pParent;

    DATETYPE            m_enumDateType;
    
	// draw functions
	VOID DrawItems(CDC *pDC);
	VOID DrawBg(CDC *pBufferDC, CDC *pMemDC);
	VOID DrawOutline(CDC* pBufferDC);
	// --
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_RSSLIST_H__6DC0CB25_30ED_47F2_A863_818113A078A2__INCLUDED_)
