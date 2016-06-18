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

#include "../fish.h"

#include "GnuRouting.h"
#include "GnuControl.h"
#include "GnuLocal.h"
#include "GnuDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFISHApp theApp;

CGnuLocal::CGnuLocal(CGnuControl* pComm)
{
	m_pComm  = pComm;

	m_Broadcasted = true;
}

CGnuLocal::~CGnuLocal()
{
	if(m_hSocket != INVALID_SOCKET)
		AsyncSelect(0);
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CGnuLocal, CAsyncSocket)
	//{{AFX_MSG_MAP(CGnuLocal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


void CGnuLocal::Init()
{
	if(!Create(UDP_PORT, SOCK_DGRAM))
	{
		return;
	}

	if(!SetSockOpt(SO_BROADCAST, &m_Broadcasted, sizeof(int)))
	{
		return;
	}
}

void CGnuLocal::SendPing()
{
	CString NetworkPing;

	NetworkPing  = theApp.m_spGD->ModeNetwork;
	NetworkPing += _T(" PING\r\n");
	NetworkPing += _T("Port: ") + DWrdtoStr(m_pComm->m_CurrentPort) + _T("\r\n");

	if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE)
	{
		if(theApp.m_spGD->m_Lan && theApp.m_spGD->m_LanName != _T(""))
			NetworkPing += _T("LAN: ") + theApp.m_spGD->m_LanName + _T("\r\n");

		if(theApp.m_spGD->m_InternalIRC && theApp.m_spGD->m_InternalIRCAddr != _T(""))
			NetworkPing += _T("IRC: ") + theApp.m_spGD->m_InternalIRCAddr + _T("\r\n");
	}

	NetworkPing += _T("\r\n");

	// Send initial ping out to LAN network
	SendTo(NetworkPing, NetworkPing.GetLength(), UDP_PORT, NULL);
}

void CGnuLocal::OnReceive(int nErrorCode) 
{
	byte buffer[1024];

	CString Host, LocalHost, NetworkPing, NetworkPong;
	CString LanName, IRCAddr, InfoURL;

	UINT    Port, LocalPort, Leaves = 0;

	int buffLength = ReceiveFrom(buffer, 1024, Host, Port);

	GetSockName(LocalHost, LocalPort);

	// Handle Errors
	if(!buffLength || buffLength == SOCKET_ERROR)
		return;
	
	CString strBuffer((char*) buffer, 128);

	NetworkPing =  theApp.m_spGD->ModeNetwork;
	NetworkPing += _T(" PING\r\n");

	NetworkPong =  theApp.m_spGD->ModeNetwork;
	NetworkPong += _T(" PONG\r\n");


	// Handle Ping over LAN
	if(strBuffer.Find(NetworkPing) == 0)
	{
		if(Host == LocalHost)
		{
			return;
		}

		// Send back pong only if not a leaf
		if(!m_pComm->m_LeafModeActive)
		{
			NetworkPong += _T("Port: ") + DWrdtoStr(m_pComm->m_CurrentPort) + _T("\r\n");

			if(theApp.m_spGD->m_NetworkModel == NETWORK_PRIVATE)
			{
				// LAN header
				if(theApp.m_spGD->m_Lan)
					NetworkPong += _T("LAN: ") + theApp.m_spGD->m_LanName + _T("\r\n");

				// IRC header
				if(theApp.m_spGD->m_InternalIRC)
					NetworkPong += _T("IRC: ") + theApp.m_spGD->m_InternalIRCAddr + _T("\r\n");
			}

			// Leaves header
			if(m_pComm->m_NodeCapability == CLIENT_ULTRAPEER)
			{
				int Leaves = m_pComm->CountLeafConnects();

				if(Leaves)
					NetworkPong += _T("Leaves: ") + DWrdtoStr(Leaves) + _T("\r\n");
			}


			NetworkPong += _T("\r\n");

			int pos = strBuffer.Find(_T("\r\nPort: "));
			if(pos != -1)
			{
				pos += 2;
				_stscanf((TCHAR*)buffer + pos, _T("Port: %d\r\n"), &Port);
			}
			
			SendTo(NetworkPong, NetworkPong.GetLength(), UDP_PORT, Host);
		}
	}

	// Extract Data from ping/pong
	if(strBuffer.Find(NetworkPing) == 0 || strBuffer.Find(NetworkPong) == 0)
	{
		int pos, backpos;

		pos = strBuffer.Find(_T("\r\nPort: "));
		if(pos != -1)
		{
			pos += 2;
			_stscanf((TCHAR*)buffer + pos, _T("Port: %d\r\n"), &Port);
		}

		pos = strBuffer.Find(_T("\r\nLAN: "));
		if(pos != -1)
		{
			pos += 2;
			backpos = strBuffer.Find(_T("\r\n"), pos);
			LanName = strBuffer.Mid(pos + 5, backpos - pos - 5);
		}

		pos = strBuffer.Find(_T("\r\nIRC: "));
		if(pos != -1)
		{
			pos += 2;
			backpos = strBuffer.Find(_T("\r\n"), pos);
			IRCAddr = strBuffer.Mid(pos + 5, backpos - pos - 5);
		}

		pos = strBuffer.Find(_T("\r\nInfoURL: "));
		if(pos != -1)
		{
			pos += 2;
			backpos = strBuffer.Find(_T("\r\n"), pos);
			InfoURL = strBuffer.Mid(pos + 9, backpos - pos - 9);
		}

		pos = strBuffer.Find(_T("\r\nLeaves: "));
		if(pos != -1)
		{
			pos += 2;
			backpos = strBuffer.Find(_T("\r\n"), pos);
			Leaves  = _ttoi( strBuffer.Mid(pos + 8, backpos - pos - 8) );
		}
	}

	
	// Handle Pong over LAN
	if(strBuffer.Find(NetworkPong) == 0)
	{
		CString Extra;

		if(Leaves)
			Extra = _T(" with ") + DWrdtoStr(Leaves) + _T(" leaves");
	}

	for(int i = 0; i < m_HostList.size(); i++)
		if( m_HostList[i].Host == Host && m_HostList[i].Port == Port)
			return;


	LanNode LocalNode;
	LocalNode.Host		= Host;
	LocalNode.Port		= Port;
	LocalNode.Name		= LanName;
	LocalNode.IRCserver = IRCAddr;
	LocalNode.InfoPage	= InfoURL;
	LocalNode.Leaves	= Leaves;
	m_HostList.push_back(LocalNode);

	CAsyncSocket::OnReceive(nErrorCode);
}
