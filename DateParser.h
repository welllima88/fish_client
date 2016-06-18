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
본 클래스는 RSS 피드에 존재하는 다양한 형태의 날짜를 COleDateTime 클래스의 형태로 변경하여,
RSS리더에서의 사용을 용이하게 하려고 함이다.

  Note
	COleDateTime 객체가 다루는 기본적인 GMT 값은 UTC (GMT+00:00)에 맞추어져 있다.
	따라서 프로그램내에서는 각 윈도우의 로케일 설정에 따라서 그 값을 변환해야한다.

  Remarkable Param
	getDate 의 2번째 인자로 
	RSS2.0 RFC822 의 경우 1
	ATOM 의 경우 2
	기타 포멧의 경우 possibleDateFormats 배열을 참조해서 알맞은 값을 인자로 넘기면 된다.
**/

//정적 멤버 변수의 초기화
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
