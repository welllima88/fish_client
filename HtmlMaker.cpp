#include "stdafx.h"
#include "HtmlMaker.h"
#include "XML/EzXMLParser.h"
#include "resource.h"

#include "fish_common.h"
#include "fish.h"
#include "LocalProperties.h"

#include "RSSCurrentList.h"

#define MAX_LENGTH		200		// ����Ʈ���⿡�� ������ ���� ������ ����....


BOOL RemoveAllTag(CString& html);

///////////////////////////////////////////////////////////////////////////////////////////////
// HTML Maker Definition



/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2005-12-1

 * [Description]
 * Post List�� �о� HTML Stream�� ����
 
 * [In/Out Parameter]
 * In : post - HTML�� ���� ����Ʈ
 *		feedtitle - �ش� ����Ʈ�� RSS Feed title
 *		feedtitle - �ش� ����Ʈ�� RSS Feed url
  
 * [Return Value]
 * �������� ��ȯ
 * ���н� m_strHtml = _T("")
 **************************************************************************/
BOOL CHtmlMaker::MakeHtml(SmartPtr<POST_LIST> &postlist, CString xsltfile, DWORD flag)
{
	return MakeHtml_Xslt(postlist, m_strHtml, xsltfile, flag);
}


/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2005-12-1

 * [Description] 
 * Post Data�� �о� HTML Stream ����
 
 * [In/Out Parameter]
 * In : post - HTML�� ���� ����Ʈ
 
 * [Return Value]
 * �������� ��ȯ
 * ���н� m_strHtml = _T("")
 **************************************************************************/
BOOL CHtmlMaker::MakeHtml(POSTITEM& post, CString xsltfile, DWORD flag)
{
	return MakeHtml_Xslt(post, m_strHtml, xsltfile, flag);
}


/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2005-12-1

 * [Description] 
 * HTML Stream�� ���Ϸ� ����
 
 * [In/Out Parameter]
 * In : filepath - ������ ����� ���
  
 * [Return Value]
 * �������� ��ȯ 
 **************************************************************************/
BOOL CHtmlMaker::Save(CString filepath)
{
	CFile	sfile;

	if((m_strHtml.GetLength() > 0) && (filepath.GetLength() > 0))
	{
		try
		{
			sfile.Open(filepath, CFile::modeCreate | CFile::modeWrite);

#ifdef _UNICODE
			int nlength = WideCharToMultiByte(CP_UTF8, 0, m_strHtml.LockBuffer(), m_strHtml.GetLength(), NULL, NULL, NULL, NULL);
			char* buffer = new char[nlength + 1];
			if(buffer == NULL)
				return FALSE;
			ZeroMemory(buffer, m_strHtml.GetLength() + 1);
			WideCharToMultiByte(CP_UTF8, 0, m_strHtml.LockBuffer(), m_strHtml.GetLength(), buffer, nlength + 1, NULL, NULL);
			//TRACE(_T("HTML RIGHT(10) : %s\r\n"), &buffer[m_strHtml.GetLength() - 10]);
			m_strHtml.UnlockBuffer();
			sfile.WriteHuge(buffer, nlength);
			delete [] buffer;
#else
			sfile.WriteHuge(m_strHtml, m_strHtml.GetLength());
#endif
			sfile.Close();

			return TRUE;
		}
		catch(CFileException&)
		{
			TRACE(_T("CFileException ERROR!!\n"));
		}
	}
	return FALSE;
}


/**************************************************************************
 * written by aquarelle
 *
 * [history]
 * created 2005-12-1
 * updated 2005-12-13 :: �־��� list�� �������� (�ð������� ���� Post�� ���� ���� HTML�� ����� ��� �߰�
						by moonknit 
 * updated 2005-12-27 :: XSLT ������ ���� Parser�� �̿��Ͽ� �����ϵ��� ���� by aquarelle
 * updated 2005-12-29 :: �ѹ��� �����ִ� �ִ� ��� ���� ���� by moonknit
 * updated 2006-01-13 :: itemcnt �߰� by moonknit
 * updated 2006-01-24 :: ���������� ��� �±� �����ϰ� 200�� ������ �����ֵ��� ����... by aquarelle
 * updated 2006-02-07 :: �Ķ���� ��ϰ��� auto_ptr���� SmartPtr�� ���� by moonknit
 * updated 2006-02-07 :: postlist�� �� �˻�
 * updated 2006-02-16 :: �ܺ� ��Ÿ�� ���� �ε� ��� ���� by aquarelle
 * updated 2006-03-21 :: parper �ۼ� ��Ÿ�� ���� ���� �Ķ���� �� xml���� ���� by aquarelle

 * [Description]
 * static method
 * Post List�� �о� HTML Stream ��ȯ
 
 * [In/Out Parameter]
 * (In SmartPtr<POST_LIST>&)	postlist - HTML�� ���� ����Ʈ  
 * (In CString)	xsltfile - ������ XSLT ����, default�� _T("")
 * (In DWORD)	flag - Html Make flage (loadtype, make style(reverse, paper ...)
 * (Out CString) htmlstream - ��ȯ�� html Stream (string) // ���н� htmlstream = _T("")
 
 * [Return Value]
 * �������� ��ȯ 
 **************************************************************************/
