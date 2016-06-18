// EzXMLParser.h: interface for the CEzXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EZXMLPARSER_H__72DB36FD_A164_4367_935C_B5FF3FEDAC73__INCLUDED_)
#define AFX_EZXMLPARSER_H__72DB36FD_A164_4367_935C_B5FF3FEDAC73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEzXMLParser  
{
public:
	enum __LOAD_FLAG
	{
		LOAD_LOCAL = 0,
		LOAD_WEB   = 1,
		LOAD_STRING = 2, 
		LOAD_ONLYCNT = 3
	};
	
public:
	CEzXMLParser();
	CEzXMLParser( TCHAR* strPath, int iLoadFlag );
	virtual ~CEzXMLParser();
	
	void Init();
	
	BOOL LoadXML(TCHAR* strPath, int iLoadFlag = LOAD_LOCAL);
	BOOL SaveXML(TCHAR* strPaht = NULL);
	BOOL CreteXML();
	
	IXMLDOMElementPtr	CreateElement( TCHAR* strTagName );
	IXMLDOMAttributePtr CreateAttribute( TCHAR* strAttrName, TCHAR* strValue = NULL );
	BOOL				AddAttribute( IXMLDOMElementPtr pElementPtr, IXMLDOMAttributePtr pAttrPtr );
	BOOL				AddAttribute( IXMLDOMElementPtr pElementPtr, TCHAR* strAttrName, TCHAR* strAttrValue = NULL );
	BOOL				AppendChild( IXMLDOMElementPtr pChildPtr, IXMLDOMNodePtr pParentPtr = NULL );
	
	IXMLDOMNodeListPtr	SearchNodes( TCHAR* strXPath );
	IXMLDOMNodeListPtr	SearchNodes( TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName = NULL );
	IXMLDOMNodeListPtr	SearchNodes( IXMLDOMNodePtr pNodePtr, TCHAR* strXPath );
	
	BOOL				RemoveNodes(IXMLDOMNodePtr pNodePtr);
	BOOL				RemoveNodes(TCHAR* strElementName, TCHAR* strValue, BOOL bElement, TCHAR* strAttrName = NULL );
	BOOL				RemoveAllTagNodes(TCHAR* pTagName);
	
	BOOL				Clear();

	IXMLDOMNodePtr		GetRootNode();

	BOOL				ApplyXSLT(TCHAR* strVal);

	__inline void SetSavePath(TCHAR* pPath)	{	ASSERT(pPath);		if( pPath != NULL )		_sntprintf(m_strXMLSavePath, _tcslen(pPath)+1, pPath);	}
	__inline TCHAR* GetSavePath(void)		{	return m_strXMLSavePath;	}
	
public:
	IXMLDOMDocument2Ptr m_pXMLDoc;		// xml document pointer	
private:
	TCHAR m_strXMLSavePath[1024*2 +1];
	int	  m_iLoadFlag;	
	
};

#endif // !defined(AFX_EZXMLPARSER_H__72DB36FD_A164_4367_935C_B5FF3FEDAC73__INCLUDED_)

inline void ErrorTestHR(HRESULT _hr)
{
//	if( SUCCEEDED(_hr) )
//	{
//		debug("oK");
//	}
//	else
	if( FAILED(_hr) )
	{
		if( _hr == REGDB_E_CLASSNOTREG )
		{
			TRACE(_T("등록 안되어 있음"));
		}
		else if( _hr==E_INVALIDARG)
		{
			TRACE(_T("E_INVALIDARG "));
		}
		else if( _hr == E_FAIL )
		{
			TRACE(_T("E_FAIL"));
		}

		CString shr;
		shr.Format(_T("%x"), _hr);

//		throw(_hr);
	}

//	if(FAILED(_hr))
//	{
//		throw(_hr);
//	}
}

// _com_error msg 출력 
inline void PrintComError(const _com_error &e)
{
	TRACE(_T("\n[ERR] ComErr : %s\r\n%s\r\n"), e.ErrorMessage(), e.Description());
}