// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "pbitmap.h"

#define NumDirect 	((SectorSize - 2 * sizeof(int)) / sizeof(int))
#define MaxFileSize 	(NumDirect * SectorSize)

#define NUM_DIRECT_FRAGMENTS 	16

// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:
    FileHeader(int id);
    ~FileHeader() {};

    void fetchFrom(); 			// Initialize file header from disk
    void writeBack(); 			// Write modifications to file header
					//  back to disk

    int FileLength();			// Return the length of the file 
					// in bytes

    void Print();			// Print the contents of the file.

    int getId() { return id; };
    void setId(int i) { id = i; };
    int getType() { return type; };
    void setType(int i) { type = i; };
    int getOwner() { return owner_id; };
    void setOwner(int i) { owner_id = i; };
    int getLastModified() { return last_modified; };
    void setLastModified(int i);
    int getLastAccessed() { return last_accessed; };
    void setLastAccessed(int i) { last_accessed = i; };
    int getLinkCount() { return link_count; };
    void setLinkCount(int i) { link_count = i; };
    int getLength() { return numBytes; };
    void setLength(int l) { numBytes = l; };
    int getNumFragments() { return numFragments; };
    void setNumFragments(int i) { numFragments = i; };
    int* getDataFragments() { return dataFragments; };
    void setDataFragments(int *frags, int len) {
	int size = len > NUM_DIRECT_FRAGMENTS ? NUM_DIRECT_FRAGMENTS : len;
	for (int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i)
	    dataFragments[i] = 0;
	for (int i = 0;i < size; ++i)
	    dataFragments[i] = frags[i];
    }
    int getIndirectBlock() { return indirect_block; };
    void setIndirectBlock(int i) { indirect_block = i; };
  private:
    int id;	// ID starts from 1. 0 is reserved for control blocks or errors.
    int type;
    int owner_id;
    int last_modified;
    int last_accessed;
    int link_count;

    int numBytes;			// Number of bytes in the file
    int numFragments;
    int dataFragments[NUM_DIRECT_FRAGMENTS];

    int indirect_block;			// Indirect block (for large files)

    int firstSector();
};

#endif // FILEHDR_H
