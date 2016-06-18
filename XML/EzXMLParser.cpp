// EzXMLParser.cpp: implementation of the CEzXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EzXMLParser.h"
#include "afxinet.h"

#include "../RSSCurrentList.h"
#include "../fish_common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define XML_NONTHREADED

BOOL LoadNamespace(IXMLDOMDocument2Ptr pXMLDoc);
BOOL GetXmlFromWeb(CString url, CString& xmlStream);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEzXMLParser::CEzXMLParser()
{
	Init();
}

CEzXMLParser::CEzXMLParser( TCHAR* strPath, int iLoadFlag )
{
	Init();
	LoadXML(strPath, iLoadFlag);	
}

CEzXMLParser::~CEzXMLParser()
{
	if(m_pXMLDoc != NULL)	
	{
		m_pXMLDoc.Release();
		m_pXMLDoc = NULL;
	}

//	TRACE(_T("CEzXMLParser Destroy\r\n"));
}

void CEzXMLParser::Init()
{
	m_strXMLSavePath[0] = 0L;
	m_iLoadFlag = LOAD_LOCAL;
	m_pXMLDoc = NULL;
}

//////////////////////////////////////////////////
// NAME : GetRootNode
// PARM : void
// RETN : Root Node Point
// DESC : return Root NodePtr
// Date : 2005-05-20 coded by happyune, origin
//////////////////////////////////////////////////
IXMLDOMNodePtr CEzXMLParser::GetRootNode()
{
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::GetRootNode() => 문서가 로드되지 않았습니다."));
		return NULL;
	}

	return m_pXMLDoc;
}


