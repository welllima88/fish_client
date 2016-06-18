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

#include "GnuUpload.h"
#include "GnuUploadShell.h"
#include "GnuDoc.h"

#include "../RSSDB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CFISHApp theApp;

CGnuUploadShell::CGnuUploadShell()
{
	m_Attempts			= 1;

	m_UpdatedInSecond	= false;

	m_ChangeTime		= CTime::GetCurrentTime();

	m_KeepAlive			= false;
	m_StopPos			= 0;
	m_BytesCompleted	= 0;
	m_FileLength		= 0;
	m_BytesTotalSend	= 0;
	m_BytesAllSend		= 0;
	m_Status			= TRANSFER_CLOSED;

	m_nSecsUnderLimit	= 0;
	m_nSecsDead			= 0;

	// Bandwidth
	for(int i = 0; i < 60; i++)
		m_dwAvgBytes[i] = 0;

	m_dwTotalBytes		= 0;
	m_dwSecBytes		= 0;
	
	m_nSecNum			= 0;
	m_nSecPos			= 0;

	m_AllocBytes		= 0;
	m_AllocBytesTotal	= 0;

	m_Socket			= NULL;

	m_Point				= 0;
	m_iIdentifyList		= 0;

	// 전송한 전체 바이트
	m_nSendTotalBytes	= 0;

	m_bTransfered		= FALSE;
	m_FileIndex			= -1;
	m_StatusUpdated		= FALSE;

	m_nOldState			= 0;
	m_nSendBytesForView	= 0;

	m_bDeleteShell		= FALSE;
}

CGnuUploadShell::~CGnuUploadShell()
{
	// shutdown upload thread
	StatusUpdate(TRANSFER_CLOSED);

	if(m_Socket)
	{
		delete m_Socket;
		m_Socket = NULL;
	}

	m_file.Abort();

	// 아래 코드는 종료할 때 수행할 필요가 없음
	if(theApp.IsClosing()) return;
		
	DecreaseCount();
}

void CGnuUploadShell::VerifyPostResult(CString xml)
{
	if(xml.IsEmpty())
	{
		m_Socket->Send_HttpNotFound();
	}
	else
	{
		m_PostXML = xml;
		m_FileLength = (xml.GetLength() + 1) * sizeof(TCHAR);
		m_Socket->Send_PostOK();
	}
}

