// Feeder.h: interface for the CFeeder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDER_H__07A154E7_57AA_4983_9792_DC23AFEDCEDA__INCLUDED_)
#define AFX_FEEDER_H__07A154E7_57AA_4983_9792_DC23AFEDCEDA__INCLUDED_

/**************************************************************************
 * class CFeeder
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-11-xx
 *
 * @Description
 * 한개의 Feed 작업을 수행하는 class이다.
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFeeder : public CObject 
{
public:
	int GetChannelID();
	BOOL IsFeeding();
	BOOL StartFeed();
	static void SetCallerWnd(HWND h);
	CFeeder(int channelid, CString url, COleDateTime lasttime);
	virtual ~CFeeder();

	HANDLE		m_hThread;
	BOOL		m_bThread;
	BOOL		m_bStop;

	BOOL		m_bToInform;
	
	static HWND	m_hCallerWnd;

	int			m_ichannelid;
	CString		m_surl;
	COleDateTime	m_dtlasttime;
	COleDateTime	m_dtstarttime;

protected:
	static DWORD WINAPI FeedThread(LPVOID lpparam);
};

#endif // !defined(AFX_FEEDER_H__07A154E7_57AA_4983_9792_DC23AFEDCEDA__INCLUDED_)
