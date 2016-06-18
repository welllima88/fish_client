// fishlistctrl.cpp : implementation file
//

#include "stdafx.h"
#include "fishlistctrl.h"
#include "../MainFrm.h"
#include "../GlobalIconManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFishListCtrl

CFishListCtrl::CFishListCtrl()
{
	g_MyClrBgHi = FISH_VIEWINFO_LISTCTRL_BGCOLOR_S;
	g_MyClrFgHi = FISH_VIEWINFO_LISTCTRL_FGCOLOR_S;
    m_sortCriterionColumn   =   FISH_VIEWINFO_LISTCTRL_DATE_INDEX;
    m_bAscendingSort =   false;
    m_sortBy        =   -1;
    m_nVertPos      =   0;
    m_dwPrevItem    =   -1;
    m_pParent       =   NULL;
}

CFishListCtrl::~CFishListCtrl()
{
    _finalize();
}

BEGIN_MESSAGE_MAP(CFishListCtrl, CSkinListCtrl)
	//{{AFX_MSG_MAP(CFishListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomDraw )
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_RSS, OnHDNItemclickListRss)
	ON_NOTIFY(HDN_BEGINDRAG, IDC_LIST_RSS, OnHDNBegindragListRss)
	ON_NOTIFY(HDN_BEGINTRACK, IDC_LIST_RSS, OnHDNBegintrackListRss)
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFishListCtrl message handlers
void CFishListCtrl::_initialize()
{
    // FONT VALUE
    m_fontHeaderHeightCoordinator.CreateFont(FISH_VIEWINFO_HEADERCTRL_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    m_fontContentsHeightCoordinator.CreateFont(FISH_VIEWINFO_LISTCTRL_FONT_SIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

	m_fontDefaultContents.CreateFont(FISH_VIEWINFO_LISTCTRL_FONT_SIZE - 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    m_fontDefaultContentsBold.CreateFont(FISH_VIEWINFO_LISTCTRL_FONT_SIZE - 5, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );
    m_fontDefaultContentsSelected.CreateFont(FISH_VIEWINFO_LISTCTRL_FONT_SIZE - 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

    m_fontProgressBar.CreateFont(FISH_VIEWINFO_LISTCTRL_FONT_SIZE - 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, DEFAULT_APPLICATION_FONT );

    // FONT SETTING
    SetFont(&m_fontContentsHeightCoordinator, TRUE);
    Init();
    SetRedraw();

    m_ctrlHeader.SetFont(&m_fontHeaderHeightCoordinator);
}

void CFishListCtrl::_finalize()
{
}

/************************************************************************
OnNMCustomDraw
@PARAM  : 
@RETURN : 
@REMARK : 
    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/custdraw/messages/nm_customdraw.asp
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/02:CREATED
************************************************************************/
void CFishListCtrl::OnNMCustomDraw ( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
    static bool bHighlighted = false;
    
    *pResult = CDRF_DODEFAULT;

    //////////////////////////////////////////////////////////////////////////
    // REGION RESIZE
    switch(pLVCD->nmcd.dwDrawStage) {
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM :     //Flag combined with CDDS_ITEMPREPAINT or CDDS_ITEMPOSTPAINT if a subitem is being drawn. This will only be set if CDRF_NOTIFYITEMDRAW is returned from CDDS_PREPAINT.
        {
            CDC* originalDC = CDC::FromHandle(pLVCD->nmcd.hdc);
            if( originalDC == NULL )    return;
            
            CRect rect;			
            GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_LABEL, rect);	
            if (pLVCD->iSubItem ==  FISH_VIEWINFO_LISTCTRL_ICON_INDEX)  {
                CRect tmpRT;
                GetClientRect(&tmpRT);
                rect.left   =   0;
                rect.right  =   1024;
            }

            CMemDC pDC(originalDC, &rect);     // 자동으로 더블 버퍼링을 처리하는 DC
            pDC->SetBkMode(TRANSPARENT);
            
            customdraw_DrawBkgnd(pDC, pLVCD);           // 배경그리기
            customdraw_DrawIcon(pDC, pLVCD);            // 아이콘 그리기
            customdraw_DrawText(pDC, pLVCD);            // 글 그리기
            customdraw_DrawProgressBar(pDC, pLVCD);     //  진행바 그리기
            *pResult = CDRF_SKIPDEFAULT;
            break;
        }
    case CDDS_POSTERASE:        //After the erasing cycle is complete.
        {
            break;
        }
    case CDDS_POSTPAINT:        //After the painting cycle is complete.
        {
            break;
        }
    case CDDS_PREERASE:         //Before the erasing cycle begins.
        {
            break;
        }
 	case CDDS_PREPAINT :        //Before the painting cycle begins.
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}
	case CDDS_ITEMPREPAINT :    //Before an item is drawn.
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
    default:
        *pResult = CDRF_DODEFAULT;
        break;
    }
    return;
}

/************************************************************************
customdraw_DrawBkgnd 그리기 요소의 배경 그리기
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishListCtrl::customdraw_DrawBkgnd(CDC* pDC, NMLVCUSTOMDRAW* pLVCD)
{
    enum {  EVEN = 0,   ODD = 1,    };

    CRect clientRT;
    GetClientRect(&clientRT);

    CRect rect;
    GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_LABEL, rect);
    
    // 배경색 지정하기
    if( GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED) & LVIS_SELECTED )
    {
        if (pLVCD->iSubItem ==  FISH_VIEWINFO_LISTCTRL_ICON_INDEX)  {
            rect.left   =   0;
            rect.right  =   1024;
        }   
        
        if( GetFocus() == (CWnd*)this)
        {
            if (pLVCD->iSubItem  ==  0)
            {
                int nWidth  =   0;
                for (int it = 0; it < GetHeaderCtrl()->GetItemCount(); it++)
                {
                    nWidth  +=  GetColumnWidth(it);
                }
                
                CClientDC dc(this);
                CRect rt;
                GetClientRect(&rt);
                rt.left =   nWidth;
                rt.top  =   rect.top;
                rt.bottom   =   rect.bottom;
                dc.FillRect( &rt, &CBrush(FISH_VIEWINFO_LISTCTRL_BGCOLOR_S) );
            }

            pDC->FillRect( rect, &CBrush(FISH_VIEWINFO_LISTCTRL_BGCOLOR_S) );
            //                      pDC->FillRect( clientRT, &CBrush(FISH_VIEWINFO_LISTCTRL_SELROW_BGCOLOR) );
            pDC->SetTextColor(FISH_VIEWINFO_LISTCTRL_FGCOLOR_S);
        }   else   {
            if (pLVCD->iSubItem  ==  0)
            {
                int nWidth  =   0;
                for (int it = 0; it < GetHeaderCtrl()->GetItemCount(); it++)
                {
                    nWidth  +=  GetColumnWidth(it);
                }
                
                CClientDC dc(this);
                CRect rt;
                GetClientRect(&rt);
                rt.left =   nWidth;
                rt.top  =   rect.top;
                rt.bottom   =   rect.bottom;
                dc.FillRect( &rt, &CBrush(FISH_VIEWINFO_LISTCTRL_BGCOLOR_NF) );
            }
            
            pDC->FillRect( rect, &CBrush(FISH_VIEWINFO_LISTCTRL_BGCOLOR_NF) );
            //                      pDC->FillRect( itemRT, &CBrush(FISH_VIEWINFO_LISTCTRL_SELROW_BGCOLOR) );
            pDC->SetTextColor(FISH_VIEWINFO_LISTCTRL_FGCOLOR_S);
        }		
    }			 
    else
    {				 
        if (pLVCD->iSubItem ==  FISH_VIEWINFO_LISTCTRL_ICON_INDEX)  {
            rect.left   =   clientRT.left;
            rect.right  =   clientRT.right;
        }        
        if( ( pLVCD->nmcd.dwItemSpec % 2 ) == EVEN ) //짝수 
        {
            if (pLVCD->iSubItem  == 0)
            {
                int nWidth  =   0;
                for (int it = 0; it < GetHeaderCtrl()->GetItemCount(); it++)
                {
                    nWidth  +=  GetColumnWidth(it);
                }
                
                CClientDC dc(this);
                CRect rt;
                GetClientRect(&rt);
                rt.left =   nWidth;
                rt.top  =   rect.top;
                rt.bottom   =   rect.bottom;
                dc.FillRect( &rt, &CBrush(FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR) );
            }
            pDC->FillRect(&rect, &CBrush(FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR) );
        }
        else
        {
            if (pLVCD->iSubItem  == 0)
            {
                int nWidth  =   0;
                for (int it = 0; it < GetHeaderCtrl()->GetItemCount(); it++)
                {
                    nWidth  +=  GetColumnWidth(it);
                }

                CClientDC dc(this);
                CRect rt;
                GetClientRect(&rt);
                rt.left =   nWidth;
                rt.top  =   rect.top;
                rt.bottom   =   rect.bottom;
                dc.FillRect( &rt, &CBrush(FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR) );
            }
            
            pDC->FillRect(&rect, &CBrush(FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR) );				
        }			 
    }

    m_dwPrevItem    =   pLVCD->nmcd.dwItemSpec;
}

/************************************************************************
customdraw_DrawText 그리기 요소의 텍스트 드로우
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/03/21:CREATED
************************************************************************/
void CFishListCtrl::customdraw_DrawText(CDC* pDC, NMLVCUSTOMDRAW* pLVCD)
{
    enum { 
        NORMAL_DRAWING_PEN  =   RGB(0x00, 0x00, 0x00),
        PROGRESS_DRAWING_PEN    =   RGB(0x83, 0x83, 0x83),
    };


    if (pLVCD->iSubItem == FISH_VIEWINFO_LISTCTRL_ICON_INDEX || pLVCD->iSubItem == FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX)   return;//uFormat |= DT_CENTER;


    CRect rect;
    pDC->SetTextColor( NORMAL_DRAWING_PEN );

    GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_LABEL, rect);
    if (pLVCD->iSubItem ==  0)  rect.InflateRect(2, 0, 0, 0);

    // 폰트 선택
    CFont* pOldFont = NULL;
    if ( GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED)&LVIS_SELECTED )
    {
        pOldFont    =   (CFont*)pDC->SelectObject(&m_fontDefaultContentsSelected);
        pDC->SetTextColor( FISH_VIEWINFO_LISTCTRL_FGCOLOR_S );
    }    else   {
        if ( GetItemText(pLVCD->nmcd.dwItemSpec, FISH_VIEWINFO_LISTCTRL_ICON_INDEX) == _T("U") )
            pDC->SetTextColor( FISH_VIEWINFO_LISTCTRL_FGCOLOR_UNREAD );
            //pOldFont    =   (CFont*)pDC->SelectObject(&m_fontDefaultContentsBold);
        else
            pDC->SetTextColor( FISH_VIEWINFO_LISTCTRL_FGCOLOR_N );
            //pOldFont    =   (CFont*)pDC->SelectObject(&m_fontDefaultContents);

        pOldFont    =   (CFont*)pDC->SelectObject(&m_fontDefaultContents);
    }


    // 내용 및 출력 형태 결정
    CString strText = GetItemText(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem);
    
    LVCOLUMN lvc;
    lvc.iSubItem = pLVCD->iSubItem;
    lvc.mask = LVCF_FMT;
    GetColumn(pLVCD->iSubItem, &lvc);

    CRect rectOrg   =   rect;

    rectOrg.DeflateRect(5, 2, 2, 2);    // 영역의 크기 지정
    pDC->DrawText(strText, rectOrg, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);

    pDC->SelectObject(pOldFont);
}

