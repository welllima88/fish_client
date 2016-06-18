/********************************************************************************

	Gnucleus - A node application for the gnutella network
    Copyright (C) 2001 John Marshall

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	For support, questions, comments, etc...
	E-Mail: 
		swabby@c0re.net
	
	Address:
		21 Cadogan Way
		Nashua, NH, USA 03062 

********************************************************************************/


#include "stdafx.h"
#include "../FISH.H"
#include "../MainFrm.h"

#include "GnuRouting.h"
#include "GnuLocal.h"
#include "GnuSock.h"
#include "GnuNode.h"
#include "GnuUploadShell.h"
#include "GnuDownloadShell.h"
#include "GnuDownload.h"
#include "GnuControl.h"

#include "GnuDoc.h"

#include "../LocalProperties.h"
#include "../fish_common.h"

// ��Ʈ ����
#include "../ChgPortDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FORCE_ULTRAPEER		false
#define VALID_CONNTIME		1000 * 60 * 10			// ten minutes
#define MIN_EFFICIENCY		80						// 50 %
#define MAX_ATTEMPT			20

#define STR_ERR_GC_PORTCOMPLICT			_T("GnuCleus �⵿ �� ��Ʈ �浹�� �߻��߽��ϴ�!!") \
										_T("��Ʈ ��ȣ�� �ڵ� �����Ͽ� �ٽ� �õ��մϴ�.\r\n")
#define STR_ERR_NETDOWN					_T("���� ��Ʈ��ũ�� ����� �� �����ϴ�.")
#define STR_ERR_DEFAULT					_T("Gnu Socket ������ �����Ͽ����ϴ�. ���� ���� �ڵ�� %d �Դϴ�.\r\n")

extern CFISHApp theApp;

CGnuControl::CGnuControl()
{
	m_CurrentPort		= 7878;
	m_ClientUptime		= CTime::GetCurrentTime();

	
	m_LANSock = new CGnuLocal(this);
	m_LANSock->Init();

	m_DownloadList.clear();

	/// SuperNodes
	if(FORCE_ULTRAPEER)
	{
		m_NodeCapability  = CLIENT_ULTRAPEER;		
		m_ForcedSupernode = true;
	}
	else
	{

		m_NodeCapability	= CLIENT_LEAF;
		m_ForcedSupernode	= false;
	}

	m_LeafModeActive = false;
	
	m_BackupTicket = false;

	m_NormalConnectsApprox = 0;

	
	// Pong caching
	m_FlushPongCache_time    = GetTickCount() + TIMEOUT_PONG_CACHE;
	m_FlushPongCacheOld_time = GetTickCount() + TIMEOUT_PONG_CACHE * 3; 
	m_stlPongCache.clear();
	m_stlPongCache.reserve(10);

	// Bandwidth
	m_NetSecBytesDown	= 0;
	m_NetSecBytesUp		= 0;

	m_DownloadSecBytes	= 0;
	m_UploadSecBytes	= 0;

	m_ExtPongBytes		= 0;
	m_UltraPongBytes	= 0;
	m_SecCount			= 0;


	// Mapping - Adding ourselves
	MapTable.clear();
	MapTable.reserve(100);
	MapNode Localhost;
	Localhost.MapID		  = 0;

	Localhost.Port        = m_CurrentPort;
	MapTable.push_back(Localhost);

	// init std
	m_CancelUploadList.clear();
	m_toggleGnuSeed = false;
	m_nPongCount	= 0;

	// init Node List
	m_NodeList.clear();
	m_NodeList.reserve(10);

	m_DLNodeList.clear();

	m_stlActiveDownloadHost.clear();

	for( int i=0; i<MAX_OTHER_HOST_COUNT; i++ )
	{
		m_SendOtherInfo[i].S_addr = StrtoIP(_T("127.0.0.1")).S_addr;
		m_ReceiveOtherInfo[i].S_addr = StrtoIP(_T("127.0.0.1")).S_addr;
	}
	m_nCurSendOtherInfo = 0;
	m_nCurReceiveOtherInfo = 0;

	// written by moonknit 2005-05-13
	m_nNodeManage2 = m_nNodeManage1 = 0;

	// written by moonknit 2005-06-22
	m_nLastDLCheck	= 0;
	m_nNormalCnt	= 0;

	m_bTurboNode	= TRUE;

	m_SearchList.clear();
	m_nSearchManage = 0;
	
	m_nDLTryingCnt	= 0;
	m_nCurDLCnt		= 0;

	bReleased		= FALSE;

}

CGnuControl::~CGnuControl()
{
	GnuConRelease();
}

void CGnuControl::GnuConRelease()
{
	if(bReleased) return;

	if(m_hSocket != INVALID_SOCKET)
		AsyncSelect(0);

	TRACE0("*** CGnuControl Deconstructing\n");

	CGnuNode *deadNode = NULL;


	// delete UDP socket
	delete m_LANSock;

	// Destroy Sock List
	while( m_SockList.size() )
	{
		delete m_SockList.back();
		m_SockList.pop_back();
	}


	// Destroy Node List
	m_NodeAccess.Lock();

	while( m_NodeList.size() )
	{
		delete m_NodeList.back();
		m_NodeList.pop_back();
	}

	m_NodeAccess.Unlock();
	

	// Destroy Upload List
	m_UploadAccess.Lock();

	while( m_UploadList.size() )
	{
		delete m_UploadList.back();
		m_UploadList.pop_back();
	}

	m_UploadAccess.Unlock();

	// added by moonknit
	// ���� ���� �����带 �����Ų��.
#ifdef USE_MERGETHREAD
	if(CGnuDownloadShell::IsMergeThread())
	{
		CGnuDownloadShell::SetStopThread(true);
		CGnuDownloadShell::TriggerEvent();
		Sleep(1000);
	}

	CGnuDownloadShell::FreeMergeItem();
#endif
	// -- added

	// Destroy Download List	
	m_DownloadAccess.Lock();
		
	while( m_DownloadList.size() )
	{
		delete m_DownloadList.back();
		m_DownloadList.pop_back();
	}

	m_DownloadAccess.Unlock();

	// Active Download Host List
	m_ActiveDownloadAccess.Lock();
	m_stlActiveDownloadHost.clear();
	m_ActiveDownloadAccess.Unlock();

	// written by moonknit 2005-07-05
	// Destroy Search List
	while( m_SearchList.size() )
	{
		delete m_SearchList.back();
		m_SearchList.pop_back();
	}

	bReleased	= TRUE;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuControl, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuControl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


/////////////////////////////////////////////////////////////////////////////
// LISTEN CONTROL

static BOOL bListenWork = FALSE;

bool CGnuControl::StartListening(BOOL bSilence)
{
	UINT AttemptPort = 0;

	bool Success  = false;
	int	 Attempts = 0;

	if(bListenWork) return false;
	bListenWork = TRUE;

	AttemptPort = theApp.m_spGD->GetGnuHostPort();

	StopListening();

	if(AttemptPort > MAX_GNUPORT || AttemptPort < MIN_GNUPORT )
		AttemptPort = DEFAULT_GNUPORT;

	// written by moonknit 2005-06-17
	m_nStartTime = GetTickCount();

	while(!Create(AttemptPort))
	{
		// ��Ʈ�� ������ ������ 
		CAsyncSocket::Close();
		if ( GetLastError() == WSAEADDRINUSE )
		{
			++Attempts;
			if(Attempts > MAX_ATTEMPT)
				AttemptPort = 0;			// �ڵ� ��Ʈ ���� ���� ��
			else
			{
				++AttemptPort;

				if(AttemptPort > MAX_GNUPORT)
					AttemptPort = DEFAULT_GNUPORT;
			}

			if(!bSilence 
				&& Attempts == 1			// ù���� �õ��� ��쿡�� �޽����� ����.
				) FishMessageBox(STR_ERR_GC_PORTCOMPLICT);
		}
		else
		{
			switch(GetLastError())
			{
			case WSAENETDOWN:
				if(!bSilence) FishMessageBox(STR_ERR_NETDOWN);
				break;
			default:
				CString msg;
				msg.Format(STR_ERR_DEFAULT, GetLastError());
				if(!bSilence) FishMessageBox(msg);
				break;

			}
			bListenWork = FALSE;
			return false;
		}
	}

//	CString stmp;
//	stmp.Format(_T("current port number[%d]\r\n"), AttemptPort);
//	AfxMessageBox(stmp);

	if(Attempts > 0)
	{
		theApp.m_spGD->SetGnuHostPort(AttemptPort);
		theApp.m_spGD->SaveProperties();
	}

	bListenWork = FALSE;

	if(Listen(30))
	{
		Success = true;
		m_CurrentPort = AttemptPort;
		return true;
	}

	return false;
}

void CGnuControl::StopListening()
{
	// Close socket
	ShutDown(2);

	Close();

	m_NodeAccess.Lock();
	std::vector<CGnuNode*>::iterator itNode;
	for(itNode = m_NodeList.begin(); itNode != m_NodeList.end(); itNode++)
		(*itNode)->Close();
	m_NodeAccess.Unlock();

	NodeMessage(SOCK_UPDATE, NULL);

}

BOOL CGnuControl::CheckSockList(CGnuSock* pSock)
{
	// Incoming Sockets
	std::vector<CGnuSock*>::iterator itSock;

	// for test
	int nCnt = m_SockList.size();

	for(itSock = m_SockList.begin(); itSock != m_SockList.end(); itSock++)
	{
		if(pSock->m_Host == (*itSock)->m_Host)
			return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAsyncSocket OVERRIDES
void CGnuControl::OnAccept(int nErrorCode) 
{
	if(nErrorCode || theApp.IsClosing())
	{
		return;
	}

	CGnuSock* Incoming = new CGnuSock(this);
	
	int Safe = Accept(*Incoming);
	if(Safe)
	{
		// written by moonknit 2005-08-17
		// Get Connected's info
		Incoming->GetPeerName(Incoming->m_Host, Incoming->m_nPort);

		m_SockList.push_back(Incoming);
	}
	else
	{
		delete Incoming;
	}
}


/////////////////////////////////////////////////////////////////////////////
// TRAFFIC CONTROL

void CGnuControl::Broadcast_Ping(packet_Ping* Ping, int length, CGnuNode* exception)
{
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p != exception && p->m_Status == SOCK_CONNECTED )
			p->SendPacket(Ping, length, PACKET_PING, false);
	}
	m_NodeAccess.Unlock();
}

void CGnuControl::Broadcast_Query(packet_Query* Query, int length, CGnuNode* exception)
{
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p != exception && p->m_Status == SOCK_CONNECTED)
		{
			p->SendPacket(Query, length, PACKET_QUERY, false);
		}
	}
	m_NodeAccess.Unlock();
}

