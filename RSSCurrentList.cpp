// RSSCurrentList.cpp: implementation of the CRSSCurrentList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "RSSDB.h"
#include "RSSCurrentList.h"
#include "Dlg_RSSList.h"
#include "fish_struct.h"
#include "MainFrm.H"
#include "HtmlMaker.h"
#include "LocalProperties.h"
#include "GnuModule/FileLock.h"		// for CAutoLock
#include "fish_common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_IPPCNT					5
#define MAX_POSTCNT					3000
#define STR_INFORM_POSTMAXPAGEHIT	_T("�ѹ��� �� �� �ִ� ����Ʈ�� ������ %d���� �Ѱ��Դϴ�.")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CFISHApp theApp;

const BOOL MakeStreamToFile(const CString& filename, CString& stream)
{
	CFile tmp;
	if(tmp.Open(filename, CFile::modeCreate | CFile::modeWrite))
	{
#ifdef _UNICODE
		int l = stream.GetLength() * 2;
		char* buf = (char*) malloc(l);
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, stream.GetBuffer(stream.GetLength() + 1), stream.GetLength() + 1
			, buf, l, NULL, NULL);
		tmp.Write(buf, strlen(buf));
		delete buf;
#else
		tmp.Write(stream.GetBuffer(stream.GetLength() + 1), stream.GetLength() + 1);
#endif
		tmp.Close();
		return TRUE;
	}

	return FALSE;
}

CRSSCurrentList::CRSSCurrentList()
{
	m_nCurrentListID	= 0;
	m_nCurrentReading	= 0;
	m_bUnreadOnly		= FALSE;
	m_hParent			= NULL;
	m_bViewReverse		= FALSE;
	m_sSearchWord		= _T("");
	m_bGroupLoad		= FALSE;
	m_splistpost		= SmartPtr<POST_LIST> (new POST_LIST);
	m_spviewposts		= SmartPtr<POST_LIST> (new POST_LIST);
}

CRSSCurrentList::~CRSSCurrentList()
{
	TRACE(_T("CRSSCurrentList Destroy\r\n"));
	if(m_splistpost != NULL)
		m_splistpost->clear();
	if(m_spviewposts != NULL)
		m_spviewposts->clear();
}

/**************************************************************************
 * method CRSSCurrentList::SetList
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * ���� POSTITEM�� ��� �����ϰ� ���ο� POST_LIST�� ����Ѵ�.
 * 
 * @Modification
 * 2005/02/08 eternalbleu �ټ� �������� ���ÿ� �Է��ϴ� ������� ������.
 *
 * [Parameters]
 * (in/out SmartPtr<POST_LIST>&) l - ���� ����� POST_LIST
 **************************************************************************/
void CRSSCurrentList::SetList(SmartPtr<POST_LIST>& l)
{
	if(!m_splistdlg || m_splistpost.get == NULL) return;

	// GUI list clear ��û	
	m_splistdlg->List_Clear();

	// ���� ����� free & �� ��� ������ ����
	m_splistpost = l;

	// ��� �������� �߰��Ѵ�.
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		// GMT �ð��� ���� �־� Local �ð����� �����.
		(*it).pubdate += theApp.m_spLP->GetSPGMT();

		// �������� �߰��Ѵ�.
		//m_splistdlg->List_ItemAdd((*it));     //commented by eternalbley
	}
    m_splistdlg->List_ItemAdd(m_splistpost);
}

/**************************************************************************
 * method CRSSCurrentList::AppendList
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 * updated 2005-12-19 :: bsearch parameter �߰� by moonknit
 *
 * @Description
 * ���� POST_LIST�� ���ο� POST_LIST�� �����Ѵ�.
 *
 * @Modification
 * �ټ��������� ���ÿ� �Է��ϴ� ������� ������.
 *
 * @Parameters
 * (in/out SmartPtr<POST_LIST>&) l - ���� �߰��� POST_LIST
 * (in int) channelid - �߰��� ����Ʈ�� ä�� ���̵�
 * (in BOOL) bsearch - �˻� ����� ��� TRUE�� �ִ´�.
 **************************************************************************/
