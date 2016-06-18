// GnuDoc.cpp: implementation of the CGnuDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../fish.h"
#include "GnuDoc.h"
#include "Conversions.h"
#include "Common.h"
#include "../fish_common.h"
#include "../LocalProperties.h"
#include "../IRCModule/httpd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	MAX_HOSTIP_COUNT	10

extern CFISHApp theApp;

//============================================================================
// NAME : class CNodeMap
// DESC : 
// Date : 2005-03-30 coded by bemlove, origin
//============================================================================
CNodeMap::CNodeMap()
{
}

CNodeMap::~CNodeMap()
{
}

bool CNodeMap::Init()
{
	m_nodeList.clear();
	return true;
}

bool CNodeMap::Push_back( TCHAR* tszIP, UINT port, int hops )
{
	ASSERT( tszIP && port > 0 );
	if( tszIP == NULL || _tcslen(tszIP)==0 || port <= 0 )
	{
		return false;
	}
	GNUSeed2 seed;
	seed.set( tszIP, port, hops );
	return Push_back( seed );
}

bool CNodeMap::Push_back( GNUSeed2 seed )
{
	ASSERT(seed.m_szSeedIP && seed.m_iSeedPort > 0 );
	if( seed.m_szSeedIP == NULL || _tcslen(seed.m_szSeedIP)==0 || seed.m_iSeedPort <= 0 )
	{
		return false;
	}
	if( Find( seed.m_szSeedIP ) )
	{
		return false;
	}
	// Hops 높음에 따른 우선 순위로 Insert
	// Start
	std::vector<GNUSeed2>::iterator itrNode;
	bool bInserted = false;
	m_nodeAccess.Lock();
		for( itrNode = m_nodeList.begin(); itrNode != m_nodeList.end(); itrNode++ )
		{
			if( seed.m_nHops >= (*itrNode).m_nHops )
			{
				m_nodeList.insert(itrNode, seed);
				bInserted = true;
				break;
			}
		}
		if( !bInserted )
			m_nodeList.push_back( seed );

		// 추가된 사항에 대해 삭제..
		if( m_nodeList.size() > MAX_NODEMAP )
		{
			itrNode = m_nodeList.end()-1;
			m_nodeList.erase(itrNode);
		}
	m_nodeAccess.Unlock();
	//end 홉 점검

	return true;
}// end of Push_back()

bool CNodeMap::Pop(GNUSeed2& seed)
{
	if( m_nodeList.empty() )
	{
		return false;
	}

	std::vector<GNUSeed2>::iterator itrNode;
	m_nodeAccess.Lock();
	itrNode = m_nodeList.begin();
	if( itrNode == m_nodeList.end() )
	{
		m_nodeAccess.Unlock();
		return false;
	}
	seed.set( (*itrNode).m_szSeedIP, (*itrNode).m_iSeedPort );
	m_nodeList.erase( itrNode );
	m_nodeAccess.Unlock();
	return true;
}//end of Pop()

bool CNodeMap::Find(TCHAR* tszIP)
{
	ASSERT( tszIP );
	if( m_nodeList.empty() )
	{
		return false;
	}
	std::vector<GNUSeed2>::iterator itrNode;
	m_nodeAccess.Lock();
	for( itrNode = m_nodeList.end(); itrNode != m_nodeList.begin(); itrNode-- )
	{
		if( _tcscmp( (*itrNode).m_szSeedIP, tszIP ) == 0 )
		{
			m_nodeAccess.Unlock();

			return true;
		}
	}
	m_nodeAccess.Unlock();
	return false;
}//end of Size

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGnuDoc::CGnuDoc()
{
	m_bUserInit				= FALSE;
	m_LocalMapping			= true;
	m_nGnuHostPort			= DEFAULT_GNUPORT;

	m_LocalHost				= StrtoIP(_T("127.0.0.1"));
	m_ForcedHost			= StrtoIP(_T("0.0.0.0")); 
	srand(time_t(NULL));
	m_LocalPort				= rand() % 22500 + 2500;
	m_ForcedPort			= 0;
	m_SpeedStat				= 0;
	m_SpeedDyn				= 0;
	GnuCreateGuid(&m_ClientID);

	m_NetworkModel			= NETWORK_INTERNET;
	m_SuperNodeAble			= false;
	m_MaxLeaves				= 400;

	m_Lan					= true;
	m_InternalIRC			= false;

	// Local Firewall
	m_BehindFirewall		= false;

	// Connect
	m_AutoConnect			= false;
	m_LeafModeConnects		= 5;


	m_MaxInConnects			= DEFAULT_MAXINCONNECTS;
	m_MaxOutConnects		= DEFAULT_MAXOUTCONNECTS;

	m_MinConnects			= 0;

	// Transfer
	m_Multisource			= false;
	m_MaxDownloads			= DEFAULT_MAXDOWNLOADS;
	m_MaxUploads			= DEFAULT_MAXUPLOADS;

	// Bandwidth
	m_BandwidthUp			= 0;
	m_BandwidthDown			= 0;
	m_MinDownSpeed			= 0;
	m_MinUpSpeed			= 0;

	// Determine what kind of system we're running
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);

	if(osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
		m_IsKernalNT = true;
	else
		m_IsKernalNT = false;
	
	ModeVersion6			= VERSION6_ISTRUE;
	ModeNetwork				= _T("GNUTELLA");

	m_RealFirewall			= true;
	m_HaveUploaded			= false;

	m_iIPCount				= 0;
}

