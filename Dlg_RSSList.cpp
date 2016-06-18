// Dlg_RSSList.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_RSSList.h"
#include "Dlg_RSSMain.h"
#include "Dlg_Browser.h"
#include "RSSCurrentList.h"
#include "FocusManager.h"
#include "MainFrm.h"
#include "HtmlMaker.h"
#include "fish_common.h"
#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_LIST_TITLE				_T("����")
#define STR_LIST_READFLAG			_T("��")
#define STR_LIST_PUBLISHER			_T("�з�")
#define STR_LIST_PUBDATE			_T("��¥")
#define STR_LIST_AUTHOR				_T("�۾���")
#define STR_LIST_EYEBALL			_T("���ɵ�")
#define STR_LIST_POSTSOURCE			_T("�˻���ġ")

#define STR_CAUTION					_T("���")
#define STR_CAUTION_DELETE			_T("������ ���� �Ͻðڽ��ϱ�?")

#define STR_ERR_LIST_HTMLTOOBIG		_T("������ HTML �������� �ʹ� Ů�ϴ�.!!")

#define STR_SEARCH_TYPE_NET			_T("��Ʈ")
#define STR_SEARCH_TYPE_KEYWORD		_T("Ű����")
#define STR_SEARCH_TYPE_LOCAL		_T("����")

#define STR_ERR_POSTSAVE			_T("Post Save Faile!")

#define USE_MAXRATE

double CDlg_RSSList::m_freadq = WEIGHT_READ_Q;
double CDlg_RSSList::m_flinkq = WEIGHT_LINK_CLICK_Q;
double CDlg_RSSList::m_fscrapq = WEIGHT_SCRAP_Q;
double CDlg_RSSList::m_ffilterq = WEIGHT_FILTER_Q;
double CDlg_RSSList::m_fhitq = WEIGHT_HIT_Q;
double CDlg_RSSList::m_fatbase = ATTENTION_BASE_VALUE;



IMPLEMENT_DYNCREATE(CDlg_RSSList, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSList dialog
CDlg_RSSList::CDlg_RSSList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_RSSList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_RSSList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInit			=   FALSE;
	m_bSelfDraw		=   TRUE;
    m_enumDateType  =   FISH_VIEWINFO_LISTCTRL_TWENTYFOURRULE;
    m_bShowSrchStyle  =   FALSE;
    m_dwSortType    =   -1;

	m_pParent		=   pParent;
}

VOID CDlg_RSSList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_RSSList)
	DDX_Control(pDX, IDC_LIST_RSS, m_listRSS);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_RSSList, CDialog)
	//{{AFX_MSG_MAP(CDlg_RSSList)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LIST_HD_AUTHOR, OnListHdAuthor)
	ON_COMMAND(ID_LIST_HD_DATE, OnListHdDate)
	ON_COMMAND(ID_LIST_HD_GOTO_NEXT, OnListHdGotoNext)
	ON_COMMAND(ID_LIST_HD_GOTO_PREV, OnListHdGotoPrev)
	ON_COMMAND(ID_LIST_HD_PUBLISHER, OnListHdPublisher)
	ON_COMMAND(ID_LIST_HD_SUBJECT, OnListHdSubject)
    ON_COMMAND(ID_LIST_ITEM_GENERATE_HTML, OnListItemGenerateHtml)
	ON_COMMAND(ID_LIST_ITEM_DELETE, OnListItemDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RSS, OnNMClick)
    ON_MESSAGE(WM_LISTVIEW_SORT, OnListSort)
	ON_WM_PAINT()
	ON_COMMAND(ID_LIST_ITEM_GOLINK, OnListItemGolink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_RSSList message handlers

BOOL CDlg_RSSList::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pParent = pParentWnd;
	
	return CDialog::Create(IDD, pParentWnd);
}

VOID CDlg_RSSList::_initialize()
{
}

VOID CDlg_RSSList::_finalize()
{
    SAVE_STATE();
}

/************************************************************************
LOAD_STATE  ����Ʈ ��Ʈ���� ���¸� �����Ѵ�.
@param  : 
@return : 
@remark : 
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/28:CREATED  �⺻���·� �÷� ���� ���ϱ�
************************************************************************/
VOID CDlg_RSSList::LOAD_STATE()
{
    //////////////////////////////////////////////////////////////////////////
    // �⺻���·μ� ������ Į���� ������ ���� Į���� �����Ѵ�.
    {
        int  nColumnCount = m_listRSS.GetHeaderCtrl()->GetItemCount();
        LPINT pnOrder = (LPINT) malloc(nColumnCount*sizeof(int));
        ASSERT(pnOrder != NULL);
        
        m_listRSS.GetColumnOrderArray(pnOrder, nColumnCount);
        
        pnOrder[0] =    FISH_VIEWINFO_LISTCTRL_ICON_INDEX;
        pnOrder[1] =    FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX;
        
        m_listRSS.SetColumnOrderArray(nColumnCount, pnOrder);
        free(pnOrder);
    }
}

