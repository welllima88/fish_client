// Dlg_Properties.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_Properties.h"
#include "Dlg_Prop_General.h"
#include "Dlg_Prop_Network.h"
#include "Dlg_Prop_Style.h"
#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Properties dialog

#define STR_DP_NETWORK_TAB			_T("네트워크")
#define STR_DP_NETWORK_PAGE			_T("네트워크 설정")

#define STR_DP_GENERAL_TAB			_T("일반")
#define STR_DP_GENERAL_PAGE			_T("일반 설정")
#define STR_DP_TITLE				_T("환경설정")

#define STR_DP_STYLE_TAB			_T("스타일")
#define STR_DP_STYLE_PAGE			_T("스타일 설정")



CDlg_Properties::CDlg_Properties(CWnd* pParent /*=NULL*/)
: CCustomDlg(CDlg_Properties::IDD, pParent, STR_DP_TITLE)
{
	//{{AFX_DATA_INIT(CDlg_Properties)
	//}}AFX_DATA_INIT
	menuseq		= 0;
	bInit		= FALSE;
	bModified	= FALSE;
}


void CDlg_Properties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Properties)
	DDX_Control(pDX, IDC_LIST_MENU, m_listMenu);
	DDX_Control(pDX, IDC_BTN_APPLY, m_btnApply);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Properties, CCustomDlg)
	//{{AFX_MSG_MAP(CDlg_Properties)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_LBN_SELCHANGE(IDC_LIST_MENU, OnSelchangeListMenu)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CHILD_INIT, OnChildInit)
	ON_MESSAGE(WM_CHILD_MODIFIED, OnChildModified)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Properties message handlers

void CDlg_Properties::AddPropPage(SmartPtr<CDlg_PropUnit>& page, CString menuname, CString title)
{
	if(title != _T(""))
		page->Title = title;

	if(menuname != _T(""))
		page->MenuName = menuname;

	if(page->MenuName.GetLength() == 0) return;

	int id = ++menuseq;
	PROPMAPITERATOR it = m_Propmap.find(id);
	while(it != m_Propmap.end())
	{
		++id;
		it = m_Propmap.find(id);
	}

	int nsize = m_listMenu.GetCount();
	if(m_listMenu.InsertString(nsize, page->MenuName) == -1)
		return;

	m_listMenu.SetItemData(nsize, id);
	m_listMenu.MoveWindow(MARGIN_LEFT, MARGIN_TOP, LIST_WIDTH, LIST_HEIGHT);

	page->PropID = id;

	m_Propmap.insert(PROPMAP::value_type(id, page));
}

