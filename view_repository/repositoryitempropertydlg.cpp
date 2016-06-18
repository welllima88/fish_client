// repositoryitempropertydlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\FISH.h"
#include "repositoryitempropertydlg.h"
#include "../fish_common.h"
#include "../fish_def.h"
#include "../resource.h"
#include "../MainFrm.h"
#include "../Dlg_Subscribe.h"

#include "../RSSCurrentList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFERSIZE	1024*1024

#define GROUP_SIZE_Y				205
#define SCRAP_SIZE_Y				205
#define CHANNEL_SIZE_Y				310
#define SEARCH_SIZE_Y				235
#define DLG_SIZE_X					460
#define CONTENTS_TOP				80
#define COMPONENT_GAP				10
#define EDIT_HEIGHT					16
#define STATIC_WIDTH				65
#define STATIC_HEIGHT				21
#define BTN_HEIGHT					25
#define BTN_WIDTH					73
#define FEEDINTERVAL_EDIT_WIDTH		100
//#define FEEDINTERVAL_EDIT_WIDTH		50
#define EDIT_MARGIN_TOP				3
#define WIZARD_COMMENT_HEIGHT		21

#define MARGIN_LEFT					18
#define MARGIN_RIGHT				18
#define MARGIN_TOP					40
#define MARGIN_BOTTOM				12

#define	ICON_MARGIN_RIGHT			12
#define	ICON_MARGIN_BOTTOM			8

#define WIZARD_URL_INTPUT_T			_T("추가하려는 RSS Feed 주소나 Blog 주소를 입력하세요.")
#define WIZARD_URL_SELECT_T			_T("%d개의 주소가 검색 되었습니다. 주소를 선택하세요.")

#define STR_ERR_XML_OR_XMLURL		_T("Feed XML이 정상적이지 않거나 잘못된 주소입니다.")
#define STR_ERR_NOT_FOUND_AUTO		_T("자동 검색으로 RSS Feed를 찾지 못했습니다. Feed 주소를 직접입력해 주세요")
#define STR_ERR_URL					_T("잘못된 주소이거나 페이지를 로드할 수 없습니다.")
#define STR_ERR_TITLE				_T("URL 검증 실패")

#define STATIC_XML_URL_T			_T("XML 주소")
#define STATIC_URL_T				_T("URL")
#define STR_5MIN					_T("5 분")
#define STR_15MIN					_T("15 분")
#define STR_30MIN					_T("30 분")
#define STR_1HOUR					_T("1 시간")
#define STR_2HOUR					_T("2 시간")
#define STR_NOUPDATE				_T("갱신하지 않음")
#define STR_FIND_MSG1				_T("개의 주소가 검색 되었습니다.")
#define STR_FIND_MSG2				_T(" 주소를 선택하세요.")
#define STR_URL_NOT_FOUND_MSG		_T("자동검색으로 Feed 주소를 찾을수 없었습니다. URL을 확인 해주세요.")
#define STR_URL_FAULT_MSG1			_T("페이지가 없거나 잘못된 주소입니다.")
#define STR_URL_FAULT_MSG2			_T("Feed 주소가 잘못입력 되었거나 파일을 로드할수 없습니다.")
#define STR_NEW_CHANNEL				_T("새로운 채널을 ")
#define STR_NEW_SCRAP				_T("새로운 스크랩 채널을 ")
#define STR_NEW_GROUP				_T("새로운 그룹을 ")
#define STR_NEW_KEYWORD				_T("새로운 키워드 자동검색 채널을 ")
#define STR_NEW_SEARCH				_T("새로운 검색 채널을 ")
#define STR_WORD_REG				_T("등록")
#define STR_WORD_MODIFY				_T("수정")
#define STR_QUESTION				_T("하시겠습니까?")
#define STR_CHANNEL_INFO			_T("채널 정보를 ")
#define STR_GROUP_INFO				_T("그룹 정보를 ")

#define STR_EDIT_GROUP              _T("그룹 수정")
#define STR_REGISTER_GROUP          _T("그룹 등록")
#define STR_GROUPNAME               _T("그룹명")
#define STR_EDIT_SUBSCRIBECHANNEL   _T("구독채널 수정")
#define STR_EDIT_SCRAPCHANNEL       _T("스크랩채널 수정")
#define STR_REGISTER_SCRAPCHANNEL   _T("스크랩채널 등록")
#define STR_EDIT_SRCH_CHANNEL       _T("검색채널 수정")
#define STR_REGISTER_SRCH_CHANNEL   _T("검색채널 등록")
#define STR_EDIT_KEYWORD_AUTOSRCH_CHANNEL       _T("키워드 자동검색 채널 수정")
#define STR_REGISTER_KEYWORD_AUTOSRCH_CHANNEL   _T("키워드 자동검색 채널 등록")
#define STR_REGISTER_SUBSCRIBE_CHANNEL          _T("구독채널 등록")
list<CString> GetUrlListInDocument(CString& htmlstream, CString doc_url, BOOL bFindFrame);

/////////////////////////////////////////////////////////////////////////////
// CFishRepItemPropertyDlg dialog


CFishRepItemPropertyDlg::CFishRepItemPropertyDlg(CWnd* pParent /*=NULL*/)
	: CCustomDlg(IDD_REPOSITORY_ITEM_PROPERTY, pParent)
{
	//{{AFX_DATA_INIT(CFishRepItemPropertyDlg)
	m_strTitle = _T("");
	m_strComment = _T("");	
	m_strXmlUrl = _T("");
	m_strHtmlUrl = _T("");
	//}}AFX_DATA_INIT

//	m_nFeedInterval = 0;
	m_nDlgType = GROUP_DLG;
	m_nWizardStep = 0;
	m_nTargetGroup = 0;
	m_bUrlCheck = FALSE;
	m_bAutoSearch = TRUE;
	m_pSubscribe = NULL;
	m_bChangeSearch = FALSE;
	m_bThreadRunning = FALSE;

    m_hPrevFocusWnd =   GetFocus()->GetSafeHwnd();     // added by eternalbleu
}

