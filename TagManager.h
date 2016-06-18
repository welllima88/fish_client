// TagManager.h: interface for the CTagManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAGMANAGER_H__84E82268_68AF_424B_AD24_18C77DEAEFD7__INCLUDED_)
#define AFX_TAGMANAGER_H__84E82268_68AF_424B_AD24_18C77DEAEFD7__INCLUDED_

/**************************************************************************
 * class CTagManager
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Fish의 Tag를 관리하기 위한 클래스이다.
 **************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTagManager  
{
protected:
	void StopThread();
	BOOL StartThread();
	void InitStatic();
	void DeinitStatic();

	static void Trigger();
	static void TagProcess();

	static DWORD WINAPI TagThread();
	static HANDLE m_hThread;
	static HANDLE m_hThreadEvent;
	static BOOL m_bThread;

	static int m_nInit;
	BOOL m_Init;
public:
	CTagManager();
	virtual ~CTagManager();

};

#endif // !defined(AFX_TAGMANAGER_H__84E82268_68AF_424B_AD24_18C77DEAEFD7__INCLUDED_)
