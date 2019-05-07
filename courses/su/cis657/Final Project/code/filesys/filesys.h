// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#include "superblock.h"
#include "pbitmap.h"

#define CYLINDER_GROUPS 	2
#define OFFSET_CYLINDER_SUMMARY	1
#define BLOCK_SIZE 		4096
#define FRAGMENT_SIZE		1024
#define INODE_RANGE		2048

#define FRAGMENT_IN_ONE_BLOCK	(BLOCK_SIZE / FRAGMENT_SIZE)

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available

class FileSystem {
  public:
    FileSystem() {}

    bool Create(char *name) {
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS

class FsControlLock;
class MetaTable;

class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.
    ~FileSystem() {
	if (super_block) delete super_block;
	if (fs_control_lock) delete fs_control_lock;
        if (meta_table) delete meta_table;
    };

    bool CreateRoot();
    bool Create(char *name, int type = 0);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)

    bool Remove(char *name);  		// Delete a file (UNIX unlink)
    bool Unlink(int inode);  		// Delete a file (UNIX unlink)
    bool ChangeDirectory(char *name);

    void List(char* directory);		// List all the files in the file system

    void Print();			// List all the files and their contents

    SuperBlock* superBlock() { return super_block; };
    MetaTable* metaTable() { return meta_table; };
    bool Allocate(FileHeader *inode, int new_length);

    void PrintInodeMap();
    void PrintFragmentMap();
    void PrintMetaTable();
    void PrintMetaTableItem(int inode);
  private:
   SuperBlock *super_block;
   FsControlLock *fs_control_lock;
   MetaTable *meta_table;

   int NextPlaceForSmallFragments(int size, PersistentBitmap *bitmap, int window, int to_align, int *min);
   void NextAlignedFragments(int size, PersistentBitmap *bitmap, int window, int to_align, int *ret, int *ret_len);
};

#endif // FILESYS

#endif // FS_H
