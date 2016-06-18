#ifndef _FISH_COMMON_H
#define _FISH_COMMON_H

#include "stdafx.h"
#include "fish_def.h"
#include <list>
using namespace std;

class CEzXMLParser;

const CString ConvertTimetoString(const COleDateTime& t);
BOOL ConvertStringtoTime(const CString& s, COleDateTime& t);

time_t ConvertDateTimeToTimeT(const COleDateTime& t);

CString GetNamedNodeText(_bstr_t sname, CEzXMLParser* parser, IXMLDOMNodePtr node = NULL, int type = 0);
int SetNamedNodeText(_bstr_t sname, CEzXMLParser* parser, _bstr_t text, int type = 0, IXMLDOMNodePtr node = NULL);


COLORREF ColorPicker(CBitmap& bm, POINT pt, CDC* pDC);
BOOL GetBitmapPart(CBitmap& src, CBitmap& dest, CRect& rect, CDC* pDC);
BOOL QuarterArcRgn(int x, int y, int size_x, int size_y, int nType, CRgn& rgnA);
void DiffRgn(int x, int y, int size_x, int size_y, int nType, CRgn& rgn);
BOOL GetRoundedWindow(CRect rc, int nRndSize, CRgn& rgnContents);
bool WindowsShutDown();

BOOL IsDir(CString sdir);

// checking avaliable disk free space..
int avldspace( LPCTSTR path, ULONGLONG fsize, ULONGLONG & freedsize );

// directory »ý¼º
int mymkdir(LPCTSTR dirname);
int makedir (LPCTSTR newdir);	// call this function..

// Wide char to Multi, vise versa
BOOL AnsiConvertWtoM(wchar_t*, char*, int);
BOOL AnsiConvertMtoW(char*, wchar_t*, int);

void PrintMemoryMap(byte*, int);

void WriteRegData( CString key, CString valueName, CString valueData, bool bHCU=true );
CString ReadRegData( CString key, CString valueName, bool bHCU=true );
DWORD ReadRegData( HKEY hkey, CString key, CString valueName);

void SetRegValue(HKEY hKey, CString strKey, CString strChildKey, 
				 CString strChildKeyValue);
BOOL GetRegData(HKEY hKey, CString strKey, CString strChildKey,void *dest);
BOOL GetRegDWordData(HKEY hKey, CString strKey, CString strChildKey,void *dest);


int	GetLine( CMemFile *m_File, TCHAR *line);
int GetRemains( CMemFile *m_File, CString& sBuff);

BOOL CheckResultCode( TCHAR* line , int& code);

UINT FishMessageBox(CString message, UINT utype = FMB_OK);
UINT FishMessageBox(CString message, CString title, UINT utype = FMB_OK);
int  GetOSVersionType();

BOOL LoadHtml(CString url, CString& htmlstream);
list<CString> GetXmlUrlListInDocument(CString& htmlstream, CString doc_url, BOOL bFindFrame);
list<CString> FindTagAttributeValues(CString& htmlstream, CString tagname, CString attribute);
BOOL UrlCheck(CString str_url);
BOOL RemoveNonXmlUrl(list<CString>& url_list, CString doc_url);
BOOL GetChannelInfo(CEzXMLParser& parser, CString& title, CString& htmlurl);
#endif