VOID CDlg_RSSList::SAVE_STATE()
{

}

VOID CDlg_RSSList::MoveControls()
{
	if(!m_bInit) return;

    CRect clientRT;
    GetClientRect(&clientRT);
    
    if (m_listRSS)  
	{
        m_listRSS.MoveWindow(&clientRT);
		m_listRSS.PositionScrollBars();
    }
}

afx_msg BOOL CDlg_RSSList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bInit = TRUE;

    //////////////////////////////////////////////////////////////////////////
    // ����Ʈ ��Ʈ�� �ʱ�ȭ
    {
        List_Init();
    }

    //////////////////////////////////////////////////////////////////////////
    // ���̾ƿ� ����
    {
        MoveControls();
        m_listRSS.ShowWindow(SW_SHOW);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

afx_msg VOID CDlg_RSSList::OnSize(UINT nType, int cx, int cy) 
{
	MoveControls();
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2006-03-02
 *
 * @Description
 * �ش� ���̵� ���� ��ϻ��� ����Ʈ�� �������� �����Ѵ�.
 **************************************************************************/
VOID CDlg_RSSList::List_Select(int id)
{
	m_listRSS.SelectByData(id);
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Control�� �ʱ�ȭ ��Ų��.
 * �÷����� ó���Ѵ�.
 **************************************************************************/
VOID CDlg_RSSList::List_Init()
{
    m_listRSS._initialize();
//    m_listRSS.m_ctrlHeader.EnableWindow(FALSE);       // disabling the Header Control 

    m_listRSS.SetRedraw(FALSE);
    m_listRSS.SetBkColor(RGB(0xFF, 0xFF, 0xFF));
    m_listRSS.SetTextColor(RGB(0x00, 0x00, 0x00));

    INSERT_COLUMN();

    m_listRSS.SetRedraw(TRUE);
    ListView_SetExtendedListViewStyle(m_listRSS.m_hWnd, LVS_EX_FULLROWSELECT  | LVS_EX_HEADERDRAGDROP);

//    LOAD_STATE();
}

/************************************************************************
INSERT_COLUMN   ����Ʈ�� Į�� ������ �ǽ��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/16:CREATED
************************************************************************/
VOID CDlg_RSSList::INSERT_COLUMN()
{
    //////////////////////////////////////////////////////////////////////////
    // ����Ʈ ��Ʈ���� ���� 
    m_vecColumnInfo.reserve(FISH_VIEWINFO_LISTCTRL_COLUMN_QUANTITY);
    LVCOLUMN    item;
    item.mask   =   LVCF_FMT | LVCF_ORDER  | LVCF_TEXT | LVCF_WIDTH;   // LVCF_SUBITEM | LVCF_IMAGE

    {//FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   1;
        item.pszText=   STR_LIST_TITLE;
        item.cx     =   350;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX]=item;
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_ICON_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   0;
        item.pszText=   STR_LIST_READFLAG;
        item.cx     =   20;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_ICON_INDEX]=item;
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_ICON_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   2;
        item.pszText=   STR_LIST_PUBLISHER;
        item.cx     =   150;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX]=item;
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_DATE_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   3;
        item.pszText=   STR_LIST_PUBDATE;
        item.cx     =   100;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_DATE_INDEX]=item;
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_DATE_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   4;
        item.pszText=   STR_LIST_AUTHOR;
        item.cx     =   100;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX]=item;
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   5;
        item.pszText=   STR_LIST_EYEBALL;
        item.cx     =   100;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX]=item;

#ifndef MARKETING_VERSION
        if (m_bShowSrchStyle  ==   FALSE)    item.cx     =   0;  // �⺻ �����ÿ��� �˻� ��ġ�� ��Ÿ���� �ʾƾ���.
#endif
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, &item );
    }

    {//FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX
        item.fmt    =   LVCFMT_LEFT;
        item.iOrder =   6;
        item.pszText=   STR_LIST_POSTSOURCE;
        item.cx     =   100;
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX]=item;

        if (m_bShowSrchStyle    ==  FALSE)    item.cx     =   0;  // �⺻ �����ÿ��� �˻� ��ġ�� ��Ÿ���� �ʾƾ���.
        m_listRSS.InsertColumn( FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, &item );
    }
}

