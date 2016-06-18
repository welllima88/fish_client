#if !defined(AFX_GNUDOWNLOADSHELL_H__651CCFA0_7071_11D5_ACF3_00A0CC533D52__INCLUDED_)
#define AFX_GNUDOWNLOADSHELL_H__651CCFA0_7071_11D5_ACF3_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"

#define DL_SECTION_SIZE (64 * 1024) // Download file in 64k pieces

class CGnuDownload;
class CGnuNode;

// written by moonknit 2005-10-14
#ifdef MULTI_UPDATETFSTATUS
struct SRLINK_ITEM
{
	int nFSerial;
	CString sSRID;

	SRLINK_ITEM(int n, CString s)
		: nFSerial(n), sSRID(s)
	{
	}
};
#endif
// --

class CGnuDownloadShell  
{
public:
#ifdef MULTI_UPDATETFSTATUS
	void DelSRLink(CString sSRID);
	void AddSRLink(int nFSerial, CString sSRID);
#endif
	bool m_AreadyPayPoint;
	int m_JoinChunk;
	int m_Point;

	// written by moonknit 2005-10-14
#ifdef MULTI_UPDATETFSTATUS
	std::list<SRLINK_ITEM> m_listSRLink;
#else
	// added by moonknit 2005-07-04
	int m_nFSerial;
	CString m_sSRID;
	// -- 
#endif

#ifdef _DEBUG
	DWORD	m_starttime;
	DWORD	m_executetime;
#endif

	int		m_nType;					// �ٿ�ε��� ���� 
										// 0 : Unknown, 1 : Post Download, 3 : File Download

	CString m_UserID;
	CString m_strIndex;
	CString m_OriginalFileName;
	BOOL m_CompleteDownload;
	int m_isPartial;
	INT64 GetTotalFileSize();
	INT64 TotalPos;
	CGnuDownloadShell();
	virtual ~CGnuDownloadShell();
	
	void AddHost(Result);
	void TryNextHost();

	void AddAltLocation(CString);
	void AddAltLocation(AltLocation);

	bool PostCompletion();
	bool CheckCompletion();
	void SetCorrupt(IP, bool);

	void ReleaseDownload();
	void Start();
	void Stop();
	void StartResearch(int nCnt = 5);

	// Query Send to single node
	void SendQuery();
	void IncomingQueryHit(packet_Log*);
	void IncomingNode(CGnuNode*);

	bool IsTrying();

	bool IsDownloading();
	DWORD GetStatus();

	void BackupDownload(CString SavePath = _T(""));
	void DeleteBackup();

	// ����Ʈ ������ ���� bytes ��� method
	INT64 GetReceiveBytes(void);

	INT64 GetBytesCompleted();
	INT64 ChunkFamilySize(int);
	void TrashFamily(int);
	int GetPartialData(INT64 pos, unsigned char *buf, int len);
	
	void Timer();

	CFileLock* ReadyFile(CGnuDownload*);

	DWORD	GetTotalSourceCount();			// ���� Queue�� �ҽ� ����
	DWORD	GetReceivingCount(void);		// ���� ������ �������� ������ ����..
	bool	SetTheTransferUserID(void);

	// added by moonknit
	static UINT MergeChunkProc(LPVOID lpParam);
	static bool IsMergeThread() {return bMergeThread;};
	static void SetMergeThread(bool bMerge) {bMergeThread = bMerge;};
	static bool IsStopThread() {return bStopThread;};
	static void SetStopThread(bool bStop) {bStopThread = bStop;};
	static void TriggerEvent();
	static void FreeMergeItem();

	bool InitMergeThread();
	BOOL IsAttachable();
	void TreatMergeResult(LPARAM lParam);
	void ProcessC2PMergeResult(MergeResult* pResult);
	bool TryAttachChunk2Partial();
	// ���� �ð����� �۾��� ������ �ִٸ� Complete ���θ� �ٽ� Ȯ���Ѵ�.
	// ���� ������ �� ��
	void CheckChunkComplete();

	BOOL CheckChunkExist(FileChunk* pChunk);

	// Get Source User ID
	void GetExploreBuddyList(LPVOID plist);

