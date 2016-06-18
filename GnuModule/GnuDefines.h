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

// FISH ���� 3.0 ���� ���� �ٿ�ε尡 �����Ǹ� ����Ѵ�.
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
// ���� ���� ��Ʈ��
////////////////////////////////////////////////////////////////////
#define STR_TRANSFER_UNKNOWN				_T("�����")
#define STR_TRANSFER_CANCEL					_T("�������")
// download state
#define STR_TRANSFER_NEEDSOURCE				_T("������ ����")
#define STR_TRANSFER_STOP					_T("�Ͻ�����")
#define STR_TRANSFER_QUEUED					_T("�����")
#define STR_TRANSFER_CONNECTING				_T("������")
//#define STR_TRANSFER_CONNECTING				STR_TRANSFER_QUEUED
#define STR_TRANSFER_RECEIVING				_T("�޴���")
#define STR_TRANSFER_CLOSED					_T("�Ϸ�")
#define STR_TRANSFER_COOLDOWN				STR_TRANSFER_QUEUED
#define STR_JOIN_CHUNK						_T("��ġ����")
#define STR_TRANSFER_DELETED				_T("������")
// upload state
#define STR_TRANSFER_CONNECTED				STR_TRANSFER_CONNECTING		
#define STR_TRANSFER_SENDING				_T("��������")
#define STR_TRANSFER_PUSH					STR_TRANSFER_SENDING

////////////////////////////////////////////////////////////////////
// from IrcdManager.cpp
////////////////////////////////////////////////////////////////////

#define STR_MSG_IRCDMANGER_001				_T("�ٸ������� ���� ���̵�� �α��� �Ͽ����ϴ�.\n���� ������� ��û�� ���� ���α׷��� �����մϴ�.")
#define STR_MSG_IRCDMANGER_002				_T("ENPPY ����")
#define STR_MSG_IRCDMANGER_003				_T("���� ��Ʈ�� ���ῡ ������ �ֽ��ϴ�. ��Ʈ�� ���¸� Ȯ���� �ֽñ� �ٶ��ϴ�.")
#define STR_MSG_IRCDMANGER_004				_T("�������� ��Ʈ�� ������ ������ϴ�.\n��Ʈ���� ������¸� Ȯ���Ͻ� �Ŀ� ���Ǹ� ������� �ֽñ� �ٶ��ϴ�.")
#define STR_MSG_IRCDMANGER_005				_T("ENPPY-��Ʈ�� ����")
#define STR_MSG_IRCDMANGER_006				_T("��й�ȣ�� Ʋ���ϴ�. �ٽ� �� �� Ȯ���� ���ñ� �ٶ��ϴ�.")
#define STR_MSG_IRCDMANGER_007				_T("��������")
#define STR_MSG_IRCDMANGER_008				_T("�� �� ���� ����� ���� �Դϴ�.")
#define STR_MSG_IRCDMANGER_009				_T("���Ǹ� �����ϰڽ��ϴ�.")
#define STR_MSG_IRCDMANGER_010				_T("�ٸ������� �� ID�� �������Դϴ�.\n�ٸ����� ������ �����ðڽ��ϱ�?")
#define STR_MSG_IRCDMANGER_011				_T("������ ä���� �������� �ʾ� ������ �� �����ϴ�.\n���Ǹ� �����ϰڽ��ϴ�.")
#define STR_MSG_IRCDMANGER_012				_T("���ͳ� �����⳪ �缳 IP�� ����ϰ� �ֱ� ������\nENPPY���� ���� ����� ���� ���� �� �����ϴ�.\n�޽��� �� �ٸ� ENPPY �������� ���� �ٿ�ε常 �����մϴ�.\n(��, ������ ������ ��Ʈ�� ȯ�� ������ ��ģ ��쿡��\n[ȯ�漳��]->[���ǿɼ�]->[��Ʈ��ũ] ���� \n'�������� ����ȯ�� ����'�� �����Ͻð�\nENPPY�� ����� �Ͻø� �ٷ� ���������մϴ�.")
#define STR_MSG_IRCDMANGER_013				_T(" %s�� ���Ŀ� '%s'�� ���Ͽ� ENPPY�� ����Ͻ� �� �����ϴ�.")
#define STR_MSG_IRCDMANGER_014				_T("ENPPY �ý��� ����")
#define STR_MSG_IRCDMANGER_015				_T("ENPPY �ȳ�")
#define STR_MSG_IRCDMANGER_016				_T("��Ʈ�� ������ ���� ENPPY�� �ڵ� �����˴ϴ�.")	//new
#define STR_MSG_IRCDMANGER_017				_T("���� Enppy ����Ʈ ������ ���� ������ �������� �ʾ� �����Ͻ� �� �����ϴ�.\n��� �� �ٽ� ������ �ֽñ� �ٶ��ϴ�.") //new
#define STR_MSG_IRCDMANGER_018				_T("ENPPY-���� ����") //new
#define STR_MSG_IRCDMANGER_019				_T("ENPPY ����Ʈ ���� ���� ������ ���� �ٽ� ���� ���Դϴ�.") //new
#define STR_MSG_IRCDMANGER_020				_T("���� ENPPY ����Ʈ ���� ������ ������ ���� ���� ����� �Դϴ�.") //new
#define STR_MSG_IRCDMANGER_021				_T("�� ������ ������ �����ϴ� �������� ������ �߻��Ͽ����ϴ�.\n���Ǹ� �����ϰڽ��ϴ�.\n��� �� �ٽ� ���� �ٶ��ϴ�.")//new
#define STR_MSG_IRCDMANGER_022				_T("����� ������ �����ϴ� �������� ������ �߻��Ͽ����ϴ�.\n���Ǹ� �����ϰڽ��ϴ�.\n��� �� �ٽ� ���� �ٶ��ϴ�.")//new


