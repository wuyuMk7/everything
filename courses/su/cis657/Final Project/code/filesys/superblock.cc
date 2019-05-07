#include "superblock.h"

#include "debug.h"
#include "synchdisk.h"
#include "main.h"

void
SuperBlock::fetchFrom(int start_sector)
{
  DEBUG(dbgFile, "Fetching super block from sector #" << start_sector);

  DEBUG(dbgFile, "Before Fetch - block size: " << this->block_size);
  kernel->synchDisk->ReadLength(start_sector, sizeof(SuperBlock), (char*) this);
  DEBUG(dbgFile, "After Fetch - block size: " << this->block_size);
}

void
SuperBlock::writeBack(int start_sector)
{
  DEBUG(dbgFile, "Super Block is writing back to sector #" << start_sector);
  
  kernel->synchDisk->WriteLength(start_sector, sizeof(SuperBlock), (char*) this);
}