// written by		: moonknit
// name				: CGnuControl::SendResults
// created			: 2005-05-16
// last updated		: 2005-05-17
// desc				:
// �˻������ Route ��Ų��.
void CGnuControl::SendResults(SE_QUERY& query, auto_ptr<SITEM_LIST>& resultlist, int nsearchtype)
{
	byte QueryReply[65536];
#ifdef USE_UTFCONVERT
	byte QueryTemp[4096];
	byte* QueryTempNext		= NULL;
	DWORD QueryTempLength	= 0;
#endif
	byte* QueryReplyNext	= NULL;
	DWORD QueryReplyLength	= 0;
	UINT  TotalReplyCount	= 0;
	byte  ReplyCount		= 0;
	// commented by moonknit 2005-05-17
	// �˻������ �ִ� n���� ������ �� �ֵ��� �����Ѵ�.
	int   MaxReplies		= 0;	
	CString MatchStr;
	int nCount				= 0;
	bool bOverSize			= false;

	QueryReplyNext			= &QueryReply[34];
	QueryReplyLength		= 0;
	TotalReplyCount			= 0;
	ReplyCount				= 0;

	if(resultlist.get() == 0) return;

	SITEM_LIST::iterator it;
	for(it = resultlist->begin(); it != resultlist->end(); ++it)
	{
		if((*it).t.GetLength() == 0) continue;

		if(MaxReplies && MaxReplies <= TotalReplyCount)
			break;

		// making flag
		byte flag[4];			// second byte is reserved
		int nflagsize = sizeof(flag);
		memset(&flag, 0, nflagsize);
		// Search Type
		flag[0] |= SIFLAG_POST;			// �˻� ����� POST �˻��� ������� �˷��ش�.

		// User Data
		if((*it).bread) flag[0] |= SIFLAG_READ;
		if((*it).breadon) flag[0] |= SIFLAG_READON;
		if((*it).bscrap) flag[0] |= SIFLAG_SCRAP;
		if((*it).bfilter) flag[0] |= SIFLAG_FILTER;
		memcpy(QueryReplyNext, &flag, nflagsize);
		QueryReplyNext   += nflagsize;
		QueryReplyLength += nflagsize;

		// pub date 4����Ʈ
		* (time_t*) QueryReplyNext = (*it).pubdate;
		QueryReplyNext   += 4;
		QueryReplyLength += 4;

		if(nsearchtype != GRAPH_SEARCH)
		{
			// �ε��� 4����Ʈ
			* (int*) QueryReplyNext = (*it).id;
			QueryReplyNext   += 4;
			QueryReplyLength += 4;

			// �ؽ�Ʈ ����
#ifdef USE_UTFCONVERT
			// SWAP Temporary Data and Transfer Buffer
			QueryTempNext = QueryReplyNext;
			QueryTempLength = QueryReplyLength;

			QueryReplyNext = &QueryTemp[0];
			QueryReplyLength = 0;
#endif
			_tcscpy ((TCHAR*) QueryReplyNext, (LPCTSTR) (*it).sguid);

			QueryReplyNext   += ((*it).sguid.GetLength() + 1) * sizeof(TCHAR);
			QueryReplyLength += ((*it).sguid.GetLength() + 1) * sizeof(TCHAR);

			if((*it).t.GetLength() > 0)
			{
				_tcscpy ((TCHAR*) QueryReplyNext, URN_TITLE);
				QueryReplyNext   += 6 * sizeof(TCHAR);
				QueryReplyLength += 6 * sizeof(TCHAR);

				_tcscpy ((TCHAR*) QueryReplyNext, (LPCTSTR) (*it).t);
				QueryReplyNext   += (*it).t.GetLength() * sizeof(TCHAR);
				QueryReplyLength += (*it).t.GetLength() * sizeof(TCHAR);
			}

			if((*it).link.GetLength() > 0)
			{
				*QueryReplyNext	= _T('\x1c');
				QueryReplyNext   += sizeof(TCHAR);
				QueryReplyLength += sizeof(TCHAR);

				_tcscpy ((TCHAR*) QueryReplyNext, URN_LINK);
				QueryReplyNext   += 6 * sizeof(TCHAR);
				QueryReplyLength += 6 * sizeof(TCHAR);

				_tcscpy ((TCHAR*) QueryReplyNext, (LPCTSTR) (*it).link);
				QueryReplyNext   += (*it).link.GetLength() * sizeof(TCHAR);
				QueryReplyLength += (*it).link.GetLength() * sizeof(TCHAR);
			}

			// ���� �߰��� Delimiter�� ������ ����.
			//  memcpy(*QueryReplyNext, _T("\x1c"), sizeof(TCHAR));
			//	QueryReplyNext   += sizeof(TCHAR);
			//	QueryReplyLength += sizeof(TCHAR);
		}

		{
			*QueryReplyNext = _T('\0');
			QueryReplyNext	+= sizeof(TCHAR);
			QueryReplyLength+= sizeof(TCHAR);
		}
#ifdef USE_UTFCONVERT
		// re-SWAP Temporary Data and Transfer Buffer

		// write utf8 header to buffer
//		memcpy(QueryTempNext, utf8header, 3);
//		QueryTempNext+=3;

		// convert temp text to UTF8
		int u8len = WideCharToMultiByte(CP_UTF8
			, NULL
			, (TCHAR*) QueryTemp, QueryTempLength
			, (char*) QueryTempNext, 4096
			, NULL, NULL);

		QueryReplyNext = QueryTempNext;
		QueryReplyLength = QueryTempLength;

		QueryReplyNext	+= u8len;
		QueryReplyLength += u8len;
#endif
		ReplyCount++;

		if(QueryReplyLength > 4096 || ReplyCount == 255)
		{
			SendResultsPacket(query, QueryReply, QueryReplyLength, ReplyCount, NULL);

			QueryReplyNext	 = &QueryReply[34];
			QueryReplyLength = 0;
			ReplyCount		 = 0;
		}		
	}

	if(ReplyCount > 0)
	{
		SendResultsPacket(query, QueryReply, QueryReplyLength, ReplyCount, NULL);
	}
}
// end CGnuControl::SendResults

// written by		: moonknit
// name				: CGnuControl::SendResultsPacket
// created			: 2005-05-17
// last updated		: 2005-05-17
// desc				:
// QueryHit ��Ŷ�� �ش� Node�� �����ϵ��� �Ѵ�.
void CGnuControl::SendResultsPacket(SE_QUERY& query, byte* pQueryHit, DWORD ReplyLength, byte ReplyCount, RemoteIndex* pRemote)
{

	// FilesAccess must be unlocked before calling this
	//////////////////////////////////////////////////////////////////
	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];
		if(p == query.Origin && p->m_Status == SOCK_CONNECTED)
		{
			//TRACE("Send QueryHit : ip=%s\t SearchQuery = %s\n", p->m_HostIP, query.Origin->m_HostIP );
			p->Send_QueryHit(query, pQueryHit, ReplyLength, ReplyCount, pRemote);
 			break;
		}
	}

	m_NodeAccess.Unlock();
}

// PIPE : Node 1

/***************************************************************************
 * CGnuControl::SendQuery

 * written by		: moonknit
 * name				: CGnuControl::SendQuery
 * created			: 2005-05-16
 * last updated		: 2005-05-16
					2005-12-29 :: query parameter changed, from Enppy 3.0 data type to FISH 1.0 data type
					by moonknit

 * desc				:
 * �˻���û�� �����Ѵ�.

 * parameters		:
 * (in CString) sword - �˻���
 * (in COleDateTime) from - �˻� ���۽ð�
 * (in int) span - �˻� �Ⱓ
 * (in int) spantype - �˻� �Ⱓ�� ���� ( 0 : ��, 1 : �ð�)
 * (in int) maxcnt - �ִ� �˻� ����
 ***************************************************************************/
GUID CGnuControl::SendQuery(CString sword, int type
							, COleDateTime from, int span /* =0 */, int spantype /* =0 */, int maxcnt /* =0 */)
{
	CString m_TempSearch = _T("");
	byte	packet[1024];

	// �˻���
	m_TempSearch += sword;							// �˻��� �Ǵ� RFID
	// ��û�� ID
	m_TempSearch += GNU_DELIMITER;
	m_TempSearch += theApp.m_spLP->GetUserID();		// ��û�� ID

	// �˻� ����
	m_TempSearch += GNU_DELIMITER;
	m_TempSearch += DWrdtoStr(type);
	// �ִ� �˻� ����
	m_TempSearch += GNU_DELIMITER;
	m_TempSearch += DWrdtoStr(maxcnt);


	// �˻� �Ⱓ
	if(from.m_dt != 0 && span > 0)
	{
		CString sspantime;
		TCHAR termunit;
		if(spantype == 0)
			termunit = _T('D');
		else
			termunit = _T('H');

		sspantime = from.Format(_T("%Y%m%d%H"));
		sspantime += _T('_');
		sspantime += termunit;
		m_TempSearch += GNU_DELIMITER;
		m_TempSearch += sspantime;
	}

	/////////////////////////////////////////////////////////////////////////

	TRACE(_T("Net Search Query: [%s]\r\n"), m_TempSearch);

//	memset(packet , 0 , 255);
	int QueryLen = 0;

	QueryLen = m_TempSearch.GetLength() * sizeof(TCHAR);

	memcpy(packet + 25, m_TempSearch , QueryLen);
	memcpy(packet + 25 + QueryLen, _T("\0"), sizeof(TCHAR));
	QueryLen += sizeof(TCHAR);
	// RFID�˻��� GnuDownloadShell�� �̸� �����ϰ� �õ尡 ������ ���� ����ϵ��� ó���Ѵ�.

	return Broadcast_LocalQuery(packet, 25 + QueryLen);
}
// -- end CGnuControl::SendQuery

GUID CGnuControl::Broadcast_LocalQuery(byte* Packet, int length)
{
	// See if any downloads have the same keywords and use same guid
	int i;

	GUID Guid = GUID_NULL;

	GnuCreateGuid(&Guid);

	if (Guid == GUID_NULL)
	{
		return Guid;
	}
	int connectedNodeCount = 0;
	bool bSendQuery = false;

	// Send Query
	m_NodeAccess.Lock();
	for(i = 0; i < m_NodeList.size(); ++i)	
	{
		packet_Query* Query = (packet_Query*) Packet;
	
		Query->Header.Guid		= Guid;
		Query->Header.Function	= 0x80;
		Query->Header.Hops		= 0;
		Query->Header.TTL		= MAX_TTL;
		Query->Header.Payload	= length - 23;
		Query->Speed			= 0;

		bSendQuery = true;
	
		CGnuNode *p = m_NodeList[i];
	
		if(p->m_Status == SOCK_CONNECTED)
		{
			connectedNodeCount++;
//			char* ExQuery = ((char*) Packet) + 25;
			p->SendPacket(Packet, length, PACKET_QUERY, true);
		}
	}
	m_NodeAccess.Unlock();

	if(bSendQuery)
	{
		m_TableLocal.Insert(&Guid, NULL);
	}
		
	return Guid;
}

// ����� ��
void CGnuControl::Route_Pong(packet_Pong* Pong, int length, key_Value* key)
{
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];
		if(p == key->Origin && p->m_Status == SOCK_CONNECTED)
			p->SendPacket(Pong, length, PACKET_PONG, false);
	}
	m_NodeAccess.Unlock();
}



void CGnuControl::Route_QueryHit(packet_QueryHit* QueryHit, DWORD length, key_Value* key)
{
	DWORD BytestoRead = length;

	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p == key->Origin && p->m_Status == SOCK_CONNECTED)
			p->SendPacket(QueryHit, length, PACKET_QUERYHIT, false);
	}
	m_NodeAccess.Unlock();
}

void CGnuControl::Route_Push(packet_Push* Push, int length, key_Value* key)
{
	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p == key->Origin && p->m_Status == SOCK_CONNECTED)
			p->SendPacket(Push, length, PACKET_PUSH, false);
	}

	m_NodeAccess.Unlock();
}

void CGnuControl::Route_LocalPush(Result Download)
{
	GUID Guid = GUID_NULL;
	GnuCreateGuid(&Guid);
	if (Guid == GUID_NULL)
	{
		return;
	}

	// Create packet
	packet_Push Push;

	Push.Header.Guid		= Guid;
	Push.Header.Function	= 0x40;
	Push.Header.TTL			= Download.Distance;
	Push.Header.Hops		= 0;
	Push.Header.Payload		= 26;
	Push.ServerID			= Download.PushID;
	Push.Index				= Download.FileIndex;
	Push.Host				= theApp.m_spGD->m_ForcedHost.S_addr ? theApp.m_spGD->m_ForcedHost : theApp.m_spGD->m_LocalHost;
	Push.Port				= m_CurrentPort;

	// Send Push
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p == Download.Origin && p->m_Status == SOCK_CONNECTED)
		{
			m_TableLocal.Insert(&Guid, p);

			p->SendPacket(&Push, 49, PACKET_PUSH, true);
		}
	}
	m_NodeAccess.Unlock();
}

////////////////////////////////////////////////////////////////////////////
// Node control