/************************************************************************
List_Mode   ����Ʈ�� ǥ����� 2������ �Ѱ����� �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/16:CREATED
************************************************************************/
BOOL CDlg_RSSList::List_Mode(BOOL bShowSrchStyle)
{
    BOOL RET    =   FALSE;
    m_bShowSrchStyle  =   bShowSrchStyle;
    switch (m_bShowSrchStyle)
    {
    case FALSE:
#ifndef MARKETING_VERSION
        m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, 0);
#endif
        m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, 0);
        break;
    case TRUE:
#ifndef MARKETING_VERSION
        m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX].cx);
#endif
        m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX].cx);
        break;
    default:
        break;
    }

//  UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX);
//  UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX);

    return RET;
}

/************************************************************************
UPDATE_COLUMNSTATE ����� Į���� ���� ������ ������
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2005/12/29:CREATED
************************************************************************/
VOID CDlg_RSSList::UPDATE_COLUMNSTATE(int sentinel)
{
    switch (sentinel)
    {
    case FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX:
        {// SUBJECT
            if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX) )
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX].cx);
            else
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, 0);
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_ICON_INDEX:
        {// ICON
            if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_ICON_INDEX) )
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_ICON_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_ICON_INDEX].cx);
            else
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_ICON_INDEX, 0);
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX:
        {// PUBLISHER
            if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX) )
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX].cx);
            else
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX, 0);
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_DATE_INDEX:
        {// DATE
            if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX) )
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_DATE_INDEX].cx);
            else
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX, 0);
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX:
        {// AUTHOR
            if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX) )
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX].cx);
            else
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX, 0);
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX:
        {// EYEBALL
#ifndef MARKETING_VERSION
            if (m_bShowSrchStyle    ==  FALSE)
            {
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, 0);
            }   else    {
#endif
                if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX) )
                    m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX].cx);
                else
                    m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, 0);
#ifndef MARKETING_VERSION
            }
#endif
        }
        break;
    case FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX:
        {// POST SOURCE
            if (m_bShowSrchStyle    ==  FALSE)
            {
                m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, 0);
            }   else    {
                if ( !m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX) )
                    m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX].cx);
                else
                    m_listRSS.SetColumnWidth(FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, 0);
            }
        }
        break;
    
    default:
        break;
    }
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Control�� ��� �������� �����Ѵ�.
 **************************************************************************/
BOOL CDlg_RSSList::List_Clear()
{
    // INTERNAL PROCESS
    m_listListItems.clear();
	return List_Sync(CLEAR);
}


