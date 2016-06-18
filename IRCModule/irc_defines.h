#ifndef _IRC_DEFINES_H
#define _IRC_DEFINES_H

#include "../XML/EzXMLParser.h"

#define CR									13
#define LF									10
#define FF									12

#define DELIMITER							FF
#define EOS									'\0'

#define	ENCRYPT_KEY							0x1A

#define	FISH_TITLE							_T("FISH1.0")


#define WM_NET_IRCD_MSG						WM_USER+600
#define WM_NET_PROCESS						WM_USER+601

#define	MAX_CHANNEL_COUNT					100
#define	MAX_HOSTIP_COUNT					10

#define	MAX_IP								128
#define	MAX_PORT							5
#define	MAX_URL								100
#define	MAX_EVENTD							20
#define	MAX_MD5								32
#define	MAX_VERSION							10
#define	MAX_SHARENAME						50
#define	MAX_SHAREID							6
#define	MAX_SEARCHID						8

#define	MAX_GROUPKEY						10
#define	MAX_GROUPNAME						50
#define	MAX_GROUPCOUNT						50

#define	MAX_USERID							20
#define	MAX_EVENTD							20
#define	MAX_CHANNEL_COUNT					100
#define	MAX_HOSTIP_COUNT					10

// TIMER
#define TIMER_NETWORK_ERROR					5
#define TIME_NETWORK_ERROR					1000

#define TIMER_EVENTD_ERROR					6
#define TIME_EVENTD_ERROR					1000

// state define
#define	INVALID_VALUE						-1
#define	INVALID_INDEX						INVALID_VALUE
#define	INVALID_STATUS						INVALID_VALUE

// ircd error
#define ERR_MSG_2		"INSTANTPASSWORD NOT MATCHED!!!"				// IRCD에 인증 요청(AR)시 or 웹 서버에 정보 요청시..에 비밀번호가 달랐을 경우에 나오는 메시지 ..  
#define	ERR_MSG_3		"NO SUCH USER ACCOUNT!!!"						// 사용자를 찾지 못한 경우..
#define ERR_MSG_4		"UNKNOWN ERROR!!!"								// 알수없는 에러..
#define ERR_MSG_5		"UID'S GROUP IS NOT EXIST"						// 사용자의 친구 그룹이 존재 하지 않음..
#define ERR_MSG_6		"<!DOCTYPE HTML PUBLIC"							// 웹페이지를 열지 못하거나, 잘못된경로일 경우..
#define ERR_MSG_7		"UID IS NOT EXIST!!"							// UID param이 비어서 넘어갔음.
#define ERR_MSG_8		"KEY IS NOT EXIST!!"							// instant password 가 비어있음
#define ERR_MSG_9		"UID IS OFFLINE NOW!!"							// 로그인 한 회원 아이디가 아님
#define ERR_MSG_10		"MSGTYPE IS NOT EXIST!!"						// msgtype 값이 없음
#define ERR_MSG_11		"SERIAL IS NOT EXIST!!"							// serial 값이 없음
#define ERR_MSG_12		"TOID IS NOT EXIST!!"							// toid 값이 없음
#define ERR_MSG_13		"AVT IS NOT EXIST!!"							// avt 값이 없음
#define ERR_MSG_14		"MSG IS NOT EXIST!!"							// msg 값이 없음
#define ERR_MSG_15		"GROUPKEY IS NOT EXIST!!"						// groupkey 가 입력되지 않음
#define ERR_MSG_16		"GNAME IS NOT EXIST!!"							// gname 이 입력되지 않음
#define ERR_MSG_17		"GROUP IS ALREADY EXIST!!"						// groupkey와 uid에 해당하는 그룹이 이미 존재함
#define ERR_MSG_18		"CREATE GROUP IS FAILED!!"						// 그룹 생성에 실패함
#define ERR_MSG_19		"USERID IS NOT EXIST!!"							// buddy id 가 입력되지 않음
#define ERR_MSG_20		"NOT A NORMALLY USER!!"							// 정상 회원이 아님
#define ERR_MSG_21		"OCCUR ERROR IN ADD BUDDY!!"					// 친구 추가시 에러 발생
#define ERR_MSG_22		"WAITING REGIST!!"								// 등록 대기중인 친구입니다
#define ERR_MSG_23		"ALREADY REGISTERED!!"							// 이미 등록된 친구입니다
#define ERR_MSG_24		"REFUSED USER!!"								// 거부되었습니다.
#define ERR_MSG_25		"TYPE IS NOT EXIST!!"							// type이 입력되지 않음
#define ERR_MSG_26		"MODE IS NOT EXIST!!"							// mode 가 입력되지 않음
#define ERR_MSG_27		"TYPE IS INCORRECT!!"							// type 이 'MY' 나 'IM' 이 아님
#define ERR_MSG_28		"CID IS NOT EXIST!!"							// cid 값이 없음
#define	ERR_MSG_29		"NO CLUB CONNECTED ENPY!!"						// 클럽에 공유자 없음.
#define ERR_MSG_30		"SVRID IS NOT EXIST!!"							// svrid 가 없음
#define ERR_MSG_31		"SCORE IS NOT EXIST!!"							// score 가 없음
#define ERR_MSG_32		"FACE IS NOT EXIST!!"							// face 가 없음	
#define ERR_MSG_33		"USER'S REMARK IS NOT EXIST!!"					// 주인장 한마디 없음..
#define ERR_MSG_34		"USERID IS NOT EXIST!!"							// userid 값이 없음


#endif