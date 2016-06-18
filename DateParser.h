// CDateParser.h: interface for the CDateParser class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable:4786)

#if !defined(AFX_CDateParser1_H__2DE14066_D12C_41E2_8323_41F9C9F0610D__INCLUDED_)
#define AFX_CDateParser1_H__2DE14066_D12C_41E2_8323_41F9C9F0610D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
CDateParser
�� Ŭ������ RSS �ǵ忡 �����ϴ� �پ��� ������ ��¥�� COleDateTime Ŭ������ ���·� �����Ͽ�,
RSS���������� ����� �����ϰ� �Ϸ��� ���̴�.

  Note
	COleDateTime ��ü�� �ٷ�� �⺻���� GMT ���� UTC (GMT+00:00)�� ���߾��� �ִ�.
	���� ���α׷��������� �� �������� ������ ������ ���� �� ���� ��ȯ�ؾ��Ѵ�.

  Remarkable Param
	getDate �� 2��° ���ڷ� 
	RSS2.0 RFC822 �� ��� 1
	ATOM �� ��� 2
	��Ÿ ������ ��� possibleDateFormats �迭�� �����ؼ� �˸��� ���� ���ڷ� �ѱ�� �ȴ�.
**/

//���� ��� ������ �ʱ�ȭ
#define DATE_FORMAT 0
#define DATE_LONG_FORMAT 1
#define DATE_TIME_FORMAT 2
#define DATE_TIME_LONG_FORMAT 3

#define RSS_2_DATE_TYPE		1
#define ATOM_DATE_TYPE		2

class CDateParser : public CObject  
{
public:
	static const CString possibleDateFormats[];

public:
	
	static BOOL getDate(CString& strDate, COleDateTime& result);
	static CString formatDate(COleDateTime& time, BOOL longDateFormat = false, BOOL withTime = false);
	static CString dateToFileName(const CString& date);
	CDateParser();
	virtual ~CDateParser();

private:
	static CString attachYearPrefix(const CString& str);
	static CString monthAlphaToNum(const CString&);
	static CString substring(const CString&, const int startIndex, const int endIndex);
	static CString substring(const CString& str, const int startIndex);
	static CString convertWellFormDateString(const CString str, const int kindDateForm = RSS_2_DATE_TYPE);
};
#endif // !defined(AFX_CDateParser1_H__2DE14066_D12C_41E2_8323_41F9C9F0610D__INCLUDED_)
