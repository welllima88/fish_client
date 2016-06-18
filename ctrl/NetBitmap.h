#if !defined( __NETBITMAP_H )
#define __NETBITMAP_H

// ********************************************************************* //
// *******			**************************************** //
// ******* VERSION: 2000.05.23	**************************************** //
// *******			**************************************** //
// ********************************************************************* //

// NetBitmap.h: interface for the CNetBitmap class.
//
//////////////////////////////////////////////////////////////////////

class CNetBitmap : public CBitmap  
{
public:
	CNetBitmap();
	virtual ~CNetBitmap();
	void DrawTransparent(CDC * pDC, int x, int y, COLORREF crColour);
	void DrawTransparent(CDC * pDC, int x, int y, int cx, int cy, COLORREF crColour);
	void DrawTransparent(CDC * pDC, int x, int y, CBitmap *bmBack, COLORREF crColour);
	void DrawTransparent(CDC * pDC, int x, int y, int cx, int cy, CBitmap *bmBack, COLORREF crColour);
	int GetWidth();
	int GetHeight();

};

#endif // !defined( __NETBITMAP_H )
