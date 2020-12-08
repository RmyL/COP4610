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
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "console.h"
#include "synch.h"

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
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

static void ConvertIntToHex(unsigned v, Console *console)
{
  unsigned x;
  if (v == 0)
    return;
  ConvertIntToHex(v / 16, console);
  x = v % 16;
  if (x < 10)
  {
    writeDone->P();
    console->PutChar('0' + x);
  }
  else
  {
    writeDone->P();
    console->PutChar('a' + x - 10);
  }
}

void ExceptionHandler(ExceptionType which)
{
  int type = machine->ReadRegister(2);
  int memval, vaddr, printval, tempval, exp;
  unsigned printvalus; // Used for printing in hex
  if (!initializedConsoleSemaphores)
  {
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 1);
    initializedConsoleSemaphores = true;
  }
  Console *console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
  ;

  if ((which == SyscallException) && (type == syscall_Halt))
  {
    DEBUG('a', "Shutdown, initiated by user program.\n");
    interrupt->Halt();
  }
  else if ((which == SyscallException) && (type == syscall_PrintInt))
  {
    printval = machine->ReadRegister(4);
    if (printval == 0)
    {
      writeDone->P();
      console->PutChar('0');
    }
    else
    {
      if (printval < 0)
      {
        writeDone->P();
        console->PutChar('-');
        printval = -printval;
      }
      tempval = printval;
      exp = 1;
      while (tempval != 0)
      {
        tempval = tempval / 10;
        exp = exp * 10;
      }
      exp = exp / 10;
      while (exp > 0)
      {
        writeDone->P();
        console->PutChar('0' + (printval / exp));
        printval = printval % exp;
        exp = exp / 10;
      }
    }
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }
  else if ((which == SyscallException) && (type == syscall_PrintChar))
  {
    writeDone->P();
    console->PutChar(machine->ReadRegister(4));
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }
  else if ((which == SyscallException) && (type == syscall_PrintString))
  {
    vaddr = machine->ReadRegister(4);
    machine->ReadMem(vaddr, 1, &memval);
    while ((*(char *)&memval) != '\0')
    {
      writeDone->P();
      console->PutChar(*(char *)&memval);
      vaddr++;
      machine->ReadMem(vaddr, 1, &memval);
    }
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }
  else if ((which == SyscallException) && (type == syscall_PrintIntHex))
  {
    printvalus = (unsigned)machine->ReadRegister(4);
    writeDone->P();
    console->PutChar('0');
    writeDone->P();
    console->PutChar('x');
    if (printvalus == 0)
    {
      writeDone->P();
      console->PutChar('0');
    }
    else
    {
      ConvertIntToHex(printvalus, console);
    }
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }

  else if ((which == SyscallException) && (type == syscall_GetReg))
  {
    machine->WriteRegister(2, machine->ReadRegister(machine->ReadRegister(4)));
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }

  else if ((which == SyscallException) && (type == syscall_GetPA))
  {
    unsigned virtAddress = machine->ReadRegister(4);
    unsigned vpn = (unsigned)virtAddress / PageSize;

    if (vpn > machine->pageTableSize)
    {
      machine->WriteRegister(2, -1);
    }

    if (!machine->pageTable[vpn].valid)
    {
      machine->WriteRegister(2, -1);
    }

    if (machine->pageTable[vpn].physicalPage > NumPhysPages)
    {
      machine->WriteRegister(2, -1);
    }

    else
    {
      machine->WriteRegister(2, ((machine->pageTable[vpn].physicalPage) * PageSize) + (virtAddress % PageSize));
    }

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }

  else if ((which == SyscallException) && (type == syscall_Time))
  {
    machine->WriteRegister(2, stats->totalTicks);

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }

  else if ((which == SyscallException) && (type == syscall_GetPID))
  {
    machine->WriteRegister(2, currentThread->getPID());

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }
  else if ((which == SyscallException) && (type == syscall_GetPPID))
  {
    machine->WriteRegister(2, currentThread->getPPID());

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
  }

  else if ((which == SyscallException) && (type == syscall_Yield))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

    currentThread->YieldCPU();
  }

  else if ((which == SyscallException) && (type == syscall_Sleep))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

    int WaitTime = machine->ReadRegister(4);

    if (WaitTime == 0)
    {
      currentThread->YieldCPU();
    }
    else
    {
      SleepingQueue->SortedInsert((void *)currentThread, stats->totalTicks + WaitTime);
      IntStatus oldlevel = interrupt->SetLevel(IntOff);
      currentThread->PutThreadToSleep();
      interrupt->SetLevel(oldlevel);
    }
  }

  else if ((which == SyscallException) && (type == syscall_Fork))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

    NachOSThread *child = new NachOSThread("Forked Thread");
    currentThread->ChildThreadPointer->SortedInsert((void *)child, child->getPID());
    child->parent = currentThread;
    currentThread->NumberOfChildren++;
    currentThread->Child_Status[child->getPID()] = 1;

    child->space = new AddrSpace(currentThread->space->getNumPages(), currentThread->space->getStartPhysPage());

    machine->WriteRegister(2, 0);
    child->SaveUserState();

    machine->WriteRegister(2, child->getPID());

    child->ThreadStackAllocate(&myFunction, 0);
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    scheduler->ReadyToRun(child);
    (void)interrupt->SetLevel(oldLevel);
  }
  else if ((which == SyscallException) && (type == syscall_Exec))
  {
    char filename[100];
    int i = 0;

    vaddr = machine->ReadRegister(4);
    machine->ReadMem(vaddr, 1, &memval);
    while ((*(char *)&memval) != '\0')
    {
      filename[i] = (char)memval;
      ++i;
      vaddr++;
      machine->ReadMem(vaddr, 1, &memval);
    }
    filename[i] = (char)memval;

    if (filename == NULL)
    {
      printf("Unable to open file %s\n", filename);
      return;
    }

    OpenFile *executable = fileSystem->Open(filename);

    AddrSpace *space;
    space = new AddrSpace(executable);
    currentThread->space = space;
    delete executable;
    space->InitRegisters();
    space->RestoreState();

    machine->Run();
    ASSERT(FALSE);
  }
  else if ((which == SyscallException) && (type == syscall_Exit))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));

    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
    int i;

    currentThread->ChildThreadPointer->RemoveChild();
    if (currentThread->parent != NULL)
    {

      currentThread->parent->ChildThreadPointer->SetNULL(currentThread->getPID());
      currentThread->parent->Child_Status[currentThread->getPID()] = 0;
      currentThread->parent->Child_ReturnValues[currentThread->getPID()] = machine->ReadRegister(4);
      WaitingQueue->RemovePid(currentThread->getPID());
    }

    currentThread->FinishThread();
  }
  else if ((which == SyscallException) && (type == syscall_Join))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));

    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);

    int pid = machine->ReadRegister(4);
    if (currentThread->Child_Status[pid] == -1)
    {
      machine->WriteRegister(2, -1);
    }
    else if (currentThread->Child_Status[pid] == 0)
    {
      machine->WriteRegister(2, currentThread->Child_ReturnValues[pid]);
    }
    else
    {
      currentThread->WaitingFor = pid;
      WaitingQueue->SortedInsert((void *)currentThread, pid);
      IntStatus oldlevel = interrupt->SetLevel(IntOff);
      currentThread->PutThreadToSleep();
      interrupt->SetLevel(oldlevel);
    }
  }
  else if ((which == SyscallException) && (type == syscall_NumInstr))
  {
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));

    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
    machine->WriteRegister(2, currentThread->numInst);
  }
  else
  {
    printf("Unexpected user mode exception %d %d\n", which, type);
    ASSERT(FALSE);
  }
}