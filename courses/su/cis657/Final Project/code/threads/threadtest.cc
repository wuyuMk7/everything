#include "kernel.h"
#include "main.h"
#include "thread.h"

#include <stdlib.h>
#include <string.h>

#define TransferSize 1024

char int_to_char(int i)
{
    return (char)(i - 1 + '1');
}

static void
Copy(char *from, char *to)
{
    int fd;
    OpenFile* openFile;
    int amountRead, fileLength;
    char *buffer;

// Open UNIX file
    if ((fd = OpenForReadWrite(from,FALSE)) < 0) {       
        printf("Copy: couldn't open input file %s\n", from);
        return;
    }

// Figure out length of UNIX file
    Lseek(fd, 0, 2);            
    fileLength = Tell(fd);
    Lseek(fd, 0, 0);

// Create a Nachos file of the same length
    DEBUG('f', "Copying file " << from << " of size " << fileLength <<  " to file " << to);
    //if (!kernel->fileSystem->Create(to, 0)) {   // Create Nachos file
    //    printf("Copy: couldn't create output file %s\n", to);
    //    Close(fd);
    //    return;
    //}
    
    openFile = kernel->fileSystem->Open(to);
    ASSERT(openFile != NULL);
    
// Copy the data in TransferSize chunks
    buffer = new char[TransferSize];
    int ws = 0;
    while ((amountRead=ReadPartial(fd, buffer, sizeof(char)*TransferSize)) > 0) {
         ws = openFile->Write(buffer, amountRead);    
	 //cout << ws << endl << endl;
    }
    delete [] buffer;

// Close the UNIX and the Nachos files
    delete openFile;
    Close(fd);
}

void
User1(int which)
{
    kernel->flag1 = 1;
    char *username = "User1";
    cout << "Current User Thread #" << which << endl;
    OpenFile *user1File = kernel->fileSystem->Open("testfile"); 
    char *buf = new char[31]();
    int read_len = user1File->Read(buf, 30);
    user1File->Seek(0);
    if (read_len != 0)
	cout << "Current reader: " << username
	     << ", From position:" << 0
	     << ", Read Length: " << read_len
	     << ", Data: " << endl << buf << endl;

    for (int i = 0;i < 30; ++i) 
        buf[i] = '\0';
    
    for (int i = 0;i < 5; ++i) {
        user1File->Seek(0);
        read_len = user1File->Read(buf, 30);
        cout << "Read time: " << i
    	     << ", Reader: " << username 
    	     << ", Data: " << endl << buf << endl;
    }

    kernel->fileSystem->Remove("testfile");

    if(!kernel->fileSystem->Remove("testfile"));
	cout << "File is still in using. Deletion failed. From: " << username  << endl;

    cout << "After " << username << "tried to delete 'testfile'." << endl;
    kernel->fileSystem->PrintFragmentMap();

    delete user1File;
    delete[] buf;

    kernel->fileSystem->Create("testfile1", 0);
    cout << username << " created 'testfile1'" << endl;

    kernel->flag1 = 0;
    if(kernel->flag2 == 0 && kernel->flag3 == 0) {
    	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	kernel->scheduler->ReadyToRun(kernel->mainThread);
    	(void) kernel->interrupt->SetLevel(oldLevel);
    }
}

void
User2(int which)
{
    kernel->flag2 = 1;
    char *username = "User2";
    cout << "Current User Thread #" << which << endl;

    OpenFile *user2File = kernel->fileSystem->Open("testfile"); 
    user2File->Seek(20);
    char *buf = new char[31]();
    int read_len = user2File->Read(buf, 30);
    if (read_len != 0)
	cout << "Current reader: " << username
	     << ", From position:" << 20
	     << ", Read Length: " << read_len
	     << ", Data: " << endl << buf << endl;

    for (int i = 0;i < 30; ++i) 
	buf[i] = '\0';

    for (int i = 0;i < 5; ++i) {
	user2File->Seek(0);
    	read_len = user2File->Read(buf, 30);
	cout << "Read time: " << i
	     << ", Reader: " << username 
	     << ", Data: " << endl << buf << endl;
    }

    kernel->fileSystem->Remove("testfile");

    if(!kernel->fileSystem->Remove("testfile"));
	cout << "File is still in using. Deletion failed. From: " << username  << endl;

    cout << "After " << username << "tried to delete 'testfile'." << endl;
    kernel->fileSystem->PrintFragmentMap();

    delete user2File;
    delete []buf;

    kernel->fileSystem->Create("testfile2", 0);
    cout << username << " created 'testfile2'" << endl;

    kernel->flag2 = 0;
    if(kernel->flag1 == 0 && kernel->flag3 == 0) {
    	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	kernel->scheduler->ReadyToRun(kernel->mainThread);
    	(void) kernel->interrupt->SetLevel(oldLevel);
    }
}

