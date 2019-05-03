///////////////////////////////////////////////////////////////////////
// TextSearch.cpp - searches file for lines matching patterns        //
//                                                                   //
//     Yang He, CSE775 - Distributed Objects, Spring 2018            //
///////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "resource.h"
#include "TextSearch_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "../FileManager/FileManager_i.h"

#include <iostream>
#include <atlsafe.h>
#include <fstream>
#include <string>
#include <regex>

using namespace ATL;

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
	#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllCanUnloadNow();
	if (hr != S_OK)
		return hr;
#endif
			return _AtlModule.DllCanUnloadNow();
	}

// Returns a class factory to create an object of the requested type.
_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllGetClassObject(rclsid, riid, ppv);
	if (hr != CLASS_E_CLASSNOTAVAILABLE)
		return hr;
#endif
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
#endif
		return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
	if (FAILED(hr))
		return hr;
	hr = PrxDllUnregisterServer();
#endif
		return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}


// TextSearch.cpp : Implementation of CTextSearch

#include "stdafx.h"
#include "TextSearch.h"


// CTextSearch

//----< receives a pattern from the client >---------------------------

STDMETHODIMP CTextSearch::sendPattern(BSTR pattern)
{
	this->pattern_ = pattern;
	return S_OK;
}

//----< searches the file to obtain all lines matching the pattern >---

STDMETHODIMP CTextSearch::getResult(VARIANT* filePath, VARIANT* searchResult)
{
	CComQIPtr<IFileManager> pFileManager = this->pFileManager_;
	CComSafeArray<BSTR> targetFiles;
	VARIANT var_targetFiles;
	var_targetFiles.vt = VT_EMPTY;
	pFileManager->getResult(&var_targetFiles);
	targetFiles.Attach(var_targetFiles.parray);

	std::wregex regex_(this->pattern_);
	CComSafeArray<BSTR> out_filePath, out_searchResult;
	out_filePath.Create(targetFiles.GetCount());
	out_searchResult.Create(targetFiles.GetCount());
	for (unsigned long i = 0; i < targetFiles.GetCount(); ++i) {
		out_filePath.SetAt(i, targetFiles.GetAt(i));

		BSTR filename = targetFiles.GetAt(i).Detach();
		std::wifstream file(filename);
		std::wstring cur_result(L"");
		std::wstring line;

		if (file.fail()) {
			std::wcout << L"Can't open file: " << filename << std::endl;
		} else {
			int line_number = 1;
			while (std::getline(file, line)) {
				if (std::regex_search(line, regex_))
					cur_result = cur_result + L"Line " + std::to_wstring(line_number) + L": " + line + L"\n";
				++line_number;
			}
		}
		
		if (cur_result.size() > 0)
		  out_searchResult.SetAt(i, CComBSTR(cur_result.size(), cur_result.data()));
		else
		  out_searchResult.SetAt(i, CComBSTR(""));
	}

	CComVariant(out_filePath).Detach(filePath);
	CComVariant(out_searchResult).Detach(searchResult);

	return S_OK;
}

//----< receives a IUnknown pointer from the client >------------------

STDMETHODIMP CTextSearch::sendFileManager(IUnknown* pFileManager)
{
	this->pFileManager_ = pFileManager;

	return S_OK;
}
