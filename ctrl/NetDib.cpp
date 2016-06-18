//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!
#include "stdafx.h"
//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!

//NETDIB.CPP BMP ȭ���� ����ϴ� Ŭ����

#include "NetDib.h"
#include "NetBitmap.h"
#include <io.h>

// ************************************************************ //
// *******			******************************* //
// ******* VERSION: 2000.05.23	******************************* //
// *******			******************************* //
// ************************************************************ //

//������ ȭ�ϸ��� �־ Ŭ������ ��������鼭 �̹����� ������ 
//�ְ� �Ѵ�.

#define STR_NETDIB_NOFILE					_T("%s ȭ���� �����ϴ�")
#define STR_NETDIB_BADBMPHEADER				_T("%s BMPHEADER �� ������ �����ϴ�!")
#define STR_NETDIB_NOTBMP					_T("%s BMP ȭ���� �ƴմϴ�.")
#define STR_NETDIB_MEMORYSHORT				_T("%s �޸𸮰� �����մϴ�.")
#define STR_NETDIB_READFAIL					_T("%s ȭ���� ������ �����ϴ�.")
#define STR_NETDIB_PALETTEMEMORYSHORT		_T("%s palette �޸𸮰� �����մϴ�.")
#define STR_NETDIB_TEMPMEMORYSHORT			_T("�޸𸮰� �����մϴ�.")

CNetDib::CNetDib()
{
}

CNetDib::CNetDib(LPTSTR filename)
{
	//ȭ�ϸ��� m_strFileName�� ����Ѵ�.
	lstrcpy((LPTSTR)m_strFileName,(LPTSTR)filename);
	//�̹��� �����Ϳ� ���۸� null�� �����Ѵ�.
	szDib=NULL;
	lpDib=NULL;
	m_palDIB=NULL;
	//�ȷ�Ʈ�� null�� �����Ѵ�.
	szPal=NULL;
	//bmp ȭ���� �ε��Ų��.
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
	//�ȷ�Ʈ�� null�� �����Ѵ�.
	szPal=NULL;
	//bmp ȭ���� �ε��Ų��.
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

	//ȭ���� ����.
//	bmpFile=_lopen((LPTSTR)m_strFileName,OF_READ);
	
	bmpFile=_topen((LPTSTR)m_strFileName,OF_READ);

	if(bmpFile<0) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOFILE,m_strFileName);
	   return FALSE;
	}

	//bmpHeder ����ü�� BMPHEADER ��� ������ �о� �´�.
	if(_lread(bmpFile,(LPTSTR)&bmpHeader,sizeof(bmpHeader))!=sizeof(bmpHeader)) {
		_stprintf(m_strMessageTemp,STR_NETDIB_BADBMPHEADER,m_strFileName);
		return FALSE;
	}

	//ȭ���� bmp �ϰ�쿡�� ȭ�� ó���� "BM"�̶�� ����������
	if (bmpHeader.bfType != DIB_HEADER_MARKER) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOTBMP,m_strFileName);
		return FALSE;
	}

	//�̹��� ����Ÿ ũ�⸸ŭ �޸𸮸� �����Ѵ�.
	szDib =::GlobalAlloc(GHND,bmpHeader.bfSize);
	if(szDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_MEMORYSHORT,m_strFileName);
		return FALSE;
	}

	//�̹��� ����Ÿ ���۸� ��Ʈ�� �Ҽ� �ֵ��� lpDip �� ��ŷ�Ѵ�.
	lpDib=(LPTSTR)::GlobalLock(szDib);
	UINT len;

	 //�̹��� ����Ÿ�� �д´�.
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
	//bmpHeder ����ü�� BMPHEADER ��� ������ �о� �´�.
	if((fp->Read((LPTSTR)&bmpHeader,sizeof(bmpHeader)))!=sizeof(bmpHeader)) {
		_stprintf(m_strMessageTemp,STR_NETDIB_BADBMPHEADER);
		return FALSE;
	}

	//ȭ���� bmp �ϰ�쿡�� ȭ�� ó���� "BM"�̶�� ����������
	if (bmpHeader.bfType != DIB_HEADER_MARKER) {
		_stprintf(m_strMessageTemp,STR_NETDIB_NOTBMP);
		return FALSE;
	}

	//�̹��� ����Ÿ ũ�⸸ŭ �޸𸮸� �����Ѵ�.
	szDib =::GlobalAlloc(GHND,bmpHeader.bfSize);
	if(szDib==NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_MEMORYSHORT);
		return FALSE;
	}

	//�̹��� ����Ÿ ���۸� ��Ʈ�� �Ҽ� �ֵ��� lpDip �� ��ŷ�Ѵ�.
	lpDib=(LPTSTR)::GlobalLock(szDib);

	 //�̹��� ����Ÿ�� �д´�.
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