void
User3(int which)
{
    kernel->flag3 = 1;
    char *username = "User3";
    cout << "Current User Thread #" << which << endl;

    OpenFile *user3File = kernel->fileSystem->Open("testfile"); 
    user3File->Seek(5);
    char *buf = new char[31]();
    int read_len = user3File->Read(buf, 30);
    user3File->Seek(0);
    if (read_len != 0)
	cout << "Current reader: " << username
	     << ", From position:" << 5
	     << ", Read Length: " << read_len
	     << ", Data: " << endl << buf << endl;

    char *write_buf = "*12345678*";
    user3File->Write(write_buf, 9);

    if(!kernel->fileSystem->Remove("testfile"));
	cout << "File is still in using. Deletion failed. From: " << username  << endl;

    cout << "After " << username << "tried to delete 'testfile'." << endl;
    kernel->fileSystem->PrintFragmentMap();

    delete user3File;
    delete []buf;

    kernel->fileSystem->Create("testfile3", 0);
    cout << username << " created 'testfile3'" << endl;

    kernel->flag3 = 0;
    if(kernel->flag1 == 0 && kernel->flag2 == 0) {
    	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	kernel->scheduler->ReadyToRun(kernel->mainThread);
    	(void) kernel->interrupt->SetLevel(oldLevel);
    }
}

