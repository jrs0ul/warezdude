#include "Threads.h"

void ThreadManager::create(long int (void*) ThreadProc)
{
    threadh = CreateThread(NULL, 0, Threadproc,0, 0, &threadID);
}

void ThreadManager::close()
{
    WaitForSingleObject( threadh,200);
    CloseHandle( threadh );
    threadh = 0;
}
