// directory.cc 
//	Routines to manage a directory of file names.
//
//	The directory is a table of fixed length entries; each
//	entry represents a single file, and contains the file name,
//	and the location of the file header on disk.  The fixed size
//	of each directory entry means that we have the restriction
//	of a fixed maximum size for file names.
//
//	The constructor initializes an empty directory of a certain size;
//	we use ReadFrom/WriteBack to fetch the contents of the directory
//	from disk, and to write back any modifications back to disk.
//
//	Also, this implementation has the restriction that the size
//	of the directory cannot expand.  In other words, once all the
//	entries in the directory are used, no more files can be created.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

//----------------------------------------------------------------------
// Directory::Directory
// 	Initialize a directory; initially, the directory is completely
//	empty.  If the disk is being formatted, an empty directory
//	is all we need, but otherwise, we need to call FetchFrom in order
//	to initialize it from disk.
//
//	"size" is the number of entries in the directory
//----------------------------------------------------------------------

Directory::Directory(): tableSize(0)
{
}

//----------------------------------------------------------------------
// Directory::~Directory
// 	De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{ 
    delete [] table;
} 

//----------------------------------------------------------------------
// Directory::FetchFrom
// 	Read the contents of the directory from disk.
//
//	"file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    int file_length = file->getInode()->getLength();
    tableSize = file_length / sizeof(DirectoryEntry);
    table = new DirectoryEntry[tableSize];
    (void) file->ReadAt((char *)table, file_length, 0);
}

//----------------------------------------------------------------------
// Directory::WriteBack
// 	Write any modifications to the directory back to disk
//
//	"file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    int file_length = tableSize * sizeof(DirectoryEntry);
    //this->List();
    (void) file->WriteDir((char *)table, file_length, 0);
}

//----------------------------------------------------------------------
// Directory::FindIndex
// 	Look up file name in directory, and return its location in the table of
//	directory entries.  Return -1 if the name isn't in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (!strncmp(table[i].name, name, FileNameMaxLen))
	    return i;
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
// 	Look up file name in directory, and return the disk sector number
//	where the file's header is stored. Return -1 if the name isn't 
//	in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::Find(char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].inode_number;
    return -1;
}

//----------------------------------------------------------------------
// Directory::Add
// 	Add a file into the directory.  Return TRUE if successful;
//	return FALSE if the file name is already in the directory, or if
//	the directory is completely full, and has no more space for
//	additional file names.
//
//	"name" -- the name of the file being added
//	"newSector" -- the disk sector containing the added file's header
//----------------------------------------------------------------------

bool
Directory::Add(char *name, int inode)
{ 
    if (FindIndex(name) != -1)
	return FALSE;

    DirectoryEntry *new_table = new DirectoryEntry[tableSize + 1];
    for (int i = 0;i < tableSize; ++i) {
	new_table[i].inode_number = table[i].inode_number;
        strncpy(new_table[i].name, table[i].name, strlen(table[i].name) + 1);
    }
    new_table[tableSize].inode_number = inode;
    strncpy(new_table[tableSize].name, name, strlen(name) + 1);
    if(tableSize > 0) delete[] table;
    tableSize += 1;
    table = new_table;
    
    return TRUE;
}

//----------------------------------------------------------------------
// Directory::Remove
// 	Remove a file name from the directory.  Return TRUE if successful;
//	return FALSE if the file isn't in the directory. 
//
//	"name" -- the file name to be removed
//----------------------------------------------------------------------

bool
Directory::Remove(char *name)
{ 
    int index = FindIndex(name);

    if (index == -1)
	return FALSE; 		// name not in directory

    DirectoryEntry *new_table = new DirectoryEntry[tableSize - 1];
    for (int i = 0, j = 0;i < tableSize - 1;) {
	if (index == j) {
  	    ++j;
	} else {
	    new_table[i].inode_number = table[j].inode_number;
            strncpy(new_table[i].name, table[j].name, strlen(table[j].name) + 1);
 	    ++i;
	    ++j;
	}
    }

    tableSize -= 1;
    delete[] table;
    table = new_table;
    
    return TRUE;	
}

//----------------------------------------------------------------------
// Directory::List
// 	List all the file names in the directory. 
//----------------------------------------------------------------------

void
Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	printf("%d - %s\n", table[i].inode_number, table[i].name);
}

