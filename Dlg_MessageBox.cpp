// Dlg_MessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "Dlg_MessageBox.h"
#include "resource.h"
#include <math.h>
#include "fish_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define ID_BTN_OK			9999
#define ID_BTN1				10001
#define ID_BTN2				10002
#define ID_BTN3				10003
#define ID_BTN4				10004


/////////////////////////////////////////////////////////////////////////////
// CDlg_MessageBox dialog

CDlg_MessageBox::CDlg_MessageBox(LPCTSTR message, UINT uType, CWnd* pParent /*=NULL*/, CString title)
	: CCustomDlg(CDlg_MessageBox::IDD, pParent, title), m_strMessage(message),	m_nClickedButton(0),
	m_uType(uType)
{
	//{{AFX_DATA_INIT(CDlg_MessageBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nLineHeight = 2;
    m_hPrevFocusWnd =   GetFocus()->GetSafeHwnd();     // added by eternalbleu
    m_hBrushBkgnd   =   CreateSolidBrush(RGB(248, 248, 248));
}

CDlg_MessageBox::~CDlg_MessageBox()
{
    //////////////////////////////////////////////////////////////////////////
    // added by eternalbleu
    // 다이얼로그의 호출 이후에도 포커스를 이전 윈도우에 유지시킬 수 잇게 한다.

	::SetFocus(m_hPrevFocusWnd);
/*
    CWnd* pCurrentFocusWnd  =   GetFocus();
    CWnd* pNextFocusWnd =   CWnd::FromHandle( m_hPrevFocusWnd );
    if (pNextFocusWnd != NULL )
    {
        AttachThreadInput(
            GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(),NULL),
            GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(),NULL),
            TRUE);
    
        pNextFocusWnd->SetFocus();
    
        AttachThreadInput(
            GetWindowThreadProcessId(pCurrentFocusWnd->GetSafeHwnd(), NULL),
            GetWindowThreadProcessId(pNextFocusWnd->GetSafeHwnd(), NULL),
            FALSE);    
			
    }
	*/
	DeleteObject(m_hBrushBkgnd);	
}

void CDlg_MessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_MessageBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_MessageBox, CCustomDlg)
	//{{AFX_MSG_MAP(CDlg_MessageBox)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_PAINT()
	ON_WM_SIZE()
