#ifndef _GNUDEFINES_H
#define _GNUDEFINES_H

#include <list>

#include "../IrcModule/Irc_defines.h"

//using namespace std;

class CGnuNode;

#ifndef uint64
typedef unsigned char		uchar;
typedef unsigned char		uint8;
typedef	  signed char		sint8;

typedef unsigned short		uint16;
typedef	  signed short		sint16;

typedef unsigned int		uint32;
typedef	  signed int		sint32;

typedef unsigned __int64	uint64;
typedef   signed __int64	sint64;

#endif

// FISH 버전 3.0 이후 파일 다운로드가 지원되면 사용한다.
//#define USE_MERGETHREAD

#define MAX_TTL								7
#define MAX_SHARE							31458312
#define CONNECT_TIMEOUT						4
#define TRANSFER_TIMEOUT					8
#define NODECACHE_SIZE						200
#define PACKETCACHE_SIZE					1000
#define MAX_EVENTS							128
#define CHAT_MAXLINES						2000

// Hardcoded Enums
#define CLIENT_NORMAL						1
#define CLIENT_ULTRAPEER					2
#define CLIENT_LEAF							3

#define UPDATE_RELEASE						1
#define UPDATE_BETA							2
#define UPDATE_NONE							3

#define NETWORK_INTERNET					1
#define NETWORK_LAN							2
#define NETWORK_PRIVATE						3

#define PACKET_CONNECTIVITY					6
#define PACKET_PING							5
#define PACKET_PONG							4
#define PACKET_QUERY						3
#define PACKET_QUERYHIT						2
#define PACKET_PUSH							1

#define PREF_LOCAL							1
#define PREF_LOCALNETWORK					2
#define PREF_LOCALFIREWALL					3
#define PREF_CONNECT						4
#define PREF_CONNECTSERVER					5
#define PREF_CONNECTSCREEN					6
#define PREF_SEARCH							7
#define PREF_SEARCHSCREEN					8
#define PREF_SEARCHBLOCK					9
#define PREF_SHARE							10
#define PREF_TRANSFER						11
#define PREF_BANDWIDTH						12
#define PREF_CHAT							13
#define PREF_PLUGINS						14

#define SHARE_CHANGE_REMOVE					1
#define SHARE_CHANGE_SEARCH					2
#define SHARE_CHANGE_UPLOAD					3
#define SHARE_CHANGE_HASH					4

#define ROOM_SERVER							1
#define ROOM_RAW							2
#define ROOM_CHANNEL						3
#define ROOM_PRIVATE						4

#define POINT_LIMIT							50

#define DEFAULT_GNUPORT						4679
#define MAX_GNUPORT							50000
#define MIN_GNUPORT							2000

#define DEFAULT_MAXINCONNECTS				4
#define DEFAULT_MAXOUTCONNECTS				3

#define DEFAULT_MAXUPLOADS					5
#define DEFAULT_MAXDOWNLOADS				5

// NETWORK MODEL
#define NETWORK_INTERNET					1
#define NETWORK_LAN							2
#define NETWORK_PRIVATE						3

////////////////////////////////////////////////////////////////////
// 전송 상태 스트링
////////////////////////////////////////////////////////////////////
#define STR_TRANSFER_UNKNOWN				_T("대기중")
#define STR_TRANSFER_CANCEL					_T("전송취소")
// download state
#define STR_TRANSFER_NEEDSOURCE				_T("공유자 없음")
#define STR_TRANSFER_STOP					_T("일시정지")
#define STR_TRANSFER_QUEUED					_T("대기중")
#define STR_TRANSFER_CONNECTING				_T("연결중")
//#define STR_TRANSFER_CONNECTING				STR_TRANSFER_QUEUED
#define STR_TRANSFER_RECEIVING				_T("받는중")
#define STR_TRANSFER_CLOSED					_T("완료")
#define STR_TRANSFER_COOLDOWN				STR_TRANSFER_QUEUED
#define STR_JOIN_CHUNK						_T("합치는중")
#define STR_TRANSFER_DELETED				_T("삭제됨")
// upload state
#define STR_TRANSFER_CONNECTED				STR_TRANSFER_CONNECTING		
#define STR_TRANSFER_SENDING				_T("보내는중")
#define STR_TRANSFER_PUSH					STR_TRANSFER_SENDING

////////////////////////////////////////////////////////////////////
// from IrcdManager.cpp
////////////////////////////////////////////////////////////////////

