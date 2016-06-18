// Dlg_OpmlImport.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_OpmlImport.h"
#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_OpmlImport dialog
#define STR_GET_OPML _T("OPML 가져오기")

CDlg_OpmlImport::CDlg_OpmlImport(CWnd* pParent /*=NULL*/)
	: CCustomDlg(CDlg_OpmlImport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_OpmlImport)
	m_szFileLocation = _T("");
	//}}AFX_DATA_INIT	
}


void CDlg_OpmlImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_OpmlImport)
	DDX_Control(pDX, IDC_STATIC_COMMENT, m_staticComment);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BTN_OK, m_btnOk);
	DDX_Control(pDX, IDC_EDIT_OPMLLOCATION, m_editFileLocation);
	DDX_Control(pDX, IDC_BTN_FILEDLG, m_btnFileDlg);
	DDX_Text(pDX, IDC_EDIT_OPMLLOCATION, m_szFileLocation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_OpmlImport, CCustomDlg)
	//{{AFX_MSG_MAP(CDlg_OpmlImport)
	ON_BN_CLICKED(IDC_BTN_FILEDLG, OnBtnFiledlg)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnOK)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_OpmlImport message handlers

BOOL CDlg_OpmlImport::OnInitDialog() 
{
	CCustomDlg::OnInitDialog();

	m_hBrushBkgnd   =   CreateSolidBrush(CLR_BG);
	
	// TODO: Add extra initialization here
    SetWindowText(STR_GET_OPML);

	m_penOutline.CreatePen(PS_SOLID, 1, CLR_OUTLINE);
	m_fontDefault.CreateFont(15, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	CRect rect;
	GetWindowRect(rect);	
	MoveWindow(rect.left, rect.top, WIDTH, HEIGHT);
	
	m_staticComment.MoveWindow(MARGIN_LEFT, MARGIN_TOP, WIDTH - MARGIN_RIGHT - MARGIN_RIGHT, DOT_BTN_HEIGHT);
	m_staticComment.SetFont(&m_fontDefault);
	m_staticComment.GetWindowRect(rect);	
	ScreenToClient(rect);

	m_editFileLocation.MoveWindow(rect.left, rect.bottom + COMPONENT_GAP + EDIT_MARGIN, rect.Width() - COMPONENT_GAP - DOT_BTN_WIDTH, DOT_BTN_HEIGHT - EDIT_MARGIN);
	m_editFileLocation.SetFont(&m_fontDefault);
	m_editFileLocation.GetWindowRect(rect);
	m_editFileLocation.SetMargins(EDIT_MARGIN, EDIT_MARGIN);
	ScreenToClient(rect);

	m_btnFileDlg.MoveWindow(rect.right + COMPONENT_GAP, rect.top - EDIT_MARGIN, DOT_BTN_WIDTH, DOT_BTN_HEIGHT);
	m_btnFileDlg.LoadBitmaps(IDB_BTN_DOT_N, IDB_BTN_DOT_D, IDB_BTN_DOT_N, IDB_BTN_DOT_N, DOT_BTN_WIDTH, DOT_BTN_HEIGHT);
	m_btnFileDlg.SetShowText(FALSE);
	m_btnFileDlg.SetFlatFocus(TRUE);

	GetClientRect(rect);
	m_btnCancel.MoveWindow(rect.right - MARGIN_RIGHT - BTN_WIDTH, rect.bottom - MARGIN_BOTTOM - BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT);
	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnCancel.SetShowText(FALSE);
	m_btnCancel.SetFlatFocus(TRUE);
	m_btnCancel.GetWindowRect(rect);
	ScreenToClient(rect);

	m_btnOk.MoveWindow(rect.left - BTN_WIDTH - COMPONENT_GAP, rect.top, BTN_WIDTH, BTN_HEIGHT);
	m_btnOk.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnOk.SetShowText(FALSE);
	m_btnOk.SetFlatFocus(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlg_OpmlImport::OnOK() 
{	
	CCustomDlg::OnOK();	
}

void CDlg_OpmlImport::OnBtnFiledlg() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, STR_OPML_FILE_FORMAT, this);

    if(dlg.DoModal() == IDOK)
    {
        //CString fpath = dlg.GetPathName();
		
		//if(PathFileExists(fpath))
		//{
			m_szFileLocation = dlg.GetPathName();

			UpdateData(FALSE);
		//}
    }
}

void CDlg_OpmlImport::OnDestroy() 
{
	CCustomDlg::OnDestroy();
	
	if(m_fontDefault.m_hObject)
		m_fontDefault.DeleteObject();

	if(m_penOutline.m_hObject)
		m_penOutline.DeleteObject();

	DeleteObject(m_hBrushBkgnd);	
}

HBRUSH CDlg_OpmlImport::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CCustomDlg::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	HBRUSH* ret = NULL;

	if(nCtlColor == CTLCOLOR_DLG)
	{
		ret =   &m_hBrushBkgnd;
	}
	else if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(CLR_BG);

		pDC->SetTextColor(CLR_NORMAL_FONT);		

		ret =   &m_hBrushBkgnd;
	}

	if(ret != NULL)
		return *ret;
	else
		return hbr;

	return hbr;
}

void CDlg_OpmlImport::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CCustomDlg::OnPaint() for painting messages

	DrawItems(&dc);

	CRect rect;
	m_editFileLocation.GetWindowRect(rect);
	ScreenToClient(rect);

	rect.InflateRect(1, 1 + EDIT_MARGIN, 1, 1);

	CPen*	pOldPen = dc.SelectObject(&m_penOutline);
	dc.Rectangle(rect);
	dc.SelectObject(pOldPen);
}