//////////////////////////////////////////////////
// NAME  : LoadXML
// PARAM : TCHAR* strpath - 로드할 xml문서 path, BOOL bHttp - xml문서의 위치 http(=TRUE) or local(=FALSE)
// RETN  : TRUE/FALSE
// DESC  : xml 문서를 DOM 형태로 메모리에 로드한다.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::LoadXML(TCHAR* strPath, int iLoadFlag /* = LOAD_LOCAL */)
{
	// xml file path String이 비었는지 검사
	if( strPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::LoadXML => 로드 경로가 지정되지 않았습니다."));
		ASSERT(FALSE);
		return FALSE;
	}
		
	m_iLoadFlag = iLoadFlag;	
	if( m_iLoadFlag == LOAD_LOCAL )
	{
		_sntprintf(m_strXMLSavePath, _tcslen(strPath)+1, strPath);	
	}
	
	int iRet = 0;				// return할 결과
	_variant_t varOut((bool)TRUE);
		
	try
	{
		if(m_pXMLDoc != NULL)	// 만약 DomDocument 객체가 빈객체(?)가 아니면..release
		{
			m_pXMLDoc.Release();	
			m_pXMLDoc = NULL;
		}
		
		switch(iLoadFlag) 
		{
			case LOAD_LOCAL:	// local의 XML File을 load
			case LOAD_STRING:	// String을 XML로 load
			{
				
				// xmldocument object 생성 
#ifdef XML_NONTHREADED
				ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
				ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

				ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc에게 asynchronously하게 load하지 않을 것이라고 알림				

				int n = _tcslen(strPath);
				// xml file load
				if( iLoadFlag == LOAD_LOCAL )
				{
					varOut = m_pXMLDoc->load(_bstr_t(strPath));
				}				
				else if( iLoadFlag == LOAD_STRING )
				{
					varOut = m_pXMLDoc->loadXML(_bstr_t(strPath));			
				}

				if((bool)varOut == false)	// xml file이 load되지 않았을 경우 Err처리
				{
					throw(0);
				}

				break;
			}
			case LOAD_WEB:		// http를 통해 XML File을 load
			{
							
				//XMLHTTP를 위한 변수
				IXMLHTTPRequestPtr pIXMLHTTPRequest = NULL;
			
				// xmlhttp object 생성
				ErrorTestHR(pIXMLHTTPRequest.CreateInstance(__uuidof(XMLHTTP40), NULL, CLSCTX_ALL));				
				//ErrorTestHR(pIXMLHTTPRequest.CreateInstance(__uuidof(XMLHTTP40)));
				ErrorTestHR(pIXMLHTTPRequest->open(_T("GET"), strPath, false));
				try
				{
					ErrorTestHR(pIXMLHTTPRequest->send());
				}
				catch(_com_error&)
				{
					// URL Redirection Check					
					CInternetSession session(_T(""));
					CHttpConnection* pServer = NULL;
					CHttpFile* pFile = NULL;
					CString strNewLocation;
					
					DWORD dwRet;
					DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
					DWORD dwSearviceType;
					CString servername;
					CString strObject;
					INTERNET_PORT nPort;

					if(AfxParseURL(strPath, dwSearviceType, servername, strObject, nPort))
					{
						pServer = session.GetHttpConnection(servername, nPort);					
						pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
						
						try
						{
							pFile->SendRequest();
						}catch(CInternetException* e)
						{
							TCHAR errMsg[1024];
							e->GetErrorMessage(errMsg, 1024);
							TRACE(_T("Connection Error : %s"), errMsg);

							pFile->Close();
							delete pFile;
							pServer->Close();
							delete pServer;
							session.Close();

							throw (int)1;
						}
						catch(...)
						{
							TRACE(_T("Connection Error\r\n"));

							pFile->Close();
							delete pFile;
							pServer->Close();
							delete pServer;
							session.Close();

							throw (int)2;
						}

						pFile->QueryInfoStatusCode(dwRet);
						if (dwRet == HTTP_STATUS_MOVED || dwRet == HTTP_STATUS_REDIRECT || dwRet == HTTP_STATUS_REDIRECT_METHOD)
						{						
							pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
							int nPlace = strNewLocation.Find(_T("Location: "));
							if (nPlace == -1)
							{
								strNewLocation.Empty();
							}
							else
							{
								strNewLocation = strNewLocation.Mid(nPlace + 10);
								nPlace = strNewLocation.Find('\n');

								if (nPlace > 0)
									strNewLocation = strNewLocation.Left(nPlace);

								TRACE(_T("redirect url : %s\r\n"), strNewLocation);
								if(!AfxParseURL(strPath, dwSearviceType, servername, strObject, nPort))
									strNewLocation.Empty();
							}
						}
					}
					else
						strNewLocation.Empty();
					
					pFile->Close();
					delete pFile;
					pServer->Close();
					delete pServer;
					session.Close();

					if(!strNewLocation.IsEmpty())
					{
						pIXMLHTTPRequest->abort();
						return LoadXML((LPTSTR)(LPCTSTR)strNewLocation , LOAD_WEB);
					}
					else					
						return FALSE;
				}

				LONG lStatus = 0;
				pIXMLHTTPRequest->get_status(&lStatus);				
								
				if(lStatus >= 500)	// An error occurred on the server.
				{
					TRACE(_T("\nStatus text: An error occurred on the server.\n"));
				}								
				else if (lStatus == 200)	// The method request was successful.
				{
					// xmldocument object 생성 			
#ifdef XML_NONTHREADED
					ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
					ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

					ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc에게 asynchronously하게 load하지 않을 것이라고 알림
					
					// xml file load
					m_pXMLDoc = pIXMLHTTPRequest->responseXML;
					
					// code added by aquarelle 2006/02/08
					// xml파일을 로드하고 IXMLDOMDocument 형식으로 변환(?)하지 못하는 경우의 처리..
					if(m_pXMLDoc->xml.length() == 0)
					{	
						_bstr_t text = pIXMLHTTPRequest->responseText;

						if(!LoadXML((LPTSTR)(LPCTSTR) text, LOAD_STRING))
						{						
							CString xmlStream;

							// IXMLHTTPRequest에서 파일을 제대로 못받아올경우 처리...
							// IXMLHTTPRequest... 사용하지 말아야하나...
							if(GetXmlFromWeb(strPath, xmlStream))
							{
								MakeStreamToFile(_T("e:\\test.xml"), xmlStream);
								LoadXML((LPTSTR)(LPCTSTR) xmlStream, LOAD_STRING);
							}
						}
					}					
				}				
				else
				{
					BSTR bstrResp;
					BSTR bstrResponseText;

					// Display the response status text.
					pIXMLHTTPRequest->get_statusText(&bstrResp);
					// Display the response text.
					pIXMLHTTPRequest->get_responseText(&bstrResponseText);

					//CString strTemp = _T("");
					//strTemp.Format(_T("\nStatus text: %s\nResponse text: %s"), (char*)(_bstr_t)bstrResp, (char*)(_bstr_t)bstrResponseText);
					
					SysFreeString(bstrResp);
					SysFreeString(bstrResponseText);
				}
				
				
				
				/*
				CString xmlStream;				
				if(GetXmlFromWeb(strPath, xmlStream))
				{
					TRACE(xmlStream.Right(10));
					MakeStreamToFile(_T("e:\\downxml.xml"), xmlStream);
					LoadXML((LPTSTR)(LPCTSTR)xmlStream, LOAD_STRING);
				}
				*/				

				break;
			}						
			default:
			{
				return FALSE;
			}
		}

		// Property Setting -- namespace
		// modified by aquarelle
		if(m_pXMLDoc != NULL)
		{
			LoadNamespace(m_pXMLDoc);
		}

	}
	catch(_com_error& e)
	{
		PrintComError(e);

		return FALSE;
	}
	catch(int e)
	{
		// xml file 로드 실패 에러 핸들링 
		if(e==0)
		{
			TRACE(_T("\n[ERR by UNE] CXMLParser::LoadXML => Can't load local xml!\r\n"));
			return FALSE;
		}
		if(e==1)
		{
			TRACE(_T("\n[ERR by UNE] CXMLParser::LoadXML => Can't load remote(web) xml!\r\n"));
			return FALSE;
		}
	}
	
	if(m_pXMLDoc != NULL && (m_pXMLDoc->xml).length() != 0)
		return TRUE;

	//TRACE(_T("LOAD XML FROM FILE RESULT : %d : ErrCode [%d]\r\n"), ((bool) varOut) ? 1 : 0, GetLastError());

	return FALSE;
}