CString CDlg_RSSList::TranslateString(CString& str)
{
    CString ret;
    ret = str;
//    ret = CString( _T("T") ); //for test
    ret.Replace( _T("&quote;"), _T("\"") );
    ret.Replace( _T("&quot;"), _T("\"") );
    ret.Replace( _T("&lt;"), _T("<") );
    ret.Replace( _T("&gt;"), _T(">") );
    ret.Replace( _T("&amp;"), _T("&") );
    ret.Replace( _T("&nbsp;"), _T(" ") );

    ret.Replace( _T("&trade;"), _T("��") );
    ret.Replace( _T("&sect;"), _T("��") );
    ret.Replace( _T("&aelig;"), _T("��") );
    ret.Replace( _T("&times;"), _T("��") );

    ret.Replace( _T("&#033;"), _T("!") );
    ret.Replace( _T("&#039;"), _T("'") );
    ret.Replace( _T("&#042;"), _T("*") );
    ret.Replace( _T("&#061;"), _T("=") );

    ret.Replace( _T("&#33;"), _T("!") );
    ret.Replace( _T("&#39;"), _T("'") );
    ret.Replace( _T("&#42;"), _T("*") );
    ret.Replace( _T("&#61;"), _T("=") );

    ret.Replace( _T("&#131;"), _T("?") );
    ret.Replace( _T("&#134;"), _T("��") );
    ret.Replace( _T("&#159;"), _T("?") );
    ret.Replace( _T("&#161;"), _T("��") );
    ret.Replace( _T("&#162;"), _T("��") );
    ret.Replace( _T("&#164;"), _T("��") );
    ret.Replace( _T("&#191;"), _T("��") );

    ret.Replace( _T("<b>"), _T("") );
    ret.Replace( _T("</b>"), _T("") );
    ret.Replace( _T("<u>"), _T("") );
    ret.Replace( _T("</u>"), _T("") );
    ret.Replace( _T("<i>"), _T("") );
    ret.Replace( _T("</i>"), _T("") );

    return ret;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Control�� �������� �߰��Ѵ�.
 * 2005/12/26 ���� ����
 **************************************************************************/
BOOL CDlg_RSSList::List_ItemAdd(POSTITEM pi)
{
    // INTERNAL PROCESS
    m_listRSS.SetRedraw(FALSE);

    m_listListItems.push_back(pi);
    BOOL RET    =   List_Sync(ADD, &pi);

    m_listRSS.PositionScrollBars();
    m_listRSS.SetRedraw(TRUE);
    m_listRSS.PositionScrollBars();
    m_listRSS.m_SkinHorizontalScrollbar.UpdateThumbPosition();
    m_listRSS.m_SkinVerticleScrollbar.UpdateThumbPosition();
    
    return RET;
}

/************************************************************************   
List_ItemAdd    ����Ʈ�� �������� �ټ��� �Է¹޴� ��ƾ
@PARAM  : 
@RETURN : 
@REMARK : 
    ������ �Ѱ� �Է��� ������ȣ�� �ϴ� ������δ� REDRAW�� �ݺ��������� Performance ������ �߻�����.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/08:CREATED
************************************************************************/
BOOL CDlg_RSSList::List_ItemAdd(SmartPtr<POST_LIST>& pitems)
{
    // INTERNAL PROCESS
    BOOL RET=TRUE;
    
    m_listRSS.SetRedraw(FALSE);

    list<POSTITEM>::iterator    it  =   pitems->begin();
    for (it; it!=pitems->end(); it++)
    {
        m_listListItems.push_back(*it);
        List_Sync(ADD, &(*it));
    }
    if ( GetSortType() != -1)    OnListSort( GetSortType(), NULL);
    m_listRSS.SetRedraw(TRUE);
    m_listRSS.PositionScrollBars();
    m_listRSS.m_SkinHorizontalScrollbar.UpdateThumbPosition();
    m_listRSS.m_SkinVerticleScrollbar.UpdateThumbPosition();
    return RET;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Control�� �������� �����Ѵ�.
 **************************************************************************/
BOOL CDlg_RSSList::List_ItemRemove(int id)
{
    // INTERNAL PROCESS
    deque<POSTITEM>::iterator it =   m_listListItems.begin();
    for(;it!=m_listListItems.end(); it++)
    {
        if ( it->postid == id ) 
        {
            m_listListItems.erase(it);
            break;
        }
    }

    POSTITEM pi;
    pi.postid   =   id;

    BOOL RET    =   List_Sync(REMOVE, &pi);
    
    m_listRSS.PositionScrollBars();
    m_listRSS.m_SkinHorizontalScrollbar.UpdateThumbPosition();
    m_listRSS.m_SkinVerticleScrollbar.UpdateThumbPosition();
    
	return RET;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Control�� �������� �����Ѵ�.
 **************************************************************************/
BOOL CDlg_RSSList::List_ItemUpdate(POSTITEM pi)
{
    // INTERNAL PROCESS
    BOOL RET    =   TRUE;
    deque<POSTITEM>::iterator it =   m_listListItems.begin();
    for(;it!=m_listListItems.end(); it++)
    {
        if ( it->postid == pi.postid ) 
        {
            *it = pi;
            break;
        }
    }
    
    m_listRSS.SetRedraw(FALSE);

    RET    =   List_Sync(UPDATE, &pi);

    m_listRSS.SetRedraw(TRUE);
   
    m_listRSS.PositionScrollBars();
    m_listRSS.m_SkinHorizontalScrollbar.UpdateThumbPosition();
    m_listRSS.m_SkinVerticleScrollbar.UpdateThumbPosition();
    return RET;
}

static CString sSearchType[] = {STR_SEARCH_TYPE_NET, STR_SEARCH_TYPE_KEYWORD, STR_SEARCH_TYPE_LOCAL};

/************************************************************************
List_Sync   ���������� ����Ʈ�� �������� ��ũ��Ų��. �⺻������ ��ũ �׼��� �⺻
@PARAM  : 
@RETURN : 
@REMARK : 
    ���� ������ ��� �ӵ� ������ ���� ó���Ѵ�.

    REMOVE : ���ڷ� ���� POSTITEM �� postid �׸� �ϳ������� �۵��Ѵ�.
    UPDATE : ���ڷ� ���� POSTITME�� postid �� ��ġ�ϴ� �׸� ���ؼ� ������Ʈ�� ����

    �������߰��� �ߺ��˻�� �Լ� ȣ�� ���� ������ �̹� �ѹ� �������� ������ �����ȴ�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/03:CREATED
************************************************************************/
BOOL CDlg_RSSList::List_Sync(enum CDlg_RSSList::LISTACTION action, POSTITEM* pi)
{
    BOOL RET = FALSE;
    switch (action)
    {
    case ADD:
        {
            int nINDEX = m_listRSS.GetItemCount();
            
            // ITEM DRAW
            m_listRSS.InsertItem(nINDEX, pi->subject);
            m_listRSS.SetItemData(nINDEX, pi->postid);
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, TranslateString(pi->subject) );
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX, pi->category );
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX, pi->author );
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_ICON_INDEX, GetIcon(pi));
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_DATE_INDEX, GetDate(pi));
            m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, GetStringFromNumber(GetAttentionRate(pi)) );

			if(pi->stype <= LOCAL_SEARCH && pi->stype >= 0)
				m_listRSS.SetItemText(nINDEX, FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, sSearchType[pi->stype]/*����� ������ �ý�Ʈ ���Ŀ� ��ó ���� �ڵ尡 ������.*/ );
            break;
        }
    case REMOVE:
        {
            int idx = 0;
	        for(; idx < m_listRSS.GetItemCount(); idx++)
	        {
		        DWORD p = m_listRSS.GetItemData(idx);
		        if( p == pi->postid )  break;
	        }
            m_listRSS.DeleteItem(idx);
            
            break;
        }
    case UPDATE:
        {
            int idx = 0;
	        for(; idx < m_listRSS.GetItemCount(); idx++)
	        {
		        DWORD p = m_listRSS.GetItemData(idx);
		        if( p == pi->postid )  break;
	        }
            
            // ITEM DRAW
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX, TranslateString(pi->subject) );
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX, pi->category );
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX, pi->author );
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_ICON_INDEX, GetIcon(pi) );
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_DATE_INDEX, GetDate(pi));
            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX, GetStringFromNumber(GetAttentionRate(pi)) );

			if(pi->stype <= LOCAL_SEARCH && pi->stype >= 0)
				m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, sSearchType[pi->stype]/*����� ������ �ý�Ʈ ���Ŀ� ��ó ���� �ڵ尡 ������.*/ );

