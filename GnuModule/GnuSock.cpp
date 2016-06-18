/********************************************************************************

	Gnucleus - A node application for the gnutella network
    Copyright (C) 2001 John Marshall

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	For support, questions, comments, etc...
	E-Mail: 
		swabby@c0re.net
	
	Address:
		21 Cadogan Way
		Nashua, NH, USA 03062 

********************************************************************************/


#include "stdafx.h"
#include "../FISH.H"
#include "GnuRouting.h"
#include "GnuControl.h"
#include "GnuDownload.h"
#include "GnuDownloadShell.h"
#include "GnuUpload.h"
#include "GnuUploadShell.h"
#include "GnuNode.h"

#include "GnuSock.h"

#include "GnuDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFISHApp theApp;

CGnuSock::CGnuSock(CGnuControl* pComm)
{	
	m_pComm				= pComm;

	m_DataSending		= false;
	m_nSecsAlive		= 0;
	m_bDestroy			= false;
	m_ServingHTTP		= false;

	// by bemlove at 2003-06-03 오전 11:51:39
	m_bFolderRequest	= false;

	// written by moonknit 2005-08-17
	m_Host				= _T("");
	m_nPort				= 0;
}

CGnuSock::~CGnuSock()
{
	// Flush receive buffer
	byte pBuff[4096];
	while(Receive(pBuff, 4096) > 0)
		;

	if(m_hSocket != INVALID_SOCKET)
		AsyncSelect(0);

	Close();
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuSock, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

void CGnuSock::OnConnect(int nErrorCode )
{
	CAsyncSocket::OnConnect(nErrorCode);
}

//=========================================================================
// NAME : OnReceive
// PARM : int nErrorCode
// RETN : void
// DESC : GNUSock 에 의한 Receive 메시지 처리
//		 1. Node로 만들기
//       2. Push에 의한 파일 전송 처리
//		 3. Giv 명령에 의한 UploadShell 생성
//       4. HTML Request .. 사용되지 않는듯..
//=========================================================================
void CGnuSock::OnReceive(int nErrorCode) 
{
	DWORD dwBuffLength = Receive(m_pBuff, 4096);

	if(!dwBuffLength || dwBuffLength == SOCKET_ERROR)
	{
		Close();
		return;
	}

#ifdef _UNICODE
	CString Data((TCHAR*) m_pBuff, dwBuffLength/2);
#else
	CString Data((TCHAR*) m_pBuff, dwBuffLength);
#endif

	TRACE(_T("Received Data : %s\r\n"), Data);

	m_Handshake += Data;

	// Firewall not up, unless this is over a LAN
	if(!theApp.m_spGD->m_BehindFirewall)
		theApp.m_spGD->m_RealFirewall = false;

	// 종료중에는 새 연결을 받지 않는다.
	if(theApp.IsClosing()) 
	{
		Close();
		return;
	}
	
	// 나를 Seed Node로 포함하고 있는 리스트의 접속 요청 메시지
	// New Connection
	if(m_Handshake.Find(VERSION_4_CONNECT) != -1 
		|| (m_Handshake.Find(_T(" CONNECT/")) != -1 
		&& m_Handshake.Find(_T("\r\n\r\n")) != -1)
		)
	{
		// Check for duplicate connections
		if(m_pComm->FindNode(m_Host, 0) != NULL)
		{
			Close();
			return;
		}

		// MAX인 경우 자신이 가지고 있는 노드 정보에서 하나를 전달하여 준다.
   		if( m_pComm->m_NodeList.size() >= MAX_GNUNODE )
		{
			Close();
			return;
		}

		// Create a new node connection
		int GnuPort = 0;
		GnuPort = theApp.m_spGD->GetGnuHostPort();;
		
		CGnuNode* NodeSock		= new CGnuNode(m_pComm, m_Host, GnuPort);
		NodeSock->m_Status		= SOCK_CONNECTING;
		NodeSock->m_Inbound		= true;

		// Convert this socket to a Node Socket
		SOCKET FreeSock = Detach();
		NodeSock->Attach(FreeSock);


		m_pComm->m_NodeAccess.Lock();
		m_pComm->m_NodeList.push_back(NodeSock);
		m_pComm->m_NodeAccess.Unlock();

		m_pComm->NodeMessage(SOCK_UPDATE, NULL);
		

		if(m_Handshake.Find(VERSION_4_CONNECT) != -1)
			NodeSock->ParseHandshake04(m_Handshake, m_pBuff, dwBuffLength);
		else
			NodeSock->ParseIncomingHandshake06(m_Handshake, m_pBuff, dwBuffLength);


		m_bDestroy = true;

		return;
	}

	// 새로운 업로드 요청이 있는 경우에..
	// New Upload
	else if( (m_Handshake.Find(_T("GET /get/")) == 0 
		|| m_Handshake.Find(_T("GET /uri-res/")) == 0) 
		&& m_Handshake.Find(_T("\r\n\r\n"))  != -1
		)
	{	
		CString HandScan = m_Handshake;
		HandScan.MakeLower();

		TCHAR		sztype[128];
		CString		stype;
		
		// Get Index
		int Index = -1;
		_stscanf((LPCTSTR) HandScan, _T("get /get/%s /%ld/*\r\n"), &sztype, &Index);

		// 이곳에서 파일 인덱스를 통해서 원하는 파일이 맞는지를 확인해야 한다.
		// 잘못된 인덱스 일 경우에는 리스트에서 파일이 존재하는 지를 확인하고 해당 파일에대한 정보를 전송해 줘야 한다.
		if(Index == -1)
		{
			Close();
			return;
		}

		stype == sztype;

		// 공통 정보
		// POST, FILE 관계 없이 모두 전송되어 오는 정보는 이곳에서 처리한다.

		// USER ID
		CString UserID = _T("");

		int i = 0;
		int UserIdPosition = m_Handshake.Find(_T("USERID:"),0);
		if ( UserIdPosition > 0 ){
			for ( i = UserIdPosition + 7 ; i < m_Handshake.GetLength() ; i ++ )
			{
				if ( m_Handshake.GetAt( i ) != _T('') ) UserID += m_Handshake.GetAt( i );
				else break;
			}
		}

		// NULL ID CHECK
		if( UserID != _T(""))
		{
			Close();
			return;
		}

		// check the upload rejector by file index nuber and user id
		// 거부자인 경우.. => 이때의 거부자란.. 전송취소를 한 경우를 말함
		if ( m_pComm->CheckRejectorList( UserID , Index ) ) return;

		// VERSION INFORMATION
		CString sVersion = _T("");
		i = 0;
		int VersionPosition	=	m_Handshake.Find(URN_VERSION,0);
		if ( VersionPosition > 0 ) {
			for ( i = VersionPosition + _tcsclen(URN_VERSION) ; i < m_Handshake.GetLength() ; i++)
			{
				if ( m_Handshake.GetAt( i ) != '' ) sVersion += m_Handshake.GetAt( i );
				else break;
			}
		}

		WORD wVersion;
		if(VersionPosition <= 0) wVersion = 0;
		else wVersion = (WORD) _ttoi((LPCTSTR) sVersion);


		// MAKE OR GET SHELL
		CGnuUploadShell* UploadSock = NULL;

		std::list<CGnuUploadShell*>::iterator it;
		for(it = m_pComm->m_UploadList.begin(); it != m_pComm->m_UploadList.end(); ++it)
		{
			CGnuUploadShell* p = (*it);

			if(p->m_Host.S_addr == StrtoIP(m_Host).S_addr && p->m_Port == 0)
			{
				p->m_Attempts++;

				if(p->m_FileIndex == Index)
				{
					UploadSock = p;
					break;
				}
				else if(p->m_Status != TRANSFER_CLOSED)
				{
					Close();
					return;
				}
			}
		}

		// new created shell will be deleted when verifying fails
		BOOL bCreatedShell = FALSE;

		if(!UploadSock)
		{
			// Upload limit checking will be executed by the CGnuUploadShell::VerifyFile
			UploadSock = new CGnuUploadShell();

			bCreatedShell = TRUE;
		}

		if(UploadSock)
		{
			// Set Variables
			UploadSock->m_Host       = StrtoIP(m_Host);
			UploadSock->m_Port       = 0;
			UploadSock->m_Handshake  = m_Handshake;
			UploadSock->m_UserID	 = UserID;					// set user's id
			UploadSock->m_Status     = TRANSFER_CONNECTED;

			// VERSION INFORMATION
			UploadSock->m_wVersion		= wVersion;

			// Convert this socket to a Node Socket
			SOCKET FreeSock = Detach();

			if(UploadSock->m_Socket)
				delete UploadSock->m_Socket;

			UploadSock->m_Socket = new CGnuUpload(UploadSock);

			UploadSock->m_Socket->m_GetRequest = m_Handshake;
			UploadSock->m_Socket->Attach(FreeSock);
				
			if(UploadSock->VerifyData(m_Handshake, Index, stype))
			{
				if(bCreatedShell)
				{
					m_pComm->m_UploadAccess.Lock();
					m_pComm->m_UploadList.push_back(UploadSock);
					m_pComm->m_UploadAccess.Unlock();
				}

				UploadSock->m_Socket->m_GetRequest = "";
			}
			else
			{
				UploadSock->m_FileIndex = -1;
				if(bCreatedShell)
					delete UploadSock;
			}
		}

		m_bDestroy = true;

		return;
	}

	// Server Requesting to Push a file to us
	else if(m_Handshake.Find(_T("GIV ")) == 0 &&
		    m_Handshake.Find(_T("\n\n")) != -1)
	{
		// Get Index Number
		DWORD	Index;
		_stscanf((LPCTSTR) m_Handshake, _T("GIV %ld:*/*\n\n"), &Index);

		// Get Server ID of client giving us the file
		int Front = m_Handshake.Find(_T(":")) + 1;
		int End   = m_Handshake.Find(_T("/"));
		CString PushGuid  = m_Handshake.Mid(Front, End - Front);
		PushGuid.MakeUpper();

		// Get the name of the file
		Front = m_Handshake.Find(_T("/")) + 1,
		End   = m_Handshake.Find(_T("\n\n"));
		CString FileName  = m_Handshake.Mid(Front, End - Front);
		FileName.MakeLower();

		// Find the download that requested the push
		std::list<CGnuDownloadShell*>::iterator it;
		for(it = m_pComm->m_DownloadList.begin(); it != m_pComm->m_DownloadList.end(); ++it)	
		{
			CGnuDownloadShell* p = (*it);

			bool Found = false;

			for(int j = 0; j < p->m_Queue.size(); j++)
			{
				if(p->m_Queue[j].NameLower == FileName)
					if(p->m_Queue[j].FileIndex == Index)
						if(GuidtoStr(p->m_Queue[j].PushID) == PushGuid)
						{
							Found = true;
							break;
						}
			}

			if(Found)
			{
				// written by moonknit 2005-06-16
				// 정지 상태에선 아무 작업도 수행하지 않는다.		
				if(p->m_bStop) break;

				p->m_Queue[j].Error     = "";
				p->m_Queue[j].Handshake = "";
				p->m_Queue[j].Alive     = true;
			
				p->m_Active  = true;
				p->m_Dead    = false;
				p->m_Cooling = 0;
				p->m_Waiting = false;
				
				CGnuDownload* PushedFile = new CGnuDownload(p, p->m_Queue[j]);
				PushedFile->m_Push = true;
				
				SOCKET FreeSock = Detach();
				PushedFile->Attach(FreeSock);
				
				p->m_Sockets.push_back(PushedFile);
				
				
				m_Handshake.Replace(_T("\n\n"), _T("\r\n\r\n"));

				PushedFile->GetRealParam()->Handshake = m_Handshake;
	
				if(PushedFile->GetStartPos())
					PushedFile->SendRequest(false);

				break;
			}
		}

		m_bDestroy = true;
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

// 소켓이 살아 있는 시간..
// Called every once a second
void CGnuSock::Timer()
{
	m_nSecsAlive++;
}

int CGnuSock::Send(const void* lpBuf, int nBufLen, int nFlags) 
{
	int ret = CAsyncSocket::Send( lpBuf, nBufLen, nFlags );	
	while( ret == SOCKET_ERROR )
	{
		if( GetLastError() == WSAEWOULDBLOCK )
		{
			ret = CAsyncSocket::Send( lpBuf, nBufLen, nFlags );
		}
		else
		{
		}
	}
	return ret;
}

void CGnuSock::OnSend(int nErrorCode) 
{
	m_DataSending = false;
	
	CAsyncSocket::OnSend(nErrorCode);
}

void CGnuSock::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		AsyncSelect(0);
		ShutDown(1);

		CAsyncSocket::Close();
	}
	m_bDestroy = true;
}
