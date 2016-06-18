// GnuDoc.h: interface for the CGnuDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GNUDOC_H__B931486F_B1F1_415E_A321_C175BA826858__INCLUDED_)
#define AFX_GNUDOC_H__B931486F_B1F1_415E_A321_C175BA826858__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Packet.h"
#include "GnuDefines.h"
#include "GnuControl.h"

class CNodeMap
{
	// attributes
public:
	std::vector<GNUSeed2>	m_nodeList;
	CCriticalSection		m_nodeAccess;

	// member functions
public:
	CNodeMap();
	virtual ~CNodeMap();
	bool Init(void);
	bool Push_back( GNUSeed2 seed );
	bool Push_back( TCHAR* tszIP, UINT port, int hops=0 );
	bool Pop(GNUSeed2& seed);
	__inline UINT Size()		{ return m_nodeList.size(); }
protected:
	bool	Find(TCHAR* tszIP);
}; //end of class CNodeMap


class CGnuDoc  
{
protected:
	// GNU HOST PORT & IP
	int			m_nGnuHostPort;
	TCHAR		m_szHostIP[MAX_IP+1];

	CString			m_szHostIPList[MAX_HOSTIP_COUNT];
	int				m_iIPCount;

	// 사용자 초기화 여부
	BOOL		m_bUserInit;

public:
	void ClearAll();
	BOOL ReqSeedNodeList();
	void LoadProperties(CString userid = _T(""));
	void SaveProperties();

	inline void SetMaxInConnection( int cur )				{ m_MaxInConnects = cur; }
	inline int GetMaxInConnection()							{ return m_MaxInConnects; }
	inline void SetMaxOutConnection( int cur )				{ m_MaxOutConnects = cur; }
	inline int GetMaxOutConnection()						{ return m_MaxOutConnects; }
	inline void	SetMaxDownload( int max )					{ m_MaxDownloads = max; }
	inline int GetMaxDownload( void )						{ return m_MaxDownloads; }
	inline void	SetMaxUpload( int max )						{ m_MaxUploads = max; };
	inline int GetMaxUpload( void )							{ return m_MaxUploads; };
	inline void SetCurConnection( int cur )					{ m_iCurConnection = cur; }
	inline int GetCurConnection()							{ return m_iCurConnection; }

	inline void	SetCurDownload( int cur )					{ m_iCurDownload = cur; }
	inline int GetCurDownload( void )						{ return m_iCurDownload; }
	inline void	IncreaseCurDownload(void)					{ m_iCurDownload++; }
	inline void	DecreaseCurDownload(void)					{ if(m_iCurDownload <= 0 ) m_iCurDownload = 0; else m_iCurDownload--; }

	inline void SetCurUpload(int cur)						{ m_iCurUpload = cur; }
	inline int GetCurUpload()								{ return m_iCurUpload; }
	inline void IncreaseCurUpload()							{ m_iCurUpload++; }
	inline void DecreaseCurUpload()							{ if(m_iCurUpload <= 0) m_iCurUpload = 0; else m_iCurUpload--; }

	inline void	SetHostIP( TCHAR* ip )						{ if(_tcslen(ip) < sizeof(m_szHostIP)) _tcscpy( m_szHostIP, ip )
																; m_LocalHost.S_addr = StrtoIP(ip).S_addr; };
	inline TCHAR* GetHostIP( void )							{ return m_szHostIP; };

	void SetHostIPList();

	inline void SetGnuHostPort(int nPort)					{ m_nGnuHostPort = nPort; }
	inline int	GetGnuHostPort()							{ return m_nGnuHostPort; }	

	CGnuDoc();
	virtual ~CGnuDoc();

	CGnuControl	m_Control;

	bool		m_LocalMapping;

	GUID		m_ClientID;

	IP			m_LocalHost;			// IP of node
	IP			m_ForcedHost;			// IP of node assigned by user
	UINT		m_LocalPort;			// Port of node	
	UINT		m_ForcedPort;			// Port of node assigned by user
	DWORD		m_SpeedStat;			// Node speed determined by the user - Not Uing, const 0
	DWORD		m_SpeedDyn;				// Node speed determined by Manage Node

	int			m_NetworkModel;			// const

	bool		m_Lan;
	CString		m_LanName;				// const NULL
	bool		m_InternalIRC;
	CString		m_InternalIRCAddr;		// const NULL

	bool		m_SuperNodeAble;		// const false -- Super Node는 사용하지 않는다. (Server Only??)
	int			m_MaxLeaves;			// const 400 -- Super Node일 경우에만 유효하다.

	bool		m_BehindFirewall;		// const false -- FireWall 검사는 안함

	// UP/DOWN BANDWIDTH CONTROL
	int			m_BandwidthDown;
	int			m_BandwidthUp;

	int			m_MinDownSpeed;
	int			m_MinUpSpeed;

	// REAL SPEED
	int			m_RealSpeedDown;
	int			m_RealSpeedUp;

	// AUTO CONNECTION
	bool	    m_AutoConnect;			// const false
	int			m_LeafModeConnects;		// const 5

	// OS TYPE
	bool		m_IsKernalNT;

	bool		ModeVersion6;
	CString		ModeNetwork;

	// MULTI SOURCE DOWNLOAD FLAG
	bool		m_Multisource;

	// Node 연결을 위한 정보를 관리한다.
	CNodeMap	m_nodeCacheMap;			// IRCD, Loader, Control, Node에서 사용된다.

	// MAXIMUM CONNECTION COUNT
	int			m_MaxInConnects;
	int			m_MaxOutConnects;
	int			m_MinConnects;
	int			m_MaxDownloads;
	int			m_MaxUploads;

	// CURRENT CONNECTION COUNT
	int			m_iCurDownload;
	int			m_iCurUpload;
	int			m_iCurConnection;

	// CHECKING WHETHER THIS NODE IS ABLE TO UPDLOAD
	bool		m_RealFirewall;		   // Assumes there is a firewall until someone connects
	bool		m_HaveUploaded;		   // If client has uploaded successfully
};

#endif // !defined(AFX_GNUDOC_H__B931486F_B1F1_415E_A321_C175BA826858__INCLUDED_)
