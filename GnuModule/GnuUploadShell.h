#if !defined(AFX_GNUUPLOADSHELL_H__73291C71_09D8_4741_9686_A46D887E8D87__INCLUDED_)
#define AFX_GNUUPLOADSHELL_H__73291C71_09D8_4741_9686_A46D887E8D87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Packet.h"

class CGnuUpload;

class CGnuUploadShell  
{
public:
	// PRIMIUM LEVEL
	// 0 : normal registered user
	// 1 : premium registered user
	// 2 : large enppy point registered user
	// 3 : guest user
	UINT	m_nPremiumLevel;			// premium user level && guest information

	WORD	m_wVersion;

	INT64	m_nSendTotalBytes;			// ���۷�.. - Enppy Point ����
	INT64	m_nSendBytesForView;		// ���۷�.. - View �۾���	added by moonknit 2005-08-23
	INT64	m_FileLength;				// ������ ũ��
	INT64	m_StopPos;					// ���۵� ������ ��
	INT64	m_BytesCompleted;			// ���� ���� ��ġ(������ ���� ũ�� )
	INT64	m_BytesTotalSend;			// ���۵Ǿ��� ���� ũ��(���� �κп���)
	INT64	m_BytesAllSend;				// ��ü������.(m_BytesTotalSend���� ���Ѱ�..)
	int m_iIdentifyList;
	CString m_UserID;
	CGnuUploadShell();
	virtual ~CGnuUploadShell();

	void Timer();
	void StatusUpdate(DWORD Status, BOOL bForce = FALSE);
	void SetStatusUpdated(BOOL bUpdated = TRUE)				{ m_StatusUpdated = bUpdated; }

	void PushFile();
	bool VerifyData(CString handshake, int index = -1, CString stype = _T(""));

	void VerifyFileResult(CString FilePath, CString FileHash);
	void VerifyPostResult(CString xml);

	DWORD  ResetBytesTotalSend();

	IP   m_Host;
	UINT m_Port;
	int  m_Attempts;

	CFile   m_file;
	CString m_FileName;
	CString m_FileNameTemp;
	CString m_FilePath;
	int		m_nType;
	int     m_FileIndex;
	CString m_Sha1Hash;				// RFID

	CString m_PostXML;

	CTime   m_ChangeTime;
	DWORD   m_Status;
	BOOL	m_StatusUpdated;		// Timer ���۽� Status������ �ܺη� �˸���. Thread���� UI�� ���� ������ �� �����Ƿ� �̿�ȴ�.
	CString m_Error;
	bool	m_UpdatedInSecond;
	
	bool	m_KeepAlive;

	CString m_Handshake;

	CGnuUpload* m_Socket;

	// ���� ����� ���Ͽ��� ����ó���� �Ǿ����� ������ ����ִ�.
	BOOL	m_bTransfered;

	// written by moonknit 2005-09-07
	// ������ ������� ���� Shell�� �ڵ����� ������ �� �ֵ��� �Ѵ�.
	BOOL	m_bDeleteShell;

	// Bandwidth Limits
	int    m_AllocBytesTotal;   // Total assigned bytes in second
	int    m_AllocBytes;		// Bytes left to use in a second
	
	// Bandwidth
	double m_dwTotalBytes;   // Bytes received in past 60 seconds
	DWORD  m_dwSecBytes;     // Bytes sent in second
	DWORD  m_dwSecBytesLast;	// Previous Bytes-sent-in-second data to show
	double m_nSecNum;	     // Number of elements in array
	int    m_nSecsUnderLimit;
	int	   m_nSecsDead;

	// written by moonknit 2005-08-23
	DWORD  m_AvgSpeed; 
	// -- 
	DWORD  m_dwAvgBytes[60]; // Average bytes from last 60 seconds
	int	   m_nSecPos;		// Position of next element insert

	int	   m_Point;

	// written by moonknit 2005-05-13
	// �ٿ�ε����� Socket ������ ����� �� �� ��Ȯ�ϰ� �ϱ� ���Ͽ�..
	bool	m_bCounted;
	void	DecreaseCount();
	void	IncreaseCount();
	// -- end

	// written by moonknit 2005-06-20
	void	IncreaseFileUPCount();

	// written by moonknit 2005-06-07
	// ȭ��� ��Ÿ���� �ִ� ���ۻ���
	int	m_nOldState;
};


bool operator > (const CGnuUploadShell &Transfer1, const CGnuUploadShell &Transfer2);


#endif // !defined(AFX_GNUUPLOADSHELL_H__73291C71_09D8_4741_9686_A46D887E8D87__INCLUDED_)