CFishRepItemPropertyDlg::~CFishRepItemPropertyDlg()
{
    //////////////////////////////////////////////////////////////////////////
    // added by eternalbleu
    // 다이얼로그의 호출 이후에도 포커스를 이전 윈도우에 유지시킬 수 잇게 한다.
/*    CWnd* pCurrentFocusWnd  =   GetFocus();
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
	DeleteObject(m_hBrushEditBg);
	DeleteObjects();
}

void CFishRepItemPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFishRepItemPropertyDlg)
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
//	DDV_MaxChars(pDX, m_strTitle, 50);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_strComment);	
//	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nFeedInterval);
	DDX_Text(pDX, IDC_EDIT_XML, m_strXmlUrl);
	DDX_Text(pDX, IDC_EDIT_HTML, m_strHtmlUrl);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_strKeyword);
	DDX_Control(pDX, IDC_EDIT_TITLE, m_editTitle);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
//	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_editFeedInterval);
	DDX_Control(pDX, IDC_COMBO_INTERVAL, m_comboFeedInterval);
	DDX_Control(pDX, IDC_EDIT_XML, m_editXmlUrl);
	DDX_Control(pDX, IDC_EDIT_HTML, m_editHtmlUrl);
	DDX_Control(pDX, IDC_EDIT_KEYWORD, m_editKeyword);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
	DDX_Control(pDX, IDC_STATIC_COMMENT, m_staticComment);
	DDX_Control(pDX, IDC_STATIC_INTERVAL, m_staticFeedInterval);
	DDX_Control(pDX, IDC_STATIC_KEYWORD, m_staticKeyword);
	DDX_Control(pDX, IDC_STATIC_XML, m_staticXmlUrl);
	DDX_Control(pDX, IDC_STATIC_HTML, m_staticHtmlUrl);
//	DDX_Control(pDX, IDC_STATIC_MIN, m_staticMin);
	DDX_Control(pDX, IDC_STATIC_TARGET_GROUP, m_staticTargetGroup);
	DDX_Control(pDX, IDC_LIST_RSSURL, m_listFeedUrl);	
	DDX_Control(pDX, IDC_STATIC_WIZARD_COMMENT, m_staticWizardComment);
	DDX_Control(pDX, IDC_COMBO_TARGET_GROUP, m_comboTargetGroup);
	DDX_Check(pDX, IDC_CHECK_AUTO_SEARCH, m_bAutoSearch);
	DDX_Control(pDX, IDC_CHECK_AUTO_SEARCH, m_checkAutoSearch);
	DDX_Check(pDX, IDC_CHECK_ISSEARCH, m_bIsSearch);
	DDX_Control(pDX, IDC_CHECK_ISSEARCH, m_checkIsSearch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFishRepItemPropertyDlg, CCustomDlg)
	//{{AFX_MSG_MAP(CFishRepItemPropertyDlg)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_ERASEBKGND()
		ON_WM_CTLCOLOR()
		ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(ID_BTN_URLCHECK, OnButtonCheck)
	ON_BN_CLICKED(ID_BTN_CANCEL, OnCancel)
	ON_BN_CLICKED(ID_BTN_OK, OnButtonOk)
	ON_BN_CLICKED(ID_BTN_PREV, OnButtonPrev)
	ON_BN_CLICKED(ID_BTN_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_CHECK_ISSEARCH, OnCheckSearch)
	ON_MESSAGE(WM_XMLURL_FOUND, OnXmlUrlFound)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishRepItemPropertyDlg message handlers
const CString   CFishRepItemPropertyDlg::getTitle()          const
{
    return m_strTitle;
}


const CString   CFishRepItemPropertyDlg::getComment()        const
{
    return m_strComment;
}


const UINT  CFishRepItemPropertyDlg::getFeedInterval()   const
{	
	return m_nFeedInterval;
}

const UINT  CFishRepItemPropertyDlg::getTargetGroup()   const
{
	//int sel = m_comboTargetGroup.GetCurSel();	
    return m_nTargetGroup;
}


const CString   CFishRepItemPropertyDlg::getXmlUrl()            const
{
    return m_strXmlUrl;
}


const CString   CFishRepItemPropertyDlg::getHtmlUrl()          const
{
    return m_strHtmlUrl;
}


const CString   CFishRepItemPropertyDlg::getKeyword()          const
{
	return m_strKeyword;
}

const BOOL   CFishRepItemPropertyDlg::getIsSearch()          const
{
	return m_bIsSearch;
}

const BOOL   CFishRepItemPropertyDlg::getChangeSearch()          const
{
	return m_bChangeSearch;
}

void CFishRepItemPropertyDlg::setDlgType(int Type)
{
	m_nDlgType = Type;
}

BOOL CFishRepItemPropertyDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(LoadCursor(NULL, m_bThreadRunning ? IDC_WAIT : IDC_ARROW));

	return TRUE;
}

INT_PTR CFishRepItemPropertyDlg::DoModal(int Type, CString url)
{
    //////////////////////////////////////////////////////////////////////////
    // URL입력이 들어왔을 경우와 그렇지 않은 경우를 따로 작성해야한다.
    INT_PTR ret =   NULL;
    if ( url == _T("") )
    {
		m_nDlgType  =   Type;
        ret  =   CCustomDlg::DoModal();
    }
    else
    {
        m_nDlgType  =   CHANNEL_ADD_DLG;
        m_strXmlUrl =   url;
        ret  =   CCustomDlg::DoModal();
    }
    return ret;
}

void CFishRepItemPropertyDlg::OnButtonOk()
{
	UpdateData(TRUE);

	if(m_strTitle.GetLength() != 0 && m_nDlgType != CHANNEL_ADD_DLG && m_nDlgType != KEYWORD_ADD_DLG)
	{
		CCustomDlg::OnOK();

		if(m_nDlgType == CHANNEL_DLG || m_nDlgType == GROUP_DLG)
		{
			int sel = m_comboFeedInterval.GetCurSel();
			if(sel != -1)
				m_nFeedInterval = m_comboFeedInterval.GetItemData(sel);			
			else
				m_nFeedInterval = 30;
		}
	}

	else if(m_nDlgType == KEYWORD_ADD_DLG && !m_strTitle.IsEmpty() && !m_strKeyword.IsEmpty())
		CCustomDlg::OnOK();

	else if(m_strTitle.GetLength() != 0 && m_bUrlCheck == TRUE)
	{
		CCustomDlg::OnOK();

		if(m_nDlgType == CHANNEL_ADD_DLG)
		{
			int sel = m_comboFeedInterval.GetCurSel();
			if(sel != -1)
				m_nFeedInterval = m_comboFeedInterval.GetItemData(sel);
			else
				m_nFeedInterval = 30;
			
			sel = m_comboTargetGroup.GetCurSel();
			if(sel != -1)
				m_nTargetGroup = m_comboTargetGroup.GetItemData(sel);
			else
				m_nTargetGroup = m_comboTargetGroup.GetItemData(0);

		}
	}

	if(m_nDlgType == SCRAP_ADD_DLG || m_nDlgType == KEYWORD_ADD_DLG)
	{
		int sel = m_comboTargetGroup.GetCurSel();
		if(sel != -1)
			m_nTargetGroup = m_comboTargetGroup.GetItemData(sel);
		else
			m_nTargetGroup = m_comboTargetGroup.GetItemData(0);
	}
}

void CFishRepItemPropertyDlg::OnButtonPrev()
{
	if( m_nWizardStep > URL_INPUT_STEP )
	{
		if(!m_editXmlUrl.IsWindowEnabled())
			m_editXmlUrl.EnableWindow(TRUE);
		m_nWizardStep--;
		if(m_nWizardStep == URL_SELECTION_STEP && m_listFeedUrl.GetCount() == 1)
			m_nWizardStep--;
		ChangeWizardForm();
		if(m_nWizardStep == URL_INPUT_STEP)
		{
			m_strXmlUrl.Empty();
			m_listFeedUrl.ResetContent();
			m_btnPrev.EnableWindow(FALSE);
			m_checkAutoSearch.ShowWindow(TRUE);
		}
	}	
}

void CFishRepItemPropertyDlg::OnButtonNext()
{
	if( m_nWizardStep < URL_FINAL_STEP )
	{
		if(m_nWizardStep == URL_INPUT_STEP)
		{
			UpdateData(TRUE);

			
			m_editXmlUrl.EnableWindow(FALSE);
			m_btnNext.EnableWindow(FALSE);
			m_checkAutoSearch.EnableWindow(FALSE);

			m_hUrlChkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) UrlCheckThread, (LPVOID) this, CREATE_SUSPENDED, &m_dwUrlChkThreadId);
			m_bThreadRunning = TRUE;
			BeginWaitCursor();
			ResumeThread(m_hUrlChkThread);
		
			/*
			UpdateData(TRUE);
			if(!m_strXmlUrl.IsEmpty())
				OnButtonCheck();
			
			if(m_bUrlCheck == TRUE)
			{
				m_btnPrev.EnableWindow(TRUE);
				m_checkAutoSearch.ShowWindow(FALSE);
			}
			*/
			
		}
		else
		{
			m_nWizardStep++;		
			ChangeWizardForm();			
		}
	}	
}

void CFishRepItemPropertyDlg::OnOK()
{
	if(m_nWizardStep != 0)
	{
		OnButtonNext();
	}
	else
		OnButtonOk();

	
}

void CFishRepItemPropertyDlg::OnCancel()
{
	if(m_bThreadRunning == TRUE)
	{
		m_bThreadRunning = FALSE;
		WaitForSingleObject(m_hUrlChkThread, INFINITE);		
	}

	CCustomDlg::OnCancel();
}

void CFishRepItemPropertyDlg::OnButtonCheck()
{
	UpdateData(TRUE);

	BeginWaitCursor();

	list<CString> str_url_list;
	list<CString>::iterator it;
	int nErrCode = 0;

	if(!m_strXmlUrl.IsEmpty())
	{
		m_bUrlCheck = FALSE;

		
		CString	htmlstream;		

		if(m_strXmlUrl.Find(_T("http://")) == -1)
		{
			m_strXmlUrl = CString(_T("http://")) + m_strXmlUrl;
		}

		if(m_strXmlUrl.Find(_T("blog.naver.com")) > -1)
		{
			// 네이버 블로그의 경우 스크립트 사용으로 인해 html 문서에서 xml 주소 추출이 어려워
			// url을 파싱하여.. naver에서 제공하는 xml 주소로 변경
			int position = m_strXmlUrl.Find(_T("blog.naver.com/")) + _tcsclen(_T("blog.naver.com/"));
			int position2;
			CString temp_url, id;
			temp_url = _T("http://blog.rss.naver.com/");
			position2 = m_strXmlUrl.Find(_T("/"), position);

			if(position2 == -1)
			{
				position2 = m_strXmlUrl.Find(_T("."), position);
			}
			if(position2 == -1)
			{
				position2 = m_strXmlUrl.Find(_T("?"), position);
			}
			if(position2 == -1)
			{
				position2 = m_strXmlUrl.GetLength();
			}
			id = m_strXmlUrl.Mid(position, position2 - position);
			if(id.Find(_T(".")) == -1 && id.Find(_T("/")) == -1)
			{
				m_strXmlUrl = temp_url + id + _T(".xml");
			}
			
			//TRACE(url);
		}	
		
		if(m_xmlParser.LoadXML((LPTSTR)(LPCTSTR) m_strXmlUrl, CEzXMLParser::LOAD_WEB))
		{
			IXMLDOMNodeListPtr p_list;
			p_list = m_xmlParser.SearchNodes(_T("//*"));
			if(p_list != NULL)
			{					
				CString type = (LPCTSTR) p_list->item[0]->nodeName;
				if(type == _T("rdf:RDF") || type == _T("feed") || type == _T("rss"))
				{
					TRACE(_T("Find Rss Url : %s \n"), m_strXmlUrl);
					str_url_list.push_back(m_strXmlUrl);
				}
			}
		}
		else if(m_bAutoSearch)
		{			
			if(LoadHtml(m_strXmlUrl, htmlstream))
			{
				str_url_list = GetXmlUrlListInDocument(htmlstream, m_strXmlUrl, TRUE);
			}
			else
				nErrCode = 3;			
		}

		if(!str_url_list.empty())
		{			
			// Test Code ////////////////////////////////////////////////////////////
			it = str_url_list.begin();
			//m_strXmlUrl = *it;
			m_bUrlCheck = TRUE;

			CString test;
			//test.Format(_T("Find Xml Url : %d개\n"), str_url_list.size());
			//FishMessageBox(test);

			str_url_list.unique();

			for(it = str_url_list.begin(); it != str_url_list.end(); it++)
			{
				m_listFeedUrl.AddString(*it);
			}
			m_listFeedUrl.SetCurSel(0);

			if(m_listFeedUrl.GetCount() > 1)
				m_nWizardStep = URL_SELECTION_STEP;			
			else
				m_nWizardStep = URL_FINAL_STEP;
			
			ChangeWizardForm();

			UpdateData(FALSE);			
			////////////////////////////////////////////////////////////////////////
		}
		else if(nErrCode == 0 && !m_bAutoSearch)
			nErrCode = 1;
		else if(nErrCode == 0)
			nErrCode = 2;
	}	

	m_xmlParser.Clear();
	
	EndWaitCursor();

//	TRACE(_T("\nMessageBox : OPEN"));

	if(nErrCode == 2)
		FishMessageBox(STR_ERR_NOT_FOUND_AUTO, STR_ERR_TITLE);
	else if(nErrCode == 1)
		FishMessageBox(STR_ERR_XML_OR_XMLURL, STR_ERR_TITLE);
	else if(nErrCode == 3)
		FishMessageBox(STR_ERR_URL, STR_ERR_TITLE);


//	TRACE(_T("\nMessageBox : CLOSE"));

}

void CFishRepItemPropertyDlg::OnCheckSearch()
{
	if(m_checkIsSearch.GetButtonStyle() == BS_AUTO3STATE)
		m_checkIsSearch.ModifyStyle(BS_AUTO3STATE, BS_AUTOCHECKBOX);

	m_bChangeSearch = TRUE;	
}

