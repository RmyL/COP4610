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
#include "synch.h" // includes semaphore, locks, and conditions

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
<<<<<<< HEAD
void run_elevator(int numFloors);
void run_person(int p);

 #if defined(HW1_SEMAPHORES)
Semaphore * lock;
 #endif


=======
>>>>>>> main
int SharedVariable;
void SimpleThread();

void SimpleThread(int which) {
	int num, val;
	for(num = 0; num < 5; num++) {
		val = SharedVariable;
		printf("*** thread %d sees value %d\n", which, val);
		currentThread -> Yield();
		SharedVariable = val+1;
		currentThread -> Yield();
	}
	val = SharedVariable;
	printf("Thread %d sees final value %d\n", which, val);
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t -> Fork(SimpleThread, 1);
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

void 
ThreadTest(int n){
	DEBUG('t', "Testing Different Thread Tests");

	for (int i=0; i<n;i++)
	{
		Thread *t= new Thread("threadtest thread");
		t -> Fork(SimpleThread, i);
	}

}
<<<<<<< HEAD
//Elevator
typedef struct Person
=======

//
//ELEVATOR
//
#if defined(HW1_ELEVATOR)
#define ELEVATOR_CAPACITY 5

struct Floor
{
    int gettingOn;
    int gettingOff;
};

struct PersonThread
>>>>>>> main
{
    int id;
    int atFloor;
    int toFloor;
<<<<<<< HEAD
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

    eLock->Acquire();a
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
=======
};

struct ElevatorThread {
    int currentFloor;
    int numPeopleIn;
};

enum Direction {UP, DOWN};
Condition *eCond = new Condition("elevator condition");
Lock *eLock = new Lock("elevator lock");
Floor *floors;
Direction direction;
ElevatorThread e;
int nextID = 1; // used to uniquely acquire a new person

void runElevator(int numFloors) {
    do {
        //Elevator is moving to next floor
        eLock->Acquire();
        for(int i = 0; i < 50; i++);

        if(direction == UP)
            e.currentFloor++;
        else if(direction == DOWN)
            e.currentFloor--;

        printf("Elevator arrives on floor %d.\n", e.currentFloor);
        eLock->Release(); // release (1)
        //Elevator Arrived

        currentThread->Yield();
        if(e.currentFloor == numFloors) {
            printf("Going down\n");
            direction = DOWN;
        }
        else if(e.currentFloor == 0) {
            printf("Going up\n");
            direction = UP;
        }
        
        //Let people get off
        eLock->Acquire();
        eCond->Broadcast(eLock);
        eLock->Release();

        eLock->Acquire();
        while(floors[e.currentFloor].gettingOff > 0)
            eCond->Wait(eLock);
        eLock->Release();// release (2)

        //Let people get on
        eLock->Acquire();
        eCond->Broadcast(eLock);
        while(floors[e.currentFloor].gettingOn > 0 && e.numPeopleIn < ELEVATOR_CAPACITY)
            eCond->Wait(eLock);    
        eLock->Release();// releae (3)
    }
    while(1);
}

void runPerson(int p)
{
    PersonThread *person = (PersonThread*)p;

    //Elevator Release so acquiing lock
    eLock->Acquire(); //aquire (1)
    floors[person->atFloor].gettingOn++;
    printf("Person %d wants to go to floor %d from floor %d.\n", person->id, person->toFloor, person->atFloor);
    eLock->Release();

    //Wait for the elevator to arrive and have space
    eLock->Acquire(); //aquire (2)
    while(e.currentFloor != person->atFloor || e.numPeopleIn == ELEVATOR_CAPACITY)
	    eCond->Wait(eLock);
    eLock->Release();
    
    //Get on elevator and wait to arrive at desired floor
    eLock->Acquire();
    floors[person->toFloor].gettingOff++;
    floors[person->atFloor].gettingOn--;
    e.numPeopleIn++;
    printf("Person %d got into the elevator.\n", person->id);
    eCond->Broadcast(eLock);
    eLock->Release();

    //Get off on desired Floor
    eLock->Acquire();
    while(e.currentFloor != person->toFloor)
	    eCond->Wait(eLock);
    e.numPeopleIn--;
    printf("Person %d got out of the elevator.\n", person->id); 
    floors[person->toFloor].gettingOff--;
    eCond->Broadcast(eLock);
    eLock->Release();
}

void Elevator(int numFloors) {
    Thread* elevator = new Thread("Elevator Thread");
    // wake up elevator
    floors = new Floor[numFloors];
    for(int i = 0; i < numFloors; i++) {
	    floors[i].gettingOn = 0;
	    floors[i].gettingOff = 0;
    }

    e.currentFloor = 0; // goes up from -1 to floor 0
    e.numPeopleIn = 0;
    direction = UP;
    //
    elevator->Fork(runElevator, numFloors);
}

void ArrivingGoingFromTo(int atFloor, int toFloor) {
    Thread* person = new Thread("Person Thread");
    PersonThread *p = new PersonThread;
    p->atFloor = atFloor;
    p->toFloor = toFloor;
    p->id = nextID++;
    person->Fork(runPerson,(int)p);
}
#endif
>>>>>>> main
