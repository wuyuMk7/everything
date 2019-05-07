#ifndef _SUPERBLOCK_H_
#define _SUPERBLOCK_H_

class SuperBlock {
public:
  SuperBlock() : disk_size(0), block_size(0), block_count(0), fragment_size(0), 
	fragment_count(0), inode_count(0),
	cylinder_groups_count(1), cylinder_in_each_group(0),
  	offset_each_superblock_cylinder(0),
	offset_each_cylinder_summary(0), offset_cylinder_free_block_list(0),
	offset_cylinder_free_inode_list(0), offset_cylinder_inode_table(0),
	offset_cylinder_data_block(0), num_inode_cylinder(0),
	num_fragment_cylinder(0)
	{};

  SuperBlock(int disk_size, int block_size, int block_count, int fragment_size, 
	int fragment_count, int inode_count, int cylinder_groups_count, 
	int cylinder_in_each_group, 
  	int offset_each_superblock_cylinder,
	int offset_each_cylinder_summary, 
	int offset_cylinder_free_block_list, int offset_cylinder_free_inode_list,
	int offset_cylinder_inode_table, int offset_cylinder_data_block,
	int num_inode_cylinder, int num_fragment_cylinder) : 
	disk_size(disk_size), block_size(block_size), 
	block_count(block_count), fragment_size(fragment_size),
	fragment_count(fragment_count), inode_count(inode_count),
	cylinder_groups_count(cylinder_groups_count),
 	cylinder_in_each_group(cylinder_in_each_group),
  	offset_each_superblock_cylinder(offset_each_superblock_cylinder),
	offset_each_cylinder_summary(offset_each_cylinder_summary),
	offset_cylinder_free_block_list(offset_cylinder_free_block_list),
	offset_cylinder_free_inode_list(offset_cylinder_free_inode_list),
	offset_cylinder_inode_table(offset_cylinder_inode_table),
	offset_cylinder_data_block(offset_cylinder_data_block),
	num_inode_cylinder(num_inode_cylinder),
	num_fragment_cylinder(num_fragment_cylinder) {};
  ~SuperBlock() {};

  void fetchFrom(int sector);
  void writeBack(int sector);

  // Getters and setters
  int getDiskSize() { return disk_size; };
  int getBlockSize() { return block_size; };
  int getBlockCount() { return block_count; };
  int getFragmentSize() { return fragment_size; };
  int getFragmentCount() { return fragment_count; };
  int getInodeCount() { return inode_count; };
  int getCylinderGroupsCount() { return cylinder_groups_count; };
  int getCylinderInEachGroup() { return cylinder_in_each_group; };
  int getOffsetSuperBlock() { return offset_each_superblock_cylinder; }
  int getOffsetEachCylinderSummary() { return offset_each_cylinder_summary; };
  int getOffsetCylinderFreeBlockList() { return offset_cylinder_free_block_list; };
  int getOffsetCylinderFreeInodeList() { return offset_cylinder_free_inode_list; };
  int getOffsetCylinderInodeTable() { return offset_cylinder_inode_table; };
  int getOffsetCylinderDataBlock() { return offset_cylinder_data_block; };
  int getNumInodeInCylinder() { return num_inode_cylinder; };
  int getNumFragmentInCylinder() { return num_fragment_cylinder; };
private:
  int disk_size;
  int block_size;
  int block_count;
  int fragment_size;
  int fragment_count;
  int inode_count;
  int cylinder_groups_count;
  int cylinder_in_each_group;
  int offset_each_superblock_cylinder;
  int offset_each_cylinder_summary;
  int offset_cylinder_free_block_list;
  int offset_cylinder_free_inode_list;
  int offset_cylinder_inode_table;
  int offset_cylinder_data_block;
  int num_inode_cylinder;
  int num_fragment_cylinder;
};

#endif
