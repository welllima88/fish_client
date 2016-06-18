/**********************************************************************
** CLASS NAME : CIrcdManager 
** FILE  NAME : IrcdManager.h
** CLASS DESC : Ircd 소켓 / 메시지 관리 클래스
** by bemlove at 2002-10-08, VER : 1.0, COMMENTS: ORG 
**********************************************************************/

#if !defined(__IRCDMANAGER_H__)
#define __IRCDMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IrcdManager.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIrcdManager view

#include "../GnuModule/GnuDefines.h"
#include "IrcSocket.h"
#include "IRC_Command.h"
#include "IRC_Error.h"

class CIrcdSock;
class CIrcdManager //: public CView
{
private:
	BOOL			m_bTryLogin;
	BOOL			m_bRandomID;
	BOOL			m_bKeepon;
	HWND			m_hMsgParent;
	HWND			m_hSearchView;
	CIrcdSock*		m_pcIrcdSock;        // IRCD socket for General.
	TCHAR			m_szIP[MAX_IP+1];
	int				m_nAlreadyTry;
	int				m_iPort;
	int				m_iState;
	char			m_szTempID[MAX_USERID + 1];
	bool			m_bAlreadyLogin;
	char			m_szEventD[MAX_EVENTD+1];
	int				m_iLogicHandlerState;		// LH 와의 현재 상태
	int				m_iChannelCount;			// 개설되어 있는 채널의 수
	char			m_szChannelList[MAX_CHANNEL_COUNT][MAX_EVENTD+1];
	char			m_szLocalIP[MAX_IP+1];
	bool			m_bClosedByOther;

	int				m_nReconCnt;
	int				m_nNSTATCnt;

	// written by moonknit 2005-07-26
	int				m_nRetryConnect;

	// Date : 2004-03-04,	By bemlove
	char			m_szSeedIPManager[MAX_EVENTD+1];

// Operations
public:
	BOOL	IsKeepon()						{ return m_bKeepon; }
	void	SetRandomID(BOOL bFlag)			{ m_bRandomID = bFlag; }

	void	Timer();
	void	SetSearchViewHandle(HWND hWnd)	{ m_hSearchView = hWnd; }
	CIrcdManager(HWND hMsgParent);
	virtual ~CIrcdManager();
	void	ReleaseResource( void );
	bool	Initial(void);
	
	virtual inline void SetIP( TCHAR* ip )	{ if(!ip || _tcslen(ip) > sizeof(m_szIP)) return; _tcscpy( m_szIP, ip ); };
	virtual inline TCHAR* GetIP( void )		{ return m_szIP; };
	virtual inline void SetPort(int port)	{ m_iPort = port; };
	virtual inline int	GetPort( void)		{ return m_iPort; };

	// written by moonknit 2006-01-31
	void	SendNR();			// Node Register Packet
	void	SendNU();			// Node Update Packet

	void	ProcessAlreadyLogin();

	// written by moonknit 2005-07-26
	void	RetryConnect();
	void	ResetConnection(BOOL bReconnect = TRUE);

	bool	IsValidSock(void);
	void	OnIrcdReceive(SOCKET hSocket, int nLength);
	LONG	OnIrcdMsg( UINT wParam, LONG lParam );
	LONG	OnNetProcess( UINT wParam, LONG lParam );
	void	SetEventD( char* data );

	void	ReceiveJoin( char* data );
	void	ReceiveHK( void );

	void	SendNSTAT( void );

	void	SendAR( void );
	void	ReceiveAROK( void );
	void	ReceiveARNK( bool InvalidPasswd );

	void	SendCP( int hostPort, int gnuPort );

	bool	AlreadyLogin( void );
	bool	SendCR( void );
	void	ReceiveCROK( void );
	void	ReceiveCRNK( bool InvalidPasswd );

	void	ReplySpace( void );
	void	ReplyNamesList353( char* data );
	void	ReplyEndOfNames366( char* data );
	void	ReplyUserIP307( char* data );

	void	Receive303( char* data );

	void	SetHostIPList( void );

	void	ReceiveGN( int type, char* data, char* msg );

	void	ReceiveNoEventD494( void );
	void	ReceiveNoSuchNick401( char* data );

protected:
	void	ProcessLoginFailed();
	void	ParsePacket( char *szPacket );
	void	ParsePrivmsg( char* prefix, char* data );
	void	ParseNotice( char* prefix,  char *data );

	// buddy list 관리 부분 추가 필요
	// 최초 버디 그룹 / 버디 리스트 추가 
public:
	void	ReqForceRemove( char* uid, int type );
	void	SendFRNotice( char* uid, int type );

	void	NetworkDisconnected( int type = TIMER_NETWORK_ERROR );
private:
	bool	m_bReconnecting;
public:// 네트웍 이상에 의해서 재연결을 시도하였을 경우에만 값이 true 로 설정된다.
	inline	void SetReconnecting( bool reconnect )		{ m_bReconnecting = reconnect; };
	inline	bool GetReconnecting( void )				{ return m_bReconnecting ; };

	// 로그인 종류에 따라 로그인을 시도한다.
	bool	Login();
};

/////////////////////////////////////////////////////////////////////////////


#endif // !defined(__IRCDMANAGER_H__)
