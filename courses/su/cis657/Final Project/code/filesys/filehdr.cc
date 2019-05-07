// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <time.h>

#include "copyright.h"

#include "filehdr.h"
#include "debug.h"
#include "synchdisk.h"
#include "main.h"

#include "indirect.h"

FileHeader::FileHeader(int inode_number) : id(inode_number), owner_id(0), type(0), link_count(1), numBytes(0), numFragments(0), indirect_block(-1) {
	int result = kernel->stats->totalTicks;
	last_modified = result;
	last_accessed = result;
    };

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::fetchFrom()
{
  int start_sector = firstSector();
  DEBUG(dbgFile, "Fetching i-node from sector #" << start_sector);

  DEBUG(dbgFile, "Before Fetch - file size: " << this->numBytes);
  kernel->synchDisk->ReadLength(start_sector, sizeof(FileHeader), (char*) this);
  DEBUG(dbgFile, "After Fetch - file size: " << this->numBytes);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::writeBack()
{
  int start_sector = firstSector();
  DEBUG(dbgFile, "Writing i-node to sector #" << start_sector);
  kernel->synchDisk->WriteLength(start_sector, sizeof(FileHeader), (char*) this);
}


//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    IndirectBlock *ib = new IndirectBlock;
    int sector_in_one_fragment = kernel->fileSystem->superBlock()->getFragmentSize()/SectorSize; 

    printf("FileHeader contents.  File size: %d.  File fragments: %d\n", numBytes, numFragments);
    printf("Fragments used by this file: \n");
    if (numFragments > NUM_DIRECT_FRAGMENTS) {
    	ib->fetchFrom(indirect_block * sector_in_one_fragment);
        int* indirect_fragments = ib->getFragments();
    	printf("Direct fragments: \n");
	for (i = 0;i < NUM_DIRECT_FRAGMENTS; ++i)
	    printf("%d ", dataFragments[i]);
	printf("\nIndirect fragment number#: ", indirect_block);
	for (i = 0;i + NUM_DIRECT_FRAGMENTS < numFragments; ++i)
	    printf("%d ", indirect_fragments[i]);	
    } else {
	for (i = 0;i < numFragments; ++i)
	    printf("%d ", dataFragments[i]);
    }

    delete ib;
}

int
FileHeader::firstSector() 
{
    int num_inodes_in_group = kernel->fileSystem->superBlock()->getNumInodeInCylinder();
    int legal_inode = id - 1; 
    
    int index = legal_inode / num_inodes_in_group;
    int offset = legal_inode % num_inodes_in_group; 

    int fragment_size = kernel->fileSystem->superBlock()->getFragmentSize();    
    int sector_in_fragment = fragment_size / SectorSize;

    int fragment_in_group = kernel->fileSystem->superBlock()->getNumFragmentInCylinder();
    int start_sector = fragment_in_group * index * sector_in_fragment + 
		kernel->fileSystem->superBlock()->getOffsetSuperBlock() * sector_in_fragment + 
		kernel->fileSystem->superBlock()->getOffsetCylinderInodeTable() * sector_in_fragment +
		offset * (128 / SectorSize);

    return start_sector;
}

void
FileHeader::setLastModified(int i)
{
    last_modified = kernel->stats->totalTicks;
}
