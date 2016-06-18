// FISH.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FISH.h"

#include "MainFrm.h"
#include "FISHDoc.h"
#include "View_Information.H"
//#include <initguid.h>

#include "RSSDB.h"
#include "LocalProperties.h"
#include "./GnuModule/GnuDoc.h"
#include "RSSCurrentList.h"
#include "SearchManager.h"
#include "FishLinkManager.h"
#include "OPMLSock.h"
#include "fish_common.h"
#include "fish_def.h"
#include "URL.h"
#include "Dlg_MessageBox.h"

#include "./util/ExceptionHandler.h"
#include "FocusManager.h"

//#include "afxpriv.h"
#include <..\src\occimpl.h>
#include "CustSite.h"

#include "XML/EzXMLParser.h"

#include "./util/GlobalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_APP_FAILTOSTART				_T("구동 실패!! 프로그램을 종료합니다.")
#define STR_APP_OSVERSIONFAIL			_T("FISH는 윈도우 95, 98, ME 버전을 지원하지 않습니다.")
#define STR_APP_NEEDMSXML4				_T("프로그램 실행에 필요한 정보 파일이 모두 설치되지 않았습니다.\n\n") \
										_T(" # 설치 되지 않은 항목 : MSXML4\n\n")
//										_T("확인을 누르시면 프로그램을 종료하고 업데이트 페이지로 이동합니다.\n")

extern MSJExceptionHandler*	g_MSJExceptionHandler;

const static UINT UWM_ARE_YOU_FISH=RegisterWindowMessage(FISH_GUID);
/////////////////////////////////////////////////////////////////////////////
// CFISHApp

BEGIN_MESSAGE_MAP(CFISHApp, CWinApp)
	//{{AFX_MSG_MAP(CFISHApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_FOCUSON_ADDR_COMBO, OnFocusOnAddrCombo)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFISHApp construction

CFISHApp::CFISHApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bClosing		= FALSE;
	m_bExit			= FALSE;
	pendinglink		= NULL;
	m_hmaininst		= NULL;

	m_ntotalpostcnt = 0;
	m_nnetpostcnt	= 0;
	m_nnetusercnt	= 0;

	m_pDispOM		= NULL;

	pMgr			= NULL;

	g_MSJExceptionHandler = NULL;
}

