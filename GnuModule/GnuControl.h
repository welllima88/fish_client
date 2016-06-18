#if !defined(AFX_GNUCONTROL_H__1EA05003_8987_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUCONTROL_H__1EA05003_8987_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GnuRouting.h"
#include "GnuSock.h"
#include "Packet.h"
#include "Common.h"
#include "GnuDefines.h"
#include "../fish_struct.h"	

// ping pong scheme
#define ACTIVATE_PONG_CACHING	false
#define TIMEOUT_PONG_CACHE		3000
#define MAX_NBPONG_RESPONSE		10
#define PONG_CACHE_SIZE			1000  
#define PONG_CACHING_VERSION	"0.1"

#define WM_NODE_CONNECTED   WM_USER + 101

#define DEFAULT_CONNECTIVITY	50

enum NODE_CONNECTIVITY
{
	CONNECTIVITY_DEFAULT = 10,
	CONNECTIVITY_LEV1 = 2,
	CONNECTIVITY_LEV2 = 5,
	CONNECTIVITY_LEV3 = 8,
	CONNECTIVITY_LEV4 = 10
};

struct key_Value;
class CGnuRouting;
class CGnuLocal;

class CGnuNode;
class CGnuDownloadShell;
class CGnuUploadShell;
class CGnuDownload;

struct CanceledUpload {
	CString			UserID;
	unsigned int	fileIndex;
};

struct PongCache	{
	IP		HostIP;
	int		Port;
	int		Hop;
	int		TryCount;
	PongCache() {
		TryCount = 0;
	}
};

struct ActiveHost {
	IP			HostIP;
	int			Port;
	CString		FileName;
	ActiveHost()
	{
		Port = 0;
		FileName = _T("");
	}
};

// written by moonknit 2005-07-05
// For RFID Direct Download
struct RFIDLink 
{
	CString FileName;
	CString RFID;
	INT64	FileSize;
	CString	HOSTINFO;
	CGnuDownloadShell* pShell;

	RFIDLink()
	{
		FileName	= _T("");
		RFID		= _T("");
		FileSize	= 0;
		HOSTINFO		= _T("");
		pShell		= NULL;
	}

	RFIDLink(CString link)
	{
		Init(link);
	}

	void SetShell(CGnuDownloadShell* p)
	{
		pShell = p;
	}

	void Init(CString link)
	{
		CString tmpString = "";
		TCHAR ch = NULL;
		int nStep = 0;	// step 0 : RFID, 1 : file size : 2 file name
		for(int i = 0; i<link.GetLength(); i++)
		{
			ch = link.GetAt(i);
			if(ch != '|' && ch != '/')
			{
				tmpString += ch;
			}
			else
			{
				if(nStep == 0)
				{
					RFID = tmpString;
					nStep ++;
				}
				else if(nStep == 1)
				{
					FileSize = _ttoi64((LPTSTR) (LPCTSTR) tmpString);
					nStep ++;
				}
				else if(nStep == 2)
				{
					FileName = tmpString;
					nStep ++;
				}
				else
				{
					HOSTINFO = tmpString;
					nStep ++;
					break;
				}
				tmpString = "";
			}
		}
	}

	RFIDLink& operator = (RFIDLink& r)
	{
		FileName	= r.FileName;
		RFID		= r.RFID;
		FileSize	= r.FileSize;
		HOSTINFO	= r.HOSTINFO;
		pShell		= r.pShell;

		return *this;
	}
};
// -- struct RFIDLink

#define		MAX_OTHER_HOST_COUNT	3

class CGnuControl : public CAsyncSocket
{
public:
	void GnuConRelease();
	int GetNodeTypeCnt(int ntype = NT_INBOUND);			// default in-node
	// written by moonknit 2005-06-01
	// check if all file transfer works are completed
	BOOL IsTRCompleted();
	void TryAutoCompleteAction(int nType);

	// check shell pointer is valid
	BOOL CheckDownloadShell(CGnuDownloadShell* pShell);
	BOOL CheckUploadShell(CGnuUploadShell* pShell);
	// --

	// moved from CGnuSock
	bool CheckRejectorList ( CString UserID , int Index );
	// --
	bool InsertRejectorList(CString UserID, int Index);

