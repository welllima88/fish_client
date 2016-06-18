// CDateParser.cpp: implementation of the CDateParser class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "DateParserTest.h"
#include "DateParser.h"
#include <map>
#include <string>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const CString CDateParser::possibleDateFormats[] = {
	/** RFC 1123 with 2-digit Year */
	_T("EEE, dd MMM yy HH:mm:ss z"),

	/** RFC 1123 with 4-digit Year */
	_T("EEE, dd MMM yyyy HH:mm:ss z"),

	/** ISO 8601 */
	_T("yyyy-MM-ddTHH:mm:ssz"),

	/** RFC 1123 without Day Name */
	_T("dd MMM yyyy HH:mm:ss z"),
	
	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHH:mm:ssZ"),

	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHH:mm:sszzzz"),

	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHH:mm:ss z"),

	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHH:mm:ss"),

	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHHmmss.SSSz"),

	/** ISO 8601 slightly modified */
	_T("yyyy-MM-ddTHH:mm:ss"),

	/** ISO 8601 w/o seconds */
	_T("yyyy-MM-ddTHH:mmZ"),

	/** Simple Date Format */

	_T("EEE, dd MMM yyyy HH:mm"),

	_T("yyyy-MM-dd HH:mm"),

	_T("yyyy-MM-dd"),	

	_T(""),
 
};

#define ISNUMBER(character) ('0' <= (character) && '9' >= (character))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDateParser::CDateParser()
{

}

CDateParser::~CDateParser()
{

}

/**
dateToFileName 인자로 받은 날짜의 이름에서 특수문자를 제거한다.
@param date 날짜 스트링 참조값

@return 특수문자가 제거된 날짜 스트링의 참조값
**/
CString CDateParser::dateToFileName(const CString& date)
{
	CString ret = date;
	CString separator= _T("_");

	ret.Replace(_T(" "), separator);
	ret.Replace(_T("\\"), separator);
	ret.Replace(_T(":"), separator);
	ret.Replace(_T("-"), separator);
	ret.Replace(_T("/"), separator);
	return ret;
}

/**
formatDate 인자로 전달된 COleDateTime 객체를 지정된 형식의 포멧으로 스트링을 반환한다.
@param time 인자로 전달될 COleDateTime 객체
@param longDateFormat 반환할 날짜의 형태가 긴형태인가?
@param withTime 반환할 스트링에 시간을 표시할 것인가?

@return 지정된 포맷에 맞게 변환되어 반환는 CString
**/
CString CDateParser::formatDate(COleDateTime &time, BOOL longDateFormat, BOOL withTime)
{
	/** Long Date Format with Time */
	if (longDateFormat && withTime)
		return time.Format(_T("%#c"));

	/** Short Date Format without Time */
	else if (withTime)
		return time.Format(_T("%A, %B %d, %Y %h:%M"));

	/** Long Date Format Without Time */
	else if (longDateFormat)
		return time.Format(_T("%A, %B %d, %Y"));

	/** Short Date Format Without Time */
	return time.Format(_T("%m/%d/%y"));
}