#define STR_MSG_IRCDMANGER_001				_T("다른곳에서 동일 아이디로 로그인 하였습니다.\n동일 사용자의 요청에 의해 프로그램을 종료합니다.")
#define STR_MSG_IRCDMANGER_002				_T("ENPPY 종료")
#define STR_MSG_IRCDMANGER_003				_T("현재 네트웍 연결에 문제가 있습니다. 네트웍 상태를 확인해 주시기 바랍니다.")
#define STR_MSG_IRCDMANGER_004				_T("서버와의 네트웍 연결이 끊겼습니다.\n네트웍의 연결상태를 확인하신 후에 엔피를 재실행해 주시기 바랍니다.")
#define STR_MSG_IRCDMANGER_005				_T("ENPPY-네트웍 오류")
#define STR_MSG_IRCDMANGER_006				_T("비밀번호가 틀립니다. 다시 한 번 확인해 보시기 바랍니다.")
#define STR_MSG_IRCDMANGER_007				_T("인증실패")
#define STR_MSG_IRCDMANGER_008				_T("알 수 없는 사용자 정보 입니다.")
#define STR_MSG_IRCDMANGER_009				_T("엔피를 종료하겠습니다.")
#define STR_MSG_IRCDMANGER_010				_T("다른곳에서 이 ID로 접속중입니다.\n다른곳의 접속을 끊으시겠습니까?")
#define STR_MSG_IRCDMANGER_011				_T("참여할 채널이 존재하지 않아 접속할 수 없습니다.\n엔피를 종료하겠습니다.")
#define STR_MSG_IRCDMANGER_012				_T("인터넷 공유기나 사설 IP를 사용하고 있기 때문에\nENPPY에서 공유 목록을 열어 놓을 수 없습니다.\n메신저 및 다른 ENPPY 공유자의 파일 다운로드만 가능합니다.\n(단, 공유가 가능한 네트웍 환경 세팅을 마친 경우에는\n[환경설정]->[엔피옵션]->[네트워크] 에서 \n'수동으로 공유환경 설정'을 선택하시고\nENPPY를 재실행 하시면 바로 공유가능합니다.")
#define STR_MSG_IRCDMANGER_013				_T(" %s초 이후에 '%s'로 인하여 ENPPY를 사용하실 수 없습니다.")
#define STR_MSG_IRCDMANGER_014				_T("ENPPY 시스템 정비")
#define STR_MSG_IRCDMANGER_015				_T("ENPPY 안내")
#define STR_MSG_IRCDMANGER_016				_T("네트웍 문제로 인해 ENPPY가 자동 재실행됩니다.")	//new
#define STR_MSG_IRCDMANGER_017				_T("현재 Enppy 포인트 관리를 위한 서버가 존재하지 않아 접속하실 수 없습니다.\n잠시 후 다시 실행해 주시기 바랍니다.") //new
#define STR_MSG_IRCDMANGER_018				_T("ENPPY-서버 오류") //new
#define STR_MSG_IRCDMANGER_019				_T("ENPPY 포인트 관리 서버 오류로 인해 다시 실행 중입니다.") //new
#define STR_MSG_IRCDMANGER_020				_T("현재 ENPPY 포인트 관리 서버의 오류로 인해 실행 대기중 입니다.") //new
#define STR_MSG_IRCDMANGER_021				_T("웹 페이지 정보를 설정하는 과정에서 오류가 발생하였습니다.\n엔피를 종료하겠습니다.\n잠시 후 다시 실행 바랍니다.")//new
#define STR_MSG_IRCDMANGER_022				_T("사용자 정보를 설정하는 과정에서 오류가 발생하였습니다.\n엔피를 종료하겠습니다.\n잠시 후 다시 실행 바랍니다.")//new


// -- socket error ----------------------------------------------
#define STR_MSG_COMMON_001					_T("현재 다른 프로그램에서 사용하는 포트와 충돌이 발생하였습니다.\n포트번호를 변경 하시겠습니까?")