/*
 * <CGnuControl::AddNode>
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-28
 *
 * @Description
 * ���ο� ȣ��Ʈ ������ Node List�� �߰��Ѵ�.
 * 
 * @Parameters
 * (in CString) Host - �� ȣ��Ʈ�� ip ����
 * (in UINT) Port - �� ȣ��Ʈ�� ��Ʈ ����
 * (in UINT) curcnt - ���� out-node�� ����
 *
 * @return
 * (void)
 *
 */
void CGnuControl::AddNode(CString Host, UINT Port, UINT curcnt)
{
	// �ִ� OUT - ����� �� �̸��� ��쿡�� ���� ��带 �����Ѵ�.
	CAutoLock al(&m_NodeAccess);
	int nMax = theApp.m_spGD->GetMaxOutConnection();
	if( nMax != 0
		&& curcnt >= nMax )
	{
		return;
	}

	if(FindNode(Host, Port) != NULL)
		return;

	CGnuNode* Node = new CGnuNode(this, Host, Port);

	//Attempt to connect to node
	if(!Node->Create())
	{
		delete Node;
		return;
	}

//	TRACE(_T("IP : %s, Port : %d\r\n"), Host, Port);
	
	if( !Node->Connect(Host, Port) )
	{
		if (Node->GetLastError() != WSAEWOULDBLOCK)
		{
			delete Node;
			return;
		}
		else {
			// 1 = WASEWOULDBLOCK
			Node->m_NodeStatus = 1;
		}
	}

	// Add node to list
	if( m_NodeList.size() >= 10 )
		m_NodeList.reserve( m_NodeList.capacity() + 1 );
	m_NodeList.push_back(Node);

	NodeMessage(SOCK_UPDATE, NULL);
}

void CGnuControl::RemoveNode(CGnuNode* pNode)
{
	m_NodeAccess.Lock();
	std::vector<CGnuNode*>::iterator itNode;
	for(itNode = m_NodeList.begin(); itNode != m_NodeList.end(); itNode++)
		if(*itNode == pNode)
			pNode->Close();
	m_NodeAccess.Unlock();

	NodeMessage(SOCK_UPDATE, NULL);
}

CGnuNode* CGnuControl::FindNode(CString Host, UINT Port)
{
	CGnuNode* pNode = NULL;
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)
		if(m_NodeList[i]->m_HostIP == Host)
		{
			pNode = m_NodeList[i];
			break;
		}
	m_NodeAccess.Unlock();

	return pNode;
}

BOOL CGnuControl::CheckNode(CGnuNode* pNode)
{
	if(!pNode) return FALSE;

	BOOL bRet = FALSE;
	
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		if(pNode == m_NodeList[i])
		{
			bRet = TRUE;
			break;
		}
	}
	m_NodeAccess.Unlock();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// TRANSFER CONTROL

int CGnuControl::CountNormalConnects(int ntype)
{
	int NormalConnects = 0;

	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		switch(ntype)
		{
		case NT_INBOUND:
			if(!p->m_Inbound) continue;
			break;
		case NT_OUTBOUND:
			if(p->m_Inbound) continue;
			break;
		}

		if(p->m_Status == SOCK_CONNECTED)
		{
			if(m_NodeCapability == CLIENT_ULTRAPEER && !m_LeafModeActive)
			{
				if(p->m_HostMode != CLIENT_LEAF)
					NormalConnects++;
			}
			else
				NormalConnects++;
		}
	}
	m_NodeAccess.Unlock();

	// removed by moonknit 2005-12-22
	//m_NormalConnectsApprox = NormalConnects;

	return NormalConnects;
}

int CGnuControl::CountSuperConnects()
{
	int SuperConnects = 0;

	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)	
	{
		CGnuNode *p = m_NodeList[i];

		if(p->m_Status == SOCK_CONNECTED)
			if(p->m_HostMode == CLIENT_ULTRAPEER)
				SuperConnects++;
	}

	m_NodeAccess.Unlock();

	return SuperConnects;
}

int CGnuControl::CountLeafConnects()
{
	int LeafConnects = 0;

	m_NodeAccess.Lock();

	if(!m_LeafModeActive && m_NodeCapability == CLIENT_ULTRAPEER)
		for(int i = 0; i < m_NodeList.size(); i++)	
		{
			CGnuNode *p = m_NodeList[i];

			if(p->m_Status == SOCK_CONNECTED)			
				if(p->m_HostMode == CLIENT_LEAF)
					LeafConnects++;
		}

	m_NodeAccess.Unlock();

	return LeafConnects;
}

int CGnuControl::CountUploading()
// Use by thread so we are careful
{
	int NumUploading = 0;

	m_UploadAccess.Lock();

	std::list<CGnuUploadShell*>::iterator it;

	for(it = m_UploadList.begin(); it != m_UploadList.end(); ++it)
	{
		CGnuUploadShell *p = (CGnuUploadShell*) (*it);

		if(p->m_Status == TRANSFER_SENDING)
			NumUploading++;
	}

	m_UploadAccess.Unlock();

	return NumUploading;
}

int CGnuControl::CountDownloading()
{
	int NumDownloading = 0;

	m_DownloadAccess.Lock();

	std::list<CGnuDownloadShell*>::iterator it;

	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)
	{
		CGnuDownloadShell *p = (CGnuDownloadShell*) (*it);

		if(p->IsDownloading())
		{
			NumDownloading++;
		}
	}

	m_DownloadAccess.Unlock();

	m_nCurDLCnt = NumDownloading;

	return NumDownloading;
}


/////////////////////////////////////////////////////////////////////////////
// This is where the Network side of MyNapster talks with the Interface side

void CGnuControl::NodeMessage(UINT dwMessID, WPARAM packet)
{
    switch(dwMessID)
	{
		// ���� �ϴ��� ��� ���� ǥ�ø� �����ϴ� �޽���
	case WM_NODEUPDATE:
		{
			int nCnt = CountNormalConnects(NT_ALL);
			UINT nType = (UINT) packet;
			UINT nMessage = 0;

			BOOL bUpdate = FALSE;
			BOOL bEnable = FALSE;

			if(nType == 0) // only node cnt update
			{
				if(m_nNormalCnt == nCnt) return;

				int bOld = m_nNormalCnt;

				m_nNormalCnt = nCnt;
				if(nCnt == 0)
				{
					// search disabled
					bEnable = FALSE;
				}
				else if(nCnt == 1 && bOld == 0)
				{
					// search enabled
					bEnable = TRUE;
				}
				
				bUpdate = TRUE;

				if(m_nNormalCnt == 3 && m_bTurboNode)
				{
					m_bTurboNode = FALSE;
				}
			}
			else
				bUpdate = TRUE;

			if(bUpdate)
			{
				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd) pWnd->SendMessage(WM_NODEUPDATE, m_nNormalCnt, bEnable);
			}
		}
		break;
	case SOCK_UPDATE:
		break;
	case PACKET_INCOMING:
		if(((packet_Log*)packet)->Header->Function == 0x81) // ���� �˻��� ���� QueryHit�� ���� ���..
		{
		}
		break;
	}	
}

void CGnuControl::TransferMessage(UINT dwMessID, WPARAM pTransfer, LPARAM lReserved)
{
	// TO DO
	// �޽��� ������ �ʿ��� ��� �̰��� �̿��Ͽ� �����Ѵ�.
}

// 1�ʸ��� ȣ�� �ǵ��� �Ѵ�.
void CGnuControl::Timer()
{
	int i = 0;

	// Ping pong scheme
	if (m_FlushPongCache_time <= GetTickCount())
	{
		FlushPongCache();
		m_FlushPongCache_time = GetTickCount() + TIMEOUT_PONG_CACHE;
	}
	

	// Clear old ping pong cache
	if (m_FlushPongCacheOld_time <= GetTickCount())
	{
		for(i = 0; i < MAX_TTL - 1; i++)
			m_PongCache_Old[i].clear();

		m_FlushPongCacheOld_time = GetTickCount() + TIMEOUT_PONG_CACHE * 3;
	}

	// Clear backup ticket, used to control amount of backups each second
	if(m_BackupTicket)
		m_BackupTicket = false;

//	NodeMessage(WM_NODEUPDATE, 1);
	CleanDeadSocks();

	// written by moonknit 2005-05-13
	// 1�� ����.
	ManageDownloads();
	ManageUploads();

	// 3�� ����.
	m_nNodeManage2++;
	m_nNodeManage1++;

	if(m_bTurboNode)
	{
		m_nNodeManage2 = -1;
	}

	if(m_nNodeManage2 > 3 || m_nNodeManage2 < 0)
	{
		m_nNodeManage2 = 0;
		ManageNodes2();
	}

	// 90�� ����.
	if(m_nNodeManage1 > 90 || m_nNodeManage1 < 0)
	{
		m_nNodeManage1 = 0;
		ManageNodes();
	}
	// -- end

	m_nSearchManage++;
	if(m_nSearchManage > 5 || m_nSearchManage < 0)
	{
		m_nSearchManage = 0;
		ManageSearch();
	}
}

// �ֱ������� ����ȴ�
// 90�� �ֱ�� ����
// ���� ������ ���� �� Net �ӵ� ���
void CGnuControl::ManageNodes()
{
	int NormalConnects = 0;
	int UltraConnects  = 0;
	int LeafConnects   = 0;

	m_NetSecBytesDown = 0;
	m_NetSecBytesUp	  = 0;

	// ����� ����� ��
	int connectedNodeCount = 0;

	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)
	{
		if(SOCK_CONNECTED == m_NodeList[i]->m_Status)
		{
			connectedNodeCount++;
			if(m_NodeList[i]->m_nSecNum)
			{
				m_NetSecBytesDown += m_NodeList[i]->m_dwTotalBytes[0] / m_NodeList[i]->m_nSecNum;
				m_NetSecBytesUp   += m_NodeList[i]->m_dwTotalBytes[1] / m_NodeList[i]->m_nSecNum;
			}
				
			if(m_NodeCapability == CLIENT_ULTRAPEER && !m_LeafModeActive)
			{
				if(m_NodeList[i]->m_HostMode == CLIENT_LEAF)
					LeafConnects++;
				else
				{
					NormalConnects++;

					if(m_NodeList[i]->m_HostMode == CLIENT_ULTRAPEER)
						UltraConnects++;
				}
			}
			else
				NormalConnects++;
		}

		m_NodeList[i]->Timer();
	}

	m_NodeAccess.Unlock();

	m_NormalConnectsApprox = NormalConnects;


	// Minute counter
	if(m_SecCount < 60)
		m_SecCount++;
	if(m_SecCount == 60)
	{
		m_UltraPongBytes = 0;
		m_SecCount = 0;
	}

	m_ExtPongBytes = 0;

	// SEED IP�� ��û�Ѵ�.
	if( theApp.m_spGD->m_nodeCacheMap.Size() == 0)
		theApp.m_spGD->ReqSeedNodeList();
}

// �ֱ������� ����ȴ�
// 3�� �ֱ�� ����

//============================================================================
// NAME : ManageNodes2()
// PARM : ..
// RETN : ..
// DESC : SOCK_CONNECTING ������ ��� ����
// Date : 2005-04-01 coded by bemlove, origin
//============================================================================
void CGnuControl::ManageNodes2()
{
	// NodeAccess Locking ���� ȣ���ؾ��Ѵ�.
	int cnt_out = GetNodeTypeCnt(NT_OUTBOUND);

	m_NodeAccess.Lock();
	int cnt_all = m_NodeList.size();

	// ������Ʈ�ÿ� ���� ������ �̷���� m_Status == SOCK_CONNECTED �� ��常 �����ش�.
	int liveCount=0;
	for( int idx=0; idx<cnt_all; idx++ )
	{
		if( m_NodeList[idx]->m_Status == SOCK_CONNECTED )	liveCount++;
	}

	// �� ��帶�� Management �۾��� �����Ѵ�.
	for(int i = 0; i < cnt_all; i++)
	{
		if(SOCK_CONNECTING == m_NodeList[i]->m_Status)
		{
			if( liveCount > 0 )
				m_NodeList[i]->NodeManagement2(true);
			else
				m_NodeList[i]->NodeManagement2(false);


		}
	}
	m_NodeAccess.Unlock();

	int nMax = theApp.m_spGD->GetMaxOutConnection();

	if( cnt_out < nMax )
	{
		int nTryCnt;;

		if(m_bTurboNode) 
		{
			nTryCnt = nMax - cnt_out;
			nTryCnt = nTryCnt * 2;
		}
		else
		{
			nTryCnt = 1;
		}

		for(int i = 0; i<nTryCnt; i++)
		{
			GNUSeed2 seed;
			if( theApp.m_spGD->m_nodeCacheMap.Pop( seed ) )
			{
				AddNode( seed.m_szSeedIP, seed.m_iSeedPort, cnt_out );
			}
			else
				break;
		}
	}
	else
	{
		// ��� ������ �������� �ʴ�...

#ifdef NODE_CONNECTIVITY
		// ��� ������ Ȯ���Ѵ�.
		int nConnectivity = GetConnectivity();

		if(nConnectivity < (nMax * CONNECTIVITY_LEV2))
		{
			DropNode();			// by Older GnuNode Efficiency
		}
#else
		// �ɷ��� ���� Node�� �����Ѵ�.
		int nEfficiency = GetAvgEfficiency();
		if(nEfficiency < MIN_EFFICIENCY)
		{
		}
#endif
	}

}//end of ManagerNodes2()


