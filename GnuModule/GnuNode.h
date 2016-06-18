#if !defined(AFX_GNUNODE_H__3774ACC1_CB81_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUNODE_H__3774ACC1_CB81_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"
#include "GnuDefines.h"


#define PACKET_BUFF	131072

class CGnuControl;

class CGnuNode : public CAsyncSocket
{
public:
	int m_NodeStatus;

	CGnuNode(CGnuControl* pComm, CString Host, UINT Port);
	virtual ~CGnuNode();


	// New connections
	void	ParseHandshake04(CString, byte*, int);
	void	ParseIncomingHandshake06(CString, byte*, int);
	void	ParseOutboundHandshake06(CString, byte*, int);
	
	CString FindHeader(CString);
	void	ParseTryHeader(CString TryHeader, bool Super=false);

	void SetConnected();
	void Send_ConnectOK(bool, int);
	void Send_ConnectBusy();


	// Sending packets
	void SendPacket(void*, int, int, bool, bool thread=false);

	void Send_Ping(int);
	void Send_Pong(GUID, int);
	void Send_PongCache(packet_Log*, int);
	void Send_QueryHit(SE_QUERY&, byte*, DWORD, byte, RemoteIndex*);
	void Send_ForwardQuery(QueryComp);
	void Send_PatchTable();
	

	// Misc functions
	bool GetAlternateHostList(CString &);
	bool GetAlternateSuperList(CString &);
	void BuildLog(packet_Log* , packet_Header* , int );
	int  UpdateStats(int type, int &);
	void AvoidTriangles();

	void Refresh();
	void RemovePacket(int);
	void Timer();

	// ==[START] Date : 2004-12-07,	By bemlove
	// 노드 관리를 위한 추가 함수
	void SendOtherNodeInfo(void);
	// ==[END]  by bemlove
	// Connected Node's info
	int	m_Status;
	int	m_nSecsTrying;
	int	m_nSecsDead;
	int m_ClosedSecs;
	int m_nSecsAlive;
	int	m_nSecsRefresh;				// Seconds since last refresh
	int m_IntervalPing;
	int m_nOrigDistance;
	int m_NextReSearchWait;

	CString m_StatusText;
	CString m_HostIP;
	CString m_HostName;
	UINT    m_Port;
	UINT    m_HostMode;
	
	CString m_NetworkName;
	bool    m_Guerilla;

	bool    m_Inbound;				// the flag which indicates that this node is in-bound node
	CString m_InitData;

	CString m_WholeHandshake;
	CString m_Handshake;
	CString m_lowHandshake;

	CTimeSpan m_HostUptime;
	CTime   m_ConnectTime;
	DWORD   m_Efficeincy;
	UINT	m_NodeFileCount;
	DWORD	m_NodeLeafCount;
	int		m_NodeLeafMax;

	bool	m_DowngradeRequest; // Is true if we request node to become child, or remote node requests us to become a child node
	bool	m_PatchUpdateNeeded;

	bool	m_UltraPongSent;

	bool IsRecentQuery(GUID Guid);
	std::vector<RecentQuery> m_RecentQueryList;

	// Ultrapeers
	UINT	m_TableInfinity;
	UINT	m_TableLength;

	char*	m_PatchTable;
	UINT	m_TableNextPos;

	byte*	m_CompressedTable;
	UINT	m_CompressedSize;

	UINT	m_CurrentSeq;

	// Pong caching
	bool  m_PongCachingEnabled;

	int   m_BestPing;
	DWORD m_TimeLastPing;

	DWORD m_PingAcceptTime; // time we can accept a new ping
	DWORD m_PingSendTime;	// time we can send a new ping

	int  m_LastHops;				// where to send the next pongs
	int  m_NextTTL;					// where to send the next ping
	GUID m_LastGuid;				// last guid recieved from a ping
	int  m_PongNeeded[MAX_TTL - 1];	// array of pongs needed by the connection
	void Send_Pong(MapPongList*);	// send cached pong 
	void Check_PongNeeds(MapPongList*,int); // called when we receive a non cached pong