BOOL CDlg_Properties::OnInitDialog() 
{
	CCustomDlg::OnInitDialog();

	//SetWindowText(STR_DP_TITLE);

	m_penOutline.CreatePen(PS_SOLID, 1, RGB(185, 185, 185));
	m_fontNormal.CreateFont(15, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	m_btnApply.EnableWindow(FALSE);


	SmartPtr<CDlg_PropUnit> GeneralPage(new CDlg_Prop_General(this));
	AddPropPage(GeneralPage, STR_DP_GENERAL_TAB, STR_DP_GENERAL_PAGE);
	SmartPtr<CDlg_PropUnit> NetworkPage(new CDlg_Prop_Network(this));
	AddPropPage(NetworkPage, STR_DP_NETWORK_TAB, STR_DP_NETWORK_PAGE);
	SmartPtr<CDlg_PropUnit> StylePage(new CDlg_Prop_Style(this));
	AddPropPage(StylePage, STR_DP_STYLE_TAB, STR_DP_STYLE_PAGE);

	currentpage = GeneralPage;

	PROPMAPITERATOR it;
	for(it = m_Propmap.begin(); it != m_Propmap.end(); ++it)
	{		
		(*it).second->Create(NULL, GeneralPage->Title, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, GeneralPage->nResourceID);
		(*it).second->ModifyStyle(0, WS_TABSTOP);		
		(*it).second->ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	}
	
	currentpage->ShowWindow(SW_NORMAL);		

	bInit = TRUE;

    //////////////////////////////////////////////////////////////////////////
    // List Ctrl 속성
	m_listMenu.SetCurSel(0);
	m_listMenu.SetFont(&m_fontNormal);
	m_listMenu.SetItemGap(3);
	m_listMenu.SetColors(RGB(229, 229, 229), RGB(0, 30, 183), RGB(243, 243, 243), RGB(69, 69, 69), RGB(255, 255, 255), RGB(255, 255, 255));
	m_listMenu.SetFontNormal(&m_fontNormal);
	m_listMenu.SetFontSelected(&m_fontBold);
	m_listMenu.SetItemHeight(0, 25);
	m_listMenu.SetMargins(4, 4);

	CRect rect;
	GetWindowRect(rect);
	MoveWindow(rect.left, rect.top, WIDTH, HEIGHT);

	m_btnOK.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnOK.SetShowText(FALSE);
	m_btnOK.SetFlatFocus(TRUE);
	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnCancel.SetShowText(FALSE);
	m_btnCancel.SetFlatFocus(TRUE);
	m_btnApply.LoadBitmaps(IDB_BTN_APPLY_N, IDB_BTN_APPLY_D, IDB_BTN_APPLY_O, IDB_BTN_APPLY_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnApply.SetShowText(FALSE);
	m_btnApply.SetFlatFocus(TRUE);

	MoveControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Properties::OnChildInit(WPARAM wParam, LPARAM lParam)
{

}

void CDlg_Properties::OnChildModified(WPARAM wParam, LPARAM lParam)
{
	bModified = TRUE;
	m_btnApply.EnableWindow(TRUE);
}

void CDlg_Properties::MoveControls()
{
	CRect crect;
	GetClientRect(crect);

	if(m_btnCancel.m_hWnd)
	{
		m_btnCancel.MoveWindow(crect.right - MARGIN_LEFT - BTN_WIDTH, crect.bottom - MARGIN_BOTTOM - BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT);
		m_btnCancel.GetWindowRect(crect);
		ScreenToClient(crect);
		m_btnOK.MoveWindow(crect.left - BTN_WIDTH - 10, crect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btnOK.GetWindowRect(crect);
		ScreenToClient(crect);
		m_btnApply.MoveWindow(crect.left - BTN_WIDTH - 10, crect.top, BTN_WIDTH, BTN_HEIGHT);
	}


	CalcPageRect(crect);

	if(!currentpage) {}
	else currentpage->MoveWindow(crect);
}

void CDlg_Properties::CalcPageRect(CRect& rect)
{
	if(!bInit) return;

	CRect tmprect;
	GetClientRect(&rect);

	m_listMenu.GetClientRect(&tmprect);
	m_listMenu.ClientToScreen(&tmprect);
	ScreenToClient(&tmprect);
	
	rect.left = tmprect.right + LIST_MARGIN_RIGHT;
	m_btnOK.GetClientRect(&tmprect);
	m_btnOK.ClientToScreen(&tmprect);
	ScreenToClient(&tmprect);

	rect.top += MARGIN_TOP + 2;
	rect.right -= MARGIN_RIGHT;
	rect.bottom = tmprect.top - 5;
}

void CDlg_Properties::OnSize(UINT nType, int cx, int cy) 
{
	CCustomDlg::OnSize(nType, cx, cy);
	
	MoveControls();
}

void CDlg_Properties::OnOK() 
{
	OnBtnApply();
	
	CCustomDlg::OnOK();
}

void CDlg_Properties::OnBtnApply() 
{
	if(!bModified) return;

	PROPMAPITERATOR it;
	for(it = m_Propmap.begin(); it != m_Propmap.end(); ++it)
	{
		(*it).second->Save();
	}

	bModified = FALSE;
	m_btnApply.EnableWindow(FALSE);
}

BOOL CDlg_Properties::PreTranslateMessage(MSG* pMsg) 
{
	return CCustomDlg::PreTranslateMessage(pMsg);
}

void CDlg_Properties::OnSelchangeListMenu() 
{
	int nItem = m_listMenu.GetCurSel();
	if(-1 == nItem) return;
	DWORD dwData = m_listMenu.GetItemData(nItem);

	PROPMAPITERATOR it = m_Propmap.find(dwData);
	if(it != m_Propmap.end())
	{
		BOOL bnewlyselected = FALSE;
		if(!currentpage)
		{
			currentpage = (*it).second;
			bnewlyselected = TRUE;
		}
		else
		{
			if((*it).second->PropID != currentpage->PropID)
			{
				currentpage->ShowWindow(SW_HIDE);
				currentpage = (*it).second;
				bnewlyselected = TRUE;
			}
		}

		if(bnewlyselected)
		{
			CRect rect;
			CalcPageRect(rect);
			currentpage->MoveWindow(rect);
			currentpage->ShowWindow(SW_NORMAL);
		}
	}
}

void CDlg_Properties::OnPaint()
{
	CPaintDC dc(this);

	DrawItems(&dc);

	CRect rect;
	if(m_listMenu.m_hWnd)
	{
		m_listMenu.GetWindowRect(rect);
		ScreenToClient(rect);
		rect.InflateRect(1, 1, 1, 1);
		
		CPen* pOldPen = (CPen*) dc.SelectObject(&m_penOutline);
		dc.Rectangle(&rect);
		dc.SelectObject(pOldPen);		
	}
}

void CDlg_Properties::OnDestroy()
{
	if(m_fontNormal.m_hObject)
		m_fontNormal.DeleteObject();
	if(m_penOutline.m_hObject)
		m_penOutline.DeleteObject();

	CCustomDlg::OnDestroy();
}