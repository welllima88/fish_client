#include "fish_common.h"
#include "fish_def.h"

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * COleDateTime 정보를 CString 형식으로 변환해준다.
 * 비정상적인 시간정보인 경우 _T("0")을 반환한다.
 * see FISH_DT_TEMPLETE
 *
 * @Parameters
 * (in COleDateTime) t - 변환할 시간
 *
 * @Returns
 * (CString) 변환된 시간
 **************************************************************************/
const CString ConvertTimetoString(const COleDateTime& t)
{
	if(t.m_dt == 0) return _T("0");

	return t.Format(FISH_DT_TEMPLETE);
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * CString 형식의 시간을 COleDateTime 정보로 변환해준다.
 * 변환에 실패할 경우 COleDateTime의 멤버변수 m_dt를 0으로 설정한다.
 * see FISH_DT_TEMPLETE
 *
 * @Parameters
 * (in CString) s - 변환할 시간
 * (out COleDateTime&) t - 변환된 시간
 *
 * @Returns
 * (BOOL) 변환 성공여부
 **************************************************************************/
BOOL ConvertStringtoTime(const CString& s, COleDateTime& t)
{
	if(s == _T("0"))
	{
		t.m_dt = 0;
		return FALSE;
	}

	try
	{
		if(!t.ParseDateTime(s))
		{
			t.m_dt = 0;
			t.SetStatus(COleDateTime::valid);
			return FALSE;
		}
	}
	catch (...)
	{
		t.m_dt = 0;
		t.SetStatus(COleDateTime::valid);
		return FALSE;
	}

	return TRUE;
}

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * COleDateTime의 시간을 time_t 형식으로 변경하여준다.
 *
 * @Parameters
 * (in const COleDateTime&) t - 변경할 시간
 *
 * @Returns
 * (time_t) 변경된 시간
 **************************************************************************/
time_t ConvertDateTimeToTimeT(const COleDateTime& t)
{
	if(t.GetStatus() != COleDateTime::valid) return -1;

	struct tm tmptm;

	tmptm.tm_isdst	= 0;
	tmptm.tm_hour	= t.GetHour();
	tmptm.tm_min	= t.GetMinute();
	tmptm.tm_sec	= t.GetSecond();
	tmptm.tm_year	= t.GetYear() - 1900;
	tmptm.tm_mon	= t.GetMonth() - 1;
	tmptm.tm_mday	= t.GetDay();
//	tmptm.tm_wday	= t.GetDayOfWeek();
//	tmptm.tm_yday	= t.GetDayOfYear();

	time_t result = mktime(&tmptm);

	return result;
}

