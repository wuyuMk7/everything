// openfile.cc 
//	Routines to manage an open Nachos file.  As in UNIX, a
//	file must be open before we can read or write to it.
//	Once we're all done, we can close it (in Nachos, by deleting
//	the OpenFile data structure).
//
//	Also as in UNIX, for convenience, we keep the file header in
//	memory while the file is open.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#ifndef FILESYS_STUB

#include "copyright.h"
#include "main.h"
#include "filehdr.h"
#include "openfile.h"
#include "synchdisk.h"

#include "indirect.h"
#include "metatable.h"

//----------------------------------------------------------------------
// OpenFile::OpenFile
// 	Open a Nachos file for reading and writing.  Bring the file header
//	into memory while the file is open.
//
//	"sector" -- the location on disk of the file header for this file
//----------------------------------------------------------------------

OpenFile::OpenFile(int inode_number)
{ 
    hdr = new FileHeader(inode_number);
    hdr->fetchFrom();
    kernel->fileSystem->metaTable()->incUsage(inode_number);
    seekPosition = 0;
}

//----------------------------------------------------------------------
// OpenFile::~OpenFile
// 	Close a Nachos file, de-allocating any in-memory data structures.
//----------------------------------------------------------------------

OpenFile::~OpenFile()
{
    kernel->fileSystem->metaTable()->decUsage(hdr->getId());
    bool ret = kernel->fileSystem->metaTable()->checkFinish(hdr->getId());
    if (ret) kernel->fileSystem->Unlink(hdr->getId());
    delete hdr;
}

//----------------------------------------------------------------------
// OpenFile::Seek
// 	Change the current location within the open file -- the point at
//	which the next Read or Write will start from.
//
//	"position" -- the location within the file for the next Read/Write
//----------------------------------------------------------------------

void
OpenFile::Seek(int position)
{
    seekPosition = position;
}	

//----------------------------------------------------------------------
// OpenFile::SeekToEnd
// 	Change the current location to the end of the file.
//----------------------------------------------------------------------

void
OpenFile::SeekToEnd()
{
    seekPosition = hdr->getLength();
}

//----------------------------------------------------------------------
// OpenFile::Read/Write
// 	Read/write a portion of a file, starting from seekPosition.
//	Return the number of bytes actually written or read, and as a
//	side effect, increment the current position within the file.
//
//	Implemented using the more primitive ReadAt/WriteAt.
//
//	"into" -- the buffer to contain the data to be read from disk 
//	"from" -- the buffer containing the data to be written to disk 
//	"numBytes" -- the number of bytes to transfer
//----------------------------------------------------------------------

int
OpenFile::Read(char *into, int numBytes)
{
   int result = ReadAt(into, numBytes, seekPosition);
   seekPosition += result;
   return result;
}

int
OpenFile::Write(char *into, int numBytes)
{
   int result = WriteAt(into, numBytes, seekPosition);
   seekPosition += result;
   return result;
}

//----------------------------------------------------------------------
// OpenFile::ReadAt/WriteAt
// 	Read/write a portion of a file, starting at "position".
//	Return the number of bytes actually written or read, but has
//	no side effects (except that Write modifies the file, of course).
//
//	There is no guarantee the request starts or ends on an even disk sector
//	boundary; however the disk only knows how to read/write a whole disk
//	sector at a time.  Thus:
//
//	For ReadAt:
//	   We read in all of the full or partial sectors that are part of the
//	   request, but we only copy the part we are interested in.
//	For WriteAt:
//	   We must first read in any sectors that will be partially written,
//	   so that we don't overwrite the unmodified portion.  We then copy
//	   in the data that will be modified, and write back all the full
//	   or partial sectors that are part of the request.
//
//	"into" -- the buffer to contain the data to be read from disk 
//	"from" -- the buffer containing the data to be written to disk 
//	"numBytes" -- the number of bytes to transfer
//	"position" -- the offset within the file of the first byte to be
//			read/written
//----------------------------------------------------------------------