/**
getDate 인자로 전달된 날짜 스트링을 COleDateTime 객체의 형태로 반환
@param strDate 스트링 형태의 날짜

@return COleDateTime 형태로 변환된 객체.
**/
BOOL CDateParser::getDate(CString& strDate, COleDateTime& result)
{
	enum
	{
		AP_NULL = 0,
		AM,
		PM
	};

	//{{ BEGIN Various Exception Handling
	CString dateEnd;	// GMT를 기록할 변수

	/** Return in case the string date is not set */
	if (!(&strDate != NULL && strDate.GetLength() > 0))
		return COleDateTime();

	// 오전/오후 정보 처리 젠장!!
	int nAMPM = AP_NULL;
	int nPos = strDate.Find(_T("오전"));
	if(nPos >= 0) nAMPM = AM;
	else
	{
		nPos = strDate.Find(_T("오후"));
		if(nPos > 0) nAMPM = PM;
	}
	
	if ( nPos > 0)
	{
#ifdef _UNICODE
		strDate.Delete(nPos, 2);
#else
		strDate.Delete(nPos, 4);
#endif
		strDate.Replace(_T("  "), _T(" "));
	}

	// EEEEEEE, dd MMMMMMM yyyy HH:mm:ss z  ->  EEE, dd MMM yyyy HH:mm:ss z
	if (strDate.Find(',') > 3)
	{
		CString week, day, month, temp;		
		week	= strDate.Left(3) + ",";
		strDate = substring(strDate, strDate.Find(' '));
		strDate.TrimLeft();
		temp = week.Left(1);
		temp.MakeUpper();
		week = temp + week.Right(3);	

		day = strDate.Left(strDate.Find(' '));
		if(day.GetLength() == 1)
			day = _T("0") + day;		

		strDate = substring(strDate, strDate.Find(' '));
		strDate.TrimLeft();

		month = strDate.Left(3);
		strDate = substring(strDate, strDate.Find(' '));
		strDate.TrimLeft();
	
		temp = month.Left(1);
		temp.MakeUpper();
		month = temp + month.Right(2);

		strDate = week + _T(" ") + day + _T(" ") + month + _T(" ") + strDate;
	}

	strDate.TrimLeft(' ');
	strDate.TrimRight(' ');

	if (strDate.GetLength() > 10) {
		/** Open: deal with +4:00 (no zero before hour) */
		CString tmp = substring(strDate, strDate.GetLength() - 5);
		
		if ((tmp.Find(_T("+")) == 0 || tmp.Find(_T("-")) == 0) && tmp.Find(_T(":")) == 2) {
		
		CString sign = substring(strDate, strDate.GetLength() - 5, strDate.GetLength() - 4);

		strDate = substring(strDate, 0, strDate.GetLength() - 5) + 
			sign + _T("0") + 
			substring(strDate, strDate.GetLength() - 4);
		}

		/**
		 * try to deal with -05:00 or +02:00 at end of date replace with -0500 or
		 * +0200
		 */

		BOOL bNoTimeBand = TRUE;
		dateEnd = substring(strDate, strDate.GetLength() - 6);
		dateEnd.TrimLeft();
		CString GMT = _T("GMT");

//		TRACE(dateEnd);
		if ((dateEnd.Find(_T("-")) == 0 || dateEnd.Find(_T("+")) == 0) && dateEnd.Find(_T(":")) == 3) {			
			if (GMT.Compare(substring(strDate, strDate.GetLength() - 9, strDate.GetLength() - 6)) != 0) {
				CString oldDate = strDate;
				dateEnd = substring(dateEnd, 0, 3) + substring(dateEnd, 4);
				strDate = substring(oldDate, 0, oldDate.GetLength() - 6) + dateEnd;

				bNoTimeBand = FALSE;
			}
			else
				dateEnd = _T("");
		}
		else if(dateEnd.Find(_T("-")) == 0 || dateEnd.Find(_T("+")) == 0)
		{			
			bNoTimeBand = FALSE;
		}

		if(bNoTimeBand)	dateEnd = _T("+0900");
	}

//	TRACE(_T("strDate : %s\r\n"), strDate);
	//}} END Various Exception Handling

	//{{BEGIN COleDateTime Parsing
	BOOL			bResult = FALSE;
	CString			strNew;

	
//	try
//	{
//		if(result.ParseDateTime(strDate) != 0)
//			bResult = TRUE;
//	}
//	catch (CException&)
//	{
//	}

	if(!bResult)
	{
		for(int i = 0; possibleDateFormats[i] != _T(""); )
		{
			/** Try to parse the date */
			strNew = convertWellFormDateString(strDate, i);

			if(strNew != _T(""))
			{
//				TRACE(strNew);

				try
				{
					if(result.ParseDateTime(strNew) != 0) 
					{
						bResult = TRUE;
						break;
					}
				}
				catch (COleException*)
				{
				}
				catch (CMemoryException*)
				{
				}
			}

			++i;
		}
	}

	if(!bResult)
	{
		return FALSE;
	}
	//}}END COleDateTime Parsing

	//{{ BEGIN XML의 정보를 기반으로 GMT를 UTC로 바꾼다.

//	TRACE(_T("dateEnd %s\r\n"), dateEnd);
	if(nAMPM != AP_NULL)
	{
		int APInc = 0;
		switch(nAMPM)
		{
		case AM:
			if(result.GetHour() == 12) APInc = -12;
			break;
		case PM:
			if(result.GetHour() != 12) APInc = 12;
			break;
		default:
			break;
		}

		COleDateTimeSpan apDifference;

		if(APInc != 0)
		{
			apDifference.SetDateTimeSpan(0, APInc, 0, 0);
			result = result + apDifference;
		}
	}


	if(!dateEnd.IsEmpty())
	{
		CString gmtHour = substring(dateEnd, 1, 3);
		CString gmtMinute = substring(dateEnd, 3, 5);

		COleDateTimeSpan gmtDifference;

		gmtDifference.SetDateTimeSpan(0, _ttoi(gmtHour), _ttoi(gmtMinute), 0);

		if (dateEnd.Find(_T("-")) == 0)
			result = result + gmtDifference;

		if (dateEnd.Find(_T("+")) == 0)
			result = result - gmtDifference;
	}
	
	//}} END
	return TRUE;
}

