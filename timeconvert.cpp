#include "fish_common.h"
#include "fish_def.h"

/**************************************************************************
 * written by moonknit
 *
 * @history
 * created 2005-10
 *
 * @Description
 * COleDateTime ������ CString �������� ��ȯ���ش�.
 * ���������� �ð������� ��� _T("0")�� ��ȯ�Ѵ�.
 * see FISH_DT_TEMPLETE
 *
 * @Parameters
 * (in COleDateTime) t - ��ȯ�� �ð�
 *
 * @Returns
 * (CString) ��ȯ�� �ð�
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
 * CString ������ �ð��� COleDateTime ������ ��ȯ���ش�.
 * ��ȯ�� ������ ��� COleDateTime�� ������� m_dt�� 0���� �����Ѵ�.
 * see FISH_DT_TEMPLETE
 *
 * @Parameters
 * (in CString) s - ��ȯ�� �ð�
 * (out COleDateTime&) t - ��ȯ�� �ð�
 *
 * @Returns
 * (BOOL) ��ȯ ��������
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
 * COleDateTime�� �ð��� time_t �������� �����Ͽ��ش�.
 *
 * @Parameters
 * (in const COleDateTime&) t - ������ �ð�
 *
 * @Returns
 * (time_t) ����� �ð�
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