void CFishRepItemPropertyDlg::SetMode(int type)
{
	CRect rect, btnRect;
	int i;	

	CWnd* pComponent[] = { (CWnd*)&m_staticTitle, (CWnd*)&m_editTitle,
						(CWnd*)&m_staticComment, (CWnd*)&m_editComment,
						(CWnd*)&m_staticFeedInterval, (CWnd*)&m_comboFeedInterval,
						(CWnd*)&m_staticHtmlUrl, (CWnd*)&m_editHtmlUrl,
						(CWnd*)&m_staticXmlUrl, (CWnd*)&m_editXmlUrl,
						(CWnd*)&m_staticKeyword, (CWnd*)&m_editKeyword,
						(CWnd*)&m_checkAutoSearch, (CWnd*)&m_staticTargetGroup,
						(CWnd*)&m_comboTargetGroup, (CWnd*)&m_staticWizardComment,
						(CWnd*)&m_listFeedUrl, (CWnd*)&m_checkIsSearch, NULL};

	GetParent()->GetWindowRect(rect);

	m_staticWizardComment.ShowWindow(FALSE);

	if(type == GROUP_DLG || type == GROUP_ADD_DLG)
	{
		if(type == GROUP_DLG)
		{
			SetWindowText(STR_EDIT_GROUP);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, GROUP_SIZE_Y + 30);
		}
		else
		{
			SetWindowText(STR_REGISTER_GROUP);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, GROUP_SIZE_Y);
		}

		for(i = 0; pComponent[i] != NULL; i++)
		{
			if(i < 4 || (type == GROUP_DLG && (i < 6 || i == 17)) )
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}			
			else
			{
				pComponent[i]->ShowWindow(FALSE);
				pComponent[i]->EnableWindow(FALSE);
			}

			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
			{
				((CEdit*)pComponent[i])->SetMargins(3, 3);
			}
		}

		pComponent[0]->SetWindowText(STR_GROUPNAME);
	}
	else if(type == CHANNEL_DLG)
	{
		SetWindowText(STR_EDIT_SUBSCRIBECHANNEL);

		for(i = 0; pComponent[i] != NULL; i++)
		{
			if(i < 10)
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else if(m_nWizardStep == URL_FINAL_STEP && (i == 13 || i == 14))
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}			
			else if(i == 17)
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}			
			else
			{
				pComponent[i]->ShowWindow(FALSE);
				pComponent[i]->EnableWindow(FALSE);
			}

			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
			{
				((CEdit*)pComponent[i])->SetMargins(3, 3);
			}
		}

		MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, CHANNEL_SIZE_Y);
	}
	else if(type == SCRAP_DLG || type == SCRAP_ADD_DLG)
	{
		if(type == SCRAP_DLG)
		{
			SetWindowText(STR_EDIT_SCRAPCHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SCRAP_SIZE_Y);
		}
		else
		{
			SetWindowText(STR_REGISTER_SCRAPCHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SCRAP_SIZE_Y + 30);
		}


		for(i = 0; pComponent[i] != NULL; i++)
		{
			if(i < 4)
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else if((type == SCRAP_ADD_DLG) && (i == 13 || i == 14))
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else
			{				
				pComponent[i]->ShowWindow(FALSE);
				pComponent[i]->EnableWindow(FALSE);
			}

			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
			{
				((CEdit*)pComponent[i])->SetMargins(3, 3);
			}
		}
		
	}
	else if(type == KEYWORD_DLG || type == SEARCH_DLG || type == KEYWORD_ADD_DLG ||
		type == SEARCH_ADD_DLG)
	{
		
		if(type == KEYWORD_DLG)
		{
			SetWindowText(STR_EDIT_KEYWORD_AUTOSRCH_CHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SEARCH_SIZE_Y);
		}
		else if(type == KEYWORD_ADD_DLG)
		{
			SetWindowText(STR_REGISTER_KEYWORD_AUTOSRCH_CHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SEARCH_SIZE_Y + 30);
		}
		else if(type == SEARCH_DLG)
		{
			SetWindowText(STR_EDIT_SRCH_CHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SEARCH_SIZE_Y);
		}
		else
		{
			SetWindowText(STR_REGISTER_SRCH_CHANNEL);
			MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, SEARCH_SIZE_Y + 30);
		}

		

		for(i = 0; pComponent[i] != NULL; i++)
		{
			if(i < 4)
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else if( i == 10 || i == 11)	// Keyword 입력폼
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else if((type == KEYWORD_ADD_DLG || type == SEARCH_ADD_DLG) && (i == 13 || i == 14))
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else
			{
				pComponent[i]->ShowWindow(FALSE);
				pComponent[i]->EnableWindow(FALSE);
			}

			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
			{
				((CEdit*)pComponent[i])->SetMargins(3, 3);
			}
		}		
	}
	else if(type == CHANNEL_ADD_DLG)
	{
		SetWindowText(STR_REGISTER_SUBSCRIBE_CHANNEL);

		m_nWizardStep = URL_INPUT_STEP;

		for(i = 0; pComponent[i] != NULL; i++)
		{
			if(i == 8 || i == 9 || i == 12 || i == 15)
			{
				pComponent[i]->ShowWindow(TRUE);
				pComponent[i]->EnableWindow(TRUE);
			}
			else
			{
				pComponent[i]->ShowWindow(FALSE);
				pComponent[i]->EnableWindow(FALSE);
			}

			pComponent[8]->SetWindowText(_T("URL"));

			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
			{
				((CEdit*)pComponent[i])->SetMargins(3, 3);
			}
		}

		MoveWindow(rect.left + 200, rect.top + 150, DLG_SIZE_X, CHANNEL_SIZE_Y);
	}

	// Component Position arrange
	{		
		CRect rect, c_rect;
		GetClientRect(rect);
		int pos_x, pos_y, visible_cnt = 0, edit_width;

		pos_x = MARGIN_LEFT + STATIC_WIDTH + COMPONENT_GAP;
		edit_width = rect.right - MARGIN_LEFT - MARGIN_RIGHT - STATIC_WIDTH - COMPONENT_GAP;

		int cnt = 0;
		if(type != CHANNEL_ADD_DLG || (type == CHANNEL_ADD_DLG && m_nWizardStep == URL_FINAL_STEP))
		{
			for(i = 0;  pComponent[i] != NULL; i++)
			{
				if(pComponent[i]->IsWindowEnabled())
				{
					if(type != CHANNEL_DLG && type != CHANNEL_ADD_DLG)
					{
						if(cnt%2 == 0)
						{
							pos_y = CONTENTS_TOP + EDIT_HEIGHT + (cnt/2 *(EDIT_HEIGHT + COMPONENT_GAP + EDIT_MARGIN_TOP));
							pComponent[i]->MoveWindow(MARGIN_LEFT, pos_y, STATIC_WIDTH, STATIC_HEIGHT);

							if(i == 17 && type == GROUP_DLG)
							{
								pComponent[5]->GetWindowRect(c_rect);
								ScreenToClient(c_rect);
								pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP * 4, c_rect.top + 3, c_rect.Width() * 2, EDIT_HEIGHT);
								
								pComponent[i]->ModifyStyle(BS_AUTOCHECKBOX, BS_AUTO3STATE);
								((CButton*)pComponent[i])->SetCheck(BST_INDETERMINATE);
							}
						}
						else
						{
							pComponent[i - 1]->GetWindowRect(c_rect);
							ScreenToClient(c_rect);
							if(i != 14 && !(type == GROUP_DLG && i == 5))
							{
								pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top, edit_width, EDIT_HEIGHT);
								((CEdit*)pComponent[i])->SetMargins(3, 3);
							}
							else if(type == GROUP_DLG && i == 5)
							{
								pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top - 3, FEEDINTERVAL_EDIT_WIDTH, EDIT_HEIGHT);
							}
							else
							{
								pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top - 4, edit_width + 2, EDIT_HEIGHT);
							}

							if(i == 11 && (type == KEYWORD_DLG || type == SEARCH_DLG))
							{
								pComponent[i]->EnableWindow(FALSE);
							}
							
						}
						pComponent[i]->SetFont(&m_DefaultFont);
						cnt++;
					}
					else if(type == CHANNEL_DLG)
					{
						if(i != 13 && i != 14)
						{
							if(cnt%2 == 0)
							{
								pos_y = CONTENTS_TOP + EDIT_HEIGHT + (cnt/2 *(EDIT_HEIGHT + COMPONENT_GAP + EDIT_MARGIN_TOP));
								pComponent[i]->MoveWindow(MARGIN_LEFT, pos_y, STATIC_WIDTH,STATIC_HEIGHT);
							}
							else
							{
								pComponent[i - 1]->GetWindowRect(c_rect);
								ScreenToClient(c_rect);
								if(i != 5)
									pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top, edit_width, EDIT_HEIGHT);
								else
									pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top - 4, 100, EDIT_HEIGHT);							

								

								/*
								if(i == 9)
									pComponent[i]->EnableWindow(FALSE);
									*/

								if(i < 12)
									((CEdit*)pComponent[i])->SetMargins(3, 3);								
							}

							if(i == 17)
							{
									pComponent[9]->GetWindowRect(c_rect);
									ScreenToClient(c_rect);
									pComponent[i]->MoveWindow(c_rect.left, c_rect.bottom + COMPONENT_GAP, c_rect.Width(), EDIT_HEIGHT);
							}
						}
						else
						{
							/*
							if(i == 12)
							{
								pComponent[5]->GetWindowRect(c_rect);
								ScreenToClient(c_rect);
								pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top, 14, STATIC_HEIGHT);
							}
							*/

							if(m_nWizardStep == URL_FINAL_STEP)
							{
								if(i == 13)
								{
									pComponent[5]->GetWindowRect(c_rect);
									ScreenToClient(c_rect);
									pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP * 2, c_rect.top + 4, 60, STATIC_HEIGHT);
									pComponent[i]->ShowWindow(TRUE);
								}
								else if(i == 14)
								{
									pComponent[13]->GetWindowRect(c_rect);
									ScreenToClient(c_rect);
									pComponent[i]->MoveWindow(c_rect.right + COMPONENT_GAP, c_rect.top - 4, rect.right - MARGIN_RIGHT - c_rect.right - COMPONENT_GAP + 1, EDIT_HEIGHT);
									pComponent[i]->ShowWindow(TRUE);
								}
							}
						}
						cnt++;
						pComponent[i]->SetFont(&m_DefaultFont);
					}				
				}
			}
		}
		else
		{
			m_staticWizardComment.MoveWindow(MARGIN_LEFT + 10, CONTENTS_TOP + 10,
					rect.Width() - MARGIN_LEFT - MARGIN_RIGHT, WIZARD_COMMENT_HEIGHT);

			m_staticWizardComment.GetWindowRect(c_rect);
			ScreenToClient(c_rect);
			m_staticXmlUrl.MoveWindow(MARGIN_LEFT, c_rect.bottom + 10, STATIC_WIDTH - 40, EDIT_HEIGHT);
			m_staticXmlUrl.SetFont(&m_BoldFont);
			m_editXmlUrl.MoveWindow(pos_x - 40, c_rect.bottom + 10, edit_width + 40, EDIT_HEIGHT);			
			m_editXmlUrl.SetFont(&m_DefaultFont);

			m_editXmlUrl.GetWindowRect(c_rect);
			ScreenToClient(c_rect);
			m_checkAutoSearch.MoveWindow(c_rect.left, c_rect.bottom + COMPONENT_GAP, 200, STATIC_HEIGHT);			
			m_checkAutoSearch.SetFont(&m_DefaultFont);			
		}			


		// Button
		GetClientRect(rect);
		m_btnOk.GetClientRect(c_rect);
		
		if(type != CHANNEL_ADD_DLG && m_nWizardStep == 0)
		{
			m_btnOk.MoveWindow((rect.right/2) - c_rect.right - 5, rect.bottom - c_rect.bottom - MARGIN_BOTTOM, c_rect.right, c_rect.bottom);
			m_btnCancel.MoveWindow((rect.right/2) + 5, rect.bottom - c_rect.bottom - MARGIN_BOTTOM, c_rect.right, c_rect.bottom);
		}
		else
		{
			if(m_nWizardStep == URL_INPUT_STEP)
			{
				m_staticWizardComment.SetWindowText(WIZARD_URL_INTPUT_T);
				m_btnPrev.EnableWindow(FALSE);
			}

			m_staticWizardComment.SetFont(&m_DefaultFont);

		//	m_btnUrlCheck.ShowWindow(FALSE);
			m_btnPrev.ShowWindow(TRUE);
			m_btnNext.ShowWindow(TRUE);

			m_btnNext.MoveWindow((rect.right - BTN_WIDTH)/2, rect.bottom - c_rect.bottom - MARGIN_BOTTOM, c_rect.right, c_rect.bottom);
			m_btnNext.GetWindowRect(c_rect);
			ScreenToClient(c_rect);
			m_btnCancel.MoveWindow(c_rect.right + 10, c_rect.top, c_rect.Width(), c_rect.Height());
			m_btnPrev.MoveWindow(c_rect.left - c_rect.Width() - 10, c_rect.top, c_rect.Width(), c_rect.Height());
			m_btnOk.MoveWindow(c_rect.left, c_rect.top, c_rect.Width(), c_rect.Height());

			if(m_nWizardStep != URL_FINAL_STEP)
				m_btnOk.ShowWindow(FALSE);
			else
			{				
				m_btnOk.ShowWindow(TRUE);
				m_btnNext.ShowWindow(FALSE);
				m_btnNext.GetWindowRect(btnRect);
				ScreenToClient(btnRect);

				m_btnOk.MoveWindow(btnRect);
			}
		}

		RedrawWindow();
	}
}