// �ѹ��� �˻�� 50������ �Ѵ�.
// DownloadShell�� ���� ��ü���� ������ �����Ƿ� ���ʰ����� ��õ������ ��Ͽ� ���� �� �����Ƿ�
// ������ �ѹ��� ó���ϸ� ���ϰ� �ɸ� �� �ִ�.
#define MAX_CHECKCNT				50

// �ֱ������� ����ȴ�
void CGnuControl::ManageDownloads()
{
	if(!m_DownloadAccess.Lock(100)) return;

	int NetBytes		= m_NetSecBytesDown;
	m_DownloadSecBytes	= 0;

	bool bDownloading	= false;
	int KillDownload	= 0;
	int BytesLeft		= 0;
	int BytesAlloc		= 0;
	int DownloadCount	= 0;
	int MaxBytes		= 0;
	int nListSize		= m_DownloadList.size();

#ifdef MANAGE_ASBYUNIT
	int nCheckCnt		= 0;

	if(m_nLastDLCheck > nListSize)
		m_nLastDLCheck = 0;
	else
	{
		m_nLastDLCheck -= MAX_CHECKCNT / 2;
		if(m_nLastDLCheck < 0) m_nLastDLCheck = 0;
	}
#endif

	// written by moonknit 2005-09-05
	int DLCnt = CountDownloading();
	KillDownload = DLCnt - theApp.m_spGD->GetMaxDownload();

	std::list<CGnuDownloadShell*>::iterator it, tit;

	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); )
	{
		tit = it;
		++it;

		CGnuDownloadShell *pDown = (CGnuDownloadShell*) (*tit);
		
		if(!pDown) break;

		// written by moonknit 2005-06-16
		// ���� ���¿��� �ƹ� �۾��� �������� �ʴ´�.		
		if(pDown->m_bStop) continue;

		bDownloading = pDown->IsDownloading();
		if(bDownloading)
		{
			for(int j = 0; j < pDown->m_Sockets.size(); j++)
			{
				if(pDown->m_Sockets[j]->m_nSecNum)
					m_DownloadSecBytes += pDown->m_Sockets[j]->m_dwTotalBytes / pDown->m_Sockets[j]->m_nSecNum;
			}
			if( pDown->m_Waiting) pDown->m_Waiting = false; // 2004-12-28
			DownloadCount++;

			// from Data Variables
			if(theApp.m_spGD->m_BandwidthDown)
			{
				BytesLeft  += pDown->m_AllocBytes;
				BytesAlloc += pDown->m_AllocBytesTotal;
			}
		}

		pDown->Timer();

		// written by moonknit 2005-09-05
		if(bDownloading)
		{
			if(KillDownload>0)
			{
				pDown->ReleaseDownload();
				--KillDownload;
			}
		}
		else
		{
			if(pDown->m_bCanceled || pDown->m_bAllComplete)
			{
				delete *tit;
				m_DownloadList.erase(tit);
			}
		}
		// --

#ifdef MANAGE_ASBYUNIT
		nCheckCnt++;
		if(nCheckCnt >= MAX_CHECKCNT)
		{
			break;
		}
#endif
	}

#ifdef MANAGE_ASBYUNIT
	m_nLastDLCheck = i;
#endif
	
	// Manage Download Bandwidth
	// from Data Variables
	// �뿪�� ����
	if(theApp.m_spGD->m_BandwidthDown)
	{
		MaxBytes = theApp.m_spGD->m_BandwidthDown * 1024 - NetBytes;

		if(BytesAlloc > MaxBytes)
			BytesLeft -= BytesAlloc - MaxBytes;

		else if(BytesAlloc < MaxBytes)
			BytesLeft += MaxBytes - BytesAlloc;

		if(BytesLeft < 0)
			BytesLeft = 0;
	}
	
	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)
	{
		CGnuDownloadShell *pDown = (CGnuDownloadShell*) (*it);

		// from Data Variables
		if(pDown->IsDownloading() && theApp.m_spGD->m_BandwidthDown && DownloadCount)
		{
			pDown->m_AllocBytesTotal = MaxBytes / DownloadCount;

			pDown->m_AllocBytes = pDown->m_AllocBytesTotal;	
		}
	}

	UINT TotalDownSpeed = NetBytes + m_DownloadSecBytes;

	// from Data Variables
	if(TotalDownSpeed > theApp.m_spGD->m_RealSpeedDown)
		theApp.m_spGD->m_RealSpeedDown = TotalDownSpeed;

	// Speed Sensing
	int BitSpeed = (TotalDownSpeed) / 1024 * 8;
	if(BitSpeed > theApp.m_spGD->m_SpeedDyn)
		theApp.m_spGD->m_SpeedDyn = BitSpeed;

	m_DownloadAccess.Unlock();
}

// �ֱ������� ����ȴ�
void CGnuControl::ManageUploads()
{
	if(!m_UploadAccess.Lock(100)) return;

	int NetBytes     = m_NetSecBytesUp;
	m_UploadSecBytes = 0;

	int BytesLeft   = 0;
	int BytesAlloc  = 0;
	int UploadCount = 0;
	int i = 0;

	std::list<CGnuUploadShell*>::iterator it, tit;

	int nSize = m_UploadList.size();

	for(it = m_UploadList.begin(); it != m_UploadList.end(); )
	{
		tit = it;
		++it;
		CGnuUploadShell* pUp = (CGnuUploadShell*) (*tit);

		if(TRANSFER_SENDING == pUp->m_Status && pUp->m_nSecNum)
		{
			int SpeedUp = pUp->m_dwTotalBytes * 8 / 1024 / pUp->m_nSecNum;

			// from Data Variables
			if(theApp.m_spGD->m_RealSpeedUp < SpeedUp)
				theApp.m_spGD->m_RealSpeedUp = SpeedUp;

			// modified by moonknit 2005-08-23
			m_UploadSecBytes += pUp->m_AvgSpeed;

			UploadCount++;

			// from Data Variables
			if(theApp.m_spGD->m_BandwidthUp)
			{
				BytesLeft  += pUp->m_AllocBytes;
				BytesAlloc += pUp->m_AllocBytesTotal;
			}
		}

		pUp->Timer();

		// written by moonknit 2005-09-07
		// ������ �߻����� �ʴ� ���� �ڵ� ����ó���Ѵ�.
		if(pUp->m_Status == TRANSFER_CLOSED && pUp->m_bDeleteShell)
		{
			delete *tit;
			m_UploadList.erase(tit);
		}
		// -- 
	}

	// Manage Upload Bandwidth
	// from Data Variables
	if(theApp.m_spGD->m_BandwidthUp)
	{
		int MaxBytes = theApp.m_spGD->m_BandwidthUp * 1024 - NetBytes;

		if(BytesAlloc > MaxBytes)
			BytesLeft -= BytesAlloc - MaxBytes;

		else if(BytesAlloc < MaxBytes)
			BytesLeft += MaxBytes - BytesAlloc;

		if(BytesLeft < 0)
			BytesLeft = 0;
	}

	for(it = m_UploadList.begin(); it != m_UploadList.end(); ++it)
	{
		CGnuUploadShell *pUp = (CGnuUploadShell*) (*it);

		// from Data Variables
		if(TRANSFER_SENDING == pUp->m_Status
			&& theApp.m_spGD->m_BandwidthUp
			&& UploadCount)
		{
			// bytes upload can use = bytes upload used last second plus the bytes never used from the last second divided by the total active uploads
			pUp->m_AllocBytesTotal = (pUp->m_AllocBytesTotal - pUp->m_AllocBytes) + (BytesLeft / UploadCount);
			
			pUp->m_AllocBytes = pUp->m_AllocBytesTotal;	
		}
	}

	m_UploadAccess.Unlock();
}

// written by moonknit 2005-05-12
// moved from doc
bool CGnuControl::ConnectingSlotsOpen()
{
	int OccupiedSlots = 0;

	m_NodeAccess.Lock();
	// Node Sockets
	for(int i = 0; i < m_NodeList.size(); i++)
		if(m_NodeList[i]->m_Status == SOCK_CONNECTING)
			OccupiedSlots++;

	m_NodeAccess.Unlock();

	// Download Sockets
	std::list<CGnuDownloadShell*>::iterator it;

	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)
		for(int j = 0; j < (*it)->m_Sockets.size(); j++)
			if((*it)->m_Sockets[j]->m_Status == TRANSFER_CONNECTING)
				OccupiedSlots++;

	return OccupiedSlots < 20 ? true : false;
} // CGnuControl::ConnectingSlotsOpen

void CGnuControl::AddConnect(bool UltraPref)
{
	if(ConnectingSlotsOpen() )
		return;
}

void CGnuControl::DropNode()
{
	// Drop Normal nodes first
	CGnuNode* DeadNode = NULL;
	DWORD LowestRating = 100;

	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)
	{
		if(SOCK_CONNECTED == m_NodeList[i]->m_Status && m_NodeList[i]->m_HostMode == CLIENT_NORMAL)
			if(m_NodeList[i]->IsValidNode() && m_NodeList[i]->m_Efficeincy <= LowestRating)
			{
				DeadNode	 = m_NodeList[i];
				LowestRating = m_NodeList[i]->m_Efficeincy;
			}
	}

	m_NodeAccess.Unlock();

	if(DeadNode)
	{
		DeadNode->Close();
		return;
	}

	// Drop ultrapeers next
	DeadNode	 = NULL;
	LowestRating = 100;

	m_NodeAccess.Lock();

	for(i = 0; i < m_NodeList.size(); i++)
		if(SOCK_CONNECTED == m_NodeList[i]->m_Status && m_NodeList[i]->m_HostMode == CLIENT_ULTRAPEER)
			if(m_NodeList[i]->m_Efficeincy <= LowestRating)
			{
				DeadNode	 = m_NodeList[i];
				LowestRating = m_NodeList[i]->m_Efficeincy;
			}

	m_NodeAccess.Unlock();

	if(DeadNode)
		DeadNode->Close();
}

void CGnuControl::DropLeaf()
{
	CGnuNode* DeadNode = NULL;
	DWORD LowestRating = 100;

	m_NodeAccess.Lock();

	for(int i = 0; i < m_NodeList.size(); i++)
		if(SOCK_CONNECTED == m_NodeList[i]->m_Status)
			if(m_NodeList[i]->m_Efficeincy <= LowestRating && m_NodeList[i]->m_HostMode == CLIENT_LEAF)
			{
				DeadNode	 = m_NodeList[i];
				LowestRating = m_NodeList[i]->m_Efficeincy;
			}

	m_NodeAccess.Unlock();

	if(DeadNode)
		DeadNode->Close();
}


