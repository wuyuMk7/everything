/////////////////////////////////////////////////////////////////////////////
// content-search.cc: client side, is using fileManager and textSearch     //
//                                                                         //
//           Yang He, CSE 775 Distributed Obbject, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <dlfcn.h>
#include <getopt.h>
//#include <unistd.h>

#include "include/IFileManager.h"
#include "include/ITextSearch.h"

//----< print help information >-------------------------------------------

void help(char* argv0);

//----< command line options handler >-------------------------------------

int handleOptions(int argc, char* argv[], std::string& path,
                   std::string& content_regex, std::vector<std::string>& file_patterns);

//----< print content search results >-------------------------------------

void displayResult(std::vector<std::pair<std::string, std::map<int, std::string> > >& results);

int main(int argc, char* argv[])
{
  std::string path = ".";
  std::string content_regex = ""; // ".*";
  std::vector<std::string> file_patterns; // {"*.*"};
  handleOptions(argc, argv, path, content_regex, file_patterns);

  void *fileManager_handle = dlopen("./libs/fileManager.so", RTLD_LAZY);
  if (!fileManager_handle) {
    std::cout << "\n  Failed to load libs/fileManager.so\n\n";
    return 1;
  }
  int (*fileManagerDllLoad)(void**);
  fileManagerDllLoad = (int (*)(void**))dlsym(fileManager_handle, "DllLoadObject");
  if (!fileManagerDllLoad) {
    std::cout << "\n  Failed to acquire DllLoadObject function \n\n";
    return 1;
  }
  IFileManager *pfm = NULL;
  if(fileManagerDllLoad((void**)&pfm) != 0) {
    std::cout << "\n Failed to load FileManager \n\n";
    return 1;
  }

  void *textSearch_handle = dlopen("./libs/textSearch.so", RTLD_LAZY);
  if (!textSearch_handle) {
    std::cout << "\n  Failed to load libs/textSearch.so\n\n";
    return 1;
  }
  int (*textSearchDllLoad)(void**);
  textSearchDllLoad = (int (*)(void**))dlsym(textSearch_handle, "DllLoadObject");
  if (!textSearchDllLoad) {
    std::cout << "\n  Failed to acquire DllLoadObject function\n\n";
    return 1;
  }
  ITextSearch *pts = NULL;
  if (textSearchDllLoad((void**)&pts) != 0) {
    std::cout << "\n Failed to load TextSearch \n\n";
    return 1;
  }

  std::vector<std::pair<std::string, std::map<int, std::string> > > results;
  pfm->SendPathAndPattern(path, file_patterns);
  pts->SearchFile(pfm, content_regex, results);
  displayResult(results);

  pfm->Release();
  pts->Release();
  return 0;
}

//----< print help information >-------------------------------------------

void help(char* argv0)
{
  std::cout << "Usage: " << argv0 << " [-p <path>] [-f <wildcard filename>] [-r <regex>] [-h]" << std::endl;
}

//----< command ling options handler >-------------------------------------

int handleOptions(int argc, char* argv[], std::string& path,
                   std::string& content_regex, std::vector<std::string>& file_patterns)
{
  int option;

  static struct option long_options[] =
  {
    {"path", required_argument, NULL, 'p'},
    {"file-format", required_argument, NULL, 'f'},
    {"content-regular-expression", required_argument, NULL, 'r'},
    {"help", no_argument, NULL, 'h'}
  };

  while ((option = getopt_long(argc, argv, "hp:f:r:", long_options, NULL)) != -1) {
    switch(option) {
    case 'p':
      path = std::string(optarg);
      break;
    case 'f':
      file_patterns.push_back(std::string(optarg));
      break;
    case 'r':
      content_regex = std::string(optarg);
      break;
    case 'h':
      help(argv[0]);
      break;
    default:
      help(argv[0]);
      break;
    }
  }

  return 1;
}

//----< print content search results >-------------------------------------

void displayResult(std::vector<std::pair<std::string, std::map<int, std::string> > > & results)
{
  for (auto& pair : results) {
    std::cout << "- Search file " << pair.first << ": " << std::endl;
    if (pair.second.empty()) {
      std::cout << "  No result matched." << std::endl;
    } else {
      for (auto& match : pair.second) {
        std::cout << "Line " << match.first << ": " << match.second << std::endl;
      }
    }
  }
}

