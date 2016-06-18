// OPMLSock.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_def.h"
#include "OPMLSock.h"
#include "LocalProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_OS_PACKET_SENDOPML					_T("OPML^1^%d^%s^%s\r\n")
#define DEFAULTIP_OPMLSERVER					_T("3fishes.co.kr")
#define DEFAULTPORT_OPMLSERVER					6520

/////////////////////////////////////////////////////////////////////////////
// COPMLSock

COPMLSock::COPMLSock()
{
	m_hThread	= NULL;
	ip			= DEFAULTIP_OPMLSERVER;
	port		= DEFAULTPORT_OPMLSERVER;
	m_hInform	= NULL;
}

COPMLSock::~COPMLSock()
{
	CloseOPMLSock();

	int ntry = 0;
	while(m_hThread)
	{
		if(++ntry > 5)
			break;

		Sleep(100);
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(COPMLSock, CAsyncSocket)
	//{{AFX_MSG_MAP(COPMLSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// COPMLSock member functions

void COPMLSock::OnReceive(int nErrorCode) 
{
	// 전송 결과 처리
	char buf[512];
	int ncode;
	CString sdate;

	int nread = Receive(buf, 512);
	if(nread > 0 && nread < 512)
	{
		TCHAR wbuf[512];

		int nlen = MultiByteToWideChar(CP_UTF8, NULL, buf, nread, wbuf, 512);
		wbuf[nlen] = 0;
		TRACE(_T("COPMLSock Read : %s\r\n"), wbuf);
		TCHAR* tok = NULL;
		tok = _tcstok(wbuf, _T("^"));
		int i = 0;
		while(tok)
		{
			switch(i)
			{
			case 0:
				break;
			case 1:
				ncode = _ttoi(tok);
				break;
			case 2:
				sdate = tok;
				break;
			}

			++i;

			tok = _tcstok(NULL, _T("^"));
		}
	}

/*
ncode means as below
1	OPML전송
2	저장 성공
3	파일저장장애
4	DB 접속장애
5	DB Update 실패
6	존재하지 않는 회원
7	틀린 형식의 패킷
8	서버장애
9	인증실패
*/
	
	CAsyncSocket::OnReceive(nErrorCode);

	::SendMessage(m_hInform, WM_UPDATEOPMLCOMPLETE, (WPARAM) ncode, (LPARAM) (LPCTSTR) sdate);

	CloseOPMLSock();
}


void COPMLSock::OnConnect(int nErrorCode) 
{
	// OPML 전송 시작
	SendOPML();

	CAsyncSocket::OnConnect(nErrorCode);
}

void COPMLSock::OnSend(int nErrorCode) 
{
	if(nErrorCode)
	{
	}

	m_DataSending = false;

	m_CanWrite.SetEvent();
	
	CAsyncSocket::OnSend(nErrorCode);
}

BOOL COPMLSock::SendOPML()
{
	if(m_hThread)
	{
		return FALSE;
	}

	// Send Header
	// OPML^CODE^USERNO^OPMLNAME\r\n
	CString sText;
	sText.Format(STR_OS_PACKET_SENDOPML
		, theApp.m_spLP->GetUserSeq()
		, theApp.m_spLP->GetEncodedPassword()
		, m_sFileName);

	TRACE(_T("Send OPML Header : %s\r\n"), sText);

	char buf[512];
	int nlen = WideCharToMultiByte(CP_UTF8, NULL, sText, sText.GetLength()
		, buf, 512, NULL, NULL) ;
//	int nlen = WideCharToMultiByte(CP_ACP, NULL, sText, sText.GetLength()
//		, buf, 512, NULL, NULL) ;

	buf[nlen] = 0;

	int nresult = CAsyncSocket::Send(buf, nlen);
	TRACE(_T("Send OPML Header size : %d\r\n"), nresult);
	if(nresult <= 0) return FALSE;

	// Create Thread
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) UploadThread, (LPVOID) this, CREATE_SUSPENDED, &dwThreadID);

	if(hThread == NULL)
		return FALSE;

	m_hThread = hThread;

	ResumeThread(hThread);
	return TRUE;
}

void COPMLSock::CloseOPMLSock()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		AsyncSelect(0);
		ShutDown(1);

		CAsyncSocket::Close();
	}

	m_CanWrite.SetEvent();
}

#define INVALID_HANDLE 0xFFFFFFFF
UINT COPMLSock::UploadThread(LPVOID lpOPML)
{
	//TRACE0("*** Upload Thread Started\n");
	COPMLSock* p = (COPMLSock*) lpOPML;

	if(!p) return -1;

	int AttemptSend;
	int	BytesCompleted = 0;
	int BytesRead = 0;
	int BytesSent = 0;
	byte *pBuffer    = new byte[4096];
	BOOL IsClosed = FALSE;

	while(BytesCompleted < p->m_FileLength || !IsClosed)
	{
		// Send chunk of bytes read from file
		if(BytesSent < BytesRead)
		{
			AttemptSend = p->Send(pBuffer + BytesSent, BytesRead - BytesSent);
				
			if(AttemptSend == SOCKET_ERROR)
			{
				if(p->GetLastError() != WSAEWOULDBLOCK)
				{
					IsClosed = TRUE;
				}
				else
				{
					WaitForSingleObject((HANDLE) p->m_CanWrite, INFINITE);
					p->m_CanWrite.ResetEvent();
				}
			}
			else
			{
				BytesSent  += AttemptSend;
				BytesCompleted += AttemptSend;
			}			
		}

		// Get next chunk of bytes from file
		else
		{
			BytesSent       = 0;
			//BytesRead       = 0;

			try
			{
				if(p->m_file.m_hFile ==(UINT) CFile::hFileNull 
					|| p->m_file.m_hFile == INVALID_HANDLE ) //
				{
					BytesRead = -1;
				}
				else
				{
					ZeroMemory(pBuffer, 4096);
					BytesRead = p->m_file.Read((byte*) pBuffer, 4096);					
				}
			}
			catch(CFileException&)
			{
				IsClosed = TRUE;
			}

			if(!BytesRead)
			{
				IsClosed = TRUE;
			}			
		}
	}
	
	TRACE(_T("Send Bytes : %d, File Length : %d\r\n"), BytesCompleted, p->m_FileLength);
	
	delete [] pBuffer;

	p->m_hThread = NULL;
	p->m_file.Abort();

	return 0;
}

BOOL COPMLSock::StartTransfer(CString sFileName)
{
	m_sFileName = sFileName;

	if(!m_file.Open(sFileName, CFile::modeRead))
	{
		return FALSE;
	}

	m_FileLength = m_file.GetLength();

	if(m_hSocket == INVALID_SOCKET)
	{
		if(!Create())
		{
			m_file.Abort();
			return FALSE;
		}
	}

//	TRACE(_T("OPML Try Connect : %s : %d\r\n"), ip, port);

	if(!Connect(ip, port))
	{
		int nerr = GetLastError();
		if (nerr != WSAEWOULDBLOCK)
		{
			m_file.Abort();
			return FALSE;
		}
	}

	return TRUE;
}