int
OpenFile::ReadAt(char *into, int numBytes, int position)
{
    //if (kernel->fileSystem->metaTable()->waitingFinish(hdr->getId()))
    //	return 0;
    if (!kernel->fileSystem->metaTable()->checkLastModified(hdr->getId(), hdr->getLastModified()))
	hdr->fetchFrom();

    int fileLength = hdr->FileLength();
    if ((numBytes <= 0) || (position >= fileLength))
    	return 0; 				// check request

    if (position + numBytes >= fileLength)
	numBytes = fileLength - position;

    IndirectBlock *ib = new IndirectBlock;
    int fragment_size = kernel->fileSystem->superBlock()->getFragmentSize();
    int position_fragment_index = divRoundDown(position, fragment_size);
    int end_point_position = position + numBytes - 1;
    int end_point_position_fragment_index = divRoundDown(end_point_position, fragment_size);
    int sector_in_one_fragment = fragment_size / SectorSize;

    int tmp_fragments[128];
    int num_fragment = hdr->getNumFragments();

    if (num_fragment > NUM_DIRECT_FRAGMENTS) {	
	ib->fetchFrom(sector_in_one_fragment * hdr->getIndirectBlock());
	for(int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i) 
	    tmp_fragments[i] = hdr->getDataFragments()[i];
        int *tmp_indirect_fragments = ib->getFragments();
	for(int i = 0;i + NUM_DIRECT_FRAGMENTS < num_fragment; ++i)
	    tmp_fragments[i + NUM_DIRECT_FRAGMENTS] = tmp_indirect_fragments[i];
    } else {
	for(int i = 0;i < num_fragment; ++i) 
	    tmp_fragments[i] = hdr->getDataFragments()[i];
    }

    DEBUG(dbgFile, "Reading " << numBytes << " bytes at " << position << " from file of length " << fileLength);

    // read in all the full and partial sectors that we need
    char *buf_data = new char[(end_point_position_fragment_index - position_fragment_index + 1) * fragment_size]();
    for (int i = position_fragment_index;i <= end_point_position_fragment_index; ++i) {
        int tmp_fragment_num = tmp_fragments[i];
	kernel->synchDisk->ReadLength(
	    tmp_fragment_num * sector_in_one_fragment,
	    fragment_size,
	    buf_data + i * fragment_size	
	);
    }

    // copy the part we want
    int position_fragment = tmp_fragments[position_fragment_index];
    bcopy(&buf_data[position - (position_fragment_index * fragment_size)], into, numBytes);

    delete ib;
    delete [] buf_data;
    return numBytes;
}

