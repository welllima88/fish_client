#if !defined(AFX_PACK_H__A2D4CDEA_22B4_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_PACK_H__A2D4CDEA_22B4_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _UNICDE
//#define USE_UTFCONVERT
#endif

#ifdef USE_UTFCONVERT
#define ET_UTF8						1
#define ET_UTF16					0
#endif

#pragma pack (push, 1)


#include <memory>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include <deque>

class CGnuNode;

#define APP_VERSION								MAKEWORD(1, 0)

#define GNU_DELIMITER							_T("")
#define GNU_DELIMITER2							_T("|")

union IP										// Size 4 	
{
	struct { u_char s_b1,s_b2,s_b3,s_b4; };		// S_un_b
	struct { u_short s_w1,s_w2; };				// S_un_w
	struct {BYTE a, b, c, d;};					// old_IP
	u_long S_addr;

	inline int operator > (union IP& first)
	{	
		bool result = false;

		if(a > first.a)
			result = true;
		else if(a == first.a)
		{
			if(b > first.b)
				result = true;
			else if(b == first.b)
			{
				if(c > first.c)
					result = true;
				else if(c == first.c)
				{
					if(d > first.d)
						result = true;
				}
			}
		}
	
		return result;
	};
};

struct ExIP
{
	int  a, b, c, d; // 0 - 255 is ip num, -1 is wildcard
	bool mode;       // false for deny, true for allow
};

struct Node
{
	CString Host;
	UINT    Port;
	CTime   Time;

	Node():Time(CTime::GetCurrentTime()) { };


	// Create a node based on a standard "Host:port" string
	Node(CString HostPort);
	Node(CString nHost, UINT nPort);

	void Clear() { Host = ""; Port = 0; };

	// Allow Node = "host:port" assignment
	Node& operator=(CString &rhs);
	CString GetString();
	bool operator == (Node& first)
	{
		if (Host == first.Host && Port == first.Port)
			return true;
		return false;
	};
};

struct LanNode /*  : public Node */
{
	CString Host;
	UINT    Port;
	UINT	Leaves;

	CString Name;
	CString IRCserver;
	//CString UpdateServer;
	CString InfoPage;

};

struct NodeEx /* : public Node */
{
	IP   Host;
	UINT Port;

	UINT Ping;
	UINT Speed;
	byte Distance;

	UINT ShareSize;
	UINT ShareCount;
	
	std::vector<Node*> Friends;
};

/////////////////////////////////////////////////////////////////////////////
// Gnutella packets

#define SIZE_PINGPACKET					23
#define SIZE_PONGPACKET					37

struct packet_Header		// Size 23
{
	GUID  Guid;					// 0  - 15							
	BYTE  Function;				// 16
	BYTE  TTL;					// 17
	BYTE  Hops;					// 18							
	UINT  Payload;				// 19 - 22
};


struct packet_Ping			// Size 23
{
	packet_Header Header;		// 0  - 22	
};


// modified by moonknit
// add LeafCount & State
// removed FileSize
struct packet_Pong			// Size 37
{
	packet_Header Header;		// 0  - 22							
	WORD Port;					// 23 - 24
	IP Host;					// 25 - 28
	UINT FileCount;				// 29 - 32
	WORD LeafCount;				// 33 - 34
	WORD State;					// 35 - 36
};


struct packet_Push			// Size 49
{
	packet_Header Header;		// 0  - 22;							
	GUID ServerID;				// 23 - 38
	UINT Index;				    // 39 - 42
	IP Host;					// 43 - 46
	WORD Port;					// 47 - 48
};


struct packet_Query			// Size 26+
{		
	packet_Header Header;		// 0  - 22						
	WORD Speed;					// 23 - 24
	// Search					// 25+
};


struct packet_QueryHit		// Size 35+
{
	packet_Header Header;		// 0  - 22
	byte TotalHits;				// 23
	WORD Port;					// 24 - 25
	IP   Host;					// 26 - 29
	UINT Speed;					// 30 - 33
#ifdef USE_UTFCONVERT
	byte enctype;				// 34
#endif
	// QueryHitItems			// 34+ or if USE_UTF8CONVERT defined, 35+
	
	// QueryHit Descriptor

	// ClientGuid				// Last 16 bytes

};

struct packet_QueryHitItem	// Size 12+
{
	UINT flag;				// 0 - 3
	UINT pubdate;			// 4 - 7
	UINT Index;				// 8 - 11
	
	// FileName				// 11+	
};

struct packet_QueryHitEx	    // Size 5+	(25)
{
	byte Length;					// 0

	// Public Sector
	unsigned	Push		 : 1;	// 1
	unsigned	FlagBad 	 : 1;
	unsigned	FlagBusy	 : 1;
	unsigned	FlagStable   : 1;
	unsigned	FlagSpeed	 : 1;
	unsigned    FlagTrash    : 3;

	unsigned    FlagPush	 : 1;	// 2
	unsigned	Bad			 : 1;
	unsigned	Busy		 : 1;
	unsigned	Stable  	 : 1;
	unsigned	Speed		 : 1;
	unsigned    Trash		 : 3;

	WORD		wVersion;			// 3~4
	char UserID[21];				// (5 - 25)
};

struct packet_RouteTableReset	// Size 29
{
	packet_Header Header;		// 0  - 22

	byte PacketType;			// 23
	UINT TableLength;			// 24 - 27
	byte Infinity;				// 28
};

struct packet_RouteTablePatch	// Size 29+
{
	packet_Header Header;		// 0  - 22

	byte PacketType;			// 23           
	byte SeqNum;				// 24
	byte SeqSize;				// 25

	byte Compression;		    // 26
	byte EntryBits;				// 27

	// Patch Table...			// 28+
};

struct packet_Log
{
	packet_Header* Header;

	int PacketLength;

	CGnuNode* Origin;
	UINT      dwError;

	int nTTL;
	int nHops;

// by bemlove at 2003-07-11 ¿ÀÈÄ 5:31:41
//	UINT nRunCountry;
};

#pragma pack (pop)

#endif // !defined(AFX_PACK_H__A2D4CDEA_22B4_11D4_ACF2_00A0CC533D52__INCLUDED_)


