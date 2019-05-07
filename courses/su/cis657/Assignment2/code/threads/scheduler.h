// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

#include "bitmap.h"

// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.

#define MAX_THREADS 256

class Scheduler {
  public:
    Scheduler();		// Initialize list of ready threads 
    ~Scheduler();		// De-allocate ready list

    void ReadyToRun(Thread* thread);	
    				// Thread can be dispatched.
    Thread* FindNextToRun();	// Dequeue first thread on the ready 
				// list, if any, and return thread.
    void Run(Thread* nextThread, bool finishing);
    				// Cause nextThread to start running
    void CheckToBeDestroyed();// Check if thread that had been
    				// running needs to be deleted
    void Print();		// Print contents of ready list
    
    // SelfTest for scheduler is implemented in class Thread

    int NextPid();
    bool CheckPid(int test) const;
    void ResetPid(int pid);

    Thread* FindWaiting(int pid);
    void AddWaiting(int pid, Thread *t);
    void RemoveWaiting(int pid);
    
  private:
    List<Thread *> *readyList;  // queue of threads that are ready to run,
				// but not running
    Thread *toBeDestroyed;	// finishing thread to be destroyed
    				// by the next thread that runs
    Thread *waiting_list[MAX_THREADS];
    
    static int pid_occupied[MAX_THREADS];
    static int pid;
};

#endif // SCHEDULER_H