	int	 m_nPongCount;
	bool m_toggleGnuSeed;
	bool CheckCache ( IP host );
	CGnuControl();
	virtual ~CGnuControl();

	// Main thread only needs to lock when changing lists
	// Other threads always have to lock before touching list
	CCriticalSection m_NodeAccess;
	CCriticalSection m_UploadAccess;
	CCriticalSection m_DownloadAccess;

	// written by moonknit 2005-05-12
	bool ConnectingSlotsOpen();

	// written by moonknit 2005-01-09
	CGnuDownloadShell* Download(ResultGroup& ItemGroup, BOOL bQueued = FALSE);

	// written by moonknit 2005-05-18
	CGnuDownloadShell* Download(DL_DATAINFO& dlinfo, BOOL bQueued = FALSE);
	CGnuDownloadShell* Download(SE_ITEM& item, BOOL bQueued = FALSE);
	CString CheckPartialFileName(CString& name, CString Path = _T("./"));
	// -- end

	// written by moonknit 2005-07-05
	// RFID Link process
	CGnuDownloadShell* DownloadAndSearch(RFIDLink& link);
	void ManageSearch();

	void VerifyPostResult(LPVOID lpuploader, CString xml);
	void VerifyFileResult(LPVOID lpuploader, CString FilePath, CString FileHash);

	std::vector<CGnuDownload*>		m_DLNodeList;
	bool AddDLNode( CGnuDownload* pDownload );
	bool RemoveDLNode( CGnuDownload* pDownload );
	bool IsDownloading( CGnuDownload* pDownload );
	void ReOrderUploadShell( CGnuUploadShell* pUpload);	// written by moonknit 2005-04-20
	void RemoveUploadShell( CGnuUploadShell* pUpload); // written by moonknit 2005-04-20
	void RemoveDownloadShell(CGnuDownloadShell* pDownload); // written by moonknit 2005-06-01
	CGnuUploadShell* GetNonPremiumUploadShell( int* x, CGnuUploadShell* pUpload = NULL ); // written by moonknit 2005-04-20

	std::vector<RFIDLink*>			m_SearchList;
	
	std::vector<CGnuNode*>			m_NodeList;
	std::vector<CGnuSock*>			m_SockList;
	std::list<CGnuDownloadShell*>	m_DownloadList;
	std::list<CGnuUploadShell*>		m_UploadList;
	std::vector<CanceledUpload>		m_CancelUploadList;
	std::vector<PongCache>			m_stlPongCache;
	IP	m_SendOtherInfo[MAX_OTHER_HOST_COUNT];
	int	m_nCurSendOtherInfo;
	IP	m_ReceiveOtherInfo[MAX_OTHER_HOST_COUNT];
	int m_nCurReceiveOtherInfo;

	// 
	BOOL bReleased;

	// written by moonknit 2005-09-07
	int m_nDLTryingCnt;
	void IncreaseDLTryingCnt();
	void DecreaseDLTryingCnt();
	bool GetDownloadingSlot();

	int m_nCurDLCnt;
	int GetDownloadingCnt()					{ return m_nCurDLCnt; }
	// --
	BOOL CheckNeeds(CGnuDownload* pDown);

	// written by moonknit 2005-08-17
	BOOL CheckSockList(CGnuSock* pSock);

	// written by moonknit 2005-06-22
	int m_nLastDLCheck;
	// -- 

	std::vector<ActiveHost>			m_stlActiveDownloadHost;
	CCriticalSection				m_ActiveDownloadAccess;
	bool	AddActiveHost( ActiveHost host );
	bool	DelActiveHost( ActiveHost hsot );
	int		FindActiveHost( ActiveHost host );

	// Listening control
	bool StartListening(BOOL bSilence = FALSE);
	void StopListening();

	CGnuLocal* m_LANSock;

	// Traffic control
	void Broadcast_Ping(packet_Ping*,   int, CGnuNode*);
	void Broadcast_Query(packet_Query*, int, CGnuNode*);
	GUID Broadcast_LocalQuery(byte*, int);

	void Route_Pong(packet_Pong*, int, key_Value*);
	void Route_QueryHit(packet_QueryHit *, DWORD, key_Value*);
	void Route_Push(packet_Push*, int, key_Value*);
	void Route_LocalPush(Result);