BOOL CFishRepItemPropertyDlg::OnInitDialog()
{
	//SetSubTitleImage(IDB_MAINFRAME_TOPLEFT);

	CCustomDlg::OnInitDialog();	

	m_hBrushBkgnd = CreateSolidBrush(RGB(248, 248, 248));
	m_hBrushEditBg = CreateSolidBrush(RGB(255, 255, 255));
	m_penDotline.CreatePen(PS_DOT, 1, RGB(213, 213, 213));

	m_DefaultFont.CreatePointFont(90, DEFAULT_APPLICATION_FONT);
	m_BoldFont.CreateFont(14, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT);

	m_listFeedUrl.SetItemGap(1, 0);
	m_listFeedUrl.SetColors(RGB(147, 179, 216), RGB(255, 255, 255), RGB(255, 255, 255), RGB(65, 65, 65), RGB(255, 255, 255), RGB(225, 225, 225));
	m_listFeedUrl.SetFont(&m_DefaultFont);
	//m_listMenu.SetFontNormal(&m_fontNormal);
	//m_listMenu.SetFontSelected(&m_fontBold);
	m_listFeedUrl.SetItemHeight(0, 20);
	m_listFeedUrl.SetMargins(4, 4);
	m_listFeedUrl.SetListFill(TRUE);

	CRect rectBtn(0, 0, BTN_WIDTH, BTN_HEIGHT);	

	// Button 초기화.....
	m_btnOk.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rectBtn, this, ID_BTN_OK);
	m_btnOk.LoadBitmaps(IDB_BTN_OK_N, IDB_BTN_OK_D, IDB_BTN_OK_O, IDB_BTN_OK_DISABLE, BTN_WIDTH, BTN_HEIGHT);

	m_btnCancel.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rectBtn, this, ID_BTN_CANCEL);
	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_N, IDB_BTN_CANCEL_D, IDB_BTN_CANCEL_O, IDB_BTN_CANCEL_DISABLE, BTN_WIDTH, BTN_HEIGHT);

	m_btnPrev.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rectBtn, this, ID_BTN_PREV);
	m_btnPrev.LoadBitmaps(IDB_BTN_PREV_N, IDB_BTN_PREV_D, IDB_BTN_PREV_O, IDB_BTN_PREV_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnPrev.ShowWindow(FALSE);

	m_btnNext.Create(_T(""), WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_FLAT|WS_TABSTOP, rectBtn, this, ID_BTN_NEXT);
	m_btnNext.LoadBitmaps(IDB_BTN_NEXT_N, IDB_BTN_NEXT_D, IDB_BTN_NEXT_O, IDB_BTN_NEXT_DISABLE, BTN_WIDTH, BTN_HEIGHT);
	m_btnNext.ShowWindow(FALSE);

	//Subtitle Bitmap Load
	{
		if(m_nDlgType == CHANNEL_ADD_DLG || m_nDlgType == CHANNEL_DLG)
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_CHANNEL);		
		}		
		else if(m_nDlgType == GROUP_ADD_DLG || m_nDlgType == GROUP_DLG)
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_GROUP);		
		}		
		else if(m_nDlgType == SCRAP_ADD_DLG || m_nDlgType == SCRAP_DLG)
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_SCRAP);		
		}		
		else if(m_nDlgType == SEARCH_ADD_DLG || m_nDlgType == SEARCH_DLG)
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_KEYWORD);
		}		
		else if(m_nDlgType == KEYWORD_ADD_DLG || m_nDlgType == KEYWORD_DLG)
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_KEYWORD);		
		}
		else
		{
			m_bmpIcon.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_ICON_GROUP);
		}
		m_bmpIcon.GetBitmap(&m_bitmapIcon);
		m_bmpBox.LoadBitmap(IDB_REPOSITORY_PROPERTYDLG_BOX);
		m_bmpBox.GetBitmap(&m_bitmapBox);
	}
	

	UpdateData(TRUE);
	SetMode(m_nDlgType);

	CRect client_rect;
	GetClientRect(client_rect);

	m_rectSubTitle = client_rect;
	m_rectSubTitle.right -= MARGIN_RIGHT;
	m_rectSubTitle.top += MARGIN_TOP;
	m_rectSubTitle.bottom = m_rectSubTitle.top + m_bitmapIcon.bmHeight;
	m_rectSubTitle.left += (MARGIN_LEFT + m_bitmapIcon.bmWidth + ICON_MARGIN_RIGHT);
	//m_btnClose.GetClientRect(rectBtn);

	
	m_comboFeedInterval.AddString(STR_5MIN);
	m_comboFeedInterval.AddString(STR_15MIN);
	m_comboFeedInterval.AddString(STR_30MIN);
	m_comboFeedInterval.AddString(STR_1HOUR);
	m_comboFeedInterval.AddString(STR_2HOUR);
	m_comboFeedInterval.AddString(STR_NOUPDATE);
	m_comboFeedInterval.SetItemData(0, 5);
	m_comboFeedInterval.SetItemData(1, 15);
	m_comboFeedInterval.SetItemData(2, 30);
	m_comboFeedInterval.SetItemData(3, 60);
	m_comboFeedInterval.SetItemData(4, 120);
	m_comboFeedInterval.SetItemData(5, 0);

	if(m_nDlgType == GROUP_DLG)
	{
		m_comboFeedInterval.AddString(_T(""));
		m_comboFeedInterval.SetItemData(6, -1);
	}
	
	if(m_nDlgType == GROUP_DLG && m_nFeedInterval == -1)
	{
		m_comboFeedInterval.SetCurSel(6);
	}
	else if(m_nFeedInterval == 0)
	{
		m_comboFeedInterval.SetCurSel(5);
	}
	else if(m_nFeedInterval == 5)
	{
		m_comboFeedInterval.SetCurSel(0);
	}
	else if(m_nFeedInterval == 15)
	{
		m_comboFeedInterval.SetCurSel(1);
	}
	else if(m_nFeedInterval == 30)
	{
		m_comboFeedInterval.SetCurSel(2);
	}
	else if(m_nFeedInterval == 60)
	{
		m_comboFeedInterval.SetCurSel(3);
	}
	else
	{
		m_comboFeedInterval.SetCurSel(4);
	}

	if(m_nDlgType == CHANNEL_ADD_DLG)
	{
		m_comboFeedInterval.SetCurSel(2);
	}
	
	
	if((((CDlg_Subscribe*)m_pSubscribe) != NULL) && (m_nDlgType == CHANNEL_ADD_DLG || m_nDlgType == SCRAP_ADD_DLG || m_nDlgType == KEYWORD_ADD_DLG))
	{		
		HTREEITEM htreeitem = ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetRootItem();
		
        int idxSel  =   0;// added by eternalbleu
		for(int i = 0; htreeitem != NULL; htreeitem = ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetNextSiblingItem(htreeitem), i++)
		{
            // added by eternalbleu
            HTREEITEM hSelectItem   =   ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetSelectedItem();
            HTREEITEM hParentItem   =   ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetParentItem(hSelectItem);
            if ( hParentItem != NULL && htreeitem  == hParentItem )   idxSel  =   i;
            else    if ( hParentItem == NULL && htreeitem  == hSelectItem )   idxSel  =   i;

			m_comboTargetGroup.AddString( ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetItemText(htreeitem));			
			m_comboTargetGroup.SetItemData(i, ((CDlg_Subscribe*)m_pSubscribe)->m_treeRepository.GetItemData(htreeitem));
		}
		m_comboTargetGroup.SetCurSel(idxSel);   // modified by eternalbleu
	}

	if(m_btnCancel.m_hWnd) m_btnCancel.SetFlatFocus(TRUE);
	if(m_btnOk.m_hWnd) m_btnOk.SetFlatFocus(TRUE);
	if(m_btnPrev.m_hWnd) m_btnPrev.SetFlatFocus(TRUE);
	if(m_btnNext.m_hWnd) m_btnNext.SetFlatFocus(TRUE);

	//m_btnClose.MoveWindow(client_rect.right - rectBtn.right - 8, 8, rectBtn.right, rectBtn.bottom);

	return TRUE;
}
void CFishRepItemPropertyDlg::SetSubscribeDlg(CDlg_Subscribe* psubscribe)
{
	m_pSubscribe = psubscribe;
}

