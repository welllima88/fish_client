// WebText.cpp: implementation of the CWebText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fish.h"
#include "WebText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWebText::CWebText()
{
	m_encoding = ASCII;
	m_offset = 0;
}

CWebText::~CWebText()
{

}

void CWebText::SetText(BYTE* pByte, int len)
{
	if(m_text.IsEmpty())
	{
		int encoding, offset;
		CheckType(pByte, len, encoding, offset);
		m_encoding = encoding;
		m_offset = offset;
		if(encoding != ASCII)
		{
			while(m_encoding == encoding)
			{
				CheckType(pByte+m_offset, len-m_offset, encoding, offset);
				m_offset += offset;
			}
		}
		AddText(pByte+m_offset, len - m_offset);
	}
	else AddText(pByte, len);
}

void CWebText::AddText(BYTE* pByte, int len)
{
#ifdef _UNICODE
	int i = 0;
	if(m_encoding == ASCII)
	{
		TCHAR* buf = (TCHAR*) malloc((len + 1) * sizeof(TCHAR));
		if(
			MultiByteToWideChar(
			CP_ACP, 
			NULL,
			(char*) pByte,
			len,
			buf,
			len+1
			) != 0
			)
		{
			buf[len] = 0;
			m_text += buf;
//			TRACE(_T("ASCII Text : %s\r\n"), m_text);
		}

		delete buf;
	}
	else if(m_encoding == UTF8)
	{
		BYTE b;
		for(i = 0; i < len; ++i)
		{
			b = pByte[i];
			WCHAR c = '?';

			if(!(b&0x80)) // 0xxxxxxx
			{
				c = b&0x7f;
			}
			else if((b&0xe0) == 0xc0) // 110xxxxx 10xxxxxx
			{
				c = (b&0x1f)<<6;
				if(++i >= len) break;
				b = pByte[i];
				c |= (b&0x3f);
			}
			else if((b&0xf0) == 0xe0) // 1110xxxx 10xxxxxx 10xxxxxx
			{
				c = (b&0x0f)<<12;
				if(++i >= len) break;
				c |= (pByte[i]&0x3f)<<6;
				if(++i >= len) break;
				c |= (pByte[i]&0x3f);
			}

			m_text += c;
		}
	}
	else if(m_encoding == LE16)
	{
		WORD w;
		for(i = 0; i < len; i+=2)
		{
			memcpy(&w, pByte+i, 2);

			m_text += w;
		}
	}
	else if(m_encoding == BE16)
	{
		WORD w;
		for(i = 0; i < len; i+=2)
		{
			memcpy(&w, pByte+i, 2);
			w = ((w>>8)&0x00ff)|((w<<8)&0xff00);
			m_text += w;
		}
	}
#else
	int i = 0;
	if(m_encoding == ASCII)
	{
		m_text += CString(pByte, len);
	}
	else if(m_encoding == UTF8)
	{
		BYTE b;
		for(i = 0; i < len; ++i)
		{
			b = pByte[i];
			char c = '?';
			if(!(b&0x80)) // 0xxxxxxx
			{
				c = b&0x7f;
			}
			else if((b&0xe0) == 0xc0) // 110xxxxx 10xxxxxx
			{
				if(++i >= len) break;
			}
			else if((b&0xf0) == 0xe0) // 1110xxxx 10xxxxxx 10xxxxxx
			{
				if(++++i >= len) break;
			}
			m_text += c;
		}
	}
	else if(m_encoding == LE16)
	{
		WORD w;
		for(i = 0; i < len; i+=2)
		{
			memcpy(&w, pByte+i, 2);
			char c = '?';
			if(!(w&0xff00)) c = w&0xff;
			m_text += c;
		}
	}
	else if(m_encoding == BE16)
	{
		WORD w;
		for(i = 0; i < len; i+=2)
		{
			memcpy(&w, pByte+i, 2);
			char c = '?';
			if(!(w&0xff)) c = w>>8;
			m_text += c;
		}
	}
#endif
}

BOOL CWebText::CheckType(BYTE* pByte, int len, int& encoding, int& offset)
{
	offset = 0;
	encoding = ASCII;
	if(len >= 2)
	{
		WORD w;
		memcpy(&w, pByte, 2);

		if(w == 0xfeff)
		{
			encoding = LE16;
			offset = 2;
		}
		else if(w == 0xfffe)
		{
			encoding = BE16;
			offset = 2;
		}
		else if(w == 0xbbef)
		{
			if(len >= 3)
			{
				if(pByte[2] == 0xbf)
				{
					encoding = UTF8;
					offset = 3;
				}
			}
			else
				return FALSE;		// need 1 more byte
		}
	}

	return TRUE;
}