/************************************************************************
customdraw_DrawProgressBar
@PARAM  : 
@RETURN : 
@REMARK : 
    관심도 컬럼의 스트링 값을 가지고 진행바형태의 보여주기를 한다.
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/04:CREATED
************************************************************************/
void CFishListCtrl::customdraw_DrawProgressBar(CDC* pDC, NMLVCUSTOMDRAW* pLVCD)
{
    enum { 
        MAX_NUM_DIGIT = 5, 
        PROGRESS_TEXT_REGION_RIGHT_MARGIN   =   39,
        PROGRESS_TEXT_LEFT_PADDING  =   5,
    };
    CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();

    CString strText = GetItemText(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem);

    if (pLVCD->iSubItem != FISH_VIEWINFO_LISTCTRL_EYEBALL_INDEX || strText == "")  return;

    // TODO : CALC 
    int percent = 0;
    TCHAR strPercent[MAX_NUM_DIGIT]; 
    _tcscpy(strPercent, strText.GetBuffer(MAX_NUM_DIGIT));

    percent = _ttoi(strPercent); 

    if (percent ==  0)  return;
    // TODO : 
    {// 진행바 그리기
        CRect rect;
        GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_BOUNDS, rect);
        rect.InflateRect(-PROGRESS_TEXT_LEFT_PADDING, 0, -PROGRESS_TEXT_REGION_RIGHT_MARGIN, 0);
        BMPManager->drawListCtrlProgressBar(pDC, rect, percent);
    }

    {// 진행바 텍스트 그리기
        if ( GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED) & LVIS_SELECTED )
            pDC->SetTextColor(FISH_VIEWINFO_LISTCTRL_PROGRESS_TEXT_S);
        else
            pDC->SetTextColor(FISH_VIEWINFO_LISTCTRL_PROGRESS_TEXT_N);

        CFont* pOldFont  =   pDC->SelectObject(&m_fontProgressBar);
        CRect rect;
        UINT uFormat = DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
        GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_BOUNDS, rect);
        rect.left    =   rect.right   -   PROGRESS_TEXT_REGION_RIGHT_MARGIN   +   PROGRESS_TEXT_LEFT_PADDING; 
        rect.DeflateRect(5, 2, 2, 2);    // 영역의 크기 지정

        CString percentage  =   strText;
        percentage  +=  _T("%");

        pDC->DrawText(percentage, &rect, uFormat);
        pDC->SelectObject(pOldFont);
    }
}