int
OpenFile::WriteAt(char *from, int numBytes, int position)
{
    //if (kernel->fileSystem->metaTable()->waitingFinish(hdr->getId()))
    //	return 0;
    if (!kernel->fileSystem->metaTable()->checkLastModified(hdr->getId(), hdr->getLastModified()))
	hdr->fetchFrom();

    DEBUG(dbgFile, "Open file - write at - length " << numBytes << "bytes, pos #" << position << endl);
    int ret;
    int fileLength = hdr->FileLength();
    int newLength = position + numBytes > fileLength ? position + numBytes : fileLength;

    IndirectBlock *ib = new IndirectBlock;
    int fragment_size = kernel->fileSystem->superBlock()->getFragmentSize();
    int position_fragment_index = divRoundDown(position, fragment_size);
    int sector_in_one_fragment = fragment_size / SectorSize;

    int tmp_old_fragments[128], tmp_new_fragments[128];
    int num_old_fragment = hdr->getNumFragments();

    if (num_old_fragment > NUM_DIRECT_FRAGMENTS) {	
	ib->fetchFrom(sector_in_one_fragment * hdr->getIndirectBlock());
	for(int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i) 
	    tmp_old_fragments[i] = hdr->getDataFragments()[i];
        int *tmp_indirect_fragments = ib->getFragments();
	for(int i = 0;i + NUM_DIRECT_FRAGMENTS < num_old_fragment; ++i)
	    tmp_old_fragments[i + NUM_DIRECT_FRAGMENTS] = tmp_indirect_fragments[i];
    } else {
	for(int i = 0;i < num_old_fragment; ++i) 
	    tmp_old_fragments[i] = hdr->getDataFragments()[i];
    }

    int num_new_fragment = 0;
    if (kernel->fileSystem->Allocate(hdr, newLength)) {
    //kernel->fileSystem->PrintFragmentMap();
	num_new_fragment = hdr->getNumFragments();
    	if (num_new_fragment > NUM_DIRECT_FRAGMENTS) {	
	    ib->fetchFrom(sector_in_one_fragment * hdr->getIndirectBlock());
	    for(int i = 0;i < NUM_DIRECT_FRAGMENTS; ++i) 
	        tmp_new_fragments[i] = hdr->getDataFragments()[i];
            int *tmp_indirect_fragments = ib->getFragments();
	    for(int i = 0;i + NUM_DIRECT_FRAGMENTS < num_new_fragment; ++i)
	        tmp_new_fragments[i + NUM_DIRECT_FRAGMENTS] = tmp_indirect_fragments[i];
        } else {
	    for(int i = 0;i < num_new_fragment; ++i) 
	        tmp_new_fragments[i] = hdr->getDataFragments()[i];
        }

	//for (int i = 0;fileLength > 0 && i <= position_fragment_index; ++i) {
	for (int i = 0;fileLength > 0 && i < position_fragment_index; ++i) {
	    if(tmp_old_fragments[i] == tmp_new_fragments[i]) {
		continue;
	    } else {
		char *buf_data = new char[fragment_size];
		int old_sector_number = sector_in_one_fragment * tmp_old_fragments[i];
		int new_sector_number = sector_in_one_fragment * tmp_new_fragments[i];
//cout << position_fragment_index << "&" << old_sector_number << "/" << new_sector_number << endl;
//cout << tmp_old_fragments[i] << ":" << tmp_new_fragments[i] << endl;
		kernel->synchDisk->ReadLength(old_sector_number, fragment_size, buf_data);
		kernel->synchDisk->WriteLength(new_sector_number, fragment_size, buf_data);
		delete[] buf_data;
	    }
	}

	char *buf_data = new char[(num_new_fragment - position_fragment_index) * fragment_size]();
	int position_fragment = fileLength > 0 ? tmp_old_fragments[position_fragment_index] : 0;

	for (int i = 0;i + position_fragment_index < num_old_fragment; ++i) {
	    int tmp_fragment_num = tmp_old_fragments[i + position_fragment_index];
	    kernel->synchDisk->ReadLength(
		tmp_fragment_num * sector_in_one_fragment,
		fragment_size,
		buf_data + i * fragment_size	
	    );
	}
    	bcopy(from, &buf_data[position - (position_fragment_index * fragment_size)], numBytes);
	for (int i = 0;i + position_fragment_index < num_new_fragment; ++i) {
	    int tmp_fragment_num = tmp_new_fragments[i + position_fragment_index];
	    kernel->synchDisk->WriteLength(
		tmp_fragment_num * sector_in_one_fragment,
		fragment_size,
		buf_data + i * fragment_size
	    );
	}
	delete[] buf_data;
	// Check if the inode has been updated
	hdr->setLastModified(time(NULL));
	hdr->writeBack();
        kernel->fileSystem->metaTable()->setLastModified(hdr->getId(), hdr->getLastModified());
	ret = numBytes;
    } else {
	ret = 0;
    }

    delete ib;
    return ret;
}

int
OpenFile::WriteDir(char *from, int numBytes, int position) 
{
    int fileLength = hdr->FileLength();
    int newLength = position + numBytes;
    int ret = 0;

    if (newLength > fileLength) {
	ret = WriteAt(from, numBytes, position);
    } else {
        //if (kernel->fileSystem->metaTable()->waitingFinish(hdr->getId()))
    	//    return 0;
        if (!kernel->fileSystem->metaTable()->checkLastModified(hdr->getId(), hdr->getLastModified()))
    	    hdr->fetchFrom();

	int fragment_size = kernel->fileSystem->superBlock()->getFragmentSize();
	int sector_in_one_fragment = fragment_size / SectorSize;
	int num_new_fragment = divRoundUp(newLength, fragment_size);
	int *tmp_new_fragments = hdr->getDataFragments();
	char *buf_data = new char[num_new_fragment * fragment_size]();
    	bcopy(from, buf_data, numBytes);
	for (int i = 0;i < num_new_fragment; ++i) {
	    int tmp_fragment_num = tmp_new_fragments[i];
	    kernel->synchDisk->WriteLength(
		tmp_fragment_num * sector_in_one_fragment,
		fragment_size,
		buf_data + i * fragment_size
	    );
	}
	delete[] buf_data;
	hdr->setLength(newLength);
	hdr->setNumFragments(num_new_fragment);
	hdr->setLastModified(time(NULL));
	hdr->writeBack();
        kernel->fileSystem->metaTable()->setLastModified(hdr->getId(), hdr->getLastModified());
	ret = numBytes;
    }

    return ret;
}

//----------------------------------------------------------------------
// OpenFile::Length
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
OpenFile::Length() 
{ 
    return hdr->FileLength(); 
}

#endif //FILESYS_STUB
