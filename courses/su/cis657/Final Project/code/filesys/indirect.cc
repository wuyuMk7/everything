#include "indirect.h"

#include "synchdisk.h"
#include "main.h"

void
IndirectBlock::fetchFrom(int start_sector) 
{
  DEBUG(dbgFile, "Fetching indirect block from start_sector #" << start_sector);

  DEBUG(dbgFile, "Before Fetch - block pos 0: " << fragments[0]);
  kernel->synchDisk->ReadLength(start_sector, sizeof(int) * 128, (char*) fragments);
  DEBUG(dbgFile, "After Fetch - block pos 0: " << fragments[0]);
}

void
IndirectBlock::writeBack(int start_sector) 
{
  DEBUG(dbgFile, "Writing indirect block to start_sector #" << start_sector);
  kernel->synchDisk->WriteLength(start_sector, sizeof(int) * 128, (char*) fragments);
}
