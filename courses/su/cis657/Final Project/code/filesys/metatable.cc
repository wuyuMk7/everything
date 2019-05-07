#include "metatable.h"

MetaTable::MetaTable(int size) : size(size)
{
    lock = new Lock("Meta table lock");
    table = new MetaTableItem[size];
    
    for(int i = 1;i <= size; ++i)
	table[i - 1] = MetaTableItem(i);
}

MetaTable::~MetaTable() 
{
    delete lock;
    delete[] table;
}

void 
MetaTable::acquire()
{
    lock->Acquire();
}

void 
MetaTable::release()
{
    lock->Release();
}

int 
MetaTable::findInodeIndex(int inode)
{
    for (int i = 0;i < size; ++i) {
	if (inode == table[i].inode)
	    return i;
    }
}

int 
MetaTable::getUsage(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    release();
    return table[index].usage;
}

void
MetaTable::setUsage(int inode, int usage)
{
    acquire();
    int index = findInodeIndex(inode);
    table[index].usage = usage;
    release();
}

void
MetaTable::incUsage(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    table[index].usage ++;
    release();
}

void 
MetaTable::decUsage(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    table[index].usage --;
    if (table[index].usage <= 0) {
	table[index].usage = 0;
	table[index].last_modified = 0;
    }
    release();
}

void
MetaTable::setLastModified(int inode, int last_modified)
{
    acquire();
    int index = findInodeIndex(inode);
    table[index].last_modified = last_modified;
    release();
}

bool
MetaTable::checkLastModified(int inode, int last_modified)
{
    acquire();
    int index = findInodeIndex(inode);
    release();
    return table[index].last_modified == last_modified || table[index].last_modified == 0;
}

bool
MetaTable::checkFinish(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    if (table[index].finish == 0) {
    	release();
	return FALSE;
    } else {
	if (table[index].usage <= 0) {
	    table[index].last_modified = 0;
	    table[index].finish = 0;
    	    release();
	    return TRUE;
  	}
    }
    release();
    return FALSE;
}

bool
MetaTable::tryFinish(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    if (table[index].usage <= 0) {
	table[index].last_modified = 0;
    	release();
	return TRUE;
    } else {
	table[index].finish = 1;
    	release();
	return FALSE;
    }
}

bool
MetaTable::waitingFinish(int inode)
{
    acquire();
    int index = findInodeIndex(inode);
    release();
    return table[index].finish == 1;
}

void
MetaTable::print()
{
    cout << "MetaTable:" << endl;
    for(int i = 1;i <= size; ++i) {
	print(i);
    }
}

void
MetaTable::print(int inode)
{
    acquire();
    int i = findInodeIndex(inode);
    cout << "Inode #" << inode << ", usage: " << table[i].usage
	 << ", last_modified: " << table[i].last_modified;
    if (table[i].finish == 0)
	cout << ", no deletion request now.";
    else
	cout << ", wait for deletion.";
    cout << endl;
    release();
}