//            m_listRSS.SetItemText(idx, FISH_VIEWINFO_LISTCTRL_POSTSOURCE_INDEX, _T("LOCAL")/*����� ������ �ý�Ʈ ���Ŀ� ��ó ���� �ڵ尡 ������.*/ );
            break;
        }
    case CLEAR:
        {
            RET =   m_listRSS.DeleteAllItems();
            break;
        }
    default:
        {
            if (m_listRSS.GetItemCount() != 0)  List_Sync(CLEAR);
            deque<POSTITEM>::iterator it = m_listListItems.begin();
            for(; it!=m_listListItems.end(); it++)
            {
                List_Sync(ADD, &(*it));
            }
            break;
        }
    }
    return RET;
}

/************************************************************************
OnListSort  ���ڷ� ���� subitem �� ������ ���߾ ������ �ǽ��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/03:CREATED
************************************************************************/
afx_msg VOID CDlg_RSSList::OnListSort(WPARAM wParam, LPARAM lParam)
{
    SetSortType( wParam );

    // TODO: SORT ACTION
    if ( GetSortType() == FISH_VIEWINFO_LISTCTRL_DATE_INDEX )
    {
        std::stable_sort(m_listListItems.begin(), m_listListItems.end(), SortCriterion(GetSortType(), m_listRSS.m_bAscendingSort));
    }   else    {
        std::stable_sort(m_listListItems.begin(), m_listListItems.end(), SortCriterion(FISH_VIEWINFO_LISTCTRL_DATE_INDEX, m_listRSS.m_bAscendingSort));
        std::stable_sort(m_listListItems.begin(), m_listListItems.end(), SortCriterion(GetSortType(), m_listRSS.m_bAscendingSort));
    }

    List_Sync();
    return;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-23
 *
 * @Description
 * List Item�� Data���� ���� List���� Index�� �����´�.
 **************************************************************************/
int CDlg_RSSList::List_FindByItemData(int data)
{
	DWORD p;
	for(int i = 0; i< m_listRSS.GetItemCount(); i++)
	{
		p = m_listRSS.GetItemData(i);
		if(p == data)
			return i;
	}

	return -1;
}

/**************************************************************************
 * written by moonknit
 *
 * @History
 * created 2005-11-24
 *
 * @Description
 * List�� Ŭ���ϸ� Post���⸦ ��û�Ѵ�.
 **************************************************************************/
afx_msg VOID CDlg_RSSList::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    {
        POSITION pos    =   m_listRSS.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            int idx     =   m_listRSS.GetNextSelectedItem(pos);
            int postid  =   m_listRSS.GetItemData(idx);
	        RequestPostOpen( postid );
        }
    }

    *pResult = 0;
}

