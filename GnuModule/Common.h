#if !defined(INLCUDE_ICON_H)
#define INLCUDE_ICON_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include "FileLock.h"
#include "packet.h"
#include "conversions.h"

DWORD	AssignThreadToCPU(CWinThread *pThread, DWORD cpuNumber);
void	SetRandColor(COLORREF &);
CString	GetFileError(CFileException* error);
CString ParseString( CString &Str, TCHAR delim = _T(','));

CTime	StrToCTime(CString& str);
CString CTimetoStr(CTime& time);

void    GnuCreateGuid(GUID *pGuid);
CString GuidtoStr(GUID inGuid);
GUID    StrtoGuid(CString strGuid);

CTimeSpan LocalTimeZone();

class CGnuNode;

class AltLocation /* : public HashedFile */
{
public:
    CString Name;			// Name of File
	DWORD	Index;			// Index of the remote file
	CString Sha1Hash;
    Node	HostPort;		// location of alternate

	AltLocation()  { Clear(); };
	AltLocation(CString& str) { *this = str;};
	~AltLocation() { Clear(); };

	void Clear()
	{
		Name   = Sha1Hash = "";
		Index  = 0; 
		HostPort.Clear();
	};

	bool isValid()
	{
		if (HostPort.Host.IsEmpty() || Sha1Hash.IsEmpty() || Name.IsEmpty() || Index == 0)
			return false;

		return true;
	};

	void UpdateTime()
	{
		HostPort.Time = CTime::GetCurrentTime() - LocalTimeZone();	
	};

	bool operator ==(AltLocation& first)
	{
		// check if host matches
		if (HostPort == first.HostPort)
			if (Sha1Hash == first.Sha1Hash || (Name == first.Name && Index == first.Index))
				return true;

		return false;
	};

	// Simple Assignment
	AltLocation& operator=(CString& str);

	// Build an AltLocation string
	CString GetString();
};

struct HashedFile
{
	CString Name;
	UINT    Index;
	CString TimeStamp;
	CString Sha1Hash;

	std::vector<AltLocation> AltHosts;

	HashedFile(CString nName = "", CString nTimeStamp = "", CString nHash= "")
	{ Name = nName; TimeStamp = nTimeStamp; Sha1Hash = nHash; };
};
 
struct SharedFile
{
	std::basic_string<char> Name;
	std::basic_string<char> NameLower;
	std::basic_string<char> Dir;

	std::basic_string<char> Sha1Hash;
	std::basic_string<char> TimeStamp;
	bool HashError;

	std::deque<AltLocation> AltHosts;

	// QRP
	std::vector< std::basic_string<char> > Keywords;
	std::vector<UINT> HashIndexes;

	UINT  Index;
	DWORD Size;  
	DWORD Matches; 
	DWORD Uploads;

	CGnuNode* RemoteNode;  // If file is indexed on a remote node

	// 레벨별 SID
	byte	SID_Level5[8];
	byte	SID_Level4[8];
	byte	SID_Level3[8];
	byte	SID_Level2[8];
	byte	SID_Level1[8];

};


struct SharedDirectory
{
	CString Name;
	bool	Recursive;
	DWORD	Size;
	DWORD	FileCount;
	
	byte	SID_Level5[8];
	byte	SID_Level4[8];
	byte	SID_Level3[8];
	byte	SID_Level2[8];
	byte	SID_Level1[8];
	
	SharedDirectory(void)
	{
		Name			= "";
		Recursive		= false;
		Size			= 0;
		FileCount		= 0;
		memset(SID_Level5, 0x00, 8 );
		memset(SID_Level4, 0x00, 8 );
		memset(SID_Level3, 0x00, 8 );
		memset(SID_Level2, 0x00, 8 );
		memset(SID_Level1, 0x00, 8 );
	};
};

struct ShareChange
{
	DWORD Index;
	DWORD Type;
	DWORD Value;
};

struct RemoteFile
{
	UINT Index;
	UINT Size;

	std::basic_string<char> Name;
	std::basic_string<char> NameLower;
	std::basic_string<char> Sha1Hash;

	CString strInfo;
	std::vector<CString> Info;
};

// 
struct WordData
{
	std::basic_string<char> Text;

	std::vector<UINT> Indexes;
};

struct WordKey
{
	std::vector<WordData>* LocalKey;  // Locally indexed
	std::list<CGnuNode*>*  RemoteKey; // Remotely indexed	

	WordKey() 
	{ 
		LocalKey = NULL; 
		RemoteKey = NULL;
	};

