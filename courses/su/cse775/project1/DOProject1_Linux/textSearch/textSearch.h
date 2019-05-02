#ifndef __TEXT_SEARCH_H__
#define __TEXT_SEARCH_H__

/////////////////////////////////////////////////////////////////////////////
// textSearch.h - Provides support for content search according to regexp  //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////

/*
 * Module Operations:
 * ==================
 * This package provides the namespace TextSearch containing:
 * classes: TextSearch and TextSearchFactory
 *
 * The TextSearch class derives from ITextSearch and IComm.
 * It supports receiving regular expressions for content search from the client.
 * It is capable of finding all lines containing strings that can match
 * the regular expression from one file. It receives a IFileManager pointer
 * from the client and triggers the FileManager to search path for files.
 * TextSearch object will match contents from these files and returns the matched
 * lines along with the line number back to the client.
 * TextSearchFactory class derives from ITextSearchFactory class
 * and provides methods for creating TextSearch instance.
 *
 * Both of these two classes implement the AddRef and Relase method
 * for handling reference count.
 *
 * Public Interface:
 * =================
 * TextSearch::SendFilePath(files)
 * - Serves as IComm to receive filepaths from FileManager object
 * TextSearch::SearchFile(*pfm, regex, results)
 * - Search lines matching regex and send results back to the client
 * TextSearch::AddRef()
 * - Increments reference count when being used
 * TextSearch::Release()
 * - Decrements reference count when released
 * - Get delelted when the reference count equlas to zero
 *
 * TextSearchFactory::CreateInstance(void**)
 * - Receives a pointer from client
 * - Creates a new TextSearch instance and assigns it to the pointer
 * TextSearchFactory::AddRef()
 * - Increments reference count when being used
 * TextSearchFactory::Release()
 * - Decrements reference count when released
 * - Get deleted when the reference count equals to zero
 *
 * DllLoadObject(void**)
 * - Exported function
 * - Used by client to load TextSearch instance
 *
 * Required Files:
 * ===============
 * textSearch.h, textSearch.cc
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

#include <iostream>
#include <fstream>
#include <regex>

#include "../include/IComm.h"
#include "../include/ITextSearch.h"

namespace TextSearch {
  class TextSearch : public ITextSearch, IComm
  {
  public:
    TextSearch() : m_Ref(1) {};
    ~TextSearch() {};

    int SendFilePath(const std::vector<std::string>& files);
    int SearchFile(IFileManager*pfm, const std::string& regex, std::vector<std::pair<std::string, std::map<int, std::string> > >& results);

    unsigned int AddRef();
    unsigned int Release();
  private:
    int m_Ref;
    std::vector<std::string> _files;
  };

  class TextSearchFactory : public ITextSearchFactory
  {
  public:
    TextSearchFactory() : m_Ref(1) {};
    int CreateInstance(void **ppv);

    unsigned int AddRef();
    unsigned int Release();
  private:
    int m_Ref;
  };
}

#endif
