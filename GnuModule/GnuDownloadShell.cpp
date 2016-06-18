/********************************************************************************

	Gnucleus - A node application for the gnutella network
    Copyright (C) 2001 John Marshall

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	For support, questions, comments, etc...
	E-Mail: 
		swabby@c0re.net
	
	Address:
		21 Cadogan Way
		Nashua, NH, USA 03062 

********************************************************************************/


#include "stdafx.h"

#include "../fish.h"

#include "GnuRouting.h"
#include "GnuControl.h"
#include "GnuNode.h"

#include "GnuDownload.h"
#include "GnuDownloadShell.h"

#include "GnuDoc.h"

#include "../LocalProperties.h"
#include "../XML/EzXMLParser.h"
#include "../SearchManager.h"
#include "../fish_common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AUTO_REQUERY
#define RETRY_WAIT						60
#define MAX_RETRY_COUNT					20
#define MAX_AUTOSEARCH_CNT				3
#define MAX_FAILCOUNT					2

bool CGnuDownloadShell::bStopThread = false;
bool CGnuDownloadShell::bMergeThread = false;
LPVOID CGnuDownloadShell::pCurMergeShell = NULL;
bool CGnuDownloadShell::pStopMerge = false;
CEvent CGnuDownloadShell::evMerge;
CCriticalSection CGnuDownloadShell::csMergeLock;
std::list<MergeItem*> CGnuDownloadShell::MergeList;

extern CFISHApp theApp;

CGnuDownloadShell::CGnuDownloadShell()
{
	m_Active			= false;
	m_Dead				= false;
	m_Cooling			= 0;
	m_Searching			= 0;
	// 호스트 정보가 없으면 대기중 처리를 한다.
	m_Waiting			= false;
	m_HostTryPos		= 1;
	
	m_DoReQuery			= false;
	m_TotalSecCount		= 0;

	// written by moonknit 2005-07-11
	m_RetryCount		= MAX_AUTOSEARCH_CNT;

	m_Retry				= false;
	m_UpdatedInSecond	= false;
	
	m_StopPos			= 0;
	m_BytesCompleted	= 0;
	m_StartBytes		= 0;
	m_OverlapBytes		= 0;

	// modified by moonknit 2005-10-12
	// set flag from false to true
	m_BackupTrigger		= true;

	HostFamilyAge		= 0;

	//Makslane: download icon status manager
	m_Hwnd				= 0;
	pGroup				= NULL;

	// Bandwidth
	m_AllocBytes		= 0;
	m_AllocBytesTotal	= 0;
	
	TotalPos			= 0;
	
	m_JoinChunk			= 0;

	m_Point				= 0;

	m_isPartial			= 1;

	m_CompleteDownload	= false;

	m_AreadyPayPoint	= false;

	m_bMerging			= false;
	m_bMerged			= false;
	m_bComplete			= false;
	m_bAllComplete		= false;
	m_nResultTry		= 0;

	m_bCanceled			= false;

	m_bStop				= FALSE;

	m_bQueued			= TRUE;

	TryCnt				= 0;

	m_nOldState			= 0;

	m_PayedBytes		= 0;
	m_StockBytes		= 0;

	m_nType				= GDT_UNKNOWN;

#ifdef USE_MERGETHREAD
	if(!bMergeThread) 
	{
		if(!InitMergeThread())
			TRACE(_T("Merge Thread 가동 실패\n"));
		else
			TRACE(_T("Merge Thread 시작!!!\n"));
	}
#endif

#ifdef _DEBUG
	m_starttime			= GetTickCount();
#endif
}

CGnuDownloadShell::~CGnuDownloadShell()
{
	if(((m_Active && !m_Dead) || m_Waiting || m_Retry || m_bCanceled))
	{
		// 소캣이 닫히면 BackupDownload를 안한다.
		if ( m_CompleteDownload == false ) BackupDownload();
	}

#ifdef USE_MERGETHREAD
	if(m_nType == GDT_FILE)
	{
		while(!csMergeLock.Lock(5000)) 
			break;

		std::list<MergeItem*>::iterator it;
		for(it = MergeList.begin(); it != MergeList.end(); ++it)
		{
			MergeItem* pItem = *it;
			if(pItem->pMaster == this)
				pItem->pMaster = NULL;
		}
		csMergeLock.Unlock();
	}
#endif

	// written by moonknit 2005-06-01
	// 사용자에의해 다운로드 중지된 경우 다운 받던 파일을 삭제한다.
	if(m_bCanceled)
	{
		m_File.Abort();
		DeleteFile(m_PartialPath);
	}
	// --


	// 현재 진행중인 JoinParts 작업이 삭제되는 Shell의 것이라면
	// 작업을 중지하도록 한다.
	if((LPVOID) this == pCurMergeShell) pStopMerge = true;


#ifdef USE_MERGETHREAD
	// added by moonknit
	csResultLock.Lock();
	while(m_ResultList.size())
	{
		MergeResult* pResult = m_ResultList.back();
		m_ResultList.pop_back();

		delete pResult;
	}
	csResultLock.Unlock();
	// added by moonknit
#endif

	m_ShellAccess.Lock();

	m_Active  = false;
	m_Dead    = true;
	m_Cooling = 0;
	
	while(m_Sockets.size())
	{
		CGnuDownload* pDead = m_Sockets.back();
		m_Sockets.pop_back();

		delete pDead;
	}

	while(m_ChunkList.size())
	{
		FileChunk* pDead = m_ChunkList.back();
		m_ChunkList.pop_back();

		pDead->File.Abort();

		// 사용자에의해 다운로드 중지된 경우 다운 받던 파일을 삭제한다.
		if(m_bCanceled)
			DeleteFile(pDead->Path);

		delete pDead;
	}


	// Close file
	m_File.Abort();

	m_ShellAccess.Unlock();

#ifdef MULTI_UPDATETFSTATUS
	m_listSRLink.clear();
#endif

}

void CGnuDownloadShell::AddHost(Result HostInfo)
{
	m_Waiting = false;
	m_Active = true;

	// Check for duplicate hosts
	for(int i = 0; i < m_Queue.size(); ++i)
	{
		if(HostInfo.Host.S_addr == m_Queue[i].Host.S_addr &&
		   HostInfo.Port	    == m_Queue[i].Port)
		{
			m_Queue[i].FileIndex = HostInfo.FileIndex;
			m_Queue[i].Name      = HostInfo.Name;
			m_Queue[i].NameLower = HostInfo.NameLower;
			m_Queue[i].UserID    = HostInfo.UserID;

			m_Queue[i].Origin	 = HostInfo.Origin;
			m_Queue[i].PushID    = HostInfo.PushID;
			m_Queue[i].RetryWait = 0;
			m_Queue[i].Alive     = false;
			m_Queue[i].FirstTry  = true;
			m_Queue[i].nConnectFailCount = 0;
			return;
		}
	}

	HostInfo.Handshake = "";
	HostInfo.Error     = "";
	HostInfo.RetryWait = 0;
	HostInfo.Alive     = false;
	HostInfo.FirstTry  = true;
	HostInfo.Tries     = 0;
	HostInfo.Corrupt   = false;
	HostInfo.RealBytesPerSec	= 0;

	// Add host to queue, positioned by speed
	bool Inserted = false;
	std::vector<Result>::iterator itHost;

	for(itHost = m_Queue.begin(); itHost != m_Queue.end(); ++itHost)
	{
		if(HostInfo.Speed >= (*itHost).Speed)
		{
			m_Queue.insert(itHost, HostInfo);
			Inserted = true;

			break;
		}
	}
	if(!Inserted)
		m_Queue.push_back(HostInfo);

	if ( m_Queue.size() == 1 ) m_UserID = HostInfo.UserID;
	else
	{
		// 멀티소스 다운로드를 사용하는 경우에만.."@@외 # 명" 표시
		if( theApp.m_spGD->m_Multisource )
		{
			m_UserID.Format(STR_MSG_GNUDWNSHL_002, HostInfo.UserID, m_Queue.size()-1 );
		}
	}

	m_UpdatedInSecond = true;
	m_BackupTrigger   = true;

	m_bQueued = FALSE;
}

