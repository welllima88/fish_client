// Dlg_Login.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "Dlg_Login.h"
#include "resource.h"
#include "GlobalIconManager.h"
#include "fish_common.h"
#include "./util/md5e.h"
#include "LocalProperties.h"
#include "URL.h"
#include "Dlg_MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_Login dialog

#define DLG_WIDTH				360
#define DLG_HEIGHT				262

#define CONTENTS_WIDTH			326
#define CONTENTS_HEIGHT			161

#define SPACE_CTRL_RECT			1
#define OUTLINE_BORDER			1
#define CONTENTS_TOP			75

#define MARGIN_BOTTOM			26
#define MARGIN_LEFT				17
#define MARGIN_RIGHT			17

#define EDIT_HEIGHT				20
#define EDIT_WIDTH				80

#define DEFAULT_NULLPASSWORD	_T("********")

#define STR_ERR_DL_NOIDFOUND			_T("아이디를 입력해 주세요.")
#define STR_ERR_DL_NOPASSWORDFOUND		_T("패스워드를 입력해 주세요.")
#define STR_ERR_DL_NOTENOUGHPASSWORD	_T("패스워드는 %d자 이상 입력해 주세요.")
#define STR_ERR_DL_PASSWORDENCODEFAIL	_T("패스워드 암호화에 실패했습니다.")

CDlg_Login::CDlg_Login(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_Login::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_Login)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bDrawBySelf = TRUE;
}


void CDlg_Login::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_Login)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_EDIT_ID, m_editId);
	DDX_Text(pDX, IDC_EDIT_ID, m_sID);
	DDV_MaxChars(pDX, m_sID, 64);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_editPasswd);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_sPasswd);
	DDV_MaxChars(pDX, m_sPasswd, 16);
	DDX_Control(pDX, IDC_CHECK_PASSWD_SAVE, m_checkPasswdSave);
	DDX_Control(pDX, IDC_CHECK_AUTO_LOGIN, m_checkAutoLogin);	
	DDX_Check(pDX, IDC_CHECK_PASSWD_SAVE, m_bSavePassword);
	DDX_Check(pDX, IDC_CHECK_AUTO_LOGIN, m_bAutoLogin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_Login, CDialog)
	//{{AFX_MSG_MAP(CDlg_Login)
	// NOTE: the ClassWizard will add message map macros here
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
//	ON_WM_MOUSEMOVE()
//	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(LOGIN_DLG_CLOSE_ID, OnCancel)
	ON_BN_CLICKED(LOGIN_DLG_LOGIN_ID, OnLogin)
	ON_BN_CLICKED(LOGIN_DLG_BTN_REG_ID, OnReg)
	ON_BN_CLICKED(LOGIN_DLG_BTN_FIND_ID, OnFind)
//	ON_EN_CHANGE(IDC_EDIT_MAXDOWNLOAD, OnChangeEditPassword)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOGINRESULT, OnLoginResult)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_Login message handlers

