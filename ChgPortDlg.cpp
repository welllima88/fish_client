// ChgPortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fish_common.h"
#include "FISH.h"
#include "ChgPortDlg.h"
#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChgPortDlg dialog


CChgPortDlg::CChgPortDlg(CWnd* pParent /*=NULL*/)
	: CCustomDlg(CChgPortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChgPortDlg)
	m_strPort = _T("");
	//}}AFX_DATA_INIT
}
CChgPortDlg::~CChgPortDlg()
{
	DetachObjects();
}


void CChgPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CCustomDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChgPortDlg)
	DDX_Text(pDX, IDC_CPD_PORT, m_strPort);
	DDV_MaxChars(pDX, m_strPort, 6);
	DDX_Control(pDX, IDC_CPD_PORT, m_edtPort);
	DDX_Control(pDX, IDOK, m_btnChg);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChgPortDlg, CCustomDlg)
	//{{AFX_MSG_MAP(CChgPortDlg)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChgPortDlg message handlers

void CChgPortDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CCustomDlg::OnCancel();
}

void CChgPortDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	m_strPort.TrimLeft();
	m_strPort.TrimRight();
	if( m_strPort == "" )
	{
		if( _ttoi(m_strPort) == 0 )
		{
			FishMessageBox(STR_MSG_CHGPORTDLG_001, STR_MSG_CHGPORTDLG_002, MB_OK | MB_ICONINFORMATION );
			return;
		}
		FishMessageBox(STR_MSG_CHGPORTDLG_003, STR_MSG_CHGPORTDLG_004, MB_OK | MB_ICONINFORMATION );
		return;
	}
	int nPort = _ttoi(m_strPort);

	// 범위 점검
	if( nPort > 49000 )
	{
		FishMessageBox(STR_MSG_CONFIGVIEW_030, STR_MSG_CONFIGVIEW_029, MB_OK | MB_ICONINFORMATION );
		return;
	}
	if( nPort <= 0 )
	{
		FishMessageBox(STR_MSG_CONFIGVIEW_031, STR_MSG_CONFIGVIEW_029, MB_OK | MB_ICONINFORMATION );
		return;
	}

	CCustomDlg::OnOK();
}


BOOL CChgPortDlg::OnInitDialog()
{
//	SetSubTitleImage(IDB_SUBDLG_CHGPORT);
	CCustomDlg::OnInitDialog();

	LoadObjects();
	
	MoveControls();

	return TRUE;
}

afx_msg BOOL CChgPortDlg::OnEraseBkgnd(CDC* pDC)
{	
	CCustomDlg::OnEraseBkgnd(pDC); 
	Drawing2(pDC);
	return TRUE;
}

void CChgPortDlg::MoveControls()
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	
	CRect rectCtrl;
	
	int nLeft = rectDlg.left + 14;
	int nRight = rectDlg.right - 14;
	int nTop = SUB_TITLE_IMAGE_HEIGHT + SUB_SUBTITLE_IMAGE_HEIGHT + 10;	
	int nTemp = 0;
	//Static
	m_rectTitle.top = nTop;
	m_rectTitle.left = nLeft - 1;
	m_rectTitle.right = nRight;
	m_rectTitle.bottom = m_rectTitle.top + 13;

	nTop += 13;
	nTop += 10;
	nTop += 1;
 
	//Edit
	if( m_edtPort.GetSafeHwnd())
	{
		m_edtPort.GetWindowRect(&rectCtrl);
		nTemp = 16;//ScreenToClient(&rectCtrl);
		rectCtrl.left = nLeft + 1;
		rectCtrl.right = nRight - 1;
		rectCtrl.top = nTop + 3;
		rectCtrl.bottom = rectCtrl.top + nTemp;
		m_edtPort.MoveWindow(&rectCtrl);	
		
		//Back Rect
		m_rectPortBack.top = nTop;
		m_rectPortBack.bottom = m_rectPortBack.top + 19;
		m_rectPortBack.left = nLeft;
		m_rectPortBack.right = nRight;
	}
	
	//Sept
	m_rectSept = m_rectPortBack;
	m_rectSept.top = m_rectSept.bottom;
	m_rectSept.top += 10;
	m_rectSept.top += 4;
	m_rectSept.bottom = m_rectSept.top + 10;	
	
	//Cancel
	if( m_btnCancel.GetSafeHwnd())
	{
		rectCtrl.right = rectDlg.right - 13;
		rectCtrl.left = rectCtrl.right - SUB_DLG_FUNC_BTN_WIDTH;
		rectCtrl.top = rectDlg.Height() - 13 - SUB_DLG_FUNC_BTN_HEIGHT;
		rectCtrl.bottom = rectCtrl.top + SUB_DLG_FUNC_BTN_HEIGHT;
		m_btnCancel.MoveWindow(&rectCtrl);
	}
	
	//Ok
	if( m_btnChg.GetSafeHwnd())
	{
		rectCtrl.left -= ( 5 + SUB_DLG_FUNC_BTN_WIDTH );
		rectCtrl.right = rectCtrl.left + SUB_DLG_FUNC_BTN_WIDTH;
		m_btnChg.MoveWindow(&rectCtrl);
	}
}

