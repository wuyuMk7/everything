#ifndef __IFILE_MANAGER_H__
#define __IFILE_MANAGER_H__

/////////////////////////////////////////////////////////////////////////////
// IFileManager.h - Provides IFileManager and IFileManagerFactory          //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * ==================
 * This file declares the IFileManager and IFileManagerFactory interface.
 * IFileManager interface defines the basic functionalities in the file manager
 * module, including to receive path and patterns data from client and
 * to search path for files meeting patterns then send the result through IComm
 * to the other components.
 * IFileManagerFactory is the interface for file manager's factory class.
 * It contains CreateInstance method used to create a new file manager instance.
 *
 * Abstract class:
 * ==================
 * IFileManager
 * IFileManagerFactory
 *
 * Methods to be implemented:
 * ==========================
 * IFileManager::SendPathAndPattern
 * IFileManager::SearchPath
 * IFileManagerFactory::CreateInstance
 *
 * Required Files:
 * ===============
 * IFileManager.h
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

#include <vector>
#include <unordered_map>

#include "IUnknown.h"
#include "IComm.h"

class IFileManager : public IUnknown
{
public:
  virtual ~IFileManager() {};
  virtual int SendPathAndPattern(const std::string& path, const std::vector<std::string>& pattern) = 0;
  virtual int SearchPath(IComm*) = 0;
};

class IFileManagerFactory : public IUnknown
{
public:
  virtual ~IFileManagerFactory() {};
  virtual int CreateInstance(void** ppv) = 0;
};

#endif