//////////////////////////////////////////////////
// NAME  : SaveXML
// PARAM : TCHAR* strPath
// RETN  : TRUE/FALSE
// DESC  : xml을 로컬 파일에 저장한다.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::SaveXML(TCHAR* strPath)
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SaveXML => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return FALSE;
			
	}	

	switch(m_iLoadFlag) 
	{
		case LOAD_LOCAL:
		case LOAD_STRING:
		{
			if( strPath != NULL )
			{
				_sntprintf(m_strXMLSavePath, _tcslen(strPath)+1, strPath);
			}
			
			try
			{
				ErrorTestHR(m_pXMLDoc->save(_bstr_t(m_strXMLSavePath)));
			}
			catch(_com_error& e)
			{
				PrintComError(e);
				return FALSE;
			}

			break;
		}
		case LOAD_WEB:
		{
			// 흠..지금으로썬.. 로컬쪽에서 서버쪽으로의 저장은.. 힘든듯..
			// 머 여러가지 상황이 있을수 있겠지만.. 보안 문제(파일을 쓸수 있는가..)도 그렇고.. 
			// 서버쪽에서 보내주는 xml문서또한 파일로서 존재한다고 보기 힘든 경우가 있기 때문에( server side script인 경우가 많아서..)
			// http를 통해 로드된 문서의 저장은.. 보류..
			ErrorTestHR(m_pXMLDoc->save(_bstr_t(strPath)));
			return FALSE;
		}		
		default:
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////
// NAME		: CreateXML
// CREATED	: 2005-10-25 written by moonknit
// PARAM	: NULL
// RETN		: BOOL
// DESC		: 비어 있는 XML을 생성한다.
// LAST UPDATED : -
//////////////////////////////////////////////////
BOOL CEzXMLParser::CreteXML()
{
	// xmldocument object 생성 					
#ifdef XML_NONTHREADED
	ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
	ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

	ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc에게 asynchronously하게 load하지 않을 것이라고 알림				
	
	return TRUE;
}

//////////////////////////////////////////////////
// NAME  : CreateElement
// PARAM : CString strTagName - Tag name
// RETN  : 생성된 Element object
// DESC  : Element object 생성
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMElementPtr CEzXMLParser::CreateElement( TCHAR* strTagName )
{
	if( strTagName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateElement => Tag name이 지정되어 있지 않습니다."));
		ASSERT(FALSE);
		return NULL;
	}

	IXMLDOMElementPtr pNewElement = NULL;

	try
	{
		if( m_pXMLDoc == NULL )
		{
			IXMLDOMDocument2Ptr pXMLDoc = NULL;
#ifdef XML_NONTHREADED
			ErrorTestHR(pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
			ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

			pNewElement = pXMLDoc->createElement(_bstr_t(strTagName));
		}
		else
		{
			pNewElement = m_pXMLDoc->createElement(_bstr_t(strTagName));
		}
		
	}
	catch(_com_error& e) 
	{
		PrintComError(e);
		pNewElement = NULL;
	}

	return pNewElement;
}

//////////////////////////////////////////////////
// NAME  : CreateAttribute
// PARAM : TCHAR* strAttrName - 속성 이름 , TCHAR* strValue - 속성 값
// RETN  : 생성된 Attrubute object
// DESC  : Attribute object 생성
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMAttributePtr CEzXMLParser::CreateAttribute( TCHAR* strAttrName, TCHAR* strValue /* = NULL */ )
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateAttribute => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return NULL;	
	}

	if( strAttrName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateAttribute => Attribute name이 지정되어 있지 않습니다."));
		ASSERT(FALSE);
		return NULL;
	}	
	
	IXMLDOMAttributePtr pNewAttr = NULL;
	
	try
	{
		pNewAttr = m_pXMLDoc->createAttribute(_bstr_t(strAttrName));
		if( strValue != NULL )
		{
			pNewAttr->value = _bstr_t(strValue);
		}		
	}
	catch(_com_error& e) 
	{
		PrintComError(e);
		pNewAttr = NULL;
	}
	
	return pNewAttr;
}