	// Node control
	void AddNode(CString, UINT, UINT);
	void RemoveNode(CGnuNode*);
	CGnuNode* FindNode(CString, UINT);
	BOOL CheckNode(CGnuNode*);			// added by moonknit 2005-06-16

	// Socket Counts
	int	 CountSuperConnects();
	int  CountNormalConnects(int ntype = NT_ALL);
	int  CountLeafConnects();
	int  CountUploading();
	int  CountDownloading();

	void LoadDownloads();
	CString GetBackupString(CString Property, int &StartPos, CString &Backup);

	// Communications
	void NodeMessage(UINT, WPARAM);
	void TransferMessage(UINT dwMessID, WPARAM pTransfer = NULL, LPARAM lReserved = NULL);

	void Timer();
	void ManageNodes();
	void ManageNodes2();
	void ManageDownloads();
	void ManageUploads();

	void AddConnect(bool);
	void DropNode();
	void DropLeaf();
	void CleanDeadSocks();
	
	void ClearList(void);
	void DeleteNode( CString strHostIP, UINT port, int reason=0 );
	void AddOtherNode( CString strHostIP, UINT port, int reason=0 );
	void RequestOtherNode( void );

	bool m_BackupTicket; // Used to control amount of backups done in an amount of time

	// Local Client Data
	CTime   m_ClientUptime;

	UINT    m_CurrentPort;
	GUID    m_ClientID;
	DWORD   m_dwFiles;
	DWORD   m_dwFilesSize;

	// fast connection
	BOOL	m_bTurboNode;

	// Normal Node Count
	int		m_nNormalCnt;

	// SuperNodes
	UINT    m_NodeCapability;		// SuperNode or Child
	bool    m_LeafModeActive;
	bool	m_ForcedSupernode;

	int m_NormalConnectsApprox;

	int	m_ExtPongBytes;
	int m_UltraPongBytes;
	int m_SecCount;

	// Used to get hostname from an IP, (one per thread)
	hostent* m_HostEnt;

	// Hash tables
	CGnuRouting m_TableRouting;
	CGnuRouting m_TablePush;
	CGnuRouting m_TableLocal;

	// Pong Caching
	void AddToPongCache(MapPongList*,int);
	void FlushPongCache();

	CCriticalSection			m_HostCacheAccess;
	std::vector<MapPongList>	m_PongCache[MAX_TTL - 1];
	std::vector<MapPongList>	m_PongCache_Old[MAX_TTL - 1];
	UINT						m_FlushPongCache_time;
	UINT						m_FlushPongCacheOld_time;

	// Bandwidth
	double m_NetSecBytesDown;
	double m_NetSecBytesUp;

	double m_DownloadSecBytes;
	double m_UploadSecBytes;

	// written by moonknit 2005-05-16
	int		m_nStartTime;			// added by moonknit 2005-06-16

#ifdef NODE_CONNECTIVITY
	// written by moonknit 2005-06-17
	WORD	GetConnectivity(CGnuNode* pNode = NULL);
#endif
	DWORD	GetAvgEfficiency(CGnuNode* pNode = NULL);

	// Mapping
	bool AddNodeMap(MapNode &);
	UINT GetNodeMap(UINT ID);
	void ReplaceNodeMap(MapNode);

	std::vector<MapNode> MapTable;

	// written by moonknit 2005-05-16
	void SendResults(SE_QUERY& query, auto_ptr<SITEM_LIST>& resultlist, int nsearchtype);
	void SendResultsPacket(SE_QUERY& query, byte* pQueryHit, DWORD ReplyLength, byte ReplyCount, RemoteIndex* pRemote);
	GUID SendQuery(CString sword, int type, COleDateTime from, int span = 0, int spantype = 0, int maxcnt = 0);
	// -- end

	// written by moonknit 2005-05-13
	int		m_nNodeManage1;
	int		m_nNodeManage2;
	// -- end

	// written by moonknit 2005-07-05
	int		m_nSearchManage;

	//{{AFX_VIRTUAL(CGnuControl)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuControl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNUCONTROL_H__1EA05003_8987_11D4_ACF2_00A0CC533D52__INCLUDED_)
