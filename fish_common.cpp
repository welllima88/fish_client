#include "stdafx.h"
#include "fish_def.h"
#include "fish_common.h"
#include <errno.h>
#include "direct.h"

// written by		: moonknit
// name				: ColorPicker
// created			: 2005-06-09
// last updated		: 2005-06-09
// desc				:
// 해당 비트맵의 해당 포인터의 컬러값을 반환한다.
COLORREF ColorPicker(CBitmap& bm, POINT pt, CDC* pDC)
{
	CDC displayDC;

	if(!pDC || !pDC->GetSafeHdc())
	{
		displayDC.CreateDC(_T("Display"), NULL, NULL, NULL);
		pDC = &displayDC;
	}

	//CBitmap destBmp, MemBmp;
	COLORREF crReturn;
	CDC MemDC;
	CBitmap *pOldMemBmp = NULL;
	CSize sz = bm.GetBitmapDimension();

	// 이미지 그리기
	MemDC.CreateCompatibleDC(pDC);
	pOldMemBmp = MemDC.SelectObject(&bm);

	// 픽셀 정보를 가져옴
	crReturn = MemDC.GetPixel(pt);

	MemDC.SelectObject(pOldMemBmp);

	return crReturn;
}

// written by		: moonknit
// name				: GetBitmapPart
// created			: 2005-06-09
// last updated		: 2005-06-09
// desc				:
// 해당 비트맵의 사각형부분을 새 Bitmap으로 반환한다.
BOOL GetBitmapPart(CBitmap& src, CBitmap& dest, CRect& rect, CDC* pDC)
{
	CDC displayDC;

	if(!pDC || !pDC->GetSafeHdc())
	{
		displayDC.CreateDC(_T("Display"), NULL, NULL, NULL);
		pDC = &displayDC;
	}

	CDC MemDC;
	CDC	DestDC;
	CBitmap *pOldBmp = NULL;
	CBitmap *pOldMemBmp = NULL;
	CSize szSrc = src.GetBitmapDimension();

	// 얻으려는 사각형이 이미지의 사이즈를 벗어났다.
	if(szSrc.cx < rect.right || szSrc.cy < rect.bottom)
		return FALSE;

	// 소스 이미지 그리기
	MemDC.CreateCompatibleDC(pDC);
	pOldMemBmp = MemDC.SelectObject(&src);

	// destBmp에 이미지 찍기
	DestDC.CreateCompatibleDC( pDC );
	dest.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	pOldBmp = DestDC.SelectObject( &dest );
	DestDC.BitBlt(0, 0, rect.Width(), rect.Width(), &MemDC, rect.left, rect.bottom, SRCCOPY);

	// Bitmap 풀어주기
	DestDC.SelectObject(pOldBmp);
	MemDC.SelectObject(pOldMemBmp);

	return TRUE;
}

BOOL GetRoundedWindow(CRect rc, int nRndSize, CRgn& rgnContents)
{ 
    int width = rc.Width() - 1;    //가로 사이즈
	
    int height = rc.Height() - 1;   //세로 사이즈
	
    int size = nRndSize * 2;    //모서리 둥근 정도
	
    int start_x = rc.left;		//시작지점_x
	
    int start_y = rc.top;		//시작지점_y
	
    //전체 영역
	
    VERIFY(rgnContents.CreateRectRgn( start_x, start_y, width, height ));
	
	
#ifdef USE_DIFFRGN
	// left-top
	DiffRgn(start_x, start_y, nRndSize, nRndSize, 0, rgnContents);
	
	// right-top
	DiffRgn(width + 1, start_y, nRndSize, nRndSize, 1, rgnContents);
	
	// left-bottom
	DiffRgn(start_x, height, nRndSize, nRndSize, 2, rgnContents);
	
	// right-bottom
	DiffRgn(width + 1, height, nRndSize, nRndSize, 3, rgnContents);
#else
	
    CRgn	rgnA, rgnB;
	
    //left-top잘라내기 영역 생성
	
    VERIFY(rgnA.CreateEllipticRgn(start_x, start_y, size - 1, size - 1));
	
    VERIFY(rgnB.CreateRectRgn(start_x, start_y, (int)size/2, (int)size/2));
	
    rgnA.CombineRgn(&rgnB,&rgnA,RGN_DIFF);
	
    //left-top영역 잘라냄.
	
    rgnContents.CombineRgn(&rgnContents,&rgnA,RGN_DIFF);
	
    //right-top잘라내기 영역 생성
	
    CRgn rgnC,rgnD;
	
    VERIFY(rgnC.CreateEllipticRgn(width+1, start_y, width-size, size-1));
	
    VERIFY(rgnD.CreateRectRgn(width-(int)size/2, start_y, width, (int)size/2));
	
    rgnC.CombineRgn(&rgnD,&rgnC,RGN_DIFF);
	
    //right-top영역 잘라냄.
	
    rgnContents.CombineRgn(&rgnContents,&rgnC,RGN_DIFF);
	
    //left-bottom잘라내기 영역 생성
	
    CRgn rgnE,rgnF;
	
    VERIFY(rgnE.CreateEllipticRgn(start_x, height+1, size-1, height+size));
	
    VERIFY(rgnF.CreateRectRgn(start_x, height-(int)size/2,(int)size/2, height));
	
    rgnE.CombineRgn(&rgnF,&rgnE,RGN_DIFF);
	
    //left-bottom영역 잘라냄.
	
    rgnContents.CombineRgn(&rgnContents,&rgnE,RGN_DIFF);
	
    //right-bottom잘라내기 영역 생성
	
    CRgn rgnG,rgnH;
	
    VERIFY(rgnG.CreateEllipticRgn(width-size+1, height-size+1, width, height));
	
    VERIFY(rgnH.CreateRectRgn(width-(int)size/2, height-(int)size/2,width, height));
	
    rgnG.CombineRgn(&rgnH,&rgnG,RGN_DIFF);
	
    //right-bottom영역 잘라냄.
	
    rgnContents.CombineRgn(&rgnContents,&rgnG,RGN_DIFF);
#endif
	
	return TRUE;
}
 

