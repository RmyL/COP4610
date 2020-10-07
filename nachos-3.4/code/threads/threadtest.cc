// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;
int numThreads=0 ;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------


 #if defined(HW1_SEMAPHORES)
Semaphore * lock;
 #endif


int SharedVariable;
void SimpleThread(int which) {
    int num, val;

    
    for(num = 0; num < 5; num++) {
        #if defined(HW1_SEMAPHORES)
        lock->P();
        #endif       
        
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        
        SharedVariable = val+1;
        
        #if defined(HW1_SEMAPHORES)
        lock->V();
        #endif
        
        currentThread->Yield();
    }
    
    #if defined(HW1_SEMAPHORES)
       if(which ==numThreads)
    {
    	lock->V();
    	lock->P();
    }    
    lock->P();
    #endif
    
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
    
    #if defined(HW1_SEMAPHORES)
    	lock->V();
    #endif
}





//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------
/*
void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
*/
void 
ThreadTest(int n)
{
#if defined(HW1_SEMAPHORES)
	lock = new Semaphore("lock", 1);
#endif

    Thread *t[n];
    int i;
    for (i=0;i<n;i++)
    {
        t[1] = new Thread("forked thread");
        t[1]->Fork(SimpleThread, i+1);
        numThreads++;
    }
    SimpleThread(0);
}
//Elevator
#define ELEVATOR_CAPACITY 5



struct PersonThread
{
    int id;
    int atFloor;
    int toFloor;
};

struct ElevatorThread
{
    int numFloors;
    int currentFloor;
    int numPeopleIn;
    bool direction; //1 for up; 0 for down
};

void Elevator(int numFloors)
{
    ElevatorThread e = new ElevatorThread;
    e->numFloors = numFloors;
    e->currentFloor = 0;
    e->numPeopleIn = 0;
    e->direction = 1;
    e->Fork(run_elevator);
}

int nextID=0;
void ArrivingGoingFromTo(int atFloor, int toFloor)
{

    PersonThread *p = new PersonThread;
    p->atFloor = atFloor;
    p->toFloor = toFloor;
    p->id = nextID++;

}


void run_elevator()
{
    do
    {


    if (ElevatorThread.numFloors <= ElevatorThread.currentFloor)
        ElevatorThread.direction=1;
    else
        ElevatorThread.direction=0;
    
    //Test if elevator is full

    //if not Take new person

    //Check if person thread needs to leave elevator

    //Elevator moves

    

	
    }
    while(1);

}


random for test
