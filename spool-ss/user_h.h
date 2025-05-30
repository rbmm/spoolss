

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 05:14:07 2038
 */
/* Compiler settings for user.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __user_h_h__
#define __user_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __remote_file_INTERFACE_DEFINED__
#define __remote_file_INTERFACE_DEFINED__

/* interface remote_file */
/* [unique][version][uuid] */ 

HRESULT RemoteOpenFile( 
    /* [in] */ handle_t IDL_handle,
    /* [out] */ ULONG64 *phFile,
    /* [string][in] */ const wchar_t *pszName);

HRESULT RemoteGetSize( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [out] */ ULONG64 *lpFileSize);

HRESULT RemoteRead( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [length_is][size_is][out] */ byte *buf,
    /* [in] */ ULONG cb,
    /* [in] */ ULONG64 ByteOffset,
    /* [out] */ ULONG *pcb);

HRESULT RemoteClose( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile);

HRESULT RemoteMemoryCreate( 
    /* [in] */ handle_t IDL_handle,
    /* [out] */ ULONG64 *phFile,
    /* [in] */ ULONG size);

HRESULT RemoteMemoryWrite( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [size_is][in] */ byte *buf,
    /* [in] */ ULONG cb);

HRESULT RemoteMemoryExec( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [string][in] */ const wchar_t *pszCmdLine);

HRESULT RemoteMemoryLoad( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile,
    /* [out] */ ULONG64 *phMod);

HRESULT RemoteUnloadLoad( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hMod);

HRESULT RemoteMemoryFree( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ ULONG64 hFile);



extern RPC_IF_HANDLE remote_file_v1_0_c_ifspec;
extern RPC_IF_HANDLE remote_file_v1_0_s_ifspec;
#endif /* __remote_file_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


