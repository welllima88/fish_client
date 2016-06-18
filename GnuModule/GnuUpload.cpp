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
#include "GnuUploadShell.h"
#include "GnuUpload.h"
#include "GnuDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFISHApp theApp;

UINT UploadFile(LPVOID pVoidUpload);
UINT UploadPost(LPVOID pVoidUpload);

IMPLEMENT_DYNCREATE(CGnuUpload, CAsyncSocket)

CGnuUpload::CGnuUpload()
{
	CGnuUpload(NULL);
}

CGnuUpload::CGnuUpload(CGnuUploadShell* pShell)
{
	m_pShell			= pShell;

	m_DataSending		= false;
	m_BytesRead			= 0;
	m_BytesSent			= 0;

	// Clean Up	
	m_pUploadThread		= NULL;
	m_ThreadRunning		= false;
}

CGnuUpload::~CGnuUpload()
{
	// Flush receive buffer
	byte pBuff[4096];
	while(Receive(pBuff, 4096) > 0)
		;

	Close();
	// let thread die
	if(m_ThreadRunning)
	{
		Sleep(1000);
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuUpload, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuUpload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CGnuUpload member functions

void CGnuUpload::OnConnect(int nErrorCode) 
{
	if(nErrorCode)
	{
		return;
	}

	CString HttpGiv = _T("GIV ") + DWrdtoStr(m_pShell->m_FileIndex) 
		+ _T(":") + GuidtoStr(theApp.m_spGD->m_Control.m_ClientID) + _T("/")+ m_pShell->m_FileName + _T("\n\n");

	Send(HttpGiv, HttpGiv.GetLength() * sizeof(TCHAR));

	m_pShell->StatusUpdate(TRANSFER_CONNECTED);

	m_pShell->m_Handshake  = _T("");
	m_pShell->m_Handshake += HttpGiv;
	
	CAsyncSocket::OnConnect(nErrorCode);
}

void CGnuUpload::OnReceive(int nErrorCode) 
{
	byte* pBuff = new byte[6000];

	DWORD dwBuffLength = Receive(pBuff, 4096);

	switch (dwBuffLength)
	{
	case 0:
		m_pShell->m_Error = _T("Bad Push");
		Close();
		delete [] pBuff;
		return;
		break;
	case SOCKET_ERROR:
		m_pShell->m_Error = _T("Bad Push");
		Close();
		delete [] pBuff;
		return;
		break;
	}

	pBuff[dwBuffLength] = 0;
	CString Header(pBuff);

	// Check if handshake needs to be reset (for multiple get requests)
	if(m_GetRequest.IsEmpty())
		if(m_pShell->m_Handshake.Find(_T("GET /get/")) != -1)
			m_pShell->m_Handshake = _T("");

	m_pShell->m_Handshake += Header;
	m_GetRequest += Header;

	// New Upload
	if(m_GetRequest.Find(_T("\r\n\r\n")) != -1)
	{
		if(m_GetRequest.Find(_T("GET /get/")) == 0)
		{
			// Get Node info
			CString Host;
			UINT    nPort;
			GetPeerName(Host, nPort);
			
			// Set Variables
			m_pShell->m_Host = StrtoIP(Host);
			m_pShell->m_Port = 0;

			if(m_pShell->VerifyData(m_GetRequest))
			{
				m_pShell->m_Socket->m_GetRequest = "";
//				theApp.m_spGD->m_Control.TransferMessage(UPLOAD_UPDATE, (WPARAM) m_pShell);
			}
			else
			{
				m_pShell->m_FileIndex = -1;
			}

		}
		else
		{
			m_pShell->m_Error = _T("Bad Push");
			Close();
		}

		// Reset get request
		m_GetRequest = _T("");
	}

	delete [] pBuff;

	CAsyncSocket::OnReceive(nErrorCode);
}

void CGnuUpload::Send_HttpOK()
{	
	if(m_ThreadRunning || m_pUploadThread || m_pShell->m_Socket != this)
		return;

	int EndPos         = m_pShell->m_FileLength;
	m_pShell->m_BytesTotalSend = 0;

	CString Version = GNUCLEUS_VERSION;

	CString HttpOK =  _T("HTTP 100 OK\r\n");
			HttpOK += _T("Server: MyNapster ") + Version + _T("\r\n");
			HttpOK += _T("Content-type:application/binary\r\n");
			
			HttpOK += _T("Accept-Ranges: bytes\r\n");
			HttpOK += _T("Content-Range: bytes=") + DWrdtoStr(0) 
				+ _T("-") + DWrdtoStr(EndPos - 1) 
				+ _T("/") + DWrdtoStr(m_pShell->m_FileLength) + _T("\r\n");
			
			HttpOK += _T("Content-Length: ") + DWrdtoStr(m_pShell->m_FileLength) + _T("\r\n");
		
			HttpOK += _T("\r\n");


	Send(HttpOK, HttpOK.GetLength() * sizeof(TCHAR));

	m_pShell->StatusUpdate(TRANSFER_SENDING);
	m_pShell->m_Handshake += HttpOK;

	m_ThreadRunning = true;
	m_pUploadThread = AfxBeginThread(UploadFile, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
//	AssignThreadToCPU(m_pUploadThread, CPU_0); 
	m_pUploadThread->ResumeThread();
}

/**************************************************************************
 * method CGnuUpload::Send_PostOK
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-02
 *
 * @Description
 * Post 전달의 준비가 되어있음을 알린다.
 *
 * @Parameters
 *
 * @Result
 **************************************************************************/
void CGnuUpload::Send_PostOK()
{
	if(m_ThreadRunning || m_pUploadThread || m_pShell->m_Socket != this)
		return;

	int EndPos         = m_pShell->m_FileLength;
	INT64				BytesCompleted = 0;

	CString Version = GNUCLEUS_VERSION;

	CString HttpOK =  _T("HTTP 100 OK\r\n");
			HttpOK += _T("Server: MyNapster ") + Version + _T("\r\n");
			HttpOK += _T("Content-type:application/binary\r\n");
			
			HttpOK += _T("Accept-Ranges: bytes\r\n");
			HttpOK += _T("Content-Range: bytes=") + DWrdtoStr(BytesCompleted) 
				+ _T("-") + DWrdtoStr(EndPos - 1) 
				+ _T("/") + DWrdtoStr(m_pShell->m_FileLength) + _T("\r\n");
			
			HttpOK += _T("Content-Length: ") + DWrdtoStr(EndPos - BytesCompleted) + _T("\r\n");

			if (!m_pShell->m_Sha1Hash.IsEmpty())
			{
				HttpOK += _T("X-Gnutella-Content-URN: urn:rfid:") + m_pShell->m_Sha1Hash + _T("\r\n");
			}
			
			HttpOK += _T("\r\n");

	Send(HttpOK, HttpOK.GetLength() * sizeof(TCHAR));

	m_pShell->StatusUpdate(TRANSFER_SENDING);
	m_pShell->m_Handshake += HttpOK;

	m_ThreadRunning = true;
	m_pUploadThread = AfxBeginThread(UploadPost, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_pUploadThread->ResumeThread();
}

void CGnuUpload::Send_HttpBusy()
{
	CString Version = GNUCLEUS_VERSION;

	CString Http503 =  _T("HTTP 503\r\n";)
			Http503 += _T("Server: MyNapster ") + Version + _T("\r\n");
			Http503 += _T("Content-type:text/html\r\n");
			if (!m_pShell->m_Sha1Hash.IsEmpty())
			{
				Http503 += _T("X-Gnutella-Content-URN: urn:rfid:") + m_pShell->m_Sha1Hash + _T("\r\n");
			}
			Http503 += _T("\r\n");
			Http503 += _T("<HTML>\r\n");
			Http503 += _T("<HEAD><TITLE>503 Server Busy</TITLE></HEAD>\r\n");
			Http503 += _T("<BODY>\r\n");
			Http503 += _T("<H1>Server Busy</H1>\r\n");
			Http503 += _T("This server's upload max has been met, try again later.\r\n");
		 	Http503 += _T("</BODY>\r\n");
			Http503 += _T("</HTML>\r\n\r\n");
			

	Send(Http503, Http503.GetLength() * sizeof(TCHAR));

	m_pShell->m_Error = _T("No Upload Slots");
	m_pShell->m_Handshake += Http503;

	Close();
}

void CGnuUpload::Send_HttpFailed()
{
	CString Version = GNUCLEUS_VERSION;

	CString Http412 =  _T("HTTP 503\r\n");
			Http412 += _T("Server: MyNapster ") + Version + _T("\r\n");
			Http412 += _T("Content-type:text/html\r\n\r\n");
			Http412 += _T("<HTML>\r\n");
			Http412 += _T("<HEAD><TITLE>412 Precondition Failed</TITLE></HEAD>\r\n");
			Http412 += _T("<BODY>\r\n");
			Http412 += _T("<H1>Already Downloading File</H1>\r\n");
			Http412 += _T("You may only download one file at a time from this server.\r\n");
			Http412 += _T("</BODY>\r\n");
			Http412 += _T("</HTML>\r\n\r\n");

	Send(Http412, Http412.GetLength() * sizeof(TCHAR));

	m_pShell->m_Error = _T("Host Already Downloading");
	m_pShell->m_Handshake += Http412;

	Close();
}


void CGnuUpload::Send_HttpNotFound()
{
	CString Version = GNUCLEUS_VERSION;

	CString Http404 =  _T("HTTP 404\r\n");
			Http404 += _T("Server: MyNapster ") + Version + _T("\r\n");
			Http404 += _T("Content-type:text/html\r\n\r\n");
			Http404 += _T("<HTML>\r\n");
			Http404 += _T("<HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n");
			Http404 += _T("<BODY>\r\n");
			Http404 += _T("<H1>Not Found</H1>\r\n");
			Http404 += _T("The requested file ") + m_pShell->m_FileName + _T(" was not found on this server.\r\n");
			Http404 += _T("</BODY>\r\n");
			Http404 += _T("</HTML>\r\n\r\n");

	Send(Http404, Http404.GetLength() * sizeof(TCHAR));

	m_pShell->m_Error = _T("File Not Found");
	m_pShell->m_Handshake += Http404;

	Close();
}



int CGnuUpload::Send(const void* lpBuf, int nBufLen, int nFlags) 
{
	int Command = CAsyncSocket::Send(lpBuf, nBufLen, nFlags);
	
	if(Command != SOCKET_ERROR)
	{
		m_DataSending = true;
		m_pShell->m_nSendTotalBytes += nBufLen;
	}

	return Command;
}


void CGnuUpload::OnSend(int nErrorCode) 
// keep buffer full
{
	if(nErrorCode)
	{
		// 전송 오류
	}

	m_DataSending = false;

	m_CanWrite.SetEvent();

	CAsyncSocket::OnSend(nErrorCode);
}

void CGnuUpload::OnClose(int nErrorCode) 
{
	if(m_pShell->m_Error == _T(""))
		m_pShell->m_Error = _T("Remotely Canceled");

	if(nErrorCode)
	{
		TCHAR	szError[1024];
		PrintError(nErrorCode, szError);
//		TextLog("OnClosed : File Name = %s, %d = %s", m_pShell->m_FileName, nErrorCode, szError);
	}

	Close();
		
	CAsyncSocket::OnClose(nErrorCode);
}

void CGnuUpload::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		AsyncSelect(0);
		ShutDown(1);

		CAsyncSocket::Close();
	}


	// Close file
	m_pShell->m_file.Abort();
	m_pShell->StatusUpdate(TRANSFER_CLOSED);

	m_CanWrite.SetEvent();
	m_MoreBytes.SetEvent();

	// written by moonknit 2005-09-07
	if(!m_pShell->m_bTransfered)
		m_pShell->m_bDeleteShell = TRUE;
	// --
}

void CGnuUpload::Timer()
{

}

/**************************************************************************
 * function UploadFile
 *
 * written by ??
 *
 * @History
 * created before 2006-01-02
 *
 * @Description
 * File Uploading을 수행하기 위한 Thread function이다
 *
 * @Parameters
 * (in LPVOID) pVoidUpload - Thread를 실행한 CGnuUpload 객체의 포인터
 *
 * @Result
 **************************************************************************/
UINT UploadFile(LPVOID pVoidUpload)
{
	//TRACE0("*** Upload Thread Started\n");
	CGnuUpload*      pSock  = (CGnuUpload*) pVoidUpload;
	CGnuUploadShell* pShell = pSock->m_pShell;

	int  AttemptSend;
	
	int  BytesRead = 0;
	byte *pBuffer    = new byte[4096];

	pShell->m_bTransfered = FALSE;

	while(pShell->m_Status == TRANSFER_SENDING)
		if((pShell->m_StopPos && pShell->m_BytesCompleted < pShell->m_StopPos) ||
		   pShell->m_BytesCompleted < pShell->m_FileLength)
		{
			// Send chunk of bytes read from file
			if(pSock->m_BytesSent < BytesRead)
			{
				AttemptSend = pSock->Send(pBuffer + pSock->m_BytesSent, BytesRead - pSock->m_BytesSent);
					
				if(AttemptSend == SOCKET_ERROR)
				{
					if(pSock->GetLastError() != WSAEWOULDBLOCK)
					{
						pShell->m_Error  = _T("Remotely Canceled");  // Make more descriptive
						pShell->m_Status = TRANSFER_CLOSED;
					}
					else
					{
						WaitForSingleObject((HANDLE) pSock->m_CanWrite, INFINITE);
						pSock->m_CanWrite.ResetEvent();

						if(pShell->m_Status != TRANSFER_SENDING)
							break;
					}
				}
				else
				{
					if(!pShell->m_bTransfered)
					{
						pShell->m_bTransfered = TRUE;
					}
					pSock->m_BytesSent  += AttemptSend;
					pShell->m_dwSecBytes += AttemptSend;
				}

				if(!pShell->m_UpdatedInSecond)
					pShell->m_UpdatedInSecond = true;
			}

			// Get next chunk of bytes from file
			else
			{
				pShell->m_BytesCompleted += pSock->m_BytesSent;
				pSock->m_BytesSent       = 0;
				BytesRead	             = 0;

				// If bandwidth limits are not turned on
				if(!theApp.m_spGD->m_BandwidthUp)
				{
					try
					{
						if(pShell->m_file.m_hFile ==(UINT) CFile::hFileNull) //
						{
							BytesRead = -1;
						}
						else
						{
							if(pShell->m_StopPos && pShell->m_StopPos - pShell->m_BytesCompleted < 4096)
								BytesRead = pShell->m_file.Read((byte*) pBuffer, pShell->m_StopPos - pShell->m_BytesCompleted);
							else	
								BytesRead = pShell->m_file.Read((byte*) pBuffer, 4096);
						}
					}
					catch(CFileException* e)
					{
						pShell->m_Error = GetFileError(e);
						e->Delete();
						pShell->m_Status = TRANSFER_CLOSED;
					}

					if(!BytesRead)
					{
						pShell->m_Status = TRANSFER_CLOSED;
					}
				}
				// And if they are
				else
				{
					if(pShell->m_AllocBytes <= 0)
					{
						pShell->m_AllocBytes = 0;

						WaitForSingleObject((HANDLE) pSock->m_MoreBytes, INFINITE);
						pSock->m_MoreBytes.ResetEvent();

						if(pShell->m_Status != TRANSFER_SENDING)
							break;
					}

					int ReadSize = (pShell->m_AllocBytes > 4096) ? 4096 : pShell->m_AllocBytes;
					
					try
					{
						if(ReadSize > 0)
						{
							if(pShell->m_file.m_hFile == CFile::hFileNull)
							{
								BytesRead = -1;
							}
							else
							{
								if(pShell->m_StopPos && pShell->m_StopPos - pShell->m_BytesCompleted < ReadSize)
									BytesRead = pShell->m_file.Read((byte*) pBuffer, pShell->m_StopPos - pShell->m_BytesCompleted);
								else	
									BytesRead = pShell->m_file.Read((byte*) pBuffer, ReadSize);
							}
						}
						else if(ReadSize < 0)
						{
							pShell->m_AllocBytes = 0;
						}
						
					}
					catch(CFileException* e)
					{
						pShell->m_Error = GetFileError(e);
						e->Delete();
						pShell->m_Status = TRANSFER_CLOSED;
					}
					
					pShell->m_AllocBytes -= BytesRead;

					if(!BytesRead && ReadSize)
					{
						pShell->m_Status = TRANSFER_CLOSED;
					}
				}

				Sleep(1);
			}
		}
		else
		{
			pShell->m_BytesCompleted += pSock->m_BytesSent;
			

			if(pShell->m_StopPos && pShell->m_BytesCompleted >= pShell->m_StopPos)
			{
				if(pShell->m_KeepAlive)
				{
					pShell->m_Status = TRANSFER_CONNECTED;
				}
				else
				{
					pShell->m_Status = TRANSFER_CLOSED;
				}
			}
			else
			{
				pShell->m_Status = TRANSFER_CLOSED;
			}


			pSock->m_BytesSent = 0;
		}
								
	delete [] pBuffer;


	// 스레드에서 직접 UI로 접근할 수 없다.
	pShell->m_Status = TRANSFER_CLOSED;
	pShell->SetStatusUpdated();
	// -- 

	// Make sure shell still exists

	std::list<CGnuUploadShell*>::iterator it;
	for(it = theApp.m_spGD->m_Control.m_UploadList.begin(); it != theApp.m_spGD->m_Control.m_UploadList.end(); ++it)
		if((*it) == pShell)
		{
			pShell->IncreaseFileUPCount();
			pShell->m_BytesCompleted += pSock->m_BytesSent;
			pShell->m_UpdatedInSecond = true;

			if(pShell->m_Socket)
				pSock->m_ThreadRunning = false;

			break;
		}

	pSock->m_pUploadThread = NULL;

	return 0;
}

/**************************************************************************
 * function UploadPost
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-02
 *
 * @Description
 * Post Uploading을 수행하기 위한 Thread function이다
 *
 * @Parameters
 * (in LPVOID) pVoidUpload - Thread를 실행한 CGnuUpload 객체의 포인터
 *
 * @Result
 **************************************************************************/
UINT UploadPost(LPVOID pVoidUpload)
{
	//TRACE0("*** Upload Thread Started\n");
	CGnuUpload*      pSock  = (CGnuUpload*) pVoidUpload;
	CGnuUploadShell* pShell = pSock->m_pShell;

	int  AttemptSend;
	
	int  BytesRead = 0;
	int	 SendPos = 0;
	byte* pBuffer    = new byte[4096];

	pShell->m_bTransfered = FALSE;

	while(pShell->m_Status == TRANSFER_SENDING)
		if(pShell->m_BytesCompleted < pShell->m_FileLength)
		{
			// Send chunk of bytes read from file
			if(pSock->m_BytesSent < BytesRead)
			{
				AttemptSend = pSock->Send(pBuffer + pSock->m_BytesSent, BytesRead - pSock->m_BytesSent);
					
				if(AttemptSend == SOCKET_ERROR)
				{
					if(pSock->GetLastError() != WSAEWOULDBLOCK)
					{
						pShell->m_Error  = _T("Remotely Canceled");  // Make more descriptive
						pShell->m_Status = TRANSFER_CLOSED;
					}
					else
					{
						WaitForSingleObject((HANDLE) pSock->m_CanWrite, INFINITE);
						pSock->m_CanWrite.ResetEvent();

						if(pShell->m_Status != TRANSFER_SENDING)
							break;
					}
				}
				else
				{
					if(!pShell->m_bTransfered)
					{
						pShell->m_bTransfered = TRUE;
					}
					pSock->m_BytesSent  += AttemptSend;
					pShell->m_dwSecBytes += AttemptSend;
				}

				if(!pShell->m_UpdatedInSecond)
					pShell->m_UpdatedInSecond = true;
			}

			// Get next chunk of bytes from file
			else
			{
				pShell->m_BytesCompleted += pSock->m_BytesSent;
				pSock->m_BytesSent       = 0;
				BytesRead	             = 0;

				// If bandwidth limits are not turned on
				if(!theApp.m_spGD->m_BandwidthUp)
				{
					try
					{
						if(pShell->m_PostXML.IsEmpty() 
							|| SendPos/sizeof(TCHAR) > pShell->m_PostXML.GetLength())
						{
							BytesRead = 0;
						}
						else
						{
							int CpySize = 0;

							if(pShell->m_FileLength - SendPos > 4096)
								CpySize = 4096;
							else
								CpySize = pShell->m_FileLength - SendPos;

							memcpy(pBuffer, ((byte*) (LPCTSTR) pShell->m_PostXML) + SendPos, CpySize);
//							_tcscpy(szBuffer, (LPCTSTR) pShell->m_PostXML.Mid(SendPos/sizeof(TCHAR), CpySize/sizeof(TCHAR)));
//							memcpy(pBuffer, &szBuffer, CpySize);
							BytesRead = CpySize;
							SendPos+=CpySize;
						}
					}
					catch (CException&)
					{
					}

					if(!BytesRead)
					{
						pShell->m_Status = TRANSFER_CLOSED;
					}
				}
				// And if they are
				else
				{
					if(pShell->m_AllocBytes <= 0)
					{
						pShell->m_AllocBytes = 0;

						WaitForSingleObject((HANDLE) pSock->m_MoreBytes, INFINITE);
						pSock->m_MoreBytes.ResetEvent();

						if(pShell->m_Status != TRANSFER_SENDING)
							break;
					}

					int ReadSize = (pShell->m_AllocBytes > 4096) ? 4096 : pShell->m_AllocBytes;
					
					try
					{
						if(ReadSize > 0)
						{
							if(pShell->m_PostXML.IsEmpty() 
								|| SendPos/sizeof(TCHAR) > pShell->m_PostXML.GetLength())
							{
								BytesRead = -1;
							}
							else
							{
								int CpySize = 0;

								if(pShell->m_FileLength - SendPos > ReadSize)
									CpySize = ReadSize;
								else
									CpySize = pShell->m_FileLength - SendPos;

								memcpy(pBuffer, ((byte*) (LPCTSTR) pShell->m_PostXML) + SendPos, CpySize);
//								_tcscpy(szBuffer, (LPCTSTR) pShell->m_PostXML.Mid(SendPos/sizeof(TCHAR), CpySize/sizeof(TCHAR)));
//								memcpy(pBuffer, &szBuffer, CpySize);
								BytesRead = CpySize;
								SendPos += CpySize;
							}
						}
						else if(ReadSize < 0)
						{
							pShell->m_AllocBytes = 0;
						}
						
					}
					catch (CException&)
					{
					}
					
					pShell->m_AllocBytes -= BytesRead;

					if(!BytesRead && ReadSize)
					{
						pShell->m_Status = TRANSFER_CLOSED;
					}
				}

				Sleep(1);
			}
		}
		else
		{
			pShell->m_BytesCompleted += pSock->m_BytesSent;
		
			pShell->m_Status = TRANSFER_CLOSED;

			pSock->m_BytesSent = 0;
		}
								
	delete [] pBuffer;


	// 스레드에서 직접 UI로 접근할 수 없다.
	pShell->m_Status = TRANSFER_CLOSED;
	pShell->SetStatusUpdated();
	// -- 

	// Make sure shell still exists
	std::list<CGnuUploadShell*>::iterator it;
	for(it = theApp.m_spGD->m_Control.m_UploadList.begin(); it != theApp.m_spGD->m_Control.m_UploadList.end(); ++it)
		if((*it) == pShell)
		{
			pShell->IncreaseFileUPCount();
			pShell->m_BytesCompleted += pSock->m_BytesSent;
			pShell->m_UpdatedInSecond = true;

			if(pShell->m_Socket)
				pSock->m_ThreadRunning = false;

			break;
		}

	pSock->m_pUploadThread = NULL;

	return 0;
}
