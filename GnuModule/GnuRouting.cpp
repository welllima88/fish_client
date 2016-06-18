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
#include "GnuNode.h"
#include "GnuRouting.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CGnuRouting::CGnuRouting()
{
	m_nHashEntries = 0;

	m_nCurrent     = 0;
	m_nOld         = 1;

	RefreshTime  = CTime::GetCurrentTime();
	HashTimeSpan = RefreshTime - RefreshTime;

	ClearTable(0);
	ClearTable(1);

	// Add[Modify] at 2003-03-31 오후 9:16:11 by bemlove 
	m_nCurrentPos	= 0;
	m_bCycled		= false;
}

CGnuRouting::~CGnuRouting()
{
	m_AccessTable.Lock();

		for(int i = 0; i < TABLE_SIZE; i++)
		{
			while(m_Table[0][i].size())
				m_Table[0][i].clear();

			while(m_Table[1][i].size())
				m_Table[1][i].clear();
		}

	m_AccessTable.Unlock();
}


void CGnuRouting::Insert(GUID* pGuid, CGnuNode* pOrigin)
{
	DWORD key = 0;
	key = CreateKey(pGuid);

	m_AccessTable.Lock();

		// Check to see if table is full
		if(m_Table[m_nCurrent][key].size() >= MAX_REHASH || m_nHashEntries >= TABLE_SIZE * 4)  
		{
			// Time to clean out the old and switch
			ClearTable(m_nOld);

			int temp = m_nCurrent;
			m_nCurrent = m_nOld;
			m_nOld = temp;
		}
		
		key_Value new_val;
		new_val.Guid   = *pGuid;
		new_val.Origin = pOrigin;


		m_Table[m_nCurrent][key].push_back(new_val);

		m_nHashEntries++;

		//if(m_nHashEntries % 50 == 0)
		//	TRACE0( "Hash " + DWrdtoStr(m_nHashEntries) + "\n");

	m_AccessTable.Unlock();
}


key_Value* CGnuRouting::FindValue(GUID *pGuid)
{
	DWORD key = CreateKey(pGuid);

	m_AccessTable.Lock();

		int nAttempts = m_Table[m_nCurrent][key].size();  

		// Search the current table
		while(nAttempts--)
		{
			if(CompareGuid(&m_Table[m_nCurrent][key][nAttempts].Guid, pGuid))
			{
				m_AccessTable.Unlock();
				return &m_Table[m_nCurrent][key][nAttempts];
			}
		}

		// Try the old table
		nAttempts = m_Table[m_nOld][key].size();  
		while(nAttempts--)
		{
			if(CompareGuid(&m_Table[m_nOld][key][nAttempts].Guid, pGuid))
			{
				m_AccessTable.Unlock();
				return &m_Table[m_nOld][key][nAttempts];
			}
		}

	m_AccessTable.Unlock();

	// not found ...
	return NULL;
}


DWORD CGnuRouting::CreateKey(GUID *pGuid)
{
	byte pGuidRaw[sizeof(GUID)+1] ={0};
	memcpy( pGuidRaw, pGuid, sizeof(GUID) );
//	byte* pGuidRaw = (byte*) pGuid;
	// XOR every 4 bytes together ...
	DWORD key =  0;
	key = 	(pGuidRaw[0] ^ pGuidRaw[4] ^ pGuidRaw[8] ^ pGuidRaw[12]) +
			256 * (pGuidRaw[1] ^ pGuidRaw[5] ^ pGuidRaw[9] ^ pGuidRaw[13]) +
			256 * 256 * (pGuidRaw[2] ^ pGuidRaw[6] ^ pGuidRaw[10] ^ pGuidRaw[14]) +
			256 * 256 * 256 * (pGuidRaw[3] ^ pGuidRaw[7] ^ pGuidRaw[11] ^ pGuidRaw[15]);

	// And modulo it down to size
	key %= TABLE_SIZE;
	return key;
}


bool  CGnuRouting::CompareGuid(GUID* pGuid1, GUID* pGuid2)
{
	if(memcmp(pGuid1, pGuid2, 16) == 0)
		return true;

	return false;
}


// Secure (the function that calls this is locked)
void  CGnuRouting::ClearTable(int nWhich)
{
	for(int i = 0; i < TABLE_SIZE; i++)
	{
		while(m_Table[nWhich][i].size())
			m_Table[nWhich][i].clear();

		m_Table[nWhich][i].reserve(10);
	}


	HashTimeSpan = CTime::GetCurrentTime() - RefreshTime;
	RefreshTime  = CTime::GetCurrentTime();

	m_nHashEntries = 0;
}



//**************************************************************************************
// NAME :  InsertValue / FindSearchValue
// PARM :  char* id, char* word
// RETN :  void
// DESC :  검색쿼리가 중복 체크가 제대로 되지 않은 관계로(GUID)값이 유지가 안된다...
//		   그래서 쿼리의 중복 검사를 하기위해서 GUID를 사용하지않고, 사용자와 검색어를 통하여 
//		   100개를 기본으로 해서 유지하도록 한다.
// COMT : by bemlove 2003-03-31 오후 9:18:04 ver - 1. 0 
//**************************************************************************************
void CGnuRouting::InsertValue( char* id, char* word )
{
	if( !strlen( id ) || !strlen( word ) ) return;
	m_searchValue[m_nCurrentPos++].setValue( id, word );

	// One more cycled.. then go to start position
	if( m_nCurrentPos >=MAX_SEARCH_VALUE )
	{
		m_bCycled = true;
		m_nCurrentPos = 0;
	}
}
bool CGnuRouting::FindSearchValue( char* id, char* word )
{
	if( !strlen( id ) || !strlen( word ) ) return false;

	search_Value searchValue;
	searchValue.setValue( id, word );
	int idx=0;
	// one more cycled...
	if( m_bCycled )
	{
		for(idx = 0; idx < MAX_SEARCH_VALUE; idx++ )
		{
			if( searchValue == m_searchValue[idx] )
				return true;
		}
	}
	else		// not cycled..
	{
		for(idx = 0; idx < m_nCurrentPos; idx++ )
		{
			if( searchValue == m_searchValue[idx] )
				return true;
		}
	}
	return false;
}