//////////////////////////////////////////////////
// NAME  : AddAttribute
// PARAM : IXMLDOMElementPtr pElementPtr - element pointer, IXMLDOMAttributePtr pAttrPtr - attr pointer
// RETN  : TRUE/FALSE
// DESC  : 지정된 element에 attribute를 추가 한다. 
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AddAttribute( IXMLDOMElementPtr pElementPtr, IXMLDOMAttributePtr pAttrPtr )
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return FALSE;	
	}
	
	if( pElementPtr == NULL || pAttrPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => 추가할 element객체가 NULL입니다."));
		ASSERT(FALSE);
		return FALSE;
	}

	try
	{
		ErrorTestHR(pElementPtr->setAttributeNode(pAttrPtr));
	}
	catch(_com_error& e) 
	{
		PrintComError(e);		
		return FALSE;
	}
	
	return TRUE;	
}

//////////////////////////////////////////////////
// NAME  : AddAttribute
// PARAM : IXMLDOMElementPtr pElementPtr - element pointer, TCHAR* strAttrName - attr name, TCHAR* strAttrValue - attr value
// RETN  : TRUE/FALSE
// DESC  : 지정된 element에 attribute를 추가 한다. 
//		   attr 객체를 따로 생성하지 않아도 해당 name의 attr이 없으면 생성하여 추가 한다. 
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AddAttribute( IXMLDOMElementPtr pElementPtr, TCHAR* strAttrName, TCHAR* strAttrValue /* = NULL */ )
{
	if( strAttrName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => 추가할 속성의 이름이 지정되어 있지 않습니다."));
		ASSERT(FALSE);
		return FALSE;
	}
	
	try
	{
		ErrorTestHR(pElementPtr->setAttribute( _bstr_t(strAttrName), _bstr_t(strAttrValue) ));
	}
	catch(_com_error& e) 
	{
		PrintComError(e);		
		return FALSE;
	}
	
	return TRUE;

}

