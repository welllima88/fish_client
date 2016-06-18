/*
 *  CFileLock
 *	Performs thread safe file access
 *  Makslane Araujo Rodrigues
 *	makslane@hotmail.com
 *	jun/2002
 */

#if !defined(AFX_FILELOCK_H__4451A848_8712_48BC_A00F_62DC79F8B864__INCLUDED_)
#define AFX_FILELOCK_H__4451A848_8712_48BC_A00F_62DC79F8B864__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

class CAutoLock
{
public:
	CAutoLock(CCriticalSection *pCriticalSection)
	{
		this->pCriticalSection = pCriticalSection;
		pCriticalSection->Lock();
	}

	~CAutoLock()
	{
		pCriticalSection->Unlock();
	}

	CCriticalSection *pCriticalSection;
};

class CFileLock : public CFile  
{
public:
	void Unlock();
	void Lock();
	
	CFileLock();
	virtual ~CFileLock();

	DWORD GetPosition();
	BOOL GetStatus(CFileStatus& rStatus);
	CString GetFileName();
	CString GetFileTitle();
	CString GetFilePath();
	void SetFilePath(LPCTSTR lpszNewName);

// Operations
	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

	
	DWORD SeekToEnd();
	void SeekToBegin();

	// backward compatible ReadHuge and WriteHuge
	// DWORD ReadHuge(void* lpBuffer, DWORD dwCount);
	// void WriteHuge(const void* lpBuffer, DWORD dwCount);

// Overridables
	CFile* Duplicate();

	LONG Seek(LONG lOff, UINT nFrom);
	void SetLength(DWORD dwNewLen);
	DWORD GetLength();

	UINT Read(void* lpBuf, UINT nCount);
	void Write(const void* lpBuf, UINT nCount);

	void LockRange(DWORD dwPos, DWORD dwCount);
	void UnlockRange(DWORD dwPos, DWORD dwCount);

	void Abort();
	void Flush();
	void Close();



	CCriticalSection m_CriticalSection;
};

#endif // !defined(AFX_FILELOCK_H__4451A848_8712_48BC_A00F_62DC79F8B864__INCLUDED_)