/**
substring 인자로 전달된 스트링에서 추출한 서브 스트링을 만든다. Java String.substring 동작방식과 동일하게 작성
@param str 원본 문자열
@param startIndex 서브스트링의 첫번째 문자가 있는 곳 (included)

@return 만들어진 서브스트링을 반환
**/
CString CDateParser::substring(const CString& str, const int startIndex)
{
	return CDateParser::substring(str, startIndex, str.GetLength());
}

/**
substring 인자로 전달된 스트링에서 일부 문자열을 추출한 서브 스트링을 만든다. Java String.substring 동작방식과 동일하게 작성
@param str 원본 문자열
@param startIndex 서브스트링의 첫번째 문자가 있는 곳 (included)
@param endIndex 서브스트링의 마지막 문자가 있는 곳 (excluded)

@return 만들어진 서브스트링을 반환
**/
CString CDateParser::substring(const CString& str, const int startIndex, const int endIndex)
{
	int iter = 0;
	TCHAR* tmp = new TCHAR [ endIndex - startIndex + 1 ];
	
	while ( iter < endIndex - startIndex ) {
		tmp[iter] = str[startIndex + iter];
		iter++;
	}

	tmp[iter] = _T('\0');

	CString ret = tmp;
	delete[] tmp;

	return ret;
}

