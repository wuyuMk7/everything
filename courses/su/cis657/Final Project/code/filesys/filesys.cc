// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#ifndef FILESYS_STUB

#include "copyright.h"
#include "debug.h"
#include "disk.h"
#include "pbitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"

#include "utility.h"

#include "cylindersummary.h"
#include "cylinderfreemap.h"
#include "indirect.h"

#include "fslocks.h"
#include "metatable.h"

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)

//*********************//
//    MAX SIZE 97280
//*********************//

//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{ 
    fs_control_lock = new FsControlLock("FS_CONTROL_LOCK");

    DEBUG(dbgFile, "Initializing the file system.");
    cout << "Initializing the file system." << endl;
    cout << "Notice: the system is using inode 1 as the first inode." << endl
	 << "This inode occupies pos#0 in the inode free map from first cylinder group." << endl;
    if (format) {
	int disk_data_size = NumSectors * SectorSize;
	int block_count = disk_data_size / BLOCK_SIZE;
	int fragment_in_one_block = BLOCK_SIZE / FRAGMENT_SIZE;
	int fragment_count = fragment_in_one_block * block_count;
	int inode_count = disk_data_size / INODE_RANGE;

	cout << "Formatting the file system." << endl
	     << "Cylinder groups: " << CYLINDER_GROUPS << endl
	     << "Block size: " << BLOCK_SIZE << endl
	     << "Fragment size: " << FRAGMENT_SIZE << endl
	     << "Total number of inode: " << inode_count << endl;

	meta_table = new MetaTable(inode_count);

	// Padding Super Block
	super_block = new SuperBlock(
		disk_data_size, BLOCK_SIZE, block_count, FRAGMENT_SIZE, 
		fragment_count, inode_count, CYLINDER_GROUPS, NumTracks / CYLINDER_GROUPS, 
		0 * fragment_in_one_block, 
		1 * fragment_in_one_block, 
		2 * fragment_in_one_block, 
		2 * fragment_in_one_block + fragment_in_one_block / 2,
		3 * fragment_in_one_block,
		4 * fragment_in_one_block,
		inode_count / CYLINDER_GROUPS,
		fragment_count / CYLINDER_GROUPS);

	// Write Back
        DEBUG(dbgFile, "Formatting the file system.");
        DEBUG(dbgFile, "Writing control blocks.");
	for (int i = 0;i < CYLINDER_GROUPS; ++i) {
 	    int start_fragment = (fragment_count / CYLINDER_GROUPS) * i;
	    int control_block_length = super_block->getOffsetCylinderDataBlock();
	    int offset_super_block = super_block->getOffsetSuperBlock() * i;
	    start_fragment += offset_super_block;
	    int offset_summary = super_block->getOffsetEachCylinderSummary();
	    int offset_block_map = super_block->getOffsetCylinderFreeBlockList();
	    int offset_inode_map = super_block->getOffsetCylinderFreeInodeList();
	    int offset_inode_table = super_block->getOffsetCylinderInodeTable();
	    int sector_in_fragment = FRAGMENT_SIZE / SectorSize;

	    CylinderSummary cSummary(block_count / CYLINDER_GROUPS - 4, inode_count / CYLINDER_GROUPS, fragment_count / CYLINDER_GROUPS - 4 * fragment_in_one_block);
	    CylinderFreemap cFreemap(inode_count / CYLINDER_GROUPS, fragment_count / CYLINDER_GROUPS);
	    for (int i = 0;i < control_block_length; ++i) 
	    	cFreemap.getFreeFragmentMap()->Mark(i + offset_super_block);

	    // Write back
	    super_block->writeBack(start_fragment * sector_in_fragment);
 	    cSummary.writeBack((start_fragment + offset_summary) * sector_in_fragment);	
	    cFreemap.writeBack((start_fragment + offset_block_map) * sector_in_fragment,
	    		(start_fragment + offset_inode_map) * sector_in_fragment);
	}
    } else {
	super_block = new SuperBlock();
	super_block->fetchFrom(0);
	meta_table = new MetaTable(super_block->getInodeCount());
    }
}

