// GlobalIconManager.cpp: implementation of the CFishBMPManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalIconManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFishBMPManager* CFishBMPManager::_instance   =   NULL;

CFishBMPManager::CFishBMPManager()
{
    _initialize();
}

CFishBMPManager::~CFishBMPManager()
{
    vector<CBitmap*>::iterator iter = m_bmpvecArray.begin();
    for (;iter != m_bmpvecArray.end() ;iter++)
    {
        if (*iter){
            (*iter)->DeleteObject();
            delete *iter;
        }
    }
}

void    CFishBMPManager::_initialize()
{
    //////////////////////////////////////////////////////////////////////////
	// ��Ʈ�� �ʱ�ȭ : ä�� ������, �׷� ������, �ִϸ��̼� ������ ��Ÿ ���
	CBitmap tempBMP;

	// CHANNEL IMG LOADING    16bit bitmap creation.		16x16 icons
	tempBMP.LoadBitmap(IDB_REPOSITORY_CHANNEL_ICONS);
	m_bmplistChannelIcons.Create(16, 16, ILC_COLOR32, 0, 1);
	m_bmplistChannelIcons.Add(&tempBMP, (COLORREF)-1);
	m_bmplistChannelIcons.SetBkColor(CLR_NONE);
	tempBMP.DeleteObject();

    // GROUP IMG LOADING
	tempBMP.LoadBitmap(IDB_REPOSITORY_GROUP_ICONS);
	m_bmplistGroupIcons.Create(16, 16, ILC_COLOR32, 0, 1);
	m_bmplistGroupIcons.Add(&tempBMP, (COLORREF)-1);
	m_bmplistGroupIcons.SetBkColor(CLR_NONE);
	tempBMP.DeleteObject();

    // ANIMATION IMG LOADING
    tempBMP.LoadBitmap(IDB_REPOSITORY_ANIMATE_SANDTIMER);
    m_bmplistAnimateIcons.Create(16, 16, ILC_COLOR32, 0, 1);
    m_bmplistAnimateIcons.Add(&tempBMP, (COLORREF)-1);
    m_bmplistAnimateIcons.SetBkColor(CLR_NONE);
    tempBMP.DeleteObject();

    // NEW ITEM POST
    m_bmpNewPost.LoadBitmap(IDB_LISTCTRL_NEW_POST);

    // TAB IMG
    tempBMP.LoadBitmap(IDB_TABCTRL_ICONS);
    m_bmplistTabIcons.Create(16, 16, ILC_COLOR32, 0, 1);
    m_bmplistTabIcons.Add(&tempBMP, (COLORREF)-1);
    tempBMP.DeleteObject();

	// GROUP ITEM BK
    m_bmplistGroupBk.LoadBitmap(IDB_REPOSITORY_GROUP_BG_N);
    m_bmplistGroupBkS.LoadBitmap(IDB_REPOSITORY_GROUP_BG_S);
    
    // LISTCTRL PROGRESS BAR
    tempBMP.LoadBitmap(IDB_LISTCTRL_ATTENTION_PROGRESSBAR);
    m_bmplistListCtrlProgressBar.Create(1, 7, ILC_COLOR32, 0, 1);
    m_bmplistListCtrlProgressBar.Add(&tempBMP, (COLORREF)-1);
    m_bmplistListCtrlProgressBar.SetBkColor(CLR_NONE);
    tempBMP.DeleteObject();

    // SYSMENU IMG
    tempBMP.LoadBitmap(IDB_MAINFRAME_ALLSYSMENU_ICONS);
    m_bmplistSysmenuIcons.Create(13, 13, ILC_COLOR32, 0, 1);
    m_bmplistSysmenuIcons.Add(&tempBMP, (COLORREF)-1);
    tempBMP.DeleteObject();

    // Splitter ��Ʈ���� �ε�
    m_bmpHoriSpliter.LoadBitmap(IDB_SPLITTER_HORIZONTAL_BMP);
    m_bmpVertSpliter.LoadBitmap(IDB_SPLITTER_VERTICAL_BMP);

    // MainFrame ��Ʈ���� �ε�
    m_bmpMainframeBk_TopLeft.LoadBitmap(IDB_MAINFRAME_TOPLEFT);
    m_bmpMainframeBk_TopRight.LoadBitmap(IDB_MAINFRAME_TOPRIGHT);
    m_bmpMainframeBk_BottomLeft.LoadBitmap(IDB_MAINFRAME_BOTTOMLEFT);
    m_bmpMainframeBk_BottomRight.LoadBitmap(IDB_MAINFRAME_BOTTOMRIGHT);
    m_bmpMainframeBk_Left.LoadBitmap(IDB_MAINFRAME_LEFT);
    m_bmpMainframeBk_Right.LoadBitmap(IDB_MAINFRAME_RIGHT);
    m_bmpMainframeBk_BottomCenter.LoadBitmap(IDB_MAINFRAME_BOTTOMCENTER);
    m_bmpMainframeBk_Top.LoadBitmap(IDB_MAINFRAME_TOP_SPAN);

    // REPOSITORY IMG
    m_bmpSubscribeBk_Dlg.LoadBitmap(IDB_REPOSITORY_DLG_BKGND);
    m_bmpSubscribeBk_DlgLeft.LoadBitmap(IDB_REPOSITORY_DLG_BKGND_LEFT);
    m_bmpSubscribeBk_DlgRight.LoadBitmap(IDB_REPOSITORY_DLG_BKGND_RIGHT);
    m_bmpSubscribeBk_DlgTop.LoadBitmap(IDB_REPOSITORY_DLG_BKGND_TOP);
    m_bmpSubscribeBk_DlgBottom.LoadBitmap(IDB_REPOSITORY_DLG_BKGND_BOTTOM);

    // EXPLORER DLG 
    m_bmpExplorerDlg_Bkgnd.LoadBitmap(IDB_EXPLORERBAR_BKGND);
    m_bmpExplorerDlg_Bkgnd_Middle.LoadBitmap(IDB_EXPLORERBAR_BKGND_MIDDLE);

    // TABCTRL IMG
    m_bmpSelectedItem_Left.LoadBitmap(IDB_TABCTRL_SELECT_BKGND_LEFT);
    m_bmpSelectedItem_Right.LoadBitmap(IDB_TABCTRL_SELECT_BKGND_RIGHT);
    m_bmpSelectedItem_Span.LoadBitmap(IDB_TABCTRL_SELECT_BKGND_SPAN);
    m_bmpDeselectedItem_Left.LoadBitmap(IDB_TABCTRL_DESELECT_BKGND_LEFT);
    m_bmpDeselectedItem_Right.LoadBitmap(IDB_TABCTRL_DESELECT_BKGND_RIGHT);
    m_bmpDeselectedItem_Span.LoadBitmap(IDB_TABCTRL_DESELECT_BKGND_SPAN);
    
    m_bmpTabPrevBtn.LoadBitmap(IDB_TABCTRL_PREV_N);
    m_bmpTabNextBtn.LoadBitmap(IDB_TABCTRL_NEXT_N);

    //////////////////////////////////////////////////////////////////////////
    m_bmpTreeGrpExpandN.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED_N);
    m_bmpTreeGrpExpandO.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED_O);
    m_bmpTreeGrpExpandP.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED_P);

    m_bmpTreeGrpCollapseN.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED2_N);
    m_bmpTreeGrpCollapseO.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED2_O);
    m_bmpTreeGrpCollapseP.LoadBitmap(IDB_REPOSITORY_GRP_EXPANDED2_P);
}

