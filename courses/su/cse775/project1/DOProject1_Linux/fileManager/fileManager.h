#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

/////////////////////////////////////////////////////////////////////////////
// fileManager.h - Provides support for file search accoring to patterns   //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////

/*
 * Module Operations:
 * ==================
 * This package provides the namespace FileManager containing:
 * classes: FileManager and FileManagerFactory
 *
 * The FileManager class derives from IFileManager class.
 * It supports receiving search path and patterns from the client.
 * It is capable of finding all files residing in a directory tree
 * rooted at the path and matching one or more filename patterns.
 * After it has finished file searching, it will send the result to
 * the other components through an IComm interface.
 *
 * FileManagerFactory class derives from IFileManagerFactory class
 * and provides methods for creating FileManager instance.
 *
 * Both of these two classes implement the AddRef and Relase method
 * for handling reference count.
 *
 * Public Interface:
 * =================
 * FileManager::SendPathAndPattern(path, pattern)
 * - Receives path and patterns from client
 * FileManager::SearchPath(IComm)
 * - Finds files meeting requirements then sends through IComm
 * FileManager::AddRef()
 * - Increments reference count when being used
 * FileManager::Release()
 * - Decrements reference count when released
 * - Get delelted when the reference count equlas to zero
 *
 * FileManagerFactory::CreateInstance(void**)
 * - Receives a pointer from client
 * - Creates a new FileManager instance and assigns it to the pointer
 * FileManagerFactory::AddRef()
 * - Increments reference count when being used
 * FileManagerFactory::Release()
 * - Decrements reference count when released
 * - Get deleted when the reference count equals to zero
 *
 * DllLoadObject(void**)
 * - Exported function
 * - Used by client to load FileManager instance
 *
 * Required Files:
 * ===============
 * fileManager.h, fileManager.cc
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>

#include "./FileSystem.h"
#include "../include/IComm.h"
#include "../include/IFileManager.h"

namespace FileManager {
  std::unordered_set<std::string> AllowedExtensions{"", "cc", "cpp", "h", "txt", "cs", "sh"};

  bool checkExt(const std::string& toBeChecked);

  class FileManager : public IFileManager
  {
  public:
    //FileManager() : m_Ref(1), _path("."), _pattern({"*.*"}) {};
    FileManager() : m_Ref(1), _path("."), _pattern({}) {};
    ~FileManager() {};

    int SendPathAndPattern(const std::string& path, const std::vector<std::string>& pattern);
    int SearchPath(IComm* comm);

    unsigned int AddRef();
    unsigned int Release();
  private:
    int m_Ref;
    std::string _path;
    std::vector<std::string> _pattern;
  };

  class FileManagerFactory : public IFileManagerFactory
  {
  public:
    FileManagerFactory() : m_Ref(1) {};
    int CreateInstance(void **ppv);

    unsigned int AddRef();
    unsigned int Release();
  private:
    int m_Ref;
  };
}

#endif
