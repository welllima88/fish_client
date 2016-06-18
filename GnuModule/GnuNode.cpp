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

#include "GnuUpload.h"
#include "GnuUploadShell.h"
#include "GnuDownloadShell.h"

#include "GnuRouting.h"
#include "GnuControl.h"

#include "GnuNode.h"

#include "GnuDoc.h"

#include "../FISH.h"
#include "../LocalProperties.h"
#include "../Fish_struct.h"
#include "../RSSDB.h"
#include "../SearchManager.h"

#include "../fish_common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VALID_NODETIME		1000 * 60 * 10			// ten minutes

extern CFISHApp theApp;

CGnuNode::CGnuNode(CGnuControl* pComm, CString Host, UINT Port)
{	
	m_pComm				= pComm;

	m_dwExtraLength		= 0;
	m_SendReady			= true;


	// Node Info
	m_Status			= SOCK_CONNECTING;	
	m_nSecsTrying		= 0;
	m_nSecsRefresh		= 0;
	m_nSecsDead			= 0;
	m_ClosedSecs		= 0;
	m_IntervalPing		= 0;
	m_nSecsAlive		= 0;
	m_nOrigDistance		= 0;
	m_NextReSearchWait  = 0;

	m_HostIP			= Host;
	m_HostName			= m_HostIP;

	m_NetworkName		= theApp.m_spGD->ModeNetwork;
	m_Guerilla			= false;
	
	m_HostMode			= CLIENT_NORMAL;
	m_Port				= Port;
	m_Inbound			= false;
	m_ConnectTime		= CTime::GetCurrentTime();
	m_Efficeincy		= 0;
	
	m_HostUptime		= CTimeSpan(0,0,0,0);
	m_NodeFileCount		= 0;
	m_NodeLeafCount		= 0;
	m_NodeLeafMax		= 0;
	
	m_DowngradeRequest  = false;
	m_PatchUpdateNeeded = false;

	m_UltraPongSent		= false;	
	
	// Ultrapeers
	m_TableInfinity		= 2;
	m_TableLength		= 64 * 1024;
	
	m_PatchTable		= NULL;
	m_TableNextPos		= 0;
	
	m_CompressedTable	= NULL;
	m_CompressedSize	= 0;

	m_CurrentSeq		= 1;

	// Pong caching
	m_PongCachingEnabled = false;

	m_PingAcceptTime	= 0;
	m_PingSendTime		= 0;
	m_NextTTL			= 0;

	for(int i = 0; i < MAX_TTL - 1; i++)
		m_PongNeeded[i] = 0;


	// Regular ping pong
	for(i = 0; i < MAX_TTL; i++)
	{
		m_dwFriends[i]		= 0;
	}

	m_dwFriendsTotal	= 0;


	// Packet priority
	for(i = 0; i < 6; i++)
		m_PacketListLength[i] = 0;
	m_BackBuffLength	= 0;


	// Node Stats
	for(i = 0; i < PACKETCACHE_SIZE; i++)
		for(int j = 0; j < 2; j++)
			m_StatPackets[i][j] = 0;

	m_StatPos			= 0;
	// removed by moonknit 2005-07-05
	// useless
//	m_StatElements		= 0;

	for(i = 0; i < 2; i++)
	{
		m_StatPings[i]		= 0;    
		m_StatPongs[i]		= 0; 
		m_StatQueries[i]	= 0; 
		m_StatQueryHits[i]	= 0; 
		m_StatPushes[i]		= 0; 
	}

	m_ThreadCall		= false;

	// Bandwidth
	for(i = 0; i < 3; i++)
		for(int j = 0; j < 60; j++)
		{
			m_dwAvgPackets[i][j] = 0;
			m_dwAvgBytes[i][j] = 0;
		}

	for(i = 0; i < 3; i++)
	{
		m_dwTotalPackets[i] = 0;
		m_dwSecPackets[i] = 0;

		m_dwTotalBytes[i] = 0;
		m_dwSecBytes[i] = 0;
	}

	m_nSecNum			= 0;
	m_nSecPos			= 0;

	CString strPort;
	strPort.Format(_T("%d"),Port);

	m_NodeStatus		= 0;

	m_pComm->NodeMessage(SOCK_UPDATE, NULL);

#ifdef NODE_CONNECTIVITY
	// written by moonknit 2005-06-17
	// 기본 연결 강도는 최대치를 유지한다.
	m_nConnectivity		= CONNECTIVITY_LEV4;
#endif

	m_nStartTime		= GetTickCount();
	m_bValidNode		= FALSE;
}

CGnuNode::~CGnuNode()
{
	// Flush receive buffer
	byte pBuff[4096];
	while(Receive(pBuff, 4096) > 0)
		;

	if(m_hSocket != INVALID_SOCKET)
		AsyncSelect(0);
	
	
	// Add to cache if we connected
	Node PermNode;
	PermNode.Host = m_HostIP;
	PermNode.Port = m_Port;

	m_BufferAccess.Lock();

	// Clean packet stuff
	for(int i = 0; i < 6; i++)
	{
		m_PacketListLength[i] = 0;
	}

	m_BackBuffLength = 0;

	for(i = 0; i < 6; i++)
	{
		while(!m_PacketList[i].empty())
			m_PacketList[i].pop_back();
	}

	m_BufferAccess.Unlock();

	if(m_PatchTable)		
		delete [] m_PatchTable;

	if(m_CompressedTable)
		delete [] m_CompressedTable;
	
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuNode, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuNode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

// 소켓 연결이 되었으면.. Handshake 를 전송하여 준다.
void CGnuNode::OnConnect(int nErrorCode) 
{
	if(nErrorCode)
	{
		Close();
		return;
	}
	
	CString GnuVersion = GNUCLEUS_VERSION;
	CString Handshake;

	// Get Local host
	if(theApp.m_spGD->m_LocalHost.S_addr == StrtoIP(_T("127.0.0.1")).S_addr)
	{
		UINT dPort;
		CString LocalIP;
		if(GetSockName(LocalIP, dPort))
		{
			theApp.m_spGD->SetHostIP((LPTSTR) (LPCTSTR) LocalIP);
		}
		else
		{
			return;
		}
	}

	// Get Remote host
	GetPeerName(m_HostIP, m_Port);

	TRACE(_T("connection information ip : %s, port %d\r\n"), m_HostIP, m_Port);


	// 연결이 되었으니 이제.. Handshake 를 보내야 겠지..
	// 나는 Version6 이다..
	if(theApp.m_spGD->ModeVersion6 || theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE)
	{
		Handshake =  theApp.m_spGD->ModeNetwork + _T(" CONNECT/0.6\r\n");

		Handshake += _T("User-Agent: MyNapster ") + GnuVersion + _T("\r\n");

		// LAN Header
		if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE && theApp.m_spGD->m_Lan)
			Handshake += _T("LAN: ") + theApp.m_spGD->m_LanName + _T("\r\n");
	
		// Ultrapeer Header
		if(m_pComm->m_NodeCapability == CLIENT_LEAF)
			Handshake += _T("X-Ultrapeer: False\r\n");

		if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
		{
			Handshake += _T("X-Ultrapeer: True\r\n");
			Handshake += _T("X-Leaf-Max: ") + DWrdtoStr(theApp.m_spGD->m_MaxLeaves) + _T("\r\n");
		}

		// Query Routing Header
		Handshake += _T("X-Query-Routing: 0.1\r\n");

		// Uptime Header
		CTimeSpan Uptime(CTime::GetCurrentTime() - m_pComm->m_ClientUptime);
		Handshake += _T("Uptime: ") + Uptime.Format(_T("%DD %HH %MM"))+ _T("\r\n");

		// Pong Caching
		if(ACTIVATE_PONG_CACHING)
		{	
			CString CachingVersion = PONG_CACHING_VERSION;
			if(theApp.m_spGD->m_NetworkModel == NETWORK_INTERNET)
				Handshake += _T("Pong-Caching: ") + CachingVersion + _T("\r\n");
		}

#ifdef NODE_CONNECTIVITY
		// written by moonknit 2005-05-16
		// 이곳에서 현재 Node의 연결강도(Weight)를 설정하여 상대 노드로 전달한다.
		// CONNECTIVITY
		Handshake += _T("Connectivity: ") + DWrdtoStr(m_pComm->GetConnectivity(this)) + _T("\r\n");
		// -- end
#endif

		Handshake += _T("\r\n");
	}
	else
		Handshake = VERSION_4_CONNECT ;

	Send(Handshake);

	// Add to log
	Handshake.Replace(_T("\n\n"), _T("\r\n\r\n"));
	m_WholeHandshake += Handshake;

	CAsyncSocket::OnConnect(nErrorCode);
}

//=========================================================================
// NAME : OnReceive
// PARM : int nErrorCode
// RETN : void 
// DESC : 소켓이 연결된 상태에선 메시지를 주고 받고..
// DATE : ver : 1.0 , at 2003-04-03 오후 1:44:55 coded by bemlove
//=========================================================================
void CGnuNode::OnReceive(int nErrorCode) 
{
	int dwOffset = 0, dwBuffLength = 0;
	bool	bUsed = false;
	// Add any unprocessed bytes from last receive
	if(m_dwExtraLength)
	{
		memcpy(m_pBuff, m_pExtra, m_dwExtraLength);
		dwOffset = m_dwExtraLength;
		m_dwExtraLength = 0;
	}

	dwBuffLength = Receive(&m_pBuff[dwOffset], PACKET_BUFF - dwOffset);

	// Bandwidth stats
	m_dwSecBytes[0] += dwBuffLength;

	// Handle Errors
	if(!dwBuffLength || dwBuffLength == SOCKET_ERROR)
	{
		Close();
		return;
	}

	dwBuffLength += dwOffset;

	// Connected to node, sending and receiving packets
	if(m_Status == SOCK_CONNECTED)
	{
		m_dwExtraLength = SplitBundle(m_pBuff, dwBuffLength);

		if (0 != m_dwExtraLength)
		{
			if(PACKET_BUFF > m_dwExtraLength)
				memcpy(m_pExtra, &m_pBuff[dwBuffLength - m_dwExtraLength], m_dwExtraLength);
			else
				return; // Very bad, dropping packets
		}
	}

	// Still in handshake mode
	else
	{
#ifdef _UNICODE
		CString MoreData((TCHAR*) m_pBuff, dwBuffLength/2);
#else
		CString MoreData((TCHAR*) m_pBuff, dwBuffLength);
#endif
		m_InitData += MoreData;

		TRACE(_T("Received Data : %s\r\n"), MoreData);

		// Gnutella 0.4 Handshake
 		if( m_InitData.Find(VERSION_4_CONNECT_OK) != -1 
			&& !theApp.m_spGD->ModeVersion6
			)
		{
			ParseHandshake04(m_InitData, m_pBuff, dwBuffLength);
		}

		// Gnutella 0.6 Handshake
		else if( m_InitData.Find(_T("\r\n\r\n")) != -1 
			&& theApp.m_spGD->ModeVersion6
			)
		{
			if(m_Inbound)
				ParseIncomingHandshake06(m_InitData, m_pBuff, dwBuffLength);
			else
				ParseOutboundHandshake06(m_InitData, m_pBuff, dwBuffLength);
		}
		

		if(m_InitData.GetLength() > 4096)
		{
			m_WholeHandshake += m_InitData;
			Close();
		}		
	}

	CAsyncSocket::OnReceive(nErrorCode);
}


/////////////////////////////////////////////////////////////////////////////
// New connections

// Handshake의 구 버전 (0.4) 에서는 연결 가중치를 추가할 필요가 없다.
void CGnuNode::ParseHandshake04(CString Data, byte* Stream, int StreamLength)
{
	// Node making an inbound 0.4 connection
	if(m_Inbound)
	{
		if(m_pComm->m_LeafModeActive)
		{
			Close();
			return;
		}

		bool Busy = false;

		// 허용 인바운드 수를 채웠다면 더 이상 연결을 받지 않는다.
		if(theApp.m_spGD->GetMaxInConnection() > 0
			&& m_pComm->CountNormalConnects(NT_INBOUND) >= theApp.m_spGD->GetMaxInConnection())	
		{
			Close();
			return;
		}

		m_WholeHandshake += _T("GNUTELLA CONNECT/0.4\r\n");
		
		// Send Connect OK
		CString Response = VERSION_4_CONNECT_OK ;

		Send(Response);
		SetConnected();

		Response.Replace(_T("\n\n"), _T("\r\n"));
		m_WholeHandshake += Response;
	}


	// This node making an outbound 0.4 connect
	else
	{
		m_WholeHandshake += _T("GNUTELLA OK\r\n");

		// Stream begins
		for(int i = 0; i < StreamLength - 2; i++)
			if(_tcsnccmp((TCHAR*) &Stream[i], _T("\n\n"), 2) == 0)
			{
				m_dwExtraLength = StreamLength - (i + 2);
				memcpy(m_pExtra, &Stream[i + 2], m_dwExtraLength);
			}

		SetConnected();
	}
}

