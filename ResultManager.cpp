// ResultManager.cpp: implementation of the CResultManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "ResultManager.h"
#include "SearchManager.h"
#include "RSSDB.h"
#include "MainFrm.h"
#include "RSSCurrentList.h"
#include "Dlg_Subscribe.h"
#include "./GnuModule/GnuDoc.h"
#include "./GnuModule/GnuControl.h"
#include "LocalProperties.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CFISHApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultManager::CResultManager()
{

}

CResultManager::~CResultManager()
{
	TRACE(_T("CResultManager Destroy\r\n"));
}

/**************************************************************************
 * CResultManager::ProcessResult
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * CRSSDB의 결과를 처리를 수행한다.
 **************************************************************************/
void CResultManager::ProcessResult()
{
	auto_ptr<RESULT> apresult;
	while(theApp.m_spRD->GetResult(apresult))
	{
		switch(apresult->resulttype)
		{
		case RT_POSTADD:
			{
				ResultAddPost* ppostadd =
					static_cast<ResultAddPost*> (apresult.get());

				RP_PostAdd(ppostadd);
			}
			break;
		case RT_NORMAL:
			{
				ResultNormal* pnormal =
					static_cast<ResultNormal*> (apresult.get());

				switch(pnormal->subtype)
				{
				case WS_POST_DELETE:
					RP_PostDelete(pnormal);
					break;
				case WS_CHANNEL_ADD:
					RP_ChannelAdd(pnormal);
					break;
				case WS_CHANNEL_DELETE:
					RP_ChannelDelete(pnormal);
					break;
				case WS_CHANNEL_LOAD:
					RP_ChannelLoad(pnormal);
					break;
				case WS_CHANNEL_FLUSH:
					RP_ChannelFlush(pnormal);
					break;
				};
			}
			break;
		case RT_POST:
			{
				ResultPost* ppost = 
					static_cast<ResultPost*> (apresult.get());

				switch(ppost->subtype)
				{
				case WS_POST_GET:
					RP_PostGet(ppost);
					break;
				case WS_CHANNEL_GET:
					RP_ChannelGet(ppost);
					break;
				case WS_POST_UPDATE:
					RP_PostUpdate(ppost);
					break;
				}
			}
			break;
		case RT_SEARCH:
			{
				ResultSearch* psearch = 
					static_cast<ResultSearch*> (apresult.get());
				RP_Search(psearch);
			}
			break;
		case RT_UPLOAD:
			{
				ResultUpload* pupload = 
					static_cast<ResultUpload*> (apresult.get());

				RP_Upload(pupload);
			}
			break;
		}
	}
}


/**************************************************************************
 * CResultManager::RP_Search
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-08
 *
 * @Description
 * 검색 결과에 대한 처리를 수행한다.
 **************************************************************************/
void CResultManager::RP_Search(ResultSearch* psearch)
{
	TRACE(_T("Search Result\r\n"));

	switch(psearch->type)
	{
	case LOCAL_SEARCH:
		{
			if(psearch->aplistpost.get() == NULL) return;

			COleDateTime lasttime = COleDateTime::GetCurrentTime();

			if(psearch->querydata.QueryGuid != NULL_GUID)
			{
				POST_LIST::iterator it;
				SE_ITEM item;
				for(it = psearch->aplistpost->begin(); it != psearch->aplistpost->end(); ++it)
				{
					theApp.m_spSM->InsertSearchResult(psearch->querydata.QueryGuid, (*it)); 
				}
			}
			else
			{
				theApp.m_spRD->IPostAdd(psearch->aplistpost, lasttime, AT_SEARCH_LOCAL, psearch->chid);
			}
		}
		break;
	case NET_SEARCH:
	case LONGTERM_SEARCH:
	case GRAPH_SEARCH:
		{
			// 네트워크로 검색 결과를 알린다.
			theApp.m_spGD->m_Control.SendResults(psearch->querydata, psearch->apitems, psearch->type);
		}
		break;
	}
}