BOOL CDlg_Login::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rectDlg;	

	GetWindowRect(rectDlg);
	MoveWindow(rectDlg.left, rectDlg.top, DLG_WIDTH, DLG_HEIGHT);

	GetClientRect(rectDlg);
	CRect rectBtn;
	rectBtn = rectDlg;
	rectBtn.top += 8;
	rectBtn.bottom = rectBtn.top + 14;
	rectBtn.right -= 8;
	rectBtn.left = rectBtn.right - 14;
	
	m_btnClose.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rectBtn,
				this, LOGIN_DLG_CLOSE_ID);

	m_btnLogin.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rectBtn,
				this, LOGIN_DLG_LOGIN_ID);

	m_btnIdPasswdFind.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rectBtn,
				this, LOGIN_DLG_BTN_FIND_ID);

	m_btnReg.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT, rectBtn,
				this, LOGIN_DLG_BTN_REG_ID);

	CClientDC dc(this);
    CFishBMPManager* BMPManager = CFishBMPManager::getInstance();
    CImageList* sysIMGLIST  =   BMPManager->getSysmenuBtn();

	CBitmap btnCls_BMP;

    m_btnClose.ShowWindow(SW_NORMAL);
	m_btnClose.SetShowText(FALSE);
	//m_btnClose.SetTransParentColor(RGB(222, 255, 0), TRUE);	
	m_btnClose.LoadBitmaps(IDB_BTN_CLOSE_N, IDB_BTN_CLOSE_N, IDB_BTN_CLOSE_D, IDB_BTN_CLOSE_N, 14, 14);

	/*
    BMPManager->getBMPFromList(&dc, sysIMGLIST, 3, &btnCls_BMP);
	m_btnClose.SetFocussedBitmap(&btnCls_BMP);

	BMPManager->getBMPFromList(&dc, sysIMGLIST, 7, &btnCls_BMP);
	m_btnClose.SetEnableBitmap(&btnCls_BMP);

    BMPManager->getBMPFromList(&dc, sysIMGLIST, 11, &btnCls_BMP);
	m_btnClose.SetDownBitmap(&btnCls_BMP);
	*/
	
	InitObjects();

	m_sID = theApp.m_spLP->GetUserID();
	if(m_sID == DEFAULT_USERID)
		m_sID.Empty();
	
	m_bSavePassword = theApp.m_spLP->GetSavePassword();
	if(m_bSavePassword && theApp.m_spLP->GetEncodedPassword().GetLength() >= 4)
	{
		m_sPasswd = DEFAULT_NULLPASSWORD;
		m_bAutoLogin = theApp.m_spLP->GetAutoLogin();
	}

	UpdateData(FALSE);

	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_Login::InitObjects()
{
	
	m_bmpTitle.LoadBitmap(IDB_LOGINDLG_TITLE);
	m_bmpTitle.GetBitmap(&m_bitmapTitle);

	/*
	m_bmpHelp.LoadBitmap(IDB_LOGINDLG_HELP);
	m_bmpHelp.GetBitmap(&m_bitmapHelp);
	*/

	m_bmpBgTop.LoadBitmap(IDB_LOGINDLG_BG_TOP);
	m_bmpBgTop.GetBitmap(&m_bitmapBgTop);
	m_bmpBgMid.LoadBitmap(IDB_LOGINDLG_BG_MID);
	m_bmpBgMid.GetBitmap(&m_bitmapBgMid);
	m_bmpBgBottom.LoadBitmap(IDB_LOGINDLG_BG_BOTTOM);
	m_bmpBgBottom.GetBitmap(&m_bitmapBgBottom);

	m_bmpBgContents.LoadBitmap(IDB_LOGINDLG_BG_CONTENTS);
	m_bmpBgContents.GetBitmap(&m_bitmapBgContents);
	
	m_bmpIdText.LoadBitmap(IDB_LOGINDLG_ID_T);
	m_bmpIdText.GetBitmap(&m_bitmapIdText);
	m_bmpPasswdText.LoadBitmap(IDB_LOGINDLG_PASSWD_T);

	m_bmpPasswdSaveText.LoadBitmap(IDB_LOGINDLG_PASSWD_SAVE_T);
	m_bmpPasswdSaveText.GetBitmap(&m_bitmapPasswdSaveText);
	m_bmpAutoLoginText.LoadBitmap(IDB_LOGINDLG_AUTOLOGIN_T);
	m_bmpAutoLoginText.GetBitmap(&m_bitmapAutoLoginText);
	
	/*
	m_bmpBox.LoadBitmap(IDB_LOGINDLG_BOX);
	m_bmpBox.GetBitmap(&m_bitmapBox);
	*/

	m_fontBold.CreateFont(14, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	m_fontNormal.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	m_fontUnderline.CreateFont(14, 0, 0, 0,  FW_NORMAL, FALSE, TRUE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);


	m_btnLogin.LoadBitmaps(IDB_LOGINDLG_LOGINBTN_N, IDB_LOGINDLG_LOGINBTN_O, IDB_LOGINDLG_LOGINBTN_O, IDB_LOGINDLG_LOGINBTN_O,
			55, 49);

	m_btnIdPasswdFind.LoadBitmaps(IDB_LOGINDLG_BTN_FIND_N, IDB_LOGINDLG_BTN_FIND_D, IDB_LOGINDLG_BTN_FIND_D, IDB_LOGINDLG_BTN_FIND_N,
			144, 25);

	m_btnReg.LoadBitmaps(IDB_LOGINDLG_BTN_REG_N, IDB_LOGINDLG_BTN_REG_D, IDB_LOGINDLG_BTN_REG_D, IDB_LOGINDLG_BTN_REG_N,
			77, 25);
	
	m_penDlgOutline_out.CreatePen(PS_SOLID, 1, COLOR_LOGIN_DLG_OUTLINE_OUT);
	m_penDlgOutline_in.CreatePen(PS_SOLID, 1, COLOR_LOGIN_DLG_OUTLINE_IN);
	m_penDotLine.CreatePen(PS_DOT, 1, RGB(170, 170, 170));

	CRect rect;
	GetClientRect(rect);

	m_btnLogin.MoveWindow(rect.right - MARGIN_LEFT - 31 - 55, CONTENTS_TOP + 18, 55, 49);
	m_btnIdPasswdFind.MoveWindow(rect.right - MARGIN_RIGHT - 144 - 31, rect.bottom - 70, 144, 25);
	m_btnIdPasswdFind.GetWindowRect(rect);
	ScreenToClient(rect);
	m_btnReg.MoveWindow(rect.left - 77 - 5, rect.top, 77, 25);

	m_btnLogin.GetWindowRect(rect);
	ScreenToClient(rect);
	m_editId.MoveWindow(MARGIN_LEFT + 31 + m_bitmapIdText.bmWidth + 10, CONTENTS_TOP + 18, rect.left - 10 - (MARGIN_LEFT + 31 + m_bitmapIdText.bmWidth + 10), EDIT_HEIGHT);
	m_editId.SetFont(&m_fontNormal);
	m_editId.GetWindowRect(rect);
	ScreenToClient(rect);
	m_editPasswd.MoveWindow(rect.left, rect.bottom + 10, rect.Width(), EDIT_HEIGHT);
	m_editPasswd.SetFont(&m_fontNormal);

	m_editPasswd.GetWindowRect(rect);
	ScreenToClient(rect);
	m_checkPasswdSave.MoveWindow(rect.left + 15, rect.bottom + 11, 13, 13);	
	m_checkAutoLogin.MoveWindow(rect.right - 13, rect.bottom + 11, 13, 13);

	/*
	GetClientRect(rect);

	m_rectReg.left = rect.left + 94;
	m_rectReg.top = rect.bottom - 30;
	m_rectReg.bottom = m_rectReg.top + 20;
	m_rectReg.right = m_rectReg.left + 55;


	m_rectFind.left = rect.left + 160;
	m_rectFind.top = rect.bottom - 30;
	m_rectFind.bottom = m_rectFind.top + 20;
	m_rectFind.right = m_rectFind.left + 120;
	*/

}

void CDlg_Login::DeleteObjects()
{
	
	if( m_bmpTitle.m_hObject)
		m_bmpTitle.DeleteObject();

	/*
	if( m_bmpHelp.m_hObject)
		m_bmpHelp.DeleteObject();
	*/
	
	if( m_bmpBgTop.m_hObject )
		m_bmpBgTop.DeleteObject();
	if( m_bmpBgMid.m_hObject )
		m_bmpBgMid.DeleteObject();
	if( m_bmpBgBottom.m_hObject )
		m_bmpBgBottom.DeleteObject();

	if( m_bmpBgContents.m_hObject )
		m_bmpBgContents.DeleteObject();

	if( m_bmpIdText.m_hObject )
		m_bmpIdText.DeleteObject();

	if( m_bmpPasswdText.m_hObject )
		m_bmpPasswdText.DeleteObject();

	if( m_bmpPasswdSaveText.m_hObject )
		m_bmpPasswdSaveText.DeleteObject();
	if( m_bmpAutoLoginText.m_hObject )
		m_bmpAutoLoginText.DeleteObject();

	//if( m_bmpBox.m_hObject )
	//	m_bmpBox.DeleteObject();

	if( m_penDlgOutline_out.m_hObject )
		m_penDlgOutline_out.DeleteObject();
	if( m_penDlgOutline_in.m_hObject )
		m_penDlgOutline_in.DeleteObject();
}

 
afx_msg BOOL CDlg_Login::OnEraseBkgnd(CDC* pDC)
{

	if(m_bDrawBySelf)
	{
		DrawItems2(pDC);
		return TRUE;
	}

	return CDialog::OnEraseBkgnd(pDC); 
}
 

void CDlg_Login::SetDrawBySelf(BOOL bDraw)
{
	m_bDrawBySelf = bDraw;
}

void CDlg_Login::DrawItems2(CDC *pDC)
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

void CDlg_Login::DrawItems(CDC *pDC)
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

void CDlg_Login::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{
	CRect rectDlg, rect;
	GetClientRect(&rectDlg);	
	//pBufferDC->FillSolidRect(0, 0, rectDlg.Width(), rectDlg.Height(), COLOR_LOGIN_DLG_BG);	
 
	//BG Top
	CBitmap* pOldBmp;	
	int nWidth = 3;
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBgTop);
	//Top Left
	pBufferDC->StretchBlt(0, 0, nWidth, m_bitmapBgTop.bmHeight, pMemDC, 0, 0, nWidth, m_bitmapBgTop.bmHeight, SRCCOPY);
	//Top Mid	
	pBufferDC->StretchBlt(nWidth, 0, rectDlg.Width() - nWidth * 2, m_bitmapBgTop.bmHeight, pMemDC,
		nWidth, 0, nWidth, m_bitmapBgTop.bmHeight, SRCCOPY);
	//Top Right 
	pBufferDC->StretchBlt(rectDlg.Width() - nWidth, 0, nWidth, m_bitmapBgTop.bmHeight, pMemDC,
		nWidth * 2, 0, nWidth, m_bitmapBgTop.bmHeight, SRCCOPY);	
	pMemDC->SelectObject(pOldBmp);


	//BG Mid
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBgMid);
	//Mid Left
	pBufferDC->StretchBlt(0, CONTENTS_TOP, nWidth, m_bitmapBgMid.bmHeight, pMemDC, 0, 0, nWidth, m_bitmapBgMid.bmHeight, SRCCOPY);
	//Mid Mid	
	pBufferDC->StretchBlt(nWidth, CONTENTS_TOP, rectDlg.Width() - nWidth * 2, m_bitmapBgMid.bmHeight, pMemDC,
		nWidth, 0, nWidth, m_bitmapBgMid.bmHeight, SRCCOPY);
	//Mid Right 
	pBufferDC->StretchBlt(rectDlg.Width() - nWidth, CONTENTS_TOP, nWidth, m_bitmapBgMid.bmHeight, pMemDC,
		nWidth * 2, 0, nWidth, m_bitmapBgMid.bmHeight, SRCCOPY);	
	pMemDC->SelectObject(pOldBmp);

	//BG Bottom
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBgBottom);
	//Bottom Left
	pBufferDC->StretchBlt(0, rectDlg.Height() - m_bitmapBgBottom.bmHeight, nWidth, m_bitmapBgBottom.bmHeight, pMemDC, 0, 0, nWidth, m_bitmapBgBottom.bmHeight, SRCCOPY);
	//Bottom Mid	
	pBufferDC->StretchBlt(nWidth, rectDlg.Height() - m_bitmapBgBottom.bmHeight, rectDlg.Width() - nWidth * 2, m_bitmapBgBottom.bmHeight, pMemDC,
		nWidth, 0, nWidth, m_bitmapBgBottom.bmHeight, SRCCOPY);
	//Bottom Right 
	pBufferDC->StretchBlt(rectDlg.Width() - nWidth, rectDlg.Height() - m_bitmapBgBottom.bmHeight, nWidth, m_bitmapBgBottom.bmHeight, pMemDC,
		nWidth * 2, 0, nWidth, m_bitmapBgBottom.bmHeight, SRCCOPY);	
	pMemDC->SelectObject(pOldBmp);



	//title Image
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpTitle);
	pBufferDC->BitBlt(12, 0, m_bitmapTitle.bmWidth, m_bitmapTitle.bmHeight, pMemDC, 0, 0, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	rectDlg.top = CONTENTS_TOP;
	rectDlg.bottom = rectDlg.bottom - MARGIN_BOTTOM;
	rectDlg.left = MARGIN_LEFT;
	rectDlg.right = rectDlg.right - MARGIN_RIGHT;

	// Contents Background
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBgContents);
	pBufferDC->StretchBlt(MARGIN_LEFT, CONTENTS_TOP, CONTENTS_WIDTH, m_bitmapBgContents.bmHeight, pMemDC, 0, 0,
		m_bitmapBgContents.bmWidth, m_bitmapBgContents.bmHeight, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	// ID Text Image
	m_editId.GetWindowRect(rect);
	ScreenToClient(rect);
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpIdText);
	pBufferDC->BitBlt(rectDlg.left + 31, rect.top + (rect.Height() - m_bitmapIdText.bmHeight)/2,
		m_bitmapIdText.bmWidth, m_bitmapIdText.bmHeight, pMemDC, 0, 0, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	// Password Text Image
	m_editPasswd.GetWindowRect(rect);
	ScreenToClient(rect);
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpPasswdText);
	pBufferDC->BitBlt(rectDlg.left + 31, rect.top + (rect.Height() - m_bitmapIdText.bmHeight)/2,
		m_bitmapIdText.bmWidth, m_bitmapIdText.bmHeight, pMemDC, 0, 0, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	// Auto Login Text Image	
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpAutoLoginText);
	pBufferDC->BitBlt(rectDlg.right - 31 - m_bitmapAutoLoginText.bmWidth + 2, rect.bottom + 11, m_bitmapPasswdSaveText.bmWidth, m_bitmapPasswdSaveText.bmHeight, pMemDC, 0, 0, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	// Password Save Text Image
	m_checkAutoLogin.GetWindowRect(rect);
	ScreenToClient(rect);
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpPasswdSaveText);
	pBufferDC->BitBlt(rect.left - 24 - m_bitmapPasswdSaveText.bmWidth, rect.top, m_bitmapPasswdSaveText.bmWidth, m_bitmapPasswdSaveText.bmHeight, pMemDC, 0, 0, SRCCOPY);
	pOldBmp = pBufferDC->SelectObject(pOldBmp);

	// Dot Line
	CPen* pOldPen = pBufferDC->SelectObject(&m_penDotLine);	
	//pBufferDC->MoveTo(rectDlg.left + 7, rectDlg.top + 16 );
	//pBufferDC->LineTo(rectDlg.right - 7, rectDlg.top + 16);
	pBufferDC->MoveTo(rectDlg.left + 31, rect.bottom + 11 );
	pBufferDC->LineTo(rectDlg.right - 31, rect.bottom + 11);
	pBufferDC->SelectObject(pOldPen);


}