	~WordKey() 
	{ 
		if(LocalKey)
		{
			delete LocalKey;
			LocalKey = NULL;
		}
		if(RemoteKey)
		{
			delete RemoteKey;
			RemoteKey = NULL;
		}
	};
};

struct RemoteIndex
{
	CGnuNode* pSock;

	IP		Host;
	UINT	Port;
	UINT	Speed;
	GUID    PushID;

	bool	Firewall;
	bool	Busy;
	bool	Stable;
	bool	RealSpeed;

	UINT	FilesCount;
	UINT	FilesSize;

	std::vector<RemoteFile> Files;
};


struct FullIcon
{
	int Index;
	CString Type;
	FullIcon(void)
	{
		Index	= -1;
		Type	= "";
	};
};

struct QueryComp
{
	std::basic_string<char> QueryText;
	std::vector<CString> QueryExtended;

	CGnuNode*    Origin;
	GUID		 QueryGuid;
	int			 nHops;

	bool SendHash;
	bool SendSha1;
	bool SendBitprint;

	bool	MatchHash;
	CString MatchSha1;
	CString MatchBitprint;

	byte	SID_Level5[8];
	byte	SID_Level4[8];
	byte	SID_Level3[8];
	byte	SID_Level2[8];
	byte	SID_Level1[8];

	CString ID;
};

struct RecentQuery
{
	GUID Guid; 
	int  SecsOld;
	
	RecentQuery();

	RecentQuery(GUID cGuid)
	{
		Guid    = cGuid;
		SecsOld = 0;
	}
};

struct Result
{
	// File info
	CString   Name;
	CString   NameLower;

	CString strInfo;
	std::vector<CString> Info;
	
	CString	  Sha1Hash;
	CString   BitprintHash;
	

	DWORD     FileIndex;
	// modified by moonknit 2005-05-20
	// DWORD   SIZE;
	INT64   Size;
	// -- end
	FullIcon  Icon;
	
	// Node info
	IP		  Host;
	WORD      Port;
	int       Speed;
	CString	  UserID;
	CString	  CID;

	// added by moonknit 2005-05-20
	CString   MetaData;
	// -- end

	int	RealBytesPerSec;

	// Flags
	bool Firewall;
	bool OpenSlots;
	bool Busy;
	bool Stable;
	bool ActualSpeed;


	// Push info
	CGnuNode* Origin;
	GUID      PushID;
	int		  Distance;

	// Download info
	CString	  Handshake;
	CString   Error;
	int		  RetryWait;
	int		  Tries;
	bool	  Alive;
	bool	  FirstTry;
	bool	  Corrupt;

	WORD		wVersion;

	// written by moonknit 2005-09-06
	bool		Trying;
	
	// by bemlove
	bool	bRetryFailed;	// 재연결상태에서 다시 연결 시도에 실패한 Queue임
	bool	bFileNotFound; //공유자가 파일을 제거한 상태임.
	int		nConnectFailCount;	// 연결시도 실패 횟수.
	Result() 
	{
		Name			= "";
		NameLower		= "";
		strInfo			= "";
		Sha1Hash		= "";
		BitprintHash	= "";
		FileIndex		= 0;
		Size			= 0;
		Port			= 0;
		Speed			= 0;
		UserID			= "";

		RealBytesPerSec = 0;

		Firewall		= false;
		OpenSlots		= false;
		Stable			= false;
		ActualSpeed		= false;
		Origin			= NULL;
		memset( &PushID, 0x00, sizeof(GUID) );
		Handshake		= "";
		Error			= "";
		RetryWait		= 0;
		Tries			= 0;
//		Alive			= false;
		Alive			= true;
		FirstTry		= false;
		Corrupt			= false;

		bRetryFailed	= false;
		bFileNotFound	= false;
		nConnectFailCount = 0;

		MetaData		= "";

		Trying			= false;
	};

	Result(AltLocation& nAltLoc)
	{
		Name = NameLower = nAltLoc.Name;
		NameLower.MakeLower();
		Sha1Hash = nAltLoc.Sha1Hash;
		FileIndex = nAltLoc.Index;
		Host = StrtoIP(nAltLoc.HostPort.Host);
		Port = nAltLoc.HostPort.Port;
	}

	// 속도순 정렬..(Speed)
	inline bool operator < ( struct Result& first )
	{
		bool bResult = false;
		if( first.Speed < Speed )
			bResult = true;
		else
			bResult = false;

		return bResult;
	}
};

// written by moonknit 2005-05-18
// copied from Old Enppy 2.0 Mars2FileSearchView.h
struct ResultGroup
{
	GUID	 SearchGUID;
	CString  Name;
	CString  NameLower;
	
