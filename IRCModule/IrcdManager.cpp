// IrcdManager.cpp : implementation file
//

#include "stdafx.h"
#include "IrcdManager.h"
#include "Irc_utils.h"
#include "../MainFrm.h"
#include "../GnuModule/GnuControl.h"
#include "../GnuModule/GnuDoc.h"
#include "../Fish_common.h"
#include "../LocalProperties.h"
#include "../FISH.h"
#include "../Dlg_Login.h"

#include "GetHttp.h"
/////////////////////////////////////////////////////////////////////////////
// CIrcdManager

#define	MAX_RETRYCONNECT				10			// �ִ� 10�� ��õ�
#define MAX_ALREADYTRYCNT				5
#define ITV_NSTAT						60			// 60 seconds
#define ITV_RECONNECT					60 * 5		// 5 minutes

#define STR_ERR_IM_SERVERCONNECT		_T("������ ������ �� �����ϴ�.")

extern CFISHApp theApp;

CIrcdManager::CIrcdManager(HWND hMsgParent)
: m_hMsgParent(hMsgParent)
{
	Initial();
}

CIrcdManager::~CIrcdManager()
{
	if( m_pcIrcdSock != NULL )
	{
		delete m_pcIrcdSock;
		m_pcIrcdSock = NULL;
	}
}

void CIrcdManager::ReleaseResource()
{
	if( m_pcIrcdSock != NULL )
	{
		delete m_pcIrcdSock;
		m_pcIrcdSock = NULL;
	}
}

bool CIrcdManager::Initial()
{
	m_hSearchView = NULL;

	memset( m_szIP, 0x00, sizeof(m_szIP) );
	m_iPort = 0;
	m_pcIrcdSock = NULL;
	m_iState = INVALID_STATUS;
	m_bAlreadyLogin = false;
	memset( m_szTempID, 0x00, sizeof(m_szTempID) );

	if( m_pcIrcdSock != NULL )
	{
		delete m_pcIrcdSock; 
		m_pcIrcdSock = NULL;
	}

	m_iLogicHandlerState	= 0;

	memset( m_szEventD, 0x00, sizeof( m_szEventD) );
	m_iChannelCount = 0;

	memset( m_szLocalIP, 0x00, sizeof(m_szLocalIP) );

	m_bClosedByOther = false;

	SetReconnecting( false );

	memset( m_szSeedIPManager, 0x00, sizeof(m_szSeedIPManager) );

	for(int i = 0; i < MAX_CHANNEL_COUNT; ++i)
	{
		m_szChannelList[i][0] = '\0';
	}

	// �翬�� �ʱ� �� ����...
	// 
	m_nRetryConnect = MAX_RETRYCONNECT;			// ������ ������ ��� ��õ� 
	m_nReconCnt = ITV_RECONNECT;		// ������ Ÿ�̸Ӱ� ���ƿ� �� �ٷ� ���� �õ�.

	m_bRandomID		= FALSE;

	m_nNSTATCnt		= 0;

	return true;
}//end of Initial()

bool CIrcdManager::IsValidSock()
{
	if( m_pcIrcdSock == NULL || ! m_pcIrcdSock->IsConnect() )
		return false;
	else
		return true;
}//end of ValidSock()


/*
** NAME    **: OnIrcdMsg()
** PARAM   **: wParam,lParam
** RETURN  **: long
** DESC    **: IRCD�� �޽��� ó�� �Լ�
** by bemlove at 2002-10-07, VER : 1.0, COMMENTS: ORG 
*/
LONG CIrcdManager::OnIrcdMsg( UINT wParam, LONG lParam )
{

	bool isNetworkError = false;
	switch( LOWORD(lParam) )
	{
		case FD_CONNECT:
			// Get Local host
			if(theApp.m_spGD->m_LocalHost.S_addr == StrtoIP(_T("127.0.0.1")).S_addr)
			{
				UINT dPort;
				CString LocalIP;
				if(m_pcIrcdSock->GetSockName(LocalIP, dPort))
				{
					theApp.m_spGD->SetHostIP((LPTSTR) (LPCTSTR) LocalIP);
				}
			}
			m_nRetryConnect = MAX_RETRYCONNECT;			// ������ ������ ��� ��õ� ȸ���� ���Ѵ�.
			OnNetProcess(WM_NET_IRCD_MSG,(LPARAM)0);
			m_nReconCnt = ITV_RECONNECT;		// ������ Ÿ�̸Ӱ� ���ƿ� �� �ٷ� ���� �õ�.
			break;
		case FD_CLOSE:
		case FD_OOB:
			if( m_bAlreadyLogin )
			{
				m_bAlreadyLogin = false;
				m_iLogicHandlerState = 0;
				break;
			}
			if(LOWORD(lParam)==FD_CLOSE)
			{
//				if( m_bClosedByOther )		// �ٸ� ������ ���� ���̵�� �����Ͽ��� ��� 
//				{
//					MessageBox(NULL, STR_MSG_IRCDMANGER_001, STR_MSG_IRCDMANGER_002, MB_OK|MB_ICONINFORMATION );
//				}
//				else
				{
					// case1.
					isNetworkError = true;
					
					theApp.m_spLP->SetIRCConnected(FALSE);

					// ���� �߿��� �� ������ �õ����� �ʴ´�.
					if(theApp.IsClosing()) break;

					// ������ ���������Ƿ� NETSTAT ������ �����Ѵ�.
					theApp.m_nnetpostcnt = 0;
					theApp.m_nnetusercnt = 0;

					::SendMessage(m_hSearchView, WM_NETCNTCHANGED, NULL, NULL);

					// ���� ��õ�
					m_nRetryConnect = MAX_RETRYCONNECT;			// ������ ������ ��� ��õ� ȸ���� ���Ѵ�.
					m_bKeepon = TRUE;
					m_nReconCnt = ITV_RECONNECT;		// ������ Ÿ�̸Ӱ� ���ƿ� �� �ٷ� ���� �õ�.
					break;
				}

//				theApp.m_spGD->m_Control.StopListening();
			}
			// �׿��� ���
			else
			{
				// case1, 2�� ��쿡�� ������� ��Ʈ�� ���°� ������ ���� ����� �� �����Ƿ� ��� ���(30��) ���¿� �ִٰ�
				// ��� ������ ping �׽�Ʈ�� ��ģ�� �۾��� �����ϵ��� �Ѵ�.
				isNetworkError = true;
				break;

			}		
			break;
		case FD_READ:
			OnIrcdReceive( (SOCKET)wParam, HIWORD(lParam) );
			break;
	}

	if( isNetworkError )
	{
		NetworkDisconnected();
		return 0L;
	}
	return 0L;
}//end of OnIrcdMsg()


