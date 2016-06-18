//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!
#include "stdafx.h"
//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!

#include "httpd.h"
#include "../GnuModule/common.h"
#include "../fish_common.h"
#include "../WebText.h"

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.10.23	**************************************** //
// *******			**************************************** //
// ********************************************************************* //
// 2000.05.29 : GetHttpFileAsc() 함수. Memory로 저장하는 함수 추가
//            : GetHttpFileBin() 함수, Memory로 저장하는 함수 추가
// 2000.10.23 : 모든 함수내에 OpenRequest()에서 get Option을 RELOAD로 변경

#if !defined(MAXBUF)
#define MAXBUF 1024
#endif	// MAXBUF

// HTTP로 파일 가져옴 :Return Value :Success:TRUE, Falure:FALSE
BOOL GetCfgFile(TCHAR *Host, int Port, TCHAR *szFileName,TCHAR *szCfgBuf)
{
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	try
	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL)	return FALSE;

		pConnection = pSession->GetHttpConnection(Host,Port,_T("mars"),_T("normalcity"));
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return FALSE;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return FALSE;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			UINT nBytesRead = pHttpFile->Read(szCfgBuf, MAXBUF -1 );

			szCfgBuf[nBytesRead] = _T('\0');
			char temp[10];
			if (pHttpFile->Read(temp,10) != 0)
			{
				FishMessageBox(_T("File overran buffer -- not cached"));
			}
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	return bresult;
}

// HTTP로 파일 가져옴 :Return Value :Success:TRUE, Falure:FALSE
BOOL GetCfgFile(TCHAR *Host, int Port, TCHAR *szFileName, CWnd *pWnd, unsigned int Msg)
{
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	try
	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL)	return FALSE;

		pConnection = pSession->GetHttpConnection(Host,Port,_T("mars"),_T("normalcity"));
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return FALSE;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return FALSE;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

			TCHAR sz[MAXBUF+1];
			while (pHttpFile->ReadString(sz, MAXBUF))
			{
				pWnd->SendMessage(Msg,NET_HTTP_RECEIVE,(WPARAM)sz);
			}
			pWnd->SendMessage(Msg,NET_HTTP_RECEIVE_END,(WPARAM)sz);
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	return bresult;
}

/////////////////////////////////////////////////////////
// HTTP로 Binary File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	TCHAR* szResultName : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
//  int   encodetype	: UNICODE 프로그램인 경우 주소를 MultiType으로 바꾸어 사용한다.

BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName,
					TCHAR* szResultName, TCHAR* username, TCHAR*passwd, CWnd *pWnd, unsigned int Msg, int encodetype){

	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	//** Initialize Variable
	CFile f_GetFile;
	//**

	//** File Open
	CFileException e;
	TCHAR* pFileName = szResultName;

	if( !f_GetFile.Open( pFileName, CFile::modeCreate | CFile::modeWrite, &e ) ){
		#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << e.m_cause << _T("\n");
		#endif
	}
	//**

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL)return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

			pWnd->SendMessage(Msg,NET_HTTP_SIZE,(WPARAM) dwFileSize);

			TCHAR sz[MAXBUF+1];
			int size = 0;
			while ( (size =pHttpFile->Read(sz, MAXBUF)) != 0 )
			{
				//TRACE("%3d\n",size);
				pWnd->SendMessage(Msg,NET_HTTP_RECEIVE,(WPARAM) size);
				f_GetFile.Write( sz, size );
			}
			pWnd->SendMessage(Msg,NET_HTTP_RECEIVE_END,(WPARAM) NULL);
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		TRACE(_T("EXCEPTION :: CInternetException &08x \n"), pEx->m_dwContext);
		pWnd->SendMessage(Msg,NET_HTTP_ERROR,(WPARAM) NULL);		
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	//** file 닫기
	f_GetFile.Close();
	//**	
	return bresult;
}

