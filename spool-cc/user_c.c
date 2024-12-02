

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 05:14:07 2038
 */
/* Compiler settings for .\user.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/

#include <string.h>

#include "user_h.h"

#define TYPE_FORMAT_STRING_SIZE   53                                
#define PROC_FORMAT_STRING_SIZE   487                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _user_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } user_MIDL_TYPE_FORMAT_STRING;

typedef struct _user_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } user_MIDL_PROC_FORMAT_STRING;

typedef struct _user_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } user_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax_2_0 = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

#if defined(_CONTROL_FLOW_GUARD_XFG)
#define XFG_TRAMPOLINES(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree(pFlags, (ObjectType *)pObject);\
}
#define XFG_TRAMPOLINES64(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize64_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize64(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree64_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree64(pFlags, (ObjectType *)pObject);\
}
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)\
static void* ObjectType ## _bind_XFG(HandleType pObject)\
{\
return ObjectType ## _bind((ObjectType) pObject);\
}\
static void ObjectType ## _unbind_XFG(HandleType pObject, handle_t ServerHandle)\
{\
ObjectType ## _unbind((ObjectType) pObject, ServerHandle);\
}
#define XFG_TRAMPOLINE_FPTR(Function) Function ## _XFG
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol ## _XFG
#else
#define XFG_TRAMPOLINES(ObjectType)
#define XFG_TRAMPOLINES64(ObjectType)
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)
#define XFG_TRAMPOLINE_FPTR(Function) Function
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol
#endif


extern const user_MIDL_TYPE_FORMAT_STRING user__MIDL_TypeFormatString;
extern const user_MIDL_PROC_FORMAT_STRING user__MIDL_ProcFormatString;
extern const user_MIDL_EXPR_FORMAT_STRING user__MIDL_ExprFormatString;

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: remote_file, ver. 1.0,
   GUID={0x8791C379,0xD199,0x4f91,{0x86,0x2E,0x83,0x79,0xEE,0xC2,0x1E,0x47}} */



static const RPC_CLIENT_INTERFACE remote_file___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x8791C379,0xD199,0x4f91,{0x86,0x2E,0x83,0x79,0xEE,0xC2,0x1E,0x47}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE remote_file_v1_0_c_ifspec = (RPC_IF_HANDLE)& remote_file___RpcClientInterface;
#ifdef __cplusplus
namespace {
#endif

extern const MIDL_STUB_DESC remote_file_StubDesc;
#ifdef __cplusplus
}
#endif

static RPC_BINDING_HANDLE remote_file__MIDL_AutoBindHandle;


HRESULT RemoteOpenFile( 
    /* [in] */ handle_t IDL_handle,
    /* [out] */ ULONG64 *phFile,
    /* [string][in] */ const wchar_t *pszName)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[0],
                  IDL_handle,
                  phFile,
                  pszName);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteGetSize( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [out] */ ULONG64 *lpFileSize)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[48],
                  IDL_handle,
                  hFile,
                  lpFileSize);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteRead( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [length_is][size_is][out] */ byte *buf,
    /* [in] */ ULONG cb,
    /* [in] */ ULONG64 ByteOffset,
    /* [out] */ ULONG *pcb)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[96],
                  IDL_handle,
                  hFile,
                  buf,
                  cb,
                  ByteOffset,
                  pcb);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteClose( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[162],
                  IDL_handle,
                  hFile);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteMemoryCreate( 
    /* [in] */ handle_t IDL_handle,
    /* [out] */ ULONG64 *phFile,
    /* [in] */ ULONG size)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[204],
                  IDL_handle,
                  phFile,
                  size);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteMemoryWrite( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [size_is][in] */ byte *buf,
    /* [in] */ ULONG cb)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[252],
                  IDL_handle,
                  hFile,
                  buf,
                  cb);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteMemoryExec( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [string][in] */ const wchar_t *pszCmdLine)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[306],
                  IDL_handle,
                  hFile,
                  pszCmdLine);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteMemoryLoad( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [out] */ ULONG64 *phMod)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[354],
                  IDL_handle,
                  hFile,
                  phMod);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteUnloadLoad( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hMod)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[402],
                  IDL_handle,
                  hMod);
    return ( HRESULT  )_RetVal.Simple;
    
}


HRESULT RemoteMemoryFree( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&remote_file_StubDesc,
                  (PFORMAT_STRING) &user__MIDL_ProcFormatString.Format[444],
                  IDL_handle,
                  hFile);
    return ( HRESULT  )_RetVal.Simple;
    
}


#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const user_MIDL_PROC_FORMAT_STRING user__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RemoteOpenFile */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x2c ),	/* 44 */
/* 18 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 20 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phFile */

/* 30 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 32 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 34 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pszName */

