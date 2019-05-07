#ifndef _CYLINDER_SUMMARY_
#define _CYLINDER_SUMMARY_

class CylinderSummary {
public:
  CylinderSummary(): num_free_block(0), num_free_inode(0), num_free_fragment(0) {};
  CylinderSummary(int num_free_block, int num_inode, int num_fragment):
	num_free_block(num_free_block), num_free_inode(num_inode), num_free_fragment(num_fragment) {};
  ~CylinderSummary() {};

  void fetchFrom(int start_sector);
  void writeBack(int start_sector);

  int incInode() { num_free_inode++; };
  int incBlock() { ++num_free_block; };
  int incFragment() { ++num_free_fragment; };
  int decInode() { --num_free_inode; };
  int decBlock() { --num_free_block; };
  int decFragment() { --num_free_fragment; };

  int numOfFreeInode() { return num_free_inode; };
  int numOfFreeBlock() { return num_free_block; };
  int numOfFreeFragment() { return num_free_fragment; };
  void setNumOfFreeInode(int num) { num_free_inode = num; };
  void setNumOfFreeBlock(int num) { num_free_block = num; };
  void setNumOfFreeFragment(int num) { num_free_fragment = num; };
private:
  int num_free_block;
  int num_free_inode;
  int num_free_fragment;
};

#endif
