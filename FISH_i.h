/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Mar 07 17:39:00 2006
 */
/* Compiler settings for C:\Documents and Settings\sapius\πŸ≈¡ »≠∏È\FISHClient\FISH.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __FISH_i_h__
#define __FISH_i_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IFISHIEControl_FWD_DEFINED__
#define __IFISHIEControl_FWD_DEFINED__
typedef interface IFISHIEControl IFISHIEControl;
#endif 	/* __IFISHIEControl_FWD_DEFINED__ */


#ifndef __FISHIEControl_FWD_DEFINED__
#define __FISHIEControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class FISHIEControl FISHIEControl;
#else
typedef struct FISHIEControl FISHIEControl;
#endif /* __cplusplus */

#endif 	/* __FISHIEControl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IFISHIEControl_INTERFACE_DEFINED__
#define __IFISHIEControl_INTERFACE_DEFINED__

/* interface IFISHIEControl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IFISHIEControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7DA16FF0-17A4-4F53-B646-8485AAA0A9D8")
    IFISHIEControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SayHello( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHwnd( 
            long __RPC_FAR *phwnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIEParent( 
            long h) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Scrap( 
            long postid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Subscribe( 
            VARIANT __RPC_FAR *link) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadPost( 
            long postid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWBIDispatch( 
            LPDISPATCH __RPC_FAR *lpid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadOn( 
            long postid) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Busy( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Document( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLink2NewWindow( 
            BOOL bnew) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTempBypass( 
            BOOL bpass) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMainWnd( 
            long h) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFISHIEControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IFISHIEControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IFISHIEControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IFISHIEControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IFISHIEControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IFISHIEControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IFISHIEControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IFISHIEControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SayHello )( 
            IFISHIEControl __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )( 
            IFISHIEControl __RPC_FAR * This,
            long __RPC_FAR *phwnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIEParent )( 
            IFISHIEControl __RPC_FAR * This,
            long h);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Scrap )( 
            IFISHIEControl __RPC_FAR * This,
            long postid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Subscribe )( 
            IFISHIEControl __RPC_FAR * This,
            VARIANT __RPC_FAR *link);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadPost )( 
            IFISHIEControl __RPC_FAR * This,
            long postid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWBIDispatch )( 
            IFISHIEControl __RPC_FAR * This,
            LPDISPATCH __RPC_FAR *lpid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadOn )( 
            IFISHIEControl __RPC_FAR * This,
            long postid);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Busy )( 
            IFISHIEControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Document )( 
            IFISHIEControl __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLink2NewWindow )( 
            IFISHIEControl __RPC_FAR * This,
            BOOL bnew);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTempBypass )( 
            IFISHIEControl __RPC_FAR * This,
            BOOL bpass);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMainWnd )( 
            IFISHIEControl __RPC_FAR * This,
            long h);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IFISHIEControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IFISHIEControl __RPC_FAR * This);
        
        END_INTERFACE
    } IFISHIEControlVtbl;

    interface IFISHIEControl
    {
        CONST_VTBL struct IFISHIEControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFISHIEControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFISHIEControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFISHIEControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFISHIEControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFISHIEControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFISHIEControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFISHIEControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFISHIEControl_SayHello(This)	\
    (This)->lpVtbl -> SayHello(This)

#define IFISHIEControl_GetHwnd(This,phwnd)	\
    (This)->lpVtbl -> GetHwnd(This,phwnd)

#define IFISHIEControl_SetIEParent(This,h)	\
    (This)->lpVtbl -> SetIEParent(This,h)

#define IFISHIEControl_Scrap(This,postid)	\
    (This)->lpVtbl -> Scrap(This,postid)

#define IFISHIEControl_Subscribe(This,link)	\
    (This)->lpVtbl -> Subscribe(This,link)

#define IFISHIEControl_ReadPost(This,postid)	\
    (This)->lpVtbl -> ReadPost(This,postid)

#define IFISHIEControl_GetWBIDispatch(This,lpid)	\
    (This)->lpVtbl -> GetWBIDispatch(This,lpid)

#define IFISHIEControl_ReadOn(This,postid)	\
    (This)->lpVtbl -> ReadOn(This,postid)

#define IFISHIEControl_get_Busy(This,pVal)	\
    (This)->lpVtbl -> get_Busy(This,pVal)

#define IFISHIEControl_get_Document(This,pVal)	\
    (This)->lpVtbl -> get_Document(This,pVal)

#define IFISHIEControl_SetLink2NewWindow(This,bnew)	\
    (This)->lpVtbl -> SetLink2NewWindow(This,bnew)

#define IFISHIEControl_SetTempBypass(This,bpass)	\
    (This)->lpVtbl -> SetTempBypass(This,bpass)

#define IFISHIEControl_SetMainWnd(This,h)	\
    (This)->lpVtbl -> SetMainWnd(This,h)

#define IFISHIEControl_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IFISHIEControl_Run(This)	\
    (This)->lpVtbl -> Run(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_SayHello_Proxy( 
    IFISHIEControl __RPC_FAR * This);


void __RPC_STUB IFISHIEControl_SayHello_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_GetHwnd_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long __RPC_FAR *phwnd);


void __RPC_STUB IFISHIEControl_GetHwnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_SetIEParent_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long h);


void __RPC_STUB IFISHIEControl_SetIEParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_Scrap_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long postid);


void __RPC_STUB IFISHIEControl_Scrap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_Subscribe_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    VARIANT __RPC_FAR *link);


void __RPC_STUB IFISHIEControl_Subscribe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_ReadPost_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long postid);


void __RPC_STUB IFISHIEControl_ReadPost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_GetWBIDispatch_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    LPDISPATCH __RPC_FAR *lpid);


void __RPC_STUB IFISHIEControl_GetWBIDispatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_ReadOn_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long postid);


void __RPC_STUB IFISHIEControl_ReadOn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_get_Busy_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IFISHIEControl_get_Busy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_get_Document_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal);


void __RPC_STUB IFISHIEControl_get_Document_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_SetLink2NewWindow_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    BOOL bnew);


void __RPC_STUB IFISHIEControl_SetLink2NewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_SetTempBypass_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    BOOL bpass);


void __RPC_STUB IFISHIEControl_SetTempBypass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFISHIEControl_SetMainWnd_Proxy( 
    IFISHIEControl __RPC_FAR * This,
    long h);


void __RPC_STUB IFISHIEControl_SetMainWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFISHIEControl_Stop_Proxy( 
    IFISHIEControl __RPC_FAR * This);


void __RPC_STUB IFISHIEControl_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFISHIEControl_Run_Proxy( 
    IFISHIEControl __RPC_FAR * This);


void __RPC_STUB IFISHIEControl_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFISHIEControl_INTERFACE_DEFINED__ */



#ifndef __FISHLib_LIBRARY_DEFINED__
#define __FISHLib_LIBRARY_DEFINED__

/* library FISHLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_FISHLib;

EXTERN_C const CLSID CLSID_FISHIEControl;

#ifdef __cplusplus

class DECLSPEC_UUID("45464D6E-37CE-4604-BF8D-B5F68564E62A")
FISHIEControl;
#endif
#endif /* __FISHLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
