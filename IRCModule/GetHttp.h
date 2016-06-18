// GetHttp.h: interface for the CGetHttp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETHTTP_H__225A2DD6_5B1B_4782_9C08_540FE4904B6A__INCLUDED_)
#define AFX_GETHTTP_H__225A2DD6_5B1B_4782_9C08_540FE4904B6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<AFXINET.H>
class CGetHttp  
{

public:
    TCHAR  m_sURLToDownload[512];
    TCHAR  m_sFileToDownloadInto[256];
    TCHAR  m_sUserName[256];
    TCHAR  m_sPassword[256];

	BOOL SetAccessRight(TCHAR * userName,TCHAR *userPass); 
	BOOL GetFile(TCHAR* host, TCHAR * remoteFile,TCHAR * localFile,CProgressCtrl *cp, CProgressCtrl * tp);

public:
	CGetHttp();
	virtual ~CGetHttp();


public:
  CWnd *Papa;
  
  TCHAR      m_sServer[256]; 
  TCHAR      m_sObject[256]; 
  TCHAR      m_sFilename[256];
  INTERNET_PORT m_nPort;
  DWORD         m_dwServiceType;
  HINTERNET     m_hInternetSession;
  HINTERNET     m_hHttpConnection;
  HINTERNET     m_hHttpFile;
  BOOL          m_bAbort;
  BOOL          m_bSafeToClose;
};

#endif // !defined(AFX_GETHTTP_H__225A2DD6_5B1B_4782_9C08_540FE4904B6A__INCLUDED_)
