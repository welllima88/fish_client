// TagManager.cpp: implementation of the CTagManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "TagManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HANDLE CTagManager::m_hThread = NULL;
HANDLE CTagManager::m_hThreadEvent = NULL;
BOOL CTagManager::m_bThread = FALSE;

int CTagManager::m_nInit = 0;

CTagManager theTags;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTagManager::CTagManager()
{
	InitStatic();
}

CTagManager::~CTagManager()
{
	DeinitStatic();
}

/**************************************************************************
 * CTagManager::InitStatic
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 **************************************************************************/
void CTagManager::InitStatic()
{
	if(m_nInit == 0)
	{
		CoInitialize(NULL);
	}

	++m_nInit;

	m_Init = TRUE;
}

/**************************************************************************
 * CTagManager::DeinitStatic
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 **************************************************************************/
void CTagManager::DeinitStatic()
{
	if(!m_Init) return;

	--m_nInit;

	if(m_nInit == 0)
	{
		StopThread();

		CoUninitialize();
	}

	m_Init = FALSE;
}

//////////////////////////////////////////////////////////////////////
// Thread Work function
//////////////////////////////////////////////////////////////////////


/**************************************************************************
 * CTagManager::StopThread
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Tag Thread를 멈춘다.
 **************************************************************************/
void CTagManager::StopThread()
{
	// signal stop
	m_bThread = FALSE;

	// close thread handle
	if(m_hThreadEvent)
		SetEvent(m_hThreadEvent);

	if(m_hThread)
	{
		DWORD dwResult = WaitForSingleObject(m_hThread, 1000);

		if(dwResult != WAIT_OBJECT_0)
		{
			CloseHandle(m_hThread);
		}

		m_hThread = NULL;
	}

	// close event handle
	if(m_hThreadEvent)
	{
		CloseHandle(m_hThreadEvent);
		m_hThreadEvent = NULL;
	}
}

/**************************************************************************
 * CTagManager::StartThread
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 **************************************************************************/
BOOL CTagManager::StartThread()
{
	if(m_hThread)
	{
		return FALSE;
	}

	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TagThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	if(m_hThreadEvent)
	{
		CloseHandle(m_hThreadEvent);
	}

	HANDLE hThreadEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

	if(hThreadEvent == NULL)
	{
		CloseHandle(hThread);
		return FALSE;
	}

	m_hThread = hThread;
	m_hThreadEvent = hThreadEvent;

	ResumeThread(hThread);
	return TRUE;
}


/**************************************************************************
 * CTagManager::TagProcess
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 **************************************************************************/
void CTagManager::TagProcess()
{
}

/**************************************************************************
 * CTagManager::TagThread
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Tag 작업의 Thread
 **************************************************************************/
DWORD CTagManager::TagThread()
{
	TRACE(_T("Tag thread Start\r\n"));
	DWORD dwWait = 0;
	m_bThread = TRUE;
	ResetEvent(m_hThreadEvent);
	CoInitialize(NULL);
	while(m_bThread)
	{
		dwWait = WaitForSingleObject(m_hThreadEvent, INFINITE);
		ResetEvent(m_hThreadEvent);

		if(dwWait == WAIT_FAILED) break;
		else if(dwWait != WAIT_OBJECT_0) continue;
		else if(!m_bThread) break;

		// TAG PROCESS 
		// TO DO

	}

	CoUninitialize();

	TRACE(_T("Tag thread End\r\n"));

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Interface function
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * CTagManager::Trigger
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Tag Thread의 Wait 상태를 해제하여 작업을 처리하도록 요청한다.
 **************************************************************************/
void CTagManager::Trigger()
{
	// 테스트를 위해 스레드 처리를 피함
#ifdef FISH_TEST
	TagProcess();
#else
	SetEvent(m_hThreadEvent);
#endif
}