/**************************************************************************
 * method CDlg_RSSList::RequestPostOpen
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-24
 *
 * @Description
 * id�� �̿��� �ش� ����Ʈ�� �о� RSS ����â�� ǥ���Ѵ�.
 *
 * @Parameters
 * (in int) id - �о�� ����Ʈ
 **************************************************************************/
VOID CDlg_RSSList::RequestPostOpen(int id)
{
	CDlg_RSSMain* parent = (CDlg_RSSMain*) m_pParent;
	if(!parent || !(parent->m_spBrowser) ) return;

	CString html;
	theApp.m_spCL->GetPostXMLStream(id, html, TRUE);

	// ��Ʈ���� �̿��Ͽ� �� �������� ����.
	parent->ViewPosts(html);
}


/**************************************************************************
 * method CDlg_RSSList::RequestPostScrap
 *
 * written by moonknit
 *
 * @History
 * created 2005-11-28
 *
 * @Description
 * post�� ��ũ���Ѵ�.

 * ���� ��û..
 * theRSSCurrent�� �����̿��ϸ� ��

 
 * @Parameters
 * (in list<int>&) idlist - ��ũ���� Post�� id ���
 *
 **************************************************************************/
VOID CDlg_RSSList::RequestPostScrap(list<int>& idlist)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	if(!pFrame) return;

	pFrame->Scrap(idlist);
}

//////////////////////////////////////////////////////////////////////////
// FOR INVALIDATION

VOID CDlg_RSSList::DrawItems(CDC *pDC)
{
}

VOID CDlg_RSSList::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
}

VOID CDlg_RSSList::DrawOutline(CDC* pBufferDC)
{
	if(!pBufferDC) return;
}


afx_msg BOOL CDlg_RSSList::OnEraseBkgnd(CDC* pDC) 
{
//    return CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}

/**************************************************************************
 * CDlg_RSSList::OnDestroy
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-09
 *
 * @Description
 * ����� CRSSCurrentList�� Inform Dialog�� �����Ѵ�.
 **************************************************************************/
afx_msg VOID CDlg_RSSList::OnDestroy() 
{
	TRACE(_T("CDlg_RSSList Destroy\r\n"));
	CDialog::OnDestroy();
	
	theApp.m_spCL->SetListDlg(SmartPtr<CDlg_RSSList> (NULL));

}

afx_msg VOID CDlg_RSSList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    CPoint m_ptContext      =   point;
    CPoint ptInList         =   m_ptContext;
    m_listRSS.ScreenToClient(&ptInList);

    {
        CMenu FISHMENU;
        if( !FISHMENU.LoadMenu(IDR_MENU_VIEWINFO) ) return;

        // LIST HEADER CONTEXT MENU
        CRect RT;
        m_listRSS.GetHeaderCtrl()->GetClientRect(&RT);
        
        if ( RT.PtInRect(ptInList) ) {
            CMenu* pContext = (CMenu*)FISHMENU.GetSubMenu(0);
            if(!pContext) return;
            
            ContextMenu_UpdateHeaderMenuStatus(pContext);
            pContext->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, m_ptContext.x, m_ptContext.y, this);
            return;
        }

        // LIST CONTENTS CONTEXT MENU
        CMenu* pContext = (CMenu*)FISHMENU.GetSubMenu(1);
        if(!pContext) return;
        pContext->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, m_ptContext.x, m_ptContext.y, this);
        return;        
    }
}

/************************************************************************
ContextMenu_UpdateHeaderMenuStatus  ����� ���� ������ ������� ���ؽ�Ʈ �޴��� ����� �����.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2005/12/29:CREATED
************************************************************************/
VOID CDlg_RSSList::ContextMenu_UpdateHeaderMenuStatus(CMenu* pMenu)
{
    // PUBLISHER
    pMenu->CheckMenuItem(ID_LIST_HD_PUBLISHER, m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX)?MF_CHECKED:MF_UNCHECKED);

    // SUBJECT
    pMenu->CheckMenuItem(ID_LIST_HD_SUBJECT, m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX)?MF_CHECKED:MF_UNCHECKED);

    // SUBJECT
    pMenu->CheckMenuItem(ID_LIST_HD_DATE, m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX)?MF_CHECKED:MF_UNCHECKED);

    // AUTHOR
    pMenu->CheckMenuItem(ID_LIST_HD_AUTHOR, m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX)?MF_CHECKED:MF_UNCHECKED);

    return;
}

VOID CDlg_RSSList::OnListHdAuthor() 
{
	// TODO: Add your command handler code here
    if ( m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX) )
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX].cx  =  m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX);
    UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_AUTHOR_INDEX);
}

