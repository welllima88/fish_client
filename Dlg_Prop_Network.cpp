// Dlg_Prop_Network.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_Prop_Network.h"
#include "./GnuModule/GnuDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Network dialog

CDlg_Prop_Network::CDlg_Prop_Network(CWnd* pParent /*=NULL*/)
	: CDlg_PropUnit(CDlg_Prop_Network::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Prop_Network)
	m_nMaxUpload = 0;
	m_nPort = 0;
	m_nMaxDownload = 0;
	//}}AFX_DATA_INIT	
}


void CDlg_Prop_Network::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Prop_Network)
	DDX_Text(pDX, IDC_EDIT_MAXUPLOAD, m_nMaxUpload);
	DDV_MinMaxUInt(pDX, m_nMaxUpload, 5, 30);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 100, 65535);
	DDX_Text(pDX, IDC_EDIT_MAXDOWNLOAD, m_nMaxDownload);
	DDV_MinMaxUInt(pDX, m_nMaxDownload, 5, 30);
	
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_EDIT_MAXUPLOAD, m_editUpload);
	DDX_Control(pDX, IDC_EDIT_MAXDOWNLOAD, m_editDownload);

	DDX_Control(pDX, IDC_STATIC_NETWORK, m_staticNetwork);
	DDX_Control(pDX, IDC_STATIC_PORT, m_staticPort);
	DDX_Control(pDX, IDC_STATIC_CONN_PORT, m_staticConPort);
	DDX_Control(pDX, IDC_STATIC_TRANSLIMIT, m_staticTransLimit);
	DDX_Control(pDX, IDC_STATIC_UPLOAD, m_staticUpload);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD, m_staticDownload);
	DDX_Control(pDX, IDC_STATIC_MAX, m_staticMax);
	DDX_Control(pDX, IDC_STATIC_MAX2, m_staticMax2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Prop_Network, CDialog)
	//{{AFX_MSG_MAP(CDlg_Prop_Network)
	ON_EN_CHANGE(IDC_EDIT_MAXDOWNLOAD, OnChangeEditMaxdownload)
	ON_EN_CHANGE(IDC_EDIT_MAXUPLOAD, OnChangeEditMaxupload)
	ON_EN_CHANGE(IDC_EDIT_PORT, OnChangeEditPort)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Network message handlers

/**************************************************************************
 * method CDlg_Prop_Network::Save
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * 설정이 변경되었다면 CGnuDoc 값을 변경하고
 * SaveProperties를 이용하여 레지스트리에 변경 내용을 기록한다.
 *
 * @Parameters
 **************************************************************************/
void CDlg_Prop_Network::Save()
{
	if(bModified)
	{
		UpdateData();

		theApp.m_spGD->SetGnuHostPort(m_nPort);
		theApp.m_spGD->m_MaxDownloads	= m_nMaxDownload;
		theApp.m_spGD->m_MaxUploads		= m_nMaxUpload;

		theApp.m_spGD->SaveProperties();
		bModified = FALSE;
	}
}

/**************************************************************************
 * method CDlg_Prop_Network::Init
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-17
 *
 * @Description
 * GnuDoc의 변수를 이용하여 설정창의 내용을 초기화한다.
 *
 * @Parameters
 **************************************************************************/
