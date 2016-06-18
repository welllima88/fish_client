// WebText.h: interface for the CWebText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEBTEXT_H__FE5B1DFC_F841_490E_9B51_2DDE4A724623__INCLUDED_)
#define AFX_WEBTEXT_H__FE5B1DFC_F841_490E_9B51_2DDE4A724623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

class CWebText
{
	enum
	{
		ASCII = 0,
		UTF8,
		BE16,
		LE16
	};
	CString	m_text;
	int m_encoding;
	int m_offset;
public:
	void AddText(BYTE* pByte, int len);
	void SetText(BYTE* pByte, int len);
	void Clear() { m_text.Empty(); }
	CString GetText() { return m_text; } 
	int GetOffset() { return m_offset; }
	BOOL CheckType(BYTE* pByte, int len, int& encoding, int& offset);

	CWebText();
	virtual ~CWebText();

};

#endif // !defined(AFX_WEBTEXT_H__FE5B1DFC_F841_490E_9B51_2DDE4A724623__INCLUDED_)