afx_msg VOID CDlg_RSSList::OnListHdDate() 
{
	// TODO: Add your command handler code here
    if ( m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX) )
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_DATE_INDEX].cx = m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_DATE_INDEX);
    UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_DATE_INDEX);
}

afx_msg VOID CDlg_RSSList::OnListHdPublisher() 
{
	// TODO: Add your command handler code here
    if ( m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX) )
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX].cx = m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX);
    UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_CATEGORY_INDEX);
}

afx_msg VOID CDlg_RSSList::OnListHdSubject() 
{
	// TODO: Add your command handler code here
    if ( m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX) )
        m_vecColumnInfo[FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX].cx = m_listRSS.GetColumnWidth(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX);
    UPDATE_COLUMNSTATE(FISH_VIEWINFO_LISTCTRL_SUBJECT_INDEX);
}

afx_msg VOID CDlg_RSSList::OnListHdGotoNext() 
{
	// TODO: Add your command handler code here
	
}

afx_msg VOID CDlg_RSSList::OnListHdGotoPrev() 
{
	// TODO: Add your command handler code here
	
}

/************************************************************************
OnListItemGenerateHtml  HTML ���� �ۼ���
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2005/12/29:CREATED
************************************************************************/
#define MAX_HTMLLENGTH						1024 * 1024 * 4  // 1024k bytes
afx_msg VOID CDlg_RSSList::OnListItemGenerateHtml() 
{
    list<int> idlist;
    POSITION pos = m_listRSS.GetFirstSelectedItemPosition();

    if (pos == NULL)    {
        TRACE0("No items were selected!\n");
        return;
    }   else    {
        while (pos)
        {
            int nITEM = m_listRSS.GetNextSelectedItem(pos);
            idlist.push_back( m_listRSS.GetItemData(nITEM) );
        }
    }

	
	/*
	CString html;

    if(!theApp.m_spCL->GetPostXMLStream( idlist, html ))
	{
		// html �������� ����
		return;
	}
	if(html.GetLength() > MAX_HTMLLENGTH)
	{
		AfxMessageBox(STR_ERR_LIST_HTMLTOOBIG);
	}	

	CDlg_RSSMain* parent = (CDlg_RSSMain*) m_pParent;
	if(!parent || !(parent->m_spBrowser) ) return;

	parent->ViewPosts(html);
	*/	

    CFileDialog dlg(FALSE, STR_XML_FORMAT, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, STR_HTML_FILE_FORMAT, this);	

    if(dlg.DoModal() == IDOK)
    {
        CString fpath = dlg.GetPathName();
		if(theApp.m_spCL->SavePost(idlist, fpath))
		{
			TRACE(_T("Save Post File : %s\n"), fpath);
		}
		else
			FishMessageBox(STR_ERR_POSTSAVE);
    }

    return;
}

afx_msg VOID CDlg_RSSList::OnListItemDelete() 
{
	// TODO: Add your command handler code here
    if ( IDYES   ==  FishMessageBox(STR_CAUTION_DELETE, STR_CAUTION, MB_YESNO | MB_ICONWARNING) )
    {
        m_listRSS.SetRedraw(FALSE);

        list<int> idlist;
        POSITION pos    =   m_listRSS.GetFirstSelectedItemPosition();
        POSITION next   =   m_listRSS.GetFirstSelectedItemPosition();
        int nIdx  =   m_listRSS.GetNextSelectedItem(next);

        if (pos == NULL)    {
            TRACE0("No items were selected!\n");
            return;
        }   else    {
            while (pos)
            {
                int nIndex = m_listRSS.GetNextSelectedItem(pos);
                idlist.push_back( m_listRSS.GetItemData(nIndex) );
            }
        }

        // RSSDB REMOVE
        theApp.m_spCL->DeletePost(idlist);

        // LISTCTRL REMOVE
        list<int>::iterator it = idlist.begin();
        for(; it != idlist.end(); it++)
        {
            List_ItemRemove( (*it) );
        }

        //////////////////////////////////////////////////////////////////////////
        // NEXT ITEM SELECTION
        if (nIdx != -1 && m_listRSS.GetItemCount() > 0)
			//&& )
        {
            //m_listRSS.SetItemState(nIdx, 0, LVIS_SELECTED);
			DWORD dwData = m_listRSS.GetItemData(nIdx);
			if(dwData)
			{
				List_Select( dwData );
				RequestPostOpen( dwData );
			}
        }

        m_listRSS.SetRedraw(TRUE);
        m_listRSS.Invalidate(FALSE);
    }
}

