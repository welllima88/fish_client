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

#define IN_CLIENT_INFO		    0x05			// 초대장으로 실행
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
#define NO_SHARE_LIST_IN_CLUB		0x2B	// 해당클럽 공유 없음 ( 순간적으로공유 목록을 삭제한 경우)
#define EXCESS_CONNECTION			0x2C	// 접속자수 초과
#define REJECT_USER					0x2D	// 접속 거부자
#define INVALID_SERVERKEY			0x2E	// 초청장의 정보가 잘못(조작)된 경우..


//-- Notification -------------------
#define NOTI_SHAREINFO_EMPTY		0x30	// 공유 목록이 없음.
#define NOTI_SHAREID_EMPTY			0x31	// share id 존재 하지 않음 
#define NOTI_FORCE_EXIT				0x32	// 서버쪽에서 폴더 탐색중일때 강제 종료를 하였을 경우
#define NOTI_FORCE_EXIT_AT_EXIT		0x36	// 서버쪽이 프로그램 종료로 인한 강제 종료
// Date : 2004-06-29,	By bemlove
#define NOTI_ONLY_BUDDY				0x33	// 폴더 탐색이 등록 친구로만 제한된경우.. (자신 포함 안됨)
#define NOTI_ONLY_SPECIFY_ALLOW		0x34	// 폴더 탐색이 특정 아이디만 허용 (자신 포함 안됨)
#define NOTI_ONLY_SPECIFY_REFUSE	0x35	// 폴더 탐색이 특정 아이디만 거부(자신 포함됨)

//-- Server Already Run -------------
#define SERVER_ALREADY_RUN			0x40	// 서버가 실행중 공유 요청시..

//---Download Error -----------------
#define EXCESS_DOWNLOAD				0x50	// 서버측의 다운로드 수 초과

// end of bemlove..

// ....coded by bemlove

// 최초 connection 완료 후 사용자 인증 과정
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

//	int		couponType;				//프리미엄 쿠폰 종류

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

struct INClientInfo		//INvite 용 인증 데이터..
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
// 공유 정보 요청...관련
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
	char	sid[MAX_SHAREID+1];			// GUID ( 동일한 공유명을 생성 가능하도록 설정 )
	char	sname[MAX_SHARENAME+1];		// 공유 이름
	char	level;						// 허용레벨

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
struct Req_ShareInfoDir		// 공유정보에서 디렉토리 리스트 요청
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



struct Req_ShareInfoFile // 공유정보에서 디렉토리내 파일 리스트 요청
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


struct Req_ShareInfoFileIndex		// 다운 로드 요청 자료에 대한  파일 인덱스 요청..
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