/*
** NAME    **: OnIrcdReceive()
** PARAM   **: SOCKET, int length
** RETURN  **: void
** DESC    **: IRCD�������� ���� �޽��� Receive ó��
** by bemlove at 2002-10-08, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::OnIrcdReceive(SOCKET hSocket, int nLength)
{
	CString msg;

	if( !IsValidSock() ) return;
	
	char szBuffer[MAXBLOCK+1];
	int nRequire = nLength;
	int nRead;

	while(nRequire>0)
	{
		nRead = m_pcIrcdSock->ReadBlock(szBuffer,nRequire);
		if(nRead==0) break;
		*(szBuffer+nRead) = NULL;
		msg = (CString)szBuffer;
		OnNetProcess( WM_NET_IRCD_MSG, (LPARAM)szBuffer);
		nRequire -= nRead;
	}
	msg.Empty();
}//end of OnIrcdReceive()

static BOOL bSocketClosing = FALSE;

/*
** NAME    **: OnNetProcess()
** PARAM   **: wParam, lParam
** RETURN  **: long
** DESC    **: IRCD�� state�� ���� ���� ���� ó��
** by bemlove at 2002-10-07, VER : 1.0, COMMENTS: ORG 
*/
LONG CIrcdManager::OnNetProcess( UINT wParam, LONG lParam )
{
	switch( m_iState )
	{
		case STATE_CONNECT_IRCD:	// 5:IRCD �� Connect �õ�
			if( m_pcIrcdSock != NULL )
			{
				if( m_pcIrcdSock->IsConnect() ) // �̹� ����� ����
				{
					m_iState ++;
					return true;
				}

				if(bSocketClosing)
				{
					return false;
				}

				bSocketClosing = TRUE;

				m_pcIrcdSock->Close();

				if( !m_pcIrcdSock->Create() )
				{
					bSocketClosing = FALSE;
					m_iState = STATE_UNKNOWN_ERROR;
					// �� ��õ��� ������
					m_bKeepon = TRUE;
					m_nRetryConnect = MAX_RETRYCONNECT;
					m_nReconCnt = 0;
					return false;
				}
				bSocketClosing = FALSE;

			}
			else
			{
				m_pcIrcdSock = new CIrcdSock;
				if( m_pcIrcdSock == NULL )
					return false;
				m_pcIrcdSock->SetManager( this );
				if( !m_pcIrcdSock->Create() )
				{
					m_pcIrcdSock->Close();
					m_iState = STATE_UNKNOWN_ERROR;
					::SendMessage(m_hMsgParent, WM_IRCDAUTHCOMPLETE, ERR_I_SOCKCREATE, 0);

					m_bKeepon = TRUE;
					return false;
				}
			}
			if( !m_pcIrcdSock->Connect( m_szIP, m_iPort ) )
			{
				if( m_pcIrcdSock->IsError() )
				{
					m_iState = STATE_UNKNOWN_ERROR;
					OnNetProcess(0, 0);
					::SendMessage(m_hMsgParent, WM_IRCDAUTHCOMPLETE, ERR_I_SOCKCONNECT, 0);

					m_bKeepon = TRUE;
					return false;
				}
			}
			m_iState++;
			break;

		case STATE_INIT_IRCD:		// 6:Init IRCD Connect
			m_pcIrcdSock->Nick( theApp.m_spLP->GetIRCID() );
			m_pcIrcdSock->Passwd( );
			m_pcIrcdSock->User( theApp.m_spLP->GetIRCID() );
			m_iState++;	// STATE_PROCESS_MSG
			m_bTryLogin = TRUE;
			break;

		case STATE_PROCESS_MSG:	// 7:Accept ����. Ircd & Client�� ������ ��Ŷ�� ó���Ѵ�.
			ParsePacket((char*)lParam);
			break;

		case STATE_CONNECTED_IRCD:	// 8:IRCD Connect �Ϸ�.
			m_iState=STATE_PROCESS_MSG;
			ReplySpace();
			m_bTryLogin = FALSE;
			break;
		case STATE_WM_CLOSE:
			break;
		default:
			break;
	}	
	return 0L;
}//end of OnNetProcess()




