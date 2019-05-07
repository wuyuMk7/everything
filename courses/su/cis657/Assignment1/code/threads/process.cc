#include "process.h"
#include "synch.h"
#include "sysdep.h"

Bitmap Process::processID(MAX_PROCESS);

Process::Process(int prior, char *name_ptr)
    : _p_id(0), _priority(prior), _needjoin(0), 
      _status(PROCESS_JUST_CREATED), _func(NULL), _func_arg(NULL),
      _parent(NULL), _children(), _threads()
{
    this->_p_id = processID.FindAndSet();

    int name_len = strlen(name_ptr);
    this->_name = new char[name_len + 1];
    strncpy(this->_name, name_ptr, name_len + 1);
    
    this->_scheduler = new Scheduler();
    
    this->_currentThread = new Thread("Initial Thread");
    this->_currentThread->SetOwner(this);
    //this->_threads.Append(this->_currentThread);

    if (prior != 0) {
        this->_currentThread->Fork((VoidFunctionPtr)(emptyfunction), NULL);
    
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	Thread *t = this->_scheduler->FindNextToRun();
	(void) kernel->interrupt->SetLevel(oldLevel);
    }
}

Process::~Process()
{
    DEBUG(dbgProcess, "Now destruct process #" << this->_p_id);
    ASSERT(this != kernel->currentProcess);

    cout << "$ Destructing Process #" << this->GetPid() << endl;

    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

    // Reset the process id in the bitmap
    // processID.Clear(this->_p_id);

    // Release name pointer
    if (this->_name != NULL) delete [] this->_name;

    // Remove current process from parent's children list
    if (this->_parent != NULL){
	this->_parent->Children().Remove(this);
	this->_parent = NULL;

        // Release all the children process pointer unmoved
        while(!(this->_children.IsEmpty())) {
            Process *tmp_p = this->_children.RemoveFront();
            DEBUG(dbgProcess, "Delete process:" << tmp_p->_p_id);
            delete tmp_p;
        }
    }

    // Release all the threads inside
    // if (this->_currentThread != NULL) delete this->_currentThread;
    while(!(this->_threads.IsEmpty())) {
	Thread *tmp_t = this->_threads.RemoveFront();
        delete tmp_t;
    }

    delete _scheduler;
    DEBUG(dbgProcess, "Process #" << this->_p_id << " has been destructed.");

    (void) kernel->interrupt->SetLevel(oldLevel);
}

void
Process::Fork(VoidFunctionPtr func, void *arg)
{
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

    cout << "$ Forking Process #" << this->_p_id << endl;
    
    Thread *t = new Thread("Main Thread");
    t->SetOwner(this);
    t->Fork(func, arg);
    this->_func = func;
    this->_func_arg = arg;

    this->_parent = kernel->currentProcess;
    kernel->currentProcess->Children().Append(this);
    
    ProcessPrint(this);

    kernel->scheduler->ReadyToRun(this);

    (void) kernel->interrupt->SetLevel(oldLevel);

    // Another implementation for Fork

    //Thread *t = new Thread("Test Fork Thread");
    //this->_currentThread = t;
    //t->SetOwner(this);
    //t->Fork(func, arg);
    //
    //this->_parent = kernel->currentProcess;
    //kernel->currentProcess->Children().Append(this);

    //oldLevel = interrupt->SetLevel(IntOff);
    //kernel->scheduler->ReadyToRun(this);
    //this->_currentThread = this->_scheduler->FindNextToRun();

    //(void) interrupt->SetLevel(oldLevel);
}

Process*
Process::Clone()
{
    ASSERT(this->_priority != 0);
    
    Process *p = new Process(this->_priority, this->_name);

    if (this->_func == NULL)
        return p;
    
    //p->currentThread()->Fork(this->_func, this->_func_arg);
    
    Thread *t = new Thread("Main Thread");
    t->SetOwner(p);
    t->Fork(_func, _func_arg);
    
    p->setFunc(this->_func);
    p->setFuncArg(this->_func_arg);
    p->setStatus(this->_status);

    return p;
}

Process*
Process::CloneWithAllThreads()
{
    ASSERT(this->_priority != 0);
    
    Process *p = new Process(this->_priority, this->_name);

    p->setFunc(this->_func);
    p->setFuncArg(this->_func_arg);
    p->setStatus(this->_status);

    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

    if (!this->_threads.IsEmpty()) {
      ListIterator<Thread*> it(&(this->_threads));
      for (; !it.IsDone(); it.Next()) {
	  Thread *t = new Thread(*(it.Item()));
	  t->SetOwner(p);
	  p->_threads.Append(t);
	  p->scheduler()->ReadyToRun(t);
      }
    }
    
    (void) kernel->interrupt->SetLevel(oldLevel);

    return p;
}

