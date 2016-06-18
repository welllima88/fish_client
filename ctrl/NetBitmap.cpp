//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!
#include "stdafx.h"
//VISUAL STUDIO 4.x users MUST INCLUDE "stdafx.h" at the BEGINNING!!!!

#include "NetBitmap.h"

// ********************************************************************* //
// *******				******************************** //
// ******* VERSION: 2000.05.23		******************************** //
// *******				******************************** //
// ********************************************************************* //

// NetBitmap.cpp: implementation of the CNetBitmap class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetBitmap::CNetBitmap()
{
}

CNetBitmap::~CNetBitmap()
{
}

int CNetBitmap::GetWidth()
{
	BITMAP bm;
	GetBitmap(&bm);
	return bm.bmWidth;
}

int CNetBitmap::GetHeight()
{
	BITMAP bm;
	GetBitmap(&bm);
	return bm.bmHeight;
}

void CNetBitmap::DrawTransparent(CDC * pDC, int x, int y, int cx, int cy, COLORREF maskClr)
{
    enum {SRCMASK     =   0x00220326,};
    CPoint ptOrigin (x, y);
    CBitmap* bmp    =   this;
    
    //////////////////////////////////////////////////////////////////////////
    // device context creation to draw transparently
    CDC memDC, maskDC, buffDC, copyDC;
    memDC.CreateCompatibleDC(pDC);
    maskDC.CreateCompatibleDC(pDC);
    buffDC.CreateCompatibleDC(pDC);
    copyDC.CreateCompatibleDC(pDC);
    
    BITMAP bm;
    bmp->GetBitmap(&bm);
    
    CBitmap buffBitmap;
    buffBitmap.CreateCompatibleBitmap( pDC, bm.bmWidth, bm.bmHeight);
    CBitmap* oldbuffBitmap = buffDC.SelectObject( &buffBitmap );
    
    CBitmap maskBitmap; 
    maskBitmap.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL );
    CBitmap* oldmaskBitmap = maskDC.SelectObject( &maskBitmap );
    
    buffDC.FillSolidRect( &CRect(0, 0, bm.bmWidth, bm.bmHeight), maskClr);

    memDC.SelectObject(bmp);
    buffDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY);

//    pImageList->DrawIndirect(&buffDC, nIndex, CPoint(0,0), sizeImage, CPoint(0, 0));

    maskDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &buffDC, 0, 0, SRCCOPY );
    
    // draw icon transparently after making mask bitmap img.
    CBitmap copyBitmap;
    copyBitmap.CreateCompatibleBitmap( pDC, bm.bmWidth, bm.bmHeight);
    CBitmap* oldcopyBitmap = copyDC.SelectObject( &copyBitmap );
    copyDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, pDC, ptOrigin.x, ptOrigin.y, SRCCOPY );
    copyDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &maskDC, 0, 0, SRCAND );
    buffDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &maskDC, 0, 0, SRCMASK );
    copyDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &buffDC, 0, 0, SRCPAINT );
    pDC->BitBlt(ptOrigin.x, ptOrigin.y, bm.bmWidth, bm.bmHeight, &copyDC, 0, 0, SRCCOPY );   
    return;    
    /*
	CDC dcImage, dcTrans, memDC;
	CBitmap bitmap;
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	
	// 화면의 그림을 가져올 DC를 생성
	memDC.CreateCompatibleDC(pDC);
	// 화면 그림을 가져올 Bitmap을 memDC에 생성
	bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	CBitmap *pOldBitmap = memDC.SelectObject(&bitmap);
	// 이미지를 가져옴
	memDC.BitBlt(0, 0, nWidth, nHeight, pDC, x, y, SRCCOPY);

	COLORREF crOldBack = pDC->SetBkColor(RGB(255, 255, 255));
	COLORREF crOldText = pDC->SetTextColor(RGB(0, 0, 0));

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(&memDC);
	dcTrans.CreateCompatibleDC(&memDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(this);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	memDC.StretchBlt(0, 0, cx, cy, &dcImage, 0, 0, nWidth, nHeight, SRCINVERT);
	memDC.StretchBlt(0, 0, cx, cy, &dcTrans, 0, 0, nWidth, nHeight, SRCAND);
	memDC.StretchBlt(0, 0, cx, cy, &dcImage, 0, 0, nWidth, nHeight, SRCINVERT);

	pDC->BitBlt(x, y, cx, cy, &memDC, 0, 0, SRCCOPY);

	// Restore settings
	memDC.SetBkColor(crOldBack);
	memDC.SetTextColor(crOldText);
	memDC.SelectObject(pOldBitmap);
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
    */
}

