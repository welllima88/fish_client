// CustomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CustomDlg.h"
#include "fish_def.h"
#include "resource.h"
#include "GlobalIconManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomDlg dialog

#define SPACE_CTRL_RECT			1

CCustomDlg::CCustomDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/, CString title /*= _T("")*/)
: CDialog(nIDTemplate, pParent), m_szCaption(title)
{
	//{{AFX_DATA_INIT(CCustomDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDrawBySelf = TRUE;
	m_nIDResource = -1;
}


void CCustomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomDlg)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_BN_CLICKED(CUSTOM_DLG_SYS_CLOSE_ID, OnCancel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomDlg message handlers

BOOL CCustomDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_szCaption);
	
	CRect rectDlg;
	GetClientRect(&rectDlg);

	CRect rectBtn;
	rectBtn = rectDlg;
	rectBtn.top = CLOSE_BTN_TOP_MARGIN;
	rectBtn.bottom = rectBtn.top + CLOSE_BTN_HEIGHT;
	rectBtn.right -= CLOSE_BTN_RIGHT_MARGIN;
	rectBtn.left = rectBtn.right - CLOSE_BTN_WIDTH;
	
	m_btnClose.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rectBtn,
				this, CUSTOM_DLG_SYS_CLOSE_ID);

	
	/*
	CClientDC dc(this);
    CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
    CImageList* sysIMGLIST  =   BMPManager->getSysmenuBtn();

	CBitmap btnCls_BMP;  
	*/

    m_btnClose.ShowWindow(SW_NORMAL);
	m_btnClose.SetShowText(FALSE);
	m_btnClose.LoadBitmaps(IDB_BTN_CLOSE_N, IDB_BTN_CLOSE_D, IDB_BTN_CLOSE_D, IDB_BTN_CLOSE_N, CLOSE_BTN_WIDTH, CLOSE_BTN_HEIGHT);

	InitObjects();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomDlg::InitObjects()
{
	/*
	m_bmpTitle.LoadBitmap(IDB_SUBDLG_TITLE);
	m_bmpTitle.GetBitmap(&m_bitmapTitle);
	m_bmpLogo.LoadBitmap(IDB_SUBDLG_LOGO);
	m_bmpLogo.GetBitmap(&m_bitmapLogo);
	*/

	m_bmpTitleLeft.LoadBitmap(IDB_SUBDLG_TITLE_LEFT);
	m_bmpTitleLeft.GetBitmap(&m_bitmapTitleLeft);
	m_bmpTitleMid.LoadBitmap(IDB_SUBDLG_TITLE_MID);
	m_bmpTitleMid.GetBitmap(&m_bitmapTitleMid);
	m_bmpTitleRight.LoadBitmap(IDB_SUBDLG_TITLE_RIGHT);
	m_bmpTitleRight.GetBitmap(&m_bitmapTitleRight);

	m_bmpOutlineR.LoadBitmap(IDB_SUBDLG_RIGHT);
	m_bmpOutlineL.LoadBitmap(IDB_SUBDLG_LEFT);
	m_bmpOutlineR.GetBitmap(&m_bitmapOutLine);
	m_bmpBottom.LoadBitmap(IDB_SUBDLG_BTM);
	m_bmpBottom.GetBitmap(&m_bitmapBottom);

//	m_bmpSubTitleBg.LoadBitmap(IDB_SUBDLG_BG);
//	m_bmpSubTitleBg.GetBitmap(&m_bitmapSubTitleBg);	
//	m_penCtrlRect.CreatePen(PS_SOLID, 1, COLOR_SUB_DLG_CTRL_RECT);
//	m_bmpSubTitleR.LoadBitmap(IDB_SUBDLG_BG_2);
//	m_bmpSubTitleR.GetBitmap(&m_bitmapSubTitleBgR);
	m_fontBold.CreateFont(FONT_HEIGHT, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	
	GetClientRect(m_rectWindowText);
	m_rectWindowText.left = m_bitmapTitle.bmWidth + CAPTION_LEFT_MARGIN;
	m_rectWindowText.right -= (CAPTION_RIGHT_MARGIN + CLOSE_BTN_WIDTH);	
	m_rectWindowText.top = (m_bitmapTitle.bmHeight - FONT_HEIGHT)/2 + 1;
	m_rectWindowText.bottom = m_bitmapTitle.bmHeight - (m_bitmapTitle.bmHeight - FONT_HEIGHT)/2 + 1;

	if(m_nIDResource != -1)
	{
		m_bmpSubTitle.LoadBitmap(MAKEINTRESOURCE(m_nIDResource));
		m_bmpSubTitle.GetBitmap(&m_bitmapSubTitle);
	}
	
//	m_btnClose.LoadBitmaps(IDB_LDBTN_CLOSE_N, IDB_LDBTN_CLOSE_P, IDB_LDBTN_CLOSE_F, IDB_LDBTN_CLOSE_N,
//			16, 16);
	m_rectWindowText.left = m_bitmapTitleLeft.bmWidth + CAPTION_LEFT_MARGIN;
	m_rectWindowText.right -= (CAPTION_RIGHT_MARGIN + CLOSE_BTN_WIDTH);
	m_rectWindowText.top = (m_bitmapTitleLeft.bmHeight - FONT_HEIGHT)/2 + 1;
	m_rectWindowText.bottom = m_bitmapTitleLeft.bmHeight - (m_bitmapTitleLeft.bmHeight - FONT_HEIGHT)/2 + 1;

	m_penSept.CreatePen(PS_DOT, 1, COLOR_CUSTOM_DLG_SEPT);
}

void CCustomDlg::DeleteObjects()
{
	if( m_bmpTitle.m_hObject)
		m_bmpTitle.DeleteObject();

	if( m_bmpTitleLeft.m_hObject)
		m_bmpTitleLeft.DeleteObject();
	if( m_bmpTitleMid.m_hObject)
		m_bmpTitleMid.DeleteObject();
	if( m_bmpTitleRight.m_hObject)
		m_bmpTitleRight.DeleteObject();

	if( m_bmpLogo.m_hObject )
		m_bmpLogo.DeleteObject();
	if( m_bmpOutlineR.m_hObject )
		m_bmpOutlineR.DeleteObject();
	if( m_bmpBottom.m_hObject )
		m_bmpBottom.DeleteObject();
	if( m_bmpSubTitle.m_hObject )
		m_bmpSubTitle.DeleteObject();

	if( m_bmpSubTitleBg.m_hObject )
		m_bmpSubTitleBg.DeleteObject();
	if( m_penCtrlRect.m_hObject )
		m_penCtrlRect.DeleteObject();
	if( m_bmpSubTitleR.m_hObject )
		m_bmpSubTitleR.DeleteObject();
	if( m_bmpSubTitle.m_hObject )
		m_bmpSubTitle.DeleteObject();
	if( m_fontBold.m_hObject )
		m_fontBold.DeleteObject();
	if( m_penSept.m_hObject )
		m_penSept.DeleteObject();	
}

 
afx_msg BOOL CCustomDlg::OnEraseBkgnd(CDC* pDC)
{

	if(m_bDrawBySelf)
	{
		DrawItems2(pDC);
		return TRUE;
	}

	return CDialog::OnEraseBkgnd(pDC); 
}
 

void CCustomDlg::SetDrawBySelf(BOOL bDraw)
{
	m_bDrawBySelf = bDraw;
}

void CCustomDlg::DrawItems2(CDC *pDC)
{
	CRect rectDlg;	
	GetClientRect(&rectDlg);
	if(!pDC) return;

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	DrawBg(pDC, &memDC);
	DrawOutline(pDC, &memDC);
	
	memDC.DeleteDC();
}

void CCustomDlg::DrawItems(CDC *pDC)
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

void CCustomDlg::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	pBufferDC->FillSolidRect(0, 0, rectDlg.Width(), rectDlg.Height(), RGB(248, 248, 248));

 
 
	//Caption	
	CBitmap* pOldBmp;

	// Title Left
	if(rectDlg.Width() < (m_bitmapTitleLeft.bmWidth + m_bitmapTitleRight.bmWidth))
	{
		CRect rect;
		GetWindowRect(rect);
		MoveWindow(rect.left, rect.top, m_bitmapTitleLeft.bmWidth + m_bitmapTitleRight.bmWidth, rect.Height());
	}

	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpTitleLeft);
	pBufferDC->BitBlt(0, 0, m_bitmapTitleLeft.bmWidth, m_bitmapTitleLeft.bmHeight, pMemDC,0, 0, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpTitleRight);
	pBufferDC->BitBlt(rectDlg.right - m_bitmapTitleRight.bmWidth, 0, m_bitmapTitleRight.bmWidth, m_bitmapTitleRight.bmHeight, pMemDC,0, 0, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	if(rectDlg.Width() > (m_bitmapTitleLeft.bmWidth + m_bitmapTitleRight.bmWidth))
	{
		pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpTitleMid);
		pBufferDC->StretchBlt(m_bitmapTitleLeft.bmWidth, 0, rectDlg.right - m_bitmapTitleLeft.bmWidth - m_bitmapTitleRight.bmWidth, m_bitmapTitleRight.bmHeight,
			pMemDC, 0, 0, m_bitmapTitleMid.bmWidth, m_bitmapTitleMid.bmHeight, SRCCOPY);
		pMemDC->SelectObject(pOldBmp);
	}	
 
	int nBottom = rectDlg.bottom - m_bitmapBottom.bmHeight;
	int nHeight = m_bitmapBottom.bmHeight;
	//Bottom
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBottom);

	//Left : Width = 5
	int nLeftWidth = 5;
	pBufferDC->StretchBlt(0, nBottom , nLeftWidth, nHeight, pMemDC,
		0, 0, nLeftWidth, nHeight, SRCCOPY);

	//Right : Width = 5
	int nRightWidth = 5;
	pBufferDC->StretchBlt(rectDlg.right - nRightWidth, nBottom, nRightWidth, nHeight, pMemDC,
		10, 0, nRightWidth, nHeight, SRCCOPY);

	//Mid
	pBufferDC->StretchBlt(nLeftWidth, nBottom, rectDlg.Width() - ( nLeftWidth + nRightWidth), nHeight,
		pMemDC, nLeftWidth, 0, 5, nHeight, SRCCOPY);	
	pMemDC->SelectObject(pOldBmp);

	//SubTitleBg
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpSubTitleBg);
	pBufferDC->StretchBlt(3, SUB_TITLE_IMAGE_HEIGHT, rectDlg.right - 3, m_bitmapSubTitleBg.bmHeight, pMemDC,
		0, 0, m_bitmapSubTitleBg.bmWidth, m_bitmapSubTitleBg.bmHeight, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	//SubTitleBg Right
	int nRight = m_bitmapSubTitleBgR.bmWidth;
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpSubTitleR);
	pBufferDC->BitBlt(rectDlg.right - nRight, SUB_TITLE_IMAGE_HEIGHT, nRight, m_bitmapSubTitleBgR.bmHeight,
		pMemDC, 0, 0, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	//SubTitleBg Left;
	int nLeft = m_bitmapSubTitle.bmWidth;
	if(m_bmpSubTitle.m_hObject)
	{
		pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpSubTitle);
		pBufferDC->BitBlt(15, SUB_TITLE_IMAGE_HEIGHT, nLeft, m_bitmapSubTitle.bmHeight, pMemDC,
			0, 0, SRCCOPY);
		pMemDC->SelectObject(pOldBmp);
	}

	CString Caption;
	GetWindowText(Caption);
	CFont*	pOldFont = (CFont*)pBufferDC->SelectObject(&m_fontBold);
	pBufferDC->SetBkColor(TRANSPARENT);	
	pBufferDC->SetTextColor(COLOR_FOCUS_CAPTION);	
	pBufferDC->DrawText(Caption, m_rectWindowText, DT_LEFT | DT_END_ELLIPSIS);	
	pBufferDC->SelectObject(pOldFont);
}



