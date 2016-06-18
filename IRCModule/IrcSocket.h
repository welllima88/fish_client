

//

#if !defined(__NETSOCKET_H)
#define __NETSOCKET_H

#define MAXBUF		1024
#define MAXDBUF		2048
#define MAXBLOCK	4096
#define TPACKETSIZE	404


enum PROCESS_STATE {
	STATE_CHECK_VERSION,
	STATE_CONNECT_IRCD,
	STATE_INIT_IRCD,
	STATE_PROCESS_MSG,
	STATE_CONNECTED_IRCD,
	STATE_UPGRADE_PROCESS,
	STATE_WM_CLOSE,
	STATE_ERROR_IRCD,
	STATE_UNKNOWN_ERROR,
	STATE_UNDEFINED
};

#define STR_IRCD						"ircd"

#define STR_IRCD_CONNECTOK				"IrcdConnectOK"
#define STR_IRCD_REJECTED				"IrcdConnectRejected"
#define STR_IRCD_PRIVMSG_FORMAT         "PRIVMSG %s :%s\n"
#define STR_IRCD_NOTICE_FORMAT          "NOTICE %s :%s\n"
#define STR_IRCD_EPRIVMSG_FORMAT		"EPRIVMSG %s :%s\n"
#define STR_IRCD_NICK_FORMAT            "NICK %s\n"
#define STR_IRCD_PASS_FORMAT            "PASS %s\n"
#define STR_IRCD_USER_FORMAT            "USER net pcname servername :%s\n"
#define STR_IRCD_SPACE_FORMAT           "SPNAMES CUR %s\n"
#define STR_IRCD_PONG_FORMAT            "PONG %s"
#define STR_IRCD_WHOIS_FORMAT           "WHOIS %s\n"
#define STR_IRCD_ISON_FORMAT            "ISON "
#define STR_IRCD_WHO_FORMAT             "WHO "
#define STR_IRCD_LIST_FORMAT            "LIST %s\n"
#define STR_IRCD_JOIN_FORMAT            "JOIN #%s %s\n"
#define STR_IRCD_PART_FORMAT            "PART #%s\n"
#define STR_IRCD_QUIT_FORMAT            "QUIT \n"
#define STR_IRCD_MODE_FORMAT            "MODE %s %s\n"
#define STR_IRCD_TOPIC_FORMAT           "TOPIC %s :%s\n"
#define STR_IRCD_USERIP_FORMAT			"USERIP %s\n"
#define STR_IRCD_NSTAT_FORMAT			"NSTAT %d %d\n"

#define STR_IRCD_PING                   "PING"
#define STR_IRCD_PRIVMSG                "PRIVMSG"
#define STR_IRCD_NOTICE                 "NOTICE"
#define STR_IRCD_JOIN                   "JOIN"
#define STR_IRCD_QUIT                   "QUIT"
#define STR_IRCD_PART                   "PART"
#define	STR_IRCD_KILL					"KILL"
#define STR_IRCD_NSTAT					"NSTAT"

#define NET_STR_IRCDPASS				"m_strPasswd"
#define MAXNICK							20

