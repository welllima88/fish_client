/*
 *  CFileLock
 *	Performs thread safe file access
 *  Makslane Araujo Rodrigues
 *	makslane@hotmail.com
 *	jun/2002
 */


#include "stdafx.h"
#include "FileLock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLock::CFileLock()
{

}

CFileLock::~CFileLock()
{

}

DWORD CFileLock::GetPosition()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetPosition();
}

BOOL CFileLock::GetStatus(CFileStatus& rStatus)
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetStatus(rStatus);
}

CString CFileLock::GetFileName()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetFileName();
}

CString CFileLock::GetFileTitle()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetFileTitle();
}

CString CFileLock::GetFilePath()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetFilePath();
}

void CFileLock::SetFilePath(LPCTSTR lpszNewName)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::SetFilePath(lpszNewName);
}

BOOL CFileLock::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::Open(lpszFileName, nOpenFlags, pError);
}


DWORD CFileLock::SeekToEnd()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::SeekToEnd();
}

void CFileLock::SeekToBegin()
{
	CAutoLock lock(&m_CriticalSection);

	CFile::SeekToBegin();
}


/*DWORD CFileLock::ReadHuge(void* lpBuffer, DWORD dwCount)
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::ReadHuge(lpBuffer, dwCount);
}

void CFileLock::WriteHuge(const void* lpBuffer, DWORD dwCount)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::WriteHuge(lpBuffer, dwCount);
}*/


CFile* CFileLock::Duplicate()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::Duplicate();
}

LONG CFileLock::Seek(LONG lOff, UINT nFrom)
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::Seek(lOff, nFrom);
}

void CFileLock::SetLength(DWORD dwNewLen)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::SetLength(dwNewLen);
}

DWORD CFileLock::GetLength()
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::GetLength();
}

UINT CFileLock::Read(void* lpBuf, UINT nCount)
{
	CAutoLock lock(&m_CriticalSection);

	return CFile::Read(lpBuf, nCount);
}

void CFileLock::Write(const void* lpBuf, UINT nCount)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::Write(lpBuf, nCount);
}

void CFileLock::LockRange(DWORD dwPos, DWORD dwCount)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::LockRange(dwPos, dwCount);
}

void CFileLock::UnlockRange(DWORD dwPos, DWORD dwCount)
{
	CAutoLock lock(&m_CriticalSection);

	CFile::UnlockRange(dwPos, dwCount);
}

void CFileLock::Abort()
{
	CAutoLock lock(&m_CriticalSection);

	CFile::Abort();
}

void CFileLock::Flush()
{
	CAutoLock lock(&m_CriticalSection);

	CFile::Flush();
}

void CFileLock::Close()
{
	CAutoLock lock(&m_CriticalSection);

	CFile::Close();
}

void CFileLock::Lock()
{
	m_CriticalSection.Lock();
}

void CFileLock::Unlock()
{
	m_CriticalSection.Unlock();
}
