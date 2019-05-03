#ifndef __I_UNKNOWN_H__
#define __I_UNKNOWN_H__

/////////////////////////////////////////////////////////////////////////////
// IUnknown.h - Provides IUnknown interface for reference counts.          //
//                                                                         //
//           Yang He, CSE 775 Distributed Objects, Spring 2019             //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * ==================
 * This file declares the IUnknown interface.
 * IUnknown interface is the base class for all the components. Every component 
 * is required to derive from the IUnknown interface and implement the methods 
 * in order to provide functionalities for reference count calculation.
 *
 * Abstract class:
 * ==================
 * IUnknown
 *
 * Methods to be implemented:
 * ==========================
 * IUnknown::AddRef
 * IUnknown::Release
 *
 * Required Files:
 * ===============
 * IUnknown.h
 *
 * Maintenance History:
 * ====================
 * ver 1.0 : 30 Mar 2019
 * - first release
 */

class IUnknown
{
public:
  virtual unsigned int AddRef() = 0;
  virtual unsigned int Release() = 0;
};

#endif