void CGnuDownloadShell::TryNextHost()
{
	// 정지상태에서는 Host에 연결하지 않는다.
	if(m_bStop || m_CompleteDownload) return;

	if(m_bAllComplete) return;

	// Queue 대기 상태에는 hOST에 연결하지 않는다.
	if(m_bQueued) return;

	// 전송 슬롯이 부족하면 더이상 시도를 하지 않는다.
	if( !theApp.m_spGD->m_Control.GetDownloadingSlot() )
		return;

	// 전송중이거나 접속시도중이 아니고 현재 전송 개수가 제한 개수 이상이면 전송 시도를 하지 않는다.
	if(!IsDownloading() && !IsTrying())
	{
//		TRACE(_T("현재 다운로드 중인 개수 : %d\r\n"), theApp.m_spGD->m_Control.GetDownloadingCnt());
		if(theApp.m_spGD->m_Control.GetDownloadingCnt() >= theApp.m_spGD->GetMaxDownload())
			return;
	}
	else
	{
		// 포스트 다운로드시 전송중이거나 접속시도중이면 더 이상 시도하지 않는다.
		if(m_nType == GDT_POST) return;
	}

	bool bComplete = true;
	if(m_bComplete)
	{
		for(int j = 0; j < m_ChunkList.size(); ++j)
		{
			if(!m_ChunkList[j]->bComplete)
			{
				bComplete = false;
				break;
			}
		}
	}
	else bComplete = false;

	if(bComplete)
	{
		m_bAllComplete = true;
		return;
	}

	if(m_HostTryPos >= m_Queue.size())
		m_HostTryPos = 1;
	else
		++m_HostTryPos;

	// Rolls through host list looking for untried hosts
	int tried_hosts = 0; 

	int res = 0;

	// List already ordered to put fastest nodes at top
	for(int i = 0; i < m_Queue.size(); ++i)
	{
		if(!m_Queue[i].Corrupt && (m_Queue[i].Alive || m_Queue[i].FirstTry))
		{
			if(m_Queue[i].Trying || m_Queue[i].RetryWait > 0)
				continue;

			// Make sure Node is not active
			bool Active = false;
			for(int j = 0; j < m_Sockets.size(); ++j)
			{
				if(m_Sockets[j]->m_Params.Host.S_addr == m_Queue[i].Host.S_addr)
					Active = true;
			}

			if(Active)
				continue;

			if(m_Queue[i].nConnectFailCount >= MAX_FAILCOUNT)
			{
				m_Queue[i].Alive = false;
				// Push를 사용하지 않는다.
//				theApp.m_spGD->m_Control.Route_LocalPush(m_Queue[i]);
				continue;
			}

			m_Queue[i].Alive = true;

			m_Queue[i].Error = "";
			CGnuDownload* pSock = new CGnuDownload(this, m_Queue[i]);

			// modified by moonknit 2005-04-01
			res = pSock->StartDownload();

			if(!res)
			{
				pSock->m_bRetry = true;
				m_Sockets.push_back(pSock);
				m_Active = true;

				// written by moonknit 2005-06-30
				// 연결을 시도할 Queue의 호스트 정보가 이미 다른 파일을 받고 있다면
				// 대기 상태로 처리한다.
				// 파일 받기를 할 수 있는 상태라면 Active Host로 정보를 등록한다.
				ActiveHost host;
				host.HostIP		= m_Queue[i].Host;
				host.Port		= m_Queue[i].Port;
				host.FileName	= m_Queue[i].NameLower;

				theApp.m_spGD->m_Control.AddActiveHost(host);

				// eg. File has 90 results, try 3 at a time
				++tried_hosts;
				if (tried_hosts < (m_Queue.size() - 1) / 3) 
					continue; 
			}
			else
				delete pSock;


			if(res != -2)
			{
				m_Queue[i].FirstTry = false;
				m_Queue[i].Trying = true;

				IncreaseTryCnt();

			}
		}
	}

	if (tried_hosts) 
	{
		return;
	}


	// All hosts in list have been attempted
	// And nothing is trying to connect
	if(m_Sockets.size() == 0)
	{
		bool StillHope = false;

		for(i = 0; i < m_Queue.size(); ++i)
		{
			bool IsAlive = m_Queue[i].Alive;
			if(IsAlive)
			{
				if(!m_Cooling)
					m_Cooling = m_Queue[i].RetryWait;

				if(m_Cooling > m_Queue[i].RetryWait)
					m_Cooling = m_Queue[i].RetryWait;

				StillHope = true;
			}
		}

		m_Retry   = true;
		m_Waiting = StillHope ? false : true;
		if(m_Waiting)
		{
			if(m_RetryCount > 0 && m_nType != GDT_POST)
			{
				m_RetryCount --;
				// 자동 재검색
				StartResearch();
			}
			else
			{
				m_ReasonDead	  = "Waiting, more hosts needed";
				m_Retry			  = false;
				m_UpdatedInSecond = true;
				m_Cooling		  = 0;

				m_Active		  = false;

				if(m_Hwnd && pGroup)
					::PostMessage(m_Hwnd, SEARCH_RESULT_NOHOSTS, (WPARAM) pGroup, 0);
			}
		}
	}
}

void CGnuDownloadShell::ReleaseDownload()
{
	// 파일 받고 있던 작업을 중지한다.
	for(int i = 0; i < m_Sockets.size(); ++i)
	{
		m_Sockets[i]->StopDownload();
	}

	m_Cooling = 10;
}

// commented by moonknit 2005-05-17
// 다운로드 다시 시작, 정지상태였던 다운로드 쉘을 재 실행시키는 것이다.
// Transfer View에서 실행시킨다.
void CGnuDownloadShell::Start()
{
	m_HostTryPos = 1;
	m_bStop				= FALSE;
	
	if(GetBytesCompleted() < m_FileLength)
	{
		m_Active		= true;
		m_Dead			= false;
		m_Cooling		= 0;
		m_Searching		= 0;
		m_Waiting		= false;
		m_StartBytes	= GetBytesCompleted();
	}
	else
		CheckCompletion();
}

// Should only be called from interface
// 다운로드 정지
// Transfer View에서 실행시킨다.
void CGnuDownloadShell::Stop()
{
	m_bStop				= TRUE;
	m_HostTryPos = 1;

	for(int i = 0; i < m_Queue.size(); ++i)
	{
		m_Queue[i].RetryWait = 0;
		m_Queue[i].Tries     = 0;
	}

	// 파일 받고 있던 작업을 중지한다.
	for(i = 0; i < m_Sockets.size(); ++i)
	{
		m_Sockets[i]->StopDownload();
	}

	BackupDownload();

	m_Active    = false;
	m_Dead      = true;
	m_Cooling   = 0;
	m_Searching = 0;
	m_Waiting   = false;

	m_ReasonDead  = "Stopped";
}

// Set Research Flag on & Clear Previouly Researched Nodes List
// Transfer View에서 실행시킨다.
void CGnuDownloadShell::StartResearch(int nCnt)
{
	// 정지중에는 재검색을 할 수 없다.
	if(m_bStop) return;

	if(nCnt < 0) nCnt = 5;
	m_Searching = nCnt;
	m_DoReQuery = true;
	m_TotalSecCount = 0;
	m_ReSearchedNodes.clear();
}