void CNetBitmap::DrawTransparent(CDC * pDC, int x, int y, COLORREF crColour)
{
	CDC memDC, dcImage, dcTrans;
	CBitmap bitmap;
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 화면의 그림을 가져올 DC를 생성
	memDC.CreateCompatibleDC(pDC);
	
	// 화면 그림을 가져올 Bitmap을 memDC에 생성
	bitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight );
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// 이미지를 가져옴
	memDC.BitBlt(0, 0, nWidth, nHeight, pDC, x, y, SRCCOPY); 
	
	COLORREF crOldBack = memDC.SetBkColor(RGB(255, 255, 255));
	COLORREF crOldText = memDC.SetTextColor(RGB(0, 0, 0));

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(&memDC);
	dcTrans.CreateCompatibleDC(&memDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(this);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	memDC.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	memDC.BitBlt(0, 0, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	memDC.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);

	pDC->BitBlt(x, y, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);
	// Restore settings
	memDC.SetBkColor(crOldBack);
	memDC.SetTextColor(crOldText);
	memDC.SelectObject(pOldBitmap);
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
}

void CNetBitmap::DrawTransparent(CDC * pDC, int x, int y, CBitmap *bmBack, COLORREF crColour)
{
	CDC memDC, dcImage, dcTrans;
	CBitmap bitmap;
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// memory DC를 생성
	memDC.CreateCompatibleDC(pDC);
	
	// 배경 그림을 가져올 Bitmap을 memDC에 생성
	//bitmap.CreateCompatibleBitmap(pDC, bmBack.GetWidth(), bmBack.GetHeight());
	CBitmap* pOldBitmap = memDC.SelectObject(bmBack);

	// 이미지를 가져옴
	//memDC.BitBlt(0, 0, nWidth, nHeight, pDC, x, y, SRCCOPY); 
	
	COLORREF crOldBack = memDC.SetBkColor(RGB(255, 255, 255));
	COLORREF crOldText = memDC.SetTextColor(RGB(0, 0, 0));

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(&memDC);
	dcTrans.CreateCompatibleDC(&memDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(this);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	memDC.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	memDC.BitBlt(0, 0, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);
	memDC.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);

	pDC->BitBlt(x, y, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);
	// Restore settings
	memDC.SetBkColor(crOldBack);
	memDC.SetTextColor(crOldText);
	memDC.SelectObject(pOldBitmap);
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
}

void CNetBitmap::DrawTransparent(CDC * pDC, int x, int y, int cx, int cy, CBitmap *bmBack, COLORREF crColour)
{
	CDC memDC, dcImage, dcTrans;
	CBitmap bitmap;
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// memory DC를 생성
	memDC.CreateCompatibleDC(pDC);
	
	// 배경 그림을 가져올 Bitmap을 memDC에 생성
	//bitmap.CreateCompatibleBitmap(pDC, bmBack.GetWidth(), bmBack.GetHeight());
	CBitmap* pOldBitmap = memDC.SelectObject(bmBack);

	// 이미지를 가져옴
	//memDC.BitBlt(0, 0, nWidth, nHeight, pDC, x, y, SRCCOPY); 
	
	COLORREF crOldBack = memDC.SetBkColor(RGB(255, 255, 255));
	COLORREF crOldText = memDC.SetTextColor(RGB(0, 0, 0));

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(&memDC);
	dcTrans.CreateCompatibleDC(&memDC);

	// Select the image into the appropriate dc
	CBitmap* pOldBitmapImage = dcImage.SelectObject(this);

	// Create the mask bitmap
	CBitmap bitmapTrans;
	
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	dcTrans.BitBlt(0, 0, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);

	memDC.StretchBlt(0, 0, cx, cy, &dcImage, 0, 0, nWidth, nHeight, SRCINVERT);
	memDC.StretchBlt(0, 0, cx, cy, &dcTrans, 0, 0, nWidth, nHeight, SRCAND);
	memDC.StretchBlt(0, 0, cx, cy, &dcImage, 0, 0, nWidth, nHeight, SRCINVERT);

	pDC->BitBlt(x, y, cx, cy, &memDC, 0, 0, SRCCOPY);
	// Restore settings
	memDC.SetBkColor(crOldBack);
	memDC.SetTextColor(crOldText);
	memDC.SelectObject(pOldBitmap);
	dcImage.SelectObject(pOldBitmapImage);
	dcTrans.SelectObject(pOldBitmapTrans);
}