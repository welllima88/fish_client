// GetHttp.cpp: implementation of the CGetHttp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GetHttp.h"
#include "stdlib.h"
#include "io.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "../fish_common.h"
//#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetHttp::CGetHttp()
{
	m_sUserName[0] = 0;
	m_sPassword[0] = 0;
}

CGetHttp::~CGetHttp()
{

}


BOOL CGetHttp::SetAccessRight(TCHAR * userName, TCHAR * userPass)
{
	_sntprintf( m_sUserName, 256+1, userName );
	_sntprintf( m_sPassword, 256+1, userPass );
	if(userName[0]==0 || userPass[0] == 0) return 0;
	return 1;
}

BOOL CGetHttp::GetFile(TCHAR* host, TCHAR * remoteFile, TCHAR * localFile,CProgressCtrl * cp, CProgressCtrl* tp)
{
	int seg = 0;
	int nowTP = 0;
	if( tp != NULL && cp != NULL )
	{
		cp->SetPos(seg);
		nowTP = tp->GetPos();
	}
	m_dwServiceType = 3;
	m_nPort=80;
	_sntprintf( m_sServer, 256+1, host );
	_sntprintf( m_sObject, 256+1, remoteFile );

	int LocalFileSize=0;
	DWORD dwFileFlags = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite;


	//This function initializes an application’s use of the Windows CE Internet functions.
	m_hInternetSession = ::InternetOpen(AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(m_hInternetSession == NULL)
	{
		return FALSE;
	}

	//This function opens an FTP or HTTP session for a specified site. 
	if(_tcslen(m_sUserName))
    m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_sServer, m_nPort, m_sUserName,m_sPassword, m_dwServiceType, 0, (DWORD) this);
	  else
	m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_sServer, m_nPort, NULL, 
                                          NULL, m_dwServiceType, 0, (DWORD) this);
	if (m_hHttpConnection == NULL)
	{
		InternetCloseHandle(m_hInternetSession);
		return FALSE;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
	ppszAcceptTypes[1] = NULL;

	
 	DWORD dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION;
	if (m_nPort == INTERNET_DEFAULT_HTTPS_PORT) 
		dwFlags	|= (INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);

	// This function opens an HTTP request handle. 
	m_hHttpFile = HttpOpenRequest(m_hHttpConnection, NULL, m_sObject, _T("HTTP/1.1"), NULL, ppszAcceptTypes, dwFlags, (DWORD) this);
	if (m_hHttpFile == NULL)
	{
		InternetCloseHandle(m_hHttpConnection);
		InternetCloseHandle(m_hInternetSession);
		return FALSE;
	}

	// This function sends the specified request to the HTTP server.
	BOOL bSend = ::HttpSendRequest(m_hHttpFile, NULL, 0, NULL, 0);
	if(!bSend)
	{
		InternetCloseHandle(m_hHttpConnection);
		InternetCloseHandle(m_hInternetSession);
		return FALSE;
	}

	int filesize=1024*1024;
	unsigned long dwSize=32;
	TCHAR buff[256];
	// This function queries for information about an HTTP request.
	// Request Content Length.
	if	(HttpQueryInfo (m_hHttpFile, HTTP_QUERY_CONTENT_LENGTH, (LPVOID) buff, &dwSize, NULL))
	{	_stscanf(buff,_T("%d"),&filesize);
	}
	if	(filesize==0)
	{	InternetCloseHandle(m_hHttpConnection);
		InternetCloseHandle(m_hInternetSession);
		return TRUE;
	}

//	swprintf( g_fileSize, L" 0 %" );
	if	(LocalFileSize!=0)
	{
		if	(filesize<=0 || LocalFileSize==filesize)
		{
			InternetCloseHandle(m_hHttpConnection);
			InternetCloseHandle(m_hInternetSession);
			return TRUE;
		}
	}
	// 파일이 sub폴더가 지정된 파일이었을 경우에는
	// 폴더가 존재하는지 확인후 없을 경우에는 새롭게 생성한다.
	TCHAR* fullDir = _tcsrchr( localFile, _T('/') );
	if (fullDir == NULL)
		fullDir = _tcsrchr( localFile, _T('\\') );
	if( fullDir != NULL )
	{
		TCHAR dirPath[MAX_PATH+1] = {0};
		_tcsncpy( dirPath, localFile, _tcslen(localFile) - _tcslen(fullDir) );
		makedir( dirPath );
	}

	if (_tcsrchr(localFile, _T('!')) != NULL) {
		for (unsigned int i = 0; i < _tcslen(localFile); i++)
			if (localFile[i] == _T('!'))
				localFile[i] = _T(' ');
	}


	FILE* fpWrite = _tfopen(localFile, _T("wb"));
	if(fpWrite==NULL) return FALSE;

 
	int div = filesize/32;
	if	(div==0) div=1;
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = 32;
	// This function queries for information about an HTTP request.
	//Receives the status code returned by the server
	if (!HttpQueryInfo(m_hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{ 
		InternetCloseHandle(m_hHttpConnection);
		InternetCloseHandle(m_hInternetSession);
		return FALSE;
	} else
	{
		long nStatusCode = _ttol(szStatusCode);
	    if (nStatusCode == HTTP_STATUS_PROXY_AUTH_REQ || nStatusCode == HTTP_STATUS_DENIED)
		{
			// We have to read all outstanding data on the Internet handle
			// before we can resubmit request. Just discard the data.
			TCHAR szData[1024];
			DWORD dwSize;
			do
			{
				::InternetReadFile(m_hHttpFile, (LPVOID)szData, 1024, &dwSize);
			}
			while (dwSize != 0);
		}
  		else if (nStatusCode != HTTP_STATUS_OK && nStatusCode != HTTP_STATUS_PARTIAL_CONTENT)
		{
			InternetCloseHandle(m_hHttpConnection);
			InternetCloseHandle(m_hInternetSession);
			return FALSE;
		}
	}
	char szReadBuf[4096];
	DWORD dwBytesToRead = 4096;
	DWORD dwTotalBytesRead = 0;
	//Setup the struct used for the read
	INTERNET_BUFFERS netBuf;
	ZeroMemory(&netBuf, sizeof(netBuf));
	netBuf.dwStructSize = sizeof(netBuf);
	netBuf.lpvBuffer = szReadBuf; 
	netBuf.dwBufferLength = 4096;
	netBuf.dwBufferTotal = 4096;
	netBuf.dwOffsetHigh = 4095;
	do
	{
		//This function reads data from a handle opened by the InternetOpenUrl, FtpOpenFile, or HttpOpenRequest function.
		// IRF_NO_WAIT : Do not wait for data. If there is data available, 
		//				the function returns either the amount of data requested 
		//				or the amount of data available (whichever is smaller). 
		netBuf.dwBufferLength = 4096;
		if (!::InternetReadFileEx(m_hHttpFile, &netBuf, IRF_NO_WAIT, 0))
		{
//			debug("Error : %d", GetLastError());
			InternetCloseHandle(m_hHttpConnection);
			InternetCloseHandle(m_hInternetSession);
			return FALSE;
	    }
	    else
		{
			Sleep(1);
			if (netBuf.dwBufferLength)
			{
		        TRY
				{
					int nWriteItems = fwrite(szReadBuf, netBuf.dwBufferLength, 1, fpWrite);
					dwTotalBytesRead += netBuf.dwBufferLength;

					if( tp != NULL && cp != NULL )
					{
						int newseg = dwTotalBytesRead/div;
//						theApp.SetCur(" %d / %d Bytes[%d%%]", dwTotalBytesRead, filesize, (dwTotalBytesRead*100)/filesize );
						if	(newseg!=seg )
						{
							seg=newseg;
							cp->SetPos(seg);
							tp->SetPos( nowTP+seg );
						}
					}
					
				}
				CATCH(CFileException, e)
				{
					InternetCloseHandle(m_hHttpConnection);
					InternetCloseHandle(m_hInternetSession);
			        e->Delete();
					return FALSE;
				}
				END_CATCH
			}	
		}
	} 
	while (netBuf.dwBufferLength);
	fclose(fpWrite);

	InternetCloseHandle(m_hHttpConnection);
	InternetCloseHandle(m_hInternetSession);
	return TRUE;
}
