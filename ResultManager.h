// ResultManager.h: interface for the CResultManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTMANAGER_H__C6ACFE83_789E_4233_AC72_0DBFBD9FB4AD__INCLUDED_)
#define AFX_RESULTMANAGER_H__C6ACFE83_789E_4233_AC72_0DBFBD9FB4AD__INCLUDED_

/**************************************************************************
 * class CResultManager
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-12-xx
 *
 * @Description
 * RSSDB의 처리 결과를 관리하기 위한 클래스이다.
 **************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ResultSearch;
struct ResultPost;
struct ResultNormal;
struct ResultAddPost;
struct ResultUpload;

class CResultManager  
{
protected:
	static void RP_Search(ResultSearch* psearch);
	static void RP_ChannelGet(ResultPost* ppost);
	static void RP_PostGet(ResultPost* ppost);
	static void RP_PostUpdate(ResultPost* ppost);
	static void RP_PostDelete(ResultNormal* pnormal);
	static void RP_ChannelAdd(ResultNormal* pnormal);
	static void RP_ChannelDelete(ResultNormal* pnormal);
	static void RP_ChannelLoad(ResultNormal* pnormal);
	static void RP_ChannelFlush(ResultNormal* pnormal);
	static void RP_PostAdd(ResultAddPost* ppostadd);
	static void RP_Upload(ResultUpload* pupload);

public:
	static void ProcessResult();
	CResultManager();
	virtual ~CResultManager();

};

#endif // !defined(AFX_RESULTMANAGER_H__C6ACFE83_789E_4233_AC72_0DBFBD9FB4AD__INCLUDED_)
