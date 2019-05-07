// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void Exit_POS(int pid)
{
  if (pid < 0 || pid > 255 || !kernel->scheduler->CheckPid(pid)) {
    cout << "(Exit_POS)Error: pid out of range or nonexistent pid." << endl;
  } else {
    // cout << "Exit_POS called by PID: " << pid << "." << endl;
    kernel->scheduler->ResetPid(pid);
    Thread *t = kernel->scheduler->FindWaiting(pid);
    if (t == NULL) {
      cout << "(Exit_POS)Error: cannot find corresponding wating parent thread." << endl;
    } else {
      kernel->scheduler->RemoveWaiting(pid);
      
      IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
      kernel->scheduler->ReadyToRun(t);
      kernel->currentThread->Finish();
      (void) kernel->interrupt->SetLevel(oldLevel);
    }
  }
}

void ForkTest1(int id)
{
	printf("ForkTest1 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest1 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void ForkTest2(int id)
{
	printf("ForkTest2 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest2 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void ForkTest3(int id)
{
	printf("ForkTest3 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest3 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case PageFaultException:
      {
	int badAddr = (int)kernel->machine->ReadRegister(BadVAddrReg);
    	DEBUG(dbgSys, "Page fault from virtual address " << badAddr << endl);
        int vpn = badAddr / PageSize;
        kernel->memoryManager->PageReplacement(&kernel->machine->pageTable[vpn]);
      }     

      return;

      break;
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;
      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;
      case SC_Write:
	{
	  DEBUG(dbgSys, "\nSC_Write is called by " << kernel->currentThread->getName() << endl);
	  int addr = (int)kernel->machine->ReadRegister(4);
	  int size = (int)kernel->machine->ReadRegister(5);

	  int ch = 0;
	  for (int i = 0;i < size; ++i) {
	    if(!kernel->machine->ReadMem(addr + i, 1, &ch))
		kernel->machine->ReadMem(addr + i, 1, &ch);
	    printf("%c", ch);
	  }

	  kernel->machine->WriteRegister(2, (int)size);
	}

	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	ASSERTNOTREACHED();

	break;
      case SC_Exit:
	{
	  int ret = (int)kernel->machine->ReadRegister(4);
	  cout << "Program exited with code " << ret << "." << endl;
	
	  kernel->currentThread->Finish();
	}

	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	ASSERTNOTREACHED();

	break;
      case SC_Fork_POS:
 	{
          int type = (int)kernel->machine->ReadRegister(4);
          VoidFunctionPtr func = NULL;

   	  switch(type) {
	  case 1:
	    func = (VoidFunctionPtr)ForkTest1;
	    break;
	  case 2:
	    func = (VoidFunctionPtr)ForkTest2;
	    break;
	  case 3:
	    func = (VoidFunctionPtr)ForkTest3;
            break;
	  default:
	    break;
          }

	  int pid = -1;
	  if (func != NULL) {
	    Thread *t = new Thread("Fork test thread");
            pid = kernel->scheduler->NextPid();
	    t->Fork(func, (void *) pid);
	  }

	  /* Prepare Result */
	  kernel->machine->WriteRegister(2, (int)pid);
	}
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	ASSERTNOTREACHED();

	break;
      case SC_Wait_POS:
	{
          int wait_pid = (int)kernel->machine->ReadRegister(4);

	  if (wait_pid < 0 || wait_pid > 255 || !kernel->scheduler->CheckPid(wait_pid)) {
	    cout << "(Wait_POS)Error: pid out of range or nonexistent pid." << endl;
          } else {
	    kernel->scheduler->AddWaiting(wait_pid, kernel->currentThread);
 	  }
	}

	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	ASSERTNOTREACHED();

	break;
      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
