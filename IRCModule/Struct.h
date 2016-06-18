#ifndef __STRUCT_H__
#define	__STRUCT_H__

//#include "..\define.h"

//-- Thread Message --------------------
#define TH_END_THREAD			0x91
#define TH_CLEAR_PDATA			0x92
#define CONNECTION_DISCONNECTED 0x93

//-- Head ------------------------------	// by bemlove
#define CLIENT_INFO		        0x01
#define	SHARE_INFO			    0x02
#define NOTI_INFO			    0x03
#define SERVER2SERVER_CON	    0x04

#define IN_CLIENT_INFO		    0x05			// �ʴ������� ����
#define	IN_SHARE_INFO		    0x06
#define	IN_NOTI_INFO		    0x07

//-- ClientInfo List -------------------
#define CLIENT_INFO_INFO	    0x11
#define CLIENT_INFO_OK		    0x12

#define SEND_PING			    0x13
#define SEND_PONG			    0x14

//-- ShareInfo  ---------------------
#define SHARE_INFO_REQ				0x20
#define	SHARE_INFO_OK				0x21
#define	SHARE_INFO_END				0x22
#define SHARE_INFO_DIR				0x23
#define	SHARE_INFO_DIR_OK			0x24
#define	SHARE_INFO_DIR_END			0x25
#define SHARE_INFO_FILE				0x26
#define	SHARE_INFO_FILE_OK			0x27
#define	SHARE_INFO_FILE_END			0x28
#define	SHARE_INFO_FILE_INDEX		0x29
#define	SHARE_INFO_FILE_INDEX_OK	0x2A

//-- Connection error------------====
#define NO_SHARE_LIST_IN_CLUB		0x2B	// �ش�Ŭ�� ���� ���� ( ���������ΰ��� ����� ������ ���)
#define EXCESS_CONNECTION			0x2C	// �����ڼ� �ʰ�
#define REJECT_USER					0x2D	// ���� �ź���
#define INVALID_SERVERKEY			0x2E	// ��û���� ������ �߸�(����)�� ���..


//-- Notification -------------------
#define NOTI_SHAREINFO_EMPTY		0x30	// ���� ����� ����.
#define NOTI_SHAREID_EMPTY			0x31	// share id ���� ���� ���� 
#define NOTI_FORCE_EXIT				0x32	// �����ʿ��� ���� Ž�����϶� ���� ���Ḧ �Ͽ��� ���
#define NOTI_FORCE_EXIT_AT_EXIT		0x36	// �������� ���α׷� ����� ���� ���� ����
// Date : 2004-06-29,	By bemlove
#define NOTI_ONLY_BUDDY				0x33	// ���� Ž���� ��� ģ���θ� ���ѵȰ��.. (�ڽ� ���� �ȵ�)
#define NOTI_ONLY_SPECIFY_ALLOW		0x34	// ���� Ž���� Ư�� ���̵� ��� (�ڽ� ���� �ȵ�)
#define NOTI_ONLY_SPECIFY_REFUSE	0x35	// ���� Ž���� Ư�� ���̵� �ź�(�ڽ� ���Ե�)

//-- Server Already Run -------------
#define SERVER_ALREADY_RUN			0x40	// ������ ������ ���� ��û��..

//---Download Error -----------------
#define EXCESS_DOWNLOAD				0x50	// �������� �ٿ�ε� �� �ʰ�

// end of bemlove..

// ....coded by bemlove

// ���� connection �Ϸ� �� ����� ���� ����
struct ClientInfo
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	clubID[MAX_CLUBID+1];		
	char	clubName[MAX_CLUBNAME+1];	
	char	userID[MAX_USERID+1];
	BYTE	level;	

	// PREMIUM USER
	UINT	nPremiumLevel;

//	int		couponType;				//�����̾� ���� ����

	ClientInfo()
	{
		size		=   0;
		type		=   0;
		subType		=   0;
		memset( clubID, 0x00, sizeof(clubID) );
		memset( clubName, 0x00, sizeof(clubName) );
		memset( userID, 0x00, sizeof(userID) );
		level = 0;
//		couponType = 0;

		// PREMIUM USER
		nPremiumLevel = 0;
	};
	
};

