// Dlg_Style.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "Dlg_Style.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_BTN_CANCEL			8887
#define ID_BTN_OK				8888

/////////////////////////////////////////////////////////////////////////////
// CDlg_Style dialog


CDlg_Style::CDlg_Style(CWnd* pParent /*=NULL*/, CString title)
	: CCustomDlg(CDlg_Style::IDD, pParent, title)
{
	//{{AFX_DATA_INIT(CDlg_Style)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hPrevFocusWnd =   GetFocus()->GetSafeHwnd();     // added by eternalbleu
}

CDlg_Style::~CDlg_Style()
{
	::SetFocus(m_hPrevFocusWnd);

	DeleteObjects();
}


void CDlg_Style::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Style)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_LIST_STYLE, m_listStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Style, CCustomDlg)
	//{{AFX_MSG_MAP(CDlg_Style)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(ID_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(ID_BTN_CANCEL, OnBtnCancel)		
	ON_LBN_DBLCLK(IDC_LIST_STYLE, OnDblclkListStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Style message handlers


BOOL CDlg_Style::OnInitDialog()
{
	CCustomDlg::OnInitDialog();

	CRect rectBtn(0, 0, BTN_WIDTH, BTN_HEIGHT);
	CRect rect;

	m_fontDefault.CreatePointFont(100, _T("Gulim"));

	// Button ÃÊ±âÈ­.....
	m_btnOk.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_FLAT| WS_TABSTOP, rectBtn, this, ID_BTN_OK);
	m_btnOk.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);

	m_btnCancel.Create(_T(""), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_FLAT | WS_TABSTOP, rectBtn, this, ID_BTN_CANCEL);
	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);

	GetClientRect(rect);

	m_listStyle.MoveWindow(MARGIN_LEFT, MARGIN_TOP, rect.Width() - MARGIN_LEFT - MARGIN_RIGHT, 100);
	m_listStyle.SetItemGap(1, 0);
	m_listStyle.SetColors(RGB(147, 179, 216), RGB(255, 255, 255), RGB(255, 255, 255), RGB(65, 65, 65), RGB(255, 255, 255), RGB(225, 225, 225));	
	m_listStyle.SetItemHeight(0, 20);
	m_listStyle.SetMargins(4, 4);
	m_listStyle.SetListFill(TRUE);
	m_listStyle.GetWindowRect(rectBtn);
	m_listStyle.SetFont(&m_fontDefault);
	ScreenToClient(rectBtn);

	m_btnOk.MoveWindow(rect.Width()/2 - COMPONENT_GAP/2 - BTN_WIDTH, rectBtn.bottom + COMPONENT_GAP, BTN_WIDTH, BTN_HEIGHT);
	m_btnCancel.MoveWindow(rect.Width()/2 + COMPONENT_GAP/2, rectBtn.bottom + COMPONENT_GAP, BTN_WIDTH, BTN_HEIGHT);
	m_btnCancel.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);

	m_penOutline.CreatePen(PS_SOLID, 1, OUTLINE_COLOR);

	GetWindowRect(rect);
	MoveWindow(rect.left, rect.top, rect.Width(), rectBtn.bottom + MARGIN_BOTTOM);
	
	SearchStyle();

	if(m_btnOk.m_hWnd) m_btnOk.SetFlatFocus(TRUE);
	if(m_btnCancel.m_hWnd) m_btnCancel.SetFlatFocus(TRUE);
	//m_btnOk.SelectButton();

	return TRUE;
}

void CDlg_Style::DrawBg(CDC* pBufferDC, CDC* pMemDC)
{
	CCustomDlg::DrawBg(pBufferDC, pMemDC);

	
	CRect rect;
	m_listStyle.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.InflateRect(1, 1, 1, 1);
	
	CPen* oldPen = pBufferDC->SelectObject(&m_penOutline);
	pBufferDC->Rectangle(&rect);
	pBufferDC->SelectObject(oldPen);
	
}