void CFishListCtrl::customdraw_DrawIcon(CDC* pDC, NMLVCUSTOMDRAW* pLVCD)
{
    // TODO : 아이템 그리기
    if (pLVCD->iSubItem == FISH_VIEWINFO_LISTCTRL_ICON_INDEX)   
    {
        CString strText = GetItemText(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem);
        if ( strText == _T("U") )
        {
            CFishBMPManager* BMPManager =   CFishBMPManager::getInstance();
            CRect rect;
            GetSubItemRect(pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem, LVIR_BOUNDS, rect);
            CBitmap* bmp = BMPManager->getNewPostIcon();
            BITMAP bm;
            bmp->GetBitmap(&bm);
            BMPManager->drawTransparent(pDC, bmp, 
                CPoint(rect.CenterPoint().x - bm.bmWidth/2, rect.CenterPoint().y - bm.bmHeight/2), 
                TRUE,
                CPoint(0,0),
                RGB(0xef, 0xef, 0xef));
        }
    }
    return;
}

/************************************************************************
CompareFunc SORT의 비교형 CALLBACK 함수
@param  : 
@return : 
@remark : 
    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcmfc98/html/_mfc_clistctrl.3a3a.sortitems.asp
@author : youngchang (eternalbleu@gmail.com)
@history:
    2005/12/28:CREATED
************************************************************************/
int CALLBACK ListCtrl_SortCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    // lParamSort contains a pointer to the list view control.
    // The lParam of an item is just its index.
    // The lParam1 and lParam2 parameters specify the item data for the two items being compared. The lParamSort parameter is the same as the dwData value.
    int RET =   0;
    {
        CFishListCtrl* pListCtrl = (CFishListCtrl*) lParamSort;
        int nIndex1 = pListCtrl->GetItemIndexFromData(lParam1);
        int nIndex2 = pListCtrl->GetItemIndexFromData(lParam2);
        ASSERT( nIndex1 != -1 && nIndex2 != -1);

        CString    strItem1 = pListCtrl->GetItemText( nIndex1, pListCtrl->m_sortCriterionColumn);
        CString    strItem2 = pListCtrl->GetItemText( nIndex2, pListCtrl->m_sortCriterionColumn);
        TRACE( _T("\"%s\", \"%s\"\n"), strItem1, strItem2);

        if (pListCtrl->m_bAscendingSort) {
            RET  =   _tcscmp(strItem1, strItem2);
        }   else {
            RET  =   _tcscmp(strItem2, strItem1);
        }
    }
    return RET;
}

