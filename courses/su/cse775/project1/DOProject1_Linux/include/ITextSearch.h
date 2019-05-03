#ifndef __ITEXT_SEARCH_H__
#define __ITEXT_SEARCH_H__

/////////////////////////////////////////////////////////////////////////////
// ITextSearch.h - Provides ITextSearch and ITextSearchFactory             //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * ==================
 * This file declares the ITextSearch and ITextSearch interface.
 * ITextSearch interface defines the basic functionalities in the text searcher
 * module, including to receive regular expression for content search from
 * the client, to receive a file manager pointer and send a IComm pointer
 * to the file manager in order that the file manager is capbable of sending
 * file search results back to text searcher, and to send content search results
 * back to the client.
 * ITextSearchFactory is the interface for text searcher's factory class.
 * It contains CreateInstance method used to create a new text search instance.
 *
 * Abstract class:
 * ==================
 * ITextSearch
 * ITextSearchFactory
 *
 * Methods to be implemented:
 * ==========================
 * ITextSearch::SearchFile
 * ITextSearchFactory::CreateInstance
 *
 * Required Files:
 * ===============
 * ITextSearch.h
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

#include <vector>
#include <map>
#include <unordered_map>

#include "IUnknown.h"
#include "IFileManager.h"

class ITextSearch : public IUnknown
{
public:
  virtual ~ITextSearch() {};
  virtual int SearchFile(IFileManager* pfm, const std::string& regex, std::vector<std::pair<std::string, std::map<int, std::string> > >& results) = 0;
};

class ITextSearchFactory : public IUnknown
{
public:
  virtual ~ITextSearchFactory() {};
  virtual int CreateInstance(void** ppv) = 0;
};

#endif
