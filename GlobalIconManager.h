/********************************************************************
	created:	2005/11/29
	created:	29:11:2005   9:09
	filename: 	GlobalIconManager.h
	file base:	GlobalIconManager
	file ext:	h
	author:		youngchang (eternalbleu@gmail.com)
	
	purpose:    
        피쉬 전체에서 접근이 가능한 아이콘 매니저를 만든다.
        피쉬에서 다루는 모든 아이콘을 이 클래스를 통해서 그리는 것이 가능하다.

        PATTERN : SINGLETON

    usage:
        CFishBMPManager* iconManager = CFishBMPManager::getInstance();

    history:
        2005/11/29 created by youngchang(eternalbleu)
*********************************************************************/

#if !defined(AFX_GLOBALICONMANAGER_H__160006ED_68F5_49A4_A1BC_23AD11FB905A__INCLUDED_)
#define AFX_GLOBALICONMANAGER_H__160006ED_68F5_49A4_A1BC_23AD11FB905A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <cmath>
#include <vector>
#include <cassert>
using namespace std;


#ifndef SRCMASK
#define SRCMASK						0x00220326 // mask raster op
#endif 

class CFishBMPManager  
{
private:
    //////////////////////////////////////////////////////////////////////////
    // SINGLETON
    static CFishBMPManager* _instance;
	CFishBMPManager();
    void    _initialize();

public:
    static CFishBMPManager* getInstance(){
        if (    _instance   ==  NULL    )   {
            _instance   =   new CFishBMPManager();
        }
        return _instance;
    }
public:
	virtual ~CFishBMPManager();

private:
	CImageList	            m_bmplistChannelIcons;      // 채널 관련 아이콘의 저장
	CImageList	            m_bmplistGroupIcons;        // 그룹 관련 아이콘의 저장
    CImageList              m_bmplistAnimateIcons;      // 애니 관련 아이콘의 저장

    CImageList              m_bmplistTabIcons;          // 탭 관련 아이콘의 저장
    CImageList              m_bmplistSysmenuIcons;      // 시스템 메뉴 아이콘의 저장

    CBitmap                 m_bmplistGroupBk;           // 그룹의 배경 사각형의 저장
    CBitmap                 m_bmplistGroupBkS;           // 그룹의 배경 사각형의 저장

    CImageList              m_bmplistListCtrlProgressBar;

    CBitmap                 m_bmpVertSpliter;           // 스플리터 관련 비트맵 저장
    CBitmap                 m_bmpHoriSpliter;           // 스필리터 관련 비트맵 저장
//////////////////////////////////////////////////////////////////////////
// TREE
    CBitmap                 m_bmpTreeGrpExpandN;
    CBitmap                 m_bmpTreeGrpExpandO;
    CBitmap                 m_bmpTreeGrpExpandP;
    CBitmap                 m_bmpTreeGrpCollapseN;
    CBitmap                 m_bmpTreeGrpCollapseO;
    CBitmap                 m_bmpTreeGrpCollapseP;


//////////////////////////////////////////////////////////////////////////
// MAINFRAME    
    CBitmap                 m_bmpMainframeBk_TopLeft;
    CBitmap                 m_bmpMainframeBk_TopRight;
    CBitmap                 m_bmpMainframeBk_BottomLeft;
    CBitmap                 m_bmpMainframeBk_BottomRight;
    CBitmap                 m_bmpMainframeBk_BottomCenter;
    CBitmap                 m_bmpMainframeBk_Left;
    CBitmap                 m_bmpMainframeBk_Right;
    CBitmap                 m_bmpMainframeBk_Top;

    CBitmap                 m_bmpSubscribeBk_Dlg;
    CBitmap                 m_bmpSubscribeBk_DlgLeft;
    CBitmap                 m_bmpSubscribeBk_DlgTop;
    CBitmap                 m_bmpSubscribeBk_DlgRight;
    CBitmap                 m_bmpSubscribeBk_DlgBottom;

    CBitmap                 m_bmpExplorerDlg_Bkgnd;
    CBitmap                 m_bmpExplorerDlg_Bkgnd_Middle;

//////////////////////////////////////////////////////////////////////////
// TABCTRL
    CBitmap                 m_bmpSelectedItem_Left;
    CBitmap                 m_bmpSelectedItem_Right;
    CBitmap                 m_bmpSelectedItem_Span;
    CBitmap                 m_bmpDeselectedItem_Left;
    CBitmap                 m_bmpDeselectedItem_Right;
    CBitmap                 m_bmpDeselectedItem_Span;
    
    CBitmap                 m_bmpTabPrevBtn;
    CBitmap                 m_bmpTabNextBtn;
    
//////////////////////////////////////////////////////////////////////////
//  NEW POST IN LISTCTRL
    CBitmap                 m_bmpNewPost;

    vector<CBitmap*>        m_bmpvecArray;              // 기타 다양한 크기의 아이콘을 저장하는 변수

public:
    CImageList* getChannelIcon();
    CImageList* getGroupIcon();
    CImageList* getAnimateIcons();
    CImageList* getTabIcons();

    CImageList* getSysmenuBtn();
    CImageList* getListCtrlProgressBar();

    CBitmap*    getGroupBk(int type = 0);
    
    CBitmap*    getSpliterBmp(bool isVert = true);

    CBitmap*    getMainframeBk(int type = 0);

    CBitmap*    getExplorerBkgnd(int type = 0);

    CBitmap*    getSubscribeBkgnd(int type = 0);

    CBitmap*    getTabBkgnd(bool selected = true, int type = 0);

    CBitmap*    getNewItem();

    CBitmap*    getNewPostIcon();

    CBitmap*    GetTabBtnsBmp(int type  =   0);

    CBitmap*    GetTreeGrpExpand(int type =   0, BOOL expand = TRUE);

public:
    static void        drawTransparent(CDC* pDC, CImageList* pImageList, int nIndex, CPoint ptOrigin, BOOL bExtractMaskClr = FALSE, CPoint ptExtractPoint = CPoint(0, 0), COLORREF maskClr = RGB(0xFF, 0x00, 0xFF));
    static void        drawTransparent(CDC* pDC, CBitmap* bmp, CPoint ptOrigin, BOOL bExtractMaskClr = FALSE, CPoint ptExtractPoint = CPoint(0, 0), COLORREF maskClr = RGB(0xFF, 0x00, 0xFF));
    static void        drawTiledBlt(CDC* pDC, CBitmap* bmp, CRect destRt, COLORREF transparentClr = RGB(0xFF, 0x00, 0xFF));
    static void        drawTiledBlt(CDC* pDC, CImageList* pImageList, int nIndex, CRect destRt, COLORREF transparentClr = RGB(0xFF, 0x00, 0xFF));
    static void        GetBMPfromList(CDC* pDC, CImageList *lstImages, int nImage, CBitmap* destBitmap);
    static void        copyBMPtoBMP(CBitmap* srcBMP, CBitmap* destBMP);

    void        drawListCtrlProgressBar(CDC* pDC, CRect rect = CRect(), int percentage = 100);
};

#endif // !defined(AFX_GLOBALICONMANAGER_H__160006ED_68F5_49A4_A1BC_23AD11FB905A__INCLUDED_)