void CGnuNode::ParseIncomingHandshake06(CString Data, byte* Stream, int StreamLength)
{
	m_Handshake = Data.Mid(0, Data.Find(_T("\r\n\r\n")) + 4);
	m_WholeHandshake += m_Handshake;

	m_lowHandshake = m_Handshake;
	m_lowHandshake.MakeLower();


	// Check if this connection is in 'Guerilla' mode
	if(m_Handshake.Find(_T("GUERILLA ")) != -1)
	{
		m_NetworkName = _T("GUERILLA");
		m_Guerilla    = true;
	}


	// Connect string, GNUTELLA CONNECT/0.6\r\n
	if(m_Handshake.Find(m_NetworkName + _T(" CONNECT/")) != -1)
	{

		// Parse LAN header
		if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE && theApp.m_spGD->m_Lan)
		{
			if(FindHeader(_T("LAN")) != theApp.m_spGD->m_LanName)
			{
				Close();
				return;
			}
		}

		// Parse X-Query-Routing
		bool QueryRouting = false;
		CString RoutingHeader = FindHeader(_T("X-Query-Routing"));
		if(!RoutingHeader.IsEmpty() && RoutingHeader == _T("0.1"))
			QueryRouting = true;


		// Parse Uptime
		int days = 0, hours = 0, minutes = 0;
		CString UptimeHeader = FindHeader(_T("Uptime"));
		if(!UptimeHeader.IsEmpty())
		{
			_stscanf(UptimeHeader, _T("%dD %dH %dM"), &days, &hours, &minutes);
			m_HostUptime += CTimeSpan(days, hours, minutes, 0);
		}


		// Parse Pong Caching
		CString PongHeader = FindHeader(_T("Pong-Caching"));
		if(!PongHeader.IsEmpty())
			m_PongCachingEnabled = true;


		//Parse Ultrapeer header
		CString UltraHeader = FindHeader(_T("X-Ultrapeer"));
		if(!UltraHeader.IsEmpty())
		{
			UltraHeader.MakeLower();

			// Connecting client an ultrapeer
			if(UltraHeader == _T("true"))
				m_HostMode = CLIENT_ULTRAPEER;
			else
				m_HostMode = CLIENT_LEAF;
		}


		// Parse leaf max header
		CString LeafMax = FindHeader(_T("X-Leaf-Max"));		
		if(!LeafMax.IsEmpty())
			m_NodeLeafMax = _ttoi(LeafMax);
		else
			m_NodeLeafMax = 75;

		if(m_NodeLeafMax > 1500)
			m_NodeLeafMax = 1500;


		if(m_lowHandshake.Find(_T("bearshare 2.")) != -1)
			m_HostMode = CLIENT_NORMAL;

#ifdef NODE_CONNECTIVITY
		// written by moonknit 2005-05-16
		// 이 노드의 연결 강도를 설정한다.
		CString Connectivity = FindHeader(_T("Connectivity"));
		int nConnTemp;
		if(!Connectivity.IsEmpty())
			nConnTemp = _ttoi(Connectivity);

		SetConnectivity(nConnTemp);
		// -- end
#endif


		// Connecting node a SuperNode
		if(m_HostMode == CLIENT_ULTRAPEER)
		{
			// A supernode connecting to our supernode
			if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
			{
				// We are in leaf mode
				if(m_pComm->m_LeafModeActive)
				{
					if(	m_pComm->CountNormalConnects() < theApp.m_spGD->m_LeafModeConnects 
						&& QueryRouting
						)
						Send_ConnectOK(false, CLIENT_LEAF);
					else
						Send_ConnectBusy();

					return;
				}

				// We are a supernode
				else
				{
					if(theApp.m_spGD->GetMaxInConnection() > 0 
						&& m_pComm->CountNormalConnects(NT_INBOUND) >= theApp.m_spGD->GetMaxInConnection())
					{
						Send_ConnectBusy();
						return;
					}

				}
			}
			
			// A supernode connecting to our leaf
			if(m_pComm->m_NodeCapability == CLIENT_LEAF)
			{
				if(m_pComm->CountNormalConnects() < theApp.m_spGD->m_LeafModeConnects && QueryRouting)
				{
					// Send connect OK, but we are still not sure if supernode
					// really wants a leaf to connect to, so leaf mode is activated
					// in the response we get from our OK response

					Send_ConnectOK(false, m_pComm->m_NodeCapability);
				}
				else
				{
					Send_ConnectBusy();
					return;
				}

			}
		}

		// Connecting node a Leaf
		else if(m_HostMode == CLIENT_LEAF)
		{
			// A leaf connecting to our supernode
			if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
			{
				if(m_pComm->m_LeafModeActive || !QueryRouting)
				{
					Send_ConnectBusy();
					return;
				}

				m_PongCachingEnabled = false;
			}
			
			// A leaf connecting to our leaf
			if(m_pComm->m_NodeCapability == CLIENT_LEAF)
			{
				if(m_pComm->m_LeafModeActive)
				{
					Send_ConnectBusy();
					return;
				}
			}
		}

		// Connecting Node a Normal Node
		else
		{
			if(m_pComm->m_LeafModeActive)
			{
				Send_ConnectBusy();
				return;
			}

			if(theApp.m_spGD->GetMaxInConnection() > 0 
				&& m_pComm->CountNormalConnects() >= theApp.m_spGD->GetMaxInConnection())
			{
				Send_ConnectBusy();
				return;
			}

		}


		Send_ConnectOK(false, m_pComm->m_NodeCapability);
	}


	// Ok string, GNUTELLA/0.6 200 OK\r\n
	else if(m_Handshake.Find(_T(" 200 OK\r\n")) != -1)
	{
		// A SuperNode finishing its connection with our leaf
		if(m_HostMode == CLIENT_ULTRAPEER)
			if(m_pComm->m_NodeCapability == CLIENT_LEAF || m_pComm->m_LeafModeActive)
			{	
				if(!m_pComm->m_LeafModeActive)
				{
					m_pComm->m_NodeAccess.Lock();
					for(int i = 0; i < m_pComm->m_NodeList.size(); i++)
					{
						CGnuNode *p = m_pComm->m_NodeList[i];

						if(p != this && p->m_Status != SOCK_CLOSED)
							p->Close();
					}
					m_pComm->m_NodeAccess.Unlock();

					m_pComm->m_LeafModeActive = true;
				}

				m_PongCachingEnabled	  = false;
			}

		// Supernode to Supernode connect finishing
		else
		{
			//Parse Ultrapeer header
			CString UltraHeader = FindHeader(_T("X-Ultrapeer"));
			if(!UltraHeader.IsEmpty())
				UltraHeader.MakeLower();
			

			if(m_DowngradeRequest)
			{
				if(UltraHeader == _T("false"))
					m_HostMode = CLIENT_LEAF;
 				else if(theApp.m_spGD->GetMaxInConnection() > 0 
					&& m_pComm->CountNormalConnects(NT_INBOUND) >= theApp.m_spGD->GetMaxInConnection())
				{
					Close();
					return;
				}
			}
		}


		SetConnected();


		// Stream begins
		for(int i = 0; i < StreamLength - 4; i++)
		{
			if(_tcsnccmp((TCHAR*) &(Stream[i]), _T("\r\n\r\n"), 4) == 0)
			{
				m_dwExtraLength = StreamLength - (i + 4);
				memcpy(m_pExtra, &Stream[i + 4], m_dwExtraLength);
			}
		}
	}

	// Error string
	else
	{
		// Parse X-Try header
		CString TryHeader = FindHeader(_T("X-Try"));
		if(!TryHeader.IsEmpty())
			ParseTryHeader( TryHeader );


		// Parse X-Try-Ultrapeers header
		CString UltraTryHeader = FindHeader(_T("X-Try-Ultrapeers"));
		if(!UltraTryHeader.IsEmpty())
			ParseTryHeader( UltraTryHeader, true );

		Close();
		return;
	}
}

void CGnuNode::ParseOutboundHandshake06(CString Data, byte* Stream, int StreamLength)
{
	m_Handshake = Data.Mid(0, Data.Find(_T("\r\n\r\n")) + 4);
	m_WholeHandshake += m_Handshake;

	m_lowHandshake = m_Handshake;
	m_lowHandshake.MakeLower();


	// Ok string, GNUTELLA/0.6 200 OK\r\n
	if(m_Handshake.Find(_T(" 200 OK\r\n")) != -1)
	{
		// Parse Remote-IP header
		CString RemoteIP = FindHeader(_T("Remote-IP"));
		if(!RemoteIP.IsEmpty())
		{
			theApp.m_spGD->m_LocalHost = StrtoIP(RemoteIP);
		}
	

		// Parse LAN header
		if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE && theApp.m_spGD->m_Lan)
		{
			if(FindHeader(_T("LAN")) != theApp.m_spGD->m_LanName)
			{
				Close();
				return;
			}
		}

		// Parse X-Query-Routing header
		bool QueryRouting = false;
		CString RoutingHeader = FindHeader(_T("X-Query-Routing"));
		if(!RoutingHeader.IsEmpty() && RoutingHeader == _T("0.1"))
			QueryRouting = true;

		
		// Parse Uptime
		int days = 0, hours = 0, minutes = 0;
		CString UptimeHeader = FindHeader(_T("Uptime"));
		if(!UptimeHeader.IsEmpty())
		{
			_stscanf(UptimeHeader, _T("%dD %dH %dM"), &days, &hours, &minutes);
			m_HostUptime += CTimeSpan(days, hours, minutes, 0);
		}


		// Parse Pong-Caching header
		CString PongHeader = FindHeader(_T("Pong-Caching"));
		if(!PongHeader.IsEmpty() && PongHeader == _T("0.1"))
			m_PongCachingEnabled = true;


		// Parse Ultrapeer header
		CString UltraHeader = FindHeader(_T("X-Ultrapeer"));		
		if(!UltraHeader.IsEmpty())
		{
			UltraHeader.MakeLower();

			if(UltraHeader == _T("true"))
				m_HostMode = CLIENT_ULTRAPEER;
			else
				m_HostMode = CLIENT_LEAF;
		}

		// Parse leaf max header
		CString LeafMax = FindHeader(_T("X-Leaf-Max"));		
		if(!LeafMax.IsEmpty())
			m_NodeLeafMax = _ttoi(LeafMax);
		else
			m_NodeLeafMax = 75;

		if(m_NodeLeafMax > 1500)
			m_NodeLeafMax = 1500;


		if(m_lowHandshake.Find(_T("bearshare 2.")) != -1)
			m_HostMode = CLIENT_NORMAL;

#ifdef NODE_CONNECTIVITY
		// written by moonknit 2005-05-16
		// 이 노드의 연결 강도를 설정한다.
		CString Connectivity = FindHeader(_T("Connectivity"));
		int nConSrc = 0;
		if(!Connectivity.IsEmpty())
			nConSrc = _ttoi(Connectivity);

		SetConnectivity(nConSrc);;
		// -- end
#endif


		// If Remote host a SuperNode
		if(m_HostMode == CLIENT_ULTRAPEER)
		{

			// Our Leaf connecting to a SuperNode
			if(m_pComm->m_NodeCapability == CLIENT_LEAF)
			{
				if(!QueryRouting)
				{
					Close();
					return;
				}

				// If leaf mode not active, disconnect from everyone
				if(!m_pComm->m_LeafModeActive)
				{
					m_pComm->m_NodeAccess.Lock();
					for(int i = 0; i < m_pComm->m_NodeList.size(); i++)
					{
						CGnuNode *p = m_pComm->m_NodeList[i];

						if(p != this && p->m_Status != SOCK_CLOSED)
							p->Close();
					}
					m_pComm->m_NodeAccess.Unlock();
					
					m_pComm->m_LeafModeActive = true;
				}
				
				m_PongCachingEnabled = false;
			}


			// Our SuperNode connecting to another SuperNode
			else if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
			{

				// Parse the Ultrapeers Needed header
				CString NeededHeader = FindHeader(_T("X-Ultrapeer-Needed"));
				NeededHeader.MakeLower();


				// This SuperNode wants more leaves
				if(NeededHeader == _T("false") && QueryRouting && !m_pComm->m_ForcedSupernode)
				{
					// We are already a leaf
					if(m_pComm->m_LeafModeActive)
					{
						m_DowngradeRequest = true;
						Send_ConnectOK(true, CLIENT_LEAF);
					}

					// We are not a leaf so check if we can be
					else
					{		
						// If we are handling less than 33% of what they can handle, downgrade
						// If our uptime is longer do not become leaf unless real firewall is false
						// commented by moonknit 2005-05-11
/*
						if(m_HostUptime > CTime::GetCurrentTime() - m_pComm->m_ClientUptime)
							if(m_pShare->RunningCapacity(m_NodeLeafMax) < 33)
							{
								m_DowngradeRequest		  = true;

								for(int i = 0; i < m_pComm->m_NodeList.size(); i++)
								{
									CGnuNode *p = m_pComm->m_NodeList[i];

									if(p != this && p->m_Status != SOCK_CLOSED)
										p->Close();
								}
								
								m_pComm->m_LeafModeActive = true;	
								m_PongCachingEnabled	  = false;
							}
*/
				
					}
				}

				// SuperNode does not need more leaves
				else
				{
					if(m_pComm->m_LeafModeActive)
					{
						// Super Node 사용안함
						if(!theApp.m_spGD->m_SuperNodeAble)
						{
							Send_ConnectBusy();
							return;
						}
						// Become a supernode 
						else
						{
							m_pComm->m_NodeAccess.Lock();
							for(int i = 0; i < m_pComm->m_NodeList.size(); i++)
							{
								CGnuNode *p = m_pComm->m_NodeList[i];

								if(p != this && p->m_Status != SOCK_CLOSED)
									p->Close();
							}
							m_pComm->m_NodeAccess.Unlock();
							
							m_pComm->m_LeafModeActive = false;
						}
					}
					else
					{
						// normal connect
					}
				}
			}	
		}
		
		// Remote client a leaf
		else if(m_HostMode == CLIENT_LEAF)
		{
			// Our Leaf connecting to another Leaf
			if(m_pComm->m_NodeCapability == CLIENT_LEAF)
			{
				if(m_pComm->m_LeafModeActive)
				{
					Send_ConnectBusy();
					return;
				}
			}


			// Our Supernode connecting to a Leaf
			if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
			{
				if(m_pComm->m_LeafModeActive || !QueryRouting)
				{
					Send_ConnectBusy();
					return;
				}

				m_PongCachingEnabled = false;
			}	
		}
		

		// Ultrapeer header not found, we are connecting to a normal node
		else
		{
			if(m_pComm->m_LeafModeActive)
			{
				Send_ConnectBusy();
				return;
			}
		}

		Send_ConnectOK(true, m_pComm->m_NodeCapability);
		SetConnected();
	}


	// Connect failed, 200 OK not received
	else
	{ 
		// Parse X-Try header
		CString TryHeader = FindHeader(_T("X-Try"));
		if(!TryHeader.IsEmpty())
			ParseTryHeader( TryHeader );


		// Parse X-Try-Ultrapeers header
		CString UltraTryHeader = FindHeader(_T("X-Try-Ultrapeers"));
		if(!UltraTryHeader.IsEmpty())
			ParseTryHeader( UltraTryHeader, true );

		
		Close();
		return;
	}
}

