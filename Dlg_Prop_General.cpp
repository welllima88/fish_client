// Dlg_Prop_General.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "Dlg_Prop_General.h"
#include "LocalProperties.h"
#include "DirDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_General dialog

CDlg_Prop_General::CDlg_Prop_General(CWnd* pParent /*=NULL*/)
	: CDlg_PropUnit(CDlg_Prop_General::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Prop_General)
	m_bAutoStart = FALSE;
	m_bUseInformWnd = FALSE;
	m_sUserPath = _T("");
	//}}AFX_DATA_INIT
}


void CDlg_Prop_General::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Prop_General)
	DDX_Check(pDX, IDC_CHK_AUTOSTART, m_bAutoStart);
	DDX_Check(pDX, IDC_CHK_USEINFORMWND, m_bUseInformWnd);
	DDX_Check(pDX, IDC_CHK_CLOSEEXIT, m_bCloseBtnExit);
	DDX_Check(pDX, IDC_CHK_SEARCHALWAYSSAVE, m_bSearchAlwaysSave);
	DDX_Text(pDX, IDC_EDIT_USERPATH, m_sUserPath);
	DDV_MaxChars(pDX, m_sUserPath, 512);
	DDX_Control(pDX, IDC_BUTTON_USERPATH, m_btnUserpath);

	DDX_Control(pDX, IDC_STATIC_NORMAL, m_staticNormalSetting);
	DDX_Control(pDX, IDC_STATIC_RUN, m_staticRunSetting);
	DDX_Control(pDX, IDC_STATIC_INFOWND, m_staticInfoWndSetting);
	DDX_Control(pDX, IDC_STATIC_SEARCH, m_staticSearchSetting);
	DDX_Control(pDX, IDC_STATIC_USERPATH, m_staticUserPathSetting);

	DDX_Control(pDX, IDC_CHK_AUTOSTART, m_chkAutoStart);
	DDX_Control(pDX, IDC_CHK_USEINFORMWND, m_chkUseInfoWnd);
	DDX_Control(pDX, IDC_CHK_CLOSEEXIT, m_chkCloseBtnExit);
	DDX_Control(pDX, IDC_CHK_SEARCHALWAYSSAVE, m_chkSearchAlwaysSave);

	DDX_Control(pDX, IDC_EDIT_USERPATH, m_editUserPath);

	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Prop_General, CDialog)
	//{{AFX_MSG_MAP(CDlg_Prop_General)
	ON_BN_CLICKED(IDC_CHK_AUTOLOGIN, OnChkAutologin)
	ON_BN_CLICKED(IDC_CHK_AUTOSTART, OnChkAutostart)
	ON_BN_CLICKED(IDC_CHK_SAVEPASS, OnChkSavepass)
	ON_BN_CLICKED(IDC_CHK_USEINFORMWND, OnChkUseinformwnd)
	ON_BN_CLICKED(IDC_CHK_CLOSEEXIT, OnChkCloseBtnExit)
	ON_EN_CHANGE(IDC_EDIT_USERPATH, OnChangeEditUserpath)
	ON_BN_CLICKED(IDC_BUTTON_USERPATH, OnButtonUserpath)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHK_SEARCHALWAYSSAVE, OnChkSearchalwayssave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_General message handlers

/**************************************************************************
 * method CDlg_Prop_General::Save
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * 설정이 변경되었다면 CLocalProperties의 값을 변경하고
 * SaveProperties를 이용하여 레지스트리에 변경 내용을 기록한다.
 *
 * @Parameters
 **************************************************************************/
void CDlg_Prop_General::Save()
{
//	if(!theApp.m_spLP) return;

	if(bModified)
	{
		UpdateData();

		theApp.m_spLP->SetAutoStart(m_bAutoStart);
		theApp.m_spLP->SetUseInformWnd(m_bUseInformWnd);
		theApp.m_spLP->SetUserPath(m_sUserPath);
		theApp.m_spLP->SetCloseBtnExit(m_bCloseBtnExit);
		theApp.m_spLP->SetSearchSave(m_bSearchAlwaysSave);

		theApp.m_spLP->SaveProperties();

		bModified = FALSE;
	}
}

/**************************************************************************
 * method CDlg_Prop_General::Init
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * LocalProperties의 변수를 이용하여 설정창의 내용을 초기화한다.
 *
 * @Parameters
 **************************************************************************/