void CFishListCtrl::ToggleIconSort()
{
    NM_LISTVIEW tmp;
    LRESULT     ret;
    tmp.iSubItem    =   FISH_VIEWINFO_LISTCTRL_ICON_INDEX;
    OnColumnclick((NMHDR*)&tmp, &ret);
}

/************************************************************************
OnColumnclick   헤더 컨트롤에서 칼럼클릭시에 행하는 일 (일반적으로 SORT)
@PARAM  : 
@RETURN : 
@REMARK : 
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2005/12/28:CREATED
************************************************************************/
void CFishListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
    if (!GetItemCount())    return;

    if (m_sortBy == pNMListView->iSubItem)      m_bAscendingSort =   !m_bAscendingSort;       // SORT TOGGLE
    else    m_bAscendingSort =   true;
    m_sortBy        =   pNMListView->iSubItem;
    GetParent()->SendMessage(WM_LISTVIEW_SORT, pNMListView->iSubItem, (LPARAM)this );

	*pResult = 0;
}

int CFishListCtrl::GetItemIndexFromData(DWORD dwData)
{
    for(int nIndex = GetNextItem(-1, LVNI_ALL); nIndex != -1; nIndex = GetNextItem(nIndex, LVNI_ALL) )
    {
        if ( dwData == GetItemData(nIndex) )
            return nIndex;
    }
    return -1;
}