/////////////////////////////////////////////////////////
// HTTP로 Ascii File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	TCHAR* szResultName : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName,
					TCHAR* szResultName, TCHAR* username, TCHAR*passwd, CWnd *pWnd, unsigned int Msg, int encodetype){

	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	//** Initialize Variable
	CFile f_GetFile;
	//**

	//** File Open
	CFileException e;
	TCHAR* pFileName = szResultName;

	if( !f_GetFile.Open( pFileName, CFile::modeCreate | CFile::modeWrite, &e ) ){
		#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << e.m_cause << _T("\n");
		#endif
	}
	//**

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL) return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);

		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);
			
			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);				
			}
			else
				dwFileSize = MAXBUF;			

			pWnd->SendMessage(Msg, NET_HTTP_SIZE, (WPARAM) dwFileSize);

			CString sz;
			TCHAR buff[MAXBUF+1];
			byte* pbuff = (byte*) &buff;

			int size, nread;

			while ( (dwFileSize > 0) && (dwFileSize < 0xdfffffff) ){
				nread = pHttpFile->Read((void*) &buff, sizeof(buff));
				if(nread == 0) break;

				CWebText wt;
				pbuff[nread] = 0;
				wt.SetText(pbuff, nread);
				sz = wt.GetText();

				size = sz.GetLength();
				if(size == 0) break;

				f_GetFile.Write( sz, size*sizeof(TCHAR) );
				dwFileSize -= (size)*sizeof(TCHAR);
				pWnd->SendMessage(Msg, NET_HTTP_RECEIVE, (WPARAM) (size)*sizeof(TCHAR));
			}
			pWnd->SendMessage(Msg,NET_HTTP_RECEIVE_END,(WPARAM) NULL);
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	//** file 닫기
	f_GetFile.Close();
	//**

	return bresult;
}


/////////////////////////////////////////////////////////
// HTTP로 Binary File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	TCHAR* szResultName : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호

BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName,	TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype){
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	//** Initialize Variable
	CFile f_GetFile;
	//**

	//** File Open
	CFileException e;
	TCHAR* pFileName = szResultName;

	if( !f_GetFile.Open( pFileName, CFile::modeCreate | CFile::modeWrite, &e ) ){
		#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << e.m_cause << _T("\n");
		#endif
	}
	//**

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL)return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

			TCHAR sz[MAXBUF+1];
			int size = 0;
			while ( (size =pHttpFile->Read(sz, MAXBUF)) != 0 )
			{
				f_GetFile.Write( sz, size );
				f_GetFile.Flush();
			}
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	//** file 닫기
	f_GetFile.Close();
	//**	
	return bresult;
}

/////////////////////////////////////////////////////////
// HTTP로 Ascii File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	TCHAR* szResultName : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype){
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	//** Initialize Variable
	CFile f_GetFile;
	//**

	//** File Open
	CFileException e;
	TCHAR* pFileName = szResultName;

	if( !f_GetFile.Open( pFileName, CFile::modeCreate | CFile::modeWrite, &e ) ){
		#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << e.m_cause << _T("\n");
		#endif
	}
	//**

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL) return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

			CString sz;
			TCHAR newline[] = _T("\r\n");	/* DOS FORMAT New line */
			int size;

			while ( (dwFileSize > 0) && (dwFileSize < 0xdfffffff) ){
				pHttpFile->ReadString(sz);
				size = sz.GetLength();
				f_GetFile.Write( sz, size*sizeof(TCHAR) );
				f_GetFile.Write( newline, _tcslen(newline)*sizeof(TCHAR));
				dwFileSize -= (size+1)*sizeof(TCHAR);
			}
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	//** file 닫기
	f_GetFile.Close();
	//**

	return bresult;
}


/////////////////////////////////////////////////////////
// HTTP로 Ascii File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	CMemFile *ResultMemFile : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *ResultMemFile, TCHAR* username, TCHAR* passwd, int encodetype){
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL) return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);

		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			int dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH ,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

//			TRACE(_T("Internet file Size : %d"), pHttpFile->GetLength());

			CString sz;
			TCHAR buff[MAXBUF+1];
			byte* pbuff = (byte*) &buff;

			int size, nread;

			CWebText wt;

			while ( (dwFileSize > 0) && (dwFileSize < 0xdfffffff) ){
				if(dwFileSize > sizeof(buff))
					nread = pHttpFile->Read((void*) &buff, sizeof(buff));
				else
					nread = pHttpFile->Read((void*) &buff, dwFileSize);

				if(nread == 0) break;
//				TRACE(_T("read raw data : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\r\n"), buff[0], buff[1], buff[2], buff[3], buff[4]);
				pbuff[nread] = 0;
				wt.SetText(pbuff, nread);
				dwFileSize -= nread;
			}

			sz = wt.GetText();
