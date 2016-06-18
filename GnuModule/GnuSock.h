#if !defined(AFX_GNUSOCK_H__D6585581_8996_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUSOCK_H__D6585581_8996_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGnuControl;

class CGnuSock : public CAsyncSocket
{
public:
	CString m_Sndshake;

	CGnuSock(CGnuControl*);
	virtual ~CGnuSock();

	void Timer();

	int   m_nSecsAlive;
	bool  m_bDestroy;
	bool  m_ServingHTTP;

	CString m_Host;
	UINT    m_nPort;

	//by bemlove
private:
	bool	m_bFolderRequest;
public:
	inline  void	SetHandshake( CString msg )				{ m_Handshake = msg ; };

	//{{AFX_VIRTUAL(CGnuSock)
	virtual void OnReceive(int nErrorCode);

	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void OnSend(int nErrorCode);
	virtual void Close();
	virtual void OnConnect(int nErrorCode);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

protected:

	CGnuControl*  m_pComm;

	CString m_Handshake;

	bool m_DataSending;

	byte m_pBuff[4096];
	DWORD m_dwBuffLength;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNUSOCK_H__D6585581_8996_11D4_ACF2_00A0CC533D52__INCLUDED_)
