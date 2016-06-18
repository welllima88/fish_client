/********************************************************************************

	Gnucleus - A node application for the gnutella network
    Copyright (C) 2001 John Marshall

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	For support, questions, comments, etc...
	E-Mail: 
		swabby@c0re.net
	
	Address:
		21 Cadogan Way
		Nashua, NH, USA 03062 

********************************************************************************/


#include "stdafx.h"
#include "Conversions.h"
#include "GnuDefines.h"

// Get rid of ugly warnings
#pragma warning (disable : 4786)

CString DWrdtoStr(UINT in)
{
	char buff[16];

	::sprintf (buff, "%u", in);

	return buff;
}

CString DWrd64toStr(DWORD64 in)
{
	char buff[32];

	::sprintf (buff, "%uI64", in);

	return buff;
}

/*
old one
removed by moonknit 2005-12-20
char* IPtoStr(IP in)
{
	char* buffer;
	buffer = inet_ntoa(*(in_addr *)&in);

	return buffer;   
}
*/

/**************************************************************************
 * IPtoStr
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-20
 *
 * @Description
 * union data IP (IP Address data)를 CString 형식으로 변환한다.
 *
 * @Parameters
 * (in IP) in - 스트링으로 변환할 IP
 *
 * @Result
 * (CString) 스트링으로 변환된 IP
 **************************************************************************/
CString IPtoStr(IP in)
{
	CString result;
	result.Format(_T("%d.%d.%d.%d"), in.a, in.b, in.c, in.d);

	return result;
}

/*
old one
removed by moonknit 2005-12-20
IP StrtoIP(const char* in)
{
	IP out;

	out.S_addr = inet_addr(in);		// standard WinSock2 function

	return out;	
}
*/

/**************************************************************************
 * IPtoStr
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-20
 *
 * @Description
 * CString 형식의 Host IP를 union IP 형식으로 변환한다.
 *
 * @Parameters
 * (in CString) in - 형식 변환할 스트링
 *
 * @Result
 * (IP) union IP 형식의 Host IP 정보
 **************************************************************************/
IP StrtoIP(CString in)
{
	IP out;

	_stscanf((LPCTSTR) in, _T("%d.%d.%d.%d"), &out.a, &out.b, &out.c, &out.d);

	return out;
}

CString ExIPtoStr(ExIP in)
{
	CString out;
	out.Format(_T("%d.%d.%d.%d:"), in.a, in.b, in.c, in.d);

	out.Replace(_T("-1"), _T("*"));
	if(in.mode == DENY)
		out.Replace(_T(":"), _T(":Deny"));
	else
		out.Replace(_T(":"), _T(":Allow"));

	return out;
}

ExIP StrtoExIP(CString in)
{
	TCHAR rawMode[8] = _T("");

	in.Replace(_T("*"), _T("-1"));

	int  a = 0, b = 0, c = 0, d = 0;

	::_stscanf((LPCTSTR) in, _T("%d.%d.%d.%d:%s"), &a, &b, &c, &d, rawMode);

	ExIP out;
	out.a = a;	out.b = b;	out.c = c;	out.d = d;
	
	CString mode(rawMode);

	mode.MakeUpper();
	if(mode == _T("ALLOW"))
		out.mode = ALLOW;
	else
		out.mode = DENY;

	return out;
}

CString CommaIze(CString in)
{
	if (in.GetLength() > 3)
		return CommaIze(in.Left(in.GetLength() - 3)) + _T(",") + in.Right(3);
	else
		return in;
}

CString InsertDecimal(double dNumber)
{
	if(dNumber <= 0.00)
		return _T("0.00");

	int    decimal, sign;
	//char*  buffer;

	CString strNumber( _fcvt( dNumber, 2, &decimal, &sign));
	
	if(decimal == 0)
		strNumber.Insert(0, _T("0."));
	else if(decimal == -1)
		strNumber.Insert(0, _T("0.0"));
	else if(decimal < -1)
		return _T("0.00");
	else
		strNumber.Insert(decimal, _T("."));

	return strNumber;
}