// written by		: moonknit
// name				: CXListCtrl::FindItemByPosition
// parameters		: in, CPoint pt
// returns			: index of item (int)
//						-1 : not fond item
//						0 or over : item index
// created			: 2005-06-10
// last updated		: 2005-06-10
// desc				:
int CFishListCtrl::FindItemByPosition(CPoint pt)
{
	CRect rect;

	int i;
	for (i = 0; i < GetItemCount(); i++)
	{
		if (CListCtrl::GetItemRect(i, &rect, LVIR_BOUNDS))
		{
			if (rect.top > 0 && rect.PtInRect(pt))
			{
				return i;
				break;
			}
		}
	}

	return -1;
}

BOOL CFishListCtrl::OnEraseBkgnd(CDC* originalDC) 
{
	// TODO: Add your message handler code here and/or call default
    //////////////////////////////////////////////////////////////////////////
    // FOR TEST

	return TRUE;

    CRect clientRT, tmpRT, viewRT;
    GetClientRect(&clientRT);
    
//    CMemDC pDC(originalDC, &clientRT);
	CDC* pDC = originalDC;
    
    if(clientRT.bottom < 0 || clientRT.right < 0)
        return CListCtrl::OnEraseBkgnd(pDC);

//    if( GetItemCount() == 0 )            // to draw backgound line, or not to draw?
	{
		int iLine = (m_nVertPos) % 2;
		COLORREF	crLine;
		
		tmpRT = clientRT;
		tmpRT.top = tmpRT.top + FISH_VIEWINFO_LISTCTRL_HEADER_HEIGHT + 2;
		
		tmpRT.bottom = tmpRT.top + FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT ;//+ 1;
		
		// draw upper 1st line
		crLine = ((iLine % 2) == 1) ? FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR ;
		tmpRT.bottom = tmpRT.top;
		tmpRT.top -= FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT + 1;
		
		pDC->FillSolidRect(&tmpRT, crLine);

		BOOL bFind = FALSE;;
		int iItem, nCnt;
		nCnt = GetItemCount();

		int x, y;
		x = m_rectClient.left + 1;
		y = m_rectClient.top + FISH_VIEWINFO_LISTCTRL_HEADER_HEIGHT + 2;

		if(nCnt > 0)
			iItem = m_nVertPos;
		else
			iItem = -1;

		bFind = (iItem != -1);
		
		// draw 1st line
		tmpRT.top = tmpRT.bottom;
		tmpRT.bottom = tmpRT.top + FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT ;//+ 1;
		crLine = ((iLine % 2) == 0) ? FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR;

		if(!bFind)
			pDC->FillSolidRect(&tmpRT, crLine);

		do
		{
			iLine ++;
			
			tmpRT.top = tmpRT.bottom;
			tmpRT.bottom = tmpRT.top + FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT ;//+ 1;
			
			crLine = ((iLine % 2) == 0) ? FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR;
			if(bFind)
			{
				if(iItem > nCnt)
				{
					pDC->FillSolidRect(&tmpRT, crLine);
				}

				++iItem;
			}
			else
				pDC->FillSolidRect(&tmpRT, crLine);
		} while (tmpRT.bottom < clientRT.bottom);
		
		return TRUE;
    }

    return CSkinListCtrl::OnEraseBkgnd(originalDC);
}

