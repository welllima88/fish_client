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
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::GetRootNode() => ������ �ε���� �ʾҽ��ϴ�."));
		return NULL;
	}

	return m_pXMLDoc;
}


//////////////////////////////////////////////////
// NAME  : LoadXML
// PARAM : TCHAR* strpath - �ε��� xml���� path, BOOL bHttp - xml������ ��ġ http(=TRUE) or local(=FALSE)
// RETN  : TRUE/FALSE
// DESC  : xml ������ DOM ���·� �޸𸮿� �ε��Ѵ�.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::LoadXML(TCHAR* strPath, int iLoadFlag /* = LOAD_LOCAL */)
{
	// xml file path String�� ������� �˻�
	if( strPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::LoadXML => �ε� ��ΰ� �������� �ʾҽ��ϴ�."));
		ASSERT(FALSE);
		return FALSE;
	}
		
	m_iLoadFlag = iLoadFlag;	
	if( m_iLoadFlag == LOAD_LOCAL )
	{
		_sntprintf(m_strXMLSavePath, _tcslen(strPath)+1, strPath);	
	}
	
	int iRet = 0;				// return�� ���
	_variant_t varOut((bool)TRUE);
		
	try
	{
		if(m_pXMLDoc != NULL)	// ���� DomDocument ��ü�� ��ü(?)�� �ƴϸ�..release
		{
			m_pXMLDoc.Release();	
			m_pXMLDoc = NULL;
		}
		
		switch(iLoadFlag) 
		{
			case LOAD_LOCAL:	// local�� XML File�� load
			case LOAD_STRING:	// String�� XML�� load
			{
				
				// xmldocument object ���� 
#ifdef XML_NONTHREADED
				ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
				ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

				ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc���� asynchronously�ϰ� load���� ���� ���̶�� �˸�				

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

				if((bool)varOut == false)	// xml file�� load���� �ʾ��� ��� Erró��
				{
					throw(0);
				}

				break;
			}
			case LOAD_WEB:		// http�� ���� XML File�� load
			{
							
				//XMLHTTP�� ���� ����
				IXMLHTTPRequestPtr pIXMLHTTPRequest = NULL;
			
				// xmlhttp object ����
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
					// xmldocument object ���� 			
#ifdef XML_NONTHREADED
					ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
					ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

					ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc���� asynchronously�ϰ� load���� ���� ���̶�� �˸�
					
					// xml file load
					m_pXMLDoc = pIXMLHTTPRequest->responseXML;
					
					// code added by aquarelle 2006/02/08
					// xml������ �ε��ϰ� IXMLDOMDocument �������� ��ȯ(?)���� ���ϴ� ����� ó��..
					if(m_pXMLDoc->xml.length() == 0)
					{	
						_bstr_t text = pIXMLHTTPRequest->responseText;

						if(!LoadXML((LPTSTR)(LPCTSTR) text, LOAD_STRING))
						{						
							CString xmlStream;

							// IXMLHTTPRequest���� ������ ����� ���޾ƿð�� ó��...
							// IXMLHTTPRequest... ������� ���ƾ��ϳ�...
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
		// xml file �ε� ���� ���� �ڵ鸵 
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
// DESC  : xml�� ���� ���Ͽ� �����Ѵ�.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::SaveXML(TCHAR* strPath)
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SaveXML => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
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
			// ��..�������ν�.. �����ʿ��� ������������ ������.. �����..
			// �� �������� ��Ȳ�� ������ �ְ�����.. ���� ����(������ ���� �ִ°�..)�� �׷���.. 
			// �����ʿ��� �����ִ� xml�������� ���Ϸμ� �����Ѵٰ� ���� ���� ��찡 �ֱ� ������( server side script�� ��찡 ���Ƽ�..)
			// http�� ���� �ε�� ������ ������.. ����..
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
// DESC		: ��� �ִ� XML�� �����Ѵ�.
// LAST UPDATED : -
//////////////////////////////////////////////////
BOOL CEzXMLParser::CreteXML()
{
	// xmldocument object ���� 					
#ifdef XML_NONTHREADED
	ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(DOMDocument40), NULL, CLSCTX_INPROC_SERVER));
#else
	ErrorTestHR(m_pXMLDoc.CreateInstance(__uuidof(FreeThreadedDOMDocument40), NULL, CLSCTX_ALL));
#endif

	ErrorTestHR(m_pXMLDoc->put_async(VARIANT_FALSE));	// doc���� asynchronously�ϰ� load���� ���� ���̶�� �˸�				
	
	return TRUE;
}

//////////////////////////////////////////////////
// NAME  : CreateElement
// PARAM : CString strTagName - Tag name
// RETN  : ������ Element object
// DESC  : Element object ����
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMElementPtr CEzXMLParser::CreateElement( TCHAR* strTagName )
{
	if( strTagName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateElement => Tag name�� �����Ǿ� ���� �ʽ��ϴ�."));
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
// PARAM : TCHAR* strAttrName - �Ӽ� �̸� , TCHAR* strValue - �Ӽ� ��
// RETN  : ������ Attrubute object
// DESC  : Attribute object ����
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMAttributePtr CEzXMLParser::CreateAttribute( TCHAR* strAttrName, TCHAR* strValue /* = NULL */ )
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateAttribute => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
		ASSERT(FALSE);
		return NULL;	
	}

	if( strAttrName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::CreateAttribute => Attribute name�� �����Ǿ� ���� �ʽ��ϴ�."));
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
// DESC  : ������ element�� attribute�� �߰� �Ѵ�. 
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AddAttribute( IXMLDOMElementPtr pElementPtr, IXMLDOMAttributePtr pAttrPtr )
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
		ASSERT(FALSE);
		return FALSE;	
	}
	
	if( pElementPtr == NULL || pAttrPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => �߰��� element��ü�� NULL�Դϴ�."));
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
// DESC  : ������ element�� attribute�� �߰� �Ѵ�. 
//		   attr ��ü�� ���� �������� �ʾƵ� �ش� name�� attr�� ������ �����Ͽ� �߰� �Ѵ�. 
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AddAttribute( IXMLDOMElementPtr pElementPtr, TCHAR* strAttrName, TCHAR* strAttrValue /* = NULL */ )
{
	if( strAttrName == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::AddAttribute => �߰��� �Ӽ��� �̸��� �����Ǿ� ���� �ʽ��ϴ�."));
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
// PARAM : IXMLDOMElementPtr pChildPtr - �߰��� �ڽ� ��ü, IXMLDOMNodePtr pParentPtr - �θ� ��ü(NULL�̸� ��Ʈ ��ü�� �߰��Ѵ�.)
// RETN  : TRUE/FALSE
// DESC  : ������ �θ� ��忡 �ڽ� ��带 �������� ���δ�.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::AppendChild( IXMLDOMElementPtr pChildPtr, IXMLDOMNodePtr pParentPtr )
{
	try
	{
		if( pChildPtr == NULL )
		{		
			TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => �߰��� �ڽ� ��尡 NULL�Դϴ�."));
			ASSERT(FALSE);
			return NULL;
		}

		if( pParentPtr == NULL )
		{
			// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
			if( m_pXMLDoc == NULL )
			{
				TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
				ASSERT(FALSE);
				return NULL;	
			}		

			pParentPtr = m_pXMLDoc->documentElement;
			if( pParentPtr == NULL )
			{
				TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => root element ��� ����"));
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
		TRACE(_T("\n[ERR by UNE] CXMLParser::AppendChild => COM �̿� ���ܻ�Ȳ"));
		return NULL;
	}
}

//////////////////////////////////////////////////
// NAME  : SearchNodes
// PARAM : TCHAR* strXPath - XPATH
// RETN  : ��� NodeList pointer
// DESC  :  XPath ���Ǹ� ���� �˻�
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMNodeListPtr CEzXMLParser::SearchNodes(TCHAR* strXPath)
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
		ASSERT(FALSE);
		return NULL;	
	}

	if( strXPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XPATH���� �������� �ʾҽ��ϴ�."));
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
		TRACE(_T("%s\nSearchNodes ����� NULL!!!!"), strXPath);	
	}
	
	return pResultNodeListPtr;
	
}

//////////////////////////////////////////////////
// NAME  : SearchNodes
// PARAM : 
// RETN  : ��� NodeList pointer
// DESC  : XPath ���Ǹ� ���� �˻�(xpath�� �������ش�.)
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
IXMLDOMNodeListPtr	CEzXMLParser::SearchNodes(TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName /* = NULL */)
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
		ASSERT(FALSE);
		return FALSE;	
	}

	if( strElementName == NULL || strValue == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::SearchNodes => name, value ���� �������� �ʾҽ��ϴ�."));
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
// DESC : ������ Node Point�� XPath�� ������ ����� ����
// Date : 2005-05-19 coded by happyune, origin
//////////////////////////////////////////////////
IXMLDOMNodeListPtr	CEzXMLParser::SearchNodes( IXMLDOMNodePtr pNodePtr, TCHAR* strXPath )
{
	if( pNodePtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::SearchNodes => ������ Node pointer�� NULL�Դϴ�."));
		ASSERT(FALSE);
		return NULL;
	}
	
	if( strXPath == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CEzXMLParser::SearchNodes => XPATH���� �������� �ʾҽ��ϴ�."));
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
// PARAM : IXMLDOMNodePtr pNodePtr - ������ ��� ������
// RETN  : TRUE/FALSE
// DESC  : ������ ��� ����Ʈ�� �����Ѵ�.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveNodes(IXMLDOMNodePtr pNodePtr)
{
	// XML�� �ε�Ǿ� attach�Ǿ� �ִ��� �˻�
	if( m_pXMLDoc == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => XML ������ �ε�Ǿ� ���� �ʽ��ϴ�."));
		ASSERT(FALSE);
		return FALSE;	
	}

	if(pNodePtr == NULL)
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => ������ ��� ����Ʈ NULL"));
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
				TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => �������� ����"));
				ASSERT(FALSE);
				return FALSE;
			}
			pRemoveNodePtr = NULL;
		}
		else 
		{
			TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => �θ� ��� ����Ʈ ȹ�� ����"));
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
// DESC  : �ش� ������ �����ϴ� ��带 ã�� ���� �Ѵ�.
// DATE  : 2004-07-07 coded by happyune, origin 
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveNodes(TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName /* = NULL */ )
{
	IXMLDOMNodeListPtr pNodeListPtr = NULL;
	pNodeListPtr = 	SearchNodes( strElementName, strValue, bElement, strAttrName );

	if( pNodeListPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => �ش� ���� ��� �˻� ����"));
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
// PARM : TCHAR* pTagName - tag name (xpath ����)
// RETN : TRUE/FALSE
// DESC : ������ �±��� ���� ��� ����
// Date : 2005-06-14 coded by happyune, origin
//////////////////////////////////////////////////
BOOL CEzXMLParser::RemoveAllTagNodes(TCHAR* pTagName)
{	
	IXMLDOMNodeListPtr pNodeListPtr = NULL;
	pNodeListPtr = 	SearchNodes( pTagName );
	
	if( pNodeListPtr == NULL )
	{
		TRACE(_T("\n[ERR by UNE] CXMLParser::RemovedNodes => �ش� ���� ��� �˻� ����"));
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


// �ӽ� �ڵ�.....
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

			// Encoding ����....
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