void CGnuNode::ParseTryHeader(CString TryHeader, bool Super)
{
	TryHeader += _T(",");
	TryHeader.Replace(_T(",,"), _T(","));
	TryHeader.Remove(_T(' '));
	
	int tryFront = 0, 
		tryMid = TryHeader.Find(_T(":")), 
		tryBack = TryHeader.Find(_T(","));
	
	while(tryBack != -1 && tryMid != -1)
	{
		Node tryNode;
		tryNode.Host = TryHeader.Mid(tryFront, tryMid - tryFront);
		tryNode.Port = _ttoi( TryHeader.Mid(tryMid + 1, tryBack - tryMid + 1));

//		m_pCache->UpdateCache(tryNode, Super);

		tryFront  = tryBack + 1; 
		tryMid    = TryHeader.Find(_T(":"), tryFront);
		tryBack   = TryHeader.Find(_T(","), tryFront);
	}
}

CString CGnuNode::FindHeader(CString Name)
{	
	CString Data;
	
	Name += ":";
	Name.MakeLower();
	
	int keyPos = m_lowHandshake.Find(Name);

	if (keyPos != -1)
	{
		keyPos += Name.GetLength();

		Data = m_Handshake.Mid(keyPos, m_Handshake.Find(_T("\r\n"), keyPos) - keyPos);
		Data.TrimLeft();
	}
	
	return Data;
}

//=========================================================================
// NAME : Send_ConnectOK
// PARM : bool, int
// RETN : void
// DESC : 각 버젼/상황별로 Handshake에 대한 패킷분석이 정상이므로, 연결이 되었음을 전달한다.
// DATE : ver : 1.0 , at 2003-04-08 오전 11:23:07 described by bemlove
//=========================================================================
void CGnuNode::Send_ConnectOK(bool Reply, int HeaderMode)
{
	CString Handshake;
	CString GnuVersion = GNUCLEUS_VERSION;


	// Reply to CONNECT OK
	if(Reply)
	{
		Handshake = m_NetworkName + _T("/0.6 200 OK\r\n");

		
		// We are converting from supernode to a leaf
		if(m_DowngradeRequest)
			Handshake += _T("X-Ultrapeer: False\r\n");


		Handshake += _T("\r\n");
	}

	// Sending initial CONNECT OK
	else
	{
		Handshake =  m_NetworkName + _T("/0.6 200 OK\r\n");
		Handshake += _T("User-Agent: MyNapster ") + GnuVersion + _T("\r\n");


		// LAN header
		if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE && theApp.m_spGD->m_Lan)
			Handshake += _T("LAN: ") + theApp.m_spGD->m_LanName + _T("\r\n");


		// Remote IP header
		UINT nTrash;
		GetPeerName(m_HostIP, nTrash);
		Handshake += _T("Remote-IP: ") + m_HostIP + _T("\r\n");

		// Query Routing Header
		Handshake += _T("X-Query-Routing: 0.1\r\n");

		// Ultrapeer header
		if(HeaderMode == CLIENT_ULTRAPEER)
		{
			Handshake += _T("X-Ultrapeer: True\r\n");
			Handshake += _T("X-Leaf-Max: ") + DWrdtoStr(theApp.m_spGD->m_MaxLeaves) + _T("\r\n");

			if(m_DowngradeRequest)
				Handshake += _T("X-Ultrapeer-Needed: False\r\n");
		}
		else
			Handshake += _T("X-Ultrapeer: False\r\n");
		

		// Uptime header
		CTimeSpan Uptime(CTime::GetCurrentTime() - m_pComm->m_ClientUptime);
		Handshake += _T("Uptime: ") + Uptime.Format(_T("%DD %HH %MM"))+ _T("\r\n");

		
		// Pong-Caching header
		if(m_PongCachingEnabled)
		{
			CString CachingVersion = PONG_CACHING_VERSION;
			if(theApp.m_spGD->m_NetworkModel == NETWORK_INTERNET)
				Handshake += _T("Pong-Caching: ") + CachingVersion + _T("\r\n");
		}

		// X-Try header
		CString HostsToTry;
		if(GetAlternateHostList(HostsToTry))
			Handshake += _T("X-Try: ") + HostsToTry + _T("\r\n");


		// X-Try-Ultrapeers header
		CString SuperHostsToTry;
		if(GetAlternateSuperList(SuperHostsToTry))
			Handshake += _T("X-Try-Ultrapeers: ") + SuperHostsToTry + _T("\r\n");

#ifdef NODE_CONNECTIVITY		
		// written by moonknit 2005-05-16
		// 이곳에서 현재 Node의 연결강도(Weight)를 설정하여 상대 노드로 전달한다.
		// CONNECTIVITY
		Handshake += _T("Connectivity: ") + DWrdtoStr(m_pComm->GetConnectivity(this)) + _T("\r\n");
		// -- end
#endif

		Handshake += _T("\r\n");
	}


	Send(Handshake);
	
	Handshake.Replace(_T("\n\n"), _T("\r\n"));
	m_WholeHandshake += Handshake;
}

void CGnuNode::Send_ConnectBusy()
{
	CString Handshake;
	CString GnuVersion = GNUCLEUS_VERSION;


	Handshake =  m_NetworkName + _T("/0.6 503 BUSY\r\n");
	Handshake += _T("User-Agent: MyNapster ") + GnuVersion + _T("\r\n");


	// LAN header
	if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE && theApp.m_spGD->m_Lan)
		Handshake += _T("LAN: ") + theApp.m_spGD->m_LanName + _T("\r\n");


	// Remote-IP header
	UINT nTrash;
	GetPeerName(m_HostIP, nTrash);
	Handshake += _T("Remote-IP: ") + m_HostIP + _T("\r\n");


	// X-Try header
	CString HostsToTry;
	if(GetAlternateHostList(HostsToTry))
		Handshake += _T("X-Try: ") + HostsToTry + _T("\r\n");


	// X-Try-Ultrapeers header
	CString SuperHostsToTry;
	if(GetAlternateSuperList(SuperHostsToTry))
		Handshake += _T("X-Try-Ultrapeers: ") + SuperHostsToTry + _T("\r\n");


	Handshake += _T("\r\n");

	Send(Handshake);
	
	Handshake.Replace(_T("\n\n"), _T("\r\n"));
	m_WholeHandshake += Handshake;
}

 //노드를 연결된 상태로 설정
void CGnuNode::SetConnected()
{
	// Get Remote host
	CString HostIP;
	UINT Port;
	GetPeerName(m_HostIP, Port);

	m_Status = SOCK_CONNECTED;
	m_pComm->NodeMessage(WM_NODEUPDATE, NULL);


	// We are a leaf send index to supernode
	if(m_pComm->m_LeafModeActive)
		m_PatchUpdateNeeded  = true;
	// Send_Ping(7);			
}

// 패킷의 전송
void CGnuNode::SendPacket(void* packet, int length, int type, bool local, bool thread)
{

	std::list<PriorityPacket>::iterator itPacket;

	// UNICODE의 경우 버퍼를 2배로 먹는다.
	if(length > (65536 / sizeof(TCHAR)) || length <= 0 || type > 5) return;

	if(local)
		type = 0;
	
	m_BufferAccess.Lock();

	// Remove old packets if back list too large (does not apply to local packets)
	if(type != 0 && m_PacketList[type].size() > 50)
		while(m_PacketList[type].size() > 25)
		{
			itPacket = m_PacketList[type].end();
			itPacket--;

			m_PacketListLength[type] -= (*itPacket).Length;
			m_PacketList[type].pop_back();

			ASSERT(m_PacketListLength[type] >= 0);
		}

	ASSERT(packet);

	// Build a priority packet
	PriorityPacket SendPacket(type, length);	
	m_PacketListLength[type] += length;
	m_PacketList[type].push_front(SendPacket);

	itPacket = m_PacketList[type].begin();
	(*itPacket).Packet = new byte[length];
	memcpy((*itPacket).Packet, packet, length);

	if(thread)
		m_ThreadCall = true;
	else
		FlushSendBuffer();

	m_BufferAccess.Unlock();

}

void CGnuNode::FlushSendBuffer()
// *** Must be called with in a BufferAccess Lock
{
	std::list<PriorityPacket>::iterator itPacket;

	int BytesSent   = 0,
		AttemptSend = 0,
		Length		= 0;

	if(m_Status != SOCK_CONNECTED)
		return;

	// Check if we can send
	if(!m_SendReady)
		return;

	// Send back buffer
	while(m_BackBuffLength > 0)
	{
		AttemptSend = Send(m_BackBuff + BytesSent, m_BackBuffLength);

		if(AttemptSend == SOCKET_ERROR || AttemptSend == 0)
		{
			if(GetLastError() != WSAEWOULDBLOCK)
				Close();
			else
				m_SendReady = false;


			memmove(m_BackBuff, m_BackBuff + BytesSent, m_BackBuffLength);

			return;
		}
		else
		{
			BytesSent += AttemptSend;
			m_BackBuffLength -= AttemptSend;
		}

//		TRACE("Remain Buffer : %d bytes, flush Buffer : %d bytes \n", m_BackBuffLength, BytesSent);
	}

	// Send prioritized buffer
	std::list<PriorityPacket>::iterator itType;

	// Each Packet type
	for(int i = 0; i < 6; i++)
		// while list not empty
		while(!m_PacketList[i].empty())
		{
			itPacket = m_PacketList[i].begin();

			AttemptSend = 0;
			BytesSent   = 0;
			Length      = (*itPacket).Length;

			ASSERT(Length >= 0 && Length < 65536);

			if(Length + m_BackBuffLength >= 65536) continue;		// flushing 전에는 고용량 패킷은 그냥 버린다.

			// Send packet
			while(BytesSent < Length)
			{
				AttemptSend = Send((*itPacket).Packet + BytesSent, Length - BytesSent);
				
				// If send fails, copy to back buffer so it is the first to be sent
				if(AttemptSend == SOCKET_ERROR || AttemptSend == 0)
				{
					if(GetLastError() != WSAEWOULDBLOCK)
					{
						Close();
						return;
					}
					else 
						m_SendReady = false;

					m_dwSecPackets[2]++;
					m_dwSecBytes[2] += Length - BytesSent;

					memcpy(m_BackBuff, (*itPacket).Packet + BytesSent, Length - BytesSent);
					m_BackBuffLength = Length - BytesSent;
					

					// Delete packet once copied
					m_PacketListLength[i] -= (*itPacket).Length;
					m_PacketList[i].pop_front();

					m_dwSecPackets[1]++;

					return;
				}
				else
					BytesSent += AttemptSend;
			}

			// Delete packet once sent
			m_PacketListLength[i] -= (*itPacket).Length;
			m_PacketList[i].pop_front();

			m_dwSecPackets[1]++;
		}	
}

