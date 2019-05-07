#ifndef _CYLINDER_FREEMAP_H_
#define _CYLINDER_FREEMAP_H_

#include "pbitmap.h"

class CylinderFreemap {
public:
  CylinderFreemap();
  CylinderFreemap(int inode, int fragment);
  ~CylinderFreemap();

  void fetchFrom(int block_start, int inode_start);
  void writeBack(int block_start, int inode_start);

  int totalInodesInCylinder() { return num_cylinder_inode; };
  int totalFragmentsInCylinder() { return num_cylinder_fragment; };

  PersistentBitmap *getFreeInodeMap() { return inode_free_map; };
  PersistentBitmap *getFreeFragmentMap() { return fragment_free_map; };
private:
  int num_cylinder_inode;
  int num_cylinder_fragment;
  
  PersistentBitmap *inode_free_map;
  PersistentBitmap *fragment_free_map;
};

#endif