void CRSSCurrentList::AppendList(auto_ptr<POST_LIST>& l, int channelid, BOOL bsearch)
{
	if(l.get() == NULL) return;

	POSTITEM pi;

	// ���� ����� ä���� ��쿡 �߰� �Ѵ�.
	list<int>::iterator it2;
	BOOL bfind = FALSE;
	int si = m_listRemainChannel.size();
	for(it2 = m_listRemainChannel.begin() ; it2 != m_listRemainChannel.end(); ++it2)
	{
		if((*it2) == channelid)
		{
			// �˻� ����� ���� ��� �߰��� �ƴϸ� ����� ä�� ��Ͽ��� �����Ѵ�.
			if(m_bGroupLoad) m_listRemainChannel.erase(it2);

			bfind = TRUE;
			break;
		}
	}

	if(!bfind) return;

	if(m_bFirstAppend)
	{
		m_bFirstAppend = FALSE;
		m_bHitMaxCnt = FALSE;
		m_splistpost->clear();

		m_npostcnt = 0;
		m_nunreadcnt = 0;
	}

	if(m_bHitMaxCnt) return;

	if(m_splistpost->size() == 0)
	{
		m_nCurrentReading = 0;
		m_splistdlg->List_Clear();

		m_splistdlg->Invalidate(FALSE);

		m_nCurrentListItemCnt += l->size();
	}
	else if(bsearch)
		m_nCurrentListItemCnt += l->size();

	TRACE(_T("list item cnt : %d\r\n"), m_nCurrentListItemCnt);

	DWORD dtstart = GetTickCount();
	SmartPtr<POST_LIST> sptemp = SmartPtr<POST_LIST> (new POST_LIST);

	if(CT_SEARCH == m_nchanneltype)
	{
		POST_LIST::iterator lit;
		for(lit = l->begin(); lit != l->end(); ++lit)
		{
			// �̹� �����ϴ� ����Ʈ�� ���� �߰����� �ʴ´�.
			if(FindPost((*lit).postid, pi)) continue;
			// GMT �ð��� ���� �־� Local �ð����� �����.
			(*lit).pubdate += theApp.m_spLP->GetSPGMT();

			++m_npostcnt;
			if(!(*lit).read)
				++m_nunreadcnt;

			m_splistpost->push_back((*lit));

			if(m_splistpost->size() > MAX_POSTCNT)
			{
				m_bHitMaxCnt = TRUE;
				break;
			}
			sptemp->push_back((*lit));
		}
	}
	else
	{
		POST_LIST::reverse_iterator rit;
		for(rit = l->rbegin(); rit != l->rend(); ++rit)
		{
			// �̹� �����ϴ� ����Ʈ�� ���� �߰����� �ʴ´�.
			if(FindPost((*rit).postid, pi)) continue;
			// GMT �ð��� ���� �־� Local �ð����� �����.
			(*rit).pubdate += theApp.m_spLP->GetSPGMT();

			++m_npostcnt;
			if(!(*rit).read)
				++m_nunreadcnt;

			m_splistpost->push_back((*rit));

			if(m_splistpost->size() > MAX_POSTCNT)
			{
				m_bHitMaxCnt = TRUE;
				break;
			}

			sptemp->push_back((*rit));
		}
	}

    m_splistdlg->List_ItemAdd(sptemp);

	if(m_nCurrentReading == 0 && m_bAutoSelect)
	{
		m_bAutoSelect = FALSE;
		m_splistdlg->m_listRSS.SetHotItem(0);
		m_splistdlg->m_listRSS.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	// ���� ����� ����Ʈ ������ �ٲ������ �˷��־�� ��!!
	::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);

	TRACE(_T("appent time : %d\r\n"), GetTickCount() - dtstart);

//	if(m_listRemainChannel.size() == 0 || bsearch)
	{
		if(m_bHitMaxCnt)
		{
			SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
			CString msg;
			msg.Format(STR_INFORM_POSTMAXPAGEHIT, MAX_POSTCNT);
			FishMessageBox(msg);
		}
		else
		{
			if(m_bGroupLoad)
			{
				TRACE(_T("remain channel size : %d\r\n"), m_listRemainChannel.size());
				if(m_listRemainChannel.size() == 0)
					SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
			}
			else
				SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
		}

	}
}

/**************************************************************************
 * methos CRSSCurrentList::FindPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * ��Ͽ��� postid�� �̿��Ͽ� POSTITEM�� ã�Ƴ��� ��ȯ�Ѵ�.
 * �б� ó���� �ؾ��� ��쿡�� RSSDB�� �б�ó���� ��û�Ѵ�.
 *
 * @Parameters
 * (in int) id - ã�ƾ��� ����Ʈ�� id
 * (out POSTITEM&) post - ��ȯ�� ����Ʈ ����
 * (in BOOL) bread - ���� ó������
 *
 * @Result
 * BOOL - ����Ʈ�� �߰߿���
 **************************************************************************/
BOOL CRSSCurrentList::FindPost(int id, POSTITEM& post, BOOL bread)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			post = (*it);

			if(bread)
			{
				theApp.m_spRD->IPostRead(id, (*it).channelid);
			}
			return TRUE;
		}
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::GetPostXMLStream
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * postid�� �̿��Ͽ� post�� �о ȭ������ �����ֱ� ���� HMTL�� �����Ͽ� ��ȯ�Ѵ�.
 *
 * @Parameters
 * (in int) id - ã�ƾ��� ����Ʈ�� id
 * (out CString&) xmlstream - ��ȯ�� �ؽ�Ʈ
 * (in BOOL) bread - ���� ó������
 *
 * @Result
 * BOOL - ����Ʈ�� �߰߿���
 **************************************************************************/