void CGnuUploadShell::VerifyFileResult(CString FilePath, CString FileHash)
{
	// 기존에 열려있는 파일이 있다면 닫는다.
	m_file.Abort();

	m_FilePath = FilePath;

	if(!m_file.Open(m_FilePath, CFile::modeRead | CFile::shareDenyWrite))
	{
		m_Socket->Send_HttpNotFound();
		return;
	}

	if(!m_Sha1Hash.IsEmpty() && FileHash != m_Sha1Hash)
	{
		m_Socket->Send_HttpNotFound();
		return;
	}

	// Get file length
	try
	{
		m_FileLength = m_file.GetLength();
	}
	catch(CFileException* e)
	{
		m_Error = GetFileError(e);
		e->Delete();
		m_Socket->Send_HttpFailed();
		return;
	}

	if(m_BytesCompleted)
	{
		// 비정삭적인 range 값에대한 처리
		if(m_BytesCompleted > m_FileLength
			|| m_BytesCompleted > m_StopPos)
		{
			m_Socket->Send_HttpFailed();
			return;
		}

		try
		{
			m_file.Seek(m_BytesCompleted, CFile::begin);
		}
		catch(CFileException* e)
		{
			m_Error = GetFileError(e);
			e->Delete();
			m_Socket->Send_HttpFailed();
			return;
		}		
	}

	// If we are already uploading to this host, give a busy signal
	std::list<CGnuUploadShell*>::iterator it;
	for(it = theApp.m_spGD->m_Control.m_UploadList.begin(); it != theApp.m_spGD->m_Control.m_UploadList.end(); ++it)
	{
		if((*it)->m_Status == TRANSFER_CONNECTED ||
		   (*it)->m_Status == TRANSFER_SENDING)
			if((*it) != this && (*it)->m_Host.S_addr == m_Host.S_addr)
			{
				m_Socket->Send_HttpFailed();
				return;
			}
	}

	bool bReturn = false;

	// If there's an upload max
	if(theApp.m_spGD->m_MaxUploads)
	{
#ifndef PREMIUM_PRIORITY
		if(theApp.m_spGD->m_Control.CountUploading() < theApp.m_spGD->m_MaxUploads)
		{
			m_Socket->Send_HttpOK();
			bReturn = true;
		}
		else
		{
			m_Socket->Send_HttpBusy();
			return;
		}
#else
		// Step 1. Check upload connection size first. If the number of upload connection is lower than preference
		//		,upload process is allowed. Or proceed Step2.
		// Step 2. Check if the current user is a premium user (nPremiumLevel is 1).
		// Step 3. If the current user is a non-premium user, failed to upload.
		// Step 4. If the current user is a premium user, check if there is one or more non-premium user of the upload connection.
		//		When non-premium users exist, kick the non-premium user and proceed to upload. Or It is failed to upload.
		CGnuUploadShell *pUploadShell;
		int x = 0;

		while(true)
		{

			if(theApp.m_spGD->m_Control.CountUploading() < theApp.m_spGD->m_MaxUploads)
			{
				m_Socket->Send_HttpOK();
				bReturn = true;
				break;
			}
			else
			{
				pUploadShell = NULL;

				// 프리미엄 유저만이 타 유저를 밀어낼 수 있다.
				if(m_nPremiumLevel == 1)
				{
					pUploadShell = theApp.m_spGD->m_Control.GetNonPremiumUploadShell( &x, this );
				}

				// No more non-premium upload remains.
				// Give up to upload to the premium user.
				if(pUploadShell == NULL)
				{
					m_Socket->Send_HttpBusy();
					break;
				}
				else
				{
					// modified by moonknit 2005-08-09
					// UploadShell must not be removed by itself

					// kick the upload connection
					// maybe this can emit pointer error
					
					// theApp.m_spGD->m_Control.RemoveUploadShell(pUploadShell);
					pUploadShell->StatusUpdate(TRANSFER_CLOSED, TRUE);
					pUploadShell->DecreaseCount();
				}
			}
		}
#endif // PREMIUM_PRIORITY
	}
	else
	{
		m_Socket->Send_HttpOK();
		bReturn = true;
	}

	if(bReturn)
	{
		m_FileName = m_FileNameTemp;
		IncreaseCount();

		m_nSendBytesForView = 0;
	}
}