#define STR_MSG_SOCKERROR_001				_T("지정된 주소가 이미 사용중입니다")
#define STR_MSG_SOCKERROR_002				_T("지정된 주소는 사용이 불가합니다")
#define STR_MSG_SOCKERROR_003				_T("지정된 패밀리 주소는 이 소켓과 사용할 수 없습니다")
#define STR_MSG_SOCKERROR_004				_T("현재 서버가 종료되어 연결을 하실 수 없습니다")
#define STR_MSG_SOCKERROR_005				_T("목적지 주소가 필요합니다")
#define STR_MSG_SOCKERROR_006				_T("lpSocketAddrLen 인자가 잘못되었습니다")
#define STR_MSG_SOCKERROR_007				_T("이 소켓은 이미 다른 주소에 바인딩되어 있습니다")
#define STR_MSG_SOCKERROR_008				_T("이 소켓은 이미 연결되어 있습니다")
#define STR_MSG_SOCKERROR_009				_T("사용가능한 파일 기술자가 없습니다")
#define STR_MSG_SOCKERROR_010				_T("목적지에 도달할 수 없습니다")
#define STR_MSG_SOCKERROR_011				_T("사용가능한 버퍼공간이 없습니다")
#define STR_MSG_SOCKERROR_012				_T("이 소켓은 연결된 소켓이 아닙니다")
#define STR_MSG_SOCKERROR_013				_T("이 기술자는 소켓이 아니라 파일입니다")
#define STR_MSG_SOCKERROR_014				_T("연결시간이 초과되었습니다")
#define STR_MSG_SOCKERROR_015				_T("고속모드로 세팅할 수 없습니다.")
#define STR_MSG_SOCKERROR_016				_T("KeepAlive모드로 세팅할 수 없습니다.")
#define STR_MSG_SOCKERROR_017				_T("패킷 읽어올 메모리 부족")
#define STR_MSG_SOCKERROR_018				_T("패킷 저장할 메모리 부족")

// Windows Custom Messages
#define SOCK_CONNECTING						(WM_APP + 1)
#define SOCK_CONNECTED						(WM_APP + 2)
#define SOCK_CLOSED							(WM_APP + 4)

#define SOCK_UPDATE							(WM_APP + 5)

#define PACKET_GOOD							(WM_APP + 10)
#define PACKET_BAD							(WM_APP + 11)
#define PACKET_INCOMING						(WM_APP + 12)

#define ERROR_HOPS							(WM_APP + 20)
#define ERROR_LOOPBACK						(WM_APP + 21)
#define ERROR_TTL							(WM_APP + 22)
#define ERROR_DUPLICATE						(WM_APP + 23)
#define ERROR_ROUTING						(WM_APP + 24)

#define PREFERENCES_CHANGE					(WM_APP + 25)

#define SHARE_RELOAD						(WM_APP + 30)
#define SHARE_UPDATE						(WM_APP + 31)

#define NODECACHE_ADD						(WM_APP + 35)
//#define WM_TRAYICON_NOTIFY					(WM_APP + 36)

#define UPLOAD_UPDATE						(WM_APP + 40)
#define DOWNLOAD_UPDATE						(WM_APP + 41)
#define PARTIAL_UPDATE						(WM_APP + 42)
#define WM_DELLIST							(WM_APP + 43)

#define TRANSFER_CANCEL						(WM_APP + 47)		// added by moonknit 2005-07-05
#define TRANSFER_NEEDSOURCE					(WM_APP + 48)		// added by moonknit 2005-06-16
#define TRANSFER_STOP						(WM_APP + 49)		// added by moonknit 2005-06-16
#define TRANSFER_QUEUED						(WM_APP + 50)		//32818
#define TRANSFER_CONNECTING					(WM_APP + 51)
#define TRANSFER_CONNECTED					(WM_APP + 52)
#define TRANSFER_SENDING					(WM_APP + 53)
#define TRANSFER_RECEIVING					(WM_APP + 54)
#define TRANSFER_PUSH						(WM_APP + 55)
#define TRANSFER_CLOSED						(WM_APP + 56)
#define TRANSFER_COOLDOWN					(WM_APP + 57)
#define JOIN_CHUNK							(WM_APP + 58)
#define TRANSFER_DELETED					(WM_APP + 59)		// added by moonknit 2005-06-02


#define EVOLVE_CLOSE						(WM_APP + 60)	

#define CHAT_UPDATE							(WM_APP + 61)
#define CHAT_SELECT							(WM_APP + 62)
#define CHAT_USER							(WM_APP + 63)
#define CHAT_TOPIC							(WM_APP + 64)
#define CHAT_NOTICE							(WM_APP + 65)

#define TRACE_ERROR							(WM_APP + 66)

#define SEARCH_RESULT_TRYING				(WM_APP + 70)
#define SEARCH_RESULT_DOWNLOADING			(WM_APP + 71)
#define SEARCH_RESULT_COMPLETED				(WM_APP + 72)
#define SEARCH_RESULT_NOHOSTS				(WM_APP + 73)
#define SEARCH_RESULT_CANCELED				(WM_APP + 74)

#define CONNECT_INFO						(WM_APP + 80)
#define REQUEST_MEDIA_SERVER				(WM_APP + 81)
#define MEDIA_PLAY							(WM_APP + 82)
#define MEDIA_PAUSE							(WM_APP + 83)
#define MEDIA_STOP_CLOSE					(WM_APP + 84)

