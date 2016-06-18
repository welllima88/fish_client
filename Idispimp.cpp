/*
 * idispimp.CPP
 * IDispatch for Extending Dynamic HTML Object Model
 *
 * Copyright (c)1995-1999 Microsoft Corporation, All Rights Reserved
 */ 

#include "stdafx.h"
#include "idispimp.h"

#include "fish_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString cszFB_IsFishBrowser					= _T("CB_IsFishBrowser");
const CString cszFB_Scrap							= _T("Scrap");
const CString cszFB_Subscribe						= _T("Subscribe");
const CString cszFB_ReadOn							= _T("ReadOn");
const CString cszFB_ReadPost						= _T("ReadPost");

#define DISPID_FB_IsFishBrowser				1
#define DISPID_FB_Scrap						2
#define DISPID_FB_Subscribe					3
#define DISPID_FB_ReadOn					4
#define DISPID_FB_ReadPost					5

/*
 * CImpIDispatch::CImpIDispatch
 * CImpIDispatch::~CImpIDispatch
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */ 

CImpIDispatch::CImpIDispatch( void )
{
    m_cRef = 0;
}

CImpIDispatch::~CImpIDispatch( void )
{
	ASSERT( m_cRef == 0 );
}


/*
 * CImpIDispatch::QueryInterface
 * CImpIDispatch::AddRef
 * CImpIDispatch::Release
 *
 * Purpose:
 *  IUnknown members for CImpIDispatch object.
 */ 

STDMETHODIMP CImpIDispatch::QueryInterface( REFIID riid, void **ppv )
{
    *ppv = NULL;


    if ( IID_IDispatch == riid )
	{
        *ppv = this;
	}
	
	if ( NULL != *ppv )
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CImpIDispatch::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CImpIDispatch::Release(void)
{
    return --m_cRef;
}


//IDispatch
STDMETHODIMP CImpIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetTypeInfo(
			/* [in] */ UINT /*iTInfo*/,
            /* [in] */ LCID /*lcid*/,
            /* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetIDsOfNames(
			/* [in] */ REFIID riid,
            /* [size_is][in] */ OLECHAR** rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT hr;
	UINT	i;

	// Assume some degree of success
	hr = NOERROR;

		for ( i=0; i < cNames; i++) {
		CString cszName  = rgszNames[i];

		if(cszName == cszFB_IsFishBrowser)
		{
			rgDispId[i] = DISPID_FB_IsFishBrowser;
		}
		else if(cszName == cszFB_Scrap)
		{
			rgDispId[i] = DISPID_FB_Scrap;
		}
		else if(cszName == cszFB_Subscribe)
		{
			rgDispId[i] = DISPID_FB_Subscribe;
		}
		else if(cszName == cszFB_ReadOn)
		{
			rgDispId[i] = DISPID_FB_ReadOn;
		}	
		else if(cszName == cszFB_ReadPost)
		{
			rgDispId[i] = DISPID_FB_ReadPost;
		}
		else {
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return hr;
}

STDMETHODIMP CImpIDispatch::Invoke(
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID /*riid*/,
            /* [in] */ LCID /*lcid*/,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS* pDispParams,
            /* [out] */ VARIANT* pVarResult,
            /* [out] */ EXCEPINFO* /*pExcepInfo*/,
            /* [out] */ UINT* puArgErr)
{
	CWnd *pWnd = AfxGetMainWnd();
	if(!pWnd) return S_OK;

	HWND hWnd = pWnd->GetSafeHwnd();

	switch(dispIdMember)
	{
	case DISPID_FB_IsFishBrowser:
		{
			if(wFlags & DISPATCH_PROPERTYGET)
			{
				if(pVarResult != NULL)
				{
					VariantInit(pVarResult);
					V_VT(pVarResult)=VT_BOOL;
					V_BOOL(pVarResult) = true;
				}
			}
		}
		break;
	case DISPID_FB_Scrap:
		{
			TRACE(_T("DISPID_SCRAP\r\n"));
			if(pDispParams->cArgs < 1) break;

			if(pDispParams->rgvarg[0].vt == VT_I4)
			{
				::SendMessage(hWnd, WM_SCRAP, (WPARAM) pDispParams->rgvarg[0].iVal, 0);
			}
		}
		break;
	case DISPID_FB_Subscribe:
		{
			TRACE(_T("DISPID_SUBSCRIBE\r\n"));
			if(pDispParams->cArgs < 1) break;

			if( pDispParams->rgvarg[0].vt == VT_BSTR )
			{
				_bstr_t tmp( pDispParams->rgvarg[0].bstrVal, FALSE ); //should use FALSE to fix memory problem

				::SendMessage(hWnd, WM_SUBSCRIBE, (WPARAM) (LPCTSTR) (LPTSTR) tmp, 0);
			}
		}
		break;

	case DISPID_FB_ReadOn:
		{
			TRACE(_T("DISPID_READON\r\n"));
			if(pDispParams->cArgs < 1) break;

			if(pDispParams->rgvarg[0].vt == VT_I4)
			{
				::SendMessage(hWnd, WM_READON, (WPARAM) pDispParams->rgvarg[0].iVal, 0);
			}
		}
		break;

	case DISPID_FB_ReadPost:
		{
			TRACE(_T("DISPID_READPOST\r\n"));
			if(pDispParams->cArgs < 1) break;

			if(pDispParams->rgvarg[0].vt == VT_I4)
			{
				::SendMessage(hWnd, WM_VIEW_IDPOST, (WPARAM) pDispParams->rgvarg[0].iVal, 0);
			}
		}
		break;
	}

	return S_OK;
}