CImageList*       CFishBMPManager::getChannelIcon()
{
    return &m_bmplistChannelIcons;
}

CImageList*       CFishBMPManager::getGroupIcon()
{
    return &m_bmplistGroupIcons;
}

CImageList*       CFishBMPManager::getAnimateIcons()
{
    return &m_bmplistAnimateIcons;
}

CImageList*        CFishBMPManager::getTabIcons()
{
    return &m_bmplistTabIcons;
}

/************************************************************************
getGroupBk  �׷� ����� �����´�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*            CFishBMPManager::getGroupBk(int type /* =   0*/)
{
    CBitmap* ret =   NULL;
    switch(type)
    {
    case 0:
        ret =   &m_bmplistGroupBk;
        break;
    case 1:
        ret =   &m_bmplistGroupBkS;
        break;
    }
    return ret;
}

/************************************************************************
getSysmenuBtn   ���׷��� �ý��� �޴� �̹����� �����´�.
@PARAM  : 
@RETURN : 
@REMARK : 
    ���� �������� �ý��� ��ư�� ���Ѵ�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CImageList*         CFishBMPManager::getSysmenuBtn()
{
    return &m_bmplistSysmenuIcons;
}

/************************************************************************
GetTreeGrpExpand    Ʈ���� �׷� ������ ǥ�� �̹����� �����´�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*    CFishBMPManager::GetTreeGrpExpand(int type /*   =   0*/, BOOL expand /* = TRUE*/)
{
    enum {
        NORMAL  =   0,
        OVER    =   1,
        PRESS   =   2,
    };

    CBitmap* ret    =   NULL;

	if(expand)
	{
		switch(type)
		{
		case OVER:
			ret =   &m_bmpTreeGrpExpandO;
			break;
		case PRESS:
			ret =   &m_bmpTreeGrpExpandP;
			break;
		case NORMAL:
		default:
			ret =   &m_bmpTreeGrpExpandN;
			break;
		}
	}
	else
	{
		switch(type)
		{
		case OVER:
			ret =   &m_bmpTreeGrpCollapseO;
			break;
		case PRESS:
			ret =   &m_bmpTreeGrpCollapseP;
			break;
		case NORMAL:
		default:
			ret =   &m_bmpTreeGrpCollapseN;
			break;
		}
	}

    return ret;
}