void CDlg_Prop_General::Init()
{
//	if(!theApp.m_spLP) return;

	m_bAutoStart	= theApp.m_spLP->GetAutoStart();
	m_bUseInformWnd	= theApp.m_spLP->GetUseInformWnd();
	m_sUserPath		= theApp.m_spLP->GetUserPath();
	m_bCloseBtnExit = theApp.m_spLP->GetCloseBtnExit();
	m_bSearchAlwaysSave = theApp.m_spLP->GetForceSave();

	m_penOutline.CreatePen(PS_SOLID, 1, CLR_OUTLINE);
	m_penTitleUnder.CreatePen(PS_DOT, 1, CLR_NORMAL_FONT);

	CRect rect;
	GetClientRect(rect);

	m_staticNormalSetting.MoveWindow(0, 0, rect.Width(), COMPONENT_HEIGHT);
	m_staticNormalSetting.SetFont(&m_fontNormal);
	m_staticNormalSetting.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticRunSetting.MoveWindow(rect.left, rect.bottom + 21, rect.Width(), COMPONENT_HEIGHT);
	m_staticRunSetting.SetFont(&m_fontNormal);
	m_staticRunSetting.GetWindowRect(rect);
	ScreenToClient(rect);

	m_chkAutoStart.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, 180, COMPONENT_HEIGHT);
	m_chkAutoStart.SetFont(&m_fontNormal);
	m_chkAutoStart.GetWindowRect(rect);
	ScreenToClient(rect);

	m_chkCloseBtnExit.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, 190, COMPONENT_HEIGHT);
	m_chkCloseBtnExit.SetFont(&m_fontNormal);
	m_chkCloseBtnExit.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticInfoWndSetting.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, rect.Width(), COMPONENT_HEIGHT);
	m_staticInfoWndSetting.SetFont(&m_fontNormal);
	m_staticInfoWndSetting.GetWindowRect(rect);
	ScreenToClient(rect);

	m_chkUseInfoWnd.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, 110, COMPONENT_HEIGHT);
	m_chkUseInfoWnd.SetFont(&m_fontNormal);
	m_chkUseInfoWnd.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticSearchSetting.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, rect.Width(), COMPONENT_HEIGHT);
	m_staticSearchSetting.SetFont(&m_fontNormal);
	m_staticSearchSetting.GetWindowRect(rect);	
	ScreenToClient(rect);

	m_chkSearchAlwaysSave.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, 125, COMPONENT_HEIGHT);
	m_chkSearchAlwaysSave.SetFont(&m_fontNormal);
	m_chkSearchAlwaysSave.GetWindowRect(rect);	
	ScreenToClient(rect);

	m_staticUserPathSetting.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, rect.Width(), COMPONENT_HEIGHT);
	m_staticUserPathSetting.SetFont(&m_fontNormal);
	m_staticUserPathSetting.GetWindowRect(rect);	
	ScreenToClient(rect);

	m_editUserPath.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, EDIT_WIDTH, BTN_HEIGHT + 1);
	m_editUserPath.SetFont(&m_fontNormal);
	m_editUserPath.GetWindowRect(rect);	
	ScreenToClient(rect);	

	m_btnUserpath.MoveWindow(rect.right + 3, rect.top, BTN_WIDTH, BTN_HEIGHT);
	m_btnUserpath.LoadBitmaps(IDB_BTN_DOT_N, IDB_BTN_DOT_D, IDB_BTN_DOT_N, IDB_BTN_DOT_N, BTN_WIDTH, BTN_HEIGHT);
	m_btnUserpath.SetShowText(FALSE);
	m_btnUserpath.SetFlatFocus(TRUE);

	UpdateData(FALSE);
}

void CDlg_Prop_General::OnChkAutologin() 
{
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_General::OnChkAutostart() 
{
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_General::OnChkSavepass() 
{
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_General::OnChkUseinformwnd() 
{
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_General::OnChkCloseBtnExit() 
{
	bModified = TRUE;
	SendModified();
}

BOOL CDlg_Prop_General::OnInitDialog() 
{
	m_hBrushBkgnd   =   CreateSolidBrush(CLR_BG);

	CDlg_PropUnit::OnInitDialog();
	
	Init();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Prop_General::OnChangeEditUserpath() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	bModified = TRUE;
	SendModified();
	
}

void CDlg_Prop_General::OnButtonUserpath() 
{
	CDirDialog dlg;
	dlg.m_strSelDir = m_sUserPath;
	dlg.m_strTitle = STR_USER_PATH_SET;
	dlg.m_strWindowTitle = STR_USER_PATH_SET;
	if( dlg.DoBrowse(this) == IDOK )
	{		
		m_sUserPath = dlg.m_strPath;

		if(m_sUserPath.GetAt(m_sUserPath.GetLength() - 1) != _T('\\')) m_sUserPath += _T("\\");

		UpdateData(FALSE);
		bModified = TRUE;
		SendModified();
	}
}

BOOL CDlg_Prop_General::PreTranslateMessage(MSG* pMsg) 
{
	return CDlg_PropUnit::PreTranslateMessage(pMsg);
}

HBRUSH	CDlg_Prop_General::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    HBRUSH* ret = NULL;
	if(nCtlColor == CTLCOLOR_DLG)
	{
		ret =   &m_hBrushBkgnd;
	}
	else if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(CLR_BG);

		if(pWnd == (CWnd*)&m_staticNormalSetting || pWnd == (CWnd*)&m_staticRunSetting ||
			pWnd == (CWnd*)&m_staticInfoWndSetting || pWnd == (CWnd*)&m_staticSearchSetting ||
			pWnd == (CWnd*)&m_staticUserPathSetting)
		{	
			pDC->SetTextColor(CLR_BOLD_FONT);
		}
		else
		{						
			pDC->SetTextColor(CLR_NORMAL_FONT);				
		}

		ret =   &m_hBrushBkgnd;
	}

	if(ret != NULL)
		return *ret;
	else
		return hbr;
}

void CDlg_Prop_General::OnPaint()
{
	CPaintDC dc(this);

	//CDialog::OnPaint();

	CRect rect;

	GetClientRect(rect);	

	CPen* pOldPen = dc.SelectObject(&m_penTitleUnder);
	dc.SetBkColor(CLR_BG);
	dc.MoveTo(rect.left, DOTLINE_TOP);
	dc.LineTo(rect.right, DOTLINE_TOP);	
	dc.SelectObject(pOldPen);
}

void CDlg_Prop_General::OnChkSearchalwayssave() 
{
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_General::OnDestroy() 
{
	if(m_hBrushBkgnd)
		DeleteObject(m_hBrushBkgnd);

	if(m_penOutline.m_hObject)
		m_penOutline.DeleteObject();
	if(m_penTitleUnder.m_hObject)
		m_penTitleUnder.DeleteObject();	

	CDlg_PropUnit::OnDestroy();
}