CGnuDoc::~CGnuDoc()
{
	TRACE(_T("CGnuDoc Destroy\r\n"));
}

/**************************************************************************
 * method CGnuDoc::LoadProperties
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-??
 *
 * @Description
 * 설정값을 레지스트리에서 읽어온다.
 * 설정 값은 CGnuDoc 과 CLocalProperties 두 곳에서 관리된다.
 *
 * @Parameters
 * (in CString) userid - 로그인 한 사용자의 id
 **************************************************************************/
void CGnuDoc::LoadProperties(CString userid)
{
	CString tmp;
	tmp = ReadRegData(userid, REG_STR_MAXDOWNLOADS);
	m_MaxDownloads = _ttoi((LPCTSTR) tmp);
	if(m_MaxDownloads < 5) 
	{
		m_MaxDownloads = 5;
		tmp.Format(_T("%d"), m_MaxDownloads);
		WriteRegData(userid, REG_STR_MAXDOWNLOADS, tmp);
	}

	tmp = ReadRegData(userid, REG_STR_MAXUPLOADS);
	m_MaxUploads = _ttoi((LPCTSTR) tmp);
	if(m_MaxUploads < 5) 
	{
		m_MaxUploads = 5;
		tmp.Format(_T("%d"), m_MaxUploads);
		WriteRegData(userid, REG_STR_MAXUPLOADS, tmp);
	}

	// Bandwidth
	tmp = ReadRegData(userid, REG_STR_BANDWIDTHUP);
	m_BandwidthUp = _ttoi((LPCTSTR) tmp);
	if(m_BandwidthUp < 10) 
	{
		m_BandwidthUp = 0;
		tmp.Format(_T("%d"), m_BandwidthUp);
		WriteRegData(userid, REG_STR_BANDWIDTHUP, tmp);
	}

	tmp = ReadRegData(userid, REG_STR_BANDWIDTHDOWN);
	m_BandwidthDown = _ttoi((LPCTSTR) tmp);
	if(m_BandwidthDown < 0) 
	{
		m_BandwidthDown = 0;
		tmp.Format(_T("%d"), m_BandwidthDown);
		WriteRegData(userid, REG_STR_BANDWIDTHDOWN, tmp);
	}

	tmp = ReadRegData(userid, REG_STR_GNUHOSTPORT);
	m_nGnuHostPort = _ttoi((LPCTSTR) tmp);
	
	if(m_nGnuHostPort < 1000)
	{
		m_nGnuHostPort = DEFAULT_GNUPORT;
		tmp.Format(_T("%d"), m_nGnuHostPort);
		WriteRegData(userid, REG_STR_GNUHOSTPORT, tmp);
	}

	m_bUserInit = TRUE;
}

/**************************************************************************
 * method CGnuDoc::SaveProperties
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-??
 *
 * @Description
 * 설정값을 레지스트리에 저장한다.
 * 설정 값은 CGnuDoc 과 CLocalProperties 두 곳에서 관리된다.
 *
 * @Parameters
 **************************************************************************/
