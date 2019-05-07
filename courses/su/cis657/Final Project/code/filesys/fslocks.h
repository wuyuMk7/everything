#ifndef _FS_CONTROL_LOCK_H_
#define _FS_CONTROL_LOCK_H_

#include "synch.h"

class FsControlLock : public Lock 
{
public:
  FsControlLock(char* name): Lock(name) {};
};

#endif