bool CGnuUploadShell::VerifyData(CString Handshake, int index, CString stype)
{
	m_FileIndex = -1;

	int Begin = 0;
	int End   = 0;

	if(!m_Socket)
		return false;

	CString lowHandshake = Handshake;
	lowHandshake.MakeLower();

	INT64 fileSize = 0;
	CString strFilesize = _T("");
	CString sFileName = _T("");

	// Get Index
	if(index == -1 || stype.IsEmpty())
	{
		TCHAR	sztype[128];
		_stscanf((LPCTSTR) lowHandshake, _T("get /get/%s /%ld/*\r\n"), &sztype, &m_FileIndex);
		stype = sztype;
	}
	else
	{
		m_FileIndex = index;
	}

	// CHECK TRANSFER TYPE
	if(stype.CompareNoCase(GDT_POST_S) == 0)
	{
		m_nType = GDT_POST;
	}
	else if(stype.CompareNoCase(GDT_FILE_S) == 0)
	{
		m_nType = GDT_FILE;
	}
	else
	{
		return false;
	}


	// Get first line from header
	CString FirstLine = lowHandshake.Mid(0, lowHandshake.Find(_T("\r\n")) );
	FirstLine.Replace(_T("%20"), _T(" "));

	// Filename
	Begin  = FirstLine.Find(_T("/"), 9) + 1;
	End    = FirstLine.Find(_T(" http/"), Begin);
	sFileName = FirstLine.Mid(Begin, End - Begin);

	m_FileNameTemp = sFileName;

	Begin = lowHandshake.Find(_T("filesize:"));
	fileSize = 0;
	if( Begin != -1 ) 
	{
		End = lowHandshake.Find(_T("user-agent:"));
		if( End != -1 ) 
		{
			strFilesize = lowHandshake.Mid( Begin, End-Begin-2 );
			_stscanf((LPCTSTR) strFilesize, _T("filesize:%u\r\n"), &fileSize ); 
			if( fileSize < 0 )
				fileSize = 0;
		}
	}

	m_FileLength = fileSize;

	// Look for keep alive
	m_KeepAlive = (lowHandshake.Find(_T("keep-alive\r\n")) != -1) ? true : false;

	if(sFileName.IsEmpty())
	{
		int nNamePos = m_FilePath.ReverseFind(_T('\\'));
		if(nNamePos != -1)
		{
			sFileName = m_FilePath.Mid(nNamePos+1);
		}
	}

	m_FileNameTemp = sFileName;
	
	// Check if file has a hash
	CString FileHash;
	int urnPos = lowHandshake.Find(_T("x-gnutella-content-urn: urn:rfid:"));
	if (urnPos != -1)
	{
		FileHash = lowHandshake.Mid(urnPos + 33, lowHandshake.Find(_T("\r\n"), urnPos) - urnPos - 33);
		FileHash.TrimLeft();
		FileHash.TrimRight();
		FileHash.MakeUpper();
		FileHash.MakeLower();
	}

	// Range
	Begin = lowHandshake.Find(_T("\r\nrange: bytes="));
	if(Begin > 0)
	{	
		CString sRange = lowHandshake.Mid(Begin);

		INT64 StopByte = 0;
		int converted = _stscanf((LPCTSTR) sRange, _T("\r\nrange: bytes=%ld-%ld\r\n")
			, &m_BytesCompleted, &StopByte);
		
		switch(converted)
		{
		case 1:
			m_StopPos = m_FileLength;
			break;
		case 2:
			m_StopPos = StopByte + 1;
			break;
		default:
			m_Socket->Send_HttpFailed();
			break;
		}
	}

	if(m_nType == GDT_POST)
	{
		theApp.m_spRD->IRequestUpload((LPVOID) this, m_FileIndex);
	}
	else if(m_nType == GDT_FILE)
	{
		// if you want to transfer file
		// then request here
	}

	return true;
}

void CGnuUploadShell::PushFile()
{
	// Set file name and size
	// 파일 가져오기
//	CSharedFileManager* pSFManager = theApp.GetSHFManager();
//	INT64 fileSize;
//	if(m_FileIndex == -1 
//		|| (pSFManager && !pSFManager->GetUploadFileInfo(m_FileIndex, m_FilePath, (INT64&) fileSize, m_Sha1Hash))
//		)
//	{	
//		m_Error = "File Not Found";
//		return;
//	}

	return;

	if(!m_file.Open(m_FilePath, CFile::modeRead))
	{
		m_Error = "File Not Found";
		return;
	}


	m_FileLength = m_file.GetLength();

	if( !theApp.m_spGD->m_Control.ConnectingSlotsOpen() )
	{ 
		m_Error = "Push Failed";
		return;
	}

	m_file.Abort();
	

	if(m_Socket)
		delete m_Socket;

	m_Socket = new CGnuUpload(this);

	if(!m_Socket->Create())
	{
		delete m_Socket;
		m_Socket = NULL;
		return;
	}

	CString ip;
	

//	if(!m_Socket->Connect(IPtoStr(m_Host), m_Port))
	if(!m_Socket->Connect(ip, m_Port))
	{
		if(m_Socket->GetLastError() != WSAEWOULDBLOCK)
		{
			m_Error = "Unable to Connect";
			StatusUpdate(TRANSFER_CLOSED);
			
			delete m_Socket;
			m_Socket = NULL;
			return;
		}
	}

	StatusUpdate(TRANSFER_PUSH);
}

