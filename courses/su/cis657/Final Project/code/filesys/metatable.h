#ifndef _META_TABLE_H_
#define _META_TABLE_H_

#include "synch.h"

class MetaTableItem {
public:
  MetaTableItem() {};
  MetaTableItem(int inode): inode(inode), usage(0), last_modified(0), finish(0) {};

  int inode;
  int usage;
  int last_modified;  
  int finish;
};

class MetaTable {
public:
  MetaTable(int size);
  ~MetaTable();

  void acquire();
  void release();

  int findInodeIndex(int inode);

  int getUsage(int inode);
  void setUsage(int inode, int usage);
  void incUsage(int inode);
  void decUsage(int inode);

  void setLastModified(int inode, int);
  bool checkLastModified(int inode, int);

  bool checkFinish(int inode);
  bool tryFinish(int inode);
  bool waitingFinish(int inode);

  void print();
  void print(int);
private:
  int size;
  MetaTableItem *table;
  Lock *lock;
};

#endif