	// Info 는 HostIP를에 넣는다.
	CString  Info;

	DWORD    Size;
	DWORD    AvgSpeed;

	CString Sha1Hash;
	CString BitprintHash;

	std::vector<Result> ResultList;

	FullIcon Icon;

	bool SyncNeeded;
	bool IconSync;
	
	static int  SortBy;

	bool bTrying;
	bool bDownloading;
	bool bCompleted;
	bool bNoHosts;

	int		Type;

	ResultGroup()
	{
		Name			= "";
		NameLower		= "";
		Info			= "";
		Size			= 0;
		AvgSpeed		= 0;
		Sha1Hash		= "";
		BitprintHash	= "";
		ResultList.clear();
		SyncNeeded		= false;
		IconSync		= false;
		SortBy			= 0;

		bTrying			= false;
		bDownloading	= false;
		bCompleted		= false;
		bNoHosts		= false;

		Type			= 0;
	}

	inline bool operator < (struct ResultGroup& first)
	{
		bool bResult = false;

		if (SortBy<0)
		{
			switch (abs(SortBy))
			{
			// Name
			case 1:
				if(first.NameLower > NameLower)
					bResult = true;
				else if(first.NameLower == NameLower)
					bResult = (first.Size > Size);
				break;

			// Size
			case 2:
				if(first.Size > Size)
					bResult = true;
				else if(first.Size == Size)
					bResult = (first.NameLower > NameLower);
				break;

			// Type
			case 3:
				if(first.Icon.Type > Icon.Type)
					bResult = true;
				else if(first.Icon.Type == Icon.Type)
					bResult = (first.NameLower > NameLower);
				break;

			// Speed
			case 4:
				if(first.AvgSpeed > AvgSpeed)
					bResult = true;
				else if(first.AvgSpeed == AvgSpeed)
					bResult = (first.NameLower > NameLower);
				break;

			// Host
			case 5:
				if(first.ResultList.size() > ResultList.size())
					bResult = true;
				else if(first.ResultList.size() == ResultList.size())
					bResult = (first.NameLower > NameLower);
				break;

			default:
				bResult = (first.NameLower > NameLower);
				break;
			}
		}
		else
		{
			switch (abs(SortBy))
			{
			// Name
			case 1:
				if(first.NameLower < NameLower)
					bResult = true;
				else if(first.NameLower == NameLower)
					bResult = (first.Size < Size);
				break;

			// Size
			case 2:
				if(first.Size < Size)
					bResult = true;
				else if(first.Size == Size)
					bResult = (first.NameLower < NameLower);
				break;

			// Type
			case 3:
				if(first.Icon.Type < Icon.Type)
					bResult = true;
				else if(first.Icon.Type == Icon.Type)
					bResult = (first.NameLower < NameLower);
				break;

			// Speed
			case 4:
				if(first.AvgSpeed < AvgSpeed)
					bResult = true;
				else if(first.AvgSpeed == AvgSpeed)
					bResult = (first.NameLower < NameLower);
				break;

			// Host
			case 5:
				if(first.ResultList.size() < ResultList.size())
					bResult = true;
				else if(first.ResultList.size() == ResultList.size())
					bResult = (first.NameLower < NameLower);
				break;

			default:
				bResult = (first.NameLower < NameLower);
				break;
			}
		}

		return bResult;
	};

	inline bool operator == (struct ResultGroup& first)
	{
		bool bResult = false;

		switch (abs(SortBy))
		{
		// Name
		case 0:
			if (first.NameLower == NameLower)
			{
				bResult = true;
			}
			break;

		// Size
		case 1:
			if (first.Size == Size)
			{
				bResult = true;
			}
			break;

		// Type
		case 2:
			if (first.Icon.Type == Icon.Type)
			{
				bResult = true;
			}
			break;

		// Speed
		case 3:
			if (first.AvgSpeed == AvgSpeed)
			{
				bResult = true;
			}

			break;

		// Host
		case 4:
			if (first.ResultList.size() == ResultList.size())
			{
				bResult = true;
			}
			break;

		/*Hashed
		case 6:
				if(first.Hash.IsEmpty() && Hash.IsEmpty())
				{
					bResult = true;
				}
				break;
		// File Info
		case 7:
				if(first.Info == Info)
				{
					bResult = true;
				}
				break;
		*/

		default:
			bResult = (first.NameLower == NameLower);
			break;
		}

		return bResult;
	}
};

// -- end


