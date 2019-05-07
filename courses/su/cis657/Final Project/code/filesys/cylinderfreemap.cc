#include "cylinderfreemap.h"

#include "synchdisk.h"
#include "main.h"

CylinderFreemap::CylinderFreemap() : 
	num_cylinder_inode(0), num_cylinder_fragment(0) {};

CylinderFreemap::CylinderFreemap(int inode, int fragment) :
	num_cylinder_inode(inode), num_cylinder_fragment(fragment)
{
  inode_free_map = new PersistentBitmap(num_cylinder_inode); 
  fragment_free_map = new PersistentBitmap(num_cylinder_fragment);
}

CylinderFreemap::~CylinderFreemap() 
{
  if (inode_free_map) 
    delete inode_free_map;
  if (fragment_free_map)
    delete fragment_free_map;
}

void
CylinderFreemap::fetchFrom(int block_start, int inode_start)
{
  DEBUG(dbgFile, "Fetching cylinder fragment free map from sector #" << block_start <<
		 " and inode free map from sector #" << inode_start);

  if (!inode_free_map)
    inode_free_map = new PersistentBitmap(num_cylinder_inode);
  if (!fragment_free_map)
    fragment_free_map = new PersistentBitmap(num_cylinder_fragment);

  DEBUG(dbgFile, "Before Fetch - first block occupation: " << fragment_free_map->Test(0));
  kernel->synchDisk->ReadLength(block_start, fragment_free_map->NumWords() * sizeof(unsigned int), (char*) fragment_free_map->Map());
  kernel->synchDisk->ReadLength(inode_start, inode_free_map->NumWords() * sizeof(unsigned int), (char*) inode_free_map->Map());
  DEBUG(dbgFile, "After Fetch - first block occupation: " << fragment_free_map->Test(0));
  //fragment_free_map->Print();
}

void
CylinderFreemap::writeBack(int block_start, int inode_start)
{
  DEBUG(dbgFile, "Writing cylinder fragment free map to sector #" << block_start << 
		 " and inode free map to sector #" << inode_start);

  if (!inode_free_map)
    inode_free_map = new PersistentBitmap(num_cylinder_inode);
  if (!fragment_free_map)
    fragment_free_map = new PersistentBitmap(num_cylinder_fragment);

  kernel->synchDisk->WriteLength(block_start, fragment_free_map->NumWords() * sizeof(unsigned int), (char*) fragment_free_map->Map());
  kernel->synchDisk->WriteLength(inode_start, inode_free_map->NumWords() * sizeof(unsigned int), (char*) inode_free_map->Map());
}
