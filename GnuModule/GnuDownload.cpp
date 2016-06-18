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

#include "../fish.h"

#include "GnuRouting.h"
#include "GnuControl.h"

#include "GnuDownloadShell.h"
#include "GnuDownload.h"

#include "GnuDoc.h"
#include "../LocalProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RETRY_WAIT						60

// 청크의 최소 크기
#define	KBYTES							1024
#define MBYTES							1024 * KBYTES
// 청크의 남은 부분의 크기가 SIZE_MINCHUNK보다 작으면 분할을 하지 않는다.
// 하지만 분할 된 이후의 청크 크기를 제한하지 않았으므로 최소 청크 크기는 정해지지 않는다.
#define	SIZE_MINCHUNK					163840		// 160 Kbytes
#define SIZE_MAXCHUNK					30 * MBYTES
#define USE_MAXCHUNK					TRUE
#define POINTS_NUMBERONE				1000000		// No 1 Club delimitation

extern CFISHApp theApp;

CGnuDownload::CGnuDownload(CGnuDownloadShell* pShell, Result Params)
{
	m_pShell			= pShell;

	m_Params			= Params;
	m_DataSending		= false;
	m_Status			= TRANSFER_QUEUED;
	m_Push				= false;

	m_pFile				= NULL;
	m_pChunk			= NULL;
	
	m_StartPos			= 0;
	m_PausePos			= 0;
	m_ResumeStart		= 0;
	m_VerifyPos			= 0;
	
	UserID				= Params.UserID;
	m_tempSpeed			= Params.Speed;
	m_KeepAlive			= false;
	m_Continuing		= false;

	m_nSecsUnderLimit	= 0;
	m_nSecsDead			= 0;

	dumbResult = new Result;

	// Bandwidth
	for(int i = 0; i < 60; i++)
		m_dwAvgBytes[i] = 0;

	m_dwTotalBytes		= 0;
	m_dwSecBytes		= 0;
	
	m_nSecNum			= 0;
	m_nSecPos			= 0;

	m_bRetry			= false;
	m_bActiveDownload	= false;
	m_bStarted			= false;
}

