// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

Scheduler::Scheduler()
{ 
    readyList = new List<Thread *>; 
    toBeDestroyed = NULL;
} 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    delete readyList; 
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRun (Thread *thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    thread->setStatus(READY);
    readyList->Append(thread);
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyList->IsEmpty()) {
	return NULL;
    } else {
    	return readyList->RemoveFront();
    }
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentProcess->currentThread();
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current thread
         ASSERT(toBeDestroyed == NULL);
	 toBeDestroyed = oldThread;
    }
    
    if (oldThread->space != NULL) {	// if this thread is a user program,
        oldThread->SaveUserState(); 	// save the user's CPU registers
	oldThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    kernel->currentProcess->setCurrentThread(nextThread);  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running

    cout << "$ (Process#" << oldThread->owner()->GetPid();
    cout << " Scheduler) Swithcing from: P";
    cout << oldThread->owner()->GetPid() << '/';
    cout << oldThread->getName();
    cout << " to P";
    cout << nextThread->owner()->GetPid() << '/';
    cout << nextThread->getName();
    cout << '\n';
    
/*
    DEBUG(
	dbgThread, 
	"Switching from: P" << oldThread->owner()->GetPid() << "/" << oldThread->getName() << 
	" to: P" << nextThread->owner()->GetPid() << "/" << nextThread->getName() 
    );
*/
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up
    
    if (oldThread->space != NULL) {	    // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
	oldThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
	toBeDestroyed = NULL;
    }
}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList->Apply(ThreadPrint);
}

//----------------------------------------------------------------------
// PriorityScheduler::PriorityScheduler
// 	Initialize the sorted list of ready but not running processes.
//	Initially, no ready processes.
//----------------------------------------------------------------------

PriorityScheduler::PriorityScheduler(int (*comp)(Process*, Process*))
	:_comp(comp)
{ 
    readyList = new SortedList<Process *>(comp); 
    toBeDestroyed = NULL;
} 

//----------------------------------------------------------------------
// PriorityScheduler::~PriorityScheduler
// 	De-allocate the list of ready processes.
//----------------------------------------------------------------------

PriorityScheduler::~PriorityScheduler()
{ 
    delete readyList; 
} 

//----------------------------------------------------------------------
// PriorityScheduler::PriorityReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
PriorityScheduler::ReadyToRun (Process *process)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    //DEBUG(dbgProcess, "Enter ready list: " << readyList->NumInList());
    DEBUG(dbgProcess, "Putting process on ready list: " << process->GetPid());

    process->setStatus(PROCESS_READY);
    readyList->Insert(process);
    //DEBUG(dbgProcess, "Pprocess on ready list: " << readyList->NumInList());
}

//----------------------------------------------------------------------
// PriorityScheduler::FindNextToRun
// 	Return the next process to be scheduled onto the CPU.
//	If there are no ready processes, return NULL.
// Side effect:
//	Process is removed from the ready list.
//----------------------------------------------------------------------

int
PriorityScheduler::HighestPriorityInRQ()
{
   if (readyList->IsEmpty())
	return -10;
   else
   	return readyList->Front()->GetPriority();
}

Process *
PriorityScheduler::FindNextToRun ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyList->IsEmpty()) {
	return NULL;
    } else {
    	return readyList->RemoveFront();
    }
}

//----------------------------------------------------------------------
// PriorityScheduler::Run
// 	Dispatch the CPU to nextProcess.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running process has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentProcess becomes nextProcess.
//
//	"nextProcess" is the process to be put into the CPU.
//	"finishing" is set if the current process is to be deleted
//		once we're no longer running on its stack
//		(when the next process starts running)
//----------------------------------------------------------------------

void
PriorityScheduler::Run (Process *nextProcess, bool finishing)
{
    Process *oldProcess = kernel->currentProcess;
    Thread *oldThread = oldProcess->currentThread();
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current process
         ASSERT(toBeDestroyed == NULL);
	 toBeDestroyed = oldProcess;
    }
    
    if (oldThread->space != NULL) {	// if this thread is a user program,
        oldThread->SaveUserState(); 	// save the user's CPU registers
	oldThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    kernel->currentProcess = nextProcess; // switch to the next process
    nextProcess->setStatus(PROCESS_RUNNING);      // nextProcess is now running
    Thread *nextThread = nextProcess->currentThread();
    nextThread->setStatus(RUNNING);

    /*
    DEBUG(dbgProcess, "Switching from: " << oldProcess->GetPid() << " to: " << nextProcess->GetPid());
    DEBUG(dbgProcess, "Switching from address: " << oldProcess << " to: " << nextProcess);
    DEBUG(dbgProcess, "Before in old thread owner: " << oldThread->owner());
    DEBUG(dbgProcess, 
	"Switching from thread: " << oldThread->getName() << "/" << oldThread->owner()->GetPid()  <<
	" to: " << nextThread->getName() << "/" << nextThread->owner()->GetPid());
    */

    cout << "$ (Kernel Scheduler) Swithcing from: P";
    cout << oldThread->owner()->GetPid() << '/';
    cout << oldThread->getName();
    cout << " to P";
    cout << nextThread->owner()->GetPid() << '/';
    cout << nextThread->getName();
    cout << '\n';
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    kernel->alarm->SetLastTicks(kernel->stats->totalTicks);

    SWITCH(oldThread, nextThread);

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgProcess, "Now in process: " << oldProcess->GetPid());
    DEBUG(dbgProcess, "Now in process next: " << nextProcess->GetPid());
    //DEBUG(dbgProcess, "Now in process next: " << nextThread->owner()->GetPid());
    DEBUG(dbgProcess, "Now in process2: " << kernel->currentProcess->GetPid());
    DEBUG(dbgProcess, "Now address: " << oldProcess << " to: " << nextProcess);

    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up
    
    if (oldThread->space != NULL) {	    // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
	oldThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// PriorityScheduler::CheckToBeDestroyed
// 	If the old process gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the process
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
PriorityScheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
	toBeDestroyed = NULL;
    }
}
 
//----------------------------------------------------------------------
// PriorityScheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
PriorityScheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList->Apply(ProcessPrint);
}