void CFishListCtrl::OnPaint() 
{
	m_nVertPos = GetScrollPos(SB_VERT);

    CRect clientRT, tmpRT, viewRT;
    GetClientRect(&clientRT);

	CDC* originalDC = GetDC();
	if(!originalDC) 
	{
		Default();
		return;
	}
	CDC* pDC = originalDC;
    
    if(clientRT.bottom < 0 || clientRT.right < 0)
	{
		ReleaseDC(originalDC);
		Default();
        return;
	}

	{
		int iLine = (m_nVertPos) % 2;
		COLORREF	crLine;
		
        //////////////////////////////////////////////////////////////////////////
        // 헤더 밑 부분을 새로 그린다.
		tmpRT = clientRT;
		tmpRT.top       = FISH_VIEWINFO_LISTCTRL_HEADER_HEIGHT;
		tmpRT.bottom    = tmpRT.top + FISH_VIEWINFO_LISTCTRL_HEADERUN_DERLINE_HEIGHT ;//+ 1;

        pDC->FillSolidRect(&tmpRT, FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR);

		// draw upper 1st line
		crLine = ((iLine % 2) == 1) ? FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR ;

		BOOL bFind = FALSE;;
		int iItem, nCnt;
		nCnt = GetItemCount();

		if(nCnt > 0)
			iItem = m_nVertPos;
		else
			iItem = -1;

		bFind = (iItem != -1);
		
		// draw 1st line
		tmpRT.top = tmpRT.bottom;
		tmpRT.bottom = tmpRT.top + FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT ;//+ 1;
		crLine = ((iLine % 2) == 0) ? FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR;

		if(!bFind)
			pDC->FillSolidRect(&tmpRT, crLine);

		do
		{
			iLine ++;
			
			tmpRT.top = tmpRT.bottom;
			tmpRT.bottom = tmpRT.top + FISH_VIEWINFO_LISTCTRL_LINE_HEIGHT ;//+ 1;
			
			crLine = ((iLine % 2) == 0) ? FISH_VIEWINFO_LISTCTRL_EVENROW_BGCOLOR : FISH_VIEWINFO_LISTCTRL_ODDROW_BGCOLOR;
			if(bFind)
			{
				if(++iItem >= nCnt)
				{
					pDC->FillSolidRect(&tmpRT, crLine);
				}
			}
			else
				pDC->FillSolidRect(&tmpRT, crLine);
		} while (tmpRT.bottom < clientRT.bottom);
	
    }

	ReleaseDC(originalDC);

	Default();
}

void CFishListCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(GetHeaderCtrl())
    {
        m_ctrlHeader.SubclassWindow(GetHeaderCtrl()->m_hWnd);
    }

	CListCtrl::PreSubclassWindow();
}

void CFishListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    SetFocus();
	CSkinListCtrl::OnLButtonDown(nFlags, point);
}

void CFishListCtrl::OnHDNItemclickListRss(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CFishListCtrl::OnHDNBegindragListRss(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY * phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CFishListCtrl::OnHDNBegintrackListRss(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}


void CFishListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CSkinListCtrl::OnLButtonUp(nFlags, point);
    SetFocus();
}

void CFishListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CSkinListCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
    GetClientRect(&m_rectClient);
	GetWindowRect(&m_rectScreenWnd);

	Invalidate();
}