bool
FileSystem::CreateRoot()
{
    DEBUG(dbgFile, "Creating root directory.");
    Directory *root = new Directory();
    //Directory *root_file;

    int fragment_count = super_block->getFragmentCount();
    int inode_count = super_block->getInodeCount();
    int offset_summary = super_block->getOffsetEachCylinderSummary();
    int offset_block_map = super_block->getOffsetCylinderFreeBlockList();
    int offset_inode_map = super_block->getOffsetCylinderFreeInodeList();
    int sector_in_fragment = FRAGMENT_SIZE / SectorSize;

    fs_control_lock->Acquire();
    CylinderSummary *cs = new CylinderSummary();
    cs->fetchFrom(offset_summary * sector_in_fragment);
    cs->decInode();
    cs->decBlock();
    cs->decFragment();

    CylinderFreemap *cfm = new CylinderFreemap(inode_count / CYLINDER_GROUPS, fragment_count / CYLINDER_GROUPS);
    cfm->fetchFrom(offset_block_map * sector_in_fragment, offset_inode_map * sector_in_fragment);

    int inode_number = cfm->getFreeInodeMap()->FindAndSet() + 1;
    FileHeader *hdr = new FileHeader(inode_number);
    hdr->setType(1);
    hdr->setLinkCount(2);
    hdr->setLength(0);
    hdr->setNumFragments(0);

    // Write Back
    cs->writeBack(offset_summary * sector_in_fragment);
    cfm->writeBack(offset_block_map * sector_in_fragment, offset_inode_map * sector_in_fragment);
    hdr->writeBack();

    // Create directory file
    OpenFile *root_file = new OpenFile(inode_number);
    root->Add(".", inode_number);
    root->Add("..", inode_number);
    root->WriteBack(root_file);

    fs_control_lock->Release();
    delete root_file;

    delete cs;
    delete cfm;
    delete hdr;
}

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

