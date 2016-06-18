#include "stdafx.h"
#include "../Fish.h"
#include "../fish_def.h"
#include "../fish_common.h"		// ansiconvert

#include "IrcSocket.h"
#include "Irc_utils.h"

/////////////////////////////////////////////////////////////////////////////
// CIrcdSock

extern CFISHApp theApp;

CIrcdSock::CIrcdSock()
{
	m_bConnect = FALSE;
	m_nMsg = 0;
	// Connect TimeOut 을 주려면 아래부분의 코멘트 처리를 지우면 된다.
	// m_dwTickCount = 0;
	m_pcManager = NULL;
}

CIrcdSock::~CIrcdSock()
{
	CSocketBlock *pBlock;
	while (!m_listReceive.IsEmpty())
	{
		pBlock = (CSocketBlock*) m_listReceive.RemoveHead();
//		pBlock->Keep(FALSE);
		delete pBlock;
	}

}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CIrcdSock, CAsyncSocket)
	//{{AFX_MSG_MAP(CIrcdSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CIrcdSock member functions

void CIrcdSock::SetWnd(HWND hWnd, int nMsg)
{
	m_Hwnd = hWnd;
	m_nMsg = nMsg;
}

void CIrcdSock::SetManager( CIrcdManager* manager )
{
	m_pcManager = manager;
}
void CIrcdSock::SetConnect(BOOL bTrue)
{
	m_bConnect = bTrue;
}

BOOL CIrcdSock::IsConnect()
{
	return m_bConnect;
}

void CIrcdSock::OnConnect(int nErrorCode) 
{
	if(IsError(nErrorCode))
	{
		m_pcManager->ResetConnection();
		return;
	}

	m_bConnect = TRUE;
	m_pcManager->OnIrcdMsg( m_hSocket, MAKELPARAM(FD_CONNECT, 0) );
	CAsyncSocket::OnConnect(nErrorCode);
}

void CIrcdSock::OnClose(int nErrorCode) 
{
	if(IsError(nErrorCode))
	{
	}

	m_bConnect = FALSE;
	m_pcManager->OnIrcdMsg( m_hSocket, MAKELPARAM(FD_CLOSE, 0) );
	CAsyncSocket::OnClose(nErrorCode);

	// 서버에 의해서 연결이 끊어지면 재연결 시도를 할 것인가?
	m_pcManager->ResetConnection(FALSE);
}

void CIrcdSock::OnReceive(int nErrorCode) 
{
	if(IsError(nErrorCode))	return;

	CAsyncSocket::OnReceive(nErrorCode);

	DWORD dwBytes;
	if (!IOCtl(FIONREAD, &dwBytes))
	{
		IsError();
		return;
	}

	char *pBlock = NULL;
	char szTmpBuf[MAXBLOCK];
	static char NotEnoughBuf[MAXBLOCK];	// MAXBLOCK = 4096
	static int  nBufIdx = 0;
	int nRequire, nRead,i;

	nRequire = dwBytes;

	while(dwBytes>0)
	{
		if(dwBytes>MAXBLOCK)
			nRequire = MAXBLOCK;
		else
			nRequire = dwBytes;

		if((nRead=Receive(szTmpBuf,nRequire)))
		{
			if(nRead!=SOCKET_ERROR)
			{
				for (i=0;i<nRead;i++,nBufIdx++)
				{
					NotEnoughBuf[nBufIdx] = szTmpBuf[i];
					if (szTmpBuf[i] == '\n')    // last of a Packet
					{
						m_listReceive.AddTail(new CSocketBlock(NotEnoughBuf, nBufIdx+1, FALSE));
						m_pcManager->OnIrcdMsg( m_hSocket, MAKELPARAM(FD_READ, nBufIdx+1) );
						nBufIdx = -1;
					}
				}
				dwBytes -= nRead;
				continue;
			}
		}
		if(IsError())
		{
			if( pBlock )
				delete pBlock;
			return;
		}
	}
}

void CIrcdSock::OnSend(int nErrorCode) 
{
	if(IsError(nErrorCode))	return;
	
	CAsyncSocket::OnSend(nErrorCode);
}

BOOL CIrcdSock::IsError()
{
	return IsError(WSAGetLastError());
}

BOOL CIrcdSock::IsError(int nError)
{
	switch (nError) 
	{
		case WSAENOBUFS:
		case WSAEWOULDBLOCK:
		case WSAEFAULT:
		case 0:
			return	FALSE;
	}

	// commented by moonknit 2005-05-26
//	PostMessage(m_Hwnd,m_nMsg,m_hSocket,MAKELPARAM(FD_OOB,nError));
	m_pcManager->OnIrcdMsg( m_hSocket, MAKELPARAM(FD_OOB, nError) );
	return	TRUE;	
}

int CIrcdSock::ReadBlock(LPSTR lpBlock, int nLength)
{
	ASSERT(lpBlock);
	ASSERT(0 < nLength);

	int	nRequire = nLength;
	nLength = 0;
	
	if (!m_bConnect)	return	nLength;
	
	if (m_listReceive.IsEmpty())	return	nLength;

	CSocketBlock* pBlock;
	int nRead;
	while (0 < nRequire && !m_listReceive.IsEmpty())
	{
		pBlock = (CSocketBlock*)m_listReceive.GetHead();
		nRead = min(nRequire, (int)*pBlock);
		memcpy(lpBlock, (LPCSTR)*pBlock, nRead);
		if (!pBlock->RemoveHead(nRead))
		{
//			pBlock->Keep(FALSE);
			delete	m_listReceive.RemoveHead();
		}
		nLength += nRead;
		nRequire -= nRead;
	}
	return nLength;
}