struct INClientInfo		//INvite �� ���� ������..
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	DWORD	serverKey;
	DWORD	timeStamp;
	char	userID[MAX_USERID + 1];	
	bool	isEmail;
	// PREMIUM USER
	UINT	nPremiumLevel;
	
	INClientInfo()
	{
		size		= 0;
		type		= 0;
		subType		= 0;
		serverKey	= 0;
		timeStamp	= 0;
		memset( userID, 0x00, sizeof(userID) );
		isEmail		= false;

		// PREMIUM USER
		nPremiumLevel = 0;
	};
};
struct ClientInfoOK
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	ClientInfoOK()
	{
		size=0;
		type = 0;
		subType = 0;
	};
};

struct INClientInfoOK
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	serverID[MAX_USERID+1];
	INClientInfoOK()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( serverID, 0x00, sizeof(serverID) );
	};
};

struct CommonMsg	
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	CommonMsg()
	{
		size = 0;
		type = 0;
		subType = 0;
	};
};
// ���� ���� ��û...����
struct Req_ShareInfo
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	clubID[MAX_CLUBID+1];
	Req_ShareInfo()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( clubID, 0x00, sizeof( clubID ) );
	};
};

struct ShareInfoData
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];			// GUID ( ������ �������� ���� �����ϵ��� ���� )
	char	sname[MAX_SHARENAME+1];		// ���� �̸�
	char	level;						// ��뷹��

	ShareInfoData()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( sname, 0x00, sizeof(sname) );
		level = '5';
	};
};

struct ShareInfoDataEnd
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	ShareInfoDataEnd()
	{
		size=0;
		type = 0;
		subType = 0;
	};
};
struct Req_ShareInfoDir		// ������������ ���丮 ����Ʈ ��û
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];
	char	sname[MAX_SHARENAME+1];
	Req_ShareInfoDir()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( sname, 0x00, sizeof(sname) );
	};

};

struct ShareInfoDirData
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];
	char	sname[MAX_SHARENAME+1];
	char	path[MAX_PATH+1];
	ShareInfoDirData()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( sname, 0x00, sizeof(sname) );
		memset( path, 0x00, sizeof(path) );
	};

};

struct ShareInfoDirDataEnd
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	ShareInfoDirDataEnd()
	{
		size=0;
		type = 0;
		subType = 0;
	};
};



struct Req_ShareInfoFile // ������������ ���丮�� ���� ����Ʈ ��û
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];
	char	sname[MAX_SHARENAME+1];
	char	path[MAX_PATH+1];
	Req_ShareInfoFile()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( sname, 0x00, sizeof(sname) );
		memset( path, 0x00, sizeof(path) );
	};

};
struct ShareInfoFileData
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];
	char	sname[MAX_SHARENAME+1];
	char	path[MAX_PATH+1];
	char	fileName[MAX_PATH+1];
	unsigned __int64 fileSize;
	char	fileTime[MAX_FILETIME+1];
	ShareInfoFileData()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( sname, 0x00, sizeof(sname) );
		memset( path , 0x00, sizeof(path) );
		memset( fileName, 0x00, sizeof(fileName) );
		memset( fileTime, 0x00, sizeof(fileTime) );
		fileSize = 0;
	};
};
struct ShareInfoFileDataEnd
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	ShareInfoFileDataEnd()
	{
		size=0;
		type = 0;
		subType = 0;
	};
};


struct Req_ShareInfoFileIndex		// �ٿ� �ε� ��û �ڷῡ ����  ���� �ε��� ��û..
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	sid[MAX_SHAREID+1];
	char	filePath[MAX_PATH*2+1];
	char	cid[MAX_CLUBID+1];
	UINT	fileSize;
	bool	isMulti;
	Req_ShareInfoFileIndex()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( sid, 0x00, sizeof(sid) );
		memset( filePath, 0x00, sizeof(filePath) );
		fileSize = 0;
		isMulti = false;
	};
};

struct ShareInfoFileIndexData
{
	DWORD	size;
	BYTE	type;
	BYTE	subType;
	char	filePath[MAX_PATH*2+1];
	UINT	fileIndex;
	UINT	fileSize;
	char	id[MAX_USERID+1];
	char	cid[MAX_CLUBID+1];
	char	ip[16];
	int		port;
	bool	isMulti;
	ShareInfoFileIndexData()
	{
		size=0;
		type = 0;
		subType = 0;
		memset( filePath, 0x00, sizeof(filePath) );
		fileIndex = 0;
		fileSize = 0;
		memset( id, 0x00, sizeof(id) );
		memset( ip, 0x00, sizeof(ip));
		port = 0;
		isMulti  = false;
	};
};


//end of bemlove
#endif  //end of __STRUCT_H__