/************************************************************************
GetTabBtnsBmp   ���� ��ư �������� �ε��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*    CFishBMPManager::GetTabBtnsBmp(int type /*  =   0*/)
{
    enum {
        PREV_N  =   0,
        NEXT_N  =   1,
    };

    CBitmap*    ret;
    switch (type)
    {
    case NEXT_N:
        ret =   &m_bmpTabNextBtn;
        break;

    case PREV_N:
        ret =   &m_bmpTabPrevBtn;
        break;

    default:
        break;

    }
    return ret;
}

/************************************************************************
getSpliterBmp   ���ø��� ���� �̹����� �ε�
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*      CFishBMPManager::getSpliterBmp(bool isVert)
{
    if (isVert)
    {
        return &m_bmpVertSpliter;
    } else {
        return &m_bmpHoriSpliter;
    }
}

/************************************************************************
getSubscribeBkgnd   Repository â�� ���׸��� �����´�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*      CFishBMPManager::getSubscribeBkgnd(int type /*=0*/)
{
    enum {
        TOP     =   0,
        LEFT    =   1,
        RIGHT   =   2,
        BOTTOM  =   3,
    };

    CBitmap* ret    =   NULL;
    switch(type)
    {
    case TOP:
        ret =   &m_bmpSubscribeBk_DlgTop;
        break;
    case LEFT:
        ret =   &m_bmpSubscribeBk_DlgLeft;
        break;
    case RIGHT:
        ret =   &m_bmpSubscribeBk_DlgRight;
        break;
    case BOTTOM:
        ret =   &m_bmpSubscribeBk_DlgBottom;
        break;
    default:
        ret =   &m_bmpSubscribeBk_Dlg;
        break;
    }

    return ret;
}

