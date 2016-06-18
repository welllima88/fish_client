// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9853F2F5_AC31_401E_9401_1D8F45FFC6CE__INCLUDED_)
#define AFX_STDAFX_H__9853F2F5_AC31_401E_9401_1D8F45FFC6CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#pragma warning(disable:4244)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define USE_HTML_EXTENDED
#ifdef USE_HTML_EXTENDED
#define USE_SENSITIVEWHEEL	// to use sensitive wheel scroll
#endif

// 전체 검색시 local검색을 포함한다면 USE_ALLSEARCHWITHLOCAL을 사용한다.
#define USE_ALLSEARCHWITHLOCAL

//#define USE_NETSEARCHMAXCHANNEL		// limit the count of the search-destinated channel
//#define USE_WBTMPFILE		// to use file of rss view html
							// if not defined, memory load will be used
#define USE_XMLBNS			// boolean of not-search define

#define _DONT_USE_CUSTOMSCROLLBAR_          // 리스트 컨트롤의 커스텀 스크롤바 사용유무

//#define FISH_TEST			// for only test, do not use thread

#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>			// mutex
#include <atlbase.h>

#include "fish_unicode.h"
#include "smartptr.h"

#ifdef USE_HTML_EXTENDED
#pragma warning( push )
#pragma warning( disable : 4192 4278 )
#import <mshtml.tlb> named_guids
#pragma warning( pop )
#endif

#include "Mshtml.h"			//Mshtml.idl

// prepared to use MSXML Parser
#import "msxml4.dll"
using namespace MSXML2;
// --


#define IDC_HAND            MAKEINTRESOURCE(32649)

#define _EXECUTE_UNIT_TEST_MODE_                // 각 모듈의 유닛 루틴 실행

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9853F2F5_AC31_401E_9401_1D8F45FFC6CE__INCLUDED_)
