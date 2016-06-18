#if !defined( __HTTPD_H )
#define __HTTPD_H

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.10.20	**************************************** //
// *******			**************************************** //
// ********************************************************************* //
// 2000.05.29 : GetHttpFileAsc() �Լ�. Memory�� �����ϴ� �Լ� �߰�
//            : GetHttpFileBin() �Լ�, Memory�� �����ϴ� �Լ� �߰�
// 2000.10.20 : SendHttpFileAsc() �Լ�, ������ ����Ÿ�� ������ �Լ� �߰�

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

// Ư�� �����쿡�� �߰���� Notify�ϴ� �Լ�
// binary file��������
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName,
	TCHAR* szResultName, TCHAR* username, TCHAR* passwd, CWnd *pWnd, unsigned int Msg, int encodetype = -1);
// Text file��������
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, CWnd *pWnd, unsigned int Msg, int encodetype = -1);

// �����쿡�� �߰���� notify���� ����
// binary file��������
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype = -1);

// Text file��������
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szResultName, TCHAR* username, TCHAR*passwd, int encodetype = -1);
 
// Memory File�� ����Ÿ�� ������
// Text File ��������
BOOL GetHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *MemFile, TCHAR* username, TCHAR* passwd, int encodetype = -1);
// Binary File��������
BOOL GetHttpFileBin(TCHAR* Host, int Port, TCHAR* szFileName, CMemFile *MemFile, TCHAR* username, TCHAR* passwd, int encodetype = -1);

// �����쿡�� �߰���� notify���� ����
// Buffer ������
BOOL SendHttpFileAsc(TCHAR* Host, int Port, TCHAR* szFileName, TCHAR* szBuf, int nVerb=CHttpConnection::HTTP_VERB_GET, TCHAR* username=NULL, TCHAR*passwd=NULL);

#endif	// __HTTPD_H