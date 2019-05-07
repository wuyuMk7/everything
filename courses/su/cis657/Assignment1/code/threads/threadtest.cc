#include "kernel.h"
#include "main.h"
#include "thread.h"
#include "process.h"

void
InnerProcess(int which)
{
    printf("*** Message from Inner Process %d \n", kernel->currentProcess->GetPid());
    kernel->currentProcess->Terminate();
}

void
SimpleThread(int which)
{
    cout << "--- Enter simple thread function. Current process: #"
	 << kernel->currentProcess->GetPid() << endl;
    printf("--- Owner process: %d, thread id: %d.\n", kernel->currentProcess->GetPid(), which);

    Process *p = new Process(3, "forked inner process");
    p->Fork((VoidFunctionPtr) InnerProcess, (void *) 5);
}

void
SimpleProcess(int which)
{
    int num;

    cout << "*** Enter simple process function. Current process: #"
         << kernel->currentProcess->GetPid() << endl;
    for (num = 0; num < which; num++) {
        printf("*** Simple process from process %d looped %d times\n",
	       kernel->currentProcess->GetPid(), num);

        char name[20] = "Test thread #";
 	name[13] = '0' + num;
	name[14] = '\0';

	Thread *t = new Thread(name);
	t->Fork((VoidFunctionPtr) SimpleThread, (void *) num);
    }
    ProcessPrint(kernel->currentProcess);
    
    kernel->currentProcess->currentThread()->Yield();
    kernel->currentProcess->Terminate();
}

void
ThreadTest()
{
    //kernel->currentProcess->Join();
  
    Process *p1 = new Process(4, "First created process in thread test");
    p1->Fork((VoidFunctionPtr) SimpleProcess, (void *) 2);
    ProcessPrint(kernel->currentProcess);
    
    Process *p2 = new Process(5, "forked process");
    p2->Fork((VoidFunctionPtr) SimpleProcess, (void *) 3);
    p2->Join();
    ProcessPrint(kernel->currentProcess);

    p1->CloneToOwnChild();
    ProcessPrint(p1);
    ProcessPrint(kernel->currentProcess);

    kernel->currentProcess->Yield();

    ProcessPrint(kernel->currentProcess);
    cout << "Finish threadtest function." << endl;
}
