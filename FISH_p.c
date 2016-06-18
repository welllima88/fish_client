/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Mar 07 17:39:00 2006
 */
/* Compiler settings for C:\Documents and Settings\sapius\πŸ≈¡ »≠∏È\FISHClient\FISH.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "FISH_i.h"

#define TYPE_FORMAT_STRING_SIZE   991                               
#define PROC_FORMAT_STRING_SIZE   403                               

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IFISHIEControl, ver. 0.0,
   GUID={0x7DA16FF0,0x17A4,0x4F53,{0xB6,0x46,0x84,0x85,0xAA,0xA0,0xA9,0xD8}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IFISHIEControl_ServerInfo;

#pragma code_seg(".orpc")
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[1];

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static const unsigned short IFISHIEControl_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    22,
    50,
    78,
    106,
    134,
    162,
    190,
    218,
    246,
    274,
    302,
    330,
    358,
    380
    };

static const MIDL_SERVER_INFO IFISHIEControl_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IFISHIEControl_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IFISHIEControl_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IFISHIEControl_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(22) _IFISHIEControlProxyVtbl = 
{
    &IFISHIEControl_ProxyInfo,
    &IID_IFISHIEControl,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* IFISHIEControl::SayHello */ ,
    (void *)-1 /* IFISHIEControl::GetHwnd */ ,
    (void *)-1 /* IFISHIEControl::SetIEParent */ ,
    (void *)-1 /* IFISHIEControl::Scrap */ ,
    (void *)-1 /* IFISHIEControl::Subscribe */ ,
    (void *)-1 /* IFISHIEControl::ReadPost */ ,
    (void *)-1 /* IFISHIEControl::GetWBIDispatch */ ,
    (void *)-1 /* IFISHIEControl::ReadOn */ ,
    (void *)-1 /* IFISHIEControl::get_Busy */ ,
    (void *)-1 /* IFISHIEControl::get_Document */ ,
    (void *)-1 /* IFISHIEControl::SetLink2NewWindow */ ,
    (void *)-1 /* IFISHIEControl::SetTempBypass */ ,
    (void *)-1 /* IFISHIEControl::SetMainWnd */ ,
    (void *)-1 /* IFISHIEControl::Stop */ ,
    (void *)-1 /* IFISHIEControl::Run */
};


