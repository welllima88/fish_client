//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!
#include "stdafx.h"
//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!

//NETDIB.CPP BMP 화일을 출력하는 클래스

#include "NetDib.h"
#include "NetBitmap.h"
#include <io.h>

// ************************************************************ //
// *******			******************************* //
// ******* VERSION: 2000.05.23	******************************* //
// *******			******************************* //
// ************************************************************ //

//생성자 화일명을 주어서 클래스가 만들어지면서 이미지를 가지고 
//있게 한다.

#define STR_NETDIB_NOFILE					_T("%s 화일이 없습니다")
#define STR_NETDIB_BADBMPHEADER				_T("%s BMPHEADER 를 읽을수 없습니다!")
#define STR_NETDIB_NOTBMP					_T("%s BMP 화일이 아닙니다.")
#define STR_NETDIB_MEMORYSHORT				_T("%s 메모리가 부족합니다.")
#define STR_NETDIB_READFAIL					_T("%s 화일을 읽을수 없습니다.")
#define STR_NETDIB_PALETTEMEMORYSHORT		_T("%s palette 메모리가 부족합니다.")
#define STR_NETDIB_TEMPMEMORYSHORT			_T("메모리가 부족합니다.")

CNetDib::CNetDib()
{
}

CNetDib::CNetDib(LPTSTR filename)
{
	//화일명을 m_strFileName에 기록한다.
	lstrcpy((LPTSTR)m_strFileName,(LPTSTR)filename);
	//이미지 포인터와 버퍼를 null로 설정한다.
	szDib=NULL;
	lpDib=NULL;
	m_palDIB=NULL;
	//팔레트를 null로 설정한다.
	szPal=NULL;
	//bmp 화일을 로드시킨다.
	if(!bLoadBmpFile())
	{
//		AfxMessageBox(m_strMessageTemp);
	}

	szDibBack=NULL;
    lpDibBack=NULL;
}

CNetDib::CNetDib(CFile *fp)
{
	szDib=NULL;
	lpDib=NULL;
	m_palDIB=NULL;
	//팔레트를 null로 설정한다.
	szPal=NULL;
	//bmp 화일을 로드시킨다.
	if(!bLoadBmpFile(fp)) AfxMessageBox(m_strMessageTemp);

	szDibBack=NULL;
    lpDibBack=NULL;
}

CNetDib::~CNetDib()
{
	if(szDib) {
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
	}

	if(szDibBack) {
		::GlobalUnlock(szDibBack);
		::GlobalFree(szDibBack);
	}

	if(m_palDIB) delete m_palDIB;
	if(szPal) {
		::GlobalUnlock((HGLOBAL) szPal);
		::GlobalFree((HGLOBAL) szPal);
	}
}

BOOL CNetDib::bLoadBmpFile()
{
	HFILE bmpFile;

	//화일을 연다.
//	bmpFile=_lopen((LPTSTR)m_strFileName,OF_READ);
	
	bmpFile=_topen((LPTSTR)m_strFileName,OF_READ);

	if(bmpFile<0) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOFILE,m_strFileName);
	   return FALSE;
	}

	//bmpHeder 구조체에 BMPHEADER 헤더 정보를 읽어 온다.
	if(_lread(bmpFile,(LPTSTR)&bmpHeader,sizeof(bmpHeader))!=sizeof(bmpHeader)) {
		_stprintf(m_strMessageTemp,STR_NETDIB_BADBMPHEADER,m_strFileName);
		return FALSE;
	}

	//화일이 bmp 일경우에는 화일 처음에 "BM"이라고 설정해있음
	if (bmpHeader.bfType != DIB_HEADER_MARKER) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOTBMP,m_strFileName);
		return FALSE;
	}

	//이미지 데이타 크기만큼 메모리를 설정한다.
	szDib =::GlobalAlloc(GHND,bmpHeader.bfSize);
	if(szDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_MEMORYSHORT,m_strFileName);
		return FALSE;
	}

	//이미지 데이타 버퍼를 컨트롤 할수 있도록 lpDip 에 락킹한다.
	lpDib=(LPTSTR)::GlobalLock(szDib);
	UINT len;

	 //이미지 데이타를 읽는다.
	if((len=_lread(bmpFile,(LPTSTR)lpDib,bmpHeader.bfSize-sizeof(bmpHeader)))!=bmpHeader.bfSize-sizeof(bmpHeader)) {
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib=NULL;
		lpDib=NULL;
		_stprintf(m_strMessageTemp,STR_NETDIB_READFAIL,m_strFileName);
		return FALSE;
	}

	bmpInfoHeader = (LPBITMAPINFOHEADER)lpDib;
	bmcInfoHeader = (LPBITMAPCOREHEADER)lpDib;
	bmpInfo =(LPBITMAPINFO)lpDib;
	bmcInfo = (LPBITMAPCOREINFO)lpDib;
	bCreatePalette();
	_lclose(bmpFile);
 
	return TRUE;
}