/************************************************************************
getNewPostIcon  �� ����Ʈ�� ��Ÿ���� �̹����� �������´�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*      CFishBMPManager::getNewPostIcon()
{
    return &m_bmpNewPost;
}

/************************************************************************
getMainframeBk  ���� �������� ��� �׸��� ��´�.
@PARAM  : 
@RETURN : 
@REMARK : 
    ���� �������� ��� �׸��� ���´�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*      CFishBMPManager::getMainframeBk(int type)
{
    enum {
        TOPLEFT     = 0, 
        TOPRIGHT    = 1, 
        BOTTOMLEFT  = 2, 
        BOTTOMRIGHT = 3, 
        BOTTOMCENTER= 4, 
        LEFT        = 5, 
        RIGHT       = 6,
        TOP_SPAN    = 7,
    };

    CBitmap* RET    =   NULL;
    switch (type)
    {
    case TOP_SPAN:
        RET =   &m_bmpMainframeBk_Top;
        break;
    case TOPLEFT:
        RET =   &m_bmpMainframeBk_TopLeft;
        break;
    case TOPRIGHT:
        RET =   &m_bmpMainframeBk_TopRight;
        break;
    case BOTTOMLEFT:
        RET =   &m_bmpMainframeBk_BottomLeft;
        break;
    case BOTTOMRIGHT:
        RET =   &m_bmpMainframeBk_BottomRight;
        break;
    case BOTTOMCENTER:
        RET =   &m_bmpMainframeBk_BottomCenter;
        break;
    case LEFT:
        RET =   &m_bmpMainframeBk_Left;
        break;
    case RIGHT:
        RET =   &m_bmpMainframeBk_Right;
        break;
    default:
        TRACE(_T("CFishBMPManager::getMainframeBk �˼� ���� ���� �߻�\n"));
        break;
    }

    return RET;
}

/************************************************************************
getExplorerBkgnd    ������ �̵� ������ ���� �̹����� �����´�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap*    CFishBMPManager::getExplorerBkgnd(int type)
{
    enum {
        MIDDLE = 1,
    };
    switch (type)
    {
    case MIDDLE:
        return &m_bmpExplorerDlg_Bkgnd_Middle;
        break;
    default:
        return &m_bmpExplorerDlg_Bkgnd;
        break;
    }
}

/************************************************************************
drawTransparent �־��� ���ڸ� �������� ������ �׸��� �׸���.
@PARAM  : 
    pDC         �׸��� �׸� DC
    pImageList  �׷��� �׸��� ImageList
    nIndex      �׷��� �׸��� ����Ʈ���� �ε���
    ptOrigin    �׷��� ������
    bExtract    �̹������� ������� ������ ���ΰ�?
    ptExtractPoint  ������� ������ ��ġ�� ����ΰ�?
    maskClr     �������� ���� ��� ������� � ���ΰ�?
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void        CFishBMPManager::drawTransparent(CDC* pDC, CImageList* pImageList, int nIndex, CPoint ptOrigin, BOOL bExtract, CPoint ptExtractPoint, COLORREF maskClr)
{
    if (bExtract)
    {
        CBitmap bmp;
        GetBMPfromList(pDC, pImageList, nIndex, &bmp);
        CDC extDC;
        extDC.CreateCompatibleDC(pDC);
        extDC.SelectObject(&bmp);
        maskClr =   extDC.GetPixel(ptExtractPoint);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // device context creation to draw transparently
    CDC memDC, maskDC, buffDC, copyDC;
    memDC.CreateCompatibleDC(pDC);
    maskDC.CreateCompatibleDC(pDC);
    buffDC.CreateCompatibleDC(pDC);
    copyDC.CreateCompatibleDC(pDC);
    
    IMAGEINFO bm;
    pImageList->GetImageInfo(nIndex,&bm);

    CSize sizeImage ( bm.rcImage.right - bm.rcImage.left, bm.rcImage.bottom - bm.rcImage.top );

    CBitmap buffBitmap;
    buffBitmap.CreateCompatibleBitmap( pDC, sizeImage.cx, sizeImage.cy);
    CBitmap* oldbuffBitmap = buffDC.SelectObject( &buffBitmap );
    
    CBitmap maskBitmap; 
    maskBitmap.CreateBitmap( sizeImage.cx, sizeImage.cy, 1, 1, NULL );
    CBitmap* oldmaskBitmap = maskDC.SelectObject( &maskBitmap );
    
    buffDC.FillSolidRect( &CRect(0, 0, sizeImage.cx, sizeImage.cy), maskClr);
    pImageList->DrawIndirect(&buffDC, nIndex, CPoint(0,0), sizeImage, CPoint(0, 0));
    maskDC.BitBlt( 0, 0, sizeImage.cx, sizeImage.cy, &buffDC, 0, 0, SRCCOPY );
    
    // draw icon transparently after making mask bitmap img.
    CBitmap copyBitmap;
    copyBitmap.CreateCompatibleBitmap( pDC, sizeImage.cx, sizeImage.cy);
    CBitmap* oldcopyBitmap = copyDC.SelectObject( &copyBitmap );
    copyDC.BitBlt( 0, 0, sizeImage.cx, sizeImage.cy, pDC, ptOrigin.x, ptOrigin.y, SRCCOPY );
    copyDC.BitBlt( 0, 0, sizeImage.cx, sizeImage.cy, &maskDC, 0, 0, SRCAND );
    buffDC.BitBlt( 0, 0, sizeImage.cx, sizeImage.cy, &maskDC, 0, 0, SRCMASK );
    copyDC.BitBlt( 0, 0, sizeImage.cx, sizeImage.cy, &buffDC, 0, 0, SRCPAINT );
    pDC->BitBlt(ptOrigin.x, ptOrigin.y, sizeImage.cx, sizeImage.cy, &copyDC, 0, 0, SRCCOPY );   
    return;
}

/************************************************************************
drawTransparent �־��� ���ڸ� �������� ������ �׸��� �׸���.
@PARAM  : 
    pDC         �׸��� �׸� DC
    bmp         �������� �׷��� �̹����� ����
    ptOrigin    �׷��� ������
    bExtract    �̹������� ������� ������ ���ΰ�?
    ptExtractPoint  ������� ������ ��ġ�� ����ΰ�?
    maskClr     �������� ���� ��� ������� � ���ΰ�?
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void        CFishBMPManager::drawTransparent(CDC* pDC, CBitmap* bmp, CPoint ptOrigin, BOOL bExtract, CPoint ptExtractPoint, COLORREF maskClr)
{
    if (bExtract)
    {
        CDC extDC;
        extDC.CreateCompatibleDC(pDC);
        extDC.SelectObject(bmp);
        maskClr =   extDC.GetPixel(ptExtractPoint);
    }
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
}

/************************************************************************
drawTiledBlt CBitmap ������ �����͸� �̿� Ÿ�� ���·� ����� �׸���.
@PARAM  : 
@RETURN : 
@REMARK : 
    �ܼ� ����� Redraw ����� Stretch Blt �� �׸��� ���� ���� ���ϴ�.
    Stretch Blt �� ����� �����ϱ� ������ ���� ���� ����� ���� ������ 
    �ʿ��� �κп��� ���ϴ� ���� ��ȿ�� �� ��.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void        CFishBMPManager::drawTiledBlt(CDC* pDC, CBitmap* bmp, CRect destRt, COLORREF transparentClr)
{
    BITMAP bm;

    //////////////////////////////////////////////////////////////////////////
    // ��� Ÿ�� ������ �׸��� 
    {
        bmp->GetBitmap(&bm);
        for (int i=destRt.left; i <= destRt.right - bm.bmWidth; i += bm.bmWidth)
        {
            for (int j=destRt.top; j <= destRt.bottom - bm.bmHeight; j += bm.bmHeight)
            {
                drawTransparent(pDC, bmp, CPoint(i, j), FALSE, CPoint(0, 0), transparentClr);
            }
        }
    } 
    return;
}

/************************************************************************
drawTiledBlt ImageList ������ �����͸� �̿� Ÿ�� ���·� ����� �׸���.
@PARAM  : 
@RETURN : 
@REMARK : 
    �ܼ� ����� Redraw ����� Stretch Blt �� �׸��� ���� ���� ���ϴ�.
    Stretch Blt �� ����� �����ϱ� ������ ���� ���� ����� ���� ������ 
    �ʿ��� �κп��� ���ϴ� ���� ��ȿ�� �� ��.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void        CFishBMPManager::drawTiledBlt(CDC* pDC, CImageList* pImageList, int nIndex, CRect destRt, COLORREF transparentClr)
{
    //////////////////////////////////////////////////////////////////////////
    // ��� Ÿ�� ������ �׸��� 
    {
        IMAGEINFO im;
        pImageList->GetImageInfo(0, &im);
 
        for (int i=destRt.left; i <= destRt.right - im.rcImage.right; i += im.rcImage.right)
        {
            for (int j=destRt.top; j <= destRt.bottom - im.rcImage.bottom; j += im.rcImage.bottom)
            {
                drawTransparent(pDC, pImageList, nIndex, CPoint(i, j), FALSE, CPoint(0, 0), transparentClr);
            }
        }
    }    
    return;
}

/************************************************************************
getBitmapFromImagelist �̹��� ����Ʈ�� ���� ���� ������Ʈ�� �����Ѵ�.
@param  : 
@return : 
@remark : 
    http://www.codeproject.com/bitmap/getimagefromlist.asp
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/16:CREATED
************************************************************************/
void CFishBMPManager::GetBMPfromList(CDC* pDC, CImageList *lstImages, int nImage, CBitmap* destBitmap)
{    
    destBitmap->DeleteObject();

    CImageList tmpList;
    tmpList.Create(lstImages);
    tmpList.Copy( 0, nImage, ILCF_SWAP );

    IMAGEINFO lastImage;
    tmpList.GetImageInfo(0,&lastImage);

    CDC dcMem; 
    dcMem.CreateCompatibleDC (pDC); 

    CRect rect (lastImage.rcImage);
    destBitmap->CreateCompatibleBitmap (pDC, rect.Width (), rect.Height ());
    CBitmap* pBmpOld = dcMem.SelectObject (destBitmap);
    tmpList.DrawIndirect (&dcMem, 0, CPoint (0, 0), CSize (rect.Width (), rect.Height ()), CPoint (0, 0));
    dcMem.SelectObject (pBmpOld);
}