//	ON_BN_CLICKED(ID_BTN_OK, OnOK)
	ON_BN_CLICKED(ID_BTN1, OnBtn1)
	ON_BN_CLICKED(ID_BTN2, OnBtn2)
	ON_BN_CLICKED(ID_BTN3, OnBtn3)
	ON_BN_CLICKED(ID_BTN4, OnBtn4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_MessageBox message handlers

BOOL CDlg_MessageBox::OnInitDialog()
{
	CCustomDlg::OnInitDialog();
	
	CRect client_rect, rect, rect2;
	int pos_x, pos_y;
//	int n_messagebox_height;
	//CSize msg_size;

	GetWindowRect(client_rect);
	pos_x = client_rect.left;
	pos_y = client_rect.top;
	
	rect.top = 0;
	rect.left = 0;
	rect.right = BTN_WIDTH;
	rect.bottom = BTN_HEIGHT;

	//m_btnOk.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rect, this, ID_BTN_OK);
	//m_btnOk.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_N, IDB_BTN_OK_DISABLE, 69, 21);
	
	m_defaultFont.CreatePointFont(90, DEFAULT_APPLICATION_FONT);
	

	switch(m_uType & MB_ICONMASK)
	{
	case FMB_ICONEXCLAMATION:
	//case FMB_ICONWARNING:
		TRACE(_T("\nIcon - Exclamation!!!"));
		m_bmpMsgIcon.LoadBitmap(IDB_MB_EXCLAMATION);
		m_bmpMsgIcon.GetBitmap(&m_bitmapMsgIcon);
		break;

	case FMB_ICONQUESTION:
		TRACE(_T("\nIcon - Question!!!"));
		m_bmpMsgIcon.LoadBitmap(IDB_MB_QUESTION);
		m_bmpMsgIcon.GetBitmap(&m_bitmapMsgIcon);
		break;

	//case FMB_ICONSTOP:
	//case FMB_ICONERROR:
	case FMB_ICONHAND:
		TRACE(_T("\nIcon - Stop!!!"));
		m_bmpMsgIcon.LoadBitmap(IDB_MB_HAND);
		m_bmpMsgIcon.GetBitmap(&m_bitmapMsgIcon);
		break;

	//case FMB_ICONINFORMATION:
	case FMB_ICONASTERISK:
		TRACE(_T("\nIcon - Information!!!"));
		m_bmpMsgIcon.LoadBitmap(IDB_MB_ASTERISK);
		m_bmpMsgIcon.GetBitmap(&m_bitmapMsgIcon);
		break;

	default:
		TRACE(_T("\nIcon - NoIcon!!!"));
		break;
	}
	
	CalcMsgBox();

	switch(m_uType & MB_TYPEMASK)
	{
	case FMB_OK:
		m_btn1.Create(_T("OK"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		
		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.SetShowText(FALSE);
		
		TRACE(_T("\nType - FMB_OK"));
		break;
	
	case FMB_OKCANCEL:
		m_btn1.Create(_T("OK"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("CANCEL"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn1.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);

		//TRACE(_T("left : %d, top : %d"), client_rect.right - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15);

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn1.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);		

		TRACE(_T("\nType - FMB_OKCANCEL"));
		break;

	case FMB_ABORTRETRYIGNORE:
		m_btn1.Create(_T("Abort"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("Retry"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn3.Create(_T("Ignore"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN3);

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn2.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.MoveWindow(rect.left - (10 + BTN_WIDTH)*2, rect.top, BTN_WIDTH, BTN_HEIGHT);
		break;

	case FMB_YESNOCANCEL:
		m_btn1.Create(_T("Yes"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("No"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn3.Create(_T("Cancel"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN3);
		m_btn1.LoadBitmaps(IDB_BTN_YES_N, IDB_BTN_YES_D, IDB_BTN_YES_O, IDB_BTN_YES_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.LoadBitmaps(IDB_BTN_NO_N, IDB_BTN_NO_D, IDB_BTN_NO_O, IDB_BTN_NO_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);
		m_btn3.SetShowText(FALSE);		

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn2.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.MoveWindow(rect.left - (10 + BTN_WIDTH)*2, rect.top, BTN_WIDTH, BTN_HEIGHT);
		break;

	case FMB_YESNO:
		m_btn1.Create(_T("Yes"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("No"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn1.LoadBitmaps(IDB_BTN_YES_N, IDB_BTN_YES_D, IDB_BTN_YES_O, IDB_BTN_YES_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.LoadBitmaps(IDB_BTN_NO_N, IDB_BTN_NO_D, IDB_BTN_NO_O, IDB_BTN_NO_DISABLE, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn1.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);		
		break;

	case FMB_RETRYCANCEL:
		m_btn1.Create(_T("Retry"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("Cancel"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		
		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn2.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn1.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);		
		break;

	case FMB_YESYESALLNONOALL:
		m_btn1.Create(_T("Yes"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("Yes All"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn3.Create(_T("No"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN3);
		m_btn4.Create(_T("No All"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN4);
		m_btn1.LoadBitmaps(IDB_BTN_YES_N, IDB_BTN_YES_D, IDB_BTN_YES_O, IDB_BTN_YES_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.LoadBitmaps(IDB_BTN_YESALL_N, IDB_BTN_YESALL_D, IDB_BTN_YESALL_O, IDB_BTN_YESALL_DISABLE, BTN_WIDTH, BTN_HEIGHT);		
		m_btn3.LoadBitmaps(IDB_BTN_NO_N, IDB_BTN_NO_D, IDB_BTN_NO_O, IDB_BTN_NO_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn4.LoadBitmaps(IDB_BTN_NOALL_N, IDB_BTN_NOALL_D, IDB_BTN_NOALL_O, IDB_BTN_NOALL_DISABLE, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);
		m_btn3.SetShowText(FALSE);
		m_btn4.SetShowText(FALSE);

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn4.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn4.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn4.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn3.MoveWindow(rect.left - (10 + BTN_WIDTH), rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.MoveWindow(rect.left - (10 + BTN_WIDTH)*2, rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.MoveWindow(rect.left - (10 + BTN_WIDTH)*3, rect.top, BTN_WIDTH, BTN_HEIGHT);
		break;

	case FMB_YESYESALLNO:
		m_btn1.Create(_T("Yes"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("Yes All"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn3.Create(_T("No"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN3);
		
		m_btn1.LoadBitmaps(IDB_BTN_YES_N, IDB_BTN_YES_D, IDB_BTN_YES_O, IDB_BTN_YES_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn2.LoadBitmaps(IDB_BTN_YESALL_N, IDB_BTN_YESALL_D, IDB_BTN_YESALL_O, IDB_BTN_YESALL_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.LoadBitmaps(IDB_BTN_NO_N, IDB_BTN_NO_D, IDB_BTN_NO_O, IDB_BTN_NO_DISABLE, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);
		m_btn3.SetShowText(FALSE);		

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn2.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.MoveWindow(rect.left - (10 + BTN_WIDTH)*2, rect.top, BTN_WIDTH, BTN_HEIGHT);
		break;

	case FMB_YESNONOALL:
		m_btn1.Create(_T("Yes"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);
		m_btn2.Create(_T("No"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN2);
		m_btn3.Create(_T("No All"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN3);
		m_btn1.LoadBitmaps(IDB_BTN_YES_N, IDB_BTN_YES_D, IDB_BTN_YES_O, IDB_BTN_YES_DISABLE, BTN_WIDTH, BTN_HEIGHT);		
		m_btn2.LoadBitmaps(IDB_BTN_NO_N, IDB_BTN_NO_D, IDB_BTN_NO_O, IDB_BTN_NO_DISABLE, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.LoadBitmaps(IDB_BTN_NOALL_N, IDB_BTN_NOALL_D, IDB_BTN_NOALL_O, IDB_BTN_NOALL_DISABLE, BTN_WIDTH, BTN_HEIGHT);		

		m_btn1.SetShowText(FALSE);
		m_btn2.SetShowText(FALSE);
		m_btn3.SetShowText(FALSE);		

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn3.MoveWindow(client_rect.Width() - MARGIN_LEFT - BTN_WIDTH, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);
		m_btn3.GetWindowRect(rect);
		ScreenToClient(rect);
		m_btn2.MoveWindow(rect.left - 10 - BTN_WIDTH, rect.top, BTN_WIDTH, BTN_HEIGHT);
		m_btn1.MoveWindow(rect.left - (10 + BTN_WIDTH)*2, rect.top, BTN_WIDTH, BTN_HEIGHT);
		break;

	default:
		m_btn1.Create(_T("OK"), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rect, this, ID_BTN1);

		if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
			m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
		else
			m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, m_rectMessageBox.bottom + 15, BTN_WIDTH, BTN_HEIGHT);

		m_btn1.SetShowText(FALSE);
		
		m_btn1.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);		
		break;
	}

	m_btn1.GetWindowRect(rect);	
	ScreenToClient(rect);

	if(m_btn1.m_hWnd) m_btn1.SetFlatFocus(TRUE);
	if(m_btn2.m_hWnd) m_btn2.SetFlatFocus(TRUE);
	if(m_btn3.m_hWnd) m_btn3.SetFlatFocus(TRUE);
	if(m_btn4.m_hWnd) m_btn4.SetFlatFocus(TRUE);

	MoveWindow(pos_x, pos_y, MAX_WIDTH, rect.bottom + MARGIN_BOTTOM);


	return TRUE;
}


HBRUSH CDlg_MessageBox::OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_STATIC)
	{
	//	pDC->SetBkColor(RGB(248, 248, 248));
	//	pDC->SetTextColor(RGB(0, 0, 0));
	}
	return m_hBrushBkgnd;	
}


int CDlg_MessageBox::DoModal()
{
	CCustomDlg::DoModal();
	return m_nClickedButton;
}



void CDlg_MessageBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
		
	DrawItems(&dc);
	
	if(m_bmpMsgIcon.m_hObject)
	{
		CBitmap* pOldBmp = (CBitmap*)MemDC.SelectObject(&m_bmpMsgIcon);
		dc.BitBlt(MARGIN_LEFT, MARGIN_TOP, m_bitmapMsgIcon.bmWidth, m_bitmapMsgIcon.bmHeight, &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(pOldBmp);
	}
		
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_defaultFont);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(CUSTOM_DLG_BG_COLOR);	
	
	CRect rect = m_rectMessageBox;
	CSize chSize;
	int linelength = 0;

	for(int i = 0; i < m_strMessage.GetLength(); i++)
	{
		if(m_nLineCnt == 1)
		{
			dc.DrawText(m_strMessage, rect, DT_CENTER);
			break;
		}
		else
		{			
			chSize = dc.GetOutputTextExtent(m_strMessage.GetAt(i));
			linelength += chSize.cx;
			if(linelength > m_rectMessageBox.Width() || ((i+1 < m_strMessage.GetLength()) && m_strMessage.GetAt(i) == _T('\r') && m_strMessage.GetAt(i+1) == _T('\n')))
			{
				rect.top += chSize.cy + m_nLineHeight;
				rect.left = m_rectMessageBox.left;

				if(!(m_strMessage.GetAt(i) == _T('\r') && ((i+1 < m_strMessage.GetLength()) && m_strMessage.GetAt(i+1) == _T('\n'))))
				{
					if(m_strMessage.GetAt(i) != _T(' '))
						dc.DrawText(m_strMessage.GetAt(i), rect, DT_LEFT);
					else
						chSize.cx = 0;
				}
				else
				{
					if((i+2 < m_strMessage.GetLength()) && m_strMessage.GetAt(i+2) != _T(' '))
					{
						dc.DrawText(m_strMessage.GetAt(i+2), rect, DT_LEFT);
						chSize = dc.GetOutputTextExtent(m_strMessage.GetAt(i + 2));
					}
					else
						chSize.cx = 0;
					i += 2;					
				}

				linelength = chSize.cx;
			}
			else
			{				
				dc.DrawText(m_strMessage.GetAt(i), rect, DT_LEFT);				
			}
			rect.left += chSize.cx;
		}
	}
	
	dc.SelectObject(pOldFont);
	MemDC.DeleteDC();
	
}

void CDlg_MessageBox::OnBtn1()
{
	switch(m_uType & MB_TYPEMASK)
	{
	case FMB_OK:		
	case FMB_OKCANCEL:
		m_nClickedButton = IDOK;	
		break;

	case FMB_ABORTRETRYIGNORE:
		m_nClickedButton = IDABORT;
		break;

	case FMB_RETRYCANCEL:
		m_nClickedButton = IDRETRY;		
		break;

	case FMB_YESNOCANCEL:
	case FMB_YESNO:
	case FMB_YESYESALLNONOALL:
	case FMB_YESYESALLNO:
	case FMB_YESNONOALL:
		m_nClickedButton = IDYES;
		break;
		
	default:
		m_nClickedButton = IDOK;		
		break;
	}
	CCustomDlg::OnOK();
}

void CDlg_MessageBox::OnBtn2()
{
	switch(m_uType & MB_TYPEMASK)
	{
	case FMB_OKCANCEL:
	case FMB_RETRYCANCEL:
		m_nClickedButton = IDCANCEL;
		break;

	case FMB_ABORTRETRYIGNORE:
		m_nClickedButton = IDRETRY;
		break;

	case FMB_YESNOCANCEL:
	case FMB_YESNO:
	case FMB_YESNONOALL:
		m_nClickedButton = IDNO;
		break;

	case FMB_YESYESALLNONOALL:
	case FMB_YESYESALLNO:
		m_nClickedButton = IDYESALL;
		break;
	
	default:
		m_nClickedButton = IDOK;		
		break;
	}
	CCustomDlg::OnOK();
}

void CDlg_MessageBox::OnBtn3()
{
	switch(m_uType & MB_TYPEMASK)
	{
	case FMB_ABORTRETRYIGNORE:
		m_nClickedButton = IDIGNORE;
		break;

	case FMB_YESNOCANCEL:	
		m_nClickedButton = IDCANCEL;
		break;

	case FMB_YESYESALLNONOALL:
	case FMB_YESYESALLNO:
		m_nClickedButton = IDNO;
		break;

	case FMB_YESNONOALL:
		m_nClickedButton = IDNOALL;
		break;
	
	default:
		m_nClickedButton = IDOK;		
		break;
	}
	CCustomDlg::OnOK();
}

void CDlg_MessageBox::OnBtn4()
{
	m_nClickedButton = IDNOALL;
	CCustomDlg::OnOK();
}

void CDlg_MessageBox::OnSize(UINT nType, int cx, int cy)
{
	CCustomDlg::OnSize(nType, cx, cy);

	CRect client_rect, rect;
	CWnd* aBtn[] = {&m_btn4, &m_btn3, &m_btn2, &m_btn1, NULL};
	int btnCnt = 0;

	CalcMsgBox();

	GetWindowRect(client_rect);

	if((m_uType & MB_TYPEMASK) == FMB_OK)
	{
		if(m_btn1.m_hWnd)
		{
			if(m_bmpMsgIcon.m_hObject && (MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom))			
				m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
			else
				m_btn1.MoveWindow((client_rect.Width() - BTN_WIDTH)/2, m_rectMessageBox.bottom + MARGIN_BOTTOM, BTN_WIDTH, BTN_HEIGHT);
			m_btn1.GetWindowRect(rect);
			ScreenToClient(rect);
			MoveWindow(client_rect.left, client_rect.top, client_rect.Width(), rect.bottom + MARGIN_BOTTOM);
		}
	}
	else
	{
		for(int i=0; aBtn[i] != NULL; i++)
		{
			if(aBtn[i]->m_hWnd)
			{
				if(MARGIN_TOP + m_bitmapMsgIcon.bmHeight > m_rectMessageBox.bottom)
					aBtn[i]->MoveWindow(client_rect.Width() - MARGIN_RIGHT - (10 * btnCnt + (BTN_WIDTH) * (btnCnt +1 )), MARGIN_TOP + m_bitmapMsgIcon.bmHeight + 15, BTN_WIDTH, BTN_HEIGHT);
				else
					aBtn[i]->MoveWindow(client_rect.Width() - MARGIN_RIGHT - (10 * btnCnt + (BTN_WIDTH) * (btnCnt +1 )), m_rectMessageBox.bottom + MARGIN_BOTTOM, BTN_WIDTH, BTN_HEIGHT);
				btnCnt++;
			}
		}

		if(m_btn1.m_hWnd)
		{
			m_btn1.GetWindowRect(rect);
			ScreenToClient(rect);
			MoveWindow(client_rect.left, client_rect.top, client_rect.Width(), rect.bottom + MARGIN_BOTTOM);
		}

		if(m_btn4.m_hWnd)
		{
			m_btn1.GetWindowRect(rect);
			ScreenToClient(rect);
			if(rect.left < MARGIN_LEFT)
			{
				MoveWindow(client_rect.left, client_rect.top, client_rect.Width() + (MARGIN_LEFT - rect.left), client_rect.Height());
			}
		}
	}	
}


void CDlg_MessageBox::CalcMsgBox()
{	
	CClientDC dc(this);
	
	CRect rect;

	CFont* pOldFont = (CFont*)dc.SelectObject(&m_defaultFont);

	m_sizeMsg = dc.GetOutputTextExtent(m_strMessage);	

	GetClientRect(m_rectMessageBox);

	if(!m_bmpMsgIcon.m_hObject)
		m_rectMessageBox.left = MARGIN_LEFT;
	else
		m_rectMessageBox.left = MARGIN_LEFT + m_bitmapMsgIcon.bmWidth + 10;

	m_rectMessageBox.right -= MARGIN_RIGHT;
	m_rectMessageBox.top = MARGIN_TOP;	
	m_rectMessageBox.bottom = MARGIN_TOP + m_sizeMsg.cy;

	rect = m_rectMessageBox;

	
	dc.DrawText(m_strMessage, rect, DT_LEFT | DT_CALCRECT | DT_WORDBREAK);
	m_nLineCnt = ceil((double) rect.Height() / (double) m_sizeMsg.cy);
	rect.bottom += (m_nLineCnt - 1) * m_nLineHeight;

	if(m_bmpMsgIcon.m_hObject && rect.bottom < MARGIN_TOP + m_bitmapMsgIcon.bmHeight)
	{
		int move_size = (MARGIN_TOP + m_bitmapMsgIcon.bmHeight - rect.bottom) / 2;
		rect.top += move_size;
		rect.bottom += move_size;
	}

	m_rectMessageBox.CopyRect(rect);

	dc.SelectObject(pOldFont);
}

void CDlg_MessageBox::OnOK()
{
	if(m_btn1.IsSelected()) OnBtn1();
	else if(m_btn2.IsSelected()) OnBtn2();
	else if(m_btn3.IsSelected()) OnBtn3();
	else if(m_btn4.IsSelected()) OnBtn4();
}

void CDlg_MessageBox::DeleteObjects()
{
	if(m_bmpMsgIcon.m_hObject)
		m_bmpMsgIcon.DeleteObject();

	if(m_defaultFont.m_hObject)
		m_defaultFont.DeleteObject();
}

UINT FishMessageBox(CString message, UINT u_type)
{
	//CDlg_MessageBox messageBox(message, u_type);
	CDlg_MessageBox messageBox(message, u_type);
	return messageBox.DoModal();
}

UINT FishMessageBox(CString message, CString title, UINT u_type)
{
	//CDlg_MessageBox messageBox(message, u_type);
	CDlg_MessageBox messageBox(message, u_type, NULL, title);
	return messageBox.DoModal();
}