// type 0 - file, type 1 - directory
bool
FileSystem::Create(char *name, int type)
{
    DEBUG(dbgFile, "Creating file: " << name << ", type: " << type);

    int cur_directory_inode = kernel->currentThread->currentDir;
    bool success = FALSE;

    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;

    fs_control_lock->Acquire();

    CylinderFreemap *cfm; 
    int new_inode = -1;
    int pos_inode_map = 0;
    int pos_fragment_map = 0;
    for(int i = 0; new_inode == -1 && i < CYLINDER_GROUPS; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
        pos_inode_map = (start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment;
	pos_fragment_map = (start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment;
	cfm = new CylinderFreemap(super_block->getNumInodeInCylinder(), fragment_in_one_cylinder);
	cfm->fetchFrom(pos_inode_map, pos_fragment_map);
	if (new_inode = -1) {
	    int empty = cfm->getFreeInodeMap()->FindAndSet(); 
	    if (empty != -1) {
		new_inode = empty + 1;
		break;
	    }
        }
    }

    if (new_inode != -1) {
    	FileHeader *hdr = new FileHeader(new_inode);
    	hdr->setType(type);
    	hdr->setLinkCount(2);
    	hdr->setLength(0);
    	hdr->setNumFragments(0);
    	hdr->writeBack();

	if (type == 1) {
	    Directory *new_directory = new Directory();
	    OpenFile *new_directory_file = new OpenFile(cur_directory_inode);
            new_directory->FetchFrom(new_directory_file);
            new_directory->Add(".", new_inode);
            new_directory->Add("..", cur_directory_inode);
	    new_directory->WriteBack(new_directory_file);

 	    delete new_directory;
	    delete new_directory_file;
 	}

    	Directory *dir = new Directory();
    	OpenFile *dir_file = new OpenFile(cur_directory_inode);
    	dir->FetchFrom(dir_file);
	if(dir->Add(name, new_inode)) {
	    dir->WriteBack(dir_file);
	    cfm->writeBack(pos_inode_map, pos_fragment_map);
        }

	delete cfm;
	delete hdr;
    	delete dir;
    	delete dir_file;
    }

    if (new_inode != -1)
    	success = TRUE;
    fs_control_lock->Release();

    return success;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

OpenFile *
FileSystem::Open(char *name)
{ 
    int cur_directory_inode = kernel->currentThread->currentDir;
    int inode = -1;

    if (strcmp("/", name) == 0) {
	inode = 1;
    } else {
        Directory *dir = new Directory();
        OpenFile *dir_file = new OpenFile(cur_directory_inode);

        DEBUG(dbgFile, "Opening file " << name);
        dir->FetchFrom(dir_file);
        inode = dir->Find(name); 

	if(meta_table->waitingFinish(inode))
	    return NULL;

        delete dir;
        delete dir_file;
    }

    if (inode != -1) {
        DEBUG(dbgFile, "Opening file " << name << " - inode#" << inode);
	return new OpenFile(inode);
    }
    else
	return NULL;
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool
FileSystem::Unlink(int inode) 
{
    bool success = FALSE;

    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;
    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();
    
    int cylinder_id = inode / (super_block->getNumInodeInCylinder());
    int cylinder_count = CYLINDER_GROUPS;

    CylinderFreemap **cf = new CylinderFreemap*[cylinder_count];
    for (int i = 0;i < cylinder_count; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
	cf[i] = new CylinderFreemap(super_block->getNumInodeInCylinder(), fragment_in_one_cylinder);
	cf[i]->fetchFrom((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		(start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
    }

    FileHeader *hdr = new FileHeader(inode);
    hdr->fetchFrom();
    int num_fragment = hdr->getNumFragments();
    int *inode_fragments = hdr->getDataFragments();
    int tmp_fragments[128];
    IndirectBlock *ib;
    if (num_fragment > NUM_DIRECT_FRAGMENTS) {
	for (int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i) {
	    tmp_fragments[i] = inode_fragments[i];
	}
        ib = new IndirectBlock; 
        ib->fetchFrom(sector_in_one_fragment * hdr->getIndirectBlock());
        int *indirect_fragments = ib->getFragments(); 
        for (int i = 0;i + NUM_DIRECT_FRAGMENTS < num_fragment; ++i)	
	    tmp_fragments[i + NUM_DIRECT_FRAGMENTS] = indirect_fragments[i];

        cf[hdr->getIndirectBlock() / fragment_in_one_cylinder]->getFreeFragmentMap()->Clear(hdr->getIndirectBlock());
        delete ib;
    } else {
	for (int i = 0;i < num_fragment; ++i)
	    tmp_fragments[i] = inode_fragments[i];
    }

    // Clear direct fragments and indirect fragments
    for (int i = 0;i < num_fragment; ++i) {
	int tmp_cur_fragment = tmp_fragments[i] % fragment_in_one_cylinder;
        int tmp_cur_id = tmp_fragments[i] / fragment_in_one_cylinder;
        cf[tmp_cur_id]->getFreeFragmentMap()->Clear(tmp_cur_fragment);
    }

    // Clear Inode
    int inode_in_one_cylinder = super_block->getNumInodeInCylinder();
    cf[inode / inode_in_one_cylinder]->getFreeInodeMap()->Clear(hdr->getId() - cylinder_id * inode_in_one_cylinder - 1);
 
    // Write back map
    for (int i = 0;i < cylinder_count; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
	cf[i]->writeBack((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		(start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
    }

    delete hdr;
    delete[] cf;

    success = TRUE;

    return success;
}

bool
FileSystem::Remove(char *name)
{ 
    DEBUG(dbgFile, "Removing file: " << name);

    int cur_directory_inode = kernel->currentThread->currentDir;
    bool success = FALSE;

    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;

    fs_control_lock->Acquire();

    Directory *dir = new Directory();
    OpenFile *dir_file = new OpenFile(cur_directory_inode);
    dir->FetchFrom(dir_file);
    int inode = dir->Find(name);

    if (inode != -1) {
        dir->Remove(name);
        dir->WriteBack(dir_file);

	if (meta_table->tryFinish(inode))
	    success = Unlink(inode);
    }

    fs_control_lock->Release();

    delete dir;
    delete dir_file;
    return success;
} 

bool
FileSystem::ChangeDirectory(char *name)
{
    DEBUG(dbgFile, "Change directory to: " << name);

    int cur_directory_inode = kernel->currentThread->currentDir;
    bool success = FALSE;

    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;

    fs_control_lock->Acquire();

    Directory *dir = new Directory();
    OpenFile *dir_file = new OpenFile(cur_directory_inode);
    dir->FetchFrom(dir_file);
    int inode = dir->Find(name);

    if (inode != -1) {
	kernel->currentThread->currentDir = inode;
	success = TRUE;
    }

    fs_control_lock->Release();
    delete dir;
    delete dir_file;
    return success;

}

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List(char* file_name)
{
    OpenFile *openFile = Open(file_name);

    int type = openFile->getInode()->getType();
    if (type == 0) {
	printf("Inode # %d - %s - %d Byte", openFile->getInode()->getId(), file_name, openFile->getInode()->getLength());
    } else if (type == 1) {
    	Directory *dir = new Directory();
    	dir->FetchFrom(openFile);
    	dir->List();
    	delete dir; 
    }
   
    delete openFile;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
} 

bool
FileSystem::Allocate(FileHeader *inode, int new_length)
{
    DEBUG(dbgFile, "Allocating space for file inode #" << inode->getId());

    IndirectBlock *ib = new IndirectBlock; 
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;
    int fragment_in_one_block = BLOCK_SIZE / FRAGMENT_SIZE; 
    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();
    int cylinder_count = super_block->getCylinderGroupsCount(); 

    int tmp_old_fragments[128] = {0};
    int tmp_new_fragments[128] = {0};

    CylinderSummary **cs = new CylinderSummary*[cylinder_count];
    CylinderFreemap **cf = new CylinderFreemap*[cylinder_count];
    for (int i = 0;i < cylinder_count; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
	cs[i] = new CylinderSummary();
	cf[i] = new CylinderFreemap(super_block->getNumInodeInCylinder(), fragment_in_one_cylinder);
 	cs[i]->fetchFrom((start_fragment + super_block->getOffsetEachCylinderSummary()) * sector_in_one_fragment);	
	cf[i]->fetchFrom((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		(start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
    }

    int old_length = inode->getLength();
    int num_old_fragment = inode->getNumFragments();
    int num_new_fragment = new_length / FRAGMENT_SIZE;
    if (new_length % FRAGMENT_SIZE != 0)
	num_new_fragment += 1;

    int *tmp_f = inode->getDataFragments();
    if (num_old_fragment > NUM_DIRECT_FRAGMENTS) {
	for (int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i)
	    tmp_old_fragments[i] = tmp_f[i];

	int indirect_fragment = inode->getIndirectBlock();
 	ib->fetchFrom(sector_in_one_fragment * indirect_fragment);

	int *indirect_fragments = ib->getFragments();
	for(int i = 0; i + NUM_DIRECT_FRAGMENTS < num_old_fragment; ++i)
	    tmp_old_fragments[i + NUM_DIRECT_FRAGMENTS] = indirect_fragments[i];
    } else {
	for (int i = 0;i < num_old_fragment; ++i)
	    tmp_old_fragments[i] = tmp_f[i];
	
	if (num_new_fragment > NUM_DIRECT_FRAGMENTS) {
	    int new_indirect_id = -1;
	    for (int i = 0;i < CYLINDER_GROUPS; ++i) {
		new_indirect_id = cf[i]->getFreeFragmentMap()->FindAndSet();
		if (new_indirect_id != -1) break;
	    }
	    if (new_indirect_id == -1) return false;
	    else inode->setIndirectBlock(new_indirect_id);
	}
    }

    int last_fragment = tmp_old_fragments[num_old_fragment-1];
    int cylinder = last_fragment / fragment_in_one_cylinder;
	
    bool success = false;

    int num_new_block = num_new_fragment / fragment_in_one_block;
    int num_new_extra_fragment = num_new_fragment % fragment_in_one_block;
    int num_old_block = num_old_fragment / fragment_in_one_block;
    int num_old_extra_fragment = num_old_fragment % fragment_in_one_block;
  
    if (num_new_fragment < num_old_fragment) {
	inode->setLength(new_length);
	success = true;
    } else if (num_new_fragment > num_old_fragment){
	int alloc_remove_fragments[128];
	int alloc_add_fragments[128]; 
	int alloc_remove_len = 0;
	int alloc_add_len = 0;

	int last_in_cylinder = cylinder;
	if (num_old_extra_fragment != 0) {
	    for (int i = 1;i <= num_old_extra_fragment; ++i) {
	        cf[last_in_cylinder]->getFreeFragmentMap()->Clear(tmp_old_fragments[num_old_fragment-i] % fragment_in_one_cylinder);
		alloc_remove_fragments[alloc_remove_len] = tmp_old_fragments[num_old_fragment-i];
	   	alloc_remove_len ++; 
	    }
	    if (num_old_block != 0)
		last_in_cylinder = tmp_old_fragments[num_old_fragment-alloc_remove_len] / fragment_in_one_cylinder;
	}
	int rest_blocks = num_new_block - num_old_block;
	int cur_last_in_cylinder = last_in_cylinder;
	for (int i = 0;rest_blocks > 0 && i < CYLINDER_GROUPS; ++i) {
	    int cur_cylinder = (i + last_in_cylinder) % CYLINDER_GROUPS;
	    int ret_fragment_len = 0;
	    int ret_fragment[8];
	    do {
		for (int j = 0;j < 8; ++j) 
		    ret_fragment[j] = 0;
	    	NextAlignedFragments(fragment_in_one_cylinder, cf[cur_cylinder]->getFreeFragmentMap(), BLOCK_SIZE / FRAGMENT_SIZE, BLOCK_SIZE / FRAGMENT_SIZE, ret_fragment, &ret_fragment_len);
		if (ret_fragment_len != 0) {
		    --rest_blocks;
		    for (int j = 0;j < ret_fragment_len; ++j) {
			alloc_add_fragments[j + alloc_add_len] = ret_fragment[j] + cur_cylinder * fragment_in_one_cylinder;
			cf[cur_cylinder]->getFreeFragmentMap()->Mark(ret_fragment[j]);
		    }
		    alloc_add_len += ret_fragment_len;
		    cur_last_in_cylinder = cur_cylinder;
		}
	    } while(rest_blocks > 0 && ret_fragment_len > 0);
	}
	if (rest_blocks == 0) {
	        //cf[0]->getFreeFragmentMap()->Print();
	    int rest_fragments = num_new_extra_fragment;
	    for (int i = 0; rest_fragments > 0 && i < CYLINDER_GROUPS; ++i) {
	        int cur_cylinder = (i + last_in_cylinder) % CYLINDER_GROUPS;
	    	int ret_fragment_len = 0;
		int ret_fragment_first = -1;
	
	        do {
	            ret_fragment_first = NextPlaceForSmallFragments(fragment_in_one_cylinder, cf[cur_cylinder]->getFreeFragmentMap(), BLOCK_SIZE / FRAGMENT_SIZE, rest_fragments,  &ret_fragment_len);
	            if (ret_fragment_first != -1) {
			if (ret_fragment_len > rest_fragments) ret_fragment_len = rest_fragments;
			rest_fragments = 0;
	                for (int j = 0;j < ret_fragment_len; ++j) {
	            	    alloc_add_fragments[j + alloc_add_len] = ret_fragment_first + j + cur_cylinder * fragment_in_one_cylinder;
			    cf[cur_cylinder]->getFreeFragmentMap()->Mark(ret_fragment_first + j);
			}
	                alloc_add_len += ret_fragment_len;
	                cur_last_in_cylinder = cur_cylinder;
	            }
	        } while(rest_fragments > 0 && ret_fragment_first != -1);
	        //cf[0]->getFreeFragmentMap()->Print();
	    }
	    if (rest_fragments == 0) {
		for (int m = 0;m < num_old_fragment - alloc_remove_len; ++m)
		    tmp_new_fragments[m] = tmp_old_fragments[m];
		for (int m = 0;m < alloc_add_len; ++m)
		    tmp_new_fragments[m + num_old_fragment - alloc_remove_len] = alloc_add_fragments[m];

		inode->setLength(new_length);
		inode->setNumFragments(num_new_fragment);
		if (num_new_fragment > NUM_DIRECT_FRAGMENTS) {
		    inode->setDataFragments(tmp_new_fragments, NUM_DIRECT_FRAGMENTS);    	
		    ib->setFragments(tmp_new_fragments + NUM_DIRECT_FRAGMENTS, num_new_fragment - NUM_DIRECT_FRAGMENTS);
		    ib->writeBack(sector_in_one_fragment * inode->getIndirectBlock());
		} else {
		    inode->setDataFragments(tmp_new_fragments, num_new_fragment);			
		}

    		// cout << endl << "aereweageg" << endl << endl;
    		for (int m = 0;m < cylinder_count; ++m) {
    		    int start_fragment = m * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
 		    cs[m]->writeBack((start_fragment + super_block->getOffsetEachCylinderSummary()) * sector_in_one_fragment);	
		    cf[m]->writeBack((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		        (start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
    		}
		success = true;
	    } else {
	        success = false;
	    }
	} else {
	    success = false;
	}
    } else if (num_new_fragment == num_old_fragment) {
	inode->setLength(new_length);
	success = true;
    }

    delete ib;
    delete[] cf;
    delete[] cs;	
    return success;
}

int
FileSystem::NextPlaceForSmallFragments(int bitmap_len, PersistentBitmap *bitmap, int window, int to_align, int *min)
{
  *min = bitmap_len;
  int minpos = -1;
  int last = 0;
  int size = to_align;
  for (int i = 0; i <= bitmap_len; i++) {
    if ((i == bitmap_len) || (bitmap->Test(i))) {
      if (i - last >= size) {
        if (i / window == last / window) {
          if (i - last < *min) {
            *min = i - last;
            minpos = last;
          }
        } else if (last % window <= window - size) {
          if (window - last % window < *min) {
            *min = window - last % window;
            minpos = last;
          }
        } else if (i % window >= size) {
          if (i % window < *min) {
            *min = i % window;
            minpos = i / window * window;
          }
        } else if (i / window - last / window > 1){
          *min = window;
          minpos = (last / window + 1) * window;
        }
      }
      if (*min == size) {
        return minpos;
      }
      last = i + 1;
    }
  }
  return minpos;
}

void
FileSystem::NextAlignedFragments(int bitmap_len, PersistentBitmap *bitmap, int window, int to_align, int *ret, int *ret_len)
{
    int i = 0;
    *ret_len = 0;
    while(i + window <= bitmap_len) {
	for (int j = i;j + to_align <= i + window; ++j) {
	    bool available = true;
	    for (int k = 0; k < to_align; ++k) {
		if (bitmap->Test(j + k)) {
		    available = false;
	            break;
		}
	    }

	    if (available) {
	        for (int k = 0; k < to_align; ++k) {
		    ret[k] = j + k;
		}
		*ret_len = to_align;
		return;
	    }
	}
	i += window;
    }
}

void
FileSystem::PrintInodeMap() 
{
    int cylinder_count = CYLINDER_GROUPS;
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;
    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();

    for (int i = 0;i < cylinder_count; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
	CylinderFreemap cfm(super_block->getNumInodeInCylinder(), fragment_in_one_cylinder);
	cfm.fetchFrom((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		(start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
	cout << "Inodes map in Cylinder#" << i << endl;
	cfm.getFreeInodeMap()->Print();
    }
}

void
FileSystem::PrintFragmentMap() 
{
    int cylinder_count = CYLINDER_GROUPS;
    int sector_in_one_fragment = FRAGMENT_SIZE / SectorSize;
    int fragment_in_one_cylinder = super_block->getNumFragmentInCylinder();

    for (int i = 0;i < cylinder_count; ++i) {
 	int start_fragment = i * (fragment_in_one_cylinder + super_block->getOffsetSuperBlock());
	CylinderFreemap cfm(super_block->getNumInodeInCylinder(), fragment_in_one_cylinder);
	cfm.fetchFrom((start_fragment + super_block->getOffsetCylinderFreeBlockList()) * sector_in_one_fragment,
		(start_fragment + super_block->getOffsetCylinderFreeInodeList()) * sector_in_one_fragment);
	cout << "Fragments map in Cylinder#" << i << endl;
	cfm.getFreeFragmentMap()->Print();
    }

}

void 
FileSystem::PrintMetaTable()
{
    meta_table->print();
}

void 
FileSystem::PrintMetaTableItem(int inode)
{
    meta_table->print(inode);
}
#endif // FILESYS_STUB