CString GetPercentage(DWORD dWhole, DWORD dPart)
{
	CString result = _T("0.00");

	if(dPart > dWhole)
		dPart = dWhole;

	if(dWhole)
	{
		result = DWrdtoStr(dPart * 10000 / dWhole);

		if(result.GetLength() > 2)
			result.Insert( result.GetLength() - 2, _T("."));
		else
		{
			switch(result.GetLength())
			{
			case 2:
				result.Insert(0, _T("0."));
				break;
			case 1:
				result.Insert(0, _T("0.0"));
				break;
			default:
				result = _T("0.00");
				break;
			}
		}
	}

	return result + _T(" %");
}

CString IncrementName(CString FileName)
{
	CString Front = FileName;
				
	int dotpos = FileName.ReverseFind(_T('.'));
	if(dotpos != -1)
		Front = FileName.Left(dotpos);
	
	int copy = 1;
	int spacepos = Front.ReverseFind(_T(' '));

	if(spacepos != -1)
	{
		copy = _ttoi( Front.Right(Front.GetLength() - spacepos - 1));
		
		if(copy == 0)
			Front += _T(" 1");
		else
		{
			copy++;
			Front = Front.Left(spacepos) + _T(" ") + DWrdtoStr(copy);
		}
	}
	else
		Front += _T(" 1");

	CString NewFile = Front;
	
	if(dotpos != -1)
		Front += FileName.Right( FileName.GetLength() - dotpos);

	return Front;
}

CString GetNewFileName(CString sFileName, int nCnt)
{
	CString sCnt;
	sCnt.Format(_T("_%d"), nCnt);
	int pos = sFileName.ReverseFind(_T('.'));
	if(pos)
	{
		sFileName.Insert(pos, sCnt);
	}
	else
		sFileName += sCnt;

	return sFileName;
}

DWORD GetSpeedinBytes(CString Speed)
// The protocol is messed, bytes are bits 
{
	if(Speed == _T("Cellular Modem"))
		return 0;
	if(Speed == _T("14.4 Modem"))
		return 14;
	if(Speed == _T("28.8 Modem"))
		return 28;
	if(Speed == _T("56K Modem"))
		return 53;
	if(Speed == _T("ISDN"))
		return 128;
	if(Speed == _T("Cable"))
		return 384;
	if(Speed == _T("DSL"))
		return 768;
	if(Speed == _T("T1"))
		return 1500;
	if(Speed == _T("T3 (or Greater)"))
		return 45000;
	
	return _ttol(Speed);
}

CString GetSpeedString(DWORD dwSpeed)
// The protocol is messed, bytes are bits 
{
	if(dwSpeed >= 45000)
		return _T("T3 (or Greater)");
	if(dwSpeed >= 1500)
		return _T("T1");
	if(dwSpeed >= 768)
		return _T("DSL");
	if(dwSpeed >= 384)
		return _T("Cable");
	if(dwSpeed >= 128)
		return _T("ISDN");
	if(dwSpeed >= 53)
		return _T("56K Modem");
	if(dwSpeed >= 28)
		return _T("28.8 Modem");
	if(dwSpeed >= 14)
		return _T("14.4 Modem");
	else
		return _T("Cellular Modem");
}

bool ValidVendor(CString VendorID)
{
	if(GetVendor(VendorID) == _T(""))
		return false;
	
	return true;
}

CString GetVendor(CString VendorID)
{
	VendorID.MakeUpper();

	if(VendorID == _T("BEAR")) return _T("BearShare");

	if(VendorID == _T("CULT")) return _T("Cultiv8r");
	
	if(VendorID == _T("GNOT")) return _T("Gnotella");
	
	if(VendorID == _T("GNUC")) return _T("Gnucleus");

	if(VendorID == _T("GNUT")) return _T("Gnut");

	if(VendorID == _T("GTKG")) return _T("Gtk-Gnutella");

	if(VendorID == _T("HSLG")) return _T("Hagelslag");
	
	if(VendorID == _T("LIME")) return _T("LimeWire");
	
	if(VendorID == _T("MACT")) return _T("Mactella");
	
	if(VendorID == _T("MNAP")) return _T("MyNapster");

	if(VendorID == _T("MRPH")) return _T("Morpheus");
	
	if(VendorID == _T("NAPS")) return _T("NapShare");

	if(VendorID == _T("OCFG")) return _T("OCFolders");

	if(VendorID == _T("SNUT")) return _T("SwapNut");

	if(VendorID == _T("TOAD")) return _T("ToadNode");

	return _T("Unknown");
}