void DiffRgn(int x, int y, int size_x, int size_y, int nType, CRgn& rgn)
{
	CRgn rgnA;
	if(!QuarterArcRgn(x, y, size_x, size_y, nType, rgnA)) return;
	
    // 영역 잘라냄.
	
    rgn.CombineRgn(&rgn,&rgnA,RGN_DIFF);
}

// 잘라내는 타입 설정
// type : 잘라내는 부위
// 0 : left-top
// 1 : right-top
// 2 : left-bottom
// 3 : right-bottom
BOOL QuarterArcRgn(int x, int y, int size_x, int size_y, int nType, CRgn& rgnA)
{
    CRgn	rgnB;
	if(size_x < 0 || size_y < 0) return FALSE;
	
    // 잘라내기 영역 생성
	int x_Elt, y_Elt, x_Ebr, y_Ebr;		// Elliptic Rgn
	int x_Rlt, y_Rlt, x_Rbr, y_Rbr;		// Rectangle Rgn
	
	switch(nType)
	{
	case 0:
		x_Elt = x;
		y_Elt = y;
		x_Ebr = size_x * 2 - 1;
		y_Ebr = size_y * 2 - 1;
		x_Rlt = x;
		y_Rlt = y;
		x_Rbr = size_x;
		y_Rbr = size_y;
		break;
	case 1:
		x_Elt = x - size_x * 2 + 1;
		y_Elt = y;
		x_Ebr = x;
		y_Ebr = y + size_y * 2 - 1;
		x_Rlt = x - size_x;
		y_Rlt = y;
		x_Rbr = x;
		y_Rbr = y + size_y;
		break;
	case 2:
		x_Elt = x;
		y_Elt = y - size_y * 2 + 1;
		x_Ebr = x + size_x * 2 - 1;
		y_Ebr = y;
		x_Rlt = x;
		y_Rlt = y - size_y;
		x_Rbr = x + size_x;
		y_Rbr = y;
		break;
	case 3:
		x_Elt = x - size_x * 2 + 1;
		y_Elt = y - size_y * 2 + 1;
		x_Ebr = x;
		y_Ebr = y;
		x_Rlt = x - size_x;
		y_Rlt = y - size_y;
		x_Rbr = x;
		y_Rbr = y;
		break;
	}
	
    VERIFY(rgnA.CreateEllipticRgn(x_Elt, y_Elt, x_Ebr, y_Ebr));
	
    VERIFY(rgnB.CreateRectRgn(x_Rlt, y_Rlt, x_Rbr, y_Rbr));
	
    rgnA.CombineRgn(&rgnB,&rgnA,RGN_DIFF);
	
	return TRUE;
}