BOOL CFishListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	return CSkinListCtrl::PreTranslateMessage(pMsg);
}

LRESULT CFishListCtrl::OnMouseLeave(WPARAM w, LPARAM l)
{
	SetOverHwnd(NULL);
	return 0;
}

void CFishListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{

	TRACKMOUSEEVENT t_MouseEvent;
	t_MouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
	t_MouseEvent.dwFlags     = TME_LEAVE;
	t_MouseEvent.hwndTrack   = m_hWnd;
	t_MouseEvent.dwHoverTime = 0;

	// We Tell Windows we want to receive WM_MOUSEHOVER and WM_MOUSELEAVE
	::_TrackMouseEvent(&t_MouseEvent);

	SetOverHwnd(GetSafeHwnd());
	CSkinListCtrl::OnMouseMove(nFlags, point);
}

BOOL CFishListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(GetOverHwnd() != GetSafeHwnd()) return TRUE;
    // TODO: Add your message handler code here and/or call defaultthis]
   	return CSkinListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


void CFishListCtrl::SelectByData(DWORD dwData)
{
	// remove previous selection
	int idx = 0;
	POSITION pos = GetFirstSelectedItemPosition();
	while(pos)
	{
		idx = GetNextSelectedItem(pos);
		SetItemState(idx, 0, LVIS_SELECTED);
	}

	// set new selection
	for(idx = 0; idx < GetItemCount(); idx++)
	{
		DWORD p = GetItemData(idx);
		if( p == dwData )
		{
			break;
		}
	}

	if(idx != GetItemCount())
	{
		CRect rect;
		CRect clRect;
		GetClientRect(&clRect);

		SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
//		SetSelectionMark(idx);

		GetItemRect(idx, &rect, LVIR_BOUNDS);
		
		if(rect.top < 0)
		{
			// 위로 스크롤
			while(rect.top < 0)
			{
				SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0), NULL);
				GetItemRect(idx, &rect, LVIR_BOUNDS);
			}
		}
		else if(rect.bottom > clRect.bottom)
		{
			// 아래로 스크롤 
			while(rect.bottom > clRect.bottom)
			{
				SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0), NULL);
				GetItemRect(idx, &rect, LVIR_BOUNDS);
			}

		}
		
	}
}

#ifdef _DONT_USE_CUSTOMSCROLLBAR_
void CFishListCtrl::PositionScrollBars()
{

}

