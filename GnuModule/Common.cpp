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
#include "Common.h"

int  ResultGroup::SortBy = 0;

DWORD AssignThreadToCPU(CWinThread *pThread, DWORD cpuNumber)
{
	DWORD dwErr = 0;
	
	// Legal thread pointer is needed ;) and Only Windows NT/2000/XP support multiple CPU
	if(pThread != NULL && GetVersion() < 0x80000000)			 
	{
	
		// Do the simple way if checking the number of CPUs: Get it from the environment ;)
		long lNrOfCpus = 0;
		char *pEnvData = getenv("NUMBER_OF_PROCESSORS");
	
		
		if(pEnvData != NULL) 
			lNrOfCpus = atoi(pEnvData);

		if(lNrOfCpus == 0) 
			lNrOfCpus = 1;


		// If only one Cpu, then forget it ;)
		// Otherwise assign the affinity (Note: We assume there are max 4 Cpus
		if (lNrOfCpus > 1) 
		{
			DWORD dwProcAffinityMask = cpuNumber;
	
			if (!SetThreadAffinityMask(pThread->m_hThread, dwProcAffinityMask)) 
			{
				// Failure will be returned as errorcode ;)
				dwErr = GetLastError();
			}
		}
	}

	return dwErr;
}

void SetRandColor(COLORREF &Color)
{
	int red   = rand() % 255 + 0;
	int green = rand() % 255 + 0;
	int blue  = rand() % 255 + 0;
	
	Color = RGB(red, green, blue);
}

CString	GetFileError(CFileException* error)
{
	if(!error)
		return "File Error Unknown";

	switch( error->m_cause )
	{

	case CFileException::none:
        return "Error Not Detected";
		break;
	case CFileException::generic:
        return "Generic Error";
		break;
	case CFileException::fileNotFound:
        return "File Not Found";
		break;
	case CFileException::badPath:
        return "Bad Path";
		break;
	case CFileException::tooManyOpenFiles:
        return "Too Many Open Files";
		break;
	case CFileException::accessDenied:
        return "Access Denied";
		break;
	case CFileException::invalidFile:
        return "Invalid File";
		break;
	case CFileException::removeCurrentDir:
        return "Cannot Remove Directory";
		break;
	case CFileException::directoryFull:
        return "Directory Full";
		break;
	case CFileException::badSeek:
        return "Error Seeking";
		break;
	case CFileException::hardIO:
        return "Hardware Error";
		break;
	case CFileException::sharingViolation:
        return "Sharing Violation";
		break;
	case CFileException::lockViolation:
        return "Lock Violation";
		break;
	case CFileException::diskFull:
        return "Disk Drive Full";
		break;
	case CFileException::endOfFile:
        return "End of File";
		break;
	}

	return "File Error Unknown";
}

GnuMessage::GnuMessage(int nType, int nLength)
{
	Length = nLength;
	Type   = nType;

	Packet = NULL;
}

GnuMessage::~GnuMessage()
{
	if(Packet)
	{
		delete [] Packet;
		Packet = NULL;
	}
}


PriorityPacket::PriorityPacket(int nType, int nLength)
{
	Length = nLength;
	Type   = nType;
	Packet = NULL;
}

PriorityPacket::~PriorityPacket()
{
	if(Packet)
	{
		delete [] Packet;
		Packet = NULL;
	}
}

// Return the first string before the delim char
// and remove the string from the main string
// Repeat calls to return all items until empty string is returned
CString ParseString( CString &Str, TCHAR delim /* = ',' */)
{
	CString RetStr;

	if (!Str.IsEmpty())
	{
		int delimpos = Str.Find(delim);
        if (delimpos == -1)
        {
			RetStr = Str;
			Str = _T("");
		}
        else
        {
            RetStr = Str.Left(delimpos);
            Str = Str.Mid(delimpos + 1);
        }
    }
   
    return RetStr;
}

//
// Build an AltLocation from a string
//
AltLocation& AltLocation::operator=(CString& str)
{
	Clear();

	str.TrimLeft();

	CString temp, newStr = str;
	newStr.MakeLower();

	// Strip the http: bit
	temp = ParseString(newStr, '/');
	if (temp != "http:")
		return *this;

	// Strip the second '/'
	temp = ParseString(newStr, '/');

	// Next should be the "host:Port" section
	HostPort = ParseString(newStr, '/');
	if (HostPort.Host.IsEmpty())
		return *this;

	temp = ParseString(newStr, '/');
	if (temp == "get")
	{
		// this is a /get/<index>/<FileName> string
		Index = _ttol(ParseString(newStr,'/'));
		Name  = ParseString(newStr, ' ');
		Name.Replace(_T("%20"), _T(" "));
	}
	else if (temp == _T("uri-res"))
	{
		// this is a /uri-res/N2R?urn:sha1:<hash> string
		temp = ParseString(newStr, ':');
		temp = ParseString(newStr, ':');
		Sha1Hash = ParseString(newStr, ' ');
	}

	if (!newStr.IsEmpty())
	{
		// we still have stuff left hopefully it is a timestamp
		HostPort.Time = StrToCTime(newStr);
	}

	return *this;
}

//
// Build an String from an AltLocation
//
CString AltLocation::GetString()
{
	// TODO: Support for get by hash only

	// Start with the address
	CString str;

	if (HostPort.Host.IsEmpty() || (Name.IsEmpty() && Sha1Hash.IsEmpty()))
	{
		// Invalid Altlocation
		return "";
	}
		
	str = _T("http://") + HostPort.GetString() + _T("/get/") + DWrdtoStr(Index) + _T("/") + Name;
	str.Replace(_T(" "), _T("%20"));

	// Timestamp
	str += " " + CTimeToStr(HostPort.Time);
	return str;
}