BOOL CHtmlMaker::MakeHtml_Xslt(SmartPtr<POST_LIST> &postlist, CString& htmlstream, CString xsltfile, DWORD flag)
{
	CEzXMLParser	parser, xsltdoc;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	IXMLDOMDocumentPtr p_doc;	
	CString		buffer;
	int			index=1;

	CString htmlTemplete;

	ASSERT(postlist != NULL);

	//if(loadtype != LOAD_STRING)
	//	TRACE(_T("Style file : %s\r\n"), xsltfile);
	

	htmlTemplete.Format(_T("<postlist type=\"%s\"></postlist>"), (flag & HM_PAPERSTYLE) ? _T("paper") : _T("list"));
	//TRACE(htmlTemplete);
		
	if(parser.LoadXML((LPTSTR)(LPCTSTR) htmlTemplete, CEzXMLParser::LOAD_STRING))
	{
		p_nodelist = parser.SearchNodes(_T("//postlist"));
		p_node = p_nodelist->item[0];

		p_element = parser.CreateElement(_T("channelinfo"));
		if(p_element == NULL)
				return FALSE;

		
		// Testing Code : Style Image ���.... ����..../////
//		TCHAR szFilePath[_MAX_PATH];
//		GetModuleFileName(NULL, szFilePath, _MAX_PATH);

#ifdef _UNICODE
		CString path(__wargv[0]);
#else
		CString path(__argv[0]);
#endif
		int pos = path.ReverseFind('\\');
		if (pos >= 0)
		{
			path = path.Left(pos + 1);
		}
				
		path += DEFAULT_STYLEIMGPATH;
		
		path.Replace(_T('\\'), _T('/'));
		/////////////////////////////////////////////////////


		//if(!parser.AddAttribute(p_element, _T("imagedir"), _T("../Style_Img")))
		if(!parser.AddAttribute(p_element, _T("imagedir"), (LPTSTR)(LPCTSTR) path))
				return FALSE;
		if(!parser.AppendChild(p_element, p_node))
				return FALSE;

		if(flag & HM_NOTREVERSE)
		{
            POST_LIST::iterator it;
			for(it = postlist->begin(); it != postlist->end(); it++, index++)
			{
				//body/div = item
				p_element = parser.CreateElement(_T("postitem"));
				if(p_element == NULL)
					return FALSE;
				if(!(*it).subject.IsEmpty())
				{
					if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) (*it).subject))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
						return FALSE;
				}
				if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) (*it).url))
					return FALSE;
				buffer.Format(_T("%d"), (*it).postid);
				if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;
				
				if((*it).description.GetLength() > (*it).encoded.GetLength())
					buffer = (*it).description;
				else
					buffer = (*it).encoded;

				
				if(flag & HM_DESCSUMMARY)
				{
					//buffer.Replace(_T("_blank"), _T(""));
					RemoveAllTag(buffer);
					if(buffer.GetLength() > MAX_LENGTH)
					{
						buffer = buffer.Left(MAX_LENGTH);
						buffer += _T("...");
					}
				}
				

				if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;		
				if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) (*it).pubdate.Format(FISH_DT_TEMPLETE)))
					return FALSE;
				if((*it).readon == TRUE)
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
						return FALSE;
				}
				buffer.Format(_T("%d"), (*it).svscrap);
				if(!parser.AddAttribute(p_element, _T("scrap"), (LPTSTR)(LPCTSTR) buffer))				
					return FALSE;

				if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) (*it).author))
					return FALSE;

				if(!parser.AppendChild(p_element, p_node))
					return FALSE;

//				if(index >= MAX_VIEWPOSTCNT)
//					break;

			}
		}
		else
		{
			POST_LIST::reverse_iterator rit;
			for(rit = postlist->rbegin(); rit != postlist->rend(); rit++, index++)
			{
				p_element = parser.CreateElement(_T("postitem"));
				//TRACE(_T("subject = %s\r\n"), (*rit).subject);
				//TRACE(_T("url = %s\r\n"), (*rit).url);
				//TRACE(_T("desc = %s\r\n"), (*rit).description);
				//TRACE(_T("date = %s\r\n"), (*rit).pubdate.Format(FISH_DT_TEMPLETE));

				if(p_element == NULL)
					return FALSE;			
				if(!(*rit).subject.IsEmpty())
				{
					if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) (*rit).subject))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
						return FALSE;
				}
				if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) (*rit).url))
					return FALSE;
				buffer.Format(_T("%d"), (*rit).postid);
				if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;
				
				if((*rit).description.GetLength() > (*rit).encoded.GetLength())
					buffer = (*rit).description;
				else
					buffer = (*rit).encoded;

				
				if(flag & HM_DESCSUMMARY)
				{
					//buffer.Replace(_T("_blank"), _T(""));
					RemoveAllTag(buffer);
					if(buffer.GetLength() > MAX_LENGTH)
					{
						buffer = buffer.Left(MAX_LENGTH);
						buffer += _T("...");
					}
				}
				

				if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;		
				if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) (*rit).pubdate.Format(FISH_DT_TEMPLETE)))
					return FALSE;
				if((*rit).readon == TRUE)
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
						return FALSE;
				}
				buffer.Format(_T("%d"), (*rit).svscrap);
				if(!parser.AddAttribute(p_element, _T("scrap"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;

				if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) (*rit).author))
					return FALSE;

				if(!parser.AppendChild(p_element, p_node))
					return FALSE;