	bool m_bMerging;
	bool m_bComplete;
	bool m_bAllComplete;
	bool m_bMerged;
	int m_nResultTry;
	int m_nBackupCount;
	// by bemlove
	bool m_bCanceled;// ����ڿ� ���� �������

	// written by moonknit 2005-06-07
	// ȭ��� ��Ÿ���� �ִ� ���ۻ���
	int	m_nOldState;

	std::list<FileChunk*> m_CompleteList;		// ���� ��� ����
	std::list<MergeResult*> m_ResultList;

	static std::list<MergeItem*> MergeList;

	// written by moonknit 2005-09-07
	// ���� ���� �õ����� ȣ��Ʈ�� ����
	int		TryCnt;
	void	IncreaseTryCnt();
	void	DecreaseTryCnt(bool bAll = false);
	// --

	// �Ͻ����� flag
	BOOL	m_bStop;			// added by moonknit 2005-06-16
	// Queued ����
	BOOL	m_bQueued;			// added by moonknit 2005-06-16
	
	// Download Properties
	bool	m_Active;
	bool    m_Dead;
	int     m_Cooling;
	int     m_Searching;
	bool	m_Waiting;
	int		m_HostTryPos;
	bool	m_DoReQuery; // reset on load
	int		m_TotalSecCount;
	int		m_RetryCount;		// by bemlove (�翬�� ��Ȳ���� ����õ�)

	bool    m_Retry;
	CString m_ReasonDead;
	bool    m_UpdatedInSecond;

	CString m_Name;
	CString m_FilePath;
	CString m_PartialPath;

	CString m_Sha1Hash;
	CString m_BitprintHash;

	CString m_Search;
	GUID    m_SearchGuid;

	bool    m_BackupTrigger;

	std::vector<CGnuDownload*> m_Sockets;
	std::vector<FileChunk*>	   m_ChunkList;
	std::vector<CGnuNode*>	   m_ReSearchedNodes;

	// post data
	CString m_PostXML;

	// File info
	CFileLock m_File;
	INT64	m_FileLength;
	INT64	m_BytesCompleted;			// ������� ���۵� ���� ũ��
	INT64	m_StartBytes;				// �ٿ�ε� ���۵Ǵ� ����
	INT64	m_OverlapBytes;

	// written by moonknit 2005-10-17
	// ��Ŷ ����Ʈ ������ ����ϱ� ���� ��Ŷ ũ��
	INT64	m_PayedBytes;				// ������� ������ ������ ũ��
	INT64	m_StockBytes;				// CGnuDownload�� ��ü�� �Ҹ��� �� �ٿ� ��Ŷ�� ũ�⸦ �����ϴ� ��

	INT64	m_StopPos; // Start postion of first chunk 

	// Host List to download from
	std::vector<IP> m_PartialHosts;
	std::vector<Result> m_Queue;
	std::deque<AltLocation> m_AltHosts;

	int HostFamilyAge; 

	// Bandwidth stuff
	DWORD   m_AvgSpeed; 

	int m_AllocBytes;
	int m_AllocBytesTotal;

	//Makslane: download icon status manager
	HWND m_Hwnd;
	void *pGroup;
	
	CCriticalSection m_ShellAccess;

	inline  void	SetAlreadyPayPoint ( bool already )			{ m_AreadyPayPoint = already ; };
	inline  bool	AlreadyPayPoint( void )						{ return m_AreadyPayPoint; };
protected:
	// added by moonknit
	static bool bMergeThread;
	static bool bStopThread;
	// ���� �������� ������ ���� ��ҿ� ���� �����Ǿ�� �� ���
	// ���� �۾��� ���߱� ���� �ʿ��ϴ�.
	static LPVOID pCurMergeShell;
	static bool pStopMerge;
	static CEvent evMerge;
	static CCriticalSection csMergeLock;
	CCriticalSection csResultLock;
	// -- added

	bool JoinParts(CFileLock &, CFileLock &, INT64, INT64,bool);
	void AddChunkHost(std::vector<IP> &, IP);

	byte m_Packet[1024];
};

#endif // !defined(AFX_GNUDOWNLOADSHELL_H__651CCFA0_7071_11D5_ACF3_00A0CC533D52__INCLUDED_)