BOOL CNetDib::bLoadBmpFile(CFile *fp)
{
	//bmpHeder 구조체에 BMPHEADER 헤더 정보를 읽어 온다.
	if((fp->Read((LPTSTR)&bmpHeader,sizeof(bmpHeader)))!=sizeof(bmpHeader)) {
		_stprintf(m_strMessageTemp,STR_NETDIB_BADBMPHEADER);
		return FALSE;
	}

	//화일이 bmp 일경우에는 화일 처음에 "BM"이라고 설정해있음
	if (bmpHeader.bfType != DIB_HEADER_MARKER) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOTBMP);
		return FALSE;
	}

	//이미지 데이타 크기만큼 메모리를 설정한다.
	szDib =::GlobalAlloc(GHND,bmpHeader.bfSize);
	if(szDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_MEMORYSHORT);
		return FALSE;
	}

	//이미지 데이타 버퍼를 컨트롤 할수 있도록 lpDip 에 락킹한다.
	lpDib=(LPTSTR)::GlobalLock(szDib);

	 //이미지 데이타를 읽는다.
	if((fp->Read((LPTSTR)lpDib,bmpHeader.bfSize-sizeof(bmpHeader)))!=bmpHeader.bfSize-sizeof(bmpHeader)) {
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib=NULL;
		lpDib=NULL;
		_stprintf(m_strMessageTemp,STR_NETDIB_READFAIL);
		return FALSE;
	}

    bmpInfoHeader = (LPBITMAPINFOHEADER)lpDib;
	bmcInfoHeader = (LPBITMAPCOREHEADER)lpDib;
	bmpInfo =(LPBITMAPINFO)lpDib;
	bmcInfo = (LPBITMAPCOREINFO)lpDib;
	bCreatePalette();

	return TRUE;
}