void CDlg_Login::DrawOutline(CDC *pBufferDC, CDC *pMemDC)
{
 
	/*
	CRect rectDlg;	
	GetClientRect(&rectDlg);
	
	rectDlg.top += OUTLINE_BORDER;
	rectDlg.left += OUTLINE_BORDER;
	rectDlg.bottom -= OUTLINE_BORDER;
	rectDlg.right -= OUTLINE_BORDER;	

	CPen* oldPen = pBufferDC->SelectObject(&m_penDlgOutline_out);
	DrawRect(pBufferDC, rectDlg, 0);
	pBufferDC->SelectObject(oldPen);

	rectDlg.top += OUTLINE_BORDER;
	rectDlg.left += OUTLINE_BORDER;
	rectDlg.bottom -= OUTLINE_BORDER;
	rectDlg.right -= OUTLINE_BORDER;

	oldPen = pBufferDC->SelectObject(&m_penDlgOutline_in);
	DrawRect(pBufferDC, rectDlg, 0);
	pBufferDC->SelectObject(oldPen);
	*/
}


UINT CDlg_Login::OnNcHitTest(CPoint point)
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



void CDlg_Login::DrawRect(CDC *pBufferDC, CRect rect, int nExcept)
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

HBRUSH CDlg_Login::OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(248, 248, 248));
		pDC->SetTextColor(RGB(85, 85, 85));
		hbr = CreateSolidBrush(RGB(248,248,248));
	}
	*/
	
	return hbr;	
}
/*
void CDlg_Login::OnMouseMove(UINT nFlags, CPoint point)
{
	
	if(PtInRect(&m_rectReg, point))
	{
	
	}
	else if(PtInRect(&m_rectFind, point))
	{
	
	}
	else
	{
	
	}
	

	CDialog::OnMouseMove(nFlags, point);
}


void CDlg_Login::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	if(PtInRect(&m_rectReg, point))
	{
		// TO DO
		// 회원가입....
		OnReg();

	}
	else if(PtInRect(&m_rectFind, point))
	{
		// TO DO
		// 아이디/비밀번호 찾기....
	}
	

	CDialog::OnLButtonDown(nFlags, point);
}
*/