#define STR_WSAEINTR		_T("Interrupted system call")
#define STR_WSAEBADF		_T("Bad file number")
#define STR_WSAEACCES		_T("Permission denied")
#define STR_WSAEFAULT		_T("Bad address")
#define STR_WSAEINVAL		_T("Invalid argument")
#define STR_WSAEMFILE		_T("Too many open files")
#define STR_WSAEWOULDBLOCK	_T("Operation would block")
#define STR_WSAEINPROGRESS	_T("Operation now in progress")
#define STR_WSAEALREADY		_T("Operation already in progress")
#define STR_WSAENOTSOCK		_T("Socket operation on non-socket")
#define STR_WSAEDESTADDRREQ	_T("Destination address required")
#define STR_WSAEMSGSIZE		_T("Message too long")
#define STR_WSAEPROTOTYPE	_T("Protocol wrong type for socket")
#define STR_WSAENOPROTOOPT	_T("Protocol not available")
#define STR_WSAEPROTONOSUPPORT	_T("Protocol not supported")
#define STR_WSAESOCKTNOSUPPORT	_T("Socket type not supported")
#define STR_WSAEOPNOTSUPP	_T("Operation not supported on socket")
#define STR_WSAEPFNOSUPPORT	_T("Protocol family not supported")
#define STR_WSAEAFNOSUPPORT	_T("Address family not supported by protocol family")
#define STR_WSAEADDRINUSE	_T("Address already in use")
#define STR_WSAEADDRNOTAVAIL	_T("Can't assign requested address")
#define STR_WSAENETDOWN		_T("Network is down")
#define STR_WSAENETUNREACH	_T("Network is unreachable")
#define STR_WSAENETRESET	_T("Network dropped connection on reset")
#define STR_WSAECONNABORTED	_T("Software caused connection abort")
#define STR_WSAECONNRESET	_T("Connection reset by peer")
#define STR_WSAENOBUFS		_T("No buffer space available")
#define STR_WSAEISCONN		_T("Socket is already connected")
#define STR_WSAENOTCONN		_T("Socket is not connected")
#define STR_WSAESHUTDOWN	_T("Can't send after socket shutdown")
#define STR_WSAETOOMANYREFS	_T("Too many references: can't splice")
#define STR_WSAETIMEDOUT	_T("Connection timed out")
#define STR_WSAECONNREFUSED	_T("Connection refused")
#define STR_WSAELOOP		_T("Too many levels of symbolic links")
#define STR_WSAENAMETOOLONG	_T("File name too long")
#define STR_WSAEHOSTDOWN	_T("Host is down")
#define STR_WSAEHOSTUNREACH	_T("No route to host")
#define STR_WSAENOTEMPTY	_T("Directory not empty")
#define STR_WSAEPROCLIM		_T("Too many processes")
#define STR_WSAEUSERS		_T("Too many users")
#define STR_WSAEDQUOT		_T("Disc quota exceeded")
#define STR_WSAESTALE		_T("Stale NFS file handle")
#define STR_WSAEREMOTE		_T("Too many levels of remote in path")
#define STR_WSAEDISCON		_T("Disconnect")
#define STR_WSASYSNOTREADY	_T("Network sub-system is unusable")
#define STR_WSAVERNOTSUPPORTED	_T("WinSock DLL cannot support this application")
#define STR_WSANOTINITIALISED	_T("WinSock not initialized")
#define STR_WSAHOST_NOT_FOUND	_T("Host not found")
#define STR_WSATRY_AGAIN	_T("Non-authoritative host not found")
#define STR_WSANO_RECOVERY	_T("Non-recoverable error")
#define STR_WSANO_DATA		_T("Valid name, no data record of requested type")

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CIrcdSock command target
#include <afxsock.h>
#include "IrcdManager.h"
#define WM_IRCD_MSG			WM_USER+602

class CIrcdManager;
class CIrcdSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CIrcdSock();
	virtual ~CIrcdSock();

// Overrides
public:
	int Send(const void *lpBuf, int nBufLen, int nFlags = 0);
	int ReadBlock(LPSTR lpBlock, int nLength);
	BOOL IsError(int nError);
	BOOL IsConnect();
	void SetConnect(BOOL bTrue);
	void SetWnd(HWND hWnd, int nMsg);
	void SetManager( CIrcdManager* manager );
	BOOL IsError();
	void Close();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIrcdSock)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CIrcdSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
private:
	HWND m_Hwnd;
	int m_nMsg;
	BOOL m_bConnect;
	CObList m_listReceive;
	CIrcdManager*	m_pcManager;

public:
	//send
	void	Nick( char* nick );
	void	Passwd( void );
	void	User( char* id );
	bool	ResponsePONG( char* data);
	bool	Response388(char* data);
	void	SPNAMES( void );
	void	Join( char *channel, char *passwd );
	void	Part( char* channel );
	void    Privmsg ( const char *channelName, const char *msg );
	void	UserIP ( char* id );
	void	Notice( char* param1, char* param2 );
	void	Nstat( int postcnt );
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CSocketBlock

class CSocketBlock : public CObject
{
// Construction
public:
	CSocketBlock(char* pBlock, int nLength, BOOL bKeep = TRUE);
	virtual ~CSocketBlock();

// Attributes
public:
	operator int() { return m_nLength; }
	operator LPCSTR() { return (LPCSTR)m_pBlock; }

protected:
	char* 	m_pBlock;
	int	    m_nLength;
	BOOL	m_bKeep;

// Operations
public:
	void Keep(BOOL bKeep = TRUE) { m_bKeep = bKeep; }
	int RemoveHead(int nLength);

};

/////////////////////////////////////////////////////////////////////////////

#endif	//__NETSOCKET_H