#define ADD_DOWNLOAD						(WM_APP + 85)
#define ADD_UPLOAD							(WM_APP + 86)

#define	COMPLETE_DOWNLOAD					(WM_APP + 90)

#define WM_MERGE_COMPLETE					(WM_APP + 100)
#define WM_ADD_MESSAGE						(WM_APP + 101)

#define WM_NODEUPDATE						(WM_APP + 102)
//#define WM_UPLOADUPDATE						(WM_APP + 103)
#define WM_UPDATEPOINT						(WM_APP + 104)
#define WM_CALCPOINTS						(WM_APP + 105)
#define WM_AUTOACTIONUPDATE					(WM_APP + 106)
#define WM_TRAYICON							(WM_APP + 107)
#define WM_TRAYSTATUS						(WM_APP + 108)

#define WM_EXTFOCUSED						(WM_APP + 110)
//#define WM_EXTLOGIN							(WM_APP + 111)

#define WM_SYSCLOSE							(WM_APP + 120)

#define WM_CHANGETRVIEW						(WM_APP + 130)

#define URN_TITLE							_T("urn:t:")
#define URN_LINK							_T("urn:l:")

#define	URN_SHA1							_T("urn:sha1:")			// 변경 불가
#define	URN_META							_T("urn:meta:")
#define	URN_SIZE							_T("urn:size:")
#define URN_PATH							_T("urn:path:")
#define URN_DATE							_T("urn:date:")
// VERSION INFORMATION
#define URN_VERSION							_T("urn:ver:")

// -- from ezdefine
#define EZ_SEND_OTHER_NODE_INFO				_T("SEND_OTHER_NODE_INFO\r\n")
#define EZ_REQ_OTHER_NODE_INFO				_T("REQ_OTHER_NODE_INFO\r\n")
#define EZ_NEWLINE							_T("\r\n")
#define EZ_ADD_NEW_NODE						_T("ADD_NEW_NODE\r\n")
#define EZ_NODE_FULL						_T("NODE_FULL\r\n")

// ?? MESSAGE
#define STR_MSG_FILSRCHVIEW_004				_T("이미 같은 이름의 파일을 다운로드 중입니다. 그 다운로드에 추가하시겠습니까?")
#define STR_MSG_FILSRCHVIEW_005				_T("다운로드 추가")

// GNU DONWLOAD SHELL MESSAGE
#define STR_MSG_GNUDWNSHL_002				_T("%s 외 %d 명")

#define GNUCLEUS_VERSION					_T("3.4.4.0")

#define FISH_VERSION_WORD					MAKEWORD(1, 0)		// 현재 클라이언트 버전 (0.1)

#define VERSION_4_CONNECT					_T("GNUTELLA CONNECT/0.4\n\n")
#define VERSION_4_CONNECT_OK				_T("GNUTELLA OK\n\n")

#define GNUMODE_VERSION6 

#ifdef GNUMODE_VERSION6
#define VERSION6_ISTRUE						true
#else
#define VERSION6_ISTRUE						false
#endif

#define NODE_CONNECTIVITY

#define UDP_PORT							5467

#define	MAX_FILETIME						20

#define	MAX_SHARECOUNT						100

#define	MAX_AUTOREPLY						100

#define	MAX_GNUNODE							7

#define MAX_CLUB_NUM						64

// packet buffer
#define	MAX_USERID							20

#define TABLE_BITS							16
#define TABLE_INFINITY						2

#define ALLOW								1
#define DENY								0

#define NT_INBOUND							1
#define NT_OUTBOUND							2
#define NT_ALL								NT_INBOUND | NT_OUTBOUND


enum GNUDATA_TYPE
{
	GDT_UNKNOWN = 0,
	GDT_POST,
	GDT_GRAPH,
	GDT_FILE
};

#define GDT_POST_S							_T("post")
#define GDT_FILE_S							_T("file")
#define GDT_GRAPH_S							_T("graph")
// -- STRUCTURES ------------------------
struct SE_USERINFO
{
	CString		suid;
	CString		shostip;
	int			ngnuport;
	int			nextport;

	WORD		wVersion;

	SE_USERINFO()
		: suid(_T("")), shostip(_T("")), ngnuport(0), nextport(0), wVersion(0)
	{
	};
};

struct SE_DATAINFO
{
	int			type;				// 0 : UNKNOWN, 1 : POST, 2 : GRAPH, 3 : FILE
	// FISH FLAG DATA
	BOOL		bread;
	BOOL		breadon;
	BOOL		bscrap;
	BOOL		bfilter;
	