void CDlg_Login::OnCancel()
{
	CDialog::OnCancel();
	OnLoginResult(ERR_LOGINIGNORED, 0);
//	::PostMessage(m_hMsgHwnd, WM_REQUESTLOGIN, 0, 0);
}

void CDlg_Login::OnLogin()
{
	// check edit item's
	UpdateData();
	// ID, Password Check & Encode Password

	if(m_sID.IsEmpty())
	{
		//AfxMessageBox(STR_ERR_DL_NOIDFOUND);
        FishMessageBox(STR_ERR_DL_NOIDFOUND);
		return;
	}

	if(m_sPasswd.IsEmpty())
	{
//		AfxMessageBox(STR_ERR_DL_NOPASSWORDFOUND);
        FishMessageBox(STR_ERR_DL_NOPASSWORDFOUND);
		return;
	}

	if(m_sPasswd.GetLength() < 4)
	{
		CString msg;
		msg.Format(STR_ERR_DL_NOTENOUGHPASSWORD, 4);
//		AfxMessageBox(msg);
        FishMessageBox((LPCTSTR)msg);
		return;
	}

	if(m_sPasswd.Compare(DEFAULT_NULLPASSWORD) != 0)
	{
		char password[128], encoded[128];

#ifdef _UNICODE
		AnsiConvertWtoM((LPTSTR) (LPCTSTR) m_sPasswd, password, sizeof(password));
#else
		strcpy(password, m_sPasswd);
#endif

		if(!md5(password, encoded))
		{
//			AfxMessageBox(STR_ERR_DL_PASSWORDENCODEFAIL);
            FishMessageBox(STR_ERR_DL_PASSWORDENCODEFAIL);
			return;
		}

		TCHAR encodedtmp[128];
#ifdef _UNICODE
		AnsiConvertMtoW(encoded, encodedtmp, sizeof(encodedtmp));
#else
		strcpy(encodedtmp, encoded);
#endif

		theApp.m_spLP->SetPassword(encodedtmp);
	}
	theApp.m_spLP->SetTempUserID(m_sID);

	m_editId.EnableWindow(FALSE);
	m_editPasswd.EnableWindow(FALSE);
	m_btnLogin.EnableWindow(FALSE);

//	::SendMessage(m_hMsgHwnd, WM_REQUESTLOGIN, 0, 0);
}