void CDlg_Prop_Network::Init()
{
	m_nMaxDownload		= theApp.m_spGD->m_MaxDownloads;
	if(m_nMaxDownload > 30) m_nMaxDownload = 30;
	else if(m_nMaxDownload < 5) m_nMaxDownload = 5;
	m_nMaxUpload		= theApp.m_spGD->m_MaxUploads;
	if(m_nMaxUpload > 30) m_nMaxUpload = 30;
	else if(m_nMaxUpload < 5) m_nMaxUpload = 5;
	m_nPort				= theApp.m_spGD->GetGnuHostPort();

	m_penOutline.CreatePen(PS_SOLID, 1, CLR_OUTLINE);
	m_penTitleUnder.CreatePen(PS_DOT, 1, CLR_NORMAL_FONT);

	CRect rect;

	m_staticNetwork.MoveWindow(0, 0, 200, COMPONENT_HEIGHT);
	m_staticNetwork.SetFont(&m_fontNormal);
	m_staticNetwork.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticPort.MoveWindow(rect.left, rect.bottom + 21, STATIC_MAX_WIDTH, COMPONENT_HEIGHT);
	m_staticPort.SetFont(&m_fontNormal);
	m_staticPort.GetWindowRect(rect);
	ScreenToClient(rect);
	
	m_staticConPort.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, 55, COMPONENT_HEIGHT);
	m_staticConPort.SetFont(&m_fontNormal);
	m_staticConPort.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticTransLimit.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, STATIC_MAX_WIDTH, COMPONENT_HEIGHT);
	m_staticTransLimit.SetFont(&m_fontNormal);
	m_staticTransLimit.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticUpload.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, STATIC_MAX_WIDTH, COMPONENT_HEIGHT);
	m_staticUpload.SetFont(&m_fontNormal);
	m_staticUpload.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticDownload.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, STATIC_MAX_WIDTH, COMPONENT_HEIGHT);
	m_staticDownload.SetFont(&m_fontNormal);
	m_staticDownload.GetWindowRect(rect);
	ScreenToClient(rect);

	m_editDownload.MoveWindow(rect.right + 10, rect.top - 2, EDIT_WIDTH, COMPONENT_HEIGHT);
	m_editDownload.SetFont(&m_fontNormal);
	m_editDownload.SetMargins(EDIT_MARGIN, EDIT_MARGIN);

	m_staticMax.MoveWindow(rect.right + 15 + EDIT_WIDTH, rect.top, 60, COMPONENT_HEIGHT);
	m_staticMax.SetFont(&m_fontNormal);
	

	m_staticUpload.GetWindowRect(rect);
	ScreenToClient(rect);
	m_editUpload.MoveWindow(rect.right + 10, rect.top - 2, EDIT_WIDTH, COMPONENT_HEIGHT);
	m_editUpload.SetFont(&m_fontNormal);
	m_editUpload.SetMargins(EDIT_MARGIN, EDIT_MARGIN);
	m_staticMax2.MoveWindow(rect.right + 15 + EDIT_WIDTH, rect.top, 60,COMPONENT_HEIGHT);
	m_staticMax2.SetFont(&m_fontNormal);	

	m_staticConPort.GetWindowRect(rect);
	ScreenToClient(rect);
	m_editPort.MoveWindow(rect.right + 10, rect.top - 2, EDIT_WIDTH, COMPONENT_HEIGHT);
	m_editPort.SetFont(&m_fontNormal);
	m_editPort.SetMargins(EDIT_MARGIN, EDIT_MARGIN);
	

	UpdateData(FALSE);
}

void CDlg_Prop_Network::OnChangeEditMaxdownload() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_Network::OnChangeEditMaxupload() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	bModified = TRUE;
	SendModified();
}

void CDlg_Prop_Network::OnChangeEditPort() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	bModified = TRUE;
	SendModified();
}

BOOL CDlg_Prop_Network::OnInitDialog() 
{
	m_hBrushBkgnd = CreateSolidBrush(CLR_BG);

	CDlg_PropUnit::OnInitDialog();
	
	Init();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlg_Prop_Network::PreTranslateMessage(MSG* pMsg) 
{
	return CDlg_PropUnit::PreTranslateMessage(pMsg);
}

HBRUSH	CDlg_Prop_Network::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

		if(pWnd == (CWnd*)&m_staticNetwork || pWnd == (CWnd*)&m_staticTransLimit || pWnd == (CWnd*)&m_staticPort)
			pDC->SetTextColor(CLR_BOLD_FONT);
		else
			pDC->SetTextColor(CLR_NORMAL_FONT);

        ret =   &m_hBrushBkgnd;
	}	

	if(ret != NULL)
		return *ret;
	else
		return hbr;
}

void CDlg_Prop_Network::OnPaint()
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
	
	
	pOldPen = dc.SelectObject(&m_penOutline);

	m_editDownload.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.InflateRect(1, 1, 1, 1);
	dc.Rectangle(rect);


	m_editUpload.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.InflateRect(1, 1, 1, 1);
	dc.Rectangle(rect);

	m_editPort.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.InflateRect(1, 1, 1, 1);
	dc.Rectangle(rect);
	

	dc.SelectObject(pOldPen);	
}

void CDlg_Prop_Network::OnDestroy()
{
	if(m_hBrushBkgnd)
		DeleteObject(m_hBrushBkgnd);

	if(m_penOutline.m_hObject)
		m_penOutline.DeleteObject();
	if(m_penTitleUnder.m_hObject)
		m_penTitleUnder.DeleteObject();	

	CDlg_PropUnit::OnDestroy();
}