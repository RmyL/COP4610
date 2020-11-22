#ifndef MEMORYMANGER_H
#define MEMORYMANGER_H

class AddrSpace;
class Thread;

#include "synch.h"
#include "bitmap.h"

class MemoryManager{

  public:

    MemoryManager(int numTotalPages);

  	~MemoryManager();

  	int FreePages();

  	int getPage();

  	void clearPage(int pageId);

   private:

       Bitmap *virtMem;
       Lock *lock;

};

#endif