bool WindowsShutDown()
{
	DWORD dwVersion = GetVersion();

	// Get the Windows version.

	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number.

	if(dwVersion < 0x800000000 || dwWindowsMajorVersion < 4) // Windows NT/2000/XP or Win32s
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 
 
		// Get a token for this process. 
 
		if (!OpenProcessToken(GetCurrentProcess(), 
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		{
//			error("OpenProcessToken"); 
			return false;
		}
 
		// Get the LUID for the shutdown privilege. 
 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
				&tkp.Privileges[0].Luid); 
 
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
		// Get the shutdown privilege for this process. 
 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
				(PTOKEN_PRIVILEGES)NULL, 0); 
 
		// Cannot test the return value of AdjustTokenPrivileges. 
 
		if (GetLastError() != ERROR_SUCCESS) 
		{
//			error("AdjustTokenPrivileges"); 
			return false;
		}
 
		// Shut down the system and force all applications to close. 
 
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE | EWX_POWEROFF, 0)) 
		{
//			error("ExitWindowsEx");
			return false;
		}
	}
	else	// Windows 95/98/Me
	{
		return ExitWindows(EWX_LOGOFF, 0) != 0;
	}

	return true;
}

/**************************************************************************
 * IsDir
 *
 * written by moonknit
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * 주어진 스트링이 Directory인지 여부를 반환한다.
 *
 * @Parameters
 * (in CString) sdir - 검사할 스트링
 *
 * @Return
 * (BOOL)
 **************************************************************************/
BOOL IsDir(CString sdir)
{
	CFileFind f;

	if(f.FindFile(sdir))
	{
		BOOL bhavenext = TRUE;
		while(bhavenext)
		{
			bhavenext = f.FindNextFile();

			f.FindNextFile();
			if(f.IsDirectory()) return TRUE;
		} 
	}

	return FALSE;
}