void CChgPortDlg::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	CFont* pOldFont = NULL;
	pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontNormal);	
	pBufferDC->SetTextColor(COLOR_SUB_DLG_TEXT);	
	pBufferDC->DrawText( STR_CHG_PORT_TITLE, &m_rectTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	pBufferDC->SelectObject(pOldFont);

	
	int nTop = SUB_TITLE_IMAGE_HEIGHT;
	
	CPen* pOldPen = NULL;
	//Separator
	pOldPen = (CPen*)pBufferDC->SelectObject(&m_penSept);
	pBufferDC->MoveTo(m_rectSept.left, m_rectSept.top);
	pBufferDC->LineTo(m_rectSept.right, m_rectSept.top);
	pBufferDC->SelectObject(pOldPen);
	
	pOldPen = (CPen*)pBufferDC->SelectObject(&m_penCtrlRect); 
	DrawRect(pBufferDC, m_rectPortBack);
	pBufferDC->SelectObject(pOldPen);
}

void CChgPortDlg::Drawing2(CDC *pDC)
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	
	CDC memDC;
	memDC.CreateCompatibleDC(pDC); 
	DrawBg(pDC, &memDC);
	memDC.DeleteDC();
}

void CChgPortDlg::DetachObjects()
{
	if( m_DlgWhiteBackBrush.m_hObject )
		m_DlgWhiteBackBrush.DeleteObject();
	if( m_fontNormal.m_hObject )
		m_fontNormal.DeleteObject();
}

void CChgPortDlg::LoadObjects()
{
	m_DlgWhiteBackBrush.CreateSolidBrush( (COLORREF )CLR_WHITE);
//	m_btnChg.LoadBitmaps(IDB_BTN_MODIFY_N, IDB_BTN_MODIFY_P, IDB_BTN_MODIFY_O, IDB_BTN_MODIFY_N, 
//		SUB_DLG_FUNC_BTN_WIDTH, SUB_DLG_FUNC_BTN_HEIGHT);
//	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_P, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_N,
//		SUB_DLG_FUNC_BTN_WIDTH, SUB_DLG_FUNC_BTN_HEIGHT);
	m_fontNormal.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
}


HBRUSH CChgPortDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CCustomDlg::OnCtlColor(pDC, pWnd, nCtlColor);
	
//	switch( pWnd->GetDlgCtrlID() )
//	{
//	case xx:
//		{
//			pDC->SetBkMode(TRANSPARENT);
//			pDC->SetBkColor(CLR_WHITE);
//			pDC->SetTextColor(COLOR_SUB_DLG_TEXT);
//			return m_DlgWhiteBackBrush;
//		}
//		break;
//	}
	
	return hbr;
}

void CChgPortDlg::OnSize(UINT nType, int cx, int cy) 
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	rectDlg.right = rectDlg.left + 310;
	rectDlg.bottom = rectDlg.top + 190;
	MoveWindow(&rectDlg);	
}