//�ȷ�Ʈ�� �����.
BOOL CNetDib::bCreatePalette()
{
	//���� ����ϴ� Į����� �ް�
	m_nColorNums=bGetColorNums();
		
	// Source�� Į����� ���� ����Ʈ��  		
	if(m_nColorNums==0) 
	{
		m_nColors = 3;
		return TRUE;
	}
	else if	(m_nColorNums==256) m_nColors = 1;
	
   //�ʷ�Ʈ �޸𸮸� Ȯ���Ѵ���
	szPal=::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* m_nColorNums);

	if(szPal == NULL) {
		_stprintf(m_strMessageTemp,STR_NETDIB_PALETTEMEMORYSHORT,m_strFileName);
		return FALSE;
	}

	//lpPal ��ŷ�ϰ�
	lpPal=(LPLOGPALETTE) ::GlobalLock((HGLOBAL) szPal);

	lpPal->palVersion=PALVERSION;
	lpPal->palNumEntries=(WORD)m_nColorNums;

    //Į�����ŭ �ȷ�Ʈ�� Į��Ʈ ��Ʈ���� �ִ´�.
	for(int i=0;i<(int)m_nColorNums;i++) {
		if (m_nDibStyle) { //win3.0 �̻�����ϰ�� ������ ���� �ϰ�
			lpPal->palPalEntry[i].peRed = bmpInfo->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen =bmpInfo->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue = bmpInfo->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
		else {//�׿ܿ��� ������ ���� �Ѵ�.
			lpPal->palPalEntry[i].peRed = bmcInfo->bmciColors[i].rgbtRed;
			lpPal->palPalEntry[i].peGreen = bmcInfo->bmciColors[i].rgbtGreen;
			lpPal->palPalEntry[i].peBlue = bmcInfo->bmciColors[i].rgbtBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
	}

//    CPalette ����ü�� lpPal�� ��ũ���Ѽ� �ȷ�Ʈ�� ����ϴ�.
	m_palDIB = new CPalette;
	m_palDIB->CreatePalette(lpPal);

	return TRUE;
}

//---------------------------------------------------------------------
// GetNumOfColors
//
// DIB�� �÷����� ����
//---------------------------------------------------------------------
int CNetDib::bGetColorNums()
{
	//3.0 ������ ��Ʈ���̸�
   	if(IS_WIN30_DIB(lpDib))	{
		//m_nDibStyle  �� 3.0 ��Ʈ�� �����̶�� ����
		m_nDibStyle=TRUE;
		m_nWidthX=bmpInfoHeader->biWidth;
		m_nWidthY=bmpInfoHeader->biHeight;
		DWORD dwClrUsed;
		//���� Į����� �޾Ƽ� ���࿡ �װ��� 0�� �ƴϸ�
		//�װ��� ���� ��Ʈ���� ����ϴ� Į��� �̹Ƿ� ����
		dwClrUsed = bmpInfoHeader->biClrUsed;
		if(dwClrUsed != 0)	return (int)dwClrUsed;
	}
	else {
		m_nDibStyle=FALSE;
		m_nWidthX=bmcInfoHeader->bcWidth;
		m_nWidthY=bmcInfoHeader->bcHeight;
	}

	WORD wBitCount;
	//��Ʈ ī��Ʈ�� �޴´�.
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
// DIB�� ������ �κ��� ã�� �����͸� ����
// databits location = pDib + Header size + Palette size
//---------------------------------------------------------------------
LPTSTR CNetDib::bFindDib()
{
	return (lpDib + *(LPDWORD)lpDib + bPaletteSize());
}

//�ȷ�Ʈ ũ�⸦ �˾ƺ���.
WORD CNetDib::bPaletteSize()
{
	//3.0�̻� ���� �̹��� �̸�
   if (m_nDibStyle) return (WORD)(m_nColorNums * sizeof(RGBQUAD));
   else return (WORD)(m_nColorNums * sizeof(RGBTRIPLE));
}

//�����쿡 �����Ǿ��ִ� �ȷ�Ʈ�� ����Ѵ�.
int CNetDib::SetSystemPalette(HDC hDC)
{
	if(m_nColorNums != 0) return FALSE;
	//���� Ʈ��Į�����̸� ��������
	if((int)(!::GetDeviceCaps(hDC, RASTERCAPS)) & RC_PALETTE) return FALSE;
	//���� ��尡 ��Į�� ����̸� �ȷ�Ʈ��Ʈ���� ����� Ȯ��
	int nSysColors = ::GetDeviceCaps(hDC, NUMCOLORS);
	int nPalEntries = ::GetDeviceCaps(hDC, SIZEPALETTE);

	if(nPalEntries==0)	return FALSE;
	
	int nEntries = (nPalEntries==0) ? nSysColors : nPalEntries;

	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		nEntries * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = nEntries;
	
	//�ý��� �ȷ�Ʈ�� ���
	::GetSystemPaletteEntries(hDC, 0, nEntries,
		(LPPALETTEENTRY) ((LPBYTE) pLogPal + 2 * sizeof(WORD)));
	HPALETTE m_hPalette;
	
	//�ý��� �ȷ�Ʈ�� �����Ѵ�.
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

	//���� �̹����� �и��Ѵ�.
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
		
// ������ ������ RGB Color�� ������ ���
void  CNetDib::PaintImage(int x,int y,HDC hDC,COLORREF color)
{
	CNetBitmap bmImage;
	CDC *pDC = CDC::FromHandle(hDC);

	bmImage.m_hObject = GetBitmap();
	bmImage.DrawTransparent(pDC, x, y, color);
}

// cx,cy �� ���� ���� ũ��
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
		
// cx,cy �� ���� ���� ũ��
void  CNetDib::PaintImagePart(HDC hDC,int x,int y,int cx,int cy,COLORREF color)
{    
	LPTSTR    lpDIBBits;          
	LPTSTR   plpDIBBits;          
	BOOL     bSuccess=FALSE;     
	HPALETTE hPal=NULL;          
	HPALETTE hOldPal=NULL;       
	char     strBack[3];
	char	 strFord[3];

	if(m_nColors!=3) return;	// 16��Į�󿡼��� ����

	strBack[0]=(char)0x00;
	strBack[1]=(char)0xff;
	strBack[2]=(char)0x00;	// ���

	strFord[0]=(char)GetRValue(color);
	strFord[1]=(char)GetGValue(color);
	strFord[2]=(char)GetBValue(color);	// ���

	if(lpDib==NULL)	return;
	//���� �̹����� �и��Ѵ�.
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

	// Temp �޸𸮸� �����ϰ� ����� ������ ���´�.
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

	//�̹��� ����Ÿ ���۸� ��Ʈ�� �Ҽ� �ֵ��� lpDip �� ��ŷ�Ѵ�.
	lpTempDib=(LPTSTR)::GlobalLock(hszTempDib);

	plpDIBBits = lpDIBBits;

	int i,j;
	for (i=0;i<nDibTempSizeY;i++)
	{
		for (j=0;j<nDibTempSizeX;j++)
		{
			if (memcmp(plpDIBBits + i*skip + j*m_nColors,strBack,m_nColors))	// ������ �ƴϸ� ����
				memcpy(lpTempDib + i*skip + j*m_nColors,
						plpDIBBits + i*skip + j*m_nColors,m_nColors);
			else	// �����̸� ������ RGB�� ����
				memcpy(lpTempDib + i*skip + j*m_nColors,
						strFord,m_nColors);
		}
	}
	// end of ��溹�� 


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

	// �Ҵ���� �޸𸮸� ��ȯ�Ѵ�.
	if(hszTempDib)
	{
		::GlobalUnlock(hszTempDib);
		::GlobalFree(hszTempDib);
	}
}
		
// cx,cy �� ���� ���� ũ��
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

//�̹����� �����Ѵ�.
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

// CNetDib *pDibBack �� ���� CNetDib �� Į����� �ݵ�� ���ƾ� ��.
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
	strBack[2]=(char)0x00;	// ���

	if(lpDib==NULL)	return;
	//���� �̹����� �и��Ѵ�.
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

	// Temp �޸𸮸� �����ϰ� ����� ������ ���´�.
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

	//�̹��� ����Ÿ ���۸� ��Ʈ�� �Ҽ� �ֵ��� lpDip �� ��ŷ�Ѵ�.
	lpTempDib=(LPTSTR)::GlobalLock(hszTempDib);

	plpDIBBits = lpDIBBits;

	DibCopy( x, y, nDibTempSizeX, nDibTempSizeY, pDibBack , lpTempDib);

	int i,j;
	for (i=0;i<nDibTempSizeY;i++)
	{
		for (j=0;j<nDibTempSizeX;j++)
		{
			if (memcmp(plpDIBBits + i*skip + j*m_nColors,strBack,m_nColors))	// ������ �ƴϸ� ����
				memcpy(lpTempDib + i*skip + j*m_nColors,
						plpDIBBits + i*skip + j*m_nColors,m_nColors);
		}
	}
	// end of ��溹�� 

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

	// �Ҵ���� �޸𸮸� ��ȯ�Ѵ�.
	if(hszTempDib)
	{
		::GlobalUnlock(hszTempDib);
		::GlobalFree(hszTempDib);
	}
}

// Source�� x,y ���� Dest�� Dest�� ������ ��ŭ ������. 
// WidthX�� WidthY�� Dest�� ������� ���ƾ� ��.
int CNetDib::DibCopy(int x, int y,int WidthX, int WidthY,CNetDib *pSource, TCHAR *pDest)
{
	int		nSourceSizeY;
	int		nSourceColor, nSourceSkip;
	LPTSTR   lpSourceBits;          

	// Source�� �̹����� ����� ��.
	lpSourceBits = pSource->bFindDib();
	// Source�� Į����� ���� ����Ʈ��  
	nSourceColor = pSource->m_nColors;

	nSourceSizeY = pSource->GetYSize();
	nSourceSkip = pSource->GetModXSize();
	
	int i, startidx, skip;	// Source�� ������ġ

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

// ���ȭ������ ���� �̹����� ������ �����Ѵ�.
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

// ��� �̹����� ������ �̹����� �����Ѵ�.
void CNetDib::InitBack(HDC hDC)
{
	if(!szDibBack) return;
	
	// ����̹����� �����ϱ����� ������ �̹����� �����Ѵ�.
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
	//  ���ο� �̹����� ������ ��� �̹����� ���� ��ǥ�� ���Ѵ�.
	int skip = GetModXSize(); // ���� ���� ����� ���Ѵ�.

	int iStart=(GetYSize()-y-cy)*skip + x*m_nColors;

	for (int i=0;i<cy;i++)
	{
		memcpy(	lpDibBack + iStart + i*( skip ) , 
				lpSrc + iStart + i*(skip), cx * m_nColors );
	}

	if(hDC) PaintBack(x,y,cx,cy,hDC);
}

// ��� �̹������� ���ο� �̹����� �����Ѵ�.
void CNetDib::PutOverBack(int x,int y,CNetDib *pDib,HDC hDC)
{
	if(!szDibBack) return;

	int nBackSizeX=GetXSize();
	int nBackSizeY=GetYSize();
	int nForeSizeX=pDib->GetXSize();
	int nForeSizeY=pDib->GetYSize();
	int nForeSkip = pDib->GetModXSize();

	// ����� ������ �Ѿ�� ����. (Ŭ���� �Ұ�)
	if(x<0 || y<0 || x+nForeSizeX>nBackSizeX || y+nForeSizeY>nBackSizeY) return;

	char strBack[3];
	strBack[0]=(char)0x00;
	strBack[1]=(char)0xff;
	strBack[2]=(char)0x00;	// ���

	LPTSTR lpDibFore;
	lpDibFore = pDib->bFindDib();
	if(lpDibFore==NULL) return;

	// ���ο� �̹����� ����̹������� �����Ѵ�.
	int i,j;

	//  ���ο� �̹����� ������ ��� �̹����� ���� ��ǥ�� ���Ѵ�.
	int skip = GetModXSize(); // ���� ���� ����� ���Ѵ�.
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

// ��� �̹����� �׸���.
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

// ����̹����� �Ϻκ��� �׸���.
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

	int skip = GetModXSize(); // ���� ���� ����� ���Ѵ�.

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

//�� �Լ��� ���� ����� 4�� ����� �ƴ� ���� �����ؾ���. �ٵ� ���� ���� ?
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

	//���� �̹����� �и��Ѵ�.
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

