#if !defined(AFX_OPMLSOCK_H__F063E30D_14F2_42E0_A101_94F3BB59804C__INCLUDED_)
#define AFX_OPMLSOCK_H__F063E30D_14F2_42E0_A101_94F3BB59804C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OPMLSock.h : header file
//

/**************************************************************************
 * class COPMLSock
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2006-02-20
 *
 * @Description
 * OPML file을 전송하기 위한 소켓 클래스
 **************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// COPMLSock command target

class COPMLSock : public CAsyncSocket
{
// Attributes
public:
	CString		ip;
	int			port;
protected:
	int			m_FileLength;
	bool		m_DataSending;

	CString		m_sFileName;

	CEvent		m_CanWrite;
	HANDLE		m_hThread;
	CFile		m_file;

	HWND		m_hInform;

// Operations
public:
	COPMLSock();
	virtual ~COPMLSock();

protected:
	BOOL SendOPML();
	void CloseOPMLSock();
	static UINT WINAPI UploadThread(LPVOID lpOPML);

// Overrides
public:
	BOOL StartTransfer(CString sFileName);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COPMLSock)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COPMLSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPMLSOCK_H__F063E30D_14F2_42E0_A101_94F3BB59804C__INCLUDED_)
