#include "cylindersummary.h"

#include "synchdisk.h"
#include "main.h"

void
CylinderSummary::fetchFrom(int start_sector) 
{
  DEBUG(dbgFile, "Fetching cylinder summary information from start_sector #" << start_sector);

  DEBUG(dbgFile, "Before Fetch - free blocks: " << this->num_free_block);
  kernel->synchDisk->ReadLength(start_sector, sizeof(CylinderSummary), (char*) this);
  DEBUG(dbgFile, "After Fetch - free blocks: " << this->num_free_block);
  DEBUG(dbgFile, "After Fetch - free blocks: " << this->num_free_fragment);
}

void
CylinderSummary::writeBack(int start_sector)
{
  DEBUG(dbgFile, "Write cylinder summary information back to the start_sector #" << start_sector);
  kernel->synchDisk->WriteLength(start_sector, sizeof(CylinderSummary), (char*) this);
}