struct UpdateFile
{
	CString Name,
			Source,
			Destination;
};

/*************************************************************
To trace lock step
created by moonknit
\*************************************************************/
struct LogLock
{
	CCriticalSection* m_pSection;
	CString sOldLog, sCurLog;

	LogLock()
	{
		m_pSection = NULL;
		sOldLog = "";
		sCurLog = "";
	};

	void SetCriticalSection(CCriticalSection* pSection)
	{
		m_pSection = pSection;
	};

	BOOL Lock(CString sLog = "", int nTime = INFINITE)
	{
		if(!m_pSection) return FALSE;
		TRACE(_T("[%d][Lock] %s <= %s [TRY]\n"), AfxGetThread()->m_nThreadID, sLog, sCurLog);

		BOOL bReturn = m_pSection->Lock(INFINITE);

		if(bReturn)
		{
			sOldLog = sCurLog;
			sCurLog = sLog;
			TRACE(_T("[%d][Lock] %s <= %s [GET]\n"), AfxGetThread()->m_nThreadID, sCurLog, sOldLog);
		}
		else
		{
			TRACE(_T("[%d][Lock] %s <= %s [FAIL]\n"), AfxGetThread()->m_nThreadID, sLog, sCurLog);
		}

		return bReturn;
	};

	BOOL Unlock(CString sLog = "")
	{
		if(!m_pSection) return FALSE;
		TRACE(_T("[%d][Unlock] step[%s] of  %s\n"), AfxGetThread()->m_nThreadID, sLog, sCurLog);

		return m_pSection->Unlock();
	};
};

struct MergeItem
{
	LPVOID	pMaster;
	LPVOID	pPreChunk;
	LPVOID	pPostChunk;
};

struct MergeResult
{
	BOOL	bSuccess;
	BOOL	bC2P;
	LPVOID	pPreChunk;
	LPVOID	pPostChunk;
};

struct FileChunk
{
	std::vector<IP>	Hosts;

	CFileLock	File;
	CString		Name;
	CString		Path;

	CString Sha1Hash;
	
	INT64 StartPos;
	INT64 ChunkLength;
	INT64 BytesCompleted;
	INT64 PreCompletedBytes;//이전에 전송된 사이즈

	// added by moonknit
	BOOL bComplete;
	BOOL bMerging;
	CCriticalSection csLock;
	LogLock ChunkLock;
	// -- added

	int OverlapBytes;

	COLORREF ChunkColor;
	int HostFamily;
	FileChunk()
	{
		StartPos = 0;
		ChunkLength = 0;
		BytesCompleted = 0;
		PreCompletedBytes = 0;

		// added by moonknit
		bComplete = FALSE;
		bMerging = FALSE;
		ChunkLock.SetCriticalSection(&csLock);
		// -- added
	};
};

struct PartialFile
{
	CString   FileName;
	CString   FullName;
	FullIcon  Icon;

	CString Keyword;

	CString Sha1Hash;


	int   FileLength;
	int   BytesCompleted;

	std::vector<FileChunk*> Chunks;
};


struct BlockedHost
{
	IP StartIP;
	IP EndIP;

	CString Reason;
};

class GnuMessage
{
public:
	GnuMessage(int, int);
	~GnuMessage();

	int Type;
	int Length;

	byte* Packet;
};


class PriorityPacket
{
public:
	PriorityPacket(int, int);
	~PriorityPacket();
	
	int   Type;
	int   Length;

	byte* Packet;
};

struct MapNode
{
	UINT MapID;

	IP		Host;
	WORD    Port;
	UINT	FileCount;
	WORD	LeafCount;
	WORD	State;
	char    Client[4];

	MapNode(void)
	{
		MapID		= 0;
		Port		= 0;
		FileCount	= 0;
		LeafCount	= 0;
		State		= 0;
		memset( Client, 0x00, sizeof(Client) );
	};
};

struct MapLink
{
	DWORD ID;
	DWORD ParentID;
	DWORD Hops;
};

struct MapPongList
{
	IP   Host;
	UINT Port;

	UINT FileCount;
	WORD LeafCount;
	WORD State;

	CGnuNode* PrimNode;
};

struct ErrorInfo
{
	CTime Time;

	CString Description;

	ErrorInfo():Time(CTime::GetCurrentTime()) { }
};

// written by moonknit 2005-08-10
// to make encoded ip, port string and decode
void enc_hostinfo(const char* ip, int port, int findex, char* result);
void dec_hostinfo(const char* src, char* ip, int& port, int& findex);

void PrintError(DWORD dwError, LPTSTR lpszError);
#endif