BOOL CRSSCurrentList::GetPostXMLStream(int id, CString& xmlstream, BOOL bread)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			// HTML Maker�� �̿��Ѵ�.
			if(!RequestHTMLMake((*it), xmlstream))
				return FALSE;

			if(bread && !(*it).read)
			{
				theApp.m_spRD->IPostRead(id, (*it).channelid);
			}

			m_nCurrentReading = id;

//			m_splistdlg->List_Select(m_nCurrentReading);

			m_bListEnd = FALSE;

			return TRUE;
		}
	}

	return FALSE;
}

std::list<int>::iterator FindIntItem(list<int>& src, int i)
{
	std::list<int>::iterator it;
	for(it = src.begin(); it != src.end(); ++it)
	{
		if(i == (*it)) return it;
	}

	return it;
}

/**************************************************************************
 * method CRSSCurrentList::GetPostXMLStream
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-29
 *
 * @Description
 * ������ postid�� �̿��Ͽ� html page�� �����Ѵ�.
 *
 * @Parameters
 * (in list<int>) ids - ã�ƾ��� ����Ʈ�� id ���
 * (out CString&) xmlstream - ��ȯ�� �ؽ�Ʈ
 *
 * @Result
 * BOOL - ����Ʈ�� �߰߿���
 **************************************************************************/
BOOL CRSSCurrentList::GetPostXMLStream(list<int> ids, CString& xmlstream)
{
	if(ids.size() == 0) return FALSE;

	POST_LIST::iterator it;
	list<int>::iterator it2;
	SmartPtr<POST_LIST> aplist (new POST_LIST);

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		it2 = FindIntItem(ids, (*it).postid);
		if(it2 != ids.end())
		{
			aplist->push_back(*it);

			ids.erase(it2);
		}
	}

	m_bListEnd = TRUE;

	// HTML Maker�� �̿��Ѵ�.
	if(aplist->size() == 0 || !RequestHTMLMake(aplist, xmlstream, FALSE))
		return FALSE;

	return TRUE;
}


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * ��ϻ��� ��� POST�� �̿��Ͽ� ȭ������ ������ HTML�� �����Ѵ�.
 *
 * @Parameters
 * (out CString&) xmlstream - ��ȯ�� �ؽ�Ʈ
 **************************************************************************/