// �ֱ������� ����ȴ�
void CGnuControl::CleanDeadSocks()
{
	// Incoming Sockets
	std::vector<CGnuSock*>::iterator itSock;

	// for test
	int nCnt = m_SockList.size();

	for(itSock = m_SockList.begin(); itSock != m_SockList.end(); itSock++)
	{
		CGnuSock *pSock = *itSock;

		if((pSock->m_ServingHTTP == 0) && (pSock->m_nSecsAlive > CONNECT_TIMEOUT || pSock->m_bDestroy))
		{
			delete *itSock;
			m_SockList.erase(itSock);

			itSock --;
		}
		else
		{
			// written by moonknit 2005-08-17
			pSock->Timer();
		}
	}

	// Node Sockets
	m_NodeAccess.Lock();

	std::vector<CGnuNode*>::iterator itNode;
	for(itNode = m_NodeList.begin(); itNode != m_NodeList.end(); itNode++)
	{
		if((*itNode)->m_Status == SOCK_CLOSED)
		{
			delete *itNode;
			m_NodeList.erase(itNode);

			itNode --;
		
		}
		else
		{
		}
	}

	m_NodeAccess.Unlock();
}

// Pong Caching
void CGnuControl::AddToPongCache(MapPongList* pNewPong, int ttl)
{
	m_HostCacheAccess.Lock();

		while (m_PongCache[ttl].size() > PONG_CACHE_SIZE)
			m_PongCache_Old[ttl].erase( m_PongCache_Old[ttl].begin() );

		m_PongCache[ttl].push_back(*pNewPong);
		
	m_HostCacheAccess.Unlock();
}

void CGnuControl::FlushPongCache()
{	

	bool MajTimer = false;
	
	m_HostCacheAccess.Lock();

	// Transfer pongs to old pong
	for(int i = 0; i < MAX_TTL - 1; i++)
	{
		if (m_PongCache[i].size())
			m_PongCache_Old[i].clear();

		if (m_PongCache[1].size())
			MajTimer = true;
			
		while(m_PongCache[i].size())
		{
			m_PongCache_Old[i].push_back(m_PongCache[i].back());
			m_PongCache[i].pop_back();
		}

		m_PongCache[i].clear();
	}

	m_HostCacheAccess.Unlock();
		

	if (MajTimer)
		m_FlushPongCacheOld_time = GetTickCount() + TIMEOUT_PONG_CACHE * 3;

}

// ���� �ް� �ִ� ������ �̾�ޱ� �����Ѵ�.
void CGnuControl::LoadDownloads()
{
	CFileFind finder;
	CString   Header;

	// Start looking for files
	CString PartialPath = theApp.m_spLP->GetPartialDir();

	PartialPath.Replace(_T("\\\\"), _T("\\"));
	int bWorking = finder.FindFile(PartialPath + _T("\\*"));

#ifdef MULTI_UPDATETFSTATUS
	CString sSRID;
	int nFSerial;
	int n, srcnt;
#endif

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// Skip . and .. files
		if (finder.IsDots() || finder.IsDirectory())
			continue;

		CString FileDir  = finder.GetFilePath();
		
		CString FullName = FileDir.Mid( FileDir.ReverseFind('\\') + 1);
	
		if(FullName.Right(4) != _T(".ent"))
			continue;

		// Read from download backup file
		CStdioFile BackupFile;
		
		CString NextLine;
		CString Backup;
		
		if (BackupFile.Open(FileDir, CFile::modeRead))
		{
			while (BackupFile.ReadString(NextLine))
			{
				Backup += NextLine + _T("\n");
			}
			BackupFile.Abort();
		}

		if(Backup.IsEmpty() || Backup.Find(_T("[Download]")) == -1)
			continue;

		int CurrentPos = Backup.Find(_T("[Download]"));

		CGnuDownloadShell* Download = new CGnuDownloadShell();
		Download->m_bQueued = FALSE;
		
		Download->m_Name = GetBackupString(_T("Name"), CurrentPos, Backup);

		// ���α׷��� �ٽ� �����ϴ� ���������� active / not dead ���·� ����
		Download->m_Active	= true; 
		Download->m_Dead	= false;

		Download->m_BytesCompleted	= _ttoi(GetBackupString(_T("Completed"), CurrentPos, Backup));
		Download->m_FileLength		= _ttoi(GetBackupString(_T("FileLength"), CurrentPos, Backup));
		Download->m_FilePath		= GetBackupString(_T("FilePath"), CurrentPos, Backup);
		Download->m_PartialPath		= GetBackupString(_T("PartialPath"), CurrentPos, Backup);
		Download->m_OverlapBytes	= _ttoi(GetBackupString(_T("Overlap"), CurrentPos, Backup));
		Download->m_StopPos			= _ttoi(GetBackupString(_T("StopPos"), CurrentPos, Backup));
		Download->m_StartBytes		= Download->m_BytesCompleted;

		// modified by moonknit 2005-10-14
#ifdef MULTI_UPDATETFSTATUS
		srcnt						= _ttoi(GetBackupString(_T("SRLintCnt"), CurrentPos, Backup));
		CString property;
		for(n = 1; n <= srcnt; ++n)
		{
			property.Format(_T("RSID[%d]", n);
			sSRID						= GetBackupString(property, CurrentPos, Backup);
			property.Format(_T("FileSerial[%d]", n);
			nFSerial					= _ttoi(GetBackupString(property, CurrentPos, Backup));

			if(sSRID == _T("") break;

			Download->AddSRLink(nFSerial, sSRID);
		}
#else
		// written by moonknit 2005-08-25
		Download->m_sSRID			= GetBackupString(_T("RSID"), CurrentPos, Backup);
		Download->m_nFSerial		= _ttoi(GetBackupString(_T("FileSerial"), CurrentPos, Backup));
		// -- 
#endif

		// Get bytes completed
		CFileStatus PartialInfo; 
		if(CFile::GetStatus(Download->m_PartialPath, PartialInfo))
		{
			if(PartialInfo.m_size > 4096)
				Download->m_BytesCompleted = PartialInfo.m_size; 
			else
				Download->m_BytesCompleted = 0;
		}
		

		Download->m_Sha1Hash		= GetBackupString(_T("Sha1Hash"), CurrentPos, Backup);
		Download->m_BitprintHash	= GetBackupString(_T("BitprintHash"), CurrentPos, Backup);
		Download->m_Search			= GetBackupString(_T("Search"), CurrentPos, Backup);
		Download->m_SearchGuid		= StrtoGuid(GetBackupString(_T("SearchGuid"), CurrentPos, Backup));

		Download->HostFamilyAge = _ttoi(GetBackupString(_T("FamilyAge"), CurrentPos, Backup));
		Download->m_AvgSpeed	= _ttoi(GetBackupString(_T("AvgSpeed"), CurrentPos, Backup));


		// Load Chunk info
		for(int i = 0; ; i++)
		{
			CurrentPos = Backup.Find(_T("[Chunk") + DWrdtoStr(i) + _T("]"));

			if(CurrentPos == -1)
				break;

			int ChunkSize = 0;

			CString ChunkName = GetBackupString(_T("Path"), CurrentPos, Backup); 

			CFileStatus ChunkInfo; 
			if (CFile::GetStatus(ChunkName, ChunkInfo))
				ChunkSize = ChunkInfo.m_size;
				 

			FileChunk* pChunk = new FileChunk;
			pChunk->Name	 = GetBackupString(_T("Name"), CurrentPos, Backup);
			pChunk->Path	 = GetBackupString(_T("Path"), CurrentPos, Backup);
			pChunk->Sha1Hash = GetBackupString(_T("Sha1Hash"), CurrentPos, Backup);

			pChunk->StartPos		= _ttoi(GetBackupString(_T("StartPos"), CurrentPos, Backup));
			pChunk->ChunkLength		= _ttoi(GetBackupString(_T("FileLength"), CurrentPos, Backup));
			pChunk->BytesCompleted	= ChunkSize;
			pChunk->OverlapBytes	= _ttoi(GetBackupString(_T("OverlapBytes"), CurrentPos, Backup));
			pChunk->ChunkColor		= _ttoi(GetBackupString(_T("ChunkColor"), CurrentPos, Backup));
			pChunk->HostFamily		= _ttoi(GetBackupString(_T("HostFamily"), CurrentPos, Backup));

			// added by moonknit
			if(i == 0 && Download->m_BytesCompleted >= pChunk->StartPos) Download->m_bComplete = true;
			if(pChunk->BytesCompleted >= pChunk->ChunkLength) pChunk->bComplete = true;
			// -- added

			// Get Chunk bytes completed
			

			Download->m_ChunkList.push_back(pChunk);
		}

		
		// Load Host info
		if(Download->m_BytesCompleted < Download->m_FileLength)
			for(int i = 0; ; i++)
			{
				CurrentPos = Backup.Find(_T("[Host") + DWrdtoStr(i) + _T("]"));

				if(CurrentPos == -1)
					break;

				CurrentPos += 5; // Host in header and value conflict

				Result nResult;
				nResult.Name = GetBackupString(_T("Name"), CurrentPos, Backup);
				nResult.NameLower = nResult.Name;
				nResult.NameLower.MakeLower();

				nResult.Sha1Hash	 = GetBackupString(_T("Sha1Hash"), CurrentPos, Backup);
				nResult.BitprintHash = GetBackupString(_T("BitprintHash"), CurrentPos, Backup);

				nResult.FileIndex	= _ttoi(GetBackupString(_T("FileIndex"), CurrentPos, Backup));
				nResult.Size		= _ttoi(GetBackupString(_T("Size"), CurrentPos, Backup));

				nResult.Host	= StrtoIP(GetBackupString(_T("Host"), CurrentPos, Backup));
				nResult.Port	= _ttoi(GetBackupString(_T("Port"), CurrentPos, Backup));
				nResult.Speed	= _ttoi(GetBackupString(_T("Speed"), CurrentPos, Backup));
				nResult.UserID	= GetBackupString(_T("UserID"), CurrentPos, Backup);

				nResult.Firewall	= _ttoi(GetBackupString(_T("Firewall"), CurrentPos, Backup)) != 0;
				nResult.OpenSlots	= _ttoi(GetBackupString(_T("OpenSlots"), CurrentPos, Backup)) != 0;
				nResult.Busy		= _ttoi(GetBackupString(_T("Busy"), CurrentPos, Backup)) != 0;
				nResult.Stable		= _ttoi(GetBackupString(_T("Stable"), CurrentPos, Backup)) != 0;
				nResult.ActualSpeed = _ttoi(GetBackupString(_T("ActualSpeed"), CurrentPos, Backup)) != 0;
				nResult.PushID		= StrtoGuid(GetBackupString(_T("PushID"), CurrentPos, Backup));
				nResult.wVersion	= _ttoi(GetBackupString(_T("Version"), CurrentPos, Backup));


				nResult.Origin   = NULL;
				nResult.Distance = 7;

				Download->AddHost(nResult);
			}
	
		Download->m_DoReQuery = false;

		m_DownloadAccess.Lock();
//		if( m_DownloadList.size() >= 20 )
//			m_DownloadList.reserve( m_DownloadList.capacity() + 1 );
		m_DownloadList.push_back(Download);
		m_DownloadAccess.Unlock();
	}

	finder.Close();
}

CString CGnuControl::GetBackupString(CString Property, int &StartPos, CString &Backup)
{
	int PropPos = Backup.Find(Property, StartPos);
	
	if(PropPos != -1)
	{
		PropPos += Property.GetLength() + 1;
		int EndPos = Backup.Find(_T("\n"), PropPos);

		if(EndPos != -1)
			return Backup.Mid(PropPos, EndPos - PropPos);
	}

	return "";
}

bool CGnuControl::CheckCache(IP host)
{
	std::vector<PongCache>::iterator	iterPongCache;
	for ( iterPongCache = m_stlPongCache.begin() ; iterPongCache != m_stlPongCache.end() ; iterPongCache++ )
	{
		// IPtoStr error...02/20
		if (  IPtoStr((*iterPongCache).HostIP) == IPtoStr(host) ) return true;
	}
	return false;
}


// for clean now nodelist 
void CGnuControl::ClearList(void)
{
	m_NodeList.clear();
	m_stlPongCache.clear();
	m_SockList.clear();

}//end of ClearList();


//============================================================================
// NAME : DeleteNode()
// PARM : CString strHostIP, UINT port, int reason
// RETN : void
// DESC : reason�� ���ؼ� ������ ��忡�� ����
//        0 : ������ ������ MAX_GNUNODE �� �ʰ��Ȱ�� ���� �ź�(Other node info ���� ��Ȳ)
// Date : 2004-12-06 coded by bemlove, origin
//============================================================================
void CGnuControl::DeleteNode( CString strHostIP, UINT port, int reason )
{
	ASSERT(strHostIP && port );
	if( strHostIP == _T("") || strHostIP.GetLength()<=0 || port <= 0 )
		return;


	switch( reason )
	{
		case 0 :	// ������ MAX_GNUNODE �� ����
		{
			m_NodeAccess.Lock();

			std::vector<CGnuNode*>::iterator itrNode;
			for( itrNode = m_NodeList.begin(); itrNode != m_NodeList.end(); itrNode++ )
			{
				if( (*itrNode)->m_HostIP == strHostIP && (*itrNode)->m_Port == port )
				{
					break;
				}
			}

			if(itrNode != m_NodeList.end())
			{
				delete *itrNode;
				m_NodeList.erase(itrNode);
			}

			m_NodeAccess.Unlock();

			break;
		}
		default:
			break;
	}
}//end of DeleteNode()


//============================================================================
// NAME : AddOtherNode()
// PARM : CString strHostIP, UINT port, int reason
// RETN : void
// DESC : reason �� ���ؼ� ���Ӱ� ���� ��� ������ ���� �õ�
//        0 : MAX_GNUNODE �� ���� ���Ӱ� ���� NODE ���� ADDNODE ����
// Date : 2004-12-06 coded by bemlove, origin
//============================================================================
void CGnuControl::AddOtherNode( CString strHostIP, UINT port, int reason )
{
	ASSERT( strHostIP && port );
	if( strHostIP == _T("") || strHostIP.GetLength()<=0 || port <= 0  )
		return;

	switch(reason)
	{
		case 0 :
		{
			// NodeAccess Locking ���� ȣ���ؾ��Ѵ�.
			int cnt_out = GetNodeTypeCnt(NT_OUTBOUND);

			AddNode( strHostIP, port, cnt_out);
			break;
		}
		default:
			break;
	}
}//end of AddOtherNode()


//============================================================================
// NAME : RequestOtherNode()
// PARM : void
// RETN : void
// DESC : �������� ��尡 close �Ǿ��� ��� ���ο� ��� ������ ��û�Ѵ�.
// Date : 2004-12-06 coded by bemlove, origin
//============================================================================
void CGnuControl::RequestOtherNode()
{
	if( m_NodeList.size () >= MAX_GNUNODE-1 )	// ����� ��尡 MAX_GNUNODE-1 ���� ���������� ��û�Ѵ�.(1���� ���� Accept ������ remain)
	{
		return;
	}
	
	int count = m_NodeList.size() ;
	if( count > 0 )
	{
		srand( GetTickCount() );
		int sel = rand() % count;
		if( m_NodeList[sel]->m_Status == SOCK_CONNECTED )
		{
			// ���� �������� ���Ͽ� ���� ������ ��û�Ѵ�.
			CString ReqData = _T("");
			ReqData = EZ_REQ_OTHER_NODE_INFO;
			int ret = m_NodeList[sel]->Send( ReqData, ReqData.GetLength() );
			if( ret == SOCKET_ERROR )
			{
				return;
			}

			return;	
		}
	}
	
}//end of RequestOtherNode()

//============================================================================
// NAME : Manager ActiveHost (Add, Delete, Find)
// PARM : ActiveHost
// RETN : bool
// DESC : ���� �ٿ�ε� ���� Host�鿡 ���� ���� ����
// Date : 2004-12-16 coded by bemlove, origin
//============================================================================
bool CGnuControl::AddActiveHost( ActiveHost host )
{
//	ASSERT( host.HostIP.S_addr && host.Port > 0 && host.FileName );
	if( !(host.HostIP.S_addr && host.Port > 0 && host.FileName) )
		return false;
	
	if( FindActiveHost( host ) )
		return false;

	m_ActiveDownloadAccess.Lock();
	m_stlActiveDownloadHost.push_back( host );
	m_ActiveDownloadAccess.Unlock();
	return true;
}//end of AddActiveHost()

bool CGnuControl::DelActiveHost( ActiveHost host )
{
	// file name not use..
	ASSERT( host.HostIP.S_addr && host.Port > 0 );
	if( !(host.HostIP.S_addr && host.Port > 0 ) )
		return false;
	std::vector<ActiveHost>::iterator itrHost;
	for( itrHost = m_stlActiveDownloadHost.begin(); itrHost != m_stlActiveDownloadHost.end(); itrHost++ )
	{
		if( host.HostIP.S_addr == (*itrHost).HostIP.S_addr 
		 && host.Port == (*itrHost).Port )
		{
			m_ActiveDownloadAccess.Lock();
			m_stlActiveDownloadHost.erase( itrHost );
			m_ActiveDownloadAccess.Unlock();
			return true;
		}
	}
	return false;
}//end of DelActiveHost()

// return
// 0 : not find or argument missing
// 1 : current download file
// 2 : other file download.
int CGnuControl::FindActiveHost( ActiveHost host )
{
	if( !(host.HostIP.S_addr && host.Port > 0 && host.FileName) )
		return 0;

	std::vector<ActiveHost>::iterator itrHost;
	for( itrHost = m_stlActiveDownloadHost.begin(); itrHost != m_stlActiveDownloadHost.end(); itrHost++ )
	{
		if( host.HostIP.S_addr == (*itrHost).HostIP.S_addr 
			&& host.Port == (*itrHost).Port )
		{

			if(host.FileName == (*itrHost).FileName )
			{
				return 1;
			}
			return 2;
		}
	}
	return false;
}//end of FindActiveHost()

// written by	: moonknit
// date			: 2005-04-01
// last updated	: 2005-04-01
// desc			: check and add new download information to the current downloading list
bool CGnuControl::AddDLNode( CGnuDownload* pDownload )
{
	m_DLNodeList.push_back(pDownload);

	return true;
}

// written by	: moonknit
// date			: 2005-04-01
// last updated	: 2005-04-01
// desc			: Removed download object pointer from the current downloading list
bool CGnuControl::RemoveDLNode( CGnuDownload* pDownload )
{
	std::vector<CGnuDownload*>::iterator c;

	for(c = m_DLNodeList.begin() ; c != m_DLNodeList.end(); c++)
	{
		if(*c == pDownload)
		{
			// ��ü �Ҹ��Ű�� ����
			m_DLNodeList.erase(c);

			return true;
		}
	}
	
	return false;
}

// written by	: moonknit
// date			: 2005-04-01
// last updated	: 2005-04-01
// desc			: check download object pointer if the same host is previously used for donwloading
//				  if you want to add new download after checking with this function,
//				  AddDLNode will do check and add.
bool CGnuControl::IsDownloading( CGnuDownload* pDownload )
{
	std::vector<CGnuDownload*>::iterator c;

	for(c = m_DLNodeList.begin() ; c != m_DLNodeList.end(); c++)
	{
		if((*c)->m_Params.Host.S_addr == pDownload->m_Params.Host.S_addr
			&& (*c)->m_Params.Port == pDownload->m_Params.Port)
		{
			return true;
		}
	}
	
	return false;
}

// PROJECT		: PREMIUM USER
// written by	: moonknit 
// date			: 2005-04-20
// last updated : 2005-04-20
// desc			: reorder the uploadshell list
void CGnuControl::ReOrderUploadShell( CGnuUploadShell* pUpload)
{
	std::list<CGnuUploadShell*>::iterator it;

	bool bFind = false;
	
	for(it = m_UploadList.begin(); it != m_UploadList.end(); it++)
	{
		if(*it == pUpload)
		{
			m_UploadList.erase(it);
			bFind = true;
			break;
		}
	}

	if(bFind)
	{
		m_UploadList.push_back(pUpload);
	}
}

// PROJECT		: PREMIUM USER
// written by	: moonknit 
// date			: 2005-04-20
// last updated : 2005-04-20
// desc			: remove uploadshell from list
void CGnuControl::RemoveUploadShell( CGnuUploadShell* pUpload )
{
	std::list<CGnuUploadShell*>::iterator it;

	m_UploadAccess.Lock();

	for(it = m_UploadList.begin(); it != m_UploadList.end(); it++)
	{
		if(*it == pUpload)
		{
			m_UploadList.erase(it);

			delete (pUpload);
			break;
		}
	}
	m_UploadAccess.Unlock();
}

// written by	: moonknit 
// date			: 2005-06-01
// last updated : 2005-06-01
// desc			: remove downloadshell from list
void CGnuControl::RemoveDownloadShell(CGnuDownloadShell* pDownload)
{
	std::list<CGnuDownloadShell*>::iterator it;

	m_DownloadAccess.Lock();

	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); it++)
	{
		if(*it == pDownload)
		{
			break;
		}
	}

	if(it != m_DownloadList.end())
	{
		m_DownloadList.erase(it);
		delete (pDownload);
	}

	m_DownloadAccess.Unlock();
}