	// Regular ping/pong info
	DWORD   m_dwFriendsTotal;

	DWORD   m_dwFriends[MAX_TTL];

#ifdef NODE_CONNECTIVITY
	// written by moonknit 2005-05-16
	// 연결되 노드의 연결 강도
	WORD	m_nConnectivity;

	// written by moonknit 2005-06-17
	void	SetConnectivity(int nConSrc);
#endif

	// written by moonknit 2005-06-17
	DWORD	m_nStartTime;
	BOOL	m_bValidNode;
	
	BOOL	IsValidNode();

	// Mapping
	void MapPong(packet_Pong* Pong, bool Near);
	void MapQueryHit(packet_QueryHit* QueryHit);

	std::vector<MapNode> NearMap[2];
	std::vector<UINT>	 LocalMap[MAX_TTL];
	std::vector<MapLink> MapLinkList;

	// Packet Info
	char  m_StatPackets[PACKETCACHE_SIZE][2]; // Last 1000 packet, can be set either bad(0) or good(1)
	int   m_StatPos;			  // Position in array
	// removed by moonknit 2005-07-05
	// useless
//	int   m_StatElements;
	int   m_StatPings[2],     m_StatPongs[2], m_StatQueries[2],
		  m_StatQueryHits[2], m_StatPushes[2];  // Total received during last 1000 packets and total that were good

	// Bandwidth, [0] is Received, [1] is Sent, [2] is Dropped
	double m_dwTotalPackets[3]; // Packets received/sent in past 60 seconds
	double m_dwTotalBytes[3];   // Bytes received/sent in past 60 seconds

	DWORD m_dwSecPackets[3];   // Packets received/sent in second
	DWORD m_dwSecBytes[3];     // Bytes received/sent in second
	
	double m_nSecNum;	       // Number of elements in array

	int Send(CString& szBuf, int nFlags = 0);

	//{{AFX_VIRTUAL(CGnuNode)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnSend(int nErrorCode);
	virtual void Close();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuNode)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


protected:
	// Packet handlers
	DWORD SplitBundle(byte*, DWORD);
	void  HandlePacket(packet_Header*, DWORD);
	bool  InspectPacket(packet_Header*);


	// Receiving packets
	void Recieve_Ping(packet_Ping*,   int);
	void Recieve_Pong(packet_Pong*,   int);
	void Recieve_Push(packet_Push*,   int);
	void Recieve_Query(packet_Query*, int);
	void Recieve_QueryHit(packet_QueryHit*, DWORD);
	
	void Recieve_RouteTableReset(packet_RouteTableReset*, UINT);
	void Recieve_RouteTablePatch(packet_RouteTablePatch*, UINT);
		
	void Recieve_Unknown(byte*, DWORD);

	// written by moonknit 2005-05-17
	void ParseQueryHit(packet_Log* pPacket);
	// -- end

	DWORD GetSpeed();
	void  NodeManagement();
public:
	void  NodeManagement2(bool connected);
protected:
	bool m_ThreadCall;


	// Bandwidth calculations, [0] is Received, [1] is Sent, [3] is dropped
	DWORD	m_dwAvgPackets[3][60];  // Average packets from last 60 seconds
	DWORD	m_dwAvgBytes[3][60];    // Average bytes from last 60 seconds
	
	int		m_nSecPos;			    // Position of next element insert

	CGnuControl*  m_pComm;

	DWORD m_dwExtraLength;
	byte  m_pBuff[PACKET_BUFF];
	byte  m_pExtra[PACKET_BUFF];

	// Packet Prioritization functions and variables
	CCriticalSection m_BufferAccess;
	void FlushSendBuffer();

	std::list<PriorityPacket> m_PacketList[6];
	int m_PacketListLength[6];
//	int m_PacketListLength[7];
//	std::list<PriorityPacket> m_PacketList[7];

	bool m_SendReady;
	byte m_BackBuff[65536];
	int  m_BackBuffLength;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNUNODE_H__3774ACC1_CB81_11D4_ACF2_00A0CC533D52__INCLUDED_)