//============================================================================
// NAME : avldspace()
// PARM : LPCTSTR path(must : C:\\ or D:\\...), ULONGLONG fsize, ULONGLONG freedsize(output-가용한 용량)
// RETN : int
//		  0 : 용량 가능
//		  1 : 용량 부족
//        2 : 잘못된 경로
// DESC : 해당 경로에 대한 여유 공간 확인..
// Date : 2004-03-08 coded by bemlove, origin
//============================================================================
int avldspace( LPCTSTR path, ULONGLONG fsize , ULONGLONG & freedsize )
{
	
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	BOOL ret = GetDiskFreeSpaceEx( path, &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
	if( ret == FALSE )
		return 2;
	freedsize = TotalNumberOfFreeBytes.QuadPart;
	// 	if( freedsize > fsize+(1024*1024*10) )
	if( freedsize > fsize )
		return 0;
	else return 1;
	
}

int mymkdir(LPCTSTR dirname)
{
    int ret=0;
#ifdef WIN32
#ifdef _UNICODE
    ret = _tmkdir(dirname);
#else
	ret = _mkdir(dirname);
#endif
#else
    #ifdef unix
        ret = mkdir (dirname,0775);
    #else
        #ifdef __TURBOC__
            ret = mkdir (dirname);
        #endif
    #endif
#endif

    return ret;
}

int makedir (LPCTSTR newdir)
{
	TCHAR *buffer;
	TCHAR *p;
	int  len = _tcslen(newdir);  
	
	if (len <= 0) 
		return 0;
	
	buffer = (TCHAR*)malloc((len+1) * sizeof(TCHAR));
	
	_tcscpy(buffer,newdir);
	
	if (buffer[len-1] == _T('/')) {
		buffer[len-1] = _T('\0');
	}
	if (mymkdir(buffer) == 0)
    {
		free(buffer);
		return 1;
    }

//	TRACE(_T("makedir : %s \r\n"), newdir);
	
	p = buffer+1;
	while (1)
    {
		TCHAR hold;
		
		while(*p && *p != _T('\\') && *p != _T('/'))
			p++;
		hold = *p;
		*p = 0;
		// errno-> ENOENT : Path was not found
		if ((mymkdir(buffer) == -1) && (errno == ENOENT))
        {
			TRACE(_T("errno : %d"), errno );
			free(buffer);
			return 0;
        }

		if (hold == 0)
			break;
		*p++ = hold;
    }
	free(buffer);
	return 1;
}

/*
 * <AnsiConvertWtoM>

 * written by moonknit

 * @History
 * created 2005-12-22

 * @Description
 * Ansi Character Convering
 * using WideCharToMultiByte 

 * @Parameters
 * (in wchar_t*) in - converting data
 * (out char*) out - converted data
 * (in int) buffersize - buffer size of [out]

 * @Return
 * (BOOL)

 */
BOOL AnsiConvertWtoM(wchar_t* in, char* out, int buffersize)
{
	if(in == NULL || out == NULL) return FALSE;
	int size = wcslen(in);
	if(size <= 0
		|| size >= buffersize
		) return FALSE;

	if(
		WideCharToMultiByte(
		CP_ACP, 
		NULL,
		in,
		size,
		out,
		buffersize,
		NULL,
		NULL
		) == 0
		) return FALSE;

	out[size] = '\0';

	return TRUE;
}

/*
 * <AnsiConvertMtoW>

 * written by moonknit

 * @History
 * created 2005-12-22

 * @Description
 * Ansi Character Convering
 * using MultiByteToWideChar

 * @Parameters
 * (in char*) in - converting data
 * (out wchar_t*) out - converted data
 * (in int) buffersize - buffer size of [out]

 * @Return
 * (BOOL)

 */
BOOL AnsiConvertMtoW(char* in, wchar_t* out, int buffersize)
{
	if(in == NULL || out == NULL) return FALSE;
	int size = strlen(in);
	if(size <= 0) return FALSE;

	if(size <= 0
		|| size >= buffersize
		) return FALSE;

	int l = 
		MultiByteToWideChar(
		CP_ACP, 
		NULL,
		in,
		size,
		out,
		buffersize
		);
	if( l == 0
		) return FALSE;

	out[l] = '\0';

	return TRUE;

}

/**************************************************************************
 * global function PrintMemoryMap
 *
 * written by moonknit
 *
 * @History
 * created 2006-01-10
 *
 * @Description
 * print memory
 *
 * @Parameters
 * (in byte*) pbuff - byte memory buffer to look into
 * (in int) len - buffer size
 *
 * @Return
 * (void)
 **************************************************************************/
void PrintMemoryMap(byte* pbuff, int len)
{
#ifndef _DEBUG
	return;
#endif

	if(len > 200) return;

	CString smemory, tmpch;
	smemory.Format(_T("Print Memory [%d]: "), len);
	for(int i = 0; i<len; ++i)
	{
		tmpch.Format(_T("%x"), pbuff[i]);

		smemory += tmpch;
	}

	smemory += _T("\r\n");

	TRACE(smemory);
}

/*
** NAME    **:  Write / Read Data
** PARAM   **: key -> id, valueName->name, vlaueData->data
** RETURN  **: 
** DESC    **: 레지스트리에 write / read
** by bemlove at 2002-10-15, VER : 1.0, COMMENTS: ORG 
## 1.1 bool bHCU -> 추가. (HKEY_CURRENT_USER:true, HKEY_LOCAL_MACHINE:false)
*/
void WriteRegData( CString key, CString valueName, CString valueData, bool bHCU )
{
//	TRACE(_T("key : %s\r\nvalue name : %s\r\nvalue data : %s\r\n"), key, valueName, valueData);

	CString tmpKey;
	tmpKey.Format(_T("%s\\%s"),REG_STR_KEYPATH, key);
	if( bHCU )
		SetRegValue( HKEY_CURRENT_USER, tmpKey, valueName, valueData );
	else
		SetRegValue( HKEY_LOCAL_MACHINE, tmpKey, valueName, valueData );
}

CString ReadRegData( CString key, CString valueName, bool bHCU )
{
	CString tmpKey;
	TCHAR	value[(MAX_VERYHUGEBUFF+1)];
	memset( value, 0x00, sizeof(value) );
	tmpKey.Format(_T("%s\\%s"), REG_STR_KEYPATH, key );
	if( bHCU )
		GetRegData( HKEY_CURRENT_USER, tmpKey, valueName, value );
	else
		GetRegData( HKEY_LOCAL_MACHINE, tmpKey, valueName, value );

//	TRACE(_T("read key : %s\r\nvalue name : %s\r\nvalue data : %s\r\n"), key, valueName, value);

	return value;

}

DWORD ReadRegData( HKEY hkey, CString key, CString valueName)
{
	DWORD value = 0;
	if( !GetRegDWordData(hkey, key, valueName, &value) )
	{
		value = 2;
	}
	
	return value;	
}

///////////////////////////////////////////////////////////////////
// writes a registry string value
void SetRegValue(HKEY hKey, CString strKey, CString strChildKey, 
				 CString strChildKeyValue)
{
   HKEY   hChildKey = 0;
   DWORD  dwDisposition;

   // Create or open the registry key
   RegCreateKeyEx(hKey,
      (LPCTSTR) strKey, 
      0, 
      NULL, 
      REG_OPTION_NON_VOLATILE,
      KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, 
      NULL, 
      &hChildKey,
      &dwDisposition
   );   

   // Set the new value for the key
   RegSetValueEx(
      hChildKey,
      (LPCTSTR) strChildKey, 
      0,
      REG_SZ,
      (const BYTE*)(LPCTSTR)strChildKeyValue, 
      (DWORD) strChildKeyValue.GetLength() * sizeof(TCHAR)
   );
   // Close the key
   RegCloseKey(hChildKey);
}

///////////////////////////////////////////////////////////////////
// read a registry string value
BOOL GetRegData(HKEY hKey, CString strKey, CString strChildKey,void *dest)
{
   // Open the registry key
   HKEY hResult;
   if (ERROR_SUCCESS != RegOpenKey(hKey,(LPCTSTR) strKey, &hResult)) 
      return FALSE;

   // Get the values
	DWORD dwCount = MAX_VERYHUGEBUFF;

   if (ERROR_SUCCESS != RegQueryValueEx(hResult, 
		(LPCTSTR) strChildKey, 0, 0, (byte*)dest, &dwCount))
   {
     RegCloseKey(hResult);    // Close the key
     return FALSE;
   }  

   // Close the key
   RegCloseKey(hResult);   

   return TRUE;
}

BOOL GetRegDWordData(HKEY hKey, CString strKey, CString strChildKey,void *dest)
{
	// Open the registry key
	HKEY hResult;
	if (ERROR_SUCCESS != RegOpenKey(hKey,(LPCTSTR) strKey, &hResult)) 
		return FALSE;
	
	// Get the values
	DWORD dwCount = 255;
	
	if (ERROR_SUCCESS != RegQueryValueEx(hResult, 
		(LPCTSTR) strChildKey, 0, 0, (LPBYTE)dest, &dwCount))
	{
		RegCloseKey(hResult);    // Close the key		
		return FALSE;
	}  
	
	// Close the key
	RegCloseKey(hResult);   
	
	return TRUE;
}

int GetLine( CMemFile *m_File, TCHAR *line)
{
	TCHAR temp[1024]={0}, ch=0L;
	int i, size=0;

	if(m_File->GetLength() == 0) return -1;

	i=0;
	do {
		size = m_File->Read( &ch, sizeof(TCHAR));
		if( size == 0 ) break;

		if( ch == 13 || ch == 10)
		{
			size = m_File->Read( &ch, sizeof(TCHAR));
			if( ch != 10 && ch != 13)
			{
				m_File->Seek(-size, CFile::current);
			}
			break;
		}
		else
		{
			temp[i++] = ch;
		}

	} while (1);

	if(i > 0)
	{
		temp[i] = _T('\0');
		_tcscpy( line, temp );

		TRACE(_T("read line : %s\r\n"), temp);
		return i;
	}

	TRACE(_T("read line : no line\r\n"));

	return -1;
}//end of GetLine()

int GetRemains(CMemFile *m_File, CString& sBuff)
{
	char buf[256];
	DWORD remain = m_File->GetLength() - m_File->GetPosition();
	int ntotalread, nread;

	sBuff.Empty();

	ntotalread = 0;

	while(remain > 0)
	{
		nread = m_File->ReadHuge(buf, sizeof(buf) - 2);
		if(nread > 0)
		{
			buf[nread] = 0;
#ifdef _UNICODE
			buf[nread+1] = 0;
#endif
			ntotalread += nread;
			remain -= nread;
			sBuff += (TCHAR*) buf;
			TRACE(_T("read buf: %s (%d/%d)\r\n"), (TCHAR*) buf, m_File->GetPosition(), m_File->GetLength());
		}
	}

	return ntotalread;
}

BOOL CheckResultCode( TCHAR* line, int& code)
{
	if(!line) return FALSE;

	BOOL bData0 = FALSE;
	if(line[0] == _T('0')) 
	{
		bData0 = TRUE;
	}

	code = _ttoi(line);
	if(code == 0 && !bData0)
	{
		code = -1;
		return FALSE;
	}
		
	return TRUE;
}


//////////////////////////////////////////////////////////
//  Function Name  
//      GetOSVersionType
//
//  Parameters 
//
//  Return Values
//      설치된 Windows 버전을 return
//      -1: Failed
//      1 : Windows 95
//      2 : Windows 98
//      3 : Windows ME
//      4 : Windows NT
//      5 : Windows 2000
//      6 : Windows XP
int GetOSVersionType()
{
    UINT nOSVersion;
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    
    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
    {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return -1;
    }
    
    switch (osvi.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT:
        // Test for the product.
        if ( osvi.dwMajorVersion <= 4 )
            nOSVersion = 4;
        else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            nOSVersion = 5;
        else if( bOsVersionInfoEx )  
            nOSVersion = 6;
        break;
    case VER_PLATFORM_WIN32_WINDOWS:
        if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
            nOSVersion = 1;
        else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
            nOSVersion = 2;
        else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
            nOSVersion = 3;
        break;
    }
    
    return nOSVersion; 
}