//팔레트를 만든다.
BOOL CNetDib::bCreatePalette()
{
	//현재 사용하는 칼라수를 받고
	m_nColorNums=bGetColorNums();
		
	// Source의 칼라수에 따른 바이트수  		
	if(m_nColorNums==0) 
	{
		m_nColors = 3;
		return TRUE;
	}
	else if	(m_nColorNums==256) m_nColors = 1;
	
   //필레트 메모리를 확보한다음
	szPal=::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* m_nColorNums);

	if(szPal == NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_PALETTEMEMORYSHORT,m_strFileName);
		return FALSE;
	}

	//lpPal 락킹하고
	lpPal=(LPLOGPALETTE) ::GlobalLock((HGLOBAL) szPal);

	lpPal->palVersion=PALVERSION;
	lpPal->palNumEntries=(WORD)m_nColorNums;

    //칼라수만큼 팔레트를 칼레트 엔트리에 넣는다.
	for(int i=0;i<(int)m_nColorNums;i++) {
		if (m_nDibStyle) { //win3.0 이상버전일경우 다음과 같이 하고
			lpPal->palPalEntry[i].peRed = bmpInfo->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen =bmpInfo->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue = bmpInfo->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
		else {//그외에는 다음과 같이 한다.
			lpPal->palPalEntry[i].peRed = bmcInfo->bmciColors[i].rgbtRed;
			lpPal->palPalEntry[i].peGreen = bmcInfo->bmciColors[i].rgbtGreen;
			lpPal->palPalEntry[i].peBlue = bmcInfo->bmciColors[i].rgbtBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
	}

//    CPalette 구조체에 lpPal를 링크시켜서 팔레트를 만듭니다.
	m_palDIB = new CPalette;
	m_palDIB->CreatePalette(lpPal);

	return TRUE;
}

//---------------------------------------------------------------------
// GetNumOfColors
//
// DIB의 컬러수를 리턴
//---------------------------------------------------------------------
int CNetDib::bGetColorNums()
{
	//3.0 버전의 비트맵이면
   	if(IS_WIN30_DIB(lpDib))	{
		//m_nDibStyle  를 3.0 비트맵 버전이라고 설정
		m_nDibStyle=TRUE;
		m_nWidthX=bmpInfoHeader->biWidth;
		m_nWidthY=bmpInfoHeader->biHeight;
		DWORD dwClrUsed;
		//사용된 칼라수를 받아서 만약에 그것이 0이 아니면
		//그것이 현재 비트맵이 사용하는 칼라수 이므로 설정
		dwClrUsed = bmpInfoHeader->biClrUsed;
		if(dwClrUsed != 0)	return (int)dwClrUsed;
	}
	else {
		m_nDibStyle=FALSE;
		m_nWidthX=bmcInfoHeader->bcWidth;
		m_nWidthY=bmcInfoHeader->bcHeight;
	}

	WORD wBitCount;
	//비트 카운트를 받는다.
	if(IS_WIN30_DIB(lpDib))	wBitCount = bmpInfoHeader->biBitCount;
	else wBitCount = bmcInfoHeader->bcBitCount;

	switch (wBitCount)	{
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			return 0;
	}
}

//---------------------------------------------------------------------
// bFindDib
//
// DIB의 데이터 부분을 찾아 포인터를 리턴
// databits location = pDib + Header size + Palette size
//---------------------------------------------------------------------
LPTSTR CNetDib::bFindDib()
{
	return (lpDib + *(LPDWORD)lpDib + bPaletteSize());
}

//팔래트 크기를 알아본다.
WORD CNetDib::bPaletteSize()
{
	//3.0이상 버전 이미지 이면
   if (m_nDibStyle) return (WORD)(m_nColorNums * sizeof(RGBQUAD));
   else return (WORD)(m_nColorNums * sizeof(RGBTRIPLE));
}

//윈도우에 설정되어있는 팔레트를 사용한다.
int CNetDib::SetSystemPalette(HDC hDC)
{
	if(m_nColorNums != 0) return FALSE;
	//현재 트루칼라모드이면 설정안함
	if((int)(!::GetDeviceCaps(hDC, RASTERCAPS)) & RC_PALETTE) return FALSE;
	//현재 모드가 몇칼라 모드이며 팔레트엔트리가 몇개인지 확인
	int nSysColors = ::GetDeviceCaps(hDC, NUMCOLORS);
	int nPalEntries = ::GetDeviceCaps(hDC, SIZEPALETTE);

	if(nPalEntries==0)	return FALSE;
	
	int nEntries = (nPalEntries==0) ? nSysColors : nPalEntries;

	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		nEntries * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = nEntries;
	
	//시스템 팔레트를 얻고
	::GetSystemPaletteEntries(hDC, 0, nEntries,
		(LPPALETTEENTRY) ((LPBYTE) pLogPal + 2 * sizeof(WORD)));
	HPALETTE m_hPalette;
	
	//시스템 팔레트를 설정한다.
	m_hPalette = ::CreatePalette(pLogPal);
	SelectPalette(hDC, m_hPalette, TRUE);
	delete pLogPal;
	return TRUE;
}

void  CNetDib::PaintImage(int x, int y, HDC hDC)
{
	LPTSTR    lpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if (lpDib == NULL) return;

	//실질 이미지로 분리한다.
	lpDIBBits = bFindDib();

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);

	::SetDIBitsToDevice(hDC,                    // hDC
								   x,             // DestX
								   y,              // DestY
								   m_nWidthX,        // nDestWidth
								   m_nWidthY,       // nDestHeight
								   0,            // SrcX
								   0,   // SrcY
								   0,                          // nStartScan
								   (WORD)m_nWidthY,  // nNumScans
								   lpDIBBits,                  // lpBits
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage


	if (hOldPal != NULL) {
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}

void  CNetDib::PaintImage(int x, int y, int cx, int cy,HDC hDC )
{    
	LPTSTR    lpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if(lpDib == NULL) return;

	lpDIBBits = bFindDib();

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);

	::StretchDIBits(hDC,                   // hDC
								   x,             // DestX
								   y,              // DestY
								   cx,        // nDestWidth
								   cy,       // nDestHeight
								   0,            // SrcX
								   0,   // SrcY
								   m_nWidthX,  
								   m_nWidthY,
								   lpDIBBits,                  // lpBits
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS,SRCCOPY);            // wUsage


	if (hOldPal != NULL) {
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}
		
// 배경색을 지정한 RGB Color로 변경후 출력
void  CNetDib::PaintImage(int x,int y,HDC hDC,COLORREF color)
{
	CNetBitmap bmImage;
	CDC *pDC = CDC::FromHandle(hDC);

	bmImage.m_hObject = GetBitmap();
	bmImage.DrawTransparent(pDC, x, y, color);
}

// cx,cy 는 찍을 곳의 크기
void  CNetDib::PaintImagePart(HDC hDC, int x, int y, int cx, int cy )
{    
	LPTSTR    lpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if(lpDib == NULL) return;

	lpDIBBits = bFindDib();

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	::StretchDIBits(hDC,                   // hDC
								   x,             // DestX
								   y,              // DestY
								   cx,        // nDestWidth
								   cy,       // nDestHeight
									x,			// SrcX
									m_nWidthY - y - cy,	//SrcY
									cx,			// nSrcWidth
									cy,			// nSrcHeight
/*								   0,            // SrcX
								   m_nWidthY-(int)(((float)m_nWidthX/(float)cx)*cy),   // SrcY
								   m_nWidthX,  
								   (int)(((float)m_nWidthX/(float)cx)*cy), 
*/								   lpDIBBits,                  // lpBits
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS,SRCCOPY);            // wUsage


	if (hOldPal != NULL) {
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}
		
// cx,cy 는 찍을 곳의 크기
void  CNetDib::PaintImagePart(HDC hDC,int x,int y,int cx,int cy,COLORREF color)
{    
	LPTSTR    lpDIBBits;          
	LPTSTR   plpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       
	char     strBack[3];
	char	 strFord[3];

	if(m_nColors!=3) return;	// 16만칼라에서만 적용

	strBack[0]=(char)0x00;
	strBack[1]=(char)0xff;
	strBack[2]=(char)0x00;	// 흰색

	strFord[0]=(char)GetRValue(color);
	strFord[1]=(char)GetGValue(color);
	strFord[2]=(char)GetBValue(color);	// 흰색

	if(lpDib==NULL)	return;
	//실질 이미지로 분리한다.
	lpDIBBits=bFindDib();

	if (m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else
	{
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	// Temp 메모리를 마련하고 배경을 복사해 놓는다.
	int nDibTempSizeX, nDibTempSizeY;
	HANDLE hszTempDib;
	LPTSTR  lpTempDib;

	nDibTempSizeX = GetXSize();
	nDibTempSizeY = GetYSize();
	int skip = GetModXSize();

	hszTempDib =::GlobalAlloc(GHND, skip * nDibTempSizeY); 

	if(hszTempDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_TEMPMEMORYSHORT);
		return ;
	}

	//이미지 데이타 버퍼를 컨트롤 할수 있도록 lpDip 에 락킹한다.
	lpTempDib=(LPTSTR)::GlobalLock(hszTempDib);

	plpDIBBits = lpDIBBits;

	int i,j;
	for (i=0;i<nDibTempSizeY;i++)
	{
		for (j=0;j<nDibTempSizeX;j++)
		{
			if (memcmp(plpDIBBits + i*skip + j*m_nColors,strBack,m_nColors))	// 배경색이 아니면 복사
				memcpy(lpTempDib + i*skip + j*m_nColors,
						plpDIBBits + i*skip + j*m_nColors,m_nColors);
			else	// 배경색이면 지정한 RGB를 복사
				memcpy(lpTempDib + i*skip + j*m_nColors,
						strFord,m_nColors);
		}
	}
	// end of 배경복사 


	::SetStretchBltMode(hDC, COLORONCOLOR);

	::StretchDIBits(hDC,                   // hDC
								   x,             // DestX
								   y,              // DestY
								   cx,        // nDestWidth
								   cy,       // nDestHeight
								   0,            // SrcX
								   m_nWidthY-(int)(((float)m_nWidthX/(float)cx)*cy),   // SrcY
								   m_nWidthX,  
								   (int)(((float)m_nWidthX/(float)cx)*cy), 
								   lpTempDib,
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS,SRCCOPY);            // wUsage


	if (hOldPal != NULL) {
		::SelectPalette(hDC, hOldPal, TRUE);
	}

	// 할당받은 메모리를 반환한다.
	if(hszTempDib)
	{
		::GlobalUnlock(hszTempDib);
		::GlobalFree(hszTempDib);
	}
}
		
// cx,cy 는 찍을 곳의 크기
void  CNetDib::PaintImagePart(HDC hDC, int x, int y, int cx, int cy, int sx, int sy )
{
	LPTSTR    lpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if(lpDib == NULL) return;

	lpDIBBits = bFindDib();

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	::StretchDIBits(hDC,					// hDC
						x,					// DestX
						y,					// DestY
						cx,					// nDestWidth
						cy,					// nDestHeight
						sx,					// SrcX
						m_nWidthY - sy - cy,	// SrcY
						cx,					// nSrcWidth
						cy,					// nSrcHeight
						lpDIBBits,          // lpBits
						bmpInfo,			// lpBitsInfo
						DIB_RGB_COLORS,SRCCOPY);            // wUsage


	if (hOldPal != NULL) {
		::SelectPalette(hDC, hOldPal, TRUE);
	}
}

//이미지를 저장한다.
int CNetDib::SaveImage(LPTSTR filename)
{
	if(filename!=NULL) lstrcpy((LPTSTR)m_strFileName,(LPTSTR)filename);
	int OffBits;
	HFILE bmpFile;
	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorNums;	
//	bmpFile=_lcreat(m_strFileName,0);
#ifdef UNICODE
	bmpFile= _wcreat(m_strFileName,0);
#else
	bmpFile= _creat(m_strFileName,0);
#endif
	UINT len;
	len=_write(bmpFile,(LPTSTR)&bmpHeader,sizeof(BITMAPFILEHEADER));
	len=_write(bmpFile,(LPTSTR)lpDib,bmpHeader.bfSize-sizeof(bmpHeader));
	_close(bmpFile);
	return TRUE;
}

// CNetDib *pDibBack 과 현재 CNetDib 의 칼라수가 반드시 같아야 함.
void  CNetDib::PaintOverBack(int x,int y,HDC hDC,CNetDib *pDibBack)
{
	LPTSTR    lpDIBBits;          
	LPTSTR   plpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       
	char     strBack[3];
	
	strBack[0]=(char)0x00;
	strBack[1]=(char)0xff;
	strBack[2]=(char)0x00;	// 흰색

	if(lpDib==NULL)	return;
	//실질 이미지로 분리한다.
	lpDIBBits=bFindDib();

	if (m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else
	{
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	// Temp 메모리를 마련하고 배경을 복사해 놓는다.
	int nDibTempSizeX, nDibTempSizeY;
	HANDLE hszTempDib;
	LPTSTR  lpTempDib;

	nDibTempSizeX = GetXSize();
	nDibTempSizeY = GetYSize();
	int skip = GetModXSize();

	hszTempDib =::GlobalAlloc(GHND, skip * nDibTempSizeY); 

	if(hszTempDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_TEMPMEMORYSHORT);
		return ;
	}

	//이미지 데이타 버퍼를 컨트롤 할수 있도록 lpDip 에 락킹한다.
	lpTempDib=(LPTSTR)::GlobalLock(hszTempDib);

	plpDIBBits = lpDIBBits;

	DibCopy( x, y, nDibTempSizeX, nDibTempSizeY, pDibBack , lpTempDib);

	int i,j;
	for (i=0;i<nDibTempSizeY;i++)
	{
		for (j=0;j<nDibTempSizeX;j++)
		{
			if (memcmp(plpDIBBits + i*skip + j*m_nColors,strBack,m_nColors))	// 배경색이 아니면 복사
				memcpy(lpTempDib + i*skip + j*m_nColors,
						plpDIBBits + i*skip + j*m_nColors,m_nColors);
		}
	}
	// end of 배경복사 

	::SetStretchBltMode(hDC, COLORONCOLOR);

	::SetDIBitsToDevice(hDC,                    // hDC
								   x,             // DestX
								   y,              // DestY
								   m_nWidthX,        // nDestWidth
								   m_nWidthY,       // nDestHeight
								   0,            // SrcX
								   0,   // SrcY
								   0,                          // nStartScan
								   (WORD)m_nWidthY,  // nNumScans
								   //lpDIBBits,                  // lpBits
								   lpTempDib,
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage


	if (hOldPal != NULL)
		::SelectPalette(hDC, hOldPal, TRUE);

	// 할당받은 메모리를 반환한다.
	if(hszTempDib)
	{
		::GlobalUnlock(hszTempDib);
		::GlobalFree(hszTempDib);
	}
}

// Source의 x,y 에서 Dest로 Dest의 사이즈 만큼 복사함. 
// WidthX와 WidthY는 Dest의 사이즈와 같아야 함.
int CNetDib::DibCopy(int x, int y,int WidthX, int WidthY,CNetDib *pSource, TCHAR *pDest)
{
	int		nSourceSizeY;
	int		nSourceColor, nSourceSkip;
	LPTSTR   lpSourceBits;          

	// Source의 이미지가 저장된 곳.
	lpSourceBits = pSource->bFindDib();
	// Source의 칼라수에 따른 바이트수  
	nSourceColor = pSource->m_nColors;

	nSourceSizeY = pSource->GetYSize();
	nSourceSkip = pSource->GetModXSize();
	
	int i, startidx, skip;	// Source의 시작위치

	startidx = (nSourceSizeY-y-WidthY) * nSourceSkip  + x * nSourceColor;
	
	if(WidthX%4==0)			skip = WidthX * nSourceColor;
	else if(WidthX%4==3)	skip = ( ((int)WidthX/4)*4 + 4 ) * nSourceColor;
	else if(WidthX%4==2)	skip = WidthX * nSourceColor + 2;
	else					skip = WidthX * nSourceColor + 1;

	for (i=0;i<WidthY;i++)
	{
		memcpy(	pDest + i*skip , 
				lpSourceBits + startidx + i*nSourceSkip,
				skip );
	}
	return 1;
}

// 배경화면으로 쓰일 이미지의 공간을 마련한다.
void CNetDib::PrepareBack(void)
{
	if(!szDibBack) {
		int skip=GetModXSize();
		szDibBack=::GlobalAlloc(GHND,skip*GetYSize());
		if(szDibBack==NULL) {
//			AfxMessageBox(STR_NETDIB_TEMPMEMORYSHORT);
			szDibBack=NULL;
			return;
		}
		lpDibBack=(LPTSTR)::GlobalLock(szDibBack);
	}
}

// 배경 이미지에 기존의 이미지를 복사한다.
void CNetDib::InitBack(HDC hDC)
{
	if(!szDibBack) return;
	
	// 배경이미지를 마련하기위해 기존의 이미지를 복사한다.
	LPTSTR lpSrc=bFindDib();
	int skip=GetModXSize();
	memcpy(lpDibBack,lpSrc,skip*GetYSize());
//	memcpy(lpDibBack,lpSrc,GetXSize()*GetYSize()*m_nColors);

	if(hDC) PaintBack(0,0,hDC);
}

void CNetDib::InitBack(int x,int y,int cx,int cy,HDC hDC)
{
	if(!szDibBack) return;

	LPTSTR lpSrc=bFindDib();
	//  새로운 이미지가 덮혀질 배경 이미지의 대응 좌표를 구한다.
	int skip = GetModXSize(); // 보정 가로 사이즈를 구한다.

	int iStart=(GetYSize()-y-cy)*skip + x*m_nColors;

	for (int i=0;i<cy;i++)
	{
		memcpy(	lpDibBack + iStart + i*( skip ) , 
				lpSrc + iStart + i*(skip), cx * m_nColors );
	}

	if(hDC) PaintBack(x,y,cx,cy,hDC);
}

// 배경 이미지위에 새로운 이미지를 복사한다.
void CNetDib::PutOverBack(int x,int y,CNetDib *pDib,HDC hDC)
{
	if(!szDibBack) return;

	int nBackSizeX=GetXSize();
	int nBackSizeY=GetYSize();
	int nForeSizeX=pDib->GetXSize();
	int nForeSizeY=pDib->GetYSize();
	int nForeSkip = pDib->GetModXSize();

	// 배경의 범위를 넘어가면 중지. (클리핑 불가)
	if(x<0 || y<0 || x+nForeSizeX>nBackSizeX || y+nForeSizeY>nBackSizeY) return;

	char strBack[3];
	strBack[0]=(char)0x00;
	strBack[1]=(char)0xff;
	strBack[2]=(char)0x00;	// 흰색

	LPTSTR lpDibFore;
	lpDibFore = pDib->bFindDib();
	if(lpDibFore==NULL) return;

	// 새로운 이미지를 배경이미지위에 복사한다.
	int i,j;

	//  새로운 이미지가 덮혀질 배경 이미지의 대응 좌표를 구한다.
	int skip = GetModXSize(); // 보정 가로 사이즈를 구한다.
	int iStart=(nBackSizeY-y-nForeSizeY)*skip + x*m_nColors;

	for(i=0;i<nForeSizeY;i++) {
		for(j=0;j<nForeSizeX;j++) {
			if(memcmp(lpDibFore + i*nForeSkip + j*m_nColors,strBack,m_nColors)) 
			{
				memcpy(lpDibBack + iStart + i*skip + j*m_nColors,
					   lpDibFore + i*nForeSkip + j*m_nColors, m_nColors);
			}
		}
	}

	if(hDC) PaintBack(x,y,nForeSizeX,nForeSizeY,hDC);
}

// 배경 이미지를 그린다.
void  CNetDib::PaintBack(int x,int y,HDC hDC)
{
	if(!szDibBack) return;

	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal=::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);

	::SetDIBitsToDevice(hDC,                    // hDC
								   x,             // DestX
								   y,              // DestY
								   m_nWidthX,        // nDestWidth
								   m_nWidthY,       // nDestHeight
								   0,					// SrcX
								   0,					// SrcY
								   0,					// nStartScan
								   (WORD)m_nWidthY,  // nNumScans
								   lpDibBack,                  // lpBits
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage

	if (hOldPal != NULL) {
		::SelectPalette(hDC,hOldPal,TRUE);
	}
}

// 배경이미지의 일부분을 그린다.
void  CNetDib::PaintBack(int x,int y,int cx,int cy,HDC hDC)
{    
	if(!szDibBack) return;

	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       

	if(m_palDIB != NULL) {	
		hPal = (HPALETTE) m_palDIB->m_hObject;
		hOldPal=::SelectPalette(hDC, hPal, TRUE);
	    RealizePalette(hDC);
	}
	else {
		if(SetSystemPalette(hDC))
		    RealizePalette(hDC);
	}

	int skip = GetModXSize(); // 보정 가로 사이즈를 구한다.

	int iStart=(GetYSize()-y-cy)*skip + x*m_nColors;

	::SetStretchBltMode(hDC, COLORONCOLOR);

	::SetDIBitsToDevice(hDC,                    // hDC
								   x,             // DestX
								   y,              // DestY
								   cx,        // nDestWidth
								   cy,       // nDestHeight
								   0,					// SrcX
								   0,					// SrcY
								   0,					// nStartScan
								   (WORD)cy,  // nNumScans
								   lpDibBack+iStart,                  // lpBits
								   bmpInfo,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage

	if (hOldPal != NULL) {
		::SelectPalette(hDC,hOldPal,TRUE);
	}
}

long CNetDib::GetBmpSize(void)
{
	return (sizeof(bmpHeader)+(long)(bmpHeader.bfSize-sizeof(bmpHeader)));
}

int CNetDib::GetModXSize()
{
	int nBackSizeX,skip;

	nBackSizeX = GetXSize();
	if(nBackSizeX%4==0)			skip = nBackSizeX * m_nColors;
	else if(nBackSizeX%4==3)	skip = ( ((int)nBackSizeX/4)*4 + 4 ) * m_nColors;
	else if(nBackSizeX%4==2)	skip = nBackSizeX * m_nColors + 2;
	else						skip = nBackSizeX * m_nColors + 1;
	return skip;
}

void CNetDib::Save(CFile *fp)
{
	fp->Write((LPTSTR)&bmpHeader,sizeof(bmpHeader));
	fp->Write((LPTSTR)lpDib,bmpHeader.bfSize-sizeof(bmpHeader));
}

//이 함수도 가로 사이즈가 4의 배수가 아닌 경우는 수정해야함. 근데 언제 하지 ?
void CNetDib::MirrorImage(void)
{
	if(!szDib) return;

	int nSizeX=GetXSize();
	int nSizeY=GetYSize();
	char *temp[3];
	LPTSTR lpDibSrc=bFindDib();          
	int skip=GetModXSize();
	int i,j;
	for(i=0;i<nSizeY;i++) {
		for(j=0;j<nSizeX/2;j++) {
			memcpy(temp,lpDibSrc+(i*nSizeX+j)*m_nColors,m_nColors);
			memcpy(lpDibSrc+(i*nSizeX+j)*m_nColors,lpDibSrc+((i+1)*nSizeX-(j+1))*m_nColors,m_nColors);
			memcpy(lpDibSrc+((i+1)*nSizeX-(j+1))*m_nColors,temp,m_nColors);
		}
	}
}

HBITMAP CNetDib::GetBitmap()
{
	HBITMAP 		hbm;
	CPalette		pal;
	CPalette*		pOldPal;
	CClientDC		dc(NULL);
	LPTSTR    lpDIBBits;          
	
	if (lpDib == NULL) return NULL;

	int nColors = bmpInfoHeader->biClrUsed ? bmpInfoHeader->biClrUsed : 
					1 << bmpInfoHeader->biBitCount;

	//실질 이미지로 분리한다.
	lpDIBBits = bFindDib();

	// Create and select a logical palette if needed
	if( nColors <= 256 && dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmpInfo->bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmpInfo->bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmpInfo->bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		pal.CreatePalette( pLP );

		delete[] pLP;

		// Select and realize the palette
		pOldPal = dc.SelectPalette( &pal, FALSE );
		dc.RealizePalette();
	}

	
	hbm = CreateDIBitmap(
			dc.GetSafeHdc(),	// handle to device context
			bmpInfoHeader,		// pointer to bitmap info header 
			(LONG)CBM_INIT,		// initialization flag
			lpDIBBits,			// pointer to initialization data 
			bmpInfo,			// pointer to bitmap info
			DIB_RGB_COLORS );	// color-data usage 

	if (pal.GetSafeHandle())
		dc.SelectPalette(pOldPal,FALSE);

	return hbm;
}