static const PRPC_STUB_FUNCTION IFISHIEControl_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IFISHIEControlStubVtbl =
{
    &IID_IFISHIEControl,
    &IFISHIEControl_ServerInfo,
    22,
    &IFISHIEControl_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

#pragma data_seg(".rdata")

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[1] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure SayHello */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/*  8 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 16 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 18 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHwnd */

/* 22 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 24 */	NdrFcLong( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 30 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 32 */	NdrFcShort( 0x8 ),	/* 8 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter phwnd */

/* 38 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
#ifndef _ALPHA_
/* 40 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 46 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetIEParent */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 58 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter h */

/* 66 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 68 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 74 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Scrap */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0xa ),	/* 10 */
#ifndef _ALPHA_
/* 86 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter postid */

/* 94 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 96 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 102 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Subscribe */

/* 106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0xb ),	/* 11 */
#ifndef _ALPHA_
/* 114 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter link */

/* 122 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 124 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 126 */	NdrFcShort( 0x3c8 ),	/* Type Offset=968 */

	/* Return value */

/* 128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 130 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadPost */

/* 134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0xc ),	/* 12 */
#ifndef _ALPHA_
/* 142 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 148 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter postid */

/* 150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 152 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 158 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetWBIDispatch */

/* 162 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0xd ),	/* 13 */
#ifndef _ALPHA_
/* 170 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 176 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter lpid */

/* 178 */	NdrFcShort( 0x200b ),	/* Flags:  must size, must free, in, srv alloc size=8 */
#ifndef _ALPHA_
/* 180 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 182 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Return value */

/* 184 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 186 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadOn */

/* 190 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0xe ),	/* 14 */
#ifndef _ALPHA_
/* 198 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter postid */

/* 206 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 208 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 212 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 214 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Busy */

/* 218 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 220 */	NdrFcLong( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0xf ),	/* 15 */
#ifndef _ALPHA_
/* 226 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x10 ),	/* 16 */
/* 232 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 234 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
#ifndef _ALPHA_
/* 236 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 240 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 242 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Document */

/* 246 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 248 */	NdrFcLong( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x10 ),	/* 16 */
#ifndef _ALPHA_
/* 254 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pVal */

/* 262 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 264 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 266 */	NdrFcShort( 0x3da ),	/* Type Offset=986 */

	/* Return value */

/* 268 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 270 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 272 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetLink2NewWindow */

/* 274 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 276 */	NdrFcLong( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x11 ),	/* 17 */
#ifndef _ALPHA_
/* 282 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 288 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter bnew */

/* 290 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 292 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 296 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 298 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetTempBypass */

/* 302 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 304 */	NdrFcLong( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x12 ),	/* 18 */
#ifndef _ALPHA_
/* 310 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 312 */	NdrFcShort( 0x8 ),	/* 8 */
/* 314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 316 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter bpass */

/* 318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 320 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 324 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 326 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 328 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMainWnd */

/* 330 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x13 ),	/* 19 */
#ifndef _ALPHA_
/* 338 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 344 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter h */

/* 346 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 348 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 350 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 354 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 356 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Stop */

/* 358 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x14 ),	/* 20 */
#ifndef _ALPHA_
/* 366 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 372 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 374 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 376 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Run */

/* 380 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 382 */	NdrFcLong( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x15 ),	/* 21 */
#ifndef _ALPHA_
/* 388 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x8 ),	/* 8 */
/* 394 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 398 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x0,	/* FC_RP */
/*  8 */	NdrFcShort( 0x3c0 ),	/* Offset= 960 (968) */
/* 10 */	
			0x12, 0x0,	/* FC_UP */
/* 12 */	NdrFcShort( 0x3a8 ),	/* Offset= 936 (948) */
/* 14 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 16 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 18 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 20 */	NdrFcShort( 0x2 ),	/* Offset= 2 (22) */
/* 22 */	NdrFcShort( 0x10 ),	/* 16 */
/* 24 */	NdrFcShort( 0x2b ),	/* 43 */
/* 26 */	NdrFcLong( 0x3 ),	/* 3 */
/* 30 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 32 */	NdrFcLong( 0x11 ),	/* 17 */
/* 36 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 38 */	NdrFcLong( 0x2 ),	/* 2 */
/* 42 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 44 */	NdrFcLong( 0x4 ),	/* 4 */
/* 48 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 50 */	NdrFcLong( 0x5 ),	/* 5 */
/* 54 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 56 */	NdrFcLong( 0xb ),	/* 11 */
/* 60 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 62 */	NdrFcLong( 0xa ),	/* 10 */
/* 66 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 68 */	NdrFcLong( 0x6 ),	/* 6 */
/* 72 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (286) */
/* 74 */	NdrFcLong( 0x7 ),	/* 7 */
/* 78 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 80 */	NdrFcLong( 0x8 ),	/* 8 */
/* 84 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (292) */
/* 86 */	NdrFcLong( 0xd ),	/* 13 */
/* 90 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (316) */
/* 92 */	NdrFcLong( 0x9 ),	/* 9 */
/* 96 */	NdrFcShort( 0xee ),	/* Offset= 238 (334) */
/* 98 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 102 */	NdrFcShort( 0xfa ),	/* Offset= 250 (352) */
/* 104 */	NdrFcLong( 0x24 ),	/* 36 */
/* 108 */	NdrFcShort( 0x304 ),	/* Offset= 772 (880) */
/* 110 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 114 */	NdrFcShort( 0x2fe ),	/* Offset= 766 (880) */
/* 116 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 120 */	NdrFcShort( 0x2fc ),	/* Offset= 764 (884) */
/* 122 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 126 */	NdrFcShort( 0x2fa ),	/* Offset= 762 (888) */
/* 128 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 132 */	NdrFcShort( 0x2f8 ),	/* Offset= 760 (892) */
/* 134 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 138 */	NdrFcShort( 0x2f6 ),	/* Offset= 758 (896) */
/* 140 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 144 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (900) */
/* 146 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 150 */	NdrFcShort( 0x2e2 ),	/* Offset= 738 (888) */
/* 152 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 156 */	NdrFcShort( 0x2e0 ),	/* Offset= 736 (892) */
/* 158 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 162 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (904) */
/* 164 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 168 */	NdrFcShort( 0x2dc ),	/* Offset= 732 (900) */
/* 170 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 174 */	NdrFcShort( 0x2de ),	/* Offset= 734 (908) */
/* 176 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 180 */	NdrFcShort( 0x2dc ),	/* Offset= 732 (912) */
/* 182 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 186 */	NdrFcShort( 0x2da ),	/* Offset= 730 (916) */
/* 188 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 192 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (920) */
/* 194 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 198 */	NdrFcShort( 0x2d6 ),	/* Offset= 726 (924) */
/* 200 */	NdrFcLong( 0x10 ),	/* 16 */
/* 204 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 206 */	NdrFcLong( 0x12 ),	/* 18 */
/* 210 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 212 */	NdrFcLong( 0x13 ),	/* 19 */
/* 216 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 218 */	NdrFcLong( 0x16 ),	/* 22 */
/* 222 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 224 */	NdrFcLong( 0x17 ),	/* 23 */
/* 228 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 230 */	NdrFcLong( 0xe ),	/* 14 */
/* 234 */	NdrFcShort( 0x2ba ),	/* Offset= 698 (932) */
/* 236 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 240 */	NdrFcShort( 0x2c0 ),	/* Offset= 704 (944) */
/* 242 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 246 */	NdrFcShort( 0x27e ),	/* Offset= 638 (884) */
/* 248 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 252 */	NdrFcShort( 0x27c ),	/* Offset= 636 (888) */
/* 254 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 258 */	NdrFcShort( 0x27a ),	/* Offset= 634 (892) */
/* 260 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 264 */	NdrFcShort( 0x274 ),	/* Offset= 628 (892) */
/* 266 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 270 */	NdrFcShort( 0x26e ),	/* Offset= 622 (892) */
/* 272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x0 ),	/* Offset= 0 (276) */
/* 278 */	NdrFcLong( 0x1 ),	/* 1 */
/* 282 */	NdrFcShort( 0x0 ),	/* Offset= 0 (282) */
/* 284 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (283) */
/* 286 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 288 */	NdrFcShort( 0x8 ),	/* 8 */
/* 290 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 292 */	
			0x12, 0x0,	/* FC_UP */
/* 294 */	NdrFcShort( 0xc ),	/* Offset= 12 (306) */
/* 296 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 298 */	NdrFcShort( 0x2 ),	/* 2 */
/* 300 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 302 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 304 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 306 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 310 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (296) */
/* 312 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 314 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 316 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 328 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 330 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 332 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 334 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 336 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 346 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 348 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 350 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 352 */	
			0x12, 0x0,	/* FC_UP */
/* 354 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (862) */
/* 356 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 358 */	NdrFcShort( 0x18 ),	/* 24 */
/* 360 */	NdrFcShort( 0xa ),	/* 10 */
/* 362 */	NdrFcLong( 0x8 ),	/* 8 */
/* 366 */	NdrFcShort( 0x58 ),	/* Offset= 88 (454) */
/* 368 */	NdrFcLong( 0xd ),	/* 13 */
/* 372 */	NdrFcShort( 0x78 ),	/* Offset= 120 (492) */
/* 374 */	NdrFcLong( 0x9 ),	/* 9 */
/* 378 */	NdrFcShort( 0x94 ),	/* Offset= 148 (526) */
/* 380 */	NdrFcLong( 0xc ),	/* 12 */
/* 384 */	NdrFcShort( 0xbc ),	/* Offset= 188 (572) */
/* 386 */	NdrFcLong( 0x24 ),	/* 36 */
/* 390 */	NdrFcShort( 0x114 ),	/* Offset= 276 (666) */
/* 392 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 396 */	NdrFcShort( 0x130 ),	/* Offset= 304 (700) */
/* 398 */	NdrFcLong( 0x10 ),	/* 16 */
/* 402 */	NdrFcShort( 0x148 ),	/* Offset= 328 (730) */
/* 404 */	NdrFcLong( 0x2 ),	/* 2 */
/* 408 */	NdrFcShort( 0x160 ),	/* Offset= 352 (760) */
/* 410 */	NdrFcLong( 0x3 ),	/* 3 */
/* 414 */	NdrFcShort( 0x178 ),	/* Offset= 376 (790) */
/* 416 */	NdrFcLong( 0x14 ),	/* 20 */
/* 420 */	NdrFcShort( 0x190 ),	/* Offset= 400 (820) */
/* 422 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (421) */
/* 424 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 426 */	NdrFcShort( 0x4 ),	/* 4 */
/* 428 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 432 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 434 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 436 */	NdrFcShort( 0x4 ),	/* 4 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x1 ),	/* 1 */
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	0x12, 0x0,	/* FC_UP */
/* 448 */	NdrFcShort( 0xffffff72 ),	/* Offset= -142 (306) */
/* 450 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 452 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 454 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 456 */	NdrFcShort( 0x8 ),	/* 8 */
/* 458 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 460 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 462 */	NdrFcShort( 0x4 ),	/* 4 */
/* 464 */	NdrFcShort( 0x4 ),	/* 4 */
/* 466 */	0x11, 0x0,	/* FC_RP */
/* 468 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (424) */
/* 470 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 472 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 474 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 482 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 486 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 488 */	NdrFcShort( 0xffffff54 ),	/* Offset= -172 (316) */
/* 490 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 492 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 494 */	NdrFcShort( 0x8 ),	/* 8 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x6 ),	/* Offset= 6 (504) */
/* 500 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 502 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 504 */	
			0x11, 0x0,	/* FC_RP */
/* 506 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (474) */
/* 508 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 520 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 522 */	NdrFcShort( 0xffffff44 ),	/* Offset= -188 (334) */
/* 524 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 526 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x6 ),	/* Offset= 6 (538) */
/* 534 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 536 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 538 */	
			0x11, 0x0,	/* FC_RP */
/* 540 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (508) */
/* 542 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 544 */	NdrFcShort( 0x4 ),	/* 4 */
/* 546 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 552 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 554 */	NdrFcShort( 0x4 ),	/* 4 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 558 */	NdrFcShort( 0x1 ),	/* 1 */
/* 560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	0x12, 0x0,	/* FC_UP */
/* 566 */	NdrFcShort( 0x17e ),	/* Offset= 382 (948) */
/* 568 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 570 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 572 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 578 */	NdrFcShort( 0x6 ),	/* Offset= 6 (584) */
/* 580 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 582 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 584 */	
			0x11, 0x0,	/* FC_RP */
/* 586 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (542) */
/* 588 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 590 */	NdrFcLong( 0x2f ),	/* 47 */
/* 594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 598 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 600 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 602 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 604 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 606 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 608 */	NdrFcShort( 0x1 ),	/* 1 */
/* 610 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 612 */	NdrFcShort( 0x4 ),	/* 4 */
/* 614 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 616 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 618 */	NdrFcShort( 0x10 ),	/* 16 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0xa ),	/* Offset= 10 (632) */
/* 624 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 626 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 628 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (588) */
/* 630 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 632 */	
			0x12, 0x0,	/* FC_UP */
/* 634 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (606) */
/* 636 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 638 */	NdrFcShort( 0x4 ),	/* 4 */
/* 640 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 646 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 648 */	NdrFcShort( 0x4 ),	/* 4 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x1 ),	/* 1 */
/* 654 */	NdrFcShort( 0x0 ),	/* 0 */
/* 656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 658 */	0x12, 0x0,	/* FC_UP */
/* 660 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (616) */
/* 662 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 664 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 666 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 668 */	NdrFcShort( 0x8 ),	/* 8 */
/* 670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x6 ),	/* Offset= 6 (678) */
/* 674 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 676 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 678 */	
			0x11, 0x0,	/* FC_RP */
/* 680 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (636) */
/* 682 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 684 */	NdrFcShort( 0x8 ),	/* 8 */
/* 686 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 688 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 690 */	NdrFcShort( 0x10 ),	/* 16 */
/* 692 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 694 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 696 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (682) */
			0x5b,		/* FC_END */
/* 700 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x18 ),	/* 24 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0xa ),	/* Offset= 10 (716) */
/* 708 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 710 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 712 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (688) */
/* 714 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 716 */	
			0x11, 0x0,	/* FC_RP */
/* 718 */	NdrFcShort( 0xffffff0c ),	/* Offset= -244 (474) */
/* 720 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 722 */	NdrFcShort( 0x1 ),	/* 1 */
/* 724 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 730 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 734 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 736 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 738 */	NdrFcShort( 0x4 ),	/* 4 */
/* 740 */	NdrFcShort( 0x4 ),	/* 4 */
/* 742 */	0x12, 0x0,	/* FC_UP */
/* 744 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (720) */
/* 746 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 748 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 750 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 752 */	NdrFcShort( 0x2 ),	/* 2 */
/* 754 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 758 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 760 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 762 */	NdrFcShort( 0x8 ),	/* 8 */
/* 764 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 766 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 768 */	NdrFcShort( 0x4 ),	/* 4 */
/* 770 */	NdrFcShort( 0x4 ),	/* 4 */
/* 772 */	0x12, 0x0,	/* FC_UP */
/* 774 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (750) */
/* 776 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 778 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 780 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 782 */	NdrFcShort( 0x4 ),	/* 4 */
/* 784 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 790 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 792 */	NdrFcShort( 0x8 ),	/* 8 */
/* 794 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 796 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 798 */	NdrFcShort( 0x4 ),	/* 4 */
/* 800 */	NdrFcShort( 0x4 ),	/* 4 */
/* 802 */	0x12, 0x0,	/* FC_UP */
/* 804 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (780) */
/* 806 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 808 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 810 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 814 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 816 */	NdrFcShort( 0x0 ),	/* 0 */
/* 818 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 820 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 822 */	NdrFcShort( 0x8 ),	/* 8 */
/* 824 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 826 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 828 */	NdrFcShort( 0x4 ),	/* 4 */
/* 830 */	NdrFcShort( 0x4 ),	/* 4 */
/* 832 */	0x12, 0x0,	/* FC_UP */
/* 834 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (810) */
/* 836 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 838 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 840 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 846 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 848 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 850 */	NdrFcShort( 0x8 ),	/* 8 */
/* 852 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 854 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 856 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 858 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (840) */
/* 860 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 862 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 864 */	NdrFcShort( 0x28 ),	/* 40 */
/* 866 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (848) */
/* 868 */	NdrFcShort( 0x0 ),	/* Offset= 0 (868) */
/* 870 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 872 */	0x38,		/* FC_ALIGNM4 */
			0x8,		/* FC_LONG */
/* 874 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 876 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffdf7 ),	/* Offset= -521 (356) */
			0x5b,		/* FC_END */
/* 880 */	
			0x12, 0x0,	/* FC_UP */
/* 882 */	NdrFcShort( 0xfffffef6 ),	/* Offset= -266 (616) */
/* 884 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 886 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 888 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 890 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 892 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 894 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 896 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 898 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 900 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 902 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 904 */	
			0x12, 0x0,	/* FC_UP */
/* 906 */	NdrFcShort( 0xfffffd94 ),	/* Offset= -620 (286) */
/* 908 */	
			0x12, 0x10,	/* FC_UP */
/* 910 */	NdrFcShort( 0xfffffd96 ),	/* Offset= -618 (292) */
/* 912 */	
			0x12, 0x10,	/* FC_UP */
/* 914 */	NdrFcShort( 0xfffffdaa ),	/* Offset= -598 (316) */
/* 916 */	
			0x12, 0x10,	/* FC_UP */
/* 918 */	NdrFcShort( 0xfffffdb8 ),	/* Offset= -584 (334) */
/* 920 */	
			0x12, 0x10,	/* FC_UP */
/* 922 */	NdrFcShort( 0xfffffdc6 ),	/* Offset= -570 (352) */
/* 924 */	
			0x12, 0x10,	/* FC_UP */
/* 926 */	NdrFcShort( 0x2 ),	/* Offset= 2 (928) */
/* 928 */	
			0x12, 0x0,	/* FC_UP */
/* 930 */	NdrFcShort( 0xfffffc5e ),	/* Offset= -930 (0) */
/* 932 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 934 */	NdrFcShort( 0x10 ),	/* 16 */
/* 936 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 938 */	0x2,		/* FC_CHAR */
			0x38,		/* FC_ALIGNM4 */
/* 940 */	0x8,		/* FC_LONG */
			0x39,		/* FC_ALIGNM8 */
/* 942 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 944 */	
			0x12, 0x0,	/* FC_UP */
/* 946 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (932) */
/* 948 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 950 */	NdrFcShort( 0x20 ),	/* 32 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* Offset= 0 (954) */
/* 956 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 958 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 960 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 962 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 964 */	NdrFcShort( 0xfffffc4a ),	/* Offset= -950 (14) */
/* 966 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 968 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0x10 ),	/* 16 */
/* 974 */	NdrFcShort( 0x0 ),	/* 0 */
/* 976 */	NdrFcShort( 0xfffffc3a ),	/* Offset= -966 (10) */
/* 978 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 980 */	NdrFcShort( 0xfffffd7a ),	/* Offset= -646 (334) */
/* 982 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 984 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 986 */	
			0x11, 0x10,	/* FC_RP */
/* 988 */	NdrFcShort( 0xfffffd72 ),	/* Offset= -654 (334) */

			0x0
        }
    };

const CInterfaceProxyVtbl * _FISH_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IFISHIEControlProxyVtbl,
    0
};

const CInterfaceStubVtbl * _FISH_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IFISHIEControlStubVtbl,
    0
};

PCInterfaceName const _FISH_InterfaceNamesList[] = 
{
    "IFISHIEControl",
    0
};

const IID *  _FISH_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _FISH_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _FISH, pIID, n)

int __stdcall _FISH_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_FISH_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo FISH_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _FISH_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _FISH_StubVtblList,
    (const PCInterfaceName * ) & _FISH_InterfaceNamesList,
    (const IID ** ) & _FISH_BaseIIDList,
    & _FISH_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