//////////////////////////////////////////////////
// NAME  : AppendChild
// PARAM : IXMLDOMElementPtr pChildPtr - 추가할 자식 객체, IXMLDOMNodePtr pParentPtr - 부모 객체(NULL이면 루트 객체에 추가한다.)
// RETN  : TRUE/FALSE
// DESC  : 지정된 부모 노드에 자식 노드를 마지막에 붙인다.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AppendChild( IXMLDOMElementPtr pChildPtr, IXMLDOMNodePtr pParentPtr )
{
	try
	{
		if( pChildPtr == NULL )
		{		
			TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => 추가될 자식 노드가 NULL입니다."));
			ASSERT(FALSE);
			return NULL;
		}

		if( pParentPtr == NULL )
		{
			// XML이 로드되어 attach되어 있는지 검사
			if( m_pXMLDoc == NULL )
			{
				TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => XML 문서가 로드되어 있지 않습니다."));
				ASSERT(FALSE);
				return NULL;	
			}		

			pParentPtr = m_pXMLDoc->documentElement;
			if( pParentPtr == NULL )
			{
				TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => root element 얻기 실패"));
				ASSERT(FALSE);
				return NULL;
			}
		}

		IXMLDOMNodePtr pResultPtr = NULL;
	
		ErrorTestHR(pResultPtr = pParentPtr->appendChild( pChildPtr ));

		return pResultPtr;
	}
	catch(_com_error& e) 
	{
		PrintComError(e);		
		return NULL;
	}
	catch(...)
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => COM 이외 예외상황"));
		return NULL;
	}
}

//////////////////////////////////////////////////
// NAME  : SearchNodes
// PARAM : TCHAR* strXPath - XPATH
// RETN  : 결과 NodeList pointer
// DESC  :  XPath 질의를 통한 검색
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMNodeListPtr CEzXMLParser::SearchNodes(TCHAR* strXPath)
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return NULL;	
	}

	if( strXPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XPATH값이 지정되지 않았습니다."));
		ASSERT(FALSE);
		return NULL;
	}
	
	IXMLDOMNodeListPtr pResultNodeListPtr;
	
	try
	{
		_bstr_t path(strXPath);	
		pResultNodeListPtr = m_pXMLDoc->selectNodes(path);
	
	}catch(_com_error& e)
	{
		PrintComError(e);		
	}

	if( pResultNodeListPtr == NULL )
	{
		TRACE(_T("%s\nSearchNodes 결과가 NULL!!!!"), strXPath);	
	}
	
	return pResultNodeListPtr;
	
}

//////////////////////////////////////////////////
// NAME  : SearchNodes
// PARAM : 
// RETN  : 결과 NodeList pointer
// DESC  : XPath 질의를 통한 검색(xpath를 생성해준다.)
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMNodeListPtr	CEzXMLParser::SearchNodes(TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName /* = NULL */)
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return FALSE;	
	}

	if( strElementName == NULL || strValue == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => name, value 값이 지정되지 않았습니다."));
		ASSERT(FALSE);
		return NULL;
	}

	CString strXPath = _T("");

	if( bElement == TRUE )
	{
		strXPath = _T("//");
		strXPath += strElementName;
		strXPath += _T("=\'");
		strXPath += strValue;
		strXPath += _T("\'");
	}
	else
	{
		strXPath = _T("//");
		strXPath += strElementName;
		strXPath += _T("[@");
		strXPath += strAttrName;
		strXPath += _T("=\'");
		strXPath += strValue;
		strXPath += _T("\']");
	}

	return SearchNodes((LPTSTR)(LPCTSTR)strXPath);
}

