#include "fish_common.h"
#include "XML/EzXMLParser.h"


/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-31
 *
 * @Description
 * CEzXMLParser를 이용하여 XML에 해당하는 텍스트를 찾아 반환한다.
 * search the given node and get the text of it
 *
 * @Parameters
 * (in _bstr_t) sname - search name
 * (in CEzXMLParser*) parser - destination xml class to parse
 * (in IXMLDOMNodePtr) node - search root node
 * (in int) type - search type 0 : attribute, 1 : element
 *
 * @Returns
 * (CString) XML에서 얻어온 텍스트
 * text of the searching item
 **************************************************************************/
CString GetNamedNodeText(_bstr_t sname, CEzXMLParser* parser, IXMLDOMNodePtr node, int type)
{
	IXMLDOMNodeListPtr	nodelist = NULL;
	IXMLDOMNodePtr		p_node = NULL;

	if(!parser) return _T("");

	try
	{
		if(node == NULL)
		{
			if(type == 0)
			{
				return _T("");
			}
			else
			{
//				p_node = parser->m_pXMLDoc->selectSingleNode(sname);
				nodelist = parser->SearchNodes((LPTSTR) sname);
			}
		}
		else
		{
			if(type == 0)
			{
				p_node = node->attributes->getNamedItem(sname);

				if(p_node != 0)
				{
					return (LPTSTR) p_node->text;
				}
			}
			else
			{
//				p_node = node->selectSingleNode(sname);
				nodelist = parser->SearchNodes(node, (LPTSTR) sname);
			}
		}

		if((nodelist != NULL) && (nodelist->length > 0))
		{
			// comment by moonknit 2005-12-16
			// 이곳에서 메모리 누수가 발생한다.
			// selectNodes 로 얻어온 node의 경우 text를 이용하면 text 용량만큼 공간을 할당한다.
			p_node = nodelist->item[0];
			return (LPTSTR) p_node->text;
		}
	}
	catch (COleException&)
	{
	}

	return _T("");
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10-31
 *
 * @Description
 * XML의 attribute나 element text를 변경 또는 추가한다.
 * update or insert item text
 *
 * @Parameters
 * (in _bstr_t) sname - search name
 * (in/out CEzXMLParser*) parser - 검색할 xml 클래스
 * (in _bstr_t) text - 변경할 내용 
 * (in int) type - 변경할 대상의 속성
 * 0 : attribute
 * 1 : element text
 * (in IXMLDOMNodePtr) node - 검색 root node
 *
 * @Returns
 * (BOOL)
 * 0 : failed
 * 1 : updated
 * 2 : inserted
 **************************************************************************/
int SetNamedNodeText(_bstr_t sname, CEzXMLParser* parser, _bstr_t text, int type, IXMLDOMNodePtr node)
{
	if(!parser) return 0;

	if(type == 0)
	{
		IXMLDOMNodePtr nodeptr = NULL;
		nodeptr = node->attributes->getNamedItem(sname);
		if(nodeptr == NULL)
		{
			IXMLDOMAttributePtr attptr = parser->CreateAttribute(sname, text);
			if(attptr == NULL) return 0;

			// create attribute
			node->attributes->setNamedItem(attptr);

			return 2;
		}
		else
		{
			// update attribute
			nodeptr->text = text;
		}
	}
	else
	{
		IXMLDOMNodeListPtr listptr;
		listptr = parser->SearchNodes(node, sname);
		
		if(listptr == NULL || listptr->length == 0)
		{
			// create element
			IXMLDOMElementPtr elptr = parser->CreateElement(sname);

			if(elptr == NULL)
				return 0;

			elptr->text = text;
			if(node->appendChild(elptr) == NULL)
				return 0;

			return 2;
		}
		else
		{
			// update element text
			listptr->item[0]->text = text;
		}
	}

	return 1;
}