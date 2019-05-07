#include "kernel.h"
#include "main.h"
#include "thread.h"

#define TransferSize 1024

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        kernel->currentThread->Yield();
    }
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
	 cout << ws << endl << endl;
    }
    delete [] buffer;

// Close the UNIX and the Nachos files
    delete openFile;
    Close(fd);
}

void
ThreadTest()
{
    //Thread *t = new Thread("forked thread");
    //t->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    //
    //SimpleThread(0);

    int crret = kernel->fileSystem->Create("testfile", 0); 
    //cout << crret << endl;
    //OpenFile *t = kernel->fileSystem->Open("testfile");
    //int ret = 0;
    //ret = t->Write("hello!", 6);
    
    kernel->fileSystem->List("/");
    //Copy("test.file2", "testfile");

    //char r[7];
    //t->Seek(0);
    //ret = t->Read(r, 6);
    //cout << r << endl;
    //char r[100000];
    
    //OpenFile *t = kernel->fileSystem->Open("testfile");
    //int ret = t->Read(r, 100000);
    //cout << r << endl;

    kernel->fileSystem->Create("testfile1", 0);
    kernel->fileSystem->Create("testfile2", 0);
    kernel->fileSystem->Create("testfile3", 0);
    kernel->fileSystem->Create("testfile4", 0);
    kernel->fileSystem->Create("testfile5", 0);
    kernel->fileSystem->Create("testfile6", 0);
    cout << "After creation" << endl;
    kernel->fileSystem->List("/");

    kernel->fileSystem->PrintInodeMap(); 

    OpenFile *testfile1 = kernel->fileSystem->Open("testfile1");	
    testfile1->Write("1234567", 7);
    kernel->fileSystem->PrintFragmentMap();

    kernel->fileSystem->Remove("testfile1");
     
    cout << "After deletion" << endl;
    kernel->fileSystem->List("/");
    kernel->fileSystem->PrintInodeMap(); 
    kernel->fileSystem->PrintFragmentMap();
}
