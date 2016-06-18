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
#include "Packet.h"
#include "Conversions.h"

// Get rid of ugly warnings
#pragma warning (disable : 4786)



Node::Node(CString HostPort)
{ 
	*this = HostPort; 
}

Node::Node(CString nHost, UINT nPort)
{
	Host = nHost;
	Port = nPort;
	Time = CTime::GetCurrentTime();
}
	
// Allow Node = "host:port" assignment
Node& Node::operator=(CString &rhs)
{
	int pos = rhs.Find(_T(":")) + 1;

	// check for a valid string
	if (pos <= 1 || pos >= rhs.GetLength())
	{
		Host = _T("");
		Port = 0;
	}
	else
	{
		Host = rhs.Left(pos - 1);
		Port = _ttoi(rhs.Mid(pos, rhs.GetLength() - pos));
		Time = CTime::GetCurrentTime();
	}
	return *this;
}

CString Node::GetString()
{
	return (Host + _T(":") + DWrdtoStr(Port));
}
