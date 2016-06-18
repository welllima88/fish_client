#if !defined(AFX_SERVER2SERVERSOCK_H__92C6AA53_D3C4_4090_AF3B_D8047D8C6B6B__INCLUDED_)
#define AFX_SERVER2SERVERSOCK_H__92C6AA53_D3C4_4090_AF3B_D8047D8C6B6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Server2ServerSock.h : header file
//
#include "IrcSocket.h"

#define UM_CLOSE_SOCK		WM_USER+400
#define UM_CLOSE_SOCK_ERR	WM_USER+401

/////////////////////////////////////////////////////////////////////////////
// CServer2ServerSock command target

class CServer2ServerSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CServer2ServerSock();
	virtual ~CServer2ServerSock();
	
	void SetCommand(CString buf,int len);
	void SetHWND(HWND hwnd);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServer2ServerSock)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CServer2ServerSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	BYTE	*bySend,*nAddress;
	int			m_len;
	HWND		m_hWND;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVER2SERVERSOCK_H__92C6AA53_D3C4_4090_AF3B_D8047D8C6B6B__INCLUDED_)