// PROJECT		: PREMIUM USER
// written by	: moonknit 
// date			: 2005-04-20
// last updated : 2005-04-20
// desc			: Get non-premium upload shell
CGnuUploadShell* CGnuControl::GetNonPremiumUploadShell( int* x, CGnuUploadShell* pUpload )
{
	CGnuUploadShell* pTempShell = NULL;
	std::list<CGnuUploadShell*>::iterator it;
	int i = 0;
	for(it = m_UploadList.begin(); it != m_UploadList.end(); ++it)
	{
		pTempShell = (CGnuUploadShell*) (*it);

		if( pTempShell != pUpload
			&& pTempShell->m_Status == TRANSFER_SENDING
			&& pTempShell->m_nPremiumLevel != 1		// Premium User
			&& pTempShell->m_nPremiumLevel != 2		// High Enppy Point User
			)
		{
			*x = i;
			++it;
			return pTempShell;
		}
	}

	return NULL;
}

CGnuDownloadShell* CGnuControl::DownloadAndSearch(RFIDLink& link)
{
	// Create Download Shell
	DL_DATAINFO dlinfo;
	dlinfo.sguid = link.RFID;
	dlinfo.name = link.FileName;
	dlinfo.nsize = link.FileSize;

	// add default host information
	if(link.HOSTINFO != _T(""))
	{
		TCHAR		szip[64];
#ifdef _UNICODE
		char		sziptmp[64];
		char		szhostinfotmp[128];
#endif
		int			port;
		int			findex;
#ifdef _UNICODE
		if(!AnsiConvertWtoM((LPTSTR) (LPCTSTR) link.HOSTINFO, szhostinfotmp, sizeof(szhostinfotmp)))
			return NULL;
		
		dec_hostinfo(szhostinfotmp, sziptmp, port, findex);

		if(!AnsiConvertMtoW(sziptmp, szip, sizeof(szip)))
			return NULL;
#else
		dec_hostinfo(link.HOSTINFO, szip, port, findex);
#endif

		DL_SOURCE dlSource;

		dlSource.nid = findex;
		dlSource.ngnuport = port;
		dlSource.shostip = szip;

		dlinfo.listSource.push_back(dlSource);
	}

	CGnuDownloadShell* pShell = Download(dlinfo, TRUE);

	link.pShell = pShell;

	// add to RFID Search List
	RFIDLink* pLink = new RFIDLink;
	*pLink = link;

	m_SearchList.push_back(pLink);

	return pShell;
}

void CGnuControl::ManageSearch()
{
	// ��� Ȯ���� �Ǹ� RFID link �˻��� �����Ѵ�.
	if(m_nNormalCnt < 1)
		return;

	if(m_SearchList.size() == 0) return;

	RFIDLink* pLink = NULL;

	while(m_SearchList.size() != 0)
	{
		// guid search or hash search
//		pLink = m_SearchList.back();
//		body.type = GUID_SEARCH;		// RFID Search
//		body.sword = pLink->RFID;
//
//		if(pLink->pShell)
//		{
//			pLink->pShell->m_SearchGuid = SendQuery(body);
//		}

		m_SearchList.pop_back();
		delete pLink;
	}
}

