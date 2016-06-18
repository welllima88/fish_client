// Dlg_PropUnit.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_PropUnit.h"
#include "resource.h"
#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_PropUnit dialog

#define SPACE_CTRL_RECT			1

CDlg_PropUnit::CDlg_PropUnit(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CDlg_PropUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	PropID		= 0;
	MenuName	= _T("");
	Title		= _T("");
	bModified	= FALSE;

	nResourceID = nIDTemplate;
}


void CDlg_PropUnit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_PropUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_PropUnit, CDialog)
	//{{AFX_MSG_MAP(CDlg_PropUnit)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_PropUnit message handlers

BOOL CDlg_PropUnit::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* parent = GetParent();
	if(parent)
		parent->SendMessage(WM_CHILD_INIT, (WPARAM) this, 0);

	m_fontBold.CreateFont(15, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	m_fontNormal.CreateFont(15, 0, 0, 0,  FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	             // EXCEPTION: OCX Property Pages should return FALSE
}
 
afx_msg BOOL CDlg_PropUnit::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC); 
}


UINT CDlg_PropUnit::OnNcHitTest(CPoint point)
{
	UINT nHit = CDialog::OnNcHitTest(point);
	return nHit;
}

BOOL CDlg_PropUnit::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(nResourceID, pParentWnd);
}

void CDlg_PropUnit::SendModified()
{
	CWnd* parent = GetParent();
	if(parent)
		parent->SendMessage(WM_CHILD_MODIFIED, 0, 0);
}

BOOL CDlg_PropUnit::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			CWnd* parent = GetParent();
			if(parent)
				parent->PostMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);

			return TRUE;
		}
        break;
	default:
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlg_PropUnit::OnDestroy()
{
	if(m_fontBold.m_hObject)
		m_fontBold.DeleteObject();
	if(m_fontBold.m_hObject)
		m_fontBold.DeleteObject();

	CDialog::OnDestroy();
}