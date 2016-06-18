#if !defined(AFX_GNUDOWNLOAD_H__81A71840_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUDOWNLOAD_H__81A71840_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define USE_KEEP_ALIVE	false
#define RECEIVE_BUFF	16384


class CGnuNode;
class CGnuDownloadShell;

class CGnuDownload : public CAsyncSocket
{
public:
	CString UserID;
	CGnuDownload(CGnuDownloadShell* pShell, Result Params);
	virtual ~CGnuDownload();

	CFileLock* ReadyFile();
	bool	   ReadyChunk();

	int StartDownload();		
	void StopDownload();

	bool HandleAltLocation(CString);

	bool GetStartPos();
	void SendRequest(bool);
	void DownloadFile(byte*, int);
	void DownloadPost(byte*, int);

	void StatusUpdate(DWORD);

	void SetError(CString);
	void Timer();

	Result* GetRealParam();

	bool ReceivePending;

	// File info

	CFileLock* m_pFile;
	FileChunk* m_pChunk;

	INT64 m_StartPos;
	INT64 m_PausePos;
	INT64 m_ResumeStart;
	int m_Speed;

	bool m_KeepAlive;
	bool m_Continuing;

	// Download Properties
	Result  m_Params;
	CString m_Header;
	int     m_Status;
	bool    m_Push;

	// Bandwidth
	double m_dwTotalBytes;    // Bytes received in past 60 seconds
	DWORD  m_dwSecBytes;      // Bytes sent in second
	double m_nSecNum;	      // Number of elements in array


	// by bemlove (재연결 시도..)
	bool	m_bRetry;
	bool	m_bActiveDownload;		// 다운로드가 진행되었는지 여부
	bool	m_bStarted;

	// 최초 검색에서 얻은 속도를 기반으로 적용하여 준다.
	int	m_tempSpeed;
	//{{AFX_VIRTUAL(CGnuDownload)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnSend(int nErrorCode);
	virtual void Close();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuDownload)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	
protected:
	void AddChunkHost(std::vector<IP> &, IP);

	bool m_DataSending;

	byte m_pBuff[RECEIVE_BUFF];
	
	// Resume Verification
	
	int   m_VerifyPos;
	byte  m_Verification[4096];

//	CGnuDownloadShell* m_pShell;
	
	int    m_nSecsUnderLimit;
	int	   m_nSecsDead;

	Result* dumbResult;

	// Bandwidth
	DWORD m_dwAvgBytes[60]; // Average bytes from last 60 seconds
	int	  m_nSecPos;		// Position of next element insert

// by bemlove
public:
	CGnuDownloadShell* m_pShell;

	INT64  GetTotalDownloadSize( void );		//소켓의 전체 다운로드 사이즈
	INT64  GetDownloadSize( void );				// 소켓이 alive한 동안 다운로드한  사이즈
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNUDOWNLOAD_H__81A71840_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_)
