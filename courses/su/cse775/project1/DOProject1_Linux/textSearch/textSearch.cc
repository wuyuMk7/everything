/////////////////////////////////////////////////////////////////////////////
// textSearch.cc - Provides support for content search according to regexp //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////

#include "textSearch.h"

namespace TextSearch {

  //----< receives file search results from FileManager >--------------------

  int TextSearch::SendFilePath(const std::vector<std::string>& files)
  {
    for (std::string file : files) {
      this->_files.push_back(file);
    }

    return 0;
  }

  //----< searches lines matching regex and sends results to the client >----

  int TextSearch::SearchFile(IFileManager *pfm, const std::string& regex, std::vector<std::pair<std::string, std::map<int, std::string> > >& results)
  {
    pfm->AddRef();
    pfm->SearchPath(dynamic_cast<IComm*>(this));

    std::regex _regex(regex);
    std::string line;

    for (auto& filepath: _files) {
      std::ifstream file(filepath);
      int line_number = 1;

      if (file.fail()) {
        std::cout << "Cannot open file: " << filepath << std::endl;
      } else {
        //std::cout << "Search file " << filepath << std::endl;
        std::map<int, std::string> match;
        while(std::getline(file, line)) {
          if (std::regex_search(line, _regex)) {
            //std::cout << "Line: " << line_number << "\n" << line << std::endl;
            match[line_number] = line;
          }
          ++ line_number;
        }

        results.push_back(std::make_pair(filepath, match));
      }
    }

    pfm->Release();
    return 1;
  }

  //----< increments reference count >---------------------------------------

  unsigned int TextSearch::AddRef()
  {
    this->m_Ref++;

    return this->m_Ref;
  }

  //----< decrements reference count and delete object when ref is zero>-----

  unsigned int TextSearch::Release()
  {
    if (this->m_Ref-- == 0) {
      delete this;
      return 0;
    } else {
      return this->m_Ref;
    }
  }

  //----< check if a file's extension is allowed >---------------------------
  int TextSearchFactory::CreateInstance(void **ppv)
  {
    TextSearch *ts = new TextSearch;

    *ppv = static_cast<ITextSearch*>(ts);
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();

    ts->Release();
    return 0;
  }

  //----< increments reference count >---------------------------------------

  unsigned int TextSearchFactory::AddRef()
  {
    this->m_Ref ++;

    return this->m_Ref;
  }

  //----< decrements reference count and delete object when ref is zero>-----

  unsigned int TextSearchFactory::Release()
  {
    if (this->m_Ref-- == 0) {
      delete this;
      return 0;
    } else {
      return this->m_Ref;
    }
  }
}

//----< exported to exteranl modules for creating new TextSearch >---------

extern "C" int DllLoadObject(void **ppv) {
  TextSearch::TextSearchFactory *pFac = new TextSearch::TextSearchFactory;
  int ret = pFac->CreateInstance(ppv);

  pFac->Release();
  return ret;
}