void CGnuDoc::SaveProperties()
{
	if(!m_bUserInit) return;

	// REGISTRY RECORD
	CString tmp;
	tmp.Format(_T("%d"), m_MaxDownloads);
	WriteRegData(theApp.m_spLP->GetUserID(), REG_STR_MAXDOWNLOADS, tmp);

	tmp.Format(_T("%d"), m_MaxUploads);
	WriteRegData(theApp.m_spLP->GetUserID(), REG_STR_MAXUPLOADS, tmp);

	tmp.Format(_T("%d"), m_BandwidthDown);
	WriteRegData(theApp.m_spLP->GetUserID(), REG_STR_BANDWIDTHDOWN, tmp);

	tmp.Format(_T("%d"), m_BandwidthUp);
	WriteRegData(theApp.m_spLP->GetUserID(), REG_STR_BANDWIDTHUP, tmp);

	tmp.Format(_T("%d"), m_nGnuHostPort);
	WriteRegData(theApp.m_spLP->GetUserID(), REG_STR_GNUHOSTPORT, tmp);
}

/**************************************************************************
 * method CGnuDoc::ReqSeedNodeList
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-26
 *
 * @Description
 * 지정된 WebPage를 이용하여 Seed Node를 확보한다.
 *
 * @Parameters
 **************************************************************************/
BOOL CGnuDoc::ReqSeedNodeList()
{
	CMemFile SNList;
	TCHAR line[MAX_BUFF+1]={0};
	CString str = _T(""), rstr;

	str.Format(_T("%s?cc=0003"), URL_WEBDB);

	if( !GetHttpFileAsc( HOST_NAME, 
		80, (LPTSTR)((LPCTSTR )str), &SNList, NULL, NULL) ){
		// ERROR : URL, PORT, filename중 하나가 비정상
		SNList.Close();
		return FALSE;
	}
	
	// 요청 결과
	if( GetLine( &SNList, line) < 0)
	{
		// ERROR임
		SNList.Close();
		return FALSE;
	}

	int ret = 0;
	// 결과 오류 검색
	BOOL ischeck = CheckResultCode( line , ret );
	if(ret != 1)
	{
		SNList.Close();
		return FALSE;
	}

	CString strHOSTIP;
	int nGnuPort;
	int nodetype = 0;

#ifdef _DEBUG
	int nodecnt = 0;
#endif
	
	while(1)
	{
		if( GetLine( &SNList, line) < 0)
		{
			break;
		}

		strHOSTIP.Empty();
		nGnuPort = 0;

		TCHAR* tok = NULL;
		tok = _tcstok( line, DELIM_NODE );
		while( tok != NULL )
		{
			strHOSTIP = tok;
			
			tok = _tcstok( NULL, DELIM_NODE );
			
			if( tok != NULL )
			{
				nGnuPort = _ttoi(tok);
				nodetype = 1;
				tok = _tcstok( NULL, DELIM_NODE );
				if(tok != NULL) nodetype = _ttoi(tok);

				m_nodeCacheMap.Push_back( (LPTSTR)(LPCTSTR)strHOSTIP, nGnuPort );

#ifdef _DEBUG
				++nodecnt;
#endif
			}

			// you can get type
		}			
	}

#ifdef _DEBUG
	TRACE(_T("total gained node count : %d\r\n"), nodecnt);
#endif
	
	SNList.Close();
	return TRUE;
}

/*
** NAME    **: SetHostIPList
** PARAM   **: void
** RETURN  **: void
** DESC    **: 현재 컴퓨터에서 사용중인 아이피 리스트를 얻는다.
** by bemlove at 2002-12-09, VER : 1.0, COMMENTS: ORG 
*/
void CGnuDoc::SetHostIPList()
{
	char hostname[MAX_IP + 1];
	int i = 0;
	IP	hostip;
	hostent *hostinfo;

	if( gethostname ( hostname, sizeof(hostname) ) == 0 ) 
	{
		if ( ( hostinfo = gethostbyname ( (const char *) hostname ) ) != NULL )
		{
			while ( *(hostinfo->h_addr_list + i) && i < MAX_HOSTIP_COUNT )
			{
				hostip.S_addr = (*(struct in_addr*) *(hostinfo->h_addr_list + i)).S_un.S_addr;
				m_szHostIPList[i] = IPtoStr(hostip);
				i ++;
			}
		}
	}

	m_iIPCount = i;
}//end of SetHostIPList()

void CGnuDoc::ClearAll()
{
	m_Control.GnuConRelease();
}