/**************************************************************************
 * CResultManager::RP_ChannelGet
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 채널의 포스트 가져오기 작업에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_ChannelGet(ResultPost* ppost)
{
	TRACE(_T("Channel Get Result\r\n"));

	int s = ppost->aplistpost->size();
	theApp.m_spCL->AppendList(ppost->aplistpost, ppost->addinfo);
}

/**************************************************************************
 * CResultManager::RP_PostGet
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 가져오기 작업에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_PostGet(ResultPost* ppost)
{
	TRACE(_T("Post Get Result\r\n"));

	theApp.m_spCL->AppendList(ppost->aplistpost);
}

/**************************************************************************
 * CResultManager::RP_PostUpdate
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 갱신에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_PostUpdate(ResultPost* ppost)
{
	TRACE(_T("Post Update Result\r\n"));

	// theRSSCurrent를 이용하여 갱신된 포스트가 있는 경우 갱신처리를 수행한다.
	theApp.m_spCL->UpdatePostList(ppost->aplistpost, ppost->addinfo, TRUE);
}

/**************************************************************************
 * CResultManager::RP_PostDelete
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 삭제에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_PostDelete(ResultNormal* pnormal)
{
	TRACE(_T("Post Delete Result\r\n"));
}

/**************************************************************************
 * CResultManager::RP_ChannelAdd
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 채널 추가에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_ChannelAdd(ResultNormal* pnormal)
{
	TRACE(_T("Channel Add Result\r\n"));
}

/**************************************************************************
 * CResultManager::RP_ChannelDelete
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 채널 삭제에 대한 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_ChannelDelete(ResultNormal* pnormal)
{
	TRACE(_T("Channel Delete Result\r\n"));
}

/**************************************************************************
 * CResultManager::RP_ChannelLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * File Cache의 포스트 읽어오기 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_ChannelLoad(ResultNormal* pnormal)
{
	TRACE(_T("Channel Load Result\r\n"));
	::SendMessage(theApp.m_spLP->GetMainHwnd(), WM_POSTINITALL, 0, 0);
}

/**************************************************************************
 * CResultManager::RP_ChannelFlush
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 정보 Flush 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_ChannelFlush(ResultNormal* pnormal)
{
	TRACE(_T("Channel Flush Result\r\n"));
}

/**************************************************************************
 * CResultManager::RP_PostAdd
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-08
 *
 * @Description
 * 포스트 추가의 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_PostAdd(ResultAddPost* ppostadd)
{
	TRACE(_T("Post Add Result\r\n"));
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	if(!pFrame) return;

	{
		SmartPtr<CDlg_Subscribe> spsubscribe;
		
		pFrame->GetDlgSubscribe(spsubscribe);
		

		if(!spsubscribe) return;

		if(ppostadd->addtype == AT_FETCH)
		{
			spsubscribe->FetchComplete(ppostadd->channelid, ppostadd->t, ppostadd->postcnt, ppostadd->Ex);
			theApp.m_spCL->AppendList(ppostadd->aplistpost, ppostadd->channelid, TRUE);
		}
		else if(ppostadd->addtype == AT_SEARCH_LOCAL)
		{
			theApp.m_spCL->AppendList(ppostadd->aplistpost, ppostadd->channelid, TRUE);
		}
		else if(ppostadd->addtype == AT_SEARCH_NET)
		{
			if(ppostadd->result && ppostadd->aplistpost->size() > 0)
			{
				TRACE(_T("search inserted size : %d\r\n"), ppostadd->aplistpost->size());
				theApp.m_spCL->AppendList(ppostadd->aplistpost, ppostadd->channelid, TRUE);

				// searcher의 post id 갱신 
				POST_LIST::iterator it = ppostadd->aplistpost->begin();
				theApp.m_spSM->PostUpdate(ppostadd->guid, (*it).guid, (*it).postid);

				TRACE(_T("post guid = %s, post id = %d\r\n"), (*it).guid, (*it).postid);

				ppostadd->aplistpost->clear();
			}
		}
	}
}

/**************************************************************************
 * static method CResultManager::RP_Upload
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-02
 *
 * @Description
 * 포스트 추가의 결과를 수행한다.
 **************************************************************************/
void CResultManager::RP_Upload(ResultUpload* pupload)
{
	theApp.m_spGD->m_Control.VerifyPostResult(pupload->lpuploader, pupload->xml);

/*
	CEzXMLParser parser;

	if(parser.LoadXML((LPTSTR) (LPCTSTR) pupload->xml, CEzXMLParser::LOAD_STRING))
	{
		AfxMessageBox(_T("Item XML Successfully Open"));
		IXMLDOMNodeListPtr listptr;
		listptr = parser.SearchNodes(_T("item"));

		if(listptr == NULL || listptr->length == 0)
		{
			AfxMessageBox(_T("No Item"));
		}

		IXMLDOMNodePtr nodeptr;
		nodeptr = listptr->item[0];

		CString sdata = GetNamedNodeText(XML_SUBJECT, &parser, nodeptr);
		CString strread;
		strread.Format(_T("%s : %s"), XML_SUBJECT, sdata);
		AfxMessageBox(strread);

	}
	*/
}