void enc_hostinfo(const char* ip, int port, int findex, char* result)
{
	if(!result || !result) return;

	srand(time(NULL));
	unsigned char r = (unsigned char) (rand() % 256);

	unsigned char c = 0;
	char	tmp[8];
	char	szport[16];
	char	szfindex[16];

	sprintf(result, "%02x", (unsigned char) r);

	for(int i = 0; i < strlen(ip); i++)
	{
		c = ip[i] + (i+1) * 5;
		c ^= (unsigned char) r;
		sprintf(tmp, "%02x", c);

		strcat(result, tmp);
	}

	strcat(result, ",");

	sprintf(szport, "%d", port);

	for(i = 0; i < strlen(szport); i++)
	{
		c = szport[i] + (i+1) * 5;
		c ^= r;
		sprintf(tmp, "%02x", c);

		strcat(result, tmp);
	}

	strcat(result, ",");

	sprintf(szfindex, "%d", findex);

	for(i = 0; i < strlen(szfindex); i++)
	{
		c = szfindex[i] + (i+1) * 5;
		c ^= r;
		sprintf(tmp, "%02x", c);

		strcat(result, tmp);
	}

}

void dec_hostinfo(const char* src, char* ip, int& port, int& findex)
{
	if(!ip || !src) return;

	char	szport[16];
	char	szfindex[16];
	char	sztemp[256];
	char	tmp[8];

	char*	pos = strchr(src, ',');
	char*	pos2;

	if(pos) pos2 = strchr(pos+1, ',');
	if(!pos || !pos2) return;

	unsigned char r = 0;
	unsigned char c = 0;

	strncpy(tmp, src, 2);
	tmp[2] = '\0';

	sscanf(tmp, "%02x", &r);

	memset(sztemp, 0, sizeof(sztemp));
	strncpy(sztemp, src+2, pos - src - 2);

	for(int i = 0; i < strlen(sztemp) / 2; i++)
	{
		strncpy(tmp, sztemp+i*2, 2);
		tmp[2] = '\0';

		sscanf(tmp, "%02x", &c);

		c ^= r;
		ip[i] = c - (i+1) * 5;
	}

	ip[i] = '\0';

	memset(szport, 0, sizeof(szport));
	memset(sztemp, 0, sizeof(sztemp));
	strncpy(sztemp, pos+1, pos2 - pos -1);

	for(i = 0; i < strlen(sztemp) / 2; i++)
	{
		strncpy(tmp, sztemp+i*2, 2);
		tmp[2] = '\0';

		sscanf(tmp, "%02x", &c);

		c ^= r;
		szport[i] = c - (i+1) * 5;
	}

	szport[i] = '\0';

	port = atoi(szport);

	memset(szfindex, 0, sizeof(szfindex));
	strcpy(sztemp, pos2+1);

	for(i = 0; i < strlen(sztemp) / 2; i++)
	{
		strncpy(tmp, sztemp+i*2, 2);
		tmp[2] = '\0';

		sscanf(tmp, "%02x", &c);

		c ^= r;
		szfindex[i] = c - (i+1) * 5;
	}

	szfindex[i] = '\0';

	findex = atoi(szfindex);
}

void PrintError(DWORD dwError, LPTSTR lpszError)
{
#ifdef _DEBUG
	if(!lpszError) return;

	lpszError[0] = _T('\0');

	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwError, 0, (LPTSTR) &lpMsgBuf, 0,	NULL );

	if(lpMsgBuf)
	{
		_tcscpy(lpszError, (LPTSTR) lpMsgBuf);
	}

	LocalFree( lpMsgBuf );
#endif
}

// GUID 생성 코드 ///////////////////////////////////////////////////////////////////////////////////
void GnuCreateGuid(GUID *pGuid)
{
	memset(pGuid, 0, sizeof(GUID));
	CoCreateGuid(pGuid);
	
	//byte* bGuid = (byte*) pGuid;
	//bGuid[8]  = 0xFF;
	//bGuid[15] = 0x00;
}

CString GuidtoStr(GUID inGuid)
{
	byte* bGuid = (byte*) &inGuid;
	
	CString strGuid;
	strGuid.Format(_T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
		bGuid[0],  bGuid[1],  bGuid[2],  bGuid[3],  bGuid[4],
		bGuid[5],  bGuid[6],  bGuid[7],  bGuid[8],  bGuid[9],
		bGuid[10], bGuid[11], bGuid[12], bGuid[13], bGuid[14],
		bGuid[15]);
	strGuid.MakeLower();
	
	return strGuid;
}

GUID StrtoGuid(CString strGuid)
{
	GUID outGuid = GUID_NULL;
	byte* bGuid = (byte*) &outGuid;

	int nGuid[16];

	_stscanf(strGuid, _T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
				&nGuid[0],  &nGuid[1],  &nGuid[2],  &nGuid[3],  &nGuid[4],
				&nGuid[5],  &nGuid[6],  &nGuid[7],  &nGuid[8],  &nGuid[9],
				&nGuid[10], &nGuid[11], &nGuid[12], &nGuid[13], &nGuid[14],
				&nGuid[15]);

	for(int i = 0; i < 16; i++)
		bGuid[i] = (byte) nGuid[i];

	return outGuid;
}