void CGnuUploadShell::StatusUpdate(DWORD Status, BOOL bForce)
{
	m_nSecsDead = 0;
	bool bChanged = false;

	if(m_Status != Status || bForce)
	{
		bChanged = true;
	}

	m_ChangeTime = CTime::GetCurrentTime();

	if(bChanged)
	{
		m_Status = Status;

		// written by moonknit 2005-08-27
		// 반드시 화면의 갱신을 하기 위해서 과거 상태를 현재 상태와 다른 값으로 설정해야한다.
		m_nOldState = 0;

		//PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPLOADUPDATE, (WPARAM) this, NULL);

//		TRACE("CGnuUploadShell::StatusUpdate - TransferMessage\r\n");

		if(m_FileIndex != -1)
			theApp.m_spGD->m_Control.TransferMessage(UPLOAD_UPDATE, (WPARAM) this);
	}
}

void CGnuUploadShell::Timer()
{
//	TRACE("CGnuUploadShell::Timer() begin\r\n");
	if(m_nSecNum < 60)
		m_nSecNum++;
	if(m_nSecPos == 60)
		m_nSecPos = 0;

	// Bytes
	m_dwTotalBytes -= m_dwAvgBytes[m_nSecPos];
	m_dwAvgBytes[m_nSecPos] = m_dwSecBytes;
	m_dwTotalBytes += m_dwSecBytes;

	if(m_nSecNum) m_AvgSpeed = m_dwTotalBytes / m_nSecNum;

	if(m_StatusUpdated)
	{
		StatusUpdate(m_Status, TRUE);
		m_StatusUpdated = FALSE;
	}

	if(m_UpdatedInSecond)
	{
		if(m_bTransfered && m_FileIndex != -1)
		{
//			TRACE("CGnuUploadShell::Timer - TransferMessage\r\n");
			theApp.m_spGD->m_Control.TransferMessage(UPLOAD_UPDATE, (WPARAM) this);
		}
		m_UpdatedInSecond = false;
	}

	// Mininum Trasfer Speed Verification
	if(TRANSFER_CONNECTING == m_Status ||
	   TRANSFER_CONNECTED  == m_Status)
	{
		m_nSecsDead++;

		if(m_nSecsDead > 15)
		{
			m_Error     = "No Response";
			m_KeepAlive = false;

			StatusUpdate(TRANSFER_CLOSED);

//			TextLog("Minium Transfer Speed");

			if(m_Socket)
				m_Socket->Close();
		}
	}

	else if(TRANSFER_PUSH == m_Status)
	{
		if(	m_FileName == "Unknown" && !m_FileLength)
			PushFile();

		m_nSecsDead++;

		if(m_nSecsDead > 15)
		{
			m_Error = "Host Unreachable";
			StatusUpdate(TRANSFER_CLOSED);
			
			if(m_Socket)
				m_Socket->Close();
		}
	}

	else if(TRANSFER_SENDING == m_Status)
	{
		if(theApp.m_spGD->m_BandwidthUp && m_AllocBytes)
		{
			if(m_Socket)
				m_Socket->m_MoreBytes.SetEvent();
		}

		// Check for dead transfer
		if(m_dwTotalBytes == 0)
		{
			m_nSecsDead++;

			if(m_nSecsDead > 30)
			{
				m_Error     = "No Response";
				m_KeepAlive = false;

//				TextLog("No Response");

				if(m_Socket)
					m_Socket->Close();
			}
		}
		else
			m_nSecsDead = 0;

		if(theApp.m_spGD->m_MinUpSpeed > 0 && m_nSecNum)
		{
			// Check if its under the bandwidth limit
			if((float)m_dwSecBytes / (float)1024 < theApp.m_spGD->m_MinUpSpeed)
				m_nSecsUnderLimit++;
			else
				m_nSecsUnderLimit = 0;

			if(m_nSecsUnderLimit > 15)
			{	
				m_Error     = "Below Minimum Speed";
				m_KeepAlive = false;

//				TextLog((LPSTR) (LPCSTR) m_Error);

				if(m_Socket)
					m_Socket->Close();
			}
		}

		// If entire file sent
		if(m_StopPos && m_BytesCompleted >= m_StopPos)
		{
//			TextLog("File Completed : Stop Pos = %d, Completed = %d"
//				, m_StopPos, m_BytesCompleted);

			if(!m_KeepAlive && m_Socket)
				m_Socket->Close();
		}
		else if(m_FileLength <= m_BytesCompleted)
			if(m_Socket)
				m_Socket->Close();
	}

	m_dwSecBytesLast = m_dwSecBytes;

	m_dwSecBytes = 0;
	m_nSecPos++;

	// Check for completion
	if((m_StopPos && m_BytesCompleted >= m_StopPos) ||
	  (m_FileLength <= m_BytesCompleted))
	{
		theApp.m_spGD->m_HaveUploaded = true;
	}

	// 파일 전송상태 알림
	// PIPE : Tranfer File 9

	// Clean up Socket
	if(m_Status == TRANSFER_CLOSED)
	{
		if(m_Socket)
		{
			delete m_Socket;
			m_Socket = NULL;
		}
		
		m_file.Abort();
	}

//	TRACE("CGnuUploadShell::Timer() end\r\n");
}