//				TRACE(_T("read data : %s\r\n"), sz.Right(10));
			size = sz.GetLength();
			if(size != 0)
			{
				ResultMemFile->Write( sz, size*sizeof(TCHAR) );
			}

		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	ResultMemFile->Seek( (LONG )0, (UINT )CFile::begin );

	return bresult;
}


/////////////////////////////////////////////////////////
// HTTP로 Binary File을 가져옴 
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host : Host filename
//	int   port : httpd port number
//	TCHAR* szFileName : 가져올 file name
//	CMemFile *ResultMemFile : 결과를 저장할 filename
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *ResultMemFile, TCHAR* szResultName, TCHAR* username, TCHAR* passwd, int encodetype){
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	try	{
		// check to see if this is a reasonable URL
		pSession = new CInternetSession(_T("Raw HTML Reader"));
		if(pSession == NULL)return false;

		pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
		if(pConnection == NULL) 
		{
			pSession->Close();
			delete pSession;
			return false;
		}

		pHttpFile = pConnection->OpenRequest(1,szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
		if(pHttpFile == NULL)
		{
			pConnection->Close();
			pSession->Close();

			delete pConnection;
			delete pSession;
			return false;
		}

		BOOL bSendRequest = pHttpFile->SendRequest();

		if(bSendRequest)
		{
			CString str;
			DWORD dwFileSize;

			BOOL bQuery=pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,
				str, NULL);

			if(bQuery)
			{
				dwFileSize = (DWORD)_ttoi((LPTSTR) (LPCTSTR) str);
			}
			else
				dwFileSize = MAXBUF;

			TCHAR sz[MAXBUF+1];
			int size = 0;
			while ( (size =pHttpFile->Read(sz, MAXBUF)) != 0 )
			{
				ResultMemFile->Write( sz, size );
				ResultMemFile->Flush();
			}
		}	// end of try
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		pEx->Delete();
		bresult = FALSE;
//		return FALSE;
	}

	if(pHttpFile) pHttpFile->Close();
	if(pConnection)	pConnection->Close();
	if(pSession) pSession->Close();

	if(pHttpFile) delete pHttpFile;
	if(pConnection) delete pConnection;
	if(pSession) delete pSession;

	ResultMemFile->Seek( (LONG )0, (UINT )CFile::begin );

	return bresult;
}

/////////////////////////////////////////////////////////
// Post로 데이타를 서버로 전송
// Return Value : Success:TRUE, Failure: FALSE
// Parameter
//	TCHAR* Host		: Host filename
//	int   port		: httpd port number
//	TCHAR* szFileName	: 내용을 보낼 file name
//	TCHAR* szBuf		: 보낼 내용을 가지고 있는 버퍼
//	int   nVerb		: POST/GET 방식을 지정. NULL 인 경우 GET 방식
//	TCHAR* username		: 로그온 사용자 id
//	TCHAR* passwd		: 로그온 사용자 암호
BOOL SendHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szBuf, int nVerb, TCHAR* username, TCHAR*passwd)
{
	CInternetSession*	pSession = NULL;
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	BOOL bresult = TRUE;

	CString strHeaders =
		_T("Content-Type: application/x-www-form-urlencoded");

	// check to see if this is a reasonable URL
	pSession = new CInternetSession(_T("Raw HTML Reader"));
	if(pSession == NULL)return false;

	pConnection = pSession->GetHttpConnection(Host,Port, username, passwd);
	if(pConnection == NULL) 
	{
		pSession->Close();
		delete pSession;
		return false;
	}

	pHttpFile = pConnection->OpenRequest(nVerb, szFileName,
			(LPCTSTR )NULL, (DWORD )1, (LPCTSTR *)NULL,
			(LPCTSTR )NULL,INTERNET_FLAG_RELOAD);
	if(pHttpFile == NULL)
	{
		pConnection->Close();
		pSession->Close();

		delete pConnection;
		delete pSession;
		return false;
	}

	BOOL bRet = pHttpFile->SendRequest(strHeaders,szBuf,_tcslen(szBuf) *sizeof(TCHAR));
	
	pHttpFile->Close();
	pConnection->Close();
	pSession->Close();

	delete pHttpFile;
	delete pConnection;
	delete pSession;

	return bRet;
}