void CIrcdSock::Nick( char* nick )
{
	char buff[MAX_SMALLBUFF + 1];
	int length = sprintf ( buff, STR_IRCD_NICK_FORMAT, nick );

	Send ( buff, length );
}// end of Nick()

void CIrcdSock::Passwd( void )
{
	char buff[MAX_SMALLBUFF + 1];
	int length = sprintf ( buff, STR_IRCD_PASS_FORMAT, NET_STR_IRCDPASS);

	Send ( buff, length );
}//end of Passwd()

void CIrcdSock::User( char* id )
{
	char buff[MAX_SMALLBUFF+1];
	int length = sprintf( buff, "USER net pcname servername :%s;T;%s;%c;%d\r\n", id, "b1_001_A127_A145_06002_11", 'M', 20 );
	Send( buff, length );
}//end of User()


bool CIrcdSock::ResponsePONG( char* data )
{
	char szBuffer[MAXBUF];
	int nlen;
	
	if( !m_bConnect ) return false;

	nlen = sprintf(szBuffer, STR_IRCD_PONG_FORMAT, data);
	Send(szBuffer,nlen);

	return true;
}//end of ResponsePONG()

bool CIrcdSock::Response388( char* data )
{
	int i=0;
	char szNick[MAXNICK+1],*pNick;

	pNick = data;
	while (*pNick != '\r' && *pNick != '\n') 
	{
		szNick[i] = *pNick;
		i++; pNick++;
	}
	szNick[i] = '\0';
	return true;
}//end of Response388()

void CIrcdSock::SPNAMES()
{
	char buff[MAX_SMALLBUFF + 1];
	int length = sprintf( buff, "SPNAMES CUR ASSISTANT_ROOM\r\n" );
	Send( buff, length );

}//end of SPNAMES()



void CIrcdSock::Join( char* channel, char* passwd )
{
	char buff[MAX_BUFF+1];
	int length = sprintf ( buff, STR_IRCD_JOIN_FORMAT, channel, passwd );
	Send ( buff, length );
}//end of Join()

void CIrcdSock::Part( char* channel )
{
	char buff[MAX_BUFF+1];
	int length;
	length = sprintf( buff, STR_IRCD_PART_FORMAT, channel );
	Send( buff, length );
}//end of Part()

void CIrcdSock::Privmsg ( const char *channelName, const char *msg )
{
	if( NULL == channelName ) return ;
	char buff[MAX_BUFF+1];
	int length;
	length = sprintf ( buff, STR_IRCD_PRIVMSG_FORMAT, channelName, msg );
	Send ( buff, length );
}// end of Privmsg()

void CIrcdSock::UserIP( char* id )
{
	if( NULL == id ) return;
	char buff[MAX_BUFF+1];
	int length;
	length = sprintf( buff, STR_IRCD_USERIP_FORMAT, id );
	Send( buff, length );
}//end of UserIP()

void CIrcdSock::Notice(char* param1, char* param2 )
{
	if( param1 == NULL || param2==NULL ) return;
	char buff[MAX_BUFF+1];
	int length;
	length = sprintf( buff, STR_IRCD_NOTICE_FORMAT, param1, param2 );
	Send( buff, length );
}//end of Notice()

void CIrcdSock::Nstat( int postcnt )
{
	char buff[MAX_BUFF+1];
	int length;
	time_t cur;
	cur = time(NULL);
	length = sprintf( buff, STR_IRCD_NSTAT_FORMAT, postcnt, (int) cur );
	Send( buff, length );
}

void CIrcdSock::Close()
{
	ShutDown(2);
	CAsyncSocket::Close();

	m_bConnect = FALSE;
}

int CIrcdSock::Send(const void *lpBuf, int nBufLen, int nFlags)
{
#ifdef _UNICODE
	TCHAR wbuff[MAX_SMALLBUFF + 1];
	AnsiConvertMtoW((char*) lpBuf, wbuff, MAX_SMALLBUFF);
	TRACE(_T("To Ircd SendMessage : %s\r\n"), wbuff );
#else
	TRACE("To Ircd SendMessage : %s\r\n", lpBuf );
#endif
	return CAsyncSocket::Send(lpBuf, nBufLen, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
  

/////////////////////////////////////////////////////////////////////////////
// CSocketBlock

CSocketBlock::CSocketBlock(char *pBlock, int nLength, BOOL bKeep) 
{ 
	m_bKeep = bKeep;
	if (bKeep)
	{
		m_pBlock = pBlock;
		m_nLength = nLength; 
	}
	else
	{
		memcpy(m_pBlock = new char[nLength], pBlock, m_nLength = nLength); 
	}
}

CSocketBlock::~CSocketBlock() 
{ 
	if (!m_bKeep)
	{
		delete m_pBlock;
	}
}

int CSocketBlock::RemoveHead(int nLength)
{
	if (m_nLength <= nLength)
		m_nLength = 0;
	else
	{
		m_nLength -= nLength;
		char *p = new char[m_nLength];
		memcpy(p, m_pBlock + nLength, m_nLength);
		delete	m_pBlock;
		m_pBlock = p;
	}
	return	m_nLength;
}

/////////////////////////////////////////////////////////////////////////////