bool CGnuDownloadShell::IsDownloading()
{
	for(int i = 0; i < m_Sockets.size(); ++i)
	{
		if(m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
			return true;
	}

	return false;
}

bool CGnuDownloadShell::IsTrying()
{
	return TryCnt > 0 ? true : false;
}

void CGnuDownloadShell::Timer()
{
	// Run timer for each socket
	for(int i = 0; i < m_Sockets.size(); ++i)
		m_Sockets[i]->Timer();

	// Remove dead sockets
	bool bCheckCompletion = false;
	std::vector<CGnuDownload*>::iterator itSock;
	for(itSock = m_Sockets.begin(); itSock != m_Sockets.end(); ++itSock)
	{
		CGnuDownload *pSock = *itSock;

		if(pSock 
			&& pSock->m_Status == TRANSFER_CLOSED
			)
		{
			m_StockBytes += pSock->GetDownloadSize();

			m_Sockets.erase(itSock);
			itSock --;

			delete pSock;

			pSock = NULL;

			bCheckCompletion = true;
		}
		else
		{
		}
	}

	if(bCheckCompletion)
		CheckCompletion();

	// 정지 상태에선 아무 작업도 수행하지 않는다.
	if(m_bStop) return;

	// 파일 전송이 끝났으므로 소켓 정리 이외의 작업은 필요없다.
	if(m_CompleteDownload) return;

	// 주기적으로 파일 결합의 완료를 확인한다.
	if(++m_nResultTry > 10)
	{
		TreatMergeResult(NULL);
		m_nResultTry = 0;
	}

	//전송 속도 계산
	m_AvgSpeed = 1;
	for(i = 0; i < m_Sockets.size(); ++i){
		if ( m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
		{
			m_AvgSpeed += m_Sockets[i]->m_Speed;
		}
	}


	/// Check for completion
	if(!m_Dead)
	{
		// CheckCompletion의 반복을 막기 위해서
		// 이전 Merged 작업이 수행 되었다면 잠시 쉰다.
		// Merge결과 처리후 CheckCompletion이 따로 수행된다.
		if(!m_bMerged)
		{
			if(m_bAllComplete
				|| (IsAttachable())
				|| (GetBytesCompleted() >= m_FileLength) )
				CheckCompletion();
		}

		for(i = 0; i < m_Queue.size(); ++i)
		{
			if(m_Queue[i].RetryWait > 0)
			{
				m_Queue[i].RetryWait--;
			}
			else if(m_Queue[i].RetryWait != 0)
			{
				m_Queue[i].RetryWait = 0;
			}
		}
	}

	// If download is being researched
	if(m_Searching)
	{
		m_Searching--;
	}

	// If it is waiting to retry
	else if(m_Cooling > 0)
	{
		m_Cooling--;

		if(m_Cooling < 0) m_Cooling = 0;

		if(m_Cooling == 0)
			m_Active = true;

		m_UpdatedInSecond = true;
	}
	
	// If this download is activated 
	else if(m_Active)
	{
		if(IsDownloading())
		{
			float NetUtilized = 0;

			if(theApp.m_spGD->m_RealSpeedDown)
				NetUtilized = ((float) theApp.m_spGD->m_Control.m_NetSecBytesDown + (float) theApp.m_spGD->m_Control.m_DownloadSecBytes) / (float) theApp.m_spGD->m_RealSpeedDown;
		}
		else
		{
		}

		TryNextHost();
	}

	else if(m_Waiting)
	{

	}

	// If there are still alive hosts there is hope
	else if(!m_Dead)
	{
		if(!theApp.m_spGD->m_MaxDownloads)
			Start();
		else if(theApp.m_spGD->m_Control.CountDownloading() < theApp.m_spGD->m_MaxDownloads)
			Start();
	}

	// Else this socket is dead
	else
	{
		for(i = 0; i < m_Sockets.size(); ++i)
			m_Sockets[i]->Close();
	}

	// Check if a re-query should be done
	if(m_Dead)
	{
		m_DoReQuery = false;
	}
	else
	{
		++m_TotalSecCount;

		if(m_TotalSecCount == 5  * 60 || // 5 minutes
		m_TotalSecCount == 20 * 60 || // 20 minutes
		(m_TotalSecCount >= 60 * 60 && m_TotalSecCount % (60 * 60) == 0)) // Each hour
		{
			m_DoReQuery = false;
			m_ReSearchedNodes.clear();
		}
	}

	// If there are still nodes to requery on, do it
	if(m_nType == GDT_FILE && m_DoReQuery)
	{
#ifndef AUTO_REQUERY
		// ReQuery를 한번 시도하여 실패하면 다시 시도하지 않는다.
		m_DoReQuery = false;
#endif
		for(i = 0; i < theApp.m_spGD->m_Control.m_NodeList.size(); ++i)	
		{
			CGnuNode* p = theApp.m_spGD->m_Control.m_NodeList[i];

			// if auto requery is enabled
			// this will do work
#ifdef AUTO_REQUERY
			BOOL bThrough = FALSE;

			// Check if we've already researched this node
			for(int j = 0; j < m_ReSearchedNodes.size(); ++j)
			{
				if(m_ReSearchedNodes[j] == p)
				{
					bThrough = TRUE;
					break;
				}
			}

			if(bThrough)
				continue;
#endif

			// Attempt to ReSearch node
			if(p->m_Status == SOCK_CONNECTED)
			{
				if(p->m_NextReSearchWait == 0)
				{
					if(!p->IsRecentQuery(m_SearchGuid))
					{
						SendQuery();
						p->m_NextReSearchWait = 30;
						p->m_RecentQueryList.push_back(RecentQuery(m_SearchGuid) );
					}	

					m_ReSearchedNodes.push_back(p);
						
					break;
				}
				// if auto requery is enabled
				// this will do work
#ifdef AUTO_REQUERY
				else
					m_DoReQuery = false;
#endif
			}
		}
	}

	if(m_nType == GDT_FILE)
	{
		// 일정 주기마다 백업을 시도한다. 
		if(m_nBackupCount > 0)
			m_nBackupCount--;
		
		// Check if we need to backup download
		// theApp.m_spGD->m_Control.m_BackupTicket을 사용하면 m_BackupTicket이 설정되는 주기마다
		// 여러 다운로드 중에서 하나만 백업파일을 생성하게 되므로 수정함.
		if(m_BackupTrigger && m_nBackupCount <= 0)
		{
			m_bMerged = false;
			m_nBackupCount = 5;

			// 소캣이 닫히면 BackupDownload를 안한다.
			if ( m_CompleteDownload == false ) BackupDownload();

			m_BackupTrigger = false;
			theApp.m_spGD->m_Control.m_BackupTicket = true;
		}
	}

	theApp.m_spGD->m_Control.TransferMessage(DOWNLOAD_UPDATE, (WPARAM) this);

	// PIPE : Tranfer File 9
	if ( (m_FileLength == m_BytesCompleted) && m_Dead && m_CompleteDownload)
	{
		// 완료 될 경우 메세지를 보낸다.
		HWND	hWnd = NULL;
		if ( hWnd  ) 
			::SendMessage(hWnd, COMPLETE_DOWNLOAD,(unsigned int)this, NULL);
	}
}

CFileLock* CGnuDownloadShell::ReadyFile(CGnuDownload* pCaller)
{
	// 파일 전송시에만 유효하다.
	if(m_nType != GDT_FILE) return NULL;

	for(int i = 0; i < m_Sockets.size(); ++i)
		if(m_Sockets[i]->m_pChunk == NULL && m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
			return NULL;

	// Close file first
	m_File.Abort();

	CFileException FileError;

	// Get plain file name with out directory crap
	CString FileName = m_Name;
	FileName.Replace(_T("\\"), _T("/"));
	FileName = FileName.Mid( FileName.ReverseFind('/') + 1);

	// Set where the file will go upon completion
	CreateDirectory(theApp.m_spLP->DownloadPath, NULL);

	m_FilePath = theApp.m_spLP->DownloadPath + _T("\\") + FileName;
	m_FilePath.Replace(_T("\\\\"), _T("\\"));


	// Create / Find the file in the partial directory
	CString PartialDir = theApp.m_spLP->GetPartialDir();

	CreateDirectory(PartialDir, NULL);


	// Add tail to it so we can support partial downloads
	CString OldPath, Tail;

	// Add hash also only if there is one
	if (!m_Sha1Hash.IsEmpty())
	{
		CString OldTail = _T(" (") + DWrdtoStr(m_FileLength) + _T(")");
		CString OldName = FileName;

		int endPos = OldName.ReverseFind('.');
		if( endPos != -1)
			OldName.Insert(endPos, OldTail);
		else
			OldName += OldTail;

		OldPath = PartialDir + _T("\\") + OldName;
		OldPath.Replace(_T("\\\\"), _T("\\"));

		Tail = _T(" (") + DWrdtoStr(m_FileLength) + _T(",") + m_Sha1Hash + _T(")");
	}
	else
	{
		Tail = _T(" (") + DWrdtoStr(m_FileLength) + _T(")");
	}


	// Add tail to partial file
	int endPos = FileName.ReverseFind('.');
	if( endPos != -1)
		FileName.Insert(endPos, Tail);
	else
		FileName += Tail;

	// modified by moonknit 2005-08-24
#ifdef PREFIX_FILENAME
	m_PartialPath = PartialDir + _T("\\FISH_") + FileName;
#else
	m_PartialPath = PartialDir + _T("\\") + FileName;
#endif
	// --

	m_PartialPath.Replace(_T("\\\\"), _T("\\"));
	

	// Make sure if a hash was found for this download, the name of the partial is changed
	if (!m_Sha1Hash.IsEmpty())
	{
		try
		{
			CFileLock::Rename(OldPath, m_PartialPath);
		}
		catch(CFileException* pEx )
		{
			pEx->Delete();
		}
	}

	// Resuming a file
	if(m_BytesCompleted > 4096)
	{
		if(!m_File.Open(m_PartialPath, CFileLock::modeCreate | CFileLock::modeNoTruncate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
		{
			if(pCaller)
				pCaller->SetError(GetFileError(&FileError));
			return NULL;
		}

		DWORD EOFpos = 0;
		try
		{
			EOFpos = m_File.SeekToEnd();

			if(EOFpos > 4096)
			{
				m_BytesCompleted = EOFpos;

				if(pCaller)
				{
					pCaller->m_ResumeStart = m_BytesCompleted - 4096;
				}

			}
			else
			{
				m_BytesCompleted = 0;
				m_File.Abort();

				if(!m_File.Open(m_PartialPath, CFileLock::modeCreate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
				{
					if(pCaller)
						pCaller->SetError(GetFileError(&FileError));
					return NULL;
				}
			}
		}
		catch(CFileException* e)
		{
			if(pCaller)
				pCaller->SetError(GetFileError(e));
			e->Delete();
			return NULL;
		}
	}

	// Starting a new file
	else if(!m_File.Open(m_PartialPath, CFileLock::modeCreate | CFileLock::modeReadWrite | CFileLock::shareDenyWrite, &FileError))
	{	
		if(pCaller)
			pCaller->SetError(GetFileError(&FileError));
		return NULL;
	}

	m_BackupTrigger = true;

	return &m_File;
}

DWORD CGnuDownloadShell::GetStatus()
{
	if(m_bStop) return TRANSFER_STOP;

	if(m_bQueued) return TRANSFER_QUEUED;

	// Cooling down
	if(!m_Active && m_Cooling)
	{
		return TRANSFER_COOLDOWN;		// 접속 대기중
	}

	// Pending
	// 한 사람에게서 여러개의 파일을 다운로드 받을 경우에는 타임 아웃이 발생을 한다.
	else if((!m_Active && !m_Dead) || m_Waiting)
	{
		return TRANSFER_NEEDSOURCE;			// 소스 부족
	}

	// Connecting / Connected / Transfering / QUEUED
	else if(m_Active)
	{
		if(IsDownloading())
			return TRANSFER_RECEIVING;
		else
		{
			// 다운로드가 완료된 뒤에 파일 연결중이다.
			if(m_bAllComplete) return JOIN_CHUNK;

			if(TryCnt > 0)
				return TRANSFER_CONNECTING;

			return TRANSFER_COOLDOWN;
		}
	}

	// Completed or Failed
	else if(!m_Active && m_Dead)
		return TRANSFER_CLOSED;

	if ( m_JoinChunk == 1 ) return JOIN_CHUNK;

	return TRANSFER_CLOSED;
}

// Query Hit을 받아서 현재 다운로드 객체의 내용과 비교하여 처리한다.
void CGnuDownloadShell::IncomingQueryHit(packet_Log* packet)
{
	// deprecated by moonknit 2006-01-19
}

void CGnuDownloadShell::SendQuery()
{
	CString m_TempSearch = _T("");

	// 여기서 아이디 값을 설정한다.

	CString FileName = m_Name;
	m_TempSearch += FileName;
	m_TempSearch += _T('');
	m_TempSearch += theApp.m_spLP->GetUserID();
	m_TempSearch += _T('');

	bool bRFID = false;
	if(!m_Sha1Hash.IsEmpty())
		bRFID = true;

	m_TempSearch += _T("|");
	if(bRFID)
		m_TempSearch += DWrdtoStr(2);					// 검색 종류 rfid 검색
	else
		m_TempSearch += DWrdtoStr(1);

	m_TempSearch += _T('');
	m_TempSearch += DWrdtoStr(0);					// 검색 대상 All

	// rfid 검색시 rfid는 검색어와 따로 전송되도록 한다.
	if(bRFID)
	{
		m_TempSearch += _T('');
		m_TempSearch += m_Sha1Hash;
	}

	/////////////////////////////////////////////////////////////////////////

	memset(m_Packet , 0 , 255);
	memcpy(m_Packet + 25, m_TempSearch , m_TempSearch.GetLength());
	memcpy(m_Packet + 25 + m_TempSearch.GetLength(), _T("\0"), 1);

	m_SearchGuid  = theApp.m_spGD->m_Control.Broadcast_LocalQuery( m_Packet , 25 + m_TempSearch.GetLength() * sizeof(TCHAR) + 1);
}

void CGnuDownloadShell::IncomingNode(CGnuNode* newNode)
{
	if(!m_Dead)
		m_DoReQuery = false;
}

INT64 CGnuDownloadShell::GetBytesCompleted()
{
	// 파일 전송에 대해서만 유효한 함수이다.
	if(m_nType != GDT_FILE) return 0;

	INT64 TotalCompleted   = 0; 
	INT64 PartialCompleted = m_BytesCompleted;
	
	if(m_StopPos)
		if(PartialCompleted > m_StopPos)
			PartialCompleted = m_StopPos;

	if(m_OverlapBytes && PartialCompleted > 4096)
		TotalCompleted = PartialCompleted - 4096;
	else
		TotalCompleted = PartialCompleted;


	for(int i = 0; i < m_ChunkList.size(); ++i)
	{
		INT64 ChunkCompleted = 0;
		INT64 ChunkLength = m_ChunkList[i]->BytesCompleted;
		
		if(ChunkLength > m_ChunkList[i]->ChunkLength)
			ChunkLength = m_ChunkList[i]->ChunkLength;
			

		if(m_ChunkList[i]->OverlapBytes && ChunkLength > 4096)
			ChunkCompleted = ChunkLength - 4096;
		else
			ChunkCompleted = ChunkLength;
		
		TotalCompleted += ChunkCompleted;
	}

	return TotalCompleted;
}

// 현재 Downloadshell에서 실제로 받은 파일의 크기 구하기..
INT64 CGnuDownloadShell::GetReceiveBytes()
{
	INT64 size = 0;
	if( m_Sockets.size() > 0 )
	{
		INT64 downloadSize;
		for( int i=0; i<m_Sockets.size(); ++i )
		{
			downloadSize = m_Sockets[i]->GetDownloadSize();
			size += downloadSize;
		}
	}

	size += m_StockBytes;

	if( size > m_FileLength )
		size = m_FileLength;

	if( size > 0 )
		return size;
	else
		return 0;
}//end of GetReceiveBytes()

void CGnuDownloadShell::AddChunkHost(std::vector<IP> &HostList, IP NewHost)
{
	// Adds host to back of list, any duplicates deleted from list
	std::vector<IP>::iterator itHost;

	for(itHost = HostList.begin(); itHost != HostList.end(); ++itHost)
	{
		if( (*itHost).S_addr == NewHost.S_addr)
		{
			HostList.erase(itHost);
			itHost --;
		}
		else
		{
		}
	}

	HostList.push_back(NewHost);
}

bool CGnuDownloadShell::PostCompletion()
{
	if(m_nType != GDT_POST) return FALSE;

	if(m_bComplete) return TRUE;

	CEzXMLParser parser;
	parser.LoadXML((LPTSTR) (LPCTSTR) m_PostXML, CEzXMLParser::LOAD_STRING);

	auto_ptr<POSTITEM> apitem(new POSTITEM);

	IXMLDOMNodeListPtr listptr;
	listptr = parser.SearchNodes(_T("item"));

	if(listptr == NULL || listptr->length == 0)
	{
		return FALSE;
	}

	IXMLDOMNodePtr nodeptr;
	nodeptr = listptr->item[0];

	apitem->subject = GetNamedNodeText(XML_SUBJECT, &parser, nodeptr);
	ConvertStringtoTime(GetNamedNodeText(XML_PUBDATE, &parser, nodeptr), apitem->pubdate);
	apitem->url = GetNamedNodeText(XML_URL, &parser, nodeptr);
	apitem->author = GetNamedNodeText(XML_AUTHOR, &parser, nodeptr);
	apitem->category = GetNamedNodeText(XML_CATEGORY, &parser, nodeptr);
	apitem->description = GetNamedNodeText(XML_DESC, &parser, nodeptr);
	apitem->enclosure = GetNamedNodeText(XML_ENCLOSURE, &parser, nodeptr);

	if(!theApp.IsClosing())	theApp.m_spSM->PostComplete(m_SearchGuid, m_Sha1Hash, apitem);

	m_bComplete = true;

#ifdef _DEBUG
	m_executetime = GetTickCount() - m_starttime;
	TRACE(_T("download execution time : %u\r\n"), m_executetime);
#endif

	return TRUE;
}

bool CGnuDownloadShell::CheckCompletion()
{
	// 파일 전송인 경우에만 유효하다.
	if(m_nType != GDT_FILE) return FALSE;

	if( TryAttachChunk2Partial() )
		return true;
	
	if(m_FileLength == m_BytesCompleted && m_Active)
	{
		// Remove dead sockets
		while(m_Sockets.size())
		{
			CGnuDownload* pDead = m_Sockets.back();
			m_Sockets.pop_back();

			delete pDead;
			pDead = NULL;
		}

		m_Active    = false;
		m_Dead      = true;
		m_Cooling   = 0;
		m_Searching = 0;
		m_Waiting   = false;

		// Close File
		m_File.Abort();

		// 전송이 끝나면 백업 파일을 지웁니다.
		DeleteBackup();
	
		// Try to move or copy file
		int nTryCnt = 0;
		bool bNameChaned = false;
		CString FileName;
		CString NewName;
		while(!MoveFile(m_PartialPath, m_FilePath))
		{
			if(GetLastError() == ERROR_ALREADY_EXISTS)
			{

				FileName = m_Name;
				FileName.Replace(_T("\\"), _T("/"));
				FileName = FileName.Mid( FileName.ReverseFind(_T('/') + 1));


				NewName = GetNewFileName(FileName, ++nTryCnt);

				bNameChaned = true;

				m_FilePath = theApp.m_spLP->DownloadPath + _T("\\") + NewName;
			}
			else
			{
				CopyFile(m_PartialPath, m_FilePath, false);
				break;
			}
		}

		if(bNameChaned)
			m_Name.Replace(FileName, NewName);

		if(m_Hwnd && pGroup)
			::PostMessage(m_Hwnd, SEARCH_RESULT_COMPLETED, (WPARAM) pGroup, 0);
		
		m_CompleteDownload = true;
		
		theApp.m_spGD->m_Control.TransferMessage(DOWNLOAD_UPDATE, (WPARAM) this);
	}

	return false;	
}

void CGnuDownloadShell::TrashFamily(int Family)
{
	// 파일 전송에 대해서만 유효하다.
	if(m_nType != GDT_FILE) return;
	
	m_bAllComplete = false;

	int i, j;

	TRACE(m_Name + _T(": Trashing family ") + DWrdtoStr(Family) + _T("\n"));
	
	if(Family == 0)
	{
		// Close active transfers
		for(i = 0; i < m_Sockets.size(); ++i)
			if(m_Sockets[i]->m_pChunk == NULL)
				m_Sockets[i]->Close();

		if(m_File.m_hFile != CFileLock::hFileNull)
		{
			try
			{
				m_File.SetLength(0);
			}
			catch(...)
			{
				
			}				
		}

		m_File.Abort();
		
		while(m_PartialHosts.size())
		{
			SetCorrupt(m_PartialHosts.back(), true);
			m_PartialHosts.pop_back();
		}

		m_BytesCompleted = 0;

		// added by moonknit
		m_bComplete = false;
	}


	for(i = 0; i < m_ChunkList.size(); ++i)
		if(m_ChunkList[i]->HostFamily == Family)
		{
			// Close any active sockets by chunk
			for(j = 0; j < m_Sockets.size(); ++j)
				if(m_Sockets[j]->m_pChunk == m_ChunkList[i])
					m_Sockets[j]->Close();

			
			m_ChunkList[i]->File.Abort();
				
			
			DeleteFile(m_ChunkList[i]->Path);
			
			while(m_ChunkList[i]->Hosts.size())
			{
				SetCorrupt(m_ChunkList[i]->Hosts.back(), true);
				m_ChunkList[i]->Hosts.pop_back();
			}

			m_ChunkList[i]->BytesCompleted = 0;

			// added by moonknit
			m_ChunkList[i]->bComplete = false;
		}
	
}

// Copy Buffer Size
// 4 K bytes -> 10 k Bytes
#define CBS					10240

bool CGnuDownloadShell::JoinParts(CFileLock &File1, CFileLock &File2, INT64 JoinPos, INT64 JoinBytes,bool m_partial )
{
	// 상태 변경 알림은 Timer 작업과 Complete 작업에서 처리된다.

	m_JoinChunk = 1;
	double progress;
	INT64 ChunkPos;

	byte Part1Buff[CBS]; 
	byte Part2Buff[CBS]; 
	
	try
	{
		File1.Seek(JoinPos, CFileLock::begin);
		File1.Read(Part1Buff, 4096);

		File2.SeekToBegin();
		File2.Read(Part2Buff, 4096);
	}
	catch(...)
	{
		TRACE(_T("*** Error reading files while joining\n"));
		return false;
	}

	// Verify both are part of the same file
	if( memcmp(Part1Buff, Part2Buff, 4096) == 0 )
	{
		File1.Seek(JoinPos + 4096, CFileLock::begin);
		File2.Seek(4096, CFileLock::begin);


		// Attach chunk to partial
		INT64 BytesToRead = JoinBytes - 4096;
		ChunkPos = 0;		
		while(BytesToRead)
		{
			// 현재 결합중인 파일의 DownloadShell은 취소 명령에 의해 삭제 되었으므로
			// 결합중인 파일 또한 삭제되어 결합작업을 수행할 수 없다.
			if(pStopMerge) break;

			int ReadSize = (BytesToRead > CBS) ? CBS : BytesToRead;

			int BytesRead = File2.Read(Part2Buff, ReadSize);
			
			ChunkPos += BytesRead; 

			if(BytesRead == ReadSize)
			{
				File1.Write(Part2Buff, BytesRead);
				
				BytesToRead -= BytesRead;

				if ( m_partial == true ) { 
					TotalPos += BytesRead;	
				}

				// 조각 모음 다이얼 로그 대신 상태바에 메세지를 보여준다.
				if ( m_partial == true ) 
				{
					CString	notiMsg;
					INT64 size =(double) File1.GetLength();
					INT64 tsize =(double) GetBytesCompleted();
					if ( GetBytesCompleted() != 0 ) 
						progress = (size*100) / tsize;
				}

			}
			else
			{
				TRACE(_T("** Failed reading file while copying"));
				break;
			}
		}
		return true;
	}
	return false;
}


// 
INT64 CGnuDownloadShell::ChunkFamilySize(int Family)
{
	INT64 Size = 0;
	
	if (Family == 0)
		Size += m_BytesCompleted; 

	for(int i = 0; i < m_ChunkList.size(); ++i)
		if(m_ChunkList[i]->HostFamily == Family)
			Size += m_ChunkList[i]->BytesCompleted;

	return Size;
}


void CGnuDownloadShell::SetCorrupt(IP Host, bool value)
{
	for(int i = 0; i < m_Queue.size(); ++i)
		if(m_Queue[i].Host.S_addr == Host.S_addr)
		{
			m_Queue[i].Corrupt = value;
		}
}
			
void CGnuDownloadShell::BackupDownload(CString SavePath)
{
	// 파일 정송시에만 유효하다.
	if(m_nType != GDT_FILE) return;

	if(SavePath == _T(""))
	{
		CString PartialDir = theApp.m_spLP->GetPartialDir(); 

		CreateDirectory(PartialDir, NULL);

		CString BackupPath = PartialDir + _T("\\") + m_Name + _T(".ent");
		BackupPath.Replace(_T("\\\\"), _T("\\"));

		SavePath = BackupPath;
	}

	if( m_bCanceled ) 
	{
		DeleteFile(SavePath);
		return;
	}

	CStdioFile BackupFile;
	if( BackupFile.Open(SavePath, CFileLock::modeCreate | CFileLock::modeWrite) )
	{	
		CString Backup = _T("");

		// Save main download info
		Backup += _T("[Download]\n");
		Backup += _T("Active=")			+ DWrdtoStr(m_Active)					+ _T("\n");
		Backup += _T("Dead=")			+ DWrdtoStr(m_Dead)						+ _T("\n");
		Backup += _T("Name=")			+ m_Name								+ _T("\n");
		Backup += _T("Completed=")		+ DWrdtoStr(m_BytesCompleted)			+ _T("\n");
#ifdef MULTI_UPDATETFSTATUS
		Backup += _T("SRLintCnt=")		+ DWrdtoStr(m_listSRLink.size())		+ _T("\n");
		std::list<SRLINK_ITEM>::iterator it;
		int n = 1;
		for(it = m_listSRLink.begin(); it != m_listSRLink.end(); ++it)
		{
			Backup += _T("RSID[")		+ DWrdtoStr(n)							+ _T("]=");
			Backup += (*it).sSRID												+ _T("\n");
			Backup += _T("FileSerial[") + DWrdtoStr(n)							+ _T("]=");
			Backup += DWrdtoStr((*it).nFSerial)									+ _T("\n");

			++n;
		}
#else
		// written by moonknit 2005-08-25
		Backup += _T("RSID=")			+ m_sSRID									+ _T("\n");
		Backup += _T("FileSerial=")		+ DWrdtoStr(m_nFSerial)						+ _T("\n");
		// --
#endif
		Backup += _T("FileLength=")		+ DWrdtoStr(m_FileLength)					+ _T("\n");
		Backup += _T("FilePath=")		+ m_FilePath								+ _T("\n");
		Backup += _T("PartialPath=")	+ m_PartialPath								+ _T("\n");
		Backup += _T("Overlap=")		+ DWrdtoStr(m_OverlapBytes)					+ _T("\n");
		Backup += _T("StopPos=")		+ DWrdtoStr(m_StopPos)						+ _T("\n");
		Backup += _T("Sha1Hash=")		+ m_Sha1Hash								+ _T("\n");
		Backup += _T("BitprintHash=")	+ m_BitprintHash							+ _T("\n");
		Backup += _T("Search=")			+ m_Search									+ _T("\n");
		Backup += _T("SearchGuid=")		+ GuidtoStr(m_SearchGuid)					+ _T("\n");
		Backup += _T("FamilyAge=")		+ DWrdtoStr(HostFamilyAge)					+ _T("\n");
		Backup += _T("AvgSpeed=")		+ DWrdtoStr(m_AvgSpeed)						+ _T("\n");

		// Save chunks
		for(int i = 0; i < m_ChunkList.size(); ++i)
		{
			Backup += _T("[Chunk")			+ DWrdtoStr(i) + _T("]\n");

			Backup += _T("Name=")			+ m_ChunkList[i]->Name						+ _T("\n");
			Backup += _T("Path=")			+ m_ChunkList[i]->Path						+ _T("\n");
			Backup += _T("Sha1Hash=")		+ m_ChunkList[i]->Sha1Hash					+ _T("\n");
			Backup += _T("StartPos=")		+ DWrdtoStr(m_ChunkList[i]->StartPos)		+ _T("\n");
			Backup += _T("FileLength=")		+ DWrdtoStr(m_ChunkList[i]->ChunkLength)	+ _T("\n");
			Backup += _T("BytesCompleted=")	+ DWrdtoStr(m_ChunkList[i]->BytesCompleted) + _T("\n");
			Backup += _T("OverlapBytes=")	+ DWrdtoStr(m_ChunkList[i]->OverlapBytes)	+ _T("\n");
			Backup += _T("ChunkColor=")		+ DWrdtoStr(m_ChunkList[i]->ChunkColor)		+ _T("\n");
			Backup += _T("HostFamily=")		+ DWrdtoStr(m_ChunkList[i]->HostFamily)		+ _T("\n");
		}

		// Save alternate hosts
		for(i = 0; i < m_Queue.size(); ++i)
		{
			Backup += _T("[Host")			+ DWrdtoStr(i)								+ _T("]\n");

			Backup += _T("Name=")			+ m_Queue[i].Name							+ _T("\n");
			Backup += _T("Sha1Hash=")		+ m_Queue[i].Sha1Hash						+ _T("\n");
			Backup += _T("BitprintHash=")	+ m_Queue[i].BitprintHash					+ _T("\n");
			Backup += _T("FileIndex=")		+ DWrdtoStr(m_Queue[i].FileIndex)			+ _T("\n");
			Backup += _T("Size=")			+ DWrdtoStr(m_Queue[i].Size)				+ _T("\n");
			
			Backup += _T("Host=");
			Backup += IPtoStr(m_Queue[i].Host);
			Backup += _T("\n");
			Backup += _T("Port=")			+ DWrdtoStr(m_Queue[i].Port)				+ _T("\n");
			Backup += _T("Speed=")			+ DWrdtoStr(m_Queue[i].Speed)				+ _T("\n");
			Backup += _T("UserID=")			+ m_Queue[i].UserID							+ _T("\n");
			
			Backup += _T("Firewall=")		+ DWrdtoStr(m_Queue[i].Firewall)			+ _T("\n");
			Backup += _T("OpenSlots=")		+ DWrdtoStr(m_Queue[i].OpenSlots)			+ _T("\n");
			Backup += _T("Busy=")			+ DWrdtoStr(m_Queue[i].Busy)				+ _T("\n");
			Backup += _T("Stable=")			+ DWrdtoStr(m_Queue[i].Stable)				+ _T("\n");
			Backup += _T("ActualSpeed=")	+ DWrdtoStr(m_Queue[i].ActualSpeed)			+ _T("\n");
			Backup += _T("PushID=")			+ GuidtoStr(m_Queue[i].PushID)				+ _T("\n");
			Backup += _T("Version=")		+ DWrdtoStr(m_Queue[i].wVersion)			+ _T("\n");
		}

		BackupFile.Write(Backup, Backup.GetLength());

		BackupFile.Abort();
	}
}

void CGnuDownloadShell::DeleteBackup()
{
	CString PartialDir = theApp.m_spLP->GetPartialDir();

	CreateDirectory(PartialDir, NULL);

	CString BackupPath=_T("");
	BackupPath = PartialDir + _T("\\") + m_Name + _T(".ent");
	BackupPath.Replace(_T("\\\\"), _T("\\"));

	try
	{
		CFileLock::Remove(BackupPath);
		m_bCanceled = true;
	}
	catch(...)
	{

	}
}

//Makslane:  returns the requested data and it restores the current file pointer 
int CGnuDownloadShell::GetPartialData(INT64 pos, unsigned char *buf, int len)
{
	if(m_File.m_hFile == CFileLock::hFileNull)
		return 0;

	int read = 0;
	CAutoLock lock(&m_File.m_CriticalSection);
	
	try
	{		
		//Do not use auto locked functions here
		if(pos >= ((CFile) m_File).GetLength())
			return 0;
		
		DWORD dwActualPosition = ((CFile) m_File).GetPosition();
		
		((CFile) m_File).Seek(pos, CFileLock::begin);

		read = ((CFile) m_File).Read(buf, len);

		((CFile) m_File).Seek(dwActualPosition, CFileLock::begin);		
	}
	catch(CFileException* e)
	{
		e->Delete();
		return 0;
	}

	return read;
}

void CGnuDownloadShell::AddAltLocation(AltLocation AltLoc)
{
	bool found = false;

	for(int i = 0; i < m_AltHosts.size(); ++i)
		if(AltLoc == m_AltHosts[i])
		{
			found = true;
			m_AltHosts[i].HostPort.Time = CTime::GetCurrentTime() - LocalTimeZone();
			break;
		}

	if(!found)
		m_AltHosts.push_back(AltLoc);
}

void CGnuDownloadShell::AddAltLocation(CString locStr)
{
	// Need to tidy up the string first
	
	locStr.Replace(_T("\r\n")	, _T(" ")); // Change EOL to space
	locStr.Replace(_T("\t")		, _T(" "));	 // Change TAB to space
	locStr.Replace(_T("  ")		, _T(" "));	 // Change Double space to single space

	CString temp = ParseString(locStr);
	
	while (!temp.IsEmpty())
	{
		// we have something so try to convert to an AltLocation
		AltLocation AltLoc = temp;
		AltLoc.Sha1Hash    = m_Sha1Hash;

		if (AltLoc.isValid())
		{
			bool LocFound   = false;
		
			for(int i = 0; i < m_AltHosts.size(); ++i)
				if (AltLoc == m_AltHosts[i])
					{
						LocFound = true;
						break;
					}
				
			if (!LocFound)
				m_AltHosts.push_back(AltLoc);


			while(m_AltHosts.size() > 25)
				m_AltHosts.pop_front();


			Result Info = Result(AltLoc);
			Info.Size   = m_FileLength;
			AddHost(Info);
		}

		temp = ParseString(locStr);
	}
}


INT64 CGnuDownloadShell::GetTotalFileSize()
{
	if(m_nType != GDT_POST) return 0;

	INT64 total = 0;
	for ( int i = 0 ; i < m_ChunkList.size() ; ++i)
		total += m_ChunkList[i]->BytesCompleted;
	return total;
}

//============================================================================
// NAME : GetReceivingCount()
// PARM : void
// RETN : DWORD
// DESC : 현재 다운로드 중인 소켓의 개수..
// Date : 2004-07-06 coded by bemlove, origin
//============================================================================
DWORD CGnuDownloadShell::GetReceivingCount()
{
	DWORD count=0;
	for( int i=0; i<m_Sockets.size(); ++i )
	{
		if( m_Sockets[i]->m_Status == TRANSFER_RECEIVING )
			++count;
	}
	return count;
}//end of GetReceivingCount()

// written by		: moonknit
// name				: CGnuDownloadShell::GetTotalSourceCount
// created			: 2005-06-02
// last updated		: 2005-06-16
// desc				:
// 현재 Queue에 등록된 노드의 개수를 반환한다.
// Corrupt Host Information will not be counted
DWORD CGnuDownloadShell::GetTotalSourceCount()
{
	DWORD count=0;
	for( int i=0; i<m_Queue.size(); ++i )
	{
		if( !m_Queue[i].Corrupt
			&& 
			(m_Queue[i].Alive			// 연결 시도 & 다운로드 중
			|| m_Queue[i].FirstTry		// 한번도 연결 시도하지 않음
			))
			++count;
	}

	return count;
}


//============================================================================
// NAME : SetTheTransferUserID()
// PARM : void
// RETN : bool
// DESC : 멀티소스 다운로드를 이용하지 않는 경우.. 현재 실제 전송중인 사용자 아이디를 찾아 세팅하여 준다.
// Date : 2004-07-06 coded by bemlove, origin
//============================================================================
bool CGnuDownloadShell::SetTheTransferUserID()
{
	CString strUID = _T("");
	for( int i=0; i<m_Sockets.size(); ++i )
	{
		if( m_Sockets[i]->m_Status == TRANSFER_RECEIVING )
		{
			m_UserID = m_Sockets[i]->UserID;
		}
	}
	return false;
}//end of SetTheTranserUserID()

// added by moonknit
/*********************************************************************************
created by moonknit
\*********************************************************************************/
UINT CGnuDownloadShell::MergeChunkProc(LPVOID lpParam)
{
	TRACE(_T("결합 스레드 시작\n"));
	SetMergeThread(true);
	
	FileChunk	*c1, *c2;
	MergeResult*	pResult;
	CGnuDownloadShell* pShell;
	unsigned int c1filehandle;
	bool bPartial;

	while(!IsStopThread())
	{
		TRACE(_T("Wait to be evented\n"));
		evMerge.Lock();
		evMerge.ResetEvent();
		evMerge.Unlock();
		TRACE(_T("Start to merge chunk\n"));
		while(true)
		{
			if(IsStopThread())
				break;

			if(MergeList.size() == 0) 
				break;

			// Lock을 얻지 못하면 이 결합작업을 종료를 시킨다.
			if(!csMergeLock.Lock(1000)) 
				break;

			MergeItem* pItem = MergeList.front();
			MergeList.pop_front();
			csMergeLock.Unlock();

			pShell = (CGnuDownloadShell*) pItem->pMaster;

			if(pShell == NULL)
				continue;

			c1 = (FileChunk*) pItem->pPreChunk;
			if(c1 == NULL) // if partial
			{
				c1filehandle = pShell->m_File.m_hFile;
				bPartial = true;
			}
			else
				bPartial = false;

			c2 = (FileChunk*) pItem->pPostChunk;

			if(c2->StartPos == 0
				|| c2->BytesCompleted == 0 // chunk reset
				|| c1filehandle == CFileLock::hFileNull 
				|| c2->File.m_hFile == CFileLock::hFileNull)
			{
				delete pItem;
				pItem = NULL;
				continue;
			}

			pResult = new MergeResult;
			pResult->pPreChunk = pItem->pPreChunk;
			pResult->pPostChunk = pItem->pPostChunk;
			pResult->bC2P = bPartial;

			pCurMergeShell = pItem->pMaster;

			if(bPartial)
				pResult->bSuccess = pShell->JoinParts(pShell->m_File, c2->File, c2->StartPos, c2->BytesCompleted, true);
			else
				pResult->bSuccess = pShell->JoinParts(c1->File, c2->File, c2->StartPos, c2->BytesCompleted, false);

			// Merge 작업은 정상적으로 수행되도록 설정한다.
			pCurMergeShell = NULL;
			pStopMerge = false;

			TRACE(_T("Join Parts [%s] %s\n")
				, bPartial 	? _T("파샬") : _T("청크")
				, pResult->bSuccess ? _T("성공") : _T("실패")
				);

			TRACE(_T("결합결과 list로 처리 시도\n"));
			// 락을 얻지 못하면 이 결합 작업을 다음으로 넘긴다.
			// Deadlock 발생이므로... 이 파일 다운로드는 불가능하다.
			if(!pShell->csResultLock.Lock(5000))
			{
				TRACE(_T("Result Lock 실패\n"));

				CWnd* pWnd = AfxGetMainWnd();
				if(pWnd 
					&& pWnd->PostMessage(WM_MERGE_COMPLETE, (WPARAM) pShell, (LPARAM) pResult))
					TRACE(_T("결합결과 직접 전송 처리됨\n"));
				else
					TRACE(_T("결합결과 전송 실패!!\n"));

				break;
			}

			pShell->m_ResultList.push_back(pResult);
			pShell->csResultLock.Unlock();

			CWnd* pWnd = AfxGetMainWnd();
			if(pWnd) 
				pWnd->PostMessage(WM_MERGE_COMPLETE, (WPARAM) pShell, NULL);


			// 작업이 끝났으므로 포인터를 지운다.
			delete pItem;
			pItem = NULL;
		}
	}

	SetStopThread(false);
	SetMergeThread(false);

	TRACE(_T("결합 스레드 종료\n"));
	return 0;
}

bool CGnuDownloadShell::InitMergeThread()
{
	if(bMergeThread) return true;
	if(AfxBeginThread(MergeChunkProc, this)) 
	{
		evMerge.SetEvent();
		return true;
	}
	return false;
}

void CGnuDownloadShell::TriggerEvent()
{
	TRACE(_T("Set Merge Event\n"));
	evMerge.SetEvent();
}

void CGnuDownloadShell::TreatMergeResult(LPARAM lParam)
{
	MergeResult* pResult;
	bool bLoop = false;			// do not use merge result list

	pResult = (MergeResult*) lParam;

	while(true)
	{
		if(!pResult)
		{
			bLoop = true;;

			if(m_ResultList.size() == 0) 
				break;

			// 락을 얻지 못하면 결과 처리 작업을 종료 시킨다.
			if(!csResultLock.Lock(1000)) break;

			pResult = (MergeResult*) m_ResultList.front();
			m_ResultList.pop_front();
			csResultLock.Unlock();
		}

		if(pResult && pResult->bC2P)
			ProcessC2PMergeResult(pResult);

		m_bMerging = false;

		delete pResult;
		pResult = NULL;

		if(!bLoop) break;
	}

	m_bMerged = true;

	// 파일 결합 작업이 완료되었으므로 또 완성된 파일 결합 작업이 있는 지 확인한다.
	CheckCompletion();

	m_BackupTrigger = true;

	m_nResultTry = 0;
}

BOOL CGnuDownloadShell::CheckChunkExist(FileChunk* pChunk)
{
	for(int i = 0; i < m_ChunkList.size(); ++i)
		if(m_ChunkList[i] == pChunk)
		{
			return TRUE;
		}

	return FALSE;
}

void CGnuDownloadShell::ProcessC2PMergeResult(MergeResult* pResult)
{
	FileChunk* pChunk = (FileChunk*) pResult->pPostChunk;
	int i, j, k;

	if(!CheckChunkExist(pChunk)) return;

	pChunk->bMerging = false;

	TRACE(_T("Chunk[%d]의 결합작업은 %s\n")
		, pChunk->StartPos, pResult->bSuccess ? _T("<성공>") : _T("<실패>"));

	if(pResult->bSuccess)
	{
		m_StopPos        = pChunk->StartPos + pChunk->ChunkLength;
		m_BytesCompleted = pChunk->StartPos + pChunk->BytesCompleted;
		m_OverlapBytes   = pChunk->OverlapBytes;
		
		for(i = 0; i < pChunk->Hosts.size(); ++i)
			AddChunkHost(m_PartialHosts, pChunk->Hosts[i]);
			//m_PartialHosts.push_back((*itChunk)->Hosts[i]);


		// Find if chunk has any hosts in common with the partial
		for(i = 0; i < m_ChunkList.size(); ++i)
			for(j = 0; j < m_ChunkList[i]->Hosts.size(); ++j)
				for(k = 0; k < m_PartialHosts.size(); ++k)
					if(m_ChunkList[i]->Hosts[j].S_addr == m_PartialHosts[k].S_addr)
					{
						m_ChunkList[i]->ChunkColor = RGB(0, 255, 0);
						m_ChunkList[i]->HostFamily = 0;
					}


		// Delete chunk
		TRACE(m_Name + _T(": Partial to Chunk Succeeded at ") + DWrdtoStr(pChunk->StartPos) + _T("\n"));
		

		pChunk->File.Abort();
		DeleteFile(pChunk->Path);

		delete pChunk;
		m_ChunkList.erase(m_ChunkList.begin());
	}

	// Join Failed
	else
	{
		// If partial and chunk are part of the same family trash the smallest 
		if( pChunk->HostFamily == 0)
		{
			// Trash Chunk
			if(pChunk->BytesCompleted < m_BytesCompleted)
			{
				m_StopPos      = pChunk->StartPos + pChunk->ChunkLength;
				m_OverlapBytes = pChunk->OverlapBytes;
	

				pChunk->File.Abort();
				
				DeleteFile(pChunk->Path);

				delete pChunk;
				m_ChunkList.erase(m_ChunkList.begin());
			}

			// Trash Partial
			else
			{
				try
				{
					m_File.SetLength(0);	
				}
				catch(...)
				{
					
				}

				m_File.Abort();
				
				while(m_PartialHosts.size())
					m_PartialHosts.pop_back();

				m_BytesCompleted = 0;
			}
		
		}


		// If partial and chunk are part of different families
		else
		{
			// Trash chunk's family
			if(ChunkFamilySize(0) > ChunkFamilySize(pChunk->HostFamily))
				TrashFamily(pChunk->HostFamily);
			
			// Trash partial's family
			else
				TrashFamily(0);
		}
	}
}

bool CGnuDownloadShell::TryAttachChunk2Partial()
{
	// 파일 전송에 대해서만 유효한 코드이다.
	if(m_nType != GDT_FILE) return false;

	// 파셜의 다운로드가 완료되지 않아서 작업을 중지한다.
	if(m_bMerging) return false;
	if(!m_bComplete) return false;

	int i;

	FileChunk* pChunk = NULL;

	if(m_ChunkList.size() == 0)
		return false;
	
	pChunk = m_ChunkList[0];

	// 청크 다운로드가 완료되지 않으면 작업을 중지한다.
	// 현재 결합작업 중이므로 Chunk를 Partial에 붙이는 작업은 중지한다.
	if(!pChunk->bComplete) return false;

	// Partial file active
	for(i = 0; i < m_Sockets.size(); ++i)
	{
		if(m_Sockets[i]->m_pChunk == NULL && m_Sockets[i]->m_Status == TRANSFER_RECEIVING)
			return false;
	}

	// Chunk file active
	for(i = 0; i < m_Sockets.size(); ++i)
	{
		if(m_Sockets[i]->m_pChunk == pChunk)
			return false;
	}


	// Make sure partial file is open
	if(m_File.m_hFile == CFileLock::hFileNull)
	{
		try
		{
			if(!m_File.Open(m_PartialPath, CFileLock::modeReadWrite | CFileLock::shareDenyWrite))
				return false;
		}
		catch(CFileException* e)
		{
			e->Delete();
			return false;
		}
	}

	// Make sure chunk file is open
	if(pChunk->File.m_hFile == CFileLock::hFileNull)
	{
		try
		{
			if(!pChunk->File.Open(pChunk->Path, CFileLock::modeReadWrite | CFileLock::shareDenyWrite))
				return false;
		}
		catch(CFileException* e)
		{
			e->Delete();
			return false;
		}
	}
	// If chunk small, just combine it with partial

	if(pChunk->BytesCompleted < 4096)
	{
		m_StopPos        = pChunk->StartPos + pChunk->ChunkLength;
		m_OverlapBytes   = pChunk->OverlapBytes;

		pChunk->File.Abort();
	
		DeleteFile(pChunk->Path);

		delete pChunk;
		m_ChunkList.erase(m_ChunkList.begin());

		return true;
	}

	// Partial must overlap chunk
	if(m_BytesCompleted - 4096 < pChunk->StartPos)
		return false;


	// Make sure partial is big enough
	if(m_BytesCompleted < 4096)
		return false;

	if(pChunk->bMerging) return true;
	pChunk->bMerging = true;

	MergeItem* pItem;
	pItem = new MergeItem;
	pItem->pMaster = (LPVOID) this;
	pItem->pPreChunk = (LPVOID) NULL;
	pItem->pPostChunk = (LPVOID) pChunk;

	// 빠른 시간에 락을 얻지 못하면 Merge Try 작업을 종료시킨다.
	if(!csMergeLock.Lock(200))
	{
		delete pItem;
		return true;
	}
	MergeList.push_back(pItem);
	csMergeLock.Unlock();

	// Merge 작업은 Shell당 한번에 한개씩만 수행한다.
	m_bMerging = true;

	// trigger to run the MergeThread
	TriggerEvent();

	return true;
}

void CGnuDownloadShell::FreeMergeItem()
{
	while(CGnuDownloadShell::MergeList.size())
	{
		MergeItem *pItem;
		pItem = CGnuDownloadShell::MergeList.back();
		CGnuDownloadShell::MergeList.pop_back();
		delete pItem;
	}
}

BOOL CGnuDownloadShell::IsAttachable()
{
	if(m_bComplete && m_ChunkList.size() > 0)
		return m_ChunkList[0]->bComplete;

	return false;
}

void CGnuDownloadShell::GetExploreBuddyList(LPVOID plist)
{
	if(!plist) return;

	CStringList* list = (CStringList*) plist;
	list->RemoveAll();

	std::vector<Result>::iterator itQueue;
	for(itQueue = m_Queue.begin(); itQueue != m_Queue.end(); ++itQueue)
	{
		if((*itQueue).wVersion >= APP_VERSION)
			list->AddTail((*itQueue).UserID);
		else
		{
			CString sTemp;
			sTemp.Format(_T("%s [old]"), (*itQueue).UserID);
			list->AddTail(sTemp);
		}
	}
}

void CGnuDownloadShell::IncreaseTryCnt()
{
	if(TryCnt == 0)
		theApp.m_spGD->m_Control.IncreaseDLTryingCnt();

	++TryCnt;
}

void CGnuDownloadShell::DecreaseTryCnt(bool bAll)
{
	if(bAll)
		TryCnt = 0;
	else
		--TryCnt;

	if(TryCnt == 0)
		theApp.m_spGD->m_Control.DecreaseDLTryingCnt();
}

#ifdef MULTI_UPDATETFSTATUS
// written by moonknit 2005-10-14
void CGnuDownloadShell::AddSRLink(int nFSerial, CString sSRID)
{
	std::list<SRLINK_ITEM>::iterator it;

	for(it = m_listSRLink.begin(); it != m_listSRLink.end(); ++it)
	{
		if((*it).sSRID == sSRID)
		{
			return;
		}
	}

	m_listSRLink.push_back(SRLINK_ITEM(nFSerial, sSRID));
}

// written by moonknit 2005-10-14
void CGnuDownloadShell::DelSRLink(CString sSRID)
{
	std::list<SRLINK_ITEM>::iterator it;

	for(it = m_listSRLink.begin(); it != m_listSRLink.end(); ++it)
	{
		if((*it).sSRID == sSRID)
		{
			m_listSRLink.erase(it);
			break;
		}
	}
}
#endif