CGnuDownloadShell* CGnuControl::Download(ResultGroup& ItemGroup, BOOL bQueued)
{
	// REQUEST POST DOWNLOAD

	// �̹� �ٿ�ް� �ִ� ������ �ִٸ� �� CGnuDownloadShell�� Item �� �־��ش�.
	std::list<CGnuDownloadShell*>::iterator iterDownShell;
	CGnuDownloadShell*	Download = NULL;
	int i;

	m_DownloadAccess.Lock();
	for ( iterDownShell = m_DownloadList.begin() ; iterDownShell != m_DownloadList.end() ; iterDownShell++)
	{
		if ( (*iterDownShell)->m_FileLength == ItemGroup.Size 
			&& (*iterDownShell)->m_Sha1Hash == ItemGroup.Sha1Hash
			&& (*iterDownShell)->GetStatus() != TRANSFER_CLOSED
			)
		{
			Download = (*iterDownShell);
			break;
		}
	}
	m_DownloadAccess.Unlock();

	if ( Download != NULL ) 
	{
//		if ( MessageBox(NULL, STR_MSG_FILSRCHVIEW_004, STR_MSG_FILSRCHVIEW_005, MB_YESNO ) == IDYES )
		{
			// Add hosts to the download list
			for(i = 0; i < ItemGroup.ResultList.size(); i++)
			{
				Download->AddHost( ItemGroup.ResultList[i] );
			}

			return Download;
		}
	}

	// Create new download and insert into control list
	Download = new CGnuDownloadShell();

	Download->m_nType		= ItemGroup.Type;
	Download->m_bQueued		= bQueued;

	// Give download some inital properties
	Download->m_Name		= ItemGroup.Name;
	Download->m_OriginalFileName = ItemGroup.Name;
	
	Download->m_Search		= ItemGroup.Sha1Hash;
 	Download->m_Sha1Hash     = ItemGroup.Sha1Hash;
	Download->m_BitprintHash = _T("");

	Download->m_FileLength	= ItemGroup.Size;
	Download->m_StopPos     = ItemGroup.Size;
	Download->m_AvgSpeed	= 0;

	Download->m_SearchGuid	= ItemGroup.SearchGUID;

	// written by moonknit 2005-06-30
	// �ٿ�ޱ� �۾��� �����Ѵ�.
	Download->m_Active		= true;

	Download->pGroup = &ItemGroup;

	m_DownloadAccess.Lock();
	m_DownloadList.push_back(Download);
	m_DownloadAccess.Unlock();

	// Add hosts to the download list
	for(i = 0; i < ItemGroup.ResultList.size(); i++)
		Download->AddHost( ItemGroup.ResultList[i] );

	// ���� �ٿ�ε� ����� ������ �õ��Ѵ�.
	Download->Timer();

	return Download;
}

// PIPE : Transfer File 2

// written by		: moonknit
// name				: CGnuControl::Download
// created			: 2005-05-18
// last updated		: 2005-05-18
// desc				:
// �ٿ�ε带 ��û�Ѵ�.
CGnuDownloadShell* CGnuControl::Download(DL_DATAINFO& dlinfo, BOOL bQueued)
{
	// �ٿ�ε� ȭ������ ��ȯ??

	int i = 0;
	ResultGroup ItemGroup;
	Result		Item;

	CString		FileName = dlinfo.name;
	int			FileSize = dlinfo.nsize;

	DL_SOURCE	source;
	
	int pos;
	pos = FileName.ReverseFind(_T('\\'));
	if ( pos > -1 )	{
		CString tempString;
		tempString = FileName.Right( FileName.GetLength() - pos -1 );
		FileName = tempString;
	}

	CString FileNameLower (FileName) ;
	FileNameLower.MakeLower();

	// ���� �ʱ�ȭ 
	ItemGroup.Name			=	FileName;
	ItemGroup.NameLower		=	FileNameLower;
	ItemGroup.AvgSpeed		=	0;
	ItemGroup.Size			=	FileSize;
	ItemGroup.bDownloading	=   false;
	ItemGroup.bCompleted	=   false;
	ItemGroup.bNoHosts		=	false;

	list<DL_SOURCE>::iterator it;

	for(it = dlinfo.listSource.begin(); it != dlinfo.listSource.end(); ++it)
	{
		Item.FileIndex			=   (*it).nid;
		Item.Name				=	FileName;
		Item.NameLower			=	FileNameLower;
		Item.UserID				=	(*it).suid;
		Item.Size				=	FileSize;
		Item.Host				=	StrtoIP( (*it).shostip );
		Item.Port				=	(WORD) (*it).ngnuport;
		Item.Busy				=	false;
		Item.Firewall			=	false;
		Item.wVersion			=	(*it).wVersion;

		ItemGroup.ResultList.push_back(Item);
	}

	// �̹� �ٿ�ް� �ִ� ������ �ִٸ� �� CGnuDownloadShell�� Item �� �־��ش�.
	std::list<CGnuDownloadShell*>::iterator iterDownShell;
	CGnuDownloadShell*	Download = NULL;

	m_DownloadAccess.Lock();
	for ( iterDownShell = m_DownloadList.begin() ; iterDownShell != m_DownloadList.end() ; iterDownShell++)
	{
		if ( (*iterDownShell)->m_FileLength == FileSize 
			&& (*iterDownShell)->m_Sha1Hash == dlinfo.sguid 
			&& (*iterDownShell)->GetStatus() != TRANSFER_CLOSED
			)
		{
			Download = (*iterDownShell);
			break;
		}
	}
	m_DownloadAccess.Unlock();

	if ( Download != NULL ) 
	{
		if ( FishMessageBox(STR_MSG_FILSRCHVIEW_004, STR_MSG_FILSRCHVIEW_005, MB_YESNO ) == IDYES )
		{
			// Add hosts to the download list
			for(i = 0; i < ItemGroup.ResultList.size(); i++)
			{
				Download->AddHost( ItemGroup.ResultList[i] );
#ifdef MULTI_UPDATETFSTATUS
				Download->AddSRLink(dlinfo.nFSerial, dlinfo.sSRID);
				Download->m_nOldState = 0;
#endif
			}

			return Download;
		}
	}

	// Create new download and insert into control list
	Download = new CGnuDownloadShell();

	Download->m_bQueued		= bQueued;

	// Give download some inital properties
	Download->m_Name		= FileName;
	Download->m_OriginalFileName = FileName;
	
	Download->m_Search		= FileName;
 	Download->m_Sha1Hash     = dlinfo.sguid;
	Download->m_BitprintHash = _T("");

	Download->m_FileLength	= FileSize;
	Download->m_StopPos     = FileSize;
	Download->m_AvgSpeed	= 0;

	// written by moonknit 2005-06-30
	// �ٿ�ޱ� �۾��� �����Ѵ�.
	Download->m_Active		= true;

	// Change download name if there's a duplicate
	bool dups = true;
	int nTryCnt = 0;

	CString sTempFileName = Download->m_Name;

	// PartialFile�� ���� �ٿ�ε� ���� ������ ���� ���� ���θ� �Ǵ��ϱ� ���� �ڷ��
	// ���ȴ�.
	// Download->m_Name�� ������ Partial ���丮�� �ִ��� Ȯ���غ���.
	// �̹� �����ϴ� ������ �̸��̶�� ���ϸ��� �����ϵ��� ó���Ѵ�.
	CString PartialDir = theApp.m_spLP->GetPartialDir();

	while(dups)
	{
		dups = false;

		std::list<CGnuDownloadShell*>::iterator it;

		for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)
		{
			if((*it)->m_Name == sTempFileName)
			{
				sTempFileName = GetNewFileName(Download->m_Name, ++nTryCnt);

				dups = true;
				break;
			}
		}

		CheckPartialFileName(sTempFileName, PartialDir);
	}

	Download->m_Name = sTempFileName;

	Download->pGroup = &ItemGroup;

#ifdef MULTI_UPDATETFSTATUS
	Download->AddSRLink(dlinfo.nFSerial, dlinfo.sSRID);
#else
	Download->m_nFSerial = dlinfo.nid;
	Download->m_sSRID = dlinfo.sguid;
#endif

	// ���� 1ȸ ������ ���� ã�⸦ �����ϵ��� �Ѵ�.
	Download->m_DoReQuery = true;
	// --

	m_DownloadAccess.Lock();
	m_DownloadList.push_back(Download);
	m_DownloadAccess.Unlock();

	// Add hosts to the download list
	for(i = 0; i < ItemGroup.ResultList.size(); i++)
		Download->AddHost( ItemGroup.ResultList[i] );

	return Download;
}

// PIPE : Transfer File 2

// written by		: moonknit
// name				: CGnuControl::Download
// created			: 2005-05-18
// last updated		: 2005-05-18
// desc				:
// �ٿ�ε带 ��û�Ѵ�.
CGnuDownloadShell* CGnuControl::Download(SE_ITEM& item, BOOL bQueued)
{
	// �ٿ�ε� ȭ������ ��ȯ??

	int i = 0;
	ResultGroup ItemGroup;
	Result		Item;

	CString		FileName = item.di.name;
	int			FileSize = item.di.nsize;

	int pos;
	pos = FileName.ReverseFind( '\\');
	if ( pos > -1 )	{
		CString tempString;
		tempString = FileName.Right( FileName.GetLength() - pos -1 );
		FileName = tempString;
	}

	CString FileNameLower (FileName) ;
	FileNameLower.MakeLower();

	ItemGroup.Name			=	FileName;
	ItemGroup.NameLower		=	FileNameLower;
	ItemGroup.AvgSpeed		=	0;
	ItemGroup.Size			=	FileSize;
	ItemGroup.bDownloading	=   false;
	ItemGroup.bCompleted	=   false;
	ItemGroup.bNoHosts		=	false;

	Item.FileIndex			=   item.di.nid;
	Item.Name				=	FileName;
	Item.NameLower			=	FileNameLower;
	Item.UserID				=	item.ui.suid;
	Item.Size				=	FileSize;
	Item.Host				=	StrtoIP( item.ui.shostip);
	Item.Port				=	(WORD) item.ui.ngnuport;
	Item.Busy				=	false;
	Item.Firewall			=	false;
	Item.PushID				=	item.PushID;
	Item.wVersion			=	item.ui.wVersion;

	ItemGroup.ResultList.push_back(Item);


	// �̹� �ٿ�ް� �ִ� ������ �ִٸ� �� CGnuDownloadShell�� Item �� �־��ش�.
	std::list<CGnuDownloadShell*>::iterator iterDownShell;
	CGnuDownloadShell*	Download = NULL;

	m_DownloadAccess.Lock();
	for ( iterDownShell = m_DownloadList.begin() ; iterDownShell != m_DownloadList.end() ; iterDownShell++)
	{
		if ( (*iterDownShell)->m_FileLength == FileSize 
			&& (*iterDownShell)->m_Name == FileName 
			&& (*iterDownShell)->GetStatus() != TRANSFER_CLOSED  )
		{
			Download = (*iterDownShell);
			break;
		}
	}
	m_DownloadAccess.Unlock();

	if ( Download != NULL ) 
	{
		if ( FishMessageBox(STR_MSG_FILSRCHVIEW_004, STR_MSG_FILSRCHVIEW_005, MB_YESNO ) == IDYES )
		{
			Download->AddHost( Item );

			return Download;
		}
	}

	// Create new download and insert into control list
	Download = new CGnuDownloadShell();

	Download->m_bQueued		= bQueued;

	// Give download some inital properties
	Download->m_Name		= FileName;
	Download->m_OriginalFileName = FileName;
	
	Download->m_Search		= FileName;
 	Download->m_Sha1Hash     = item.di.sguid;
	Download->m_BitprintHash = _T("");
	Download->m_FileLength	= FileSize;
	Download->m_StopPos     = FileSize;
	Download->m_AvgSpeed	= 0;

	// Change download name if there's a duplicate
	bool dups = true;
	int nTryCnt = 0;
	CString sTempFileName = Download->m_Name;
	CString PartialDir = theApp.m_spLP->GetPartialDir();

	std::list<CGnuDownloadShell*>::iterator it;
	while(dups)
	{
		dups = false;

		for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)
		{
			if((*it)->m_Name == Download->m_Name)
			{
				sTempFileName = GetNewFileName(Download->m_Name, ++nTryCnt);

				dups = true;
				break;
			}
		}

		CheckPartialFileName(sTempFileName, PartialDir);
	}

	Download->m_Name = sTempFileName;

	Download->pGroup = &ItemGroup;

	m_DownloadAccess.Lock();
	m_DownloadList.push_back(Download);
	m_DownloadAccess.Unlock();

	Download->AddHost( Item );

	return Download;
}