BOOL CRSSCurrentList::GetAllXMLStream(CString& xmlstream)
{
	return RequestHTMLMake(xmlstream, FALSE);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-23
 *
 * @Description
 * LIST�� ó���� �˷��� �� ��� ���Ǵ� LIST DIALOG�� ����Ʈ �����͸� �����Ѵ�.
 *
 * @Parameters
 * (in/out SmartPtr<CDlg_RSSList>&) splistdlg - ����� LIST DIALOG�� ����Ʈ ������ (reference count)
 **************************************************************************/
void CRSSCurrentList::SetListDlg(SmartPtr<CDlg_RSSList>& splistdlg)
{
	m_splistdlg = splistdlg;
}

/**************************************************************************
 * method CRSSCurrentList::ReadOnPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 * updated 2005-11-28 :: readon ó��
 * modified 2006-03-20 :: parameter bNav �߰� by moonknit
 *
 * @Description
 * �ش� id�� ����Ʈ�� link�� ����.
 *
 * @Parameters
 * (in int) id - link�� �� post
 * (in BOOL) bNav - url �� �̿��� navigation ����
 **************************************************************************/
BOOL CRSSCurrentList::ReadOnPost(int id, BOOL bNav)
{
	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == id)
		{
			if(!(*it).readon)
			{
				(*it).readon = TRUE;
				theApp.m_spRD->IPostReadOn(id, (*it).channelid);
			}
			
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			if(pFrame)
			{
				pFrame->NavigateNewWindow((*it).url);
			}

			return TRUE;
		}
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::ReadPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * ����/�� ���� ó�� 
 *
 * @Parameters
 * (in int) id - ���� ���¸� ������ ����Ʈ�� id
 * (in BOOL) bread - ���� ó������
 *
 * @Result
 * BOOL - ����Ʈ�� �߰߿���
 **************************************************************************/
BOOL CRSSCurrentList::ReadPost(list<int>& idlist, BOOL bread)
{
	POST_LIST::iterator it;
	BOOL result = FALSE;
	BOOL single = FALSE;
	int pid, cid;

	list<int>::iterator idit;
	POST_LIST	pl;

	if(idlist.size() == 1) single = TRUE;

	for(idit = idlist.begin(); idit != idlist.end(); ++idit)
	{
		for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
		{
			if((*it).postid == (*idit))
			{
				if(single)
				{
					pid = (*it).postid;
					cid = (*it).channelid;
				}
				else
					pl.push_back( POSTITEM((*it).postid, (*it).channelid, bread, bread, bread) );

				(*it).read = bread;

				result = TRUE;
				break;
			}
		}
	}

	if(single)
		theApp.m_spRD->IPostRead(pid, cid, bread);
	else
		theApp.m_spRD->IPostUpdate(pl, FISH_ITEM_READ);

	return result;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-11-24
 *
 * @Description
 * ����Ʈ�� ������
 *
 * @Parameters
 * (in list<int>) idlist - ������ ����Ʈ�� idlist
 *
 * @Result
 * BOOL - ����Ʈ�� �߰߿���
 **************************************************************************/
BOOL CRSSCurrentList::DeletePost(list<int>& idlist)
{
	POST_LIST::iterator it, tit;
	BOOL result = FALSE;

	list<int>::iterator idit;

	for(idit = idlist.begin(); idit != idlist.end(); ++idit)
	{
		for(it = m_splistpost->begin(); it != m_splistpost->end();)
		{
			tit = it;
			++it;

			if((*tit).postid == (*idit))
			{
				// count post
				if(!(*tit).read)
					--m_nunreadcnt;
				--m_npostcnt;

				m_splistpost->erase(tit);
				result = TRUE;
				break;
			}
		}
	}

	// ���� ����� ����Ʈ ������ �ٲ������ �˷��־�� ��!!
	::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);

	theApp.m_spRD->IPostDelete(idlist);

	return result;
}

void CRSSCurrentList::SetCurrentListID(int nID)
{
	m_nCurrentListID = nID;
}

int CRSSCurrentList::GetCurrentListID()
{
	return m_nCurrentListID;
}

/**************************************************************************
 * method CRSSCurrentList::RequestSearchChannel
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-19
 *
 * @Description
 * �˻� ä���� ���
 **************************************************************************/
BOOL CRSSCurrentList::RequestSearchChannel(int channelid)
{
	if(m_nCurrentListID == channelid) return FALSE;

	m_sSearchWord = _T("");

	m_nCurrentListID = channelid;
	m_bUnreadOnly = FALSE;			// Search Channel View���� ���� ä�ΰ� ���� ���� ä�� ��θ� �����ش�.

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_bFirstAppend = TRUE;
	m_listRemainChannel.push_back(channelid);
	m_listCurrentChannel.push_back(channelid);

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::RequestChannelOnLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * �ش� ä���� ����Ʈ�� �о�ͼ� CurrentList�� �߰��Ѵ�.
 *
 * @Parameters
 * (in int) channelid - �о�� ä���� id
 * (in int) channeltype - ä�� ��� ������ style ����
 * (in BOOL) bunreadonly - ���� ���� ����Ʈ�� �������⸦ ���ϸ� TRUE, ��� ����Ʈ�� �������⸦ ���ϸ� FLASE
 **************************************************************************/
BOOL CRSSCurrentList::RequestChannelOnLoad(int channelid
										   , int channeltype
										   , BOOL bunreadonly
										   , BOOL bautoselect)
{
//	if( m_nCurrentListID == channelid && m_bUnreadOnly == bunreadonly) 
//        return FALSE;

    //////////////////////////////////////////////////////////////////////////
    // ����Ʈ ä���� ��� ����
    if ( channeltype  == CT_SEARCH || channeltype == CT_LSEARCH )
        theApp.m_spCL->SetListMode(TRUE);
    else
        theApp.m_spCL->SetListMode(FALSE);
    //////////////////////////////////////////////////////////////////////////

	m_nchanneltype = channeltype;

	m_sSearchWord = _T("");

	m_nCurrentListID	= channelid;
	m_bUnreadOnly		= bunreadonly;
	m_bAutoSelect		= bautoselect;
	m_bGroupLoad		= FALSE;

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_bFirstAppend = TRUE;
	m_listRemainChannel.push_back(channelid);
	m_listCurrentChannel.push_back(channelid);

	if(bunreadonly)
		theApp.m_spRD->IChannelGet(channelid, FISH_GET_DEFAULT | FISH_GET_UNREADONLY);
	else
		theApp.m_spRD->IChannelGet(channelid);

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::RequestGroupOnLoad
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-09
 *
 * @Description
 * �ش� �׷��� ����Ʈ�� �о�ͼ� CurrentList�� �߰��Ѵ�.
 *
 * @Parameters
 * (in int) groupid - �о�� �׷��� id
 * (in list<int>&) idlist - �о�� ä���� id ���
 * (in BOOL) bunreadonly - ���� ���� ����Ʈ�� �������⸦ ���ϸ� TRUE, ��� ����Ʈ�� �������⸦ ���ϸ� FLASE
 **************************************************************************/
BOOL CRSSCurrentList::RequestGroupOnLoad(int groupid, list<int>& idlist, BOOL bunreadonly, BOOL bautoselect)
{
//	if(m_nCurrentListID == groupid  && m_bUnreadOnly == bunreadonly ) 
//        return FALSE;
	m_nchanneltype = CT_NONE;

	m_sSearchWord = _T("");

	m_nCurrentListID	= groupid;
	m_bUnreadOnly		= bunreadonly;
	m_bAutoSelect		= bautoselect;
	m_bGroupLoad		= TRUE;

	m_listRemainChannel.clear();
	m_listCurrentChannel.clear();
	m_listRemainChannel.insert(m_listRemainChannel.end(), idlist.begin(), idlist.end());
	m_listCurrentChannel.insert(m_listCurrentChannel.end(), idlist.begin(), idlist.end());

	m_bFirstAppend = TRUE;

	list<int>::iterator it;
	for(it = idlist.begin(); it != idlist.end(); ++it)
	{
		if(it == idlist.begin())
			m_nFirstChannel = (*it);

		if(bunreadonly)
			theApp.m_spRD->IChannelGet((*it), FISH_GET_DEFAULT | FISH_GET_UNREADONLY);
		else
			theApp.m_spRD->IChannelGet((*it));
	}

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetCurrentPage
 *
 * written by moonknit
 *
 * @history
 * created 2006-03-03
 *
 * @Description
 * ���簡 ���° ���������� �˷��ش�.
 *
 * @Parameters
 * (in int) npagepost = �������� ����Ʈ ��
 *
 * @Return
 * (int) page ��ȣ -1�� �������� �߰����� ���� ����
 **************************************************************************/
int CRSSCurrentList::GetCurrentPage(int npagepost)
{
	if(m_bListEnd) return -1;

	int postcnt = 0;

	POST_LIST::iterator it;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == m_nCurrentReading)
		{
			int npage = (postcnt) / npagepost;
//			if(0 == (postcnt % npagepost)) --npage;
			return npage;
		}

		++postcnt;
	}

	return -1;
}

/**************************************************************************
 * method CRSSCurrentList::GetNextPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 * updated 2006-01-13 :: npos, ncnt �߰� �� �׿� ���� ���� by moonknit
 *
 * @Description
 * ���� ���õ� ����Ʈ�� ���� ����Ʈ�� �о�´�.
 * ncnt > 1 �ΰ�쿡�� readó���� ���� �ʴ´�?
 *
 * @Parameters
 * (out CString&) xmlstream - ���õ� ����Ʈ�� ������ html
 * (in int) npos - ���° ��?
 * (in int) ncnt - ��� ������
 * (in BOOL) bread - ���� ó�� ����
 **************************************************************************/
BOOL CRSSCurrentList::GetNextPost(CString& xmlstream, int npos, int ncnt, BOOL bread)
{
	if(npos < 1 
		|| ncnt < 1
		|| ncnt > MAX_IPPCNT			// MAXIMUM ITEMS PER PAGE COUNT
		|| m_splistpost->size() == 0) return FALSE;

	// page is 0-device
	int nPage = GetCurrentPage(ncnt);
	int nLastPage = (m_splistpost->size() - 1) / ncnt;
//	if(0 == (m_splistpost->size() % ncnt)) --nLastPage;

	if(nPage == nLastPage) return FALSE;

#ifdef USE_POSTJUMP
	int nDestPage = nPage + npos;
	if( nDestPage > nLastPage ) nDestPage = nLastPage;
#else
	int nDestPage = nPage + 1;
#endif

	int nDestPost = nDestPage * ncnt;

	// ���� �������� ù ����Ʈ �߰�
	POST_LIST::iterator it, tit;
	int n = 0;
	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if(n++ == nDestPost) break;
	}

	if(ncnt == 1)
	{
		if(!RequestHTMLMake((*it), xmlstream))
			return FALSE;

		if(bread && !(*it).read)
		{
			theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
		}

		m_nCurrentReading = (*it).postid;
	}
	else
	{
		int tmpcnt = ncnt;

		tit = it;
		SmartPtr<POST_LIST> aptmplist(new POST_LIST);
		for(; it != m_splistpost->end(); ++it)
		{
			if(tmpcnt-- == 0) break;

			aptmplist->push_back((*it));
			m_nCurrentReading = (*it).postid;
		}

		if(!RequestHTMLMake(aptmplist, xmlstream, FALSE))
			return FALSE;

		if(bread)
		{
			it = tit;
			tmpcnt = ncnt;
			for(; it != m_splistpost->end(); ++it)
			{
				if(tmpcnt-- == 0) break;
				if(!(*it).read)
				{
					theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
				}
			}
		}
	}

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = FALSE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetPrevPost
 *
 * written by moonknit
 *
 * @history
 * created 2005-12-12
 * updated 2006-01-13 :: npos, ncnt �߰� �� �׿� ���� ���� by moonknit
 *
 * @Description
 * ���� ���õ� ����Ʈ�� ���� ����Ʈ�� �о�´�.
 *
 * @Parameters
 * (out CString&) xmlstream - ���õ� ����Ʈ�� ������ html
 * (in int) npos - ���° ��?
 * (in int) ncnt - ��� ������
 * (in BOOL) bread - ���� ó�� ����
 **************************************************************************/
BOOL CRSSCurrentList::GetPrevPost(CString& xmlstream, int npos, int ncnt, BOOL bread)
{
	if(npos < 1 
		|| ncnt < 1
		|| ncnt > MAX_IPPCNT			// MAXIMUM ITEMS PER PAGE COUNT
		|| m_splistpost->size() == 0) return FALSE;

	// page is 0-device
	int nPage = GetCurrentPage(ncnt);
	int nLastPage = ((m_splistpost->size() - 1) / ncnt);

	// ù ���������� �� �̻� �ڷ� �� �� ����.
	if(nPage == 0) return FALSE;

#ifdef USE_POSTJUMP
	int nDestPage = nPage + npos;
	if( nDestPage > nLastPage ) nDestPage = nLastPage;
#else
	int nDestPage;
	if(nPage == -1) nDestPage = nLastPage;
	else nDestPage = nPage - 1;
#endif

	int nDestPost = nDestPage * ncnt;

	// ���� �������� ù ����Ʈ �߰�
	POST_LIST::iterator it, tit;
	int n = 0;
	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if(n++ == nDestPost) break;
	}

	if(ncnt == 1)
	{
		if(!RequestHTMLMake((*it), xmlstream))
			return FALSE;

		if(bread && !(*it).read)
		{
			theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
		}

		m_nCurrentReading = (*it).postid;
	}
	else
	{
		int tmpcnt = ncnt;
		BOOL bCR = FALSE;

		tit = it;
		SmartPtr<POST_LIST> aptmplist(new POST_LIST);
		for(; it != m_splistpost->end(); ++it)
		{
			if(tmpcnt-- == 0) break;

			aptmplist->push_back((*it));
			if(!bCR)
			{
				m_nCurrentReading = (*it).postid;
				bCR = TRUE;
			}
		}

		if(!RequestHTMLMake(aptmplist, xmlstream, FALSE))
			return FALSE;

		if(bread)
		{
			it = tit;
			tmpcnt = ncnt;
			for(; it != m_splistpost->end(); ++it)
			{
				if(tmpcnt-- == 0) break;
				if(!(*it).read)
				{
					theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
				}
			}
		}
	}

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = FALSE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetFirstPost
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-13
 *
 * @Description
 * ���� ����� ������ ����Ʈ�� �о�´�.
 *
 * @Parameters
 * (in int) chid - �˻��� ä��
 **************************************************************************/
BOOL CRSSCurrentList::GetFirstPost(CString& xmlstream, BOOL bread)
{
	if(m_splistpost->size() == 0) return FALSE;

	POST_LIST::iterator it = m_splistpost->begin();

	if(!RequestHTMLMake((*it), xmlstream))
		return FALSE;

	if(bread && !(*it).read)
	{
		theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
	}

	m_nCurrentReading = (*it).postid;

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = TRUE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::GetLastPost
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-13
 *
 * @Description
 * ���� ����� ������ ����Ʈ�� �о�´�.
 *
 * @Parameters
 * (in int) chid - �˻��� ä��
 **************************************************************************/
BOOL CRSSCurrentList::GetLastPost(CString& xmlstream, BOOL bread)
{
	if(m_splistpost->size() == 0) return FALSE;

	POST_LIST::reverse_iterator it = m_splistpost->rbegin();

	if(!RequestHTMLMake((*it), xmlstream))
		return FALSE;

	if(bread && !(*it).read)
	{
		theApp.m_spRD->IPostRead((*it).postid, (*it).channelid);
	}

	m_nCurrentReading = (*it).postid;

	m_splistdlg->List_Select(m_nCurrentReading);

	m_bListEnd = TRUE;

	return TRUE;
}

/**************************************************************************
 * method CRSSCurrentList::IsCurrentChannel
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * ���� ��ϵ� ä�� ��� �� �־��� ä�� id�� ���� ä���� ������ �����Ѵ�.
 *
 * @Parameters
 * (in int) chid - �˻��� ä��
 **************************************************************************/
BOOL CRSSCurrentList::IsCurrentChannel(int chid)
{
	list<int>::iterator it;

	for(it = m_listCurrentChannel.begin(); it != m_listCurrentChannel.end(); ++it)
	{
		if((*it) == chid) return TRUE;
	}

	return FALSE;
}

/**************************************************************************
 * method CRSSCurrentList::UpdatePostList
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * ���� ��ϰ� ���Ͽ� ���ŵ� ����Ʈ�� ������ �������ش�.
 *
 * @Parameters
 * (in POSTITEM&) item - ���ŵ� ����Ʈ�� ���
 **************************************************************************/
BOOL CRSSCurrentList::UpdatePost(POSTITEM& item, int updateflag, BOOL bredraw)
{
	ASSERT(m_splistdlg.get());

	POST_LIST::iterator it;
	BOOL updated = FALSE;

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		if((*it).postid == item.postid)
		{
			// ���� ���� �����
			if(updateflag & FISH_ITEM_READ)
			{
				if((*it).read != item.read)
				{
					if(item.read)	// ������ �� -> ���� ��
						--m_nunreadcnt;
					else
						++m_nunreadcnt;
				}
			}

			if(updateflag == FISH_ITEM_ALL)
			{
				(*it) = item;
				(*it).pubdate += theApp.m_spLP->GetSPGMT();
				updated = TRUE;
			}
			else
			{
				if(updateflag & FISH_ITEM_READ)
				{
					(*it).read = item.read;
//					(*it).svread = item.svread;
				}

				if(updateflag & FISH_ITEM_READON)
				{
					(*it).readon = item.readon;
//					(*it).svreadon = item.svreadon;
				}

				if(updateflag & FISH_ITEM_SEARCHVALUE)
				{
					(*it).svread = item.svread;
					(*it).svreadon = item.svreadon;
					(*it).svscrap = item.svscrap;
					(*it).svfilter = item.svfilter;
					(*it).svhit = item.svhit;
					TRACE(_T("Search Value Changed : r[%d], ro[%d], s[%d], f[%d], h[%d]\r\n")
						, item.svread
						, item.svreadon
						, item.svscrap
						, item.svfilter
						, item.svhit
						);
				}
			}

			m_splistdlg->List_ItemUpdate((*it));
			break;
		}
	}

	return updated;
}

/**************************************************************************
 * method CRSSCurrentList::UpdatePostList
 *
 * written by moonknit
 *
 * @history
 * created 2006-01-12
 *
 * @Description
 * ���� ��ϰ� ���Ͽ� ���ŵ� ����Ʈ�� ������ �������ش�.
 *
 * @Parameters
 * (in/out SmartPtr<POST_LIST>&) aplist - ������ ����Ʈ ���
 **************************************************************************/
BOOL CRSSCurrentList::UpdatePostList(auto_ptr<POST_LIST>& aplist, int updateflag, BOOL bredraw)
{
	ASSERT(aplist.get());
	BOOL updated = FALSE;

	int oldunreadcnt = m_nunreadcnt;

	POST_LIST::iterator it;
	for(it = aplist->begin(); it != aplist->end(); ++it)
	{
		if(UpdatePost((*it), updateflag))
			updated = TRUE;
		
	}

	if(oldunreadcnt != m_nunreadcnt)
	{
		::SendMessage(m_hPostCntHwnd, WM_INFORMCURRENTPOSTS, (WPARAM) m_npostcnt, (LPARAM) m_nunreadcnt);
	}

	if(updated && bredraw && m_nCurrentReading == 0)
	{
		SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);
	}

	return updated;
}

/************************************************************************
SetListMode ����Ʈ�� ǥ����带 �����Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/02/16:CREATED
************************************************************************/
VOID CRSSCurrentList::SetListMode(BOOL bShowSrchStyle)
{
    m_splistdlg->List_Mode(bShowSrchStyle);
    return;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 *
 * @Description
 * Html ����⸦ ��û�԰� ���ÿ� �ӽ� ��Ͽ� ������ �д�.
 *
 * @Parameters
 * (in POSTITEM&) post - Html�� ���鶧 ����� ����Ʈ ����ü
 * (out CString&) xmlstream - Html�� ������ �����
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(POSTITEM& post, CString& xmlstream)
{
	m_spviewposts->clear();
	BOOL bresult;
	m_spviewposts->push_back(post);
	m_bSinglePost = TRUE;
	bresult = CHtmlMaker::MakeHtml_Xslt(post, xmlstream, theApp.m_spLP->GetStyleFile());
	m_bViewReverse = FALSE;
	m_bUseChannelPage = FALSE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 * updated 2006-03-21 :: MakeHtml_Xslt �Լ� �������� ���� ���� by aquarelle
 *
 * @Description
 * Html ����⸦ ��û�԰� ���ÿ� �ӽ� ��Ͽ� ������ �д�.
 *
 * @Parameters
 * (in SmartPtr<POST_LIST>&) aplist - Html�� ���鶧 ����� ����Ʈ ���
 * (out CString&) xmlstream - Html�� ������ �����
 * (in BOOL) breserse - Html�� ������ �� ����� ������ html�� ������ �� �ʿ��� ����
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(SmartPtr<POST_LIST> &aplist, CString& xmlstream, BOOL breverse)
{
	m_spviewposts->clear();
	BOOL bresult;
	m_bSinglePost = FALSE;
	m_spviewposts->insert(m_spviewposts->end(), aplist->begin(), aplist->end());

	DWORD flag = HM_LOADLOCAL | (!breverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);
	
	bresult = CHtmlMaker::MakeHtml_Xslt(aplist, xmlstream, theApp.m_spLP->GetStyleFile(), flag);

	m_bViewReverse = breverse;
	m_bUseChannelPage = FALSE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::RequestHTMLMake
 *
 * written by moonknit
 *
 * @history
 * created 2006-03-20
 *
 * @Description
 * m_splistpost���̿��� Html �����.
 *
 * @Parameters
 * (out CString&) xmlstream - Html�� ������ �����
 * (in BOOL) breserse - Html�� ������ �� ����� ������ html�� ������ �� �ʿ��� ����
 **************************************************************************/
BOOL CRSSCurrentList::RequestHTMLMake(CString& xmlstream, BOOL breverse)
{
	BOOL bresult;
	m_spviewposts->clear();
	m_bSinglePost = FALSE;

	DWORD flag = HM_LOADLOCAL | (!breverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);
	bresult = CHtmlMaker::MakeHtml_Xslt(m_splistpost, xmlstream, theApp.m_spLP->GetStyleFile(), flag);
	m_bViewReverse = breverse;
	m_bUseChannelPage = TRUE;

	return bresult;
}

/**************************************************************************
 * method CRSSCurrentList::Refresh
 *
 * written by moonknit
 *
 * @history
 * created 2006-02-28
 * updated 2006-03-21 :: MakeHtml_Xslt �Լ� �������� ���� ���� by aquarelle
 *
 * @Description
 * ���� ���� �ִ� �ӽ� Post ����� �̿��� Html�� �ٽ� �����Ѵ�.
 *
 * @Parameters
 * (out CString&) xmlstream - Html�� ������ �����
 **************************************************************************/
BOOL CRSSCurrentList::Refresh(CString& xmlstream)
{
	BOOL bresult;

	DWORD flag = HM_LOADLOCAL | (!m_bViewReverse ? HM_NOTREVERSE : 0) | (!theApp.m_spLP->GetListDescSummary() ? HM_DESCSUMMARY : 0);

	if(m_spviewposts->size() > 0)
	{
		if(m_bSinglePost)
			bresult = CHtmlMaker::MakeHtml_Xslt(m_spviewposts->front(), xmlstream, theApp.m_spLP->GetStyleFile(), flag);
		else
			bresult = CHtmlMaker::MakeHtml_Xslt(m_spviewposts, xmlstream, theApp.m_spLP->GetStyleFile(), flag);
	}
	else
		bresult = CHtmlMaker::MakeHtml_Xslt(m_splistpost, xmlstream, theApp.m_spLP->GetStyleFile(), flag);

	return bresult;
}





BOOL CRSSCurrentList::SavePostCurrent(CString filename)
{

	if(!m_spviewposts->empty())
	{
		//CString buffer;
		/*
		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_POSTSAVE_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);
		*/

		CHtmlMaker htmlmaker;
		//if(!htmlmaker.MakeHtml(m_spviewposts, buffer, HM_LOADSTRING | HM_PAPERSTYLE))
		if(!htmlmaker.MakeHtml(m_spviewposts, theApp.m_spLP->GetPaperStyle(), HM_LOADLOCAL | HM_PAPERSTYLE))
			return FALSE;
		if(!htmlmaker.Save(filename))
			return FALSE;

		return TRUE;
	}
	else
		return FALSE;	
}

BOOL CRSSCurrentList::SavePost(list<int> ids, CString filename)
{
	if(ids.size() == 0) return FALSE;

	POST_LIST::iterator it;
	list<int>::iterator it2;
	SmartPtr<POST_LIST> aplist (new POST_LIST);

	for(it = m_splistpost->begin(); it != m_splistpost->end(); ++it)
	{
		it2 = FindIntItem(ids, (*it).postid);
		if(it2 != ids.end())
		{
			aplist->push_back(*it);

			ids.erase(it2);
		}
	}

	if(!aplist->empty())
	{
		/*
		CString buffer;
		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_POSTSAVE_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		*/

		CHtmlMaker htmlmaker;
		
		DWORD flag = HM_LOADLOCAL | HM_PAPERSTYLE;
		if(!htmlmaker.MakeHtml(aplist, theApp.m_spLP->GetPaperStyle(), flag))
			return FALSE;
		if(!htmlmaker.Save(filename))
			return FALSE;

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CRSSCurrentList::RequestChannelDelete(int channelid)
{
	if(IsCurrentChannel(channelid))
	{
		m_listRemainChannel.clear();
		m_listCurrentChannel.clear();

		m_splistpost->clear();
		m_spviewposts->clear();

		m_nCurrentReading = 0;
		m_splistdlg->List_Clear();

		m_splistdlg->Invalidate(FALSE);

		::SendMessage(m_hParent, WM_VIEW_CURRENTLIST, 0, 0);

		return TRUE;
	}

	return FALSE;
}