//				if(index >= MAX_VIEWPOSTCNT)
//					break;

			}
		}
	}	

	buffer.Empty(); // TEST
	if((flag & HM_LOADMASK) == HM_LOADLOCAL)
	{
		buffer.Empty();

		CFile file;	
		if(file.Open((LPTSTR)(LPCTSTR) xsltfile, CFile::modeRead))
		{
			//xsltfile.UnlockBuffer();

		#ifdef _UNICODE
			int flen = file.GetLength();
			char* xslt = new char[flen+1];

			if(xslt != NULL)
			{
				file.ReadHuge((void*)xslt, file.GetLength());

				// modified by moonknit 2006-03-04
//				BSTR uni_xslt = SysAllocStringLen(NULL, strlen(xslt));
//				MultiByteToWideChar(CP_ACP, 0, (char*)xslt, strlen(xslt), uni_xslt, strlen(xslt));
				
//				USES_CONVERSION;
//				buffer = CString(OLE2A(uni_xslt));
//				SysFreeString(uni_xslt);

				wchar_t* wbuf = new wchar_t[flen+1];

				xslt[flen] = 0;
				int l = MultiByteToWideChar(CP_ACP, 0, xslt, -1, wbuf, flen + 1);
				buffer = wbuf;

				delete wbuf;
				// --

			}
			else
			{
				buffer.Empty();
			}

			if(xslt != NULL)
				delete [] xslt;
		#else
			file.ReadHuge((void*)buffer.LockBuffer(), file.GetLength());
			buffer.UnlockBuffer();
			buffer = buffer.Left(file.GetLength());
		#endif
//			buffer = buffer.Left(buffer.ReverseFind(_T('>')) + 1);

			file.Close();
		}
		else
			// modified by moonknit 2006-03-05
			//return FALSE;
			xsltfile.Empty();
	}
	else if(!xsltfile.IsEmpty() && ((flag & HM_LOADMASK) == HM_LOADSTRING))
	{
		buffer = xsltfile;
	}

	if(xsltfile.IsEmpty() || !xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
	{	// XSLT ������ �������� �ʾҰų�.. XSLT ���Ϸε忡 �����������
		// Resource�� �⺻ XSLT�� �ε�
		if(!buffer.IsEmpty() && (flag & HM_LOADMASK == HM_LOADSTRING))
			TRACE(_T("String File failure!!!\r\n"));
		else if(buffer.IsEmpty() && xsltfile.IsEmpty())
			TRACE(_T("Default File Load!!!\r\n"));
		else
			TRACE(_T("file load failure\r\n"));


		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFAULT_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		if(buffer.IsEmpty())
			return FALSE;

		if(!xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
			return FALSE;
	}

	p_doc = parser.GetRootNode();
	
	try
	{
		htmlstream = (LPTSTR) p_doc->transformNode(xsltdoc.GetRootNode());
	}
	catch(_com_error&)
	{		
		TRACE(_T("\nXSLT Transform Error!!!\n"));
	}

	xsltdoc.Clear();
	parser.Clear();

	if(htmlstream.IsEmpty())
		return FALSE;

	return TRUE;
}


/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2005-12-1
 * updated 2005-12-27 :: XSLT ������ ���� Parser�� �̿��Ͽ� �����ϵ��� ���� by aquarelle
 * updated 2006-02-16 :: �ܺ� ��Ÿ�� ���� �ε� ��� ���� by aquarelle
 * updated 2006-03-21 :: parper �ۼ� ��Ÿ�� ���� ���� �Ķ���� �� xml���� ���� by aquarelle

 * [Description]
 * static method
 * Post Data�� �о� HTML Stream ��ȯ
 
 * [In/Out Parameter]
 * (In POSTITEM&)	post - HTML�� ���� ����Ʈ
 * (In CString)		xsltfile - ������ XSLT ����, default�� _T("")
 * (Out CString&)	htmlstream - ��ȯ�� html Stream (string) // ���н� htmlstream = _T("")
 
 * [Return Value]
 * �������� ��ȯ 
 
 **************************************************************************/
BOOL CHtmlMaker::MakeHtml_Xslt(POSTITEM& post, CString& htmlstream, CString xsltfile, DWORD flag)
{
	CEzXMLParser	parser, xsltdoc;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	IXMLDOMDocumentPtr p_doc;	
	CString		buffer;
	int			index=1;

	TRACE(_T("Style file : %s\r\n"), xsltfile);

	CString htmlTemplete;
	htmlTemplete.Format(_T("<postlist type=\"%s\"></postlist>"), (flag & HM_PAPERSTYLE) ? _T("paper") : _T("post"));
	
	if(parser.LoadXML((LPTSTR)(LPCTSTR) htmlTemplete, CEzXMLParser::LOAD_STRING))
	{
		p_nodelist = parser.SearchNodes(_T("//postlist"));
		p_node = p_nodelist->item[0];

		p_element = parser.CreateElement(_T("channelinfo"));
		if(p_element == NULL)
				return FALSE;

		// Testing Code : Style Image ���.... ����..../////
//		TCHAR szFilePath[_MAX_PATH];
//		GetModuleFileName(NULL, szFilePath, _MAX_PATH);

#ifdef _UNICODE
		CString path(__wargv[0]);
#else
		CString path(__argv[0]);
#endif

		int pos = path.ReverseFind('\\');
		if (pos >= 0)
		{
			path = path.Left(pos + 1);
		}
						
		path += DEFAULT_STYLEIMGPATH;
		
		path.Replace(_T('\\'), _T('/'));
		/////////////////////////////////////////////////////

		if(!parser.AddAttribute(p_element, _T("imagedir"), (LPTSTR)(LPCTSTR) path))
				return FALSE;
		if(!parser.AppendChild(p_element, p_node))
				return FALSE;
		
		//body/div = item
		p_element = parser.CreateElement(_T("postitem"));
		if(p_element == NULL)
			return FALSE;			
		if(!post.subject.IsEmpty())
		{
			if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) post.subject))
				return FALSE;
		}
		else
		{
			if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
				return FALSE;
		}
		if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) post.url))
			return FALSE;
		buffer.Format(_T("%d"), post.postid);
		if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
			return FALSE;

		//buffer = post.description;
		//buffer.Replace(_T("_blank"), _T(""));

		if(post.description.GetLength() > post.encoded.GetLength())
			buffer = post.description;
		else
			buffer = post.encoded;

		if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
			return FALSE;		
		if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) post.pubdate.Format(FISH_DT_TEMPLETE)))
			return FALSE;
		if(post.readon == TRUE)
		{
			if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
				return FALSE;
		}
		else
		{
			if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
				return FALSE;
		}

		if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) post.author))
					return FALSE;

		if(!parser.AppendChild(p_element, p_node))
			return FALSE;
	}

	
	if(!xsltfile.IsEmpty() && ((flag & HM_LOADMASK) == HM_LOADLOCAL))
	{
		buffer.Empty();

		CFile file;	
		if(file.Open((LPTSTR)(LPCTSTR) xsltfile, CFile::modeRead))
		{
			//xsltfile.UnlockBuffer();

		#ifdef _UNICODE

			int flen = file.GetLength();

			char* xslt = new char[flen + 1];		

			if(xslt != NULL)
			{
				file.ReadHuge((void*)xslt, file.GetLength());
				file.Close();

				// modified by moonknit 2006-03-04
//				BSTR uni_xslt = SysAllocStringLen(NULL, strlen(xslt));
//				MultiByteToWideChar(CP_ACP, 0, (char*)xslt, strlen(xslt), uni_xslt, strlen(xslt));
				
//				USES_CONVERSION;
//				buffer = CString(OLE2A(uni_xslt));
//				SysFreeString(uni_xslt);

				wchar_t* wbuf = new wchar_t[flen+1];

				xslt[flen] = 0;
				int l = MultiByteToWideChar(CP_ACP, 0, xslt, -1, wbuf, flen + 1);
				buffer = wbuf;

				delete wbuf;
				// --

			}
			else
			{
				buffer.Empty();
			}

			if(xslt != NULL)
				delete [] xslt;		

		#else
			file.ReadHuge((void*)buffer.LockBuffer(), file.GetLength());
			buffer.UnlockBuffer();
			buffer = buffer.Left(file.GetLength());
			file.Close();
		#endif
//			buffer = buffer.Left(buffer.ReverseFind(_T('>')) + 1);
		}
		else
			xsltfile.Empty();
	}
	else if(!xsltfile.IsEmpty() && ((flag & HM_LOADMASK) == HM_LOADSTRING))
	{
		buffer = xsltfile;
	}