void CCustomDlg::DrawOutline(CDC *pBufferDC, CDC *pMemDC)
{
 
	CRect rectDlg;
	GetClientRect(&rectDlg);

	CBitmap* pOldBmp;
	

	int nWidth = m_bitmapOutLine.bmWidth;
	int nHeight = rectDlg.Height() - ( SUB_TITLE_IMAGE_HEIGHT + m_bitmapBottom.bmHeight);
	
	//Left
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpOutlineL);
	pBufferDC->StretchBlt(0, SUB_TITLE_IMAGE_HEIGHT, nWidth, nHeight, pMemDC,
		0, 0, m_bitmapOutLine.bmWidth, m_bitmapOutLine.bmHeight, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	//Right
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpOutlineR);
	pBufferDC->StretchBlt(rectDlg.right - nWidth, SUB_TITLE_IMAGE_HEIGHT, nWidth, nHeight, pMemDC,
		0, 0, m_bitmapOutLine.bmWidth, m_bitmapOutLine.bmHeight, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

}


UINT CCustomDlg::OnNcHitTest(CPoint point)
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	
	ClientToScreen(&rectDlg);
	m_rectCaption = rectDlg;
	m_rectCaption.bottom = m_rectCaption.top + SUB_TITLE_IMAGE_HEIGHT;
	
	UINT nHit = CDialog::OnNcHitTest(point);
	if( m_rectCaption.PtInRect(point))
		return HTCAPTION;
	else
		return nHit;
}


void CCustomDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_btnClose.m_hWnd)
		m_btnClose.MoveWindow(cx - CLOSE_BTN_WIDTH - CLOSE_BTN_RIGHT_MARGIN, CLOSE_BTN_TOP_MARGIN, CLOSE_BTN_WIDTH, CLOSE_BTN_HEIGHT);

	if(m_bmpTitleLeft.m_hObject)
	{
		GetClientRect(m_rectWindowText);
		m_rectWindowText.left = m_bitmapTitleLeft.bmWidth + CAPTION_LEFT_MARGIN;
		m_rectWindowText.right -= (CAPTION_RIGHT_MARGIN + CLOSE_BTN_WIDTH);
		m_rectWindowText.top = (m_bitmapTitleLeft.bmHeight - FONT_HEIGHT)/2 + 1;
		m_rectWindowText.bottom = m_bitmapTitleLeft.bmHeight - (m_bitmapTitleLeft.bmHeight - FONT_HEIGHT)/2 + 1;
	}

	
}


void CCustomDlg::DrawRect(CDC *pBufferDC, CRect rect, int nExcept)
{
	switch(nExcept)
	{
	case 0 : 
		//LT - LB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top );
		pBufferDC->LineTo(rect.left - SPACE_CTRL_RECT, rect.bottom);
		
		//LB - RB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.bottom );
		pBufferDC->LineTo(rect.right, rect.bottom );
		
		//RB - RT
		pBufferDC->MoveTo(rect.right , rect.bottom);
		pBufferDC->LineTo(rect.right , rect.top - 1);
		
		//LT - RT
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );
		pBufferDC->LineTo(rect.right + SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );	
		break;
	case 1 : 
		//LB - RB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.bottom );
		pBufferDC->LineTo(rect.right, rect.bottom );
		
		//RB - RT
		pBufferDC->MoveTo(rect.right , rect.bottom);
		pBufferDC->LineTo(rect.right , rect.top - SPACE_CTRL_RECT);
		
		//LT - RT
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );
		pBufferDC->LineTo(rect.right + SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );	
		break;
	case 2 : 
		//LT - LB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top );
		pBufferDC->LineTo(rect.left - SPACE_CTRL_RECT, rect.bottom); 
		
		//RB - RT
		pBufferDC->MoveTo(rect.right , rect.bottom);
		pBufferDC->LineTo(rect.right , rect.top - SPACE_CTRL_RECT);
		
		//LT - RT
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );
		pBufferDC->LineTo(rect.right + SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );	
		break;
	case 3 : 
		//LT - LB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top );
		pBufferDC->LineTo(rect.left - SPACE_CTRL_RECT, rect.bottom);
		
		//LB - RB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.bottom );
		pBufferDC->LineTo(rect.right, rect.bottom ); 
		//LT - RT
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );
		pBufferDC->LineTo(rect.right + SPACE_CTRL_RECT, rect.top - SPACE_CTRL_RECT  );	
		break;
	case 4 : 
		//LT - LB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.top );
		pBufferDC->LineTo(rect.left - SPACE_CTRL_RECT, rect.bottom);
		
		//LB - RB
		pBufferDC->MoveTo(rect.left - SPACE_CTRL_RECT, rect.bottom );
		pBufferDC->LineTo(rect.right, rect.bottom );
		
		//RB - RT
		pBufferDC->MoveTo(rect.right , rect.bottom);
		pBufferDC->LineTo(rect.right , rect.top - SPACE_CTRL_RECT); 
		break;
	}			
}

void CCustomDlg::SetSubTitleImage(UINT nIDResource)
{
	m_nIDResource = nIDResource;
}

void CCustomDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CCustomDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	CPaintDC dc(this);
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	//pMemDC->CreateCompatibleDC(&dc);
	OnEraseBkgnd(&MemDC);

	MemDC.DeleteDC();
}