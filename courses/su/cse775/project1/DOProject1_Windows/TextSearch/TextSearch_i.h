

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sun Apr 07 20:51:40 2019
 */
/* Compiler settings for TextSearch.idl:
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

#ifndef __TextSearch_i_h__
#define __TextSearch_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITextSearch_FWD_DEFINED__
#define __ITextSearch_FWD_DEFINED__
typedef interface ITextSearch ITextSearch;

#endif 	/* __ITextSearch_FWD_DEFINED__ */


#ifndef __TextSearch_FWD_DEFINED__
#define __TextSearch_FWD_DEFINED__

#ifdef __cplusplus
typedef class TextSearch TextSearch;
#else
typedef struct TextSearch TextSearch;
#endif /* __cplusplus */

#endif 	/* __TextSearch_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITextSearch_INTERFACE_DEFINED__
#define __ITextSearch_INTERFACE_DEFINED__

/* interface ITextSearch */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITextSearch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BC3B3A10-8F1F-4A82-A21D-39E5BB4C2D38")
    ITextSearch : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE sendPattern( 
            /* [in] */ BSTR pattern) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getResult( 
            /* [out] */ VARIANT *filePath,
            /* [out] */ VARIANT *searchResult) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE sendFileManager( 
            /* [in] */ IUnknown *pFileManager) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITextSearchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITextSearch * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITextSearch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITextSearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *sendPattern )( 
            ITextSearch * This,
            /* [in] */ BSTR pattern);
        
        HRESULT ( STDMETHODCALLTYPE *getResult )( 
            ITextSearch * This,
            /* [out] */ VARIANT *filePath,
            /* [out] */ VARIANT *searchResult);
        
        HRESULT ( STDMETHODCALLTYPE *sendFileManager )( 
            ITextSearch * This,
            /* [in] */ IUnknown *pFileManager);
        
        END_INTERFACE
    } ITextSearchVtbl;

    interface ITextSearch
    {
        CONST_VTBL struct ITextSearchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITextSearch_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITextSearch_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITextSearch_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITextSearch_sendPattern(This,pattern)	\
    ( (This)->lpVtbl -> sendPattern(This,pattern) ) 

#define ITextSearch_getResult(This,filePath,searchResult)	\
    ( (This)->lpVtbl -> getResult(This,filePath,searchResult) ) 

#define ITextSearch_sendFileManager(This,pFileManager)	\
    ( (This)->lpVtbl -> sendFileManager(This,pFileManager) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITextSearch_INTERFACE_DEFINED__ */



#ifndef __TextSearchLib_LIBRARY_DEFINED__
#define __TextSearchLib_LIBRARY_DEFINED__

/* library TextSearchLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_TextSearchLib;

EXTERN_C const CLSID CLSID_TextSearch;

#ifdef __cplusplus

class DECLSPEC_UUID("0F4F2813-96AC-40D4-9547-712DBFBFA8CE")
TextSearch;
#endif
#endif /* __TextSearchLib_LIBRARY_DEFINED__ */

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