CGnuDownload::~CGnuDownload()
{
	// Flush receive buffer
	byte pBuff[4096];
	while(Receive(pBuff, 4096) > 0)
		;

//	if(m_hSocket != INVALID_SOCKET)
//		AsyncSelect(0);

	// written by moonknit 2005-08-29
	// 객체 소멸시 해당 객체의 포인터를 사용하는 부분의 정리가 필요하다.
	// 
	Close();
	// --

	delete dumbResult;

	bool ChunkActive = false;

	for(int i = 0; i < m_pShell->m_ChunkList.size(); i++)
		if(m_pChunk == m_pShell->m_ChunkList[i])
			ChunkActive = true;

	if(!ChunkActive && m_pChunk)
	{
		delete m_pChunk;
		m_pChunk = NULL;
	}

	m_pFile = NULL;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuDownload, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuDownload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


void CGnuDownload::OnConnect(int nErrorCode) 
{
	Result* pResult = GetRealParam();

	if(nErrorCode)
	{
		// counting connect fail..
		pResult->nConnectFailCount++; // Date : 2005-01-13,	By bemlove
		pResult->RetryWait = RETRY_WAIT;			// 연결 실패한 것은 잠시 쉰다.

		Close();
	
		return;
	}

	pResult->nConnectFailCount = 0;

	SendRequest(false);
	
	CAsyncSocket::OnConnect(nErrorCode);
}

#define MAX_POSTSIZE					3145728				// 3 MEGA BYTES

void CGnuDownload::OnReceive(int nErrorCode) 
{
	int BuffLength = 0;
	ReceivePending = false;

	// Connected and downloading
	if(m_Status == TRANSFER_RECEIVING)
	{
		int BuffSize = RECEIVE_BUFF;

		BuffLength = Receive(m_pBuff, BuffSize);


		// Check for errors
		switch (BuffLength)
		{
		case 0:
			return;
			break;
		case SOCKET_ERROR:
			int nError = GetLastError();
			if(WSAEWOULDBLOCK != nError)
			{
				SetError("Error Receiving");
				Close();
				return;
			}
			return;
			break;
		}

		if(m_pShell->m_AllocBytes > 0)
			m_pShell->m_AllocBytes -= BuffLength;
		else
			m_pShell->m_AllocBytes = 0;


		if(m_pShell->m_nType == GDT_FILE)
			DownloadFile(m_pBuff, BuffLength);
		else if(m_pShell->m_nType == GDT_POST)
			DownloadPost(m_pBuff, BuffLength);
		else
		{
			SetError("Wrong Transfer Type");
			Close();
			return;
		}
	}

	// Server sending us download header
	else if(m_Status == TRANSFER_CONNECTED)
	{
	
		// Receive Data
		int BuffLength = Receive(m_pBuff, RECEIVE_BUFF);

		// Check for errors
		switch (BuffLength)
		{
		case 0:
			SetError("No Data");
			Close();
			return;
			break;
		case SOCKET_ERROR:
			SetError("Error Receiving");
			Close();
			return;
			break;
		}

		CString Header;
		int     FileBegin = -1;

		for(int i = 0; i < BuffLength; i+=sizeof(TCHAR))
		{
			if( memcmp(&m_pBuff[i], _T("\r\n\r\n"), 4*sizeof(TCHAR)) == 0)
			{
				Header   += _T("\r\n\r\n");
				FileBegin = i + 8;
				break;
			}
			else if(Header.GetLength() > 4096)
			{
				SetError("Handshake Error");
				// written by moonknit 2005-09-06
				++(GetRealParam()->nConnectFailCount);
				// --
				Close();
				return;
			}
			else
			{
				Header += CString((TCHAR) m_pBuff[i]);
			}
		}

		GetRealParam()->Handshake += Header;

		m_Header += Header;


		// If the entire header is here
		if(m_Header.Find(_T("\r\n\r\n")) != -1)
		{
			GetRealParam()->Handshake += _T("\r\n");

			m_Header = m_Header.Left( m_Header.Find(_T("\r\n\r\n")) + 4);

			// Extract HTTP code from first line
			int  Code       = 0;
			char okBuff[10] = "";
			_stscanf(m_Header.Mid(0, m_Header.Find(_T("\r\n"))), _T("%s %d"), okBuff, &Code);

			// Success code
			// file transfer
			if(200 <= Code && Code < 300)
			{		
				m_Header.MakeLower();

				INT64 LengthPos = m_Header.Find(_T("\r\ncontent-length:"));
				INT64 RangePos  = m_Header.Find(_T("\r\ncontent-range:"));
				
				DWORD RemoteLength = 0, StartByte = 0, EndByte = 0;
				
				if(LengthPos > 0)
					_stscanf(m_Header.Mid(LengthPos), _T("\r\ncontent-length:%ld\r\n"), &RemoteLength);

				if(RangePos > 0)
				{
					m_Header.Replace(_T("="), _T(" ")); // Adds compatibility with new headers
					_stscanf(m_Header.Mid(RangePos + 16), _T(" bytes %ld-%ld/%ld\r\n"), &StartByte, &EndByte, &RemoteLength);
				}

				if(USE_KEEP_ALIVE)
				{
					if(EndByte == m_PausePos - 1 || RemoteLength == m_PausePos - m_StartPos)
						m_KeepAlive = true;
					else
						m_KeepAlive = false;
				}

				// New download
				if(StartByte == 0)
				{	
					if(m_pShell->m_BytesCompleted != 0)
					{	
						SetError("No Resume Support");
						GetRealParam()->Alive = false;
						Close();
					}
					else if(RemoteLength == m_pShell->m_FileLength || RemoteLength == m_PausePos - m_StartPos)
					{	
						if(!m_Continuing)
							m_pFile = m_pShell->ReadyFile(this);

						if(m_pFile && m_pChunk == NULL && ReadyChunk())
						{
							StatusUpdate(TRANSFER_RECEIVING);
							SetError("Receiving");

							GetRealParam()->RetryWait = 0;

							if(m_pShell->m_Hwnd && m_pShell->pGroup)
								::PostMessage(m_pShell->m_Hwnd, SEARCH_RESULT_DOWNLOADING, (WPARAM) m_pShell->pGroup, 0);

							if(BuffLength - FileBegin > 0)
								DownloadFile(&m_pBuff[FileBegin], BuffLength - FileBegin);
						}
						else
						{
							Close();
						}
					}
					else
					{
						SetError("Wrong File on Server");
						GetRealParam()->Alive = false;
						Close();
					}
				}

				// Resumed download
				else
				{

					if(StartByte != m_StartPos)
					{
						SetError("No Resume Support");
						Close();
					}
					else if(RemoteLength == m_pShell->m_FileLength || RemoteLength == m_PausePos)
					{
						if(!m_Continuing)
						{
							if(m_pChunk == NULL)
								m_pFile = m_pShell->ReadyFile(this);
							else
								m_pFile = ReadyFile();
						}

						if(m_pFile && ReadyChunk())
						{
							StatusUpdate(TRANSFER_RECEIVING);
							SetError("Receiving");

							GetRealParam()->RetryWait = 0;

							if(m_pShell->m_Hwnd && m_pShell->pGroup)
								::PostMessage(m_pShell->m_Hwnd, SEARCH_RESULT_DOWNLOADING, (WPARAM) m_pShell->pGroup, 0);

							if(BuffLength - FileBegin > 0)
								DownloadFile(&m_pBuff[FileBegin], BuffLength - FileBegin);
						}
						else
						{
							Close();
						}
					}
					else
					{
						SetError("Wrong File on Server");
						GetRealParam()->Alive = false;
						Close();
					}
				}

				// 전송상태 리스트뷰에 나오는 사용자 아이디 갱신하여 주기..
				if( theApp.m_spGD->m_Multisource == false )
				{
					m_pShell->SetTheTransferUserID();
				}
			}

			// post transfer
			else if(100 <= Code && Code < 200)
			{
				m_Header.MakeLower();

				INT64 LengthPos = m_Header.Find(_T("\r\ncontent-length:"));
				
				DWORD RemoteLength = 0, StartByte = 0, EndByte = 0;
				
				if(LengthPos > 0)
					_stscanf(m_Header.Mid(LengthPos), _T("\r\ncontent-length:%ld\r\n"), &RemoteLength);
				else
				{
					SetError("Coudn't find post length");
					Close();
					return;
				}

				if(RemoteLength > MAX_POSTSIZE)
				{
					SetError("Too big post!!");
					Close();
					return;
				}

				EndByte = RemoteLength - 1;

				// Always New download
				m_pShell->m_FileLength = RemoteLength;
				m_pShell->m_BytesCompleted = 0;
				m_pShell->m_PostXML = _T("");

				StatusUpdate(TRANSFER_RECEIVING);
				SetError("Receiving");

				GetRealParam()->RetryWait = 0;

				if(m_pShell->m_Hwnd && m_pShell->pGroup)
					::PostMessage(m_pShell->m_Hwnd, SEARCH_RESULT_DOWNLOADING, (WPARAM) m_pShell->pGroup, 0);

				if(BuffLength - FileBegin > 0)
					DownloadPost(&m_pBuff[FileBegin], BuffLength - FileBegin);
			}

			// Server error code
			else if(400 <= Code && Code < 500)
			{
				SetError("File Not Found");
				GetRealParam()->Alive = false;
				if( Code == 404 )
					GetRealParam()->bFileNotFound = true;

				Close();
			}
	
			else if(500 <= Code && Code < 600)
			{
				SetError("Server Busy");
				Close();
			}
	
			else
			{
				SetError("Server Error - " + DWrdtoStr(Code));
				GetRealParam()->Alive = false;
				Close();
			}
		}

		// else, still waiting for rest of header
	}

	// We are not in a connected or receiving state
	else
	{
		SetError("Not Ready to Receive");
		Close();		
	}


	CAsyncSocket::OnReceive(nErrorCode);
}

int CGnuDownload::Send(const void* lpBuf, int nBufLen, int nFlags) 
{
	int Command = CAsyncSocket::Send(lpBuf, nBufLen, nFlags);
	
	if(Command != SOCKET_ERROR)
		m_DataSending = true;

	return Command;
}

void CGnuDownload::OnSend(int nErrorCode) 
{
	m_DataSending = false;
	
	CAsyncSocket::OnSend(nErrorCode);
}

void CGnuDownload::OnClose(int nErrorCode) 
{
	Close();

	CAsyncSocket::OnClose(nErrorCode);
}

void CGnuDownload::Close()
{
	if(m_nSecNum)
		GetRealParam()->RealBytesPerSec = m_dwTotalBytes / m_nSecNum;

	if(m_hSocket != INVALID_SOCKET)
	{
		AsyncSelect(0);
		ShutDown(1);

		if(GetRealParam()->Error == _T(""))
			GetRealParam()->Error = _T("Remotely Canceled");

		CAsyncSocket::Close();
	}


	// 아래 작업은 종료시 처리할 필요가 없음
	if(theApp.IsClosing()) return;

	StatusUpdate(TRANSFER_CLOSED);
	// 실제 전송이 진행되었던 소켓이라면 activeDownloadHost 리스트에서 제거
	if( m_bActiveDownload )
	{
		ActiveHost host;
		host.HostIP.S_addr = GetRealParam()->Host.S_addr;
		host.Port = GetRealParam()->Port;
		theApp.m_spGD->m_Control.DelActiveHost( host );
		m_bActiveDownload = false;
	}

	if(m_bStarted)
	{
		theApp.m_spGD->m_Control.RemoveDLNode(this);
		m_bStarted = false;
	}

	Result* pResult = GetRealParam();
	if(pResult->Trying)
	{
		pResult->Trying = false;
		// 전송 시도중인 내부 호스트의 개수를 줄인다.
		m_pShell->DecreaseTryCnt();
	}
}

bool CGnuDownload::GetStartPos()
{
	// Reset m_pChunk because this can be called multiple times
	if(m_pChunk)
	{
		bool destroy = true;

		for(int i = 0; i < m_pShell->m_ChunkList.size(); i++)
			if(m_pShell->m_ChunkList[i] == m_pChunk)
				destroy = false;

		if(destroy)
			delete m_pChunk;

		m_pChunk = NULL;
	}


	// At least 4096 bytes needed for resume verification
	m_StartPos    = 0;
	m_PausePos	  = 0;
	m_ResumeStart = 0;
	m_VerifyPos   = 0;

	bool   MainFileLocked = false;
	double MainFileSpeed  = 0;

	double ThisBps = GetRealParam()->RealBytesPerSec;

	for(int i = 0; i < m_pShell->m_Sockets.size(); i++)
	{
		// 현재 연결된 소켓이 파샬을 다운로드 중인 경우 메인 파일 사용중으로 설정
		if(m_pShell->m_Sockets[i]->m_pChunk == NULL && 
			m_pShell->m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
		{
			if(m_pShell->m_Sockets[i]->m_nSecNum)
				MainFileSpeed = m_pShell->m_Sockets[i]->m_dwTotalBytes / m_pShell->m_Sockets[i]->m_nSecNum;
			
			MainFileLocked = true;
		}
	}

	// Make sure we can add to main file
	if(!MainFileLocked)
	{
		// 파일 전송을 중지한 위치가 현재 다운로드 받은 사이즈 보다 작을 경우
		// 현재 메인 파일을 전송중임.
		if(m_pShell->m_bComplete && m_pShell->m_StopPos && m_pShell->m_BytesCompleted >= m_pShell->m_StopPos)
			MainFileLocked = true;
	}



	// Main file in use, must create or add to a chunk
	if(MainFileLocked)
	{
		// No chunks created yet
		if(m_pShell->m_ChunkList.size() == 0)
		{
			m_pChunk = new FileChunk;

			m_pChunk->Sha1Hash = m_Params.Sha1Hash;
			m_pChunk->BytesCompleted = 0;

			// 최저 Chunk 크기 변경
			// m_pChunk삭제
			if((m_pShell->m_FileLength - m_pShell->m_BytesCompleted) < SIZE_MINCHUNK)
			{
				delete m_pChunk;
				m_pChunk = NULL;
				return false;
			}

			double BytesLeft = m_pShell->m_FileLength - m_pShell->m_BytesCompleted;
			
			if(MainFileSpeed && ThisBps)
			{
				int TotalBps = MainFileSpeed + ThisBps;

				m_StartPos = m_pShell->m_BytesCompleted + (BytesLeft * (MainFileSpeed / TotalBps) );

			}
			else
				m_StartPos = m_pShell->m_BytesCompleted + (BytesLeft / 2);

#ifdef USE_MAXCHUNK
			// 분할 된 뒤의 앞부분의 청크의 크기가 최대 청크의 크기보다 큰 경우에는
			// 앞부분의 청크의 크기를 최대 청크의 크기로 맞춘다.
			if((m_StartPos - m_pShell->m_BytesCompleted) > SIZE_MAXCHUNK + SIZE_MINCHUNK)
				m_StartPos = m_pShell->m_BytesCompleted + SIZE_MAXCHUNK;
#endif

			
			m_pChunk->StartPos     = m_StartPos;
			m_pChunk->ChunkLength  = m_pShell->m_FileLength - m_StartPos;

			m_pChunk->OverlapBytes = 0;

			if(USE_KEEP_ALIVE)
			{
				if(m_pChunk->ChunkLength > DL_SECTION_SIZE)
					m_PausePos = m_StartPos + DL_SECTION_SIZE;
				else
					m_PausePos = m_StartPos + m_pChunk->ChunkLength;
			}
			else
				m_PausePos = m_StartPos + m_pChunk->ChunkLength;

			SetRandColor(m_pChunk->ChunkColor);
			m_pChunk->HostFamily = ++m_pShell->HostFamilyAge;

			return true;
		}


		// Look for inactive chunk
		for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
		{
			// added by moonknit
			if(m_pShell->m_ChunkList[i]->bComplete) continue;

			bool ChunkLocked = false;

			for(int j = 0; j < m_pShell->m_Sockets.size(); j++)
			{
				if(m_pShell->m_ChunkList[i] == m_pShell->m_Sockets[j]->m_pChunk)
					ChunkLocked = true;
			}

			// Resuming off a current chunk
			
			// Chunk 리스트 중에 남아 있는 자료가 있는지 확인.
			if(!ChunkLocked &&
				m_pShell->m_ChunkList[i]->BytesCompleted < m_pShell->m_ChunkList[i]->ChunkLength)
			{	
				m_pChunk = m_pShell->m_ChunkList[i];

				INT64 UnfinishedChunkSize = m_pChunk->ChunkLength - m_pChunk->BytesCompleted;
				
				if(m_pChunk->BytesCompleted > 4096)
				{
					m_StartPos = m_pChunk->StartPos + m_pChunk->BytesCompleted - 4096;
					UnfinishedChunkSize += 4096;
				}
				else
				{
					m_pChunk->BytesCompleted = 0;
					m_StartPos = m_pChunk->StartPos;
				}

				if(USE_KEEP_ALIVE)
				{
					if(UnfinishedChunkSize > DL_SECTION_SIZE)
						m_PausePos = m_StartPos + DL_SECTION_SIZE;
					else
						m_PausePos = m_StartPos + UnfinishedChunkSize;
				}
				else
					m_PausePos = m_StartPos + UnfinishedChunkSize;

				return true;
			}
		}


		// All chunks active make a new one (not inserted into chunk table until d/l starts)
		FileChunk* pEmpty		= NULL;
		INT64	   EmptySize	= 0;

		// 청크 분할시 분할 우선 순위를 앞쪽의 청크로 둔다.
		// m_pShell->m_pStopPos - m_pShell->m_BytesCompleted 블록에 가중치를 둔다.
		// 그러면 Partial 블록을 청크 분할하는 경우가 늘어난다.
		// 청크 분할 가중치는 
		// Partial 4
		// Chunk[0] 3
		// Chunk[1] 2
		// Chunk[2] 1 이하 동일
		__int64 Block = m_pShell->m_StopPos - m_pShell->m_BytesCompleted;
		__int64 vBlock = Block * 4;	// 4는 가중치이다.
		__int64 vEmptySize = 0;

		if(Block > SIZE_MINCHUNK)
		{
			EmptySize = Block;
			vEmptySize = vBlock;
		}

		// chunk도 앞 부분의 청크를 먼저 받기 위해서 앞 부분의 청크에 더 가중치를 두도록 한다.
		for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
		{
			// 왼성된 청크인 경우는 다음 청크로 넘어간다.
			if(m_pShell->m_ChunkList[i]->bComplete) 
				continue;

			Block = m_pShell->m_ChunkList[i]->ChunkLength - m_pShell->m_ChunkList[i]->BytesCompleted;

			// 남겨진 Block이 최소 청크 크기보다 작다면 다음 청크로 넘어간다.
			if(Block <= SIZE_MINCHUNK)
				continue;

			if(i < 2)
			{
				vBlock = Block * (3  - i);
			}
			else
				vBlock = Block;
			
			if(vBlock > vEmptySize)
			{
				EmptySize = Block;
				pEmpty	= m_pShell->m_ChunkList[i];

				vEmptySize = vBlock;
			}
		}

		// 다운로드 청크의 최소크기 변경 16 Kbyte -> 160 Kbyte
		// 청크나 Partial의 최대 크기에 맞추어 새로 생기는 청크의 크기를 설정한다.
		// 분할어진 Partial과 Chunk는 남은 부분의 크기가 SIZE_MAXCHUNK를 넘어선 안된다.
		if(EmptySize > SIZE_MINCHUNK)
		{
			m_pChunk = new FileChunk;
			
			m_pChunk->Sha1Hash = m_Params.Sha1Hash;

			double BytesLeft = 0;

			// New chunk between two chunks
			if(pEmpty)
			{
				double ChunkSpeed = 0;

				for(int j = 0; j < m_pShell->m_Sockets.size(); j++)
					if(pEmpty == m_pShell->m_Sockets[j]->m_pChunk)
					{
						if(m_pShell->m_Sockets[j]->m_nSecNum)
							ChunkSpeed = m_pShell->m_Sockets[j]->m_dwTotalBytes / m_pShell->m_Sockets[j]->m_nSecNum;

						break;
					}

				BytesLeft = pEmpty->ChunkLength - pEmpty->BytesCompleted;
			
				if(ChunkSpeed && ThisBps)
				{
					double TotalBps = ChunkSpeed + ThisBps;

					m_pChunk->StartPos = (pEmpty->StartPos + pEmpty->BytesCompleted) + (BytesLeft * (ChunkSpeed / TotalBps) );

				}
				else
					m_pChunk->StartPos = (pEmpty->StartPos + pEmpty->BytesCompleted) + (BytesLeft / 2);

#ifdef USE_MAXCHUNK
				// 분할 된 뒤의 앞부분의 청크의 크기가 최대 청크의 크기보다 큰 경우에는
				// 앞부분의 청크의 크기를 최대 청크의 크기로 맞춘다.
				if((m_pChunk->StartPos - pEmpty->StartPos - pEmpty->BytesCompleted) > SIZE_MAXCHUNK + SIZE_MINCHUNK)
					m_pChunk->StartPos = pEmpty->StartPos + pEmpty->BytesCompleted + SIZE_MAXCHUNK;
#endif
				
				m_pChunk->ChunkLength = pEmpty->StartPos + pEmpty->ChunkLength - m_pChunk->StartPos; 
			}

			// New chunk between partial and another chunk
			else
			{
				BytesLeft = m_pShell->m_StopPos - m_pShell->m_BytesCompleted;

				if(MainFileSpeed && ThisBps)
				{
					int TotalBps = MainFileSpeed + ThisBps;

					m_pChunk->StartPos = m_pShell->m_BytesCompleted + (BytesLeft * (MainFileSpeed / TotalBps) );
				}
				else
					m_pChunk->StartPos = m_pShell->m_BytesCompleted + (BytesLeft / 2);

#ifdef USE_MAXCHUNK
				// 분할 된 뒤의 앞부분의 청크의 크기가 최대 청크의 크기보다 큰 경우에는
				// 앞부분의 청크의 크기를 최대 청크의 크기로 맞춘다.
				if((m_pChunk->StartPos - m_pShell->m_BytesCompleted) > SIZE_MAXCHUNK + SIZE_MINCHUNK)
					m_pChunk->StartPos = m_pShell->m_BytesCompleted + SIZE_MAXCHUNK;
#endif

				m_pChunk->ChunkLength = m_pShell->m_StopPos - m_pChunk->StartPos; 
			}

			m_pChunk->BytesCompleted = 0;
			m_StartPos = m_pChunk->StartPos;

			if(USE_KEEP_ALIVE)
			{
				if(m_pChunk->ChunkLength > DL_SECTION_SIZE)
					m_PausePos = m_StartPos + DL_SECTION_SIZE;
				else
					m_PausePos = m_StartPos + m_pChunk->ChunkLength;
			}
			else
				m_PausePos = m_StartPos + m_pChunk->ChunkLength;

			SetRandColor(m_pChunk->ChunkColor);
			m_pChunk->HostFamily = ++m_pShell->HostFamilyAge;
			
			return true;
		}
		else
		{
			// 청크들의 다운로드는 완료된 상태임.
			// 파샬의 남은 위치를 다운로드 요청하기 위해 위치 정보를 설정하여 준다.
		}
	}


	// Using main file
	else
	{
		INT64 UnfinishedSize = m_pShell->m_StopPos - m_pShell->m_BytesCompleted;
		
		if(m_pShell->m_BytesCompleted > 4096)
		{
			m_StartPos = m_pShell->m_BytesCompleted - 4096;
			UnfinishedSize += 4096;
		}
		else
		{
			m_pShell->m_BytesCompleted = 0;
		}

		if(USE_KEEP_ALIVE)
		{
			if(UnfinishedSize > DL_SECTION_SIZE)
				m_PausePos = m_StartPos + DL_SECTION_SIZE;
			else
				m_PausePos = m_StartPos + UnfinishedSize;
		}
		else
			m_PausePos = m_StartPos + UnfinishedSize;
	}

	return true;
}

/**************************************************************************
 * CGnuDownload::SendRequest
 *
 * coded by bemlove
 *
 * @history
 * created 2003-04-02 오후 5:52:25
 *
 * @Description
 * 다운로드를 요청한다.
 *
 * @Parameters
 * (in bool) Continuation - 이어받이인 경우 true, 새로 받기인 경우 false
 *
 * @return
 **************************************************************************/
void CGnuDownload::SendRequest(bool Continuation)
{
	// Server is alive
	GetRealParam()->Alive = true;

	if(m_pShell->m_nType == GDT_FILE)
	{
		m_Continuing = Continuation;

		if(!Continuation)
		{
			if(!GetStartPos())
			{
				Close();
				return;
			}
		}

		// Continuing same chunk with keep-alive
		else
		{
			m_StartPos = m_PausePos;

			if(m_pChunk)
			{
				INT64 UnfinishedChunkSize = m_pChunk->ChunkLength - m_pChunk->BytesCompleted;

				if(UnfinishedChunkSize > DL_SECTION_SIZE)
					m_PausePos = m_StartPos + DL_SECTION_SIZE;
				else
					m_PausePos = m_StartPos + UnfinishedChunkSize;
			}
			else
			{
				INT64 UnfinishedSize = m_pShell->m_StopPos - m_pShell->m_BytesCompleted;

				if(UnfinishedSize > DL_SECTION_SIZE)
					m_PausePos = m_StartPos + DL_SECTION_SIZE;
				else
					m_PausePos = m_StartPos + UnfinishedSize;
			}

		}

		if(m_PausePos <= m_StartPos)
		{
			Close();
			return;
		}

		CString GetFile;
		CString UserID;

		CString Version = GNUCLEUS_VERSION;

		// modify Get protocol by moonknit 2006-01-02
		// old format
		// GetFile.Format(_T(GET /get/%ld/%s/ HTTP/1.1\r\n"), m_Params.FileIndex, m_Params.Name);
		GetFile.Format(_T("GET /get/%s /%ld HTTP/1.1\r\n")
			, GDT_FILE_S, m_Params.FileIndex);

		GetFile += UserID;
		
		CString Host; 
		Host.Format(_T("Host: %s:%i\r\n"), IPtoStr(GetRealParam()->Host), GetRealParam()->Port); 

		GetFile += Host;

		// written by moonknit 2005-08-11
		// VERSION INFORMATION
		CString sVersion;
		sVersion.Format(_T("%s%d\n"), URN_VERSION, FISH_VERSION_WORD);
		GetFile += sVersion;
		// -- 

		// 파일 사이즈도 함께 전송하여 준다. ( 파일이 일치하는지 확인하기 위함 )
		CString FileSize;
		FileSize.Format(_T("FileSize:%u\r\n"), GetRealParam()->Size );
		GetFile += FileSize;

		#ifdef FIGHT_SYSTEM
			GetFile += _T("User-Agent: Suelcung ") + Version + _T("\r\n");
		#else
			GetFile += _T("User-Agent: MyNapster ") + Version + _T("\r\n");
		#endif
		
		if(USE_KEEP_ALIVE)
			GetFile += _T("Connection: Close\r\n");
		else
			GetFile += _T("Connection: Keep-Alive\r\n");
		
		GetFile	+= _T("Range: bytes=") + DWrdtoStr(m_StartPos) + _T("-") + DWrdtoStr(m_PausePos - 1) + _T("\r\n");


		// Add hash and alternate locations
		if(!m_pShell->m_Sha1Hash.IsEmpty())
		{
			GetFile	+= _T("X-Gnutella-Content-URN: urn:rfid:") + m_pShell->m_Sha1Hash + _T("\r\n");
		}


		GetFile		+= _T("\r\n");

		Send(GetFile, GetFile.GetLength() * sizeof(TCHAR));

		GetRealParam()->Handshake += GetFile;
	}
	else if(m_pShell->m_nType == GDT_POST)
	{
		CString GetPost;
		CString UserID;

		CString Version = GNUCLEUS_VERSION;
		// modify Get protocol by moonknit 2006-01-02
		// old format
		// GetFile.Format(_T(GET /get/%ld/%s/ HTTP/1.1\r\n"), m_Params.FileIndex, m_Params.Name);
		GetPost.Format(_T("GET /get/%s /%ld HTTP/1.1\r\n")
			, GDT_POST_S, m_Params.FileIndex);

		GetPost += UserID;
		
		CString Host; 
		Host.Format(_T("Host: %s:%i\r\n"), IPtoStr(GetRealParam()->Host), GetRealParam()->Port); 

		GetPost += Host;

		// VERSION INFORMATION
		CString sVersion;
		sVersion.Format(_T("%s%d\n"), URN_VERSION, FISH_VERSION_WORD);
		GetPost += sVersion;
		
		if(USE_KEEP_ALIVE)
			GetPost += _T("Connection: Close\r\n");
		else
			GetPost += _T("Connection: Keep-Alive\r\n");
		
		GetPost		+= _T("\r\n");

		Send(GetPost, GetPost.GetLength() * sizeof(TCHAR));

		GetRealParam()->Handshake += GetPost;
	}
	else
	{
		Close();
		return;
	}

	m_nSecsDead = 0;
	StatusUpdate(TRANSFER_CONNECTED);

	m_bActiveDownload = true;
}

CFileLock* CGnuDownload::ReadyFile()
{
	static int co = 0;
	for(int i = 0; i < m_pShell->m_Sockets.size(); i++)
		if(m_pShell->m_Sockets[i]->m_pChunk == m_pChunk && m_pShell->m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
			return NULL;

	// Close file first
	m_pChunk->File.Abort();

	CFileException FileError;

	// Get plain file name with out directory crap
	CString FileName = m_pShell->m_Name;
	FileName.Replace(_T("\\"), _T("/"));
	FileName = FileName.Mid( FileName.ReverseFind(_T('/') + 1));

	// Set where the file will go upon completion

	// Add tail to it so we can support partial downloads

	CString Tail = _T(" (") + DWrdtoStr(m_pChunk->StartPos) + _T(")");

	int endPos = FileName.ReverseFind(_T('.'));
	if( endPos != -1)
		FileName.Insert(endPos, Tail);
	else
		FileName += Tail;

	m_pChunk->Name = FileName;

	// Create / Find the file in the partial directory
	CString ChunkDir = theApp.m_spLP->PartialsInDir ? 
		theApp.m_spLP->DownloadPath + _T("\\Partials\\Chunks") 
		: theApp.m_spLP->RunPath + _T("Partials\\Chunks");
	CreateDirectory(ChunkDir, NULL);

	// modified by moonknit 2005-08-24
#ifdef PREFIX_FILENAME
	m_pChunk->Path = ChunkDir + _T("\\FISH_") + FileName;
#else
	m_pChunk->Path = ChunkDir + _T("\\") + FileName;
#endif

	m_pChunk->Path.Replace(_T("\\\\"), _T("\\"));

	// Resuming a file
	if(m_pChunk->BytesCompleted > 4096)
	{
		if(!m_pChunk->File.Open(m_pChunk->Path, CFileLock::modeCreate | CFileLock::modeNoTruncate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
		{
			SetError(GetFileError(&FileError));
			return NULL;
		}

		DWORD EOFpos = 0;
		try
		{
			EOFpos = m_pChunk->File.SeekToEnd();

			if(EOFpos > 4096)
			{
				m_pChunk->BytesCompleted = EOFpos;
				m_ResumeStart = m_pChunk->BytesCompleted - 4096;
				m_pChunk->PreCompletedBytes = m_ResumeStart;
			}
			else
			{
				m_pChunk->BytesCompleted = 0;
				m_pChunk->File.Abort();

				if(!m_pChunk->File.Open(m_pChunk->Path, CFileLock::modeCreate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
				{
					SetError(GetFileError(&FileError));
					return NULL;
				}
			}
		}
		catch(CFileException* e)
		{
			SetError(GetFileError(e));
			e->Delete();
			return NULL;
		}
	}

	// Starting a new file
	else if(!m_pChunk->File.Open(m_pChunk->Path, CFileLock::modeCreate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
	{
		SetError(GetFileError(&FileError));
		return NULL;
	}

	m_pShell->m_BackupTrigger = true;
	// 파일 생성 또는 이어받기를 시작하면 강제로 Backup을 생성한다.
	m_pShell->m_nBackupCount = 0;

	return &m_pChunk->File;
}

void CGnuDownload::AddChunkHost(std::vector<IP> &HostList, IP NewHost)
{
	// Adds host to back of list, any duplicates deleted from list
	std::vector<IP>::iterator itHost;

	for(itHost = HostList.begin(); itHost != HostList.end(); itHost++)
	{
		if( (*itHost).S_addr == NewHost.S_addr)
		{
			HostList.erase(itHost);
			itHost --;
		}
		else
		{
		}
	}

	HostList.push_back(NewHost);
}

bool CGnuDownload::ReadyChunk()
{
	int i, j, k;

	// If chunk is the main file
	if(m_pChunk == NULL)
	{
		if(m_pShell->m_BytesCompleted > m_pShell->m_StopPos)
		{
			// for test by bemlove at 2003-08-22 오전 11:33:33
			m_pShell->m_StopPos = m_pShell->m_BytesCompleted;
			return false;
		}

		AddChunkHost(m_pShell->m_PartialHosts, m_Params.Host);

		if(m_pShell->m_ChunkList.size())
		{
			m_pShell->m_StopPos = m_pShell->m_ChunkList[0]->StartPos + 4096;
			m_pShell->m_OverlapBytes = 4096;	
		}
		else
		{
			m_pShell->m_StopPos = m_pShell->m_FileLength;
			m_pShell->m_OverlapBytes = 0;
		}
				
		return true;
	}

	

	// Insert chunk into chunk list
	else
	{

		AddChunkHost(m_pChunk->Hosts, m_Params.Host);

		// Check if resuming a current chunk
		for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
			if(m_pShell->m_ChunkList[i] == m_pChunk)
			{
				AddChunkHost(m_pChunk->Hosts, m_Params.Host);
				//m_pChunk->Hosts.push_back(m_Params.Host);
				return true;
			}


		// Find if chunk has any hosts in common with other chunks or the partial
		for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
			if(m_pShell->m_ChunkList[i] != m_pChunk && m_pShell->m_ChunkList[i]->HostFamily < m_pChunk->HostFamily)
			{
				for(j = 0; j < m_pShell->m_ChunkList[i]->Hosts.size(); j++)
					for(k = 0; k < m_pChunk->Hosts.size(); k++)
						if(m_pShell->m_ChunkList[i]->Hosts[j].S_addr == m_pChunk->Hosts[k].S_addr)
						{
							m_pChunk->ChunkColor = m_pShell->m_ChunkList[i]->ChunkColor;
							m_pChunk->HostFamily = m_pShell->m_ChunkList[i]->HostFamily;
						}
			}

		for(i = 0; i < m_pShell->m_PartialHosts.size(); i++)
			for(j = 0; j < m_pChunk->Hosts.size(); j++)
				if(m_pShell->m_PartialHosts[i].S_addr == m_pChunk->Hosts[j].S_addr)
				{
					m_pChunk->ChunkColor = RGB(0, 255, 0);
					m_pChunk->HostFamily = 0;
				}


		// Chunk list currently empty
		if(m_pShell->m_ChunkList.size() == 0)
		{
			if(m_pShell->m_BytesCompleted > m_pShell->m_StopPos)
			{

//				TRACE0("** Main file overrun at chunk insert\n");
				m_pShell->m_StopPos = m_pShell->m_BytesCompleted; // => GetStartPos()에서 값의 설정이 잘못되었으니.. 필요가 없을듯..
				return false;
			}

			if(m_pShell->m_FileLength - m_pChunk->StartPos < 8192)
			{
				TRACE(_T("** Chunk too small to insert after main file\n"));
				return false;
			}

			m_pChunk->ChunkLength   = m_pShell->m_FileLength - m_pChunk->StartPos;

			m_pChunk->OverlapBytes  = 0;
			m_pShell->m_StopPos      = m_pChunk->StartPos + 4096;
			m_pShell->m_OverlapBytes = 4096;

			m_pShell->m_ChunkList.push_back(m_pChunk);


			return true;
		}


		std::vector<FileChunk*>::iterator itChunk = m_pShell->m_ChunkList.begin();

		// Chunk at front of list
		if(m_pChunk->StartPos < (*itChunk)->StartPos)
		{	
			if(m_pShell->m_BytesCompleted > m_pChunk->StartPos + 4096)
			{

				TRACE(_T("** Chunk overrun at front\n"));
				return false;
			}
		
			if((*itChunk)->StartPos + 4096 - m_pChunk->StartPos < 8192)
			{
				TRACE(_T("** Chunk too small to insert at front\n"));
				return false;
			}
			
			m_pShell->m_StopPos		 = m_pChunk->StartPos + 4096;
			m_pShell->m_OverlapBytes = 4096;
			
			m_pChunk->ChunkLength   = (*itChunk)->StartPos + 4096 - m_pChunk->StartPos;
			
			m_pChunk->OverlapBytes  = 4096;


			m_pShell->m_ChunkList.insert(itChunk, m_pChunk);
			
			return true;
		}

		// Chunk at middle or end of list
		for(itChunk = m_pShell->m_ChunkList.begin(); itChunk != m_pShell->m_ChunkList.end(); itChunk++)
			if(m_pChunk->StartPos > (*itChunk)->StartPos + (*itChunk)->BytesCompleted && m_pChunk->StartPos < (*itChunk)->StartPos + (*itChunk)->ChunkLength)
			{	
				if( (*itChunk)->StartPos + (*itChunk)->ChunkLength - m_pChunk->StartPos < 8192)
				{
					TRACE(_T("** New Chunk too small to insert at middle or end\n"));
					return false;
				}

				if(m_pChunk->StartPos + 4096 - (*itChunk)->StartPos + (*itChunk)->BytesCompleted < 8192)
				{
					TRACE(_T("** Not enough space to insert chunk at middle or end\n"));
					return false;
				}

				m_pChunk->ChunkLength  = (*itChunk)->StartPos + (*itChunk)->ChunkLength - m_pChunk->StartPos;

				(*itChunk)->ChunkLength   = m_pChunk->StartPos + 4096 - (*itChunk)->StartPos;
				(*itChunk)->OverlapBytes = 4096;
				

				if(itChunk + 1 == m_pShell->m_ChunkList.end())
				{
					m_pChunk->OverlapBytes = 0;
					m_pShell->m_ChunkList.push_back(m_pChunk);
				}
				else
				{
					m_pChunk->OverlapBytes = 4096;
					m_pShell->m_ChunkList.insert(itChunk + 1, m_pChunk);
				}
		
				return true;
			}
	}


	// Chunk not inserted
	int error = 666;
	if(m_pChunk)
		error = m_pChunk->StartPos;
	TRACE(_T("** Chunk not inserted - ") + DWrdtoStr(error) + _T("\n"));

	
	return false;
}

int CGnuDownload::StartDownload()
{
	// Find somewhere in the file to start downloading from
	// critical error
	// how to??
	if(m_pShell->m_nType == GDT_FILE && !GetStartPos())
	{
		return -1;
	}

	// Reset handshake
	GetRealParam()->Handshake = "";
	
	if(m_hSocket == INVALID_SOCKET)
	{
		if(!Create())
		{
			SetError("Socket Create Failed");
			return -1;
		}
	}

	// Attempt connect

//	TRACE(_T("Try To Connect for Download : ip [%s], port [%d]\r\n"), IPtoStr(m_Params.Host), m_Params.Port);

	// this code will limit the same client try to download files with two or more connection
	if(theApp.m_spGD->m_Control.IsDownloading(this))
	{
//		TRACE(_T("The same host is now being used\r\n"));
		return -2;
	}

	if(!Connect(IPtoStr(m_Params.Host), m_Params.Port))
	{
		if(GetLastError() != WSAEWOULDBLOCK)
		{
			// not use push
//			theApp.m_spGD->m_Control.Route_LocalPush(m_Params);

			SetError("Socket Connect Failed (push sent)");
			return -1;
		}
	}

	theApp.m_spGD->m_Control.AddDLNode(this);

	StatusUpdate(TRANSFER_CONNECTING);

	m_bStarted = true;

	return 0;
}

void CGnuDownload::StopDownload()
{
	SetError("Stopped");
	Close();
}
					
void CGnuDownload::DownloadFile(byte* pBuff, int nSize)
{
	if(m_pFile == NULL)
		Close();

	INT64* pBytesCompleted = NULL;

	if(m_pChunk)
		pBytesCompleted = &m_pChunk->BytesCompleted;
	else
		pBytesCompleted = &m_pShell->m_BytesCompleted;


	if(m_pFile->m_hFile != CFileLock::hFileNull)
	{
		// Verify resuming (done at download beginning)
		if(m_ResumeStart)
		{
			int VerSize   = nSize;
			m_ResumeStart += VerSize;

			if(m_ResumeStart > *pBytesCompleted)
				VerSize -= m_ResumeStart - *pBytesCompleted;
	
			if( VerSize < 0 )
				VerSize = 0;

			if(m_VerifyPos + VerSize > 4096)
				VerSize = 4096 - m_VerifyPos;
			
			memcpy(m_Verification + m_VerifyPos, pBuff, VerSize);

			m_VerifyPos += VerSize;

			if(m_VerifyPos == 4096)
			{
				// Compare the buffers
				byte FileBuff[4096];

				try
				{
					m_pFile->Seek(-4096, CFileLock::end);
					m_pFile->Read(FileBuff, 4096);
				}
				catch(CFileException* e)
				{
					SetError(GetFileError(e));
					e->Delete();
					Close();
					return;
				}

				if(memcmp(m_Verification, FileBuff, 4096) == 0)
				{
					try
					{
						m_pFile->SeekToEnd();
					}
					catch(CFileException* e)
					{
						SetError(GetFileError(e));
						e->Delete();
						Close();
						return;
					}

					TRACE(m_pShell->m_Name + _T(": Resume Succeeded at ") + DWrdtoStr(m_ResumeStart) + _T("\n"));

					// Verification successful, see if we can combine common hosts
					if(m_pChunk)
					{
						int i, j, k;

						for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
							if(m_pShell->m_ChunkList[i] != m_pChunk && m_pShell->m_ChunkList[i]->HostFamily < m_pChunk->HostFamily)
							{
								for(j = 0; j < m_pShell->m_ChunkList[i]->Hosts.size(); j++)
									for(k = 0; k < m_pChunk->Hosts.size(); k++)
										if(m_pShell->m_ChunkList[i]->Hosts[j].S_addr == m_pChunk->Hosts[k].S_addr)
										{
											m_pChunk->ChunkColor = m_pShell->m_ChunkList[i]->ChunkColor;
											m_pChunk->HostFamily = m_pShell->m_ChunkList[i]->HostFamily;
										}
							}

						
						for(i = 0; i < m_pShell->m_PartialHosts.size(); i++)
							for(j = 0; j < m_pChunk->Hosts.size(); j++)
								if(m_pShell->m_PartialHosts[i].S_addr == m_pChunk->Hosts[j].S_addr)
								{
									m_pChunk->ChunkColor = RGB(0, 255, 0);
									m_pChunk->HostFamily = 0;
								}
					}
				}

				// Verification failed, decide what to do
				else
				{	
					TRACE(m_pShell->m_Name + _T(": Resume Failed at ") + DWrdtoStr(m_ResumeStart) +_T( "\n"));

					IP  ResumeHost;
					int ResumeFamily = m_pShell->HostFamilyAge + 1;
					int i, j;
					
					// Host now not associated with chunk
					if(m_pChunk)
					{
						ResumeHost = m_pChunk->Hosts.back();
						m_pChunk->Hosts.pop_back();
					}
					else
					{
						ResumeHost = m_pShell->m_PartialHosts.back();
						m_pShell->m_PartialHosts.pop_back();
					}

					GetRealParam()->Alive = true;
					

					// Find what family host is part of
					for(i = 0; i < m_pShell->m_ChunkList.size(); i++)
						for(j = 0; j < m_pShell->m_ChunkList[i]->Hosts.size(); j++)
							if(m_pShell->m_ChunkList[i]->Hosts[j].S_addr == ResumeHost.S_addr)
								if(m_pShell->m_ChunkList[i]->HostFamily < ResumeFamily)
									ResumeFamily = m_pShell->m_ChunkList[i]->HostFamily;

					
					int HostFamily = (m_pChunk) ? m_pChunk->HostFamily : 0;
					
					INT64 ResumeFamilySize = m_pShell->ChunkFamilySize(ResumeFamily);
					INT64 HostFamilySize   = m_pShell->ChunkFamilySize(HostFamily);
					
					
					if(HostFamily == ResumeFamily)
					{
						m_pShell->SetCorrupt(ResumeHost, true);
					}
					else if(ResumeFamilySize > HostFamilySize)
					{
						m_pShell->TrashFamily(HostFamily);
					}
					else
					{
						m_pShell->SetCorrupt(ResumeHost, true);
						m_pShell->TrashFamily(ResumeFamily);
					}


					Close();
					return;
				}

				// Copy extra data to the buffer
				if(m_ResumeStart > *pBytesCompleted)
				{
					INT64 nOffSet = nSize - (m_ResumeStart - *pBytesCompleted);

					try
					{
						m_pFile->Write(pBuff + nOffSet, nSize - nOffSet);
					
						*pBytesCompleted += nSize - nOffSet;
						m_dwSecBytes     += nSize - nOffSet;
					}
					catch(CFileException* e)
					{
						SetError(GetFileError(e));
						e->Delete();
						Close();
						return;
					}
				}

				m_ResumeStart = 0;
			}
		}

		// Tranfering begun
		else
		{
			if(nSize > RECEIVE_BUFF)
			{
				nSize = RECEIVE_BUFF;
				TRACE(_T("** RECEIVE_BUFF overrun\n"));
			}

			try
			{
				m_pFile->Write(pBuff, nSize);

				*pBytesCompleted	+= nSize;
				m_dwSecBytes		+= nSize;
			}
			catch(CFileException* e)
			{
				SetError(GetFileError(e));
				e->Delete();
				Close();
				return;
			}
		}
	}
	else 
	{
		SetError("File Error");
		Close();
	}

	if(!m_pShell->m_UpdatedInSecond)
		m_pShell->m_UpdatedInSecond = true;
	
	
	// If chunk completed
	if(m_pChunk && m_pChunk->BytesCompleted >= m_pChunk->ChunkLength)
	{
		// added by moonknit
		m_pChunk->bComplete = true;

		if(m_KeepAlive)
		{
			GetRealParam()->Handshake = _T("");
			m_Header = _T("");

			SendRequest(false);

			m_pShell->CheckCompletion();
			m_pShell->m_BackupTrigger = true;

			return;
		}
		
		Close();

		GetRealParam()->Alive = true;
		GetRealParam()->RetryWait = 0;

	}

	// If partial completed
	else if(!m_pChunk && m_pShell->m_BytesCompleted >= m_pShell->m_StopPos)
	{
		// added by moonknit
		m_pShell->m_bComplete = true;
		if(m_KeepAlive)
		{
			GetRealParam()->Handshake = _T("");
			m_Header = _T("");

			SendRequest(false);

			m_pShell->CheckCompletion();
			m_pShell->m_BackupTrigger = true;

			return;
		}
		
		Close();

		GetRealParam()->Alive = true;
		GetRealParam()->RetryWait = 0;
	}

	// If download section completed
	else if(m_KeepAlive && *pBytesCompleted == m_PausePos)
	{
		GetRealParam()->Handshake = _T("");
		m_Header = _T("");

		SendRequest(true);
		return;
	}
}

/**************************************************************************
 * method CGnuDownload::DownloadPost
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-03
 *
 * @Description
 * Post 데이터를 받아서 기록한다.
 *
 * @Parameters
 * (in byte*) pBuff - 받아온 버퍼
 * (in int) nSize - 받아온 버퍼의 크기
 *
 * @Result
 **************************************************************************/
void CGnuDownload::DownloadPost(byte* pBuff, int nSize)
{
	INT64* pBytesCompleted = NULL;

	pBytesCompleted = &m_pShell->m_BytesCompleted;

	// Tranfering begun
	// no Resume
	if(nSize > RECEIVE_BUFF)
	{
		nSize = RECEIVE_BUFF;
		TRACE(_T("** RECEIVE_BUFF overrun\n"));
	}

	try
	{
		m_pShell->m_PostXML += CString((LPCTSTR) pBuff, nSize/sizeof(TCHAR));

		*pBytesCompleted	+= nSize;
		m_dwSecBytes		+= nSize;
	}
	catch(CFileException* e)
	{
		SetError(GetFileError(e));
		e->Delete();
		Close();
		return;
	}

	if(!m_pShell->m_UpdatedInSecond)
		m_pShell->m_UpdatedInSecond = true;
	
	
	// If Post completed
	if(m_pShell->m_BytesCompleted >= m_pShell->m_FileLength)
	{
		// added by moonknit
		m_pShell->PostCompletion();
		if(!theApp.m_spGD->m_Control.CheckNeeds(this))
		{
			Close();
		}
	}
}

void CGnuDownload::StatusUpdate(DWORD Status)
{
	m_nSecsDead = 0;

	m_Status = Status;

	// RECEIVING 상태가 되면 TryCnt를 줄인다.
	if(Status != TRANSFER_RECEIVING) return;

	Result* pResult = GetRealParam();
	if(pResult && pResult->Trying)
	{
		pResult->Trying = false;
		m_pShell->DecreaseTryCnt();
	}
}

void CGnuDownload::SetError(CString Error)
{
	GetRealParam()->Error = Error;
}

Result* CGnuDownload::GetRealParam()
{
	for(int i = 0; i < m_pShell->m_Queue.size(); i++)
		if(m_pShell->m_Queue[i].Host.S_addr == m_Params.Host.S_addr && m_pShell->m_Queue[i].Port == m_Params.Port)
			return &m_pShell->m_Queue[i];

	return dumbResult;
}

void CGnuDownload::Timer()
{
	// Bandwidth
	if(m_nSecNum < 60)
		m_nSecNum++;
	if(m_nSecPos == 60)
		m_nSecPos = 0;

	// Bytes
	m_dwTotalBytes			-= m_dwAvgBytes[m_nSecPos];
	m_dwAvgBytes[m_nSecPos]	 = m_dwSecBytes;
	m_dwTotalBytes			+= m_dwSecBytes;

	// 평균 속도 계산
	m_Speed = 0;
	for ( int i = 0 ; i < m_nSecNum ; i++) {
		m_Speed += m_dwAvgBytes[i];
	}
	m_Speed = m_Speed / m_nSecNum;

	Result* pResult = NULL;

	// Time out downloads
	// 한곳에서 여러개의 자료 다운로드시 재연결 상태로 빠지는 경우
	// 이곳에서 타임아웃이 발생 ^^
	if(m_Status == TRANSFER_CONNECTING)
	{
		pResult = GetRealParam();
		// Active Host 상태 확인..
		ActiveHost host;
		host.HostIP.S_addr = pResult->Host.S_addr;
		host.Port = pResult->Port;
		host.FileName = pResult->NameLower;
		int ret = theApp.m_spGD->m_Control.FindActiveHost( host );
		if( ret == 2 )	// 현재 다른 파일을 다운로드 중이어서 발생한 Timeout
		{
			pResult->nConnectFailCount = 0;
			pResult->Alive = true;
			if( m_pShell->m_Waiting == true )
				m_pShell->m_Waiting = false;
		}
		else if(m_nSecsDead > TRANSFER_TIMEOUT*2) //16 Sec
		{
			// ret  : 0 => 소켓 연결이 안된 상태임, 1 => 다운로드 도중에 전송 중단.
			theApp.m_spGD->m_Control.Route_LocalPush(m_Params);

			SetError("Connect Timed Out (push sent)");

			++(pResult->nConnectFailCount);
			m_pShell->DecreaseTryCnt();
			pResult->RetryWait = RETRY_WAIT;			// 연결 실패한 것은 잠시 쉰다.
			pResult->Trying = false;

			Close();
		}	
	}

	else if(m_Status == TRANSFER_CONNECTED)
	{
		m_nSecsDead++;

		if(m_nSecsDead > TRANSFER_TIMEOUT)
		{
			theApp.m_spGD->m_Control.Route_LocalPush(m_Params);

			SetError("No Response (push sent)");

			Close();
		}
	}

	else if(m_Status == TRANSFER_RECEIVING)
	{
		if(ReceivePending)
			OnReceive(0);

		// Check for dead transfer
		if(m_dwSecBytes == 0)
		{
			m_nSecsDead++;

			if(m_nSecsDead > 10)
			{
				SetError("No Response Receiving");

				Close();
			}
		}
		else
			m_nSecsDead = 0;

		if(theApp.m_spGD->m_MinDownSpeed != -1 && m_nSecNum)
		{
			// Check if its under the bandwidth limit
			if((float)m_dwSecBytes / (float)1024 < theApp.m_spGD->m_MinDownSpeed)
				m_nSecsUnderLimit++;
			else
				m_nSecsUnderLimit = 0;

			if(m_nSecsUnderLimit > 15)
			{
				SetError("Below Minimum Speed");
				
				Close();
			}
		}
	}

	else 
	{
	}


	m_dwSecBytes = 0;
	m_nSecPos++;
}

bool CGnuDownload::HandleAltLocation(CString Handshake)
{
	CString FileHash;
	int urnPos = m_Header.Find(_T("x-gnutella-content-urn: urn:rfid:"));
	if (urnPos != -1)
	{
		FileHash = m_Header.Mid(urnPos + 33, m_Header.Find(_T("\r\n"), urnPos) - urnPos - 33);
		FileHash.Remove(' ');
		FileHash.MakeUpper();

		if(!m_pShell->m_Sha1Hash.IsEmpty() && FileHash != m_pShell->m_Sha1Hash)
		{
			SetError("Incorrect Hash");
			GetRealParam()->Alive = false;
			Close();
			return false;
		}
	
		// Read alt locations the host gives us
		int altpos = 0, endpos = 0;
		for (;;)
		{
			altpos = m_Header.Find(_T("alt-location:"), altpos);
			
			if (altpos == -1)
				break;
			
			altpos = altpos + 13; 			
			endpos = m_Header.Find(_T("\r\n"), altpos);

			CString AltLocString = m_Header.Mid(altpos, endpos - altpos);
		
			m_pShell->AddAltLocation(AltLocString);

			altpos = endpos;
		}
	

		// Add this host to the alt location list
		if(!m_Push)
		{
			AltLocation AltLoc;

			AltLoc.HostPort.Host = IPtoStr(m_Params.Host);
			AltLoc.HostPort.Port = m_Params.Port;
			AltLoc.HostPort.Time = CTime::GetCurrentTime() - LocalTimeZone();
			AltLoc.Index         = m_Params.FileIndex;
			AltLoc.Name          = m_Params.Name;
			AltLoc.Sha1Hash      = FileHash;
		
			m_pShell->AddAltLocation(AltLoc);						
		}
	}

	return true;
}


/*
** NAME    **: GetTotalDownloadSize()
** PARAM   **: void
** RETURN  **: DWORD
** DESC    **: 다운로드 받은 전체 크기를 얻는다.
** by bemlove at 2003-02-06, VER : 1.0, COMMENTS: ORG 
*/
INT64 CGnuDownload::GetTotalDownloadSize()
{
	INT64 size=0;

	if(m_pChunk)
		size= m_pChunk->BytesCompleted;
	else
		size= m_pShell->m_BytesCompleted;

	return size;
}//end of GetTotalDownloadSize()


/*
** NAME    **: GetDownloadSize()
** PARAM   **: void
** RETURN  **: DWORD
** DESC    **: 소켓이 살아있는 동안 다운로드한 사이즈
** by bemlove at 2003-02-06, VER : 1.0, COMMENTS: ORG 
*/
INT64 CGnuDownload::GetDownloadSize()
{
	INT64 size = 0;
	if( m_pChunk )		// 청크 다운로드 중이었을 경우
	{
		size = GetTotalDownloadSize() - m_pChunk->PreCompletedBytes;
	}
	else				// 메인 다운로드 중이었을 경우..
	{
		if( m_pShell->m_StartBytes == 0 )
			size = m_pShell->m_BytesCompleted ;
		else
			// modified by moonknit 2005-10-18
			//size = m_pShell->m_BytesCompleted - m_pShell->m_StopPos;
			size = m_pShell->m_BytesCompleted - m_pShell->m_StartBytes;
	}

	
	if( size < 0 || size > m_pShell->m_FileLength )
		size = 0;
	return size;
}//end of GetDownloadSize();
