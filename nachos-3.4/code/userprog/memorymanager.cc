/*
	MemoryManger will be used to track allocated pages, free pages, allocate pages and free up physical memory
*/
#include "memorymanager.h"

MemoryManager::MemoryManager(int numTotalPages)
{
	lock = new Lock("memLock");
	bitmap = new BitMap(numTotalPages);
}

MemoryManager::~MemoryManager() // Deallocation
{
	delete lock;
	delete bitmap;
}
//check how many many pages are avaibale to allocate
int MemoryManager::FreePages()
{
	lock->Acquire();
	int freePages = bitmap->NumClear(); //Store the number of free pages from NumClear
	lock->Release();
	return freePages;
}

int MemoryManager::getPage()
{
	lock->Acquire();
	int page = bitmap->Find();
	lock->Release();

	return page;
}


void MemoryManager::clearPage(int pageId)
{
	lock->Acquire();
	if(bitmap->Test(pageId))
		bitmap->Clear(pageId);
	lock->Release();
}