void CFishListCtrl::Init()
{

}
#else
/************************************************************************
PositionScrollBars  ListCtrl 기본 스크롤바를 숨기고 비트맵 스크롤 바로 변경한다.
@PARAM  : 
@RETURN : 
@REMARK : 
    http://www.codeguru.com/forum/showthread.php?s=36a2689f1a0c1395ba30950f993c559b&threadid=178221&highlight=hide+scrollbars
@AUTHOR : youngchang (eternalbleu@gmail.com)
@HISTORY :
    2006/01/02:CREATED
************************************************************************/
void CFishListCtrl::PositionScrollBars()
{
    enum { SCROLLBAR_WIDTH=12, SCROLLBAR_HEIGHT=12};

	CWnd* pParent = GetParent();
	
	CRect windowRT;
	GetWindowRect(&windowRT);

	int nTitleBarHeight = 0;
	
	if(pParent->GetStyle() & WS_CAPTION)
		nTitleBarHeight = GetSystemMetrics(SM_CYSIZE);

    // WINDOW SYSTEM VALUE
	int nDialogFrameHeight = 0;
	int nDialogFrameWidth = 0;
    int cxvs    =   0;
    int cyvs    =   0;
    int cxhs    =   0;
    int cyhs    =   0;

    if (pParent->GetStyle() & WS_VSCROLL)   {
        cxvs = GetSystemMetrics (SM_CXVSCROLL);
        cyvs = GetSystemMetrics (SM_CYVSCROLL);
    }

    if (pParent->GetStyle() & WS_HSCROLL)   {
        cxhs = GetSystemMetrics (SM_CXHSCROLL);
        cyhs = GetSystemMetrics (SM_CYHSCROLL);
    }
    
	if((pParent->GetStyle() & WS_BORDER))	{
		nDialogFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);
		nDialogFrameWidth = GetSystemMetrics(SM_CYDLGFRAME);
	}
	
	if(pParent->GetStyle() & WS_THICKFRAME)    {
		nDialogFrameHeight+=1;
		nDialogFrameWidth+=1;
	}
	
	pParent->ScreenToClient(&windowRT);

    windowRT.InflateRect(nDialogFrameWidth,
        nTitleBarHeight+nDialogFrameHeight,
        nDialogFrameWidth,
        nTitleBarHeight+nDialogFrameHeight);

    // Vertical Scrollbar
    BOOL bVShow, bHShow;
	BOOL bVChanged = FALSE, bHChanged = FALSE;
    CRect regionRT  =   windowRT;
    if (GetScrollLimit(SB_VERT) != 0)
    {
		if(!m_SkinVerticleScrollbar.IsWindowVisible()) bVChanged = TRUE;

        regionRT.left   =   windowRT.left-nDialogFrameWidth;
        regionRT.right  =   windowRT.right-nDialogFrameWidth - SCROLLBAR_WIDTH;
        m_SkinVerticleScrollbar.ShowWindow(SW_NORMAL);
        bVShow  =   TRUE;
    }   else    {
		if(m_SkinVerticleScrollbar.IsWindowVisible()) bVChanged = TRUE;

        m_SkinVerticleScrollbar.ShowWindow(SW_HIDE);
        bVShow  =   FALSE;
    }

    // Horizontal Scrollbar
    if (GetScrollLimit(SB_HORZ) != 0)
    {
		if(!m_SkinHorizontalScrollbar.IsWindowVisible()) bHChanged = TRUE;

        regionRT.top    =   windowRT.top-nTitleBarHeight-nDialogFrameHeight;
        regionRT.bottom =   windowRT.bottom-nTitleBarHeight-nDialogFrameHeight - SCROLLBAR_HEIGHT;
        m_SkinHorizontalScrollbar.ShowWindow(SW_NORMAL);
        bHShow  =   TRUE;
    }   else        {
		if(m_SkinHorizontalScrollbar.IsWindowVisible()) bHChanged = TRUE;

        m_SkinHorizontalScrollbar.ShowWindow(SW_HIDE);
        bHShow  =   FALSE;
    }

    if (bVChanged) m_ctrlHeader.Invalidate(FALSE);

    HRGN iehrgn = CreateRectRgn(regionRT.left, regionRT.top, regionRT.right, regionRT.bottom);
    SetWindowRgn(iehrgn, TRUE);

    // SCROLLBAR SIZE SET
	CRect vBar(windowRT.right-nDialogFrameWidth - SCROLLBAR_WIDTH, 
        windowRT.top-nTitleBarHeight-nDialogFrameHeight- SCROLLBAR_WIDTH + 1, 
        windowRT.right+0-nDialogFrameWidth, 
        windowRT.bottom-nTitleBarHeight-nDialogFrameHeight);
	CRect hBar(windowRT.left-nDialogFrameWidth, 
        windowRT.bottom-nTitleBarHeight-nDialogFrameHeight - SCROLLBAR_HEIGHT, 
        bVShow ? windowRT.right-nDialogFrameWidth - SCROLLBAR_WIDTH:windowRT.right-nDialogFrameWidth, 
        windowRT.bottom+0-nTitleBarHeight-nDialogFrameHeight);

    // SCROLLBAR WINDOWS RESIZING
	m_SkinVerticleScrollbar.SetWindowPos(NULL,vBar.left,vBar.top,vBar.Width(),vBar.Height(),SWP_NOZORDER);
	m_SkinHorizontalScrollbar.SetWindowPos(NULL,hBar.left,hBar.top,hBar.Width(),hBar.Height(),SWP_NOZORDER);
	
	m_SkinHorizontalScrollbar.UpdateThumbPosition();
	m_SkinVerticleScrollbar.UpdateThumbPosition();
}
#endif