// written by		: moonknit
// name				: CGnuControl::CheckPartialFileName
// created			: 2005-05-19
// last updated		: 2005-05-19
// desc				:
// �ش� ���丮�� ������ ������ �̸��� ������ ������ �����ϴ��� Ȯ���Ѵ�.
// ������ �̸��� ������ �����ϸ� �ٿ�ε����ϸ��� �����Ų��.
CString CGnuControl::CheckPartialFileName(CString& name, CString Path)
{
	CFileFind	f;
	CString		sFindName;
	CString		NewName;
	CString		sCnt;
	int			pos;
	int			nCnt=1;

	NewName = name;

	sFindName = Path + _T("\\") + NewName;

	while( f.FindFile(sFindName) )
	{
		NewName = name;

		sCnt.Format(_T("_%d"), nCnt++);
		pos = NewName.ReverseFind('.');
		if(pos)
		{
			NewName.Insert(pos, sCnt);
		}
		else
			NewName += sCnt;

		sFindName = Path + _T("\\") + NewName;
	}

	f.Close();
	name = NewName;
	return NewName;
}

// written by		: moonknit
// name				: CGnuControl::CheckDownloadShell
// created			: 2005-06-01
// last updated		: 2005-06-01
// desc				:
// �־��� GnuDownloadShell �����Ͱ� ��ȿ���� Ȯ���Ѵ�.
BOOL CGnuControl::CheckDownloadShell(CGnuDownloadShell* pShell)
{
	if(!pShell) return FALSE;

	BOOL bRet = FALSE;

	m_DownloadAccess.Lock();

	std::list<CGnuDownloadShell*>::iterator it;

	for(it = m_DownloadList.begin(); it != m_DownloadList.end(); ++it)	
	{
		CGnuDownloadShell *p = (*it);

		if(p == pShell)
		{
			bRet = TRUE;
			break;
		}
	}

	m_DownloadAccess.Unlock();

	return bRet;
}

// written by		: moonknit
// name				: CGnuControl::CheckUploadShell
// created			: 2005-06-01
// last updated		: 2005-06-01
// desc				:
// �־��� GnuUploadShell �����Ͱ� ��ȿ���� Ȯ���Ѵ�.
BOOL CGnuControl::CheckUploadShell(CGnuUploadShell* pShell)
{
	if(!pShell) return FALSE;
	BOOL bRet = FALSE;

	m_UploadAccess.Lock();

	std::list<CGnuUploadShell*>::iterator it;
	for(it = m_UploadList.begin(); it != m_UploadList.end(); ++it)	
	{
		CGnuUploadShell *p = (*it);

		if(p == pShell)
		{
			bRet = TRUE;
			break;
		}
	}

	m_UploadAccess.Unlock();

	return bRet;
}

// written by		: moonknit
// name				: CGnuControl::IsTRCompleted
// created			: 2005-06-01
// last updated		: 2005-06-01
// desc				:
// ��� ���� �����۾��� �������� Ȯ���Ѵ�.
BOOL CGnuControl::IsTRCompleted()
{
	int nStatus;
	m_DownloadAccess.Lock();
	std::list<CGnuDownloadShell*>::iterator dit;
	for(dit = m_DownloadList.begin(); dit != m_DownloadList.end(); ++dit)	
	{
		CGnuDownloadShell *p = (*dit);

		nStatus = p->GetStatus();

		if(nStatus != TRANSFER_CLOSED && nStatus != TRANSFER_NEEDSOURCE && nStatus != TRANSFER_STOP)
		{
			m_DownloadAccess.Unlock();
			return FALSE;
		}
	}

	m_DownloadAccess.Unlock();

	m_UploadAccess.Lock();

	std::list<CGnuUploadShell*>::iterator uit;
	for(uit = m_UploadList.begin(); uit != m_UploadList.end(); ++uit)	
	{
		CGnuUploadShell *p = (*uit);

		if(p->m_Status != TRANSFER_CLOSED)
		{
			m_UploadAccess.Unlock();
			return FALSE;
		}
	}

	m_UploadAccess.Unlock();

	return TRUE;
}

// written by		: moonknit
// name				: CGnuControl::TryAutoCompleteAction
// created			: 2005-06-02
// last updated		: 2005-06-02
// desc				:
// �Ϸ�� �ڵ� ó�� ����
void CGnuControl::TryAutoCompleteAction(int nType)
{
	switch(nType)
	{
	case 1:
		break;
	case 3:
		break;
	case 2:
		break;
	}
}

#ifdef NODE_CONNECTIVITY
// written by		: moonknit
// name				: int CGnuControl::GetConnectivity(CGnuNode* pNode)
// created			: 2005-06-17
// last updated		: 2005-06-17
// desc				:
// ���� �� ������ ����� ����� ���ᰭ���� ���� ����Ѵ�.
// Parameter pNode�� ���ᰭ���� ��꿡�� �����Ѵ�.
// ���� ���� �⵿ �� ��ȿ�ð��� ������� �ʾҴٸ� �⺻ ���� ������ ��ȯ�Ѵ�.
// �ּ� ���ᰭ�� = 1
// �ִ� ���ᰭ�� = (�ִ� ���� ���� - 1) * 10 + 1
WORD CGnuControl::GetConnectivity(CGnuNode* pNode)
{
	DWORD now = GetTickCount();
	if(now - m_nStartTime > VALID_CONNTIME)
	{
		WORD nConnectivity = 1;
		m_NodeAccess.Lock();
		for(int i = 0; i < m_NodeList.size(); i++)
		{
			CGnuNode *p = m_NodeList[i];
			
			if(pNode == p) continue;

			nConnectivity += p->m_nConnectivity;
		}

		m_NodeAccess.Unlock();

		return nConnectivity;
	}

	return DEFAULT_CONNECTIVITY;
}
#endif

DWORD CGnuControl::GetAvgEfficiency(CGnuNode* pNode)
{
	int TotalEfficiency = 0;
	int nCnt = 0;
	m_NodeAccess.Lock();
	for(int i = 0; i < m_NodeList.size(); i++)
	{
		CGnuNode *p = m_NodeList[i];

		if(pNode == p) continue;

		if(!p->IsValidNode()) continue;

		TotalEfficiency += p->m_Efficeincy;
		nCnt++;
	}
	m_NodeAccess.Unlock();

	if(!nCnt) return 0;

	return TotalEfficiency / nCnt;
}

// ���ε� ���߿� ������Ҹ� �� ����� ������ Ȯ���Ѵ�.
bool CGnuControl::CheckRejectorList(CString UserID, int Index)
{
	std::vector<CanceledUpload>::iterator iterCancelUpload;

	CString strMessage;

	for ( iterCancelUpload = m_CancelUploadList.begin() ; iterCancelUpload != m_CancelUploadList.end(); iterCancelUpload++ )
	{
		if ( (*iterCancelUpload).UserID == UserID && (*iterCancelUpload).fileIndex == Index ) {
			return true;
		}
	}
	return false;
}

bool CGnuControl::InsertRejectorList(CString UserID, int Index)
{
	if(CheckRejectorList(UserID, Index)) return false;

	CanceledUpload cu;

	cu.fileIndex	= Index;
	cu.UserID		= UserID;

	m_CancelUploadList.push_back(cu);

	return true;
}

void CGnuControl::IncreaseDLTryingCnt()
{
	++m_nDLTryingCnt;
}

void CGnuControl::DecreaseDLTryingCnt()
{
	--m_nDLTryingCnt;
}

bool CGnuControl::GetDownloadingSlot()
{
	int nRemain = theApp.m_spGD->GetMaxDownload() - m_nCurDLCnt - m_nDLTryingCnt;

	return nRemain > 0 ? true : false;
}

/**************************************************************************
 * CGnuControl::GetNodeTypeCnt
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-21
 *
 * @Description
 * ����� ���Ŀ� ���� ���� ���� ��� ������ �˱� ���� �ۼ��Ͽ���.
 *
 * @Parameters
 * (in int) ntype - ���� ���� ������ �˰� ���� ����� ����
 *					NT_INBOUND : inbound node, NT_OUTBOUND : outbound node
 *					NT_ALL : inbound&outbound node
 *
 * @Result
 * (int) ��û�� ������ ����� ���� ���� ����
 **************************************************************************/
int CGnuControl::GetNodeTypeCnt(int ntype)
{
	CAutoLock al(&m_NodeAccess);
	
	int n = 0;
	std::vector<CGnuNode*>::iterator it;
	for(it = m_NodeList.begin(); it != m_NodeList.end(); ++it)
	{
		switch(ntype)
		{
		case NT_INBOUND:
			if(!(*it)->m_Inbound) continue;
			break;
		case NT_OUTBOUND:
			if((*it)->m_Inbound) continue;
			break;
		}
		++n;
	}

	return n;
}

/**************************************************************************
 * method CGnuControl::VerifyPostResult
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-02
 *
 * @Description
 * DB�� Post�� Verify ����� Upload Shell�� �����Ѵ�.
 *
 * @Parameters
 * (in LPVOID) lpuploader - Verify Post �� ��û�� Uploader
 * (in CString) xml - Verify Post ��� ����� Post�� XML string
 *
 * @Result
 **************************************************************************/
void CGnuControl::VerifyPostResult(LPVOID lpuploader, CString xml)
{
	CGnuUploadShell* pShell = (CGnuUploadShell*) lpuploader;

	if(!CheckUploadShell(pShell)) return;

	pShell->VerifyPostResult(xml);
}

/**************************************************************************
 * method CGnuControl::VerifyPostResult
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-02
 *
 * @Description
 * DB�� File�� Verify ����� Upload Shell�� �����Ѵ�.
 *
 * @Parameters
 * (in LPVOID) lpuploader - Verify File �� ��û�� Uploader
 * (in CString) FilePath - Verify File ��� ����� ������ File�� ���� ��ġ
 * (in CString) FileHash - Verify File ��� ����� ������ File�� hash��
 *
 * @Result
 **************************************************************************/
void CGnuControl::VerifyFileResult(LPVOID lpuploader, CString FilePath, CString FileHash)
{
	CGnuUploadShell* pShell = (CGnuUploadShell*) lpuploader;

	if(!CheckUploadShell(pShell)) return;

	pShell->VerifyFileResult(FilePath, FileHash);
}

/**************************************************************************
 * method CGnuControl::CheckNeeds
 *
 * written by moonknit
 *
 * @History
 * created 2006-03-20
 *
 * @Description
 * CGnuDownload�� Host ������ �̿��� ���� �ٿ�ε� ���� �׸��� �ִ��� ã�ƺ���.
 * ���� �ٿ�ε� ���� �׸��� �ִٸ� �ش� DownloadShell�� Download�� �ű�� �ٿ�ε带
 * �ǽ��Ѵ�.
 *
 * @Parameters
 * (in CGnuDownload*) pDown - ���� �ٿ�ε� ���� �׸��� �˻��Ѵ�.
 *
 * @Result
 **************************************************************************/
BOOL CGnuControl::CheckNeeds(CGnuDownload* pDown)
{
	return FALSE;
}