/////////////////////////////////////////////////////////////////////////////
// fileManager.cc - Provides support for file search according to patterns //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////

#include "fileManager.h"
#include "../textSearch/textSearch.h"

namespace FileManager {

  //----< checks if a file's extension is allowed >--------------------------

  bool checkExt(const std::string& toBeChecked) {
    return AllowedExtensions.find(toBeChecked) != AllowedExtensions.end();
  }

  //----< receives a path and several patterns from the client >-------------

  int FileManager::SendPathAndPattern(const std::string& path, const std::vector<std::string>& pattern)
  {
    if (path.length() != 0) this->_path = path;
    if (pattern.size() != 0) this->_pattern = pattern;

    return 0;
  }

  //----< finds files meeting requirements and sends results using IComm >---

  int FileManager::SearchPath(IComm* comm)
  {
    comm->AddRef();

    std::queue<std::string> directories;
    directories.push(this->_path);

    std::unordered_set<std::string> files;
    std::vector<std::string> send_files;
    std::vector<std::string> tmp_dirs;
    std::vector<std::string> tmp_files;
    std::string cur_dir;
    std::string cur_file_extension;

    while(!directories.empty()) {
      cur_dir = directories.front();
      //std::cout << cur_dir << std::endl;

      for (auto& pattern : this->_pattern) {
        tmp_files = FileSystem::Directory::getFiles(cur_dir, pattern);
        for (std::string tmp_file : tmp_files) {
          cur_file_extension = FileSystem::Path::getExt(tmp_file);
          if (checkExt(cur_file_extension) && (files.insert(cur_dir + "/" + tmp_file).second))
            send_files.push_back(cur_dir + "/" + tmp_file);
            //files.insert(cur_dir + "/" + tmp_file);
        }
      }

      tmp_dirs = FileSystem::Directory::getDirectories(cur_dir);
      for (std::string tmp_dir : tmp_dirs) {
        //std::cout << cur_dir + "/" + tmp_dir << std::endl;
        directories.push(cur_dir + "/" + tmp_dir);
      }

      directories.pop();
    }

    //std::vector<std::string> send_files;
    //std::move(files.begin(), files.end(), std::back_inserter(send_files));
    int ret = comm->SendFilePath(send_files);

    comm->Release();
    return ret;
  }

  //----< increments reference count >---------------------------------------

  unsigned int FileManager::AddRef()
  {
    this->m_Ref++;

    return this->m_Ref;
  }

  //----< decrements reference count and delete object when ref is zero>-----

  unsigned int FileManager::Release()
  {
    if (this->m_Ref-- == 0) {
      delete this;
      return 0;
    } else {
      return this->m_Ref;
    }
  }

  //----< creates a new FileManager instance and assigns it to the ptr >-----

  int FileManagerFactory::CreateInstance(void **ppv)
  {
    FileManager *fm = new FileManager;

    *ppv = static_cast<IFileManager*>(fm);
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();

    fm->Release();
    return 0;
  }

  //----< increments reference count >---------------------------------------

  unsigned int FileManagerFactory::AddRef()
  {
    this->m_Ref ++;

    return this->m_Ref;
  }

  //----< decrements reference count and delete object when ref is zero >----

  unsigned int FileManagerFactory::Release()
  {
    if (this->m_Ref -- == 0) {
      delete this;
      return 0;
    } else {
      return this->m_Ref;
    }
  }
}


//----< exported to exteranl modules for creating new FileManager >--------

extern "C" int DllLoadObject(void **ppv) {
  FileManager::FileManagerFactory *pFac = new FileManager::FileManagerFactory;
  int ret = pFac->CreateInstance(ppv);

  pFac->Release();
  return ret;
}
