#if !defined(AFX_GNUUPLOAD_H__81A71841_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUUPLOAD_H__81A71841_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGnuUploadShell;
class CGnuShare;

class CGnuUpload : public CAsyncSocket
{
	DECLARE_DYNCREATE(CGnuUpload)

public:
	CGnuUpload();
	CGnuUpload(CGnuUploadShell*);
	virtual ~CGnuUpload();

	void Send_PostOK();

	void Send_HttpOK();
	void Send_HttpBusy();
	void Send_HttpFailed();
	void Send_HttpNotFound();

	void Timer();


	bool m_ThreadRunning;
	
	DWORD m_BytesRead;
	DWORD m_BytesSent;

	CString m_GetRequest;

	CEvent		 m_CanWrite;
	CEvent		 m_MoreBytes;

	CWinThread*  m_pUploadThread;
	
	CGnuUploadShell* m_pShell;


	//{{AFX_VIRTUAL(CGnuUpload)
	public:
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	virtual void Close();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuUpload)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


protected:	
	bool m_DataSending;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNUUPLOAD_H__81A71841_D0FA_11D4_ACF2_00A0CC533D52__INCLUDED_)