void CFishRepItemPropertyDlg::DrawItems(CDC *pDC)
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

void CFishRepItemPropertyDlg::DrawOutline(CDC* pBufferDC, CDC* pMemDC)
{
	CCustomDlg::DrawOutline(pBufferDC, pMemDC);
}

void CFishRepItemPropertyDlg::DrawBg(CDC *pBufferDC, CDC *pMemDC)
{	
	CCustomDlg::DrawBg(pBufferDC, pMemDC);
	
	CRect client_rect, edit_rect;
	CWnd* pComponents[] = {&m_editTitle, &m_editComment, &m_editKeyword, &m_editHtmlUrl,
		&m_editXmlUrl, NULL};
	
	// EditBox Outline Draw ...
	CPen outlinePen;
	outlinePen.CreatePen(PS_SOLID, 1, RGB(185, 185, 185));
	CPen* oldPen;

	for(int i=0; pComponents[i] != NULL; i++)
	{
		if(!pComponents[i]->IsWindowVisible())
			continue;
		
		GetClientRect(client_rect);
		pComponents[i]->GetWindowRect(edit_rect);
		ScreenToClient(edit_rect);

		if(i != 5)
			edit_rect.top -= EDIT_MARGIN_TOP;

		edit_rect.InflateRect(1, 1, 1, 1);		
		
		oldPen = pBufferDC->SelectObject(&outlinePen);
		pBufferDC->Rectangle(&edit_rect);
		pBufferDC->SelectObject(oldPen);
	}

	
	if(m_listFeedUrl.IsWindowVisible())
	{
		m_listFeedUrl.GetWindowRect(edit_rect);
		ScreenToClient(edit_rect);
		edit_rect.InflateRect(1, 1, 1, 1);

		oldPen = pBufferDC->SelectObject(&outlinePen);
		pBufferDC->Rectangle(&edit_rect);
		pBufferDC->SelectObject(oldPen);
	}
	
	

	GetClientRect(client_rect);

		
	CBitmap* pOldBmp;
	
	//SubTitle Icon
	pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpIcon);
	pBufferDC->BitBlt(MARGIN_LEFT, MARGIN_TOP, m_bitmapIcon.bmWidth, m_bitmapIcon.bmHeight, pMemDC,
		0, 0, SRCCOPY);
	pMemDC->SelectObject(pOldBmp);

	oldPen = (CPen*)pBufferDC->SelectObject(&m_penDotline);
	pBufferDC->MoveTo(MARGIN_LEFT, MARGIN_TOP + m_bitmapIcon.bmHeight + ICON_MARGIN_BOTTOM);
	pBufferDC->LineTo(client_rect.right - MARGIN_RIGHT, MARGIN_TOP + m_bitmapIcon.bmHeight + ICON_MARGIN_BOTTOM);	

	if(m_btnCancel.m_hWnd)
	{
		m_btnCancel.GetWindowRect(edit_rect);
		ScreenToClient(edit_rect);
		pBufferDC->MoveTo(MARGIN_LEFT, edit_rect.top - MARGIN_BOTTOM);
		pBufferDC->LineTo(client_rect.right - MARGIN_RIGHT, edit_rect.top - MARGIN_BOTTOM);
	}

	pBufferDC->SelectObject(oldPen);

	CFont* pOldFont = (CFont*)pBufferDC->SelectObject(&m_BoldFont);
	pBufferDC->SetBkColor(TRANSPARENT);
	CRect rect = m_rectSubTitle;

	if(m_nDlgType == GROUP_ADD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_NEW_GROUP, m_rectSubTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_NEW_GROUP, rect, DT_LEFT | DT_CALCRECT);

		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_CALCRECT);

		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if(m_nDlgType == GROUP_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_GROUP_INFO, m_rectSubTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_GROUP_INFO, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_MODIFY, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_MODIFY, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if(m_nDlgType == CHANNEL_ADD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_NEW_CHANNEL, m_rectSubTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_NEW_CHANNEL, rect, DT_LEFT | DT_CALCRECT);

		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_CALCRECT);

		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}	
	else if(m_nDlgType == SCRAP_ADD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_NEW_SCRAP, m_rectSubTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_NEW_SCRAP, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_SINGLELINE| DT_VCENTER);
	}	
	else if(m_nDlgType == SEARCH_ADD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_NEW_SEARCH, m_rectSubTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		pBufferDC->DrawText(STR_NEW_SEARCH, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}	
	else if(m_nDlgType == KEYWORD_ADD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_NEW_KEYWORD, m_rectSubTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		pBufferDC->DrawText(STR_NEW_KEYWORD, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_WORD_REG, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if(m_nDlgType == CHANNEL_DLG || m_nDlgType == SCRAP_DLG || m_nDlgType == SEARCH_DLG || m_nDlgType == KEYWORD_DLG)
	{
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(STR_CHANNEL_INFO, m_rectSubTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pBufferDC->DrawText(STR_CHANNEL_INFO, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(220, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_WORD_MODIFY, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		pBufferDC->DrawText(STR_WORD_MODIFY, rect, DT_LEFT | DT_CALCRECT);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		rect.left = rect.right;
		rect.right = m_rectSubTitle.right;
		rect.bottom = m_rectSubTitle.bottom;
		pBufferDC->DrawText(STR_QUESTION, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	
	pBufferDC->SelectObject(pOldFont);


	if(m_nDlgType == CHANNEL_ADD_DLG && (m_nWizardStep == URL_INPUT_STEP || m_nWizardStep == URL_SELECTION_STEP))
	{
		m_staticWizardComment.GetWindowRect(edit_rect);
		ScreenToClient(edit_rect);
		pOldBmp = (CBitmap*)pMemDC->SelectObject(&m_bmpBox);
		pBufferDC->BitBlt(MARGIN_LEFT, edit_rect.top + 4, m_bitmapBox.bmWidth, m_bitmapBox.bmHeight,
			pMemDC, 0, 0, SRCCOPY);
		pMemDC->SelectObject(pOldBmp);
	}

	if(m_nWizardStep == URL_SELECTION_STEP)
	{
		m_staticWizardComment.GetWindowRect(edit_rect);
		ScreenToClient(edit_rect);
		m_staticWizardComment.ShowWindow(FALSE);

		CFont* pOldFont;
		CString msg;
		CSize textsize;
		msg.Format(_T("%d"), m_listFeedUrl.GetCount());		
		pOldFont = (CFont*)pBufferDC->SelectObject(&m_BoldFont);
		pBufferDC->SetTextColor(RGB(244, 0, 0));
		pBufferDC->DrawText(msg, edit_rect, DT_LEFT);
		textsize = pBufferDC->GetTextExtent(msg);
		pMemDC->SelectObject(pOldFont);

		pOldFont = (CFont*)pBufferDC->SelectObject(&m_DefaultFont);
		edit_rect.left += textsize.cx;
		msg = STR_FIND_MSG1;
		pBufferDC->SetTextColor(RGB(0, 0, 0));	
		pBufferDC->DrawText(msg, edit_rect, DT_LEFT);
		textsize = pBufferDC->GetTextExtent(msg);
		pMemDC->SelectObject(pOldFont);

		msg = STR_FIND_MSG2;		
		edit_rect.left += textsize.cx;
		pOldFont = (CFont*)pBufferDC->SelectObject(&m_DefaultFont);
		pBufferDC->SetTextColor(RGB(0, 0, 0));
		pBufferDC->DrawText(msg, edit_rect, DT_LEFT);
		pMemDC->SelectObject(pOldFont);
	}
}


void CFishRepItemPropertyDlg::DeleteObjects()
{
	
	if( m_bmpIcon.m_hObject )
		m_bmpIcon.DeleteObject();
	if( m_bmpBox.m_hObject )
		m_bmpBox.DeleteObject();
	if( m_DefaultFont.m_hObject )
		m_DefaultFont.DeleteObject();
	if( m_BoldFont.m_hObject )
		m_BoldFont.DeleteObject();

	CCustomDlg::DeleteObjects();	
}

BOOL CFishRepItemPropertyDlg::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bDrawBySelf)
	{
		DrawItems(pDC);
		return TRUE;
	}	

	return CCustomDlg::OnEraseBkgnd(pDC);
}

HBRUSH CFishRepItemPropertyDlg::OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(248, 248, 248));

		if(m_nWizardStep == URL_INPUT_STEP && pWnd == (CWnd*)&m_staticXmlUrl)
			pDC->SetTextColor(RGB(0, 0, 0));
		else
			pDC->SetTextColor(RGB(0, 0, 0));

		hbr = m_hBrushBkgnd;
	}
	else if(nCtlColor == CTLCOLOR_EDIT)
	{
		if(pWnd != (CWnd*)&m_editTitle)
		{
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextColor(RGB(130, 130, 130));
			hbr = m_hBrushEditBg;
		}
	}
	
	return hbr;	
}


void CFishRepItemPropertyDlg::ChangeWizardForm()
{
	CRect client_rect;

	CWnd* editlist[] = {&m_editTitle, &m_editComment, &m_comboFeedInterval, &m_editHtmlUrl,
		&m_editXmlUrl, &m_editKeyword};
	CStatic* staticlist[] = {&m_staticTitle, &m_staticComment, &m_staticFeedInterval, &m_staticHtmlUrl,
		&m_staticXmlUrl, &m_staticKeyword};

	m_btnOk.ShowWindow(FALSE);
	m_btnNext.ShowWindow(TRUE);

	if(m_nWizardStep == URL_INPUT_STEP)
	{
		SetMode(CHANNEL_ADD_DLG);
		m_staticXmlUrl.SetFont(&m_BoldFont);
	}
	else if(m_nWizardStep == URL_SELECTION_STEP)
	{
		for(int i = 0; i < 6; i++)
		{
			// Font Setting...
			{
				editlist[i]->ShowWindow(FALSE);
				staticlist[i]->ShowWindow(FALSE);
				//if(i == 5)
				//	staticlist[i+1]->ShowWindow(FALSE);
			}

			GetClientRect(client_rect);
						
			m_listFeedUrl.MoveWindow(MARGIN_LEFT, CONTENTS_TOP + WIZARD_COMMENT_HEIGHT + 11, client_rect.Width() - MARGIN_LEFT * 2,
				client_rect.Height() - CONTENTS_TOP - MARGIN_BOTTOM - WIZARD_COMMENT_HEIGHT - 60);
			m_listFeedUrl.ShowWindow(TRUE);
			m_listFeedUrl.EnableWindow(TRUE);
		}
		CString comment;
		comment.Format(WIZARD_URL_SELECT_T, m_listFeedUrl.GetCount());
		m_staticTargetGroup.ShowWindow(FALSE);
		m_comboTargetGroup.ShowWindow(FALSE);
		m_staticWizardComment.SetWindowText(comment);
		m_staticWizardComment.ShowWindow(TRUE);	
		m_checkIsSearch.ShowWindow(FALSE);
		//RedrawWindow();
	}
	else if(m_nWizardStep == URL_FINAL_STEP)
	{
		m_listFeedUrl.GetText(m_listFeedUrl.GetCurSel(), m_strXmlUrl);
		
		if(m_xmlParser.LoadXML((LPTSTR)(LPCTSTR)m_strXmlUrl, CEzXMLParser::LOAD_WEB))
		{
			GetChannelInfo(m_xmlParser, m_strTitle, m_strHtmlUrl);
            m_strTitle  =   TranslateString( m_strTitle );
		}
		m_staticXmlUrl.SetWindowText(STATIC_XML_URL_T);
		m_staticXmlUrl.SetFont(&m_DefaultFont);
		UpdateData(FALSE);
		SetMode(CHANNEL_DLG);
	}
	Invalidate();
}


/*
 * UrlCheckThread

 * written by aquarelle

 * [history]
 * created 2006-03-20
 
 * [Description]
 * Url이 rss feed 주소인지 검사

 * [Parameters] 
 * (in LPVOID) param - CFishRepItemPropertyDlg의 포인터.
 
 * [Return]
  
 */
DWORD CFishRepItemPropertyDlg::UrlCheckThread(LPVOID param)
{
	

	CFishRepItemPropertyDlg* pWnd = (CFishRepItemPropertyDlg*)param;
	//pWnd->BeginWaitCursor();


	CString htmlStream;
	CString url;
	list<CString> url_list;
	list<CString>::iterator it, next_location;
	CEzXMLParser xml_parser;
	BOOL* bContinue = NULL;
	BOOL bFound = FALSE;
	bContinue = &(pWnd->m_bThreadRunning);
	url = pWnd->m_strXmlUrl;
	int nErrCode = 0;

	CoInitialize(NULL);

	
	if(url.Find(_T("http://")) == -1)
	{
		url = CString(_T("http://")) + url;
	}

	if(url.Find(_T("blog.naver.com")) > -1)
	{
		// 네이버 블로그의 경우 스크립트 사용으로 인해 html 문서에서 xml 주소 추출이 어려워
		// url을 파싱하여.. naver에서 제공하는 xml 주소로 변경
		int position = url.Find(_T("blog.naver.com/")) + _tcsclen(_T("blog.naver.com/"));
		int position2;
		CString temp_url, id;
		temp_url = _T("http://blog.rss.naver.com/");
		position2 = url.Find(_T("/"), position);

		if(position2 == -1)
		{
			position2 = url.Find(_T("."), position);
		}
		if(position2 == -1)
		{
			position2 = url.Find(_T("?"), position);
		}
		if(position2 == -1)
		{
			position2 = url.GetLength();
		}
		id = url.Mid(position, position2 - position);
		if(id.Find(_T(".")) == -1 && id.Find(_T("/")) == -1)
		{
			url = temp_url + id + _T(".xml");
		}
		
		//TRACE(url);
	}	
	
	if(xml_parser.LoadXML((LPTSTR)(LPCTSTR) url, CEzXMLParser::LOAD_WEB))
	{
		IXMLDOMNodeListPtr p_list;
		p_list = xml_parser.SearchNodes(_T("//*"));
		if(p_list != NULL)
		{
			CString type = (LPCTSTR) p_list->item[0]->nodeName;
			if(type == _T("rdf:RDF") || type == _T("feed") || type == _T("rss"))
			{
				TRACE(_T("Find Rss Url : %s \n"), url);					
				::SendMessage(pWnd->GetSafeHwnd(), WM_XMLURL_FOUND, (WPARAM) (LPCTSTR) url, NULL);
				bFound = TRUE;
			}
		}
	}
	
	if(!bFound && pWnd->m_bAutoSearch)
	{
		if(LoadHtml(url, htmlStream))
		{
			url_list = GetUrlListInDocument(htmlStream, url, TRUE);

			//it = url_list.begin();
			TRACE(_T("Found Url : %d\r\n"), url_list.size());
			
			for(it = url_list.begin(); *bContinue && it != url_list.end(); it++)
			{
				//next_location = it;
				//next_location++;
				//TRACE(str_url);
				//TRACE(_T("\n"));

				// 검색시간 단축을 위해..
				// url에 rss 나 atom이 들어가 있지 않은 링크는 확인 하지 않음....			
				if(!UrlCheck( (*it) ))
				{
					//url_list.erase(it);			
					continue;
				}

				if((*it).Right(4) == _T(".xml"))
				{
					// 절대 경로가 아닐경우 절대경로 URL 변경...
					// 절대경로로 변경된 URL이 정확한 URL이 아닐수도 있다....
					if((*it).Left(7) != _T("http://"))
					{						
						CString temp(url);				
						if(temp.Right(1) == _T('/'))
							temp = temp.Left(temp.GetLength() - 1);
						int position;
						if((*it).Left(1) == _T("/"))
						{
							position  = temp.Find(_T("/"), 8);
							*it = temp.Left(position) + (*it);					
						}
						else
						{
							position = temp.ReverseFind(_T('/'));

							if(position < 7 )
							{						
								if(temp.Right(1) == _T('/'))						
									*it = temp + (*it);						
								else
									*it = temp + _T("/") + (*it);
							}
							else
								*it = temp.Left(position + 1) + (*it);
						}
					}
				}

				// URL에서 XML을 로드하여 RSS 파일인지 검색
				try
				{
					TRACE(_T("\nLoading (URL): %s\n"), (*it));
					if(xml_parser.LoadXML((LPTSTR)(LPCTSTR) (*it), CEzXMLParser::LOAD_WEB))
					{
						TRACE((*it) + _T("\n"));
						IXMLDOMNodeListPtr p_list;
						p_list = xml_parser.SearchNodes(_T("//*"));
						if(p_list != NULL)
						{					
							CString type = (LPCTSTR) p_list->item[0]->nodeName;
							if(type != _T("rdf:RDF") && type != _T("feed") && type != _T("rss"))
							{						
								//url_list.erase(it);
							}
							else
							{
								TRACE(_T("\nFind Rss Url : %s \n"), *it);						
								::SendMessage(pWnd->GetSafeHwnd(), WM_XMLURL_FOUND, (WPARAM) (LPCTSTR)(*it), NULL);
							}
						}
					}
				}
				catch(...)
				{
					TRACE(_T("XML Load Fail (URL): %s"), *it);
				//	url_list.erase(it);
				}
				xml_parser.Clear();
				xml_parser.Init();
			}			
		}
		else
		{
			nErrCode = 3;				
		}
	}
	
	if(*bContinue)
	{
		int count = pWnd->m_listFeedUrl.GetCount();
		if(count == 0 && nErrCode == 0 && !pWnd->m_bAutoSearch)
			nErrCode = 1;
		else if(count == 0 && nErrCode == 0)
			nErrCode = 2;
		
		::SendMessage(pWnd->GetSafeHwnd(), WM_XMLURL_FOUND, NULL, (LPARAM) nErrCode);
	}

	CoUninitialize();
	//pWnd->EndWaitCursor();

	return 0;
}

void CFishRepItemPropertyDlg::OnXmlUrlFound(WPARAM wParam, LPARAM lParam)
{
	if(wParam != NULL)
	{
		if(m_listFeedUrl.FindString(0, (LPCTSTR) wParam) == LB_ERR)
			m_listFeedUrl.AddString((LPCTSTR) wParam);
	}
	else
	{
		m_editXmlUrl.EnableWindow(TRUE);
		m_btnNext.EnableWindow(TRUE);
		m_checkAutoSearch.EnableWindow(TRUE);
		m_bThreadRunning = FALSE;
		EndWaitCursor();

		if(m_listFeedUrl.GetCount() > 0)
		{
			m_checkAutoSearch.ShowWindow(FALSE);
			if(m_listFeedUrl.GetCount() > 1)
				m_nWizardStep = URL_SELECTION_STEP;			
			else
				m_nWizardStep = URL_FINAL_STEP;

			m_listFeedUrl.SetCurSel(0);
			m_btnPrev.EnableWindow(TRUE);
			m_bUrlCheck = TRUE;
			ChangeWizardForm();
			UpdateData(FALSE);
		}		
		else
		{
			int nErrCode = (int) lParam;

			if(nErrCode == 2)
				FishMessageBox(STR_ERR_NOT_FOUND_AUTO, STR_ERR_TITLE);
			else if(nErrCode == 1)
				FishMessageBox(STR_ERR_XML_OR_XMLURL, STR_ERR_TITLE);
			else if(nErrCode == 3)
				FishMessageBox(STR_ERR_URL, STR_ERR_TITLE);
		}		
	}
}


/*
 * LoadHtml

 * written by aquarelle

 * [history]
 * created 2006-01-04
 
 * [Description]
 * 해당 URL이 RSS 문서의 URL인 가능성 체크 (필터링?)

 * [Parameters] 
 * (in CString) str_url - 체크할 URL
 
 * [Return]
 * TRUE : RSS 문서의 URL일 가능성 높음.
 * FALSE : RSS 문서의 URL일 가능성 낮음.
 
 */
BOOL UrlCheck(CString str_url)
{
	str_url.MakeLower();

//	TRACE(_T("%s\n"), str_url);

	BOOL result = FALSE;

	if(str_url.Find(_T("rss")) > -1)
		result = TRUE;
	
	if(str_url.Find(_T("atom")) > -1)
		result = TRUE;

	if(str_url.Find(_T("feed")) > -1)
		result = TRUE;

	if(str_url.Find(_T("rdf")) > -1)
		result = TRUE;

	if(str_url.Find(_T("xml")) > -1)
		result = TRUE;

	if(str_url.Right(4) == _T(".xml"))
		result = TRUE;	
	else if(result != TRUE)
	{
		int position = str_url.ReverseFind(_T('/'));
		int position2 = str_url.ReverseFind(_T('.'));
		
		if( position2 > -1 && position < position2)
		{
			if(str_url.GetLength() - position2 - 1 <= 3)
				result = FALSE;
		}
	}

	if(str_url.Right(4) == _T(".exe"))
		result = FALSE;

	if(str_url.Find(_T("mailto:")) > -1)
		result = FALSE;

	return result;
}


/*
 * LoadHtml

 * written by aquarelle

 * [history]
 * created 2006-01-04
 
 * [Description]
 * 해당 URL부터 HTML 문서를 로드함.

 * [Parameters] 
 * (in CString) url - 로드할 HTML URL
 * (out CString&) htmlstream - 반환할 HTML Stream
 
 * [Return]
 * TRUE : HTML 문서 로드 성공
 * FALSE : HTML 문서 로드 실패
 
 */
BOOL LoadHtml(CString url, CString& htmlstream)
{
	IStream* pStream;
	char* result;
	BOOL b_result;

	if(URLOpenBlockingStream( 0, url, &pStream, 0, 0) != S_OK)
		return FALSE;

	result = (char*)malloc(BUFFERSIZE);
	if(result == NULL)
		return FALSE;

	ZeroMemory(result, BUFFERSIZE);
	
	DWORD dwGot;
	
	char buffer[5000];

	HRESULT hr;
	ZeroMemory(buffer, sizeof(buffer));

	while(SUCCEEDED(hr = pStream->Read(buffer, sizeof(buffer), &dwGot)) && hr != S_FALSE)
	{
		// Do something with contents of buffer. 
		if(BUFFERSIZE <= (strlen(result) + dwGot - 1))
		{
			char* temp;
			int newallocsize = BUFFERSIZE + strlen(result) + dwGot * 2;
			temp = (char*)malloc(newallocsize);
			if(temp == NULL)
			{
				free(result);
				return FALSE;
			}
			ZeroMemory(temp, newallocsize);
			strncat(temp, result, strlen(result));
			free(result);
			result = temp;
		}
		strncat(result, buffer, strlen(buffer));
		ZeroMemory(buffer, sizeof(buffer));
	}

#ifdef _UNICODE
	BSTR str_unicode;
	//int uniLen = MultiByteToWideChar(CP_ACP, 0, Result, strlen(Result), NULL, NULL);
	result[strlen(result)] = '\0';

	int uniLen = MultiByteToWideChar(CP_ACP, 0, result, strlen(result), NULL, NULL);
	str_unicode = SysAllocStringLen(NULL, uniLen + 1);

	ZeroMemory(str_unicode, uniLen + 1);
	
	MultiByteToWideChar(CP_ACP, 0, result, -1, str_unicode, uniLen + 1);

	_bstr_t bsData(str_unicode);
	
	SysFreeString(str_unicode);
	free(result);
	
	htmlstream = (LPTSTR) bsData;

#else
	htmlstream = result;
#endif

	htmlstream.TrimLeft();
	htmlstream.TrimRight();

	if(htmlstream.IsEmpty())
		b_result = FALSE;
	else
		b_result = TRUE;

	return b_result;
}

/*
 * GetXmlUrlListInDocument

 * written by aquarelle

 * [history]
 * created 2006-01-04
 
 * [Description]
 * 로드한 HTML 문서 내에서 RSS 주소들을 찾아냄.
 * 프레임있는 경우 각 프레임의 HTML 내용 까지 검색... (bFindFrame 플래그 TRUE일 경우)

 * [Parameters] 
 * (in CStirng&) htmlstream - Xml주소를 찾을 HTML stream
 * (in CString) doc_url - HTML 문서의 URL
 * (in BOOL) bFindFrame - 프레임의 문서까지 찾을지 여부
 
 * [Return]
 * list<CString> : 찾아낸 RSS 파일의 주소리스트
 
 */
list<CString> GetXmlUrlListInDocument(CString& htmlstream, CString doc_url, BOOL bFindFrame)
{
	list<CString> xml_url_list, temp_list;
	int position = 0, position2 = 0;
	int length = htmlstream.GetLength();
	CString linkurl;
	list<CString>::iterator it;

	//htmlstream.MakeLower();	

	xml_url_list = FindTagAttributeValues(htmlstream, _T("a"), _T("href"));
	RemoveNonXmlUrl(xml_url_list, doc_url);

	if(xml_url_list.empty())
	{
		xml_url_list = FindTagAttributeValues(htmlstream, _T("link"), _T("href"));		
		RemoveNonXmlUrl(xml_url_list, doc_url);
	}	
		

	if(xml_url_list.empty())
	{
		xml_url_list = FindTagAttributeValues(htmlstream, _T("a"), _T("title"));
		RemoveNonXmlUrl(xml_url_list, doc_url);
	}
	
	if(xml_url_list.empty() && bFindFrame)
	{
		CString FindFrame[] = {_T("frame"), _T("iframe")};
		
		for(int index = 0; index < 2; index++)
		{
			temp_list = FindTagAttributeValues(htmlstream, FindFrame[index], _T("src"));
			if(temp_list.empty())
				continue;
			
			CString str_url;

			TRACE(_T("TAG COUNT : %d\n"), temp_list.size());

			for(it = temp_list.begin(); it != temp_list.end(); it++)
			{				
				str_url = *it;
				
				if(str_url.Left(7) != _T("http://"))
				{
					CString url = doc_url;				
					
					if(!url.IsEmpty())
					{
						CString temp = url;
						int position;
						if(temp.Right(1) == _T('/'))
							temp = temp.Left(temp.GetLength() - 1);

						if(str_url.Left(1) == _T("/"))
						{
							position  = temp.Find(_T("/"), 8);
							str_url = temp.Left(position) + str_url;					
						}
						else
						{
							position = temp.ReverseFind(_T('/'));
							if(position < 7)
							{
								//str_url = temp + _T("/") + str_url;

								if(temp.Right(1) == _T('/'))						
									str_url  = temp + str_url ;						
								else
									str_url  = temp + _T("/") + str_url;
							}
							else
								str_url = temp.Left(position + 1) + str_url;
						}
					}
				}

				//TRACE(str_url + _T("\n"));

				CString frame_html;
				
				if(LoadHtml(str_url, frame_html))
				{
					list<CString> frame_list;
					list<CString>::iterator frame_it;

					frame_list = GetXmlUrlListInDocument(frame_html, str_url, FALSE);
					
					for(frame_it = frame_list.begin(); frame_it != frame_list.end(); frame_it++)
					{
						xml_url_list.push_back(*frame_it);
					}						
				}
				str_url.Empty();
			}			
		}
	}

	return xml_url_list;
}


/*
 * FindTagAttributeValues

 * written by aquarelle

 * [history]
 * created 2006-01-04
 
 * [Description]
 * HTML 문서 내에서 지정된 태그의 속성값들을 찾아냄. 

 * [Parameters] 
 * (in CStirng&) htmlstream - 속성값 찾을 HTML stream
 * (in CString) tagname - 찾을 태그
 * (in CString) attribute - 찾을 속성
 
 * [Return]
 * list<CString> : 찾아낸 태그의 속성값 리스트
 
 */
list<CString> FindTagAttributeValues(CString& htmlstream, CString tagname, CString attribute)
{
	list<CString> findvaluelist;

	CString searchTag = _T("<");
	CString linkurl;
	searchTag += tagname;

	CString Buffer = htmlstream;
	Buffer.MakeLower();

	int position = 0, position2 = 0, position_close = 0, old_position = 0;
	int length = htmlstream.GetLength();

	
	do
	{
		old_position = position;
		if(position == 0)
			position = Buffer.Find(searchTag, 0);			
		else
			position = Buffer.Find(searchTag, position + 1);
			

		if(position > -1)
		{
			position = Buffer.Find(attribute, position + 1);
			position = Buffer.Find(_T("="), position + 1);
			position_close = Buffer.Find(_T(">"), position + 1);
						
			if(position < position_close && position_close > -1)
			{				
				if(Buffer.GetAt(position+1) == _T(' ') || Buffer.GetAt(position+1) == _T('\t'))
				{
					position++;
					while(Buffer.GetAt(position) != _T(' ') && Buffer.GetAt(position) != _T('\t') && position< position_close)
					{
						position++;
					}
					position--;
				}
				else if(Buffer.GetAt(position+1) == _T('\'') || Buffer.GetAt(position+1) == _T('"'))
				{
					position++;
				}
				
				if(Buffer.GetAt(position) == _T('\''))
				{
					position2 = Buffer.Find(_T("\'"), position + 1);
				}
				else if(Buffer.GetAt(position) == _T('\"'))
				{
					position2 = Buffer.Find(_T('\"'), position + 1);
				}
				else
				{
					position2 = Buffer.Find(_T(" "), position + 1);
				}

			}
			else
			{
				position = position_close;
				continue;
			}

			if(position2 > position && position != -1 && position2 != -1 && position && position2 < position_close)
			{
				linkurl = htmlstream.Left(position2);
				linkurl = linkurl.Right(position2 - position - 1);

				linkurl.TrimLeft();
				linkurl.TrimRight();

			//	TRACE(linkurl + _T("\n"));
									
				if(!linkurl.IsEmpty())
					findvaluelist.push_back(linkurl);
				
				linkurl.Empty();
			}
			else
			{
				position = position_close;
				continue;
			}
		}
	}while(position != -1 && position < length && position > old_position);

	return findvaluelist;
}


/*
 * RemoveNonXmlUrl

 * written by aquarelle

 * [history]
 * created 2006-01-04
 
 * [Description]
 * URL 리스트에서 xml url이 아닌 URL 제거

 * [Parameters] 
 * (in/out list<CString>&) url_list - 제거대상 URL 리스트
 * (in CString) doc_url - URL들이 포함되어있던 HTML문서의 URL
 
 * [Return]
 * TRUE : xml url이 하나라도 남아 있는 경우
 * FALSE : xml url이 없어 전부 제거 되었을 경우
 
 */
BOOL RemoveNonXmlUrl(list<CString>& url_list, CString doc_url)
{
	list<CString>::iterator it, next_location;	
	CString url;
	CEzXMLParser xml_parser;

	TRACE(_T("\nRemove Befor: %d"), url_list.size());

	for(it = url_list.begin(); it != url_list.end(); it = next_location )
	{
		next_location = it;
		next_location++;
		//TRACE(str_url);
		//TRACE(_T("\n"));

		// 검색시간 단축을 위해..
		// url에 rss 나 atom이 들어가 있지 않은 링크는 확인 하지 않음....			
		if(!UrlCheck( (*it) ))
		{
			url_list.erase(it);			
			continue;
		}

		if((*it).Right(4) == _T(".xml"))
		{
			// 절대 경로가 아닐경우 절대경로 URL 변경...
			// 절대경로로 변경된 URL이 정확한 URL이 아닐수도 있다....
			if((*it).Left(7) != _T("http://"))
			{						
				CString temp(doc_url);				
				if(temp.Right(1) == _T('/'))
					temp = temp.Left(temp.GetLength() - 1);
				int position;
				if((*it).Left(1) == _T("/"))
				{
					position  = temp.Find(_T("/"), 8);
					*it = temp.Left(position) + (*it);					
				}
				else
				{
					position = temp.ReverseFind(_T('/'));

					if(position < 7 )
					{						
						if(temp.Right(1) == _T('/'))						
							*it = temp + (*it);						
						else
							*it = temp + _T("/") + (*it);
					}
					else
						*it = temp.Left(position + 1) + (*it);
				}
			}
		}

			// URL에서 XML을 로드하여 RSS 파일인지 검색
		try
		{
			TRACE(_T("\nLoading (URL): %s\n"), (*it));
			if(xml_parser.LoadXML((LPTSTR)(LPCTSTR) (*it), CEzXMLParser::LOAD_WEB))
			{
				TRACE((*it) + _T("\n"));
				IXMLDOMNodeListPtr p_list;
				p_list = xml_parser.SearchNodes(_T("//*"));
				if(p_list != NULL)
				{					
					CString type = (LPCTSTR) p_list->item[0]->nodeName;
					if(type != _T("rdf:RDF") && type != _T("feed") && type != _T("rss"))
					{						
						url_list.erase(it);
					}
					else
						TRACE(_T("\nFind Rss Url : %s \n"), *it);
				}
				else
					url_list.erase(it);
			}
			else
			{
				url_list.erase(it);
			}
		}
		catch(...)
		{
			TRACE(_T("XML Load Fail (URL): %s"), *it);
			url_list.erase(it);
		}
		xml_parser.Clear();
		xml_parser.Init();
	}		
	TRACE(_T("Remove After: %d\n"), url_list.size());

	if(url_list.empty())
		return FALSE;
	else
		return TRUE;
}


BOOL GetChannelInfo(CEzXMLParser& parser, CString& title, CString& htmlurl)
{
	IXMLDOMNodeListPtr p_list;
	IXMLDOMNodePtr p_node, p_node_in;	

	p_list = parser.SearchNodes(_T("//*"));	
	if(p_list != NULL)
	{
		p_node = p_list->item[0];
		CString type = (LPCTSTR) p_node->nodeName;
		if(type == _T("rdf:RDF"))
		{
			p_list = parser.SearchNodes(_T("//*[name()='channel']"));
			p_node = p_list->item[0];
			
			title =  GetNamedNodeText(XML_TITLE_F, &parser, p_node, 1);
			htmlurl = GetNamedNodeText(XML_LINK_F, &parser, p_node, 1);
		}
		else if(type == _T("feed"))
		{			
			title = GetNamedNodeText(XML_TITLE_F, &parser, p_node, 1);			
			
			p_list = parser.SearchNodes(p_node, XML_LINK_F);
		
			for(int i = 0; p_list->item[i] != NULL; i++)
			{
				p_node_in = p_list->item[i];

				if(p_node_in->parentNode != p_node)
					break;

				//	URL
				if(GetNamedNodeText(XML_REL, &parser, p_node_in, 0) == XML_TYPE_ALTERNATE)
				{			
					htmlurl	= GetNamedNodeText(XML_HREF, &parser, p_node_in, 0);
				}
			}
		}
		else if(type == _T("rss"))
		{
			p_list = parser.SearchNodes(p_node, XML_CHANNEL);
			p_node = p_list->item[0];
			title = GetNamedNodeText(XML_TITLE, &parser, p_node, 1);
			htmlurl = GetNamedNodeText(XML_LINK, &parser, p_node, 1);			
		//	TRACE(_T("\nTiltle: %s\nUrl: %s\n"), GetNamedNodeText(XML_TITLE, &parser, p_node, 1), htmlurl);
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}


CString CFishRepItemPropertyDlg::TranslateString(CString str)
{
    CString ret =   str;
    ret.Replace( _T("&quote;"), _T("\"") );
    ret.Replace( _T("&quot;"), _T("\"") );
    ret.Replace( _T("&lt;"), _T("<") );
    ret.Replace( _T("&gt;"), _T(">") );
    ret.Replace( _T("&amp;"), _T("&") );
    ret.Replace( _T("&nbsp;"), _T(" ") );

    ret.Replace( _T("&trade;"), _T("™") );
    ret.Replace( _T("&sect;"), _T("§") );
    ret.Replace( _T("&aelig;"), _T("æ") );
    ret.Replace( _T("&times;"), _T("×") );

    ret.Replace( _T("&#033;"), _T("!") );
    ret.Replace( _T("&#039;"), _T("'") );
    ret.Replace( _T("&#042;"), _T("*") );
    ret.Replace( _T("&#061;"), _T("=") );

    ret.Replace( _T("&#33;"), _T("!") );
    ret.Replace( _T("&#39;"), _T("'") );
    ret.Replace( _T("&#42;"), _T("*") );
    ret.Replace( _T("&#61;"), _T("=") );

    ret.Replace( _T("&#131;"), _T("?") );
    ret.Replace( _T("&#134;"), _T("†") );
    ret.Replace( _T("&#159;"), _T("?") );
    ret.Replace( _T("&#161;"), _T("¡") );
    ret.Replace( _T("&#162;"), _T("￠") );
    ret.Replace( _T("&#164;"), _T("¤") );
    ret.Replace( _T("&#191;"), _T("¿") );
    
    ret.Replace( _T("<b>"), _T("") );
    ret.Replace( _T("</b>"), _T("") );
    ret.Replace( _T("<u>"), _T("") );
    ret.Replace( _T("</u>"), _T("") );
    ret.Replace( _T("<i>"), _T("") );
    ret.Replace( _T("</i>"), _T("") );
    return ret;
}



list<CString> GetUrlListInDocument(CString& htmlstream, CString doc_url, BOOL bFindFrame)
{
	list<CString> xml_url_list, temp_list;
	int position = 0, position2 = 0;
	int length = htmlstream.GetLength();
	CString linkurl;
	list<CString>::iterator it;

	//htmlstream.MakeLower();	

	xml_url_list = FindTagAttributeValues(htmlstream, _T("a"), _T("href"));
	
	temp_list = FindTagAttributeValues(htmlstream, _T("link"), _T("href"));
	for(it = temp_list.begin(); it != temp_list.end(); it++)
	{
		xml_url_list.push_back(*it);
	}

	temp_list = FindTagAttributeValues(htmlstream, _T("a"), _T("title"));
	for(it = temp_list.begin(); it != temp_list.end(); it++)
	{
		xml_url_list.push_back(*it);
	}
	
	if(xml_url_list.empty() && bFindFrame)
	{
		CString FindFrame[] = {_T("frame"), _T("iframe")};
		
		for(int index = 0; index < 2; index++)
		{
			temp_list = FindTagAttributeValues(htmlstream, FindFrame[index], _T("src"));
			if(temp_list.empty())
				continue;
			
			CString str_url;

			TRACE(_T("TAG COUNT : %d\n"), temp_list.size());

			for(it = temp_list.begin(); it != temp_list.end(); it++)
			{				
				str_url = *it;
				
				if(str_url.Left(7) != _T("http://"))
				{
					CString url = doc_url;				
					
					if(!url.IsEmpty())
					{
						CString temp = url;
						int position;
						if(temp.Right(1) == _T('/'))
							temp = temp.Left(temp.GetLength() - 1);

						if(str_url.Left(1) == _T("/"))
						{
							position  = temp.Find(_T("/"), 8);
							str_url = temp.Left(position) + str_url;					
						}
						else
						{
							position = temp.ReverseFind(_T('/'));
							if(position < 7)
							{
								//str_url = temp + _T("/") + str_url;

								if(temp.Right(1) == _T('/'))						
									str_url  = temp + str_url ;						
								else
									str_url  = temp + _T("/") + str_url;
							}
							else
								str_url = temp.Left(position + 1) + str_url;
						}
					}
				}

				//TRACE(str_url + _T("\n"));

				CString frame_html;
				
				if(LoadHtml(str_url, frame_html))
				{
					list<CString> frame_list;
					list<CString>::iterator frame_it;

					frame_list = GetUrlListInDocument(frame_html, str_url, FALSE);
					
					for(frame_it = frame_list.begin(); frame_it != frame_list.end(); frame_it++)
					{
						xml_url_list.push_back(*frame_it);
					}						
				}
				str_url.Empty();
			}			
		}
	}

	xml_url_list.unique();

	return xml_url_list;
}