/*
** NAME    **: ParsePacket()
** PARAM   **: char* packet
** RETURN  **: void
** DESC    **: IRCD���� �ްԵǴ� �޽������� parsing �Ѵ�.
** by bemlove at 2002-10-07, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ParsePacket( char* szPacket )
{
#ifdef _UNICODE
	TCHAR wBuff[1024];
	AnsiConvertMtoW(szPacket, wBuff, sizeof(wBuff) - 1);
//	TRACE(_T("�ڡڡڡڡ�Receive From Ircd : %s\n"), wBuff );
#else
//	TRACE("�ڡڡڡڡ�Receive From Ircd : %s\n", szPacket );
#endif

	char *prefix,*cmd,*param,*data;
	char *ptmp,*pdest;
	char Scmd[MAXBUF];
	int result;

	// prefix �� cmd parameter �� �̾Ƴ��ϴ�.
	if (*(szPacket)==':')	// prefix
	{
			prefix = szPacket;
			while (*(szPacket)!=' ')szPacket++;		
			while (*(szPacket)==' ') szPacket++;
			ptmp = szPacket;
	}
	else	
	{
		prefix	= NULL;
		ptmp	= szPacket;
	}

	data = NULL;
	/* Search forward. */
	pdest = strchr( ptmp, ' ' );
	if( pdest != NULL )
	{
		result = pdest - ptmp;
		strncpy(Scmd,ptmp,result);
		Scmd[result] = '\0';
	}
	cmd = szPacket;

	while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
	param = szPacket;
	// end of parsing

	if((result=atoi(Scmd))!=0)
	{
		switch(result)
		{
		case RPL_USERIP:		// 307 : ������ ������ �˾Ƴ���
			while (*(szPacket)!='@') szPacket++; 
			data = szPacket;
			ReplyUserIP307( data+1 );
			break;
		case RPL_ENDOFMOTD:		// 376 : :End of /MOTD command"
			// Login Completed
			if(!m_bAlreadyLogin || m_bRandomID)
			{
				m_iState = STATE_CONNECTED_IRCD;
				OnNetProcess(0, 0 );
			}
			else
				ProcessAlreadyLogin();
			break;
		case ERR_NICKNAMEINUSE:	// 433 : �̹� ������ �ִ� ����� ���� �ٲپ� ��õ��� �Ѵ�.
			if(++m_nAlreadyTry < MAX_ALREADYTRYCNT)
			{
				AlreadyLogin();
				break;
			}

			ProcessLoginFailed();

			m_iState = STATE_ERROR_IRCD;
			OnNetProcess( 0, 0 );
			break;
		case ERR_NOSUCHNICK:	// 401 : PRIVMSG �� error Response
			// skip receiver
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			data = szPacket;
			ReceiveNoSuchNick401(data);
			break;
		case RPL_NAMREPLY:		// 353 : names �� ����
			while (*(szPacket)!=':') szPacket++; 
			data = szPacket;
			ReplyNamesList353( data+1 );
			break;
		case RPL_ENDOFNAMES:	// 366 : names �� ��
			ReplyEndOfNames366( data );
			break;
		case 388:			// 388 : EventD �� Nick reply
			// skip receiver
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			data = szPacket;
			SetEventD( data+1 );		// data�� : ���� �����մϴ�.
			break;
		case 494:			// 494 : EventD�� IRCD�� ������� �ʾ��� ���� reply
			// skip receiver
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			data = szPacket;
			ReceiveNoEventD494();		
			break;
		case 303:			// 303 : ���� �¶��� ������ ����� ���� ����
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			data = szPacket;
			Receive303( data+1 );
			break;
		default:
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			data = szPacket;
			TRACE(_T("IRC ERROR\n"));
			break;
		}
	}
	else
	{
		if( !strncmp( cmd, STR_IRCD_PING, 4 ) )
			m_pcIrcdSock->ResponsePONG( param );
		else if (!strncmp(cmd,STR_IRCD_PRIVMSG,7))
		{
			// skip receiver - cmd �ڿ� receiver �� ������ data�� �������� cmd�� ����
			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			szPacket[strlen(szPacket)-2] = 0x00;
			data = szPacket;
			ParsePrivmsg( prefix+1,  data+1 );
		}
		else if(!strncmp(cmd,STR_IRCD_JOIN,4))
		{

			ReceiveJoin( prefix+1 );
		} 
		else if( !strncmp( cmd, STR_IRCD_KILL, 4 ) )
		{
			m_bClosedByOther = true;
		}
		else if( !strncmp(cmd, STR_IRCD_NOTICE, 6) )
		{
 			while (*(szPacket)!=' ') szPacket++; while (*(szPacket)==' ') szPacket++;
			szPacket[strlen(szPacket)-2] = 0x00;
			data = szPacket;
			ParseNotice( prefix+1, data+1 );
		
		}
		// added by moonknit 2006-01-19
		else if( !strncmp(cmd, STR_IRCD_NSTAT, 5) )
		{
 			while (*(szPacket)!=' ') ++szPacket;
			*(szPacket) = 0x00;
			++szPacket;
			while (*(szPacket)==' ') ++szPacket;
			szPacket[strlen(szPacket)-2] = 0x00;
			data = szPacket;

			while(*(data) == ':') ++data;

			// param = user cnt
			// data = post cnt
			theApp.m_nnetpostcnt = atoi(data);
			theApp.m_nnetusercnt = atoi(param);
			if(theApp.m_nnetusercnt) -- theApp.m_nnetusercnt;

			::SendMessage(m_hSearchView, WM_NETCNTCHANGED, NULL, NULL);
		}
	}
}//end of ParsePacket()

#define PVCT_UNKNOWN				0
#define PVCT_NR						1
#define PVCT_EK						2
#define PVCT_PQ						3
#define PVCT_PU						4
#define PVCT_GN						5
#define PVCT_CR						6

int parseprivcmd(char* data, char** param)
{
	int type = PVCT_UNKNOWN;
	if( *data == 'N' )		// NR
	{
		++data;
		if( *data == 'R' ) type = PVCT_NR;
	}
	else if( *data == 'E' ) // EK
	{
		++data;
		if( *data == 'K' )	type = PVCT_EK;
	}
	else if( *data == 'P' ) // PQ, PU
	{
		++data;
		if( *data == 'Q' )	type = PVCT_PQ;
		else if( *data == 'U' )	type = PVCT_PU;
	}
	else if( *data == 'G' ) // GN
	{
		++data;
		if( *data == 'N' ) type = PVCT_GN;
	}
	else if( *data == 'C' )
	{
		++data;
		if( *data == 'R' ) type = PVCT_CR;
	}

	if(type != PVCT_UNKNOWN)
	{
		++data; ++data;
		while( *data == ' ' ) ++data;		// ��������
		*param = data;
	}
	else
		*param = NULL;

	return type;
}

/*
** NAME    **: ParsePrivmsg()
** PARAM   **: char* prefix, char* data
** RETURN  **: void
** DESC    **: PRIVMSG �� ������ �м� ( - ���� ��û(RA)�� ���� �޽��� Ȯ�� �κ� -> AR;OK / AR;NK )
** by bemlove at 2002-10-16, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ParsePrivmsg(char* prefix,  char* data )
{
	static enum
	{
		TYPE_UNKNOWN = 0,
		TYPE_REGUSER,
		TYPE_UNREGUSER
	};

	int type = TYPE_UNKNOWN;
	if( *data == 'F' ) type = TYPE_REGUSER;
	else if( *data == 'N' ) type = TYPE_UNREGUSER;
	if( type != TYPE_UNKNOWN )		// IRCD������ �޴� �޽��� ����Ʈ
	{
		while( *data == ' ' ) ++data;		// ��������
		while( *data != ';' ) ++data; ++data;// FISH;, NFSH; ����

		char* param;

		type = parseprivcmd(data, &param);
		int rcode = 0;

		switch(type)
		{
		case PVCT_NR:
			rcode = atoi(param);
			break;
		case PVCT_EK:
			break;
		case PVCT_PQ:
			rcode = atoi(param);
			break;
		case PVCT_PU:
			rcode = atoi(param);
			break;
		case PVCT_GN:
			break;
		case PVCT_CR:
			rcode = atoi(param);
			break;
		}

/*
		if( *data == 'N' )		// NR
		{
			++data;
			if( *data == 'R' )
			{
				// NR Return
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������

				int rcode = atoi(data);
				// return code
			}
		}
		else if( *data == 'E' ) // EK
		{
			++data;
			if( *data == 'K' )
			{
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������

				// data is EncriptKey
			}
		}
		else if( *data == 'P' ) // PQ, PU
		{
			++data;
			if( *data == 'Q' )
			{
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������

				int rcode = atoi(data);
				// return code
			}
			else if( *data == 'U' )
			{
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������

				int rcode = atoi(data);
				// return code
			}
		}
		else if( *data == 'G' ) // GN
		{
			++data;
			if( *data == 'N' )
			{
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������
			}
		}
		else if( *data == 'C' )
		{
			++data;
			if( *data == 'R' )
			{
				++data; ++data;
				while( *data == ' ' ) ++data;		// ��������

				int rcode = atoi(data);
			}
		}
*/
	}
	else					// ��Ÿ ENPY��ü���� �����ϴ� �޽��� ����Ʈ
	{	
		char* p;
		char	sender[MAX_USERID+1];
		int i=0;
		p = prefix;
		while( *p == ' ' ) p++;		//prefix ���� ���� ����
		while( *p != '!' && i < MAX_USERID )
		{	sender[i++] = *(p++); }		// sender id 
		sender[i] = 0;
		int iRead = 0;
		char buff[MAX_BUFF+1];
		memset(buff, 0x00, sizeof(buff) );
		int msgType = 0;
		// Read msgType
		if( -1 == ( iRead = getString (buff, sizeof(buff), data, MAX_BUFF, true) ) ) return ;
		msgType = atoi( buff );
		data += (iRead + 1 );
		int status = -1;
	}
}//end of ParsePrivmsg()


