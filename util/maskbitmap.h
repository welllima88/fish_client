#ifndef _MASKBITMAP_H
#define _MASKBITMAP_H

#ifndef _AFXWIN_H
#include <afxwin.h>
#endif

void MakeMaskBitmap(
	CBitmap* pBmpSource, CBitmap* pBmpMask,
	COLORREF clrpTransColor, // Pass 0xFFFFFFFF if unknown
	int iTransPixelX = 0,
	int iTransPixelY = 0 );

#endif