// -- socket error ----------------------------------------------
#define STR_MSG_COMMON_001					_T("���� �ٸ� ���α׷����� ����ϴ� ��Ʈ�� �浹�� �߻��Ͽ����ϴ�.\n��Ʈ��ȣ�� ���� �Ͻðڽ��ϱ�?")

#define STR_MSG_SOCKERROR_001				_T("������ �ּҰ� �̹� ������Դϴ�")
#define STR_MSG_SOCKERROR_002				_T("������ �ּҴ� ����� �Ұ��մϴ�")
#define STR_MSG_SOCKERROR_003				_T("������ �йи� �ּҴ� �� ���ϰ� ����� �� �����ϴ�")
#define STR_MSG_SOCKERROR_004				_T("���� ������ ����Ǿ� ������ �Ͻ� �� �����ϴ�")
#define STR_MSG_SOCKERROR_005				_T("������ �ּҰ� �ʿ��մϴ�")
#define STR_MSG_SOCKERROR_006				_T("lpSocketAddrLen ���ڰ� �߸��Ǿ����ϴ�")
#define STR_MSG_SOCKERROR_007				_T("�� ������ �̹� �ٸ� �ּҿ� ���ε��Ǿ� �ֽ��ϴ�")
#define STR_MSG_SOCKERROR_008				_T("�� ������ �̹� ����Ǿ� �ֽ��ϴ�")
#define STR_MSG_SOCKERROR_009				_T("��밡���� ���� ����ڰ� �����ϴ�")
#define STR_MSG_SOCKERROR_010				_T("�������� ������ �� �����ϴ�")
#define STR_MSG_SOCKERROR_011				_T("��밡���� ���۰����� �����ϴ�")
#define STR_MSG_SOCKERROR_012				_T("�� ������ ����� ������ �ƴմϴ�")
#define STR_MSG_SOCKERROR_013				_T("�� ����ڴ� ������ �ƴ϶� �����Դϴ�")
#define STR_MSG_SOCKERROR_014				_T("����ð��� �ʰ��Ǿ����ϴ�")
#define STR_MSG_SOCKERROR_015				_T("��Ӹ��� ������ �� �����ϴ�.")
#define STR_MSG_SOCKERROR_016				_T("KeepAlive���� ������ �� �����ϴ�.")
#define STR_MSG_SOCKERROR_017				_T("��Ŷ �о�� �޸� ����")
#define STR_MSG_SOCKERROR_018				_T("��Ŷ ������ �޸� ����")

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

#define	URN_SHA1							_T("urn:sha1:")			// ���� �Ұ�
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
#define STR_MSG_FILSRCHVIEW_004				_T("�̹� ���� �̸��� ������ �ٿ�ε� ���Դϴ�. �� �ٿ�ε忡 �߰��Ͻðڽ��ϱ�?")
#define STR_MSG_FILSRCHVIEW_005				_T("�ٿ�ε� �߰�")

// GNU DONWLOAD SHELL MESSAGE
#define STR_MSG_GNUDWNSHL_002				_T("%s �� %d ��")

#define GNUCLEUS_VERSION					_T("3.4.4.0")

#define FISH_VERSION_WORD					MAKEWORD(1, 0)		// ���� Ŭ���̾�Ʈ ���� (0.1)

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

	CString			NameLower;			// �˻���� ������ Lowered String ��
	CString			sExInfo;			// �˻������ �߰������� RAW String ��

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
	CString		suid;		// �˻� ��� ���̵�

	std::list<DL_SOURCE> listSource;

	DL_DATAINFO()
		: sguid(_T("")), name(_T("")), nsize(0), suid(_T("")), nid(0)
	{
		listSource.clear();
	}
};

// -- Gnucleus STRUCTURE -------------------------

enum		// ���� �׷� Ÿ��..
{
	TYPE_MY = 0,		// ���׷�
	TYPE_IM				// ���� ���� �׷�
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