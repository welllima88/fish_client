//NetDib.H BMP 화일을 출력하는 클래스

#if !defined( __NETDIB_H )
#define __NETDIB_H

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.05.23	**************************************** //
// *******			**************************************** //
// ********************************************************************* //

#define DEFAULTLEN 80
#define DIB_HEADER_MARKER ((WORD)('M'<<8) | 'B')
#define PALVERSION   0x300
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define DIB_WIDTHBYTES(bits)   (((bits) + 31) / 32 * 4)

class CNetDib
{

public:
	CNetDib();
	CNetDib(LPTSTR filename);
	CNetDib(CFile *fp);
	~CNetDib();
	void  PaintImage(int x, int y, HDC hDC);
	void  PaintImage(int x, int y, int cx, int cy,HDC hDC );
	void  PaintImage(int x,int y,HDC hDC,COLORREF color);

	void  PaintImagePart(HDC hDC, int x, int y, int cx, int cy );
	void  PaintImagePart(HDC hDC,int x,int y,int cx,int cy,COLORREF color);
	void  PaintImagePart(HDC hDC, int x, int y, int cx, int cy, int sx, int sy );
	void  PaintOverBack(int x,int y,HDC hDC,CNetDib *pDibBack);
	
	int	SetSystemPalette(HDC hDC);
	int SaveImage(LPTSTR filename=NULL);
	int GetXSize(){ return m_nWidthX;}
	int GetYSize(){ return m_nWidthY;}

	int DibCopy(int x, int y,int WidthX, int WidthY,CNetDib *Source, TCHAR *Dest);
	int bGetColorNums(); //칼라수를 얻는다.

// 배경 이미지로 쓰일 때의 함수
	void PrepareBack(void);

	void InitBack(HDC hDC=NULL);
	void InitBack(int x,int y,int cx,int cy,HDC hDC=NULL);
	void PutOverBack(int x,int y,CNetDib *pDib,HDC hDC=NULL);

	void PaintBack(int x,int y,HDC hDC);
	void PaintBack(int x,int y,int cx,int cy,HDC hDC);

// 파일관련 함수
	long GetBmpSize(void);
	void Save(CFile *fp);

// Mirror Image 변환
	void MirrorImage(void);

protected:				   
	//멤버
	TCHAR m_strFileName[MAX_PATH];//bmp 화일명
	TCHAR m_strMessageTemp[DEFAULTLEN];//에라 메세지를 담는 template
	BITMAPFILEHEADER bmpHeader; //bmp 화일의 헤더
	LPBITMAPINFOHEADER bmpInfoHeader; //bmp 정보헤더
	LPBITMAPCOREHEADER bmcInfoHeader; 
	LPBITMAPINFO bmpInfo; //bmp 정보헤더
	LPBITMAPCOREINFO bmcInfo; 
	int m_nWidthX;
	int	m_nWidthY;
	HANDLE szPal;
	LPLOGPALETTE lpPal;     

	HANDLE szDib;
	LPTSTR  lpDib;
	int m_nColorNums;//현재 bmp의 컬러수
	int m_nColors;//16M Color인 경우:3, 256 Color인 경우:1 
	int m_nDibStyle;//IS_WIN30_DIB 인가 아닌가 설정
	CPalette* m_palDIB;//빨레트

	HANDLE szDibBack;
	LPTSTR lpDibBack;

protected:
	//내부 함수
	int bLoadBmpFile();
	BOOL bCreatePalette(); //빨레트를 만든다.
	BOOL bLoadBmpFile(CFile *fp);
	LPTSTR bFindDib();//데이타에서 비트멥 데이타만 찾는다.
	WORD bPaletteSize();//빨레트 크기를 얻는다.
	int GetModXSize();
	// Dib에서 Bitmap을 구하는 함수
	HBITMAP GetBitmap();
};

#endif 	// __NETDIB_H