//////////////////////////////////////////////////
// NAME : SearchNodes
// PARM : IXMLDOMNodePtr pNodePtr, TCHAR* strXPath
// RETN : IXMLDOMNodeListPtr
// DESC : 인자의 Node Point에 XPath를 적용한 결과를 생성
// Date : 2005-05-19 coded by happyune, origin
//////////////////////////////////////////////////
IXMLDOMNodeListPtr	CEzXMLParser::SearchNodes( IXMLDOMNodePtr pNodePtr, TCHAR* strXPath )
{
	if( pNodePtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::SearchNodes => 지정된 Node pointer가 NULL입니다."));
		ASSERT(FALSE);
		return NULL;
	}
	
	if( strXPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::SearchNodes => XPATH값이 지정되지 않았습니다."));
		ASSERT(FALSE);
		return NULL;
	}
	
	IXMLDOMNodeListPtr pResultNodeListPtr;
	
	try
	{		
		pResultNodeListPtr = pNodePtr->selectNodes(_bstr_t(strXPath));
		
	}catch(_com_error& e)
	{
		PrintComError(e);		
	}
	
	return pResultNodeListPtr;
}

//////////////////////////////////////////////////
// NAME  : RemoveNodes
// PARAM : IXMLDOMNodePtr pNodePtr - 삭제될 노드 포인터
// RETN  : TRUE/FALSE
// DESC  : 지정된 노드 포인트를 삭제한다.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveNodes(IXMLDOMNodePtr pNodePtr)
{
	// XML이 로드되어 attach되어 있는지 검사
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => XML 문서가 로드되어 있지 않습니다."));
		ASSERT(FALSE);
		return FALSE;	
	}

	if(pNodePtr == NULL)
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => 삭제될 노드 포인트 NULL"));
		ASSERT(FALSE);
		return FALSE;
	}
	
	IXMLDOMNodePtr pRemoveNodePtr = NULL;
	IXMLDOMNodePtr pParentNodePtr = NULL;
	
	try
	{
		pParentNodePtr = pNodePtr->GetparentNode();
		
		if(pParentNodePtr != NULL)
		{
			pRemoveNodePtr = pParentNodePtr->removeChild(pNodePtr);
			if( pRemoveNodePtr == NULL )
			{
				TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => 삭제도중 오류"));
				ASSERT(FALSE);
				return FALSE;
			}
			pRemoveNodePtr = NULL;
		}
		else 
		{
			TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => 부모 노드 포인트 획득 실패"));
			ASSERT(FALSE);
			return FALSE;
		}		
	}
	catch(_com_error& e)
	{
		PrintComError(e);
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////
// NAME  : RemoveNodes
// PARAM : TCHAR* strElementName - element name, TCHAR* strValue - value, 
//		   BOOL bElement - elemen(=TRUE) or attr(=FALSE), TCHAR* strAttrName - attr name
// RETN  : TRUE/FALSE
// DESC  : 해당 조건을 만족하는 노드를 찾아 삭제 한다.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveNodes(TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName /* = NULL */ )
{
	IXMLDOMNodeListPtr pNodeListPtr = NULL;
	pNodeListPtr = 	SearchNodes( strElementName, strValue, bElement, strAttrName );

	if( pNodeListPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => 해당 값의 노드 검색 실패"));
		ASSERT(FALSE);
		return FALSE;
	}
	
	int iListCnt = 0;
	iListCnt = pNodeListPtr->Getlength();

	for( int cnt = 0; cnt < iListCnt; cnt ++)
	{
		if( RemoveNodes(pNodeListPtr->item[cnt]) == FALSE )
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////
// NAME : RemoveAllTagNodes
// PARM : TCHAR* pTagName - tag name (xpath 형식)
// RETN : TRUE/FALSE
// DESC : 지정된 태그의 값을 모두 삭제
// Date : 2005-06-14 coded by happyune, origin
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveAllTagNodes(TCHAR* pTagName)
{	
	IXMLDOMNodeListPtr pNodeListPtr = NULL;
	pNodeListPtr = 	SearchNodes( pTagName );
	
	if( pNodeListPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => 해당 값의 노드 검색 실패"));
		ASSERT(FALSE);
		return FALSE;
	}
	
	int iListCnt = 0;
	iListCnt = pNodeListPtr->Getlength();
	
	for( int cnt = 0; cnt < iListCnt; cnt ++)
	{
		if( RemoveNodes(pNodeListPtr->item[cnt]) == FALSE )
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}


BOOL CEzXMLParser::Clear()
{
	try
	{
		if( m_pXMLDoc != NULL )
		{
			m_pXMLDoc.Release();		
			m_pXMLDoc = NULL;
		}	
	}
	catch(_com_error& e)
	{
		PrintComError(e);
		return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////
// NAME : 
// PARM : 
// RETN : 
// DESC : 
// Date : 2005-06-14 coded by happyune, origin
//////////////////////////////////////////////////
BOOL CEzXMLParser::ApplyXSLT(TCHAR* strVal)
{
	return TRUE;
}



/*
 *	written by aquarelle
 *	created			2005.11.18. 
 *
 *	XmlDocument namespace Loader
 *
 */
BOOL LoadNamespace(IXMLDOMDocument2Ptr pXMLDoc)
{
	CString nsUrl, tempStr;
	CString ns(_T("xmlns"));
//	HRESULT hr;

	IXMLDOMElementPtr pElement = pXMLDoc->documentElement;
	IXMLDOMNodeListPtr ptrNode = pElement->selectNodes(_T("//*"));
//	IXMLDOMNodeListPtr ptrNode = pXMLDoc->selectNodes(_T("//*"));

	
		
//	TRACE(ptrNode->item[0]->nodeName);

	//Namespace Check
	for(int i=0; ptrNode->item[0]->attributes->item[i] != NULL; i++)
	{
		tempStr.Format(_T("%s"), (LPTSTR) ptrNode->item[0]->attributes->item[i]->nodeName);

		if( ns == tempStr.Left(5) )
		{
			nsUrl += tempStr+ _T("='") + CString((LPTSTR) ptrNode->item[0]->attributes->item[i]->text) + _T("' ");
			//break;
		}
	}


	nsUrl.TrimRight();

	if( nsUrl.GetLength() == 0)
		return FALSE;

	//Namespace Setting
	pXMLDoc->setProperty(_T("SelectionNamespaces"), (LPCTSTR) nsUrl);
	pXMLDoc->setProperty(_T("SelectionLanguage"), _T("XPath"));

	return TRUE;
}


// 임시 코드.....
BOOL GetXmlFromWeb(CString url, CString& xmlStream)
{
	CInternetSession session(_T(""));
	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	
	DWORD dwRet;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
	DWORD dwServiceType;
	CString servername;
	CString strObject;
	INTERNET_PORT nPort;
	CString strNewLocation;

	if(AfxParseURL(url, dwServiceType, servername, strObject, nPort))
	{
		try
		{
			pServer = session.GetHttpConnection(servername, nPort);					
			pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
			
			pFile->SendRequest();	

			pFile->QueryInfoStatusCode(dwRet);

			if(dwRet == HTTP_STATUS_DENIED)
			{
				throw (int) 1;
			}

			if (dwRet == HTTP_STATUS_MOVED || dwRet == HTTP_STATUS_REDIRECT || dwRet == HTTP_STATUS_REDIRECT_METHOD)
			{						
				pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
				int nPlace = strNewLocation.Find(_T("Location: "));
				if (nPlace == -1)
				{
					strNewLocation.Empty();
				}
				else
				{
					strNewLocation = strNewLocation.Mid(nPlace + 10);
					nPlace = strNewLocation.Find('\n');

					if (nPlace > 0)
						strNewLocation = strNewLocation.Left(nPlace);

					TRACE(_T("redirect url : %s\r\n"), strNewLocation);
					if(!AfxParseURL(strNewLocation, dwServiceType, servername, strObject, nPort))
						strNewLocation.Empty();
				}

				pFile->Close();
				delete pFile;
				pFile = NULL;
				pServer->Close();
				delete pServer;
				pServer = NULL;

				if(!strNewLocation.IsEmpty() && AfxParseURL(strNewLocation, dwServiceType, servername, strObject, nPort))
				{
					pServer = session.GetHttpConnection(servername, nPort);
					pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
					pFile->SendRequest();

					pFile->QueryInfoStatusCode(dwRet);

					if(dwRet != HTTP_STATUS_OK)
					{
						throw (int) 2;
					}

					if(dwServiceType != INTERNET_SERVICE_HTTP)
					{
						throw (int) 3;
					}
				}
				else
				{
					throw (int) 4;
				}
			}			

			
			int size = pFile->GetLength();
			char* buff = new char[size + 1];
			ZeroMemory(buff, size + 1);
			
			//pFile->ReadHuge(buff, size + 1);

			char tempbuf[1024];
			ZeroMemory(tempbuf, 1024);
			//CString strbuf;
			int nRead = 0, total = 0;
			
			while((nRead = pFile->Read(tempbuf, 1024)))
			{
				total += nRead;
				if(total > size)
				{
					char* temp = buff;
					buff = new char[strlen(temp) + nRead + 1];
					ZeroMemory(buff, strlen(temp) + nRead + 1);
					if(strlen(temp) > 0)
						strncpy(buff, temp, strlen(temp));
					strncat(buff, tempbuf, nRead);
					delete temp;
				}
				else
					strncat(buff, tempbuf, nRead);
				ZeroMemory(tempbuf, 1024);
			}			

			wchar_t* xml = NULL;
			int Len = 0;


			//USES_CONVERSION;
			//const WCHAR* wStr = T2CW((LPCTSTR)buff);

			// Encoding 정보....
			BOOL b_utf8 = FALSE;
			for(int i = 20; i < 50; i++)
			{				
				if((buff[i] == 'u' || buff[i] == 'U') &&
					(buff[i+1] == 't' || buff[i+1] == 'T') &&
					(buff[i+2] == 'f' || buff[i+2] == 'F') &&
					(buff[i+3] == '-') && (buff[i+4] == '8'))
				{
					TRACE(_T("find encoding : %c%c%c%c%c\r\n"), buff[i], buff[i+1], buff[i+2], buff[i+3], buff[i+4]);
					b_utf8 = TRUE;
					break;
				}			
			}
			
			if(!b_utf8)
			{
				Len = MultiByteToWideChar(CP_ACP, 0, buff, -1, NULL, NULL);
				xml = new wchar_t[Len + 1];
				ZeroMemory(xml, Len + 1);
				MultiByteToWideChar(CP_ACP, 0, buff, -1, xml, Len + 1);
			}
			else
			{
				Len = MultiByteToWideChar(CP_UTF8, 0, buff, -1, NULL, NULL);
				xml = new wchar_t[Len + 1];
				ZeroMemory(xml, Len + 1);
				MultiByteToWideChar(CP_UTF8, 0, buff, -1, xml, Len + 1);
			}
			
			xmlStream = xml;			
			TRACE(_T("Get XML Right 10 : %s,   %c%c%c\r\n"), xmlStream.Right(20), buff[size-3], buff[size-2], buff[size-1]);
			
			/*
			TCHAR buf[1024];
			xmlStream.Empty();
			while(pFile->Read())
			{
				xmlStream += buf;
			}
			*/
			

			//TRACE(xmlStream.Right(10));

			delete [] buff;
			delete [] xml;
		}
		catch(CInternetException* pIE)
		{
			TCHAR errMsg[1024];
			pIE->GetErrorMessage(errMsg, 1024);
			TRACE(_T("Internet Exception : %s\r\n"), errMsg);
			xmlStream.Empty();
		}
		catch(int e)
		{
			if(e == 1)
				TRACE(_T("Service Dinied!\r\n"));
			else if(e == 2 || e == 3 || e == 4)
				TRACE(_T("URL Redirection Error : %d!\r\n"), e);

			xmlStream.Empty();
		}
		catch(...)
		{
			xmlStream.Empty();
		}

		if(pFile != NULL)
		{
			pFile->Close();
			delete pFile;
		}
		if(pServer != NULL)
		{
			pServer->Close();
			delete pServer;
		}
		session.Close();		
	}
	else
		xmlStream.Empty();

	if(xmlStream.IsEmpty())
	{
		return FALSE;
	}
	else
	{		
		return TRUE;
	}
}