void CDlg_Login::OnOK()
{
	OnLogin();
}

CString CDlg_Login::GetLoginErrString(int err)
{
	switch(err)
	{
	case ERR_WB_HOSTNOTFOUND:
		return STR_LERR_HOSTNOTFOUND;
		break;
	case ERR_WB_WRONGPASS:
		return STR_LERR_PASSWORD;
		break;
	case ERR_WB_IDNOTEXIST:
		return STR_LERR_ID;
		break;
	case ERR_WB_DBCONN:
		return STR_LERR_DB;
		break;
	case ERR_WB_LOGINPARAMETER:
		return STR_LERR_PARAMETER;
		break;
	}

	return STR_LERR_UNKNOWN;
}

/**************************************************************************
 * method CMainFrame::OnLoginResult
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-27
 *
 * @Description
 * 웹과 IRC의 로그인 작업 결과를 알려온다.
 * 결과에 따라 창을 닫고 진행을 하거나
 * 오류를 보여준다.
 **************************************************************************/
void CDlg_Login::OnLoginResult(WPARAM wParam, LPARAM lParam)
{
	if(wParam == ERR_NONE)
	{
		theApp.m_spLP->SetSavePassword(m_bSavePassword);
		if(m_bSavePassword)
		{
			theApp.m_spLP->SetAutoLogin(m_bAutoLogin);
		}

		theApp.m_spLP->SaveProperties();

		ShowWindow(SW_HIDE);

		EndDialog(IDOK);
		SetTimer(TID_INITUSER, 10, 0);
//		::SendMessage(m_hMsgHwnd, WM_LOGINCOMPLETE, 0, 0);
	}
	else if(wParam == ERR_LOGINIGNORED)		// ignored means canceled
	{
		ShowWindow(SW_HIDE);
		EndDialog(IDOK);
		SetTimer(TID_INITUSER, 10, 0);
	}
	else
	{
		m_editId.EnableWindow(TRUE);
		m_editPasswd.EnableWindow(TRUE);
		m_btnLogin.EnableWindow(TRUE);

		// 적절한 오류 처리를 수행한다.

		if(wParam != ERR_LOGINRETRY)
		{
			CString s;
			s.Format(_T("%s"), GetLoginErrString((int) wParam));
//			AfxMessageBox(s);
            FishMessageBox((LPCTSTR)s);
		}
	}
}

BOOL CDlg_Login::Create(CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, pParentWnd);
}

void CDlg_Login::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TID_INITUSER)
	{
		KillTimer(nIDEvent);
//		::SendMessage(m_hMsgHwnd, WM_LOGINCOMPLETE, 0, 0);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlg_Login::OnReg()
{
	CURL url;
	url.Open(REGISTER_PAGE);
//	::ShellExecute(NULL,_T("open"),REGISTER_PAGE, NULL,NULL,SW_SHOW);
}

void CDlg_Login::OnFind()
{

}