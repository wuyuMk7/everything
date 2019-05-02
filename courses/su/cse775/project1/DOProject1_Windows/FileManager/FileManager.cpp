///////////////////////////////////////////////////////////////////////
// FileManager.cpp - searches path to get files matching patterns    //
//                                                                   //
//     Yang He, CSE775 - Distributed Objects, Spring 2018            //
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "FileManager_i.h"
#include "dllmain.h"
#include "xdlldata.h"

#include "../FileSystem/FileSystemDemo/FileSystem.h"

#include <iostream>
#include <queue>
#include <unordered_set>

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


// FileManager.cpp : Implementation of CFileManager

#include "stdafx.h"
#include "FileManager.h"


// CFileManager

//----< receives a path and several file patterns from client >--------

STDMETHODIMP CFileManager::sendPathAndPatterns(BSTR path, VARIANT patterns)
{
	this->path_ = path;
	this->patterns_.Attach(patterns.parray);
	//std::wcout << this->patterns_.GetAt(3).Detach() << std::endl;
	return S_OK;
}

//----< searches path recursively and gets files matching patterns >---

STDMETHODIMP CFileManager::getResult(VARIANT* result)
{
	// Convert path_ and patterns_ from CComBSTR to std::string
	const size_t newPathSize = (this->path_.Length() + 1) * 2;
	char *path = new char[newPathSize];
	CW2A tmp_path(this->path_);
	strcpy_s(path, newPathSize, tmp_path);

	std::vector<std::string> patterns(this->patterns_.GetCount());
	for (size_t i = 0; i < this->patterns_.GetCount(); ++i) {
		CComBSTR cur_pattern = this->patterns_.GetAt(i);
		const size_t newPatternSize = (cur_pattern.Length() + 1) * 2;
		char *pattern = new char[newPatternSize];
		CW2A tmp_pattern(cur_pattern);
		strcpy_s(pattern, newPatternSize, tmp_pattern);
		patterns.push_back(pattern);
	}
	
	std::queue<std::string> directories;
	directories.push(path);
	std::unordered_set<std::string> filePaths;
	std::vector<std::string> sendFilePaths;
	std::vector<std::string> tmpDirs;
	std::vector<std::string> tmpFiles;
	std::string cur_dir;
	while (!directories.empty()) {
		cur_dir = directories.front();
		for (auto& pattern : patterns) {
			tmpFiles = FileSystem::Directory::getFiles(cur_dir, pattern);
			for (auto& tmp_file : tmpFiles)
				if (filePaths.insert(cur_dir + "\\" + tmp_file).second)
					sendFilePaths.push_back(cur_dir + "\\" + tmp_file);
		}
		tmpDirs = FileSystem::Directory::getDirectories(cur_dir);
		for (auto& dir : tmpDirs)
			if (dir != "." && dir != "..") 
				directories.push(cur_dir + "\\" + dir);
		
		directories.pop();
	}

	CComSafeArray<BSTR> outputResult;
	outputResult.Create(sendFilePaths.size());
	for (size_t i = 0; i < sendFilePaths.size(); ++i) 
		outputResult.SetAt(i, CComBSTR(sendFilePaths[i].c_str()));
	CComVariant(outputResult).Detach(result);
	return S_OK;
}
