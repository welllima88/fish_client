// Dlg_Prop_Style.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_Prop_Style.h"
#include "fish_def.h"
#include "LocalProperties.h"
//#include "DirDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Style dialog


CDlg_Prop_Style::CDlg_Prop_Style(CWnd* pParent /*=NULL*/)
	: CDlg_PropUnit(CDlg_Prop_Style::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Prop_Style)
	m_bSummary = FALSE;
	m_szPaperStyle = _T("");	
	//}}AFX_DATA_INIT	
}


void CDlg_Prop_Style::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Prop_Style)
	DDX_Control(pDX, IDC_STATIC_SUMMARY, m_staticSummary);
	DDX_Control(pDX, IDC_STATIC_PAPERSTYLE, m_staticPaperStyle);
	DDX_Control(pDX, IDC_EDIT_PAPERSTYLE, m_editPaperStyle);
	DDX_Control(pDX, IDC_CHECK_SUMMARY, m_chkSummary);
	DDX_Control(pDX, IDC_BTN_FILEDLG, m_btnFileDlg);
	DDX_Control(pDX, IDC_STATIC_STYLE, m_staticStyle);
	DDX_Check(pDX, IDC_CHECK_SUMMARY, m_bSummary);
	DDX_Text(pDX, IDC_EDIT_PAPERSTYLE, m_szPaperStyle);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Prop_Style, CDialog)
	//{{AFX_MSG_MAP(CDlg_Prop_Style)
	ON_BN_CLICKED(IDC_BTN_FILEDLG, OnBtnFiledlg)
	ON_BN_CLICKED(IDC_CHECK_SUMMARY, OnCheckSummary)
	ON_EN_CHANGE(IDC_EDIT_PAPERSTYLE, OnChangeEditPaperstyle)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Prop_Style message handlers

BOOL CDlg_Prop_Style::OnInitDialog() 
{
	m_hBrushBkgnd   =   CreateSolidBrush(CLR_BG);

	CDlg_PropUnit::OnInitDialog();
	
	Init();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Prop_Style::Init()
{
	
	m_szPaperStyle	= theApp.m_spLP->GetPaperStyle();
	m_bSummary	= theApp.m_spLP->GetListDescSummary();
	

	m_penOutline.CreatePen(PS_SOLID, 1, CLR_OUTLINE);
	m_penTitleUnder.CreatePen(PS_DOT, 1, CLR_NORMAL_FONT);

	CRect rect;
	GetClientRect(rect);

	m_staticStyle.MoveWindow(0, 0, rect.Width(), COMPONENT_HEIGHT);
	m_staticStyle.SetFont(&m_fontNormal);
	m_staticStyle.GetWindowRect(rect);
	ScreenToClient(rect);

	m_staticSummary.MoveWindow(rect.left, rect.bottom + 21, rect.Width(), COMPONENT_HEIGHT);
	m_staticSummary.SetFont(&m_fontNormal);
	m_staticSummary.GetWindowRect(rect);
	ScreenToClient(rect);

	m_chkSummary.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, rect.Width(), COMPONENT_HEIGHT);
	m_chkSummary.SetFont(&m_fontNormal);
	m_chkSummary.GetWindowRect(rect);
	ScreenToClient(rect);

	
	m_staticPaperStyle.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, rect.Width(), COMPONENT_HEIGHT);
	m_staticPaperStyle.SetFont(&m_fontNormal);
	m_staticPaperStyle.GetWindowRect(rect);
	ScreenToClient(rect);
	
	m_editPaperStyle.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP_V, EDIT_WIDTH, BTN_HEIGHT + 1);
	m_editPaperStyle.SetFont(&m_fontNormal);
	m_editPaperStyle.GetWindowRect(rect);	
	ScreenToClient(rect);	

	m_btnFileDlg.MoveWindow(rect.right + 3, rect.top, BTN_WIDTH, BTN_HEIGHT);
	m_btnFileDlg.LoadBitmaps(IDB_BTN_DOT_N, IDB_BTN_DOT_D, IDB_BTN_DOT_N, IDB_BTN_DOT_N, BTN_WIDTH, BTN_HEIGHT);
	m_btnFileDlg.SetShowText(FALSE);
	m_btnFileDlg.SetFlatFocus(TRUE);

	UpdateData(FALSE);
}

void CDlg_Prop_Style::Save()
{
//	if(!theApp.m_spLP) return;

	if(bModified)
	{
		UpdateData();

		theApp.m_spLP->SetListDescSummary(m_bSummary);
		theApp.m_spLP->SetPaperStyle(m_szPaperStyle);

		theApp.m_spLP->SaveProperties();		

		bModified = FALSE;
	}
}

BOOL CDlg_Prop_Style::PreTranslateMessage(MSG* pMsg) 
{
	return CDlg_PropUnit::PreTranslateMessage(pMsg);
}

HBRUSH	CDlg_Prop_Style::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

		if(pWnd == (CWnd*)&m_staticStyle || pWnd == (CWnd*)&m_staticSummary ||
			pWnd == (CWnd*)&m_staticPaperStyle)
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

void CDlg_Prop_Style::OnPaint()
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

void CDlg_Prop_Style::OnBtnFiledlg() 
{
	// TODO: Add your control notification handler code here

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, STR_STYLE_FILE_FORMAT, this);

    if(dlg.DoModal() == IDOK)
    {
        m_szPaperStyle = dlg.GetPathName();
		UpdateData(FALSE);
		bModified = TRUE;
		SendModified();
    }		
}

void CDlg_Prop_Style::OnCheckSummary() 
{
	// TODO: Add your control notification handler code here

	bModified = TRUE;
	SendModified();	
}

void CDlg_Prop_Style::OnChangeEditPaperstyle() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	bModified = TRUE;
	SendModified();	
}

void CDlg_Prop_Style::OnDestroy()
{
	if(m_hBrushBkgnd)
		DeleteObject(m_hBrushBkgnd);
	
	if(m_penOutline.m_hObject)
		m_penOutline.DeleteObject();
	if(m_penTitleUnder.m_hObject)
		m_penTitleUnder.DeleteObject();	

	CDlg_PropUnit::OnDestroy();
}