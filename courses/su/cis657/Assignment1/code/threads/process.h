// process.h

#ifndef PROCESS_H
#define PROCESS_H

#include "copyright.h"
#include "utility.h"
#include "sysdep.h"

#include "machine.h"
#include "addrspace.h"

#include "list.h"
#include "bitmap.h"
#include "thread.h"

#define MAX_PROCESS 100

enum ProcessStatus { 
    PROCESS_JUST_CREATED, 
    PROCESS_RUNNING, 
    PROCESS_READY, 
    PROCESS_BLOCKED,
    PROCESS_JOINED
};

class Scheduler;

class Process {
public:
    Process(int priority, char *name);
    ~Process();

    void Fork(VoidFunctionPtr func, void *arg);
    Process* Clone();
    Process* CloneWithAllThreads();
    Process* CloneToOwnChild(bool allThreads = FALSE);
    void Yield();
    void Join() { this->_needjoin = 1; } ;
    void Sleep(bool finishing);
    void Terminate();
    void CheckJoin();

    void Pid(const int id) { this->_p_id = id; };
    int GetPid() const { return this->_p_id; };
    void Priority(const int priority) { this->_priority = priority; };
    int GetPriority() const { return this->_priority; }

    void setStatus(ProcessStatus st) { this->_status = st; };

    void setFunc(VoidFunctionPtr func) { this->_func = func; };
    void setFuncArg(void* args) { this->_func_arg = args; };

    Process* parentProcess() { return this->_parent; };
    void setParentProcess(Process *p) { this->_parent = p; };
    Thread* currentThread() { return this->_currentThread; };
    void setCurrentThread(Thread *t) { this->_currentThread = t; };

    List<Process*>& Children() { return this->_children; };
    List<Thread*>& Threads() { return this->_threads; };

    Scheduler* scheduler() { return this->_scheduler; };
private:
    int _p_id;
    int _priority;
    char *_name;
    int _needjoin;
    ProcessStatus _status;

    VoidFunctionPtr _func;
    void* _func_arg;

    Process *_parent;  
    List<Process*> _children;

    Thread *_currentThread;
    List<Thread*> _threads; 

    Scheduler *_scheduler;

    static Bitmap processID;
};

void ProcessPrint(Process *p);
int ProcessComp(Process *x, Process *y) ;

static void emptyfunction() {};

#endif
