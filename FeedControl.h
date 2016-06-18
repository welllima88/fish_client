// FeedControl.h: interface for the CFeedControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDCONTROL_H__B3484D49_C5A9_4F5A_93F7_1AEA56125491__INCLUDED_)
#define AFX_FEEDCONTROL_H__B3484D49_C5A9_4F5A_93F7_1AEA56125491__INCLUDED_

/**************************************************************************
 * class CFeedControl
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
 * Feed객체를 관리하기 위한 클래스이다.
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feeder.h"

class CFeedControl : public CObject 
{
protected:
	HWND m_hCallerWnd;
public:
	BOOL IsFeedAny();
	BOOL IsChannelFeeding(int channelid);
	void SetCallerWnd(HWND h) 
	{ 
		m_hCallerWnd = h; 
		CFeeder::SetCallerWnd(m_hCallerWnd);
	}
	void ClearFeed();
	void RemoveFeed(int channelid);
	BOOL AddFeed(int channelid, CString url, COleDateTime lasttime);
	void Feed();
	CFeedControl();
	virtual ~CFeedControl();
};

#endif // !defined(AFX_FEEDCONTROL_H__B3484D49_C5A9_4F5A_93F7_1AEA56125491__INCLUDED_)