int CGnuNode::Send(const void* lpBuf, int nBuffLen, int nFlags) 
{
	int BytesSent = 0;

	BytesSent = CAsyncSocket::Send(lpBuf, nBuffLen, nFlags);
			
	if(BytesSent > 0)
		m_dwSecBytes[1] += BytesSent;

	return BytesSent;
}

int CGnuNode::Send(CString& szBuf, int nFlags)
{
	return Send(szBuf, szBuf.GetLength() * sizeof(TCHAR), nFlags);
}

void CGnuNode::OnSend(int nErrorCode) 
{
	m_SendReady = true;
	
	m_BufferAccess.Lock();
	FlushSendBuffer();
	m_BufferAccess.Unlock();

	CAsyncSocket::OnSend(nErrorCode);
}

void CGnuNode::OnClose(int nErrorCode) 
{
	Close();

	CAsyncSocket::OnClose(nErrorCode);
}

void CGnuNode::Close() 
{
	if(m_hSocket != INVALID_SOCKET)
	{
		// Close socket
		AsyncSelect(0);
		ShutDown(1);

		CAsyncSocket::Close();
	}
		

	m_Status = SOCK_CLOSED;
	m_pComm->NodeMessage(WM_NODEUPDATE, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// Packet handlers

DWORD CGnuNode::SplitBundle(byte* bundle, DWORD length)
{
	UINT Payload = 0;
	UINT nextPos = 0;

	packet_Header* packet;

	enum status { status_DONE,       status_CONTINUE, 
				  status_BAD_PACKET, status_INCOMPLETE_PACKET };

	status theStatus = status_CONTINUE;

	do
	{
		if (nextPos + sizeof(packet_Header) > length)
			theStatus = status_INCOMPLETE_PACKET;
		else
		{
			packet = (packet_Header*) (bundle + nextPos);

			Payload = packet->Payload; 
		 
			if((packet->Function == 0x10 && Payload ==  0)					||
			   (packet->Function == 0x11 && Payload == 14)					||
			   (packet->Function == 0x30 && Payload > 2 && Payload < 32768) ||
			   (packet->Function == 0x40 && Payload == 26)					||
			   (packet->Function == 0x80 && Payload >  2 && Payload <= MAX_BUFF) ||
			   (packet->Function == 0x81 && Payload > 26 && Payload <= 32768))
			{
				if (nextPos + sizeof(packet_Header) + Payload <= length)
				{
					HandlePacket(packet, 23 + Payload);		// 23 means header +
					
					nextPos += 23 + Payload;
					if (nextPos == length)
						theStatus = status_DONE;
				}
				else
					theStatus = status_INCOMPLETE_PACKET;
			}
			else
			{
		        if (nextPos < length - sizeof(packet_Header))
					nextPos++;
		        else   
					theStatus = status_BAD_PACKET;
			}
		}
	} while(status_CONTINUE == theStatus);


	return length - nextPos;
}

void CGnuNode::HandlePacket(packet_Header* packet, DWORD length)
{
	m_dwSecPackets[0]++;

	switch(packet->Function)
	{
	case 0x10:
		Recieve_Ping((packet_Ping*) packet, length);
		break;

	case 0x11:
		Recieve_Pong((packet_Pong*) packet, length);
		break;
	case 0x30:
		if( ((packet_RouteTableReset*) packet)->PacketType == 0x0)
			Recieve_RouteTableReset((packet_RouteTableReset*) packet, length);
		else if(((packet_RouteTableReset*) packet)->PacketType == 0x1)
			Recieve_RouteTablePatch((packet_RouteTablePatch*) packet, length);

		break;
	case 0x40:
		Recieve_Push((packet_Push*) packet, length);
		break;

	case 0x80:
		Recieve_Query((packet_Query*) packet, length);
		break;

	case 0x81:
		Recieve_QueryHit((packet_QueryHit*) packet, length);
		break;

	default:
		// Disable unknowns
		// Recieve_Unknown((byte *) packet, length);
		break;
	}
}

bool CGnuNode::InspectPacket(packet_Header* packet)
{
	if(packet->TTL == 0 ||  packet->Hops >= MAX_TTL)
		return false;


	packet->Hops++; 
	packet->TTL--;


	// Reset TTL if too high
	if(packet->TTL >= MAX_TTL)
		packet->TTL = MAX_TTL - packet->Hops;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Receiving packets
// QueryHit에만 LOG가 사용되고 있는 상태.
void CGnuNode::BuildLog(packet_Log* Log, packet_Header* packet, int length)
{
	Log->Header		  = packet;
	Log->PacketLength = length;
	Log->Origin		  = this;
	Log->dwError	  = 0;
	Log->nHops		  = packet->Hops + 1;
	Log->nTTL		  = packet->TTL ? packet->TTL - 1 : 0;
}

int CGnuNode::UpdateStats(int type, int &StatCounter)
{
	// removed by moonknit 2005-07-05
	// useless
//	if(m_StatElements < PACKETCACHE_SIZE)
//		m_StatElements++;
	if(m_StatPos == PACKETCACHE_SIZE)
		m_StatPos = 0;

	int StatPos = m_StatPos;

	RemovePacket(m_StatPos);
	
	m_StatPackets[m_StatPos][0] = type;
	m_StatPos++;
	StatCounter++;

	return StatPos;
}

void CGnuNode::Recieve_Ping(packet_Ping* Ping, int nLength)
{
	// Stats
	int StatPos = UpdateStats(PACKET_PING, m_StatPings[0]);


	// Inspect
	if(!InspectPacket(&Ping->Header))
	{
		return;
	}

	key_Value* key		= m_pComm->m_TableRouting.FindValue(&Ping->Header.Guid);
	key_Value* LocalKey = m_pComm->m_TableLocal.FindValue(&Ping->Header.Guid);

	if(LocalKey)		// LOOPBACK
	{
		return;
	}

	// Fresh Ping?
	if(key == NULL)
	{
		m_pComm->m_TableRouting.Insert(&Ping->Header.Guid, this);
		
		Send_Pong(Ping->Header.Guid, Ping->Header.Hops);
		// 브로드 캐스팅 부분.
		// Broadcast if still alive
		if(Ping->Header.Hops < MAX_TTL && Ping->Header.TTL > 0)
		{
			m_pComm->Broadcast_Ping(Ping, nLength, this);
		}
		
		m_StatPackets[StatPos][1] = 1;
		m_StatPings[1]++;
		return;
	}
	else	// Packet Duplicate, Error Routing
	{
		return;
	}
}


void CGnuNode::Recieve_Pong(packet_Pong* Pong, int nLength)
{
	// Stats
	int StatPos = UpdateStats(PACKET_PONG, m_StatPongs[0]);


	// Inspect
	if(!InspectPacket(&Pong->Header))
	{
		return;
	}

	key_Value* key		= m_pComm->m_TableRouting.FindValue(&Pong->Header.Guid);
	key_Value* LocalKey = m_pComm->m_TableLocal.FindValue(&Pong->Header.Guid);

	// 들어온 Pong을 stlCache에 저장한다.
	if ( !m_pComm->CheckCache( Pong->Host ) )
	{
		PongCache pong_cache;
		pong_cache.HostIP = Pong->Host;
		pong_cache.Port   = Pong->Port;
		pong_cache.Hop	  = Pong->Header.Hops;
		m_pComm->m_stlPongCache.push_back( pong_cache );
	}
		
	if(LocalKey)
	{
		// If this pong is one we sent out
		if(LocalKey->Origin != NULL)
		{
			if(LocalKey->Origin == this) // LoopBack
			{
				return;
			}
		}
		else
		{
			// Nodes file count
			if(Pong->Header.Hops == 1)
			{
				m_NodeFileCount = Pong->FileCount;
				m_NodeLeafCount	= Pong->LeafCount;
#ifdef NODE_CONNECTIVITY
				if(IsValidNode())
					SetConnectivity(m_NodeLeafCount);
#endif 
			}


			// Pong Caching 
			if(m_PongCachingEnabled && Pong->Header.Hops < MAX_TTL - 1)
			{
				MapPongList AddPong;
				AddPong.PrimNode	= this;
				AddPong.Host		= Pong->Host;
				AddPong.Port		= Pong->Port;
				AddPong.FileCount	= Pong->FileCount;
				AddPong.LeafCount	= Pong->LeafCount;
				AddPong.State		= Pong->State;
				
				m_pComm->AddToPongCache(&AddPong, Pong->Header.Hops);
				Check_PongNeeds(&AddPong, Pong->Header.Hops);
			}

			// Mapping
			if(theApp.m_spGD->m_LocalMapping)
				MapPong(Pong, false);

			if(Pong->Header.Hops < 3)
				MapPong(Pong, true);

			m_dwFriendsTotal++;
			m_dwFriends[Pong->Header.Hops - 1]++;

			// removed by moonknit 2005-12-23
			// other code of these variables was also been removed
			// if you want restore these code, look into gnucleus origial
			// m_dwLibrary, m_dwLibraryTotal, m_dwSharingTotal
/*			DWORD dwLibrary = 0;
			dwLibrary = (MAX_SHARE < Pong->FileSize) ? MAX_SHARE : Pong->FileSize;
			if(dwLibrary)
			{
				m_dwLibrary[Pong->Header.Hops - 1] += dwLibrary;
				m_dwLibraryTotal += dwLibrary;
				m_dwSharingTotal++;
			}
*/
			m_StatPackets[StatPos][1] = 1;
			m_StatPongs[1]++;
			return;
		}
	}

	if(key)
	{	
		// Send it out	
		if(Pong->Header.Hops < MAX_TTL && Pong->Header.TTL > 0)
			m_pComm->Route_Pong(Pong, nLength, key);

		m_StatPackets[StatPos][1] = 1;
		m_StatPongs[1]++;
		return;
	}
}


void CGnuNode::Recieve_Push(packet_Push* Push, int nLength)
{
	// Stats
	int StatPos = UpdateStats(PACKET_PUSH, m_StatPushes[0]);


	// Host Cache
	Node NetNode;
	NetNode.Host		= IPtoStr(Push->Host);
	NetNode.Port		= Push->Port;


	// Inspect
	if(!InspectPacket(&Push->Header))
	{
		return;
	}


	// Find packet in hash tables
	key_Value* key = m_pComm->m_TableRouting.FindValue(&Push->Header.Guid);
	key_Value* LocalKey = m_pComm->m_TableLocal.FindValue(&Push->Header.Guid);
	key_Value* PushKey  = m_pComm->m_TablePush.FindValue(&Push->ServerID);

	if(LocalKey) // LoopBack
	{
		return;
	}

	int i = 0;

	// Check ServerID of Push with ClientID of the client
	if(m_pComm->m_ClientID == Push->ServerID)
	{
		CGnuUploadShell* Upload = NULL;

		std::list<CGnuUploadShell*>::iterator it;

		// Make sure not already pushing file
		for(it = m_pComm->m_UploadList.begin(); it != m_pComm->m_UploadList.end(); ++it)
		{
			CGnuUploadShell* p = (*it);

			if(p->m_Host.S_addr == Push->Host.S_addr && p->m_FileIndex == Push->Index)
			{
				p->m_Attempts++;

				if(p->m_Socket != NULL)
					return;
				else if(p->m_FileIndex == Push->Index)
					Upload = p;
			}
		}

		// 이미 진행중인 Upload에 포함되어 있지 않다면.. 새롭게 생성하여 준다.
		if(!Upload)
		{
			Upload = new CGnuUploadShell();

			if( Upload == NULL ) 
			{
				 return;
			}
		
			m_pComm->m_UploadAccess.Lock();
			m_pComm->m_UploadList.push_back(Upload);
			m_pComm->m_UploadAccess.Unlock();
			Upload->m_Host			= Push->Host;
			Upload->m_Port			= Push->Port;

			// 파일 공유의 FileIndex 의 정의가 필요하다.
			Upload->m_FileIndex		= Push->Index;

			Upload->m_nPremiumLevel = 1;		// Push를 쓰는 사람은 모두 프리미엄 유저라고 본다.

			Upload->PushFile();
			m_StatPackets[StatPos][1] = 1;
			m_StatPushes[1]++;

		}
		return;
	}

	if(key == NULL)
	{
		m_pComm->m_TableRouting.Insert(&Push->Header.Guid, this);
	}
	else		// LOG -> Duplicate, error routing
	{
		return;
	}

	if(PushKey)
	{	
		if(Push->Header.Hops < MAX_TTL && Push->Header.TTL > 0)
			m_pComm->Route_Push(Push, nLength, PushKey);
		
		m_StatPackets[StatPos][1] = 1;
		m_StatPushes[1]++;
		return;	
	}
	else
	{
		return;
	}
}

// Broadcast_LocalQuery 와 내용을 맞춘다.
void CGnuNode::Recieve_Query(packet_Query* Query, int nLength)
{
	// Stats
	int StatPos = UpdateStats(PACKET_QUERY, m_StatQueries[0]);

	// Inspect
	int QuerySize  = Query->Header.Payload - 2;
	int TextSize   = _tcslen((TCHAR*) ((byte*) Query + 25)) + 1;

	// Bad packet, text bigger than payload
	if (TextSize > QuerySize)
	{
		return;
	}
	

	// 쿼리의 TTL 과 Hops를 검사한다.
	if(!InspectPacket(&Query->Header))
	{
		return;
	}


	// 이곳에서 검색어와 ID를 분리한다.
	//휘슬milky0821

	key_Value* key = m_pComm->m_TableRouting.FindValue(&Query->Header.Guid);
	key_Value* LocalKey = m_pComm->m_TableLocal.FindValue(&Query->Header.Guid);
	
	// 자신이 보낸 패킷인지 검사 후 자신이 보낸 패킷이면 로그에 남기고 함수를 나온다.
	if(LocalKey)
	{
		return;
	}

	// 지나왔던 패킷인가를 검사 후 지나갔던 패킷이면 함수를 나간다.
	// Fresh Query?
	if(key == NULL)
	{
		m_pComm->m_TableRouting.Insert(&Query->Header.Guid, this);
		
		// 패킷이 살아 있으면 브로트 캐스팅 쿼리
		if(Query->Header.Hops < MAX_TTL && Query->Header.TTL > 0)
			m_pComm->Broadcast_Query(Query, nLength, this);


		m_StatPackets[StatPos][1] = 1;
		m_StatQueries[1]++;

		byte* pSearch = (byte*) &Query->Header;

		////////////////////////////////////////////////////////////////////////
		// 클럽아이디 부분
		 
		CString BeforeParsing( (TCHAR*) ((byte*) Query + 25) ); // 검색전의 Query
		CString LoginID;
		CString	tmpString;
		tmpString = _T("");
		int nIndexParsing = 0;

		bool bNewQuery = false;

		int ntype = 0;
		int nmaxcnt = 0;
		CString sword = _T("");
		CString sterm = _T("");

		SE_QUERY	querydata;

		BeforeParsing += _T("");

		for ( int i = 0 ; i < BeforeParsing.GetLength() ; i++ )
		{
			if ( BeforeParsing.GetAt(i) != _T('') ) 
			{
				if( BeforeParsing.GetAt(i) == _T('|'))
				{
					bNewQuery = true;
				}
				else
					tmpString += BeforeParsing.GetAt(i);
			}
			else
			{
				if( (nIndexParsing == 2 || nIndexParsing == 3)		// 클럽아이디 추출중
					&& bNewQuery									// New Query
					)
				{
					nIndexParsing = 4;
				}

				// 검색어 추출
				if ( nIndexParsing == 0 ) 
				{ 
					sword = tmpString;

					tmpString = _T("");

					++nIndexParsing;
					continue;
				}

				// 로그인 아이디 추출
				if ( nIndexParsing == 1 ) 
				{ 
					LoginID	= tmpString;

					querydata.userid = tmpString;

					tmpString = _T("");

					++nIndexParsing;
					
					continue;
				}

				// 검색 종류 추출
				if ( nIndexParsing == 2 )
				{
					// 검색 종류
					ntype = _ttoi(tmpString);

					tmpString = _T("");
					++nIndexParsing;
					continue;
				}

				// 최대 검색 개수 추출
				if ( nIndexParsing == 3 )
				{
					nmaxcnt = _ttoi(tmpString);

					tmpString = _T("");
					++nIndexParsing;
					
					continue;
				}

				// 검색 기간 추출
				if ( nIndexParsing == 4 )
				{
					sterm = tmpString;

					tmpString = _T("");
					++nIndexParsing;

					break;
				}
				
			}
		}


		////////////////////////////////////////////////////////////////////////

		querydata.Origin	= this;
		querydata.nHops     = Query->Header.Hops;
		querydata.QueryGuid = Query->Header.Guid;

		// PIPE : Node 2
		// 공유 쓰레드 큐에 쿼리를 넣어준다.
		// 쿼리를 던져준다.
		// Send_QueryHit을 보낼수 있다.
		theApp.m_spRD->IRequestNetSearch((SEARCH_TYPE) ntype, sword, _T(""), sterm, nmaxcnt, querydata);
		return;
	}
	else
	{
		return;
	}
}

// Send_QueryHit과 내용을 맞춘다.
void CGnuNode::Recieve_QueryHit(packet_QueryHit* QueryHit, DWORD dwLength)
{
	static UINT count=0;
	// Log
	packet_Log Log;
	BuildLog(&Log, &QueryHit->Header, dwLength);
	
	// Stats
	int StatPos = UpdateStats(PACKET_QUERYHIT, m_StatQueryHits[0]);


	// Host Cache
	Node NetNode;
	NetNode.Host		= IPtoStr(QueryHit->Host);
	NetNode.Port		= QueryHit->Port;
 	int hops = QueryHit->Header.Hops;
	// 검색결과의 host 정보를 Node Cache map 에 추가
	theApp.m_spGD->m_nodeCacheMap.Push_back((LPTSTR)(LPCTSTR)NetNode.Host, NetNode.Port, hops);

	// Inspect
	if(!InspectPacket(&QueryHit->Header))
	{
		return;
	}
	key_Value* key		= m_pComm->m_TableRouting.FindValue(&QueryHit->Header.Guid);
	key_Value* LocalKey = m_pComm->m_TableLocal.FindValue(&QueryHit->Header.Guid);

	if(LocalKey)
	{
		int i = 0;

		// Check for query hits we sent out
		if(LocalKey->Origin != NULL)
		{
			return;
		}
		else
		{
			m_StatPackets[StatPos][1] = 1;
			m_StatQueryHits[1]++;

			if(theApp.m_spGD->m_LocalMapping)
				MapQueryHit(QueryHit);
			
			// 이녀석으로 QueryHit 파싱 처리한다.
			ParseQueryHit(&Log);
			// -- end

			// removed by moonknit 2006-01-09
			// move to the ParseQueryHit
			// Look for matches in current downloads
//			for(i = 0; i < m_pComm->m_DownloadList.size(); i++)
//			{
//				if(QueryHit->Header.Guid == m_pComm->m_DownloadList[i]->m_SearchGuid)
//				{
//					// 이미 파싱 작업을 수행했으므로 파싱된 결과를 넘긴다.
//					// 노드 추가
//					m_pComm->m_DownloadList[i]->IncomingQueryHit(&Log);
//				}
//			}
		
			return;
		}
	}

	if(key)
	{	
		// Add ClientID of packet to push table
		if(!m_pComm->m_TablePush.FindValue((GUID*) ((byte*)QueryHit + (dwLength - 16))))
			m_pComm->m_TablePush.Insert( (GUID*) ((byte*)QueryHit + (dwLength - 16)), this);
		
		// Send it out
		if(QueryHit->Header.Hops < MAX_TTL && QueryHit->Header.TTL > 0)
			m_pComm->Route_QueryHit(QueryHit, dwLength, key);

		m_StatPackets[StatPos][1] = 1;
		m_StatQueryHits[1]++;

		// Mapping
		if(theApp.m_spGD->m_LocalMapping)
			MapQueryHit(QueryHit);
		
		return;
	}
	else
	{
		return;
	}  
}

// written by		: moonknit
// name				: CGnuNode::ParseQueryHit
// created			: 2005-05-17
// last updated		: 2005-05-17
// desc				:
// Query Hit을 파싱하여 검색결과를 DSRM으로 전달한다.
void CGnuNode::ParseQueryHit(packet_Log* pPacket)
{
#ifdef USE_UTFCONVERT
	TCHAR wzbuf[4096];
	char* pbuf;
	int u16len = 0;
	int u8len = 0;
#endif

	int Whole;
	int Current;
	Whole = 0;
	Current  = 0;
 
	packet_Log*      Log      = pPacket;
	packet_QueryHit* QueryHit = (packet_QueryHit*) Log->Header;
	byte*			 Packet   = (byte*) QueryHit;

	bool ExtendedPacket = false;
	bool Firewall		= false;
	bool Busy			= false;
	bool Stable			= false;
	bool ActualSpeed	= false;
	
	// VERSION INFORMATION
	WORD wVersion		= MAKEWORD(0, 0);

	std::vector<CString> Info;

	int HitsLeft		= QueryHit->TotalHits, pos, i;
	UINT NextPos		= 34;
	UINT Length			= QueryHit->Header.Payload + 23; // packet Header size + Payload = Total packet length

//	int Max = Length / 100;
//	for(i = 0; i < Max; ++i)
//	{
//		PrintMemoryMap((byte*) (Packet+i*100), 100);
//	}

	// Find start of QHD
	int ItemCount = 0;
	BOOL use2byte = FALSE;
#ifdef _UNICODE
	use2byte = TRUE;
#ifdef USE_UTFCONVERT
	if(QueryHit->enctype == ET_UTF8)
		use2byte = FALSE;
#endif // USE_UTF8CONVERT
#endif // _UNICODE

	if(!use2byte)
	{
		// utf8의 경우 1byte씩 검증하여 NULL('\0')을 찾는다.
		for(i = 46; i < Length - 16; i++)		// 42 means packet_QueryHit size + flag&id&pubdate(binary size)
			if(Packet[i] == 0)
			{
				while(Packet[++i] != 0)
					if(i > Length - 16)
						break;

				ItemCount++;
			
				if(ItemCount != QueryHit->TotalHits)
					i += 13;					// flag&id&pubdate(binary size)
				else
					break;
			}
	}
	else
	{
		// unicode의 경우 2byte씩 검증한다. UTF-16
		TCHAR* punipacket = (TCHAR*) &(Packet[46]);
		int unilen = (Length - 58) / 2;
		for(i = 0; i < unilen; i++)
			if(punipacket[i] == _T('\0'))
			{
				while(punipacket[++i] != _T('\0'))
					if(i >unilen)
						break;

				ItemCount++;
			
				if(ItemCount != QueryHit->TotalHits)
					i += 7;						// flag&id&pubdate(binary size/2)
				else
					break;
			}

		i = i * 2 + 47;			// 46 + 1
	}

	packet_QueryHitEx* QHD = (packet_QueryHitEx*) &Packet[i + 1];

	CString UserID ( (char*) QHD->UserID, sizeof(QHD->UserID));
	
	if(ValidVendor(UserID))
	{
		ExtendedPacket = true;

		if(QHD->Length == 1)
			if(QHD->Push == 1)
				Firewall = true;

		if(QHD->Length == 2 || QHD->Length == 4)
		{
			if(QHD->FlagPush)
				Firewall = QHD->Push;

			if(QHD->FlagBusy)
				Busy = QHD->Busy;
			
			if(QHD->FlagStable)
				Stable = QHD->Stable;

			if(QHD->FlagSpeed)
				ActualSpeed = QHD->Speed;
		}

		// VERSION INFORMATION
		if(QHD->Length == 4)
		{
			wVersion = QHD->wVersion;
		}

	}

	SE_ITEM item;
	byte flag[4];

	int nflagsize = sizeof(flag);
	int nbinarysize = nflagsize + 8;			// flag size + index size + pubdate size
	int nstep = 0;

	// Extract results from the packet
	while(HitsLeft > 0 && NextPos < Length - 16)
	{
		// 검색 결과 중 FLAG 정보 추출
		memcpy(&flag, &Packet[NextPos], nflagsize);
		// 검색 결과의 종류를 정한다.
		if(flag[0] & SIFLAG_POST) item.di.type = GDT_POST;
		else if(flag[0] & SIFLAG_GRAPH) item.di.type = GDT_GRAPH;
		else if(flag[0] & SIFLAG_FILE) item.di.type = GDT_FILE;

		item.di.bread = flag[0] & SIFLAG_READ;
		item.di.breadon = flag[0] & SIFLAG_READON;
		item.di.bscrap = flag[0] & SIFLAG_SCRAP;
		item.di.bfilter = flag[0] & SIFLAG_FILTER;
		// 검색 결과 중 pubdate 정보를 추출
		memcpy(&item.di.pubdate, &Packet[NextPos+nflagsize], 4);
		TRACE(_T("pubdate : %u\r\n"), item.di.pubdate);
		// 검색 결과 중 id 정보를 추출
		memcpy(&item.di.nid, &Packet[NextPos+nflagsize+4], 4);

		if ( IPtoStr(QueryHit->Host) == "127.0.0.1" ) item.ui.shostip = Log->Origin->m_HostIP;
		else item.ui.shostip = IPtoStr(QueryHit->Host);

		item.QueryID		= QueryHit->Header.Guid;
		item.ui.ngnuport	= QueryHit->Port;
		item.Speed			= QueryHit->Speed;

		item.Firewall		= Firewall;
		item.Busy			= Busy;
		item.Stable			= Stable;
		item.ActualSpeed	= ActualSpeed;

		if(ExtendedPacket)
			item.ui.suid = UserID;

		item.Origin = Log->Origin;
		memcpy(&item.PushID, &Packet[Length - 16], 16);
		item.Distance = Log->Header->Hops;

		// VERSION INFORMATION
		item.ui.wVersion	= wVersion;

#ifdef USE_UTFCONVERT
		// Get Filename
		pbuf = (char*) &Packet[NextPos + nbinarysize];
		u8len = strlen((char*) pbuf);
		u16len = MultiByteToWideChar(CP_UTF8
			, NULL
			, pbuf, u8len
			, wzbuf, 4096);
		wzbuf[u16len] = 0;
		item.di.sguid = wzbuf;
		nstep = u8len;
#else
		// Get Filename
		item.di.sguid = (TCHAR*) (&Packet[NextPos + nbinarysize]);
		nstep = item.di.sguid.GetLength() * sizeof(TCHAR);

//		TRACE(_T("guid : %s\r\n"), (TCHAR*) (&Packet[NextPos + nbinarysize]));
#endif
		item.NameLower = item.di.sguid;
		item.NameLower.MakeLower();

//		TRACE(_T("get guid : %s\r\n"), item.di.sguid);

		pos = NextPos + nbinarysize + nstep;

		Info.clear();

		// Get Extended file info
#ifdef USE_UTFCONVERT
		if(Packet + pos + 1 != NULL)
#else
		if(Packet + pos + sizeof(TCHAR) != NULL)
#endif
		{
#ifdef USE_UTFCONVERT
			pbuf = (char*) (Packet + pos + 1);
			u8len = strlen((char*) pbuf) + 1;
			u16len = MultiByteToWideChar(CP_UTF8
				, NULL
				, pbuf, u8len
				, wzbuf, 4096);
			nstep = u8len;
			CString ExInfo = wzbuf;
			int ExLength = u8len;
#else
			CString ExInfo = (TCHAR*) (Packet + pos + sizeof(TCHAR));
			int ExLength = (ExInfo.GetLength()+1) * sizeof(TCHAR);
#endif
			TRACE(_T("get title : %s\r\n"), ExInfo);

			while(!ExInfo.IsEmpty())
			{
				CString SubExInfo = ParseString(ExInfo, _T('\x1c'));
				
				if(Info.size())
					item.sExInfo += _T(" and ") + SubExInfo;
				else
					item.sExInfo = SubExInfo;

				Info.push_back(SubExInfo);
			}

			pos += ExLength;
		}

		int i = 0;
		for(i = 0; i < Info.size(); i++)
		{
			// Get FISH TITLE
			int ExPos = Info[i].Find(URN_TITLE);

			if(ExPos != -1 && Info[i].GetLength() > 6 )
			{
				item.di.name = Info[i].Mid(6);
				continue;
			}

			// Get FISH LINK
			ExPos = Info[i].Find(URN_LINK);

			if(ExPos != -1 && Info[i].GetLength() > 6 )
			{
				item.di.link = Info[i].Mid(6);
			}
		}
		// -- Item data

		// Check for duplicates in master list
		bool AddItem = true;

		if(AddItem)
		{
			Whole++;
			// 결과를 전달한다. DSRM으로 SE_ITEM 객체 item을 던진다.
			// QueryHit->Header.Guid를 이용해 대상을 찾아서 결과를 넣을 수 있도록 한다. (SE_ITEM에 넣음??)
			// ResultGroup* CMars2FileSearchView::AddtoGroup(Result &Item) 을 통해 다운로드가 실행된다.
			theApp.m_spSM->InsertSearchResult(item.QueryID, item.di.sguid, item);
		}

		std::list<CGnuDownloadShell*>::iterator it;

		// Look for matches in current downloads
		for(it = m_pComm->m_DownloadList.begin(); it != m_pComm->m_DownloadList.end(); ++it)
		{
			// 이미 파싱 작업을 수행했으므로 파싱된 결과를 넘긴다.
			// 노드 추가
			if(QueryHit->Header.Guid == (*it)->m_SearchGuid
				&& item.di.sguid == (*it)->m_Sha1Hash
				)
			{
				Result Item;
				Item.FileIndex			=   item.di.nid;
				Item.Name				=	item.di.name;
				CString FileNameLower (item.di.name) ;
				FileNameLower.MakeLower();
				Item.NameLower			=	FileNameLower;
				Item.UserID				=	item.ui.suid;
				Item.Size				=	item.di.nsize;
				Item.Host				=	StrtoIP( item.ui.shostip);
				Item.Port				=	(WORD) item.ui.ngnuport;
				Item.Busy				=	false;
				Item.Firewall			=	false;
				Item.PushID				=	item.PushID;
				Item.wVersion			=	item.ui.wVersion;
				Item.Sha1Hash			=	item.di.sguid;
				(*it)->AddHost(Item);
				break;
			}
		}

		// Check for end of reply packet
		if(pos + sizeof(TCHAR) >= Length - 16)
			HitsLeft = 0;
		else
		{
			HitsLeft--;
#ifdef USE_UTFCONVERT
			NextPos = pos + 1;
#else
			NextPos = pos + sizeof(TCHAR);
#endif
		}
	}
}
// -- end CGnuNode::ParseQueryHit

// commented by moonknit 2005-05-13
// 공유목록에 대한 테이블을 Reset한다.
// ResetTable 에서 특별하게 수행되는 사항이 없음..
// -> 현재 사용이 안되는듯...

// commented by moonknit 2005-05-16
// 사용할 필요가 없다.
// RouteTablePatch는 공유하고 있는 모든 파일의 정보를 서버로 전송할 때 사용하는 것으로
// Star Topology에서 사용된다.
void CGnuNode::Recieve_RouteTableReset(packet_RouteTableReset* TableReset, UINT Length)
{
	if(m_HostMode != CLIENT_LEAF || TableReset->Header.Hops > 0)
		return;

	m_TableInfinity = TableReset->Infinity;
	m_TableLength   = TableReset->TableLength;

	m_CurrentSeq = 1;
}

// commented by moonknit 2005-05-16
// 사용할 필요가 없다.
// RouteTablePatch는 공유하고 있는 모든 파일의 정보를 서버로 전송할 때 사용하는 것으로
// Star Topology에서 사용된다.
void CGnuNode::Recieve_RouteTablePatch(packet_RouteTablePatch* TablePatch, UINT Length)
{
	if(m_HostMode != CLIENT_LEAF || TablePatch->Header.Hops > 0)
		return;

	// Connection must be closed if patch is out of synch
	if(m_CurrentSeq != TablePatch->SeqNum)
	{
		Close();
		return;
	}

	// If first patch in sequence, reset table
	if(TablePatch->SeqNum == 1)
	{
		if(m_PatchTable)
			delete [] m_PatchTable;

		m_PatchTable   = new char[m_TableLength];
		m_TableNextPos = 0;
		
		memset(m_PatchTable, 0, m_TableLength);
 
		// Compressed patch table
		if(m_CompressedTable)
			delete [] m_CompressedTable;

		m_CompressedTable = new byte[m_TableLength];
		m_CompressedSize  = 0;
	}
	

	if(TablePatch->SeqNum <= TablePatch->SeqSize)
	{
		UINT PatchSize = TablePatch->Header.Payload - 5;

		// As patches come in, build buffer of data
		if(m_CompressedSize + PatchSize <= m_TableLength)
		{
			memcpy(m_CompressedTable + m_CompressedSize, (byte*) TablePatch + 28, PatchSize);

			m_CompressedSize += PatchSize;
		}
		else
		{
			Close();
		}
	}

	// Final patch received
	if(TablePatch->SeqNum == TablePatch->SeqSize)
	{
		DWORD UncompressedSize = m_TableLength;
		// Uncompress packet if needed
		if(TablePatch->Compression == 0x1)
		{
		}
		else
		{
			UncompressedSize = m_CompressedSize;
			memcpy(m_PatchTable, m_CompressedTable, UncompressedSize);
		}
	
		delete [] m_PatchTable;
		delete [] m_CompressedTable;

		m_PatchTable	  = NULL;
		m_CompressedTable = NULL;

		m_CurrentSeq = 1;
	}
	else
	{
		m_CurrentSeq++;
	}
}

void CGnuNode::Recieve_Unknown(byte*, DWORD dwLength)
{

}


/////////////////////////////////////////////////////////////////////////////
// Sending packets

// PING 패킷은 이곳에서 발생된다.
void CGnuNode::Send_Ping(int TTL)
{
	// by bemlove at 2003-04-03 오후 2:23:29
	CString Host;
	UINT    nPort;
	GetPeerName(Host, nPort);
	static count=0;

	GUID Guid = GUID_NULL;
	GnuCreateGuid(&Guid);

	if (Guid == GUID_NULL)
		return;

	packet_Ping Ping;
	
	Ping.Header.Guid	 = Guid;
	Ping.Header.Function = 0x10;
	Ping.Header.Hops	 = 0;
	Ping.Header.TTL		 = TTL;
	Ping.Header.Payload  = 0;

	m_pComm->m_TableLocal.Insert(&Guid, NULL);

	SendPacket(&Ping, 23, PACKET_PING, true);

}

void CGnuNode::Send_Pong(GUID Guid, int nHops)
{
	// Build the packet

	// Get Local host
	if(theApp.m_spGD->m_LocalHost.S_addr == StrtoIP(_T("127.0.0.1")).S_addr)
	{
		UINT dPort;
		CString LocalIP;
		if(GetSockName(LocalIP, dPort))
		{
			theApp.m_spGD->m_LocalHost.S_addr = StrtoIP(LocalIP).S_addr;
		}
		else
		{
			return;
		}
	}

	packet_Pong Pong;

	Pong.Header.Guid		= Guid;
	Pong.Header.Function	= 0x11;
	Pong.Header.TTL			= 7;
	Pong.Header.Hops		= 0;
	Pong.Header.Payload		= 14;

	Pong.Port				= (WORD) m_pComm->m_CurrentPort;
	Pong.Host				= theApp.m_spGD->m_LocalHost;
	Pong.LeafCount			= m_pComm->GetNodeTypeCnt(NT_ALL);		// 연결된 노드의 개수를 반환한다.
	// TO DO
	// 노드의 상태 정보도 보낼 수 있다.
//	Pong.State				= m_pComm->GetState();

	m_pComm->m_TableLocal.Insert(&Guid, this);

	SendPacket(&Pong, SIZE_PONGPACKET, PACKET_PONG, true);
}

void CGnuNode::Send_PongCache(packet_Log* Log, int StatPos)
{
	int MaxTtlPongNeeded = 0, i;

	packet_Ping* Ping = (packet_Ping*) Log->Header;
	int nLength = Log->PacketLength;


	if(m_PingAcceptTime >= GetTickCount() || m_PingAcceptTime == 0)
	{
		m_LastGuid = Ping->Header.Guid;
		m_LastHops = Ping->Header.Hops;

		if(theApp.m_spGD->m_MinConnects > 0 && m_pComm->m_NormalConnectsApprox < theApp.m_spGD->m_MinConnects)
			Send_Pong(Ping->Header.Guid, Ping->Header.Hops);

		// Determine how many pongs to send
		for (i = 0; i < MAX_TTL - 1; i++)
		{
			if (i <= Ping->Header.TTL - 1)
				m_PongNeeded[i] = MAX_NBPONG_RESPONSE / Ping->Header.TTL;
			else
				m_PongNeeded[i] = 0;
		}
		 
		// Send Pongs from Cache
		m_PongNeeded[0] += MAX_NBPONG_RESPONSE % Ping->Header.TTL;

		for (i = 0; i < (Ping->Header.TTL - 1 > MAX_TTL - 1 ? MAX_TTL - 1 : Ping->Header.TTL - 1);i++)
		{
			if (!m_PongNeeded[i])
				continue;
	
			if (m_pComm->m_PongCache[i].size())
			{
				std::vector<MapPongList>::reverse_iterator itPongList;
				for(itPongList = m_pComm->m_PongCache[i].rbegin(); itPongList != m_pComm->m_PongCache[i].rend(); itPongList++) // freshest pong
				{
					MapPongList* pPongCache = &(*itPongList);
					
					if (pPongCache->PrimNode != this)
					{
						m_PongNeeded[i]--;
						Send_Pong(pPongCache);

						if(!m_PongNeeded[i])
							break;
					}
				}
			}
			else
			{
				std::vector<MapPongList>::reverse_iterator itPongList;
				for(itPongList = m_pComm->m_PongCache_Old[i].rbegin(); itPongList != m_pComm->m_PongCache_Old[i].rend(); itPongList++) // freshest pong
				{
					MapPongList* pPongCache = &(*itPongList);
					if (pPongCache->PrimNode != this)
					{
						m_PongNeeded[i]--;
						Send_Pong(pPongCache);

						if (!m_PongNeeded[i])
							break;
					}
				}

				MaxTtlPongNeeded = i; 
			}

			if (m_PongNeeded[i])
			{
				m_pComm->m_FlushPongCacheOld_time = GetTickCount() + TIMEOUT_PONG_CACHE; //timeout old cache lowered
			}		
		}

		// set ping next time
		m_PingAcceptTime = GetTickCount() + TIMEOUT_PONG_CACHE;

		m_StatPackets[StatPos][1] = 1;
		m_StatPings[1]++;
	}
	
	// throttle ping
	else
		return; 


	if (MaxTtlPongNeeded && Ping->Header.Hops < MAX_TTL && Ping->Header.TTL > 0)
	{
		Ping->Header.TTL = 	MaxTtlPongNeeded + 1; // set the new ttl
	
		// Forward ping to pong caching hosts only
		m_pComm->m_NodeAccess.Lock();
		for(i = 0; i < m_pComm->m_NodeList.size(); i++)	
		{
			CGnuNode *p = m_pComm->m_NodeList[i];

			if(p != this && p->m_PongCachingEnabled && p->m_Status == SOCK_CONNECTED)
				p->SendPacket(Ping, nLength, PACKET_PING, false);
		}
		m_pComm->m_NodeAccess.Unlock();
	}
}

// written by		: moonknit
// name				: CGnuNode::Send_QueryHit
// created			: 2005-05-17
// last updated		: 2005-05-17
// desc				:
// QueryHit 패킷을 해당 Node로 전송하도록 한다.
void CGnuNode::Send_QueryHit(SE_QUERY& Query, byte* pQueryHit, DWORD ReplyLength, byte ReplyCount, RemoteIndex* pRemote)
{ 
	int payload = sizeof(packet_QueryHit) + ReplyLength;
	int packetLength = payload + sizeof(packet_QueryHitEx) + 16;

	packet_QueryHit*  QueryHit = (packet_QueryHit*) pQueryHit;
	packet_QueryHitEx QHD;

	// Build Query Packet
	QueryHit->Header.Guid	  = Query.QueryGuid;
	QueryHit->Header.Function = 0x81;
	QueryHit->Header.TTL	  = Query.nHops;
	QueryHit->Header.Hops	  = 0;
	QueryHit->Header.Payload  = packetLength - 23;		// total packet length - header size

	QueryHit->TotalHits		  = ReplyCount;

	// If we are sending query hit for child

	QueryHit->Port	 = (WORD) m_pComm->m_CurrentPort;
	QueryHit->Host	 = theApp.m_spGD->m_LocalHost;
	QueryHit->Speed	 = GetSpeed();

#ifdef USE_UTFCONVERT
	QueryHit->enctype = ET_UTF8;
#endif

	// Add Query Hit Descriptor
	bool Busy = false;
	if(theApp.m_spGD->m_MaxUploads)
	{
		if(m_pComm->CountUploading() >= theApp.m_spGD->m_MaxUploads)
			Busy = true;
	}

	strcpy(QHD.UserID, theApp.m_spLP->GetIRCID());

	QHD.Length		= 4;			// version check added
	QHD.FlagPush	= true;
	QHD.FlagBad		= true;
	QHD.FlagBusy	= true;
	QHD.FlagStable	= true;
	QHD.FlagSpeed	= true;
	QHD.FlagTrash   = 0;

	if(pRemote)
	{
		QHD.Push	= pRemote->Firewall;
		QHD.Bad		= 0;
		QHD.Busy	= pRemote->Busy;
		QHD.Stable	= pRemote->Stable;
		QHD.Speed	= pRemote->RealSpeed;
		QHD.Trash	= 0;
	}
	else
	{
		QHD.Push	= theApp.m_spGD->m_RealFirewall;
		QHD.Bad		= 0;
		QHD.Busy	= Busy;
		QHD.Stable	= theApp.m_spGD->m_HaveUploaded;
		QHD.Speed	= theApp.m_spGD->m_RealSpeedUp ? true : false;
		QHD.Trash	= 0;
	}

	int pos = payload;
	pQueryHit += pos;		// packet QueryHit size + ReplyLength

	// VERSION INFORMATION
	QHD.wVersion	= FISH_VERSION_WORD;

	memcpy(pQueryHit, &QHD, sizeof(packet_QueryHitEx));
	pQueryHit -= pos;

	// Add ID of this node
	pos += sizeof(packet_QueryHitEx);
	pQueryHit += pos;		// find PushID region

	if(pRemote)
		memcpy(pQueryHit, &pRemote->PushID, 16);
	else
		memcpy(pQueryHit, &m_pComm->m_ClientID, 16);

	pQueryHit -= pos;		// find packet head

	// Send the packet
	SendPacket(pQueryHit, packetLength, PACKET_QUERYHIT, true, false);
}

// commented by moonknit 2005-05-17
// 검색 처리후 Hop이 남아있는 경우 Query를 재전송할 때 사용한다.
// Query 재전송 기능을 사용하지 않는 경우에는 이 함수는 사용하지 않는다.
void CGnuNode::Send_ForwardQuery(QueryComp QueryInfo)
{
	byte rawPacket[512];

	packet_Query* Query = (packet_Query*) rawPacket;

	Query->Header.Guid		= QueryInfo.QueryGuid;
	Query->Header.Function	= 0x80;
	Query->Header.TTL		= MAX_TTL - QueryInfo.nHops;
	Query->Header.Hops		= QueryInfo.nHops;

	Query->Speed = 0;
	
	UINT PacketSize = 25;
	memcpy(rawPacket + PacketSize, QueryInfo.QueryText.c_str(), QueryInfo.QueryText.length());
	PacketSize += QueryInfo.QueryText.length();

	rawPacket[PacketSize] = 0;
	PacketSize++;

	for(int i = 0; i < QueryInfo.QueryExtended.size(); i++)
	{
		for(int j = 0; j < QueryInfo.QueryExtended[i].GetLength(); j++)
		{
			rawPacket[PacketSize] = QueryInfo.QueryExtended[i].GetAt(j);
			PacketSize++;
		}

		rawPacket[PacketSize] = 0x1C;
		PacketSize++;
	}

	rawPacket[PacketSize - 1] = 0;

	Query->Header.Payload	= PacketSize - sizeof(packet_QueryHitEx) - 16;

	SendPacket(rawPacket, PacketSize, PACKET_QUERY, true, true);
}


// commented by moonknit 2005-05-25
// 사용되지 않는다.
void CGnuNode::Send_PatchTable()
{
	byte* PacketBuff   = new byte[1 << TABLE_BITS]; // Used so everything is sent in the correct order
	UINT  NextPos	   = 0;

	GUID Guid = GUID_NULL;
	GnuCreateGuid(&Guid);

	if (Guid == GUID_NULL)
		return;

	// Build the packet
	packet_RouteTableReset Reset;

	Reset.Header.Guid		= Guid;
	Reset.Header.Function	= 0x30;
	Reset.Header.TTL		= 1;
	Reset.Header.Hops		= 0;
	Reset.Header.Payload	= 6;

	Reset.PacketType	= 0x0;
	Reset.TableLength	= 1 << TABLE_BITS;
	Reset.Infinity		= TABLE_INFINITY;

	// Send reset packet so remote host clears entries for us
	memcpy(PacketBuff + NextPos, &Reset, 29);
	NextPos += 29;


	// Compress patch table
	DWORD PatchSize  = 1 << TABLE_BITS;
	DWORD CompSize	= PatchSize * 1.2 + 12;

	byte* CompBuff	= new byte[CompSize];

	// commented by moonknit 2005-05-11
//	if(compress(CompBuff, &CompSize, (byte*) m_pShare->m_pWordTable->m_PatchTable, PatchSize) != Z_OK)
//	{
//		return;
//	}
	

	// Determine how man 1024 byte packets to send
	int SeqSize = 1;

	while(SeqSize * 1024 < CompSize)
		SeqSize++;
	
	int CopyPos  = 0;
	int CopySize = 0;

	byte* RawPacket = new byte[1024 + 28];
	packet_RouteTablePatch* PatchPacket = (packet_RouteTablePatch*) RawPacket;
	

	for(int SeqNum = 1; SeqNum <= SeqSize; SeqNum++)
	{
		if(CompSize - CopyPos < 1024)
			CopySize = CompSize - CopyPos;
		else
			CopySize = 1024;

		// Build packet
		GnuCreateGuid(&Guid);

		PatchPacket->Header.Guid		= Guid;
		PatchPacket->Header.Function	= 0x30;
		PatchPacket->Header.TTL			= 1;
		PatchPacket->Header.Hops		= 0;
		PatchPacket->Header.Payload		= 5 + CopySize;
		
		PatchPacket->PacketType = 0x1;
		PatchPacket->SeqNum		= SeqNum;
		PatchPacket->SeqSize	= SeqSize;

		PatchPacket->Compression = 0x1;
		PatchPacket->EntryBits	 = 8;

		memcpy(RawPacket + 28, CompBuff + CopyPos, CopySize);

		memcpy(PacketBuff + NextPos, RawPacket, 28 + CopySize);
		NextPos += 28 + CopySize;

		CopyPos += 1024;
	}

	// This mega packet includes the reset and all patches
	SendPacket(PacketBuff, NextPos, PACKET_QUERY, true);

	delete [] PacketBuff;
	delete [] CompBuff;
	delete [] RawPacket;
}

bool CGnuNode::GetAlternateHostList(CString &HostList)
{
	return false;
}

bool CGnuNode::GetAlternateSuperList(CString &HostList)
{
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Misc functions

void CGnuNode::Refresh()
{
	for(int i = 0; i < MAX_TTL; i++)
	{
		m_dwFriends[i]		= 0;
	}

	m_dwFriendsTotal = 0;
	m_nSecsRefresh   = 0;

	m_pComm->NodeMessage(SOCK_UPDATE, NULL);
	Send_Ping(MAX_TTL);
}

void CGnuNode::RemovePacket(int pos)
{
	switch(m_StatPackets[pos][0])
	{
	case PACKET_PING:
		m_StatPings[0]--;

		if(m_StatPackets[pos][1])
			m_StatPings[1]--;

		break;
	case PACKET_PONG:
		m_StatPongs[0]--;

		if(m_StatPackets[pos][1])
			m_StatPongs[1]--;

		break;
	case PACKET_QUERY:
		m_StatQueries[0]--;

		if(m_StatPackets[pos][1])
			m_StatQueries[1]--;

		break;
	case PACKET_QUERYHIT:
		m_StatQueryHits[0]--;

		if(m_StatPackets[pos][1])
			m_StatQueryHits[1]--;

		break;
	case PACKET_PUSH:
		m_StatPushes[0]--;

		if(m_StatPackets[pos][1])
			m_StatPushes[1]--;

		break;
	}

	m_StatPackets[pos][1] = 0;
}

// 8로 나누면 KB/s
DWORD CGnuNode::GetSpeed()
{
	if(theApp.m_spGD->m_RealSpeedUp)
	{
		return theApp.m_spGD->m_RealSpeedUp;
	}
	else
	{
		if(theApp.m_spGD->m_SpeedStat)
			return theApp.m_spGD->m_SpeedStat;
		else
			return theApp.m_spGD->m_SpeedDyn;
	}

	return 0;
}

// 노드 능률 계산
// Ping Buffer Flush
// 노드 연결유지를 위한 Ping 전송
void CGnuNode::Timer()
{
	if(m_ThreadCall)
	{	
		m_BufferAccess.Lock();
		FlushSendBuffer();
		m_BufferAccess.Unlock();

		m_ThreadCall = false;
	}

	// Decrement time till next ReSearch is allowed on this socket
	if(m_NextReSearchWait)
		m_NextReSearchWait--;

	// Pong caching - Send Ping to receive cache of pongs
	if (m_PongCachingEnabled && (m_PingSendTime >= GetTickCount() || !m_PingSendTime))
	{
		Send_Ping(m_NextTTL);
	}

	
	// Minute counter
	if(m_nSecNum < 60)
		m_nSecNum++;
	if(m_nSecPos == 60)
		m_nSecPos = 0;

	// Increment recent queries
	std::vector<RecentQuery>::iterator itRecent;
	for(itRecent = m_RecentQueryList.begin(); itRecent != m_RecentQueryList.end(); itRecent++)
	{
		(*itRecent).SecsOld++;

		if((*itRecent).SecsOld >= 5 * 60)
		{
			m_RecentQueryList.erase(itRecent);
			break;
		}
	}

	// When i = 0 its receive stats
    //      i = 1 its send stats
	// Statistics
	// Calculate Efficiency
	for(int i = 0; i < 3; i++)
	{
		// Packets
		m_dwTotalPackets[i] -= m_dwAvgPackets[i][m_nSecPos];
		m_dwAvgPackets[i][m_nSecPos] = m_dwSecPackets[i];
		m_dwTotalPackets[i] += m_dwSecPackets[i];

		m_dwSecPackets[i] = 0;

		// Bytes
		m_dwTotalBytes[i] -= m_dwAvgBytes[i][m_nSecPos];
		m_dwAvgBytes[i][m_nSecPos] = m_dwSecBytes[i];
		m_dwTotalBytes[i] += m_dwSecBytes[i];

		m_dwSecBytes[i] = 0;	
	}
	
	m_nSecPos++;

	// Efficiency calculation
	UINT dPart  = m_StatPings[1] + m_StatPongs[1] + m_StatQueries[1] + m_StatQueryHits[1] + m_StatPushes[1]; 
	UINT dWhole = m_StatPings[0] + m_StatPongs[0] + m_StatQueries[0] + m_StatQueryHits[0] + m_StatPushes[0];
	
	// hey ~ variable name was misspelled
	// 능률(Efficiency)은 들어온 패킷이 유효한 패킷인경우 유용한 노드로 판단한다.
	// 유효한 패킷이란 아직 받지 않은 패킷을 말한다.
	if(dWhole)
		m_Efficeincy = dPart * 100 / dWhole;
	else
		m_Efficeincy = 0;

	NodeManagement();
}

void CGnuNode::NodeManagement()
{
	if(SOCK_CONNECTING == m_Status)
	{
		return;
	}

	else if(SOCK_CONNECTED == m_Status)
	{
		// remain the node in active node
		Send_Ping(1);

		// Check if we need to update the patch table
		if(m_pComm->m_LeafModeActive)
			if(m_PatchUpdateNeeded)
			{
				// removed by moonknit 2005-05-13
				// 중앙 서버 처리형 공유 방식을 사용하지 않는다.
				//Send_PatchTable();
				// -- end
			}

		m_PatchUpdateNeeded = false;

		// Normal ping at 10 secs
		if(!m_pComm->m_LeafModeActive && m_HostMode != CLIENT_LEAF)
			if(m_IntervalPing == 45)
			{
				AvoidTriangles();

				Send_Ping(1);

				m_IntervalPing = 0;
			}
			else
				m_IntervalPing++;
		
		// Drop if not socket gone mute 30 secs
		if(m_dwTotalBytes[0] == 0)
		{
			m_nSecsDead++;

			if(m_nSecsDead == 15)
			{
				Send_Ping(1);
			}
			if(m_nSecsDead > 30)
			{
				Close();
				return;
			}
		}
		else
			m_nSecsDead = 0;


		// Re-Search on new connect after 10 seconds
		if(m_nSecsAlive == 10)
		{
			Send_Ping(MAX_TTL);

			std::list<CGnuDownloadShell*>::iterator it;

			for(it = m_pComm->m_DownloadList.begin(); it != m_pComm->m_DownloadList.end(); ++it)
				(*it)->IncomingNode(this);

			m_pComm->NodeMessage(SOCK_UPDATE, (WPARAM) this);

			m_nSecsAlive++;
		}
		else if(m_nSecsAlive < 60)
			m_nSecsAlive++;
	}
}
// Date : 2005-04-01,	By bemlove
// SOCK_CONNECTING 상태인 노드만을 관리
void CGnuNode::NodeManagement2(bool connected)
{
	if(SOCK_CONNECTING == m_Status)
	{
		m_nSecsTrying++;
		if( connected )
		{
			if( m_nSecsTrying > CONNECT_TIMEOUT)
			{
				Close();
				return;
			}
		}
		else
		{
			if(m_nSecsTrying > 1 ) // CONNECT_TIMEOUT)
			{
				Close();
				return;
			}

		}
	}
}
void CGnuNode::AvoidTriangles()
{
	
}

void CGnuNode::MapPong(packet_Pong* Pong, bool Near)
{

}

void CGnuNode::MapQueryHit(packet_QueryHit* QueryHit)
{
	// Find node in table
	for(int i = 0; i < m_pComm->MapTable.size(); i++)
		if(QueryHit->Host.S_addr == m_pComm->MapTable[i].Host.S_addr)
			if(QueryHit->Port == m_pComm->MapTable[i].Port)
			{
				// Get Host of packet
				byte* Packet   = (byte*) QueryHit;

				int    HitsLeft = QueryHit->TotalHits, pos;
				DWORD  NextPos  = 34;
				DWORD  Length   = QueryHit->Header.Payload + sizeof(packet_QueryHitEx) + 16;
				
				// Find start of QHD
				int ItemCount = 0;
				for(pos = 42; pos < Length - 16; pos++)
					if(Packet[pos] == 0 && Packet[pos + 1] == 0)
					{
						ItemCount++;
					
						if(ItemCount != QueryHit->TotalHits)
							pos += 9;
						else
							break;
					}


				packet_QueryHitEx* QHD = (packet_QueryHitEx*) &Packet[pos + 2];
	
				CString Vendor( (TCHAR*) QHD->UserID, 21 * sizeof(TCHAR));
			
				if(ValidVendor(Vendor))
				{
					memcpy(m_pComm->MapTable[i].Client, (LPCTSTR) Vendor, 4);
				}

				return;
			}
}

// Pong Caching
void CGnuNode::Send_Pong(MapPongList* pPongCache)
{
	packet_Pong Pong;

	Pong.Header.Guid		= m_LastGuid;
	Pong.Header.Function	= 0x11;

	//Pong.Header.TTL			= m_LastHops;
	Pong.Header.TTL			= 7;
	Pong.Header.Hops		= 0;
	Pong.Header.Payload		= 14;

	Pong.Port				= pPongCache->Port;
	Pong.Host				= pPongCache->Host;

	Pong.FileCount	= pPongCache->FileCount;
	Pong.LeafCount	= pPongCache->LeafCount;
	Pong.State		= pPongCache->State;

	m_pComm->m_TableLocal.Insert(&m_LastGuid, this);
	SendPacket(&Pong, SIZE_PONGPACKET, PACKET_PONG, true);
	
}

void CGnuNode::Check_PongNeeds(MapPongList* pPongCache,int pttl)
{
	m_pComm->m_NodeAccess.Lock();
	for(int i = 0; i < m_pComm->m_NodeList.size(); i++)
	{
		CGnuNode* p = m_pComm->m_NodeList[i];

		if(p != this && p->m_PongCachingEnabled)
			if(p->m_PongNeeded[pttl])
			{
				p->Send_Pong(pPongCache);
				p->m_PongNeeded[pttl]--;
			}
	}
	m_pComm->m_NodeAccess.Unlock();
}

bool CGnuNode::IsRecentQuery(GUID Guid)
{
	for(int i = 0; i < m_RecentQueryList.size(); i++)
		if(m_RecentQueryList[i].Guid == Guid)
			return true;

	return false;
}


//============================================================================
// NAME : SendOtherNodeInfo()
// PARM : void
// RETN : void
// DESC : 연결된 노드로 부터 자신이 가지고 있던 노드와의 연결이 끊어져서, 
//          새로운 노드 정보를 요청하여, 현재 보유중인 노드 정보중 하나를 전달하여 준다.
// Date : 2004-12-07 coded by bemlove, origin
//============================================================================
void CGnuNode::SendOtherNodeInfo()
{
	//1. 요청 노드가 아닌 다른 노드들중 Connected 상태인 노드 찾기
	int count = m_pComm->m_NodeList.size() ;
	if( count > 0 )
	{
		srand( GetTickCount() );
		int sel = rand() % count;
		if( m_pComm->m_NodeList[sel]->m_HostIP != m_HostIP &&
		    m_pComm->m_NodeList[sel]->m_Status == SOCK_CONNECTED )
		{
			//2. 패킷 전달 (노드정보)
			CString tmpStr=_T(""), SendData=_T("");
			
			SendData = EZ_SEND_OTHER_NODE_INFO;
			tmpStr.Format(_T("HOSTIP : %s\r\n"), m_pComm->m_NodeList[sel]->m_HostIP);
			SendData += tmpStr;
			tmpStr.Format(_T("PORT : %d\r\n"), m_pComm->m_NodeList[sel]->m_Port);
			SendData += tmpStr;
			SendData += EZ_ADD_NEW_NODE;
			int ret = Send( SendData );
			if( ret == SOCKET_ERROR )
			{
			}
			return;
		}
	}		
}//end of SendOtherNodeInfo()

#ifdef NODE_CONNECTIVITY
// written by		: moonknit
// name				: void CGnuNode::SetConnectivity(int nConSrc)
// created			: 2005-06-17
// last updated		: 2005-06-17
// desc				:
// Connectivity 계산 소스를 통해 연결된 노드의 연결강도를 설정한다.
// 연결강도는 노드로부터의 연결강도 계산값과 로컬 서버의 최대 연결 개수에 의해 결정된다.
// ex) 최대 연결 개수가 1이하인 경우는 무조건 최고 연결강도값을 반환한다.
//		최대 연결 개수 6이고 연결강도 계산값이 30인 경우는 연결강도는 CONNECTIVITY_LEV2값= 5이 된다.
void CGnuNode::SetConnectivity(int nConSrc)
{
	int nMax = theApp.m_spGD->GetMaxInConnection() + theApp.m_spGD->GetMaxOutConnection();

	int nDel[4];
	nDel[0] = CONNECTIVITY_LEV1 * (nMax - 1);		// when max 6, 10
	nDel[1] = CONNECTIVITY_LEV2 * (nMax - 1);		// when max 6, 25
	nDel[2] = CONNECTIVITY_LEV3 * (nMax - 1);		// when max 6, 40

	if(nConSrc <= 0)			// 연결 노드는 초기 생성노드이다.
		m_nConnectivity = CONNECTIVITY_DEFAULT;
	else if(nConSrc <= nDel[0])		// 연결 노드는 연결도가 약한 노드이다.
		m_nConnectivity = CONNECTIVITY_LEV1;
	else if(nConSrc <= nDel[1])		// 연결 노드는 연결도가 조금 강하다.
		m_nConnectivity = CONNECTIVITY_LEV2;
	else if(nConSrc <= nDel[2])		// 연결 노드는 연결도가 강하다.
		m_nConnectivity = CONNECTIVITY_LEV3;
	else						// 연결 노드는 연결도가 최고이다.
		m_nConnectivity = CONNECTIVITY_LEV4;
}
#endif

BOOL CGnuNode::IsValidNode()
{
	if(m_bValidNode) return TRUE;

	DWORD now = GetTickCount();
	if(now - m_nStartTime < VALID_NODETIME)
		return FALSE;

	m_bValidNode = TRUE;
	return TRUE;
}