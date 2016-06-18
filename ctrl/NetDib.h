//NetDib.H BMP ȭ���� ����ϴ� Ŭ����

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
	int bGetColorNums(); //Į����� ��´�.

// ��� �̹����� ���� ���� �Լ�
	void PrepareBack(void);

	void InitBack(HDC hDC=NULL);
	void InitBack(int x,int y,int cx,int cy,HDC hDC=NULL);
	void PutOverBack(int x,int y,CNetDib *pDib,HDC hDC=NULL);

	void PaintBack(int x,int y,HDC hDC);
	void PaintBack(int x,int y,int cx,int cy,HDC hDC);

// ���ϰ��� �Լ�
	long GetBmpSize(void);
	void Save(CFile *fp);

// Mirror Image ��ȯ
	void MirrorImage(void);

protected:				   
	//���
	TCHAR m_strFileName[MAX_PATH];//bmp ȭ�ϸ�
	TCHAR m_strMessageTemp[DEFAULTLEN];//���� �޼����� ��� template
	BITMAPFILEHEADER bmpHeader; //bmp ȭ���� ���
	LPBITMAPINFOHEADER bmpInfoHeader; //bmp �������
	LPBITMAPCOREHEADER bmcInfoHeader; 
	LPBITMAPINFO bmpInfo; //bmp �������
	LPBITMAPCOREINFO bmcInfo; 
	int m_nWidthX;
	int	m_nWidthY;
	HANDLE szPal;
	LPLOGPALETTE lpPal;     

	HANDLE szDib;
	LPTSTR  lpDib;
	int m_nColorNums;//���� bmp�� �÷���
	int m_nColors;//16M Color�� ���:3, 256 Color�� ���:1 
	int m_nDibStyle;//IS_WIN30_DIB �ΰ� �ƴѰ� ����
	CPalette* m_palDIB;//����Ʈ

	HANDLE szDibBack;
	LPTSTR lpDibBack;

protected:
	//���� �Լ�
	int bLoadBmpFile();
	BOOL bCreatePalette(); //����Ʈ�� �����.
	BOOL bLoadBmpFile(CFile *fp);
	LPTSTR bFindDib();//����Ÿ���� ��Ʈ�� ����Ÿ�� ã�´�.
	WORD bPaletteSize();//����Ʈ ũ�⸦ ��´�.
	int GetModXSize();
	// Dib���� Bitmap�� ���ϴ� �Լ�
	HBITMAP GetBitmap();
};

#endif 	// __NETDIB_H