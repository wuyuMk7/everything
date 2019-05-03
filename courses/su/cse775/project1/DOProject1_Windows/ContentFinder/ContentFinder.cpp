///////////////////////////////////////////////////////////////////////
// ContentFinder.cpp - as client, is using FileManager and TextSearch//
//                                                                   //
//     Yang He, CSE775 - Distributed Objects, Spring 2018            //
///////////////////////////////////////////////////////////////////////

#include "../FileManager/FileManager_i.h"
#include "../TextSearch/TextSearch_i.h"

#include <iostream>
#include <string>
#include <atlbase.h>
#include <atlstr.h>
#include <atlsafe.h>

//----< print help message >-------------------------------------------

void help();

//----< print content search result >----------------------------------

void display(CComSafeArray<BSTR> fileNames, CComSafeArray<BSTR> searchResults);

//----< command line options handler >---------------------------------

int handleOptions(int argc, char *argv[],
	CComBSTR& path, CComSafeArray<BSTR>& patterns, CComBSTR& contentRegex);

int main(int argc, char *argv[], char *envp[])
{

	CoInitialize(NULL);
	{
		CComBSTR path(".");
		CComBSTR contentRegex("");
		CComSafeArray<BSTR> patterns((ULONG)0);
		
		/*
		patterns.Create(1);
		for (long i = 0; i < 1; ++i) {
			patterns.SetAt(i, CComBSTR("*.cpp"));
		}
		*/
		if (handleOptions(argc, argv, path, patterns, contentRegex) == 0) {
			// create a FileManager instance
			CComQIPtr<IFileManager> pFileManager;
			pFileManager.CoCreateInstance(CLSID_FileManager);
			if (pFileManager) {
				VARIANT var;
				var.vt = VT_EMPTY;
				CComVariant(patterns).Detach(&var);
				pFileManager->sendPathAndPatterns(path, var);
			}

			// create a TextSearch instance
			CComQIPtr<ITextSearch> pITextSearch;
			pITextSearch.CoCreateInstance(CLSID_TextSearch);
			if (pITextSearch) {
				pITextSearch->sendPattern(contentRegex);
				pITextSearch->sendFileManager(pFileManager);

				CComSafeArray<BSTR> fileNames, searchResults;
				VARIANT var_fileNames, var_searchResults;
				var_fileNames.vt = VT_EMPTY;
				var_searchResults.vt = VT_EMPTY;
				pITextSearch->getResult(&var_fileNames, &var_searchResults);
				fileNames.Attach(var_fileNames.parray);
				searchResults.Attach(var_searchResults.parray);

				display(fileNames, searchResults);
			}
			std::wcout << "\n\n";
		}
	}
	CoUninitialize();
	return 0;
}

//----< print help message >-------------------------------------------

void help()
{
	std::cout << "Usage: " <<
		"[/p path] " <<
		"[/f file pattern1, file pattern2, ...] " <<
		"[/r content regular expression] " <<
		"[/h help]" << std::endl;
}

//----< print content search result >----------------------------------

void display(CComSafeArray<BSTR> fileNames, CComSafeArray<BSTR> searchResults)
{
	for (size_t i = 0; i < fileNames.GetCount(); ++i)
		if (searchResults.GetAt(i).Length() != 0)
			std::wcout << L"- Search file: " << fileNames.GetAt(i).Detach() << std::endl
			<< searchResults.GetAt(i).Detach() << std::endl;
		else
			std::wcout << L"- Search file: " << fileNames.GetAt(i).Detach() << std::endl
			<< L"No result matched." << std::endl << std::endl;
}

//----< command line options handler >---------------------------------

int handleOptions(int argc, char *argv[],
	CComBSTR& path, CComSafeArray<BSTR>& patterns, CComBSTR& contentRegex) 
{
	int count = 1;
 	for (; count < argc; ++count) {
		std::string cur_argv(argv[count]);
		if (cur_argv == "/p" || cur_argv == "/path") {
			++count;
			if (count < argc)
				path = CComBSTR(argv[count]);
		} else if (cur_argv == "/f" || cur_argv == "/fileFormat") {
			++count;
			for (int fCount = count; fCount < argc; ++fCount) {
				if (argv[fCount][0] == '/') {
					count = fCount - 1;
					break;
				}
				else {
					patterns.Add(CComBSTR(argv[fCount]));
				}
			}
		} else if (cur_argv == "/r" || cur_argv == "/contentRegex") {
			++count;
			if (count < argc)
				contentRegex = CComBSTR(argv[count]);
		} else if (cur_argv == "/h" || cur_argv == "/help") {
			help();
			return 1;
		} else {
			if (argv[count][0] == '/') {
				std::cout << "Invalid Option: " << argv[count] << std::endl;
				help();
				return 1;
			}
		}
	}

	return 0;
}