/* 36 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 38 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 40 */	NdrFcShort( 0x8 ),	/* Type Offset=8 */

	/* Return value */

/* 42 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 44 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteGetSize */

/* 48 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x1 ),	/* 1 */
/* 56 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 58 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 62 */	NdrFcShort( 0x10 ),	/* 16 */
/* 64 */	NdrFcShort( 0x2c ),	/* 44 */
/* 66 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 68 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 78 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 80 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 82 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter lpFileSize */

/* 84 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 86 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 88 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 90 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 92 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 94 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteRead */

/* 96 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 98 */	NdrFcLong( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x2 ),	/* 2 */
/* 104 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 106 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 108 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 110 */	NdrFcShort( 0x28 ),	/* 40 */
/* 112 */	NdrFcShort( 0x24 ),	/* 36 */
/* 114 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 116 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 118 */	NdrFcShort( 0x1 ),	/* 1 */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 130 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter buf */

/* 132 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 134 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 136 */	NdrFcShort( 0xe ),	/* Type Offset=14 */

	/* Parameter cb */

/* 138 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 140 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ByteOffset */

/* 144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 146 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 148 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pcb */

/* 150 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 152 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 158 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteClose */

/* 162 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0x3 ),	/* 3 */
/* 170 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 172 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 176 */	NdrFcShort( 0x10 ),	/* 16 */
/* 178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 180 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 182 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 192 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 194 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 196 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 198 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 200 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 202 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteMemoryCreate */

/* 204 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 206 */	NdrFcLong( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x4 ),	/* 4 */
/* 212 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 214 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 220 */	NdrFcShort( 0x2c ),	/* 44 */
/* 222 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 224 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phFile */

/* 234 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 236 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 238 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter size */

/* 240 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 242 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 248 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteMemoryWrite */

/* 252 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x5 ),	/* 5 */
/* 260 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 262 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 264 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 266 */	NdrFcShort( 0x18 ),	/* 24 */
/* 268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 270 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 272 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x1 ),	/* 1 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 282 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 284 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 286 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter buf */

/* 288 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 290 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 292 */	NdrFcShort( 0x28 ),	/* Type Offset=40 */

	/* Parameter cb */

/* 294 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 296 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 298 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 302 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteMemoryExec */

/* 306 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x6 ),	/* 6 */
/* 314 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 316 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 320 */	NdrFcShort( 0x10 ),	/* 16 */
/* 322 */	NdrFcShort( 0x8 ),	/* 8 */
/* 324 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 326 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 336 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 338 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 340 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pszCmdLine */

/* 342 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 344 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 346 */	NdrFcShort( 0x8 ),	/* Type Offset=8 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteMemoryLoad */

/* 354 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x7 ),	/* 7 */
/* 362 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 364 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 366 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 368 */	NdrFcShort( 0x10 ),	/* 16 */
/* 370 */	NdrFcShort( 0x2c ),	/* 44 */
/* 372 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 374 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 382 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 384 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 386 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 388 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter phMod */

/* 390 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 392 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 394 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteUnloadLoad */

/* 402 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x8 ),	/* 8 */
/* 410 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 412 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 414 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 416 */	NdrFcShort( 0x10 ),	/* 16 */
/* 418 */	NdrFcShort( 0x8 ),	/* 8 */
/* 420 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 422 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hMod */

/* 432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 434 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 436 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 438 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 440 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoteMemoryFree */

/* 444 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 446 */	NdrFcLong( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x9 ),	/* 9 */
/* 452 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 454 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 456 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 458 */	NdrFcShort( 0x10 ),	/* 16 */
/* 460 */	NdrFcShort( 0x8 ),	/* 8 */
/* 462 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 464 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hFile */

/* 474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 476 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 478 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const user_MIDL_TYPE_FORMAT_STRING user__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  8 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x11, 0x0,	/* FC_RP */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 16 */	NdrFcShort( 0x1 ),	/* 1 */
/* 18 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 20 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 22 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 24 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 26 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 28 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 30 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 32 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 34 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 36 */	
			0x11, 0x0,	/* FC_RP */
/* 38 */	NdrFcShort( 0x2 ),	/* Offset= 2 (40) */
/* 40 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 42 */	NdrFcShort( 0x1 ),	/* 1 */
/* 44 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 46 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 48 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 50 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const unsigned short remote_file_FormatStringOffsetTable[] =
    {
    0,
    48,
    96,
    162,
    204,
    252,
    306,
    354,
    402,
    444
    };


#ifdef __cplusplus
namespace {
#endif
static const MIDL_STUB_DESC remote_file_StubDesc = 
    {
    (void *)& remote_file___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &remote_file__MIDL_AutoBindHandle,
    0,
    0,
    0,
    0,
    user__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x8010274, /* MIDL Version 8.1.628 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };
#ifdef __cplusplus
}
#endif
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