void CDlg_Style::DrawItems(CDC *pDC)
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	if(!pDC) return;
	
	CDC memDC, bufferDC;
	CBitmap bmpBuffer;
	CBitmap* pOldBmp;
	bmpBuffer.CreateCompatibleBitmap(pDC, rectDlg.Width(), rectDlg.Height());
	
	memDC.CreateCompatibleDC(pDC);
	bufferDC.CreateCompatibleDC(pDC);
	bufferDC.SetBkMode(TRANSPARENT);
	pOldBmp = (CBitmap*) bufferDC.SelectObject(&bmpBuffer);
	DrawBg(&bufferDC, &memDC);
	DrawOutline(&bufferDC, &memDC);
	
	pDC->BitBlt(0, 0, rectDlg.Width(), rectDlg.Height(), &bufferDC, 0, 0, SRCCOPY);
	
	bufferDC.SelectObject(pOldBmp);
	
	bmpBuffer.DeleteObject();
	bufferDC.DeleteDC();
	memDC.DeleteDC();
}

BOOL CDlg_Style::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bDrawBySelf)
	{
		DrawItems(pDC);
		return TRUE;
	}

	return CCustomDlg::OnEraseBkgnd(pDC);
}

void CDlg_Style::SearchStyle()
{
	CFileFind finder;
	BOOL	bWorking;		
	CString styleFilePath;
//	TCHAR szFile[_MAX_PATH];

//	GetModuleFileName(NULL, szFile, _MAX_PATH);
#ifdef _UNICODE
	m_strStylePath = __wargv[0];
#else
	m_strStylePath = __argv[0];
#endif
	int pos = m_strStylePath.ReverseFind(_T('\\'));
//	TRACE(_T("\n%s"), szFile);

	if(pos != 0)
	{
		m_strStylePath = m_strStylePath.Left(pos + 1);		
		
//		TRACE(_T("\n%s"), m_strStylePath);
		
		m_strStylePath = m_strStylePath + DEFAULT_STYLEPATH;
//		TRACE(_T("\n%s"), m_strStylePath);
		
		//bWorking = finder.FindFile(styleFilePath);
		bWorking = finder.FindFile(m_strStylePath + DEFAULT_STYLEFORMAT);
//		TRACE(_T("\n%s"), m_strStylePath);

		m_listStyle.AddString(_T("Default"));

		while(bWorking)
		{			
			bWorking = finder.FindNextFile();

			if(finder.IsDots() || finder.IsDirectory())
				continue;
			else
			{
				m_listStyle.AddString(finder.GetFileTitle());
			}
		}
	}
}

CString CDlg_Style::GetSelectStyle()
{
	return m_strSelectionStyle;
}

void CDlg_Style::OnBtnOk()
{
	int sel = m_listStyle.GetCurSel();

	if(sel > 0)
	{
		m_listStyle.GetText(sel, m_strSelectionStyle);
		m_strSelectionStyle = m_strStylePath + m_strSelectionStyle + _T(".xslt");
	}
	else
		m_strSelectionStyle = _T("");

	m_strSelectionStyle.Replace(_T("\\"), _T("/"));

	CCustomDlg::OnOK();
}

void CDlg_Style::OnBtnCancel()
{
	CCustomDlg::OnCancel();
}

void CDlg_Style::DeleteObjects()
{
	if(m_fontDefault.m_hObject)
		m_fontDefault.DeleteObject();

	CCustomDlg::DeleteObjects();
}

void CDlg_Style::OnDblclkListStyle() 
{
	OnBtnOk();
	
}

void CDlg_Style::OnOK()
{	
	if(m_btnOk.IsSelected())
		OnBtnOk();
	else if(m_btnCancel.IsSelected())
		OnBtnCancel();

	if(m_listStyle.GetCurSel() != -1)
		OnBtnOk();
}