Process*
Process::CloneToOwnChild(bool allThreads)
{
    ASSERT(this->_priority != 0);
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
  
    Process *p;
    if (allThreads == TRUE) {
        p = this->CloneWithAllThreads();
    } else {
        p = this->Clone();
    }

    cout << "$ Cloning Process #" << this->_p_id << " To Process #" << p->GetPid() << endl;

    this->_children.Append(p);
    p->setParentProcess(this);

    ProcessPrint(p);
    
    kernel->scheduler->ReadyToRun(p);
    (void) kernel->interrupt->SetLevel(oldLevel);
}

void
Process::Yield()
{
    Process *nextProcess;
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

    ASSERT(this == kernel->currentProcess);

    DEBUG(dbgProcess, "Yielding process: " << this->_p_id);

    cout << "$ Yielding Process #" << this->_p_id << endl;
    cout << "$ ";
    ProcessPrint(this);

    if (this->_priority <= kernel->scheduler->HighestPriorityInRQ()) {
        nextProcess = kernel->scheduler->FindNextToRun();
        if (nextProcess != NULL) {
	    cout << "$ To Process #" << nextProcess->_p_id << endl;
	    cout << "$ ";
	    ProcessPrint(nextProcess);
	    
            kernel->scheduler->ReadyToRun(this);
            kernel->scheduler->Run(nextProcess, FALSE);
        } else {
	    cout << "$ No more threads in the ready queue." << endl;
	}
    } else {
      cout << "$ Cannot find process with higher priority. Stop switching." << endl;
    }

    (void) kernel->interrupt->SetLevel(oldLevel);
}

void 
Process::Sleep(bool finishing)
{
    Process *nextProcess;
    
    ASSERT(this == kernel->currentProcess);
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgProcess, "Sleeping process: " << this->_p_id);

    this->_status = PROCESS_BLOCKED;
    while((nextProcess = kernel->scheduler->FindNextToRun()) == NULL)
	kernel->interrupt->Idle();

    kernel->scheduler->Run(nextProcess, finishing);
}

void
Process::Terminate()
{
    (void) kernel->interrupt->SetLevel(IntOff);
    ASSERT(this == kernel->currentProcess);

    DEBUG(dbgProcess, "Terminating process: " <<this-> _p_id);

    cout << "$ Terminating Process #" << this->_p_id << endl;

    if (this->_needjoin == 1) {
        Process *nextProcess;
        if (!_children.IsEmpty()) {
	    cout << "$ Process #" << this->_p_id;
  	    cout << " has been joined. Waiting for children processes termination." << endl;
	    this->_status = PROCESS_JOINED;
	    while((nextProcess = kernel->scheduler->FindNextToRun()) == NULL)
	        kernel->interrupt->Idle();
	    kernel->scheduler->Run(nextProcess, FALSE);
        }
	cout << "$ Process #" << this->_p_id;
	cout << " has been joined. No running child process. Terminating it." << endl;
    } else {
        // Move all the children process to parent's children list
        if (this->_parent != NULL) {
	    while(!_children.IsEmpty()) {
  	        Process *tmp_c = _children.RemoveFront();
	        tmp_c -> setParentProcess(this->_parent);
		this->_parent->Children().Append(tmp_c);
	    }
	}
    }

    if (this->_parent != NULL)
        this->_parent->CheckJoin();

    Sleep(TRUE);
}

void
Process::CheckJoin()
{
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

    if (_status == PROCESS_JOINED && (_children.NumInList() == 1 || _children.IsEmpty())) {
        kernel->scheduler->ReadyToRun(this);
    }
    
    (void) kernel->interrupt->SetLevel(oldLevel);
}

void ProcessPrint(Process *p) {
    cout << "# Process #" << p->GetPid();
    cout << " Info - priority: " << p->GetPriority();
    
    if (p->parentProcess() == NULL)
        cout << ", no parent process";
    else
        cout << ", parent process #" << p->parentProcess()->GetPid();
    cout << ", " << p->Children().NumInList();
    cout << " child process, " << p->Threads().NumInList();
    cout << " threads." << endl;
}

int ProcessComp(Process *x, Process *y)
{
    if (x->GetPriority() > y->GetPriority())
	return -1;
    else if (x->GetPriority() < y->GetPriority())
  	return 1;
    else 
	return 0;
}

