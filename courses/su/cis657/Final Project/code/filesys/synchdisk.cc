// synchdisk.cc 
//	Routines to synchronously access the disk.  The physical disk 
//	is an asynchronous device (disk requests return immediately, and
//	an interrupt happens later on).  This is a layer on top of
//	the disk providing a synchronous interface (requests wait until
//	the request completes).
//
//	Use a semaphore to synchronize the interrupt handlers with the
//	pending requests.  And, because the physical disk can only
//	handle one operation at a time, use a lock to enforce mutual
//	exclusion.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchdisk.h"

#include "kernel.h"


//----------------------------------------------------------------------
// SynchDisk::SynchDisk
// 	Initialize the synchronous interface to the physical disk, in turn
//	initializing the physical disk.
//
//----------------------------------------------------------------------

SynchDisk::SynchDisk()
{
    semaphore = new Semaphore("synch disk", 0);
    lock = new Lock("synch disk lock");
    disk = new Disk(this);
}

//----------------------------------------------------------------------
// SynchDisk::~SynchDisk
// 	De-allocate data structures needed for the synchronous disk
//	abstraction.
//----------------------------------------------------------------------

SynchDisk::~SynchDisk()
{
    delete disk;
    delete lock;
    delete semaphore;
}

//----------------------------------------------------------------------
// SynchDisk::ReadSector
// 	Read the contents of a disk sector into a buffer.  Return only
//	after the data has been read.
//
//	"sectorNumber" -- the disk sector to read
//	"data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::ReadSector(int sectorNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    disk->ReadRequest(sectorNumber, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

//----------------------------------------------------------------------
// SynchDisk::ReadBlock
// 	Read the contents of a disk block into a buffer.  Return only
//	after the data has been read.
//
//	"blockNumber" -- the disk block to read
//	"data" -- the buffer to hold the contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::ReadBlock(int blockNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    int count_sector = kernel->fileSystem->superBlock()->getBlockSize() / SectorSize;
    int start_sector = count_sector * blockNumber;
    for (unsigned int i = 0;i < count_sector; ++i)
    	disk->ReadRequest(start_sector + i, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

void
SynchDisk::ReadFragment(int fragmentNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    int count_sector = kernel->fileSystem->superBlock()->getFragmentSize() / SectorSize;
    int start_sector = count_sector * fragmentNumber;
    for (unsigned int i = 0;i < count_sector; ++i)
    	disk->ReadRequest(start_sector + i, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

void
SynchDisk::ReadLength(int sectorNumber, int len, char *data)
{
    int num_sectors = divRoundUp(len, SectorSize);
    char *buf = new char[num_sectors * SectorSize];
    memset(buf, '\0', num_sectors * SectorSize);
    for (unsigned int i = 0;i < num_sectors; ++i)
    	ReadSector(sectorNumber + i, buf + i * SectorSize);

    //strncpy(data, buf, len);
    bcopy(buf, data, len);
    delete[] buf;
}

//----------------------------------------------------------------------
// SynchDisk::WriteSector
// 	Write the contents of a buffer into a disk sector.  Return only
//	after the data has been written.
//
//	"sectorNumber" -- the disk sector to be written
//	"data" -- the new contents of the disk sector
//----------------------------------------------------------------------

void
SynchDisk::WriteSector(int sectorNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    disk->WriteRequest(sectorNumber, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

void
SynchDisk::WriteBlock(int blockNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    int count_sector = kernel->fileSystem->superBlock()->getBlockSize() / SectorSize;
    int start_sector = count_sector * blockNumber;
    for (unsigned int i = 0;i < count_sector; ++i)
    	disk->WriteRequest(start_sector + i, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

void
SynchDisk::WriteFragment(int fragmentNumber, char* data)
{
    lock->Acquire();			// only one disk I/O at a time
    int count_sector = kernel->fileSystem->superBlock()->getFragmentSize() / SectorSize;
    int start_sector = count_sector * fragmentNumber;
    for (unsigned int i = 0;i < count_sector; ++i)
    	disk->WriteRequest(start_sector + i, data);
    semaphore->P();			// wait for interrupt
    lock->Release();
}

void
SynchDisk::WriteLength(int sectorNumber, int len, char *data)
{
    int num_sectors = divRoundUp(len, SectorSize);
    char *buf = new char[num_sectors * SectorSize];
    memset(buf, '\0', num_sectors * SectorSize);
    //strncpy(buf, data, len);
    bcopy(data, buf, len);
    for (unsigned int i = 0;i < num_sectors; ++i)
    	WriteSector(sectorNumber + i, buf + i * SectorSize);

    delete[] buf;
}

//----------------------------------------------------------------------
// SynchDisk::CallBack
// 	Disk interrupt handler.  Wake up any thread waiting for the disk
//	request to finish.
//----------------------------------------------------------------------

void
SynchDisk::CallBack()
{ 
    semaphore->V();
}
