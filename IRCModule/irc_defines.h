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
#define ERR_MSG_2		"INSTANTPASSWORD NOT MATCHED!!!"				// IRCD�� ���� ��û(AR)�� or �� ������ ���� ��û��..�� ��й�ȣ�� �޶��� ��쿡 ������ �޽��� ..  
#define	ERR_MSG_3		"NO SUCH USER ACCOUNT!!!"						// ����ڸ� ã�� ���� ���..
#define ERR_MSG_4		"UNKNOWN ERROR!!!"								// �˼����� ����..
#define ERR_MSG_5		"UID'S GROUP IS NOT EXIST"						// ������� ģ�� �׷��� ���� ���� ����..
#define ERR_MSG_6		"<!DOCTYPE HTML PUBLIC"							// ���������� ���� ���ϰų�, �߸��Ȱ���� ���..
#define ERR_MSG_7		"UID IS NOT EXIST!!"							// UID param�� �� �Ѿ��.
#define ERR_MSG_8		"KEY IS NOT EXIST!!"							// instant password �� �������
#define ERR_MSG_9		"UID IS OFFLINE NOW!!"							// �α��� �� ȸ�� ���̵� �ƴ�
#define ERR_MSG_10		"MSGTYPE IS NOT EXIST!!"						// msgtype ���� ����
#define ERR_MSG_11		"SERIAL IS NOT EXIST!!"							// serial ���� ����
#define ERR_MSG_12		"TOID IS NOT EXIST!!"							// toid ���� ����
#define ERR_MSG_13		"AVT IS NOT EXIST!!"							// avt ���� ����
#define ERR_MSG_14		"MSG IS NOT EXIST!!"							// msg ���� ����
#define ERR_MSG_15		"GROUPKEY IS NOT EXIST!!"						// groupkey �� �Էµ��� ����
#define ERR_MSG_16		"GNAME IS NOT EXIST!!"							// gname �� �Էµ��� ����
#define ERR_MSG_17		"GROUP IS ALREADY EXIST!!"						// groupkey�� uid�� �ش��ϴ� �׷��� �̹� ������
#define ERR_MSG_18		"CREATE GROUP IS FAILED!!"						// �׷� ������ ������
#define ERR_MSG_19		"USERID IS NOT EXIST!!"							// buddy id �� �Էµ��� ����
#define ERR_MSG_20		"NOT A NORMALLY USER!!"							// ���� ȸ���� �ƴ�
#define ERR_MSG_21		"OCCUR ERROR IN ADD BUDDY!!"					// ģ�� �߰��� ���� �߻�
#define ERR_MSG_22		"WAITING REGIST!!"								// ��� ������� ģ���Դϴ�
#define ERR_MSG_23		"ALREADY REGISTERED!!"							// �̹� ��ϵ� ģ���Դϴ�
#define ERR_MSG_24		"REFUSED USER!!"								// �źεǾ����ϴ�.
#define ERR_MSG_25		"TYPE IS NOT EXIST!!"							// type�� �Էµ��� ����
#define ERR_MSG_26		"MODE IS NOT EXIST!!"							// mode �� �Էµ��� ����
#define ERR_MSG_27		"TYPE IS INCORRECT!!"							// type �� 'MY' �� 'IM' �� �ƴ�
#define ERR_MSG_28		"CID IS NOT EXIST!!"							// cid ���� ����
#define	ERR_MSG_29		"NO CLUB CONNECTED ENPY!!"						// Ŭ���� ������ ����.
#define ERR_MSG_30		"SVRID IS NOT EXIST!!"							// svrid �� ����
#define ERR_MSG_31		"SCORE IS NOT EXIST!!"							// score �� ����
#define ERR_MSG_32		"FACE IS NOT EXIST!!"							// face �� ����	
#define ERR_MSG_33		"USER'S REMARK IS NOT EXIST!!"					// ������ �Ѹ��� ����..
#define ERR_MSG_34		"USERID IS NOT EXIST!!"							// userid ���� ����


#endif