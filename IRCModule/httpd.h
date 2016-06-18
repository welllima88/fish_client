#if !defined( __HTTPD_H )
#define __HTTPD_H

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.10.20	**************************************** //
// *******			**************************************** //
// ********************************************************************* //
// 2000.05.29 : GetHttpFileAsc() 함수. Memory로 저장하는 함수 추가
//            : GetHttpFileBin() 함수, Memory로 저장하는 함수 추가
// 2000.10.20 : SendHttpFileAsc() 함수, 서버로 데이타를 보내는 함수 추가

#if !defined( __AFXINET_H_ )
#include <afxinet.h>
#endif	// __AFXINET_H_

#define NET_HTTP_RECEIVE		10
#define NET_HTTP_RECEIVE_END		11
#define	NET_HTTP_SIZE			12
#define	NET_HTTP_ERROR			13
#define	NET_DIALOG_CLOSE		15

BOOL GetCfgFile(TCHAR *Host, int Port, TCHAR *szFileName,TCHAR *szCfgBuf);
BOOL GetCfgFile(TCHAR *Host, int Port, TCHAR *szFileName, CWnd *pWnd, unsigned int Msg);

// 특정 윈도우에게 중간결과 Notify하는 함수
// binary file가져오기
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName,
	TCHAR* szResultName, TCHAR* username, TCHAR* passwd, CWnd *pWnd, unsigned int Msg, int encodetype = -1);
// Text file가져오기
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, CWnd *pWnd, unsigned int Msg, int encodetype = -1);

// 윈도우에게 중간결과 notify없이 수행
// binary file가져오기
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype = -1);

// Text file가져오기
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype = -1);
 
// Memory File에 데이타를 저장함
// Text File 가져오기
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *MemFile, TCHAR* username, TCHAR* passwd, int encodetype = -1);
// Binary File가져오기
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *MemFile, TCHAR* username, TCHAR* passwd, int encodetype = -1);

// 윈도우에게 중간결과 notify없이 수행
// Buffer 보내기
BOOL SendHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szBuf, int nVerb=CHttpConnection::HTTP_VERB_GET, TCHAR* username=NULL, TCHAR*passwd=NULL);

#endif	// __HTTPD_H