/**
convertWellFormDateString 인자로 전달된 날짜 문자열을 COleDateTime::ParseDateTime() 이 받을 수 있는 형식으로 변환
@param str 원본 문자열

@return 원본 문자열이 COleDateTime 에 알맞게 변환된 문자열
**/
CString CDateParser::convertWellFormDateString(const CString str, const int kindDateForm)
{
	// iterator
	int iter = 0, index=0;	// 폼 스트링의 인덱스
	int dateStr_index=0;	// 인자로 전달 받은 날짜 스트링의 인덱스

	map<tstring, CString> tmp;

	// 연도의 갯수가 2개인지 4개인지를 비교해서 에러 처리를 위한 부분
	int yCount = possibleDateFormats[kindDateForm].ReverseFind('y') - possibleDateFormats[kindDateForm].Find('y') + 1;

	while(dateStr_index < str.GetLength() && iter < possibleDateFormats[kindDateForm].GetLength()){
		//TRACE(_T("this : %c\n"), possibleDateFormats[kindDateForm][iter]);
		switch (possibleDateFormats[kindDateForm][iter]) {
			case 'y':
				if ( ISNUMBER( str[dateStr_index] )) {
					tmp[_T("year")].Insert(index, str[dateStr_index]);
					dateStr_index++;
				}
				break;

			case 'M':
				tmp[_T("month")].Insert(index, str[dateStr_index]);
				dateStr_index++;
				break;

			case 'd':
				if ( ISNUMBER( str[dateStr_index] )) {
					tmp[_T("day")].Insert(index, str[dateStr_index]);
					dateStr_index++;
				}
				else if(kindDateForm < 2)//날자형식이 0, 1일때 두자리 단위에서 0이 생략되고 자릿 수는 일치...
				{
					if(str[dateStr_index] == _T(' ') && str[dateStr_index + 2] == _T(' '))
					{
						tmp[_T("day")].Insert(index, str[dateStr_index]);
						dateStr_index++;
					}
				}
				break;

			case 'H':
				if ( ISNUMBER( str[dateStr_index] )) {
					tmp[_T("hour")].Insert(index, str[dateStr_index]);
					dateStr_index++;
				}
				break;

			case 'm':
				if ( ISNUMBER( str[dateStr_index] )) {
					tmp[_T("minute")].Insert(index, str[dateStr_index]);
					dateStr_index++;
				}
				break;

			case 's':
				if ( ISNUMBER( str[dateStr_index] )) {
					tmp[_T("second")].Insert(index, str[dateStr_index]);
					dateStr_index++;
				}
				break;

			case 'Z':
			case 'z':
				tmp[_T("timezone")].Insert(index, substring(str, dateStr_index, str.GetLength()));
				dateStr_index = str.GetLength();
				break;

			case ' ':
				if ( ISNUMBER( str[dateStr_index] ))		// yy, yyyy의 형식이 잘못 파싱되는 것을 막는다.
					return "";

			default:
				index = 0;
				dateStr_index++;
				break;
		}
		iter++;
		index++;
	}

/*
#ifdef _DEBUG
	TRACE(_T("M: %s\n"), tmp[_T("month")]);
	TRACE(_T("D: %s\n"), tmp[_T("day")]);
	TRACE(_T("Y: %s\n"), tmp[_T("year")]);
	TRACE(_T("H: %s\n"), tmp[_T("hour")]);
	TRACE(_T("M: %s\n"), tmp[_T("minute")]);
	TRACE(_T("S: %s\n"), tmp[_T("second")]);
#endif
*/
	if(tmp[_T("hour")] == _T("") && tmp[_T("minute")] == _T("") && tmp[_T("second")] == _T("") && kindDateForm >= 10)
		return monthAlphaToNum(tmp[_T("month")]) + _T("/") + tmp[_T("day")] + _T("/") + attachYearPrefix(tmp[_T("year")]) + _T(" ") + _T("00:00:00");
	else if(kindDateForm == 11 || kindDateForm == 12)
		return monthAlphaToNum(tmp[_T("month")]) + _T("/") + tmp[_T("day")] + _T("/") + attachYearPrefix(tmp[_T("year")]) + _T(" ") + 
			tmp[_T("hour")] + _T(":") + tmp[_T("minute")] + _T(":") + _T("00");
	else
		return monthAlphaToNum(tmp[_T("month")]) + _T("/") + tmp[_T("day")] + _T("/") + attachYearPrefix(tmp[_T("year")]) + _T(" ") + 
			tmp[_T("hour")] + _T(":") + tmp[_T("minute")] + _T(":") + tmp[_T("second")];
}

/**
monthAlphaToNum 영문자로 표기된 month 표기를 숫자 2개의 표현으로 변경하여 리턴
@param str 달을 나타내는 문자열

@return 달을 나타내는 숫자 문자열
**/
CString CDateParser::monthAlphaToNum(const CString& str)
{
	if (str == "Jan" || str == "01" || str == "1") return "01";
	if (str == "Feb" || str == "02" || str == "2") return "02";
	if (str == "Mar" || str == "03" || str == "3") return "03";
	if (str == "Apr" || str == "04" || str == "4") return "04";
	if (str == "May" || str == "05" || str == "5") return "05";
	if (str == "Jun" || str == "06" || str == "6") return "06";
	if (str == "Jul" || str == "07" || str == "7") return "07";
	if (str == "Aug" || str == "08" || str == "8") return "08";
	if (str == "Sep" || str == "09" || str == "9") return "09";
	if (str == "Oct" || str == "10") return "10";
	if (str == "Nov" || str == "11") return "11";
	if (str == "Dec" || str == "12") return "12";

	return "00";
}

/**
attachYearPrefix 2자리로 만들어지거나 변경되어야할 필요가 있는 연의 표현식을 필요한 형식으로 변환한다.
@param remark 판단의 기준은 우선적으로 1970년을 기준으로 한다. 
			70이하의 수가 입력으로 들어올경우 접두어로 20을 붙여반환. 그 이상일 경우 19를 붙여 반환한다.
@param str 인자로 받는 날짜 표기형식
@return 변환된 날짜 표기형식이 반환된다.
**/
CString CDateParser::attachYearPrefix(const CString& str)
{
	if (str.GetLength() != 4) {
		CString ret = str;
		int i = _ttoi(str);
		if (70 <= i && 99 >= i) ret.Insert(0, _T("19"));
		if (00 <= i && 70 >= i) ret.Insert(0, _T("20"));
		return ret;
	} else
		return str;
}