/************************************************************************
copyBMPtoBMP ��Ʈ���� ������ ��Ʈ������ �����Ѵ�.
@PARAM  : 
    m_bmpOut : ����� ����� �����ͷ� �̹� ������ �־���Ѵ�.
@RETURN : 
@REMARK : 
    �� Ŭ������ ������ ������ ������ ���� 2���� ��Ʈ�� ��ü�� �����ϴµ� �ִ�.
    ���� ��Ŭ������ �Ķ���ʹ� �����ͷ� �Ѱ������Ѵ�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishBMPManager::copyBMPtoBMP(CBitmap* m_bmpData, CBitmap* m_bmpOut)
{
    assert ( m_bmpData != NULL && m_bmpOut != NULL);
    m_bmpOut->DeleteObject();
    BITMAP bm;
    m_bmpData->GetBitmap(&bm);

    BYTE* bd = new BYTE[bm.bmWidthBytes * bm.bmHeight];
    m_bmpData->GetBitmapBits(bm.bmWidthBytes*bm.bmHeight, bd);
    bm.bmBits = bd;
    m_bmpOut->CreateBitmapIndirect(&bm);
    delete bd;
}

CImageList*   CFishBMPManager::getListCtrlProgressBar()
{
    return &m_bmplistListCtrlProgressBar;
}

/************************************************************************
drawListCtrlProgressBar ����Ʈ ��Ʈ�ѿ� ���Ǵ� ����ٸ� �׸���.
@PARAM  : 
@RETURN : 
@REMARK : 
    �׸��� �׸��� ������ ���� �ʺ� 4�ȼ� ������ ��쿡�� �׸��� �ʴ´�.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/03:CREATED
************************************************************************/
void CFishBMPManager::drawListCtrlProgressBar(CDC* pDC, CRect rect, int percentage)
{
    assert(percentage>=0 && percentage <= 100);
    enum { PADDING = 1, };
    CBitmap leftProgress, middleProgress, rightElapse, middleElapse;
    GetBMPfromList(pDC, getListCtrlProgressBar(), 0, &leftProgress);
    GetBMPfromList(pDC, getListCtrlProgressBar(), 1, &middleProgress);
    GetBMPfromList(pDC, getListCtrlProgressBar(), 2, &middleElapse);
    GetBMPfromList(pDC, getListCtrlProgressBar(), 3, &rightElapse);

    BITMAP bm;
    leftProgress.GetBitmap(&bm);

    if (rect.Height() < bm.bmHeight && rect.Width() < 4 ) return;

    // �޾ƿ� ������ ������ �׸��� �׷����� �κ�
    CRect drawnRect( 
        rect.left + 1 , 
        rect.CenterPoint().y - (int)(bm.bmHeight / 2),
        rect.right - 1, 
        rect.CenterPoint().y + (bm.bmHeight - (int)(bm.bmHeight / 2))
        );

    CRect outlineRect   =   drawnRect;
    outlineRect.InflateRect(1, 1, 1, 1);

    // DRAWING ACTION
    // �ͱ� �簢�� �׸���
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0x8C, 0X8C, 0X8C));
    CPen* oldPen = pDC->SelectObject(&pen);
    pDC->Rectangle(outlineRect);
    pDC->SelectObject(oldPen);

    int nProgressWidth = (drawnRect.Width() * percentage / 100)<0 ? (-1) : (drawnRect.Width() * percentage / 100);
    for (int it = drawnRect.left; it<=drawnRect.right; it++)
    {
        
        if ( it < drawnRect.left + nProgressWidth || percentage >= 100 ) {
            // ������� ���� ���ʰ� ���� ������ ó��
            if (it == drawnRect.left)   
                m_bmplistListCtrlProgressBar.Draw(pDC, 0, CPoint(it, drawnRect.top), SRCCOPY);
            else if (it == drawnRect.right)
                m_bmplistListCtrlProgressBar.Draw(pDC, 0, CPoint(it-1, drawnRect.top), SRCCOPY);        // 1�� ������ϴ� ���� IMAGELIST ���׷� ����
            else 
                m_bmplistListCtrlProgressBar.Draw(pDC, 1, CPoint(it, drawnRect.top), SRCCOPY);
        } else {
            // ���� ������ ���� ���ʰ� ���� ������ ó��
            if (it == drawnRect.left)   
                m_bmplistListCtrlProgressBar.Draw(pDC, 3, CPoint(it, drawnRect.top), SRCCOPY);
            else if (it == drawnRect.right)
                m_bmplistListCtrlProgressBar.Draw(pDC, 3, CPoint(it-1, drawnRect.top), SRCCOPY);
            else
                m_bmplistListCtrlProgressBar.Draw(pDC, 2, CPoint(it, drawnRect.top), SRCCOPY);
        }
    }
}

/************************************************************************
getTabBkgnd ���� �� �������� ��� �׸��� �ε��Ѵ�.
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
CBitmap* CFishBMPManager::getTabBkgnd(bool selected, int type)
{
    enum {
        LEFT    = 0,
        SPAN    = 1,
        RIGHT   = 2,
    };

    if (selected)
    {
        switch(type) {
        case LEFT:
            return &m_bmpSelectedItem_Left;
        	break;
        case RIGHT:
            return &m_bmpSelectedItem_Right;
        	break;
        case SPAN:
            return &m_bmpSelectedItem_Span;
        	break;
        default:
            return NULL;
        }
    } else {
        switch(type) {
        case LEFT:
            return &m_bmpDeselectedItem_Left;
        	break;
        case RIGHT:
            return &m_bmpDeselectedItem_Right;
        	break;
        case SPAN:
            return &m_bmpDeselectedItem_Span;
        	break;
        default:
            return NULL;
        }
    }
}