CString GetMapColor(int Hops)
{
	CString color;

	switch(Hops)
	{
	case 1:
		color = _T("orange");
		break;
	case 2:
		color = _T("yellow");
		break;
	case 3:
		color = _T("green");
		break;
	case 4:
		color = _T("blue");
		break;
	case 5:
		color = _T("purple");
		break;
	case 6:
		color = _T("brown");
		break;
	default:
		color = _T("black");
		break;
	}

	return _T("color=") + color;
}

// Converts a string representation to a CTime
CTime	StrToCTime(CString& str)
{
	CTime temp(0);
	
	// 0         1         2 
	// 0123456789012345678901
	// YYYY-MM-DDTHH:MM+HH:MM
	// or
	// YYYY-MM-DDTHH:MMZ

	if (str[4] != _T('-') || str[7] != _T('-')
		|| tolower(str[10]) != _T('t') || str[13] != _T(':'))
	{
		return temp;
	}

	int Year  = _ttoi(str.Mid(0,4));
	int Month = _ttoi(str.Mid(5,2));
	int Day   = _ttoi(str.Mid(8,2));
	int Hour  = _ttoi(str.Mid(11,2));
	int Min   = _ttoi(str.Mid(14,2));

	// Check for valid values
	CTime CurrentTime = CTime::GetCurrentTime();

	if(Year  < 1970 || Year  > CurrentTime.GetYear() ||
	   Month < 1    || Month > 12 ||
	   Day   < 1    || Day   > 31 )
	{
	   return temp;
	}

	temp = CTime(Year, Month, Day, Hour, Min, 0, 0);

	if (_totlower(str[16]) == _T('z'))
	{
		return temp;
	}

	int tzh   = _ttoi(str.Mid(16,3));
	int tzm   = _ttoi(str.Mid(20,2));


	return temp;
}

// Convert Ctime to String
CString CTimeToStr(CTime& time)
{
	// The Format as per http://www.w3.org/TR/NOTE-datetime 
	// YYYY-MM-DDTHH:MMTZD
	CString TimeStr;

	TimeStr = time.Format(_T("%Y-%m-%dT%H:%MZ"));

	// CTime doesn't store time zone so have to convert all times to UTC
	//	TimeStr += GetTimeZoneStr();

	return TimeStr;
}

CString GetTimeZoneStr(void)
{
	CString TZStr;

	int TZHours = abs(_timezone) / (60 * 60);
	int TZMins = ((abs(_timezone) / 60) - (TZHours * 60));

	TZStr.Format(_T("%c%2.2d:%2.2d"), (_timezone < 0 ? _T('-') : _T('+')), TZHours, TZMins);

	return TZStr;
}

CTimeSpan LocalTimeZone()
{
	int TZHours = _timezone / (60 * 60);
	int TZMins = ((abs(_timezone) / 60) - (abs(TZHours) * 60));

	return CTimeSpan(0, TZHours, TZMins, 0);
}

// 전송 상태의 스트링 값으로 반환한다.
// 기본 값은 STR_TRASFER_UNKNOWN을 반환한다.
CString GetTRStatusString(int nStatus)
{
	switch(nStatus)
	{
	// download state
	case TRANSFER_NEEDSOURCE:
		return STR_TRANSFER_NEEDSOURCE;
	case TRANSFER_STOP:
		return STR_TRANSFER_STOP;
	case TRANSFER_QUEUED:
		return STR_TRANSFER_QUEUED;
	case TRANSFER_CONNECTING:
		return STR_TRANSFER_CONNECTING;
	case TRANSFER_RECEIVING:
		return STR_TRANSFER_RECEIVING;
	case TRANSFER_CLOSED:
		return STR_TRANSFER_CLOSED;
	case TRANSFER_COOLDOWN:
		return STR_TRANSFER_COOLDOWN;
	case JOIN_CHUNK:
		return STR_JOIN_CHUNK;
	case TRANSFER_DELETED:
		return STR_TRANSFER_DELETED;
	// upload state
	case TRANSFER_CONNECTED:
		return STR_TRANSFER_CONNECTED;
	case TRANSFER_SENDING:
		return STR_TRANSFER_SENDING;
	case TRANSFER_PUSH:
		return STR_TRANSFER_PUSH;
	case TRANSFER_CANCEL:
		return STR_TRANSFER_CANCEL;
	}

	return STR_TRANSFER_UNKNOWN;
}