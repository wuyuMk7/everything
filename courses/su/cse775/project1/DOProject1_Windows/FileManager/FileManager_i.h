

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 07 20:51:50 2019
 */
/* Compiler settings for FileManager.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __FileManager_i_h__
#define __FileManager_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFileManager_FWD_DEFINED__
#define __IFileManager_FWD_DEFINED__
typedef interface IFileManager IFileManager;

#endif 	/* __IFileManager_FWD_DEFINED__ */


#ifndef __FileManager_FWD_DEFINED__
#define __FileManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileManager FileManager;
#else
typedef struct FileManager FileManager;
#endif /* __cplusplus */

#endif 	/* __FileManager_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IFileManager_INTERFACE_DEFINED__
#define __IFileManager_INTERFACE_DEFINED__

/* interface IFileManager */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IFileManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0A7495AD-0CC0-49F7-B78A-8A8E17814ACE")
    IFileManager : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE sendPathAndPatterns( 
            /* [in] */ BSTR path,
            /* [in] */ VARIANT patterns) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getResult( 
            /* [out] */ VARIANT *result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFileManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileManager * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *sendPathAndPatterns )( 
            IFileManager * This,
            /* [in] */ BSTR path,
            /* [in] */ VARIANT patterns);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getResult )( 
            IFileManager * This,
            /* [out] */ VARIANT *result);
        
        END_INTERFACE
    } IFileManagerVtbl;

    interface IFileManager
    {
        CONST_VTBL struct IFileManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFileManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFileManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFileManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFileManager_sendPathAndPatterns(This,path,patterns)	\
    ( (This)->lpVtbl -> sendPathAndPatterns(This,path,patterns) ) 

#define IFileManager_getResult(This,result)	\
    ( (This)->lpVtbl -> getResult(This,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileManager_INTERFACE_DEFINED__ */



#ifndef __FileManagerLib_LIBRARY_DEFINED__
#define __FileManagerLib_LIBRARY_DEFINED__

/* library FileManagerLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_FileManagerLib;

EXTERN_C const CLSID CLSID_FileManager;

#ifdef __cplusplus

class DECLSPEC_UUID("77D13340-3683-4013-ADC1-46432E3FDA5B")
FileManager;
#endif
#endif /* __FileManagerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