//	TRACE(_T("xsltfile : %s\r\n"));

	if(xsltfile.IsEmpty() || !xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
	{	// XSLT ������ �������� �ʾҰų�.. XSLT ���Ϸε忡 �����������
		// Resource�� �⺻ XSLT�� �ε�

		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFAULT_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		if(buffer.IsEmpty())
			return FALSE;

		if(!xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
			return FALSE;
	}

	p_doc = parser.GetRootNode();
	
	try
	{
		htmlstream = (LPTSTR) p_doc->transformNode(xsltdoc.GetRootNode());
	}
	catch(_com_error&)
	{
		TRACE(_T("\nXSLT Transform Error!!!\n"));
	}

	xsltdoc.Clear();
	parser.Clear();

	if(htmlstream.IsEmpty())
		return FALSE;

	return TRUE;
}


/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2006-01-24
 
 * [Description]
 * �ش� html�� ��� �±׸� �����Ѵ�.... (�����۾���... ����������..... ��,.��)
 
 * [In/Out Parameter]
 * (In/Out CStirng&) html - ���� ��� htmlstream 
 
 * [Return Value]
 * ���ŵ� �±װ� �ִ����� ����...
 
 **************************************************************************/
BOOL RemoveAllTag(CString& html)
{
	int bRemoved = FALSE;
	int pos = 0, pos2 = 0;
	
	CString buffer;
	
	do
	{
		
		pos = html.Find(_T("<"), pos);
		//if(pos == -1)
		//	pos = html.Find(_T("<"), 0);
		//if(pos == -1)
		//	pos = html.Find(_T("<"), 0);

		pos2 = html.Find(_T(">"), pos + 1);
		//pos3 = html.Find(_T("<"), pos2 + 1);

		if(pos != -1 && pos2 != -1 && pos < pos2)
		{
			bRemoved = TRUE;
			buffer = html.Left(pos2 + 1);
			buffer = buffer.Right(pos2 - pos + 1);
		//	TRACE(buffer);
			// ��ũ��Ʈ ����...
			if(buffer.Find(_T("<script")) != -1 || buffer.Find(_T("<SCRIPT")) != -1 || buffer.Find(_T("<Script")) != -1 )
			{
				int pos_script;

				pos_script = html.Find(_T("</script>"), pos2);
				if(pos_script == -1)
					pos_script = html.Find(_T("</SCRIPT>"), pos2);
				if(pos_script == -1)
					pos_script = html.Find(_T("</Script>"), pos2);

				if(pos_script != -1)
				{
					pos_script += _tcsclen(_T("</script>"));
					buffer = html.Left(pos_script + 1);
					buffer = buffer.Right(pos_script - pos - 1);

					html.Replace(buffer, _T(""));
				}
				else
					pos++;
			}
			else
				html.Replace(buffer, _T(""));
		}
		else if(pos != -1)
			pos++;

	}while(pos != -1 && pos2 != -1);

	return bRemoved;

//	return TRUE;
}












#define MAX_VIEWPOSTCNT							30

/**************************************************************************
 * written by aquarelle
 *
 * [history]
 * created 2005-12-1
 * updated 2005-12-13 :: �־��� list�� �������� (�ð������� ���� Post�� ���� ���� HTML�� ����� ��� �߰�
						by moonknit 
 * updated 2005-12-27 :: XSLT ������ ���� Parser�� �̿��Ͽ� �����ϵ��� ���� by aquarelle
 * updated 2005-12-29 :: �ѹ��� �����ִ� �ִ� ��� ���� ���� by moonknit
 * updated 2006-01-13 :: itemcnt �߰� by moonknit
 * updated 2006-01-24 :: ���������� ��� �±� �����ϰ� 200�� ������ �����ֵ��� ����... by aquarelle
 * updated 2006-02-07 :: �Ķ���� ��ϰ��� auto_ptr���� SmartPtr�� ���� by moonknit
 * updated 2006-02-07 :: postlist�� �� �˻�
 * updated 2006-02-16 :: �ܺ� ��Ÿ�� ���� �ε� ��� ���� by aquarelle

 * [Description]
 * static method
 * Post List�� �о� HTML Stream ��ȯ
 
 * [In/Out Parameter]
 * In : postlist - HTML�� ���� ����Ʈ
 *		feedtitle - �ش� ����Ʈ�� RSS Feed title
 *		feedtitle - �ش� ����Ʈ�� RSS Feed url
 *		breverse - HTML�� ����� ����Ʈ�� �ð��� �������� �����Ѵ�.
 *      xsltfile - ������ XSLT ����, default�� _T("")
 * Out : htmlstream - ��ȯ�� html Stream (string)
 
 * [Return Value]
 * �������� ��ȯ
 * ���н� htmlstream = _T("")
 **************************************************************************/
 /*
BOOL CHtmlMaker::MakeHtml_Xslt(SmartPtr<POST_LIST> &postlist, CString& htmlstream, BOOL breverse, CString xsltfile, int loadtype)
{
	CEzXMLParser	parser, xsltdoc;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	IXMLDOMDocumentPtr p_doc;	
	CString		buffer;
	int			index=1;

	CString htmlTemplete;

	ASSERT(postlist != NULL);

	if(loadtype != LOAD_STRING)
		TRACE(_T("Style file : %s\r\n"), xsltfile);
	
	htmlTemplete = _T("<postlist type=\"list\"></postlist>");
		
	if(parser.LoadXML((LPTSTR)(LPCTSTR) htmlTemplete, CEzXMLParser::LOAD_STRING))
	{
		p_nodelist = parser.SearchNodes(_T("//postlist"));
		p_node = p_nodelist->item[0];

		p_element = parser.CreateElement(_T("channelinfo"));
		if(p_element == NULL)
				return FALSE;

		
		// Testing Code : Style Image ���.... ����..../////
//		TCHAR szFilePath[_MAX_PATH];
//		GetModuleFileName(NULL, szFilePath, _MAX_PATH);

#ifdef _UNICODE
		CString path(__wargv[0]);
#else
		CString path(__argv[0]);
#endif
		int pos = path.ReverseFind('\\');
		if (pos >= 0)
		{
			path = path.Left(pos + 1);
		}
				
		path += DEFAULT_STYLEIMGPATH;
		
		path.Replace(_T('\\'), _T('/'));
		/////////////////////////////////////////////////////


		//if(!parser.AddAttribute(p_element, _T("imagedir"), _T("../Style_Img")))
		if(!parser.AddAttribute(p_element, _T("imagedir"), (LPTSTR)(LPCTSTR) path))
				return FALSE;
		if(!parser.AppendChild(p_element, p_node))
				return FALSE;

		if(!breverse)
		{
            POST_LIST::iterator it;
			for(it = postlist->begin(); it != postlist->end(); it++, index++)
			{
				//body/div = item
				p_element = parser.CreateElement(_T("postitem"));
				if(p_element == NULL)
					return FALSE;
				if(!(*it).subject.IsEmpty())
				{
					if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) (*it).subject))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
						return FALSE;
				}
				if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) (*it).url))
					return FALSE;
				buffer.Format(_T("%d"), (*it).postid);
				if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;
				
				if((*it).description.GetLength() > (*it).encoded.GetLength())
					buffer = (*it).description;
				else
					buffer = (*it).encoded;

				
				if(loadtype != LOAD_STRING && !theApp.m_spLP->GetListDescSummary())
				{
					//buffer.Replace(_T("_blank"), _T(""));
					RemoveAllTag(buffer);
					if(buffer.GetLength() > MAX_LENGTH)
						buffer = buffer.Left(MAX_LENGTH);
				}
				

				if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;		
				if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) (*it).pubdate.Format(FISH_DT_TEMPLETE)))
					return FALSE;
				if((*it).readon == TRUE)
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
						return FALSE;
				}
				buffer.Format(_T("%d"), (*it).svscrap);
				if(!parser.AddAttribute(p_element, _T("scrap"), (LPTSTR)(LPCTSTR) buffer))				
					return FALSE;

				if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) (*it).author))
					return FALSE;

				if(!parser.AppendChild(p_element, p_node))
					return FALSE;

//				if(index >= MAX_VIEWPOSTCNT)
//					break;

			}
		}
		else
		{
			POST_LIST::reverse_iterator rit;
			for(rit = postlist->rbegin(); rit != postlist->rend(); rit++, index++)
			{
				p_element = parser.CreateElement(_T("postitem"));
				//TRACE(_T("subject = %s\r\n"), (*rit).subject);
				//TRACE(_T("url = %s\r\n"), (*rit).url);
				//TRACE(_T("desc = %s\r\n"), (*rit).description);
				//TRACE(_T("date = %s\r\n"), (*rit).pubdate.Format(FISH_DT_TEMPLETE));

				if(p_element == NULL)
					return FALSE;			
				if(!(*rit).subject.IsEmpty())
				{
					if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) (*rit).subject))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
						return FALSE;
				}
				if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) (*rit).url))
					return FALSE;
				buffer.Format(_T("%d"), (*rit).postid);
				if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;
				
				if((*rit).description.GetLength() > (*rit).encoded.GetLength())
					buffer = (*rit).description;
				else
					buffer = (*rit).encoded;

				
				if(loadtype != LOAD_STRING && !theApp.m_spLP->GetListDescSummary())
				{
					//buffer.Replace(_T("_blank"), _T(""));
					RemoveAllTag(buffer);
					if(buffer.GetLength() > MAX_LENGTH)
						buffer = buffer.Left(MAX_LENGTH);
				}
				

				if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;		
				if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) (*rit).pubdate.Format(FISH_DT_TEMPLETE)))
					return FALSE;
				if((*rit).readon == TRUE)
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
						return FALSE;
				}
				else
				{
					if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
						return FALSE;
				}
				buffer.Format(_T("%d"), (*rit).svscrap);
				if(!parser.AddAttribute(p_element, _T("scrap"), (LPTSTR)(LPCTSTR) buffer))
					return FALSE;

				if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) (*rit).author))
					return FALSE;

				if(!parser.AppendChild(p_element, p_node))
					return FALSE;

//				if(index >= MAX_VIEWPOSTCNT)
//					break;

			}
		}
	}	

	buffer.Empty(); // TEST
	if(!xsltfile.IsEmpty() && loadtype != LOAD_STRING)
	{
		buffer.Empty();

		CFile file;	
		if(file.Open((LPTSTR)(LPCTSTR) xsltfile, CFile::modeRead))
		{
			//xsltfile.UnlockBuffer();

		#ifdef _UNICODE
			int flen = file.GetLength();
			char* xslt = new char[flen+1];

			if(xslt != NULL)
			{
				file.ReadHuge((void*)xslt, file.GetLength());

				// modified by moonknit 2006-03-04
//				BSTR uni_xslt = SysAllocStringLen(NULL, strlen(xslt));
//				MultiByteToWideChar(CP_ACP, 0, (char*)xslt, strlen(xslt), uni_xslt, strlen(xslt));
				
//				USES_CONVERSION;
//				buffer = CString(OLE2A(uni_xslt));
//				SysFreeString(uni_xslt);

				wchar_t* wbuf = new wchar_t[flen+1];

				xslt[flen] = 0;
				int l = MultiByteToWideChar(CP_ACP, 0, xslt, -1, wbuf, flen + 1);
				buffer = wbuf;

				delete wbuf;
				// --

			}
			else
			{
				buffer.Empty();
			}

			if(xslt != NULL)
				delete [] xslt;
		#else
			file.ReadHuge((void*)buffer.LockBuffer(), file.GetLength());
			buffer.UnlockBuffer();
			buffer = buffer.Left(file.GetLength());
		#endif
//			buffer = buffer.Left(buffer.ReverseFind(_T('>')) + 1);

			file.Close();
		}
		else
			// modified by moonknit 2006-03-05
			//return FALSE;
			xsltfile.Empty();
	}
	else if(!xsltfile.IsEmpty() && loadtype == LOAD_STRING)
	{
		buffer = xsltfile;
	}

	if(xsltfile.IsEmpty() || !xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
	{	// XSLT ������ �������� �ʾҰų�.. XSLT ���Ϸε忡 �����������
		// Resource�� �⺻ XSLT�� �ε�
		if(!buffer.IsEmpty() && loadtype != LOAD_STRING)
			TRACE(_T("String File failure!!!\r\n"));
		else if(buffer.IsEmpty() && xsltfile.IsEmpty())
			TRACE(_T("Default File Load!!!\r\n"));
		else
			TRACE(_T("file load failure\r\n"));


		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFAULT_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		if(buffer.IsEmpty())
			return FALSE;

		if(!xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
			return FALSE;
	}

	p_doc = parser.GetRootNode();
	
	try
	{
		htmlstream = (LPTSTR) p_doc->transformNode(xsltdoc.GetRootNode());
	}
	catch(_com_error&)
	{		
		TRACE(_T("\nXSLT Transform Error!!!\n"));
	}

	xsltdoc.Clear();
	parser.Clear();

	if(htmlstream.IsEmpty())
		return FALSE;

	return TRUE;
}
*/


/**************************************************************************
 * written by aquarelle

 * [history]
 * created 2005-12-1
 * updated 2005-12-27 :: XSLT ������ ���� Parser�� �̿��Ͽ� �����ϵ��� ���� by aquarelle
 * updated 2006-02-16 :: �ܺ� ��Ÿ�� ���� �ε� ��� ���� by aquarelle

 * [Description]
 * static method
 * Post Data�� �о� HTML Stream ��ȯ
 
 * [In/Out Parameter]
 * In : post - HTML�� ���� ����Ʈ
 *      xsltfile - ������ XSLT ����, default�� _T("")
 * Out : htmlstream - ��ȯ�� html Stream (string)
 
 * [Return Value]
 * �������� ��ȯ
 * ���н� htmlstream = _T("")
 
 **************************************************************************/
 /*
BOOL CHtmlMaker::MakeHtml_Xslt(POSTITEM& post, CString& htmlstream, CString xsltfile, int loadtype)
{
	CEzXMLParser	parser, xsltdoc;
	IXMLDOMNodeListPtr p_nodelist;
	IXMLDOMNodePtr p_node;
	IXMLDOMElementPtr p_element;	
	IXMLDOMAttributePtr p_attribute;
	IXMLDOMDocumentPtr p_doc;	
	CString		buffer;
	int			index=1;

	TRACE(_T("Style file : %s\r\n"), xsltfile);

	CString htmlTemplete;
	htmlTemplete = _T("<postlist type=\"post\"></postlist>");
	
	if(parser.LoadXML((LPTSTR)(LPCTSTR) htmlTemplete, CEzXMLParser::LOAD_STRING))
	{
		p_nodelist = parser.SearchNodes(_T("//postlist"));
		p_node = p_nodelist->item[0];

		p_element = parser.CreateElement(_T("channelinfo"));
		if(p_element == NULL)
				return FALSE;

		// Testing Code : Style Image ���.... ����..../////
//		TCHAR szFilePath[_MAX_PATH];
//		GetModuleFileName(NULL, szFilePath, _MAX_PATH);

#ifdef _UNICODE
		CString path(__wargv[0]);
#else
		CString path(__argv[0]);
#endif

		int pos = path.ReverseFind('\\');
		if (pos >= 0)
		{
			path = path.Left(pos + 1);
		}
						
		path += DEFAULT_STYLEIMGPATH;
		
		path.Replace(_T('\\'), _T('/'));
		/////////////////////////////////////////////////////

		if(!parser.AddAttribute(p_element, _T("imagedir"), (LPTSTR)(LPCTSTR) path))
				return FALSE;
		if(!parser.AppendChild(p_element, p_node))
				return FALSE;
		
		//body/div = item
		p_element = parser.CreateElement(_T("postitem"));
		if(p_element == NULL)
			return FALSE;			
		if(!post.subject.IsEmpty())
		{
			if(!parser.AddAttribute(p_element, _T("title"), (LPTSTR)(LPCTSTR) post.subject))
				return FALSE;
		}
		else
		{
			if(!parser.AddAttribute(p_element, _T("title"), _T("&nbsp;&nbsp;&nbsp;")))
				return FALSE;
		}
		if(!parser.AddAttribute(p_element, _T("url"), (LPTSTR)(LPCTSTR) post.url))
			return FALSE;
		buffer.Format(_T("%d"), post.postid);
		if(!parser.AddAttribute(p_element, _T("id"), (LPTSTR)(LPCTSTR) buffer))
			return FALSE;

		//buffer = post.description;
		//buffer.Replace(_T("_blank"), _T(""));

		if(post.description.GetLength() > post.encoded.GetLength())
			buffer = post.description;
		else
			buffer = post.encoded;

		if(!parser.AddAttribute(p_element, _T("desc"), (LPTSTR)(LPCTSTR) buffer))
			return FALSE;		
		if(!parser.AddAttribute(p_element, _T("date"), (LPTSTR)(LPCTSTR) post.pubdate.Format(FISH_DT_TEMPLETE)))
			return FALSE;
		if(post.readon == TRUE)
		{
			if(!parser.AddAttribute(p_element, _T("readon"), _T("1")))
				return FALSE;
		}
		else
		{
			if(!parser.AddAttribute(p_element, _T("readon"), _T("0")))
				return FALSE;
		}

		if(!parser.AddAttribute(p_element, _T("author"), (LPTSTR)(LPCTSTR) post.author))
					return FALSE;

		if(!parser.AppendChild(p_element, p_node))
			return FALSE;
	}

	
	if(!xsltfile.IsEmpty())
	{
		buffer.Empty();

		CFile file;	
		if(file.Open((LPTSTR)(LPCTSTR) xsltfile, CFile::modeRead))
		{
			//xsltfile.UnlockBuffer();

		#ifdef _UNICODE

			int flen = file.GetLength();

			char* xslt = new char[flen + 1];		

			if(xslt != NULL)
			{
				file.ReadHuge((void*)xslt, file.GetLength());
				file.Close();

				// modified by moonknit 2006-03-04
//				BSTR uni_xslt = SysAllocStringLen(NULL, strlen(xslt));
//				MultiByteToWideChar(CP_ACP, 0, (char*)xslt, strlen(xslt), uni_xslt, strlen(xslt));
				
//				USES_CONVERSION;
//				buffer = CString(OLE2A(uni_xslt));
//				SysFreeString(uni_xslt);

				wchar_t* wbuf = new wchar_t[flen+1];

				xslt[flen] = 0;
				int l = MultiByteToWideChar(CP_ACP, 0, xslt, -1, wbuf, flen + 1);
				buffer = wbuf;

				delete wbuf;
				// --

			}
			else
			{
				buffer.Empty();
			}

			if(xslt != NULL)
				delete [] xslt;		

		#else
			file.ReadHuge((void*)buffer.LockBuffer(), file.GetLength());
			buffer.UnlockBuffer();
			buffer = buffer.Left(file.GetLength());
			file.Close();
		#endif
//			buffer = buffer.Left(buffer.ReverseFind(_T('>')) + 1);
		}
		else
			xsltfile.Empty();
	}

//	TRACE(_T("xsltfile : %s\r\n"));

	if(xsltfile.IsEmpty() || !xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
	{	// XSLT ������ �������� �ʾҰų�.. XSLT ���Ϸε忡 �����������
		// Resource�� �⺻ XSLT�� �ε�

		HRSRC hRsrc;
		HGLOBAL hGResource;

		hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFAULT_XSLT), _T("XSLT"));

		hGResource = LoadResource(NULL, hRsrc);

		buffer = (LPSTR)LockResource(hGResource);	
		UnlockResource(hGResource);

		if(buffer.IsEmpty())
			return FALSE;

		if(!xsltdoc.LoadXML((LPTSTR)(LPCTSTR) buffer, CEzXMLParser::LOAD_STRING))
			return FALSE;
	}

	p_doc = parser.GetRootNode();
	
	try
	{
		htmlstream = (LPTSTR) p_doc->transformNode(xsltdoc.GetRootNode());
	}
	catch(_com_error&)
	{
		TRACE(_T("\nXSLT Transform Error!!!\n"));
	}

	xsltdoc.Clear();
	parser.Clear();

	if(htmlstream.IsEmpty())
		return FALSE;

	return TRUE;
}
*/