void CIrcdManager::SetEventD( char* data )
{
	int i=0;
	char szNick[MAX_EVENTD+1],*pNick;

	pNick = data;
	while (*pNick != '\r' && *pNick != '\n') 
	{
		if(i > MAX_EVENTD) 
			break;

		szNick[i] = *pNick;
		i++; pNick++;
	}
	szNick[i] = '\0';

	strcpy(m_szEventD, szNick);

	// by bemlove at 2003-05-29 ���� 1:41:21
	m_iChannelCount = 1;
	strcpy(m_szChannelList[0], szNick);

	// added by moonknit 2006-02-20
	// �α��� ���� �ƴ� ��� ���� �̺�Ʈ ������ ��ϵǸ� NR ��Ŷ�� ������
	if(!m_bTryLogin)
		SendNR();
}//end of SetEventD()




/*
** NAME    **: ReceiveJoin
** PARAM   **: char* data
** RETURN  **: void
** DESC    **: �ٸ� ������ ���� ������� �α������� ���� ���� ���� �Ǿ����� ���θ� Ȯ��
** by bemlove at 2002-11-27, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveJoin( char* data )
{
	char* p;
	char	sender[MAX_USERID+1];
	int i=0;
	p = data;
	while( *p == ' ' ) p++;		//prefix ���� ���� ����
	while( *p != '!' && i < MAX_USERID )
	{	sender[i++] = *(p++); }		// sender id 
	sender[i] = 0;

	char tmpID[MAX_USERID+1];
	tmpID[0] = 0L;

	if( isEqual( tmpID, sender ) )
	{
		m_bClosedByOther = true;
	}
	
}//end of ReceiveJoin()


/*
** NAME    **:  ReceiveHK()
** PARAM   **: void
** RETURN  **: void 
** DESC    **: IRCD�� ������ ä�ο� �� ���� ������ ���� handler key ���� ����(HK)
** by bemlove at 2002-10-31, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveHK()
{
	char szMd5[MAX_SMALLBUFF+1], szPasswd[1000+1];
	memset( &szMd5, 0x00, sizeof(szMd5) );
	memset( &szPasswd, 0x00, sizeof(szPasswd) );

	// Guest Password ����
//	if(m_pDM->GetGuestLevel() == 0)
//		sprintf(szMd5, "%s%s%s", m_pDM->m_UserID, m_pDM->GetPasswd(), m_pDM->GetHandlerKey() );
//	else
//		sprintf(szMd5, "%s%s", m_pDM->m_UserID, m_pDM->GetHandlerKey() );

	md5_epw( szMd5, szPasswd );
//	m_pDM->SetMD5( szPasswd );
}//end of ReceiveHK()


/*
** NAME    **: SendAR()
** PARAM   **: 
** RETURN  **: 
** DESC    **: ����� ���� ��û
** by bemlove at 2002-10-09, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::SendAR()
{	
	char sendBuff[MAX_BUFF+1];
	memset( sendBuff, 0x00, sizeof(sendBuff) );

	bool autoLogin = false;

	if( !IsValidSock() ) 
		return;

	// ���⿡�� ���� ������Ʈ���� ��ϵǾ� �ִ� ������ ������ �����Ͽ� �ش�. ���ٸ� ����Ʈ ��
	CString expPort, srcPort;
//	expPort = ReadRegData( REG_STR_SERVERINFO, REG_STR_EFSPORT );
//	srcPort = ReadRegData( REG_STR_SERVERINFO, REG_STR_GNUPORT );

	// port ���� ����
//		sprintf(sendBuff, "{%s;AR;%s;%d;%d;%s}%s", ENPPY_GUESTTITLE, m_pDM->GetHostIP(), m_pDM->GetFESHostPort(), m_pDM->GetGnuHostPort(), m_pDM->GetAvatar(), m_pDM->GetMD5() );

	m_pcIrcdSock->Privmsg( m_szChannelList[0], sendBuff );

}//end of SendAR()


/*
** NAME    **: ReceiveAROK
** PARAM   **: void
** RETURN  **: void 
** DESC    **: ���� ����
** by bemlove at 2002-10-31, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveAROK()
{	
	if(!GetReconnecting())
	{
	}


//	//[�ɼ�] ���� Ŭ�� ������ ����Ʈ�� �߰��� ������ ��쿡�� ����.

//	m_pDM->InitInformation();

//	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
//	if(pMainFrame)
//	{
//		pMainFrame->StartServer();
//
//		if(!GetReconnecting())
//			pMainFrame->ChildViewInit();
//	}

//	if(!GetReconnecting())
//		m_pDM->m_pComm->LoadDownloads();

	// ���� Ÿ���� ������ Ŭ�� ���� �̿��� ���
//	if(m_pDM->GetRunType()==RT_REG_SUSER )
//		theApp.GetSRManager()->RequestEF(m_pDM->GetSUID());
//	else if( m_pDM->GetRunType() == RT_UNREG_RFID || m_pDM->GetRunType() == RT_REG_RFID)
// 		theApp.GetMainFrame()->RFIDLinkStart();

}//end of ReceiveAROK()


/*
** NAME    **: ReceiveARNK()
** PARAM   **: boolen InvalidPasswd - ��й�ȣ Ʋ��(true), �׷� ����� ����(false) 
** RETURN  **: void
** DESC    **: ���� ����
** by bemlove at 2002-10-31, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveARNK( bool InvalidPasswd )
{
	if( InvalidPasswd )
	{
		FishMessageBox(STR_MSG_IRCDMANGER_006, STR_MSG_IRCDMANGER_007, MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		FishMessageBox(STR_MSG_IRCDMANGER_008, STR_MSG_IRCDMANGER_007, MB_OK | MB_ICONINFORMATION );
	}

//	loginDlg.m_strID = m_pDM->GetID();
	
	char szMd5[MAX_SMALLBUFF+1], szPasswd[1000+1];
	memset( &szMd5, 0x00, sizeof(szMd5) );
	memset( &szPasswd, 0x00, sizeof(szPasswd) );
//	char* id= _strlwr( _strdup( m_pDM->GetID() ) );
//	sprintf(szMd5, "%s%s%s", id, m_pDM->GetPasswd(), m_pDM->GetHandlerKey() );
	md5_epw( szMd5, szPasswd );
//	m_pDM->SetMD5( szPasswd );

	SendAR();
}//end of ReceiveARNK()

/*
** NAME    **: AlreadyLogin()
** PARAM   **: void
** RETURN  **: boolean
** DESC    **: NICK�� �̹� IRCD�� ������ �ִ� ���
** by bemlove at 2002-10-08, VER : 1.0, COMMENTS: ORG 
*/
bool CIrcdManager::AlreadyLogin()
{
	if(!m_bRandomID)
	{
		m_bAlreadyLogin = true ;

		strcpy(m_szTempID, theApp.m_spLP->GetIRCID());

		if(! GetReconnecting() ) // �������� �����̾��� ���.
		{
			if (FishMessageBox(STR_MSG_IRCDMANGER_010, MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
			}
			else
			{
				// �ٽ� �Է� �۾��� ��û�Ѵ�.
				::SendMessage(m_hMsgParent, WM_IRCDAUTHCOMPLETE, ERR_LOGINRETRY, 0);
				return false;
			}
		}
		else // ��Ʈ�� ���� �߻����� �翬�� ��û�̾��� ���
		{
		}

		if( !IsValidSock() ) return false;
		char tmpID[MAX_USERID+1];
		strcpy( tmpID, theApp.m_spLP->GetIRCID() );
		strcat( tmpID, "MA" );
		m_pcIrcdSock->Nick( tmpID );
	}
	else
	{
		// MakeRandom Nick ID
		theApp.m_spLP->MakeRandomIRCID();

		strcpy(m_szTempID, theApp.m_spLP->GetIRCID());
		char tmpID[MAX_USERID+1];
		strcpy( tmpID, theApp.m_spLP->GetIRCID() );
		m_pcIrcdSock->Nick( tmpID );

		// and send nick
	}
	
	return true;
	
}//end of AlreadyLogin()


/*
** NAME    **: SendCR()
** PARAM   **: 
** RETURN  **: 
** DESC    **: �̹� IRCD �� ����� ���̵� �α��� ���� �� IRCD ó��
** created 2006-02-17 :: by moonknit 
*/
bool CIrcdManager::SendCR()
{
	char buff[MAX_BUFF+1];
	memset( buff, 0x00, sizeof(buff) );

	if( !IsValidSock() )
		return false;

	char tmpPass[MAX_MD5+1];
#ifdef _UNICODE
	CString ep = theApp.m_spLP->GetEncodedPassword();
	int nresult = WideCharToMultiByte(CP_ACP
		, WC_COMPOSITECHECK
		, ep, ep.GetLength()
		, tmpPass, sizeof(tmpPass)
		, NULL, NULL);

	if(nresult > MAX_MD5)
		return false;
	tmpPass[nresult] = 0;
#else
	strcpy(tmpPass, theApp.m_spLP->GetEncodedPassword());
#endif
	sprintf( buff, "%s %s", theApp.m_spLP->GetIRCID(), tmpPass );
	
	m_pcIrcdSock->Privmsg( m_szChannelList[0], buff );

	return true;
}//end of SendCR()


/*
** NAME    **: ReplySpace()
** PARAM   **: 
** RETURN  **: 
** DESC    **: IRCD�� ���� �Ϸ� �Ŀ� Assistant ä�� ����Ʈ�� ��û. deprecated
**				: Node Information Transfer && IRC ���� �Ϸ� �˸� by moonknit 2006-01-31
** by bemlove at 2002-10-09, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReplySpace()
{
	if( !IsValidSock() ) 
		return;

	SendNR();
	SendNSTAT();

	theApp.m_spLP->SetIRCConnected(TRUE);
	::SendMessage(m_hMsgParent, WM_IRCDAUTHCOMPLETE, ERR_NONE, 0);

}//end of ReplySpace()


/*
** NAME    **: ReplyNamesList353()
** PARAM   **: 
** RETURN  **: 
** DESC    **: channel name list Reply ��Ŷ �м�
** by bemlove at 2002-10-09, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReplyNamesList353( char* data )
{
	return;
	int i=0;
	char *pData;
	if( (m_iLogicHandlerState==0 ) || (m_iLogicHandlerState==2) )
	{
		m_iChannelCount++;
		m_szChannelList[m_iChannelCount-1][0] = '\0';
		pData= data;
		while( *pData != '\r' && *pData != '\n' )
		{
			if( *pData != ' ' )
			{
				m_szChannelList[m_iChannelCount-1][i] += *pData;
			}
			else
			{
				m_szChannelList[m_iChannelCount-1][i] += '\0';
				m_iChannelCount ++ ;
			}
			++i;
			++pData;
		}
	}
	m_iChannelCount --;
}//end of ReplyNamesList353()



/*
** NAME    **: ReplyEndOfNames366()
** PARAM   **: 
** RETURN  **: 
** DESC    **: Channel Name List�� ��. ChannelList�� �߿��� ������ �ϳ��� �����Ͽ� Join �ǽ�
** by bemlove at 2002-10-09, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReplyEndOfNames366( char* data )
{
}//end of ReplyEndOfNames366()



/*
** NAME    **: ReplyUserIP307()
** PARAM   **: 
** RETURN  **: 
** DESC    **: Local Host IP ����
** by bemlove at 2002-10-10, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReplyUserIP307( char* data )
{
	int i=0;
	char szIP[16],*pIP;

	pIP = data;
	while (*pIP != '\r' && *pIP != '\n') 
	{
		szIP[i] = *pIP;
		i++; pIP++;
	}
	szIP[i] = '\0';

	CString readData= "";

	// local ip ��� �κ��� �� ���� ������?
	theApp.m_spGD->SetHostIPList();

}//end of ReplyUserIP307()


/*
** NAME    **: ReceiveCROK
** PARAM   **: void
** RETURN  **: void
** DESC    **: ���̵� �̹� ������̾����� ���� ����� ���̵� ���� ��û 
** by bemlove at 2002-10-22, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveCROK()
{
	Sleep(100);
	if( m_pcIrcdSock != NULL )
	{
 		m_pcIrcdSock->Close();
		int iError = GetLastError();
		m_pcIrcdSock->OnClose( iError );

		m_pcIrcdSock = NULL;
	}
	Login();
}//end of ReceiveCROK()



/*
** NAME    **: ReceiveCRNK
** PARAM   **: void
** RETURN  **: void
** DESC    **: �̹� ������� ���̵� ���� ��û ����
** by bemlove at 2002-11-02, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveCRNK( bool InvalidPasswd )
{
	if( InvalidPasswd )
	{
		FishMessageBox(STR_MSG_IRCDMANGER_006, STR_MSG_IRCDMANGER_007, MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		FishMessageBox(STR_MSG_IRCDMANGER_008, STR_MSG_IRCDMANGER_007, MB_OK | MB_ICONINFORMATION );
	}

	// COMMENT by moonknit 2006-02-12
	// ����â�� ��� �� �ֵ��� �޽����� �����Ѵ�.

}//end of ReceiveCRNK()

/*
** NAME    **: SendCP()
** PARAM   **: int hostPort, int gnuPort
** RETURN  **: void
** DESC    **: ��Ʈ ���� ��û
** by bemlove at 2002-11-02, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::SendCP( int hostPort, int gnuPort )
{
	if( hostPort == 0 || gnuPort == 0 ) return;

	char sendBuff[MAX_BUFF+1];
	memset( sendBuff, 0x00, sizeof(sendBuff) );
	if( !IsValidSock() )
	{
		TRACE(_T("[err] SendCP�ÿ� ircd socket ���� "));
		return;
	}
	sprintf(sendBuff, "{%s;CP;%d;%d}", FISH_TITLE, hostPort, gnuPort );

	m_pcIrcdSock->Privmsg( m_szChannelList[0], sendBuff );
}//end of SendCP()


/*
** NAME    **: ParseNotice
** PARAM   **: char* prefix, char* data
** RETURN  **: void
** DESC    **: IRCD �� NOTICE �޽����� ���� ��Ŷ �м�
** by bemlove at 2002-10-25, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ParseNotice( char* prefix, char* data )
{
	char* p;
	char	sender[MAX_USERID+1];
	int i=0;
	p = prefix;
	while( *p == ' ' ) p++;		//prefix ���� ���� ����
	while( *p != '!' && i < MAX_USERID )
	{	sender[i++] = *(p++); }		// sender id 
	sender[i] = 0;


	if( strncmp( data, "{ENPY;GN", 8 ) == 0 )	// Global Notice
	{
		while( *data == ' ' ) data++;	// ��������
		while( *data != ';' ) data++; // {ENPY ����
		data++;					// ; ����
		while( *data != ';' ) data++; // GN ����
		data++;					// ; ����
	
		if( (strncmp( data, "RESET", 5 ) == 0 ) || (strncmp( data, "SHUTDOWN", 8 )==0) )		// �ý��� ���� �Ǵ� �˴ٿ�
		{
			while( *data != ';' ) data ++ ;		// RESET | SHUTDOWN ����
			data++;
			char szSec[10]={0};
			i=0;
			while( *data != '}' )				// sec(�ð�) ����
			{
				szSec[i++] = *data ++;
			}
			szSec[i] = 0L;
			ReceiveGN( 0, szSec, data+1 );
		}
		else if( strncmp( data, "NOTICE", 6 ) == 0 )		// �ȳ� �޽���
		{
			data += 7;
			char szPos[10]={0};
			i=0; 
			while( *data != '}' )
			{
				szPos[i++] = *data ++;
			}
			szPos[i] = 0L;
			ReceiveGN( 1, szPos, data+1);
			
		}
		else if( strncmp( data, "MIG", 3) == 0 )			// �ڵ鷯 �˴ٿ� ����, msg�� �翬���� �ش� ���� �ڵ鷯�� �ƿ��ϴ�, �ٸ� �ڵ鷯�� �̻簡��� �޽���
		{
			ReceiveGN( 2, NULL, data+4 );
		}

	}
	else
	{
		int iRead = 0;
		char buff[MAX_BUFF+1];
		memset(buff, 0x00, sizeof(buff) );
		int msgType = 0;
		// Read msgType
		if( -1 == ( iRead = getString (buff, sizeof(buff), data, MAX_BUFF, true) ) ) return ;
		msgType = atoi( buff );
		data += (iRead + 1 );
	}
}//end of ParaseNotice()




/*
** NAME    **: Receive303
** PARAM   **: char* data
** RETURN  **: void
** DESC    **: ���� �¶��� ģ������Ʈ 
** by bemlove at 2002-12-07, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::Receive303( char* data )
{
	char* p;
	char  buddyID[MAX_USERID+1]={0};
	int  size=0;
	p = data;

	int len = strlen(p);
	for( int idx = 0; idx < len; idx++ )
	{
		if( (*p) != ' ' && (*p) != '\r' && (*p) != '\n' )
		{
			buddyID[size++] = (*p);
			p++;
		}
		else
		{
			if( size >0 )
			{
				buddyID[size] = 0;
				// commented by moonknit 2005-05-26
//				if( pApp->m_pMessengerLeftView == NULL ) return;
//				if( theApp->GetDataManager()->ChgBuddyStatus( buddyID, STATUS_ONLINE) )
//					pApp->m_pMessengerLeftView->OnBuddyStatusChange( buddyID, STATUS_ONLINE);

			}

			p++;
			memset( buddyID, 0x00, sizeof(buddyID) );
			size = 0;
		}
	}

}//end of Receive303()

/*
** NAME    **: ReceiveGN
** PARAM   **: int type(�ý�������/�˴ٿ�->0, �ȳ��޽���->1, �ڵ鷯 �˴ٿ�->2), char* data(sec/pos/NULL), char* msg
** RETURN  **: void
** DESC    **: �ý��� �޽����� ���� ���.
** by bemlove at 2002-12-11, VER : 1.0, COMMENTS: ORG 
*/
void CIrcdManager::ReceiveGN( int type, char* data, char* msg )
{
	CString strMsg="";
	if( type == 0 )
	{
		strMsg.Format(STR_MSG_IRCDMANGER_013, data, msg );
//		MessageBox(NULL, strMsg, STR_MSG_IRCDMANGER_014, MB_OK | MB_ICONINFORMATION );
	}
	else if( type == 1 )
	{
//		MessageBox(NULL, msg, STR_MSG_IRCDMANGER_015, MB_OK | MB_ICONINFORMATION );
	}
	else if( type == 2 )
	{
		// �̵��� ���� �ڵ鷯�� ���� ��쿡�� �׳� ���� �ִ´�.
		if( m_iChannelCount < 2 )
		{
			TRACE(_T("�̵��� ä���� ��� �� �ű�"));
			return;
		}

//		m_pDM->SetLHShutdown( true );

		// Just Test... 
		// �̰����� ���� ���� �ִ� ä���� ���� �ľ��Ͽ�.. 2�� �̻��� ��쿡�� ������ 
		// ä�η� ������ �����ϵ��� �Ѵ�.
		m_pcIrcdSock->Part( m_szChannelList[0] );
		int selectedChannel = 0;			
		srand ( GetTickCount() );
		if( !IsValidSock() ) return;
		selectedChannel = rand() % m_iChannelCount;
		if( selectedChannel == 0 ) selectedChannel++;
		char tempChannel[MAX_CHANNEL_COUNT];
		strcpy(tempChannel, m_szChannelList[0]);
		strcpy(m_szChannelList[0], m_szChannelList[selectedChannel]);
		strcpy(m_szChannelList[selectedChannel], tempChannel);

//	 	TRACE(_T("IRCD Join Request : %s"), m_szChannelList[0] );
		m_pcIrcdSock->Join( m_szChannelList[0], "");
	}
	else
		return;
		
}//end of ReceiveGN()


//**************************************************************************************
// NAME :  ReceiveNoEventD494()
// PARM :  void
// RETN :  void 
// DESC :  ���� �������� �̺�Ʈ ������ �������� �ʰų�, �����߿� �̺�Ʈ ������ ����� ���.
// COMT : by bemlove 2003-03-24 ���� 3:55:18 ver - 1. 0 
//**************************************************************************************
// online1.entica.com 494 armisan :No event deamon found
void CIrcdManager::ReceiveNoEventD494()
{

	// commented by moonknit 2005-07-26
	// ���� ���� �̺�Ʈ ������ ����Ǹ� �ٸ� ó��?
	// 1. ���� ����� �̺�Ʈ ������ ���� ���..
	if( IsEmpty( m_szEventD ) )
	{
	}
	// 2. ���� ���� �̺�Ʈ ������ ����� ���..
	else 
	{
		ResetConnection();
	}
}//end of ReceiveNoEventD494()


//**************************************************************************************
// NAME :  ReceiveNoSuchNick401
// PARM :  char* data -> ex) id :No such nick
// RETN :  void
// DESC :  �ش����ڰ� ���� ����߿� ���� ä�������� �ϳ��� ������ ��쿡��
//		   ä���� �̵��ϵ��� �Ѵ�.
// COMT : by bemlove 2003-03-24 ���� 5:47:51 ver - 1. 0 
//**************************************************************************************
void CIrcdManager::ReceiveNoSuchNick401(char* data )
{
	char tmpID[MAX_USERID+1]={0};
	int idx=0;
	while( *data != ' ' )
	{
		tmpID[idx++] = *data++;
	}
	if( idx <= 0 ) return;
	tmpID[idx] = 0L;

	// �������� �ʴ� ���̵� ä�θ���Ʈ�� �ϳ������� Ȯ���Ѵ�.
	for( idx=0; idx < m_iChannelCount; idx++ )
	{
		// ä���� ����� ���̹Ƿ�.. ä���� �̵��ؾ� �Ѵ�.
		// �̶��� ���Ӱ� ������ �õ��Ѵ�..
		if( isEqualNocase( tmpID, m_szChannelList[idx] ) )
		{
			NetworkDisconnected();
		} 
	}

}//end of ReceiveNoSuchNick401()

//**************************************************************************************
// NAME :  ReqForceRemove
// PARM :  char* uid, int type (TIME_OUT, UNKNOWN)
// RETN :  void
// DESC :  ����Ž������ ��Ʈ���� �̻��� ������� ��쿡 ������ ����Ʈ���� ���Ÿ� ��û�Ѵ�.
// COMT : by bemlove 2003-03-07 ���� 11:48:21 ver - 1. 0 
//**************************************************************************************
void CIrcdManager::ReqForceRemove( char* uid, int type )
{
	if( IsEmpty( uid ) ) return;
	if( type <= 0 || type >2 ) return;
	char sendBuff[MAX_SMALLBUFF+1]={0};
	sprintf(sendBuff, "{%s;FR;%d;%s}", FISH_TITLE, type, uid );
	m_pcIrcdSock->Privmsg( m_szChannelList[0], sendBuff );
}//end of ReqForceRemove()


//**************************************************************************************
// NAME :  SendFRNotice
// PARM :  char* uid, int type
// RETN :  void
// DESC :  uid ����ڿ��� ��Ʈ�� �ҷ����� ���� Ŭ�� ��������Ʈ���� �����Ǿ����� �뺸.
// COMT : by bemlove 2003-03-07 ���� 1:30:17 ver - 1. 0 
//**************************************************************************************
void CIrcdManager::SendFRNotice( char* uid, int type )
{
	if( IsEmpty( uid ) ) return;
	if( type <= 0 || type >2 ) return;
	char sendBuff[MAX_SMALLBUFF+1]={0};
//	sprintf( sendBuff, "%d%c%d", TICA_FORCE_REMOVE, DELIMITER, type);
	m_pcIrcdSock->Privmsg( uid, sendBuff );
}//end of SendFRNotice()



//**************************************************************************************
// NAME :  NetworkDisconnected
// PARM :  void 
//		   1.1 : int type -> EVENTD ���� ���� ������ ���� Ÿ�̸�..
// RETN :  void
// DESC :  ��Ʈ���� ������ �߻��Ͽ� ������ ����Ǿ���.
//		   ���� ��Ʈ�� ������ �������� ���ƿö����� �ֱ������� ��Ʈ�� ��Ȳ�� �˻��Ͽ�
//         ��Ʈ���� �������� ���ƿ��� ��쿡 �ٽ� ���α׷��� �����Ͽ� �ش�.
//		  - 1.1 : EventD ������ �����ϱ� ���� Ÿ�� �߰�..
// COMT : by bemlove 2003-03-24 ���� 10:43:32 ver - 1. 1
//  1.2 : ���� �̻����� ���� ������ �������ϴ� �ð� ���� ���� (1��~10��)
//        �� �� ��Ʈ�� ������ 1~2��
//**************************************************************************************
void CIrcdManager::NetworkDisconnected(int type)
{
	// commented by moonknit 2005-05-20
/*
	pApp->m_pMainFrame->SwitchChildFrame( FIRST_VIEW );
	pApp->m_pMainFrame->SetNetworkDisconnected( true );
	pApp->m_WaitCursor = false;
	int time = 0;
	srand(GetTickCount() );
	if( type == TIMER_EVENTD_ERROR )
	{
		m_pcIrcdSock->Close();
		time = (rand() % 540 ) + 60;
		pApp->m_pMainFrame->ErrorMessage( STR_MSG_IRCDMANGER_020, 30, false);
		pApp->m_pMainFrame->SetTimer( TIMER_EVENTD_ERROR, time * TIME_EVENTD_ERROR, NULL );

	}
	else
	{
		m_pcIrcdSock->Close();
		time = (rand() % 60 ) + 60;
		pApp->m_pMainFrame->ErrorMessage( STR_MSG_IRCDMANGER_003, 10);
		pApp->m_pMainFrame->SetTimer( TIMER_NETWORK_ERROR, time * TIME_NETWORK_ERROR, NULL );
	}
	*/
}//end of NetwordkDisconnected()

// commented by moonknit 2005-05-24
// Login ���ǿ� ���� Login ����� ���� �����Ѵ�.
/*
** NAME    **: Login
** PARAM   **: ..
** RETURN  **: boolean
** DESC    **: ������Ʈ�� ������ ���� �α��� ..
** by bemlove at 2002-10-15, VER : 1.0, COMMENTS: ORG 
**	// Date : 2005-06-09,	By bemlove
*/
bool CIrcdManager::Login()
{
	m_bKeepon = FALSE;
	if(m_bRandomID)
	{
		if(*theApp.m_spLP->GetIRCID() == 0)
			theApp.m_spLP->MakeRandomIRCID();
	}

	m_iState = STATE_CONNECT_IRCD;
	OnNetProcess(0, 0);
	return true;
}//end of Login()

void CIrcdManager::ResetConnection(BOOL bReconnect)
{
/*	CMainFrame *pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
	{
		pMainFrame->StopServer();
		pMainFrame->SetTrayStatus(TS_NONODE);
	}

	m_pcIrcdSock->Close();
	if(bReconnect)
	{
		AfxMessageBox("������ ������ ������ �� ������ �õ��մϴ�.");
		// 1�е� �翬�� �õ�
		pMainFrame->SetTimer(TI_RECONNECT, 1000 * 60, NULL);
	}
*/
}

// ���� ��õ�
void CIrcdManager::RetryConnect()
{
	m_nReconCnt = 0;
	if(m_nRetryConnect == 0)
	{
		m_bKeepon = FALSE;
		FishMessageBox(STR_ERR_IM_SERVERCONNECT);
		return;
	}

	SetReconnecting( true );

	if(m_nRetryConnect > 0)
		m_nRetryConnect --;

	Login();
}

void CIrcdManager::SendNR()
{
	char sendBuff[MAX_BUFF+1];
	memset( sendBuff, 0x00, sizeof(sendBuff) );

	if( !IsValidSock() ) 
		return;

	struct in_addr in;
	in.S_un.S_addr = theApp.m_spGD->m_LocalHost.S_addr;
	char* hostip = inet_ntoa(in);

	if(!hostip) return;

	if(m_bRandomID)
		sprintf(sendBuff, "%s;%s; %s %d %d"
			, IPPH_NFISH
			, IPCMD_NR
			, hostip
			, theApp.m_spGD->GetGnuHostPort()
			, 1);
	else
		sprintf(sendBuff, "%s;%s; %s %d %d %d"
			, IPPH_FISH
			, IPCMD_NR
			, hostip
			, theApp.m_spGD->GetGnuHostPort()
			, 1
			, theApp.m_spLP->GetUserSeq()
			);

	m_pcIrcdSock->Privmsg( m_szChannelList[0], sendBuff );
}

void CIrcdManager::SendNU()
{
	char sendBuff[MAX_BUFF+1];
	memset( sendBuff, 0x00, sizeof(sendBuff) );

	if( !IsValidSock() ) 
		return;

	if(m_bRandomID)
		sprintf(sendBuff, "%s;%s; %d"
			, IPPH_NFISH
			, IPCMD_NU
			, theApp.m_spGD->GetGnuHostPort()
			);
	else
		sprintf(sendBuff, "%s;%s; %d"
			, IPPH_FISH
			, IPCMD_NU
			, theApp.m_spGD->GetGnuHostPort()
			);

	m_pcIrcdSock->Privmsg( m_szChannelList[0], sendBuff );
}

void CIrcdManager::SendNSTAT()
{
	m_nNSTATCnt = 0;
	m_pcIrcdSock->Nstat(theApp.m_ntotalpostcnt);
}

/**************************************************************************
 * method CIrcdManager::ProcessAlreadyLogin
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-17
 *
 * @Description
 * User ID�� �̿��Ͽ� �α��� �� �� �̹� ���� ���̵��� ����ڰ� ������ ���
 * ���� ����ڸ� �����Ѵ�.
 **************************************************************************/
void CIrcdManager::ProcessAlreadyLogin()
{
	if(!SendCR())
	{
		// TO DO
		// �α��� â�� ������ �����Ѵ�.
	}
}

// Call This Every Second
void CIrcdManager::Timer()
{
	if(++m_nNSTATCnt > ITV_NSTAT)
		SendNSTAT();

	if(m_bKeepon)
	{
		if(++m_nReconCnt > ITV_RECONNECT) RetryConnect();
	}
}

void CIrcdManager::ProcessLoginFailed()
{
	::SendMessage(m_hMsgParent, WM_IRCDAUTHCOMPLETE, ERR_I_LOGINTRYOVER, 0);
}