CFISHApp::~CFISHApp()
{
	TRACE(_T("CFISHApp Destroy\r\n"));
	if(pendinglink) delete pendinglink;

	if( g_MSJExceptionHandler != NULL )
	{
		delete g_MSJExceptionHandler;
		g_MSJExceptionHandler = NULL;
	}

	if(m_pDispOM) delete m_pDispOM;

	if(pMgr) delete pMgr;

	::CloseHandle(m_hMutexOneInstance);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFISHApp object

CFISHApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFISHApp initialization

//#define INFORMATION_FILE					_T(".\\FishInform.exe")

BOOL CFISHApp::InitInstance()
{
	//
/*	if(GetOSVersionType() < 4)
	{
		CFileFind f;
		if(f.FindFile(INFORMATION_FILE))
		{
			ShellExecute(NULL, _T("open"), INFORMATION_FILE, NULL, NULL, SW_SHOW);
		}
		return FALSE;
	}
*/
	// check for MSXML4.dll registration
	CEzXMLParser parser;

	CString str = _T("");
	str.Empty();
	str = XML_DECLARATION;
	str += XML_ROOT_DECLARATION;

	if(!parser.LoadXML((LPTSTR)(LPCTSTR)str, CEzXMLParser::LOAD_STRING))
	{
//		if( IDOK == AfxMessageBox(STR_APP_NEEDMSXML4) )
        if( IDOK == FishMessageBox(STR_APP_NEEDMSXML4) )
		{
//			CURL url;
//			url.Open(REGISTER_PAGE);
		}

		return FALSE;
	}

	if (!AfxSocketInit())
	{
//		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        CString msg;
        msg.LoadString( IDP_SOCKETS_INIT_FAILED );
        FishMessageBox( (LPCTSTR)msg );
		return FALSE;
	}
	CoInitialize(NULL);

	pMgr = new CCustomOccManager;

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);

//	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("rss-reader-FISH"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_bRunAleady = CheckRun();

	SendArgumentToAnother();		

    //////////////////////////////////////////////////////////////////////////
    // FISH 링크 레지스트리 등록 과정
    CFishLinkManager* mgr   =   CFishLinkManager::getInstance();
    mgr->Ask4RegFix(false, true);
    mgr->RevertReg();

	// CreateExceptionHandler
	m_spLP = SmartPtr<CLocalProperties> (new CLocalProperties);	// 제일 먼저 생성해야 함

	m_spRD = SmartPtr<CRSSDB> (new CRSSDB);
	m_spGD = SmartPtr<CGnuDoc> (new CGnuDoc);
	m_spCL = SmartPtr<CRSSCurrentList> (new CRSSCurrentList);
	m_spSM = SmartPtr<CSearchManager> (new CSearchManager);
	m_spOS = SmartPtr<COPMLSock> (new COPMLSock);

	g_MSJExceptionHandler = new MSJExceptionHandler;
	if( g_MSJExceptionHandler )
	{
		CTime time(CTime::GetCurrentTime());
		CString fileName;
		fileName.Format(_T("Log\\%02d%02d_exception.log"), time.GetMonth(), time.GetDay() );
		// Process 실행 경로 설정.
		TCHAR String[512];
//		GetModuleFileName(NULL,String,256);
#ifdef _UNICODE
		_tcscpy(String, __wargv[0]);
#else
		_tcscpy(String, __argv[0]);
#endif

		int len=_tcslen(String);
		for (int i=len-1;i>0;i--) {	if (String[i]=='\\') {String[i]=0;break;}  }
		SetCurrentDirectory(String);
		CreateDirectory(_T("Log"), NULL);
		g_MSJExceptionHandler->SetLogFileName((PTSTR)(LPCTSTR)fileName);
	}

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFISHDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CView_Information)
		);
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}

    // Dispatch commands specified on the command line
	if (!ProcessCommandline(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFISHApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFISHApp message handlers

int CFISHApp::ExitInstance()
{
	if(m_spRD != NULL)
		m_spRD->Release();

	CoUninitialize();

	CFocusManager::Release();

	return CWinApp::ExitInstance();
}

void CFISHApp::SetClosing(BOOL bclose)
{
	m_bClosing = bclose;
}

BOOL CFISHApp::IsClosing()
{
	return m_bClosing;
}

BOOL CFISHApp::IsExiting()
{
	return m_bExit;
}

BOOL CALLBACK CFISHApp::SearchFishWindow(HWND hWnd, LPARAM lParam)
{
	DWORD dwMsgResult;
	LRESULT res = ::SendMessageTimeout(hWnd, UWM_ARE_YOU_FISH, 0, 0, SMTO_BLOCK|SMTO_ABORTIFHUNG, 10000, &dwMsgResult);

	if(res == 0)
		return TRUE;
	if(dwMsgResult == UWM_ARE_YOU_FISH)
	{ 
		HWND * target = (HWND *)lParam;
		*target = hWnd;
		return FALSE; 
	}

	return TRUE; 
} 

/**************************************************************************
 * method CFISHApp::OnAreYou
 *
 * written by moonknit
 *
 * @History
 * created 2005-07-06
 * updated 2005-12-14 :: FISH용으로 변경
 *
 * @Description
 * 외부에서 Fish버전 확인을 휘한 메시지를 보내올 경우 응답 하기 위한 이벤트 함수
 **************************************************************************/
bool CFISHApp::CheckRun()
{
	// Check Running Already Algorithm from Emule
	CString strMutextName;
	strMutextName.Format(_T("%s"), FISH_GUID);
	m_hMutexOneInstance = ::CreateMutex(NULL, FALSE, strMutextName);
	
	m_hmaininst = NULL;

	bool bAlreadyRunning = false;
	bAlreadyRunning = ( ::GetLastError() == ERROR_ALREADY_EXISTS ||::GetLastError() == ERROR_ACCESS_DENIED);
   	if ( bAlreadyRunning ) EnumWindows(SearchFishWindow, (LPARAM)&m_hmaininst);

	return bAlreadyRunning;
}

/************************************************************************
SendArgumentToAnother
@PARAM  : 
@RETURN : 
@REMARK : 
    CString 에서 4바이트로 잡는 듯 보임.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/09:CREATED
************************************************************************/
void CFISHApp::SendArgumentToAnother()
{
	CString command, tmp;
	for(int i = 1; i<__argc; ++i)
	{
#ifdef _UNICODE
		tmp = __wargv[i];
#else
		tmp = __argv[i];
#endif
		command += tmp;
//		command += FISHDATA_DIV;	// 구분자 (ESC)
	}


#ifdef _UNICODE
	sendstruct.cbData = 2 * (command.GetLength() + 1) ;       // 유니코드를 MFC 내부에서 이상변환하기 때문에 2배를 해야함. 데이터 영역의 바이트임
#else
	sendstruct.cbData = (command.GetLength() + 1) ;       // 유니코드를 MFC 내부에서 이상변환하기 때문에 2배를 해야함. 데이터 영역의 바이트임
#endif
	sendstruct.dwData = OP_FISHDATA; 

    TCHAR*  ptmpStr     =   new TCHAR[sendstruct.cbData+1];
    memcpy(ptmpStr, command.GetBuffer( command.GetLength() + 1 ), sendstruct.cbData);
	
    sendstruct.lpData = ptmpStr;
    if (m_bRunAleady && m_hmaininst)
	{
      	SendMessage(m_hmaininst, WM_COPYDATA, (WPARAM)0, (LPARAM)(PCOPYDATASTRUCT) &sendstruct); 
		Sleep(10);		// wait
		m_bExit = TRUE;
        delete ptmpStr;
		exit(0);
	} 
    else 
	{
      	pendinglink = new CString(command);
	}
    
    delete ptmpStr;
}

/************************************************************************
ProcessCommandline  커맨드 명령어를 처리한다. 또한 fish:// 와 같은 접두어를 처리한다.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/09:CREATED
************************************************************************/
#define MAX_COMMANDLINE     1024
BOOL CFISHApp::ProcessCommandline(CCommandLineInfo& rCmdInfo)
{
    BOOL bResult = TRUE;
	switch (rCmdInfo.m_nShellCommand)
	{
	case CCommandLineInfo::FileNew:
		if (!AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL))
			OnFileNew();
		if (m_pMainWnd == NULL)
			bResult = FALSE;
		break;

		// If we've been asked to open a file, call OpenDocumentFile()

	case CCommandLineInfo::FileOpen:
        //////////////////////////////////////////////////////////////////////////
        // remark by eternalbleu
        // dialog와 다르게 mainframe 은 기본적으로 초기생성시 OnCmdMsg(ID_FILE_NEW...)를 호출해야 생성이 된다. 따라서 
        // FISH 와 같은 방식의 구조와 동작방식에서는 FileOpen 시 mainframe 이 없는 상태이기 때문에 이런 후처리 과정이 필요하다.
        if (m_pDocManager != NULL)
            m_pDocManager->OnFileNew();
        if (m_pMainWnd == NULL)
            bResult = FALSE;

        //////////////////////////////////////////////////////////////////////////
        // FISH 데이터 전달함.
        if (rCmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen) {
            CString command = rCmdInfo.m_strFileName;
            if (command.Find( _T("://") )>0) {
                sendstruct.cbData = command.GetLength()+1; 
                sendstruct.dwData = OP_FISHLINK; 
                sendstruct.lpData = command.GetBuffer(MAX_COMMANDLINE); 
                if (m_hmaininst){
                    SendMessage(m_pMainWnd->GetSafeHwnd(),WM_COPYDATA,(WPARAM)0,(LPARAM) (PCOPYDATASTRUCT) &sendstruct); 
                    return true; 
                } 
                else 
                    pendinglink = new CString(command);
            } else {
                sendstruct.cbData = command.GetLength()+1; 
                sendstruct.dwData = OP_CLCOMMAND;
                sendstruct.lpData = command.GetBuffer(MAX_COMMANDLINE); 
                if (m_hmaininst){
                    SendMessage(m_pMainWnd->GetSafeHwnd(),WM_COPYDATA,(WPARAM)0,(LPARAM) (PCOPYDATASTRUCT) &sendstruct); 
                    return true; 
                }
            }
        }
        break;

		// If the user wanted to print, hide our main window and
		// fire a message to ourselves to start the printing

	case CCommandLineInfo::FilePrintTo:
	case CCommandLineInfo::FilePrint:
		m_nCmdShow = SW_HIDE;
		ASSERT(m_pCmdInfo == NULL);
		OpenDocumentFile(rCmdInfo.m_strFileName);
		m_pCmdInfo = &rCmdInfo;
		m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
		m_pCmdInfo = NULL;
		bResult = FALSE;
		break;

		// If we're doing DDE, hide ourselves

	case CCommandLineInfo::FileDDE:
		m_pCmdInfo = (CCommandLineInfo*)m_nCmdShow;
		m_nCmdShow = SW_HIDE;
		break;

	// If we've been asked to unregister, unregister and then terminate
	case CCommandLineInfo::AppUnregister:
		{
			UnregisterShellFileTypes();
			BOOL bUnregistered = Unregister();

			// if you specify /EMBEDDED, we won't make an success/failure box
			// this use of /EMBEDDED is not related to OLE

			if (!rCmdInfo.m_bRunEmbedded)
			{
				if (bUnregistered)
                {
//					AfxMessageBox(AFX_IDP_UNREG_DONE);
                    CString msg;
                    msg.LoadString( AFX_IDP_UNREG_DONE );
                    FishMessageBox( (LPCTSTR)msg );
                }
				else
                {
//					AfxMessageBox(AFX_IDP_UNREG_FAILURE);
                    CString msg;
                    msg.LoadString( AFX_IDP_UNREG_FAILURE );
                    FishMessageBox( (LPCTSTR)msg );
                }
			}
			bResult = FALSE;    // that's all we do

			// If nobody is using it already, we can use it.
			// We'll flag that we're unregistering and not save our state
			// on the way out. This new object gets deleted by the
			// app object destructor.

			if (m_pCmdInfo == NULL)
			{
				m_pCmdInfo = new CCommandLineInfo;
				m_pCmdInfo->m_nShellCommand = CCommandLineInfo::AppUnregister;
			}
		}
		break;
	}

	// FOR TEST
	if(!bResult)
//		AfxMessageBox(STR_APP_FAILTOSTART);
        FishMessageBox(STR_APP_FAILTOSTART);


	return bResult;
}

afx_msg void CFISHApp::OnFocusOnAddrCombo()
{
//    ::MessageBox(m_pMainWnd->GetSafeHwnd(), _T("TEST"), NULL, MB_OK);
}