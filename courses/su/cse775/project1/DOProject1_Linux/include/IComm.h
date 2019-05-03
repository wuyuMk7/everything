#ifndef __I_COMM_H__
#define __I_COMM_H__

/////////////////////////////////////////////////////////////////////////////
// IComm.h - Provides IComm interface for communication between components //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * ==================
 * This file declares the IComm interface.
 * IComm interface derives from IUnknonwn interface. It also provides one more 
 * method used by file manager module to send file searcn results to the 
 * text search module. Text search implementation is required to derive from 
 * this interface in order to implement this method for file manager.
 *
 * Abstract class:
 * ==================
 * Icomm
 *
 * Methods to be implemented:
 * ==========================
 * IComm::SendFilePath
 *
 * Required Files:
 * ===============
 * Icomm.h
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

#include <iostream>
#include <vector>

#include "IUnknown.h"

class IComm : public IUnknown
{
public:
  virtual ~IComm() {};
  virtual int SendFilePath(const std::vector<std::string>& files) = 0;
};

#endif