bool operator > (const CGnuUploadShell &Transfer1, const CGnuUploadShell &Transfer2)
{
	if(Transfer1.m_ChangeTime > Transfer2.m_ChangeTime)
		return true;
	else
		return false;
}


//**************************************************************************************
// NAME :  ResetBytesTotalSend
// PARM :  void
// RETN :  DWORD
// DESC :  전송창에 전송항 전체 크기가 0으로 나오는 경우 다시 계산을 하여 결과 값 리턴.
// COMT : by bemlove 2003-03-03 오전 11:40:25 ver - 1. 0 
//**************************************************************************************
DWORD CGnuUploadShell::ResetBytesTotalSend()
{
	if( m_BytesTotalSend > 0 ) return m_BytesTotalSend;
	return m_BytesTotalSend = m_StopPos - m_BytesCompleted ;
}//end of ResetBytesTotalSend()


// written by moonknit 2005-05-13
// 현재 연결 개수는 Connection을 끊을 때 줄일 수도 있어야 한다.
void CGnuUploadShell::DecreaseCount()
{
	if(m_bCounted)
	{
		theApp.m_spGD->DecreaseCurUpload();
		m_bCounted = false;
	}
} 
// -- end CGnuUploadShell::DecreaseCount

// written by moonknit 2005-05-13
// 현재 연결 개수는 객체 생성시 늘이는 것이 아니라 실제 다운로드 작업이 시작될 때
// 늘여야 한다.
void CGnuUploadShell::IncreaseCount()
{
	if(!m_bCounted)
	{
		theApp.m_spGD->IncreaseCurUpload();
		m_bCounted = true;
	}
}
// -- end CGnuUploadShell::IncreaseCount

// written by moonknit 2005-06-20
// 공유파일 관리자에게 업로드 카운트를 증가시킨다.
void CGnuUploadShell::IncreaseFileUPCount()
{
//	CSharedFileManager* pSFManager = theApp.GetSHFManager();
//	if(pSFManager
//		&& m_BytesCompleted >= m_FileLength
//		)
//	{
//		pSFManager->UpdateFileULCount(m_FileIndex);
//	}
}
// -- end CGnuUploadShell::IncreaseFileUPCount()