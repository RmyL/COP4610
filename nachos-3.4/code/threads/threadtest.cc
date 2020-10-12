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
void run_elevator(int numFloors);
void run_person(int p);

 #if defined(HW1_SEMAPHORES)
Semaphore * lock;
 #endif


int SharedVariable;
void SimpleThread(int which) {
    int num, val;

     #if defined(HW1_LOCKS)
        testlock = new Lock("test");
        #endif    
    for(num = 0; num < 5; num++) {
        #if defined(HW1_SEMAPHORES)
        lock->P();
        #elif defined(HW1_LOCKS)  
			testlock->Acquire();
        #endif   
        
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        
        SharedVariable = val+1;
        
        #if defined(HW1_SEMAPHORES)
        lock->V();
        #elif defined(HW1_LOCKS)  
 			testlock->Release();
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
typedef struct Person
{
    int id;
    int atFloor;
    int toFloor;
}Person;
#define ELEVATOR_CAPACITY 5
Condition* eCond = new Condition("eCond");
Lock* eLock= new Lock("eLock");
int currFloor=0;
int occupied=0;
bool direction=true;

void Elevator(int numFloors)
{
    Thread* e = new Thread("eThread");
    e->Fork(run_elevator, numFloors);
}
void run_elevator(int numFloors)
{
    while(1)
    {
    for(int i = 0; i < 500000000; i++); 

    if(direction && currFloor!=numFloors)
        currFloor++;
    else 
        currFloor--;
    printf("Elevator arrives at floor %d.\n", currFloor);
    
    currentThread->Yield(); 

    if(numFloors == currFloor)
        direction = false;
    else if(currFloor == 0)
        direction = true;

    eLock->Acquire();
    eCond->Broadcast(eLock);
    eLock->Release();
    
    }
}

int nextID;
void ArrivingGoingFromTo(int atFloor, int toFloor)
{
    Thread* person = new Thread("Person Thread");
    Person *p = new Person;
    p->atFloor = atFloor;
    p->toFloor = toFloor;
    p->id = nextID++;
    person->Fork(run_person,(int)p);
}

void run_person(int p)
{
    Person *person= (Person*) p;
    int x=person->id;
    int y=person->toFloor;
    int z=person->atFloor;

    printf("Person %d wants to go to floor %d from floor %d.\n",x,y,z);

    eLock->Acquire();
    while(z != currFloor || occupied == ELEVATOR_CAPACITY)
        eCond->Wait(eLock);
    eLock->Release();

    occupied++;
    printf("Person %d got into the elevator at floor %d.\n", x, currFloor);

    eLock->Acquire();
    while(y != currFloor)
        eCond->Wait(eLock);
    eLock->Release();

    occupied--;
    printf("Person %d got out of the elevator at floor %d.\n", x, currFloor); 

}