	CString		sguid;
	CString		name;
	CString		link;
	CString		name_ex;			// written by une 2005-06-15
	INT64		nsize;
	int			nid;				// written by moonknit 2005-05-17
	time_t		pubdate;
	CString		date;
	CString		data_ex;

	SE_DATAINFO ()
		: sguid(_T("")), name(_T("")), name_ex(_T("")), nsize(0), nid(0), date(_T("")), data_ex(_T(""))
		, bread(FALSE), breadon(FALSE), bscrap(FALSE), bfilter(FALSE), link(_T(""))
	{
	}
};

struct SE_QUERY
{
	CString		userid;

	int			nHops;
	GUID		QueryGuid;
	CGnuNode*	Origin;					// CGnuNode Point to return the QueryHit

	SE_QUERY()
		: nHops(0), Origin(NULL)
	{
	}
};

struct SE_ITEM
{
	SE_USERINFO		ui;
	SE_DATAINFO		di;

	GUID			QueryID;

	int				Speed;

	bool			Firewall;
	bool			Busy;
	bool			Stable;
	bool			ActualSpeed;

	// For Push
	CGnuNode*		Origin;
	GUID			PushID;
	int				Distance;
	// -- 

	CString			NameLower;			// 검색결과 파일의 Lowered String 값
	CString			sExInfo;			// 검색결과의 추가정보의 RAW String 값

	SE_ITEM()
		: Speed(0), Firewall(false), Busy(false), Stable(false), ActualSpeed(false)
		, Origin(NULL), Distance(0), NameLower(_T("")), sExInfo(_T(""))
	{
	}
};

struct DL_SOURCE
{
	CString		shostip;
	CString		suid;
	int			ngnuport;
	int			nid;
	GUID		PushID;

	// written by moonknit 2005-08-11
	// VERSION INFORMATION
	WORD		wVersion;

	DL_SOURCE()
		: shostip(_T("127.0.0.1")), suid(_T("")), ngnuport(0), nid(0), wVersion(0)
	{
	}
};

struct DL_DATAINFO
{
	int			type;			// 0 : UNKNOWN, 1 : POST, 2 : GRAPH, 3 : FILE
	CString		sguid;
	CString		name;
	DWORD64		nsize;
	
	// add by happyune
	int			nid;		// file Serial number
	CString		suid;		// 검색 결과 아이디

	std::list<DL_SOURCE> listSource;

	DL_DATAINFO()
		: sguid(_T("")), name(_T("")), nsize(0), suid(_T("")), nid(0)
	{
		listSource.clear();
	}
};

// -- Gnucleus STRUCTURE -------------------------

enum		// 버디 그룹 타입..
{
	TYPE_MY = 0,		// 내그룹
	TYPE_IM				// 내가 속한 그룹
};


struct GNUSeed
{
	TCHAR	m_szSeedIP[MAX_IP+1];
	int		m_iSeedPort;
	GNUSeed()
	{
		memset( m_szSeedIP, 0x00, sizeof(m_szSeedIP) );
		m_iSeedPort=0;
	}

	void set( TCHAR* ip, int port )
	{
		int len = _tcslen(ip);
		if(len >= sizeof(m_szSeedIP)) return;

		memcpy( &m_szSeedIP, ip, len );
		m_szSeedIP[len] = _T('\0');
		m_iSeedPort = port;
	}
};

#define MAX_NODEMAP		200
struct GNUSeed2
{
	TCHAR	m_szSeedIP[MAX_IP+1];
	int		m_iSeedPort;
	int		m_nHops;
	GNUSeed2()
	{
		memset( m_szSeedIP, 0x00, sizeof(m_szSeedIP) );
		m_iSeedPort=0;
		m_nHops = 0;
	}

	void set( TCHAR* ip, int port, int hops=0 )
	{
		int len = _tcslen(ip);
		if(len >= sizeof(m_szSeedIP)) return;

		_tcscpy( m_szSeedIP, ip);
		m_szSeedIP[len] = _T('\0');
		m_iSeedPort = port;
		m_nHops = hops;
	}
};

struct USERID
{
	TCHAR	m_szID[MAX_USERID+1];
	USERID()
	{
		memset( &m_szID, 0x00, sizeof(m_szID) );
	}
	
	USERID( TCHAR* id )
	{
		memset( &m_szID, 0x00, sizeof(m_szID) );
		memcpy( m_szID, id, sizeof(m_szID) );
	}
	bool isEqualID( TCHAR* id )
	{
		if( _tcsicmp( m_szID, id ) )
			return true;
		else
			return false;
	}
};

CString GetTRStatusString(int nStatus);

#endif // _GUNDEFINES_H