void
ThreadTest()
{
    cout << endl;
    cout << "- Current status of the system." << endl;
    cout << "Root directory is using one fragment and one inode now." << endl;
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();
    cout << "Print current inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();
    
    cout << endl << "- Demonstration on file creation: " << endl << endl;
    cout << "List contents in the current directory (root directory):" << endl;
    kernel->fileSystem->List("/");
    cout << "Create file - testfile. " << endl;
    kernel->fileSystem->Create("testfile", 0); 
    cout << "Print current inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();
    cout << "List contents in the current directory (root directory):" << endl;
    kernel->fileSystem->List("/");

    char buf[50000];
    for (int i = 0;i < 50000; ++i) 
	buf[i] = rand() % 9 + 'a';

    cout << endl << "- Demonstration on file open, R/W, seek and space expansion: " << endl << endl;
    OpenFile *test_file = kernel->fileSystem->Open("testfile");
    cout << "Write 512 bytes data to the file 'testfile.' (1 fragment)" << endl;
    cout << "Display the data that will be written to the file: " << endl;
    for (int i = 0;i < 512; ++i) cout << buf[i];
    cout << endl;
    test_file->Write(buf, 512); 
    cout << endl << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << endl << "Read 100 bytes data from position 5 in the file 'testfile'. " << endl;
    test_file->Seek(4);
    char buf_read_test_file[101];
    buf_read_test_file[100] = '\0';
    int tmp_read_len_test_file = test_file->Read(buf_read_test_file, 100); 
    if (tmp_read_len_test_file != 0)
	cout << "From position: 5 (Seek 4)" 
	     << ", Read Length: " << tmp_read_len_test_file
	     << ", Data: " << endl << buf_read_test_file << endl;

    cout << endl << "Seek to the end of the filw and keep writing data to the file. " << endl;
    cout << "Write 3000 bytes data to the file 'testfile.' Total size: 3512 bytes. (1 block)" << endl;
    cout << "(Demonstration on expansion #3)" << endl;
    test_file->SeekToEnd();
    test_file->Write(buf, 3000);
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << "Keep writing 500 bytes data to the file 'testfile.' Total size: 4012 bytes. (1 block)" << endl;
    cout << "(Demonstration on expansion #1)" << endl;
    test_file->Write(buf, 500);
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << "Keep writing 6000 bytes data to the file 'testfile.' Total size: 10012 bytes. (2 blocks, 2 fragments)" << endl;
    cout << "(Demonstration on expansion #2)" << endl;
    test_file->Write(buf, 6000);
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << "Keep writing 11000 bytes data to the file 'testfile.' Total size: 21012 bytes. (5 blocks, 1 fragment + 1 indirect block)" << endl;
    cout << "(Demonstration on expansion #3)" << endl;
    test_file->Write(buf, 11000);
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << endl << "- Demonstartion on large file: " << endl << endl;
    cout << "Keep writing 50000 bytes data to the file 'testfile.' Total size: 71012 bytes. (17 blocks, 2 fragments)" << endl;
    test_file->Write(buf, 50000);
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();

    cout << endl << "- Demonstration on dynamic directory size" << endl;
    cout << "Create 9 directories and 15 files in the current directory (root directory):" << endl;
    char tmp_dir_name[5] = {'d', 'i', 'r', '\0', '\0'};
    char tmp_file_name[7] = {'f', 'i', 'l', 'e', '0', '\0', '\0'};
    for (int i = 1;i <= 15; ++ i) {
	if (i <= 9) {
	    tmp_file_name[5] = int_to_char(i);
	    tmp_dir_name[3] = int_to_char(i);
	    kernel->fileSystem->Create(tmp_file_name, 0);
	    kernel->fileSystem->Create(tmp_dir_name, 1);
	} else {
	    tmp_file_name[4] = int_to_char(i / 10);
	    tmp_file_name[5] = int_to_char(i % 10);
	    kernel->fileSystem->Create(tmp_file_name, 0);
	}
    }
    cout << "List contents in the current directory (root directory):" << endl;
    kernel->fileSystem->List("/");
    cout << "Print current Inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();

    cout << endl << "- Demonstration on file deletion: " << endl << endl;
    cout << "Remove files that just created: " << endl;
    tmp_file_name[4] = '0';
    for (int i = 1;i <= 15; ++ i) {
	if (i <= 9) {
	    tmp_file_name[5] = int_to_char(i);
	    tmp_dir_name[3] = int_to_char(i);
	    kernel->fileSystem->Remove(tmp_file_name);
	    kernel->fileSystem->Remove(tmp_dir_name);
	} else {
	    tmp_file_name[4] = int_to_char(i / 10);
	    tmp_file_name[5] = int_to_char(i % 10);
	    kernel->fileSystem->Remove(tmp_file_name);
	}
    }
    cout << "Print current Inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();

    cout << "Try to remove 'testfile.'" << endl;
    cout << "But the 'testfile' is still in use since its openfile object still exists. " << endl;
    cout << "This deletion should fail. " << endl;
    kernel->fileSystem->Remove("testfile");
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();
    cout << "Print current Inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();
    cout << endl << "Close 'testfile' and remove it again." << endl;
    delete test_file;
    kernel->fileSystem->Remove("testfile");
    cout << "Print current fragment free map:" << endl;
    kernel->fileSystem->PrintFragmentMap();
    cout << "Print current Inode free map:" << endl;
    kernel->fileSystem->PrintInodeMap();

    kernel->fileSystem->Create("testfile", 0);
    Copy("Makefile", "testfile");


    cout << endl << endl;
    cout << "- Demonstration on concurrent operations.";
    cout << endl << endl;


    Thread *th1 = new Thread("User1 thread");
    th1->Fork((VoidFunctionPtr)User1, (void*)1);

    Thread *th2 = new Thread("User2 thread");
    th2->Fork((VoidFunctionPtr)User2, (void*)2);

    Thread *th3 = new Thread("User3 thread");
    th3->Fork((VoidFunctionPtr)User3, (void*)3);

    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(FALSE);
    (void) kernel->interrupt->SetLevel(oldLevel);

    cout << endl << "Now in the main thread: " << endl;
    cout << "List contents in the current directory (root directory):" << endl;
    kernel->fileSystem->List("/");
    cout << "Final free inode map: " << endl;
    kernel->fileSystem->PrintInodeMap();
    cout << "Remove testfile1, testfile2, and testfile3" << endl;
    kernel->fileSystem->Remove("testfile1");
    kernel->fileSystem->Remove("testfile2");
    kernel->fileSystem->Remove("testfile3");
    cout << "Final free inode map: " << endl;
    kernel->fileSystem->PrintInodeMap();
    cout << endl << "Test finished." << endl;
    
}