VOID CDlg_RSSList::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CDialog::OnPaint() for painting messages
}

CString CDlg_RSSList::GetIcon(POSTITEM* pi)
{
    if (pi->read)   return POST_READ;
    else return POST_UNREAD;
    
}

CString CDlg_RSSList::GetDate(POSTITEM* pi)  
{
    if ( m_enumDateType == FISH_VIEWINFO_LISTCTRL_TWENTYFOURRULE )  return pi->pubdate.Format( _T("%Y-%m-%d %H:%M:%S") );     //http://msdn.microsoft.com/library/en-us/vclib/html/_crt_strftime.2c_.wcsftime.asp?frame=true
    else    return pi->pubdate.Format( _T("%Y-%m-%d %I:%M:%S %p") );
}

/************************************************************************
GetAttentionRate    ���ɵ��� ���Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
    �׸��� �׸��� �Է¹޴� ���� 0~100 �̾���ϱ� ������ ��°��� �׻�
    0~100 ���̾���Ѵ�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/17:CREATED
************************************************************************/
UINT CDlg_RSSList::GetAttentionRate(POSTITEM* pi)  
{
    UINT ret = (
		pi->svread * m_freadq 
		+ pi->svhit * m_fhitq
		+ pi->svscrap * m_fscrapq
		+ pi->svreadon * m_flinkq
		+ pi->svfilter * m_ffilterq);
#ifdef USE_MAXRATE
	ret = (ret * 100) / m_fatbase;
#endif
    if ( ret < 0 )  ret =   0;
    else    if (ret >   100)    ret =   100;
    return ret;
}

CString CDlg_RSSList::GetStringFromNumber(UINT num)
{
    CString szRet;
    szRet.Format(_T("%d"), num);
    return szRet;
}

BOOL CDlg_RSSList::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		
        if(pMsg->wParam == VK_RETURN)
		{
            //////////////////////////////////////////////////////////////////////////
            // ENTER KEY PRESS PROCESS
            POSITION pos    =   m_listRSS.GetFirstSelectedItemPosition();
            if (pos != NULL)
            {
                int idx     =   m_listRSS.GetNextSelectedItem(pos);
                int postid  =   m_listRSS.GetItemData(idx);
	            RequestPostOpen( postid );
            }
            return TRUE;
		}

        if(pMsg->wParam == VK_DELETE)
        {
            OnListItemDelete();
			return TRUE;
        }

        if(pMsg->wParam ==  VK_TAB)
        {
            CFocusManager::getInstance()->SetNextFocus();
            return TRUE;
        }

        if(pMsg->wParam ==  VK_RIGHT)
        {
            CFocusManager::getInstance()->MoveToTreeCtrl();
            return TRUE;
        }

        if(pMsg->wParam ==  VK_LEFT)
        {
            CFocusManager::getInstance()->MoveToTreeCtrl();
            return TRUE;
        }
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

DWORD CDlg_RSSList::GetSortType()
{
    return m_dwSortType;
}

void CDlg_RSSList::SetSortType(DWORD SortType)
{
    m_dwSortType    =   SortType;
}


void CDlg_RSSList::OnListItemGolink() 
{
    POSITION pos    =   m_listRSS.GetFirstSelectedItemPosition();
	int nIdx, nPostID;

	while(pos)
	{
		nIdx = m_listRSS.GetNextSelectedItem(pos);
		nPostID = m_listRSS.GetItemData(nIdx);

		// ��ũ Ŭ�� ó��
		theApp.m_spCL->ReadOnPost(nPostID, TRUE);
	}
}

void CDlg_RSSList::SetReadWeight(double f)
{ if(f < 0) f = WEIGHT_READ_Q; if(f != m_freadq) m_freadq = f; }
void CDlg_RSSList::SetLinkWeight(double f)
{ if(f < 0) f = WEIGHT_LINK_CLICK_Q; if(f != m_flinkq) m_flinkq = f; }
void CDlg_RSSList::SetScrapWeight(double f)
{ if(f < 0) f = WEIGHT_SCRAP_Q; if(f != m_fscrapq) m_fscrapq = f; }
void CDlg_RSSList::SetFilterWeight(double f)
{ if(f < 0) f = WEIGHT_FILTER_Q; if(f != m_ffilterq) m_ffilterq = f; }
void CDlg_RSSList::SetHitWeight(double f)
{ if(f < 0) f = WEIGHT_HIT_Q; if(f != m_fhitq) m_fhitq = f; }
void CDlg_RSSList::SetAtBaseWeight(double f)
{ if(f < 100) f = ATTENTION_BASE_VALUE; if(f != m_fatbase) m_fatbase = f; }
