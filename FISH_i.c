/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Mar 07 17:39:00 2006
 */
/* Compiler settings for C:\Documents and Settings\sapius\πŸ≈¡ »≠∏È\FISHClient\FISH.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IFISHIEControl = {0x7DA16FF0,0x17A4,0x4F53,{0xB6,0x46,0x84,0x85,0xAA,0xA0,0xA9,0xD8}};


const IID LIBID_FISHLib = {0xEC2EB015,0x3D57,0x47A0,{0xB7,0x95,0x10,0xD7,0x64,0x45,0x82,0xD1}};


const CLSID CLSID_FISHIEControl = {0x45464D6E,0x37CE,0x4604,{0xBF,0x8D,0xB5,0xF6,0x85,0x64,0xE6,0x2A}};


#ifdef __cplusplus
}
#endif

